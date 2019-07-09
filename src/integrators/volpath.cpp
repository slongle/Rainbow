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

    int bounce;
    for (bounce = 0;; bounce++) {
        SurfaceInteraction inter;
        bool foundIntersect = scene.IntersectP(ray, &inter);

        // TODO:delete
        //if (!foundIntersect) break;

        MediumInteraction mi;
        if (ray.medium) {
            beta *= ray.medium->Sample(ray, sampler, arena, &mi);
        }
        if (beta.IsBlack()) break;

        if (mi.IsValid()) {
            if (bounce >= maxDepth) break;

            L += beta * UniformSampleOneLight(mi, scene, sampler, true);

            Vector3f wo = -ray.d, wi;
            mi.phase->SampleP(wo, &wi, sampler.Next2D());
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
                //return RGBSpectrum(1, 0, 0);
                ray = inter.SpawnToRay(ray.d);
                bounce--;
                continue;
            }

            L += beta * UniformSampleOneLight(inter, scene, sampler, true);

            Vector3f wo = -ray.d, wi;
            Float BSDFPdf;
            BxDFType BSDFType;
            RGBSpectrum f = inter.bsdf->SampleF(wo, &wi, sampler.Next2D(), &BSDFPdf, BSDF_ALL, &BSDFType);
            if (BSDFPdf == 0. || f.IsBlack()) break;
            beta *= f * AbsDot(wi, inter.n) / BSDFPdf;

            SpecularBounce = (BSDFType & BSDF_SPECULAR) != 0;

            ray = inter.SpawnToRay(wi);            
        }

        // Russian roulette
        if (bounce > 3) {
            Float q = std::min((Float)0.95, beta.MaxComponent());
            if (sampler.Next1D() >= q)
                break;
            beta /= q;
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


VolPathIntegrator * CreateVolPathIntegrator(
    PropertyList              &list,
    std::shared_ptr<Camera>    camera,
    std::shared_ptr<Sampler>   sampler) 
{
    int maxDepth = list.getInteger("maxDepth", 5);
    int sampleNum = list.getInteger("sampleNum", 10);
    int delta = list.getInteger("delta", 1);
    return new VolPathIntegrator(camera, sampler, maxDepth, sampleNum, delta);
}

RAINBOW_NAMESPACE_END
