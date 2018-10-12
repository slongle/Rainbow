#include "film.h"

RAINBOW_NAMESPACE_BEGIN

Film::Film(const std::string & _filename, const Point2i & _resolution) :
	resolution(_resolution), filename(_filename), aspect(static_cast<Float>(resolution.x) / resolution.y) {
	pixels = std::unique_ptr<Pixel[]>(new Pixel[resolution.x * resolution.y]);
}

void Film::SetPixel(const Point2i & p, const RGBSpectrum & L) const {
	Pixel &pixel = GetPixel(p);
	for (int i = 0; i < 3; i++)
		pixel.rgb[i] = L[i];
}

void Film::SaveImage() {
	Float *rgb = new Float[3 * resolution.x * resolution.y];
	Float *pos = rgb;

	for (int y = 0; y < resolution.y; y++) {
		for (int x = 0; x < resolution.x; x++) {
			Pixel &pixel = GetPixel(Point2i(x, y));
			Float invWeight = Float(1);// / pixel.filterSum;
			pos[0] = pixel.rgb[0] * invWeight;
			pos[1] = pixel.rgb[1] * invWeight;
			pos[2] = pixel.rgb[2] * invWeight;
			pos += 3;
		}
	}
	//std::cout << filename;
	WriteImage(filename, rgb, resolution);
}

Film * CreateFilm(PropertyList & list) {
	std::string filename = list.getString("filename", "output.png");
	Point2i resolution;
	resolution.x = list.getInteger("width" , 1280);
	resolution.y = list.getInteger("height", 720);
	return new Film(filename, resolution);
}

RAINBOW_NAMESPACE_END
