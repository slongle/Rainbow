#ifndef __SHAPE_H
#define __SHAPE_H

#include "transform.h"
#include "sampling.h"
#include "propertylist.h"

RAINBOW_NAMESPACE_BEGIN

class Shape{
public:
	Shape() {}
	Shape(const Transform *_ObjectToWorld, const Transform *_WorldToObject);
	virtual ~Shape() = default;

	virtual Bounds3f WorldBounds() const;
	virtual Bounds3f ObjectBounds() const = 0;

	virtual bool Intersect(const Ray & ray,Float *tHit, SurfaceInteraction*inter) const = 0;
	virtual bool IntersectP(const Ray & ray) const;

	virtual Point3f Sample(const Point2f& sample, Float* pdf)const = 0;
	virtual Float Area() const = 0;

	enum EShapeType {
		EShapeMesh,
		EShapeOthers
	};
	virtual EShapeType getShapeType() const { return EShapeOthers; }

	const Transform *ObjectToWorld, *WorldToObject;
};

RAINBOW_NAMESPACE_END

#endif // !__SHAPE_H
