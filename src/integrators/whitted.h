#ifndef __WHITTED_H
#define __WHITTED_H

#include "../core/integrator.h"

RAINBOW_NAMESPACE_BEGIN

/*
V[f(x)] = E[(f(x)-E[f(x)])^2]
        = E[f^2(x) + E^2[f(x)] - 2 f(x) E[f(x)]]
		= E[f^2(x)] + E^2[f(x)] - 2 E^2[f(x)]  
		= E[f^2(x)] - E^2[f(x)]
*/

class WhittedIntegrator :public Integrator {
public:
	WhittedIntegrator(const int& maxDep) :maxDep(maxDep) {}

	void Render(const Scene& scene) override;
	RGBSpectrum Li(const Ray &ray, const Scene& scene, int depth);
	RGBSpectrum SpecularReflect(const Ray&ray, const Scene& scene, int depth, SurfaceInteraction intersection);
	RGBSpectrum SpecularRefract(const Ray&ray, const Scene& scene, int depth, SurfaceInteraction intersection);

	int maxDep;
};

WhittedIntegrator* CreateWhittedIntegrator(const PropertyList &list);

RAINBOW_NAMESPACE_END

#endif // __WHITTED_H
