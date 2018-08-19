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
					else if (ipiv[k] > 1) {
						Error("Singular matrix in MatrixInvert at Position 1");
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
		if (std::fabs(minv[icol][icol]) < Epsilon) {
			Error("Singular matrix in MatrixInvert at Position 2");
		}

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


RAINBOW_NAMESPACE_END

