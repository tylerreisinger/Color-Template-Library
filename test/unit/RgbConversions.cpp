#include "gtest/gtest.h"

#include "ToHsv.h"
#include "ToRgb.h"
#include "ToHsl.h"
#include "ToHsi.h"
#include "ColorCast.h"
#include "Hsv.h"
#include "Hsl.h"
#include "Alpha.h"

#include <iostream>

#include "ConversionRef.h"
#include "Assertions.h"

using namespace color;

template <typename T>
void rgb_to_hsv_test_function(T ERROR_TOL) {
    for(int i = 0; i < ref_vals::RGB_TEST.size(); ++i) {
        const auto test_rgb = color_cast<T>(ref_vals::RGB_TEST[i]);
        const auto hsv = to_hsv(test_rgb);
        const auto rgb2 = to_rgb(hsv);
        const auto ref_color = color_cast<T>(ref_vals::HSV_TEST[i]);

        ASSERT_TRUE(equal_within_error(hsv.hue(), ref_color.hue(), ERROR_TOL));
        ASSERT_TRUE(equal_within_error(
                hsv.saturation(), ref_color.saturation(), ERROR_TOL));
        ASSERT_TRUE(
                equal_within_error(hsv.value(), ref_color.value(), ERROR_TOL));

        const T BACK_ERROR_TOL = 2 * ERROR_TOL;
        ASSERT_TRUE(
                equal_within_error(rgb2.red(), test_rgb.red(), BACK_ERROR_TOL));
        ASSERT_TRUE(equal_within_error(
                rgb2.green(), test_rgb.green(), BACK_ERROR_TOL));
        ASSERT_TRUE(equal_within_error(
                rgb2.blue(), test_rgb.blue(), BACK_ERROR_TOL));
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

        ASSERT_FLOAT_EQ(hsv.hue(), 0.0);
        ASSERT_FLOAT_EQ(hsv.saturation(), 0.0);
        ASSERT_FLOAT_EQ(hsv.value(), 0.0);
    }
    {
        auto rgb = color_cast<float>(Rgba<uint8_t>(57, 121, 77, 255));
        auto hsv = to_hsv(rgb);

        ASSERT_LE(
                std::abs(hsv.color().hue_angle<Degrees>().value - 139.0), 1.0);
        ASSERT_LE(std::abs(hsv.color().saturation() - 0.529), 0.01);
        ASSERT_LE((hsv.color().value() - 0.475), 0.01);
        ASSERT_FLOAT_EQ(hsv.alpha(), 1.0);
    }
    {
        auto rgb = Rgb<uint8_t>(57, 121, 77);
        auto hsv = to_hsv(rgb);

        ASSERT_EQ(hsv, Hsv<uint8_t>(98, 134, 121));
    }
    {
        auto rgb = Rgba<uint8_t>(100, 200, 0, 200);
        auto hsv = to_hsv(rgb);

        ASSERT_EQ(hsv, Hsva<uint8_t>(64, 255, 200, 200));
    }
    {
        auto rgb = Rgba<uint16_t>(65535, 65535, 65535, 65535);
        auto hsv = to_hsv(rgb);

        ASSERT_EQ(hsv, Hsva<uint16_t>(0, 0, 65535, 65535));
    }
    {
        auto rgb = Rgb<float>(0.628, 0.643, 0.142);
        auto hsv = to_hsv(rgb);
        ASSERT_LE(std::abs(hsv.hue_angle<Degrees>().value - 61.8), 0.01);
        ASSERT_LE(std::abs(hsv.saturation() - 0.779), 0.001);
        ASSERT_LE(std::abs(hsv.value() - 0.643), 0.001);
    }
}

