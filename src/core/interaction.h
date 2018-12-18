#ifndef __INTERACTION_H
#define __INTERACTION_H

#include "vector.h"
#include "spectrum.h"
#include "bsdf.h"
#include "ray.h"

RAINBOW_NAMESPACE_BEGIN

class Interaction {
public:
	Interaction() {}
	Interaction(const Point3f &m_p, const Normal3f & m_n,const Vector3f& m_wo);
	
    Ray SpawnToRay(const Vector3f& d) const {
        Point3f o = p + Point3f(n * Epsilon);
		return Ray(o, d);
	}

	Ray SpawnToRay(const Interaction& p1) const {
		Point3f o = p + Point3f(n * Epsilon);
		Vector3f d = p1.p - o;
		return Ray(o, d, 1 - Epsilon);
	}

	Point3f p;
	Normal3f n;
	Vector3f wo;
};

class SurfaceInteraction :public Interaction {
public:
	SurfaceInteraction() {}
	SurfaceInteraction(const Point3f &m_p, const Normal3f &m_n, const Vector3f& m_wo, const Shape * m_shape);

	void ComputeScatteringFunctions(MemoryArena& arena);
	
	RGBSpectrum Le(const Vector3f& w) const;

	const Shape *shape = nullptr;
	const Primitive *primitive = nullptr;
	BSDF* bsdf = nullptr;
};

RAINBOW_NAMESPACE_END

#endif // !__INTERACTION_H
