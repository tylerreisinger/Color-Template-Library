#include "gtest/gtest.h"

#include <iostream>

#include "Assertions.h"
#include "Alpha.h"
#include "ColorCast.h"
#include "ConversionRef.h"
#include "Hsi.h"
#include "Hsl.h"
#include "Hsv.h"
#include "Rgb.h"

using namespace color;

template <typename T>
void rgb_to_hsv_test_function(T ERROR_TOL) {
    for(int i = 0; i < ref_vals::RGB_TEST.size(); ++i) {
        const auto test_rgb = color_cast<T>(ref_vals::RGB_TEST[i]);
        const auto hsv = to_hsv(test_rgb);
        const auto rgb2 = to_rgb(hsv);
        const auto ref_color = color_cast<T>(ref_vals::HSV_TEST[i]);

        ASSERT_COLORS_NEAR(hsv, ref_color, ERROR_TOL);

        const T BACK_ERROR_TOL = 2 * ERROR_TOL;
        ASSERT_COLORS_NEAR(rgb2, test_rgb, BACK_ERROR_TOL);
    }
}


TEST(RgbConversions, rgb_to_hsv) {
    {
        auto rgb = color_cast<float>(Rgb<uint8_t>(100, 200, 255));

        auto hsv = to_hsv(rgb);

        ASSERT_LE(std::abs(hsv.hue_angle<Degrees>().value - 201.0), 1.0);
        ASSERT_LE(std::abs(hsv.saturation() - 0.608), 0.01);
        ASSERT_FLOAT_EQ(hsv.value(), 1.0);
    }
    {
        auto rgb = Rgb<float>(0.0, 0.0, 0.0);
        auto hsv = to_hsv(rgb);

        ASSERT_COLORS_NEAR(hsv, Hsv<float>(0.0, 0.0, 0.0), FLOAT_TOL);
    }
    {
        auto rgb = color_cast<float>(Rgba<uint8_t>(57, 121, 77, 255));
        auto hsv = to_hsv(rgb);

        ASSERT_COLORS_NEAR(
                hsv, Hsva<float>(139.0 / 360.0, 0.529, 0.475, 1.0), 1e-3);
    }
    {
        auto rgb = Rgb<uint8_t>(57, 121, 77);
        auto hsv = to_hsv(rgb);

        ASSERT_COLORS_EQ(hsv, Hsv<uint8_t>(98, 134, 121));
    }
    {
        auto rgb = Rgba<uint8_t>(100, 200, 0, 200);
        auto hsv = to_hsv(rgb);

        ASSERT_COLORS_EQ(hsv, Hsva<uint8_t>(64, 255, 200, 200));
    }
    {
        auto rgb = Rgba<uint16_t>(65535, 65535, 65535, 65535);
        auto hsv = to_hsv(rgb);

        ASSERT_COLORS_EQ(hsv, Hsva<uint16_t>(0, 0, 65535, 65535));
    }
    {
        auto rgb = Rgb<float>(0.628, 0.643, 0.142);
        auto hsv = to_hsv(rgb);
        ASSERT_COLORS_NEAR(hsv, Hsv<float>(61.8 / 360.0, 0.779, 0.643), 1e-3);
    }
}

TEST(RgbConversions, hsv_to_rgb) {
    {
        auto hsv = Hsva<float>(0.55, 0.25, 0.5, 0.8);
        auto rgb = to_rgb(hsv);

        ASSERT_COLORS_NEAR(rgb, Rgba<float>(0.375, 0.4625, 0.5, 0.8), 1e-3);
    }

    {
        auto hsv = Hsv<float>(0.85, 0.10, 0.10);
        auto rgb = to_rgb(hsv);
        auto hsv2 = to_hsv(rgb);

        ASSERT_COLORS_NEAR(hsv, hsv2, FLOAT_TOL);
    }
    {
        auto hsv = Hsv<float>(0.0, 0.0, 0.0);
        auto rgb = to_rgb(hsv);
        ASSERT_COLORS_NEAR(hsv, Hsv<float>(0.0, 0.0, 0.0), FLOAT_TOL);
    }

    {
        // Test a list of reference values, both ways.
        rgb_to_hsv_test_function<float>(1e-3f);
        rgb_to_hsv_test_function<double>(1e-3f);
        rgb_to_hsv_test_function<uint8_t>(2);
        rgb_to_hsv_test_function<uint16_t>(65);
        rgb_to_hsv_test_function<uint32_t>(
                std::numeric_limits<uint32_t>::max() / 1000);
    }

    {
        // Test that a large collection of generated colors
        // correctly converts Hsv->Rgb->Hsv and gets back
        // nearly the same result as went in (up to the error
        // tolerance).
        const auto ERROR_TOL = 1e-5;
        for(float h = 0.01; h < 1.0; h += 0.06) {
            for(float s = 0.01; s < 1.0; s += 0.06) {
                for(float v = 0.01; v < 1.0; v += 0.06) {
                    auto hsv = Hsv<float>(h, s, v);
                    auto rgb = to_rgb(hsv);
                    auto hsv2 = to_hsv(rgb);

                    ASSERT_COLORS_NEAR(hsv, hsv2, ERROR_TOL);
                }
            }
        }

        {
            auto hsv = Hsva<uint8_t>(255, 255, 255, 255);
            auto rgb = to_rgb(hsv);

            ASSERT_COLORS_EQ(rgb, Rgba<uint8_t>(255, 0, 5, 255));
        }
        {
            auto hsv = Hsv<uint16_t>(65535, 65535, 65535);
            auto hsv2 = Hsv<uint16_t>(65500, 65535, 65535);
            auto rgb = color_cast<uint8_t>(hsv);
            auto rgb2 = color_cast<uint8_t>(hsv2);

            ASSERT_COLORS_EQ(rgb, rgb2);
        }

        {
            auto hsv = Hsva<uint16_t>(0, 0, 0, 0);
            auto rgb = to_rgb(hsv);
            ASSERT_COLORS_EQ(rgb, Rgba<uint16_t>(0, 0, 0, 0));
        }
    }
}

