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
#include "Angle.h"

namespace color {

template <typename T>
class BoundedChannel;

template <typename T>
class PeriodicChannel;

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

namespace details {

template <typename T,
        typename Pos,
        typename = typename std::enable_if_t<std::is_floating_point<Pos>::value>>
inline T lerp_flat(T start, T end, Pos pos) {
    auto inv_pos = (Pos(1.0) - pos);

    return T(inv_pos * start + pos * end);
}

template <template <typename> class ChannelType,
        typename T,
        typename Pos,
        typename = typename std::enable_if_t<std::is_floating_point<Pos>::value>>
T lerp_cyclic_int(T start, T end, Pos pos) {
    auto forward_len = std::abs(end - start);
    auto center = ChannelType<T>::center_value();

    if(forward_len > center) {
        auto max_val = ChannelType<T>::max_value();
        auto inv_pos = Pos(1.0) - pos;

        //   Wrapping Right   len
        //   --|              |--->
        //   [-X--------------X---]
        // left|     center   |right

        if(start > end) {
            auto right = max_val - start;
            auto left = end;
            auto len = right + left;

            auto out = start + len * pos;
            if(out >= max_val) {
                out -= max_val;
            }
            return out;
            //   Wrapping Left    len
            //   <-|              |---|
            //   [-X--------------X---]
            // left|     center   |right
        } else {
            auto right = max_val - end;
            auto left = start;
            auto len = right + left;
            auto out = start - len * pos;
            if(out < 0) {
                out += max_val;
            }
            return out;
        }

    } else {
        return lerp_flat(start, end, pos);
    }
}

template <typename T,
        typename Pos,
        typename = typename std::enable_if_t<std::is_floating_point<Pos>::value>>
T lerp_cyclic(T start, T end, Pos pos) {
    using ChannelType = PeriodicChannel<T>;
    auto forward_len = std::abs(end - start);
    auto center = ChannelType::center_value();

    if(forward_len > center) {
        auto max_val = ChannelType::max_value();
        auto inv_pos = (Pos(1.0) - pos);
        T out;

        if(start > end) {
            out = start * inv_pos + (end + max_val) * pos;
        } else {
            out = (start + max_val) * inv_pos + end * pos;
        }
        if(out >= max_val) {
            out -= max_val;
        }

        return out;
    } else {
        return lerp_flat(start, end, pos);
    }
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
    static constexpr T end_point() { return max_value(); }

    static constexpr T center_value() { return T(0.5); }

    static constexpr T min_value() { return T(0.0); }

    constexpr T inverse() const { return 1.0 - value; }

    constexpr T normalize() const { return this->clamp(T(0.0), T(1.0)); }

    template <typename Pos>
    constexpr T lerp(T end, Pos pos) const {
        return details::lerp_flat(this->value, end, pos);
    }

    template <typename Pos>
    constexpr T lerp_flat(T end, Pos pos) const {
        return lerp(end, pos);
    }

    template <typename Out = T,
            typename = std::enable_if_t<std::is_floating_point<Out>::value>>
    constexpr Out to_float_channel() const {
        return Out(value);
    }

    template <typename FloatType = T,
            typename =
                    std::enable_if_t<std::is_floating_point<FloatType>::value>>
    static constexpr BoundedChannel<T> from_float_channel(FloatType val) {
        return BoundedChannel<T>(T(val));
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
    static constexpr uintmax_t end_point() { return max_value(); }

    static constexpr T center_value() { return max_value() >> 1; }

    static constexpr T min_value() { return T(0); }

    constexpr T inverse() const { return ~value; }

    constexpr T normalize() const { return value; }

    template <typename Pos>
    constexpr T lerp(T end, Pos pos) const {
        return details::lerp_flat(this->value, end, pos);
    }

    template <typename Pos>
    constexpr T lerp_flat(T end, Pos pos) const {
        return lerp(end, pos);
    }

    template <typename Out,
            typename = std::enable_if_t<std::is_floating_point<Out>::value>>
    constexpr Out to_float_channel() const {
        return value / Out(end_point());
    }

    template <typename FloatType,
            typename =
                    std::enable_if_t<std::is_floating_point<FloatType>::value>>
    static constexpr BoundedChannel<T> from_float_channel(FloatType val) {
        return BoundedChannel<T>(T(val * FloatType(end_point())));
    }

    using ChannelBase<T>::value;

protected:
    using ChannelBase<T>::ChannelBase;
};

template <typename T, typename enable = void>
class PeriodicChannelImpl;

template <typename T>
class PeriodicChannelImpl<T, std::enable_if_t<std::is_floating_point<T>::value>>
        : public ChannelBase<T> {
public:
    static constexpr T max_value() {
        return T(1.0 - std::numeric_limits<T>::epsilon());
    }
    static constexpr T center_value() { return 0.5; }
    static constexpr T min_value() { return T(0.0); }

    static constexpr T end_point() { return T(1.0); }

    constexpr T inverse() const {
        T out = value + 0.5;
        if(out >= end_point()) {
            out -= end_point();
        }
        return out;
    }
    constexpr T normalize() const {
        if(value == 0.0) {
            return value;
        } else {
            auto val = std::fmod(value, end_point());
            if(val < min_value()) {
                val = max_value() + val;
            }
            return val;
        }
    }

    template <typename Pos>
    constexpr T lerp(T end, Pos pos) const {
        return details::lerp_cyclic(this->value, end, pos);
    }

    template <typename Pos>
    constexpr T lerp_flat(T end, Pos pos) const {
        return details::lerp_flat(this->value, end, pos);
    }

    template <template <typename> class Angle,
            typename U,
            typename std::enable_if_t<std::is_floating_point<U>::value, int> = 0>
    constexpr void set_angle(Angle<U> angle) {
        value = angle.to_normalized_coordinate();
    }

    template <template <typename> class Angle, typename U>
    constexpr Angle<U> get_angle() const {
        return Angle<U>::from_normalized_coordinate(value);
    }

    template <typename Out = T,
            typename = std::enable_if_t<std::is_floating_point<Out>::value>>
    constexpr Out to_float_channel() const {
        return Out(value);
    }

    template <typename FloatType = T,
            typename =
                    std::enable_if_t<std::is_floating_point<FloatType>::value>>
    static constexpr BoundedChannel<T> from_float_channel(FloatType val) {
        return BoundedChannel<T>(T(val));
    }

    using ChannelBase<T>::value;

protected:
    using ChannelBase<T>::ChannelBase;
};

template <typename T>
class PeriodicChannelImpl<T, std::enable_if_t<std::is_integral<T>::value>>
        : public ChannelBase<T> {
public:
    static constexpr T max_value() { return std::numeric_limits<T>::max(); }
    static constexpr T center_value() { return end_point() >> 1; }
    static constexpr T min_value() { return 0; }
    static constexpr uintmax_t end_point() {
        return uintmax_t(std::numeric_limits<T>::max()) + 1;
    }

    constexpr T inverse() const {
        T out = value + center_value();
        if(out < 0) {
            out = value - center_value();
        }
        return out;
    }

    constexpr T normalize() const { return value; }

    template <typename Pos>
    constexpr T lerp(T end, Pos pos) const {
        return details::lerp_cyclic_int<PeriodicChannel>(this->value, end, pos);
    }

    template <typename Pos>
    constexpr T lerp_flat(T end, Pos pos) const {
        return details::lerp_flat(this->value, end, pos);
    }

    template <template <typename> class Angle,
            typename U,
            typename std::enable_if_t<std::is_floating_point<U>::value, int> = 0>
    constexpr void set_angle(Angle<U> angle) {
        value = angle.to_normalized_coordinate() * end_point();
    }

    template <template <typename> class Angle,
            typename U,
            typename std::enable_if_t<std::is_floating_point<U>::value, int> = 0>
    constexpr Angle<U> get_angle() const {
        auto normalized_coord = U(value) / end_point();
        return Angle<U>::from_normalized_coordinate(normalized_coord);
    }

    constexpr T wrap_endpoint() const { return value; }

    template <typename Out,
            typename std::enable_if_t<std::is_floating_point<Out>::value>>
    constexpr Out to_normalized_coordinate() const {
        return value / Out(end_point());
    }

    template <typename FloatType,
            typename =
                    std::enable_if_t<std::is_floating_point<FloatType>::value>>
    static constexpr BoundedChannel<T> from_float_channel(FloatType val) {
        return BoundedChannel<T>(T(val * FloatType(end_point())));
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

template <typename T>
class PeriodicChannel : public details::PeriodicChannelImpl<T> {
public:
    using details::PeriodicChannelImpl<T>::PeriodicChannelImpl;
};
}

#endif
