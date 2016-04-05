#include "gtest/gtest.h"

#include "Alpha.h"
#include "Rgb.h"
#include "Color.h"
#include "ColorCast.h"

#include <iostream>

TEST(Convert, color_cast_equivalent) {
    auto c1 = color::Rgb<uint8_t>(127, 127, 127);
    auto c2 = color::color_cast<uint8_t>(c1);

    ASSERT_EQ(c1, c2);

    auto c3 = color::Rgb<float>(0.25, 0.55, 0.85);
    auto c4 = color::color_cast<float>(c3);

    ASSERT_EQ(c3, c4);

    auto c5 = color::Alpha<uint8_t, color::Rgb>({100, 200, 255}, 50);
    auto c6 = color::color_cast<uint8_t>(c5);
}

TEST(Convert, color_cast_float_to_int) {
    auto c1 = color::Rgb<float>(0.25, 0.5, 0.75);
    auto c2 = color::color_cast<uint8_t>(c1);

    ASSERT_EQ(c2, color::Rgb<uint8_t>(63, 127, 191));
}

TEST(Convert, color_cast_int_to_int) {
    auto c1 = color::Rgb<uint8_t>(0x1, 0x80, 0xFF);

    ASSERT_EQ(color::color_cast<uint16_t>(c1), 
            color::Rgb<uint16_t>(0x101, 0x8080, 0xFFFF));
}
