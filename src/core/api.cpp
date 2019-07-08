#include <memory>

//#include "gui.h"

#include "api.h"
#include "film.h"
#include "scene.h"
#include "light.h"
#include "material.h"
#include "medium.h"
#include "volume.h"

#include "scenes/rainbowscene.h"
#include "scenes/embreescene.h"

#include "cameras/perspective.h"

#include "shapes/sphere.h"
#include "shapes/triangle.h"

#include "integrators/direct.h"
#include "integrators/whitted.h"
#include "integrators/path.h"
#include "integrators/volpath.h"

#include "materials/matte.h"
#include "materials/mirror.h"
#include "materials/glass.h"
#include "materials/roughconductor.h"

#include "samplers/independent.h"
#include "samplers/halton.h"

#include "accelerators/bvh.h"

#include "media/homogeneous.h"

#include "lights/point.h"
#include "lights/spot.h"
#include "lights/ies/ieslight.h"

#include "filters/gaussian.h"

#include "filters/box.h"
#include "filters/tent.h"
#include "filters/gaussian.h"

RAINBOW_NAMESPACE_BEGIN


/**
 * \brief RenderOptions support some details setting for render.
 *        The function named CreateXxx() aims to create instance.
 *        
 *        Whereas the function named RainbowXxx outside RenderOptions 
 *        struct aims to add some information from parser, which 
 *        builds up the bridge between .xml file or parser and RenderOptions.
 */
struct RenderOptions {
    RenderOptions(bool embree = true) :m_embree(embree), m_hasAreaLight(false) {}

	std::shared_ptr<Scene>        CreateScene();
    std::shared_ptr<Integrator>   CreateIntegrator();
	std::shared_ptr<Camera>       CreateCamera();
    std::shared_ptr<Sampler>      CreateSampler();
    std::shared_ptr<Film>         CreateFilm();
    std::shared_ptr<Filter>       CreateFilter();
    std::shared_ptr<Aggregate>    CreateAggregate();

    std::shared_ptr<TriangleMesh> CreateMesh(const std::string &type, const PropertyList &list);
    std::vector<std::shared_ptr<Shape>> CreateShapes(const std::string &type, const PropertyList &list);
    std::shared_ptr<Material> CreateMaterial(const std::string &type, const PropertyList&  list);
    //std::shared_ptr<Medium> CreateMedium(const std::string &type, const PropertyList &list);
    Medium* CreateMedium(const std::string &type, const PropertyList &list);


    // Decide to use embree3 or my own SAH-BVH for accelerator
    bool m_embree;

    // Render Mode (progressive, final, adaptive, eye)
    std::string m_renderMode;

	std::string m_integratorType;
	PropertyList m_integratorProperty;
	
	std::string m_cameraType;
    PropertyList m_cameraProperty;
    std::shared_ptr<Camera> m_camera;
    
    std::string m_samplerType;
	PropertyList m_samplerProperty;
    std::shared_ptr<Sampler> m_sampler;

    std::string m_filmType;
	PropertyList m_filmProperty;
    std::shared_ptr<Film> m_film;

    std::string m_filterType;
    PropertyList m_filterProperty;
    std::shared_ptr<Filter> m_filter;

	std::vector<std::shared_ptr<Primitive>> m_primitives;
    std::vector<std::vector<std::shared_ptr<Primitive>>> m_embreePrimitives;
    std::vector<std::shared_ptr<TriangleMesh>> m_embreeMeshes;

    std::vector<std::shared_ptr<Light>> m_lights;
    
    bool m_hasAreaLight;
	PropertyList m_areaLightProperty;

    // Record current BSDF and Medium
	std::shared_ptr<Material> m_currentMaterial;
    Medium *m_currentMediumInter, *m_currentMediumExter;    

    // Record the BSDF and Medium with id
    std::map<std::string, std::shared_ptr<Material>> m_materialMap;
    std::map<std::string, std::shared_ptr<Medium>> m_mediumMap;
};

static std::shared_ptr<RenderOptions> renderOptions;

void RainbowShowSettings(
    std::shared_ptr<Integrator> integrator,
    std::shared_ptr<Scene> scene)
{
    cout << integrator->toString() << endl;
    cout << integrator->m_camera->toString() << endl;
    cout << scene->toString() << endl;
}

void RainbowSceneBegin() 
{
    renderOptions = std::make_shared<RenderOptions>();
}


