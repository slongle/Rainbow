#include <memory>

#include "api.h"
#include "film.h"
#include "scene.h"
#include "material.h"

#include "../cameras/perspective.h"
#include "../shapes/triangle.h"
#include "../integrators/whitted.h"
#include "../materials/matte.h"
#include "../samplers/independent.h"

RAINBOW_NAMESPACE_BEGIN

struct RenderOptions {
	Integrator* MakeIntegrator();
	Sampler* MakeSampler();
	Scene* MakeScene();
	Camera* MakeCamera();
	Film* MakeFilm();
	Aggregate* MakeAggregate();	
	
	std::string IntegratorType;
	PropertyList IntegratorProperty;
	std::string CameraType;
	PropertyList CameraProperty;
	std::string SamplerType;
	PropertyList SamplerProperty;
	std::string FilmType;
	PropertyList FilmProperty;
	std::shared_ptr<Material> CurrentMaterial;
	std::vector<std::shared_ptr<Primitive>> primitives;
};

static std::shared_ptr<RenderOptions> renderOptions;

std::vector<std::shared_ptr<Shape>> MakeShapes(const std::string & type, PropertyList & list);
std::shared_ptr<Material> MakeMaterial(const std::string & type, PropertyList & list);

void RainbowInit() {
	renderOptions = std::make_shared<RenderOptions>();
}

void RainbowWorld() {
	std::unique_ptr<Integrator> integrator(renderOptions->MakeIntegrator());
	std::unique_ptr<Scene> scene(renderOptions->MakeScene());

	if (scene && integrator)
		integrator->Render(*scene);
}

void RainbowIntegrator(const std::string& type,const PropertyList& list) {
	renderOptions->IntegratorType = type;
	renderOptions->IntegratorProperty = list;
}

void RainbowCamera(const std::string & type, const PropertyList & list) {
	renderOptions->CameraType = type;
	renderOptions->CameraProperty = list;
}

void RainbowSampler(const std::string & type, const PropertyList & list) {
	renderOptions->SamplerType = type;
	renderOptions->SamplerProperty = list;
}

void RainbowFilm(const std::string & type, const PropertyList & list) {
	renderOptions->FilmType = type;
	renderOptions->FilmProperty = list;
}

void RainbowShape(const std::string & type, PropertyList & list) {
	std::vector<std::shared_ptr<Primitive>> prims;
	std::vector<std::shared_ptr<Shape>> shapes = MakeShapes(type, list);
	std::shared_ptr<Material> mtl = renderOptions->CurrentMaterial;
	if (shapes.empty()) return;
	for (auto s : shapes) {
		prims.push_back(std::make_shared<Primitive>(s, mtl));
	}
	renderOptions->primitives.insert(renderOptions->primitives.end(), prims.begin(), prims.end());
}

void RainbowMaterial(const std::string & type, PropertyList & list) {
	std::shared_ptr<Material> material = MakeMaterial(type, list);
	renderOptions->CurrentMaterial = material;
}

Aggregate* RenderOptions::MakeAggregate() {
	Aggregate* aggregate = new Aggregate();
	aggregate->primitives = primitives;
	primitives.clear();
	return aggregate;
}

Scene* RenderOptions::MakeScene() {
	std::shared_ptr<Aggregate> aggregate(MakeAggregate());
	Scene* scene = new Scene(aggregate);
	return scene;
}

Film* RenderOptions::MakeFilm() {
	Film* film = nullptr;
	if (FilmType == "hdrfilm") {
		film = CreateFilm(FilmProperty);
	}
	return film;
}

Camera * RenderOptions::MakeCamera() {
	std::shared_ptr<Film> film(MakeFilm());
	Assert(film != nullptr, "Unable to create film!");

	Camera *camera = nullptr;
	if (CameraType == "perspective") {
		camera = CreatePerspectiveCamera(CameraProperty, film);
	}
	return camera;
}

Sampler* RenderOptions::MakeSampler() {
	Sampler* sampler = nullptr;
	if (SamplerType == "independent") {
		sampler = CreateIndependentSampler(SamplerProperty);
	}
	return sampler;
}

Integrator* RenderOptions::MakeIntegrator() {
	std::shared_ptr<Camera> camera(MakeCamera());
	Assert(camera != nullptr, "Unable to create camera!");

	
	std::shared_ptr<Sampler> sampler(MakeSampler());
	Assert(sampler != nullptr, "Unable to create sampler!");	

	Integrator* integrator = nullptr;
	if (IntegratorType == "whitted") {
		integrator = CreateWhittedIntegrator(IntegratorProperty);
	}
	integrator->camera = camera;
	integrator->sampler = sampler;
	return integrator;
}

std::vector<std::shared_ptr<Shape>> MakeShapes(const std::string & type, PropertyList & list) {
	std::vector<std::shared_ptr<Shape>> shapes;
	if (type == "obj") {
		shapes = CreateWavefrontOBJ(list);
	}
	return shapes;
}

std::shared_ptr<Material> MakeMaterial(const std::string & type, PropertyList & list) {
	Material* material = nullptr;
	if (type == "diffuse" || type == "matte") {
		material = CreateMatteMaterial(list);
	}
	return std::shared_ptr<Material>(material);
}

RAINBOW_NAMESPACE_END
