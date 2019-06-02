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
    int bounce;

    for (bounce = 0;; bounce++) {        
        SurfaceInteraction inter;
        bool foundIntersect = scene.IntersectP(ray, &inter);

        if (bounce == 0 || SpecularBounce) {
            if (foundIntersect) {
                L += beta * inter.Le(-ray.d);
            }
        }
        //L = Clamp(L, RGBSpectrum(1, 1, 1));

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

        //L = Clamp(L, RGBSpectrum(1, 1, 1));

        Vector3f wo = -ray.d, wi;
        Float BSDFPdf;
        BxDFType BSDFType;
        RGBSpectrum f = inter.bsdf->SampleF(wo, &wi, sampler.Next2D(), &BSDFPdf, BSDF_ALL, &BSDFType);
        if (BSDFPdf == 0 || f.IsBlack()) break;
        beta *= f * AbsDot(wi, inter.n) / BSDFPdf;

        SpecularBounce = (BSDFType & BSDF_SPECULAR) != 0;
        if ((BSDFType & BSDF_SPECULAR) && (BSDFType & BSDF_REFRACTION)) {
            Float eta = inter.bsdf->eta;
            etaScale *= (Dot(wo, inter.n) > 0) ? (eta*eta) : 1 / (eta*eta);
        }

        ray = inter.SpawnToRay(wi);

        RGBSpectrum rrBeta = beta * etaScale;
        /*
        if (bounce > 5) {
             *Float q = std::min(1.0f, rrBeta.MaxComponent());
            if (sampler.Next1D() >= q)
                break;
            beta /= q;
        }
        */

        if (rrBeta.MaxComponent() < 1 && bounce > 3) {
            Float q = std::max((Float).05, 1 - rrBeta.MaxComponent());
            if (sampler.Next1D() < q) break;
            beta /= 1 - q;            
        }
    }

    if (L.IsBlack())
        statistic.zeroPath++;
    statistic.pathNum++;
    statistic.sumBounce += bounce;
    statistic.maxBounce = std::max(statistic.maxBounce, static_cast<unsigned long long>(bounce));
    statistic.minBounce = std::min(statistic.minBounce, static_cast<unsigned long long>(bounce));

    return L;
}


PathIntegrator * CreatePathIntegrator(
    PropertyList              &list,
    std::shared_ptr<Camera>    camera,
    std::shared_ptr<Sampler>   sampler)
{
    int maxDepth = list.getInteger("maxDepth", 5);
    int sampleNum = list.getInteger("sampleNum", 10);
    int delta = list.getInteger("delta", 1);
    return new PathIntegrator(camera, sampler, maxDepth, sampleNum, delta);
}

RAINBOW_NAMESPACE_END
