#include "interaction.h"
#include "light.h"
#include "primitive.h"

RAINBOW_NAMESPACE_BEGIN

Point3f OffsetRayOrigin(const Point3f &p, const Vector3f &pError,
    const Normal3f &n, const Vector3f &w) {
    return p + w * Epsilon;
}

Interaction::Interaction(
    const Point3f&           p,
    const Normal3f&          n,
    const Vector3f&          wo,
    const MediumInterface&   mediumInterface)
    :p(p), n(n), wo(wo), mediumInterface(mediumInterface) {}

Interaction::Interaction(
    const Point3f & m_p, const Vector3f & m_pError,
    const Normal3f & m_n, const Vector3f& m_wo,
    const MediumInterface& m_mediumInterface) :
    p(m_p), n(m_n), wo(m_wo), mediumInterface(m_mediumInterface) {}

Interaction::Interaction(
    const Point3f &m_p, const Vector3f &m_wo,
    const MediumInterface &m_mediumInterface) : 
    p(m_p), wo(m_wo), mediumInterface(m_mediumInterface) {}

Interaction::Interaction(
    const Point3f & m_p,
    const MediumInterface & m_mediumInterface) :
    p(m_p), mediumInterface(m_mediumInterface) {}

SurfaceInteraction::SurfaceInteraction(
    const Point3f &   p,
    const Point2f &   uv,
    const Vector3f&   wo,
    const Vector3f&   dpdu,
    const Vector3f&   dpdv,
    const Normal3f&   dndu,
    const Normal3f&   dndv,
    const Shape*      shape)
    : Interaction(p, Normal3f(Normalize(Cross(dpdu, dpdv))), wo, nullptr),
    uv(uv), dpdu(dpdu), dpdv(dpdv), dndu(dndu), dndv(dndv), shape(shape){}

SurfaceInteraction::SurfaceInteraction(
    const Point3f & m_p, const Vector3f & m_pError,
    const Normal3f & m_n, const Vector3f & m_wo, const Shape * m_shape) 
    :
    Interaction(m_p, m_pError, m_n, m_wo, MediumInterface()), shape(m_shape) {}

void SurfaceInteraction::ComputeScatteringFunctions(MemoryArena& arena) {
	if (primitive)
		primitive->ComputeScatteringFunctions(arena, this);
}

RGBSpectrum SurfaceInteraction::Le(const Vector3f & w) const {
	AreaLight* areaLight= primitive->getAreaLight();
	return areaLight ? areaLight->L(*this, w) : RGBSpectrum(0.0);
}

MediumInteraction::MediumInteraction(
    const Point3f & m_p, const Vector3f & m_wo,
    const Medium * m_medium, const PhaseFunction * m_phase)
    :Interaction(m_p, m_wo, MediumInterface(m_medium)), phase(m_phase) {}

RAINBOW_NAMESPACE_END
