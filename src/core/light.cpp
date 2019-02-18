#include "light.h"
#include "scene.h"

RAINBOW_NAMESPACE_BEGIN

// Return true only if on intersection
bool Visibility::Occluded(const Scene & scene) const {
	return scene.Intersect(p0.SpawnToRay(p1));
}

RGBSpectrum Visibility::Tr(const Scene & scene, Sampler& sampler) const {
    Ray ray(p0.SpawnToRay(p1));
    RGBSpectrum Tr(1.);
    for(;;) {
        SurfaceInteraction inter;
        bool hitSurface = scene.IntersectP(ray, &inter);

        // Intersect real surface
        if (hitSurface && inter.primitive->material != nullptr)
            return RGBSpectrum(0.);

        // Update Tr for current ray segment
        if (ray.medium) Tr *= ray.medium->Tr(ray, sampler);

        if (!hitSurface) break;
        ray = inter.SpawnToRay(p1);
    }
    return Tr;
}

RGBSpectrum AreaLight::L(const Interaction& interaction, const Vector3f & w) const {
	return (Dot(interaction.n, w) > 0) ? Le : RGBSpectrum(0.0);
}

Float AreaLight::PdfLi(const Interaction & ref, const Vector3f & wi) const {
    Ray ray = ref.SpawnToRay(wi);
    Float tHit;
    SurfaceInteraction inter;
    if (!shape->IntersectP(ray, &tHit, &inter))
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

std::shared_ptr<AreaLight> CreateAreaLight(
    PropertyList& list, 
    const std::shared_ptr<Shape>& shape,
    const MediumInterface& mediumInterface) {
	RGBSpectrum L = list.getColor("radiance", RGBSpectrum(1.0));
    return std::make_shared<AreaLight>(mediumInterface, L, shape);
}

RAINBOW_NAMESPACE_END
