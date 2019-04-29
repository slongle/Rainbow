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
        const Bounds2i&                 pixelBounds,
        const Bounds2i&                 sampleBounds,
        const std::shared_ptr<Filter>   filter) 
    : pixelBounds(pixelBounds), sampleBounds(sampleBounds), filter(filter),
      size(pixelBounds.pMax.x - pixelBounds.pMin.x, pixelBounds.pMax.y - pixelBounds.pMin.y)
    {       
        pixels = std::unique_ptr<FilmTilePixel[]>(new FilmTilePixel[size.x * size.y]);
    }

    void AddSample(
        const Point2f &pixelSample, 
        const RGBSpectrum &L, 
        const int &num = 1);

    void AddSample(
        const Point2i&       position,
        const Point2f&       sample,
        const RGBSpectrum&   L
    );

    static std::vector<FilmTile> GenerateTiles(
        const Point2i&                   resolution, 
        const int&                       tileSize,
        const std::shared_ptr<Filter>&   filter) {
        Point2i numTiles(std::ceil(resolution.x / static_cast<Float>(tileSize)),
            std::ceil(resolution.y / static_cast<Float>(tileSize)));
        std::vector<FilmTile> tiles;
        Point2i p0(0, 0);
        for (int i = 0; i < numTiles.x * numTiles.y; i++) {
            Point2i p1 = Min(p0 + Point2i(tileSize), resolution);
            Bounds2i tilePixelBounds(p0, p1);
            Bounds2i tileSampleBounds(
                Point2i(Floor(p0 + Point2f(0.5) - filter->m_radius)),
                Point2i(Ceil (p1 - Point2f(0.5) + filter->m_radius)));
            tiles.emplace_back(tilePixelBounds, tileSampleBounds, filter);
            if (p1.x == resolution.x) {
                p0.x = 0;
                p0.y = p1.y;
            }
            else {
                p0.x = p1.x;
            }
        }
        return tiles;
    }    

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
    std::shared_ptr<Filter>            filter;
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
    void AddPixel(const Point2i& p, const RGBSpectrum& L, const int &num = 1) const;
	void SetPixel(const Point2i& p, const RGBSpectrum& L) const;
    RGBSpectrum RetPixel(const Point2i & p) const;
    void SaveImage() const;
    void SaveImage(const std::string &name) const;    
    void SaveHeatMapImage() const;
    void SaveHeatMapImage(const std::string &name) const;

    void ExportToLinearFloat(float* data) const;
    void ExportToHeatMapUnsignedCharPointer(unsigned char* data) const;
    void ExportToUnsignedCharPointer(unsigned char* data) const;
    void UpdateToUnsignedCharPointer(unsigned char* data, const int &x, const int &y) const;    

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
};

Film * CreateFilm(
    PropertyList&                    list, 
    const std::shared_ptr<Filter>&   filter);

RAINBOW_NAMESPACE_END

#endif // !__IMAGEIO_H
