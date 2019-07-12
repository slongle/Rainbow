#include "constvolume.h"

RAINBOW_NAMESPACE_BEGIN

Float ConstDensityVolume::LookUpDensityFloat(
    const Point3f& p) const
{
    return value[0];
}

RGBSpectrum ConstDensityVolume::LookUpSpectrum(
    const Point3f& p) const
{
    return value;
}

RGBSpectrum ConstDensityVolume::LookUpEmissionSpectrum(const Point3f& p) const
{
    return RGBSpectrum(0.);
}

ConstDensityVolume* CreateConstDensityVolume(const PropertyList& list) {
    Float value = list.getFloat("value", 1);
    return new ConstDensityVolume(value);
}


RAINBOW_NAMESPACE_END
