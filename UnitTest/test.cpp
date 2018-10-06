#include "pch.h"
#include "../src/core/vector.h"
#include "../src/shapes/triangle.h"
#include "../src/core/transform.h"
RAINBOW_NAMESPACE_BEGIN

TEST(VectorTest, Basic) {
	Vector3f a(3, 4, 2), b(6, 3, 9);
	EXPECT_EQ(Vector3f(9, 7, 11), a + b);
	EXPECT_TRUE(true);
}

TEST(TriangleTest, A) {
	Float fov = 30, nearClip = 1e-2, farClip = 1000;
	Transform a = Perspective(fov, nearClip, farClip);
	Float f = farClip, n = nearClip;
	Float invTanAng = 1 / std::tan(Radians(fov*0.5f));
	Matrix4x4 persp(
		invTanAng, 0, 0, 0,
		0, invTanAng, 0, 0,
		0, 0, f / (f - n), -f * n / (f - n),
		0, 0, 1, 0);
	Transform b(persp);
	EXPECT_EQ(a, b);
}


RAINBOW_NAMESPACE_END