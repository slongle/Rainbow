#ifndef __INTEGRATOR_H
#define __INTEGRATOR_H

#include "common.h"
#include "object.h"

RAINBOW_NAMESPACE_BEGIN

class Integrator :public Object{
public:
	Integrator() {}
	virtual void Render() = 0;
};

class SamplerIntegrator :public Integrator {
public:
	SamplerIntegrator() {}
	void Render () override;

	EClassType getClassType() const override { return Object::EIntegrator; }
};

SamplerIntegrator* CreateSampleIntegrator(PropertyList &list);

RAINBOW_NAMESPACE_END

#endif // !__INTEGRATOR_H
