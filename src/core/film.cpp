#include "film.h"

RAINBOW_NAMESPACE_BEGIN

Film::Film(const Point2i & _resolution, std::unique_ptr<Filter> _filter, const std::string & _filename) :
	resolution(_resolution), filter(std::move(_filter)), filename(_filename) {
	pixels = std::unique_ptr<Pixel[]>(new Pixel[resolution.x * resolution.y]);
}

void Film::SaveImage() {
	Float *rgb = new Float[3 * resolution.x * resolution.y];
	Float *pos = rgb;

	for (int y = 0; y < resolution.y; y++) {
		for (int x = 0; x < resolution.x; x++) {
			Pixel &pixel = GetPixel(Point2i(x, y));
			Float invWeight = Float(1) / pixel.filterSum;
			pos[0] = pixel.rgb[0] * invWeight;
			pos[1] = pixel.rgb[1] * invWeight;
			pos[2] = pixel.rgb[2] * invWeight;
			pos += 3;
		}
	}
	WriteImage(filename, rgb, resolution);
}

RAINBOW_NAMESPACE_END
