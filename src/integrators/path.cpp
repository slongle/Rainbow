#include "path.h"

RAINBOW_NAMESPACE_BEGIN

RGBSpectrum PathIntegrator::Li(const Ray & r, const Scene & scene, int depth) {

    RGBSpectrum L(0.f), beta(1);
    SurfaceInteraction inter;
    bool SpecularBounce = false;
    Ray ray(r);

    int goal = 2;
    int bounce = 0;
    for (bounce = 0;; bounce++) {
        //RGBSpectrum StoreL = L;
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

        delete inter.bxdf;

        // FIX:
        SpecularBounce = (inter.bxdf->type & BxDF::BSDF_SPECULAR) != 0;

        if (pdf == 0 || f.IsBlack()) break;
        beta *= f * AbsDot(wi, inter.n) / pdf;        


        ray = inter.SpawnToRay(wi);

        if (bounce > 3) {
            Float q = std::min(0.95f, beta.MaxComponent());
            if (sampler->Get1D() >= q)
                break;
            beta /= q;  
        }

        //if (bounce != goal) L = StoreL;
    }

    if (bounce == 0) return L;
    else return L / bounce/2;
}


PathIntegrator * CreatePathIntegrator(PropertyList & list) {
    int maxDep = list.getInteger("maxDepth", 10);
    return new PathIntegrator(maxDep);
}

RAINBOW_NAMESPACE_END
