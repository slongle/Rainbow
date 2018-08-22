#include "transform.h"

RAINBOW_NAMESPACE_BEGIN

Matrix4x4::Matrix4x4(const Float mat[4][4]) {
	memcpy(m, mat, 16 * sizeof(Float));
}

Matrix4x4::Matrix4x4(Float m00, Float m01, Float m02, Float m03, 
	                 Float m10, Float m11, Float m12, Float m13, 
	                 Float m20, Float m21, Float m22, Float m23, 
	                 Float m30, Float m31, Float m32, Float m33){
	m[0][0] = m00; m[0][1] = m01; m[0][2] = m02; m[0][3] = m03;
	m[1][0] = m10; m[1][1] = m11; m[1][2] = m12; m[1][3] = m13;
	m[2][0] = m20; m[2][1] = m21; m[2][2] = m22; m[2][3] = m23;
	m[3][0] = m30; m[3][1] = m31; m[3][2] = m32; m[3][3] = m33;
}

Matrix4x4 Transpose(const Matrix4x4 &m1) {
	return Matrix4x4(m1.m[0][0], m1.m[1][0], m1.m[2][0], m1.m[3][0],
		             m1.m[0][1], m1.m[1][1], m1.m[2][1], m1.m[3][1],
		             m1.m[0][2], m1.m[1][2], m1.m[2][2], m1.m[3][2],
		             m1.m[0][3], m1.m[1][3], m1.m[2][3], m1.m[3][3]);
}

Matrix4x4 Inverse(const Matrix4x4 &m1) {
	int indxc[4], indxr[4];
	int ipiv[4] = { 0, 0, 0, 0 };
	Float minv[4][4];
	memcpy(minv, m1.m, 4 * 4 * sizeof(Float));
	for (int i = 0; i < 4; i++) {
		int irow = 0, icol = 0;
		Float big = 0.f;
		// Choose pivot
		for (int j = 0; j < 4; j++) {
			if (ipiv[j] != 1) {
				for (int k = 0; k < 4; k++) {
					if (ipiv[k] == 0) {
						if (std::abs(minv[j][k]) >= big) {
							big = Float(std::abs(minv[j][k]));
							irow = j;
							icol = k;
						}
					}
					else {
						DCHECK(ipiv[k] <= 1, "Singular matrix in MatrixInvert at Position 1")
					}
				}
			}
		}
		++ipiv[icol];
		// Swap rows _irow_ and _icol_ for pivot
		if (irow != icol) {
			for (int k = 0; k < 4; ++k) 
				std::swap(minv[irow][k], minv[icol][k]);
		}
		indxr[i] = irow;
		indxc[i] = icol;		
		
		DCHECK(std::fabs(minv[icol][icol]) > Epsilon, "Singular matrix in MatrixInvert at Position 2")

		// Set $m[icol][icol]$ to one by scaling row _icol_ appropriately
		Float pivinv = Float(1) / minv[icol][icol];
		minv[icol][icol] = 1.;
		for (int j = 0; j < 4; j++) 
			minv[icol][j] *= pivinv;

		// Subtract this row from others to zero out their columns
		for (int j = 0; j < 4; j++) {
			if (j != icol) {
				Float save = minv[j][icol];
				minv[j][icol] = 0;
				for (int k = 0; k < 4; k++) 
					minv[j][k] -= minv[icol][k] * save;
			}
		}
	}
	// Swap columns to reflect permutation
	for (int j = 3; j >= 0; j--) {
		if (indxr[j] != indxc[j]) {
			for (int k = 0; k < 4; k++)
				std::swap(minv[k][indxr[j]], minv[k][indxc[j]]);
		}
	}
	return Matrix4x4(minv);
}

Transform Translate(const Vector3f & delta) {
	Matrix4x4 m(1, 0, 0, delta.x,
		        0, 1, 0, delta.y,
		        0, 0, 1, delta.z,
		        0, 0, 0, 1);
	Matrix4x4 mInv(1, 0, 0, -delta.x,
		           0, 1, 0, -delta.y,
		           0, 0, 1, -delta.z,
		           0, 0, 0, 1);
	return Transform(m, mInv);
}

