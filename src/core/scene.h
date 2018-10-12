#ifndef __SCENE_H
#define __SCENE_H

#include "object.h"
#include "primitive.h"


RAINBOW_NAMESPACE_BEGIN

class Scene :public Object{
public:
	Scene() {}
	Scene(std::shared_ptr<Aggregate> _aggregate) :aggregate(_aggregate) {}

	void addChild(Object *child);

	bool Intersect(const Ray & ray, SurfaceInteraction*inter) const {
		return aggregate->Intersect(ray, inter);
	}
	bool IntersectP(const Ray & ray) const {
		return aggregate->IntersectP(ray);
	}
	
	EClassType getClassType() const { return EScene; }
	
	std::shared_ptr<Aggregate> aggregate;
};

RAINBOW_NAMESPACE_END

#endif // !__SCENE_H
