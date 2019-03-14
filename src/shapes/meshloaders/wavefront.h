#ifndef __WAVEFRONT_H
#define __WAVEFRONT_H

#include "src/core/vector.h"
#include "src/shapes/triangle.h"

RAINBOW_NAMESPACE_BEGIN

void ParseWavefrontOBJ(
    const std::string&       name,         // input filename
    int*                     vertexNum,    // the number of vertex
    int*                     triangleNum,  // the number of triangle
    std::vector<Point3f>*    vertices,     // v
    std::vector<Normal3f>*   normals,      // vn
    std::vector<Point2f>*    texcoords,    // vt
    std::vector<Index>*      indices);     // 

RAINBOW_NAMESPACE_END

#endif // !__WAVEFRONT_H