// Generic function to run the test on several data types.
template <typename T>
void rgb_to_hsl_test_function(T ERROR_TOL) {
    for(int i = 0; i < ref_vals::RGB_TEST.size(); ++i) {
        const auto test_rgb = color_cast<T>(ref_vals::RGB_TEST[i]);
        const auto hsl = to_hsl(test_rgb);
        const auto ref_color = color_cast<T>(ref_vals::HSL_TEST[i]);

        // Rounding errors can cofound to create errors of about 2/255.

        ASSERT_COLORS_NEAR(hsl, ref_color, ERROR_TOL);

        const auto rgb = to_rgb(hsl);

        const auto BACK_ERROR_TOL = T(2 * ERROR_TOL);
        ASSERT_COLORS_NEAR(rgb, test_rgb, BACK_ERROR_TOL);
    }
}

template <typename T>
void hsl_to_rgb_test_function(T ERROR_TOL) {
    for(int i = 0; i < ref_vals::HSL_TEST.size(); ++i) {
        const auto ref_color = color_cast<T>(ref_vals::RGB_TEST[i]);
        const auto test_hsl = Hsla<T>(
                color_cast<T>(ref_vals::HSL_TEST[i]), ref_color.green());
        const auto rgb = to_rgb(test_hsl);

        //To rgb...
        ASSERT_COLORS_NEAR(rgb.color(), ref_color, ERROR_TOL);
        ASSERT_EQ(rgb.alpha(), ref_color.green());

        //...And back
        const auto hsl = to_hsl(rgb);
        const auto BACK_ERROR_TOL = T(2 * ERROR_TOL);
        ASSERT_COLORS_NEAR(hsl, test_hsl, BACK_ERROR_TOL);
    }
}

TEST(RgbConversions, rgb_to_hsl) {
    // Test rgb->hsl conversion for several component data types.
    rgb_to_hsl_test_function<float>(1e-3);
    rgb_to_hsl_test_function<double>(1e-3);
    rgb_to_hsl_test_function<uint8_t>(2);
    rgb_to_hsl_test_function<uint16_t>(65);
    rgb_to_hsl_test_function<uint32_t>(
            std::numeric_limits<uint32_t>::max() / 1000);
}

TEST(RgbConversions, hsl_to_rgb) {
    hsl_to_rgb_test_function<float>(1e-3);
    hsl_to_rgb_test_function<double>(1e-3);
    hsl_to_rgb_test_function<uint8_t>(4);
    hsl_to_rgb_test_function<uint16_t>(65);
    hsl_to_rgb_test_function<uint32_t>(
            std::numeric_limits<uint32_t>::max() / 1000);
}


template <typename T>
void rgb_to_hsi_test_function(T ERROR_TOL) {
    for(int i = 0; i < ref_vals::RGB_TEST.size(); ++i) {
        const auto test_rgb = color_cast<T>(ref_vals::RGB_TEST[i]);
        const auto hsi = to_hsi(test_rgb);
        const auto ref_color = color_cast<T>(ref_vals::HSI_TEST[i]);

        ASSERT_COLORS_NEAR(hsi, ref_color, ERROR_TOL);

        const auto rgb = to_rgb(hsi);

        const auto BACK_ERROR_TOL = T(2 * ERROR_TOL);
        ASSERT_COLORS_NEAR(rgb, test_rgb, BACK_ERROR_TOL);
    }
}


TEST(RgbConversions, rgb_to_hsi) {
    rgb_to_hsi_test_function<float>(1e-3);
    rgb_to_hsi_test_function<double>(1e-3);
}

TEST(RgbConversions, hsi_to_rgb) {
    {
        auto c1 = Hsi<float>(0.0, 1.0, 1.0);
        auto c2 = to_rgb(c1, HsiOutOfGamutMode::Preserve);
        auto c3 = to_rgb(c1, HsiOutOfGamutMode::Clip);


        ASSERT_COLORS_NEAR(c2, Rgb<float>(3.0, 0.0, 0.0), 1e-5);
        ASSERT_COLORS_NEAR(c3, Rgb<float>(1.0, 0.0, 0.0), 1e-5);
    }
}
