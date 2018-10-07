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
			camera->GenerateRay(&ray, Point2f(x - film->resolution.x / 2, y - film->resolution.y / 2));
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

WhittedIntegrator* CreateWhittedIntegrator(const PropertyList &list) {
	return new WhittedIntegrator();
}

RAINBOW_NAMESPACE_END