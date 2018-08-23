#ifndef __COMMON_H
#define __COMMON_H

// Global Include Files
#include<iostream>
#include<iomanip>
#include<assert.h>
#include"../../ext/tinyformat.h"
#include"error.h"

#if defined(_MSC_VER)
/* Disable some warnings on MSVC++ */
#pragma warning(disable : 4127 4702 4100 4515 4800 4146 4512)
#define WIN32_LEAN_AND_MEAN     /* Don't ever include MFC on Windows */
#define NOMINMAX                /* Don't override min/max */
#endif

/* Convecience for checking */
#define DCHECK(EXP, LOG) if (!(EXP)) tfm::format(std::cerr, "%s\n", LOG), assert(EXP)

/* Convenience definitions */
#define RAINBOW_NAMESPACE_BEGIN namespace rainbow {
#define RAINBOW_NAMESPACE_END }

/* Define Epsilon for precision */
#define Epsilon 1e-5

/* Define some useful constant variable*/
#undef M_PI
#define M_PI         3.14159265358979323846f
#define INV_PI       0.31830988618379067154f
#define INV_TWOPI    0.15915494309189533577f
#define INV_FOURPI   0.07957747154594766788f
#define SQRT_TWO     1.41421356237309504880f
#define INV_SQRT_TWO 0.70710678118654752440f

/* Define Float for changing precision conveniently*/
typedef float Float;

RAINBOW_NAMESPACE_BEGIN

/* Forward declarations */
template<typename T> class Vector2;
template<typename T> class Vector3;
template<typename T> class Point2;
template<typename T> class Point3;
template<typename T> class Normal3;
template<typename T> class Bounds2;
template<typename T> class Bounds3;

typedef Vector2<Float> Vector2f;
typedef Vector3<Float> Vector3f;
typedef Point2<int>	   Point2i;
typedef Point2<Float>  Point2f;
typedef Point3<Float>  Point3f;
typedef Normal3<Float> Normal3f;
typedef Bounds2<Float> Bounds2f;
typedef Bounds2<int>   Bounds2i;
typedef Bounds3<Float> Bounds3f;

class Shape;
class Intersection;
class SurfaceIntersection;


inline Float radToDeg(Float theta) {
	return theta * M_PI / 180;
}

inline Float degToRad(Float theta) {
	return theta * 180 / M_PI;
}

inline bool Quadratic(Float a, Float b, Float c, Float * t0, Float * t1) {
	Float discrim = b * b - 4 * a*c;
	if (discrim < 0) return false;
	Float rootDiscrim = std::sqrt(discrim);

	*t0 = static_cast<Float>(0.5) * (-b + rootDiscrim) / a;
	*t1 = static_cast<Float>(0.5) * (-b - rootDiscrim) / a;
	if (*t0 > *t1) std::swap(*t0, *t1);

	return true;
}

RAINBOW_NAMESPACE_END

#endif // !__COMMON_H


