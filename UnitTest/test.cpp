#include "pch.h"
#include "../src/core/vector.h"

RAINBOW_NAMESPACE_BEGIN

TEST(VectorTest, Basic) {
	Vector3f a(3, 4, 2), b(6, 3, 9);
	EXPECT_EQ(Vector3f(9, 7, 11), a + b);
	EXPECT_TRUE(true);
}

RAINBOW_NAMESPACE_END