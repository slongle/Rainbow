#pragma once
#ifndef __RAINBOW_IESLIGHT_H
#define __RAINBOW_IESLIGHT_H

#include "core/light.h"
#include "iesparser.h"

RAINBOW_NAMESPACE_BEGIN

class IESLight :public Light {
public:
    IESLight(
        const Transform& m_LightToWorld,
        const MediumInterface& m_mediumInterface,
        const std::string& path,
        const RGBSpectrum& m_I);

    Float PdfLi(
        const Interaction&   ref, 
        const Vector3f&      wi) const override;
    
    RGBSpectrum SampleLi(
        const Interaction&   intersection, 
        const Point2f&       sample, 
        Vector3f*            wi, 
        Float*               pdf,
        Visibility*          vis) const override;

    bool IsDeltaLight() const override 
    {
        return true;
    }

    std::string toString() const override {
        return tfm::format(
            "IESLight[]"
        );
    }

private:
    const RGBSpectrum   I;
    const Point3f       pLight;
    float               invMaxValue;
    IESFileInfo         info;
    IESLoadHelper       IESLoader;
};

std::shared_ptr<IESLight> CreateIESLight(const PropertyList & list);

RAINBOW_NAMESPACE_END

#endif  // __RAINBOW_IESLIGHT_H
