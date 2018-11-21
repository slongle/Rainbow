#include "direct.h"

RAINBOW_NAMESPACE_BEGIN

RGBSpectrum DirectLightIntegrator::Li(const Ray & ray, const Scene & scene, int depth) {
    RGBSpectrum L(0.0);
    SurfaceInteraction intersection;
    if (!scene.Intersect(ray, &intersection)) {
        return L;
    }

    Vector3f wo = intersection.wo;
    Normal3f n = intersection.n;

    /*Direct Light*/
    L += intersection.Le(wo);

    intersection.ComputeScatteringFunctions();
    if (!intersection.bxdf) {
        return L;
    }

    if (scene.lights.size() > 0) {
        L += UniformSampleOneLight(intersection, scene);
    }

    if (depth + 1 < maxDepth) {
        L += SpecularReflect(ray, scene, depth, intersection);        
        //L += SpecularRefract(ray, scene, depth, intersection);
    }    

    delete(intersection.bxdf);

    return L;
}

DirectLightIntegrator * CreateDirectLightIntegrator(PropertyList & list) {
    int maxDepth = list.getInteger("maxDepth", 3);
    int sampleNum = list.getInteger("sampleNum", 10);
    return new DirectLightIntegrator(maxDepth, sampleNum);
}

RAINBOW_NAMESPACE_END
