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
    Hsi(const Hsi& other) = default;
    Hsi(Hsi&& other) noexcept = default;
    Hsi& operator=(const Hsi& other) = default;
    Hsi& operator=(Hsi&& other) noexcept = default;

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

    friend constexpr void swap<T>(Hsi<T>& lhs, Hsi<T>& rhs);

protected:
    using Base::_hue;
    using Base::_saturation;
    using Base::_c3;
};

template <typename T>
inline std::ostream& operator<<(std::ostream& stream, const Hsi<T>& rhs) {
    stream << "HSI(" << rhs.hue_channel() << ", " << rhs.saturation_channel()
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
