/** \file
 *  Provides support types for selecting correct implementation
 *  details from typename and category. All color classes use
 *  channel types internally to select the desired behavior
 *  and to seamlessly handle both integer and floating point
 *  channel types.
 */
#ifndef COLOR_CHANNEL_H_
#define COLOR_CHANNEL_H_

#include <cstdint>
#include <ostream>
#include <type_traits>
#include <limits>
#include <cmath>

#include "Tuple_Util.h"

namespace color {

template <typename T>
static constexpr T FLOAT_EPSILON = 1e-5;

/** Base class for all channel types.
 *  Defines a few common operations but is not designed
 *  to be used directly. Rather, all other channel types
 *  inherit from it.
 */
template <typename T>
class ChannelBase {
public:
    constexpr ChannelBase() {}
    constexpr ChannelBase(T value) : value(value) {}

    using ValueType = T;

    ~ChannelBase() = default;

    ChannelBase(const ChannelBase& other) = default;
    ChannelBase(ChannelBase&& other) noexcept = default;
    ChannelBase& operator=(const ChannelBase& other) = default;
    ChannelBase& operator=(ChannelBase&& other) noexcept = default;

    ChannelBase<T>& operator=(T value) { this->value = value; }

    T clamp(T min, T max) const {
        if(value < min) {
            return min;
        } else if(value > max) {
            return max;
        } else {
            return value;
        }
    }

    T value;

protected:
};

template <typename T,
        typename Pos,
        typename = typename std::enable_if_t<std::is_floating_point<Pos>::value>>
inline T lerp_flat(T start, T end, Pos pos) {
    auto inv_pos = (Pos(1.0) - pos);

    return T(inv_pos * start + pos * end);
}

template <typename T,
        typename Pos,
        typename = typename std::enable_if_t<std::is_floating_point<Pos>::value>>
T lerp_cyclic_int(T start, T end, Pos pos) {}

template <typename T,
        typename Pos,
        typename = typename std::enable_if_t<std::is_floating_point<Pos>::value>>
T lerp_cyclic(T start, T end, Pos pos) {
    auto forward_len = std::abs(end - start);
    auto center = start.center_value();

    if(forward_len > center) {
        auto max_val = start.max_value();
        auto inv_pos = (max_val - pos);
        T out;

        if(start > end) {
            out = start * inv_pos + (end + max_val) * pos;
        } else {
            out = (start + max_val) * inv_pos + end * pos;
        }
        if(out > max_val) {
            out -= max_val;
        }

        return out;
    } else {
        return lerp_flat(start, end, pos);
    }
}


template <typename T>
inline std::ostream& operator<<(std::ostream& stream, ChannelBase<T> rhs) {
    stream << rhs.value;
    return stream;
}

// We always want char channels to print as integers.
template <>
inline std::ostream& operator<<(
        std::ostream& stream, ChannelBase<uint8_t> rhs) {
    stream << static_cast<int>(rhs.value);
    return stream;
}

template <typename T>
bool operator==(T lhs, ChannelBase<T> rhs) {
    return lhs == rhs.value;
}

template <typename T>
bool operator==(ChannelBase<T> lhs, T rhs) {
    return lhs.value == rhs;
}

template <typename T>
bool operator==(ChannelBase<T> lhs, ChannelBase<T> rhs) {
    return lhs.value == rhs.value;
}

template <typename T>
bool operator!=(T lhs, ChannelBase<T> rhs) {
    return lhs != rhs.value;
}

template <typename T>
bool operator!=(ChannelBase<T> lhs, T rhs) {
    return lhs.value != rhs;
}

template <typename T>
bool operator!=(ChannelBase<T> lhs, ChannelBase<T> rhs) {
    return lhs.value != rhs.value;
}

template <typename T>
bool operator>(ChannelBase<T> lhs, T rhs) {
    return lhs.value > rhs;
}

template <typename T>
bool operator<(ChannelBase<T> lhs, T rhs) {
    return lhs.value < rhs;
}

template <typename T>
bool operator>=(ChannelBase<T> lhs, T rhs) {
    return lhs.value >= rhs;
}

template <typename T>
bool operator<=(ChannelBase<T> lhs, T rhs) {
    return lhs.value <= rhs;
}

template <typename T>
bool operator>(T lhs, ChannelBase<T> rhs) {
    return lhs > rhs.value;
}

template <typename T>
bool operator<(T lhs, ChannelBase<T> rhs) {
    return lhs < rhs.value;
}

template <typename T>
bool operator>=(T lhs, ChannelBase<T> rhs) {
    return lhs >= rhs.value;
}

template <typename T>
bool operator<=(T lhs, ChannelBase<T> rhs) {
    return lhs <= rhs.value;
}

// Channel type implementations

namespace details {

template <typename T, typename enable = void>
class BoundedChannelImpl;

template <typename T>
class BoundedChannelImpl<T, std::enable_if_t<std::is_floating_point<T>::value>>
        : public ChannelBase<T> {
public:
    static constexpr T max_value() { return T(1.0); }

    static constexpr T center_value() { return T(0.5); }

    static constexpr T min_value() { return T(0.0); }

    constexpr T inverse() const { return 1.0 - value; }

    constexpr T normalize() const { return this->clamp(T(0.0), T(1.0)); }

    template <typename Pos>
    constexpr T lerp(T end, Pos pos) const {
        return lerp_flat(this->value, end, pos);
    }

    using ChannelBase<T>::value;

protected:
    using ChannelBase<T>::ChannelBase;
};

template <typename T>
class BoundedChannelImpl<T, std::enable_if_t<std::is_integral<T>::value>>
        : public ChannelBase<T> {
public:
    static constexpr T max_value() { return std::numeric_limits<T>::max(); }

    static constexpr T center_value() { return max_value() / T(2); }

    static constexpr T min_value() { return T(0); }

    constexpr T inverse() const { return ~value; }

    constexpr T normalize() const { return value; }

    template <typename Pos>
    constexpr T lerp(T end, Pos pos) const {
        return lerp_flat(this->value, end, pos);
    }

    using ChannelBase<T>::value;

protected:
    using ChannelBase<T>::ChannelBase;
};
}

/** Channel with explicit bounds and no periodic behavior.
 *  - Integral BoundedChannel types are bounded between
 *  0 and `std::numeric_limits<T>::max()`.
 *  - Floating point BoundedChannel types are bounded
 *  between 0 and 1 for "normalized" types.
 *  Values are not explicitly constrained to the bounds except for when
 *  calling BoundedChannel::normalize().
 */
template <typename T>
class BoundedChannel : public details::BoundedChannelImpl<T> {
public:
    using details::BoundedChannelImpl<T>::BoundedChannelImpl;
};
}


#endif
