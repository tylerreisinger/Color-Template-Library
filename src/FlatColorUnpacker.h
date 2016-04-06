/** \file
 *  Defines the FlatColorUnpacker class.
 */
#ifndef COLOR_FLATCOLORUNPACKER_H_
#define COLOR_FLATCOLORUNPACKER_H_

#include "Unpacker.h"

#include <vector>
#include <string>

#include "Exceptions.h"

namespace color {

/** Unpacker class for unpacking a color from an array of components without
 * conversion.
 *  FlatColorUnpacker is the unpacking counterpart to FlatColorPacker and
 * supports
 *  unpacking anything it can pack. Specifically, FlatColorUnpacker supports
 *  reordering components and skipping array elements to match most pixel
 * formats.
 *
 *  FlatColorUnpacker is configured the same way as FlatColorPacker.
 */
template <typename Color>
class FlatColorUnpacker : public Unpacker<Color> {
public:
    using ElementType = typename Color::ElementType;

    FlatColorUnpacker() = default;

    FlatColorUnpacker(std::vector<int> pack_format) {
        set_packing_format(std::move(pack_format));
    }

    virtual ~FlatColorUnpacker() {}

    FlatColorUnpacker(const FlatColorUnpacker& other) = default;
    FlatColorUnpacker(FlatColorUnpacker&& other) noexcept = default;
    FlatColorUnpacker& operator=(const FlatColorUnpacker& other) = default;
    FlatColorUnpacker& operator=(FlatColorUnpacker&& other) noexcept = default;

    virtual std::size_t packed_size() const override {
        return m_pack_format.size() * sizeof(ElementType);
    }

    virtual const void* unpack_single(
            const void* in, Color& out) const override {
        auto in_elems = reinterpret_cast<const ElementType*>(in);
        for(auto elem : m_pack_format) {
            if(elem != -1) {
                out.data()[elem] = *in_elems;
            }
            ++in_elems;
        }
        return in_elems;
    }

    /** Set the packing format.
     *  \throw InvalidPackingFormatError An out-of-range index
     *  was supplied in \a value.
     */
    FlatColorUnpacker& set_packing_format(std::vector<int> value) {
        for(int i = 0; i < value.size(); ++i) {
            auto elem = value[i];
            if(elem >= Color::num_channels || elem < -1) {
                std::string error_mesg =
                        "Out of range value in packing format: ";
                error_mesg += std::to_string(elem) + " at index ";
                error_mesg += std::to_string(i);
                throw InvalidPackingFormatError(std::move(error_mesg));
            }
        }
        m_pack_format = std::move(value);
        return *this;
    }

    /// Return the packing format
    const std::vector<int>& packing_format() const { return m_pack_format; }

private:
    std::vector<int> m_pack_format;
};
}

#endif
