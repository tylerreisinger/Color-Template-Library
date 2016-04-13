#ifndef CONVERSIONREF_H_
#define CONVERSIONREF_H_

#include <array>

#include "Rgb.h"
#include "Hsv.h"

using namespace color;

// List of test cases comes from:
// https://en.wikipedia.org/wiki/HSL_and_HSV

namespace ref_vals {

static constexpr std::array<Rgb<float>, 19> RGB_TEST = {
        Rgb<float>(1.0, 1.0, 1.0),
        Rgb<float>(0.5, 0.5, 0.5),
        Rgb<float>(0.0, 0.0, 0.0),
        Rgb<float>(1.0, 0.0, 0.0),
        Rgb<float>(0.75, 0.75, 0.0),
        Rgb<float>(0.0, 0.5, 0.0),
        Rgb<float>(0.5, 1.0, 1.0),
        Rgb<float>(0.5, 0.5, 1.0),
        Rgb<float>(0.75, 0.25, 0.75),
        Rgb<float>(0.628, 0.643, 0.142),
        Rgb<float>(0.255, 0.104, 0.918),
        Rgb<float>(0.116, 0.675, 0.255),
        Rgb<float>(0.941, 0.785, 0.053),
        Rgb<float>(0.704, 0.187, 0.897),
        Rgb<float>(0.931, 0.463, 0.316),
        Rgb<float>(0.998, 0.974, 0.532),
        Rgb<float>(0.099, 0.795, 0.591),
        Rgb<float>(0.211, 0.149, 0.597),
        Rgb<float>(0.495, 0.493, 0.721)};

static constexpr std::array<Hsv<float>, 19> HSV_TEST = {
        Hsv<float>(0.0, 0.0, 1.0),
        Hsv<float>(0.0, 0.0, 0.5),
        Hsv<float>(0.0, 0.0, 0.0),
        Hsv<float>(0.0, 1.0, 1.0),
        Hsv<float>(0.16666666666666666, 1.0, 0.75),
        Hsv<float>(0.3333333333333333, 1.0, 0.5),
        Hsv<float>(0.5, 0.5, 1.0),
        Hsv<float>(0.6666666666666666, 0.5, 1.0),
        Hsv<float>(0.8333333333333334, 0.667, 0.75),
        Hsv<float>(0.17166666666666666, 0.779, 0.643),
        Hsv<float>(0.6975, 0.887, 0.918),
        Hsv<float>(0.37472222222222223, 0.828, 0.675),
        Hsv<float>(0.1375, 0.944, 0.941),
        Hsv<float>(0.7880555555555555, 0.792, 0.897),
        Hsv<float>(0.03972222222222222, 0.661, 0.931),
        Hsv<float>(0.15805555555555556, 0.467, 0.998),
        Hsv<float>(0.45111111111111113, 0.875, 0.795),
        Hsv<float>(0.6897222222222222, 0.75, 0.597),
        Hsv<float>(0.6680555555555555, 0.316, 0.721)};
}

#endif
