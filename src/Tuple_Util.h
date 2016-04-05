#ifndef COLOR_TUPLE_UTIL_H_
#define COLOR_TUPLE_UTIL_H_

#include <tuple>
#include <array>

namespace color {
namespace details {

template <typename TupleType, typename NewType, typename SeqType>
struct tuple_append_type_right_impl;

template <typename TupleType, typename NewType, std::size_t... indices>
struct tuple_append_type_right_impl<TupleType,
        NewType,
        std::index_sequence<indices...>> {
    using type =
            std::tuple<typename std::tuple_element<indices, TupleType>::type...,
                    NewType>;
};

template <typename TupleType, std::size_t start, std::size_t... indices>
constexpr auto tuple_slice_impl(
        const TupleType& tuple, std::index_sequence<indices...>) {
    return std::make_tuple(std::get<indices + start>(tuple)...);
}

template <typename TupleType, typename FnType, std::size_t N>
struct tuple_for_each_impl {
    static void for_each(TupleType&& tuple, const FnType& fn) {
        fn(std::get<std::tuple_size<TupleType>::value - 1 - N>(tuple));
        tuple_for_each_impl<TupleType, FnType, N - 1>(tuple, fn);
    }
};

template <typename TupleType, typename FnType>
struct tuple_for_each_impl<TupleType, FnType, 0> {
    static void for_each(TupleType&& tuple, const FnType& fn) {
        fn(std::get<std::tuple_size<TupleType>::value - 1>(tuple));
    }
};

template <typename TupleType, typename FnType>
constexpr void tuple_for_each(TupleType&& tuple, const FnType& fn) {
    tuple_for_each_impl<TupleType,
            FnType,
            std::tuple_size<TupleType>::value - 1>::for_each(tuple, fn);
}

template <typename TupleType, typename FnType, std::size_t... indices>
constexpr auto tuple_map_impl(const TupleType& tuple,
        const FnType& fn,
        std::index_sequence<indices...>) {
    return std::make_tuple(fn(std::get<indices>(tuple))...);
}

template <typename TupleType, typename FnType>
constexpr auto tuple_map(const TupleType& tuple, const FnType& fn) {
    using indices = std::make_index_sequence<std::tuple_size<TupleType>::value>;

    return tuple_map_impl(tuple, fn, indices());
}

template <typename TupleType, std::size_t... indices>
constexpr std::array<
        typename std::remove_cv<typename std::remove_reference<
                typename std::tuple_element<0, TupleType>::type>::type>::type,
        std::tuple_size<TupleType>::value>
tuple_to_array_impl(const TupleType& tuple, std::index_sequence<indices...>) {
    return {std::get<indices>(tuple)...};
}

template <typename TupleType>
constexpr std::array<
        typename std::remove_cv<typename std::remove_reference<
                typename std::tuple_element<0, TupleType>::type>::type>::type,
        std::tuple_size<TupleType>::value>
tuple_to_array(const TupleType& tuple) {
    using indices = std::make_index_sequence<std::tuple_size<TupleType>::value>;

    return tuple_to_array_impl(tuple, indices());
}

template <std::size_t start, std::size_t end, typename TupleType>
constexpr auto tuple_slice(const TupleType& tuple) {
    static_assert(end < std::tuple_size<TupleType>::value,
            "'end' beyond the end of the tuple");

    using indices = std::make_index_sequence<end - start>;

    return tuple_slice_impl<TupleType, start>(tuple, indices());
}

template <typename TupleType, typename NewType>
struct tuple_append_type_right {
    using type = typename tuple_append_type_right_impl<TupleType,
            NewType,
            std::make_index_sequence<std::tuple_size<TupleType>::value>>::type;
};
}
}

#endif
