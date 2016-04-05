#include "gtest/gtest.h"

#include "Rgb.h"
#include "Alpha.h"
#include "FlatColorUnpacker.h"
#include "FlatColorPacker.h"

using namespace color;

TEST(Unpack, unpack_single) {
    //Test unpacking in order
    {
        auto in_data = std::array<uint8_t, 3>{50, 150, 250};
        auto unpacker = FlatColorUnpacker<Rgb<uint8_t>>({0, 1, 2});
        Rgb<uint8_t> out_color;
        unpacker.unpack_single(in_data.begin(), out_color);

        ASSERT_EQ(out_color, Rgb<uint8_t>(50, 150, 250));
    }
    //Test unpacking in reverse order
    {
        auto in_data = std::array<uint8_t, 3>{50, 150, 250};
        auto unpacker = FlatColorUnpacker<Rgb<uint8_t>>({2, 1, 0});
        Rgb<uint8_t> out_color;
        unpacker.unpack_single(in_data.begin(), out_color);

        ASSERT_EQ(out_color, Rgb<uint8_t>(250, 150, 50));
    }
    //Test unpacking XRGB
    {
        auto in_data = std::array<uint8_t, 4>{255, 100, 200, 50};
        auto unpacker = FlatColorUnpacker<Rgb<uint8_t>>({-1, 0, 1, 2});
        Rgb<uint8_t> out_color;
        unpacker.unpack_single(in_data.begin(), out_color);

        ASSERT_EQ(out_color, Rgb<uint8_t>(100, 200, 50));
        ASSERT_EQ(unpacker.packed_size(), sizeof(uint8_t)*4);
    }

}

TEST(Unpack, invalid_pack_format) {
    //Test invalid packing format
    {
        ASSERT_THROW({
            auto packer = FlatColorUnpacker<Rgb<float>>({0, 2, 1, 3});
        }, InvalidPackingFormatError);
        ASSERT_THROW({
            auto packer = FlatColorUnpacker<Rgb<float>>({0, 2, 1, -2});
        }, InvalidPackingFormatError);
    }
}

TEST(Unpack, unpack) {
    auto in_data = std::array<float, 12>{0.05, 0.1, 0.2, 0.27, 0.32, 0.35,
        0.45, 0.50, 0.57, 0.67, 0.80, 0.92};
    auto out_data = std::array<float, 12>();
    auto colors = std::array<Rgba<float>, 3>();

    auto pack_order = std::vector<int>{3, 0, 1, 2};
    auto packer = FlatColorPacker<Rgba<float>>(pack_order);
    auto unpacker = FlatColorUnpacker<Rgba<float>>(pack_order);

    unpacker.unpack(in_data.data(), in_data.size()*sizeof(float), colors.begin());
    packer.pack(colors.begin(), colors.end(), out_data.data());

    for(int i = 0; i < in_data.size(); ++i) {
        ASSERT_FLOAT_EQ(in_data[i], out_data[i]);
    }
    ASSERT_EQ(packer.packed_size(), sizeof(float)*4);
    ASSERT_EQ(unpacker.packed_size(), sizeof(float)*4);
}
