#include "box.h"

RAINBOW_NAMESPACE_BEGIN

Float BoxFilter::Evaluate(const Point2f & p) const {
	return 1.;
}

std::shared_ptr<BoxFilter> CreateBoxFilter(const PropertyList& list) 
{
    return std::make_shared<BoxFilter>(Vector2f(1));
}

RAINBOW_NAMESPACE_END
