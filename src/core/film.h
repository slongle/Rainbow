#ifndef __FILM_H
#define __FILM_H

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

class Film{
public:	
	Film(const std::string & _filename, const Point2i & _resolution);

    void AddPixel(const Point2i& p, const RGBSpectrum& L, const int &num = 1) const;
	void SetPixel(const Point2i& p, const RGBSpectrum& L) const;
    RGBSpectrum RetPixel(const Point2i & p) const;
    void SaveImage() const;
    void SaveImage(const std::string &name) const;
    void ExportToUnsignedCharPointer(unsigned char* data) const;
    void UpdateToUnsignedCharPointer(unsigned char* data, const int &x, const int &y) const;

	const std::string filename;
	const Point2i resolution;
	const Float aspect;
	std::unique_ptr<Filter> filter;

private:
	struct Pixel {
        Pixel() { rgb[0] = rgb[1] = rgb[2] = sampleNum = 0; }
		Float rgb[3];
		int sampleNum;
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
