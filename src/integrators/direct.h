#ifndef RAINBOW_DIRECT_INTEGRATOR_H
#define RAINBOW_DIRECT_INTEGRATOR_H

#include"../core/integrator.h"

RAINBOW_NAMESPACE_BEGIN

class DirectLightIntegrator :public SamplerIntegrator{
public:
    DirectLightIntegrator(const int &m_maxDep) :maxDep(m_maxDep) {}
    RGBSpectrum Li(const Ray &ray, const Scene& scene, int depth);

    int maxDep;
};

DirectLightIntegrator* CreateDirectLightIntegrator(PropertyList& list);

RAINBOW_NAMESPACE_END

#endif // !RAINBOW_DIRECT_INTEGRATOR_H