#include "primitive.h"

RAINBOW_NAMESPACE_BEGIN

bool Primitive::Intersect(const Ray & ray, Float *tHit, SurfaceInteraction*inter) const {
	if (!shape->Intersect(ray, tHit, inter)) return false;
	ray.tMax = *tHit;
	inter->primitive = this;
	return true;
}
bool Primitive::IntersectP(const Ray & ray) const {
	return shape->IntersectP(ray);
}

void Primitive::ComputeScatteringFunctions(SurfaceInteraction * intersection) const {
	if (material)
		material->ComputeScatteringFunctions(intersection);
	//else Assert(material != nullptr, "No Material for Primitive");
}

bool Aggregate::Intersect(const Ray & ray, SurfaceInteraction* inter) const {
	bool hit = false;
	Float tHit;
	for (const auto& primitive : primitives) {
		if (primitive->Intersect(ray, &tHit, inter)) {			
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
