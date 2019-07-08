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
HeterogeneousMedium::Tr(Ray rWorld, Sampler& sampler) const 
{
    Assert(std::abs(rWorld.d.LengthSquare() - 1) < Epsilon, "No Normalizeing");

    Ray rMedium = WorldToMedium(rWorld);
    Float Tr = 1.;
    Float tMin, tMax;
    if (!box.Intersect(rMedium, &tMin, &tMax)) {
        return RGBSpectrum(1.);
    }

    Float t = 0;
    while (true) {
        t += -std::log(1 - sampler.Next1D()) * invMaxDensity;
        if (t > tMax) {
            break;
        }

        Point3f p = rMedium(t);
        Float densityAtT = density->LookUpFloat(p);
        Tr *= 1 - densityAtT * invMaxDensity;
    }

    return RGBSpectrum(Tr);
}

RGBSpectrum 
HeterogeneousMedium::Sample(
    const Ray& rWorld, 
    Sampler& sampler, 
    MemoryArena& arena,
    MediumInteraction* mi) const 
{
    Assert(std::abs(rWorld.d.LengthSquare() - 1) < Epsilon, "No Normalizeing");

    Ray rMedium = WorldToMedium(rWorld);
    Float tMin, tMax;
    if (!box.Intersect(rMedium, &tMin, &tMax)) {
        return RGBSpectrum(1.);
    }
    
    Float t = 0;
    while(true) {
        t += -std::log(1 - sampler.Next1D()) * invMaxDensity;
        if (t > tMax) {            
            break;
        }

        Point3f p = rMedium(t);
        Float densityAtT = density->LookUpFloat(p);        
        if (sampler.Next1D() < densityAtT * invMaxDensity) {
            RGBSpectrum albedoAtT = albedo->LookUpSpectrum(p);
            //RGBSpectrum sigma_t = densityAtT;
            //RGBSpectrum sigma_s = densityAtT * albedoAtT;
            //RGBSpectrum sigma_a = RGBSpectrum(densityAtT) - sigma_s;

            return albedoAtT;
        }
    }

    return RGBSpectrum(1.);
}

Medium* CreateHeterogeneousMedium(const PropertyList& list) {
    return nullptr;
}

RAINBOW_NAMESPACE_END
