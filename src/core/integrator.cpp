#include"Integrator.h"

RAINBOW_NAMESPACE_BEGIN

void SamplerIntegrator::Render() {
}


SamplerIntegrator * CreateSampleIntegrator(PropertyList & list) {
	return new SamplerIntegrator();
}

RAINBOW_NAMESPACE_END