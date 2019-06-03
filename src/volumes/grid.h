#pragma once
#ifndef __RAINBOW_GRIDMEDIUM_H
#define __RAINBOW_GRIDMEDIUM_H

#include "core/volume.h"

RAINBOW_NAMESPACE_BEGIN

class GridDensityVolume :public Volume {
public:
    GridDensityVolume(){}
    void LoadFile(const std::string& filename) override;
    Float LookUpFloat(const Float& x, const Float& y, const Float& z) const override;
    RGBSpectrum LookUpSpectrum(const Float& x, const Float& y, const Float& z) const override;
};

RAINBOW_NAMESPACE_END

#endif // __RAINBOW_GRIDMEDIUM_H