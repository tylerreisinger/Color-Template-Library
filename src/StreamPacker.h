#ifndef COLOR_STREAMPACKER_H_
#define COLOR_STREAMPACKER_H_

#include <vector>
#include <memory>
#include <ostream>

#include "Packer.h"

namespace color {

/** Adapter class for packing colors into a stream.
 */
template <typename Color>
class StreamPacker {
public:
    /** Construct a StreamPacker instance the packs Color instances to an
     *  std::ostream using a given Packer. The StreamPacker takes ownership
     *  of both the stream and the Packer.
     */
    StreamPacker(std::unique_ptr<std::ostream> owned_stream,
            std::unique_ptr<Packer<Color>> packer)
        : m_owned_stream(std::move(owned_stream)), m_packer(std::move(packer)) {
        m_stream_ptr = m_owned_stream.get();
        m_elem_buffer.resize(m_packer->packed_size());
    }

    /** Construct a StreamPacker instance the packs Color instances to an
     *  std::ostream using a given Packer. The StreamPacker instance does not
     *  take ownership of the stream, so the stream must live at least
     *  as long as the referencing StreamUnpacker. This overload is
     *  primarily provided to support the standard streams which cannot
     *  be owned.
     */
    StreamPacker(std::ostream& referenced_stream,
            std::unique_ptr<Packer<Color>> packer)
        : m_stream_ptr(&referenced_stream), m_packer(std::move(packer)) {}

    ~StreamPacker() = default;

    StreamPacker(const StreamPacker& other) = delete;
    StreamPacker(StreamPacker&& other) noexcept = default;
    StreamPacker& operator=(const StreamPacker& other) = delete;
    StreamPacker& operator=(StreamPacker&& other) noexcept = default;

    /** Pack one Color into the current position of the stream.
     *  If an error occurs, the relevant state flags of the
     *  stream object will be set accordingly and the
     *  color will not be written.
     */
    StreamPacker& operator<<(const Color& color) { return pack_single(color); }

    /// Equivalent to operator <<(const Color&).
    StreamPacker& pack_single(const Color& color) {
        auto& stream = get_stream();
        m_packer->pack_single(color, m_elem_buffer.data());
        stream.write(m_elem_buffer.data(), m_packer->packed_size());
        return *this;
    }

    /** Pack all elements between \a first and \a last.
     */
    template <typename Iterator>
    void pack(Iterator first, Iterator last) {
        for(auto it = first; it != last; ++it) {
            pack_single(*it);
        }
    }

    /// Equivalent to get_stream().good().
    bool good() const { return get_stream().good(); }

    /// Equivalent to get_stream().fail().
    bool fail() const { return get_stream().fail(); }

    /// Equivalent to get_stream().eof().
    bool eof() const { return get_stream().eof(); }

    /// Equivalent to get_stream().bad().
    bool bad() const { return get_stream().bad(); }

    /// Get the internal stream object.
    std::ostream& get_stream() { return *m_stream_ptr; }

    /// Get the internal stream object.
    const std::ostream& get_stream() const { return *m_stream_ptr; }

    /** Take ownership of the internal std::ostream.
     *  This should be treated similarly to a move operation,
     *  afterward the StreamPacker object should not be used.
     */
    std::unique_ptr<std::ostream> release_stream() {
        m_stream_ptr = nullptr;
        return std::move(m_owned_stream);
    }

private:
    std::ostream* m_stream_ptr;
    std::unique_ptr<Packer<Color>> m_packer;
    std::unique_ptr<std::ostream> m_owned_stream;
    std::vector<char> m_elem_buffer;
};
}

#endif
