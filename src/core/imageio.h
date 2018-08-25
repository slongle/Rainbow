#ifndef __IMAGEIO_H
#define __IMAGEIO_H

#include "common.h"
#include "spectrum.h"

#include "../../ext/lodepng/lodepng.h"

RAINBOW_NAMESPACE_BEGIN

/*
  left-upper corner is (0, 0)
  right-upper corner is (width - 1, 0)
  left-lower corner is (0, height - 1)
  right-lower corner is (width - 1, height - 1)
*/

class Image{
public:
	Image(const int &_width, const int &_height) :width(_width), height(_height) { image.resize(width*height); }

	RGBSpectrum *operator () (const int &x, const int &y) { return &image[y*width + x]; }

	void save(const std::string &filename);

	int width, height;
	std::vector<RGBSpectrum> image;
};

RAINBOW_NAMESPACE_END

#endif // !__IMAGEIO_H
