#pragma once
#ifndef __RAINBOW_HOMOGENEOUS_H
#define __RAINBOW_HOMOGENEOUS_H

#include "src/core/medium.h"

RAINBOW_NAMESPACE_BEGIN

class HomogeneousMedium :public Medium {
public:
    HomogeneousMedium(
        const RGBSpectrum& m_sigma_a, const RGBSpectrum& m_sigma_s, const Float& m_g) :
        sigma_a(m_sigma_a), sigma_s(m_sigma_s), sigma_t(sigma_a + sigma_s), g(m_g) {}

    RGBSpectrum Tr(Ray ray, Sampler& sampler) const override;
    RGBSpectrum Sample(const Ray &ray, Sampler &sampler,
        MemoryArena &arena, MediumInteraction *mi) const override;

    const RGBSpectrum sigma_a, sigma_s, sigma_t;
    const Float g;
};

Medium *CreateHomogeneousMedium(PropertyList& list);

RAINBOW_NAMESPACE_END

#endif // __RAINBOW_HOMOGENEOUS_H