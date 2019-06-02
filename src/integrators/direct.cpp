#include "direct.h"

RAINBOW_NAMESPACE_BEGIN

RGBSpectrum DirectLightIntegrator::Li(MemoryArena& arena, const Ray & ray, const Scene & scene, Sampler &sampler, int depth) {
    RGBSpectrum L(0.0);
    SurfaceInteraction intersection;
    if (!scene.IntersectP(ray, &intersection)) {
        return L;
    }

    Vector3f wo = intersection.wo;
    Normal3f n = intersection.n;

    /*Direct Light*/
    L += intersection.Le(wo);

    intersection.ComputeScatteringFunctions(arena);
    if (!intersection.bsdf) {
        return L;
    }

    if (scene.lights.size() > 0) {
        L += UniformSampleOneLight(intersection, scene, sampler);
    }

    if (depth + 1 < maxDepth) {
        L += SpecularReflect(arena, ray, scene, depth, intersection);        
        //L += SpecularRefract(ray, scene, depth, intersection);
    }    

    delete(intersection.bsdf);

    return L;
}

DirectLightIntegrator * CreateDirectLightIntegrator(
    PropertyList              &list,
    std::shared_ptr<Camera>    camera,
    std::shared_ptr<Sampler>   sampler) 
{
    int maxDepth = list.getInteger("maxDepth", 3);
    int sampleNum = list.getInteger("sampleNum", 10);
    return new DirectLightIntegrator(camera, sampler, maxDepth, sampleNum);
}

RAINBOW_NAMESPACE_END

