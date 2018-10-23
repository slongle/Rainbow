#include "light.h"

RAINBOW_NAMESPACE_BEGIN


RGBSpectrum AreaLight::L(const SurfaceInteraction& interaction, const Vector3f & w) const {
	return (Dot(interaction.n, w) > 0) ? Lemit : RGBSpectrum(0.0);
}

RGBSpectrum AreaLight::SampleLi(const Point3f& p, const Point2f& sample, Vector3f* wo, Float* pdf) const {
	Point3f pLight = shape->Sample(sample, pdf);
	*wo = Normalize(pLight - p);
	return Lemit;
}

std::shared_ptr<AreaLight> CreateAreaLight(PropertyList & list, const std::shared_ptr<Shape>& shape) {
	RGBSpectrum L = list.getColor("radiance", RGBSpectrum(1.0));
	return std::make_shared<AreaLight>(L, shape);
}

RAINBOW_NAMESPACE_END
