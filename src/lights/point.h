#pragma once
#ifndef __RAINBOW_POINTLIGHT_H
#define __RAINBOW_POINTLIGHT_H

#include "src/core/light.h"

RAINBOW_NAMESPACE_BEGIN

class PointLight :public Light {
public:
    PointLight(const Transform& m_LightToWorld,
        const MediumInterface& m_mediumInterface,
        const RGBSpectrum& m_I)
        :Light(m_LightToWorld, m_mediumInterface),
        I(m_I), pLight(LightToWorld(Point3f(0, 0, 0)))
    {}

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
};

std::shared_ptr<PointLight> CreatePointLight(PropertyList& list);

RAINBOW_NAMESPACE_END

#endif // __RAINBOW_POINTLIGHT_H