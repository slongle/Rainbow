#pragma once
#ifndef __RAINBOW_CONSTVOLUME_H
#define __RAINBOW_CONSTVOLUME_H

#include "core/volume.h"

RAINBOW_NAMESPACE_BEGIN

class ConstDensityVolume :public Volume {
public:
    ConstDensityVolume(const Float &f) :value(f) {}
    ConstDensityVolume(const RGBSpectrum &s) :value(s) {}

    Float LookUpFloat(const Point3f& p) const override;
    RGBSpectrum LookUpSpectrum(const Point3f& p) const override;
    Float GetMaxFloatValue() const override;
private:
    RGBSpectrum value;
};


RAINBOW_NAMESPACE_END

#endif //__RAINBOW_CONSTVOLUME_H