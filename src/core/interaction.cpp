#include "interaction.h"

RAINBOW_NAMESPACE_BEGIN

Interaction::Interaction(const Point3f & _p, const Normal3f & _n) :p(_p), n(_n) {}


SurfaceInteraction::SurfaceInteraction(const Point3f & _p, const Normal3f & _n, const Shape * _shape)
	:Interaction(_p, _n), shape(_shape) {}

RAINBOW_NAMESPACE_END