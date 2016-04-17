#include "gtest/gtest.h"

#include <array>

#include "Rgb.h"
#include "Color.h"
#include "ConversionRef.h"
#include "Angle.h"

using namespace color;

TEST(Rgb, default_constructor) {
    color::Rgb<uint8_t> c1;
    ASSERT_EQ(c1.red(), uint8_t(0));
    ASSERT_EQ(c1.green(), uint8_t(0));
    ASSERT_EQ(c1.blue(), uint8_t(0));

    color::Rgb<uint8_t> c2;
    ASSERT_EQ(c1.red(), c2.red());
    ASSERT_EQ(c1.green(), c2.green());
    ASSERT_EQ(c1.blue(), c2.blue());

    color::Rgb<double> c3;
    ASSERT_EQ(c3.red(), 0.0);
    ASSERT_EQ(c3.green(), 0.0);
    ASSERT_EQ(c3.blue(), 0.0);
}

TEST(Rgb, explicit_constructor) {
    color::Rgb<uint8_t> c1(30, 20, 200);
    ASSERT_EQ(c1.red(), uint8_t(30));
    ASSERT_EQ(c1.green(), uint8_t(20));
    ASSERT_EQ(c1.blue(), uint8_t(200));

    color::Rgb<float> c2(0.2, 0.5, 0.75);
    ASSERT_EQ(c2.red(), 0.2f);
    ASSERT_EQ(c2.green(), 0.5f);
    ASSERT_EQ(c2.blue(), 0.75f);

    color::Rgb<float> c3(0.2, 0.5, 0.75);
    ASSERT_EQ(c2, c3);

    ASSERT_EQ(c1, color::Rgb<uint8_t>(std::make_tuple(30, 20, 200)));
}

TEST(Rgb, data) {
    color::Rgb<uint8_t> c1(50, 100, 137);
    auto data = c1.data();
    ASSERT_EQ(data[0], 50);
    ASSERT_EQ(data[1], 100);
    ASSERT_EQ(data[2], 137);

    color::Rgb<double> c_arr[2] = {{0.1, 0.3, 0.5}, {0.7, 0.9, 0.2}};
    auto data2 = c_arr[0].data();
    ASSERT_EQ(data2[0], 0.1);
    ASSERT_EQ(data2[1], 0.3);
    ASSERT_EQ(data2[2], 0.5);
    ASSERT_EQ(data2[3], 0.7);
    ASSERT_EQ(data2[4], 0.9);
    ASSERT_EQ(data2[5], 0.2);
}

TEST(Rgb, clamp) {
    color::Rgb<uint8_t> c1(30, 150, 230);
    auto c2 = c1.clamp(0, 250);
    ASSERT_EQ(c2, c1);
    auto c3 = c1.clamp(50, 200);
    ASSERT_EQ(c3, color::Rgb<uint8_t>(50, 150, 200));
}

TEST(Rgb, as_array) {
    color::Rgb<float> c1(0.23, 0.34, 0.75);
    auto test_array = std::array<float, 3>{0.23, 0.34, 0.75};

    ASSERT_EQ(c1.as_array(), test_array);
}

TEST(Rgb, as_tuple) {
    color::Rgb<uint16_t> c1(50, 80, 160);

    auto vals = c1.as_tuple();
    ASSERT_EQ(std::get<0>(vals), 50);
    ASSERT_EQ(std::get<1>(vals), 80);
    ASSERT_EQ(std::get<2>(vals), 160);

    color::Rgb<float> c2(0.7, 0.3, 0.8);
    float r, g, b;
    std::tie(r, g, b) = c2.as_tuple();
    ASSERT_FLOAT_EQ(r, 0.7);
    ASSERT_FLOAT_EQ(g, 0.3);
    ASSERT_FLOAT_EQ(b, 0.8);
}

