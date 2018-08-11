#ifndef __BBOX_H
#define __BBOX_H

#include "common.h"

RAINBOW_NAMESPACE_BEGIN

template<typename T>
class Bounds2 {
	Bounds2() {
		T minNum = std::numeric_limits<T>::lowest();
		T maxNum = std::numeric_limits<T>::max();
		pMin = Point2<T>(minNum);
		pMax = Point2<T>(maxNum);
	}
	Bounds2(const Point2<T> &u) :pMin(u), pMax(u) {}
	Bounds2(const Point2<T> &u, const Point2<T> &v) {
		pMin(std::min(u.x, v.x), std::min(u.y, v.y));
		pMax(std::max(u.x, v.x), std::max(u.y, v.y));
	}

	friend std::ostream &operator << (std::ostream &os, const Bounds2<T> &bounds) {
		cout << "[\n"
			<< "  pMin = " << bounds.pMin << "\n"
			<< "  pMax = " << bounds.pMax << "\n"
			<< "]";
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
	Bounds3(const Point3<T> &u, const Point3<T> &v) {
		pMin(std::min(u.x, v.x), std::min(u.y, v.y), std::min(u.z, v.z));
		pMax(std::max(u.x, v.x), std::max(u.y, v.y), std::max(u.z, v.z));
	}

	friend std::ostream &operator << (std::ostream &os, const Bounds3<T> &bounds) {
		cout << "[\n"
			 << "  pMin = " << bounds.pMin << "\n"
			 << "  pMax = " << bounds.pMax << "\n"
			 << "]";
		return os;
	}

	Point3<T> pMin, pMax;
};

RAINBOW_NAMESPACE_END

#endif // !__BBOX_H
