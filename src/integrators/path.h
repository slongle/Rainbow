#ifndef __RAINBOW_PATHTRACER_H
#define __RAINBOW_PATHTRACER_H

#include "../core/integrator.h"

RAINBOW_NAMESPACE_BEGIN

class PathIntegrator :public SamplerIntegrator {
public:
    PathIntegrator(const int& m_maxDepth, const int& m_sampleNum,const int &m_delta) :
        maxDepth(m_maxDepth), SamplerIntegrator(m_sampleNum, m_delta) {}
    RGBSpectrum Li(MemoryArena& arena, const Ray &ray, const Scene& scene, int depth);

    int maxDepth;
};

PathIntegrator* CreatePathIntegrator(PropertyList& list);

RAINBOW_NAMESPACE_END

#endif // !__RAINBOW_PATHTRACER_H