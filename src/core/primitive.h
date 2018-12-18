#ifndef __PRIMITIVE_H
#define __PRIMITIVE_H

#include "shape.h"
#include "material.h"

RAINBOW_NAMESPACE_BEGIN

class Primitive{
public:
	Primitive(std::shared_ptr<Shape> m_shape, std::shared_ptr<Material> m_material = nullptr,
		std::shared_ptr<AreaLight> m_areaLight = nullptr) :
		shape(m_shape), material(m_material), areaLight(m_areaLight) {}

	bool Intersect(const Ray & ray, Float *tHit, SurfaceInteraction*inter) const;
	bool IntersectP(const Ray & ray) const;

	void ComputeScatteringFunctions(MemoryArena& arena, SurfaceInteraction* intersection) const;

	AreaLight* getAreaLight() const { return areaLight.get(); }

	std::shared_ptr<Shape> shape;
	std::shared_ptr<Material> material;
	std::shared_ptr<AreaLight> areaLight;
};

class Aggregate{
public:
	Aggregate() {}
	virtual bool Intersect(const Ray & ray, SurfaceInteraction*inter) const;
	virtual bool IntersectP(const Ray & ray) const;

	Bounds3f Bounds() const ;

	std::vector<std::shared_ptr<Primitive>> primitives;
};

RAINBOW_NAMESPACE_END

#endif // __PRIMITIVE_H
