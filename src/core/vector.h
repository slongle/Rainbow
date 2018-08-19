#ifndef __VECTOR_H
#define __VECTOR_H

#include <algorithm>
#include "common.h"

RAINBOW_NAMESPACE_BEGIN

template<typename T>
class Vector2 {
public:
	Vector2(T _v = 0) { x = y = _v; DCHECK(!HasNaNs()); }
	Vector2(T _x, T _y) { x = _x; y = _y; DCHECK(!HasNaNs()); }

	bool HasNaNs() const {
		return std::isnan(x) || std::isnan(y);
	}

	T operator[](int i) const {
		DCHECK(0 <= i && i <= 1);
		if (i == 0) return x;
		return y;
	}

	T &operator[](int i) {
		DCHECK(0 <= i && i <= 1);
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
	Vector2<T> operator - (const Vector2 &v) const { return Vector2<T>(x - v.x, y - v.y, z - v.z); }
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
		DCHECK(f == 0);
		double inv = double(1) / f;
		return Vector2<T>(x*inv, y*inv);
	}
	template<typename U>
	Vector2<T> &operator /= (U f) {
		DCHECK(f == 0);
		double inv = double(1) / f;
		x *= inv;
		y *= inv;
		return *this;
	}
	Vector2<T> operator-() const { return Vector2<T>(-x, -y); }

