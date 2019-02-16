#ifndef __FILM_H
#define __FILM_H

#include "filter.h"
#include "spectrum.h"
#include "vector.h"
#include "imageio.h"
#include "object.h"

RAINBOW_NAMESPACE_BEGIN

#define RAINBOW_TILE_SIZE 64

struct Pixel {
    Pixel() { rgb[0] = rgb[1] = rgb[2] = sampleNum = 0; }
    Float rgb[3];
    int sampleNum;
};

class FilmTile {
public:
    FilmTile(const Bounds2i &m_bounds) :
        bounds(m_bounds),
        size(m_bounds.pMax.x - m_bounds.pMin.x, m_bounds.pMax.y - m_bounds.pMin.y) {
        pixels = std::unique_ptr<Pixel[]>(new Pixel[size.x*size.y]);
    }

    void AddSample(const Point2f &pixelSample, const RGBSpectrum &L);

    static std::vector<FilmTile> GenerateTiles(const Point2i &resolution, const int &tileSize) {
        Point2i numTiles(std::ceil(resolution.x / static_cast<Float>(tileSize)),
            std::ceil(resolution.y / static_cast<Float>(tileSize)));
        std::vector<FilmTile> tiles;
        Point2i position(0, 0);
        for (int i = 0; i < numTiles.x * numTiles.y; i++) {
            Point2i diag(std::min(position.x + tileSize, resolution.x),
                std::min(position.y + tileSize, resolution.y));
            Bounds2i tileBounds(position, diag);
            tiles.emplace_back(tileBounds);
            if (diag.x == resolution.x) {
                position.x = 0;
                position.y = diag.y;
            }
            else {
                position.x = diag.x;
            }
        }
        return tiles;
    }

    Pixel &GetPixel(const Point2i &p) const {
        Assert(bounds.pMin.x <= p.x && p.x < bounds.pMax.x
            && bounds.pMin.y <= p.y && p.y < bounds.pMax.y, "Access Violation");
        return pixels[(p.y - bounds.pMin.y)*size.x + p.x - bounds.pMin.x];
    }

    const Bounds2i bounds;
    const Point2i size;
private:
    std::unique_ptr<Pixel[]> pixels;
    //mutable tbb::mutex m_mutex;

};

/*
  left-upper corner is (0, 0)
  right-upper corner is (width - 1, 0)
  left-lower corner is (0, height - 1)
  right-lower corner is (width - 1, height - 1)
*/

class Film{
public:	
	Film(const std::string & _filename, const Point2i & _resolution);

    void MergeFilmTile(const FilmTile &tile);
    void AddPixel(const Point2i& p, const RGBSpectrum& L, const int &num = 1) const;
	void SetPixel(const Point2i& p, const RGBSpectrum& L) const;
    RGBSpectrum RetPixel(const Point2i & p) const;
    void SaveImage() const;
    void SaveImage(const std::string &name) const;
    void SaveHeatMapImage() const;
    void SaveHeatMapImage(const std::string &name) const;
    void ExportToHeatMapUnsignedCharPointer(unsigned char* data) const;
    void ExportToUnsignedCharPointer(unsigned char* data) const;
    void UpdateToUnsignedCharPointer(unsigned char* data, const int &x, const int &y) const;    

	const std::string filename;
	const Point2i resolution;
	const Float aspect;
	std::unique_ptr<Filter> filter;

private:	
	std::unique_ptr<Pixel[]> pixels;

	Pixel &GetPixel(const Point2i &p) const {
		Assert(0 <= p.x && p.x < resolution.x 
            && 0 <= p.y && p.y < resolution.y, "Access Violation");
		return pixels[p.y*resolution.x + p.x];
	}

};

Film * CreateFilm(PropertyList &list);

RAINBOW_NAMESPACE_END

#endif // !__IMAGEIO_H
