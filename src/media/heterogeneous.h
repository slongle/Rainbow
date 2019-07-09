#pragma once
#ifndef __RAINBOW_HETEROGENEOUS_H
#define __RAINBOW_HETEROGENEOUS_H

#include "core/medium.h"
#include "core/volume.h"
#include "core/transform.h"

RAINBOW_NAMESPACE_BEGIN

class HeterogeneousMedium :public Medium {
public:
    HeterogeneousMedium(
        const Volume *density,
        const Volume *albedo,
        const Float &scale,
        const Float &g,
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

    const Transform m_worldToMedium, m_mediumToWorld;

    /*
     * m_albedo = sigma_s / sigma_t
     * m_density = sigma_t = densityAtT * scale
     * m_maxDensity = sigma = getMaxDensity * scale
     * 
     * sigma_n(null-collision) = sigma - sigma_t
     * sigma_s = sigma_t * m_albedo
     * sigma_a = sigma_t * (1 - m_albedo)
     */
    const Volume *m_density = nullptr;
    const Volume *m_albedo = nullptr;
    Float m_scale;
    Float m_maxDensity;
    Float m_invMaxDensity;

    // m_g is for phase function
    Float m_g;

    Bounds3f m_box;
};

Medium* CreateHeterogeneousMedium(
    const PropertyList &list, 
    const Volume *densityVolume, 
    const Volume *albedoVolume);

RAINBOW_NAMESPACE_END

#endif //__RAINBOW_HETEROGENEOUS_H