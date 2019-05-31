#pragma once
#ifndef __RAINBOW_GRIDMEDIUM_H
#define __RAINBOW_GRIDMEDIUM_H

#include "core/medium.h"

RAINBOW_NAMESPACE_BEGIN

class GridMedium :public Medium {
public:
    GridMedium(){}
    RGBSpectrum Tr(Ray ray, Sampler& sampler) const override;
    RGBSpectrum Sample(const Ray& ray, Sampler& sampler, MemoryArena& arena, MediumInteraction* mi) const override;
};

RAINBOW_NAMESPACE_END

#endif // __RAINBOW_GRIDMEDIUM_H