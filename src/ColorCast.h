/** \file
 *  Provides methods for transforming between colors with different data types.
 */
#ifndef COLOR_COLORCAST_H_
#define COLOR_COLORCAST_H_

#include <tuple>
#include <utility>

namespace color {

namespace details {

template <typename To>
struct tuple_transform_functor {
    template <typename From, template <typename> class ChanType>
    constexpr To operator()(ChanType<From> chan) const {
        constexpr auto scaling_factor =
                (ChanType<To>::end_point() - ChanType<To>::min_value()) /
                (ChanType<From>::end_point() - ChanType<From>::min_value());
        constexpr auto shift =
                ChanType<To>::min_value() - ChanType<From>::min_value();

        return chan.value * scaling_factor + shift;
    }
};

template <typename To, typename ToColor, typename Color, std::size_t... indices>
inline constexpr auto color_cast_impl(
        const Color& color, std::index_sequence<indices...>) {
    auto transform_fn = tuple_transform_functor<To>();

    return ToColor((transform_fn(std::get<indices>(color.channel_tuple())))...);
}

template <typename To,
        typename From,
        template <typename> class Color,
        typename enable = void>
struct color_cast_specialization;

template <typename To, typename From, template <typename> class Color>
struct color_cast_specialization<To,
        From,
        Color,
        std::enable_if_t<!std::is_same<To, From>::value>> {
    inline static constexpr Color<To> cast(const Color<From>& color) {
        using FromColorType = Color<From>;
        using ToColorType = Color<To>;
        using indices = std::make_index_sequence<FromColorType::num_channels>;

        return details::color_cast_impl<To, ToColorType>(color, indices());
    }
};

// In the case where To and From are equivalent,
// make color_cast just return its argument.
//
// This is useful primarily in generic code and ensures
// that such a cast will not modify the color.
template <typename To, template <typename> class Color>
struct color_cast_specialization<To, To, Color> {
    inline static constexpr Color<To> cast(const Color<To>& color) {
        return color;
    }
};
}

/** Convert the components of `color` from one data type to another.
 *  The interpretation of the color is unchanged (within the precision
 *  of the data types). Works on all basic color types.
 *
 *  Only the target typename needs to be specified in the template parameters.
 */
template <typename To, typename From, template <typename> class Color>
inline constexpr Color<To> color_cast(const Color<From>& color) {
    return details::color_cast_specialization<To, From, Color>::cast(color);
}

/** Convert the components of `color` from one data type to another.
 *  The interpretation of the color is unchanged (within the precision
 *  of the data types). This overload is specific for composite color
 *  types, specifically Alpha.
 *
 *  Only the target typename needs to be specified in the template parameters.
 */
template <typename To,
        typename From,
        template <typename> class InnerColor,
        template <typename, template <typename> class> class OuterColor>
inline constexpr OuterColor<To, InnerColor> color_cast(
        const OuterColor<From, InnerColor>& color) {
    using FromColorType = OuterColor<From, InnerColor>;
    using ToColorType = OuterColor<To, InnerColor>;
    using indices = std::make_integer_sequence<std::size_t,
            FromColorType::num_channels>;

    auto new_channels =
            details::color_cast_impl<To, ToColorType>(color, indices());

    return ToColorType(new_channels);
}
}

#endif
