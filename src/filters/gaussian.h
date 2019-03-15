#pragma once
#ifndef __RAINBOW_GAUSSIANFILTER
#define __RAINBOW_GAUSSIANFILTER

#include "src/core/filter.h"

RAINBOW_NAMESPACE_BEGIN

class GaussianFilter : public Filter{
public:
    GaussianFilter(
        const Vector2f&   radius, 
        const Float&      stddev);
    Float Evaluate(const Point2f& p) const override;
private:
    const Float m_stddev; // Standard deviation
    const Float m_alpha, m_expX, m_expY;

    Float Gaussian(const Float& d, const Float& exp) const {
        return std::max(Float(0), Float(std::exp(m_alpha * d * d) - exp));
    }
};

std::shared_ptr<GaussianFilter> CreateGaussianFilter(PropertyList& list);

RAINBOW_NAMESPACE_END

#endif // __RAINBOW_GAUSSIANFILTER