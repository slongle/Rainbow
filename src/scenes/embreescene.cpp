#include "embreescene.h"
#include "shapes/triangle.h"
#include "shapes/sphere.h"

RAINBOW_NAMESPACE_BEGIN

void SetRTCRay(RTCRay& ret, const Ray&ray) {
    ret.org_x = ray.o.x;
    ret.org_y = ray.o.y;
    ret.org_z = ray.o.z;
    ret.dir_x = ray.d.x;
    ret.dir_y = ray.d.y;
    ret.dir_z = ray.d.z;
    ret.tnear = Epsilon;
    ret.tfar = ray.tMax;
    // TODO: set the time for ray
    //ret.time = ray.time;    
    ret.time = 0.;
}

void SetRTCRayHit(RTCRayHit& query,const Ray& ray) {
    SetRTCRay(query.ray, ray);
    query.hit.geomID = RTC_INVALID_GEOMETRY_ID;
    query.hit.primID = RTC_INVALID_GEOMETRY_ID;
}

EmbreeScene::EmbreeScene(
    std::vector<std::shared_ptr<TriangleMesh>>&            m_meshes,
    std::vector<std::vector<std::shared_ptr<Primitive>>>&  m_primitives,
    std::vector<std::shared_ptr<Light>>&                   m_lights)
    : Scene(m_lights), meshes(m_meshes), primitives(m_primitives), 
      triangleMeshNum((int)meshes.size()), triangleNum(0)
{
    device = rtcNewDevice(nullptr);
    scene = rtcNewScene(device);

    rtcSetSceneFlags(scene, RTC_SCENE_FLAG_ROBUST);
    rtcSetSceneBuildQuality(scene, RTC_BUILD_QUALITY_HIGH);

    for(int i=0;i<meshes.size();i++) {
        AddTriangleMesh(meshes[i], i);
        triangleNum += meshes[i]->m_triangleNum;
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
    SetRTCRayHit(query, ray);
    // trace ray
    rtcIntersect1(scene, &context, &query);
    // hit data filled on hit
    if (query.hit.geomID == RTC_INVALID_GEOMETRY_ID) return false;
    // hit data filled on hit

    int meshID = query.hit.geomID;
    int triangleID = query.hit.primID;


    float t = query.ray.tfar;
    ray.tMax = t;
    Point3f p = ray.o + ray.d * t;

    float u = query.hit.u;
    float v = query.hit.v;

    Vector3f dpdu, dpdv;
    rtcInterpolate1(rtcGetGeometry(scene, meshID), triangleID, u, v,
        RTC_BUFFER_TYPE_VERTEX, 0, nullptr, &dpdu.x, &dpdv.x, 3);

    inter->dpdu = dpdu;
    inter->dpdv = dpdv;
    Normal3f ng = Normal3f(Normalize(Cross(dpdu, dpdv)));

    Index* idx = &meshes[meshID]->m_indices[3 * triangleID + 0];
    if (idx[0].normalIndex == -1 || idx[1].normalIndex == -1 || idx[2].normalIndex == -1) {
        inter->n = inter->shading.n = ng;
    }
    else {        
        Normal3f n0 = meshes[meshID]->m_normals[idx[0].normalIndex];
        Normal3f n1 = meshes[meshID]->m_normals[idx[1].normalIndex];
        Normal3f n2 = meshes[meshID]->m_normals[idx[2].normalIndex];
        
        inter->shading.n = Normalize((1 - u - v) * n0 + u * n1 + v * n2);
        //inter->shading.n = Normalize(v * n0 + (1 - u - v) * n1 + u * n2);
        //inter->shading.n = Normalize(u * n0 + v * n1 + (1 - u - v) * n2);
        inter->n = FaceForward(ng, inter->shading.n);
    }

    inter->p = p;
    inter->wo = -ray.d;
    inter->uv = Point2f(u, v);
    if (std::fabs(inter->wo.Length() - 1.f) > Epsilon) {
        int a = 1;
        a++;
    }

    inter->primitive = primitives[meshID][triangleID].get();
    inter->shape = inter->primitive->shape.get();
    
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
    // create shadow ray
    RTCRay shadow;
    SetRTCRay(shadow, ray);    
    // trace ray
    rtcOccluded1(scene, &context, &shadow);
    // check hit
    //cout << shadow.tfar << " " << ray.tMax << endl;
    return shadow.tfar < 0.;
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
    int* verticesIndices = reinterpret_cast<int *>(
        rtcSetNewGeometryBuffer(geom, RTC_BUFFER_TYPE_INDEX, 0, RTC_FORMAT_UINT3, 
            3 * sizeof(int), mesh->m_triangleNum));



    for (int i = 0; i < mesh->m_vertexNum; i++) {
        vertices[i * 3 + 0] = mesh->m_vertices[i].x;
        vertices[i * 3 + 1] = mesh->m_vertices[i].y;
        vertices[i * 3 + 2] = mesh->m_vertices[i].z;
    }

    for (int i = 0; i < mesh->m_triangleNum * 3; i++) {        
        verticesIndices[i] = mesh->m_indices[i].vertexIndex;
    }

    rtcCommitGeometry(geom);    
}

RAINBOW_NAMESPACE_END
