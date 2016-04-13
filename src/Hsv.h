/** \file
 *  Defines a Hsv class and other utilities for dealing with the HSV color
 * model.
 */
#ifndef COLOR_HSV_H_
#define COLOR_HSV_H_

#include <ostream>
#include <tuple>

#include "Channel.h"

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
class Hsv {
public:
    static constexpr int num_channels = 3;

    using ElementType = T;
    using TupleType = std::tuple<T, T, T>;
    using ChannelTupleType = std::tuple<PeriodicChannel<T>&,
            BoundedChannel<T>&,
            BoundedChannel<T>&>;
    using ConstChannelTupleType =
            std::tuple<PeriodicChannel<T>, BoundedChannel<T>, BoundedChannel<T>>;

    ~Hsv() = default;

    Hsv(const Hsv& other) = default;
    Hsv(Hsv&& other) noexcept = default;
    Hsv& operator=(const Hsv& other) = default;
    Hsv& operator=(Hsv&& other) noexcept = default;

    /// Construct an Hsv instance with all components set to 0.
    constexpr Hsv() : _hue(T(0)), _saturation(T(0)), _value(T(0)) {}
    /// Construct an Hsv instance with specific component values.
    constexpr Hsv(T hue, T saturation, T value)
        : _hue(PeriodicChannel<T>(hue).wrap_endpoint()),
          _saturation(saturation), _value(value) {}
    template <template <typename> class Angle, typename U>
    constexpr Hsv(Angle<U> hue, T saturation, T value)
        : Hsv(hue.to_normalized_coordinate(), saturation, value) {}

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

    /// Strict equality of two Hsv instances.
    constexpr bool operator==(const Hsv<T>& rhs) const {
        return (_hue == rhs._hue && _saturation == rhs._saturation &&
                _value == rhs._value);
    }
    /// Strict inequality of two Hsv instances.
    constexpr bool operator!=(const Hsv<T>& rhs) const {
        return !(*this == rhs);
    }

    /// Return a pointer to the internal array of components.
    constexpr T* data() { return reinterpret_cast<T*>(this); }

    /** Return a pointer to the internal array of components.
     */
    constexpr const T* data() const { return reinterpret_cast<const T*>(this); }

    /// Clamp each component to the range `[min, max]`.
    constexpr Hsv<T> clamp(T min, T max) const {
        return Hsv<T>(_hue.clamp(min, max),
                _saturation.clamp(min, max),
                _value.clamp(min, max));
    }

    /** Return a copy of the color with all channels clamped to the "normal"
     *  range. For integer components, values are always normalized, but
     *  floating point components are normalized to the range `[0, 1]`. For hue,
     *  it will "wrap" in a periodic fashion values outside the range.
     */
    constexpr Hsv<T> normalize() const {
        return Hsv<T>(
                _hue.normalize(), _saturation.normalize(), _value.normalize());
    }

    template <typename Pos>
    constexpr Hsv<T> lerp_flat(const Hsv<T>& end, Pos pos) const {
        return Hsv<T>(_hue.lerp_flat(end.hue(), pos),
                _saturation.lerp_flat(end.saturation(), pos),
                _value.lerp_flat(end.value(), pos));
    }

    template <typename Pos>
    constexpr Hsv<T> lerp(const Hsv<T>& end, Pos pos) const {
        return Hsv<T>(_hue.lerp(end.hue(), pos),
                _saturation.lerp(end.saturation(), pos),
                _value.lerp(end.value(), pos));
    }

    /** Return the inverse of the color.
     *  The inverse is the maximum value minus the current value.
     */
    constexpr Hsv<T> inverse() const {
        return Hsv<T>(_hue.inverse(), _saturation.inverse(), _value.inverse());
    }

    /** Return a copy of the color with all components scaled by a constant
     * factor.
     */
    template <typename U,
            typename std::enable_if_t<std::is_floating_point<U>::value, int> = 0>
    constexpr Hsv<T> scale(U factor) const {
        return Hsv<T>(_hue.value * factor,
                _saturation.value * factor,
                _value.value * factor);
    }

