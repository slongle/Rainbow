#ifndef __WHITTED_H
#define __WHITTED_H

#include "../core/integrator.h"

RAINBOW_NAMESPACE_BEGIN

class WhittedIntegrator :public SamplerIntegrator {
public:
    WhittedIntegrator(
        std::shared_ptr<Camera>    camera,
        std::shared_ptr<Sampler>   sampler,
        const int& m_maxDepth,
        const int& m_sampleNum) :
        maxDepth(m_maxDepth), SamplerIntegrator(camera, sampler, m_sampleNum) {}
	
	RGBSpectrum Li(
        MemoryArena   &arena, 
        const Ray     &ray, 
        const Scene   &scene, 
        Sampler       &sampler, 
        int            depth);

	int maxDepth;
};

WhittedIntegrator* CreateWhittedIntegrator(
    PropertyList              &list,
    std::shared_ptr<Camera>    camera,
    std::shared_ptr<Sampler>   sampler);

RAINBOW_NAMESPACE_END

#endif // __WHITTED_H
