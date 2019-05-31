#pragma once
#ifndef __RAINBOW_HETEROGENEOUS_H
#define __RAINBOW_HETEROGENEOUS_H

#include "core/medium.h"

RAINBOW_NAMESPACE_BEGIN

class HeterogeneousMedium:public Medium{
public:
    RGBSpectrum Tr(
        Ray ray, 
        Sampler& sampler) const override;
    RGBSpectrum Sample(
        const Ray& ray, 
        Sampler& sampler, 
        MemoryArena& arena, 
        MediumInteraction* mi) const override;
};

RAINBOW_NAMESPACE_END

#endif //__RAINBOW_HETEROGENEOUS_H