#pragma once
#ifndef __RAINBOW_HETEROGENEOUS_H
#define __RAINBOW_HETEROGENEOUS_H

#include "core/medium.h"
#include "core/volume.h"
#include "core/transform.h"

RAINBOW_NAMESPACE_BEGIN

class HeterogeneousMedium:public Medium{
public:
    HeterogeneousMedium(
        Volume *m_density,
        Volume *m_albedo,
        const Float &m_g,
        const Transform &w2m);

    RGBSpectrum Tr(
        Ray ray, 
        Sampler& sampler) const override;
    RGBSpectrum Sample(
        const Ray& ray, 
        Sampler& sampler, 
        MemoryArena& arena, 
        MediumInteraction* mi) const override;

private:

    const Transform WorldToMedium;

    /*
     * albedo = sigma_s / sigma_t
     * density = sigma_t
     * maxDensity = sigma
     * 
     * sigma_n(null-collision) = maxDensity - density
     * sigma_s = density * albedo
     * sigma_a = density * (1 - albedo)
     * 
     */
    Volume *density = nullptr;
    Volume *albedo = nullptr;   // sigma_s / sigma_t
    Float maxDensity;
    Float invMaxDensity;

    // g is for phase function
    Float g;
};

RAINBOW_NAMESPACE_END

#endif //__RAINBOW_HETEROGENEOUS_H