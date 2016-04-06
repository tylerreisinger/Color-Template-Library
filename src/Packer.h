/** \file 
 *  Defines the Packer class.
 */
#ifndef COLOR_PACKER_H_
#define COLOR_PACKER_H_

#include <vector>

namespace color {
static constexpr int packer_index_skip = -1;

/** Base class for all Packer types.
 *  Provides a base for classes that serialize colors into arrays of elements.
 */
template <typename Color>
class Packer {
public:
    virtual ~Packer() {}

    /// Get the number of bytes needed to pack a Color.
    virtual std::size_t packed_size() const = 0;

    /** Pack a color into a buffer.
     *  \a out needs to have at least Packer::packed_size() free bytes
     *  available or pack_single will overflow the buffer.
     *  \returns A pointer to one byte after the written data in \a out.
     */
    virtual void* pack_single(const Color& src, void* out) const = 0;

    /** Pack a collection of colors into a buffer.
     *  All elements from \a first to \a last are packed
     *  into \a out. \a out must be large enough to hold
     *  all of the packed data, or pack will overflow the buffer.
     *
     *  `Packer::packed_size() * element_count` is the number of
     *  bytes requires to pack `element_count`colors.
     *
     *  \returns A pointer to one byte after the written data in \a out.
     */
    template <typename Iterator>
    void* pack(Iterator first, Iterator last, void* out) const {
        for(auto it = first; it != last; ++it) {
            out = pack_single(*it, out);
        }
        return out;
    }
};
}

#endif
