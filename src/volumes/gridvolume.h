#pragma once
#ifndef __RAINBOW_GRIDMEDIUM_H
#define __RAINBOW_GRIDMEDIUM_H

#include "core/volume.h"

RAINBOW_NAMESPACE_BEGIN

class GridDensityVolume :public Volume {
public:
    GridDensityVolume(){}
    Float LookUpFloat(const Point3f& p) const override;
    RGBSpectrum LookUpSpectrum(const Point3f& p) const override;
    Float GetMaxFloatValue() const override;
private:
    void LoadFile(const std::string& filename);
};

RAINBOW_NAMESPACE_END

#endif // __RAINBOW_GRIDMEDIUM_H