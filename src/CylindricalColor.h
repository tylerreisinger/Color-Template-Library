/** \file
 *  Defines a base class for Hsv, Hsl, and Hsi colors.
 */
#ifndef COLOR_CYLINDRICALCOLOR_H_
#define COLOR_CYLINDRICALCOLOR_H_

#include <tuple>

#include "Channel.h"

namespace color {

/** Base type for HS* colors.
 *  CylindricalColor allows for significant code sharing between all
 *  HS* colors. It is not intended to be used directly.
 */
template<typename T, typename Subclass>
class CylindricalColor {
public:
    static constexpr int num_channels = 3;

    using ElementType = T;
    using TupleType = std::tuple<T, T, T>;
    using ChannelTupleType = std::tuple<PeriodicChannel<T>&,
            BoundedChannel<T>&,
            BoundedChannel<T>&>;
    using ConstChannelTupleType =
            std::tuple<PeriodicChannel<T>, BoundedChannel<T>, BoundedChannel<T>>;

    ~CylindricalColor() = default;

    constexpr CylindricalColor(const CylindricalColor& other) = default;
    constexpr CylindricalColor(CylindricalColor&& other) noexcept = default;
    constexpr CylindricalColor& operator=(
            const CylindricalColor& other) = default;
    constexpr CylindricalColor& operator=(
            CylindricalColor&& other) noexcept = default;

    /// Return a pointer to the internal array of components.
    constexpr T* data() { return reinterpret_cast<T*>(this); }

    /** Return a pointer to the internal array of components.
     */
    constexpr const T* data() const { return reinterpret_cast<const T*>(this); }

    /** Clamp each component to the range `[min, max]`.
     *  Internally, calls clamp on each channel.
     */
    constexpr Subclass clamp(T min, T max) const {
        return Subclass(_hue.clamp(min, max),
                _saturation.clamp(min, max),
                _c3.clamp(min, max));
    }

    /** Return a copy of the color with all channels clamped to the "normal"
     *  range. For integer components, values are always normalized, but
     *  floating point components are normalized to the range `[0, 1]`. For hue,
     *  it will "wrap" in a periodic fashion values outside the range.
     */
    constexpr Subclass normalize() const {
        return Subclass(
                _hue.normalize(), _saturation.normalize(), _c3.normalize());
    }

    /** Return the inverse of the color.
     *  The inverse is the maximum value minus the current value.
     */
    constexpr Subclass inverse() const {
        return Subclass(_hue.inverse(), 
                _saturation.inverse(), _c3.inverse());
    }

    template <typename Pos>
    constexpr Subclass lerp_flat(const Subclass& end, Pos pos) const {
        return Subclass(_hue.lerp_flat(end.hue(), pos),
                _saturation.lerp_flat(end.saturation(), pos),
                _c3.lerp_flat(end._c3.value, pos));
    }

    template <typename Pos>
    constexpr Subclass lerp(const Subclass& end, Pos pos) const {
        return Subclass(_hue.lerp(end.hue(), pos),
                _saturation.lerp(end.saturation(), pos),
                _c3.lerp(end._c3.value, pos));
    }

    /** Return a copy of the color with all components scaled by a constant
     * factor.
     */
    template <typename U,
            typename std::enable_if_t<std::is_floating_point<U>::value, int> = 0>
    constexpr Subclass scale(U factor) const {
        return Subclass(_hue.value * factor,
                _saturation.value * factor,
                _c3.value * factor);
    }

    template <typename PosType = std::
                      conditional_t<std::is_floating_point<T>::value, T, float>,
            typename std::enable_if_t<std::is_floating_point<PosType>::value,
                    int> = 0>
    PosType squared_distance(const Subclass& rhs) const {
        auto d_z = (_c3.value - rhs._c3.value);

        auto d_mag_1 = saturation() * saturation();
        auto d_mag_2 = rhs.saturation() * rhs.saturation();

        auto h1_cos = std::cos(hue_angle<Radians>().value);
        auto h1_sin = std::sin(hue_angle<Radians>().value);
        auto h2_cos = std::cos(rhs.template hue_angle<Radians>().value);
        auto h2_sin = std::sin(rhs.template hue_angle<Radians>().value);

        auto x1 = 0.5 * d_mag_1 * h1_cos;
        auto y1 = 0.5 * d_mag_1 * h1_sin;
        auto x2 = 0.5 * d_mag_2 * h2_cos;
        auto y2 = 0.5 * d_mag_2 * h2_sin;

        auto dx = x1 - x2;
        auto dy = y1 - y2;

        return PosType(1.0 / 2.0) * (dx * dx + dy * dy + d_z* d_z);
    }

    template <typename PosType = std::
                      conditional_t<std::is_floating_point<T>::value, T, float>>
    PosType distance(const Subclass& rhs) const {
        return std::sqrt(squared_distance(rhs));
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
    constexpr Subclass& set_hue_angle(Angle<U> angle) {
        _hue.set_angle(angle);
        return static_cast<Subclass&>(*this);
    }

    constexpr PeriodicChannel<T>& hue_channel() { return _hue; }
    constexpr BoundedChannel<T>& saturation_channel() { return _saturation; }
    constexpr PeriodicChannel<T> hue_channel() const { return _hue; }
    constexpr BoundedChannel<T> saturation_channel() const { return _saturation; }
    constexpr T& hue() { return _hue.value; }
    constexpr T hue() const { return _hue.value; }
    constexpr T saturation() const { return _saturation.value; }
    constexpr T& saturation() { return _saturation.value; }

    constexpr Subclass& set_hue(T value) {
        _hue.value = value;
        return static_cast<Subclass&>(*this);
    }
    constexpr Subclass& set_saturation(T value) {
        _saturation.value = value;
        return *this;
    }

    /// Return channel values as an tuple.
    constexpr TupleType as_tuple() const {
        return std::make_tuple(hue(), saturation(), _c3.value);
    }

    /// Return channel values as an array.
    constexpr std::array<T, num_channels> as_array() const {
        return {hue(), saturation(), _c3.value};
    }

    constexpr ChannelTupleType channel_tuple() {
        return std::make_tuple(_hue, _saturation, _c3);
    }

    constexpr ConstChannelTupleType channel_tuple() const {
        return std::make_tuple(_hue, _saturation, _c3);
    }

    /// Return a new color with all components set to \a value.
    static constexpr Subclass broadcast(T value) {
        return Subclass(value, value, value);
    }

    /// Strict equality of two colors.
    constexpr bool operator==(const Subclass& rhs) const {
        return (_hue == rhs._hue && _saturation == rhs._saturation &&
                _c3 == rhs._c3);
    }
    /// Strict inequality of two colors.
    constexpr bool operator!=(const Subclass& rhs) const {
        return !(*this == rhs);
    }


protected:
    PeriodicChannel<T> _hue;
    BoundedChannel<T> _saturation;
    BoundedChannel<T> _c3;

    constexpr CylindricalColor():
        _hue(T(0)), _saturation(T(0)), _c3(T(0)) {}

    constexpr CylindricalColor(T hue, T saturation, T c3):
        _hue(hue), _saturation(saturation), _c3(c3) {}

    template <template <typename> class Angle, typename U>
    constexpr CylindricalColor(Angle<U> hue, T saturation, T c3)
        : CylindricalColor(hue.to_normalized_coordinate(), saturation, c3) {}

};

}

#endif
