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

    using namespace bee;

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
        ASSERT_EQ(decompressed, bytes);
    }
}