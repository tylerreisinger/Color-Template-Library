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

namespace details {

template <typename Color,
        typename OutIterator,
        int idx,
        typename std::enable_if_t<(idx == -1), int> = 0>
void pack_set_elem_impl(const Color& in, OutIterator out) {
    *out = Color::ElementType(0);
}

template <typename Color,
        typename OutIterator,
        int idx,
        typename std::enable_if_t<(idx != -1), int> = 0>
void pack_set_elem_impl(const Color& in, OutIterator out) {
    *out = std::get<idx>(in.as_tuple());
}

template <typename Color, typename OutIterator, std::size_t N>
struct static_pack_single_impl {
    static void pack_single(const Color& in, OutIterator& out) {
        static_pack_single_impl<Color, OutIterator, N - 1>::pack_single(
                in, out);
        pack_set_elem_impl(in, out);
        ++out;
    }
};

template <typename Color, typename OutIterator>
struct static_pack_single_impl<Color, OutIterator, 0> {
    static void pack_single(const Color& in, OutIterator& out) {
        pack_set_elem_impl(in, out);
        ++out;
    }
};


template <typename Color, std::size_t N>
class StaticFlatColorPacker {
public:
    using ElementType = typename Color::ElementType;

    StaticFlatColorPacker(std::array<int, N> pack_order) {
        set_pack_order(std::move(pack_order));
    }

    StaticFlatColorPacker& set_pack_order(std::vector<int> value) {
        // TODO: handle the case of out-of-range indices.
        m_pack_format = std::move(value);
        return *this;
    }

    template <typename Iterator>
    Iterator pack_single(const Color& in, Iterator out) const {
        static_pack_single_impl<Color, Iterator, N - 1>::pack_single(in, out);
        return out;
    }

private:
    std::array<int, N> m_pack_format;
};
}

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
