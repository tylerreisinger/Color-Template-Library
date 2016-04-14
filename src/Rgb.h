/**\file
 * \brief Defines an Rgb class and other utilities for dealing with the RGB
 * color model.
 */

#ifndef COLOR_RGB_H_
#define COLOR_RGB_H_

#include <ostream>
#include <tuple>
#include <algorithm>

#include "Channel.h"

namespace color {

///\cond forward_decl
template <typename T, template <typename> class Color>
class Alpha;
template <typename T>
class Rgb;
///\endcond

/** Print a color to a data stream.
 *  The output is of the format `RGB(red, green, blue)`.
 */
template <typename T>
std::ostream& operator<<(std::ostream& stream, const Rgb<T>& rgb);
template <typename T>
constexpr Rgb<T> operator+(const Rgb<T>& lhs, const Rgb<T>& rhs);
template <typename T>
constexpr Rgb<T> operator-(const Rgb<T>& lhs, const Rgb<T>& rhs);


/// Convenience type for Rgb colors with an alpha channel.
template <typename T>
using Rgba = Alpha<T, Rgb>;

/** A color represented by red, green and blue components.
 *
 *  ## Component Format:
 *
 *  * data()[0]: Red
 *  * data()[1]: Green
 *  * data()[2]: Blue
 */
template <typename T>
class Rgb {
public:
    static constexpr int num_channels = 3;

    using ElementType = T;
    using TupleType = std::tuple<T, T, T>;
    using ChannelTupleType = std::tuple<BoundedChannel<T>&,
            BoundedChannel<T>&,
            BoundedChannel<T>&>;
    using ConstChannelTupleType =
            std::tuple<BoundedChannel<T>, BoundedChannel<T>, BoundedChannel<T>>;

    /// Construct an Rgb instance with all components set to 0.
    constexpr Rgb() : _red(T(0)), _green(T(0)), _blue(T(0)) {}

    /// Construct an Rgb instance with specific component values.
    constexpr Rgb(T red, T green, T blue)
        : _red(red), _green(green), _blue(blue) {}

    constexpr Rgb(BoundedChannel<T> red,
            BoundedChannel<T> green,
            BoundedChannel<T> blue)
        : _red(red), _green(green), _blue(blue) {}

    /** Construct an Rgb instance from an array of values.
     *  \a values must contain at least three values, and
     *  the first three values will be copied into the
     *  corresponding components of the Rgb instance
     *  in the data() component order.
     */
    explicit constexpr Rgb(const T* values)
        : _red(values[0]), _green(values[1]), _blue(values[2]) {}

    /** Construct an Rgb instance from an array of values.
     *  Functionally equivalent to Rgb(const T*).
     */
    explicit constexpr Rgb(const std::array<T, num_channels>& values)
        : Rgb(values.data()) {}

    /// Construct an Rgb instance from a tuple of component values.
    explicit constexpr Rgb(const TupleType& values)
        : _red(std::get<0>(values)), _green(std::get<1>(values)),
          _blue(std::get<2>(values)) {}

    ~Rgb() = default;

    constexpr Rgb(const Rgb& rhs) = default;
    constexpr Rgb(Rgb&& rhs) noexcept = default;
    constexpr Rgb& operator=(const Rgb& rhs) = default;
    constexpr Rgb& operator=(Rgb&& rhs) noexcept = default;

    /** Strict equality of two Rgb instances.
     *  For floating point channels, this has all the caveats
     *  of floating point equality.
     */
    constexpr bool operator==(const Rgb<T>& rhs) const {
        return red() == rhs.red() && green() == rhs.green() &&
                blue() == rhs.blue();
    }

    /** Strict inequality of two Rgb instances.
     *  Negation of Rgb::operator ==
     */
    constexpr bool operator!=(const Rgb<T>& rhs) const {
        return !(*this == rhs);
    }

    /** Return a pointer to the internal array of components.
     *  That is, `my_color.data()[0]` is equivalent to `my_color.red()`
     *  and so on for the other components.
     */
    constexpr T* data() { return reinterpret_cast<T*>(this); }

    /// Return a pointer to the internal array of components.
    constexpr const T* data() const { return reinterpret_cast<const T*>(this); }

    /** Return a copy of the color with each channel clamped to the range
     *  `[min, max]`. For channel values above \a max, they will be set to
     *  \a max and likewise for channels below \a min, they will be set to
     *  \a min.
     */
    constexpr Rgb<T> clamp(T min, T max) const {
        return Rgb<T>(_red.clamp(min, max),
                _green.clamp(min, max),
                _blue.clamp(min, max));
    }

    /** Return a copy of the color with all channels clamped to the "normal"
     *  range. For integer components, values are always normalized, but
     *  floating point components are clamped to `[0, 1]`.
     */
    constexpr Rgb<T> normalize() const {
        return Rgb<T>(_red.normalize(), _green.normalize(), _blue.normalize());
    }

