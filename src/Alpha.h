/** \file
 * Provides utilities for working with colors that include an alpha channel.
 */
#ifndef COLOR_ALPHA_H_
#define COLOR_ALPHA_H_

#include <tuple>
#include <ostream>

#include "Channel.h"
#include "Tuple_Util.h"
#include "Color.h"

namespace color {

template <typename T, template <typename> class Color>
class Alpha;

template <typename T, template <typename> class Color>
constexpr void swap(Alpha<T, Color>& lhs, Alpha<T, Color>& rhs);

/** Defines a color with an alpha channel specifying opacity.
*   Alpha is a composite color that wraps another color type
*   and provides convenience methods for acting on the
*   composite. The inner color can be accessed with the
*   Alpha::color method and the alpha channel can be accessed
*   with Alpha::alpha. The alpha value is stored internally
*   after the components of the inner color. All
*   functions that return channel values maintain the
*   internal order.
*/
template <typename T, template <typename> class Color>
class Alpha {
public:
    static constexpr int num_channels = Color<T>::num_channels + 1;

    using ElementType = T;
    using TupleType = typename details::
            tuple_append_type_right<typename Color<T>::TupleType, T>::type;
    using ChannelTupleType = typename details::tuple_append_type_right<
            typename Color<T>::ChannelTupleType,
            BoundedChannel<T>&>::type;
    using ConstChannelTupleType = typename details::tuple_append_type_right<
            typename Color<T>::ConstChannelTupleType,
            BoundedChannel<T>>::type;

    /// Construct an Alpha instance with all components set to 0.
    constexpr Alpha() : _color(Color<T>()), _alpha(T(0)) {}

    /// Construct an Alpha instance from the inner color `color` and an alpha
    /// value.
    constexpr Alpha(Color<T> color, T alpha) : _color(color), _alpha(alpha) {}

    /// Construct an Alpha instance from a tuple of component values.
    explicit constexpr Alpha(const TupleType& tuple)
        : _color(details::tuple_slice<0, Color<T>::num_channels>(tuple)),
          _alpha(std::get<num_channels - 1>(tuple)) {}

    /** Construct an Alpha instance from an array of values.
     *  \a values must contain at least Alpha::num_channels values,
     */
    explicit constexpr Alpha(const T* values)
        : _color(values), _alpha(values[num_channels - 1]) {}

    /// Construct an Alpha instance from an array of values.
    explicit constexpr Alpha(const std::array<T, num_channels>& values)
        : Alpha(values.data()) {}

    /** Construct an Alpha instance from specific component values.
     */
    template <typename... Args>
    explicit constexpr Alpha(T c1, Args... args)
        : Alpha(std::make_tuple(c1, args...)) {}

    /** Strict equality of two Alpha instances.
     *  For floating point channels, this has all the caveats
     *  of floating point equality.
     */
    constexpr bool operator==(const Alpha<T, Color>& rhs) const {
        return (_color == rhs.color() && _alpha == rhs.alpha());
    }

    /** Strict inequality of two Alpha instances.
     *  Negation of Alpha::operator ==
     */
    constexpr bool operator!=(const Alpha<T, Color>& rhs) const {
        return !(*this == rhs);
    }


    ~Alpha() = default;

    Alpha(const Alpha& other) = default;
    Alpha(Alpha&& other) noexcept = default;
    Alpha& operator=(const Alpha& other) = default;
    Alpha& operator=(Alpha&& other) noexcept = default;

    /** Returns a pointer to the internal representation of components.
     *  The alpha component is the last component in the array and
     *  all of the components from the inner color are at the same
     *  index they would be for an instance of the Color type.
     */
    constexpr T* data() { return reinterpret_cast<T*>(this); }

    constexpr const T* data() const { return reinterpret_cast<const T*>(this); }

