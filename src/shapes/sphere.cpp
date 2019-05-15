#include "sphere.h"

RAINBOW_NAMESPACE_BEGIN

Bounds3f Sphere::ObjectBounds() const {
	return Bounds3f(Point3f(-radius, -radius, -radius), Point3f(radius, radius, radius));
}

bool Sphere::IntersectP(const Ray & ray, Float * tHit, SurfaceInteraction* inter) const {
	Ray r = (*WorldToObject)(ray);
	Float ox = r.o.x, oy = r.o.y, oz = r.o.z;
	Float dx = r.d.x, dy = r.d.y, dz = r.d.z;
	
	Float a = dx * dx + dy * dy + dz * dz;
	Float b = 2 * (ox * dx + oy * dy + oz * dz);
	Float c = ox * ox + oy * oy + oz * oz - radius * radius;
	
	Float t0, t1;
	if (!Quadratic(a, b, c, &t0, &t1)) return false;

	if (t0 > r.tMax || t1 < 0) return false;
	Float tShapeHit = t0;
	if (tShapeHit < 0) {
		tShapeHit = t1;
		if (t1 > r.tMax) return false;
	}

	Point3f pHit = r(tShapeHit);
    Vector3f pError = gamma(5) * Abs((Vector3f)pHit);

	*inter = (*ObjectToWorld)(SurfaceInteraction(pHit, pError, 
        static_cast<Normal3f>(pHit / radius), -r.d, this));

	*tHit = tShapeHit;

	return true;
}

bool Sphere::Intersect(const Ray & ray) const {
	Ray r = (*WorldToObject)(ray);
	Float ox = r.o.x, oy = r.o.y, oz = r.o.z;
	Float dx = r.d.x, dy = r.d.y, dz = r.d.z;

	Float a = dx * dx + dy * dy + dz * dz;
	Float b = 2 * (ox * dx + oy * dy + oz * dz);
	Float c = ox * ox + oy * oy + oz * oz - radius * radius;

	Float t0, t1;
	if (!Quadratic(a, b, c, &t0, &t1)) return false;

	if (t0 > r.tMax || t1 < 0) return false;
	Float tShapeHit = t0;
	if (tShapeHit < 0) {
		tShapeHit = t1;
		if (t1 > r.tMax) return false;
	}

	return true;
}

Interaction Sphere::Sample(const Point3f & p, const Point2f & sample, Float * pdf) const {
    Point3f sampleP = Point3f(0,0,0) + radius * UniformSampleSphere(sample);
    Interaction it;
    it.n = Normalize((*ObjectToWorld)(Normal3f(sampleP)));
    it.p = (*ObjectToWorld)(sampleP);
    *pdf = 1 / Area();
    return it;
}

Float Sphere::Area() const {
	return 4 * M_PI * radius *radius;
}

std::shared_ptr<Sphere> CreateSphere(const Transform* o2w, const Transform* w2o, PropertyList &list) {
	Float radius = list.getFloat("radius", 1.0f);
	return std::make_shared<Sphere>(o2w, w2o, radius);
}


RAINBOW_NAMESPACE_END
