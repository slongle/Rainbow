#ifndef __RAINBOW_API_H
#define __RAINBOW_API_H

#include "common.h"
#include "scene.h"

RAINBOW_NAMESPACE_BEGIN

Scene* MakeScene();
Integrator* MakeIntegrator(std::string &name, PropertyList &list);
Camera* MakeCamera(std::string &name, PropertyList &list, const Film* film);
Film* MakeFilm(std::string &name, PropertyList &list);
BSDF* MakeBSDF(std::string &name, PropertyList &list);
Primitive MakeShape(std::string &name, PropertyList &list);


RAINBOW_NAMESPACE_END

#endif // !__RAINBOW_API_H
