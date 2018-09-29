#include "gtest/gtest.h"
#include "../core/vector.h"

RAINBOW_NAMESPACE_BEGIN

TEST(Vector, Basic) {
	Vector3f a(1, 2, 3), b(3, 5, 2);
	EXPECT_EQ(Vector3f(4, 7, 5), a + b);
}

RAINBOW_NAMESPACE_END
