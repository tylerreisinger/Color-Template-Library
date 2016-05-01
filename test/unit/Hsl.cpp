#include "gtest/gtest.h"

#include <array>

#include "Hsl.h"
#include "Color.h"

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

        ASSERT_EQ(c1, c2);
        ASSERT_EQ(c1, c3);
    }
    //Test tuple constructor
    {
        auto c = Hsl<float>(std::make_tuple(0.3, 0.4, 0.6));

        ASSERT_FLOAT_EQ(c.hue(), 0.3);
        ASSERT_FLOAT_EQ(c.saturation(), 0.4);
        ASSERT_FLOAT_EQ(c.lightness(), 0.6);
    }
}

TEST(Hsl, swap) {
    auto c1 = Hsl<float>(0.25, 0.5, 0.75);
    auto c2 = Hsl<float>(0.33, 0.69, 0.99);
    auto c3 = c1;
    auto c4 = c2;

    swap(c1, c2);

    ASSERT_TRUE(float_eq(c1, c4));
    ASSERT_TRUE(float_eq(c2, c3));
}

TEST(Hsl, broadcast) {
    auto c = Hsl<uint8_t>::broadcast(135);

    ASSERT_EQ(c, Hsl<uint8_t>(135, 135, 135)); 
}
