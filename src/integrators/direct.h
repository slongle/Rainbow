#ifndef RAINBOW_DIRECT_INTEGRATOR_H
#define RAINBOW_DIRECT_INTEGRATOR_H

#include"../core/integrator.h"

RAINBOW_NAMESPACE_BEGIN

class DirectLightIntegrator :public SamplerIntegrator {
public:
    DirectLightIntegrator(
        std::shared_ptr<Camera>    camera,
        std::shared_ptr<Sampler>   sampler,
        const int                 &m_maxDepth,
        const int                 &m_sampleNum)
        : SamplerIntegrator(camera, sampler, m_sampleNum), maxDepth(m_maxDepth) {}

    RGBSpectrum Li(
        MemoryArena   &arena, 
        const Ray     &ray, 
        const Scene   &scene, 
        Sampler       &sampler, 
        int            depth);

    int maxDepth;
};

DirectLightIntegrator* CreateDirectLightIntegrator(
    PropertyList              &list,
    std::shared_ptr<Camera>    camera,
    std::shared_ptr<Sampler>   sampler);

RAINBOW_NAMESPACE_END

#endif // !RAINBOW_DIRECT_INTEGRATOR_H