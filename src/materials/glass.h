#ifndef __RAINBOW_METAL_H
#define __RAINBOW_METAL_H

#include "../core/material.h"

RAINBOW_NAMESPACE_BEGIN

class GlassMaterial :public Material {
public:
    GlassMaterial(const RGBSpectrum& m_R, const RGBSpectrum& m_T, const Float& m_etaI, const Float& m_etaT)
        :R(m_R), T(m_T), etaI(m_etaI), etaT(m_etaT) {}

    void ComputeScatteringFunctions(MemoryArena& arena, SurfaceInteraction* intersection);

    Float etaI, etaT;
    RGBSpectrum T, R;
};

GlassMaterial* CreateGlassMaterial(PropertyList& list);

RAINBOW_NAMESPACE_END

#endif // __RAINBOW_METAL_H