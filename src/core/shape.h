#ifndef __SHAPE_H
#define __SHAPE_H

#include "bbox.h"
#include "common.h"
#include "interaction.h"
#include "ray.h"
#include "transform.h"

RAINBOW_NAMESPACE_BEGIN

class Shape {
public:
	Shape(const Transform *_ObjectToWorld, const Transform *_WorldToObject);
	virtual ~Shape() = default;

	virtual Bounds3f WorldBounds() const;
	virtual Bounds3f ObjectBounds() const = 0;

	virtual bool Intersect(const Ray & ray,Float *tHit, Interaction *inter) const = 0;
	virtual bool IntersectP(const Ray & ray) const;

	virtual Float Area() const = 0;

	const Transform *ObjectToWorld, *WorldToObject;
};

RAINBOW_NAMESPACE_END

#endif // !__SHAPE_H
