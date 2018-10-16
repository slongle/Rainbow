#ifndef __INTEGRATOR_H
#define __INTEGRATOR_H

#include "scene.h"
#include "camera.h"
#include "sampler.h"
#include "interaction.h"

RAINBOW_NAMESPACE_BEGIN

class Integrator{
public:
	Integrator() {}
	virtual void Render(const Scene &scene) = 0;

	std::shared_ptr<Camera> camera;
	std::shared_ptr<Sampler> sampler;
};

//class SamplerIntegrator :public Integrator {
//public:
//	SamplerIntegrator() {}
//	void Render (const Scene &scene) override;
//};
//
//SamplerIntegrator* CreateSampleIntegrator(PropertyList &list);

RAINBOW_NAMESPACE_END

#endif // !__INTEGRATOR_H
