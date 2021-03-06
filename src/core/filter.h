#ifndef __FILTER_H
#define __FILTER_H

#include "propertylist.h"

RAINBOW_NAMESPACE_BEGIN

class Filter {
public:
    Filter(const Vector2f& radius);    
	
    virtual Float Evaluate(const Point2f &p) const = 0;
    virtual std::string toString() const = 0;

	const Vector2f m_radius, m_invRadius;
    
};

RAINBOW_NAMESPACE_END

#endif // !__FILTER_H