    /** Return a copy of the color with each channel clamped to the range
     *  `[min, max]`. For channel values above \a max, they will be set to
     *  \a max and likewise for channels below \a min, they will be set to
     *  \a min.
     */
    constexpr Alpha<T, Color> clamp(T min, T max) const {
        return Alpha<T, Color>(_color.clamp(min, max), _alpha.clamp(min, max));
    }

    /** Return a copy of the color with all channels clamped to the "normal"
     *  range. For integer components, values are always normalized, but
     *  floating point components are clamped to `[0, 1]`.
     */
    constexpr Alpha<T, Color> normalize() const {
        return Alpha<T, Color>(_color.normalize(), _alpha.normalize());
    }

    /** Compute a new color by linearly interpolating between two endpoints.
     *
     *  \param pos The position between `*this` and \a end.
     *  A \a pos of 0 specifies `*this` while a \a pos of 1
     *  specifies \a end.
     */
    template <typename Pos>
    constexpr Alpha<T, Color> lerp(const Alpha<T, Color>& end, Pos pos) const {
        return Alpha<T, Color>(_color.lerp(end.color(), pos),
                _alpha.lerp(end._alpha.value, pos));
    }

    template <typename Pos>
    constexpr Alpha<T, Color> lerp_flat(
            const Alpha<T, Color>& end, Pos pos) const {
        return Alpha<T, Color>(_color.lerp_flat(end.color(), pos),
                _alpha.lerp_flat(end._alpha.value, pos));
    }

    /** Return the inverse of the color.
     */
    constexpr Alpha<T, Color> inverse() const {
        return Alpha<T, Color>(_color.inverse(), _alpha.inverse());
    }

    /** Return a copy of the color with each channel value multiplied by
     * `factor`.
     *  If factor is larger than one, the resultant color may become
     *  un-normalized. This function does not attempt to re-normalize --
     *  call Alpha::normalize() afterward to ensure normalization.
     */
    template <typename U,
            typename std::enable_if_t<std::is_floating_point<U>::value, int> = 0>
    constexpr Alpha<T, Color> scale(U factor) const {
        return Alpha<T, Color>(_color.scale(), _alpha * factor);
    }

    /// Set the alpha component explicitly.
    constexpr Alpha<T, Color>& set_alpha(T value) {
        _alpha = value;
        return *this;
    }

    /// Return a mutable reference to the alpha component.
    constexpr T& alpha() { return _alpha.value; }
    constexpr T alpha() const { return _alpha.value; }

    constexpr BoundedChannel<T> alpha_channel() const { return _alpha; }
    constexpr BoundedChannel<T>& alpha_channel() { return _alpha; }

    /// Get a mutable reference to the inner color object.
    constexpr Color<T>& color() { return _color; }

    /// Get the inner color object.
    constexpr const Color<T>& color() const { return _color; }

    /// Return a copy of the color part.
    constexpr Color<T> strip_alpha() const { return _color; }

    /** Return a tuple of the channel values.
     *  This can be useful for generalizing algorithms
     *  to arbitrary colors or for decomposing a color into
     *  its components.
     */
    constexpr TupleType as_tuple() const {
        return std::tuple_cat(_color.as_tuple(), std::make_tuple(_alpha.value));
    }

    /// Return an array of channel values.
    constexpr std::array<T, num_channels> as_array() const {
        return details::tuple_to_array(as_tuple());
    }

    /** Return a tuple of references to the channel objects of an Rgb instance.
     *  This is potentially useful for acting differently depending on the
     *  channel kind.
     */
    ChannelTupleType channel_tuple() {
        return std::tuple_cat(_color.channel_tuple(), std::make_tuple(_alpha));
    }

    /// A const overload of Rgb::channel_tuple().
    /// Returns a tuple of copies of the channels of an Alpha instance.
    ConstChannelTupleType channel_tuple() const {
        return std::tuple_cat(_color.channel_tuple(), std::make_tuple(_alpha));
    }

