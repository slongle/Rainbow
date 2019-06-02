#ifndef __RAINBOW_PATHTRACER_H
#define __RAINBOW_PATHTRACER_H

#include "core/integrator.h"

RAINBOW_NAMESPACE_BEGIN

class PathIntegrator :public SamplerIntegrator {
public:
    PathIntegrator(
        std::shared_ptr<Camera>    camera,
        std::shared_ptr<Sampler>   sampler,
        const int                 &m_maxDepth,
        const int                 &m_sampleNum,
        const int                 &m_delta) :
        SamplerIntegrator(camera, sampler, m_sampleNum, m_delta), 
        maxDepth(m_maxDepth) {}

    RGBSpectrum Li(
        MemoryArena &arena, 
        const Ray &ray, 
        const Scene &scene, 
        Sampler &sampler, 
        int depth) override;

    std::string toString() const 
    {
        return tfm::format(
            "Path Integrator[\n"
            "    MaxDepth : %d,\n"
            "]", 
            maxDepth
        );
    }

    const int maxDepth;
};

PathIntegrator* CreatePathIntegrator(
    PropertyList              &list,
    std::shared_ptr<Camera>    camera,
    std::shared_ptr<Sampler>   sampler);

RAINBOW_NAMESPACE_END

#endif // !__RAINBOW_PATHTRACER_H