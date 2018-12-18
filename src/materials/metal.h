#ifndef __RAINBOW_METAL_H
#define __RAINBOW_METAL_H

#include "../core/material.h"

RAINBOW_NAMESPACE_BEGIN

class MetalMaterial :public Material {
public:
    //MetalMaterial(const RGBSpectrum& m_R,const Float& etaI,) :R(m_R) {}

    void ComputeScatteringFunctions(MemoryArena& arena, SurfaceInteraction* intersection);

    RGBSpectrum R;
};

MetalMaterial* CreateMetalMaterial(PropertyList& list);

RAINBOW_NAMESPACE_END

#endif // __RAINBOW_METAL_H