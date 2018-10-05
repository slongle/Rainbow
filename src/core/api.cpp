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

Primitive MakeShape(std::string &name, PropertyList &list) {
	if (name == "obj") {
		// TODO: Fix issue about return value's type		
		TriangleMesh mesh = CreateWavefrontOBJ(list);
		return Primitive(&mesh);
				
	}
	return Primitive(nullptr);
	//if (shape != nullptr) return new Primitive(std::shared_ptr<Shape>(shape));
	//else return nullptr;
}

RAINBOW_NAMESPACE_END
