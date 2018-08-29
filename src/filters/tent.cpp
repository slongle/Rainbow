#include "tent.h"

RAINBOW_NAMESPACE_BEGIN

Float TentFilter::Evaluate(const Point2f & p) const {
	return std::max(static_cast<Float>(0), radius.x - std::abs(p.x)) *
		   std::max(static_cast<Float>(0), radius.y - std::abs(p.y));
}

RAINBOW_NAMESPACE_END