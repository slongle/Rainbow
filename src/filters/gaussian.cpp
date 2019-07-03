#include "gaussian.h"

RAINBOW_NAMESPACE_BEGIN

GaussianFilter::GaussianFilter(
    const Vector2f&   radius,
    const Float&      stddev)
  : Filter(radius), m_stddev(stddev), 
    m_alpha(-1.0f / (2.0f * m_stddev * m_stddev)),
    m_expX(std::exp(m_alpha * m_radius.x * m_radius.x)),
    m_expY(std::exp(m_alpha * m_radius.y * m_radius.y)) {}

Float GaussianFilter::Evaluate(const Point2f& p) const 
{
    return Gaussian(p.x, m_expX)*Gaussian(p.y, m_expY);
}

std::shared_ptr<GaussianFilter> CreateGaussianFilter(const PropertyList& list) 
{   
    Float radius = list.getFloat("radius", 2.0f);
    /* Standard deviation of the Gaussian */
    Float stddev = list.getFloat("stddev", 0.5f);
    return std::make_shared<GaussianFilter>(Vector2f(radius), stddev);
}

RAINBOW_NAMESPACE_END
