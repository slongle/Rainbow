#include "interaction.h"
#include "light.h"
#include "primitive.h"

RAINBOW_NAMESPACE_BEGIN

Interaction::Interaction(const Point3f & m_p, const Vector3f & m_pError, const Normal3f & m_n, const Vector3f& m_wo) :
	p(m_p), n(m_n), wo(m_wo) {}

SurfaceInteraction::SurfaceInteraction(const Point3f & m_p, const Vector3f & m_pError,
    const Normal3f & m_n, const Vector3f & m_wo, const Shape * m_shape) :
    Interaction(m_p, m_pError, m_n, m_wo), shape(m_shape) {
}

void SurfaceInteraction::ComputeScatteringFunctions(MemoryArena& arena) {
	if (primitive)
		primitive->ComputeScatteringFunctions(arena, this);
}

RGBSpectrum SurfaceInteraction::Le(const Vector3f & w) const {
	AreaLight* areaLight= primitive->getAreaLight();
	return areaLight ? areaLight->L(*this, w) : RGBSpectrum(0.0);
}

RAINBOW_NAMESPACE_END