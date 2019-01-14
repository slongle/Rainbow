#include"Integrator.h"
#include <stdio.h>

RAINBOW_NAMESPACE_BEGIN

RGBSpectrum SamplerIntegrator::UniformSampleOneLight(const SurfaceInteraction & inter, const Scene & scene) {
    int lightNum = scene.lights.size();
    if (lightNum == 0) return RGBSpectrum(0.0);
    int lightID = std::min(int(sampler->Get1D()*lightNum), lightNum - 1);    
    Float lightPdf = Float(1) / lightNum;
    std::shared_ptr<Light> light = scene.lights[lightID];
    return EstimateDirectLight(inter, light, scene) / lightPdf;
}

RGBSpectrum SamplerIntegrator::EstimateDirectLight(const SurfaceInteraction & inter, 
    std::shared_ptr<Light> light, const Scene & scene) {
    
    RGBSpectrum Ld(0.0);
    Float lightPdf = 0, BSDFPdf = 0;
    Vector3f wi;
    Visibility visibility;

    // Sample Light with MSI
    RGBSpectrum Li = light->SampleLi(inter, sampler->Get2D(), &wi, &lightPdf, &visibility);
    if (lightPdf > 0 && !Li.IsBlack()) {

        // Estimate BSDF * AbsDotTheta and PDF
        RGBSpectrum f;
        f = inter.bsdf->f(inter.wo, wi) * AbsDot(wi, inter.n);
        BSDFPdf = inter.bsdf->Pdf(inter.wo, wi);

        if (!f.IsBlack()) {
            if (!visibility.Test(scene))
                Li = RGBSpectrum(0.0);

            if (!Li.IsBlack()) {
                if (light->IsDeltaLight()) {
                    Ld += Li * f / lightPdf;
                }
                else {
                    Float weight = PowerHeuristic(1, lightPdf, 1, BSDFPdf);
                    Ld += Li * f * weight / lightPdf;
                }
            }
        }
    }

    // Sample BSDF with MSI
    if (!light->IsDeltaLight()) {

        BxDFType BSDFType;
        RGBSpectrum f;
        f = inter.bsdf->SampleF(inter.wo, &wi, sampler->Get2D(), &BSDFPdf,BSDF_ALL,&BSDFType) * AbsDot(wi, inter.n);

        bool SampleSpecular = (BSDFType & BSDF_SPECULAR) != 0;

        if (BSDFPdf > 0 && !f.IsBlack()) {
            Float weight = 1;
            if (!SampleSpecular) {
                lightPdf = light->PdfLi(inter, wi);
                if (lightPdf == 0) return Ld;
                weight = PowerHeuristic(1, BSDFPdf, 1, lightPdf);
            }


            Ray ray = inter.SpawnToRay(wi);
            SurfaceInteraction LightIntersect;

            bool FoundIntersection = scene.Intersect(ray, &LightIntersect);

            RGBSpectrum Li(0.0);
            if (FoundIntersection) {
                if (LightIntersect.primitive->getAreaLight() == light.get())
                    Li = LightIntersect.Le(-wi);
            }
            else {
                //Li = light.Le(ray);
            }

            if (!Li.IsBlack())
                Ld += f * Li * weight / BSDFPdf;
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
        return f * Li(arena, r, scene, depth + 1) * AbsDot(wi, n) / pdf;
    }
    else
        return RGBSpectrum(0.f);
}

RGBSpectrum SamplerIntegrator::SpecularRefract
(MemoryArena& arena, const Ray & ray, const Scene & scene, int depth, SurfaceInteraction intersection) {
    return RGBSpectrum();
}

void SamplerIntegrator::ProgressiveRender(const Scene &scene,const int& x,const int & y, bool reset) {    
    std::shared_ptr<Film> film = camera->film;
    Ray ray;
    RGBSpectrum L(0.0);            
    camera->GenerateRay(&ray, Point2f(x - film->resolution.x *0.5, y - film->resolution.y *0.5) + sampler->Get2D());
    L = Li(arena, ray, scene, 0);    
    film->AddPixel(Point2i(x, y), L);
    if (reset) {
        arena.Reset();
    }
}

void SamplerIntegrator::Render(const Scene &scene) {
    std::shared_ptr<Film> film = camera->film;
    Ray ray;

    Timer timer;

    std::cout << scene.aggregate->primitives.size() << std::endl;
    puts("!!!!!");
    for (int y = 0; y < film->resolution.y; y++) {
        fprintf(stderr, "\rRendering (%d spp) %5.2f%%", sampleNum, 100.*(y + 1) / film->resolution.y);
        for (int x = 0; x < film->resolution.x; x++) {
            RGBSpectrum L(0.0);            
            for (int i = 0; i < sampleNum; i++) {
                camera->GenerateRay(&ray,
                    Point2f(x - film->resolution.x *0.5, y - film->resolution.y *0.5) + sampler->Get2D());
                L += Li(arena, ray, scene, 0);
                //RGBSpectrum L = Li(arena, ray, scene, 0);
                //if (!L.IsBlack()) std:cout << L << std::endl;
                //film->AddPixel(Point2i(x, y), Li(arena, ray, scene, 0));
                //if (!L.IsBlack()) std::cout << std::endl;
            }
            L /= sampleNum;
            film->SetPixel(Point2i(x, y), L);
        }
        arena.Reset();
    }
    film->SaveImage();

    cout << "\n";
    cout << timer.LastTime() << endl;
}


void SamplerIntegrator::TestRender(const Scene &scene) {
    std::shared_ptr<Film> film = camera->film;
    Ray ray;
    Timer timer;
    std::string name = film->filename, tmpName;
    for (int i = 1; i <= sampleNum / delta; i++) {  
        tmpName = name;
        for (int y = 0; y < film->resolution.y; y++) {
            fprintf(stderr, "\rRendering (%d spp) %5.2f%%", sampleNum, 100.*(y + 1) / film->resolution.y);
            for (int x = 0; x < film->resolution.x; x++) {
                RGBSpectrum L(0.0);
                for (int j = 0; j < delta; j++) {
                    camera->GenerateRay(&ray,
                        Point2f(x - film->resolution.x *0.5, y - film->resolution.y *0.5) + sampler->Get2D());
                    L += Li(arena, ray, scene, 0);
                }
                film->AddPixel(Point2i(x, y), L, delta);
            }
            arena.Reset();
        }
        tmpName.insert(name.find_last_of('.'), "_" + std::to_string(i*delta) + "spp");
        std::cout << tmpName << std::endl;
        film->SaveImage(tmpName);
    }
}

RAINBOW_NAMESPACE_END