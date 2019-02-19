#include <stdio.h>
#include <atomic>
#include <thread>
#include <mutex>
#include <ext/tbb/include/tbb/parallel_for.h>
#include <ext/tbb/include/tbb/blocked_range.h>

#include "integrator.h"

RAINBOW_NAMESPACE_BEGIN

RGBSpectrum SamplerIntegrator::UniformSampleOneLight(
    const Interaction& inter,
    const Scene& scene,
    Sampler& sampler,
    const bool handleMedium
    ) {
    int lightNum = scene.lights.size();
    if (lightNum == 0) return RGBSpectrum(0.0);
    int lightID = std::min(int(sampler.Get1D()*lightNum), lightNum - 1);    
    Float lightPdf = Float(1) / lightNum;
    std::shared_ptr<Light> light = scene.lights[lightID];
    return EstimateDirectLight(inter, light, scene, sampler.Get2D(), sampler.Get2D(), handleMedium) / lightPdf;
}

RGBSpectrum SamplerIntegrator::EstimateDirectLight(
    const Interaction& inter,
    const std::shared_ptr<Light>& light,
    const Scene& scene,
    const Point2f& scatteru,
    const Point2f& lightu,
    const bool handleMedia)
{
    RGBSpectrum Ld(0.0);
    Float lightPdf = 0, scatterPdf = 0;
    Vector3f wi;
    Visibility visibility;

    // Sample Light with MSI
    RGBSpectrum Li = light->SampleLi(inter, sampler->Get2D(), &wi, &lightPdf, &visibility);
    if (lightPdf > 0 && !Li.IsBlack()) {
        RGBSpectrum f;
        // Estimate BSDF or Phase Function
        if (inter.IsSurfaceInteraction()) {
            // Sample BSDF
            const SurfaceInteraction& surfaceInter = static_cast<const SurfaceInteraction&>(inter);
            f = surfaceInter.bsdf->f(inter.wo, wi) * AbsDot(wi, inter.n);
            scatterPdf = surfaceInter.bsdf->Pdf(inter.wo, wi);
        }
        else {
            // Sample Phase Function
            const MediumInteraction& mediumInter = static_cast<const MediumInteraction&>(inter);
            Float p = mediumInter.phase->P(mediumInter.wo, wi);
            f = RGBSpectrum(p);
            scatterPdf = p;
        }
        if (!f.IsBlack()) {
            // Visibility test
            if (handleMedia) {
                Li *= visibility.Tr(scene, *sampler);
            }
            else {
                if (visibility.Occluded(scene))
                    Li = RGBSpectrum(0.0);                
            }
            // Compute Le contribute
            if (!Li.IsBlack()) {
                if (light->IsDeltaLight()) {
                    Ld += Li * f / lightPdf;
                }
                else {
                    Float weight = PowerHeuristic(1, lightPdf, 1, scatterPdf);
                    Ld += Li * f * weight / lightPdf;
                }
            }
        }
    }

    // Sample BSDF with MIS
    if (!light->IsDeltaLight()) {
        RGBSpectrum f;
        bool sampledSpecular = false;
        if (inter.IsSurfaceInteraction()) {
            // Sample BSDF
            BxDFType BSDFType;
            const SurfaceInteraction& surfaceInter = static_cast<const SurfaceInteraction&>(inter);            
            f = surfaceInter.bsdf->SampleF(surfaceInter.wo, &wi, scatteru, 
                                           &scatterPdf, BSDF_ALL, &BSDFType);
            f *= AbsDot(wi, surfaceInter.n);      
            sampledSpecular = (BSDFType & BSDF_SPECULAR) != 0;
        }
        else {
            // Sample Phase Function
            const MediumInteraction& mediumInter = static_cast<const MediumInteraction&>(inter);
            Float p = mediumInter.phase->SampleP(mediumInter.wo, &wi, lightu);
            f = RGBSpectrum(p);
            scatterPdf = p;
        }
        if (scatterPdf > 0 && !f.IsBlack()) {
            Float weight = 1;
            if (!sampledSpecular) {
                lightPdf = light->PdfLi(inter, wi);
                if (lightPdf == 0) return Ld;
                weight = PowerHeuristic(1, scatterPdf, 1, lightPdf);
            }

            // Light find and Evaluate Tr
            Ray ray = inter.SpawnToRay(wi);
            SurfaceInteraction lightInter;
            RGBSpectrum Tr(1.);
            bool foundIntersection =
                handleMedia ? scene.IntersectTr(ray, *sampler, &lightInter, &Tr) :
                              scene.IntersectP(ray, &lightInter);

            // Compute Le contribute
            RGBSpectrum Li(0.);
            if (foundIntersection) {
                if (lightInter.primitive->getAreaLight() == light.get())
                    Li = lightInter.Le(-wi);
            }
            else {
                //Le = light.Le(ray);
            }
            if (!Li.IsBlack())
                Ld += f * Li * weight / scatterPdf;
        }
    }
    return Ld;
}

