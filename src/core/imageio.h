#ifndef __IMAGEIO_H
#define __IMAGEIO_H

#include "common.h"

RAINBOW_NAMESPACE_BEGIN

//void ExportToPNG(const std::string &filename, const Float *rgb, const Point2i &resolution);
void ExportToPNG(const std::string &filename, const unsigned char *rgba, const int& width, const int& height);

RAINBOW_NAMESPACE_END

#endif // !__IMAGEIO_H
