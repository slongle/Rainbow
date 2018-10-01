#include "imageio.h"

RAINBOW_NAMESPACE_BEGIN

void WriteImage(const std::string & filename, const Float * rgb, const Point2i & resolution) {
	std::vector<uint8_t> data;
	for (int y = 0; y < resolution.y; y++) {
		for (int x = 0; x < resolution.x; x++) {
#define TO_BYTE(v) (uint8_t) Clamp(255.f * GammaCorrect(v) + 0.5f, 0.f, 255.f)
			data.push_back(TO_BYTE(rgb[3 * (y*resolution.x + x) + 0]));
			data.push_back(TO_BYTE(rgb[3 * (y*resolution.x + x) + 1]));
			data.push_back(TO_BYTE(rgb[3 * (y*resolution.x + x) + 2]));
			data.push_back(TO_BYTE(1));
#undef TO_BYTE
		}
	}	
	unsigned error = lodepng::encode(filename, data, resolution.x, resolution.y);
	Assert(!error, "Encode Error as Saving PNG");
}


RAINBOW_NAMESPACE_END

