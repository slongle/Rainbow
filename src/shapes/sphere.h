#ifndef __SPHERE_H
#define __SPHERE_H

#include "../core/shape.h"

RAINBOW_NAMESPACE_BEGIN

class Sphere :public Shape {
public:	
	Sphere(const Transform *_ObjectToWorld, const Transform *_WorldToObject, Float _radius) :
		Shape(_ObjectToWorld, _WorldToObject), radius(_radius) {}

	Bounds3f ObjectBounds() const;

	bool Intersect(const Ray & ray, Float *tHit, SurfaceInteraction*inter) const;
	bool IntersectP(const Ray & ray) const;
	Float Area() const;

private:
	Float radius;
};

Sphere* CreateSphere(const PropertyList &list);

RAINBOW_NAMESPACE_END

#endif // !__SPHERE_H
