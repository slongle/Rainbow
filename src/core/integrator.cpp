#include <stdio.h>
#include <atomic>
#include <thread>
#include <mutex>
#include <tbb/parallel_for.h>
#include <tbb/blocked_range.h>

#include "integrator.h"

RAINBOW_NAMESPACE_BEGIN

RGBSpectrum Clamp(RGBSpectrum a, RGBSpectrum b) {
    for (int i = 0; i < 3; i++)
        if (a[i] > b[i]) a[i] = b[i];
    return a;
}

RGBSpectrum SamplerIntegrator::UniformSampleOneLight(
    const Interaction& inter,
    const Scene& scene,
    Sampler& sampler,
    const bool handleMedium) 
{
    int lightNum = scene.lights.size();
    if (lightNum == 0) return RGBSpectrum(0.0);
    int lightID = std::min(int(sampler.Next1D()*lightNum), lightNum - 1);    
    Float lightPdf = Float(1) / lightNum;
    std::shared_ptr<Light> light = scene.lights[lightID];
    return EstimateDirectLight(
        inter, light, scene, sampler.Next2D(), sampler.Next2D(), 
        sampler, handleMedium) / lightPdf;
}

RGBSpectrum SamplerIntegrator::EstimateDirectLight(
    const Interaction& inter,
    const std::shared_ptr<Light>& light,
    const Scene& scene,
    const Point2f& scatteru,
    const Point2f& lightu,
    Sampler&       sampler,
    const bool handleMedia)
{
    RGBSpectrum Ld(0.0);
    Float lightPdf = 0, scatterPdf = 0;
    Vector3f wi;
    Visibility visibility;

    // Sample Light with MSI
    RGBSpectrum Li = light->SampleLi(inter, lightu, &wi, &lightPdf, &visibility);
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
                Li *= visibility.Tr(scene, sampler);
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
        //const SurfaceInteraction& a = static_cast<const SurfaceInteraction&>(inter);
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
                handleMedia ? scene.IntersectTr(ray, sampler, &lightInter, &Tr) :
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

    RGBSpectrum f = intersection.bsdf->SampleF(wo, &wi, sampler->Next2D(), &pdf);
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

void SamplerIntegrator::RenderTileAdaptive(const Scene &scene, Sampler& sampler, FilmTile &tile, bool clamp) {
    Ray ray;
    MemoryArena arena;
    Float maxError = 0.05, pValue = 0.05;
    Float quantile = 1.95996;
    for (int y = tile.sampleBounds.pMin.y; y < tile.sampleBounds.pMax.y; y++) {
        for (int x = tile.sampleBounds.pMin.x; x < tile.sampleBounds.pMax.x; x++) {
            sampler.SetPixel(Point2i(x, y));
            RGBSpectrum L(0.0);
            Float mean = 0, meanSqr = 0;
            int sampleCount;
            for (sampleCount = 1; sampleCount <= 1024; sampleCount++) {
                Point2f cameraSample = sampler.GetCameraSample();
                RGBSpectrum weight = camera->GenerateRay(&ray, cameraSample);
                RGBSpectrum li = weight * Li(arena, ray, scene, sampler, 0);
                if (clamp)
                    li = Clamp(li, RGBSpectrum(1));
                L += li;

                Float sampleLuminance = li.Luma();
                const Float delta = sampleLuminance - mean;
                mean += delta / sampleCount;
                meanSqr += delta * (sampleLuminance - mean);

                if (sampleCount % 64 == 0) {                    
                    const Float variance = meanSqr / (sampleCount - 1);

                    Float stdError = std::sqrt(variance / sampleCount);

                    /* Half width of the confidence interval */
                    Float ciWidth = stdError * quantile;

                    /* Relative error heuristic */
                    //Float base = std::max(mean, m_averageLuminance * 0.01f);
                    Float base = mean;

                    if (ciWidth <= maxError * base)
                        break;
                }
                tile.AddSample(Point2i(x, y), Point2f(x, y), L);
                sampler.StartNextSample();
            }
            //tile.AddSample(Point2f(x, y), L, sampleCount);
        }
    }
}

void SamplerIntegrator::RenderTile(
    const Scene&   scene, 
    Sampler&       sampler, 
    FilmTile&      tile, 
    bool           clamp, 
    const int&     preSampleSum) 
{
    Ray ray;
    MemoryArena arena;
    for (int y = tile.sampleBounds.pMin.y; y < tile.sampleBounds.pMax.y; y++) {
        for (int x = tile.sampleBounds.pMin.x; x < tile.sampleBounds.pMax.x; x++) {            
            RGBSpectrum L(0.);
            sampler.SetPixel(Point2i(x, y));
            sampler.SetSampleNumber(preSampleSum);
            for (int i = 0; i < delta; i++) {
                Point2f cameraSample = sampler.GetCameraSample();
                RGBSpectrum weight = camera->GenerateRay(&ray, cameraSample);
                L = weight * Li(arena, ray, scene, sampler, 0);
                if (clamp)
                    L = Clamp(L, RGBSpectrum(1));
                tile.AddSample(Point2i(x, y), cameraSample, L);
                sampler.StartNextSample();
            }
        }                
    }
}

void SamplerIntegrator::RenderTileEyeLight(const Scene &scene, Sampler& sampler, FilmTile &tile) {
    Ray ray;
    MemoryArena arena;
    for (int y = tile.pixelBounds.pMin.y; y < tile.pixelBounds.pMax.y; y++) {
        for (int x = tile.pixelBounds.pMin.x; x < tile.pixelBounds.pMax.x; x++) {
            sampler.SetPixel(Point2i(x, y));
            Point2f cameraSample = sampler.GetCameraSample();
            RGBSpectrum weight = camera->GenerateRay(&ray, cameraSample);
            SurfaceInteraction inter;
            if (scene.IntersectP(ray, &inter)) {
                Float dotLN = Dot(-ray.d, inter.n);
                if (dotLN > 0)
                    tile.AddSample(Point2f(x, y), RGBSpectrum(dotLN), 1);
                else
                    tile.AddSample(Point2f(x, y), RGBSpectrum(-dotLN, 0, 0), 1);
            }
        }
    }
}

void SamplerIntegrator::Render(const Scene &scene) {    
    std::shared_ptr<Film> film = camera->film;
    std::vector<FilmTile> tiles;
    tiles = FilmTile::GenerateTiles(camera->film->resolution, RAINBOW_TILE_SIZE, film->filter);
    
    cout << "Rendering .. \n";
    cout.flush();
    std::string filename = film->filename;
    Timer timer;

    std::thread renderThread([&] {

        tbb::blocked_range<int> range(0, tiles.size());
        int cnt = 1;
        int area = (film->resolution.x)*(film->resolution.y);
        std::atomic<int> finishedTiles = 0;

        int preSumSample = 0;
        auto map = [&](const tbb::blocked_range<int> &range) {        
            for (int i = range.begin(); i < range.end(); ++i) {        
                /* Request an image block from the block generator */
                FilmTile &tile = tiles[i];

                /* Inform the sampler about the block to be rendered */
                Point2i seed = Point2i((cnt - 1)*area, (cnt - 1)*area) + 
                               tile.pixelBounds.pMin;
                
                std::unique_ptr<Sampler> clonedSampler(sampler->Clone(seed));

                /* Render all contained pixels */
                RenderTile(scene, *clonedSampler, tile, false, preSumSample);

                film->MergeFilmTile(tile);

                finishedTiles++;
                fprintf(stderr, "\r%.2f%%", 100. * finishedTiles / tiles.size());
            }
        };

        for (cnt = 1; cnt <= sampleNum / delta; cnt++)
        {
            /// Uncomment the following line for single threaded rendering
            //map(range);

            /// Default: parallel rendering
            tbb::parallel_for(range, map);

            preSumSample += delta;

            std::string tmpName = filename;
            tmpName.insert(film->filename.find_last_of('.'), "_" + std::to_string(cnt * delta) + "spp");
            //std::cout << tmpName << std::endl;
            film->SaveImage(tmpName);
            statistic.ShowStatistic();
        }

    });

    renderThread.join();    

    cout << timer.LastTime() << endl;
}


void SamplerIntegrator::RenderAdaptive(const Scene &scene) {
    std::shared_ptr<Film> film = camera->film;
    std::vector<FilmTile> tiles;
    tiles = FilmTile::GenerateTiles(camera->film->resolution, RAINBOW_TILE_SIZE, film->filter);
    
    cout << "Rendering .. \n";
    cout.flush();
    std::string filename = film->filename;
    Timer timer;

    std::thread renderThread([&] {

        tbb::blocked_range<int> range(0, tiles.size());
        std::atomic<int> cnt = 0;
        int area = (film->resolution.x)*(film->resolution.y);

        auto map = [&](const tbb::blocked_range<int> &range) {
            for (int i = range.begin(); i < range.end(); ++i) {                  
                /* Request an image block from the block generator */
                FilmTile &tile = tiles[i];

                /* Inform the sampler about the block to be rendered */
                Point2i seed = Point2i((cnt - 1)*area, (cnt - 1)*area) +
                    tile.pixelBounds.pMin;

                std::unique_ptr<Sampler> clonedSampler(sampler->Clone(seed));

                /* Render all contained pixels */
                RenderTileAdaptive(scene, *clonedSampler, tile);

                film->MergeFilmTile(tile);

                cnt++;
                fprintf(stderr, "\r%.2f%%", 100. * cnt / tiles.size());

                if (cnt % 20 == 0) film->SaveImage(std::to_string(cnt) + '_' + filename);
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

void SamplerIntegrator::RenderEyeLight(const Scene &scene) {
    std::shared_ptr<Film> film = camera->film;
    std::vector<FilmTile> tiles;
    tiles = FilmTile::GenerateTiles(camera->film->resolution, RAINBOW_TILE_SIZE, film->filter);
    
    cout << "Rendering .. \n";
    cout.flush();
    std::string filename = film->filename;
    Timer timer;

    std::thread renderThread([&] {

        tbb::blocked_range<int> range(0, tiles.size());        
        int cnt = 1;
        int area = (film->resolution.x)*(film->resolution.y);
        std::atomic<int> finishedTiles = 0;

        auto map = [&](const tbb::blocked_range<int> &range) {        
            for (int i = range.begin(); i < range.end(); ++i) {                 
                /* Request an image block from the block generator */
                FilmTile &tile = tiles[i];

                /* Inform the sampler about the block to be rendered */
                Point2i seed = Point2i((cnt - 1)*area, (cnt - 1)*area) +
                    tile.pixelBounds.pMin;

                std::unique_ptr<Sampler> clonedSampler(sampler->Clone(seed));

                /* Render all contained pixels */
                RenderTileEyeLight(scene, *clonedSampler, tile);

                film->MergeFilmTile(tile);

                finishedTiles++;
                fprintf(stderr, "\r%.2f%%", 100. * finishedTiles / tiles.size());
            }
        };        
        /// Uncomment the following line for single threaded rendering
        map(range);

        /// Default: parallel rendering
        //tbb::parallel_for(range, map);
    });

    renderThread.join();
    film->SaveImage();
    cout << timer.LastTime() << endl;
}

RAINBOW_NAMESPACE_END