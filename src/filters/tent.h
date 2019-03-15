#ifndef __TENTFILTER
#define __TENTFILTER

#include "src/core/filter.h"

RAINBOW_NAMESPACE_BEGIN

class TentFilter :public Filter {
public:
	TentFilter(const Vector2f &radius) :Filter(radius) {}
	Float Evaluate(const Point2f &p) const;
};

RAINBOW_NAMESPACE_END

#endif // !__TENTFILTER
