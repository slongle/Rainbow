#ifndef __SAMPLING_H
#define __SAMPLING_H

#include "vector.h"

RAINBOW_NAMESPACE_BEGIN

class Distribution1D {
public:
    Distribution1D(Float* f, int n);

    //void BuildAliasTable();
    int SampleDiscrete(Float u) const;


    std::vector<Float> func, cdf;
    Float funcInt;

    //std::vector<int> aliasTable;
    //std::vector<Float> pdf;
};

inline Point2f ConcentricSampleDisk(const Point2f &u) {
	// Map uniform random numbers to $[-1,1]^2$
	Point2f uOffset = 2.f * u - Vector2f(1, 1);

	// Handle degeneracy at the origin
	if (uOffset.x == 0 && uOffset.y == 0) return Point2f(0, 0);

	// Apply concentric mapping to point
	Float theta, r;
	if (std::abs(uOffset.x) > std::abs(uOffset.y)) {
		r = uOffset.x;
		theta = PiOver4 * (uOffset.y / uOffset.x);
	}
	else {
		r = uOffset.y;
		theta = PiOver2 - PiOver4 * (uOffset.x / uOffset.y);
	}
	return r * Point2f(std::cos(theta), std::sin(theta));
}

inline Vector3f CosineSampleHemisphere(const Point2f &u) {
	Point2f d = ConcentricSampleDisk(u);
	Float z = std::sqrt(std::max((Float)0, 1 - d.x * d.x - d.y * d.y));
	return Vector3f(d.x, d.y, z);
}

inline Vector3f UniformSampleSphere(const Point2f &u) {
    Float z = 1 - 2 * u[0];
    Float r = std::sqrt(std::max((Float)0, (Float)1 - z * z));
    Float phi = 2 * M_PI * u[1];
    return Vector3f(r * std::cos(phi), r * std::sin(phi), z);
}

inline Point2f UniformSampleTriangle(const Point2f &u) {
	Float su0 = std::sqrt(u[0]);
	return Point2f(1 - su0, u[1] * su0);
}

inline Float PowerHeuristic(int nf, Float fPdf, int ng, Float gPdf) {
    Float f = nf * fPdf, g = ng * gPdf;
    return (f * f) / (f * f + g * g);
}


RAINBOW_NAMESPACE_END

#endif // __SAMPLING_H
