#ifndef __BBOX_H
#define __BBOX_H

#include "common.h"
#include "ray.h"

RAINBOW_NAMESPACE_BEGIN

template<typename T>
class Bounds2 {
public:
	Bounds2() {
		T minNum = std::numeric_limits<T>::lowest();
		T maxNum = std::numeric_limits<T>::max();
		pMin = Point2<T>(minNum);
		pMax = Point2<T>(maxNum);
	}
	Bounds2(const Point2<T> &u) :pMin(u), pMax(u) {}
	Bounds2(const Point2<T> &u, const Point2<T> &v):
		pMin(std::min(u.x, v.x), std::min(u.y, v.y)),
		pMax(std::max(u.x, v.x), std::max(u.y, v.y)){}

	std::string toString(const int &spaceNum=0) const {
		return
		indent("[\n", spaceNum) +
			indent(tfm::format("pMin = %s,\n", pMin), spaceNum + 4) +
			indent(tfm::format("pMax = %s,\n", pMax), spaceNum + 4) +
		indent("]", spaceNum);		
	}

	friend std::ostream &operator << (std::ostream &os, const Bounds2<T> &bounds) {
		os << bounds.toString();
		return os;
	}

	Point2<T> pMin, pMax;
};

template<typename T>
class Bounds3 {
public:
	Bounds3() {
		T minNum = std::numeric_limits<T>::lowest();
		T maxNum = std::numeric_limits<T>::max();
		pMin = Point3<T>(maxNum);
		pMax = Point3<T>(minNum);
	}
	Bounds3(const Point3<T> &u) :pMin(u), pMax(u) {}
	Bounds3(const Point3<T> &u, const Point3<T> &v):
		pMin(std::min(u.x, v.x), std::min(u.y, v.y), std::min(u.z, v.z)),
		pMax(std::max(u.x, v.x), std::max(u.y, v.y), std::max(u.z, v.z)) {}

	bool operator == (const Bounds3<T> & b) const {
		return pMin == b.pMin && pMax == b.pMax;
	}
	
	bool Intersect(const Ray &ray) const;

	std::string toString() const {
		return
		indent("[\n", spaceNum) +
			indent(tfm::format("pMin = %s,\n", pMin), spaceNum + 4) +
			indent(tfm::format("pMax = %s,\n", pMax), spaceNum + 4) +
		indent("]", spaceNum);
	}

	friend std::ostream &operator << (std::ostream &os, const Bounds3<T> &bounds) {
		os << bounds.toString();
		return os;
	}

	Point3<T> pMin, pMax;
};

template<typename T>
Bounds3<T> Union(const Bounds3<T> &bounds, const Point3<T> &p) {
	Bounds3<T> ret;
	ret.pMin = Min(bounds.pMin, p);
	ret.pMax = Max(bounds.pMax, p);
	return ret;
}

template<typename T>
Bounds3<T> Union(const Bounds3<T> &b1, const Bounds3<T> &b2) {
	Bounds3<T> ret;
	ret.pMin = Min(b1.pMin, b2.pMin);
	ret.pMax = Max(b1.pMax, b2.pMax);
	return ret;
}

RAINBOW_NAMESPACE_END

#endif // !__BBOX_H