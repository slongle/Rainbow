#include "primitive.h"

RAINBOW_NAMESPACE_BEGIN


bool Aggregate::Intersect(const Ray & ray, Float * tHit, Interaction * inter) const {
	bool hit = false;
	for (const auto& primitive : primitives) {
		if (primitive->Intersect(ray, tHit, inter)) {
			ray.tMax = *tHit;
			hit = true;
		}
	}
	return hit;
}

bool Aggregate::IntersectP(const Ray & ray) const {
	for (const auto& primitive : primitives) {
		if (primitive->IntersectP(ray)) return true;		
	}
	return false;
}

RAINBOW_NAMESPACE_END
