#ifndef COLOR_UNPACKER_H_
#define COLOR_UNPACKER_H_

#include <cassert>
#include <cstdint>
#include <vector>

namespace color {

/** Base class for all Unpacker types.
 *  Provides a base for all classes that de-serialize colors
 *  from an array of bytes.
 */
template <typename Color>
class Unpacker {
public:
    virtual ~Unpacker(){};

    /// Get the number of bytes a packed Color occupies.
    virtual std::size_t packed_size() const = 0;

    /** Unpack a color from \a in into \a out.
     *  \a in must contain at least `Unpacker::packed_size()`
     *  bytes containing a packed color.
     *  \returns A pointer to one byte after the read data in \a src.
     */
    virtual const void* unpack_single(const void* src, Color& out) const = 0;

    /** Unpack colors from a buffer.
     *  \param src should point to the first element of the buffer and
     *  should be a multiple of Unpacker::packed_size() in length.
     *  \param num_bytes should contain the number of **bytes** in the input
     * array.
     *  \param out An iterator to the beginning of the output range. The
     *  backing container needs to have enough space to hold all of the unpacked
     *  colors. \a out will be overwritten with an iterator to the element after
     *  the last inserted element.
     *
     *  \returns A pointer to one byte after the read data in \a src.
     */
    template <typename OutIterator>
    const void* unpack(
            const void* src, std::size_t num_bytes, OutIterator&& out) {
        assert(num_bytes % packed_size() == 0 &&
                "src must have a length that is a multiple of packed_size()");
        auto last = reinterpret_cast<void*>(
                reinterpret_cast<uintptr_t>(src) + num_bytes);
        while (src != last) {
            auto color = Color();
            src = unpack_single(src, color);
            *out = color;
            ++out;
        }
        return src;
    }

    /** Unpack colors from a buffer into an std::vector.
     *  Same as Unpacker::unpack(const void*, std::size_t, OutIterator&&),
     *  but returns a vector holding the unpacked colors.
     */
    std::vector<Color> unpack(const void* src, std::size_t num_bytes) {
        std::vector<Color> out(num_bytes / packed_size());
        unpack(src, num_bytes, out.begin());
        return out;
    }
};
}

#endif
