#include "gtest/gtest.h"

#include <array>
#include <type_traits>

#include "Angle.h"
#include "Assertions.h"
#include "Color.h"
#include "ConversionRef.h"
#include "Rgb.h"

using namespace color;

TEST(Rgb, default_constructor) {
    Rgb<uint8_t> c1;
    ASSERT_EQ(c1.red(), uint8_t(0));
    ASSERT_EQ(c1.green(), uint8_t(0));
    ASSERT_EQ(c1.blue(), uint8_t(0));

    Rgb<uint8_t> c2;
    ASSERT_EQ(c1.red(), c2.red());
    ASSERT_EQ(c1.green(), c2.green());
    ASSERT_EQ(c1.blue(), c2.blue());

    Rgb<double> c3;
    ASSERT_EQ(c3.red(), 0.0);
    ASSERT_EQ(c3.green(), 0.0);
    ASSERT_EQ(c3.blue(), 0.0);
}

TEST(Rgb, explicit_constructor) {
    Rgb<uint8_t> c1(30, 20, 200);
    ASSERT_EQ(c1.red(), uint8_t(30));
    ASSERT_EQ(c1.green(), uint8_t(20));
    ASSERT_EQ(c1.blue(), uint8_t(200));

    Rgb<float> c2(0.2, 0.5, 0.75);
    ASSERT_EQ(c2.red(), 0.2f);
    ASSERT_EQ(c2.green(), 0.5f);
    ASSERT_EQ(c2.blue(), 0.75f);

    Rgb<float> c3(0.2, 0.5, 0.75);
    ASSERT_EQ(c2, c3);

    ASSERT_EQ(c1, Rgb<uint8_t>(std::make_tuple(30, 20, 200)));
}

TEST(Rgb, data) {
    Rgb<uint8_t> c1(50, 100, 137);
    auto data = c1.data();
    ASSERT_EQ(data[0], 50);
    ASSERT_EQ(data[1], 100);
    ASSERT_EQ(data[2], 137);

    Rgb<double> c_arr[2] = {{0.1, 0.3, 0.5}, {0.7, 0.9, 0.2}};
    auto data2 = c_arr[0].data();
    ASSERT_EQ(data2[0], 0.1);
    ASSERT_EQ(data2[1], 0.3);
    ASSERT_EQ(data2[2], 0.5);
    ASSERT_EQ(data2[3], 0.7);
    ASSERT_EQ(data2[4], 0.9);
    ASSERT_EQ(data2[5], 0.2);
}

TEST(Rgb, clamp) {
    Rgb<uint8_t> c1(30, 150, 230);
    auto c2 = c1.clamp(0, 250);
    ASSERT_COLORS_EQ(c2, c1);
    auto c3 = c1.clamp(50, 200);
    ASSERT_COLORS_EQ(c3, Rgb<uint8_t>(50, 150, 200));
}

TEST(Rgb, as_array) {
    Rgb<float> c1(0.23, 0.34, 0.75);
    auto test_array = std::array<float, 3>{0.23, 0.34, 0.75};

    ASSERT_EQ(c1.as_array(), test_array);
}

TEST(Rgb, as_tuple) {
    Rgb<uint16_t> c1(50, 80, 160);

    auto vals = c1.as_tuple();
    ASSERT_EQ(std::get<0>(vals), 50);
    ASSERT_EQ(std::get<1>(vals), 80);
    ASSERT_EQ(std::get<2>(vals), 160);

    Rgb<float> c2(0.7, 0.3, 0.8);
    float r, g, b;
    std::tie(r, g, b) = c2.as_tuple();
    ASSERT_FLOAT_EQ(r, 0.7);
    ASSERT_FLOAT_EQ(g, 0.3);
    ASSERT_FLOAT_EQ(b, 0.8);
}

