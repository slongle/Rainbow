#ifndef RAINBOW_DIRECT_INTEGRATOR_H
#define RAINBOW_DIRECT_INTEGRATOR_H

#include"../core/integrator.h"

RAINBOW_NAMESPACE_BEGIN

class DirectLightIntegrator :public SamplerIntegrator{
public:
    DirectLightIntegrator(const int &m_maxDepth,const int & m_sampleNum) :
        maxDepth(m_maxDepth), SamplerIntegrator(m_sampleNum) {}
    RGBSpectrum Li(const Ray &ray, const Scene& scene, int depth);

    int maxDepth;
};

DirectLightIntegrator* CreateDirectLightIntegrator(PropertyList& list);

RAINBOW_NAMESPACE_END

#endif // !RAINBOW_DIRECT_INTEGRATOR_H