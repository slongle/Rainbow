#include "imageio.h"

#include "vector.h"

#include "../../ext/lodepng/lodepng.h"

#define STB_IMAGE_IMPLEMENTATION
#include "ext/stb_images/stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "ext/stb_images/stb_image_write.h" 

RAINBOW_NAMESPACE_BEGIN

/*void ExportToPNG(const std::string & filename, const Float * rgb, const Point2i & resolution) {
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
}*/

void ExportToPNG(const std::string& filename, const unsigned char* rgba, const int& width, const int& height) {
    stbi_write_png(filename.c_str(), width, height, 4, rgba, 0);
}

void MergeRawToPNG(const std::string & inputFilename1, const std::string & inputFilename2, 
    const std::string & outputFilename) {
    Assert(false, "No Implement!");
}

void MergePNGToPNG(const std::string& inputFilename1, const std::string& inputFilename2,
    const std::string& outputFilename) {
    int width1, height1;
    unsigned char* input1 = stbi_load(inputFilename1.c_str(), &width1, &height1, NULL, 4);
    int width2, height2;
    unsigned char* input2 = stbi_load(inputFilename2.c_str(), &width2, &height2, NULL, 4);
    Assert(height1 == height2 && width1 == width2, "Try To Merge Different Resolution Images");
    for (int i = 0; i < width1*height1 * 4; i++) {
        input1[i] = (input1[i] + input2[i]) / 2;        
    }
    stbi_write_png(outputFilename.c_str(), width1, height1, 4, input1, 0);
}

RAINBOW_NAMESPACE_END

