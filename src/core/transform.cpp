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

void Matrix4x4::Identify() {
	m[0][0] = m[1][1] = m[2][2] = m[3][3] = 1;
	m[0][1] = m[0][2] = m[0][3] = m[1][0] = m[1][2] = m[1][3] =
	m[2][0] = m[2][1] = m[2][3] = m[3][0] = m[3][1] = m[3][2] = 0;
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
						Assert(ipiv[k] <= 1, "Singular matrix in MatrixInvert at Position 1");
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

		Assert(minv[icol][icol] != 0.f, "Singular matrix in MatrixInvert at Position 2");

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

Matrix4x4 toMatrix(const std::string & str) {
	Matrix4x4 ret;
	char *endptr;
#ifdef __FLOAT_TYPE
	ret[0] = strtof(str.c_str(), &endptr);
	for (int i = 1; i < 16; i++) {	    
        endptr++;
		ret[i] = strtof(endptr, &endptr);
	}
#else
	ret[0] = strtod(str.c_str(), &endptr);
	for (int i = 1; i < 16; i++) {
        endptr++;
		ret[i] = strtod(endptr, &endptr);
	}
#endif // __FLOAT_TYPE
	//std::cout << ret << std::endl;
	return ret;
}

const Transform Transform::identityTransform = Transform();

Transform Translate(const Float& x,const Float& y,const Float& z) {
	Matrix4x4 m(1, 0, 0, x,
		0, 1, 0, y,
		0, 0, 1, z,
		0, 0, 0, 1);
	Matrix4x4 mInv(1, 0, 0, -x,
		0, 1, 0, -y,
		0, 0, 1, -z,
		0, 0, 0, 1);
	return Transform(m, mInv);
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

Transform Scale(const Vector3f & scale) {
	Matrix4x4 m(scale.x, 0, 0, 0,
				0, scale.y, 0, 0,
				0, 0, scale.z, 0,
				0, 0, 0, 1);
	Matrix4x4 mInv(1 / scale.x, 0, 0, 0,
	               0, 1 / scale.y, 0, 0,
	               0, 0, 1 / scale.z, 0,
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
	Float sinTheta = std::sin(Radians(theta));
	Float cosTheta = std::cos(Radians(theta));
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
	Float sinTheta = std::sin(Radians(theta));
	Float cosTheta = std::cos(Radians(theta));
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
	Float sinTheta = std::sin(Radians(theta));
	Float cosTheta = std::cos(Radians(theta));
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
	Float sinTheta = std::sin(Radians(theta));
	Float cosTheta = std::cos(Radians(theta));
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

Transform LookAt(const Vector3f & target, const Vector3f & origin, const Vector3f & up) {
	Matrix4x4 cameraToWorld;
	cameraToWorld.m[0][3] = origin[0];
	cameraToWorld.m[1][3] = origin[1];
	cameraToWorld.m[2][3] = origin[2];
	cameraToWorld.m[3][3] = 1;

	Vector3f dir = Normalize(target - origin);
	//std::cout << dir << std::endl;
	Vector3f left = Normalize(Cross(up, dir));
	//std::cout << left << std::endl;
	Vector3f newUp = Cross(dir, left);
	//std::cout << newUp << std::endl;
	cameraToWorld.m[0][0] = left[0];
	cameraToWorld.m[1][0] = left[1];
	cameraToWorld.m[2][0] = left[2];
	cameraToWorld.m[3][0] = 0;

	cameraToWorld.m[0][1] = newUp[0];
	cameraToWorld.m[1][1] = newUp[1];
	cameraToWorld.m[2][1] = newUp[2];
	cameraToWorld.m[3][1] = 0;

	cameraToWorld.m[0][2] = dir[0];
	cameraToWorld.m[1][2] = dir[1];
	cameraToWorld.m[2][2] = dir[2];
	cameraToWorld.m[3][2] = 0;

	//std::cout << cameraToWorld << std::endl;

	return Transform(cameraToWorld,Inverse(cameraToWorld));
}

Transform Perspective(const Float & fov, const Float &n, const Float &f) {
	Matrix4x4 persp(
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, f / (f - n), -f * n / (f - n),
		0, 0, 1, 0);
	Float invTanAng = 1 / std::tan(Radians(fov*0.5f));
	return Scale(invTanAng, invTanAng, 1) *Transform(persp);
}


template<typename T>
Point3<T> Transform::operator()(const Point3<T> & p) const {
	T x = m.m[0][0] * p.x + m.m[0][1] * p.y + m.m[0][2] * p.z + m.m[0][3];
	T y = m.m[1][0] * p.x + m.m[1][1] * p.y + m.m[1][2] * p.z + m.m[1][3];
	T z = m.m[2][0] * p.x + m.m[2][1] * p.y + m.m[2][2] * p.z + m.m[2][3];
	T w = m.m[3][0] * p.x + m.m[3][1] * p.y + m.m[3][2] * p.z + m.m[3][3];
	Assert(w != 0, "Divide Zero");
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

void Transform::Identify() {
	m.Identify();
	mInv.Identify();
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
	const Transform &M = *this;
	Bounds3f ret(M(Point3f(bounds.pMin.x, bounds.pMin.y, bounds.pMin.z)));
	ret = Union(ret, M(Point3f(bounds.pMax.x, bounds.pMin.y, bounds.pMin.z)));
	ret = Union(ret, M(Point3f(bounds.pMin.x, bounds.pMax.y, bounds.pMin.z)));
	ret = Union(ret, M(Point3f(bounds.pMin.x, bounds.pMin.y, bounds.pMax.z)));
	ret = Union(ret, M(Point3f(bounds.pMin.x, bounds.pMax.y, bounds.pMax.z)));
	ret = Union(ret, M(Point3f(bounds.pMax.x, bounds.pMax.y, bounds.pMin.z)));
	ret = Union(ret, M(Point3f(bounds.pMax.x, bounds.pMin.y, bounds.pMax.z)));
	ret = Union(ret, M(Point3f(bounds.pMax.x, bounds.pMax.y, bounds.pMax.z)));
	return ret;

	/*
	Transforming Axis-Aligned Bounding Boxes
	by Jim Arvo
	from "Graphics Gems", Academic Press, 1990
	*/
	Point3f mn(m.m[0][3], m.m[1][3], m.m[2][3]);
	Point3f mx(m.m[0][3], m.m[1][3], m.m[2][3]);
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			Float a = m.m[i][j] * bounds.pMin[j];
			Float b = m.m[i][j] * bounds.pMax[j];
			mn[i] += std::min(a, b);
			mx[i] += std::max(a, b);
		}
	}

	Bounds3f ret1(mn, mx);
	Assert(ret1 == ret, "Transform Bounds3 Algorithm Error");
	return ret1;
}

SurfaceInteraction Transform::operator()(const SurfaceInteraction & si) const {
	SurfaceInteraction ret = si;
    ret.p = (*this)(si.p, si.pError, &ret.pError);
	ret.n = (*this)(si.n);
	ret.wo = (*this)(si.wo);
	return ret;
}

RAINBOW_NAMESPACE_END