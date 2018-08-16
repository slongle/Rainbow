#ifndef __TRANSFORM_H
#define __TRANSFORM_H

#include "common.h"

RAINBOW_NAMESPACE_BEGIN

class Matrix4x4 {
public:
	Matrix4x4() {
		m[0][0] = m[1][1] = m[2][2] = m[3][3] = 1;
		m[0][1] = m[0][2] = m[0][3] = m[1][0] = m[1][2] = m[1][3] = 
		m[2][0] = m[2][1] = m[2][3] = m[3][0] = m[3][1] = m[3][2] = 0;
	}

	Matrix4x4(Float mat[4][4]);
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

	static Matrix4x4 Mul(const Matrix4x4 &m1, const Matrix4x4 &m2) {
		Matrix4x4 ret;
		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				ret.m[i][j] = m1.m[i][0] * m2.m[0][j] +
				              m1.m[i][1] * m2.m[1][j] +
				              m1.m[i][2] * m2.m[2][j] +
                              m1.m[i][3] * m2.m[3][j];
		return ret;
	}

	friend Matrix4x4 Transpose(const Matrix4x4 &m1);
	friend Matrix4x4 Inverse(const Matrix4x4 &m1);

	friend std::ostream &operator << (std::ostream &os, const Matrix4x4 &m1) {
		os.setf(std::ios::fixed);				
		
		os << "Matrix[\n" << std::setprecision(4) <<
			  "  [" << m1.m[0][0] << ", " << m1.m[0][1] << ", " << m1.m[0][2] << ", " << m1.m[0][3] << "],\n" <<
			  "  [" << m1.m[1][0] << ", " << m1.m[1][1] << ", " << m1.m[1][2] << ", " << m1.m[1][3] << "],\n" <<
			  "  [" << m1.m[2][0] << ", " << m1.m[2][1] << ", " << m1.m[2][2] << ", " << m1.m[2][3] << "],\n" <<
			  "  [" << m1.m[3][0] << ", " << m1.m[3][1] << ", " << m1.m[3][2] << ", " << m1.m[3][3] << "],\n" <<
			  "]";
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

RAINBOW_NAMESPACE_END

#endif // !__TRANSFORM_H
