#include "whitted.h"

RAINBOW_NAMESPACE_BEGIN	

void WhittedIntegrator::Render(const Scene & scene) {
	Camera* camera = scene.camera;
	Film* film = camera->film;
	Ray ray;
	for (int y = 0; y < film->resolution.y; y++) {
		for (int x = 0; x < film->resolution.x; x++) {
			// TODO: Implement Sampler
			camera->GenerateRay(&ray, Point2f(x + 0.1f, y + 0.1f));
			if (scene.IntersectP(ray)) {
				film->SetPixel(Point2i(x, y), RGBSpectrum(1, 1, 1));
			}
			else {
				film->SetPixel(Point2i(x, y), RGBSpectrum(0, 0, 0));
			}
		}
	}
	film->SaveImage();
}

WhittedIntegrator* CreateWhittedIntegrator(const PropertyList &list) {
	return new WhittedIntegrator();
}

RAINBOW_NAMESPACE_END