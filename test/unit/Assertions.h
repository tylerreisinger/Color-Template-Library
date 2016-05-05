#include "gtest/gtest.h"

#include <type_traits>
#include <cmath>

template <typename T,
        typename std::enable_if_t<std::is_floating_point<T>::value, int> = 0>
inline ::testing::AssertionResult equal_within_error(
        T lhs, T rhs, T error_bound) {
    const auto difference = std::abs(lhs - rhs);
    if(difference <= error_bound) {
        return ::testing::AssertionSuccess();
    } else {
        return ::testing::AssertionFailure()
                << rhs << " compared to " << lhs << ": "
                                                    "differs by "
                << difference << " (tolerance=" << error_bound << ").";
    }
}

template <typename T,
        typename std::enable_if_t<std::is_integral<T>::value, int> = 0>
inline ::testing::AssertionResult equal_within_error(
        T lhs, T rhs, T error_bound) {
    auto left_bound = lhs + error_bound;
    if(left_bound < lhs) {
        left_bound = std::numeric_limits<T>::max();
    }
    auto right_bound = lhs - error_bound;
    if(right_bound > lhs) {
        right_bound = std::numeric_limits<T>::min();
    }

    if(rhs <= left_bound && rhs >= right_bound) {
        return ::testing::AssertionSuccess();
    } else {
        return ::testing::AssertionFailure()
                << +rhs << " compared to " << +lhs << ": "
                                                      "differs by "
                << +(lhs - rhs) << " (tolerance=" << +error_bound << ").";
    }
}
