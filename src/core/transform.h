#ifndef __TRANSFORM_H
#define __TRANSFORM_H

#include "ray.h"
#include "bbox.h"
#include "interaction.h"

RAINBOW_NAMESPACE_BEGIN

class Matrix4x4 {
public:
	Matrix4x4() {
		m[0][0] = m[1][1] = m[2][2] = m[3][3] = 1;
		m[0][1] = m[0][2] = m[0][3] = m[1][0] = m[1][2] = m[1][3] = 
		m[2][0] = m[2][1] = m[2][3] = m[3][0] = m[3][1] = m[3][2] = 0;
	}

	Matrix4x4(const Float mat[4][4]);
	Matrix4x4(Float m00, Float m01, Float m02, Float m03,
		      Float m10, Float m11, Float m12, Float m13,
		      Float m20, Float m21, Float m22, Float m23,
		      Float m30, Float m31, Float m32, Float m33);

	void Identify();

	bool operator == (const Matrix4x4 &m1) const {
		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				if (m[i][j] != m1.m[i][j])
					return false;
		return true;
	}

	bool operator != (const Matrix4x4 &m1) const {
		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				if (m[i][j] != m1.m[i][j])
					return true;
		return false;
	}

	Float &operator [](const int index) {
		return m[index / 4][index % 4];
	}

	Matrix4x4 operator * (const Matrix4x4 &m1) const {
		Matrix4x4 ret;
		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				ret.m[i][j] = m[i][0] * m1.m[0][j] +
				              m[i][1] * m1.m[1][j] +
				              m[i][2] * m1.m[2][j] +
                              m[i][3] * m1.m[3][j];
		return ret;
	}

	std::string toString() const {
        return tfm::format(
            "[\n"
            "    [%.3f, %.3f, %.3f, %.3f],\n"
            "    [%.3f, %.3f, %.3f, %.3f],\n"
            "    [%.3f, %.3f, %.3f, %.3f],\n"
            "    [%.3f, %.3f, %.3f, %.3f],\n"
            "]"
            , m[0][0], m[0][1], m[0][2], m[0][3]
            , m[1][0], m[1][1], m[1][2], m[1][3]
            , m[2][0], m[2][1], m[2][2], m[2][3]
            , m[3][0], m[3][1], m[3][2], m[3][3]
        );	    	  	    
	}

	friend Matrix4x4 Transpose(const Matrix4x4 &m1);
	friend Matrix4x4 Inverse(const Matrix4x4 &m1);

	friend std::ostream &operator << (std::ostream &os, const Matrix4x4 &m1) {
		os << m1.toString();
		return os;
	}

	friend std::istream &operator >>(std::istream &os, Matrix4x4 &m1) {
		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				os >> m1.m[i][j];
		return os;
	}

	Float m[4][4];
};

Matrix4x4 toMatrix(const std::string &str);

class Transform {
public:
	Transform() { Identify(); }
	Transform(const Float mat[4][4]) {
		m = Matrix4x4(mat[0][0], mat[0][1], mat[0][2], mat[0][3],
			          mat[1][0], mat[1][1], mat[1][2], mat[1][3],
			          mat[2][0], mat[2][1], mat[2][2], mat[2][3],
			          mat[3][0], mat[3][1], mat[3][2], mat[3][3]);
		mInv = Inverse(m);
	}
	Transform(const Matrix4x4 &_m) :m(_m), mInv(Inverse(m)) {}
	Transform(const Matrix4x4 &_m, const Matrix4x4 &_mInv) :m(_m), mInv(_mInv) {}

	void Identify();

	bool operator == (const Transform& t) const {
		return m == t.m;
	}

	Transform operator * (const Transform &t) const {
		return Transform(m * t.m, t.mInv * mInv);
	}

	Transform &operator *= (const Transform &t) {
		m = m * t.m;
        mInv = t.mInv * mInv;
		return *this;
	}

