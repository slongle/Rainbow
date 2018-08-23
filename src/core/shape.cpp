#include "shape.h"

RAINBOW_NAMESPACE_BEGIN

Shape::Shape(const Transform * _ObjectToWorld, const Transform * _WorldToObject) : 
	ObjectToWorld(_ObjectToWorld), WorldToObject(_WorldToObject) {}

Bounds3f Shape::WorldBounds() const {
	return (*ObjectToWorld)(ObjectBounds());
}

bool Shape::IntersectP(const Ray & ray) const {
	return Intersect(ray, nullptr, nullptr);
}

RAINBOW_NAMESPACE_END
