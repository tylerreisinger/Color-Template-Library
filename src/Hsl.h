#ifndef COLOR_HSL_H_
#define COLOR_HSL_H_

#include <cmath>
#include <type_traits>

#include "CylindricalColor.h"

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
    Hsl(const Hsl& other) = default;
    Hsl(Hsl&& other) noexcept = default;
    Hsl& operator=(const Hsl& other) = default;
    Hsl& operator=(Hsl&& other) noexcept = default;

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
    stream << "HSL(" << rhs.hue_channel() << ", " << rhs.saturation_channel()
           << ", " << rhs.lightness_channel() << ")";
    return stream;
}

template <typename T>
constexpr inline Hsl<T> operator+(const Hsl<T>& lhs, const Hsl<T>& rhs) {
    return Hsl<T>(lhs.hue() + rhs.hue(),
            lhs.saturation() + rhs.saturation(),
            lhs.lightness() + rhs.lightness());
}

template <typename T>
constexpr inline Hsl<T> operator-(const Hsl<T>& lhs, const Hsl<T>& rhs) {
    return Hsl<T>(lhs.hue() - rhs.hue(),
            lhs.saturation() - rhs.saturation(),
            lhs.lightness() - rhs.lightness());
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
}

#endif
