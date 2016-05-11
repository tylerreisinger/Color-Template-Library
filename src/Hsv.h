/** \file
 *  Defines a Hsv class and other utilities for dealing with the HSV color
 * model.
 */
#ifndef COLOR_HSV_H_
#define COLOR_HSV_H_

#include "CylindricalColor.h"

#include <cassert>
#include <ostream>
#include <tuple>
#include <type_traits>

#include "ConvertUtil.h"
#include "Channel.h"
#include "ColorCast.h"

namespace color {

///\cond forward_decl
template <typename T>
class Hsv;
template <typename T, template <typename> class Color>
class Alpha;
///\endcond

/// Convenience type for Hsv colors with an alpha channel.
template <typename T>
using Hsva = Alpha<T, Hsv>;

template <typename T>
std::ostream& operator<<(std::ostream& stream, const Hsv<T>& rhs);

template <typename T>
constexpr void swap(Hsv<T>& lhs, Hsv<T>& rhs);

/** A color represented by a hue, saturation and value.
 *  HSV is a straightforward transformation from RGB
 *  and is part of the same color space as the corresponding
 *  RGB color.
 *
 *  ## Component Format:
 *
 *  * data()[0]: Hue
 *  * data()[1]: Saturation
 *  * data()[2]: Value
 */
template <typename T>
class Hsv: public CylindricalColor<T, Hsv<T>> {
    using Base = CylindricalColor<T, Hsv<T>>;
    friend class CylindricalColor<T, Hsv<T>>;
public:
    static constexpr int num_channels = Base::num_channels;

    using ElementType = typename Base::ElementType;
    using TupleType = typename Base::TupleType;
    using ChannelTupleType = typename Base::ChannelTupleType;
    using ConstChannelTupleType = typename Base::ConstChannelTupleType;

    ~Hsv() = default;

    constexpr Hsv(const Hsv& other) = default;
    constexpr Hsv(Hsv&& other) noexcept = default;
    constexpr Hsv& operator=(const Hsv& other) = default;
    constexpr Hsv& operator=(Hsv&& other) noexcept = default;

    /// Construct an Hsv instance with all components set to 0.
    constexpr Hsv(): Base() {}
    /// Construct an Hsv instance with specific component values.
    constexpr Hsv(T hue, T saturation, T value)
        : Base(hue, saturation, value) {}
    template <template <typename> class Angle, typename U>
    constexpr Hsv(Angle<U> hue, T saturation, T value)
        : Base(hue, saturation, value) {}

    /** Construct an Hsv instance from an array of values.
     *  \a values must contain at least three values, and
     *  the first three values will be copied into the
     *  corresponding components of the Hsv instance
     *  in the data() component order.
     */
    explicit constexpr Hsv(const T* values)
        : Hsv(values[0], values[1], values[2]) {}

    /**Construct an Hsv instance from an array of values.
     * Functionally equivalent to Hsv(const T*).
     */
    explicit constexpr Hsv(const std::array<T, num_channels>& values)
        : Hsv(values.data()) {}
    /** Construct an Hsv instance from a tuple of component values.
     */
    explicit constexpr Hsv(const TupleType& values)
        : Hsv(std::get<0>(values), std::get<1>(values), std::get<2>(values)) {}

    constexpr BoundedChannel<T>& value_channel() { return _c3; }
    constexpr BoundedChannel<T> value_channel() const { return _c3; }

    constexpr T& value() { return _c3.value; }
    constexpr T value() const { return _c3.value; }

    constexpr Hsv<T>& set_value(T value) {
        _c3.value = value;
        return *this;
    }

    friend std::ostream& operator<<<T>(std::ostream& stream, const Hsv<T>& rhs);
    friend void swap<T>(Hsv<T>& lhs, Hsv<T>& rhs);

protected:
    using Base::_hue;
    using Base::_saturation;
    using Base::_c3;

private:
};

/** Print an Hsv instance to a stream.
 *  Output is of the form `HSV(hue, sat, value)`.
 */
template <typename T>
inline std::ostream& operator<<(std::ostream& stream, const Hsv<T>& rhs) {
    stream << "Hsv(" << rhs._hue << ", " << rhs._saturation << ", "
           << rhs.value_channel() << ")";
    return stream;
}

template <typename T,
        typename = std::enable_if_t<std::is_floating_point<T>::value>>
constexpr inline T chroma(const Hsv<T>& color) {
    return color.saturation() * color.value();
}

template <typename T>
constexpr inline void swap(Hsv<T>& lhs, Hsv<T>& rhs) {
    swap(lhs._hue, rhs._hue);
    swap(lhs._saturation, rhs._saturation);
    swap(lhs._c3, rhs._c3);
}

// Conversion functions

template <typename T>
class Rgb;
template <typename T>
using Rgba = Alpha<T, Rgb>;

template <typename T,
        typename std::enable_if_t<std::is_floating_point<T>::value, int> = 0>
inline Hsv<T> to_hsv(const Rgb<T>& from) {
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

// Specialization for integer-typed channels.
template <typename T,
        typename FloatType = float,
        typename std::enable_if_t<std::is_integral<T>::value, int> = 0>
inline Hsv<T> to_hsv(const Rgb<T>& from) {
    return color_cast<T>(to_hsv(color_cast<FloatType>(from)));
}

template <typename T,
        typename std::enable_if_t<std::is_floating_point<T>::value, int> = 0>
inline Hsva<T> to_hsv(const Rgba<T>& from) {
    return Hsva<T>(to_hsv(from.color()), from.alpha());
}

template <typename T,
        typename FloatType = float,
        typename std::enable_if_t<std::is_integral<T>::value, int> = 0>
inline Hsva<T> to_hsv(const Rgba<T>& from) {
    return Hsva<T>(to_hsv<T, FloatType>(from.color()), from.alpha());
}

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
}

#endif
