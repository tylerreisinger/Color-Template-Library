#include "gtest/gtest.h"

#include <type_traits>
#include <cmath>
#include <sstream>

#include "Color.h"

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

template <typename T, typename Color>
inline ::testing::AssertionResult assert_colors_near(const char* lhs_expr,
        const char* rhs_expr,
        const char* tol_expr,
        const Color& lhs_color,
        const Color& rhs_color,
        T error_tol) {
    bool is_failure = false;
    T max_error = 0.0;

    color::for_each_element(lhs_color,
            [&is_failure, &rhs_color, &max_error, error_tol](
                                    auto elem, std::size_t idx) {
                auto diff = std::abs(elem - T(rhs_color.as_array()[idx]));
                if(diff > error_tol) {
                    is_failure = true;
                    max_error = std::max(diff, max_error);
                }
            });

    if(is_failure) {
        return ::testing::AssertionFailure()
                << "Colors `" << lhs_expr << "` and `" << rhs_expr
                << "` differ by more than " << error_tol
                << " in one or more channels.\n\t" << lhs_color << " ~ "
                << rhs_color << "\n\tMaximum channel difference: " << max_error;
    } else {
        return ::testing::AssertionSuccess();
    }
}
template <typename Color>
inline ::testing::AssertionResult assert_colors_equal(const char* lhs_expr,
        const char* rhs_expr,
        const Color& lhs_color,
        const Color& rhs_color) {
    bool is_failure = false;

    color::for_each_element(lhs_color,
            [&is_failure, &rhs_color](auto elem, std::size_t idx) {
                if(elem != rhs_color.as_array()[idx]) {
                    is_failure = true;
                }
            });

    if(is_failure) {
        return ::testing::AssertionFailure()
                << "Colors `" << lhs_expr << "` and `" << rhs_expr
                << "` are not equal:\n\t" << lhs_color << " != " << rhs_color;
    } else {
        return ::testing::AssertionSuccess();
    }
}

#define ASSERT_COLORS_EQ(c1, c2)                                               \
    ASSERT_PRED_FORMAT2(assert_colors_equal, c1, c2)
#define ASSERT_COLORS_NEAR(c1, c2, tol)                                        \
    ASSERT_PRED_FORMAT3(assert_colors_near, c1, c2, tol)
