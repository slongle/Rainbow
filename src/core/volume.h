#pragma once
#ifndef __RAINBOW_VOLUME_H
#define __RAINBOW_VOLUME_H

#include <string>
#include "spectrum.h"
#include "bbox.h"

RAINBOW_NAMESPACE_BEGIN

class Volume {
public:
    Volume(){}
    virtual Float LookUpFloat(const Point3f& p) const = 0;
    virtual RGBSpectrum LookUpSpectrum(const Point3f& p) const = 0;
    virtual Float GetMaxFloatValue() const = 0;

    Bounds3f& GetBounds() {
        return box;
    }

private:
    Bounds3f box;

};

RAINBOW_NAMESPACE_END

#endif //__RAINBOW_VOLUME_H
