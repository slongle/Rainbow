#ifndef __INTERACTION_H
#define __INTERACTION_H

#include "vector.h"
#include "spectrum.h"
#include "bsdf.h"
#include "ray.h"

RAINBOW_NAMESPACE_BEGIN

inline Point3f OffsetRayOrigin(const Point3f &p, const Vector3f &pError,
    const Normal3f &n, const Vector3f &w) {
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

class Interaction {
public:
	Interaction() {}
	Interaction(const Point3f &m_p, const Vector3f & m_pError, const Normal3f & m_n,const Vector3f& m_wo);
	
    Ray SpawnToRay(const Vector3f& d) const {
        Point3f o = OffsetRayOrigin(p, pError, n, d);
		return Ray(o, d);
	}

	Ray SpawnToRay(const Interaction& it) const {
        Point3f orgin = OffsetRayOrigin(p, pError, n, it.p - p);
        Point3f target = OffsetRayOrigin(it.p, it.pError, it.n, p - it.p);
        Vector3f d = target - orgin;
		return Ray(orgin, d, 1 - Epsilon);
	}

	Point3f p;
    Vector3f pError;
	Normal3f n;
	Vector3f wo;
};

class SurfaceInteraction :public Interaction {
public:
	SurfaceInteraction() {}
	SurfaceInteraction(const Point3f &m_p, const Vector3f & m_pError, 
        const Normal3f &m_n, const Vector3f& m_wo, const Shape * m_shape);

	void ComputeScatteringFunctions(MemoryArena& arena);
	
	RGBSpectrum Le(const Vector3f& w) const;

	const Shape *shape = nullptr;
	const Primitive *primitive = nullptr;
	BSDF* bsdf = nullptr;
};

RAINBOW_NAMESPACE_END

#endif // !__INTERACTION_H
