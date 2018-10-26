#include "light.h"

RAINBOW_NAMESPACE_BEGIN


RGBSpectrum AreaLight::L(const Interaction& interaction, const Vector3f & w) const {
	return (Dot(interaction.n, w) > 0) ? Lemit : RGBSpectrum(0.0);
}

RGBSpectrum AreaLight::SampleLi(const Interaction& ref, const Point2f& sample, 
	Vector3f* wi, Float* pdf) const {

	Interaction pShape = shape->Sample(ref, sample, pdf);
	
	if (*pdf == 0 || (pShape.p - ref.p).LengthSquare() == 0) {
		*pdf = 0;
		return 0.f;
	}
	
	*wi = Normalize(pShape.p - ref.p);	
	return L(pShape, -*wi);
}

std::shared_ptr<AreaLight> CreateAreaLight(PropertyList & list, const std::shared_ptr<Shape>& shape) {
	RGBSpectrum L = list.getColor("radiance", RGBSpectrum(1.0));
	return std::make_shared<AreaLight>(L, shape);
}

RAINBOW_NAMESPACE_END
