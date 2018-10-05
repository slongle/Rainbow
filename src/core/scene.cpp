#include"scene.h"
#include "../shapes/triangle.h"
#include <memory>

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
	else if (child->getClassType() == EPrimitive) {
		Primitive* primitive = static_cast<Primitive *>(child);
		if (primitive->shape->getShapeType() == Shape::EShapeMesh) {
			primitive->shape;
			TriangleMesh* mesh = static_cast<TriangleMesh*>(primitive->shape);
			for (int i = 0; i < mesh->TriangleNum; i++) {				
				primitives.push_back(new Primitive(new Triangle(mesh, i)));
			}
		}
		else primitives.push_back(primitive);
	}
}

RAINBOW_NAMESPACE_END
