#ifndef __BBOX_H
#define __BBOX_H

#include "ray.h"
#include "interaction.h"

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

    Point3<T> operator[] (const int idx) const {
        return idx == 0 ? pMin : pMax;
	}

	bool operator == (const Bounds3<T> & b) const {
		return pMin == b.pMin && pMax == b.pMax;
	}
	
	bool Intersect(const Ray &ray, Float* hitt0 = nullptr, Float* hitt1 = nullptr) const;
    bool Intersect(const Ray &ray, const Vector3f &invDir,
        const int dirIsNeg[3]) const;

    Vector3<T> Offset(const Point3<T> &p) const {
        Vector3<T> ret = p - pMin;
        if (pMax.x > pMin.x) ret.x /= pMax.x - pMin.x;
        if (pMax.y > pMin.y) ret.y /= pMax.y - pMin.y;
        if (pMax.z > pMin.z) ret.z /= pMax.z - pMin.z;
        return ret;
    }

	Point3<T> Centroid() const { return (pMax + pMin)*0.5; }
	Vector3<T> Diagonal() const { return pMax - pMin; }

    int MaximumExtent() const {
        Vector3<T> diag = Diagonal();
        if (diag.x > diag.y && diag.x > diag.z)
            return 0;
        else if (diag.y > diag.z)
            return 1;
        else return 2;
	}

    int MinimumExtent() const {
        Vector3<T> diag = Diagonal();
        if (diag.x < diag.y && diag.x < diag.z)
            return 0;
        else if (diag.y < diag.z)
            return 1;
        else return 2;
	}

	T Area() const {
		Vector3<T> det = Diagonal();
		return 2 * (det.x * det.y + det.y * det.z + det.x * det.z);
	}
	T Volume() const {
		Vector3<T> det = Diagonal();
		return det.x * det.y * det.z;
	}

	std::string toString(const int &spaceNum = 0) const {
        return
            tfm::format("[\n"
                "    pMin = %s,\n"
                "    pMax = %s,\n"
                "]", pMin, pMax);
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

template<typename T>
bool Bounds3<T>::Intersect(const Ray & ray, Float* hitt0, Float* hitt1) const {
    Float t0 = 0, t1 = ray.tMax;
    for (int i = 0; i < 3; i++) {
        Float invDir = 1 / ray.d[i];
        Float tNear = (pMin[i] - ray.o[i]) * invDir;
        Float tFar = (pMax[i] - ray.o[i]) * invDir;
        if (tNear > tFar) std::swap(tNear, tFar);

        t0 = std::max(t0, tNear);
        t1 = std::min(t1, tFar);
        if (t0 > t1) return false;
    }
    if (hitt0) *hitt0 = t0;
    if (hitt1) *hitt1 = t1;
    return true;
}

template <typename T>
inline bool Bounds3<T>::Intersect(const Ray &ray, const Vector3f &invDir,
    const int dirIsNeg[3]) const {
    const Bounds3f &bounds = *this;

    // Check x and y slabs
    Float tMin = (bounds[dirIsNeg[0]].x - ray.o.x) * invDir.x;
    Float tMax = (bounds[1 - dirIsNeg[0]].x - ray.o.x) * invDir.x;
    Float tyMin = (bounds[dirIsNeg[1]].y - ray.o.y) * invDir.y;
    Float tyMax = (bounds[1 - dirIsNeg[1]].y - ray.o.y) * invDir.y;

    // Update tMin and tMax
    if (tMin > tyMax || tyMin > tMax) return false;
    tMin = std::max(tMin, tyMin);
    tMax = std::min(tMax, tyMax);

    // Check z slabs
    Float tzMin = (bounds[dirIsNeg[2]].z - ray.o.z) * invDir.z;
    Float tzMax = (bounds[1 - dirIsNeg[2]].z - ray.o.z) * invDir.z;

    // Update tMin and tMax
    if (tMin > tzMax || tzMin > tMax) return false;
    tMin = std::max(tMin, tzMin);
    tMax = std::min(tMax, tzMax);

    //Assert(tMax > 0, "Why!");
    return (tMin < ray.tMax) && (tMax > 0);
}

RAINBOW_NAMESPACE_END

#endif // !__BBOX_H