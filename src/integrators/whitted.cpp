#include "whitted.h"

RAINBOW_NAMESPACE_BEGIN	

void WhittedIntegrator::Render(const Scene & scene) {
	Camera* camera = scene.camera;
	camera->film->SaveImage();
}

WhittedIntegrator* CreateWhittedIntegrator(const PropertyList &list) {
	return new WhittedIntegrator();
}

RAINBOW_NAMESPACE_END