#ifndef COLOR_CONVERTUTIL_H_
#define COLOR_CONVERTUTIL_H_

#include <algorithm>
#include <utility>

namespace color {
namespace details {

///Returns a scaling factor used in hue computations.
template <typename Chan, typename FloatType = Chan>
constexpr inline FloatType order_channels_for_hue(Chan& c1, Chan& c2, 
        Chan& c3, Chan& min_channel_out) {
    using std::swap;

    auto hue_scaling_factor = FloatType(0.0);

    if(c2 < c3) {
        swap(c2, c3);
        hue_scaling_factor = FloatType(-1.0);
    }
    min_channel_out = c3;
    if(c1 < c2) {
        swap(c1, c2);
        hue_scaling_factor = FloatType(-1.0 / 3.0) - hue_scaling_factor;
        min_channel_out = std::min(c2, c3);
    }

    return hue_scaling_factor;
}

template<typename T>
constexpr inline T chroma(T max_channel, T min_channel) {
    return max_channel - min_channel;
}

//Computer the hue from a chroma, scaling factor and two (not max) channels.
//EPSILON is used internally to prevent division by zero in the case
//of `chroma=0`.
template<typename T>
constexpr inline T hue(T chroma, T scaling, T c2, T c3, const T EPSILON = 1e-10) {
    return std::abs(scaling + (c2 - c3) / (6.0 * chroma + EPSILON));
}


}
}

#endif
