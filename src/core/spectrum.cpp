#include"spectrum.h"

RAINBOW_NAMESPACE_BEGIN

RGBSpectrum toColor(const std::string &str) {
	/*Float tmp[3];
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
	return RGBSpectrum(tmp[0], tmp[1], tmp[2]);*/
	RGBSpectrum ret;
	char *endptr;
#ifdef __FLOAT_TYPE
	ret[0] = strtof(str.c_str(), &endptr);
	for (int i = 1; i < 3; i++) {
		endptr++;
		ret[i] = strtof(endptr, &endptr);
	}
#else
	ret[0] = strtod(str.c_str(), &endptr);
	for (int i = 1; i < 3; i++) {
		endptr++;
		ret[i] = strtod(endptr, &endptr);
	}
#endif // __FLOAT_TYPE
	//std::cout << ret << std::endl;
	return ret;
}


RAINBOW_NAMESPACE_END