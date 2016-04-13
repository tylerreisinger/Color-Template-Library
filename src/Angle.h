/** \file
 *  Utilities for working with angles. Used for polar channels
 * (PeriodicChannel).
 *  Provides a type checked way of setting angles in either degrees or
 *  radians.
 */
#ifndef COLOR_ANGLE_H_
#define COLOR_ANGLE_H_

#include <cmath>

namespace color {

///\cond forward_decl
template <typename T>
class Radians;
///\endcond

/** The numerical constant pi.
 */
template <typename T>
T PI = 3.141592653589793238462643383279502884197169399375105820974944592307816406286;

/** Represents an angle in degrees.
 */
template <typename T>
class Degrees {
public:
    /// Return the length of a full period.
    static constexpr T period_length() { return T(360.0); }

    constexpr Degrees() : value(T(0)) {}
    explicit constexpr Degrees(T angle) : value(angle) {}

    ~Degrees() = default;

    /** Return a value in `[0,1)` representing
     *  a floating point channel value.
     *
     *  Note that the returned value can be outside
     *  of this range if the angle is not
     *  properly normalized within the first period.
     */
    constexpr T to_normalized_coordinate() const {
        return value / period_length();
    }

    /** Return an angle normalized to the first period.
     */
    constexpr Degrees<T> normalize() const {
        return Degrees<T>(std::abs(std::fmod(value, period_length())));
    }

    /// Return the same angle expressed in Radians.
    constexpr Radians<T> to_radians() const {
        return Radians<T>(
                value * (Radians<T>::period_length() / period_length()));
    }

    Degrees<T>& operator+=(Degrees<T> rhs) {
        value += rhs.value;
        value = normalize().value;
        return *this;
    }
    Degrees<T>& operator-=(Degrees<T> rhs) {
        value -= rhs.value;
        value = normalize().value;
        return *this;
    }
    Degrees<T>& operator*=(T rhs) {
        value *= rhs;
        value = normalize().value;
        return *this;
    }
    Degrees<T>& operator/=(T rhs) {
        value /= rhs;
        value = normalize().value;
        return *this;
    }

    /** Construct an angle from a normalized coordinate
     *  in the range `[0,1]`.
     */
    static constexpr Degrees<T> from_normalized_coordinate(T value) {
        return Degrees<T>(value * period_length());
    }


    T value;
};

/** Represents an angle in radians.
 */
template <typename T>
class Radians {
public:
    static constexpr T period_length() { return PI<T> * 2.0; }

    constexpr Radians() : value(T(0)) {}
    explicit constexpr Radians(T angle) : value(angle) {}

    ~Radians() = default;

    /** Return a value in `[0,1)` representing
     *  a floating point channel value.
     *
     *  Note that the returned value can be outside
     *  of this range if the angle is not
     *  properly normalized within the first period.
     */
    constexpr T to_normalized_coordinate() const {
        return value / period_length();
    }

    /// Normalize the angle value to the first
    //  period `[0, 2*pi)`.
    constexpr Radians<T> normalize() const {
        return std::abs(std::modf(value, period_length()));
    }

    /// Return the same angle expressed in Degrees.
    constexpr Degrees<T> to_degrees() const {
        return Degrees<T>(
                (period_length() / Degrees<T>::period_length()) * value);
    }

    Radians<T>& operator+=(Radians<T> rhs) {
        value += rhs.value;
        value = normalize().value;
        return *this;
    }
    Radians<T>& operator-=(Radians<T> rhs) {
        value -= rhs.value;
        value = normalize().value;
        return *this;
    }
    Radians<T>& operator*=(T rhs) {
        value *= rhs;
        value = normalize().value;
        return *this;
    }
    Radians<T>& operator/=(T rhs) {
        value /= rhs;
        value = normalize().value;
        return *this;
    }

    /** Construct an angle from a normalized coordinate
     *  in the range `[0,1]`.
     */
    static constexpr Radians<T> from_normalized_coordinate(T value) {
        return Radians<T>(value * period_length());
    }

    T value;
};

template <typename T>
inline constexpr bool operator==(Degrees<T> lhs, Degrees<T> rhs) {
    return lhs.value == rhs.value;
}
template <typename T>
inline constexpr bool operator!=(Degrees<T> lhs, Degrees<T> rhs) {
    return !(lhs == rhs);
}
template <typename T>
constexpr inline Degrees<T> operator+(Degrees<T> lhs, Degrees<T> rhs) {
    return Degrees<T>(lhs.value + rhs.value).normalize();
}
template <typename T>
inline constexpr Degrees<T> operator-(Degrees<T> lhs, Degrees<T> rhs) {
    return Degrees<T>(lhs.value - rhs.value).normalize();
}
template <typename T>
inline constexpr Degrees<T> operator*(Degrees<T> lhs, T rhs) {
    return Degrees<T>(lhs.value * rhs).normalize();
}
template <typename T>
inline constexpr Degrees<T> operator/(Degrees<T> lhs, T rhs) {
    return Degrees<T>(lhs.value / rhs).normalize();
}

template <typename T>
inline constexpr bool operator==(Radians<T> lhs, Radians<T> rhs) {
    return lhs.value == rhs.value;
}
template <typename T>
inline constexpr bool operator!=(Radians<T> lhs, Radians<T> rhs) {
    return !(lhs == rhs);
}
template <typename T>
constexpr inline Radians<T> operator+(Radians<T> lhs, Radians<T> rhs) {
    return Radians<T>(lhs.value + rhs.value).normalize();
}
template <typename T>
inline constexpr Radians<T> operator-(Radians<T> lhs, Radians<T> rhs) {
    return Radians<T>(lhs.value - rhs.value).normalize();
}
template <typename T>
inline constexpr Radians<T> operator*(Radians<T> lhs, T rhs) {
    return Radians<T>(lhs.value * rhs).normalize();
}
template <typename T>
inline constexpr Radians<T> operator/(Radians<T> lhs, T rhs) {
    return Radians<T>(lhs.value / rhs).normalize();
}
}

#endif
