#include"scene.h"
#include "../shapes/triangle.h"
#include <memory>

RAINBOW_NAMESPACE_BEGIN

/*void Scene::addChild(Object * child) {
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
			Shape* & s = primitive->shape;
			//TriangleMesh* mesh = static_cast<TriangleMesh*>(primitive->shape);
			TriangleMesh* m = static_cast<TriangleMesh* >(s);
			//std::cout << s << std::endl;
			//std::cout << m << std::endl;
			for (int i = 0; i < m->TriangleNum; i++) {				
				primitives.push_back(std::make_shared<Primitive>(new Triangle(m, i)));
			}
		}
		//else primitives.push_back(std::make_shared<Primitive>(static_cast<Primitive *>(child)));
	}
}*/

RAINBOW_NAMESPACE_END
