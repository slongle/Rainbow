#ifndef __RAINBOW_PATHTRACER_H
#define __RAINBOW_PATHTRACER_H

#include "../core/integrator.h"

RAINBOW_NAMESPACE_BEGIN

class PathIntegrator :public SamplerIntegrator {
public:
    PathIntegrator(const int& m_maxDepth, const int& m_sampleNum) :
        maxDepth(m_maxDepth), SamplerIntegrator(m_sampleNum) {}
    RGBSpectrum Li(const Ray &ray, const Scene& scene, int depth);

    int maxDepth;
};

PathIntegrator* CreatePathIntegrator(PropertyList& list);

RAINBOW_NAMESPACE_END

#endif // !__RAINBOW_PATHTRACER_H