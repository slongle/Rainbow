#ifndef __FILM_H
#define __FILM_H

#include "filter.h"
#include "spectrum.h"
#include "imageio.h"

RAINBOW_NAMESPACE_BEGIN

#define RAINBOW_TILE_SIZE 16



struct FilmTilePixel {
    FilmTilePixel(
        const RGBSpectrum&   contribSum = RGBSpectrum(0.),
        const Float&         filterWeightSum = 0)
        : contribSum(contribSum), filterWeightSum(filterWeightSum) {};

    RGBSpectrum   contribSum;
    Float         filterWeightSum;
};

class FilmTile {
public:
    FilmTile(
        const Bounds2i  &pixelBounds,
        const Bounds2i  &sampleBounds,
        const Point2i   &resolution,
        const Vector2f  &filterRadius,
        const Vector2f  &invFilterRadius,
        const Float     *filterTable,
        const int       &filterTableWidth,
        const double    &invFilterTableWidth)
        : pixelBounds(pixelBounds), sampleBounds(sampleBounds), filmResolution(resolution),
        filterRadius(filterRadius), invFilterRadius(invFilterRadius), filterTable(filterTable),
        filterTableWidth(filterTableWidth), invFilterTableWidth(invFilterTableWidth),
        size(pixelBounds.pMax.x - pixelBounds.pMin.x, pixelBounds.pMax.y - pixelBounds.pMin.y)
    {       
        pixels = std::unique_ptr<FilmTilePixel[]>(new FilmTilePixel[size.x * size.y]);
        int xWeightSize = (int)std::ceil(2 * filterRadius.x) + 1;
        int yWeightSize = (int)std::ceil(2 * filterRadius.y) + 1;
        xIndex = new int[xWeightSize];
        yIndex = new int[yWeightSize];
        memset(xIndex, 0, sizeof(int) * xWeightSize);
        memset(yIndex, 0, sizeof(int) * yWeightSize);
    }
    
    /*~FilmTile() {
        delete[] filterTable;
        delete[] xIndex;
        delete[] yIndex;
    }*/

    void AddSample(
        const Point2f &pixelSample, 
        const RGBSpectrum &L, 
        const int &num = 1);

    void AddSample(
        const Point2i&       position,
        const Point2f&       sample,
        const RGBSpectrum&   L
    );

      

    FilmTilePixel &GetPixel(const Point2i &p) const 
    {
        Assert(pixelBounds.pMin.x <= p.x && p.x < pixelBounds.pMax.x
            && pixelBounds.pMin.y <= p.y && p.y < pixelBounds.pMax.y, "Access Violation");
        return pixels[(p.y - pixelBounds.pMin.y)*size.x + p.x - pixelBounds.pMin.x];
    }

    const Bounds2i pixelBounds;    //[)
    const Bounds2i sampleBounds;   //[)
    const Point2i  size;
    
private:    

    std::unique_ptr<FilmTilePixel[]>   pixels;
    const Point2i filmResolution;

    const Vector2f filterRadius, invFilterRadius;
    const Float* filterTable;
    int filterTableWidth;
    double invFilterTableWidth;

    int *xIndex = nullptr;
    int *yIndex = nullptr;

};

/*
  left-upper corner is (0, 0)
  right-upper corner is (width - 1, 0)
  left-lower corner is (0, height - 1)
  right-lower corner is (width - 1, height - 1)
*/

class Film{
public:	
	Film(
        const std::string&               filename, 
        const Point2i&                   resolution,
        const std::shared_ptr<Filter>&   filter);

    void MergeFilmTile(const FilmTile &tile);
    void AddSplat(const Point2i& p, const RGBSpectrum& L);
    void AddPixel(const Point2i& p, const RGBSpectrum& L, const int &num = 1) const;
	void SetPixel(const Point2i& p, const RGBSpectrum& L) const;
    RGBSpectrum RetPixel(const Point2i & p) const;
    void SaveImage(Float splatScale = 1) const;
    void SaveImage(const std::string &name,Float splatScale = 1) const;
    void SaveHeatMapImage() const;
    void SaveHeatMapImage(const std::string &name) const;

    void ExportToLinearFloat(float* data, Float splatScale) const;
    void ExportToHeatMapUnsignedCharPointer(unsigned char* data) const;
    void ExportToUnsignedCharPointer(unsigned char* data, Float splatScale) const;
    void UpdateToUnsignedCharPointer(unsigned char* data, const int &x, const int &y) const;    
    std::vector<FilmTile> GenerateTiles(const int &tileSize);

    std::string toString() const {
        return tfm::format(
            "Film [\n"
            "    Filename : %s,\n"
            "    Resolution : %d * %d,\n"
            "    Filter : %s,\n"
            "]"
            , filename
            , resolution.x, resolution.y
            , indent(filter->toString(), 13)
        );
    }

	const std::string filename;
	const Point2i resolution;
	const Float aspect;
	std::shared_ptr<Filter> filter;

private:	
    struct Pixel {
        Pixel() { rgb[0] = rgb[1] = rgb[2] = filterWeightSum = 0; }
        Float rgb[3];
        Float filterWeightSum;
    };


	Pixel &GetPixel(const Point2i &p) const {
		Assert(0 <= p.x && p.x < resolution.x 
            && 0 <= p.y && p.y < resolution.y, "Access Violation");
		return pixels[p.y*resolution.x + p.x];
	}

	std::unique_ptr<Pixel[]> pixels;

    static const int filterTableWidth = 16;
    static constexpr  double invFilterTableWidth = 1. / filterTableWidth;
    Float filterTable[filterTableWidth * filterTableWidth];
};

Film * CreateFilm(
    const PropertyList              &list, 
    const std::shared_ptr<Filter>   &filter);

RAINBOW_NAMESPACE_END

#endif // !__IMAGEIO_H
