#include "primitive.h"

RAINBOW_NAMESPACE_BEGIN

Primitive::Primitive(
    const std::shared_ptr<Shape>& m_shape,
    const std::shared_ptr<Material>& m_material,
    const std::shared_ptr<AreaLight>& m_areaLight,
    const MediumInterface & m_mediumInterface)
    :
    shape(m_shape), material(m_material), areaLight(m_areaLight),
    mediumInterface(m_mediumInterface) {}


bool Primitive::IntersectP(const Ray & ray, SurfaceInteraction*inter) const {
    Float tHit;
	if (!shape->IntersectP(ray, &tHit, inter)) return false;
	ray.tMax = tHit;
	inter->primitive = this;

    if (mediumInterface.IsMediumTransition())
        inter->mediumInterface = mediumInterface;
    else
        inter->mediumInterface = MediumInterface(ray.medium);

	return true;
}
bool Primitive::Intersect(const Ray & ray) const {
	return shape->Intersect(ray);
}

void Primitive::ComputeScatteringFunctions(MemoryArena& arena, SurfaceInteraction * intersection) const {
	if (material)
		material->ComputeScatteringFunctions(arena, intersection);
	//else Assert(material != nullptr, "No Material for Primitive");
}

bool Aggregate::IntersectP(const Ray & ray, SurfaceInteraction* inter) const {
	bool hit = false;
	for (const auto& primitive : primitives) {
		if (primitive->IntersectP(ray, inter)) {			
			hit = true;
		}
	}
	return hit;
}

bool Aggregate::Intersect(const Ray & ray) const {
	for (const auto& primitive : primitives) {
		if (primitive->Intersect(ray)) return true;		
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
