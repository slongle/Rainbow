#ifndef __INTERACTION_H
#define __INTERACTION_H

#include "common.h"
#include "vector.h"

RAINBOW_NAMESPACE_BEGIN

class Interaction {
public:
	Interaction() {}
	Interaction(const Point3f &_p, const Vector3f & _n);
	
	Point3f p;
	Normal3f n;
};

class SurfaceInteraction :public Interaction {
public:
	SurfaceInteraction() {}
	SurfaceInteraction(const Point3f &_p, const Vector3f & _n, const Shape * _shape);
	
	const Shape *shape = nullptr;
};

RAINBOW_NAMESPACE_END

#endif // !__INTERACTION_H
