#include "gtest/gtest.h"

#include <array>
#include <iostream>

#include "Hsv.h"
#include "Color.h"
#include "Alpha.h"

#include "ConversionRef.h"

using namespace color;

TEST(Hsv, constructor) {
    // Test default constructor
    {
        auto c1 = Hsv<float>();

        ASSERT_FLOAT_EQ(c1.hue(), 0.0);
        ASSERT_FLOAT_EQ(c1.saturation(), 0.0);
        ASSERT_FLOAT_EQ(c1.value(), 0.0);
    }

    // Test component-wise constructor and
    {
        auto c1 = Hsv<float>(0.3, 0.6, 0.8);

        ASSERT_FLOAT_EQ(c1.hue(), 0.3);
        ASSERT_FLOAT_EQ(c1.saturation(), 0.6);
        ASSERT_FLOAT_EQ(c1.value(), 0.8);
    }

    // Test data and array constructor
    {
        std::array<uint8_t, 3> test_data{50, 120, 200};

        auto c1 = Hsv<uint8_t>(test_data.data());
        auto c2 = Hsv<uint8_t>(test_data);
        auto c3 = Hsv<uint8_t>(test_data[0], test_data[1], test_data[2]);

        ASSERT_EQ(c1, c2);
        ASSERT_EQ(c1, c3);
    }

    // Test tuple constructor
    {
        auto c1 = Hsv<double>(std::make_tuple(0.25, 0.5, 0.75));
        auto c2 = Hsv<double>(0.25, 0.5, 0.75);

        ASSERT_EQ(c1, c2);
    }
}

TEST(Hsv, equality) {
    {
        auto c1 = Hsv<uint8_t>(100, 200, 50);
        auto c2 = Hsv<uint8_t>(100, 200, 50);
        auto c3 = Hsv<uint8_t>(50, 106, 210);

        ASSERT_EQ(c1, c2);
        ASSERT_NE(c1, c3);
    }
}

TEST(Hsv, data) {
    {
        auto c1 = Hsv<float>(0.42, 0.67, 0.99);

        ASSERT_FLOAT_EQ(c1.hue(), c1.data()[0]);
        ASSERT_FLOAT_EQ(c1.saturation(), c1.data()[1]);
        ASSERT_FLOAT_EQ(c1.value(), c1.data()[2]);
        c1.data()[0] = 0.80;
        ASSERT_FLOAT_EQ(c1.hue(), 0.80);
        ASSERT_FLOAT_EQ(c1.hue(), c1.data()[0]);
    }

    {
        const auto c1 = Hsv<float>(0.33, 0.66, 0.99);


        ASSERT_FLOAT_EQ(c1.hue(), c1.data()[0]);
        ASSERT_FLOAT_EQ(c1.saturation(), c1.data()[1]);
        ASSERT_FLOAT_EQ(c1.value(), c1.data()[2]);
    }
}

TEST(Hsv, clamp) {
    {
        auto c1 = Hsv<float>(-0.2, 0.3, 1.7);
        auto c2 = c1.clamp(0.0, 0.9);

        ASSERT_TRUE(float_eq(c2, Hsv<float>(0.0, 0.3, 0.9)));
    }

    {
        auto c1 = Hsv<uint8_t>(50, 125, 225);
        auto c2 = c1.clamp(0, 255);

        ASSERT_EQ(c1, c2);
    }
}

TEST(Hsv, normalize) {
    {
        auto c1 = Hsv<float>(-0.2, 0.5, 1.3);
        auto c2 = Hsv<float>(1.35, -0.5, 1.2);
        auto c3 = Hsv<float>(0.88, 0.45, 0.72);
        c1 = c1.normalize();
        c2 = c2.normalize();

        ASSERT_TRUE(float_eq(c1, Hsv<float>(0.8, 0.5, 1.0)));
        ASSERT_TRUE(float_eq(c2, Hsv<float>(0.35, 0.0, 1.0)));
        ASSERT_TRUE(float_eq(c3, c3.normalize()));
    }

    {
        auto c1 = Hsv<uint8_t>(100, 200, 35);
        auto c2 = Hsv<uint8_t>(0, 0, 0);

        ASSERT_EQ(c1, c1.normalize());
        ASSERT_EQ(c2, c2.normalize());
    }
}

