#include "gtest/gtest.h"

#include "Rgb.h"
#include "Alpha.h"
#include "FlatColorPacker.h"

using namespace color;

TEST(FlatColorPacker, constructor) {
    using ColorType = Rgb<float>;

    auto packer = FlatColorPacker<ColorType>();
    auto packer2 = FlatColorPacker<ColorType>({0, 1, 2});
}

TEST(FlatColorPacker, pack_single) {

    //Test packing in the in-memory order.
    {
        std::array<float, 3> values;
        auto color = Rgb<float>(0.1, 0.2, 0.3);
        auto packer = FlatColorPacker<Rgb<float>>({0, 1, 2});
        packer.pack_single(color, values.data());

        ASSERT_FLOAT_EQ(values[0], color.data()[0]);
        ASSERT_FLOAT_EQ(values[1], color.data()[1]);
        ASSERT_FLOAT_EQ(values[2], color.data()[2]);
    }

    //Test packing an RGB as a BGR
    {
        std::array<uint8_t, 3> values;
        auto test_array = std::array<uint8_t, 3>{225, 125, 50};

        auto color = Rgb<uint8_t>(50, 125, 225);
        auto packer = FlatColorPacker<decltype(color)>({2, 1, 0});
        packer.pack_single(color, values.data());

        ASSERT_EQ(values, test_array);
    }

    //Test packing an RGB as XRGBXRGB
    {
        std::array<uint16_t, 8> values;
        auto test_array = std::array<uint16_t, 8>{0, 10000, 25000, 50000,
            0, 10000, 25000, 50000};

        auto color = Rgb<uint16_t>(10000, 25000, 50000);
        auto packer = FlatColorPacker<decltype(color)>({packer_index_skip, 0, 1, 2,
                packer_index_skip, 0, 1, 2});
        packer.pack_single(color, values.data());

        ASSERT_EQ(values, test_array);
    }

    //Test packing RGBA as ARGB
    {
        std::array<uint8_t, 4> values;
        auto test_array = std::array<uint8_t, 4>{255, 52, 104, 222};

        auto color = Rgba<uint8_t>(52, 104, 222, 255);
        auto packer = FlatColorPacker<decltype(color)>({3, 0, 1, 2});
        packer.pack_single(color, values.data());

        ASSERT_EQ(values, test_array);
    }

    //Test packing to vector.
    /*{
        auto test_array = std::array<uint8_t, 4>{33, 22, 11, 44};

        auto color = Rgba<uint8_t>(11, 22, 33, 44);
        auto packer = FlatColorPacker<decltype(color)>({2, 1, 0, 3});
        auto vec = packer.pack_single_to_vector(color);

        for(int i = 0; i < vec.size(); ++i) {
            ASSERT_EQ(test_array[i], vec[i]);
        }
    }*/
}

TEST(FlatColorPacker, pack) {
    {
        auto colors = std::array<Rgb<uint8_t>, 4>{Rgb<uint8_t>(1, 2, 3), 
            {4, 5, 6}, {7, 8, 9}, {10, 11, 12}};
        std::array<uint8_t, 12> values;
        auto test_array = std::array<uint8_t, 12>{3, 2, 1, 6, 5, 4, 9, 8, 7, 12, 11, 10};
        
        auto packer = FlatColorPacker<Rgb<uint8_t>>({2, 1, 0});
        packer.pack(colors.begin(), colors.end(), values.begin());

        ASSERT_EQ(values, test_array);
    }
}
