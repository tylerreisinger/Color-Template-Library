#ifndef COLOR_HSL_H_
#define COLOR_HSL_H_

#include <cmath>
#include <cassert>
#include <type_traits>

#include "CylindricalColor.h"
#include "ConvertUtil.h"
#include "ColorCast.h"

namespace color {

template <typename T>
class Hsl;
template <typename T, template <typename> class Color>
class Alpha;

template <typename T>
constexpr void swap(Hsl<T>& lhs, Hsl<T>& rhs);

/// Convenience type for an Hsl color with an alpha channel.
template <typename T>
using Hsla = Alpha<T, Hsl>;

/// Synonym for Hsl, some people prefer to call
///'lightness' 'brightness'.
template <typename T>
using Hsb = Hsl<T>;
/// Synonym for Hsla.
template <typename T>
using Hsba = Hsla<T>;

template <typename T>
class Hsl : public CylindricalColor<T, Hsl<T>> {
    using Base = CylindricalColor<T, Hsl<T>>;
    friend class CylindricalColor<T, Hsl<T>>;

public:
    static constexpr int num_channels = Base::num_channels;

    using ElementType = typename Base::ElementType;
    using TupleType = typename Base::TupleType;
    using ChannelTupleType = typename Base::ChannelTupleType;
    using ConstChannelTupleType = typename Base::ConstChannelTupleType;

    ~Hsl() = default;
    constexpr Hsl(const Hsl& other) = default;
    constexpr Hsl(Hsl&& other) noexcept = default;
    constexpr Hsl& operator=(const Hsl& other) = default;
    constexpr Hsl& operator=(Hsl&& other) noexcept = default;

    constexpr Hsl() : Base() {}
    constexpr Hsl(T hue, T saturation, T lightness)
        : Base(hue, saturation, lightness) {}

    template <template <typename> class Angle, typename U>
    constexpr Hsl(Angle<U> hue, T saturation, T lightness)
        : Base(hue, saturation, lightness) {}

    explicit constexpr Hsl(const T* values)
        : Hsl(values[0], values[1], values[2]) {}
    explicit constexpr Hsl(const std::array<T, num_channels>& values)
        : Hsl(values.data()) {}
    explicit constexpr Hsl(const TupleType& values)
        : Hsl(std::get<0>(values), std::get<1>(values), std::get<2>(values)) {}

    constexpr BoundedChannel<T>& lightness_channel() { return _c3; }
    constexpr BoundedChannel<T> lightness_channel() const { return _c3; }
    constexpr BoundedChannel<T>& brightness_channel() {
        return lightness_channel;
    }
    constexpr BoundedChannel<T> brightness_channel() const {
        return lightness_channel;
    }

    constexpr T& lightness() { return _c3.value; }
    constexpr T lightness() const { return _c3.value; }
    /// Synonym for lightness().
    constexpr T& brightness() { return lightness(); }
    /// Synonym for lightness().
    constexpr T brightness() const { return lightness(); }

    constexpr Hsl<T>& set_lightness(T value) {
        _c3.value = value;
        return *this;
    }
    /// Synonym for set_lightness(T).
    constexpr Hsl<T>& set_brightness(T value) { return set_lightness(value); }

    friend constexpr void swap<T>(Hsl<T>& lhs, Hsl<T>& rhs);

protected:
    using Base::_hue;
    using Base::_saturation;
    using Base::_c3;
};

template <typename T>
inline std::ostream& operator<<(std::ostream& stream, const Hsl<T>& rhs) {
    stream << "Hsl(" << rhs.hue_channel() << ", " << rhs.saturation_channel()
           << ", " << rhs.lightness_channel() << ")";
    return stream;
}

template <typename T>
constexpr inline void swap(Hsl<T>& lhs, Hsl<T>& rhs) {
    swap(lhs._hue, rhs._hue);
    swap(lhs._saturation, rhs._saturation);
    swap(lhs._c3, rhs._c3);
}

template <typename T,
        typename = std::enable_if_t<std::is_floating_point<T>::value>>
inline T chroma(const Hsl<T>& color) {
    return (1.0 - std::abs(2.0 * color.lightness() - 1.0)) * color.saturation();
}

// Conversion functions

template <typename T>
class Rgb;
template <typename T>
using Rgba = Alpha<T, Rgb>;

template <typename T,
        typename std::enable_if_t<std::is_floating_point<T>::value, int> = 0>
inline Hsl<T> to_hsl(const Rgb<T>& from) {
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
    auto saturation =
            chroma / (T(1.0) - std::abs(T(2.0) * lightness - T(1.0)) + EPSILON);

    return Hsl<T>(hue, saturation, lightness);
}

template <typename T,
        typename FloatType = float,
        typename std::enable_if_t<std::is_integral<T>::value, int> = 0>
inline Hsl<T> to_hsl(const Rgb<T>& from) {
    return color_cast<T>(to_hsl(color_cast<FloatType>(from)));
}

template <typename T,
        typename std::enable_if_t<std::is_floating_point<T>::value, int> = 0>
inline Hsla<T> to_hsl(const Rgba<T>& from) {
    return Hsla<T>(to_hsl(from.color()), from.alpha());
}

template <typename T,
        typename FloatType = float,
        typename std::enable_if_t<std::is_integral<T>::value, int> = 0>
inline Hsla<T> to_hsl(const Rgba<T>& from) {
    return Hsla<T>(to_hsl<T, FloatType>(from.color()), from.alpha());
}

template <typename T,
        typename std::enable_if_t<std::is_floating_point<T>::value, int> = 0>
inline Rgb<T> to_rgb(const Hsl<T>& from) {
    Rgb<T> out;

    T hue_frac;
    const auto hue_seg = details::decompose_hue(from.hue(), hue_frac);

    const auto chroma = color::chroma(from);
    const auto color_min_bound = from.lightness() - T(0.5) * chroma;
    const auto color_max_bound = color_min_bound + chroma;

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
}

#endif
