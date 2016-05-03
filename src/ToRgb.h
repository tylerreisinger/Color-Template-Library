#ifndef COLOR_TORGB_H_
#define COLOR_TORGB_H_

#include <cmath>
#include <type_traits>
#include <cassert>

#include "Rgb.h"
#include "Hsv.h"
#include "Hsl.h"
#include "ColorCast.h"
#include "ConvertUtil.h"

namespace color {

template <typename T,
        typename std::enable_if_t<std::is_floating_point<T>::value, int> = 0>
Rgb<T> to_rgb(const Hsv<T>& from) {
    Rgb<T> out;

    T hue_frac;
    auto hue_seg = details::decompose_hue(from.hue(), hue_frac);

    auto color_min_bound = from.value() * (1.0 - from.saturation());

    switch(hue_seg) {
    case -1:
    case 0: {
        auto g = from.value() * (1.0 - from.saturation() * (1.0 - hue_frac));
        out = Rgb<T>(from.value(), g, color_min_bound);
        break;
    }
    case 1: {
        auto r = from.value() * (1.0 - from.saturation() * hue_frac);
        out = Rgb<T>(r, from.value(), color_min_bound);
        break;
    }
    case 2: {
        auto b = from.value() * (1.0 - from.saturation() * (1.0 - hue_frac));
        out = Rgb<T>(color_min_bound, from.value(), b);
        break;
    }
    case 3: {
        auto g = from.value() * (1.0 - from.saturation() * hue_frac);
        out = Rgb<T>(color_min_bound, g, from.value());
        break;
    }
    case 4: {
        auto r = from.value() * (1.0 - from.saturation() * (1.0 - hue_frac));
        out = Rgb<T>(r, color_min_bound, from.value());
        break;
    }
    case 5:
    case 6: {
        auto b = from.value() * (1.0 - from.saturation() * hue_frac);
        out = Rgb<T>(from.value(), color_min_bound, b);
        break;
    }
    default: {
        assert(false &&
                "In unreachable hue segment -- input color likely invalid");
        out = Rgb<T>::broadcast(0.0);
        break;
    }
    }

    return out;
}

template <typename T,
        typename FloatType = float,
        typename std::enable_if_t<std::is_integral<T>::value, int> = 0>
Rgb<T> to_rgb(const Hsv<T>& from) {
    return color_cast<T>(to_rgb(color_cast<FloatType>(from)));
}

template <typename T,
        typename std::enable_if_t<std::is_floating_point<T>::value, int> = 0>
Rgba<T> to_rgb(const Hsva<T>& from) {
    return Rgba<T>(to_rgb(from.color()), from.alpha());
}

template <typename T,
        typename FloatType = float,
        typename std::enable_if_t<std::is_integral<T>::value, int> = 0>
Rgba<T> to_rgb(const Hsva<T>& from) {
    return Rgba<T>(to_rgb<T, FloatType>(from.color()), from.alpha());
}

template <typename T,
        typename std::enable_if_t<std::is_floating_point<T>::value, int> = 0>
Rgb<T> to_rgb(const Hsl<T>& from) {
    Rgb<T> out;

    T hue_frac;
    const auto hue_seg = details::decompose_hue(from.hue(), hue_frac);

    const auto chroma = color::chroma(from);
    const auto color_min_bound = from.lightness() - T(0.5)*chroma;
    const auto color_max_bound = color_min_bound+chroma;

    switch(hue_seg) {
    case -1:
    case 0: {
        const auto green = chroma * (hue_frac - T(0.5)) + from.lightness();
        out = Rgb<T>(color_max_bound, green, color_min_bound);
        break;
    }
    case 1: {
        const auto red = chroma * (T(0.5) - hue_frac) + from.lightness();
        out = Rgb<T>(red, color_max_bound, color_min_bound);
        break;
    }
    case 2: {
        const auto blue = chroma * (hue_frac - T(0.5)) + from.lightness();
        out = Rgb<T>(color_min_bound, color_max_bound, blue);
        break;
    }
    case 3: {
        const auto green = chroma * (T(0.5) - hue_frac) + from.lightness();
        out = Rgb<T>(color_min_bound, green, color_max_bound);
        break;
    }
    case 4: {
        const auto red = chroma * (hue_frac - T(0.5)) + from.lightness();
        out = Rgb<T>(red, color_min_bound, color_max_bound);
        break;
    }
    case 5:
    case 6: {
        const auto blue = chroma * (T(0.5) - hue_frac) + from.lightness();
        out = Rgb<T>(color_max_bound, color_min_bound, blue);
        break;
    }
    default:
        assert(false &&
                "In unreachable hue segment -- input color likely invalid");
        out = Rgb<T>::broadcast(0.0);
        break;
    }
    return out;
}

template <typename T,
        typename FloatType = float,
        typename std::enable_if_t<std::is_integral<T>::value, int> = 0>
Rgb<T> to_rgb(const Hsl<T>& from) {
    return color_cast<T>(to_rgb(color_cast<FloatType>(from)));
}

template <typename T,
        typename std::enable_if_t<std::is_floating_point<T>::value, int> = 0>
Rgba<T> to_rgb(const Hsla<T>& from) {
    return Rgba<T>(to_rgb(from.color()), from.alpha());
}

template <typename T,
        typename FloatType = float,
        typename std::enable_if_t<std::is_integral<T>::value, int> = 0>
Rgba<T> to_rgb(const Hsla<T>& from) {
    return Rgba<T>(to_rgb<T, FloatType>(from.color()), from.alpha());
}

}

#endif
