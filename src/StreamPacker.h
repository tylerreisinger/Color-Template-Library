#ifndef COLOR_STREAMPACKER_H_
#define COLOR_STREAMPACKER_H_

#include <memory>
#include <ostream>

template <typename Color>
class StreamPacker {
public:
    StreamPacker(std::unique_ptr<std::ostream> owned_stream,
            std::unique_ptr<Packer<Color>> packer)
        : m_owned_stream(std::move(owned_stream)), m_packer(std::move(packer)) {
    }

    StreamPacker(std::ostream& referenced_stream,
            std::unique_ptr<Packer<Color>> packer)
        : m_referenced_stream(&referenced_stream), m_packer(std::move(packer)) {
    }


    ~StreamPacker() {}

    StreamPacker(const StreamPacker& other) = delete;
    StreamPacker(StreamPacker&& other) noexcept = default;
    StreamPacker& operator=(const StreamPacker& other) = delete;
    StreamPacker& operator=(StreamPacker&& other) noexcept = default;

private:
    std::ostream* m_referenced_stream;
    std::unique_ptr<Packer<Color>> m_packer;
    std::unique_ptr<std::ostream> m_owned_stream;
};

#endif
