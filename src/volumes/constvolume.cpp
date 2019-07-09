#include "constvolume.h"

RAINBOW_NAMESPACE_BEGIN

Float ConstDensityVolume::LookUpFloat(
    const Point3f& p) const
{
    return value[0];
}

RGBSpectrum ConstDensityVolume::LookUpSpectrum(
    const Point3f& p) const
{
    return value;
}

Float ConstDensityVolume::GetMaxFloatValue() const 
{
    return value[0];
}

ConstDensityVolume* CreateConstDensityVolume(const PropertyList& list) {
    Float value = list.getFloat("value", 1);
    return new ConstDensityVolume(value);
}


RAINBOW_NAMESPACE_END
