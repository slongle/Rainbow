#ifndef __COMMON_H
#define __COMMON_H

#if defined(_MSC_VER)
/* Disable some warnings on MSVC++ */
#pragma warning(disable : 4127 4702 4100 4515 4800 4146 4512 4244)
#define WIN32_LEAN_AND_MEAN     /* Don't ever include MFC on Windows */
#define NOMINMAX                /* Don't override min/max */
#endif

#if defined(Assert)
	#undef Assert
#endif

#include "rainbow.h"
#include "ext/filesystem/filesystem/resolver.h"

/* Convenience definitions */
#define RAINBOW_NAMESPACE_BEGIN namespace rainbow {
#define RAINBOW_NAMESPACE_END }

/* Define Epsilon for precision */
#define Epsilon 1e-6f

/* Define some useful constant variable*/
#undef M_PI
#define M_PI         3.14159265358979323846f
#define INV_PI       0.31830988618379067154f
#define INV_TWOPI    0.15915494309189533577f
#define INV_FOURPI   0.07957747154594766788f
#define SQRT_TWO     1.41421356237309504880f
#define INV_SQRT_TWO 0.70710678118654752440f
#define PiOver2      1.57079632679489661923f
#define PiOver4      0.78539816339744830961f


/* Define Float for changing precision conveniently*/
#define __FLOAT_TYPE
#ifdef __FLOAT_TYPE
typedef float Float;
#else
typedef double Float;
#endif

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

//class MicrofacetDistribution;
class PropertyList;
class Shape;
class Integrator;
class Scene;
class Intersection;
class SurfaceInteraction;
class Primitive;
class Light;
class AreaLight;
class Sampler;
class BxDF;
class MicrofacetDistribution;

static Float MachineEpsilon = (std::numeric_limits<Float>::epsilon() * 0.5);

inline filesystem::resolver *getFileResolver() {
	static filesystem::resolver *resolver = new filesystem::resolver();
	return resolver;
}

inline std::string indent(const std::string &str, const int &num) {
	return std::string(num, ' ') + str;
}

inline bool toBoolean(const std::string &str) {
	Assert(str == "true" || str == "false", "Can't convert " + str + " to Boolean type");
	if (str == "true") return true;
	else return false;
}

inline int toInteger(const std::string &str) {
	char *end_ptr = nullptr;
	int result = strtol(str.c_str(), &end_ptr, 10);
	Assert((*end_ptr == '\0'), "Can't convert " + str + " to Integer type");
	return result;
}

inline Float toFloat(const std::string &str) {
	char *end_ptr = nullptr;
#ifdef __FLOAT_TYPE
	float result = strtof(str.c_str(), &end_ptr);
#else
	double result = strtod(str.c_str(), &end_ptr);
#endif // __FLOAT_TYPE
	Assert((*end_ptr == '\0'), "Can't convert " + str + " to Float type");
	return result;
}

inline Float Radians(Float deg) {
	return deg * M_PI / 180;
}

inline Float Degrees(Float rad) {
	return rad * 180 / M_PI;
}

/*inline bool Quadratic(Float a, Float b, Float c, Float * t0, Float * t1) {
	Float discrim = b * b - 4 * a*c;
	if (discrim < 0) return false;
	Float rootDiscrim = std::sqrt(discrim);

	*t0 = static_cast<Float>(0.5) * (-b + rootDiscrim) / a;
	*t1 = static_cast<Float>(0.5) * (-b - rootDiscrim) / a;
	if (*t0 > *t1) std::swap(*t0, *t1);

	return true;
}*/

inline bool Quadratic(Float a, Float b, Float c, Float *t0, Float *t1) {
	// Find quadratic discriminant
	double discrim = (double)b * (double)b - 4 * (double)a * (double)c;
	if (discrim < 0) return false;
	double rootDiscrim = std::sqrt(discrim);

	// Compute quadratic _t_ values
	double q;
	if (b < 0)
		q = -.5 * (b - rootDiscrim);
	else
		q = -.5 * (b + rootDiscrim);
	*t0 = q / a;
	*t1 = c / q;
	if (*t0 > *t1) std::swap(*t0, *t1);
	return true;
}

inline uint32_t FloatToBits(float f) {
    uint32_t ui;
    memcpy(&ui, &f, sizeof(float));
    return ui;
}

inline float BitsToFloat(uint32_t ui) {
    float f;
    memcpy(&f, &ui, sizeof(uint32_t));
    return f;
}

inline uint64_t FloatToBits(double f) {
    uint64_t ui;
    memcpy(&ui, &f, sizeof(double));
    return ui;
}

inline double BitsToFloat(uint64_t ui) {
    double f;
    memcpy(&f, &ui, sizeof(uint64_t));
    return f;
}


inline float NextFloatUp(float v) {
    // Handle infinity and negative zero for _NextFloatUp()_
    if (std::isinf(v) && v > 0.) return v;
    if (v == -0.f) v = 0.f;

    // Advance _v_ to next higher float
    uint32_t ui = FloatToBits(v);
    if (v >= 0)
        ++ui;
    else
        --ui;
    return BitsToFloat(ui);
}

inline float NextFloatDown(float v) {
    // Handle infinity and positive zero for _NextFloatDown()_
    if (std::isinf(v) && v < 0.) return v;
    if (v == 0.f) v = -0.f;
    uint32_t ui = FloatToBits(v);
    if (v > 0)
        --ui;
    else
        ++ui;
    return BitsToFloat(ui);
}

inline double NextFloatUp(double v, int delta = 1) {
    if (std::isinf(v) && v > 0.) return v;
    if (v == -0.f) v = 0.f;
    uint64_t ui = FloatToBits(v);
    if (v >= 0.)
        ui += delta;
    else
        ui -= delta;
    return BitsToFloat(ui);
}

inline double NextFloatDown(double v, int delta = 1) {
    if (std::isinf(v) && v < 0.) return v;
    if (v == 0.f) v = -0.f;
    uint64_t ui = FloatToBits(v);
    if (v > 0.)
        ui -= delta;
    else
        ui += delta;
    return BitsToFloat(ui);
}

inline Float gamma(int n) {
    return (n * MachineEpsilon) / (1 - n * MachineEpsilon);
}

inline Float GammaCorrect(Float value) {
	if (value <= 0.0031308f) 
		return 12.92f * value;
	return 1.055f * std::pow(value, (Float)(1.f / 2.4f)) - 0.055f;
}

template<typename T, typename U, typename V>
inline T Clamp(const T &val, const U &low, const V &up) {
	if (val < low)
		return low;
	if (val > up)
		return up;
	return val;
}

RAINBOW_NAMESPACE_END

#endif // !__COMMON_H


