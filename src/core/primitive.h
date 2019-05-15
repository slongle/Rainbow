#ifndef __PRIMITIVE_H
#define __PRIMITIVE_H

#include "sampler.h"
#include "shape.h"
#include "material.h"

RAINBOW_NAMESPACE_BEGIN

class Primitive{
public:
    Primitive(const std::shared_ptr<Shape> &m_shape,
              const std::shared_ptr<Material> &m_material,
              const std::shared_ptr<AreaLight> &m_areaLight,
              const MediumInterface &m_mediumInterface);

	bool IntersectP(const Ray & ray, SurfaceInteraction*inter) const;
	bool Intersect(const Ray & ray) const;

	void ComputeScatteringFunctions(MemoryArena& arena, SurfaceInteraction* intersection) const;

	AreaLight* getAreaLight() const { return areaLight.get(); }

	std::shared_ptr<Shape> shape;
	std::shared_ptr<Material> material;
	std::shared_ptr<AreaLight> areaLight;
    MediumInterface mediumInterface;
};

class Aggregate{
public:
	Aggregate(std::vector<std::shared_ptr<Primitive>>& m_primitives) : primitives(m_primitives) {}
	virtual bool IntersectP(const Ray & ray, SurfaceInteraction*inter) const;
	virtual bool Intersect(const Ray & ray) const;
    virtual std::string toString() const = 0;

	Bounds3f Bounds() const ;

	std::vector<std::shared_ptr<Primitive>> primitives;
};

RAINBOW_NAMESPACE_END

#endif // __PRIMITIVE_H
