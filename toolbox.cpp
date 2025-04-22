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
#include <unistd.h>
#include <format>
#include <pwd.h>
#include <sstream>
#include <filesystem>

namespace bee {

    // Konwersja tekstu z liczbą całkowitą na liczbę.
    // \param sv Widok tekstu zawierającego liczbę,
    // \param base System liczbowy, w którym liczba jest prezentowana w tekście (domyślnie 10)
    // \return Opcjonalnie wyznaczona liczba
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

    // Podział przysłanego tekstu na wektor tekstów. \n
    // Wyodrębnianie tekstów składowych odbywa się po napotkaniu 'delimiter'.
    // \param text Tekst do podziału,
    // \param delimiter Znak sygnalizujący podział,
    // \return Wektor stringów.
    std::vector<std::string> box::split(
        std::string const& text,
        char const delimiter) noexcept
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

    std::vector<std::string> box::split(
        std::string&& text,
        char const delimiter) noexcept
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
        std::istringstream stream{std::move(text)};
        while (std::getline(stream, token, delimiter))
            if (auto line = trim(token); !line.empty())
                tokens.push_back(std::move(line));

        tokens.shrink_to_fit();
        return tokens;
    }

    auto box::join(
        std::span<std::string> data,
        std::string const &delimiter) noexcept
    -> std::string
    {
        if (data.empty())
            return {};

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
        std::ranges::for_each(data, [&buffer, delimiter](auto&& token) {
            if (!token.empty())
                buffer += token + delimiter;
        });

        if (buffer.ends_with(delimiter))
            buffer.resize(buffer.size() - delimiter.size());
        buffer.shrink_to_fit();

        return buffer;
    }

    bool box::create_dirs(std::string_view const path) {
        namespace fs = std::filesystem;

        if (fs::exists(path))
            return true;

        std::error_code ec{};
        if (fs::create_directories(path, ec))
            return true;

        std::cerr << std::format("{}\n", ec.message());;
        return {};
    }

    std::string box::home_dir() {
        if (auto const pw = getpwuid(getuid()))
            return pw->pw_dir;
        return {};
    }
}
