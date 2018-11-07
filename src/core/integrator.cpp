#include"Integrator.h"

RAINBOW_NAMESPACE_BEGIN

RGBSpectrum SamplerIntegrator::UniformSampleOneLight(const SurfaceInteraction & inter, const Scene & scene) {
    int lightNum = scene.lights.size();
    if (lightNum == 0) return RGBSpectrum(0.0);
    int lightID = std::min(int(sampler->Get1D()*lightNum), lightNum - 1);
    Float lightPdf = Float(1) / lightNum;
    Light& light = *(scene.lights[lightID]);
    return EstimateDirectLight(inter, light, scene);
}

RGBSpectrum SamplerIntegrator::EstimateDirectLight(const SurfaceInteraction & inter, 
    const Light& light, const Scene & scene) {
    
    RGBSpectrum Ld(0.0);
    Float LightPdf = 0, BSDFPdf = 0;
    Vector3f wi;
    Visibility visibility;

    // Sample Light with MSI
    RGBSpectrum Li = light.SampleLi(inter, sampler->Get2D(), &wi, &LightPdf, &visibility);
    if (LightPdf > 0 && !Li.IsBlack()) {
        RGBSpectrum f;
        f = inter.bxdf->f(inter.wo, wi) * AbsDot(wi, inter.n);
        BSDFPdf = inter.bxdf->Pdf(inter.wo, wi);
        if (!f.IsBlack()) {
            if (!visibility.Test(scene))
                Li = RGBSpectrum(1.0);

            if (!Li.IsBlack()) {
                if (light.IsDeltaLight()) {
                    Ld += Li * f / LightPdf;
                }
                else {
                    Float weight = PowerHeuristic(1, LightPdf, 1, BSDFPdf);
                    Ld += Li * f * weight / LightPdf;
                }
            }
        }
    }

    // Sample BSDF with MSI
    if (!light.IsDeltaLight()) {
        RGBSpectrum f;
        Float weight = 1;
        f = inter.bxdf->sampleF(inter.wo, &wi, sampler->Get2D(), &BSDFPdf) * AbsDot(wi, inter.n);
        if (BSDFPdf > 0 && !f.IsBlack()) {
            LightPdf = light.Pdf(wi);

            Ray ray = inter.SpawnToRay(wi);
            SurfaceInteraction LightIntersect;

            bool FoundIntersection = scene.Intersect(ray, &LightIntersect);

            RGBSpectrum Li(0.0);
            if (FoundIntersection) {
                if (LightIntersect.primitive->getAreaLight() == &light)
                    Li = LightIntersect.Le(-wi);
            }
            else {
                Li = light.Le(ray);
            }

            if (!Li.IsBlack())
                Ld += f * Li * weight / BSDFPdf;
        }
    }

    return Ld;
}

RGBSpectrum SamplerIntegrator::SpecularReflect
(const Ray & ray, const Scene & scene, int depth, SurfaceInteraction intersection) {
    Vector3f wo = intersection.wo, wi;
    Float pdf;

    RGBSpectrum f = intersection.bxdf->sampleF(wo, &wi, sampler->Get2D(), &pdf);
    Normal3f n = intersection.n;

    if (pdf > 0.f && !f.IsBlack() && Dot(n, wi) != 0.f) {
        Ray r = intersection.SpawnToRay(wi);
        return f * Li(r, scene, depth + 1) * AbsDot(wi, n) / pdf;
    }
    else
        return RGBSpectrum(0.f);
}

RGBSpectrum SamplerIntegrator::SpecularRefract
(const Ray & ray, const Scene & scene, int depth, SurfaceInteraction intersection) {
    return RGBSpectrum();
}

void SamplerIntegrator::Render(const Scene &scene) {
    std::shared_ptr<Film> film = camera->film;
    Ray ray;

    std::cout << scene.aggregate->primitives.size() << std::endl;

    int SampleNum = 50;
    for (int y = 0; y < film->resolution.y; y++) {
        fprintf(stderr, "\rRendering (%d spp) %5.2f%%", SampleNum, 100.*(y + 1) / film->resolution.y);
        for (int x = 0; x < film->resolution.x; x++) {

            /*
            camera->GenerateRay(&ray,
                Point2f(x - film->resolution.x *0.5, y - film->resolution.y *0.5) + sampler->Get2D());
            if (scene.IntersectP(ray)) {
                film->SetPixel(Point2i(x, y), RGBSpectrum(1, 1, 1));
            }
            else
                film->SetPixel(Point2i(x, y), RGBSpectrum(0, 0, 0));

            continue;
            */

            RGBSpectrum L(0.0);            
            for (int i = 0; i < SampleNum; i++) {
                camera->GenerateRay(&ray,
                    Point2f(x - film->resolution.x *0.5, y - film->resolution.y *0.5) + sampler->Get2D());
                L += Li(ray, scene, 0);
            }
            L /= SampleNum;
            film->SetPixel(Point2i(x, y), L);
        }
    }
    film->SaveImage();
}

RAINBOW_NAMESPACE_END