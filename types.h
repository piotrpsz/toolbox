// MIT License
//
// Copyright (c) 2024 Piotr Pszczółkowski
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
// Created by piotr on 09.01.25.
#pragma once

/*------- include files:
-------------------------------------------------------------------*/
#include <cstdint>
#include <span>
#include <string>
#include <vector>
#include <string_view>
#include <type_traits>

template <typename T>
concept BytesContainer = std::is_class_v<std::string> ||
                         std::is_class_v<std::vector<char>> ||
                         std::is_class_v<std::vector<unsigned char>>;

template <typename T>
concept BytesView = std::is_class_v<std::string_view> ||
                    std::is_class_v<std::span<char>> ||
                    std::is_class_v<std::span<char const>> ||
                    std::is_class_v<std::span<unsigned char>> ||
                    std::is_class_v<std::span<unsigned char const>>;


namespace bee {
    using i8 = int8_t;
    using i16 = int16_t;
    using i32 = int32_t;
    using i64 = int64_t;
    using u8 = uint8_t;
    using u16 = uint16_t;
    using u32 = uint32_t;
    using u64 = uint64_t;
    using f32 = float;
    using f64 = double;
}