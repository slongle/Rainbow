#include "path.h"

RAINBOW_NAMESPACE_BEGIN

RGBSpectrum PathIntegrator::Li(const Ray & r, const Scene & scene, int depth) {
    RGBSpectrum L(0.f), beta(1);
    SurfaceInteraction inter;
    bool SpecularBounce = false;
    Ray ray(r);

    for (int bounce = 0;; bounce++) {        
        bool FoundIntersect = scene.Intersect(ray, &inter);

        if (bounce == 0 || SpecularBounce) {
            if (FoundIntersect) {
                L += beta * inter.Le(-ray.d);
            }
        }

        if (!FoundIntersect || bounce >= maxDepth) break;                

        inter.ComputeScatteringFunctions();
        if (!inter.bxdf) {
            // TODO:
            break;

            //ray = inter.SpawnToRay(ray.d);
            //bounce--;
            //continue;
        }        

        L += beta * UniformSampleOneLight(inter, scene);
        Vector3f wo = Normalize(-ray.d), wi;
        Float BSDFPdf;
        RGBSpectrum f = inter.bxdf->SampleF(wo, &wi, sampler->Get2D(), &BSDFPdf);

        delete inter.bxdf;

        // FIX:
        SpecularBounce = (inter.bxdf->type & BxDF::BSDF_SPECULAR) != 0;

        if (BSDFPdf == 0 || f.IsBlack()) break;
        beta *= f * AbsDot(wi, inter.n) / BSDFPdf;


        ray = inter.SpawnToRay(wi);

        if (bounce > 3) {
            Float q = std::min(1.0f, beta.MaxComponent());
            if (sampler->Get1D() >= q)
                break;
            beta /= q;  
        }
    }
    return L;
}


PathIntegrator * CreatePathIntegrator(PropertyList & list) {
    int maxDepth = list.getInteger("maxDepth", 10);
    int sampleNum = list.getInteger("sampleNum", 10);
    return new PathIntegrator(maxDepth, sampleNum);
}

RAINBOW_NAMESPACE_END
