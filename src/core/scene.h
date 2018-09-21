#ifndef __SCENE_H
#define __SCENE_H

#include "object.h"
#include "bsdf.h"
#include "integrator.h"
#include "shape.h"
#include "camera.h"


RAINBOW_NAMESPACE_BEGIN

class Scene :public Object{
public:
	Scene() {}

	void addChild(Object *child);

	EClassType getClassType() const { return EScene; }
	

	std::vector<Shape*> shapes;
	Camera* camera = nullptr;
	Integrator* integrator = nullptr;
};

RAINBOW_NAMESPACE_END

#endif // !__SCENE_H
