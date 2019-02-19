#include "volpath.h"

RAINBOW_NAMESPACE_BEGIN

RGBSpectrum VolPathIntegrator::Li(
    MemoryArena &arena,
    const Ray &r,
    const Scene &scene,
    Sampler &sampler,
    int depth)
{
    RGBSpectrum L(0.), beta(1.);
    Ray ray(r);
    bool SpecularBounce = false;

    for (int bounce = 0;; bounce++) {
        SurfaceInteraction inter;
        bool foundIntersect = scene.IntersectP(ray, &inter);

        MediumInteraction mi;
        if (ray.medium) beta *= ray.medium->Sample(ray, sampler, arena, &mi);
        if (beta.IsBlack()) break;

        if (mi.IsValid()) {
            if (bounce >= maxDepth) break;

            L += beta * UniformSampleOneLight(mi, scene, sampler, true);

            Vector3f wo = -ray.d, wi;
            mi.phase->SampleP(wo, &wi, sampler.Get2D());
            ray = mi.SpawnToRay(wi);
            SpecularBounce = false;
        }
        else {
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

            L += beta * UniformSampleOneLight(inter, scene, sampler, true);

            Vector3f wo = -ray.d, wi;
            Float BSDFPdf;
            BxDFType BSDFType;
            RGBSpectrum f = inter.bsdf->SampleF(wo, &wi, sampler.Get2D(), &BSDFPdf, BSDF_ALL, &BSDFType);
            if (BSDFPdf == 0. || f.IsBlack()) break;
            beta *= f * AbsDot(wi, inter.n) / BSDFPdf;

            SpecularBounce = (BSDFType & BSDF_SPECULAR) != 0;

            ray = inter.SpawnToRay(wi);            
        }

        // Russian roulette
        if (bounce > 3) {
            Float q = std::min((Float)0.95, beta.MaxComponent());
            if (sampler.Get1D() >= q)
                break;
            beta /= q;
        }
    }
    return L;
}


VolPathIntegrator * CreateVolPathIntegrator(PropertyList & list) {
    int maxDepth = list.getInteger("maxDepth", 5);
    int sampleNum = list.getInteger("sampleNum", 10);
    int delta = list.getInteger("delta", 1);
    return new VolPathIntegrator(maxDepth, sampleNum, delta);
}

RAINBOW_NAMESPACE_END