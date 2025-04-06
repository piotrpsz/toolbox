//
// Created by piotr on 06.04.25.
//
#include <gtest/gtest.h>
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
