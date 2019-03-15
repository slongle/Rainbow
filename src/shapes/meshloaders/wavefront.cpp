#define TINYOBJLOADER_IMPLEMENTATION
#include "ext/tinyobjloader/tiny_obj_loader.h"

#include "src/shapes/triangle.h"

RAINBOW_NAMESPACE_BEGIN

void ParseWavefrontOBJ(
    const std::string&       name,         // input filename
    int*                     vertexNum,    // the number of vertex
    int*                     triangleNum,  // the number of triangle
    std::vector<Point3f>*    vertices,     // v
    std::vector<Normal3f>*   normals,      // vn
    std::vector<Point2f>*    texcoords,    // vt
    std::vector<Index>*      indices)      // 
{
    Assert(vertices , "Null Pointer!");
    Assert(normals  , "Null Pointer!");
    Assert(texcoords, "Null Pointer!");
    Assert(indices  , "Null Pointer!");

    // Clear Data
    *vertexNum = 0;
    *triangleNum = 0;
    vertices->clear(); 
    vertices->resize(0);
    normals->clear();
    normals->resize(0);
    texcoords->clear();
    texcoords->resize(0);
    indices->clear();
    indices->resize(0);

    //std::string filename = "F:/Document/Graphics/code/Rainbow/scenes/rainbow/meshes/rainbow.obj";
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;

    std::string warn;
    std::string err;
    filesystem::path filename = getFileResolver()->resolve(name);
    bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filename.str().c_str());

    if (!err.empty()) 
    {
        std::cerr << err << std::endl;
    }

    if (!ret) 
    {
        std::cerr << ret << std::endl;
        return;
    }

    for (int i = 0; i < attrib.vertices.size(); i += 3) {
        Float x = attrib.vertices[i];
        Float y = attrib.vertices[i + 1];
        Float z = attrib.vertices[i + 2];
        vertices->emplace_back(x, y, z);
    }
    *vertexNum = vertices->size();

    for (int i = 0; i < attrib.normals.size(); i += 3) {
        Float x = attrib.normals[i];
        Float y = attrib.normals[i + 1];
        Float z = attrib.normals[i + 2];
        normals->emplace_back(x, y, z);
    }    

    for (int i = 0; i < attrib.texcoords.size(); i += 2) {
        Float x = attrib.texcoords[i];
        Float y = attrib.texcoords[i + 1];        
        texcoords->emplace_back(x, y);
    }

    // Loop over shapes
    for (int s = 0; s < shapes.size(); s++) 
    {
        *triangleNum += shapes[s].mesh.num_face_vertices.size();
        // Loop over faces (polygon)
        int index_offset = 0;
        for (int f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) 
        {
            int fv = shapes[s].mesh.num_face_vertices[f];

            // Loop over vertices in the face.
            for (int v = 0; v < fv; v++) {
                // access to vertex
                tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
                indices->emplace_back(idx.vertex_index, idx.normal_index, idx.texcoord_index);
                //Float vx = attrib.vertices[3 * idx.vertex_index + 0];
                //Float vy = attrib.vertices[3 * idx.vertex_index + 1];
                //Float vz = attrib.vertices[3 * idx.vertex_index + 2];
                //Float nx = attrib.normals[3 * idx.normal_index + 0];
                //Float ny = attrib.normals[3 * idx.normal_index + 1];
                //Float nz = attrib.normals[3 * idx.normal_index + 2];
                //Float tx = attrib.texcoords[2 * idx.texcoord_index + 0];
                //Float ty = attrib.texcoords[2 * idx.texcoord_index + 1];                                           
                // Optional: vertex colors
                // tinyobj::real_t red = attrib.colors[3*idx.vertex_index+0];
                // tinyobj::real_t green = attrib.colors[3*idx.vertex_index+1];
                // tinyobj::real_t blue = attrib.colors[3*idx.vertex_index+2];
            }
            index_offset += fv;

            // per-face material
            shapes[s].mesh.material_ids[f];
        }
    }
}

RAINBOW_NAMESPACE_END