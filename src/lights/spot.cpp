#include "spot.h"

RAINBOW_NAMESPACE_BEGIN

Float SpotLight::PdfLi(
    const Interaction & ref, 
    const Vector3f & wi) const 
{
    return 0.;
}

RGBSpectrum SpotLight::SampleLi(
    const Interaction & intersection, 
    const Point2f & sample, 
    Vector3f * wi, 
    Float * pdf, 
    Visibility * vis) const 
{
    *wi = Normalize(pLight - intersection.p);
    *pdf = 1;
    *vis = Visibility(intersection, Interaction(pLight, mediumInterface));
    Vector3f woLocal = WorldToLight(-*wi);
    return I * Falloff(woLocal) / DistanceSquare(intersection.p, pLight);
}

Float SpotLight::Falloff(const Vector3f& wo) const {
    Vector3f localWo = WorldToLight(wo);
    Float cosTheta = localWo.z;    
    if (cosTheta >= cosFalloffStart) return 1;
    if (cosTheta < cosTotalWidth) return 0;
    Float ret = (cosTheta - cosTotalWidth) / (cosFalloffStart - cosTotalWidth);
    return (ret*ret)*(ret*ret);
}

std::shared_ptr<SpotLight> CreateSpotLight(const PropertyList & list)
{
    Transform lightToWorld = list.getTransform("toWorld", Transform());
    MediumInterface mediumInterface;
    RGBSpectrum I = list.getColor("intensity", RGBSpectrum(1));
    Float thetaTotalWidth = list.getFloat("totalAngle", 90);
    Float thetaFalloffStart = list.getFloat("falloffAngle", 45);
    return std::make_shared<SpotLight>(
        lightToWorld, mediumInterface, I, thetaTotalWidth, thetaFalloffStart);
}

RAINBOW_NAMESPACE_END
