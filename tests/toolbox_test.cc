//
// Created by piotr on 06.04.25.
//
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

    for (auto&& [input, expected] : tests) {
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

    for (auto&& [input, expected] : tests) {
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

    for (auto&& [input, expected] : tests) {
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
        { "", {}},
        {"a", { "a"}},
        {"a, b", {"a", "b"}},
        {"a, , , b", {"a", "b"}},
        {"a,,b, ,\n,  c,, d,e, f", {"a", "b", "c", "d", "e", "f"}},
        {"\n, , a,,b , ,\n,  c,, d,e, f, \n, ,", {"a", "b", "c", "d", "e", "f"}},
    };

    for (auto&& [input, expected] : tests) {
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
        { {}, ""},
        {{"a"},"a"},
        {{"a", "b", "",  "c", ""}, "a,b,c"}
    };

    for (auto&& [input, expected] : tests) {
        auto const retv = box::join(input, ",");
        ASSERT_EQ(retv, expected);
    }
}
