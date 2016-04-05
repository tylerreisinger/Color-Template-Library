#ifndef COLOR_H_
#define COLOR_H_

#include <tuple>

namespace color {

template <std::size_t N, typename T, template <typename> class Color>
constexpr T get(const Color<T>& color) {
    return std::get<N>(color.as_tuple());
}

template <std::size_t N,
        typename T,
        typename FnType,
        template <typename...> class TupleType,
        typename... ChanTypes>
typename std::enable_if<N != 0>::type _for_each_channel_imp(
        TupleType<ChanTypes...>& channels) {
    FnType(std::get<N>(channels));
    _for_each_channel_imp<N - 1, T, FnType>(channels);
}

template <std::size_t N,
        typename T,
        typename FnType,
        template <typename...> class TupleType,
        typename... ChanTypes>
typename std::enable_if<N == 0>::type _for_each_channel_imp(
        TupleType<ChanTypes...>& channels) {}

template <typename T, template <typename> class Color, typename FnType>
void for_each_channel(Color<T>& color) {
    using TupleType = typename Color<T>::TupleType;
    _for_each_channel_imp<std::tuple_size<TupleType>::value, T, FnType>(
            color.channel_tuple());
}
}

#endif
