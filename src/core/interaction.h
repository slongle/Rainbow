#ifndef __INTERACTION_H
#define __INTERACTION_H

#include "vector.h"
#include "spectrum.h"

RAINBOW_NAMESPACE_BEGIN

class Interaction {
public:
	Interaction() {}
	Interaction(const Point3f &_p, const Normal3f & _n);
	
	Point3f p;
	Normal3f n;
};

class SurfaceInteraction :public Interaction {
public:
	SurfaceInteraction() {}
	SurfaceInteraction(const Point3f &_p, const Normal3f & _n, const Shape * _shape);
	
	RGBSpectrum Le(const Vector3f& w) const;

	const Shape *shape = nullptr;
	const Primitive *primitive = nullptr;
};

RAINBOW_NAMESPACE_END

#endif // !__INTERACTION_H
