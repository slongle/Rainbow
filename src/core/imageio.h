#ifndef __IMAGEIO_H
#define __IMAGEIO_H

#include "common.h"
#include "vector.h"

#include "lodepng/lodepng.h"

RAINBOW_NAMESPACE_BEGIN

void WriteImage(const std::string &filename, const Float *rgb, const Point2i &resolution);

RAINBOW_NAMESPACE_END

#endif // !__IMAGEIO_H
