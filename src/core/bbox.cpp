#include "bbox.h"

RAINBOW_NAMESPACE_BEGIN

template<typename T>
inline bool Bounds3<T>::Intersect(const Ray & ray) const{
	Float t0 = 0, t1 = ray.tMax;
	for (int i = 0; i < 3; i++) {
		Float invDir = 1 / ray.d[i];
		Float tNear = (pMin[i] - ray.o[i]) * invDir;
		Float tFar  = (pMax[i] - ray.o[i]) * invDir;
		if (tNear > tFar) std::swap(tNear, tFar);

		t0 = std::max(t0, tNear);
		t1 = std::min(t1, tFar);
		if (t0 > t1) return false;
	}
	return true;
}

RAINBOW_NAMESPACE_END