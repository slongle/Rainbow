#include "film.h"

RAINBOW_NAMESPACE_BEGIN

Film::Film(const std::string & _filename, const Point2i & _resolution) :
    filename(_filename), resolution(_resolution), 
    aspect(static_cast<Float>(resolution.x) / resolution.y) {
	pixels = std::unique_ptr<Pixel[]>(new Pixel[resolution.x * resolution.y]);
}

void Film::AddPixel(const Point2i & p, const RGBSpectrum & L) const {
    Pixel &pixel = GetPixel(p);
    if (!L.IsBlack())std::cout << L << std::endl;
    for (int i = 0; i < 3; i++)
        pixel.rgb[i] += L[i];
    pixel.sampleNum ++;
}

void Film::SetPixel(const Point2i & p, const RGBSpectrum & L) const {
	Pixel &pixel = GetPixel(p);
	for (int i = 0; i < 3; i++)
		pixel.rgb[i] = L[i];
    pixel.sampleNum = 1;
}

void Film::SaveImage() const {
	unsigned char *rgba = new unsigned char[4 * resolution.x * resolution.y];
    ExportToUnsignedCharPointer(rgba);
    ExportToPNG(filename, rgba, resolution.x, resolution.y);
}

void Film::ExportToUnsignedCharPointer(unsigned char* data) const {
    int idx = 0;
    for (int y = 0; y < resolution.y; y++) {
        for (int x = 0; x < resolution.x; x++) {
            Pixel &pixel = GetPixel(Point2i(x, y));
            Float invWeight = Float(1) / pixel.sampleNum;
#define TO_BYTE(v) (uint8_t) Clamp(255.f * GammaCorrect(v) + 0.5f, 0.f, 255.f)
            data[idx + 0] = (TO_BYTE(pixel.rgb[0] * invWeight));
            data[idx + 1] = (TO_BYTE(pixel.rgb[1] * invWeight));
            data[idx + 2] = (TO_BYTE(pixel.rgb[2] * invWeight));
            data[idx + 3] = (TO_BYTE(1));
#undef TO_BYTE
            idx += 4;
        }
    }
}

void Film::UpdateToUnsignedCharPointer(unsigned char* data, const int &x, const int &y) const {
    int idx = 4 * (y*resolution.x + x);
    Pixel &pixel = GetPixel(Point2i(x, y));
    Float invWeight = Float(1) / pixel.sampleNum;
#define TO_BYTE(v) (uint8_t) Clamp(255.f * GammaCorrect(v) + 0.5f, 0.f, 255.f)
    data[idx + 0] = (TO_BYTE(pixel.rgb[0] * invWeight));
    data[idx + 1] = (TO_BYTE(pixel.rgb[1] * invWeight));
    data[idx + 2] = (TO_BYTE(pixel.rgb[2] * invWeight));
    data[idx + 3] = (TO_BYTE(1));
#undef TO_BYTE
}

Film * CreateFilm(PropertyList & list) {
	std::string filename = list.getString("filename", "output.png");
	Point2i resolution;
	resolution.x = list.getInteger("width" , 1280);
	resolution.y = list.getInteger("height", 720);
	return new Film(filename, resolution);
}

RAINBOW_NAMESPACE_END
