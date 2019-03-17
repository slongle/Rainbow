#ifndef __TENTFILTER
#define __TENTFILTER

#include "src/core/filter.h"

RAINBOW_NAMESPACE_BEGIN

class TentFilter :public Filter {
public:
	TentFilter(const Vector2f &radius) :Filter(radius) {}
	Float Evaluate(const Point2f &p) const;

    std::string toString() const override {
        return tfm::format(
            "TentFiltr[\n"
            "    Radius : %f * %f,\n"
            "]"
            , m_radius.x, m_radius.y
        );
    }
};

std::shared_ptr<TentFilter> CreateTentFilter(PropertyList& list);

RAINBOW_NAMESPACE_END

#endif // !__TENTFILTER
