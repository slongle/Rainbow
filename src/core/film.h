#ifndef __IMAGEIO_H
#define __IMAGEIO_H

#include "common.h"
#include "spectrum.h"
#include "filter.h"

#include "../../ext/lodepng/lodepng.h"

RAINBOW_NAMESPACE_BEGIN

/*
  left-upper corner is (0, 0)
  right-upper corner is (width - 1, 0)
  left-lower corner is (0, height - 1)
  right-lower corner is (width - 1, height - 1)
*/

class Film{
public:
	Film(const int &_width, const int &_height) :width(_width), height(_height) { film.resize(width*height); }

	RGBSpectrum *operator () (const int &x, const int &y) { return &film[y*width + x]; }

	void save(const std::string &filename) const;

private:
	int width, height;
	const std::unique_ptr<Filter> filter;
	std::vector<RGBSpectrum> film;
};

RAINBOW_NAMESPACE_END

#endif // !__IMAGEIO_H
