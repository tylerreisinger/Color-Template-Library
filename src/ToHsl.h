#ifndef COLOR_TOHSL_H_
#define COLOR_TOHSL_H_

#include <type_traits>

#include "Hsl.h"
#include "Rgb.h"
#include "ConvertUtil.h"
#include "ColorCast.h"

namespace color {

template<typename T,
    typename std::enable_if_t<std::is_floating_point<T>::value, int> = 0>
Hsl<T> to_hsl(const Rgb<T>& from) {
    const auto EPSILON = T(1e-10);
    auto c1 = from.red();
    auto c2 = from.green();
    auto c3 = from.blue();
    T min_channel;

    T scaling_factor = details::order_channels_for_hue(c1, c2, c3, min_channel);

    auto max_channel = c1;
    auto chroma = details::chroma(max_channel, min_channel); 
    auto hue = details::hue(chroma, scaling_factor, c2, c3);
    auto lightness = T(0.5) * (max_channel + min_channel);
    auto saturation = chroma / (T(1.0) - std::abs(T(2.0) * lightness - T(1.0))
            + EPSILON);

    return Hsl<T>(hue, saturation, lightness);
}

template <typename T,
        typename FloatType = float,
        typename std::enable_if_t<std::is_integral<T>::value, int> = 0>
Hsl<T> to_hsl(const Rgb<T>& from) {
    return color_cast<T>(to_hsl(color_cast<FloatType>(from)));
}

template <typename T,
        typename std::enable_if_t<std::is_floating_point<T>::value, int> = 0>
Hsla<T> to_hsl(const Rgba<T>& from) {
    return Hsla<T>(to_hsl(from.color()), from.alpha());
}

template <typename T,
        typename FloatType = float,
        typename std::enable_if_t<std::is_integral<T>::value, int> = 0>
Hsla<T> to_hsl(const Rgba<T>& from) {
    return Hsla<T>(to_hsl<T, FloatType>(from.color()), from.alpha());
}
}

#endif
