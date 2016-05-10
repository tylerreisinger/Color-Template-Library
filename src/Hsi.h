#ifndef COLOR_HSI_H_
#define COLOR_HSI_H_

#include "CylindricalColor.h"
#include "ConvertUtil.h"

#include <array>
#include <tuple>

namespace color {

template <typename T>
class Hsi;
template <typename T, template <typename> class Color>
class Alpha;

template <typename T>
constexpr void swap(Hsi<T>& lhs, Hsi<T>& rhs);

template <typename T>
using Hsia = Alpha<T, Hsi>;

template <typename T>
class Hsi : public CylindricalColor<T, Hsi<T>> {
    using Base = CylindricalColor<T, Hsi<T>>;
    friend class CylindricalColor<T, Hsi<T>>;

public:
    static constexpr int num_channels = Base::num_channels;

    using ElementType = typename Base::ElementType;
    using TupleType = typename Base::TupleType;
    using ChannelTupleType = typename Base::ChannelTupleType;
    using ConstChannelTupleType = typename Base::ConstChannelTupleType;

    ~Hsi() = default;
    constexpr Hsi(const Hsi& other) = default;
    constexpr Hsi(Hsi&& other) noexcept = default;
    constexpr Hsi& operator=(const Hsi& other) = default;
    constexpr Hsi& operator=(Hsi&& other) noexcept = default;

    constexpr Hsi() : Base() {}
    constexpr Hsi(T hue, T saturation, T intensity)
        : Base(hue, saturation, intensity) {}

    template <template <typename> class Angle, typename U>
    constexpr Hsi(Angle<T> hue, T saturation, T intensity)
        : Base(hue, saturation, intensity) {}
    explicit constexpr Hsi(const T* values)
        : Hsi(values[0], values[1], values[2]) {}
    explicit constexpr Hsi(const std::array<T, num_channels>& values)
        : Hsi(values.data()) {}
    explicit constexpr Hsi(const TupleType& values)
        : Hsi(std::get<0>(values), std::get<1>(values), std::get<2>(values)) {}

    constexpr BoundedChannel<T>& intensity_channel() { return _c3; }
    constexpr BoundedChannel<T> intensity_channel() const { return _c3; }
    constexpr T& intensity() { return _c3.value; }
    constexpr T intensity() const { return _c3.value; }

    constexpr Hsi<T>& set_intensity(T value) {
        _c3.value = value;
        return *this;
    }

    /** Compute the maximum intensity value that will keep this color in
     *  gamut. Giving a color with the same hue and saturation as this
     *  an intensity greater than the returned value will make the
     *  color no longer correspond to a valid in-range RGB color.
     *
     *  The returned value is computed by projecting the color onto
     *  the surface that divides the in-gamut region to the out-of-gamut
     *  region. An approximation to hue is used, which makes this function
     *  considerably faster than converting and testing, but at the expense
     *  of some accuracy.
     *
     *  The maximum error tolerance is +/- 0.015 of the RGB result, so a
     *  color with one RGB channel value of at most 1.015 could potentially
     *  appear in-gamut with regards to this function.
     */
    constexpr T max_in_gamut_intensity() const {
        const T scaled_hue = _hue.value * 3.0;
        const int seg = static_cast<int>(scaled_hue);
        const T hue_param = T(1.0 / 3.0) * (scaled_hue - seg * T(1.0));

        // hue_alpha is the linear interpolation factor along
        // the changing RGB coordinate in the hue function.
        T hue_alpha = 0.0;
        if(hue_param <= T(1.0 / 6.0)) {
            hue_alpha = T(6.0) * hue_param;
        } else {
            hue_alpha = T(6.0) * (T(1.0 / 3.0) - hue_param);
        }

        const auto s = _saturation.value;

        // We are looking for a maximum intensity, so
        // assume the highest channel is always 1.0.
        const T max_channel = 1.0;

        const T min_channel =
                ((hue_alpha + T(1.0)) * max_channel * (s - T(1.0))) /
                (hue_alpha * (s - T(1.0)) - 2 * s - T(1.0));

        const T max_intensity = T(1.0 / 3.0) *
                (max_channel + min_channel + hue_alpha * max_channel +
                                        (T(1.0) - hue_alpha) * min_channel);

        return max_intensity;
    }

    /// Compare max_in_gamut_intensity() to intensity(), returning true if
    /// intensity() is smaller.
    constexpr bool is_in_gamut() const {
        return intensity() <= max_in_gamut_intensity();
    }

    friend constexpr void swap<T>(Hsi<T>& lhs, Hsi<T>& rhs);

protected:
    using Base::_hue;
    using Base::_saturation;
    using Base::_c3;
};

template <typename T>
inline std::ostream& operator<<(std::ostream& stream, const Hsi<T>& rhs) {
    stream << "Hsi(" << rhs.hue_channel() << ", " << rhs.saturation_channel()
           << ", " << rhs.intensity_channel() << ")";
    return stream;
}

template <typename T>
constexpr inline void swap(Hsi<T>& lhs, Hsi<T>& rhs) {
    swap(lhs._hue, rhs._hue);
    swap(lhs._saturation, rhs._saturation);
    swap(lhs._c3, rhs._c3);
}
}

#endif
