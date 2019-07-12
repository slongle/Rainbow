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
    
    virtual Float LookUpDensityFloat(const Point3f& p) const = 0;
    virtual RGBSpectrum LookUpSpectrum(const Point3f& p) const = 0;
    virtual RGBSpectrum LookUpEmissionSpectrum(const Point3f& p) const = 0;
    
    Float GetMaxFloatValue() const {
        return m_maxDensity;
    }

    Bounds3f GetBounds() const {
        return m_bounds;
    }

protected:
    Bounds3f m_bounds;
    Float m_maxDensity;

};

RAINBOW_NAMESPACE_END

#endif //__RAINBOW_VOLUME_H
