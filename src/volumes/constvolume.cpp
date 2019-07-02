#include "constvolume.h"

RAINBOW_NAMESPACE_BEGIN

Float ConstDensityVolume::LookUpFloat(
    const Float& x, 
    const Float& y, 
    const Float& z) const 
{
    return value[0];
}

RGBSpectrum ConstDensityVolume::LookUpSpectrum(
    const Float& x, 
    const Float& y, 
    const Float& z) const 
{
    return value;
}

Float ConstDensityVolume::GetMaxFloatValue() const 
{
    return value[0];
}


RAINBOW_NAMESPACE_END
