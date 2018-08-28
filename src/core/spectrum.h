#ifndef __SPECTRUM_H
#define __SPECTRUM_H

#include "common.h"

RAINBOW_NAMESPACE_BEGIN

class RGBSpectrum {
public:
	RGBSpectrum(const Float &r = 0, const Float &g = 0, const Float &b = 0) { c[0] = r; c[1] = g; c[2] = b; }
	RGBSpectrum(const Float *_c) { c[0] = _c[0]; c[1] = _c[1]; c[2] = _c[2]; }

	std::string toString(const int &spaceNum = 0) const {
		return indent(tfm::format("%.3f %.3f %.3f", c[0], c[1], c[2]), spaceNum);
	}

	friend std::ostream &operator << (std::ostream &os, const RGBSpectrum &color) {
		os << color.toString();
		return os;
	}

	Float c[3];
};

extern RGBSpectrum toColor(const std::string &str);

RAINBOW_NAMESPACE_END

#endif // !__SPECTRUM_H
