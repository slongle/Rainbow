#ifndef __RAINBOWSCENE_H
#define __RAINBOWSCENE_H

#include "core/scene.h"

RAINBOW_NAMESPACE_BEGIN

class RainbowScene :public Scene {
public:
    RainbowScene(std::shared_ptr<Aggregate> m_aggregate, std::vector<std::shared_ptr<Light>> m_lights) :
        aggregate(m_aggregate), Scene(m_lights) {}

    bool IntersectP(const Ray& ray, SurfaceInteraction *inter) const override {
        return aggregate->IntersectP(ray, inter);
    }
    bool Intersect(const Ray& ray) const override {
        return aggregate->Intersect(ray);
    }

    std::string toString() const
    {
        std::string ret = tfm::format("RainbowScene[\n");
        ret += indent(aggregate->toString(), 4) + "\n";
        for (auto light : lights) {
            ret += indent(light->toString(), 8) + "\n";
        }
        ret += "]";
        return ret;
    }

    std::shared_ptr<Aggregate> aggregate;    
};

RAINBOW_NAMESPACE_END

#endif // !__RAINBOWSCENE_H
