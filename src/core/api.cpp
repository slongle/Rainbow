#include "api.h"


#include "../cameras/perspective.h"
#include "../shapes/triangle.h"
#include "../shapes/cube.h"
#include "../integrators/whitted.h"

RAINBOW_NAMESPACE_BEGIN

Scene * MakeScene() {
	return new Scene();
}

Integrator * MakeIntegrator(std::string &name, PropertyList &list) {
	Integrator* integrator = nullptr;
	if (name == "whitted") {
		integrator = CreateWhittedIntegrator(list);
	}
	return integrator;
}

Camera * MakeCamera(std::string &name, PropertyList &list, const Film* film) {
	Camera *camera = nullptr;
	if (name == "perspective") {
		camera = CreatePerspectiveCamera(list, film);
	}
	return camera;
}

Film * MakeFilm(std::string &name, PropertyList &list) {
	Film *film = nullptr;
	if (name == "hdrfilm") {
		film = CreateFilm(list);
	}
	return film;
}

BSDF * MakeBSDF(std::string &name, PropertyList &list) {
	BSDF * bsdf = nullptr;
	if (name == "diffuse") {
		bsdf = CreateDiffuseBSDF(list);
	}
	return bsdf;
}

Shape* MakeShape(std::string &name, PropertyList &list) {
	Shape* shape = nullptr;
	if (name == "obj") {
		// TODO: Fix issue about return value's type
		shape = CreateWavefrontOBJ(list);
	}
	return shape;
}

RAINBOW_NAMESPACE_END
