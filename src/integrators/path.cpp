#include "path.h"

RAINBOW_NAMESPACE_BEGIN

RGBSpectrum PathIntegrator::Li(MemoryArena& arena, const Ray & r, const Scene & scene, Sampler &sampler, int depth) {
    RGBSpectrum L(0.f), beta(1);
    SurfaceInteraction inter;
    bool SpecularBounce = false;
    Ray ray(r);
    bool Trans = false;

    for (int bounce = 0;; bounce++) {        
        bool FoundIntersect = scene.Intersect(ray, &inter);

        if (bounce == 0 || SpecularBounce) {
            if (FoundIntersect) {
                L += beta * inter.Le(-ray.d);
            }
        }

        if (!FoundIntersect || bounce >= maxDepth) break;                

        inter.ComputeScatteringFunctions(arena);
        if (!inter.bsdf) {
            break;
        }        

        L += beta * UniformSampleOneLight(inter, scene);

        Vector3f wo = -ray.d, wi;
        Float BSDFPdf;
        BxDFType BSDFType;
        RGBSpectrum f = inter.bsdf->SampleF(wo, &wi, sampler.Get2D(), &BSDFPdf, BSDF_ALL, &BSDFType);
        if (BSDFPdf == 0 || f.IsBlack()) break;
        beta *= f * AbsDot(wi, inter.n) / BSDFPdf;

        SpecularBounce = (BSDFType & BSDF_SPECULAR) != 0;

        ray = inter.SpawnToRay(wi);

        if (bounce > 3) {
            Float q = std::min(1.0f, beta.MaxComponent());
            if (sampler.Get1D() >= q)
                break;
            beta /= q;  
        }
    }
    return L;
}


PathIntegrator * CreatePathIntegrator(PropertyList & list) {
    int maxDepth = list.getInteger("maxDepth", 5);
    int sampleNum = list.getInteger("sampleNum", 10);
    int delta = list.getInteger("delta", 1);
    return new PathIntegrator(maxDepth, sampleNum, delta); 
}

RAINBOW_NAMESPACE_END
