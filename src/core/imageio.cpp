#include "imageio.h"

RAINBOW_NAMESPACE_BEGIN

void WriteImage(const std::string & filename, const Float * rgb, const Point2i & resolution) {
	std::unique_ptr<uint8_t[]> rgb8(new uint8_t[4 * resolution.x*resolution.y]);
	uint8_t *pos = rgb8.get();
	for (int y = 0; y < resolution.y; y++) {
		for (int x = 0; x < resolution.x; x++) {
#define TO_BYTE(v) (uint8_t) Clamp(255.f * GammaCorrect(v) + 0.5f, 0.f, 255.f)
			pos[0] = TO_BYTE(rgb[3 * (y*resolution.x + x) + 0]);
			pos[1] = TO_BYTE(rgb[3 * (y*resolution.x + x) + 1]);
			pos[2] = TO_BYTE(rgb[3 * (y*resolution.x + x) + 2]);
			pos[3] = TO_BYTE(1);
#undef TO_BYTE
			pos += 4;
		}
	}
	std::vector<uint8_t> data(*(rgb8._Myptr()));
	unsigned error = lodepng::encode(filename, data, resolution.x, resolution.y);
	Assert(!error, "Encode Error as Saving PNG");
}


RAINBOW_NAMESPACE_END

