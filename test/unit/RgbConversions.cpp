#include "gtest/gtest.h"

#include "ToHsv.h"
#include "ToRgb.h"
#include "ToHsl.h"
#include "ColorCast.h"
#include "Hsv.h"
#include "Hsl.h"
#include "Alpha.h"

#include <iostream>

#include "ConversionRef.h"

using namespace color;

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
        for(int i = 0; i < ref_vals::RGB_TEST.size(); ++i) {
            auto hsv = to_hsv(ref_vals::RGB_TEST[i]);
            auto rgb2 = to_rgb(hsv);

            const auto ERROR_TOL = 5e-4f;

            ASSERT_LE(std::abs(hsv.hue() - ref_vals::HSV_TEST[i].hue()),
                    ERROR_TOL);
            ASSERT_LE(std::abs(hsv.saturation() -
                              ref_vals::HSV_TEST[i].saturation()),
                    ERROR_TOL);
            ASSERT_LE(std::abs(hsv.value() - ref_vals::HSV_TEST[i].value()),
                    ERROR_TOL);

            ASSERT_LE(std::abs(rgb2.red() - ref_vals::RGB_TEST[i].red()),
                    ERROR_TOL);
            ASSERT_LE(std::abs(rgb2.green() - ref_vals::RGB_TEST[i].green()),
                    ERROR_TOL);
            ASSERT_LE(std::abs(rgb2.blue() - ref_vals::RGB_TEST[i].blue()),
                    ERROR_TOL);
        }
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

TEST(RgbConversions, to_hsl) {
    // Test a list of reference values, both ways.
    for(int i = 0; i < ref_vals::RGB_TEST.size(); ++i) {
        const auto test_rgb = ref_vals::RGB_TEST[i];
        const auto hsl = to_hsl(test_rgb);
        const auto ref_color = ref_vals::HSL_TEST[i];

        const auto ERROR_TOL = 1e-3;

        //Test all reference colors Rgb->Hsl.
        ASSERT_LE(std::abs(hsl.hue() - ref_color.hue()),
                ERROR_TOL);
        ASSERT_LE(std::abs(hsl.saturation() - ref_color.saturation()),
                ERROR_TOL);
        ASSERT_LE(std::abs(hsl.lightness() - ref_color.lightness()),
                ERROR_TOL);

        //Test going from the Hsl result back to the (hopefully original)
        //Rgb.
        const auto rgb = to_rgb(hsl);

        ASSERT_LE(std::abs(rgb.red() - test_rgb.red()), ERROR_TOL);
        ASSERT_LE(std::abs(rgb.green() - test_rgb.green()), ERROR_TOL);
        ASSERT_LE(std::abs(rgb.blue() - test_rgb.blue()), ERROR_TOL);
    }
}

TEST(RgbConversions, hsl_to_rgb) {
    for(int i = 0; i < ref_vals::HSL_TEST.size(); ++i) {
        const auto test_hsl = ref_vals::HSL_TEST[i];
        const auto rgb = to_rgb(test_hsl);
        const auto ref_color = ref_vals::RGB_TEST[i];
        const auto ERROR_TOL = 1e-3;

        //To rgb...
        ASSERT_LE(std::abs(rgb.red() - ref_color.red()), ERROR_TOL);
        ASSERT_LE(std::abs(rgb.green() - ref_color.green()), ERROR_TOL);
        ASSERT_LE(std::abs(rgb.blue() - ref_color.blue()), ERROR_TOL);

        //...And back
        const auto hsl = to_hsl(rgb);
        ASSERT_LE(std::abs(hsl.hue() -test_hsl.hue()), ERROR_TOL);
        ASSERT_LE(std::abs(hsl.saturation() -test_hsl.saturation()), ERROR_TOL);
        ASSERT_LE(std::abs(hsl.brightness() -test_hsl.brightness()), ERROR_TOL);
    }
}
