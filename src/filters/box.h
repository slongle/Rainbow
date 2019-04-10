#ifndef __BOXFILTER_H
#define __BOXFILTER_H

#include "core/filter.h"

RAINBOW_NAMESPACE_BEGIN

class BoxFilter : public Filter {
public:
	BoxFilter(const Vector2f &radius) :Filter(radius) {}
	Float Evaluate(const Point2f &p) const;

    std::string toString() const override {
        return tfm::format(
            "BoxFiltr[\n"
            "    Radius : %f * %f,\n"
            "]"
            , m_radius.x, m_radius.y
        );
    }
};

std::shared_ptr<BoxFilter> CreateBoxFilter(PropertyList& list);

RAINBOW_NAMESPACE_END

#endif // !__BOXFILTER_H
