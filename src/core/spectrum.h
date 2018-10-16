#ifndef __SPECTRUM_H
#define __SPECTRUM_H

#include "common.h"

RAINBOW_NAMESPACE_BEGIN

class RGBSpectrum {
public:
	RGBSpectrum(const Float &_c = 0) { r = g = b = _c; }
	RGBSpectrum(const Float &_r, const Float &_g, const Float &_b) { r = _r; g = _g; b = _b; }
	RGBSpectrum(const Float *_c) { r = _c[0]; g = _c[1]; b = _c[2]; }	

	bool IsBlack() const {
		return (r == 0.f && g == 0.f && b == 0.f);
	}

	Float operator [] (const int index) const {
		Assert(0 <= index && index <= 2, "Access Violation");
		if (index == 0) return r;
		else if (index == 1) return g;
		else return b;
	}

	Float &operator [] (const int index) {
		Assert(0 <= index && index <= 2, "Access Violation");
		if (index == 0) return r;
		else if (index == 1) return g;
		else return b;
	}

	RGBSpectrum operator + (const RGBSpectrum& s) const { return RGBSpectrum(r + s.r, g + s.g, b + s.b); }
	RGBSpectrum &operator += (const RGBSpectrum& s) { r += s.r; g += s.g; b += s.b; return *this; }
	RGBSpectrum operator - (const RGBSpectrum& s) const { return RGBSpectrum(r - s.r, g - s.g, b - s.b); }
	RGBSpectrum &operator -= (const RGBSpectrum& s) { r -= s.r; g -= s.g; b -= s.b; return *this; }
	RGBSpectrum operator *(const RGBSpectrum& s) const { return RGBSpectrum(r*s.r, g*s.g, b*s.b); }
	RGBSpectrum &operator *=(const RGBSpectrum& s) { r *= s.r; g *= s.g; b *= s.b; return *this; }
	RGBSpectrum operator *(const Float& u) const { return RGBSpectrum(r*u, g*u, b*u); }
	RGBSpectrum &operator *=(const Float& u) { r *= u; g *= u; b *= u; return *this; }
	RGBSpectrum operator / (const Float& u) const {
		Assert(u != 0, "Divide zero");
		Float inv = 1.0f / u;
		return (*this)*inv;
	}
	RGBSpectrum &operator /= (const Float& u) {
		Assert(u != 0, "Divide zero");
		Float inv = 1.0f / u;
		return (*this) *= inv;
	}

	std::string toString(const int &spaceNum = 0) const {
		return indent(tfm::format("%.3f, %.3f, %.3f", r, g, b), spaceNum);
	}

	friend RGBSpectrum operator * (const Float u,const RGBSpectrum s) { return RGBSpectrum(u*s.r, u*s.g, u*s.b); }

	friend std::ostream &operator << (std::ostream &os, const RGBSpectrum &color) {
		os << color.toString();
		return os;
	}

	Float r,g,b;
};

extern RGBSpectrum toColor(const std::string &str);

RAINBOW_NAMESPACE_END

#endif // !__SPECTRUM_H
