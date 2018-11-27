#ifndef __WAVEFRONT_H
#define __WAVEFRONT_H

#include "../../core/vector.h"

RAINBOW_NAMESPACE_BEGIN

void ParseWavefrontOBJ(const std::string& name, int* VertexNum, int* TriangleNum,
    std::vector<Point3f>* Position, std::vector<int>* Indices, std::vector<Normal3f>* Normal);

RAINBOW_NAMESPACE_END

#endif // !__WAVEFRONT_H
