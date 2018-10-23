#ifndef __SAMPLING_H
#define __SAMPLING_H

#include "vector.h"

RAINBOW_NAMESPACE_BEGIN

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

inline Point2f UniformSampleTriangle(const Point2f &u) {
	Float su0 = std::sqrt(u[0]);
	return Point2f(1 - su0, u[1] * su0);
}


RAINBOW_NAMESPACE_END

#endif // __SAMPLING_H
