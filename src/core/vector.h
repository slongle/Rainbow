#ifndef __VECTOR_H
#define __VECTOR_H

#include <algorithm>
#include "common.h"

RAINBOW_NAMESPACE_BEGIN

template<typename T>
inline bool isNaN(const T x) {
	return std::isnan(x);
}

//template<>
inline bool isNaN(const int x) {
	return false;
}

template<typename T>
class Vector2 {
public:
	Vector2(T _v = 0) { x = y = _v; Assert(!HasNaNs(), "Vector2 has NaN"); }
	Vector2(T _x, T _y) { x = _x; y = _y; Assert(!HasNaNs(), "Vector2 has NaN"); }

	bool HasNaNs() const {
		return isNaN(x) || isNaN(y);
	}

	T operator[](int i) const {
		Assert(0 <= i && i <= 1, "Access Violation");
		if (i == 0) return x;
		return y;
	}

	T &operator[](int i) {
		Assert(0 <= i && i <= 1, "Access Violation");
		if (i == 0) return x;
		return y;
	}

	bool operator ==(const Vector2<T> &v) const { return x == v.x && y == v.y; }
	bool operator !=(const Vector2<T> &v) const { return x != v.x || y != v.y; }
	Vector2<T> operator + (const Vector2<T> &v) const { return Vector2<T>(x + v.x, y + v.y); }
	Vector2<T> &operator+=(const Vector2<T> &v) {
		x += v.x;
		y += v.y;
		return *this;
	}
	Vector2<T> operator - (const Vector2<T> &v) const { return Vector2<T>(x - v.x, y - v.y, z - v.z); }
	Vector2<T> &operator-=(const Vector2<T> &v) {
		x -= v.x;
		y -= v.y;
		return *this;
	}
	template<typename U>
	Vector2<T> operator * (U f) const { return Vector2<T>(x*f, y*f); }
	template<typename U>
	Vector2<T> &operator *=(U f) {
		x *= f;
		y *= f;
		return *this;
	}
	template<typename U>
	Vector2<T> operator / (U f) const {
		Assert(f != 0, "Divide Zero");
		double inv = double(1) / f;
		return Vector2<T>(x*inv, y*inv);
	}
	template<typename U>
	Vector2<T> &operator /= (U f) {
		Assert(f != 0, "Divide Zero");
		double inv = double(1) / f;
		x *= inv;
		y *= inv;
		return *this;
	}
	Vector2<T> operator-() const { return Vector2<T>(-x, -y); }

	std::string toString(const int &spaceNum = 0) const {
		return indent(tfm::format("[ %.4f, %.4f ]", x, y), spaceNum);
	}

	template<typename U>
	friend Vector2<T> operator * (U f, Vector2<T> v) { return Vector2<T>(v.x*f, v.y*f); }

	friend std::ostream &operator<<(std::ostream &os, const Vector2<T> &v) {
		os << v.toString();
		return os;
	}

	T LengthSquare() const { return x * x + y * y; }
	T Length() const { return std::sqrt(LengthSquare()); }

	T x, y;
};

template<typename T>
inline T Dot(Vector2<T> u, Vector2<T> v) {
	return u.x*v.x + u.y*v.y;
}

template<typename T>
inline T Cross(Vector2<T> u, Vector2<T> v) {
	return u.x*v.y - v.x*u.y;
}

template<typename T>
inline Vector2<T> Normalize(Vector2<T> v) {
	return v / v.Length();
}

template<typename T>
class Vector3 {
public:
	Vector3(const T _v = 0) { x = y = z = _v; Assert(!HasNaNs(), "Vector3 has NaN"); }
	Vector3(const T _x, const T _y, const T _z) { x = _x; y = _y; z = _z; Assert(!HasNaNs(), "Vector3 has NaN"); }

	bool HasNaNs() const {
		return isNaN(x) || isNaN(y) || isNaN(z);
	}

	T operator[](int i) const {
		Assert(0 <= i && i <= 2, "Access Violation");
		if (i == 0) return x;
		if (i == 1) return y;
		return z;
	}

