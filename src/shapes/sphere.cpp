#include "sphere.h"

RAINBOW_NAMESPACE_BEGIN

Bounds3f Sphere::ObjectBounds() const {
	return Bounds3f(Point3f(-radius, -radius, -radius), Point3f(radius, radius, radius));
}

bool Sphere::Intersect(const Ray & ray, Float * tHit, Interaction * inter) const {
	Ray r = (*WorldToObject)(ray);
	Float ox = r.o.x, oy = r.o.y, oz = r.o.z;
	Float dx = r.d.x, dy = r.d.y, dz = r.d.z;
	
	Float a = dx * dx + dy * dy + dz * dz;
	Float b = 2 * (ox*dx + oy * dy + oz * dz);
	Float c = ox * ox + oy * oy + oz * oz;
	
	Float t0, t1;
	if (!Quadratic(a, b, c, &t0, &t1)) return false;

	if (t0 > r.tMax || t1 < 0) return false;
	Float tShapeHit = t0;
	if (tShapeHit < 0) {
		tShapeHit = t1;
		if (t1 > r.tMax) return false;
	}

	Point3f pHit = r(tShapeHit);

	*inter = (*ObjectToWorld)(SurfaceInteraction(pHit, -r.d, this));

	*tHit = tShapeHit;

	return true;
}

bool Sphere::IntersectP(const Ray & ray) const {
	return false;
}

Float Sphere::Area() const {
	return 4 * M_PI * radius *radius;
}

RAINBOW_NAMESPACE_END