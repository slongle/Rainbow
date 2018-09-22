#include "api.h"


#include "../cameras/perspective.h"
#include "../shapes/triangle.h"

RAINBOW_NAMESPACE_BEGIN

Scene * MakeScene() {
	return new Scene();
}

Integrator * MakeIntegrator(std::string &name, PropertyList &list) {
	Integrator* integrator = nullptr;
	if (name == "path") {
		integrator = CreateSampleIntegrator(list);
	}
	return integrator;
}

Camera * MakeCamera(std::string &name, PropertyList &list) {
	Camera *camera = nullptr;
	if (name == "perspective") {
		camera = CreatePerspectiveCamera(list);
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

Shape * MakeShape(std::string &name, PropertyList &list) {
	Shape * shape = nullptr;
	if (name == "obj") {
		shape = CreateWavefrontOBJ(list);
	}
	return shape;
}

RAINBOW_NAMESPACE_END
