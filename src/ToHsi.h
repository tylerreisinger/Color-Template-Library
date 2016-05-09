#ifndef COLOR_TOHSI_H_
#define COLOR_TOHSI_H_

#include <cmath>
#include <type_traits>

#include "Angle.h"
#include "Hsi.h"
#include "Rgb.h"
#include "ColorCast.h"

namespace color {

template <typename T,
        typename std::enable_if_t<std::is_floating_point<T>::value, int> = 0>
Hsi<T> to_hsi(const Rgb<T>& from) {
    const T intensity =
            T(1.0 / 3.0) * (from.red() + from.green() + from.blue());

    T alpha, beta;
    std::tie(alpha, beta) = chromacity_coordinates(from);

    T hue = std::atan2(beta, alpha) / Radians<T>::period_length();

    if(hue < 0.0) {
        hue = 1.0 + hue;
    }

    const T min = std::min({from.red(), from.green(), from.blue()});

    T saturation;
    if(intensity != 0.0) {
        saturation = T(1.0) - min / intensity;
    } else {
        saturation = T(0.0);
    }

    return Hsi<T>(hue, saturation, intensity);
}

template <typename T,
        typename FloatType = float,
        typename std::enable_if_t<std::is_integral<T>::value, int> = 0>
Hsi<T> to_hsi(const Rgb<T>& from) {
    return color_cast<T>(to_hsl(color_cast<FloatType>(from)));
}

template <typename T,
        typename std::enable_if_t<std::is_floating_point<T>::value, int> = 0>
Hsia<T> to_hsi(const Rgba<T>& from) {
    return Hsia<T>(to_hsi(from.color()), from.alpha());
}

template <typename T,
        typename FloatType = float,
        typename std::enable_if_t<std::is_integral<T>::value, int> = 0>
Hsia<T> to_hsi(const Rgba<T>& from) {
    return Hsia<T>(to_hsi<T, FloatType>(from.color()), from.alpha());
}
}

#endif
