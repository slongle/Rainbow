#pragma once
#ifndef __RAINBOW_EMBREESCENE_H
#define __RAINBOW_EMBREESCENE_H

#include <embree3/rtcore.h>
#include "core/scene.h"
#include "shapes/triangle.h"

RAINBOW_NAMESPACE_BEGIN

class EmbreeScene:public Scene {
public:
    EmbreeScene(
        std::vector<std::shared_ptr<TriangleMesh>>& m_meshes,
        std::vector<std::vector<std::shared_ptr<Primitive>>>& m_primitives,
        std::vector<std::shared_ptr<Light>>& m_lights);

    ~EmbreeScene();

    bool IntersectP(const Ray& ray, SurfaceInteraction* inter) const override;
    bool Intersect(const Ray& ray) const override;
    
    std::string toString() const override {
        std::string ret = tfm::format("EmbreeScene[\n");
        ret += "]";
        return ret;
    }

private:
    void AddTriangleMesh(const std::shared_ptr<TriangleMesh>& mesh, const int& id);

    RTCDevice device;
    RTCScene scene;
    
    std::vector<std::shared_ptr<TriangleMesh>> meshes;
    std::vector<std::vector<std::shared_ptr<Primitive>>> primitives;
};

void EmbreeSceneMain();

RAINBOW_NAMESPACE_END

#endif  // __RAINBOW_EMBREESCENE_H
