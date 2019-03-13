#define TINYOBJLOADER_IMPLEMENTATION
#include "ext/tinyobjloader/tiny_obj_loader.h"

#include "wavefront.h"

RAINBOW_NAMESPACE_BEGIN

void foo(
    const std::string&       filename,         // input filename
    int*                     vertexNum,    // the number of vertex
    int*                     triangleNum,  // the number of triangle
    std::vector<Point3f>*    vertices,     // v
    std::vector<Normal3f>*   normals,      // vn
    std::vector<Point2f>*    texcoords,    // vt
    std::vector<int>*        indices)      // 
{
    // Clear Data
    *vertexNum = 0;
    *triangleNum = 0;
    vertices->clear();
    normals->clear();
    texcoords->clear();
    indices->clear();

    //std::string filename = "F:/Document/Graphics/code/Rainbow/scenes/rainbow/meshes/rainbow.obj";
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;

    std::string warn;
    std::string err;
    bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filename.c_str());

    if (!err.empty()) 
    {
        std::cerr << err << std::endl;
    }

    if (!ret) 
    {
        std::cerr << ret << std::endl;
        return;
    }

    // Loop over shapes
    for (size_t s = 0; s < shapes.size(); s++) 
    {
        // Loop over faces (polygon)
        size_t index_offset = 0;
        for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) 
        {
            int fv = shapes[s].mesh.num_face_vertices[f];

            // Loop over vertices in the face.
            for (size_t v = 0; v < fv; v++) {
                // access to vertex
                tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
                Float vx = attrib.vertices[3 * idx.vertex_index + 0];
                Float vy = attrib.vertices[3 * idx.vertex_index + 1];
                Float vz = attrib.vertices[3 * idx.vertex_index + 2];
                Float nx = attrib.normals[3 * idx.normal_index + 0];
                Float ny = attrib.normals[3 * idx.normal_index + 1];
                Float nz = attrib.normals[3 * idx.normal_index + 2];
                Float tx = attrib.texcoords[2 * idx.texcoord_index + 0];
                Float ty = attrib.texcoords[2 * idx.texcoord_index + 1];
                
                vertices->emplace_back(vx, vy, vz);
                normals->emplace_back(nx, ny, nz);
                texcoords->emplace_back(tx, ty);
                
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

int ParseOBJVertexIndex(const std::string& s) {
    return toInteger(s.substr(0, s.find('//')));
}

int ParseOBJVertex(const std::string& s) {
    std::string ret;
    int cnt = std::count(s.begin(), s.end(), '//');
    if (cnt == 0) {
        // f v1 v2 v3

    }
    else if (cnt == 1) {
        // f v1/vt1 v2/vt2 v3/vt3
    }
    else if (cnt == 2) {
        // f v1 / vt1 / vn1 v2 / vt2 / vn2 v3 / vt3 / vn3
        // f v1//vn1 v2//vn2 v3//vn3

    }    
    return ParseOBJVertexIndex(s);
}

void ParseWavefrontOBJ(const std::string& name, int* VertexNum, int* TriangleNum,
    std::vector<Point3f>* Position, std::vector<int>* Indices, std::vector<Normal3f>* Normal) {

    auto DealFace = [&](std::string& v1, std::string& v2, std::string& v3) {
        (*TriangleNum)++;
        int id1 = ParseOBJVertex(v1) - 1;
        int id2 = ParseOBJVertex(v2) - 1;
        int id3 = ParseOBJVertex(v3) - 1;
        Indices->push_back(id1);
        Indices->push_back(id2);
        Indices->push_back(id3);

        // Vertices are stored in a counter-clockwise order by default in Wavefront .obj file
        // making explicit declaration of face normals unnecessary.
        Point3f a = (*Position)[id1];
        Point3f b = (*Position)[id2];
        Point3f c = (*Position)[id3];
        Normal3f n(Cross(b - a, c - a));
        if (n.SquareLength() == 0) n = Normal3f(1, 0, 0);
        Assert(n.SquareLength() != 0, "Length of Normal is Zero!");
        Normal->push_back(Normalize(n));
    };


    filesystem::path filename = getFileResolver()->resolve(name);
    std::ifstream is(filename.str());
    Assert(is.fail() == false, "Parsing OBJ File " + name + " fail!");
    std::vector<std::string> v;
    std::string line_str;
    while (std::getline(is, line_str)) {
        std::istringstream line(line_str);
        std::string prefix;
        line >> prefix;

        if (prefix == "v") {
            Point3f p;
            line >> p.x >> p.y >> p.z;
            (*VertexNum)++;
            Position->push_back(p);
        }
        else if (prefix == "f") {
            std::string v1, v2, v3, v4;
            line >> v1 >> v2 >> v3 >> v4;
            if (!v4.empty()) {
                DealFace(v1, v2, v3);
                DealFace(v4, v1, v3);
            }
            else
                DealFace(v1, v2, v3);                        
        }
    }
}

RAINBOW_NAMESPACE_END