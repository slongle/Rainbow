#include "heterogeneous.h"
#include "core/sampler.h"

RAINBOW_NAMESPACE_BEGIN

HeterogeneousMedium::HeterogeneousMedium(
    Volume *m_density,
    Volume *m_albedo,
    const Float &m_g,
    const Transform &w2m)
    :density(m_density), albedo(m_albedo), g(m_g), WorldToMedium(w2m)
{
    maxDensity = density->GetMaxFloatValue();
    invMaxDensity = 1. / maxDensity;
    box = density->GetBounds();
}

RGBSpectrum 
HeterogeneousMedium::Tr(Ray ray, Sampler& sampler) const 
{
    return RGBSpectrum(0.);
}

RGBSpectrum 
HeterogeneousMedium::Sample(
    const Ray& ray, 
    Sampler& sampler, 
    MemoryArena& arena,
    MediumInteraction* mi) const 
{
    Ray r = WorldToMedium(ray);
    Float t0, t1;
    bool hit = box.Intersect(ray, &t0, &t1);
    t1 = std::min(t1, ray.tMax);

    if (!hit) {
        return RGBSpectrum(1.);
    }

    while(true) {
        Float t = -std::log(1 - sampler.Next1D()) * invMaxDensity;
        if (t > t1) {
            mi = nullptr;
            return RGBSpectrum(0.);
        }

        Point3f p = r(t1);
        Float densityAtT = density->LookUpFloat(p);        

        if (sampler.Next1D() < densityAtT * invMaxDensity) {
            RGBSpectrum albedoAtT = albedo->LookUpSpectrum(p);
            RGBSpectrum sigma_s = densityAtT * albedoAtT;
            RGBSpectrum sigma_a = RGBSpectrum(densityAtT) - sigma_s;

            //return sigma_s / sigma_t;
        }
        break;
    }

    return RGBSpectrum(1.);
}

RAINBOW_NAMESPACE_END