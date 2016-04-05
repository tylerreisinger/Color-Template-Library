/** \file
 *  Provides methods for transforming between colors with different data types.
 */
#ifndef COLOR_COLORCAST_H_
#define COLOR_COLORCAST_H_

#include <utility>
#include <tuple>

namespace color {

namespace details {

template <typename To>
struct tuple_transform_functor {
    template <typename From, template <typename> class ChanType>
    constexpr To operator()(ChanType<From> chan) {
        auto scaling_factor =
                (ChanType<To>::max_value() - ChanType<To>::min_value()) /
                (ChanType<From>::max_value() - ChanType<From>::min_value());
        auto shift = ChanType<To>::min_value() - ChanType<From>::min_value();

        return scaling_factor * chan.value + shift;
    }
};

template <typename To, typename Color, std::size_t... indices>
inline constexpr auto color_cast_impl(
        const Color& color, std::index_sequence<indices...>) {
    auto channels = color.channel_tuple();
    auto transform_fn = tuple_transform_functor<To>();

    return std::make_tuple((transform_fn(std::get<indices>(channels)))...);
}
}

/** Convert the components of `color` from one data type to another.
 *  The interpretation of the color is unchanged (within the precision
 *  of the data types). Works on all basic color types.
 *
 *  Only the target typename needs to be specified in the template parameters.
 */
template <typename To, typename From, template <typename> class Color>
inline constexpr Color<To> color_cast(const Color<From>& color) {
    using FromColorType = Color<From>;
    using ToColorType = Color<To>;
    using indices = std::make_integer_sequence<std::size_t,
            FromColorType::num_channels>;

    auto new_channels = details::color_cast_impl<To>(color, indices());

    return ToColorType(new_channels);
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

    auto new_channels = details::color_cast_impl<To>(color, indices());

    return ToColorType(new_channels);
}
}

#endif
