#ifndef __SPHERE_H
#define __SPHERE_H

#include "../core/shape.h"

RAINBOW_NAMESPACE_BEGIN

class Sphere :public Shape {
public:	
	Sphere(const Transform *m_ObjectToWorld, const Transform *m_WorldToObject, Float m_radius) :
		Shape(m_ObjectToWorld, m_WorldToObject), radius(m_radius) {}

	Bounds3f ObjectBounds() const;

	bool Intersect(const Ray & ray, Float *tHit, SurfaceInteraction*inter) const;
	bool IntersectP(const Ray & ray) const;

	Interaction Sample(const Point3f &p, const Point2f& sample, Float* pdf)const {
		Assert(false, "No Implement!");
	}
	Float Area() const;

private:
	Float radius;
};

std::shared_ptr<Sphere> CreateSphere(const Transform* o2w, const Transform* w2o, PropertyList &list);

RAINBOW_NAMESPACE_END

#endif // !__SPHERE_H
