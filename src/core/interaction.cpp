#include "interaction.h"
#include "light.h"
#include "primitive.h"

RAINBOW_NAMESPACE_BEGIN

Interaction::Interaction(const Point3f & _p, const Normal3f & _n) :p(_p), n(_n) {}


SurfaceInteraction::SurfaceInteraction(const Point3f & _p, const Normal3f & _n, const Shape * _shape)
	:Interaction(_p, _n), shape(_shape) {}

RGBSpectrum SurfaceInteraction::Le(const Vector3f & w) const {
	AreaLight* areaLight= primitive->getAreaLight();
	return areaLight ? areaLight->L(*this, w) : RGBSpectrum(0.0);
}

RAINBOW_NAMESPACE_END