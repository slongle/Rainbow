#include"scene.h"

RAINBOW_NAMESPACE_BEGIN

void Scene::addChild(Object * child) {
	if (child->getClassType() == ECamera) {		
		cout << (camera == nullptr) << endl;
		Assert(camera == nullptr, "Scene has a camera!");
		camera = static_cast<Camera *>(child);
	}
	else if (child->getClassType() == EIntegrator) {
		Assert(integrator == nullptr, "Scene has an integrator!");
		integrator = static_cast<Integrator *>(child);
	}
	else if (child->getClassType() == EShape) {
		shapes.push_back(static_cast<Shape *>(child));
	}
}

RAINBOW_NAMESPACE_END