	T &operator[](int i) {
		Assert(0 <= i && i <= 2, "Access Violation");
		if (i == 0) return x;
		if (i == 1) return y;
		return z;
	}

	bool operator ==(const Vector3<T> &v) const { return x == v.x && y == v.y && z == v.z; }
	bool operator !=(const Vector3<T> &v) const { return x != v.x || y != v.y || z != v.z; }
	Vector3<T> operator + (const Vector3<T> &v) const { return Vector3<T>(x + v.x, y + v.y, z + v.z); }
	Vector3<T> &operator+=(const Vector3<T> &v) {
		x += v.x;
		y += v.y;
		z += v.z;
		return *this;
	}
	Vector3<T> operator - (const Vector3<T> &v) const { return Vector3<T>(x - v.x, y - v.y, z - v.z); }
	Vector3<T> operator - (const Normal3<T> &v) const { return Vector3<T>(x - v.x, y - v.y, z - v, z); }
	Vector3<T> &operator-=(const Vector3<T> &v) {
		x -= v.x;
		y -= v.y;
		z -= v.z;
		return *this;
	}
	template<typename U>
	Vector3<T> operator * (U f) const { return Vector3<T>(x*f, y*f, z*f); }
	template<typename U>
	Vector3<T> &operator *=(U f) {
		x *= f;
		y *= f;
		z *= f;
		return *this;
	}
	template<typename U>
	Vector3<T> operator / (U f) const {
		Assert(f != 0, "Divide Zero");
		Float inv = Float(1) / f;
		return Vector3<T>(x*inv, y*inv, z*inv);
	}
	template<typename U>
	Vector3<T> &operator /= (U f) {
		Assert(f != 0, "Divide Zero");
		double inv = double(1) / f;
		x *= inv;
		y *= inv;
		z *= inv;
		return *this;
	}
	Vector3<T> operator-() const { return Vector3<T>(-x, -y, -z); }

	std::string toString(const int &spaceNum = 0) const {
		return indent(tfm::format("[ %.4f, %.4f, %.4f ]", x, y, z), spaceNum);
	}

	template<typename T>
	Vector3<T> operator + (const Normal3<T> &v) const {
		return Vector3<T>(x + v.x, y + v.y, z + v.z);
	}

	template<typename U>
	friend Vector3<T> operator * (U f, Vector3<T> v) { return Vector3<T>(v.x*f, v.y*f, v.z*f); }


	friend std::ostream &operator<<(std::ostream &os, const Vector3<T> &v) {
		os << v.toString();
		return os;
	}

	T LengthSquare() const { return x * x + y * y + z * z; }
	T Length() const { return std::sqrt(LengthSquare()); }

	T x, y, z;
};

template<typename T>
inline T Dot(const Vector3<T> u, const Vector3<T> v) {
	return u.x*v.x + u.y*v.y + u.z*v.z;
}

template<typename T>
inline Vector3<T> Cross(const Vector3<T> u, const Vector3<T> v) {
	Float ux = u.x, uy = u.y, uz = u.z;
	Float vx = v.x, vy = v.y, vz = v.z;
	return Vector3<T>(
		uy * vz - uz * vy,
		uz * vx - ux * vz,
		ux * vy - uy * vx);
}

template<typename T>
inline Vector3<T> Normalize(const Vector3<T> &v) {
	return v / v.Length();
}
template<typename T>
inline Vector3<T> Normalize(const Point3<T> &p) {
	return Normalize(Vector3<T>(p));
}

template<typename T>
inline Vector3<T> Abs(const Vector3<T> &v) {
	return Vector3<T>(std::abs(v.x), std::abs(v.y), std::abs(v.z));
}

template<typename T>
inline int MaxDimension(const Vector3<T> &v) {
	if (v.x > v.y && v.x > v.z) return 0;
	if (v.y > v.z) return 1;
	return 2;
}

