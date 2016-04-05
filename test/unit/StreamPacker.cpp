#include "gtest/gtest.h"

#include "Rgb.h"
#include "StreamPacker.h"
#include "StreamUnpacker.h"
#include "FlatColorPacker.h"
#include "FlatColorUnpacker.h"

#include <sstream>
#include <memory>

using namespace color;

TEST(StreamPacker, stream_pack_unpack) {
    std::array<uint8_t, 12> test_data{
            127, 255, 100, 33, 66, 111, 0, 75, 150, 120, 160, 220};

    std::vector<int> pack_format{0, 1, 2};

    auto test_stream_ptr = std::make_unique<std::stringstream>();
    auto& test_stream = *test_stream_ptr;

    test_stream.write(
            reinterpret_cast<char*>(test_data.data()), test_data.size());

    using ColorType = Rgb<uint8_t>;
    using UnpackerType = FlatColorUnpacker<ColorType>;
    using PackerType = FlatColorPacker<ColorType>;

    auto unpacker = std::make_unique<UnpackerType>(pack_format);
    auto stream_unpacker = StreamUnpacker<ColorType>(
            std::move(test_stream_ptr), std::move(unpacker));

    std::array<ColorType, 4> unpacked_colors_single;
    std::array<ColorType, 4> unpacked_colors_batch;

    for(int i = 0; i < test_data.size() / ColorType::num_channels; ++i) {
        stream_unpacker >> unpacked_colors_single[i];
    }

    stream_unpacker.get_stream().seekg(0, std::ios_base::beg);

    stream_unpacker.unpack_all(unpacked_colors_batch.begin());

    ASSERT_EQ(unpacked_colors_single, unpacked_colors_batch);

    stream_unpacker.get_stream().clear();
    stream_unpacker.get_stream().seekg(0, std::ios_base::beg);

    auto out_vec = stream_unpacker.unpack_all();

    for(int i = 0; i < unpacked_colors_single.size(); ++i) {
        ASSERT_EQ(out_vec[i], unpacked_colors_single[i]);
    }

    ASSERT_TRUE(stream_unpacker.eof());

    auto pack_stream = std::make_unique<std::stringstream>();

    auto packer = std::make_unique<PackerType>(pack_format);
    auto stream_packer =
            StreamPacker<ColorType>(std::move(pack_stream), std::move(packer));

    stream_packer.pack(
            unpacked_colors_single.begin(), unpacked_colors_single.end());
    stream_packer << ColorType(55, 110, 235);

    pack_stream =
            std::unique_ptr<std::stringstream>(static_cast<std::stringstream*>(
                    stream_packer.release_stream().release()));

    auto packed_string = pack_stream->str();
    auto end_test_data = std::array<uint8_t, 3>{55, 110, 235};

    for(int i = 0; i < test_data.size(); ++i) {
        ASSERT_EQ(static_cast<uint8_t>(packed_string[i]), test_data[i]);
    }
    for(int i = test_data.size(); i < packed_string.size(); ++i) {
        auto scaled_i = i - test_data.size();
        ASSERT_EQ(static_cast<uint8_t>(packed_string[i]),
                end_test_data[scaled_i]);
    }
}
