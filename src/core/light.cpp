#include "light.h"
#include "scene.h"

RAINBOW_NAMESPACE_BEGIN

// Return true only if on intersection
bool Visibility::Occluded(const Scene & scene) const {
	return scene.IntersectP(p0.SpawnToRay(p1));
}

RGBSpectrum AreaLight::L(const Interaction& interaction, const Vector3f & w) const {
	return (Dot(interaction.n, w) > 0) ? Lemit : RGBSpectrum(0.0);
}

Float AreaLight::PdfLi(const SurfaceInteraction & ref, const Vector3f & wi) const {
    Ray ray = ref.SpawnToRay(wi);
    Float tHit;
    SurfaceInteraction inter;
    if (!shape->Intersect(ray, &tHit, &inter))
        return 0;
    Float Pdf = DistanceSquare(ref.p, inter.p) / (AbsDot(-ray.d, inter.n) * shape->Area());
    if (std::isinf(Pdf)) 
        Pdf = 0;
    return Pdf;
}

RGBSpectrum AreaLight::SampleLi(const Interaction& ref, const Point2f& sample, 
	Vector3f* wi, Float* pdf, Visibility* vis) const {

	Interaction pShape = shape->Sample(ref, sample, pdf);
	
	if (*pdf == 0 || (pShape.p - ref.p).LengthSquare() == 0) {
		*pdf = 0;
		return 0.f;
	}
	
	*wi = Normalize(pShape.p - ref.p);
	*vis = Visibility(ref, pShape);
	return L(pShape, -*wi);
}

std::shared_ptr<AreaLight> CreateAreaLight(PropertyList & list, const std::shared_ptr<Shape>& shape) {
	RGBSpectrum L = list.getColor("radiance", RGBSpectrum(1.0));
	return std::make_shared<AreaLight>(L, shape);
}

RAINBOW_NAMESPACE_END
