#include"spectrum.h"

RAINBOW_NAMESPACE_BEGIN

RGBSpectrum toColor(const std::string &str) {
	Float tmp[3];
	int num = 0;
	std::string basStr = "";
	for (size_t i = 0; i < str.length(); i++) {
		if (str[i] == ',') {
			Assert(num < 3, "Can't convert " + str + " to Vector type");
			tmp[num] = toFloat(basStr);
			num++;
			basStr = "";
		}
		else basStr += str[i];
	}
	Assert(num < 3, "Can't convert " + str + " to Vector type");
	tmp[num] = toFloat(basStr);
	return RGBSpectrum(tmp[0], tmp[1], tmp[2]);
}


RAINBOW_NAMESPACE_END