TEST(RgbConversions, hsv_to_rgb) {
    {
        auto hsv = Hsva<float>(0.55, 0.25, 0.5, 0.8);
        auto rgb = to_rgb(hsv);

        ASSERT_FLOAT_EQ(rgb.color().red(), 0.375);
        ASSERT_FLOAT_EQ(rgb.color().green(), 0.4625);
        ASSERT_FLOAT_EQ(rgb.color().blue(), 0.5);
        ASSERT_FLOAT_EQ(rgb.alpha(), 0.8);
    }

    {
        auto hsv = Hsv<float>(0.85, 0.10, 0.10);
        auto rgb = to_rgb(hsv);
        auto hsv2 = to_hsv(rgb);

        ASSERT_TRUE(float_eq(hsv, hsv2));
    }
    {
        auto hsv = Hsv<float>(0.0, 0.0, 0.0);
        auto rgb = to_rgb(hsv);
        ASSERT_FLOAT_EQ(rgb.red(), 0.0);
        ASSERT_FLOAT_EQ(rgb.green(), 0.0);
        ASSERT_FLOAT_EQ(rgb.blue(), 0.0);
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
        float max_error = 0.0;
        const auto ERROR_TOL = 1e-5;
        for(float h = 0.01; h < 1.0; h += 0.06) {
            for(float s = 0.01; s < 1.0; s += 0.06) {
                for(float v = 0.01; v < 1.0; v += 0.06) {
                    auto hsv = Hsv<float>(h, s, v);
                    auto rgb = to_rgb(hsv);
                    auto hsv2 = to_hsv(rgb);

                    auto error = std::abs(hsv.hue() - hsv2.hue());
                    ASSERT_LE(error, ERROR_TOL);
                    max_error = std::max(error, max_error);
                    error = std::abs(hsv.saturation() - hsv2.saturation());
                    ASSERT_LE(error, ERROR_TOL);
                    max_error = std::max(error, max_error);
                    error = std::abs(hsv.value() - hsv2.value());
                    ASSERT_LE(error, ERROR_TOL);
                    max_error = std::max(error, max_error);
                }
            }
        }

        {
            auto hsv = Hsva<uint8_t>(255, 255, 255, 255);
            auto rgb = to_rgb(hsv);

            ASSERT_EQ(rgb, Rgba<uint8_t>(255, 0, 5, 255));
        }
        {
            auto hsv = Hsv<uint16_t>(65535, 65535, 65535);
            auto hsv2 = Hsv<uint16_t>(65500, 65535, 65535);
            auto rgb = color_cast<uint8_t>(hsv);
            auto rgb2 = color_cast<uint8_t>(hsv2);

            ASSERT_EQ(rgb, rgb2);
        }

        {
            auto hsv = Hsva<uint16_t>(0, 0, 0, 0);
            auto rgb = to_rgb(hsv);
            ASSERT_EQ(rgb, Rgba<uint16_t>(0, 0, 0, 0));
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

        ASSERT_TRUE(equal_within_error(hsl.hue(), ref_color.hue(), ERROR_TOL));
        ASSERT_TRUE(equal_within_error(
                hsl.saturation(), ref_color.saturation(), ERROR_TOL));
        ASSERT_TRUE(equal_within_error(
                hsl.lightness(), ref_color.lightness(), ERROR_TOL));

        const auto rgb = to_rgb(hsl);

        const auto BACK_ERROR_BOUND = T(2 * ERROR_TOL);
        ASSERT_TRUE(equal_within_error(
                rgb.red(), test_rgb.red(), BACK_ERROR_BOUND));
        ASSERT_TRUE(equal_within_error(
                rgb.green(), test_rgb.green(), BACK_ERROR_BOUND));
        ASSERT_TRUE(equal_within_error(
                rgb.blue(), test_rgb.blue(), BACK_ERROR_BOUND));
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
        ASSERT_TRUE(equal_within_error(
                rgb.color().red(), ref_color.red(), ERROR_TOL));
        ASSERT_TRUE(equal_within_error(
                rgb.color().green(), ref_color.green(), ERROR_TOL));
        ASSERT_TRUE(equal_within_error(
                rgb.color().blue(), ref_color.blue(), ERROR_TOL));
        ASSERT_EQ(rgb.alpha(), ref_color.green());

        //...And back
        const auto hsl = to_hsl(rgb);
        const auto BACK_ERROR_TOL = T(2 * ERROR_TOL);
        ASSERT_TRUE(equal_within_error(
                hsl.color().hue(), test_hsl.color().hue(), BACK_ERROR_TOL));
        ASSERT_TRUE(equal_within_error(hsl.color().saturation(),
                test_hsl.color().saturation(),
                BACK_ERROR_TOL));
        ASSERT_TRUE(equal_within_error(hsl.color().brightness(),
                test_hsl.color().brightness(),
                BACK_ERROR_TOL));
        ASSERT_EQ(hsl.alpha(), test_hsl.alpha());
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

        ASSERT_TRUE(equal_within_error(hsi.hue(), ref_color.hue(), ERROR_TOL));
        ASSERT_TRUE(equal_within_error(
                hsi.saturation(), ref_color.saturation(), ERROR_TOL));
        ASSERT_TRUE(equal_within_error(
                hsi.intensity(), ref_color.intensity(), ERROR_TOL));

        const auto rgb = to_rgb(hsi);

        const auto BACK_ERROR_BOUND = T(2 * ERROR_TOL);
        ASSERT_TRUE(equal_within_error(
                rgb.red(), test_rgb.red(), BACK_ERROR_BOUND));
        ASSERT_TRUE(equal_within_error(
                rgb.green(), test_rgb.green(), BACK_ERROR_BOUND));
        ASSERT_TRUE(equal_within_error(
                rgb.blue(), test_rgb.blue(), BACK_ERROR_BOUND));
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

        ASSERT_PRED_FORMAT3(
                assert_colors_near, c2, Rgb<float>(3.0, 0.0, 0.0), 1e-5);
        ASSERT_PRED_FORMAT3(
                assert_colors_near, c3, Rgb<float>(1.0, 0.0, 0.0), 1e-5);
    }
}
