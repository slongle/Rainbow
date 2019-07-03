#pragma once
#ifndef __RAINBOW_ROUGHCONDUCTOR_H
#define __RAINBOW_ROUGHCONDUCTOR_H

#include "core/material.h"

RAINBOW_NAMESPACE_BEGIN

class RoughConductorMaterial :public Material {
public:
    RoughConductorMaterial(const RGBSpectrum& m_R, const RGBSpectrum& m_eta, const RGBSpectrum& m_k,
        const Float& m_alphaU, const Float& m_alphaV, const std::string& m_distributionType);
    RoughConductorMaterial(const RGBSpectrum& m_R, const RGBSpectrum& m_eta, const RGBSpectrum& m_k,
        const Float& m_alpha, const std::string& m_distributionType);

    void ComputeScatteringFunctions(MemoryArena& arena, SurfaceInteraction* intersection);

    RGBSpectrum R, eta, k;
    Float alphaU, alphaV;
    std::string distributionType;
};

RoughConductorMaterial* CreateRoughConductorMaterial(const PropertyList& list);

RAINBOW_NAMESPACE_END

#endif //__RAINBOW_ROUGHCONDUCTOR_H
