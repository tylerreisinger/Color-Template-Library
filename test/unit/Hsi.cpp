#include "gtest/gtest.h"

#include "Hsi.h"
#include "ConversionRef.h"
#include "Assertions.h"

using namespace color;

TEST(Hsi, constructor) {
    // Test default constructor
    {
        auto c = Hsi<float>();

        ASSERT_COLORS_NEAR(c, Hsi<float>(0.0, 0.0, 0.0), 1e-5);
    }
    // Test component constructor
    {
        auto c = Hsi<float>(0.2, 0.6, 0.9);

        ASSERT_FLOAT_EQ(c.hue(), 0.2);
        ASSERT_FLOAT_EQ(c.saturation(), 0.6);
        ASSERT_FLOAT_EQ(c.intensity(), 0.9);
    }
    // Test data constructors
    {
        std::array<uint8_t, 3> data{100, 127, 222};

        auto c1 = Hsi<uint8_t>(data);
        auto c2 = Hsi<uint8_t>(data.data());
        auto c3 = Hsi<uint8_t>(data[0], data[1], data[2]);

        ASSERT_COLORS_EQ(c1, c2);
        ASSERT_COLORS_EQ(c1, c3);
    }
    // Test tuple constructor
    {
        auto c = Hsi<float>(std::make_tuple(0.3, 0.4, 0.6));

        ASSERT_FLOAT_EQ(c.hue(), 0.3);
        ASSERT_FLOAT_EQ(c.saturation(), 0.4);
        ASSERT_FLOAT_EQ(c.intensity(), 0.6);
    }
}

TEST(Hsi, swap) {
    auto c1 = Hsi<float>(0.25, 0.5, 0.75);
    auto c2 = Hsi<float>(0.33, 0.69, 0.99);
    auto c3 = c1;
    auto c4 = c2;
    swap(c1, c2);

    ASSERT_COLORS_NEAR(c1, c4, 1e-5);
    ASSERT_COLORS_NEAR(c2, c3, 1e-5);
}

TEST(Hsi, broadcast) {
    auto c = Hsi<float>::broadcast(0.4);

    ASSERT_COLORS_EQ(c, Hsi<float>(0.4, 0.4, 0.4));
}