template<typename T>
inline Vector3<T> Permute(const Vector3<T> &v, const int &x, const int &y, const int &z) {
	return Vector3<T>(v[x], v[y], v[z]);
}

inline Vector3f toVector(const std::string &str) {
	Vector3f ret;
	char * endptr;
#ifdef __FLOAT_TYPE
	ret[0] = strtof(str.c_str(), &endptr);
	for (int i = 1; i < 3; i++){
		endptr++;
		ret[i] = strtof(endptr, &endptr);
	}
#else
	ret[0] = strtod(str.c_str(), &endptr);
	for (int i = 1; i < 3; i++) {
		endptr++;
		ret[i] = strtod(endptr, &endptr);
	}
#endif // __FLOAT_TYPE
	return ret;
	/*Float tmp[3];
	int num = 0;
	std::string basStr = "";
	for (size_t i = 0; i < str.length(); i++) {
		if (str[i] == ',') {
			tmp[num] = toFloat(basStr);
			num++;
			Assert(num < 3, "Can't convert " + str + " to Vector type");
			basStr = "";
		}
		else basStr += str[i];
	}
	tmp[2] = toFloat(basStr);
	return Vector3f(tmp[0], tmp[1], tmp[2]);*/
}

template<typename T>
class Point2 {
public:
	Point2(const T _v = 0) { x = y = _v; Assert(!HasNaNs(), "Point2 has NaN"); }
	Point2(const T _x, const T _y) :x(_x), y(_y) { Assert(!HasNaNs(), "Point2 has NaN"); }
	explicit Point2(const Point3<T> &u) :x(u.x), y(u.y) { Assert(!HasNaNs(), "Point2 has NaN"); }

	T operator [] (const int& i) const {
		Assert(0 <= i && i <= 1, "Access Violation!");
		if (i == 0) return x;
		else return y;
	}

	bool HasNaNs() { return isNaN(x) || isNaN(y); }

	Point2<T> operator + (const Point2<T>& u) const { return Point2<T>(x + u.x, y + u.y); }

	friend Point2<T> operator * (const T& u, const Point2<T>& v) { return Point2<T>(u*v.x, u*v.y); }

	std::string toString(const int &spaceNum = 0) const {
		return indent(tfm::format("[ %.4f, %.4f ]", x, y), spaceNum);
	}

	friend std::ostream &operator << (std::ostream &os, const Point2<T> u) {
		os << u.toString();
		return os;
	}

	T x, y;
};

template<typename T>
Point2<T> operator - (const Point2<T>& u,const Vector2<T>& v){
	return Point2<T>(u.x - v.x, u.y - v.y);
}

template<typename T>
class Point3 {
public:
	Point3(const T _v = 0) { x = y = z = _v; Assert(!HasNaNs(), "Point3 has NaN"); }
	Point3(const T _x, const T _y, const T _z) :x(_x), y(_y), z(_z) { Assert(!HasNaNs(), "Point3 has NaN"); }
	explicit Point3(const Normal3<T> &n) :x(T(n.x)), y(T(n.y)), z(T(n.z)) { Assert(!HasNaNs(), "Point3 has NaN"); }
	template<typename U> 
	explicit Point3(const Point3<U> &v) :x(T(v.x)), y(T(v.y)), z(T(v.z)) { Assert(!HasNaNs(), "Point3 has NaN"); }
	template<typename U> 
	explicit operator Vector3<U>() const {return Vector3<U>(x, y, z); }

	bool HasNaNs() { return isNaN(x) || isNaN(y) || isNaN(z); }
	
	T operator[](int i) const {
		Assert(0 <= i && i <= 2, "Access Violation");
			if (i == 0) return x;
		if (i == 1) return y;
		return z;
	}

	T &operator[](int i) {
		Assert(0 <= i && i <= 2, "Access Violation");
			if (i == 0) return x;
		if (i == 1) return y;
		return z;
	}

