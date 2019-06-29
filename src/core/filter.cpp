#include "filter.h"

RAINBOW_NAMESPACE_BEGIN
Filter::Filter(const Vector2f& radius)
    : m_radius(radius), m_invRadius(Vector2f(1 / radius.x, 1 / radius.y))
{
}
RAINBOW_NAMESPACE_END
