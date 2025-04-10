//
// Created by piotr on 06.04.25.
//
/*
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "../toolbox.h"

TEST(Toolbox, trim_left) {
    struct Test {
        std::string input;
        std::string expected;
    } tests[] = {
                {"", ""},
                {"text", "text"},
                {" text", "text"},
                {"  text", "text"},
                {"   text", "text"},
                {"    text", "text"},
                {"     text", "text"},
                {"      text", "text"},
                {"       text", "text"},
            };

    using namespace bee;

    for (auto&& [input, expected]: tests) {
        auto const result = box::trim_left(input);
        EXPECT_EQ(result, expected);
    }
}

TEST(Toolbox, trim_right) {
    struct Test {
        std::string input;
        std::string expected;
    } tests[] = {
                {"", ""},
                {"text", "text"},
                {"text ", "text"},
                {"text  ", "text"},
                {"text   ", "text"},
                {"text    ", "text"},
                {"text     ", "text"},
                {"text      ", "text"},
                {"text       ", "text"},
            };

    using namespace bee;

    for (auto&& [input, expected]: tests) {
        auto const result = box::trim_right(input);
        EXPECT_EQ(result, expected);
    }
}

TEST(Toolbox, trim) {
    using namespace bee;

    struct Test {
        std::string input;
        std::string expected;
    } tests[] = {
                {"", ""},
                {"text", "text"},
                {" text ", "text"},
                {"  text  ", "text"},
                {"   text   ", "text"},
                {"    text    ", "text"},
                {"     text     ", "text"},
                {"      text      ", "text"},
                {"        text       ", "text"},
            };

    for (auto&& [input, expected]: tests) {
        auto const result = box::trim(input);
        EXPECT_EQ(result, expected);
    }
}

TEST(Toolbox, lzav_compress_decompress) {
    using namespace bee;

    for (auto i = 0; i < 2048; ++i) {
        auto const bytes = box::random_bytes<char>(i);
        const auto compressed = box::compress(bytes);
        auto const decompressed = box::decompress(compressed);
        EXPECT_EQ(decompressed, bytes);
    }
}

TEST(Toolbox, split) {
    using namespace bee;

    struct Test {
        std::string input;
        std::vector<std::string> expected;
    } tests[] = {
                {"", {}},
                {"a", {"a"}},
                {"a, b", {"a", "b"}},
                {"a, , , b", {"a", "b"}},
                {"a,,b, ,\n,  c,, d,e, f", {"a", "b", "c", "d", "e", "f"}},
                {"\n, , a,,b , ,\n,  c,, d,e, f, \n, ,", {"a", "b", "c", "d", "e", "f"}},
            };

    for (auto&& [input, expected]: tests) {
        auto const retv = box::split(input, ',');
        ASSERT_EQ(retv, expected);
    }
}

TEST(Toolbox, join) {
    using namespace bee;

    struct Test {
        std::vector<std::string> input;
        std::string expected;
    } tests[] = {
                {{}, ""},
                {{"a"}, "a"},
                {{"a", "b", "", "c", ""}, "a,b,c"}
            };

    for (auto&& [input, expected]: tests) {
        auto const retv = box::join(input, ",");
        ASSERT_EQ(retv, expected);
    }
}

TEST(Toolbox, bytes_to_string) {
    using namespace bee;

    struct Test {
        std::vector<u8> input;
        std::string expected;
    } tests[] = {
                {{}, ""},
                {
                    {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
                    "0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f"
                },
                {
                    {0xab, 0xcd, 0xef, 0xba, 0xbb, 0xbe, 0xef},
                    "0xab, 0xcd, 0xef, 0xba, 0xbb, 0xbe, 0xef"
                }
            };

    for (auto&& [input, expected]: tests) {
        auto const retv = box::bytes_to_string(input, true);
        ASSERT_EQ(retv, expected);
    }
}

TEST(Toolbox, bytes_to_string_n) {
    using namespace bee;

    struct Test {
        std::vector<u8> input;
        int n;
        std::string expected;
    } tests[] = {
                {
                    {},
                    3,
                    ""
                },
                {
                    {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
                    0,
                    ""
                },
                {
                    {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
                    5,
                    "0x01, 0x02, 0x03, 0x04, 0x05"
                },
                {
                    {0xab, 0xcd, 0xef, 0xba, 0xbb, 0xbe, 0xef},
                    3,
                    "0xab, 0xcd, 0xef"
                },
                {
                    {0xab, 0xcd, 0xef, 0xba, 0xbb, 0xbe, 0xef},
                    100,
                    "0xab, 0xcd, 0xef, 0xba, 0xbb, 0xbe, 0xef"
                }
            };

    for (auto&& [input, n, expected]: tests) {
        auto const retv = box::bytes_to_string(input, n, true);
        ASSERT_EQ(retv, expected);
    }
}

TEST(Toolbox, to_string_integral) {
    using namespace bee;

    struct Test {
        int n;
        std::string expected;
    } tests[] = {
                {0, "0"},
                {1, "1"},
                {12, "12"},
                {123, "123"},
                {1234, "1.234"},
                {12345, "12.345"},
                {123456, "123.456"},
                {1234567, "1.234.567"},
                {-1, "-1"},
                {-12, "-12"},
                {-123, "-123"},
                {-1234, "-1.234"},
                {-12345, "-12.345"},
                {-123456, "-123.456"},
                {-1234567, "-1.234.567"}
            };

    for (auto&& [n, expected]: tests) {
        auto const retv = box::to_string(n);
        ASSERT_EQ(retv, expected);
    }
}

TEST(Toolbox, to_string_float) {
    using namespace bee; {
        struct Test {
            double value;
            std::string expected;
        } tests[] = {
            {0, "0,00"},
            {1.1, "1,10"},
            {12.12, "12,12"},
            {123.123, "123,12"},
            {1234.1234, "1.234,12"},
            {12345.1234, "12.345,12"},
            {123456.1234, "123.456,12"},
            {1234567.1234, "1.234.567,12"},
            {0, "0,00"},
            {-1.1, "-1,10"},
            {-12.12, "-12,12"},
            {-123.123, "-123,12"},
            {-1234.1234, "-1.234,12"},
            {-12345.1234, "-12.345,12"},
            {-123456.1234, "-123.456,12"},
            {-1234567.1234, "-1.234.567,12"},
        };

        for (auto&& [value, expected]: tests) {
            auto const retv = box::to_string(value);
            ASSERT_EQ(retv, expected);
        }
    } {
        struct Test {
            double value;
            int digits_after_decpoint; // digits number after decimal point
            std::string expected;
        } tests[] = {
            {0, 3, "0,000"},
            {1.1, 3, "1,100"},
            {12.12, 3, "12,120"},
            {123.123, 3, "123,123"},
            {1234.1234, 3, "1.234,123"},
            {12345.12346, 4, "12.345,1235"},
            {123456.1234, 3, "123.456,123"},
            {1234567.1234, 3, "1.234.567,123"},
            {0, 3, "0,000"},
            {-1.1, 3, "-1,100"},
            {-12.12, 3, "-12,120"},
            {-123.123, 3, "-123,123"},
            {-1234.12346, 4, "-1.234,1235"},
            {-12345.1234, 3, "-12.345,123"},
            {-123456.1234, 3, "-123.456,123"},
            {-1234567.1234, 3, "-1.234.567,123"},
        };
        for (auto&& [value, digits_after_decpoint, expected]: tests) {
            auto const retv = box::to_string(value, digits_after_decpoint);
            ASSERT_EQ(retv, expected);
        }
    }
    {
        struct Test {
            double value;
            int digits_after_decpoint; // digits number after decimal point
            char decimal_point;
            char thousand_separator;
            std::string expected;
        } tests[] = {
            {1234.1234, 3, '.', '`', "1`234.123"},
            {1234567.1234, 3, ',', ' ', "1 234 567,123"},
            {1234567.1234, 4, '#', '$', "1$234$567#1234"},
        };
        for (auto&& [value, digits_after_decpoint, point, separator, expected]: tests) {
            auto const retv = box::to_string(value, digits_after_decpoint, point, separator);
            ASSERT_EQ(retv, expected);
        }
    }
}
*/