    /** Compute a new color by linearly interpolating between two endpoints.
     *
     *  \param pos The position between `*this` and \a end.
     *  A \a pos of 0 specifies `*this` while a \a pos of 1
     *  specifies \a end.
     */
    template <typename Pos>
    constexpr Rgb<T> lerp(const Rgb<T>& end, Pos pos) const {
        return Rgb<T>(_red.lerp(end._red.value, pos),
                _green.lerp(end._green.value, pos),
                _blue.lerp(end._blue.value, pos));
    }

    template <typename Pos>
    constexpr Rgb<T> lerp_flat(const Rgb<T>& end, Pos pos) const {
        return lerp(end, pos);
    }

    /** Return the inverse of the color.
     *  The inverse of a component is the maximum value minus
     *  the current value. Thus, white would go to black and vice versa.
     */
    constexpr Rgb<T> inverse() const {
        return Rgb<T>(_red.inverse(), _green.inverse(), _blue.inverse());
    }


    /** Return a copy of a color with each channel value multiplied by `factor`.
     *  If factor is larger than one, the resultant color may become
     *  un-normalized. This function does not attempt to re-normalize --
     *  call Rgb::normalize() afterward to ensure normalization.
     */
    template <typename U,
            typename std::enable_if_t<std::is_floating_point<U>::value, int> = 0>
    constexpr Rgb<T> scale(U factor) const {
        return Rgb<T>(_red.value * factor,
                _green.value * factor,
                _blue.value * factor);
    }

    constexpr Rgb<T>& set_red(T value) {
        _red.value = value;
        return *this;
    }

    constexpr Rgb<T>& set_green(T value) {
        _green.value = value;
        return *this;
    }

    constexpr Rgb<T>& set_blue(T value) {
        _blue.value = value;
        return *this;
    }

    constexpr T red() const { return _red.value; }

    constexpr T green() const { return _green.value; }

    constexpr T blue() const { return _blue.value; }

    constexpr T& red() { return _red.value; }

    constexpr T& green() { return _green.value; }

    constexpr T& blue() { return _blue.value; }

    /** Return a tuple of channel values.
     *  This can be useful for generalizing algorithms
     *  to arbitrary colors or for decomposing a color
     *  into its components.
     */
    constexpr TupleType as_tuple() const {
        return std::make_tuple(_red.value, _green.value, _blue.value);
    }

    /// Return an array of channel values.
    constexpr std::array<T, num_channels> as_array() const {
        return {_red.value, _green.value, _blue.value};
    }

    /** Return a tuple of references to the channel objects of an Rgb instance.
     *  This is potentially useful for acting differently depending on the
     *  channel kind.
     */
    constexpr ChannelTupleType channel_tuple() {
        return std::make_tuple(_red, _green, _blue);
    }

    /// A const overload of Rgb::channel_tuple().
    /// Returns a tuple of copies of the channels of an Rgb instance.
    constexpr ConstChannelTupleType channel_tuple() const {
        return std::make_tuple(_red, _green, _blue);
    }

    /// Construct an Rgb instance with all channels set to \a value.
    static constexpr Rgb<T> broadcast(T value) {
        return Rgb<T>(value, value, value);
    }

    friend std::ostream& operator<<<T>(std::ostream& stream, const Rgb<T>& rgb);

    friend Rgb<T> operator+<T>(const Rgb<T>& lhs, const Rgb<T>& rhs);
    friend Rgb<T> operator-<T>(const Rgb<T>& lhs, const Rgb<T>& rhs);

protected:
    BoundedChannel<T> _red;
    BoundedChannel<T> _green;
    BoundedChannel<T> _blue;
};

template <typename T>
inline constexpr Rgb<T> operator+(const Rgb<T>& lhs, const Rgb<T>& rhs) {
    return Rgb<T>(lhs._red.value + rhs._red.value,
            lhs._green.value + rhs._green.value,
            lhs._blue.value + rhs._blue.value);
}

template <typename T>
inline constexpr Rgb<T> operator-(const Rgb<T>& lhs, const Rgb<T>& rhs) {
    return Rgb<T>(lhs._red.value - rhs._red.value,
            lhs._green.value - rhs._green.value,
            lhs._blue.value - rhs._blue.value);
}

template <typename T>
inline std::ostream& operator<<(std::ostream& stream, const Rgb<T>& rgb) {
    stream << "RGB(" << rgb._red << ", " << rgb._green << ", " << rgb._blue
           << ")";

    return stream;
}

template <typename T>
inline T chroma(const Rgb<T>& color) {
    T min, max;
    std::tie(min, max) =
            std::minmax({color.red(), color.green(), color.blue()});
    return max - min;
}
}

#endif
