#ifndef COLOR_HSI_H_
#define COLOR_HSI_H_

#include "CylindricalColor.h"
#include "ConvertUtil.h"
#include "ColorCast.h"

#include <array>
#include <cassert>
#include <tuple>

namespace color {

template <typename T>
class Hsi;
template <typename T, template <typename> class Color>
class Alpha;

template <typename T>
constexpr void swap(Hsi<T>& lhs, Hsi<T>& rhs);

template <typename T>
using Hsia = Alpha<T, Hsi>;

template <typename T>
class Hsi : public CylindricalColor<T, Hsi<T>> {
    using Base = CylindricalColor<T, Hsi<T>>;
    friend class CylindricalColor<T, Hsi<T>>;

public:
    static constexpr int num_channels = Base::num_channels;

    using ElementType = typename Base::ElementType;
    using TupleType = typename Base::TupleType;
    using ChannelTupleType = typename Base::ChannelTupleType;
    using ConstChannelTupleType = typename Base::ConstChannelTupleType;

    ~Hsi() = default;
    constexpr Hsi(const Hsi& other) = default;
    constexpr Hsi(Hsi&& other) noexcept = default;
    constexpr Hsi& operator=(const Hsi& other) = default;
    constexpr Hsi& operator=(Hsi&& other) noexcept = default;

    constexpr Hsi() : Base() {}
    constexpr Hsi(T hue, T saturation, T intensity)
        : Base(hue, saturation, intensity) {}

    template <template <typename> class Angle, typename U>
    constexpr Hsi(Angle<T> hue, T saturation, T intensity)
        : Base(hue, saturation, intensity) {}
    explicit constexpr Hsi(const T* values)
        : Hsi(values[0], values[1], values[2]) {}
    explicit constexpr Hsi(const std::array<T, num_channels>& values)
        : Hsi(values.data()) {}
    explicit constexpr Hsi(const TupleType& values)
        : Hsi(std::get<0>(values), std::get<1>(values), std::get<2>(values)) {}

    constexpr BoundedChannel<T>& intensity_channel() { return _c3; }
    constexpr BoundedChannel<T> intensity_channel() const { return _c3; }
    constexpr T& intensity() { return _c3.value; }
    constexpr T intensity() const { return _c3.value; }

    constexpr Hsi<T>& set_intensity(T value) {
        _c3.value = value;
        return *this;
    }

    /** Compute the maximum intensity value that will keep this color in
     *  gamut. Giving a color with the same hue and saturation as this
     *  an intensity greater than the returned value will make the
     *  color no longer correspond to a valid in-range RGB color.
     *
     *  The returned value is computed by projecting the color onto
     *  the surface that divides the in-gamut region to the out-of-gamut
     *  region. An approximation to hue is used, which makes this function
     *  considerably faster than converting and testing, but at the expense
     *  of some accuracy.
     *
     *  The maximum error tolerance is +/- 0.015 of the RGB result, so a
     *  color with one RGB channel value of at most 1.015 could potentially
     *  appear in-gamut with regards to this function.
     */
    constexpr T max_in_gamut_intensity() const {
        const T scaled_hue = _hue.value * 3.0;
        const int seg = static_cast<int>(scaled_hue);
        const T hue_param = T(1.0 / 3.0) * (scaled_hue - seg * T(1.0));

        // hue_alpha is the linear interpolation factor along
        // the changing RGB coordinate in the hue function.
        T hue_alpha = 0.0;
        if(hue_param <= T(1.0 / 6.0)) {
            hue_alpha = T(6.0) * hue_param;
        } else {
            hue_alpha = T(6.0) * (T(1.0 / 3.0) - hue_param);
        }

        const auto s = _saturation.value;

        // We are looking for a maximum intensity, so
        // assume the highest channel is always 1.0.
        const T max_channel = 1.0;

        const T min_channel =
                ((hue_alpha + T(1.0)) * max_channel * (s - T(1.0))) /
                (hue_alpha * (s - T(1.0)) - 2 * s - T(1.0));

        const T max_intensity = T(1.0 / 3.0) *
                (max_channel + min_channel + hue_alpha * max_channel +
                                        (T(1.0) - hue_alpha) * min_channel);

        return max_intensity;
    }

    /// Compare max_in_gamut_intensity() to intensity(), returning true if
    /// intensity() is smaller.
    constexpr bool is_in_gamut() const {
        return intensity() <= max_in_gamut_intensity();
    }

    friend constexpr void swap<T>(Hsi<T>& lhs, Hsi<T>& rhs);

protected:
    using Base::_hue;
    using Base::_saturation;
    using Base::_c3;
};

template <typename T>
inline std::ostream& operator<<(std::ostream& stream, const Hsi<T>& rhs) {
    stream << "Hsi(" << rhs.hue_channel() << ", " << rhs.saturation_channel()
           << ", " << rhs.intensity_channel() << ")";
    return stream;
}

template <typename T>
constexpr inline void swap(Hsi<T>& lhs, Hsi<T>& rhs) {
    swap(lhs._hue, rhs._hue);
    swap(lhs._saturation, rhs._saturation);
    swap(lhs._c3, rhs._c3);
}

// Conversion Functions

template <typename T>
class Rgb;
template <typename T>
using Rgba = Alpha<T, Rgb>;

template <typename T,
        typename std::enable_if_t<std::is_floating_point<T>::value, int> = 0>
inline Hsi<T> to_hsi(const Rgb<T>& from) {
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
inline Hsi<T> to_hsi(const Rgb<T>& from) {
    return color_cast<T>(to_hsl(color_cast<FloatType>(from)));
}

template <typename T,
        typename std::enable_if_t<std::is_floating_point<T>::value, int> = 0>
inline Hsia<T> to_hsi(const Rgba<T>& from) {
    return Hsia<T>(to_hsi(from.color()), from.alpha());
}

template <typename T,
        typename FloatType = float,
        typename std::enable_if_t<std::is_integral<T>::value, int> = 0>
inline Hsia<T> to_hsi(const Rgba<T>& from) {
    return Hsia<T>(to_hsi<T, FloatType>(from.color()), from.alpha());
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
