#ifndef __IMAGEIO_H
#define __IMAGEIO_H

#include "common.h"

RAINBOW_NAMESPACE_BEGIN


// TODO: AOV(Arbitrary Output Variables)

//void ExportToPNG(const std::string &filename, const Float *rgb, const Point2i &resolution);
void ExportToPNG(const std::string &filename, const unsigned char *rgba, const int& width, const int& height);
void MergeRawToPNG(const std::string& inputFilename1, const std::string& inputFilename2, const std::string& outputFilename);
void MergePNGToPNG(const std::string& inputFilename1, const std::string& inputFilename2, const std::string& outputFilename);

RAINBOW_NAMESPACE_END

#endif // !__IMAGEIO_H