RGBSpectrum SamplerIntegrator::SpecularReflect
(MemoryArena& arena, const Ray & ray, const Scene & scene, int depth, SurfaceInteraction intersection) {
    Vector3f wo = intersection.wo, wi;
    Float pdf;

    RGBSpectrum f = intersection.bsdf->SampleF(wo, &wi, sampler->Get2D(), &pdf);
    Normal3f n = intersection.n;

    if (pdf > 0.f && !f.IsBlack() && Dot(n, wi) != 0.f) {
        Ray r = intersection.SpawnToRay(wi);
        return f * Li(arena, r, scene, *sampler, depth + 1) * AbsDot(wi, n) / pdf;
    }
    else
        return RGBSpectrum(0.f);
}

RGBSpectrum SamplerIntegrator::SpecularRefract
(MemoryArena& arena, const Ray & ray, const Scene & scene, int depth, SurfaceInteraction intersection) {
    return RGBSpectrum();
}


void SamplerIntegrator::RenderTileAdaptive(const Scene &scene, Sampler& sampler, FilmTile &tile) {
    Ray ray;
    MemoryArena arena;
    Float maxTolerance = 0.05;
    for (int y = tile.bounds.pMin.y; y < tile.bounds.pMax.y; y++) {
        for (int x = tile.bounds.pMin.x; x < tile.bounds.pMax.x; x++) {
            RGBSpectrum L(0.0);
            Float sum = 0, squareSum = 0, sigma, mu, I;
            int num;
            for (num = 1; num <= 10240; num++) {                
                RGBSpectrum weight = camera->GenerateRay(&ray, Point2f(x, y) + sampler.Get2D());
                RGBSpectrum li = weight * Li(arena, ray, scene, sampler, 0);
                L += li;
                Float illum = li.Luma();
                sum += illum;
                squareSum += illum * illum;
                if (num % 64 == 0) {
                    mu = sum / num;
                    sigma = std::sqrt((squareSum - sum * sum / num) / (num - 1));
                    I = 1.96 * sigma / std::sqrt(num);
                    if (I <= maxTolerance * mu) {
                        break;
                    }
                }
            }
            tile.AddSample(Point2f(x, y), L, num);
        }
    }
}

void SamplerIntegrator::RenderTile(const Scene &scene, Sampler& sampler, FilmTile &tile) {    
    Ray ray;
    MemoryArena arena;
    for (int y = tile.bounds.pMin.y; y < tile.bounds.pMax.y; y++) {
        for (int x = tile.bounds.pMin.x; x < tile.bounds.pMax.x; x++) {            
            RGBSpectrum L(0.);
            for (int i = 0; i < sampleNum; i++) {
                Point2f pixelSample = Point2f(x, y) + sampler.Get2D();
                RGBSpectrum weight = camera->GenerateRay(&ray, pixelSample);
                L += weight * Li(arena, ray, scene, sampler, 0);
            }
            tile.AddSample(Point2f(x, y), L, sampleNum);
        }                
    }
}

void SamplerIntegrator::Render(const Scene &scene) {    
    std::shared_ptr<Film> film = camera->film;
    std::vector<FilmTile> tiles;
    tiles = FilmTile::GenerateTiles(camera->film->resolution, RAINBOW_TILE_SIZE);

    cout << sampleNum << endl;
    cout << "Rendering .. ";
    cout.flush();
    std::string filename = film->filename;
    Timer timer;

    std::thread renderThread([&] {

        tbb::blocked_range<int> range(0, tiles.size());                

        auto map = [&](const tbb::blocked_range<int> &range) {               
            for (int i = range.begin(); i<range.end(); ++i) {
                /* Request an image block from the block generator */
                FilmTile &tile = tiles[i];
                
                /* Inform the sampler about the block to be rendered */
                std::unique_ptr<Sampler> clonedSampler(sampler->Clone(tile.bounds.pMin));

                /* Render all contained pixels */
                RenderTileAdaptive(scene, *clonedSampler, tile);

                film->MergeFilmTile(tile);
            }                
        };

        
            /// Uncomment the following line for single threaded rendering
            //map(range);

            /// Default: parallel rendering
            tbb::parallel_for(range, map);

           
    });
    
    
    renderThread.join();

    film->SaveImage();
    film->SaveHeatMapImage("heat_" + filename);

    cout << timer.LastTime() << endl;
}

