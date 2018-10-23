#ifndef __CUBE_H
#define __CUBE_H

#include "../core/shape.h"

RAINBOW_NAMESPACE_BEGIN

class Cube :public Shape {
public:
	Cube(const Transform *_ObjectToWorld, const Transform *_WorldToObject);

	Bounds3f ObjectBounds() const override;

	bool Intersect(const Ray & ray, Float *tHit, SurfaceInteraction*inter) const override;
	bool IntersectP(const Ray & ray) const override;

	Point3f Sample(const Point2f& sample, Float* pdf)const {
		Assert(false, "No Implement!");
	}
	Float Area() const override; 
private:
	Bounds3f bound;
};

Cube* CreateCube(const PropertyList &list);

RAINBOW_NAMESPACE_END

#endif // __CUBE_H
