/** \file 
 *  Defines the FlatColorPacker class.
 */
#ifndef COLOR_FLATCOLORPACKER_H_
#define COLOR_FLATCOLORPACKER_H_

#include "Packer.h"

#include <tuple>
#include <type_traits>
#include <string>

#include "Exceptions.h"


namespace color {

/** Packer class for packing color components into an array without conversion.
 *  FlatColorPacker supports reordering components, skipping elements and
 *  replicating component values in order to adapt to many pixel formats.
 *
 *  FlatColorPacker is configured with a vector of component indices. Each
 *  element in this vector specifies a component to be written, or
 *  `packer_index_skip` to skip an element. The vector is iterated through
 *  for every color packed, and the specified color component is written,
 *  in order, for every element of the pack order vector.
 *
 *  Thus, packing a color writes sizeof(Color::ElementType)*`pack_order.size()`
 *  bytes.
 */
template <typename Color>
class FlatColorPacker : public Packer<Color> {
public:
    using ElementType = typename Color::ElementType;

    FlatColorPacker() = default;
    FlatColorPacker(std::vector<int> pack_order) {
        set_packing_format(std::move(pack_order));
    }

    FlatColorPacker(const FlatColorPacker& other) = default;
    FlatColorPacker(FlatColorPacker&& other) noexcept = default;
    FlatColorPacker& operator=(const FlatColorPacker& other) = default;
    FlatColorPacker& operator=(FlatColorPacker&& other) noexcept = default;

    virtual ~FlatColorPacker() {}

    virtual std::size_t packed_size() const override {
        return m_pack_format.size() * sizeof(ElementType);
    }

    virtual void* pack_single(const Color& in, void* out) const override {
        auto data = in.data();
        auto out_elems = reinterpret_cast<ElementType*>(out);

        for(auto elem : m_pack_format) {
            if(elem != packer_index_skip) {
                *out_elems = data[elem];
            } else {
                *out_elems = ElementType(0);
            }
            ++out_elems;
        }
        return out_elems;
    }

    /** Set the packing format.
     *  \throw InvalidPackingFormatError An out-of-range index
     *  was supplied in \a value.
     */
    FlatColorPacker& set_packing_format(std::vector<int> value) {
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

protected:
    std::vector<int> m_pack_format;
};
}

#endif
