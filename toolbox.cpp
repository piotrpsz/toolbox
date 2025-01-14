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

/*------- include files:
-------------------------------------------------------------------*/
#include "toolbox.h"
#include <iostream>
#include <numeric>
#include <random>
#include <charconv>
#include <format>
#include <span>

namespace bee {

    /****************************************************************
    *                                                               *
    *                        t o _ i n t                            *
    *                                                               *
    ****************************************************************/

    auto box::to_int(std::string_view sv, int const base) noexcept
    -> std::optional<int>
    {
        int value = 0;
        auto const [_, ec] = std::from_chars(sv.data(), sv.data() + sv.size(), value, base);

        if (ec == std::errc{})
            return value;

        // Coś poszło nie tak.
        if (ec == std::errc::invalid_argument)
            std::cerr << std::format("This is not a number ({}).\n", sv);
        else if (ec == std::errc::result_out_of_range)
            std::cerr << std::format("The number is to big ({}).\n", sv);
        return {};
    }

    /****************************************************************
    *                                                               *
    *                         s p l i t                             *
    *                                                               *
    ****************************************************************/
    auto box::split(
        std::string const& text,
        char const delimiter) noexcept
    -> std::vector<std::string>
    {
        // Lepiej policzyć delimitery niż później realokować wektor.
        auto const n = std::accumulate(
                text.cbegin(),
                text.cend(),
                size_t{},
                [delimiter](int const count, char const c) {
                    return c == delimiter ? count + 1 : count;
                }
        );

        // Wektor o wstępnie zaalokowanej liczbie elementów.
        std::vector<std::string> tokens{};
        tokens.reserve(n + 1);

        std::string token{};
        std::istringstream stream{text};
        while (std::getline(stream, token, delimiter))
            if (auto line = trim(token); !line.empty())
                tokens.push_back(std::move(line));

        tokens.shrink_to_fit();
        return tokens;
    }

    /****************************************************************
    *                                                               *
    *                          j o i n                              *
    *                                                               *
    ****************************************************************/

    auto box::join(
        std::span<std::string> data,
        std::string_view const delimiter) noexcept
    -> std::string
    {
        const auto tokens_size = std::accumulate(
            std::begin(data),
            std::end(data),
            size_t{},
            [](auto const count, auto const& str) {
               return count + str.size();
            });

        // String o wstępnie zaalokowanej liczbie znaków.
        std::string buffer{};
        buffer.reserve(tokens_size + data.size() - 1);

        // Ze spanu kopiujemy elementy od początku, ale bez ostatniego elementu.
        std::ranges::for_each(data.first(data.size() - 1), [&buffer, delimiter](auto&& token) {
            buffer += token + std::string(delimiter)  ;
        });
        buffer += data.back();
        return buffer;
    }

    /****************************************************************
    *                                                               *
    *                    r a n d o m _ b y t e s                    *
    *                                                               *
    ****************************************************************/

    auto box::random_bytes(size_t const n) noexcept
    -> std::vector<unsigned char>
    {
        std::random_device rd;
        std::array<int, std::mt19937::state_size> seed_data{};
        std::ranges::generate(seed_data, ref(rd));
        std::seed_seq seq(begin(seed_data), end(seed_data));

        auto mersenne_twister_engine = std::mt19937{seq};
        auto ud = std::uniform_int_distribution<>{0, 255};

        std::vector<unsigned char> buffer;
        buffer.reserve(n);
        for (auto i = 0; i < n; ++i) {
            auto const c = ud(mersenne_twister_engine);
            buffer.push_back(static_cast<unsigned char>(c));
        }
        return buffer;

        // Problem 'to<>'
        // return std::views::iota(0, n)
        //         | std::views::transform([&](auto _) { return static_cast<unsigned char>(ud(mersenne_twister_engine)); })
        //         | std::ranges::to<std::vector>();
    }

    /****************************************************************
    *                                                               *
    *                       h o m e _ d i r                         *
    *                                                               *
    ****************************************************************/

    std::string box::home_dir() {
        if (auto const pw = getpwuid(getuid()))
            return pw->pw_dir;
        return {};
    }

    /****************************************************************
    *                                                               *
    *                    c r e a t e _ d i r s                      *
    *                                                               *
    ****************************************************************/

    bool box::create_dirs(std::string_view const path) {
        if (fs::exists(path))
            return true;

        std::error_code ec{};
        if (fs::create_directories(path, ec))
            return true;

        std::cerr << std::format("{}\n", ec.message());;
        return {};
    }

}
