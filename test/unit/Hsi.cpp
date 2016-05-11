#include "gtest/gtest.h"

#include <random>
#include <chrono>

#include "Hsi.h"
#include "Rgb.h"
#include "ConversionRef.h"
#include "Assertions.h"

using namespace color;

template <typename ComponentType>
::testing::AssertionResult test_random_conversions_in_gamut(
        ComponentType error_tol, int test_count);

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

    ASSERT_COLORS_EQ(c1, c4);
    ASSERT_COLORS_EQ(c2, c3);
}

TEST(Hsi, broadcast) {
    auto c = Hsi<float>::broadcast(0.4);

    ASSERT_COLORS_EQ(c, Hsi<float>(0.4, 0.4, 0.4));
}

TEST(Hsi, is_in_gamut) {
    ASSERT_TRUE(Hsi<float>(0.0, 1.0, 0.0).is_in_gamut());
    ASSERT_FALSE(Hsi<float>(0.0, 1.0, 0.4).is_in_gamut());
    ASSERT_TRUE(Hsi<float>(1.0 / 6.0, 1.0, 0.4).is_in_gamut());
    ASSERT_TRUE(Hsi<float>(0.25, 0.0, 1.0).is_in_gamut());
    ASSERT_FALSE(Hsi<float>(0.25, 0.2, 1.0).is_in_gamut());
    ASSERT_TRUE(Hsi<float>(1.0 / 6.0, 1.0, 0.66).is_in_gamut());
    ASSERT_FALSE(Hsi<float>(1.0 / 6.0, 1.0, 0.67).is_in_gamut());
    ASSERT_TRUE(Hsi<float>(1.0 / 3.0, 1.0, 0.30).is_in_gamut());
    ASSERT_FALSE(Hsi<float>(1.0 / 3.0, 1.0, 0.35).is_in_gamut());

    ASSERT_TRUE(test_random_conversions_in_gamut<float>(0.015, 500));
}

template <typename ComponentType>
::testing::AssertionResult test_random_conversions_in_gamut(
        ComponentType error_tol, int test_count) {
    auto rng_seed = std::chrono::system_clock::now().time_since_epoch().count();
    auto rng = std::default_random_engine(rng_seed);
    auto chan_dist = std::uniform_real_distribution<ComponentType>(0.0, 1.0);

    const ComponentType ERROR_TOL = error_tol;

    for(int i = 0; i < test_count; ++i) {
        const auto color = Hsi<ComponentType>(
                chan_dist(rng), chan_dist(rng), chan_dist(rng));

        const auto in_gamut = color.is_in_gamut();

        const auto rgb = to_rgb(color, HsiOutOfGamutMode::Preserve);

        bool converted_in_gamut = true;
        for(int i = 0; i < rgb.num_channels; ++i) {
            auto component = rgb.as_array()[i];
            if(component > 1.0) {
                if(std::abs(component - 1.0) <= ERROR_TOL) {
                    if(converted_in_gamut == in_gamut) {
                        continue;
                    }
                }
                converted_in_gamut = false;
            } else if(std::abs(component - 1.0) <= ERROR_TOL) {
                if(in_gamut == false && converted_in_gamut == true) {
                    converted_in_gamut = false;
                }
            }
        }

        if(in_gamut != converted_in_gamut) {
            return ::testing::AssertionFailure()
                    << "Conversion " << i + 1 << " with " << color
                    << ":\n\t is_in_gamut() returned " << in_gamut
                    << ", but the converted result was " << rgb;
        }
    }
    return ::testing::AssertionSuccess();
}
