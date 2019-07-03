#pragma once
#ifndef __RAINBOW_GAUSSIANFILTER
#define __RAINBOW_GAUSSIANFILTER

#include "core/filter.h"

RAINBOW_NAMESPACE_BEGIN

class GaussianFilter : public Filter{
public:
    GaussianFilter(
        const Vector2f&   radius, 
        const Float&      stddev);
    Float Evaluate(const Point2f& p) const override;
    std::string toString() const override {
        return tfm::format(
            "GaussianFilter[\n"
            "    radius : %f * %f,\n"
            "    stddev : %f,\n"
            "]"
            , m_radius.x, m_radius.y
            , m_stddev
        );
    }
private:
    const Float m_stddev; // Standard deviation
    const Float m_alpha, m_expX, m_expY;

    Float Gaussian(const Float& d, const Float& exp) const {
        return std::max(Float(0), Float(std::exp(m_alpha * d * d) - exp));
    }
};

std::shared_ptr<GaussianFilter> CreateGaussianFilter(const PropertyList& list);

RAINBOW_NAMESPACE_END

#endif // __RAINBOW_GAUSSIANFILTER