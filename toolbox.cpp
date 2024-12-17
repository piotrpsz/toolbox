// MIT License
//
// Copyright (c) 2023 Piotr Pszczółkowski
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
#include <numeric>
#include <random>
#include <charconv>

namespace bee {
    auto box::to_int(std::string_view sv, int const base) noexcept
    -> std::optional<int>
    {
        int value = 0;
        auto const [_, ec] = std::from_chars(sv.data(), sv.data() + sv.size(), value, base);

        if (ec == std::errc{})
            return value;

        // Coś poszło nie tak.
        if (ec == std::errc::invalid_argument)
            std::println(stderr, "This is not a number ({}).\n", sv);
        else if (ec == std::errc::result_out_of_range)
            std::println(stderr, "The number is to big ({}).\n", sv);
        return {};
    }

    auto box::to_string(
        std::integral auto const value,
        char const separator) noexcept
    -> std::string
    {
        auto const text = std::to_string(value);
        auto const size = text.size();
        std::vector<char> buffer;
        buffer.reserve(size + size/3);
        auto offset = 0;

        if (value < 0) {
            buffer.push_back('-');
            offset = 1;
        }

        auto comma_idx = 3 - (size - offset) % 3;
        if (comma_idx == 3) comma_idx = 0;

        for (auto i = offset; i < size; ++i) {
            if (comma_idx == 3) {
                buffer.push_back(separator);
                comma_idx = 0;
            }
            ++comma_idx;
            buffer.push_back(text[i]);
        }

        return {buffer.begin(), buffer.end()};
    }

    auto box::to_string(
        std::floating_point auto const value,
        int const n,
        char const point,
        char const separator) noexcept
    -> std::string
    {
        auto const fmt = std::format("{{:.{}f}}", n);
        auto const text = std::vformat(fmt, std::make_format_args(value));
        auto const size = text.size();
        auto const point_idx = std::ranges::find(text, '.') - text.begin();
        auto offset = 0;

        std::vector<char> buffer;
        buffer.reserve(size + point_idx/3);

        if (value < 0) {
            buffer.push_back('-');
            offset = 1;
        }

        auto comma_idx = 3 - (point_idx - offset) % 3;
        if (comma_idx == 3) comma_idx = 0;

        for (auto i = offset; i < point_idx; ++i) {
            if (comma_idx == 3) {
                buffer.push_back(separator);
                comma_idx = 0;
            }
            ++comma_idx;
            buffer.push_back(text[i]);
        }

        // Dodajemy kropkę dziesiętną i cyfry po kropce.
        buffer.push_back(point);
        std::ranges::copy_n(text.begin() + point_idx + 1, n, std::back_inserter(buffer));

        return {buffer.begin(), buffer.end()};
    }

    auto box::to_string(
        std::span<unsigned char const> bytes,
        int const n) noexcept
    -> std::string
    {
        return bytes
            | std::views::transform([] (unsigned char const c) { return static_cast<char>(c); })
            | std::views::take(n)
            | std::ranges::to<std::string>();
    }

    auto box::split(
        std::string const& text,
        char const delimiter) noexcept
    -> std::vector<std::string>
    {
        // Lepiej policzyć delimitery niż później realokować wektor.
        auto const n = std::accumulate(
                text.cbegin(),
                text.cend(),
                0,
                [delimiter](int const count, char const c) {
                    return c == delimiter ? count + 1 : count;
                }
        );

        std::vector<std::string> tokens{};
        tokens.reserve(n + 1);

        std::string token;
        std::istringstream stream(text);
        while (std::getline(stream, token, delimiter))
            if (auto line = trim(token); !line.empty())
                tokens.push_back(std::move(line));

        tokens.shrink_to_fit();
        return tokens;
    }


    auto box::join(
        std::span<std::string> data,
        std::string_view const delimiter) noexcept
    -> std::string
    {
        const auto tokens_size = std::accumulate(
            std::begin(data),
            std::end(data),
            ssize_t{0},
            [](auto const count, auto const& str) {
               return count + str.size();
            });


        std::string buffer{};
        buffer.reserve(tokens_size + data.size() - 1);

        std::ranges::for_each(data.first(data.size() - 1), [&buffer, delimiter](auto const& token) {
            buffer += token + std::string(delimiter)  ;
        });
        buffer += data.back();
        return buffer;
    }


    auto box::bytes_to_string(
        std::span<unsigned char> const data,
        bool const as_hex) noexcept
    -> std::string
    {
        auto const fmt = [as_hex] (unsigned char const ch) noexcept {
            return as_hex ? std::format("0x{:02x}", ch) : std::format("{}", ch);
        };

        auto elements = data
            | std::views::transform([&](auto const ch) { return fmt(ch); })
            | std::ranges::to<std::vector>();

        return join(elements, ", ");
    }

    auto box::random_bytes(int const n) noexcept
    -> std::vector<unsigned char>
    {
        std::random_device rd;
        std::array<int, std::mt19937::state_size> seed_data{};
        std::ranges::generate(seed_data, ref(rd));
        std::seed_seq seq(begin(seed_data), end(seed_data));

        auto mtgen = std::mt19937{seq};
        auto ud = std::uniform_int_distribution<>{0, 255};

        return std::views::iota(0, n)
                | std::views::transform([&](auto _) { return static_cast<unsigned char>(ud(mtgen)); })
                | std::ranges::to<std::vector>();
    }
}
