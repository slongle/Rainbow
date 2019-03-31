#pragma once
#ifndef __RAINBOW_SPOT_H
#define __RAINBOW_SPOT_H

#include "src/core/light.h"

RAINBOW_NAMESPACE_BEGIN

class SpotLight :public Light {
public:
    SpotLight(const Transform& m_LightToWorld,
        const MediumInterface& m_mediumInterface,
        const RGBSpectrum& m_I,
        const Float& thetaTotalWidth,
        const Float& thetaFalloffStart)
        :Light(m_LightToWorld, m_mediumInterface),
        I(m_I), pLight(LightToWorld(Point3f(0, 0, 0))),
        cosTotalWidth(std::cos(Radians(thetaTotalWidth))),
        cosFalloffStart(std::cos(Radians(thetaFalloffStart))) 
    {
        //cout << pLight << endl;
    }

    Float PdfLi(const Interaction& ref, const Vector3f& wi) const override;
    RGBSpectrum SampleLi(
        const Interaction& intersection, 
        const Point2f& sample, 
        Vector3f* wi, Float* pdf,
        Visibility* vis) const override;
    
    bool IsDeltaLight() const override {
        return true;
    }

private:
    const RGBSpectrum I;
    const Point3f pLight;
    const Float cosTotalWidth, cosFalloffStart;

    Float Falloff(const Vector3f& wo) const;
};

std::shared_ptr<SpotLight> CreateSpotLight(PropertyList& list);

RAINBOW_NAMESPACE_END

#endif //__RAINBOW_POINT_H