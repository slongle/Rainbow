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

    virtual void ProgressiveRender(const Scene &scene, const int& x, const int & y, bool reset = false)
    {
        Assert(false, "No Implement!");
    }

	std::shared_ptr<Camera> camera;
	std::shared_ptr<Sampler> sampler;
};

class SamplerIntegrator :public Integrator {
public:
    SamplerIntegrator(const int& m_sampleNum) :sampleNum(m_sampleNum),arena() {}

    RGBSpectrum UniformSampleOneLight(const SurfaceInteraction& inter, const Scene& scene);
    RGBSpectrum EstimateDirectLight(const SurfaceInteraction& inter, std::shared_ptr<Light> light, const Scene& scene);

    RGBSpectrum SpecularReflect(MemoryArena& arena, const Ray&ray, const Scene& scene, int depth, SurfaceInteraction intersection);
    RGBSpectrum SpecularRefract(MemoryArena& arena, const Ray&ray, const Scene& scene, int depth, SurfaceInteraction intersection);

	void Render (const Scene &scene);
    void ProgressiveRender(const Scene &scene, const int& x, const int & y, bool reset = false);
    virtual RGBSpectrum Li(MemoryArena& arena, const Ray &ray, const Scene& scene, int depth) = 0;

    int sampleNum;
    MemoryArena arena;
};

RAINBOW_NAMESPACE_END

#endif // !__INTEGRATOR_H
