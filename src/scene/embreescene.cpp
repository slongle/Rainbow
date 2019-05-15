#include "embreescene.h"
#include "shapes/triangle.h"
#include <iostream>

RAINBOW_NAMESPACE_BEGIN

void SetRTCRay(RTCRayHit& query,const Ray& ray) {
    query.ray.org_x = ray.o.x;
    query.ray.org_y = ray.o.y;
    query.ray.org_z = ray.o.z;
    query.ray.dir_x = ray.d.x;
    query.ray.dir_y = ray.d.y;
    query.ray.dir_z = ray.d.z;
    query.ray.tnear = Epsilon;
    query.ray.tfar = ray.tMax;
    query.ray.time = 0.0f;
    query.hit.geomID = RTC_INVALID_GEOMETRY_ID;
    query.hit.primID = RTC_INVALID_GEOMETRY_ID;
}

EmbreeScene::EmbreeScene(
    std::vector<std::shared_ptr<TriangleMesh>>&            m_meshes,
    std::vector<std::vector<std::shared_ptr<Primitive>>>&  m_primitives,
    std::vector<std::shared_ptr<Light>>&                   m_lights) 
    : Scene(m_lights), meshes(m_meshes), primitives(m_primitives) 
{
    device = rtcNewDevice(nullptr);
    scene = rtcNewScene(device);

    rtcSetSceneFlags(scene, RTC_SCENE_FLAG_ROBUST);
    rtcSetSceneBuildQuality(scene, RTC_BUILD_QUALITY_HIGH);

    for(int i=0;i<meshes.size();i++) {
        AddTriangleMesh(meshes[i], i);
    }

    rtcCommitScene(scene);
}

EmbreeScene::~EmbreeScene() {
    rtcReleaseScene(scene);
    rtcReleaseDevice(device);
}

bool EmbreeScene::IntersectP(
    const Ray& ray, 
    SurfaceInteraction* inter) const 
{
    // create intersection context
    RTCIntersectContext context;
    rtcInitIntersectContext(&context);
    // create ray
    RTCRayHit query;
    SetRTCRay(query, ray);
    // trace ray
    rtcIntersect1(scene, &context, &query);
    // hit data filled on hit
    if (query.hit.geomID == RTC_INVALID_GEOMETRY_ID) return false;
    // hit data filled on hit

    int meshID = query.hit.geomID;
    int triangleID = query.hit.primID;

    float tHit;
    bool hit = primitives[meshID][triangleID]->shape->IntersectP(ray, &tHit, inter);
    if (!hit) return false;
    inter->primitive = primitives[meshID][triangleID].get();
    inter->shape = inter->primitive->shape.get();
    ray.tMax = tHit;


    /*float u = query.hit.u;
    float v = query.hit.v;
    float t = query.ray.tfar;
    Point3f p = ray.o + ray.d * t;
    Normal3f n(query.hit.Ng_x, query.hit.Ng_y, query.hit.Ng_z);


    inter->p = p;
    inter->n = n;
    inter->wo = -ray.d;
    inter->uv = Point2f(u, v);

    inter->primitive = primitives[meshID][triangleID].get();
    inter->shape = inter->primitive->shape.get();*/

    if (inter->primitive->mediumInterface.IsMediumTransition())
        inter->mediumInterface = inter->primitive->mediumInterface;
    else
        inter->mediumInterface = MediumInterface(ray.medium);

    return true;
}

bool EmbreeScene::Intersect(
    const Ray& ray) const 
{
    // create intersection context
    RTCIntersectContext context;
    rtcInitIntersectContext(&context);
    // create ray
    RTCRayHit query;
    SetRTCRay(query, ray);
    // trace ray
    rtcIntersect1(scene, &context, &query);
    // hit data filled on hit
    if (query.hit.geomID == RTC_INVALID_GEOMETRY_ID) return false;
    return true;
}

void EmbreeScene::AddTriangleMesh(
    const std::shared_ptr<TriangleMesh>&  mesh, 
    const int&                            id) 
{
    RTCGeometry geom = rtcNewGeometry(device, RTC_GEOMETRY_TYPE_TRIANGLE);
    rtcSetGeometryBuildQuality(geom, RTC_BUILD_QUALITY_HIGH);
    rtcSetGeometryTimeStepCount(geom, 1);

    rtcAttachGeometryByID(scene, geom, id);
    rtcReleaseGeometry(geom);

    float *vertices = reinterpret_cast<float *>(
        rtcSetNewGeometryBuffer(geom, RTC_BUFFER_TYPE_VERTEX, 0, RTC_FORMAT_FLOAT3,
            3 * sizeof(float), mesh->m_vertexNum));
    int* indices = reinterpret_cast<int *>(
        rtcSetNewGeometryBuffer(geom, RTC_BUFFER_TYPE_INDEX, 0, RTC_FORMAT_UINT3, 
            3 * sizeof(int), mesh->m_triangleNum));


    for (int i = 0; i < mesh->m_vertexNum; i++) {
        vertices[i * 3 + 0] = mesh->m_vertices[i].x;
        vertices[i * 3 + 1] = mesh->m_vertices[i].y;
        vertices[i * 3 + 2] = mesh->m_vertices[i].z;
    }

    for (int i = 0; i < mesh->m_triangleNum * 3; i++) {        
        indices[i] = mesh->m_indices[i].vertexIndex;
    }

    //rtcSetSharedGeometryBuffer(geom, RTC_BUFFER_TYPE_VERTEX, 0, RTC_FORMAT_FLOAT3,
    //    vertex, 0, sizeof(float) * 3, mesh->m_vertexNum);
    //rtcSetSharedGeometryBuffer(geom, RTC_BUFFER_TYPE_INDEX, 0, RTC_FORMAT_UINT3,    //    index, 0, sizeof(int) * 3, mesh->m_triangleNum);
    rtcCommitGeometry(geom);    
}

RAINBOW_NAMESPACE_END