TEST(Hsv, inverse) {
    {
        auto c1 = Hsv<float>(0.8, 0.2, 0.5);
        auto c2 = Hsv<float>(0.5, 0.5, 0.5);
        auto c3 = Hsv<float>(0.0, 0.25, 1.0);

        ASSERT_TRUE(float_eq(c1.inverse(), Hsv<float>(0.3, 0.8, 0.5)));
        ASSERT_TRUE(float_eq(c2.inverse(), Hsv<float>(0.0, 0.5, 0.5)));
        ASSERT_TRUE(float_eq(c3.inverse(), Hsv<float>(0.5, 0.75, 0.0)));
    }

    {
        auto c1 = Hsv<uint8_t>(127, 250, 50);
        auto c2 = Hsv<uint8_t>(0, 255, 130);
        auto c3 = Hsv<uint8_t>(200, 200, 100);

        ASSERT_EQ(c1.inverse(), Hsv<uint8_t>(255, 5, 205));
        ASSERT_EQ(c2.inverse(), Hsv<uint8_t>(128, 0, 125));
        ASSERT_EQ(c3.inverse(), Hsv<uint8_t>(72, 55, 155));
    }
}

TEST(Hsv, lerp) {
    {
        auto c1 = Hsv<float>(0.9, 0.5, 0.9);
        auto c2 = Hsv<float>(0.1, 0.2, 0.2);
        auto c3 = Hsv<float>(0.5, 0.4, 0.9);

        auto result1 = c1.lerp(c2, 0.5);
        auto result2 = c1.lerp(c3, 0.5);


        ASSERT_TRUE(float_eq(result1, Hsv<float>(0.0, 0.35, 0.55)));
        ASSERT_TRUE(float_eq(result2, Hsv<float>(0.7, 0.45, 0.9)));
    }

    {
        auto c1 = Hsv<uint8_t>(100, 200, 255);
        auto c2 = Hsv<uint8_t>(200, 100, 100);

        ASSERT_EQ(c1.lerp(c2, 0.4), Hsv<uint8_t>(140, 160, 193));
        ASSERT_EQ(c2.lerp(c1, 0.6), Hsv<uint8_t>(140, 160, 193));
    }

    {
        auto c1 = Hsv<uint8_t>(50, 125, 200);
        auto c2 = Hsv<uint8_t>(205, 250, 250);

        ASSERT_EQ(c1.lerp(c2, 0.5), Hsv<uint8_t>(0, 187, 225));
        ASSERT_EQ(c2.lerp(c1, 0.5), Hsv<uint8_t>(0, 187, 225));
        ASSERT_EQ(c1.lerp(c2, 0.75), Hsv<uint8_t>(230, 218, 237));
        ASSERT_EQ(c2.lerp(c1, 0.25), Hsv<uint8_t>(230, 218, 237));
        ASSERT_EQ(c1.lerp(c2, 0.25), Hsv<uint8_t>(25, 156, 212));
        ASSERT_EQ(c2.lerp(c1, 0.75), Hsv<uint8_t>(25, 156, 212));
        ASSERT_EQ(c1.lerp(c2, 0.0), Hsv<uint8_t>(50, 125, 200));
        ASSERT_EQ(c2.lerp(c1, 1.0), Hsv<uint8_t>(50, 125, 200));
    }

    {
        auto c1 = Hsv<float>(0.9, 0.5, 0.9);
        auto c2 = Hsv<float>(0.1, 0.2, 0.2);
        auto c3 = Hsv<float>(0.5, 0.4, 0.9);

        auto result1 = c1.lerp_flat(c2, 0.5);
        auto result2 = c1.lerp_flat(c3, 0.5);


        ASSERT_TRUE(float_eq(result1, Hsv<float>(0.5, 0.35, 0.55)));
        ASSERT_TRUE(float_eq(result2, Hsv<float>(0.7, 0.45, 0.9)));
    }
}

