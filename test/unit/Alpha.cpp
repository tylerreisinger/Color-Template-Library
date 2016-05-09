#include "gtest/gtest.h"

#include "Rgb.h"
#include "Color.h"
#include "Alpha.h"
#include "Assertions.h"

#include <iostream>

using namespace color;

TEST(Alpha, default_constructor) {
    auto c1 = Rgba<float>();
    ASSERT_FLOAT_EQ(c1.color().red(), 0.0);
    ASSERT_FLOAT_EQ(c1.color().green(), 0.0);
    ASSERT_FLOAT_EQ(c1.color().blue(), 0.0);
    ASSERT_FLOAT_EQ(c1.alpha(), 0.0);
}

TEST(Alpha, explicit_constructor) {
    // Color + alpha constructor
    auto c1 = Rgba<uint8_t>({75, 150, 225}, 200);
    ASSERT_COLORS_EQ(c1.color(), Rgb<uint8_t>(75, 150, 225));
    ASSERT_EQ(c1.alpha(), 200);

    // Tuple constructor
    auto c2 = Alpha<uint8_t, Rgb>(std::make_tuple(25, 100, 200, 254));

    ASSERT_COLORS_EQ(c2.color(), Rgb<uint8_t>(25, 100, 200));
    ASSERT_EQ(c2.alpha(), 254);

    // Channel constructor
    auto c3 = Rgba<float>(0.2, 0.4, 0.6, 0.86);
    ASSERT_COLORS_NEAR(c3.color(), Rgb<float>(0.2, 0.4, 0.6), FLOAT_TOL);
    ASSERT_FLOAT_EQ(c3.alpha(), 0.86);
}

TEST(Alpha, clamp) {
    auto c1 = Rgba<uint8_t>(3, 129, 223, 250);
    c1 = c1.clamp(50, 200);

    ASSERT_COLORS_EQ(c1.color(), Rgb<uint8_t>(50, 129, 200));
    ASSERT_EQ(c1.alpha(), 200);

    auto c2 = Rgba<float>(0.03, 0.25, 0.55, 0.85);
    c2 = c2.clamp(0.20, 0.80);

    ASSERT_COLORS_NEAR(c2.color(), Rgb<float>(0.20, 0.25, 0.55), FLOAT_TOL);
    ASSERT_FLOAT_EQ(c2.alpha(), 0.80);
}

TEST(Alpha, normalize) {
    auto c1 = Rgba<float>(-0.2, 0.1, 1.0, 2.3);
    c1 = c1.normalize();

    ASSERT_COLORS_NEAR(c1.color(), Rgb<float>(0.0, 0.1, 1.0), FLOAT_TOL);
    ASSERT_FLOAT_EQ(c1.alpha(), 1.0);
}

TEST(Alpha, lerp) {
    auto c1 = Rgba<float>(0.2, 1.0, 0.5, 1.0);
    auto c2 = Rgba<float>(0.7, 1.0, 0.25, 0.0);

    auto c3 = c1.lerp(c2, 0.5);

    ASSERT_COLORS_NEAR(c3.color(), Rgb<float>(0.45, 1.0, 0.375), FLOAT_TOL);
    ASSERT_FLOAT_EQ(c3.alpha(), 0.5);
}

TEST(Alpha, as_tuple) {
    auto c1 = Rgba<uint8_t>(50, 103, 178, 249);

    ASSERT_EQ(c1.as_tuple(), std::make_tuple(50, 103, 178, 249));
}

TEST(Alpha, as_array) {
    auto c1 = Rgba<uint8_t>(50, 103, 178, 249);

    auto test_array = std::array<uint8_t, 4>{50, 103, 178, 249};

    ASSERT_EQ(c1.as_array(), test_array);
}

TEST(Alpha, alpha_blend) {
    {
        auto c1 = Rgba<float>(0.0, 0.0, 0.0, 1.0);
        auto c2 = Rgba<float>(1.0, 1.0, 1.0, 0.0);

        auto c3 = alpha_blend(c1, c2);

        ASSERT_FLOAT_EQ(c3.color().red(), 0.0);
        ASSERT_FLOAT_EQ(c3.alpha(), 1.0);

        auto c4 = alpha_blend(c2, c1);

        ASSERT_FLOAT_EQ(c4.color().red(), 0.0);
        ASSERT_FLOAT_EQ(c4.alpha(), 1.0);
    }


    {
        auto c1 = Rgba<float>(0.5, 0.0, 1.0, 0.5);
        auto c2 = Rgba<float>(0.2, 0.0, 0.0, 1.0);

        auto c3 = alpha_blend(c1, c2);
        ASSERT_COLORS_NEAR(c3, Rgba<float>(0.35, 0.0, 0.5, 1.0), FLOAT_TOL);
    }

    {
        auto c1 = Rgba<float>(0.3, 0.6, 0.5, 0.0);
        auto c2 = Rgba<float>(0.7, 0.23, 0.55, 0.0);

        auto c3 = alpha_blend(c1, c2);
        ASSERT_COLORS_NEAR(c3, Rgba<float>(0.0, 0.0, 0.0, 0.0), FLOAT_TOL);
    }

    {
        auto c1 = Rgba<float>(0.5, 1.0, 0.3, 0.75);
        auto c2 = Rgb<float>(1.0, 1.0, 1.0);

        auto c3 = alpha_blend(c1, c2);
        ASSERT_COLORS_NEAR(c3, Rgba<float>(0.625, 1.0, 0.475, 1.0), FLOAT_TOL);
    }

    {
        auto c1 = Rgba<uint8_t>(57, 128, 212, 0);
        auto c2 = Rgba<uint8_t>(20, 120, 217, 255);

        ASSERT_COLORS_EQ(alpha_blend(c1, c2), Rgba<uint8_t>(20, 120, 217, 255));
    }

    {
        auto c1 = Rgba<uint8_t>(100, 200, 0, 127);
        auto c2 = Rgba<uint8_t>(200, 100, 255, 255);

        ASSERT_COLORS_EQ(
                alpha_blend(c1, c2), Rgba<uint8_t>(150, 149, 127, 255));
    }
}

TEST(Alpha, swap) {
    auto c1 = Rgba<uint16_t>(1000, 55555, 22121, 0);
    auto c2 = Rgba<uint16_t>(50, 550, 5550, 55550);

    swap(c1, c2);

    ASSERT_COLORS_EQ(c1, Rgba<uint16_t>(50, 550, 5550, 55550));
    ASSERT_COLORS_EQ(c2, Rgba<uint16_t>(1000, 55555, 22121, 0));
}
