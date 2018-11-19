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

        if (!FoundIntersect || bounce >= maxDep) break;        

        inter.ComputeScatteringFunctions();
        if (!inter.bxdf) {
            // TODO:
            break;

            //ray = inter.SpawnToRay(ray.d);
            //bounce--;
            //continue;
        }

        L += beta * UniformSampleOneLight(inter, scene);

        Vector3f wo(-ray.d), wi;
        Float pdf;
        RGBSpectrum f = inter.bxdf->SampleF(wo, &wi, sampler->Get2D(), &pdf);

        if (pdf == 0 || f.IsBlack()) break;
        beta *= f * AbsDot(wi, inter.n) / pdf;

        // FIX:
        SpecularBounce = (inter.bxdf->type & BxDF::BSDF_SPECULAR) != 0;

        ray = inter.SpawnToRay(wi);

        if (bounce > 3) {
            Float q = std::max(0.05, 1.0 - beta.g);
            if (sampler->Get1D() < q)
                break;
            beta /= 1 - q;  
        }
    }

    return L;
}


PathIntegrator * CreatePathIntegrator(PropertyList & list) {
    int maxDep = list.getInteger("maxDepth", 10);
    return new PathIntegrator(maxDep);
}

RAINBOW_NAMESPACE_END