TEST(Hsv, scale) {
    {
        auto c1 = Hsv<float>(0.9, 0.5, 0.0);
        auto c2 = Hsv<float>(-0.5, 0.8, 0.1);

        ASSERT_TRUE(float_eq(c1.scale(0.5), Hsv<float>(0.45, 0.25, 0.0)));
        ASSERT_TRUE(float_eq(c2.scale(0.1), Hsv<float>(-0.05, 0.08, 0.01)));
    }

    {
        auto c1 = Hsv<uint16_t>(30000, 10000, 60000);

        ASSERT_EQ(c1.scale(0.1), Hsv<uint16_t>(3000, 1000, 6000));
    }
}

TEST(Hsv, as_tuple) {
    {
        auto c1 = Hsv<double>(0.3, 0.1252, 0.525);

        ASSERT_FLOAT_EQ(std::get<0>(c1.as_tuple()), c1.hue());
        ASSERT_FLOAT_EQ(std::get<1>(c1.as_tuple()), c1.saturation());
        ASSERT_FLOAT_EQ(std::get<2>(c1.as_tuple()), c1.value());
    }
}

TEST(Hsv, as_array) {
    {
        auto c1 = Hsv<uint8_t>(82, 121, 1);
        auto test_array = std::array<uint8_t, 3>({82, 121, 1});

        ASSERT_EQ(c1.as_array(), test_array);
    }
}

TEST(Hsv, broadcast) {
    {
        auto c1 = Hsv<float>::broadcast(0.66);
        auto c2 = Hsv<uint8_t>::broadcast(173);
        auto c3 = Hsv<unsigned int>::broadcast(0);

        ASSERT_TRUE(float_eq(c1, Hsv<float>(0.66, 0.66, 0.66)));
        ASSERT_EQ(c2, Hsv<uint8_t>(173, 173, 173));
        ASSERT_EQ(c3, Hsv<unsigned int>(0, 0, 0));
    }
}

TEST(Hsv, hue_angle) {
    {
        auto c1 = Hsv<float>(0.5, 0.3, 0.75);

        ASSERT_FLOAT_EQ(c1.hue_angle<Radians>().value, PI<float>);
        ASSERT_FLOAT_EQ(c1.hue_angle<Degrees>().value, 180.0);

        c1.set_hue_angle(Degrees<float>(90.0));
        ASSERT_FLOAT_EQ(c1.hue(), 0.25);
        ASSERT_FLOAT_EQ(c1.hue_angle<Radians>().value, PI<float> / 2.0);
        ASSERT_FLOAT_EQ(c1.hue_angle<Degrees>().value, 90.0);
    }

    {
        auto c1 = Hsv<uint8_t>(127, 50, 250);

        auto deg_angle = c1.hue_angle<Degrees, float>().value;
        auto deg_rad = c1.hue_angle<Radians, float>().value;

        ASSERT_LE(std::abs(180.0 - deg_angle), 2.0);
        ASSERT_LE(std::abs(PI<float> - deg_rad), PI<float> / 90.);

        c1.set_hue_angle(Degrees<float>(90));

        deg_angle = c1.hue_angle<Degrees, float>().value;
        deg_rad = c1.hue_angle<Radians, float>().value;

        ASSERT_EQ(c1.hue(), 64);
        ASSERT_LE(std::abs(90.0 - deg_angle), 2.0);
        ASSERT_LE(std::abs(PI<float> / 2.0 - deg_rad), PI<float> / 90.);
    }
}

TEST(Hsv, chroma) {
    {
        for(int i = 0; i < ref_vals::CHROMA_TEST[i]; ++i) {
            auto ch = chroma(ref_vals::HSV_TEST[i]);

            static float ERROR_TOL = 1e-4;

            ASSERT_LE(std::abs(ch - ref_vals::CHROMA_TEST[i]), ERROR_TOL);
        }
    }
}
