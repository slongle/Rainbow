#ifndef __RAINBOW_PATHTRACER_H
#define __RAINBOW_PATHTRACER_H

#include "../core/integrator.h"

RAINBOW_NAMESPACE_BEGIN

class PathIntegrator :public SamplerIntegrator {
public:
    PathIntegrator() {}
    RGBSpectrum Li(const Ray &ray, const Scene& scene, int depth);
};

PathIntegrator* CreatePathIntegrator(PropertyList& list);

RAINBOW_NAMESPACE_END

#endif // !__RAINBOW_PATHTRACER_H