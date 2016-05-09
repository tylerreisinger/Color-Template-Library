#include "gtest/gtest.h"

#include "Alpha.h"
#include "Assertions.h"
#include "Rgb.h"
#include "Color.h"
#include "ColorCast.h"
#include "Hsv.h"

using namespace color;

TEST(Convert, color_cast_equivalent) {
    auto c1 = Rgb<uint8_t>(127, 127, 127);
    auto c2 = color_cast<uint8_t>(c1);

    ASSERT_COLORS_EQ(c1, c2);

    auto c3 = Rgb<float>(0.25, 0.55, 0.85);
    auto c4 = color_cast<float>(c3);

    ASSERT_COLORS_EQ(c3, c4);

    auto c5 = Alpha<uint8_t, Rgb>({100, 200, 255}, 50);
    auto c6 = color_cast<uint8_t>(c5);

    ASSERT_COLORS_EQ(c5, c6);
}

TEST(Convert, color_cast_float_to_int) {
    auto c1 = Rgb<float>(0.25, 0.5, 0.75);
    auto c2 = color_cast<uint8_t>(c1);

    ASSERT_COLORS_EQ(c2, Rgb<uint8_t>(63, 127, 191));
}

TEST(Convert, color_cast_int_to_int) {
    auto c1 = Rgb<uint8_t>(0x1, 0x80, 0xFF);

    ASSERT_COLORS_EQ(
            color_cast<uint16_t>(c1), Rgb<uint16_t>(0x101, 0x8080, 0xFFFF));
}

TEST(Convert, color_cast_hsv) {
    {
        auto c1 = Hsv<uint8_t>(255, 255, 255);
        auto c2 = color_cast<float>(c1);

        ASSERT_LT(c2.hue(), 1.0);
        ASSERT_GT(c2.hue(), 0.0);
    }
}
