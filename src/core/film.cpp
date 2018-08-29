#include "film.h"

RAINBOW_NAMESPACE_BEGIN

void Film::save(const std::string & filename) const {
	std::vector<unsigned char> data; 
	data.reserve(width * height * 4);
	for (size_t i = 0; i < film.size(); i++) {
		data.push_back(static_cast<unsigned char>(film[i].c[0] * 255));
		data.push_back(static_cast<unsigned char>(film[i].c[1] * 255));
		data.push_back(static_cast<unsigned char>(film[i].c[2] * 255));
		data.push_back(static_cast<unsigned char>(255));
	}
	unsigned error = lodepng::encode(filename, data, width, height);
	Assert(!error, "Encode Error as Saving PNG");
}


RAINBOW_NAMESPACE_END
