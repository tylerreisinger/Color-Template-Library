#ifndef COLOR_TORGB_H_
#define COLOR_TORGB_H_

#include <cmath>
#include <type_traits>
#include <cassert>

#include "Rgb.h"
#include "Hsv.h"
#include "Hsl.h"
#include "Hsi.h"
#include "ColorCast.h"
#include "ConvertUtil.h"

namespace color {

template <typename T,
        typename std::enable_if_t<std::is_floating_point<T>::value, int> = 0>
inline Rgb<T> to_rgb(const Hsv<T>& from) {
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
inline Rgb<T> to_rgb(const Hsv<T>& from) {
    return color_cast<T>(to_rgb(color_cast<FloatType>(from)));
}

template <typename T,
        typename std::enable_if_t<std::is_floating_point<T>::value, int> = 0>
inline Rgba<T> to_rgb(const Hsva<T>& from) {
    return Rgba<T>(to_rgb(from.color()), from.alpha());
}

template <typename T,
        typename FloatType = float,
        typename std::enable_if_t<std::is_integral<T>::value, int> = 0>
inline Rgba<T> to_rgb(const Hsva<T>& from) {
    return Rgba<T>(to_rgb<T, FloatType>(from.color()), from.alpha());
}

template <typename T,
        typename std::enable_if_t<std::is_floating_point<T>::value, int> = 0>
inline Rgb<T> to_rgb(const Hsl<T>& from) {
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
inline Rgb<T> to_rgb(const Hsl<T>& from) {
    return color_cast<T>(to_rgb(color_cast<FloatType>(from)));
}

template <typename T,
        typename std::enable_if_t<std::is_floating_point<T>::value, int> = 0>
inline Rgba<T> to_rgb(const Hsla<T>& from) {
    return Rgba<T>(to_rgb(from.color()), from.alpha());
}

template <typename T,
        typename FloatType = float,
        typename std::enable_if_t<std::is_integral<T>::value, int> = 0>
inline Rgba<T> to_rgb(const Hsla<T>& from) {
    return Rgba<T>(to_rgb<T, FloatType>(from.color()), from.alpha());
}

enum class HsiOutOfGamutMode { Clip, Preserve };

namespace details {
template <typename T>
inline void preserve_oog_mode(const Hsi<T>& from, T& c1, T& c2, T& c3) {}
template <typename T>
inline void clip_oog_mode(const Hsi<T>& from, T& c1, T& c2, T& c3) {
    c1 = std::min(c1, T(1.0));
    c2 = std::min(c2, T(1.0));
    c3 = std::min(c3, T(1.0));
}
}

template <typename T,
        typename FnType,
        typename std::enable_if_t<std::is_floating_point<T>::value, int> = 0>
inline Rgb<T> to_rgb(const Hsi<T>& from, const FnType& gamut_fn) {
    T hue_angle = from.hue() * Radians<T>::period_length();
    hue_angle = std::fmod(hue_angle, T(2.0 * PI<T> / 3.0));

    T c1 = from.intensity() * (T(1.0) - from.saturation());
    T c2 = from.intensity() *
            (T(1.0) +
                    (from.saturation() * std::cos(hue_angle)) /
                            (std::cos(PI<T> / 3.0 - hue_angle)));
    T c3 = T(3.0) * from.intensity() - (c1 + c2);

    gamut_fn(from, c1, c2, c3);

    if(from.hue() < 1.0 / 3.0) {
        return Rgb<T>(c2, c3, c1);
    } else if(from.hue() < 2.0 / 3.0) {
        return Rgb<T>(c1, c2, c3);
    } else {
        return Rgb<T>(c3, c1, c2);
    }
}

template <typename T>
inline Rgb<T> to_rgb(const Hsi<T>& from,
        HsiOutOfGamutMode gamut_fix_mode = HsiOutOfGamutMode::Clip) {
    switch(gamut_fix_mode) {
    case HsiOutOfGamutMode::Clip: {
        return to_rgb(from, details::clip_oog_mode<T>);
    }
    case HsiOutOfGamutMode::Preserve: {
        return to_rgb(from, details::preserve_oog_mode<T>);
    }
    default: {
        assert(false && "Invalid HsiOutOfGamutMode provided.");
        return Rgb<T>(0, 0, 0);
    }
    }
}
}

#endif
