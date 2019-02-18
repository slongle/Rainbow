#include "shape.h"

RAINBOW_NAMESPACE_BEGIN

Shape::Shape(const Transform * _ObjectToWorld, const Transform * _WorldToObject) : 
	ObjectToWorld(_ObjectToWorld), WorldToObject(_WorldToObject) {}

Bounds3f Shape::WorldBounds() const {
	return (*ObjectToWorld)(ObjectBounds());
}

bool Shape::Intersect(const Ray & ray) const {
	return IntersectP(ray, nullptr, nullptr);
}

Interaction Shape::Sample(const Interaction & ref, const Point2f & sample, Float * pdf) const {
	Interaction intr = Sample(ref.p, sample, pdf);
	Vector3f wi = intr.p - ref.p;
	if (wi.LengthSquare() == 0)
		*pdf = 0;
	else {
		wi = Normalize(wi);
		// Convert from area measure, as returned by the Sample() call
		// above, to solid angle measure.
		*pdf *= DistanceSquare(ref.p, intr.p) / AbsDot(-wi, intr.n);
		if (std::isinf(*pdf)) *pdf = 0.f;
	}
	return intr;
}

RAINBOW_NAMESPACE_END
