#pragma once
#ifndef __RAINBOW_VOLUME_H
#define __RAINBOW_VOLUME_H

#include <string>
#include "bbox.h"
#include "propertylist.h"

RAINBOW_NAMESPACE_BEGIN

class Volume {
public:
    Volume(){}
    virtual Float LookUpFloat(const Point3f& p) const = 0;
    virtual RGBSpectrum LookUpSpectrum(const Point3f& p) const = 0;
    virtual Float GetMaxFloatValue() const = 0;

    Bounds3f GetBounds() const {
        return m_bounds;
    }

protected:
    Bounds3f m_bounds;

};

RAINBOW_NAMESPACE_END

#endif //__RAINBOW_VOLUME_H
