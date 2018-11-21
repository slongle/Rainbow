#ifndef __INTEGRATOR_H
#define __INTEGRATOR_H

#include "scene.h"
#include "camera.h"
#include "sampler.h"
#include "light.h"

RAINBOW_NAMESPACE_BEGIN

class Integrator{
public:
	Integrator() {}
	virtual void Render(const Scene &scene) = 0;

	std::shared_ptr<Camera> camera;
	std::shared_ptr<Sampler> sampler;
};

class SamplerIntegrator :public Integrator {
public:
    SamplerIntegrator(const int& m_sampleNum) :sampleNum(m_sampleNum) {}

    RGBSpectrum UniformSampleOneLight(const SurfaceInteraction& inter, const Scene& scene);
    RGBSpectrum EstimateDirectLight(const SurfaceInteraction& inter, std::shared_ptr<Light> light, const Scene& scene);

    RGBSpectrum SpecularReflect(const Ray&ray, const Scene& scene, int depth, SurfaceInteraction intersection);
    RGBSpectrum SpecularRefract(const Ray&ray, const Scene& scene, int depth, SurfaceInteraction intersection);

	void Render (const Scene &scene);
    virtual RGBSpectrum Li(const Ray &ray, const Scene& scene, int depth) = 0;

    int sampleNum;
};

RAINBOW_NAMESPACE_END

#endif // !__INTEGRATOR_H