TEST(Rgb, inverse) {
    Rgb<uint8_t> c1;
    ASSERT_COLORS_EQ(c1.inverse(), Rgb<uint8_t>(255, 255, 255));

    Rgb<uint8_t> c2(127, 127, 127);
    ASSERT_COLORS_EQ(c2.inverse(), Rgb<uint8_t>(128, 128, 128));

    Rgb<float> c3(0.3, 0.5, 0.7);
    auto c3i = c3.inverse();
    ASSERT_COLORS_NEAR(c3i, Rgb<float>(0.7, 0.5, 0.3), FLOAT_TOL);
}

TEST(Rgb, normalize) {
    Rgb<uint8_t> c1(255, 127, 0);
    ASSERT_COLORS_EQ(c1.normalize(), c1);

    Rgb<float> c2(0.2, 0.8, 1.0);
    ASSERT_COLORS_EQ(c2.normalize(), c2);

    Rgb<float> c3(1.7, 0.35, 999);
    ASSERT_COLORS_NEAR(c3.normalize(), Rgb<float>(1.0, 0.35, 1.0), FLOAT_TOL);

    Rgb<float> c4(1.0, -0.35, -999);
    ASSERT_COLORS_NEAR(c4.normalize(), Rgb<float>(1.0, 0.0, 0.0), FLOAT_TOL);
}

TEST(Rgb, lerp) {
    Rgb<float> c1(0.2, 1.0, 0.0);
    Rgb<float> c2(0.8, 0.0, 1.0);

    auto interp = c1.lerp(c2, 0.5);
    ASSERT_COLORS_NEAR(interp, Rgb<float>(0.5, 0.5, 0.5), FLOAT_TOL);

    Rgb<uint8_t> c3(100, 200, 255);
    Rgb<uint8_t> c4(200, 0, 0);
    auto interp2 = c3.lerp(c4, 0.25);
    ASSERT_COLORS_EQ(interp2, Rgb<uint8_t>(125, 150, 191));
}

TEST(Rgb, scale) {
    auto c1 = Rgb<uint8_t>(100, 200, 50);
    c1 = c1.scale(0.5);

    ASSERT_COLORS_EQ(c1, Rgb<uint8_t>(50, 100, 25));
}

TEST(Rgb, arithmetic) {
    Rgb<uint8_t> c1(25, 25, 50);
    Rgb<uint8_t> c2(125, 100, 150);
    ASSERT_COLORS_EQ(c1 + c2, Rgb<uint8_t>(150, 125, 200));
    ASSERT_COLORS_EQ(c2 - c1, Rgb<uint8_t>(100, 75, 100));
}

TEST(Rgb, get) {
    Rgb<uint8_t> c1(5, 10, 15);
    ASSERT_EQ(get<0>(c1), 5);
    ASSERT_EQ(get<1>(c1), 10);
    ASSERT_EQ(get<2>(c1), 15);
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

TEST(Rgb, distance) {
    {
        auto c1 = Rgb<float>(1.0, 0.0, 0.0);
        auto c2 = Rgb<float>(0.0, 1.0, 1.0);

        ASSERT_FLOAT_EQ(c1.distance(c2), 1.0);
    }
    {
        auto c1 = Rgb<float>(0.25, 0.25, 0.40);
        auto c2 = Rgb<float>(0.50, 0.40, 0.65);

        ASSERT_FLOAT_EQ(c1.squared_distance(c2), 0.04916666666666666);
        ASSERT_FLOAT_EQ(c1.distance(c2), 0.2217355782608345);
    }
    {
        auto c1 = Rgb<uint8_t>(100, 200, 50);
        auto c2 = Rgb<uint8_t>(166, 125, 150);

        static_assert(std::is_floating_point<decltype(c1.distance(c2))>::value,
                "distance() yields an integral result");

        ASSERT_FLOAT_EQ(c1.distance(c2), 81.61086528969861);
    }
}

TEST(Rgb, swap) {
    auto c1 = Rgb<uint8_t>(120, 240, 55);
    auto c2 = Rgb<uint8_t>(183, 59, 111);

    swap(c1, c2);

    ASSERT_COLORS_EQ(c1, Rgb<uint8_t>(183, 59, 111));
    ASSERT_COLORS_EQ(c2, Rgb<uint8_t>(120, 240, 55));
}