void RainbowSceneEnd() 
{
    std::shared_ptr<Integrator> integrator = renderOptions->CreateIntegrator();
    std::shared_ptr<Scene> scene = renderOptions->CreateScene();

    RainbowShowSettings(integrator, scene);

    Assert(scene && integrator, "No Scene or Integrator!");

    if (renderOptions->m_renderMode == "progressive") {
        //show(integrator, scene);        
        //AdaptiveShow(integrator, scene);
    }
    else if (renderOptions->m_renderMode == "final") {
        integrator->Render(*scene);        
    }
    else if (renderOptions->m_renderMode == "adaptive") {
        integrator->RenderAdaptive(*scene);
    }
    else if (renderOptions->m_renderMode == "eye") {
        integrator->RenderEyeLight(*scene);
    }
}

void RainbowRenderMode(
    const std::string& type) 
{
    renderOptions->m_renderMode = type;
}

void RainbowIntegrator(
    const std::string&    type,
    const PropertyList&   list) 
{
	renderOptions->m_integratorType = type;
	renderOptions->m_integratorProperty = list;
}

void RainbowCamera(
    const std::string&    type, 
    const PropertyList&   list) 
{
    renderOptions->m_cameraType = type;
    renderOptions->m_cameraProperty = list;

    renderOptions->m_filter = renderOptions->CreateFilter();
    renderOptions->m_film = renderOptions->CreateFilm();
    renderOptions->m_sampler = renderOptions->CreateSampler();
    renderOptions->m_camera = renderOptions->CreateCamera();
}


void RainbowFilter(
    const std::string&   type, 
    PropertyList&        list) 
{
    renderOptions->m_filterType = type;
    renderOptions->m_filterProperty = list;
}

void RainbowFilm(
    const std::string&    type, 
    const PropertyList&   list) 
{
	renderOptions->m_filmType = type;
	renderOptions->m_filmProperty = list;
}

void RainbowSampler(
    const std::string&    type, 
    const PropertyList&   list) 
{
	renderOptions->m_samplerType = type;
	renderOptions->m_samplerProperty = list;    
}

void RainbowOldShape(
    const std::string&   type, 
    PropertyList&        list) 
{	
	std::vector<std::shared_ptr<Primitive>> prims;
	std::vector<std::shared_ptr<Shape>> shapes = renderOptions->CreateShapes(type, list);
	std::vector<std::shared_ptr<Light>> lights;
	std::shared_ptr<Material> mtl = renderOptions->m_currentMaterial;
	renderOptions->m_currentMaterial = nullptr;
    MediumInterface mi = MediumInterface(renderOptions->m_currentMediumInter, 
                                         renderOptions->m_currentMediumExter);
    renderOptions->m_currentMediumInter = nullptr;
    renderOptions->m_currentMediumExter = nullptr;

	if (shapes.empty()) return;
	for (auto s : shapes) {
		std::shared_ptr<AreaLight> area;
		if (renderOptions->m_hasAreaLight) {
            area = CreateAreaLight(renderOptions->m_areaLightProperty, s, mi);
			lights.push_back(area);
		}
        prims.push_back(std::make_shared<Primitive>(s, mtl, area, mi));
	}
	renderOptions->m_primitives.insert(renderOptions->m_primitives.end(), prims.begin(), prims.end());
	if (!lights.empty()) {
		renderOptions->m_lights.insert(renderOptions->m_lights.end(), lights.begin(), lights.end());
	}

    renderOptions->m_hasAreaLight = false;
}

void RainbowShape(
    const std::string&   type,
    PropertyList&        list)
{
    Assert(type == "obj" || type == "rectangle" || type == "cube" || type == "sphere", 
        "Only support triangle mesh");

    std::vector<std::shared_ptr<Primitive>> prims;    
    std::shared_ptr<TriangleMesh> mesh = renderOptions->CreateMesh(type, list);

    std::vector<std::shared_ptr<Shape>> shapes;
    for(int i=0;i<mesh->m_triangleNum;i++) {
        shapes.push_back(std::make_shared<Triangle>(mesh, i));
    }

    std::vector<std::shared_ptr<Light>> lights;
    std::shared_ptr<Material> mtl = renderOptions->m_currentMaterial;
    renderOptions->m_currentMaterial = nullptr;
    MediumInterface mi = MediumInterface(renderOptions->m_currentMediumInter,
        renderOptions->m_currentMediumExter);
    renderOptions->m_currentMediumInter = nullptr;
    renderOptions->m_currentMediumExter = nullptr;

    if (shapes.empty()) return;
    for (auto s : shapes) {
        std::shared_ptr<AreaLight> area;
        if (renderOptions->m_hasAreaLight) {
            area = CreateAreaLight(renderOptions->m_areaLightProperty, s, mi);
            lights.push_back(area);
        }
        prims.push_back(std::make_shared<Primitive>(s, mtl, area, mi));
    }

    renderOptions->m_primitives.insert(renderOptions->m_primitives.end(), prims.begin(), prims.end());
    renderOptions->m_embreeMeshes.push_back(mesh);
    renderOptions->m_embreePrimitives.push_back(prims);

    if (!lights.empty()) {
        renderOptions->m_lights.insert(renderOptions->m_lights.end(), lights.begin(), lights.end());
    }

    renderOptions->m_hasAreaLight = false;
}

