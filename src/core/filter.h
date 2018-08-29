#ifndef __FILTER_H
#define __FILTER_H

#include "common.h"
#include "vector.h"

RAINBOW_NAMESPACE_BEGIN

class Filter {
public:
	Filter(const Vector2f &_radius) :
		radius(_radius), invRadius(Vector2f(1 / radius.x, 1 / radius.y)) {}
	virtual Float Evaluate(const Point2f &p) const = 0;

	const Vector2f radius, invRadius;
};

RAINBOW_NAMESPACE_END

#endif // !__FILTER_H
