#ifndef __INTERACTION_H
#define __INTERACTION_H

#include "vector.h"
#include "spectrum.h"
#include "bsdf.h"
#include "ray.h"

RAINBOW_NAMESPACE_BEGIN

Point3f OffsetRayOrigin(const Point3f &p, const Vector3f &pError,
    const Normal3f &n, const Vector3f &w);

class Interaction {
public:
	Interaction() {}
	Interaction(const Point3f &m_p, const Vector3f & m_pError, const Normal3f & m_n,const Vector3f& m_wo);
	
    Ray SpawnToRay(const Vector3f& d) const {
        Point3f o = OffsetRayOrigin(p, pError, n, d);        
		return Ray(o, d);
	}

	Ray SpawnToRay(const Interaction& it) const {        
        Point3f origin = OffsetRayOrigin(p, pError, n, it.p - p);
        Point3f target = OffsetRayOrigin(it.p, it.pError, it.n, origin - it.p);
        Vector3f d = target - origin;
		return Ray(origin, d, 1 - Epsilon);
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

class MediumInteraction :public Interaction {
public:
    MediumInteraction() {};
};

RAINBOW_NAMESPACE_END

#endif // !__INTERACTION_H