Transform Scale(Float x, Float y, Float z) {
	Matrix4x4 m(x, 0, 0, 0,
		        0, y, 0, 0,
		        0, 0, z, 0,
		        0, 0, 0, 1);
	Matrix4x4 mInv(1 / x, 0, 0, 0,
		           0, 1 / y, 0, 0,
		           0, 0, 1 / z, 0,
		           0, 0, 0, 1);
	return Transform(m, mInv);
}

Transform RotateX(Float theta) {
	Float sinTheta = std::sin(radToDeg(theta));
	Float cosTheta = std::cos(radToDeg(theta));
	Matrix4x4 m(1, 0, 0, 0,
		        0, cosTheta, -sinTheta, 0,
		        0, sinTheta, cosTheta, 0,
		        0, 0, 0, 1);
	Matrix4x4 mInv(1, 0, 0, 0,
		           0, cosTheta, sinTheta, 0,
		           0, -sinTheta, cosTheta, 0,
		           0, 0, 0, 1);
	return Transform(m, mInv);
}

Transform RotateY(Float theta) {
	Float sinTheta = std::sin(radToDeg(theta));
	Float cosTheta = std::cos(radToDeg(theta));
	Matrix4x4 m(cosTheta, 0,  sinTheta, 0,
		               0, 1,         0, 0,
		       -sinTheta, 0,  cosTheta, 0,
		               0, 0,         0, 1);
	Matrix4x4 mInv(cosTheta, 0, -sinTheta, 0,
		           0, 1, 0, 0,
		           sinTheta, 0, cosTheta, 0,
		           0, 0, 0, 1);
	return Transform(m, mInv);
}

Transform RotateZ(Float theta) {
	Float sinTheta = std::sin(radToDeg(theta));
	Float cosTheta = std::cos(radToDeg(theta));
	Matrix4x4 m(cosTheta, -sinTheta, 0, 0,
		        sinTheta, cosTheta, 0, 0,
		        0, 0, 1, 0,
		        0, 0, 0, 1);
	Matrix4x4 mInv(1, 0, 0, 0,
		0, cosTheta, sinTheta, 0,
		0, -sinTheta, cosTheta, 0,
		0, 0, 0, 1);
	return Transform(m, mInv);
}

Transform Rotate(Float theta, const Vector3f & axis){
	Vector3f a = Normalize(axis);
	Float sinTheta = std::sin(radToDeg(theta));
	Float cosTheta = std::cos(radToDeg(theta));
	Matrix4x4 m;
	// Compute rotation of first basis vector
	m.m[0][0] = a.x * a.x + (1 - a.x * a.x) * cosTheta;
	m.m[0][1] = a.x * a.y * (1 - cosTheta) - a.z * sinTheta;
	m.m[0][2] = a.x * a.z * (1 - cosTheta) + a.y * sinTheta;
	m.m[0][3] = 0;

	// Compute rotations of second basis vector
	m.m[1][0] = a.x * a.y * (1 - cosTheta) + a.z * sinTheta;
	m.m[1][1] = a.y * a.y + (1 - a.y * a.y) * cosTheta;
	m.m[1][2] = a.y * a.z * (1 - cosTheta) - a.x * sinTheta;
	m.m[1][3] = 0;

	// Compute rotations of third basis vector
	m.m[2][0] = a.x * a.z * (1 - cosTheta) - a.y * sinTheta;
	m.m[2][1] = a.y * a.z * (1 - cosTheta) + a.x * sinTheta;
	m.m[2][2] = a.z * a.z + (1 - a.z * a.z) * cosTheta;
	m.m[2][3] = 0;
	return Transform(m, Transpose(m));
}


template<typename T>
Point3<T> Transform::operator()(const Point3<T> & p) const {
	T x = m.m[0][0] * p.x + m.m[0][1] * p.y + m.m[0][2] * p.z + m.m[0][3];
	T y = m.m[1][0] * p.x + m.m[1][1] * p.y + m.m[1][2] * p.z + m.m[1][3];
	T z = m.m[2][0] * p.x + m.m[2][1] * p.y + m.m[2][2] * p.z + m.m[2][3];
	T w = m.m[3][0] * p.x + m.m[3][1] * p.y + m.m[3][2] * p.z + m.m[3][3];
	DCHECK(w != 0, "Divide Zero")
	if (w == 1)
		return Point3<T>(x, y, z);
	else
		return Point3<T>(x, y, z) / w;
}

