#include "gtest/gtest.h"

#include <array>

#include "Rgb.h"
#include "Hsl.h"
#include "Color.h"
#include "ConversionRef.h"
#include "Assertions.h"

using namespace color;

TEST(Hsl, constructor) {
    //Test default constructor
    {
        auto c = Hsl<float>();

        ASSERT_FLOAT_EQ(c.hue(), 0.0);
        ASSERT_FLOAT_EQ(c.saturation(), 0.0);
        ASSERT_FLOAT_EQ(c.lightness(), 0.0);
    }
    //Test component constructor
    {
        auto c = Hsl<float>(0.2, 0.6, 0.9);

        ASSERT_FLOAT_EQ(c.hue(), 0.2);
        ASSERT_FLOAT_EQ(c.saturation(), 0.6);
        ASSERT_FLOAT_EQ(c.lightness(), 0.9);
    }
    //Test data constructors
    {
        std::array<uint8_t, 3> data{100, 127, 222};

        auto c1 = Hsl<uint8_t>(data);
        auto c2 = Hsl<uint8_t>(data.data());
        auto c3 = Hsl<uint8_t>(data[0], data[1], data[2]);

        ASSERT_COLORS_EQ(c1, c2);
        ASSERT_COLORS_EQ(c1, c3);
    }
    //Test tuple constructor
    {
        auto c = Hsl<float>(std::make_tuple(0.3, 0.4, 0.6));

        ASSERT_COLORS_NEAR(c, Hsl<float>(0.3, 0.4, 0.6), FLOAT_TOL);
    }
}

TEST(Hsl, swap) {
    auto c1 = Hsl<float>(0.25, 0.5, 0.75);
    auto c2 = Hsl<float>(0.33, 0.69, 0.99);
    auto c3 = c1;
    auto c4 = c2;

    swap(c1, c2);

    ASSERT_COLORS_NEAR(c1, c4, FLOAT_TOL);
    ASSERT_COLORS_NEAR(c2, c3, FLOAT_TOL);
}

TEST(Hsl, broadcast) {
    auto c = Hsl<uint8_t>::broadcast(135);

    ASSERT_COLORS_EQ(c, Hsl<uint8_t>(135, 135, 135));
}

TEST(Hsl, chroma) {
    for(int i = 0; i < ref_vals::HSL_TEST.size(); ++i) {
        const auto chroma = color::chroma(ref_vals::HSL_TEST[i]);
        const auto test_chroma = ref_vals::CHROMA_TEST[i];

        const float ERROR_TOL = 1e-3;

        ASSERT_TRUE(equal_within_error(chroma, test_chroma, ERROR_TOL));
    }
}
