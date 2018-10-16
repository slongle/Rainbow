#ifndef __RAY_H
#define __RAY_H

#include "vector.h"

RAINBOW_NAMESPACE_BEGIN

class Ray {
public:
	Ray() :tMax(INFINITY) {}
	Ray(const Point3f &m_o, const Vector3f &m_d, const Float m_tMax = INFINITY) :o(m_o), d(m_d), tMax(m_tMax) {}

	Point3f operator ()(Float t) const { return o + d * t; }

	std::string toString(const int &spaceNum = 0) const {
		return
		indent("[\n", spaceNum) +
			indent(tfm::format("o = %s,\n", o), spaceNum + 4) +
			indent(tfm::format("d = %s,\n", d), spaceNum + 4) +
			indent(tfm::format("tMax = %.3f,\n", tMax), spaceNum + 4) +
		indent("]", spaceNum);
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
