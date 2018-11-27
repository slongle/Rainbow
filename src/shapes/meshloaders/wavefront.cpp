#include "wavefront.h"

RAINBOW_NAMESPACE_BEGIN

int ParseOBJVertexIndex(const std::string& s) {
    return toInteger(s.substr(0, s.find('//')));
}

int ParseOBJVertex(const std::string& s) {
    std::string ret;
    int cnt = std::count(s.begin(), s.end(), '\\');
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
            Assert(v4.empty() == true, "Quad?");
            (*TriangleNum)++;
            int id1 = ParseOBJVertex(v1) - 1;
            int id2 = ParseOBJVertex(v2) - 1;
            int id3 = ParseOBJVertex(v3) - 1;
            Indices->push_back(id1);
            Indices->push_back(id2);
            Indices->push_back(id3);

            // Vertices are stored in a counter-clockwise order by default in Wavefront .obj file
            // making explicit declaration of face normals unnecessary.
            Normal3f n(Cross((*Position)[id2] - (*Position)[id1], (*Position)[id3] - (*Position)[id1]));
            Normal->push_back(Normalize(n));
        }
    }
}

RAINBOW_NAMESPACE_END