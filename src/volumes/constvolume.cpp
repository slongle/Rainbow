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


RAINBOW_NAMESPACE_END
