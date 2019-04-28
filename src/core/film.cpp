#include "film.h"

#include "utility/string.h"

RAINBOW_NAMESPACE_BEGIN

void FilmTile::AddSample(
    const Point2f&       pixelSample,
    const RGBSpectrum&   L,
    const int&           num)
{
    FilmTilePixel &pixel = GetPixel(Point2i(pixelSample.x, pixelSample.y));
    pixel.contribSum += L;
    pixel.filterWeightSum += num;
}

void FilmTile::AddSample(
    const Point2i&       position,
    const Point2f&       sample,
    const RGBSpectrum&   L)
{
    FilmTilePixel &pixel = GetPixel(position);
    Float scale = filter->Evaluate(sample - Vector2f(0.5));
    pixel.contribSum += L * scale;
    pixel.filterWeightSum += scale;
}

Film::Film(
    const std::string&               m_filename, 
    const Point2i&                   m_resolution,
    const std::shared_ptr<Filter>&   m_filter) :
    filename(m_filename), resolution(m_resolution), filter(m_filter), 
    aspect(static_cast<Float>(resolution.x) / resolution.y) 
{
	pixels = std::unique_ptr<Pixel[]>(new Pixel[resolution.x * resolution.y]);
}

void Film::MergeFilmTile(const FilmTile & tile) 
{
    for (int y = tile.bounds.pMin.y; y < tile.bounds.pMax.y; y++)
        for (int x = tile.bounds.pMin.x; x < tile.bounds.pMax.x; x++) 
        {
            Pixel& pixel = GetPixel(Point2i(x, y));
            FilmTilePixel tilePixel = tile.GetPixel(Point2i(x, y));
            for (int i = 0; i < 3; i++)
                pixel.rgb[i] = tilePixel.contribSum[i];
            pixel.filterWeightSum = tilePixel.filterWeightSum;            
        }
}

void Film::AddPixel(
    const Point2i & p, 
    const RGBSpectrum & L, 
    const int &num) const 
{
    Pixel &pixel = GetPixel(p);
    for (int i = 0; i < 3; i++)
        pixel.rgb[i] += L[i];
    pixel.filterWeightSum += num;
}

void Film::SetPixel(const Point2i & p, const RGBSpectrum & L) const {
	Pixel &pixel = GetPixel(p);
	for (int i = 0; i < 3; i++)
		pixel.rgb[i] = L[i];
    pixel.filterWeightSum = 1;
}

RGBSpectrum Film::RetPixel(const Point2i & p) const {
    Pixel &pixel = GetPixel(p);
    RGBSpectrum rgb;
    Float invWeight = Float(1) / pixel.filterWeightSum;
    for (int i = 0; i < 3; i++)
        rgb[i] = pixel.rgb[i] * invWeight;
    return rgb;
}

void Film::SaveImage() const
{
    std::string extension = utility::GetExtension(filename);
    if (extension == "PNG" || extension == "png") {
        unsigned char *rgba = new unsigned char[4 * resolution.x * resolution.y];
        ExportToUnsignedCharPointer(rgba);
        ExportToPNG(filename, rgba, resolution.x, resolution.y);        
    }
    else if (extension == "HDR" || extension == "hdr") {
        float* rgb = new float[3 * resolution.x * resolution.y];
        ExportToLinearFloat(rgb);
        ExportToHDR(filename, rgb, resolution.x, resolution.y);
    }
}

void Film::SaveImage(
    const std::string& name) const 
{
    std::string extension = utility::GetExtension(name);
    if (extension == "PNG" || extension == "png") {
        unsigned char *rgba = new unsigned char[4 * resolution.x * resolution.y];
        ExportToUnsignedCharPointer(rgba);
        ExportToPNG(name, rgba, resolution.x, resolution.y);
    }
    else if (extension == "HDR" || extension == "hdr") {
        float* rgb = new float[3 * resolution.x * resolution.y];
        ExportToLinearFloat(rgb);
        ExportToHDR(name, rgb, resolution.x, resolution.y);
    }
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

void Film::ExportToLinearFloat(float* data) const 
{
    int idx = 0;
    for (int y = 0; y < resolution.y; y++) {
        for (int x = 0; x < resolution.x; x++) {
            Pixel &pixel = GetPixel(Point2i(x, y));
            const Float invWeight = Float(1) / pixel.filterWeightSum;
            data[idx + 0] = (pixel.rgb[0] * invWeight);
            data[idx + 1] = (pixel.rgb[1] * invWeight);
            data[idx + 2] = (pixel.rgb[2] * invWeight);
            idx += 3;
        }
    }
}

void Film::ExportToHeatMapUnsignedCharPointer(unsigned char * data) const {
    int midVal, maxVal = pixels[0].filterWeightSum, minVal = pixels[0].filterWeightSum;
    for (int i = 1; i < resolution.x*resolution.y; i++) {
        if (pixels[i].filterWeightSum == 0) break;
        maxVal = std::max(maxVal, (int)pixels[i].filterWeightSum);
        minVal = std::min(minVal, (int)pixels[i].filterWeightSum);
    }
    midVal = (maxVal + minVal) / 2;
    int halfLen = midVal - minVal;
    Assert(halfLen != 0, "All pixels' SampleNum are the same!");
    
    int idx = 0;
    for (int i = 0; i < resolution.x*resolution.y; i++) {
        Float r, g, b;
        if (pixels[i].filterWeightSum == 0) break;
        if (pixels[i].filterWeightSum <= midVal) {
            r = 0;
            g = Float(pixels[i].filterWeightSum - minVal) / halfLen;
            b = 1 - g;
        }
        else {
            r = Float(pixels[i].filterWeightSum - midVal) / halfLen;
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
            Float invWeight = Float(1) / pixel.filterWeightSum;
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
    Float invWeight = Float(1) / pixel.filterWeightSum;
#define TO_BYTE(v) (uint8_t) Clamp(255.f * GammaCorrect(v) + 0.5f, 0.f, 255.f)
    data[idx + 0] = (TO_BYTE(pixel.rgb[0] * invWeight));
    data[idx + 1] = (TO_BYTE(pixel.rgb[1] * invWeight));
    data[idx + 2] = (TO_BYTE(pixel.rgb[2] * invWeight));
    data[idx + 3] = (TO_BYTE(1));
#undef TO_BYTE
}

Film *CreateFilm(
    PropertyList&                    list, 
    const std::shared_ptr<Filter>&   filter) 
{
	std::string filename = list.getString("filename", "output.png");
	Point2i resolution;
	resolution.x = list.getInteger("width" , 1280);
	resolution.y = list.getInteger("height", 720);
	return new Film(filename, resolution, filter);
}

RAINBOW_NAMESPACE_END
