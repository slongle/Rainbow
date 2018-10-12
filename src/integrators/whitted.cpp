#include "whitted.h"

RAINBOW_NAMESPACE_BEGIN	

void WhittedIntegrator::Render(const Scene & scene) {	
	std::shared_ptr<Film> film = camera->film;
	Ray ray;
	std::cout << scene.aggregate->primitives.size() << std::endl;
	//exit(0);
	for (int y = 0; y < film->resolution.y; y++) {
		for (int x = 0; x < film->resolution.x; x++) {
			// TODO: Implement Sampler
			camera->GenerateRay(&ray, Point2f(x - film->resolution.x *0.5, y - film->resolution.y *0.5));
			if (scene.IntersectP(ray)) {
				film->SetPixel(Point2i(x, y), RGBSpectrum(0, 0, 0));
			}
			else {
				film->SetPixel(Point2i(x, y), RGBSpectrum(1, 1, 1));
			}			
			//std::cout << x << " " << y << std::endl;
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

	Vector3f wo = ray.d;

	L += intersection.Le(wo);

	if (depth + 1 < maxDep) {
		L += SpecularReflect(ray, scene, depth, intersection);
		L += SpecularRefract(ray, scene, depth, intersection);
	}

	return L;
}

RGBSpectrum WhittedIntegrator::SpecularReflect
(const Ray & ray, const Scene & scene, int depth, SurfaceInteraction intersection) {
	return RGBSpectrum();
}

RGBSpectrum WhittedIntegrator::SpecularRefract
(const Ray & ray, const Scene & scene, int depth, SurfaceInteraction intersection) {
	return RGBSpectrum();
}

WhittedIntegrator* CreateWhittedIntegrator(const PropertyList &list) {
	return new WhittedIntegrator(5);
}

RAINBOW_NAMESPACE_END