void RainbowMaterial(
    const std::string&   type, 
    PropertyList&        list) 
{
	renderOptions->m_currentMaterial = renderOptions->CreateMaterial(type, list);
}

void RainbowMedium(
    const std::string&   type, 
    const std::string&   name, 
    PropertyList&        list) 
{
     Medium *medium = renderOptions->CreateMedium(type, list);
     if (name == "exterior")
         renderOptions->m_currentMediumExter = medium;
     else if (name == "interior")
         renderOptions->m_currentMediumInter = medium;
}

void RainbowNamedMaterial(
    const std::string& id, 
    const std::string& type, 
    const PropertyList& list) 
{
    auto material = renderOptions->CreateMaterial(type, list);
    renderOptions->m_materialMap[id] = material;
    renderOptions->m_currentMaterial = material; 
}

void RainbowNamedMedium(
    const std::string& id, 
    const std::string& type, 
    const std::string& name,
    const PropertyList& list) 
{
    Medium* medium = renderOptions->CreateMedium(type, list);
    renderOptions->m_mediumMap[id] = std::shared_ptr<Medium>(medium);
    if (name == "exterior")
        renderOptions->m_currentMediumExter = medium;
    else if (name == "interior")
        renderOptions->m_currentMediumInter = medium;
}

void RainbowLight(
    const std::string&   type, 
    PropertyList&        list) 
{	
    if (type=="spot") {
        renderOptions->m_lights.push_back(CreateSpotLight(list));
    }
    else if (type == "point") {
        renderOptions->m_lights.push_back(CreatePointLight(list));
    }
    else if (type == "ies") {
        renderOptions->m_lights.push_back(CreateIESLight(list));
    }
    else if (type == "area") {
        renderOptions->m_areaLightProperty = list;
        renderOptions->m_hasAreaLight = true;
    }
}

void RainbowRef(
    const std::string &name, 
    const std::string &id)
{    
    if (renderOptions->m_materialMap.count(id) != 0) {
        renderOptions->m_currentMaterial = renderOptions->m_materialMap[id];
    } else if (renderOptions->m_mediumMap.count(id) != 0) {
        auto &medium = renderOptions->m_mediumMap[id];
        if (name == "exterior") {
            renderOptions->m_currentMediumExter = medium.get();            
        } else if (name == "interior") {
            renderOptions->m_currentMediumInter = medium.get();           
        }
    } else {
        Assert(false, "No reference with \"" + id);
    }
}

std::shared_ptr<Scene> RenderOptions::CreateScene() 
{
    Scene* scene = nullptr;    

    if (m_embree) {        
        scene = new EmbreeScene(m_embreeMeshes, m_embreePrimitives, m_lights);
    } else {        
	    std::shared_ptr<Aggregate> aggregate(CreateAggregate());    
        scene = new RainbowScene(aggregate, m_lights);
    }

    m_embreeMeshes.clear();
    m_embreePrimitives.clear();
	m_primitives.clear();
	m_lights.clear();

	return std::shared_ptr<Scene>(scene);
}

std::shared_ptr<Integrator> RenderOptions::CreateIntegrator()
{
	Assert(m_camera != nullptr, "Unable to create camera!");	
	Assert(m_sampler != nullptr, "Unable to create sampler!");	

	Integrator* integrator = nullptr;
	if (m_integratorType == "whitted") {
        integrator = CreateWhittedIntegrator(m_integratorProperty, m_camera, m_sampler);
	}
    else if (m_integratorType == "direct") {
        integrator = CreateDirectLightIntegrator(m_integratorProperty, m_camera, m_sampler);
    }
    else if (m_integratorType == "path") {
        integrator = CreatePathIntegrator(m_integratorProperty, m_camera, m_sampler);
    }
    else if (m_integratorType == "volpath") {
        integrator = CreateVolPathIntegrator(m_integratorProperty, m_camera, m_sampler);
    }
	return std::shared_ptr<Integrator>(integrator);
}

