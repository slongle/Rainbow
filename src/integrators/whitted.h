#ifndef __WHITTED_H
#define __WHITTED_H

#include "../core/integrator.h"

RAINBOW_NAMESPACE_BEGIN

class WhittedIntegrator :public SamplerIntegrator {
public:
	WhittedIntegrator(const int& maxDep) :maxDep(maxDep) {}
	
	RGBSpectrum Li(const Ray &ray, const Scene& scene, int depth);	

	int maxDep;
};

WhittedIntegrator* CreateWhittedIntegrator(PropertyList &list);

RAINBOW_NAMESPACE_END

#endif // __WHITTED_H
