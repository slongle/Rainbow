#ifndef __RAINBOW_MIRROR_H
#define __RAINBOW_MIRROR_H

#include "../core/material.h"

RAINBOW_NAMESPACE_BEGIN

class MirrorMaterial :public Material {
public:
    MirrorMaterial(const RGBSpectrum& m_R) :R(m_R) {}
    void ComputeScatteringFunctions(SurfaceInteraction* intersection) override;

private:
    RGBSpectrum R;
};

MirrorMaterial* CreateMirrorMaterial(PropertyList& list);

RAINBOW_NAMESPACE_END

#endif // __RAINBOW_MIRROR_H