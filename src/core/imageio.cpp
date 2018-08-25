#include "imageio.h"

RAINBOW_NAMESPACE_BEGIN

void Image::save(const std::string & filename) {
	std::vector<unsigned char> data; 
	data.reserve(width * height * 4);
	for (size_t i = 0; i < image.size(); i++) {
		data.push_back(static_cast<unsigned char>(image[i].c[0] * 255));
		data.push_back(static_cast<unsigned char>(image[i].c[1] * 255));
		data.push_back(static_cast<unsigned char>(image[i].c[2] * 255));
		data.push_back(static_cast<unsigned char>(255));
	}
	unsigned error = lodepng::encode(filename, data, width, height);
	DCHECK(!error, "Encode Error as Saving PNG");
}


RAINBOW_NAMESPACE_END
