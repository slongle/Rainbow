#include "point.h"

RAINBOW_NAMESPACE_BEGIN

Float PointLight::PdfLi(
    const Interaction & ref,
    const Vector3f & wi) const
{
    return 0.;
}

RGBSpectrum PointLight::SampleLi(
    const Interaction & intersection,
    const Point2f & sample,
    Vector3f * wi,
    Float * pdf,
    Visibility * vis) const
{
    *wi = Normalize(pLight - intersection.p);
    *pdf = 1;
    *vis = Visibility(intersection, Interaction(pLight, mediumInterface));
    return I / DistanceSquare(intersection.p, pLight);
}

std::shared_ptr<PointLight> CreatePointLight(const PropertyList & list)
{
    Transform lightToWorld = list.getTransform("toWorld", Transform());
    MediumInterface mediumInterface;
    RGBSpectrum I = list.getColor("intensity", RGBSpectrum(1));
    return std::make_shared<PointLight>(lightToWorld, mediumInterface, I);
}

RAINBOW_NAMESPACE_END