	std::string toString() const {
		return tfm::format("[ %6.4f, %6.4f ]", x, y);
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
T Dot(Vector2<T> u, Vector2<T> v) {
	return u.x*v.x + u.y*v.y;
}

template<typename T>
T Cross(Vector2<T> u, Vector2<T> v) {
	return u.x*v.y - v.x*u.y;
}

template<typename T>
Vector2<T> Normalize(Vector2<T> v) {
	return v / v.Length();
}

template<typename T>
class Vector3 {
public:
	Vector3(T _v = 0) { x = y = z = _v; DCHECK(!HasNaNs()); }
	Vector3(T _x, T _y, T _z) { x = _x; y = _y; z = _z; DCHECK(!HasNaNs()); }

	bool HasNaNs() const {
		return std::isnan(x) || std::isnan(y) || std::isnan(z);
	}

	T operator[](int i) const {
		DCHECK(0 <= i && i <= 2);
		if (i == 0) return x;
		if (i == 1) return y;
		return z;
	}

	T &operator[](int i) {
		DCHECK(0 <= i && i <= 2);
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
	Vector3<T> operator - (const Vector3 &v) const { return Vector3<T>(x - v.x, y - v.y, z - v.z); }
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
		DCHECK(f == 0);
		Float inv = Float(1) / f;
		return Vector3<T>(x*inv, y*inv, z*inv);
	}
	template<typename U>
	Vector3<T> &operator /= (U f) {
		DCHECK(f == 0);
		double inv = double(1) / f;
		x *= inv;
		y *= inv;
		z *= inv;
		return *this;
	}
	Vector3<T> operator-() const { return Vector3<T>(-x, -y, -z); }

	std::string toString() const {
		return tfm::format("[ %6.4f, %6.4f, %6.4f ]", x, y, z);
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
T Dot(const Vector3<T> u, const Vector3<T> v) {
	return u.x*v.x + u.y*v.y + u.z*v.z;
}

template<typename T>
Vector3<T> Cross(const Vector3<T> u, const Vector3<T> v) {
	double ux = u.x, uy = u.y, uz = u.z;
	double vx = v.x, vy = v.y, vz = v.z;
	return Vector3<T>(
		uy * vz - uz * vy,
		uz * vx - ux * vz,
		ux * vy - uy * vx);
}

template<typename T>
Vector3<T> Normalize(Vector3<T> v) {
	return v / v.Length();
}

template<typename T>
class Point2 {
public:
	Point2(T _v = 0) { x = y = _v; DCHECK(!HasNaNs()); }
	Point2(T _x, T _y) :x(_x), y(_y) { DCHECK(!HasNaNs()); }
	explicit Point2(const Point3<T> &u) :x(u.x), y(u.y) { DCHECK(!HasNaNs()); }

	bool HasNaNs() { return std::isnan(x) || std::isnan(y); }

	std::string toString() const {
		return tfm::format("[ %6.4f, %6.4f ]", x, y);
	}

	friend std::ostream &operator << (std::ostream &os, const Point2<T> u) {
		os << u.toString();
		return os;
	}

	T x, y;
};

template<typename T>
class Point3 {
public:
	Point3(T _v = 0) { x = y = z = _v; DCHECK(!HasNaNs()); }
	Point3(T _x, T _y, T _z) :x(_x), y(_y), z(_z) { DCHECK(!HasNaNs()); }	
	template<typename U> 
	explicit Point3(const Point3<U> &v) :x(T(v.x)), y(T(v.y)), z(T(v.z)) { DCHECK(!HasNaNs()); }
	template<typename U> 
	explicit operator Vector3<U>() const {return Vector3<U>(x, y, z); }

	bool HasNaNs() { return std::isnan(x) || std::isnan(y) || std::isnan(z); }
	
	Point3<T> operator + (const Vector3<T> &u) const { return Point3<T>(x + u.x, y + u.y, z + u.z); }
	Point3<T> &operator += (const Vector3<T> &u) { x += u.x; y += u.y; z += u.z; return *this; }
	Point3<T> operator + (const Point3<T> &u) const { return Point3<T>(x + u.x, y + u.y, z + u.z); }
	Point3<T> &operator += (const Point3<T> &u) { x += u.x; y += u.y; z += u.z; return *this; }
	Vector3<T> operator - (const Point3<T> &u) const { return Vector3<T>(x - u.x, y - u.y, z - u.x); }
	Point3<T> operator - (const Vector3<T> &u) const { return Point3<T>(x - u.x, y - u.y, z - u.z); }
	Point3<T> &operator -= (const Vector3<T> &u) { x -= u.x; y -= u.y; z -= u.z; return *this; }
	template<typename U>
	Point3<T> operator * (const U f) const { return Point3<T>(x*f, y*f, z*f); }

	std::string toString() const {
		return tfm::format("[ %6.4f, %6.4f, %6.4f ]", x, y, z);
	}

	friend std::ostream &operator << (std::ostream &os, const Point3<T> u) {
		os << u.toString();
		return os;
	}

	T x, y, z;
};

template<typename T>
Float DistanceSquare(const Point3<T> &u, const Point3<T> &v) {
	return (u.x - v.x)*(u.x - v.x) + (u.y - v.y)*(u.y - v.y) + (u.z - v.z)*(u.z - v.z);
}

template<typename T>
Float Distance(const Point3<T> &u, const Point3<T> &v) {
	return std::sqrt(DistanceSquare(u, v));
}

template<typename T>
Point3<T> Lerp(const Point3<T> &u, const Point3<T> &v, Float t) {
	return u * (1 - t) + v * t;
}

template<typename T>
class Normal3 {
public:
	Normal3(T _v = 0) { x = y = z = _v; DCHECK(!HasNaNs()); }
	Normal3(T _x, T _y, T _z) :x(_x), y(_y), z(_z) { DCHECK(!HasNaNs()); }
	explicit Normal3(const Vector3<T> &v) :x(v.x), y(v.y), z(v.z) { DCHECK(!HasNaNs()); }

	bool HasNaNs() const { return std::isnan(x) || std::isnan(y) || std::isnan(z); }

	std::string toString() const {
		return tfm::format("[ %6.4f, %6.4f, %6.4f ]", x, y, z);
	}

	friend std::ostream &operator << (std::ostream &os, const Normal3<T> &u) {
		os << u.toString();
		return os;
	}

	T x, y, z;
};

template<typename T>
T Dot(const Normal3<T> &u, const Normal3<T> &v) {
	return u.x*v.x + u.y*v.y + u.z*v.z;
}

template<typename T>
Normal3<T> Cross(const Normal3<T> &u, const Normal3<T> &v) {
	double ux = u.x, uy = u.y, uz = u.z;
	double vx = v.x, vy = v.y, vz = v.z;
	return Normal3<T>(uy * vz - uz * vy,
                      uz * vx - ux * vz,
		              ux * vy - uy * vx);
}

template<typename T>
Vector3<T> Cross(const Normal3<T> &u, const Vector3<T> &v) {
	Float ux = u.x, uy = u.y, uz = u.z;
	Float vx = v.x, vy = v.y, vz = v.z;
	return Vector3<T>(uy * vz - uz * vy,
		uz * vx - ux * vz,
		ux * vy - uy * vx);
}

template<typename T>
Vector3<T> Cross(const Vector3<T> &u, const Normal3<T> &v) {
	Float ux = u.x, uy = u.y, uz = u.z;
	Float vx = v.x, vy = v.y, vz = v.z;
	return Vector3<T>(uy * vz - uz * vy,
		uz * vx - ux * vz,
		ux * vy - uy * vx);
}

template<typename T>
Normal3<T> FaceForward(const Normal3<T> &n, const Vector3<T> &u) {
	return Dot(n, v) > 0 ? n : -n;
}

RAINBOW_NAMESPACE_END

#endif // !__VECTOR_H