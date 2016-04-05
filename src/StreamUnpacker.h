#ifndef COLOR_STREAMUNPACKER_H_
#define COLOR_STREAMUNPACKER_H_

#include <istream>
#include <memory>
#include <vector>
#include <limits>

#include "Unpacker.h"

namespace color {

/** Adapter class for unpacking colors from a stream.
 */
template <typename Color>
class StreamUnpacker {
public:
    /** Construct a StreamUnpacker instance that unpacks from an std::istream
     *  using a given Unpacker. The StreamUnpacker takes ownership of both
     *  the stream and Packer.
     */
    StreamUnpacker(std::unique_ptr<std::istream> owned_stream,
            std::unique_ptr<Unpacker<Color>> unpacker)
        : m_owned_stream(std::move(owned_stream)),
          m_unpacker(std::move(unpacker)) {
        m_stream_ptr = m_owned_stream.get();
        m_elem_buffer.resize(m_unpacker->packed_size());
    }

    /** Construct a StreamUnpacker instance that unpacks from an std::istream
     *  using a given Unpacker. The StreamUnpacker does not take ownership of
     * the
     *  the stream, so the stream must live at least
     *  as long as the referencing StreamUnpacker. This overload is primarily
     *  provided to support the standard streams which cannot be owned.
     */
    StreamUnpacker(
            std::istream& stream, std::unique_ptr<Unpacker<Color>> unpacker)
        : m_unpacker(std::move(unpacker)), m_stream_ptr(&stream) {}

    ~StreamUnpacker() = default;

    StreamUnpacker(const StreamUnpacker& other) = delete;
    StreamUnpacker(StreamUnpacker&& other) noexcept = default;
    StreamUnpacker& operator=(const StreamUnpacker& other) = delete;
    StreamUnpacker& operator=(StreamUnpacker&& other) noexcept = default;

    /** Unpack one Color at the current position in the stream.
     *  If the stream does not have enough data to unpack a color,
     *  or the read encounters an error, color will be unmodified
     *  and the state flags of the internal stream object will be
     *  adjusted appropriately.
     */
    StreamUnpacker& operator>>(Color& color) { return unpack_single(color); }

    /// Equivalent to StreamUnpacker::operator>>(Color&)
    StreamUnpacker& unpack_single(Color& color) {
        auto& stream = get_stream();
        auto color_size = m_unpacker->packed_size();
        stream.read(m_elem_buffer.data(), color_size);
        if(stream.gcount() == color_size) {
            m_unpacker->unpack_single(m_elem_buffer.data(), color);
        }
        return *this;
    }

    /** Unpack up to \a n colors from the stream, writing them
     *  to the iterator \a out. If the internal stream encounters
     *  an error or the end of the stream is reached before \a n
     *  elements are unpacked, the function will terminate early.
     *
     *  Note that \a out is updated to one past the last element
     *  written when the function returns.
     *
     *  \returns The number of colors successfully unpacked.
     */
    template <typename OutIterator>
    std::streamsize unpack(std::streamsize n, OutIterator&& out) {
        for(decltype(n) i = 0; i < n; ++i) {
            // Passing *out to unpack_single does not work
            // for insertion iterators.
            auto color = Color();
            unpack_single(color);
            *out = color;
            if(!good()) {
                return i;
            }
            ++out;
        }
        return n;
    }

    /// Unpacks as many colors as can be extracted from the stream.
    /// Equivalent to StreamUnpacker::unpack with a sufficiently large \a n.
    template <typename OutIterator>
    std::streamsize unpack_all(OutIterator&& out) {
        return unpack(std::numeric_limits<std::streamsize>::max(), out);
    }

    /// Unpack all colors to a vector.
    std::vector<Color> unpack_all() {
        auto out = std::vector<Color>();
        unpack_all(std::back_inserter(out));
        return out;
    }

    /// Equivalent to get_stream().good().
    bool good() const { return get_stream().good(); }

    /// Equivalent to get_stream().fail().
    bool fail() const { return get_stream().fail(); }

    /// Equivalent to get_stream().eof().
    bool eof() const { return get_stream().eof(); }

    /// Equivalent to get_stream().bad().
    bool bad() const { return get_stream().bad(); }

    /// Get the internal std::istream instance.
    std::istream& get_stream() { return *m_owned_stream; }

    /// Get the internal std::istream instance.
    const std::istream& get_stream() const { return *m_stream_ptr; }

    /// Get the internal Unpacker.
    const Unpacker<Color>& get_unpacker() const { return *m_unpacker; }

    /** Take ownership of the internal std::istream.
     *  This should be treated similarly to a move operation,
     *  afterward the StreamUnpacker object should not be used.
     */
    std::unique_ptr<std::istream> release_stream() {
        m_stream_ptr = nullptr;
        return std::move(m_owned_stream);
    }

private:
    std::unique_ptr<std::istream> m_owned_stream;
    std::unique_ptr<Unpacker<Color>> m_unpacker;
    std::istream* m_stream_ptr;

    std::vector<char> m_elem_buffer;
};
}

#endif
