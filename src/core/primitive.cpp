#include "primitive.h"

RAINBOW_NAMESPACE_BEGIN

bool Primitive::Intersect(const Ray & ray, SurfaceInteraction*inter) const {
    Float tHit;
	if (!shape->Intersect(ray, &tHit, inter)) return false;
	ray.tMax = tHit;
	inter->primitive = this;
	return true;
}
bool Primitive::IntersectP(const Ray & ray) const {
	return shape->IntersectP(ray);
}

void Primitive::ComputeScatteringFunctions(MemoryArena& arena, SurfaceInteraction * intersection) const {
	if (material)
		material->ComputeScatteringFunctions(arena, intersection);
	//else Assert(material != nullptr, "No Material for Primitive");
}

bool Aggregate::Intersect(const Ray & ray, SurfaceInteraction* inter) const {
	bool hit = false;
	for (const auto& primitive : primitives) {
		if (primitive->Intersect(ray, inter)) {			
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

Bounds3f Aggregate::Bounds() const {
	Bounds3f ret;
	for (const auto& primitive : primitives) {
		ret = Union(ret, primitive->shape->WorldBounds());
	}
	return ret;
}

RAINBOW_NAMESPACE_END
