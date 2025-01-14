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
#include "types.h"
#include <cstring>
#include <string>
#include <vector>
#include <format>
#include <fstream>
#include <iostream>

namespace bee::file {
    static std::optional<std::vector<char>> read_binary(std::string const& fpath) {
        std::ifstream f(fpath, std::ios::in | std::ios::binary);
        if (!f.is_open()) {
            std::cerr << strerror(errno) << std::endl;
            return {};
        }

        f.seekg(0, std::ios::end);
        auto const size = f.tellg();
        f.seekg(0, std::ios::beg);

        std::vector<char> buffer;
        buffer.resize(size);
        f.read(reinterpret_cast<char*>(buffer.data()), size);
        if (!f.fail() && f.gcount() == size)
            return buffer;

        std::cerr << strerror(errno) << std::endl;
        return {};
    }

    static bool write_binary(std::string const& fpath, BytesView auto const data) {
        if (std::ofstream f(fpath, std::ios::out | std::ios::binary | std::ios::trunc); f.is_open()) {
            try {
                f.write(reinterpret_cast<char const*>(data.data()), static_cast<ssize_t>(data.size()));
            }
            catch (std::ios_base::failure const& err) {
                std::cerr << err.what() << std::endl;
                return {};
            }
            return true;
        }
        std::cerr << strerror(errno) << std::endl;
        return {};
    }

    static bool write_text(std::string const& fpath, std::string_view const data) {
        if (std::ofstream f(fpath, std::ios::out | std::ios::trunc); f.is_open()) {
            try {
                f.write(data.data(), static_cast<ssize_t>(data.size()));
            }
            catch (std::ios_base::failure const& err) {
                std::cerr << err.what() << std::endl;
                return {};
            }
            return true;
        }
        std::cerr << strerror(errno) << std::endl;
        return {};
    }

}