template<typename T>
Vector3<T> Transform::operator()(const Vector3<T> & v) const {
	return Vector3<T>(m.m[0][0] * v.x + m.m[0][1] * v.y + m.m[0][2] * v.z,
		              m.m[1][0] * v.x + m.m[1][1] * v.y + m.m[1][2] * v.z,
		              m.m[2][0] * v.x + m.m[2][1] * v.y + m.m[2][2] * v.z);
}

template<typename T>
Normal3<T> Transform::operator()(const Normal3<T>& n) const {
	return Normal3<T>(mInv.m[0][0] * n.x + mInv.m[1][0] * n.y + mInv.m[2][0] * n.z,
		              mInv.m[0][1] * n.x + mInv.m[1][1] * n.y + mInv.m[2][1] * n.z,
		              mInv.m[0][2] * n.x + mInv.m[1][2] * n.y + mInv.m[2][2] * n.z);
}

Ray Transform::operator()(const Ray & r) const {
	Point3f o = (*this)(r.o);
	Vector3f d = (*this)(r.d);
	return Ray(o, d, r.tMax);
}

Bounds3f Transform::operator()(const Bounds3f & bounds) const {
	/*
	Transforming Axis-Aligned Bounding Boxes
	by Transforming 8 Points
	*/
	/*const Transform &t = (*this);
	Bounds3f ret(t(Point3f(bounds.pMin.x,bounds.pMin.y,bounds.pMin.z)));
	ret = Union(ret, t(Point3f(bounds.pMin.x, bounds.pMin.y, bounds.pMax.z)));	
	ret = Union(ret, t(Point3f(bounds.pMin.x, bounds.pMax.y, bounds.pMin.z)));
	ret = Union(ret, t(Point3f(bounds.pMin.x, bounds.pMax.y, bounds.pMax.z)));
	ret = Union(ret, t(Point3f(bounds.pMax.x, bounds.pMin.y, bounds.pMin.z)));
	ret = Union(ret, t(Point3f(bounds.pMax.x, bounds.pMin.y, bounds.pMax.z)));
	ret = Union(ret, t(Point3f(bounds.pMax.x, bounds.pMax.y, bounds.pMin.z)));
	ret = Union(ret, t(Point3f(bounds.pMax.x, bounds.pMax.y, bounds.pMax.z)));*/


	const Transform &M = *this;
	Bounds3f ret(M(Point3f(bounds.pMin.x, bounds.pMin.y, bounds.pMin.z)));
	ret = Union(ret, M(Point3f(bounds.pMax.x, bounds.pMin.y, bounds.pMin.z)));
	ret = Union(ret, M(Point3f(bounds.pMin.x, bounds.pMax.y, bounds.pMin.z)));
	ret = Union(ret, M(Point3f(bounds.pMin.x, bounds.pMin.y, bounds.pMax.z)));
	ret = Union(ret, M(Point3f(bounds.pMin.x, bounds.pMax.y, bounds.pMax.z)));
	ret = Union(ret, M(Point3f(bounds.pMax.x, bounds.pMax.y, bounds.pMin.z)));
	ret = Union(ret, M(Point3f(bounds.pMax.x, bounds.pMin.y, bounds.pMax.z)));
	ret = Union(ret, M(Point3f(bounds.pMax.x, bounds.pMax.y, bounds.pMax.z)));

	//return ret;

	/*
	Transforming Axis-Aligned Bounding Boxes
	by Jim Arvo
	from "Graphics Gems", Academic Press, 1990
	*/
	
	Point3f mn(m.m[0][3], m.m[1][3], m.m[2][3]);
	Point3f mx(m.m[0][3], m.m[1][3], m.m[2][3]);
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			Float a = m.m[j][i] * bounds.pMin[i];
			Float b = m.m[j][i] * bounds.pMax[i];
			mn[j] += std::min(a, b);
			mx[j] += std::max(a, b);
		}
	}
	Bounds3f ret1(mn, mx);
	DCHECK(ret1 == ret, "Transform Bounds3 Algorithm Error")
	return ret1;
}

RAINBOW_NAMESPACE_END