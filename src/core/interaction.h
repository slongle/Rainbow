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
		
	Ray SpawnRay(const Vector3f& d) const {
		// TODO: Implement Offset P to Avoid Intersection at Oneself
		Point3f o = p;
		return Ray(o, d);
	}

	Point3f p;
	Normal3f n;
	Vector3f wo;
};

class SurfaceInteraction :public Interaction {
public:
	SurfaceInteraction() {}
	SurfaceInteraction(const Point3f &m_p, const Normal3f &m_n, const Vector3f& m_wo, const Shape * m_shape);

	void ComputeScatteringFunctions();
	
	RGBSpectrum Le(const Vector3f& w) const;

	const Shape *shape = nullptr;
	const Primitive *primitive = nullptr;
	BxDF* bxdf = nullptr;
};

RAINBOW_NAMESPACE_END

#endif // !__INTERACTION_H
