#ifndef __PRIMITIVE_H
#define __PRIMITIVE_H

#include "shape.h"

RAINBOW_NAMESPACE_BEGIN

class Primitive :public Object {
public:
	Primitive(std::shared_ptr<Shape> _shape) :shape(_shape) {}

	bool Intersect(const Ray & ray, Float *tHit, Interaction *inter) const {
		return shape->Intersect(ray, tHit, inter);
	}
	bool IntersectP(const Ray & ray) const {
		return shape->IntersectP(ray);
	}

	EClassType getClassType() const override { return EPrimitive; }

	std::shared_ptr<Shape> shape;
};

class Aggregate{
public:
	Aggregate() {}
	virtual bool Intersect(const Ray & ray, Float *tHit, Interaction *inter) const;
	virtual bool IntersectP(const Ray & ray) const;

	std::vector<std::shared_ptr<Primitive>> primitives;
};

RAINBOW_NAMESPACE_END

#endif // __PRIMITIVE_H
