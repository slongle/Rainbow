#ifndef __RAY_H
#define __RAY_H

#include "vector.h"

RAINBOW_NAMESPACE_BEGIN

class Ray {
public:
	Ray() :tMax(INFINITY) {}
	Ray(const Point3f &_o, const Vector3f &_d, const Float _tMax = INFINITY) :o(_o), d(_d), tMax(_tMax) {}

	Point3f operator ()(Float t) const { return o + d * t; }

	std::string toString() const {
		return tfm::format("[\n\to = %s\n\td = %s\n\ttMax = \6.4f\n]", o, d, tMax);
	}

	friend std::ostream &operator << (std::ostream &os, const Ray &r) {
		os << r.toString();
		return os;
	}

	mutable Float tMax;
	Point3f o;
	Vector3f d;
};

RAINBOW_NAMESPACE_END

#endif // !__RAY_H
