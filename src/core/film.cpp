#include "film.h"

RAINBOW_NAMESPACE_BEGIN

Film::Film(const std::string & m_filename, const Point2i & m_resolution) :
    filename(m_filename), resolution(m_resolution), 
    aspect(static_cast<Float>(resolution.x) / resolution.y) {
	pixels = std::unique_ptr<Pixel[]>(new Pixel[resolution.x * resolution.y]);
}

void Film::MergeFilmTile(const FilmTile & tile) {
    for (int y = tile.bounds.pMin.y; y < tile.bounds.pMax.y; y++)
        for (int x = tile.bounds.pMin.x; x < tile.bounds.pMax.x; x++)
            GetPixel(Point2i(x,y)) = tile.GetPixel(Point2i(x, y));
}

void Film::AddPixel(const Point2i & p, const RGBSpectrum & L, const int &num) const {
    Pixel &pixel = GetPixel(p);
    //if (!L.IsBlack())std::cout << L << std::endl;
    for (int i = 0; i < 3; i++)
        pixel.rgb[i] += L[i];
    pixel.sampleNum += num;
}

void Film::SetPixel(const Point2i & p, const RGBSpectrum & L) const {
	Pixel &pixel = GetPixel(p);
	for (int i = 0; i < 3; i++)
		pixel.rgb[i] = L[i];
    pixel.sampleNum = 1;
}

RGBSpectrum Film::RetPixel(const Point2i & p) const {
    Pixel &pixel = GetPixel(p);
    RGBSpectrum rgb;
    Float invWeight = Float(1) / pixel.sampleNum;
    for (int i = 0; i < 3; i++)
        rgb[i] = pixel.rgb[i] * invWeight;
    return rgb;
}

void Film::SaveImage() const {
    unsigned char *rgba = new unsigned char[4 * resolution.x * resolution.y];
    ExportToUnsignedCharPointer(rgba);
    ExportToPNG(filename, rgba, resolution.x, resolution.y);
}


void Film::SaveImage(const std::string &name) const {
	unsigned char *rgba = new unsigned char[4 * resolution.x * resolution.y];
    ExportToUnsignedCharPointer(rgba);
    ExportToPNG(name, rgba, resolution.x, resolution.y);
}

void Film::SaveHeatMapImage() const {
    unsigned char *rgba = new unsigned char[4 * resolution.x * resolution.y];
    ExportToHeatMapUnsignedCharPointer(rgba);
    ExportToPNG(filename, rgba, resolution.x, resolution.y);
}



void Film::SaveHeatMapImage(const std::string & name) const {
    unsigned char *rgba = new unsigned char[4 * resolution.x * resolution.y];
    ExportToHeatMapUnsignedCharPointer(rgba);
    ExportToPNG(name, rgba, resolution.x, resolution.y);
}

void Film::ExportToHeatMapUnsignedCharPointer(unsigned char * data) const {
    int midVal, maxVal = pixels[0].sampleNum, minVal = pixels[0].sampleNum;
    for (int i = 1; i < resolution.x*resolution.y; i++) {
        if (pixels[i].sampleNum == 0) break;
        maxVal = std::max(maxVal, pixels[i].sampleNum);
        minVal = std::min(minVal, pixels[i].sampleNum);
    }
    midVal = (maxVal + minVal) / 2;
    int halfLen = midVal - minVal;
    Assert(halfLen != 0, "All pixels' SampleNum are the same!");
    
    int idx = 0;
    for (int i = 0; i < resolution.x*resolution.y; i++) {
        Float r, g, b;
        if (pixels[i].sampleNum == 0) break;
        if (pixels[i].sampleNum <= midVal) {
            r = 0;
            g = Float(pixels[i].sampleNum - minVal) / halfLen;
            b = 1 - g;
        }
        else {
            r = Float(pixels[i].sampleNum - midVal) / halfLen;
            g = 1 - r;
            b = 0;
        }
#define TO_BYTE(v) (uint8_t) Clamp(255.f * GammaCorrect(v) + 0.5f, 0.f, 255.f)
        data[idx + 0] = (TO_BYTE(r));
        data[idx + 1] = (TO_BYTE(g));
        data[idx + 2] = (TO_BYTE(b));
        data[idx + 3] = (TO_BYTE(1));
#undef TO_BYTE
        idx += 4;
    }
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

Film *CreateFilm(PropertyList & list) {
	std::string filename = list.getString("filename", "output.png");
	Point2i resolution;
	resolution.x = list.getInteger("width" , 1280);
	resolution.y = list.getInteger("height", 720);
	return new Film(filename, resolution);
}

void FilmTile::AddSample(const Point2f & pixelSample, const RGBSpectrum & L, const int &num) {
    //Pixel &pixel = GetPixel(Point2i(std::floor(pixelSample.x), std::floor(pixelSample.y)));
    Pixel &pixel = GetPixel(Point2i(pixelSample.x, pixelSample.y));
    for (int i = 0; i < 3; i++)
        pixel.rgb[i] += L[i];
    pixel.sampleNum += num;
}

RAINBOW_NAMESPACE_END