	bool operator == (const Point3<T> & u) const { return x == u.x && y == u.y && z == u.z; }
	Point3<T> operator + (const Vector3<T> &u) const { return Point3<T>(x + u.x, y + u.y, z + u.z); }
	Point3<T> &operator += (const Vector3<T> &u) { x += u.x; y += u.y; z += u.z; return *this; }
	Point3<T> operator + (const Point3<T> &u) const { return Point3<T>(x + u.x, y + u.y, z + u.z); }
	Point3<T> &operator += (const Point3<T> &u) { x += u.x; y += u.y; z += u.z; return *this; }
	Vector3<T> operator - (const Point3<T> &u) const { return Vector3<T>(x - u.x, y - u.y, z - u.z); }
	Point3<T> operator - (const Vector3<T> &u) const { return Point3<T>(x - u.x, y - u.y, z - u.z); }
	Point3<T> &operator -= (const Vector3<T> &u) { x -= u.x; y -= u.y; z -= u.z; return *this; }
	Point3<T> &operator -= (const Point3<T> &p) { x -= p.x; y -= p.y; z -= p.z; return *this; }

	template<typename U>
	Point3<T> operator * (const U f) const { return Point3<T>(x*f, y*f, z*f); }
	template<typename U>
	Point3<T> &operator *= (const U f) { x *= f; y *= f; z *= f; return *this; }
	template<typename U>
	Point3<T> operator / (const U &f) const { 
		Assert(f != 0, "Divide Zero");
		U inv = U(1) / f; 
		return Point3<T>(x, y, z) * inv;
	}
	template<typename U>
	Point3<T> &operator /= (const U &f) {
		Assert(f != 0, "Divide Zero");
		U inv = U(1) / f;
		x *= inv; 
		y *= inv;
		z *= inv;
		return *this;
	}

	std::string toString(const int &spaceNum = 0) const {
		return indent(tfm::format("[ %.4f, %.4f, %.4f ]", x, y, z), spaceNum);
	}

	template<typename U, typename T>
	friend Point3<T> operator * (const U& f, const Point3<T>& p) {
		return Point3<T>(f*p.x, f*p.y, f*p.z);
	}

	friend std::ostream &operator << (std::ostream &os, const Point3<T> u) {
		os << u.toString();
		return os;
	}

	T x, y, z;
};

template<typename T>
inline Point3<T> Min(const Point3<T> &p1, const Point3<T> &p2) {
	return Point3<T>(std::min(p1.x, p2.x),
                     std::min(p1.y, p2.y),
                     std::min(p1.z, p2.z));
}

template<typename T>
inline Point3<T> Max(const Point3<T> &p1, const Point3<T> &p2) {
	return Point3<T>(std::max(p1.x, p2.x),
                     std::max(p1.y, p2.y),
                     std::max(p1.z, p2.z));
}

template<typename T>
inline Float DistanceSquare(const Point3<T> &u, const Point3<T> &v) {
	return (u.x - v.x)*(u.x - v.x) + (u.y - v.y)*(u.y - v.y) + (u.z - v.z)*(u.z - v.z);
}

template<typename T>
inline Float Distance(const Point3<T> &u, const Point3<T> &v) {
	return std::sqrt(DistanceSquare(u, v));
}

template<typename T>
inline Point3<T> Lerp(const Point3<T> &u, const Point3<T> &v, Float t) {
	return u * (1 - t) + v * t;
}

template<typename T>
inline Point3<T> Permute(const Point3<T> &p, const int &x, const int &y, const int &z) {
	return Point3<T>(p[x], p[y], p[z]);
}

template<typename T>
class Normal3 {
public:
	Normal3(const T _v = 0) { x = y = z = _v; Assert(!HasNaNs(), "Normal3 has NaN"); }
	Normal3(const T _x, const T _y, const T _z) :x(_x), y(_y), z(_z) { Assert(!HasNaNs(), "Normal3 has NaN"); }
	explicit Normal3(const Vector3<T> &v) :x(v.x), y(v.y), z(v.z) { Assert(!HasNaNs(), "Normal3 has NaN"); }
	explicit Normal3(const Point3<T> &p) :x(p.x), y(p.y), z(p.z) { Assert(!HasNaNs(), "Normal3 has NaN"); }

