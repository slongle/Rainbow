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

	std::string toString(const int &spaceNum = 0) const {
		return
		indent("[\n", spaceNum) +
			indent(tfm::format("[%.3f, %.3f, %.3f, %.3f],\n", m[0][0], m[0][1], m[0][2], m[0][3]), spaceNum + 4) +
			indent(tfm::format("[%.3f, %.3f, %.3f, %.3f],\n", m[1][0], m[1][1], m[1][2], m[1][3]), spaceNum + 4) +
			indent(tfm::format("[%.3f, %.3f, %.3f, %.3f],\n", m[2][0], m[2][1], m[2][2], m[2][3]), spaceNum + 4) +
			indent(tfm::format("[%.3f, %.3f, %.3f, %.3f],\n", m[3][0], m[3][1], m[3][2], m[3][3]), spaceNum + 4) +
		indent("]", spaceNum);
	}

	friend Matrix4x4 Transpose(const Matrix4x4 &m1);
	friend Matrix4x4 Inverse(const Matrix4x4 &m1);

	friend std::ostream &operator << (std::ostream &os, const Matrix4x4 &m1) {
		os << m1.toString();

		/*os.setf(std::ios::fixed);						
		os << "Matrix[\n" << std::setprecision(4) <<
			  "  [" << m1.m[0][0] << "\t, " << m1.m[0][1] << "\t, " << m1.m[0][2] << "\t, " << m1.m[0][3] << "],\n" <<
			  "  [" << m1.m[1][0] << "\t, " << m1.m[1][1] << "\t, " << m1.m[1][2] << "\t, " << m1.m[1][3] << "],\n" <<
			  "  [" << m1.m[2][0] << "\t, " << m1.m[2][1] << "\t, " << m1.m[2][2] << "\t, " << m1.m[2][3] << "],\n" <<
			  "  [" << m1.m[3][0] << "\t, " << m1.m[3][1] << "\t, " << m1.m[3][2] << "\t, " << m1.m[3][3] << "],\n" <<
			  "]";*/
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
	Transform() { }
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

	Transform operator * (const Transform &t) const {
		return Transform(m * t.m, mInv * t.mInv);
	}

	Transform &operator *= (const Transform &t) {
		m = m * t.m;
		mInv = mInv * t.mInv;
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
	SurfaceInteraction operator() (const SurfaceInteraction &si) const;

	std::string toString(const int &spaceNum = 0) const {
		return 
		indent("[\n", spaceNum) +
			m.toString(spaceNum + 4) + "\n" +
			mInv.toString(spaceNum + 4) + "\n" +
		indent("]", spaceNum);		
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
Transform LookAt(const Vector3f &pos, const Vector3f &look, const Vector3f &up);
Transform Perspective(const Float &fov, const Float& near, const Float& far);

RAINBOW_NAMESPACE_END

#endif // !__TRANSFORM_H