    /// Construct an Alpha instance with all channels set to \a value.
    static constexpr Alpha<T, Color> broadcast(T value) {
        return Alpha<T, Color>(Color<T>::broadcast(value), value);
    }

    friend void swap<T, Color>(Alpha<T, Color>& lhs, Alpha<T, Color>& rhs);

private:
    Color<T> _color;
    BoundedChannel<T> _alpha;
};

/** Return an Alpha instance from a color and alpha channel.
 *  Functionally equivalent to Alpha::Alpha(Color<T> color, T alpha).
 */
template <typename T, template <typename> class Color>
Alpha<T, Color> with_alpha(Color<T> color, T alpha) {
    return Alpha<T, Color>(color, alpha);
}

template <typename T, template <typename> class Color>
std::ostream& operator<<(std::ostream& stream, const Alpha<T, Color>& color) {
    stream << "Alpha(" << color.color() << ", " << +color.alpha() << ")"
           << std::endl;
    return stream;
}

/** Performs alpha blending of two colors.
 *  Both \a src and \a dest alphas are taken into account.
 *  If \a dest is known to be opaque, pass dest.color() instead
 *  for a modest performance increase.
 */
template <typename T,
        template <typename> class Color,
        typename std::enable_if_t<std::is_floating_point<T>::value, int> = 0>
Alpha<T, Color> alpha_blend(
        const Alpha<T, Color>& src, const Alpha<T, Color>& dest) {
    auto out = Alpha<T, Color>();

    auto inv_src_alpha = (T(1.0) - src.alpha());
    out.set_alpha(src.alpha() + dest.alpha() * inv_src_alpha);

    if(out.alpha() <= FLOAT_EPSILON<T>) {
        out.color() = Color<T>::broadcast(T(0.0));
    } else {
        auto premul_dest = dest.color().scale(dest.alpha());

        out.color() = src.color().lerp(premul_dest, inv_src_alpha);
    }

    return out;
}

template <typename T,
        template <typename> class Color,
        typename Pos = float,
        typename std::enable_if_t<std::is_integral<T>::value, int> = 0>
Alpha<T, Color> alpha_blend(
        const Alpha<T, Color>& src, const Alpha<T, Color>& dest) {
    Pos scaled_src_alpha = src.alpha_channel().template to_float_channel<Pos>();
    Pos scaled_dest_alpha =
            dest.alpha_channel().template to_float_channel<Pos>();

    Pos inv_src_alpha = Pos(1.0) - scaled_src_alpha;

    auto out = Alpha<T, Color>();

    auto out_alpha_float = scaled_src_alpha + scaled_dest_alpha * inv_src_alpha;

    if(out_alpha_float <= FLOAT_EPSILON<Pos>) {
        out.color() = Color<T>::broadcast(T(0.0));
    } else {
        auto premul_dest = dest.color().scale(scaled_dest_alpha);

        out.color() = src.color().lerp(premul_dest, inv_src_alpha);
        out.alpha_channel() =
                BoundedChannel<T>::from_float_channel(out_alpha_float);
    }

    return out;
}

/** Performs alpha blending of two colors.
 *  \a dest is assumed to be fully opaque, and the resulting
 *  color with also be opaque. This is equivalent to
 *  calling `lerp` on \a src with a position of `1-src.alpha()`.
 */
template <typename T, template <typename> class Color>
Alpha<T, Color> alpha_blend(const Alpha<T, Color>& src, const Color<T>& dest) {
    return Alpha<T, Color>(src.color().lerp(dest, 1.0 - src.alpha()),
            BoundedChannel<T>::max_value());
}

template <typename T, template <typename> class Color>
constexpr inline void swap(Alpha<T, Color>& lhs, Alpha<T, Color>& rhs) {
    using std::swap;
    swap(lhs._color, rhs._color);
    swap(lhs._alpha, rhs._alpha);
}
}

#endif
