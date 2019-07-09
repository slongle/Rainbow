#include "heterogeneous.h"
#include "core/sampler.h"

RAINBOW_NAMESPACE_BEGIN

HeterogeneousMedium::HeterogeneousMedium(
    const Volume *density,
    const Volume *albedo,
    const Float &scale,
    const Float &g,
    const Transform &w2m)
    :m_density(density), m_albedo(albedo), m_scale(scale), m_g(g), m_worldToMedium(w2m)
{
    m_maxDensity = m_density->GetMaxFloatValue() * m_scale;
    m_invMaxDensity = 1. / m_maxDensity;
    m_box = m_density->GetBounds();
}

RGBSpectrum 
HeterogeneousMedium::Tr(Ray rWorld, Sampler& sampler) const 
{
    Assert(std::abs(rWorld.d.LengthSquare() - 1) < Epsilon, "No Normalizeing");

    Ray rMedium = m_worldToMedium(rWorld);
    //rMedium.tMax = rMedium.tMax*rMedium.d.Length();
    //rMedium.d = Normalize(rMedium.d);
    //Assert(std::abs(rMedium.d.LengthSquare() - 1) < Epsilon, "No Normalizeing");

    Float Tr = 1.;
    Float tMin, tMax;
    if (!m_box.Intersect(rMedium, &tMin, &tMax)) {
        return RGBSpectrum(1.);
    }

    tMin = std::max(tMin, (Float)0.);
    tMax = std::min(tMax, rMedium.tMax);
    //int cnt= 0;

    Float t = tMin;
    while (true) {
        t += -std::log(1 - sampler.Next1D()) * m_invMaxDensity;
        if (t > tMax) {
            break;
        }

        Point3f p = rMedium(t);
        Float densityAtT = m_density->LookUpFloat(p) * m_scale;
        Tr *= 1 - densityAtT * m_invMaxDensity;

        const Float rrThreshold = .1;
        if (Tr < rrThreshold) {
            Float q = std::max((Float).05, 1 - Tr);
            if (sampler.Next1D() < q) {
                return RGBSpectrum(0.);
            }
            Tr /= 1 - q;
        }
        /*cnt++;
        if(cnt==1000) {
            int b = 1;
        }*/
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

    Ray rMedium = m_worldToMedium(rWorld);
    //rMedium.tMax = rMedium.tMax*rMedium.d.Length();
    //rMedium.d = Normalize(rMedium.d);
    //Assert(std::abs(rMedium.d.LengthSquare() - 1) < Epsilon, "No Normalizeing");

    Float tMin, tMax;
    if (!m_box.Intersect(rMedium, &tMin, &tMax)) {
        return RGBSpectrum(1.);
    }
    tMin = std::max(tMin, (Float)0.);
    tMax = std::min(tMax, rMedium.tMax);

    //int cnt = 0;

    Float t = tMin;
    while(true) {
        t += -std::log(1 - sampler.Next1D()) * m_invMaxDensity;
        if (t > tMax) {            
            break;
        }

        Point3f p = rMedium(t);
        Float densityAtT = m_density->LookUpFloat(p) * m_scale;        
        if (sampler.Next1D() < densityAtT * m_invMaxDensity) {
            Ray ray = m_mediumToWorld(rMedium);
            *mi = MediumInteraction(p, -rMedium.d, this, ARENA_ALLOCA(arena, HenyeyGreenstein)(m_g));
            RGBSpectrum albedoAtT = m_albedo->LookUpSpectrum(p);
            //RGBSpectrum sigma_t = densityAtT;
            //RGBSpectrum sigma_s = densityAtT * albedoAtT;
            //RGBSpectrum sigma_a = RGBSpectrum(densityAtT) - sigma_s;

            return albedoAtT;
        }

        /*cnt++;
        if (cnt == 1000) {
            int b = 1;
        }*/
    }

    return RGBSpectrum(1.);
}

Medium* CreateHeterogeneousMedium(
    const PropertyList& list, 
    const Volume* densityVolume,
    const Volume* albedoVolume) 
{
    Transform w2m = list.getTransform("toWorld", Transform::identityTransform);
    Float scale = list.getFloat("scale", 1.);
    Float g = list.getFloat("g", 0);
    return new HeterogeneousMedium(densityVolume, albedoVolume, scale, g, w2m);
}


RAINBOW_NAMESPACE_END
