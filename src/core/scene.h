#ifndef __SCENE_H
#define __SCENE_H

#include "primitive.h"
#include "light.h"

RAINBOW_NAMESPACE_BEGIN

class Scene{
public:
    Scene() {}
    Scene(std::vector<std::shared_ptr<Light>> m_lights) :lights(m_lights) {}

    virtual bool IntersectP(const Ray& ray, SurfaceInteraction *inter) const = 0;
    virtual bool Intersect(const Ray& ray) const = 0;

    bool IntersectTr(Ray ray, Sampler& sampler,
        SurfaceInteraction *inter, RGBSpectrum *Tr) const;

    virtual std::string toString() const = 0;

    std::vector<std::shared_ptr<Light>> lights;
};

RAINBOW_NAMESPACE_END

#endif // !__SCENE_H