TEST(Rgb, inverse) {
    color::Rgb<uint8_t> c1;
    ASSERT_EQ(c1.inverse(), color::Rgb<uint8_t>(255, 255, 255));

    color::Rgb<uint8_t> c2(127, 127, 127);
    ASSERT_EQ(c2.inverse(), color::Rgb<uint8_t>(128, 128, 128));

    color::Rgb<float> c3(0.3, 0.5, 0.7);
    auto c3i = c3.inverse();
    ASSERT_FLOAT_EQ(c3i.red(), 0.7);
    ASSERT_FLOAT_EQ(c3i.green(), 0.5);
    ASSERT_FLOAT_EQ(c3i.blue(), 0.3);
}

TEST(Rgb, normalize) {
    color::Rgb<uint8_t> c1(255, 127, 0);
    ASSERT_EQ(c1.normalize(), c1);

    color::Rgb<float> c2(0.2, 0.8, 1.0);
    ASSERT_EQ(c2.normalize(), c2);

    color::Rgb<float> c3(1.7, 0.35, 999);
    ASSERT_EQ(c3.normalize(), color::Rgb<float>(1.0, 0.35, 1.0));

    color::Rgb<float> c4(1.0, -0.35, -999);
    ASSERT_EQ(c4.normalize(), color::Rgb<float>(1.0, 0.0, 0.0));
}

TEST(Rgb, lerp) {
    color::Rgb<float> c1(0.2, 1.0, 0.0);
    color::Rgb<float> c2(0.8, 0.0, 1.0);

    auto interp = c1.lerp(c2, 0.5);
    ASSERT_FLOAT_EQ(interp.red(), 0.5);
    ASSERT_FLOAT_EQ(interp.green(), 0.5);
    ASSERT_FLOAT_EQ(interp.blue(), 0.5);

    color::Rgb<uint8_t> c3(100, 200, 255);
    color::Rgb<uint8_t> c4(200, 0, 0);
    auto interp2 = c3.lerp(c4, 0.25);
    ASSERT_EQ(interp2, color::Rgb<uint8_t>(125, 150, 191));
}

TEST(Rgb, scale) {
    auto c1 = color::Rgb<uint8_t>(100, 200, 50);
    c1 = c1.scale(0.5);

    ASSERT_EQ(c1, color::Rgb<uint8_t>(50, 100, 25));
}

TEST(Rgb, arithmetic) {
    color::Rgb<uint8_t> c1(25, 25, 50);
    color::Rgb<uint8_t> c2(125, 100, 150);
    ASSERT_EQ(c1 + c2, color::Rgb<uint8_t>(150, 125, 200));
    ASSERT_EQ(c2 - c1, color::Rgb<uint8_t>(100, 75, 100));
}

TEST(Rgb, get) {
    color::Rgb<uint8_t> c1(5, 10, 15);
    ASSERT_EQ(color::get<0>(c1), 5);
    ASSERT_EQ(color::get<1>(c1), 10);
    ASSERT_EQ(color::get<2>(c1), 15);
}

TEST(Rgb, chromacity_coordinates) {
    {
        auto c1 = Rgb<float>(1.0, 0.0, 0.0);
        float alpha, beta;
        std::tie(alpha, beta) = chromacity_coordinates(c1);

        ASSERT_FLOAT_EQ(alpha, 1.0);
        ASSERT_FLOAT_EQ(beta, 0.0);
    }
    {
        auto c1 = Rgb<float>(0.75, 0.33, 0.5);
        float alpha, beta;
        std::tie(alpha, beta) = chromacity_coordinates(c1);

        ASSERT_FLOAT_EQ(alpha, .335);
        ASSERT_FLOAT_EQ(beta, -0.1472243186);
    }
}

TEST(Rgb, circular_chromacity) {
    for(int i = 0; i < ref_vals::RGB_TEST.size(); ++i) {
        float alpha, beta;
        std::tie(alpha, beta) = chromacity_coordinates(ref_vals::RGB_TEST[i]);
        auto chroma = circular_chroma(alpha, beta);
        auto hue = circular_hue(alpha, beta);

        constexpr auto ERROR_TOL = 1e-3f;

        ASSERT_LE(std::abs(ref_vals::CIRCULAR_CHROMA_TEST[i] - chroma),
                ERROR_TOL);
        ASSERT_LE(std::abs(ref_vals::CIRCULAR_HUE_TEST[i] - hue), ERROR_TOL);
    }
}
