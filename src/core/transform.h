#ifndef __TRANSFORM_H
#define __TRANSFORM_H

#include "common.h"
#include "vector.h"

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

	Matrix4x4 operator * (const Matrix4x4 &m1) {
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
		return tfm::format("[\n\t[%7.3f, %7.3f, %7.3f, %7.3f],\n\t[%7.3f, %7.3f, %7.3f, %7.3f],\n\t[%7.3f, %7.3f, %7.3f, %7.3f],\n\t[%7.3f, %7.3f, %7.3f, %7.3f],\n]",
			m[0][0],m[0][1],m[0][2],m[0][3],
			m[1][0], m[1][1], m[1][2], m[1][3], 
			m[2][0], m[2][1], m[2][2], m[2][3], 
			m[3][0], m[3][1], m[3][2], m[3][3]);
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

	std::string toString() const {
		return tfm::format("[\nm = %s\nmInv = %s\n]", m, mInv);
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

Transform Translate(const Vector3f &delta);
Transform Scale(Float x, Float y, Float z);
Transform RotateX(Float theta);
Transform RotateY(Float theta);
Transform RotateZ(Float theta);

RAINBOW_NAMESPACE_END

#endif // !__TRANSFORM_H
