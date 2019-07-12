#pragma once
#ifndef __RAINBOW_CONSTVOLUME_H
#define __RAINBOW_CONSTVOLUME_H

#include "core/volume.h"

RAINBOW_NAMESPACE_BEGIN

class ConstDensityVolume :public Volume {
public:
    ConstDensityVolume(const Float& f) :value(f) { m_maxDensity = f; }
    ConstDensityVolume(const RGBSpectrum& s) :value(s) { m_maxDensity = s[0]; }

    Float LookUpDensityFloat(const Point3f& p) const override;
    RGBSpectrum LookUpSpectrum(const Point3f& p) const override;
    RGBSpectrum LookUpEmissionSpectrum(const Point3f& p) const override;

private:
    RGBSpectrum value;
};

ConstDensityVolume * CreateConstDensityVolume(const PropertyList &list);

RAINBOW_NAMESPACE_END

#endif //__RAINBOW_CONSTVOLUME_H