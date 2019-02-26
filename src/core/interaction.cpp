#include "interaction.h"
#include "light.h"
#include "primitive.h"

RAINBOW_NAMESPACE_BEGIN

Point3f OffsetRayOrigin(const Point3f &p, const Vector3f &pError,
    const Normal3f &n, const Vector3f &w) {
    return p + w * Epsilon;
    Float d = Dot(Abs(n), pError);
#ifdef __DOUBLE_TYPE
    // We have tons of precision; for now bump up the offset a bunch just
    // to be extra sure that we start on the right side of the surface
    // (In case of any bugs in the epsilons code...)
    d *= 1024.;
#endif
    Vector3f offset = d * Vector3f(n);
    if (Dot(w, n) < 0) offset = -offset;
    Point3f po = p + offset;
    // Round offset point _po_ away from _p_
    for (int i = 0; i < 3; ++i) {
        if (offset[i] > 0)
            po[i] = NextFloatUp(po[i]);
        else if (offset[i] < 0)
            po[i] = NextFloatDown(po[i]);
    }
    return po;
}

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
