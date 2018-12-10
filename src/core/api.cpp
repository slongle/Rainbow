#include <memory>

#include "gui.h"

#include "api.h"
#include "film.h"
#include "scene.h"
#include "light.h"
#include "material.h"

#include "../cameras/perspective.h"

#include "../shapes/sphere.h"
#include "../shapes/triangle.h"

#include "../integrators/direct.h"
#include "../integrators/whitted.h"
#include "../integrators/path.h"

#include "../materials/matte.h"
#include "../samplers/independent.h"

RAINBOW_NAMESPACE_BEGIN

class TransformPool {
public:
	TransformPool() {}

	Transform* insert(const Transform& t) {
		transforms.push_back(new Transform(t));
		return transforms.back();
	}

private:
	std::vector<Transform*> transforms;
};

struct RenderOptions {
	Integrator* MakeIntegrator();
	Sampler* MakeSampler();
	Scene* MakeScene();
	Camera* MakeCamera();
	Film* MakeFilm();
	Aggregate* MakeAggregate();	
	
	Transform CurrentTransform = Transform();

	std::string IntegratorType;
	PropertyList IntegratorProperty;
	std::string CameraType;
	PropertyList CameraProperty;
	std::string SamplerType;
	PropertyList SamplerProperty;
	std::string FilmType;
	PropertyList FilmProperty;
	std::string LightType;
	PropertyList LightProperty;
	std::shared_ptr<Material> CurrentMaterial;
	std::vector<std::shared_ptr<Primitive>> primitives;
	std::vector<std::shared_ptr<Light>> lights;
};

static std::shared_ptr<RenderOptions> renderOptions;
static std::shared_ptr<TransformPool> transformPool;

std::vector<std::shared_ptr<Shape>> MakeShapes(const std::string & type, 
	const Transform* o2w, const Transform* w2o, PropertyList & list);
std::shared_ptr<Material> MakeMaterial(const std::string & type, PropertyList & list);
std::shared_ptr<AreaLight> MakeAreaLight(PropertyList & list, const std::shared_ptr<Shape> shape);

void RainbowInit() {
	renderOptions = std::make_shared<RenderOptions>();
	transformPool = std::make_shared<TransformPool>();
}

void RainbowWorld() {
	//std::shared_ptr<Integrator> integrator(renderOptions->MakeIntegrator());
	//std::shared_ptr<Scene> scene(renderOptions->MakeScene());

    Integrator* integrator = renderOptions->MakeIntegrator();
    Scene* scene = renderOptions->MakeScene();

    Assert(scene && integrator, "No Scene or Integrator!");

    show(integrator, scene);

    //integrator->Render(*scene);
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
	Transform* ObjectToWorld = transformPool->insert(renderOptions->CurrentTransform);
	Transform* WorldToObject = transformPool->insert(Inverse(renderOptions->CurrentTransform));

	std::vector<std::shared_ptr<Primitive>> prims;
	std::vector<std::shared_ptr<Shape>> shapes = MakeShapes(type, ObjectToWorld, WorldToObject, list);
	std::vector<std::shared_ptr<Light>> lights;
	std::shared_ptr<Material> mtl = renderOptions->CurrentMaterial;
	if (shapes.empty()) return;
	for (auto s : shapes) {
		std::shared_ptr<AreaLight> area;
		if (!renderOptions->LightType.empty()) {
			area = MakeAreaLight(renderOptions->LightProperty, s);
			lights.push_back(area);
		}
		prims.push_back(std::make_shared<Primitive>(s, mtl, area));
	}
	renderOptions->primitives.insert(renderOptions->primitives.end(), prims.begin(), prims.end());
	renderOptions->CurrentMaterial = nullptr;
	if (!lights.empty()) {
		renderOptions->lights.insert(renderOptions->lights.end(), lights.begin(), lights.end());
		renderOptions->LightType = "";
	}
	renderOptions->CurrentTransform = Transform();
}

void RainbowMaterial(const std::string & type, PropertyList & list) {
	std::shared_ptr<Material> material = MakeMaterial(type, list);
	renderOptions->CurrentMaterial = material;
}

void RainbowLight(const std::string & type, PropertyList & list) {
	renderOptions->LightType = type;
	renderOptions->LightProperty = list;
}

void InitialTransform() {
	renderOptions->CurrentTransform = Transform();
}

void RainbowTransform(const Transform & ObjectToWorld) {
	renderOptions->CurrentTransform = ObjectToWorld;
}

Aggregate* RenderOptions::MakeAggregate() {
	Aggregate* aggregate = new Aggregate();
	aggregate->primitives = primitives;
	primitives.clear();
	return aggregate;
}

Scene* RenderOptions::MakeScene() {
	std::shared_ptr<Aggregate> aggregate(MakeAggregate());
	Scene* scene = new Scene(aggregate, lights);

	primitives.clear();
	lights.clear();

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

	Transform CameraToWorld = CameraProperty.getTransform("toWorld",Transform());

	if (CameraType == "perspective") {
		camera = CreatePerspectiveCamera(CameraToWorld, CameraProperty, film);
	}
	return camera;

	renderOptions->CurrentTransform = Transform();
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
    else if (IntegratorType == "direct") {
        integrator = CreateDirectLightIntegrator(IntegratorProperty);
    }
    else if (IntegratorType == "path") {
        integrator = CreatePathIntegrator(IntegratorProperty);
    }
	integrator->camera = camera;
	integrator->sampler = sampler;
	return integrator;
}

std::vector<std::shared_ptr<Shape>> MakeShapes(const std::string & type, 
	const Transform* o2w, const Transform* w2o, PropertyList & list) {
	std::vector<std::shared_ptr<Shape>> shapes;
	if (type == "obj") {
		shapes = CreateWavefrontOBJ(o2w, w2o, list);
	}
	else if (type == "sphere") {
		shapes.push_back(CreateSphere(o2w, w2o, list));
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

std::shared_ptr<AreaLight> MakeAreaLight(PropertyList & list, const std::shared_ptr<Shape> shape) {
	std::shared_ptr<AreaLight> area;
	area = CreateAreaLight(list, shape);
	return area;
}

RAINBOW_NAMESPACE_END
