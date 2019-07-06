#include "homogeneous.h"
#include "core/propertylist.h"
#include "core/sampler.h"
#include "core/interaction.h"

RAINBOW_NAMESPACE_BEGIN

RGBSpectrum 
HomogeneousMedium::Tr(
    Ray       ray, 
    Sampler & sampler) const 
{
    return Exp(-sigma_t * std::min(ray.tMax*ray.d.Length(), MAXFLOAT));
}

RGBSpectrum 
HomogeneousMedium::Sample(
    const Ray         & ray, 
    Sampler           & sampler, 
    MemoryArena       & arena, 
    MediumInteraction * mi) const 
{
    Assert(std::abs(ray.d.LengthSquare() - 1) < DBL_EPSILON, "No Normalizeing");

    int channel = std::min((int)(3 * sampler.Next1D()), 2);
    Float dist = -std::log(1 - sampler.Next1D()) / sigma_t[channel];
    Float t = std::min(dist / ray.d.Length(), ray.tMax);
    bool sampledMedium = t < ray.tMax;
    if (sampledMedium)
        *mi = MediumInteraction(ray(t), -ray.d, this, ARENA_ALLOCA(arena, HenyeyGreenstein)(g));
    RGBSpectrum Tr = Exp(-sigma_t * std::min(t*ray.d.Length(), MAXFLOAT));
    
    // Return weighting factor for scattering from homogeneous medium
    RGBSpectrum density = sampledMedium ? (sigma_t * Tr) : Tr;
    Float pdf = 0;
    for (int i = 0; i < 3; ++i) pdf += density[i];
    pdf /= (Float) 3;

    if (pdf == 0) {        
        pdf = 1;
    }
    return sampledMedium ? (Tr * sigma_s / pdf) : (Tr / pdf);
}

Medium * CreateHomogeneousMedium(const PropertyList & list) {
    RGBSpectrum sigma_a = list.getColor("sigmaA", RGBSpectrum(0.));
    RGBSpectrum sigma_s = list.getColor("sigmaS", RGBSpectrum(1.));
    Float g = list.getFloat("g", 0.);
    return new HomogeneousMedium(sigma_a, sigma_s, g);
}

RAINBOW_NAMESPACE_END
