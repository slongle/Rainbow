#ifndef __FILM_H
#define __FILM_H

#include "common.h"
#include "filter.h"
#include "spectrum.h"
#include "vector.h"
#include "imageio.h"
#include "object.h"

RAINBOW_NAMESPACE_BEGIN

/*
  left-upper corner is (0, 0)
  right-upper corner is (width - 1, 0)
  left-lower corner is (0, height - 1)
  right-lower corner is (width - 1, height - 1)
*/

class Film : public Object{
public:
	Film() {}
	Film(const std::string & _filename, const Point2i & _resolution);


	void SaveImage();

	void addChild(Object *child) override;
	EClassType getClassType() const override { return EFilm; }

	const std::string filename;
	const Point2i resolution;
	std::unique_ptr<Filter> filter;

private:
	struct Pixel {
		Pixel() { rgb[0] = rgb[1] = rgb[2] = filterSum; }
		Float rgb[3];
		Float filterSum;
	};
	std::unique_ptr<Pixel[]> pixels;

	Pixel &GetPixel(const Point2i &p) const {
		Assert(0 <= p.x && p.x < resolution.x && 0 <= p.y && p.y < resolution.y, "Access Violation");
		return pixels[p.y*resolution.x + p.x];
	}

};

Film * CreateFilm(PropertyList &list);

RAINBOW_NAMESPACE_END

#endif // !__IMAGEIO_H
