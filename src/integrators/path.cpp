#include "path.h"

RAINBOW_NAMESPACE_BEGIN

RGBSpectrum PathIntegrator::Li(
    MemoryArena &arena, 
    const Ray &r, 
    const Scene &scene, 
    Sampler &sampler, 
    int depth) 
{
    RGBSpectrum L(0.f), beta(1);
    Float etaScale = 1;
    bool SpecularBounce = false;
    Ray ray(r);

    for (int bounce = 0;; bounce++) {        
        SurfaceInteraction inter;
        bool foundIntersect = scene.IntersectP(ray, &inter);

        if (bounce == 0 || SpecularBounce) {
            if (foundIntersect) {
                L += beta * inter.Le(-ray.d);
            }
        }

        if (!foundIntersect || bounce >= maxDepth) break;                

        inter.ComputeScatteringFunctions(arena);
        if (!inter.bsdf) {
            ray = inter.SpawnToRay(ray.d);
            bounce--;
            continue;
        }        

        if (inter.bsdf->NumComponents(BxDFType(BSDF_ALL & ~BSDF_SPECULAR)) > 0) {
            L += beta * UniformSampleOneLight(inter, scene, sampler);       
        }

        Vector3f wo = -ray.d, wi;
        Float BSDFPdf;
        BxDFType BSDFType;
        RGBSpectrum f = inter.bsdf->SampleF(wo, &wi, sampler.Next2D(), &BSDFPdf, BSDF_ALL, &BSDFType);
        if (BSDFPdf == 0 || f.IsBlack()) break;
        beta *= f * AbsDot(wi, inter.n) / BSDFPdf;

        SpecularBounce = (BSDFType & BSDF_SPECULAR) != 0;
        if ((BSDFType & BSDF_SPECULAR) && (BSDFType & BSDF_TRANSMISSION)) {
            Float eta = inter.bsdf->eta;
            etaScale *= (Dot(wo, inter.n) > 0) ? (eta*eta) : 1 / (eta*eta);
        }

        ray = inter.SpawnToRay(wi);

        RGBSpectrum rrBeta = beta * etaScale;
        if (bounce > 3) {
            Float q = std::min(1.0f, rrBeta.MaxComponent());
            if (sampler.Next1D() >= q)
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