	template<typename T>
	Point3<T> operator () (const Point3<T> &p) const;
	template<typename T>
	Vector3<T> operator () (const Vector3<T> &v) const;
	template<typename T>
	Normal3<T> operator () (const Normal3<T> &n) const;
	Ray operator() (const Ray &r) const;
	Bounds3f operator () (const Bounds3f &b) const;
    template <typename T>
    inline Point3<T> operator()(const Point3<T> &p, const Vector3<T> &pError,
        Vector3<T> *pTransError) const;
    SurfaceInteraction operator() (const SurfaceInteraction &si) const;


	std::string toString() const {
        return tfm::format(
            "Transform[\n"
            "    M    : %s,\n"
            "    MInv : %s,\n"
            "]"
            , indent(m.toString(), 11)
            , indent(mInv.toString(),11)
        );
	}

	friend Transform Inverse(const Transform &t) {
		return Transform(t.mInv, t.m);
	}

	friend Transform Transpose(const Transform &t) {
		return Transform(Transpose(t.m), Transpose(t.mInv));
	}

	friend std::ostream &operator << (std::ostream &os, const Transform &mat) {
		os << mat.toString();
		return os;
	}

private:
	Matrix4x4 m, mInv;
};

Transform Translate(const Float& x, const Float& y, const Float& z);
Transform Translate(const Vector3f &delta);
Transform Scale(const Vector3f &scale);
Transform Scale(Float x, Float y, Float z);
Transform RotateX(Float theta);
Transform RotateY(Float theta);
Transform RotateZ(Float theta);
Transform Rotate(Float theta, const Vector3f &axis);
Transform LookAt(const Vector3f &target, const Vector3f &origin, const Vector3f &up);
Transform Perspective(const Float &fov, const Float& near, const Float& far);

template <typename T>
inline Point3<T> Transform::operator()(const Point3<T> &pt, const Vector3<T> &ptError, Vector3<T> *absError) const {
    T x = pt.x, y = pt.y, z = pt.z;
    T xp = m.m[0][0] * x + m.m[0][1] * y + m.m[0][2] * z + m.m[0][3];
    T yp = m.m[1][0] * x + m.m[1][1] * y + m.m[1][2] * z + m.m[1][3];
    T zp = m.m[2][0] * x + m.m[2][1] * y + m.m[2][2] * z + m.m[2][3];
    T wp = m.m[3][0] * x + m.m[3][1] * y + m.m[3][2] * z + m.m[3][3];
    absError->x =
        (gamma(3) + (T)1) *
        (std::abs(m.m[0][0]) * ptError.x + std::abs(m.m[0][1]) * ptError.y +
            std::abs(m.m[0][2]) * ptError.z) +
        gamma(3) * (std::abs(m.m[0][0] * x) + std::abs(m.m[0][1] * y) +
            std::abs(m.m[0][2] * z) + std::abs(m.m[0][3]));
    absError->y =
        (gamma(3) + (T)1) *
        (std::abs(m.m[1][0]) * ptError.x + std::abs(m.m[1][1]) * ptError.y +
            std::abs(m.m[1][2]) * ptError.z) +
        gamma(3) * (std::abs(m.m[1][0] * x) + std::abs(m.m[1][1] * y) +
            std::abs(m.m[1][2] * z) + std::abs(m.m[1][3]));
    absError->z =
        (gamma(3) + (T)1) *
        (std::abs(m.m[2][0]) * ptError.x + std::abs(m.m[2][1]) * ptError.y +
            std::abs(m.m[2][2]) * ptError.z) +
        gamma(3) * (std::abs(m.m[2][0] * x) + std::abs(m.m[2][1] * y) +
            std::abs(m.m[2][2] * z) + std::abs(m.m[2][3]));
    Assert(wp != 0, "F?!");
    if (wp == 1.)
        return Point3<T>(xp, yp, zp);
    else
        return Point3<T>(xp, yp, zp) / wp;
}

RAINBOW_NAMESPACE_END

#endif // !__TRANSFORM_H