	bool HasNaNs() const { return isNaN(x) || isNaN(y) || isNaN(z); }

	Normal3<T> operator + (const Normal3<T> &n) const { return Normal3<T>(x + n.x, y + n.y, z + n.z); }
	Normal3<T> operator - () const { return Normal3<T>(-x, -y, -z); }
	Normal3<T> operator * (const Float &u) const { return Normal3<T>(x*u, y*u, z*u); }
	Normal3<T> operator / (const Float &u) const { 
		Assert(u != 0, "Divide Zero!");
		Float inv = 1.0f / u;
		return Normal3<T>(x*inv, y*inv, z*inv);
	}

	Float Length() const { return std::sqrt(x*x + y * y + z * z); }

	std::string toString(const int &spaceNum = 0) const {
		return indent(tfm::format("[ %.4f, %.4f, %.4f ]", x, y, z), spaceNum);
	}

	template<typename U>
	friend Normal3<T> operator * (const U& f, const Normal3<T>& n) { return Normal3<T>(n.x*f, n.y*f, n.z*f); }	
	template<typename T>
	friend Vector3<T> operator - (const Normal3<T>& n, const Vector3<T>& v) { return Vector3<T>(n.x - v.x, n.y - v.y, n.z - v.z); }

	friend std::ostream &operator << (std::ostream &os, const Normal3<T> &u) {
		os << u.toString();
		return os;
	}

	T x, y, z;
};

template<typename T>
inline T Dot(const Normal3<T> &u, const Normal3<T> &v) {
	return u.x*v.x + u.y*v.y + u.z*v.z;
}

template<typename T>
inline T Dot(const Normal3<T> &u, const Vector3<T> &v) {
	return u.x*v.x + u.y*v.y + u.z*v.z;
}

template<typename T>
inline T Dot(const Vector3<T> &u, const Normal3<T> &v) {
	return u.x*v.x + u.y*v.y + u.z*v.z;
}

template<typename T>
inline T AbsDot(const Vector3<T> &u, const Normal3<T> &v) {
	return std::abs(Dot(u, v));
}

template<typename T>
inline Normal3<T> Cross(const Normal3<T> &u, const Normal3<T> &v) {
	double ux = u.x, uy = u.y, uz = u.z;
	double vx = v.x, vy = v.y, vz = v.z;
	return Normal3<T>(uy * vz - uz * vy,
                      uz * vx - ux * vz,
		              ux * vy - uy * vx);
}

template<typename T>
inline Vector3<T> Cross(const Normal3<T> &u, const Vector3<T> &v) {
	Float ux = u.x, uy = u.y, uz = u.z;
	Float vx = v.x, vy = v.y, vz = v.z;
	return Vector3<T>(uy * vz - uz * vy,
		uz * vx - ux * vz,
		ux * vy - uy * vx);
}

template<typename T>
inline Vector3<T> Cross(const Vector3<T> &u, const Normal3<T> &v) {
	Float ux = u.x, uy = u.y, uz = u.z;
	Float vx = v.x, vy = v.y, vz = v.z;
	return Vector3<T>(uy * vz - uz * vy,
		uz * vx - ux * vz,
		ux * vy - uy * vx);
}

template<typename T>
inline Normal3<T> Normalize(const Normal3<T>& n) {
	return n / n.Length();
}

template<typename T>
inline Normal3<T> FaceForward(const Normal3<T> &n, const Vector3<T> &v) {
	Float dot = Dot(n, v);
	if (dot != 0)
		return dot > 0 ? n : -n;
	dot = Dot(n + Normal3<T>(Epsilon, 0, 0), v);
	return dot > 0 ? n : -n;
}

template <typename T>
inline Normal3<T> FaceForward(const Normal3<T> &n, const Normal3<T> &n2) {
	return (Dot(n, n2) < 0.f) ? -n : n;
}

RAINBOW_NAMESPACE_END

#endif // !__VECTOR_H