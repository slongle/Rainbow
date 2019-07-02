#include "heterogeneous.h"

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

    return RGBSpectrum(0.);
}

RAINBOW_NAMESPACE_END