/** \file
 *  General utilities for color types.
 */
#ifndef COLOR_H_
#define COLOR_H_

#include <tuple>
#include <type_traits>
#include <cmath>

namespace color {

namespace details {

template<std::size_t N, typename FnType, typename TupleType,
    typename std::enable_if_t<N==0, int> = 0>
void for_each_channel_impl(TupleType&& channels, const FnType& fn) {
    fn(std::get<0>(channels), 0);    
}

template<std::size_t N, typename FnType, typename TupleType,
    typename std::enable_if_t<N!=0, int> = 0>
void for_each_channel_impl(TupleType&& channels, const FnType& fn) {
    for_each_channel_impl<N-1>(std::forward<TupleType>(channels), fn);
    fn(std::get<N>(channels), N);    
}

}

template <typename T>
static constexpr T FLOAT_EPSILON = 1e-5;

/// Equivalent to std::get<N>(color.as_tuple()).
template<std::size_t N, typename Color>
typename Color::ElementType get(const Color& color) {
    return std::get<N>(color.as_tuple());
}

/** Execute fn once for every element in \a color.
 *  This function loops through channels via template expansion,
 *  so the loop is unrolled at compile time.
 *
 *  FnType should have a signature compatible with:
 *  `void (typename Color::ElementType elem, std::size_t idx)`.
 */
template<typename Color, typename FnType>
void for_each_element(const Color& color, const FnType& fn) {
    using TupleType = typename Color::TupleType;
    details::for_each_channel_impl<std::tuple_size<TupleType>::value-1>(
            color.as_tuple(), fn);
}

/** Approximate equality of floating point color types.
 *  Return true if all components of lhs and rhs are within \a threshold
 *  of each other.
 */
template<typename Color,
    typename std::enable_if_t<std::is_floating_point<typename Color::ElementType>::value, int> = 0>
bool float_eq(const Color& lhs, const Color& rhs, 
        typename Color::ElementType threshold = 
            FLOAT_EPSILON<typename Color::ElementType>) {
    auto is_equal = true;
    for_each_element(lhs, 
        [&is_equal, &rhs, threshold](typename Color::ElementType elem, std::size_t idx) {
            if(std::abs(elem - rhs.as_array()[idx] ) > threshold) {
                is_equal = false;
            }
    });

    return is_equal;
}

}

#endif
