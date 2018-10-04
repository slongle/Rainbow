#include"scene.h"
#include "../shapes/triangle.h"

RAINBOW_NAMESPACE_BEGIN

void Scene::addChild(Object * child) {
	if (child->getClassType() == ECamera) {		
		//cout << (camera == nullptr) << endl;
		Assert(camera == nullptr, "Scene has a camera!");
		camera = static_cast<Camera *>(child);
	}
	else if (child->getClassType() == EIntegrator) {
		Assert(integrator == nullptr, "Scene has an integrator!");
		integrator = static_cast<Integrator *>(child);
	}
	else if (child->getClassType() == EShape) {
		Shape *shape = static_cast<Shape *>(child);
		if (shape->getShapeType() == Shape::EShapeMesh) {
			std::shared_ptr<TriangleMesh> mesh(static_cast<TriangleMesh*>(shape));
			for (int i = 0; i < mesh->TriangleNum; i++) {
				shapes.push_back(new Triangle(mesh, i));
			}
		}
		else shapes.push_back(shape);
	}
}

RAINBOW_NAMESPACE_END
