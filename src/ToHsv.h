/** \file
 *  Defines to_hsv for converting colors to the Hsv color model.
 */
#ifndef COLOR_TOHSV_H_
#define COLOR_TOHSV_H_

#include <cmath>
#include <type_traits>

#include "Hsv.h"
#include "Rgb.h"

#include "ConvertUtil.h"

namespace color {

/** Convert `from` to an Hsv equivalent.
 */
template <typename T,
        typename std::enable_if_t<std::is_floating_point<T>::value, int> = 0>
Hsv<T> to_hsv(const Rgb<T>& from) {
    // Used to avoid division by zero by making the number
    // very slightly greater than zero.
    const auto EPSILON = T(1e-10);
    auto c1 = from.red();
    auto c2 = from.green();
    auto c3 = from.blue();
    T min_channel;

    T scaling_factor = details::order_channels_for_hue(c1, c2, c3, min_channel);

    auto max_channel = c1;
    auto chroma = details::chroma(max_channel, min_channel);
    auto hue = details::hue(chroma, scaling_factor, c2, c3);
    auto value = c1;
    auto saturation = chroma / (value + EPSILON);
    return Hsv<T>(hue, saturation, value);
}

//Specialization for integer-typed channels.
template <typename T,
        typename FloatType = float,
        typename std::enable_if_t<std::is_integral<T>::value, int> = 0>
Hsv<T> to_hsv(const Rgb<T>& from) {
    // Used to avoid division by zero by making the number
    // very slightly greater than zero.
    const auto EPSILON = FloatType(1e-10);
    auto c1 = from.red();
    auto c2 = from.green();
    auto c3 = from.blue();
    T min_channel;

    auto scaling_factor = details::order_channels_for_hue<T, FloatType>(
        c1, c2, c3, min_channel);

    // c1 is guaranteed to be the max
    FloatType one_over_max = FloatType(1.0) / BoundedChannel<T>::end_point();

    auto chroma = FloatType(c1 - min_channel) * one_over_max;
    auto channel_diff = FloatType(c2 - c3) * one_over_max;

    FloatType hue = std::abs(scaling_factor +
            channel_diff / (FloatType(6.0) * chroma + EPSILON));

    auto value = c1;
    auto saturation = chroma / FloatType(value * one_over_max + EPSILON);
    return Hsv<T>(PeriodicChannel<T>::from_float_channel(hue).value,
            BoundedChannel<T>::from_float_channel(saturation).value,
            value); // * max_channel_val, saturation * max_channel_val, value);
}

template <typename T,
        typename std::enable_if_t<std::is_floating_point<T>::value, int> = 0>
Hsva<T> to_hsv(const Rgba<T>& from) {
    return Hsva<T>(to_hsv(from.color()), from.alpha());
}

template <typename T,
        typename FloatType = float,
        typename std::enable_if_t<std::is_integral<T>::value, int> = 0>
Hsva<T> to_hsv(const Rgba<T>& from) {
    return Hsva<T>(to_hsv<T, FloatType>(from.color()), from.alpha());
}
}

#endif
