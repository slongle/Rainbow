#include "whitted.h"

RAINBOW_NAMESPACE_BEGIN	

void WhittedIntegrator::Render(const Scene & scene) {	
	std::shared_ptr<Film> film = camera->film;
	Ray ray;
	std::cout << scene.aggregate->primitives.size() << std::endl;
	//exit(0);
	for (int y = 0; y < film->resolution.y; y++) {
		for (int x = 0; x < film->resolution.x; x++) {
			RGBSpectrum L(0.0);
			int SampleNum = 500;
			for (int i = 0; i < SampleNum; i++) {
				camera->GenerateRay(&ray,
					Point2f(x - film->resolution.x *0.5, y - film->resolution.y *0.5) + sampler->Get2D());
				L += Li(ray, scene, 0);
			}
			L /= SampleNum;
			film->SetPixel(Point2i(x, y), L);
		}
		std::cout << y << std::endl;
	}
	film->SaveImage();
}

RGBSpectrum WhittedIntegrator::Li(const Ray & ray, const Scene & scene, int depth) {
	RGBSpectrum L(0.0);
	SurfaceInteraction intersection;
	if (!scene.Intersect(ray, &intersection)) {
		return L;
	}


	//std::cout << "Hit!" << std::endl;
	Vector3f wo = intersection.wo;

	/*Direct Light*/
	L += intersection.Le(wo);
	
	intersection.ComputeScatteringFunctions();
	if (!intersection.bxdf)
		return L;

	if (depth + 1 < maxDep) {
		L += SpecularReflect(ray, scene, depth, intersection);
		//L += SpecularRefract(ray, scene, depth, intersection);
	}

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
		Ray r = intersection.SpawnRay(wi);
	
		//return f;
		
		return f * Li(r, scene, depth + 1)*AbsDot(wi, n) / pdf;
	}
	else
		return RGBSpectrum(0.f);
}

RGBSpectrum WhittedIntegrator::SpecularRefract
(const Ray & ray, const Scene & scene, int depth, SurfaceInteraction intersection) {
	return RGBSpectrum();
}

WhittedIntegrator* CreateWhittedIntegrator(const PropertyList &list) {
	return new WhittedIntegrator(5);
}

RAINBOW_NAMESPACE_END