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

    virtual void AdaptiveProgressiveRender(const Scene &scene, const int& x, const int & y)
    {
        Assert(false, "Progressive Render No Implement!");
    }

    virtual void ProgressiveRender(const Scene &scene, const int& x, const int & y, bool reset = false)
    {
        Assert(false, "Progressive Render No Implement!");
    }

    virtual void TestRender(const Scene &scene) {
        Assert(false, "Test Render No Implement!");
    }
    virtual void AdaptiveRender(const Scene &scene) {
        Assert(false, "Adaptive Render No Implement!");
    }

	std::shared_ptr<Camera> camera;
	std::shared_ptr<Sampler> sampler;
};

class SamplerIntegrator :public Integrator {
public:
    SamplerIntegrator(const int& m_sampleNum, const int &m_delta = 1) :
        sampleNum(m_sampleNum), delta(m_delta) {}

    RGBSpectrum UniformSampleOneLight(const SurfaceInteraction& inter, const Scene& scene);
    RGBSpectrum EstimateDirectLight(const SurfaceInteraction& inter, std::shared_ptr<Light> light, const Scene& scene);

    RGBSpectrum SpecularReflect(MemoryArena& arena, const Ray&ray, const Scene& scene, int depth, SurfaceInteraction intersection);
    RGBSpectrum SpecularRefract(MemoryArena& arena, const Ray&ray, const Scene& scene, int depth, SurfaceInteraction intersection);

    void RenderTileAdaptive(const Scene &scene, Sampler& sampler, FilmTile &tile);
    void RenderTile(const Scene &scene, Sampler& sampler, FilmTile &tile);
	void Render (const Scene &scene);
    virtual RGBSpectrum Li(MemoryArena& arena, const Ray &ray, const Scene& scene, Sampler &sampler, int depth) = 0;


    void AdaptiveProgressiveRender(const Scene &scene, const int& x, const int & y);
    void ProgressiveRender(const Scene &scene, const int& x, const int & y, bool reset = false);
    void TestRender(const Scene &scene);
    void AdaptiveRender(const Scene &scene);

    int sampleNum, delta;    
};

RAINBOW_NAMESPACE_END

#endif // !__INTEGRATOR_H
