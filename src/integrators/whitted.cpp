#include "whitted.h"

RAINBOW_NAMESPACE_BEGIN	

RGBSpectrum WhittedIntegrator::Li(MemoryArena& arena, const Ray & ray, const Scene & scene, Sampler &sampler, int depth) {
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

    int LightNum = 0;
	
    for (auto light : scene.lights) {
	    Vector3f wi;
	    Float pdf;
	    Visibility vis;
	    RGBSpectrum Li = light->SampleLi(intersection, sampler.Get2D(), &wi, &pdf, &vis);
	    if (Li.IsBlack() || pdf == 0) continue;		
	    RGBSpectrum f = intersection.bsdf->f(wo, wi);
	    if (!f.IsBlack() && !vis.Occluded(scene)) {			
	    	L += f * Li * AbsDot(wi, n) / pdf;
	    }
        LightNum++;	    
    }

	if (depth + 1 < maxDepth) {
		L += SpecularReflect(arena, ray, scene, depth, intersection);
        LightNum++;
		//L += SpecularRefract(ray, scene, depth, intersection);
	}

    L /= LightNum;

	delete(intersection.bsdf);	
	
	return L;
}

WhittedIntegrator* CreateWhittedIntegrator(PropertyList &list) {
    int maxDepth = list.getInteger("maxDepth", 1);
    int sampleNum = list.getInteger("sampleNum", 10);
    return new WhittedIntegrator(maxDepth, sampleNum);
}

RAINBOW_NAMESPACE_END