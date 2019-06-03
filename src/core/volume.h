#pragma once
#ifndef __RAINBOW_VOLUME_H
#define __RAINBOW_VOLUME_H

#include <string>
#include "spectrum.h"

RAINBOW_NAMESPACE_BEGIN

class Volume {
public:
    Volume(){}
    virtual void LoadFile(const std::string& filename) = 0;
    virtual Float LookUpFloat(const Float& x, const Float& y, const Float& z)const = 0;
    virtual RGBSpectrum LookUpSpectrum(const Float& x, const Float& y, const Float& z)const = 0;
};

RAINBOW_NAMESPACE_END

#endif //__RAINBOW_VOLUME_H
