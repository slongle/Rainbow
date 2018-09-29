#ifndef __CUBE_H
#define __CUBE_H

#include "../core/shape.h"

RAINBOW_NAMESPACE_BEGIN

class Cube :public Shape {
public:
	Cube(const Transform *_ObjectToWorld, const Transform *_WorldToObject) :
		Shape(_ObjectToWorld, _WorldToObject) {
		bound = (*ObjectToWorld)(Bounds3f(Point3f(-1, -1, -1), Point3f(1, 1, 1)));
	}

	Bounds3f ObjectBounds() const override;

	bool Intersect(const Ray & ray, Float *tHit, Interaction *inter) const override;
	bool IntersectP(const Ray & ray) const override;

	Float Area() const override; 

	Bounds3f bound;
};

Cube* CreateCube(const PropertyList &list);

RAINBOW_NAMESPACE_END

#endif // __CUBE_H
