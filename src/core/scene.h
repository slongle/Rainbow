#ifndef __SCENE_H
#define __SCENE_H

#include "object.h"
#include "bsdf.h"
#include "integrator.h"
#include "primitive.h"
#include "camera.h"


RAINBOW_NAMESPACE_BEGIN

class Scene :public Object{
public:
	Scene() {}

	void addChild(Object *child);

	bool Intersect(const Ray & ray, Float *tHit, Interaction *inter) const {
		return aggregate->Intersect(ray, tHit, inter);
	}
	bool IntersectP(const Ray & ray) const {
		return aggregate->IntersectP(ray);
	}
	
	EClassType getClassType() const { return EScene; }
	
	Aggregate* aggregate;
	std::vector<Primitive*> primitives;
	Camera* camera = nullptr;
	Integrator* integrator = nullptr;
};

RAINBOW_NAMESPACE_END

#endif // !__SCENE_H