    /** Return the value of hue as an angle.
     *
     *  The angle will be represented as the
     *  corresponding `Angle` class, e.g.
     *  Degrees or Radians. Any class
     *  can be used as an angle type
     *  as long as it defines a static
     *  `from_normalized_coordinate` function
     *  and accepts a single template parameter.
     *
     *  For floating point colors, only the
     *  Angle type must be specified. The internal
     *  type for Angle will match the ElementType
     *  of the color. For integer colors, or to
     *  specifically use a different datatype in the
     *  Angle, a second type parameter should be specified
     *  giving this type. Using an integral type for U is
     *  disallowed.
     *
     *  Example:
     *  ```
     *  auto float_color = Hsv<float>(0, 0, 0);
     *  auto angle_in_degrees = float_color.hue_angle<Degrees>().value;
     *
     *  auto int_color = Hsv<uint8_t>(0, 0, 0);
     *  angle_in_degrees = int_color.hue_angle<Degrees, float>().value;
     *  ```
     */
    template <template <typename> class Angle, typename U = T>
    constexpr Angle<U> hue_angle() const {
        return _hue.template get_angle<Angle, U>();
    }

    template <template <typename> class Angle, typename U>
    constexpr Hsv<T>& set_hue_angle(Angle<U> angle) {
        _hue.set_angle(angle);
        return *this;
    }

    constexpr T& hue() { return _hue.value; }
    constexpr T& saturation() { return _saturation.value; }
    constexpr T& value() { return _value.value; }
    constexpr T hue() const { return _hue.value; }
    constexpr T saturation() const { return _saturation.value; }
    constexpr T value() const { return _value.value; }
    constexpr Hsv<T>& set_hue(T value) {
        _hue.value = value;
        return *this;
    }
    constexpr Hsv<T>& set_saturation(T value) {
        _saturation.value = value;
        return *this;
    }
    constexpr Hsv<T>& set_value(T value) {
        _value.value = value;
        return *this;
    }

    /// Return channel values as an tuple.
    constexpr TupleType as_tuple() const {
        return std::make_tuple(_hue.value, _saturation.value, _value.value);
    }

    /// Return channel values as an array.
    constexpr std::array<T, num_channels> as_array() const {
        return {_hue.value, _saturation.value, _value.value};
    }

    constexpr ChannelTupleType channel_tuple() {
        return std::make_tuple(_hue, _saturation, _value);
    }

    constexpr ConstChannelTupleType channel_tuple() const {
        return std::make_tuple(_hue, _saturation, _value);
    }

    /// Return a new Hsv instance with all components set to \a value.
    static constexpr Hsv<T> broadcast(T value) {
        return Hsv<T>(value, value, value);
    }

    friend std::ostream& operator<<<T>(std::ostream& stream, const Hsv<T>& rhs);

private:
    PeriodicChannel<T> _hue;
    BoundedChannel<T> _saturation;
    BoundedChannel<T> _value;
};

/** Print an Hsv instance to a stream.
 *  Output is of the form `HSV(hue, sat, value)`.
 */
template <typename T>
inline std::ostream& operator<<(std::ostream& stream, const Hsv<T>& rhs) {
    stream << "HSV(" << rhs._hue << ", " << rhs._saturation << ", "
           << rhs._value << ")";
    return stream;
}

template <typename T>
Hsv<T> operator+(const Hsv<T>& lhs, const Hsv<T>& rhs) {
    return Hsv<T>(lhs.hue() + rhs.hue(),
            lhs.saturation() + rhs.saturation(),
            lhs.value() + rhs.value());
}

template <typename T>
Hsv<T> operator-(const Hsv<T>& lhs, const Hsv<T>& rhs) {
    return Hsv<T>(lhs.hue() - rhs.hue(),
            lhs.saturation() - rhs.saturation(),
            lhs.value() - rhs.value());
}
}

#endif
