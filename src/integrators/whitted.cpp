#include "whitted.h"

RAINBOW_NAMESPACE_BEGIN	

RGBSpectrum WhittedIntegrator::Li(const Ray & ray, const Scene & scene, int depth) {
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

    int LightNum = 0;

	for (auto light : scene.lights) {
		Vector3f wi;
		Float pdf;
		Visibility vis;
		RGBSpectrum Li = light->SampleLi(intersection, sampler->Get2D(), &wi, &pdf, &vis);
		if (Li.IsBlack() || pdf == 0) continue;		
		RGBSpectrum f = intersection.bxdf->f(wo, wi);
		if (!f.IsBlack() && vis.Test(scene)) {			
			L += f * Li * AbsDot(wi, n) / pdf;
		}
        LightNum++;
	}

	if (depth + 1 < maxDep) {
		L += SpecularReflect(ray, scene, depth, intersection);
        LightNum++;
		//L += SpecularRefract(ray, scene, depth, intersection);
	}

    L /= LightNum;

	delete(intersection.bxdf);	
	
	return L;
}

RGBSpectrum WhittedIntegrator::SpecularReflect
(const Ray & ray, const Scene & scene, int depth, SurfaceInteraction intersection) {
	Vector3f wo = intersection.wo, wi;
	Float pdf;

	RGBSpectrum f = intersection.bxdf->sampleF(wo, &wi, sampler->Get2D(), &pdf);
	Normal3f n = intersection.n;	

	if (pdf > 0.f && !f.IsBlack() && Dot(n, wi) != 0.f) {
		Ray r = intersection.SpawnToRay(wi);	
		return f * Li(r, scene, depth + 1) * AbsDot(wi,n) / pdf;
	}
	else
		return RGBSpectrum(0.f);
}

RGBSpectrum WhittedIntegrator::SpecularRefract
(const Ray & ray, const Scene & scene, int depth, SurfaceInteraction intersection) {
	return RGBSpectrum();
}

WhittedIntegrator* CreateWhittedIntegrator(PropertyList &list) {
    int maxDepth = list.getInteger("maxDepth", 1);
    return new WhittedIntegrator(maxDepth);
}

RAINBOW_NAMESPACE_END