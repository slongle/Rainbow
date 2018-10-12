#ifndef __INTEGRATOR_H
#define __INTEGRATOR_H

#include "scene.h"
#include "camera.h"
#include "interaction.h"

RAINBOW_NAMESPACE_BEGIN

class Integrator :public Object{
public:
	Integrator() {}
	virtual void Render(const Scene &scene) = 0;

	EClassType getClassType() const override { return Object::EIntegrator; }

	std::shared_ptr<Camera> camera;
	//std::shared_ptr<Sampler> sampler;
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
