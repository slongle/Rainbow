#ifndef __PRIMITIVE_H
#define __PRIMITIVE_H

#include "shape.h"

RAINBOW_NAMESPACE_BEGIN

class Primitive :public Object {
public:
	Primitive(std::shared_ptr<Shape> _shape, std::shared_ptr<AreaLight> _areaLight = nullptr) :
		shape(_shape), areaLight(_areaLight) {}

	bool Intersect(const Ray & ray, Float *tHit, SurfaceInteraction*inter) const;
	bool IntersectP(const Ray & ray) const;

	AreaLight* getAreaLight() const { return areaLight.get(); }
	EClassType getClassType() const override { return EPrimitive; }

	std::shared_ptr<Shape> shape;
	std::shared_ptr<AreaLight> areaLight;
};

class Aggregate{
public:
	Aggregate() {}
	virtual bool Intersect(const Ray & ray, SurfaceInteraction*inter) const;
	virtual bool IntersectP(const Ray & ray) const;

	std::vector<std::shared_ptr<Primitive>> primitives;
};

RAINBOW_NAMESPACE_END

#endif // __PRIMITIVE_H
