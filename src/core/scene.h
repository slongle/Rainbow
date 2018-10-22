#ifndef __SCENE_H
#define __SCENE_H

#include "object.h"
#include "primitive.h"


RAINBOW_NAMESPACE_BEGIN

class Scene{
public:
	Scene() {}
	Scene(std::shared_ptr<Aggregate> m_aggregate, std::vector<std::shared_ptr<Light>> m_lights) :
		aggregate(m_aggregate), lights(m_lights) {}

	bool Intersect(const Ray & ray, SurfaceInteraction*inter) const {
		return aggregate->Intersect(ray, inter);
	}
	bool IntersectP(const Ray & ray) const {
		return aggregate->IntersectP(ray);
	}
	
	std::shared_ptr<Aggregate> aggregate;
	std::vector<std::shared_ptr<Light>> lights;
};

RAINBOW_NAMESPACE_END

#endif // !__SCENE_H
