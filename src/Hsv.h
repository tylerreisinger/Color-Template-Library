/** \file
 *  Defines a Hsv class and other utilities for dealing with the HSV color
 * model.
 */
#ifndef COLOR_HSV_H_
#define COLOR_HSV_H_

#include "CylindricalColor.h"

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

    Hsv(const Hsv& other) = default;
    Hsv(Hsv&& other) noexcept = default;
    Hsv& operator=(const Hsv& other) = default;
    Hsv& operator=(Hsv&& other) noexcept = default;

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

    /// Return a new Hsv instance with all components set to \a value.
    static constexpr Hsv<T> broadcast(T value) {
        return Hsv<T>(value, value, value);
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
    stream << "HSV(" << rhs._hue << ", " << rhs._saturation << ", "
           << rhs.value_channel() << ")";
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

template <typename T>
inline T chroma(const Hsv<T>& color) {
    return color.saturation() * color.value();
}

template <typename T>
constexpr inline void swap(Hsv<T>& lhs, Hsv<T>& rhs) {
    swap(lhs._hue, rhs._hue);
    swap(lhs._saturation, rhs._saturation);
    swap(lhs._c3, rhs._c3);
}
}

#endif
