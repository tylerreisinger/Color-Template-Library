#include "gtest/gtest.h"

#include "Rgb.h"
#include "Color.h"

using namespace color;

TEST(Float, float_eq) {
    auto c1 = Rgb<float>(0.2, 0.1999999999, 0.5);
    auto c2 = Rgb<float>(0.2, 0.2, 0.5);
    auto c3 = Rgb<float>(0.1, 0.2, 0.5);

    ASSERT_TRUE(float_eq(c1, c2, 1e-5f));
    ASSERT_FALSE(float_eq(c1, c3, 1e-5f));
}
