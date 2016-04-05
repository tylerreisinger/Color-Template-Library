#include "gtest/gtest.h"

#include "Rgb.h"
#include "Color.h"
#include "Alpha.h"

#include <iostream>

TEST(Alpha, default_constructor) {
    auto c1 = color::Rgba<float>();
    ASSERT_FLOAT_EQ(c1.color().red(), 0.0);
    ASSERT_FLOAT_EQ(c1.color().green(), 0.0);
    ASSERT_FLOAT_EQ(c1.color().blue(), 0.0);
    ASSERT_FLOAT_EQ(c1.alpha(), 0.0);
}

TEST(Alpha, explicit_constructor) {
    // Color + alpha constructor
    auto c1 = color::Rgba<uint8_t>({75, 150, 225}, 200);
    ASSERT_EQ(c1.color(), color::Rgb<uint8_t>(75, 150, 225));
    ASSERT_EQ(c1.alpha(), 200);

    // Tuple constructor
    auto c2 = color::Alpha<uint8_t, color::Rgb>(
            std::make_tuple(25, 100, 200, 254));

    ASSERT_EQ(c2.color(), color::Rgb<uint8_t>(25, 100, 200));
    ASSERT_EQ(c2.alpha(), 254);

    // Channel constructor
    auto c3 = color::Rgba<float>(0.2, 0.4, 0.6, 0.86);
    ASSERT_EQ(c3.color(), color::Rgb<float>(0.2, 0.4, 0.6));
    ASSERT_FLOAT_EQ(c3.alpha(), 0.86);
}

TEST(Alpha, clamp) {
    auto c1 = color::Rgba<uint8_t>(3, 129, 223, 250);
    c1 = c1.clamp(50, 200);

    ASSERT_EQ(c1.color(), color::Rgb<uint8_t>(50, 129, 200));
    ASSERT_EQ(c1.alpha(), 200);

    auto c2 = color::Rgba<float>(0.03, 0.25, 0.55, 0.85);
    c2 = c2.clamp(0.20, 0.80);

    ASSERT_FLOAT_EQ(c2.color().red(), 0.20);
    ASSERT_FLOAT_EQ(c2.color().green(), 0.25);
    ASSERT_FLOAT_EQ(c2.color().blue(), 0.55);
    ASSERT_FLOAT_EQ(c2.alpha(), 0.80);
}

TEST(Alpha, normalize) {
    auto c1 = color::Rgba<float>(-0.2, 0.1, 1.0, 2.3);
    c1 = c1.normalize();

    ASSERT_FLOAT_EQ(c1.color().red(), 0.0);
    ASSERT_FLOAT_EQ(c1.color().green(), 0.1);
    ASSERT_FLOAT_EQ(c1.color().blue(), 1.0);
    ASSERT_FLOAT_EQ(c1.alpha(), 1.0);
}

TEST(Alpha, lerp) {
    auto c1 = color::Rgba<float>(0.2, 1.0, 0.5, 1.0);
    auto c2 = color::Rgba<float>(0.7, 1.0, 0.25, 0.0);

    auto c3 = c1.lerp(c2, 0.5);

    ASSERT_FLOAT_EQ(c3.color().red(), 0.45);
    ASSERT_FLOAT_EQ(c3.color().green(), 1.0);
    ASSERT_FLOAT_EQ(c3.color().blue(), 0.375);
    ASSERT_FLOAT_EQ(c3.alpha(), 0.5);
}

TEST(Alpha, as_tuple) {
    auto c1 = color::Rgba<uint8_t>(50, 103, 178, 249);

    ASSERT_EQ(c1.as_tuple(), std::make_tuple(50, 103, 178, 249));
}

TEST(Alpha, as_array) {
    auto c1 = color::Rgba<uint8_t>(50, 103, 178, 249);

    auto test_array = std::array<uint8_t, 4>{50, 103, 178, 249};

    ASSERT_EQ(c1.as_array(), test_array);
}

TEST(Alpha, alpha_blend) {
    {
        auto c1 = color::Rgba<float>(0.0, 0.0, 0.0, 1.0);
        auto c2 = color::Rgba<float>(1.0, 1.0, 1.0, 0.0);

        auto c3 = alpha_blend(c1, c2);

        ASSERT_FLOAT_EQ(c3.color().red(), 0.0);
        ASSERT_FLOAT_EQ(c3.alpha(), 1.0);

        auto c4 = alpha_blend(c2, c1);

        ASSERT_FLOAT_EQ(c4.color().red(), 0.0);
        ASSERT_FLOAT_EQ(c4.alpha(), 1.0);
    }


    {
        auto c1 = color::Rgba<float>(0.5, 0.0, 1.0, 0.5);
        auto c2 = color::Rgba<float>(0.2, 0.0, 0.0, 1.0);

        auto c3 = alpha_blend(c1, c2);
        ASSERT_FLOAT_EQ(c3.color().red(), 0.35);
        ASSERT_FLOAT_EQ(c3.color().green(), 0.0);
        ASSERT_FLOAT_EQ(c3.color().blue(), 0.5);
        ASSERT_FLOAT_EQ(c3.alpha(), 1.0);
    }

    {
        auto c1 = color::Rgba<float>(0.3, 0.6, 0.5, 0.0);
        auto c2 = color::Rgba<float>(0.7, 0.23, 0.55, 0.0);

        auto c3 = alpha_blend(c1, c2);
        ASSERT_FLOAT_EQ(c3.alpha(), 0.0);
        ASSERT_FLOAT_EQ(c3.color().red(), 0.0);
        ASSERT_FLOAT_EQ(c3.color().green(), 0.0);
        ASSERT_FLOAT_EQ(c3.color().blue(), 0.0);
    }

    {
        auto c1 = color::Rgba<float>(0.5, 1.0, 0.3, 0.75);
        auto c2 = color::Rgb<float>(1.0, 1.0, 1.0);

        auto c3 = alpha_blend(c1, c2);
        ASSERT_FLOAT_EQ(c3.color().red(), 0.625);
        ASSERT_FLOAT_EQ(c3.color().green(), 1.0);
        ASSERT_FLOAT_EQ(c3.color().blue(), 0.475);
        ASSERT_FLOAT_EQ(c3.alpha(), 1.0);
    }

    {
        auto c1 = color::Rgba<uint8_t>(57, 128, 212, 0);
        auto c2 = color::Rgba<uint8_t>(20, 120, 217, 255);

        ASSERT_EQ(alpha_blend(c1, c2), color::Rgba<uint8_t>(20, 120, 217, 255));
    }

    {
        auto c1 = color::Rgba<uint8_t>(100, 200, 0, 127);
        auto c2 = color::Rgba<uint8_t>(200, 100, 255, 255);

        ASSERT_EQ(
                alpha_blend(c1, c2), color::Rgba<uint8_t>(150, 149, 127, 255));
    }
}

