#include "embreescene.h"

#include <iostream>

/*unsigned int ConvertTriangleMesh(
    RTCDevice device, 
    ISPCTriangleMesh* mesh, 
    RTCBuildQuality quality, 
    RTCScene scene_out, 
    unsigned int geomID)
{
    RTCGeometry geom = rtcNewGeometry(device, RTC_GEOMETRY_TYPE_TRIANGLE);
    //rtcSetGeometryTimeStepCount(geom, mesh->numTimeSteps);
    //rtcSetGeometryTimeRange(geom, mesh->startTime, mesh->endTime);
    rtcSetGeometryBuildQuality(geom, quality);
    //for (unsigned int t = 0; t < mesh->numTimeSteps; t++) {
    //    rtcSetSharedGeometryBuffer(geom, RTC_BUFFER_TYPE_VERTEX, t, RTC_FORMAT_FLOAT3, mesh->positions[t], 0, sizeof(Vec3fa), mesh->numVertices);
    //}
    rtcSetSharedGeometryBuffer(geom, RTC_BUFFER_TYPE_INDEX, 0, RTC_FORMAT_UINT3, 
        mesh->triangles, 0, sizeof(ISPCTriangle), mesh->numTriangles);
    rtcCommitGeometry(geom);
    rtcAttachGeometryByID(scene_out, geom, geomID);
    mesh->geom.geometry = geom;
    mesh->geom.scene = scene_out;
    mesh->geom.geomID = geomID;
    return geomID;
}*/

void EmbreeSceneMain() {
    RTCDevice device = rtcNewDevice(nullptr);
    
    RTCScene scene = rtcNewScene(device);

    

    RTCGeometry geometry1 = rtcNewGeometry(device, RTC_GEOMETRY_TYPE_TRIANGLE);
    rtcSetGeometryBuildQuality(geometry1, RTC_BUILD_QUALITY_HIGH);
    rtcCommitGeometry(geometry1);


    RTCGeometry geometry2 = rtcNewGeometry(device, RTC_GEOMETRY_TYPE_QUAD);
    rtcCommitGeometry(geometry2);


    rtcAttachGeometry(scene, geometry1);
    rtcAttachGeometry(scene, geometry2);
    
    rtcSetSceneBuildQuality(scene, RTC_BUILD_QUALITY_HIGH);
    rtcSetSceneFlags(scene, RTC_SCENE_FLAG_COMPACT);

    rtcCommitScene(scene);

    RTCIntersectContext context;
    RTCRay ray;
    RTCHit hit;
    RTCRayHit rayhit;
    rayhit.ray = ray;
    rayhit.hit = hit;
    rtcIntersect1(scene, &context, &rayhit);
    rtcOccluded1(scene, &context, &ray);    

    rtcReleaseScene(scene);

    rtcReleaseDevice(device);

    //rtcSetGeometryIntersectFunction();

    std::cout << "Yes!\n";
}