std::shared_ptr<Camera> RenderOptions::CreateCamera()
{
	Assert(m_film != nullptr, "Unable to create film!");

	Camera* camera = nullptr;	

	if (m_cameraType == "perspective") {
		camera = CreatePerspectiveCamera(m_cameraProperty, m_film);
	}

    return std::shared_ptr<Camera>(camera);
}

std::shared_ptr<Film> RenderOptions::CreateFilm()
{
	Film* film = nullptr;
    if (m_filmType == "hdrfilm" || m_filmType == "ldrfilm") {
		film = rainbow::CreateFilm(m_filmProperty, renderOptions->m_filter);
	}
	return std::shared_ptr<Film>(film);
}

std::shared_ptr<Filter> RenderOptions::CreateFilter() 
{
    std::shared_ptr<Filter> filter = nullptr;
    if (m_filterType == "gaussian") {
        filter = CreateGaussianFilter(m_filterProperty);
    }
    else if (m_filterType == "box") {
        filter = CreateBoxFilter(m_filterProperty);
    }
    else if (m_filterType == "triangle" || m_filterType == "tent") {
        filter = CreateTentFilter(m_filterProperty);
    }
    return filter;
}


std::shared_ptr<Sampler> RenderOptions::CreateSampler()
{
	Sampler* sampler = nullptr;
	if (m_samplerType == "independent") {
		sampler = CreateIndependentSampler(m_samplerProperty);
	} else if (m_samplerType == "halton") {
        sampler = CreateHaltonSampler(m_samplerProperty, m_film->resolution);
    }
	return std::shared_ptr<Sampler>(sampler);
}


std::shared_ptr<Aggregate> RenderOptions::CreateAggregate()
{    
	//Aggregate* aggregate = new Aggregate(primitives);
    Aggregate* aggregate = CreateBVHAccelerator(m_primitives);
	m_primitives.clear();
	return std::shared_ptr<Aggregate>(aggregate);
}

std::shared_ptr<TriangleMesh> RenderOptions::CreateMesh(
    const std::string  &type,
    const PropertyList &list)
{
    std::shared_ptr<TriangleMesh> mesh = nullptr;
    if (type == "obj") {
        mesh = CreateWavefrontOBJMesh(list);
    }
    else if (type == "rectangle") {
        mesh = CreateRectangleMesh(list);
    }
    else if (type == "cube") {
        mesh = CreateCubeMesh(list);
    }
    else if (type == "sphere") {
        mesh = CreateSphereTriangleMesh(list);
    }
    return mesh;
}

std::vector<std::shared_ptr<Shape>> RenderOptions::CreateShapes(
    const std::string    &type,
    const PropertyList   &list)
{
    std::vector<std::shared_ptr<Shape>> shapes;
    if (type == "obj") {
        std::vector<std::shared_ptr<Triangle>> tris(CreateWavefrontOBJ(list));
        shapes.insert(shapes.end(), tris.begin(), tris.end());
    }
    else if (type == "sphere") {
        shapes.push_back(CreateSphere(list));
    }
    else if (type == "rectangle") {
        std::vector<std::shared_ptr<Triangle>> tris(CreateRectangle(list));
        shapes.insert(shapes.end(), tris.begin(), tris.end());
    }
    else if (type == "cube") {
        std::vector<std::shared_ptr<Triangle>> tris(CreateCube(list));
        shapes.insert(shapes.end(), tris.begin(), tris.end());
    }
    return shapes;
}

std::shared_ptr<Material> RenderOptions::CreateMaterial(
    const std::string&   type,
    const PropertyList&  list)
{
    Material* material = nullptr;
    if (type == "diffuse" || type == "matte") {
        material = CreateMatteMaterial(list);
    }
    else if (type == "mirror") {
        material = CreateMirrorMaterial(list);
    }
    else if (type == "glass" || type == "dielectric") {
        material = CreateGlassMaterial(list);
    }
    else if (type == "roughconductor") {
        material = CreateRoughConductorMaterial(list);
    }
    return std::shared_ptr<Material>(material);
}

Medium* RenderOptions::CreateMedium(
    const std::string&   type,
    const PropertyList&  list)
{
    Medium *medium = nullptr;
    if (type == "homogeneous") {
        medium = CreateHomogeneousMedium(list);
    }
    return medium;
}

/*std::shared_ptr<Medium> RenderOptions::CreateMedium(
    const std::string&   type,
    const PropertyList&  list)
{
    Medium *medium = nullptr;
    if (type == "homogeneous") {
        medium = CreateHomogeneousMedium(list);
    }
    return std::shared_ptr<Medium>(medium);
}*/

RAINBOW_NAMESPACE_END