void SamplerIntegrator::ProgressiveRender(const Scene &scene,const int& x,const int & y, bool reset) {    
    std::shared_ptr<Film> film = camera->film;
    Ray ray;
    RGBSpectrum L(0.0);
    MemoryArena arena;
    camera->GenerateRay(&ray, Point2f(x - film->resolution.x *0.5, y - film->resolution.y *0.5) + sampler->Get2D());
    L = Li(arena, ray, scene, *sampler, 0);    
    film->AddPixel(Point2i(x, y), L);
}

void SamplerIntegrator::AdaptiveProgressiveRender(const Scene & scene, const int & x, const int & y) {
    std::shared_ptr<Film> film = camera->film;
    Ray ray;
    Float maxTolerance = 0.05;
    int n;
    MemoryArena arena;
    RGBSpectrum L(0.0);
    RGBSpectrum sum, squareSum, sigma, mu, I;
    for (n = 1; ; n++) {
        camera->GenerateRay(&ray, Point2f(x, y) + sampler->Get2D());
        RGBSpectrum li = Li(arena, ray, scene, *sampler, 0);
        L += li;
        sum += li;
        squareSum += Sqr(li);
        if (n % 64 == 0) {
            mu = sum / n;
            sigma = Sqrt((squareSum - Sqr(sum) / n) / (n - 1));
            I = 1.96 * sigma / std::sqrt(n);
            if (I <= maxTolerance * mu) {
                //cout << I << endl << maxTolerance * mu << endl << endl;
                //cout << n << endl;
                break;
            }
        }
    }
    film->AddPixel(Point2i(x, y), L, n);
    if (y % 32 == 0) arena.Reset();
}

void SamplerIntegrator::TestRender(const Scene &scene) {
    cout << scene.aggregate->primitives.size() << endl;
    MemoryArena arena;
    std::shared_ptr<Film> film = camera->film;
    Ray ray;
    Timer timer;
    std::string name = film->filename, tmpName;
    for (int i = 1; i <= sampleNum / delta; i++) {  
        tmpName = name;
        timer.Start();
        for (int y = 0; y < film->resolution.y; y++) {
            fprintf(stderr, "\rRendering (%d spp) %5.2f%%", i*delta, 100.*(y + 1) / film->resolution.y);
            for (int x = 0; x < film->resolution.x; x++) {
                RGBSpectrum L(0.0);
                for (int j = 0; j < delta; j++) {
                    camera->GenerateRay(&ray, Point2f(x, y) + sampler->Get2D());
                    L += Li(arena, ray, scene, *sampler, 0);
                }
                film->AddPixel(Point2i(x, y), L, delta);
            }
            arena.Reset();
        }
        fflush(stderr);
        cout << timer.LastTime() << endl;
        tmpName.insert(name.find_last_of('.'), "_" + std::to_string(i*delta) + "spp");
        std::cout << tmpName << std::endl;
        film->SaveImage(tmpName);
    }
}

void SamplerIntegrator::AdaptiveRender(const Scene &scene) {
    cout << scene.aggregate->primitives.size() << endl;
    MemoryArena arena;
    std::shared_ptr<Film> film = camera->film;
    Ray ray;
    Timer timer;
    std::string name = film->filename, tmpName;
    timer.Start();
    Float maxTolerance = 0.05;
    int total = film->resolution.x*film->resolution.y, now = 0;
    for (int y = 0; y < film->resolution.y; y++) {
        for (int x = 0; x < film->resolution.x; x++) {
            RGBSpectrum L(0.0);
            Float sum = 0, squareSum = 0, sigma, mu, I;
            int n;
            for (n = 1; n <= 10240 ; n++) {
                camera->GenerateRay(&ray, Point2f(x, y) + sampler->Get2D());
                RGBSpectrum li = Li(arena, ray, scene, *sampler, 0);
                L += li;
                Float illum = li.Luma();
                sum += illum;
                squareSum += illum * illum;
                if (n % 64 == 0) {
                    mu = sum / n;
                    sigma = std::sqrt((squareSum - sum * sum / n) / (n - 1));
                    I = 1.96 * sigma / std::sqrt(n);
                    if (I <= maxTolerance * mu) {                        
                        //cout << I << endl << maxTolerance * mu << endl << endl;
                        //cout << n << endl;
                        break;
                    }
                }
            }
            now++;
            fprintf(stderr, "\rRendering %d / %d : %d", now, total, n);
            film->AddPixel(Point2i(x, y), L, n);
        }
        if (y % 5 == 1) {
            std::string heatMapName = "heat_" + std::to_string(y) + "_" + name;
            film->SaveHeatMapImage(heatMapName);            
        }
        if (y % 32 == 0) arena.Reset();
    }
    fflush(stderr);
    cout << timer.LastTime() << endl;
    film->SaveImage(name);
    std::string heatMapName = "heat_" + name;
    film->SaveHeatMapImage(heatMapName);
}

RAINBOW_NAMESPACE_END