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
#pragma once


/*------- include files:
-------------------------------------------------------------------*/
#include "types.h"
#include "lzav.h"
#include <pwd.h>        // getpwuid
#include <unistd.h>     // getuid
#include <string>
#include <vector>
#include <concepts>
#include <algorithm>
#include <ranges>
#include <format>
#include <span>
#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

namespace bee {

    class box {
        static constexpr auto DECIMAL_POINT = ',';
        static constexpr auto THOUSAND_SEPARATOR = '.';
        static constexpr auto DIGITS_AFTER_DECIMAL_POINT = 2;
    public:

        /// Zamiana wszystkich znaków tekstu na małe litery.
        static std::string to_lower(std::string_view const sv) {
            std::string buffer;
            buffer.reserve(sv.size());
            for (auto const ch : sv) {
                auto const c = std::tolower(ch);
                buffer.push_back(static_cast<char>(c));
            }
            return buffer;

            // problem with 'to<>'
            // return sv
            //     | std::views::transform([](auto ch) { return tolower(ch); })
            //     | std::ranges::to<std::string>();
        };

        /// Konwersja tekstu z liczbą całkowitą na liczbę.
        /// \param sv View tekstu zawierającego liczbę,
        /// \param base System, w którym liczba jest prezentowana w tekście (domyślnie 10)
        /// \return Opcjonalnie wyznaczona liczba
        static auto to_int(
            std::string_view sv,
            int base = 10) noexcept
        -> std::optional<int>;

        /// Konwersja liczby całkowitej na tekst.
        /// Formatowanie obejmuje separatory tysięcy.
        /// \param value Wartość do sformatowania,
        /// \param separator Znak oddzielający grupy tysięcy.
        /// \return Tekst ze sformatowaną liczbą całkowitą.
        static auto to_string(
            std::integral auto const value,
            char const separator = THOUSAND_SEPARATOR) noexcept
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

        /// Konwersja liczby zmiennoprzecinkowej na tekst.
        /// Formatowanie obejmuje separatory tysięcy w liczbie przed przecinkiem.
        /// Określić można również liczbę cyfr po przecinku.
        /// \param value Wartość do sformatowania,
        /// \param n Liczba cyfr po kropce (zaokrąglenie)
        /// \param point Znak oddzielający część całkowitą od ułamka.
        /// \param separator Znak oddzielający grupy tysięcy.
        /// \return Tekst ze sformatowaną liczbą.
        static auto to_string(
            std::floating_point auto const value,
            int const n = DIGITS_AFTER_DECIMAL_POINT,
            char const point = DECIMAL_POINT,
            char const separator = THOUSAND_SEPARATOR) noexcept
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

            auto separator_idx = 3 - (point_idx - offset) % 3;
            if (separator_idx == 3) separator_idx = 0;

            for (auto i = offset; i < point_idx; ++i) {
                if (separator_idx == 3) {
                    buffer.push_back(separator);
                    separator_idx = 0;
                }
                ++separator_idx;
                buffer.push_back(text[i]);
            }

            // Dodajemy przecinek i cyfry po przecinku.
            buffer.push_back(point);
            std::ranges::copy_n(text.begin() + point_idx + 1, n, std::back_inserter(buffer));

            return {buffer.begin(), buffer.end()};
        }

        /// Konwersja wektora bajtów (unsigned char) na tekst.
        /// \param bytes Span bajtów do konwersji,
        /// \param n Liczba bajtów do konwersji (-1 dla wszystkich).
        static auto to_string(
            BytesView auto const  bytes,
            int n = -1) noexcept
        -> std::string {
            std::string buffer;
            buffer.reserve(bytes.size());
            if (bytes.size() > n)
                bytes = bytes.subspan(0, n);

            for (auto const v : bytes)
                buffer.push_back(static_cast<char>(v));
            return buffer;
        }

        /// Sprawdzenie, czy przysłany znak NIE jest białym znakiem.
        /// \param c Znak do sprawdzenia
        /// \return TRUE, jeśli NIE jest białym znakiem, FALSE w przeciwnym przypadku (jest białym znakiem).
        static bool is_not_space(const char c) noexcept {
            return !std::isspace(c);
        }

        /// Obcięcie początkowych białych znaków.
        /// \param s Tekst, z którego należy usunąć białe znaki
        /// \return Tekst bez początkowych białych znaków.
        static std::string trim_left(std::string s) noexcept {
            s.erase(s.begin(), std::ranges::find_if(s, is_not_space));
            return s;
        }

        /// Usunięcie zamykających (końcowych) białych znaków (z prawej strony).
        /// \param s Tekst, z którego należy usunąć białe znaki
        /// \return Tekst bez zamykających białych znaków.
        static std::string trim_right(std::string s) noexcept {
            s.erase(std::find_if(s.rbegin(), s.rend(), is_not_space).base(), s.end());
            return s;
        }

        /// Usunięcie początkowych i końcowych białych znaków.
        /// \param s Tekst, z którego należy usunąć białe znaki
        /// \return Tekst bez początkowych i końcowych białych znaków.
        static std::string trim(std::string s) noexcept {
            return trim_left(trim_right(std::move(s)));
        }

        /// Podział przysłanego tekstu na wektor tekstów. \n
        /// Wyodrębnianie tekstów składowych odbywa się po napotkaniu delimiter'a.
        /// \param text Tekst do podziału,
        /// \param delimiter Znak sygnalizujący podział,
        /// \return Wektor stringów.
        static auto split(
            std::string const& text,
            char delimiter) noexcept
        -> std::vector<std::string>;

        /// Tworzy tekst będący złączeniem tekstów przysłanych w wektorze. \n
        /// Łączone teksty rozdzielone są przysłanym delimiter'em.
        /// \param data Wektor tekstów do połączenia,
        /// \param delimiter Tekst wstawiany pomiędzy łączonymi tekstami.
        /// \return String jako suma połączonych stringów.
        static auto join(
            std::span<std::string> data,
            std::string_view delimiter = ",") noexcept
        -> std::string;

        /// Zamiana wektora bajtów na tekst, w którym liczby rozdzielone są przecinkami.
        /// \param data Wektor/span bajtów do konwersji.
        /// \param as_hex Flaga określająca czy liczby mają być w reprezentacji szesnastkowej.
        /// \return Tekst z liczbami.
        static auto bytes_to_string(
            BytesView auto const data,
            bool as_hex = false) noexcept
        -> std::string
        {
            auto const fmt = [as_hex] (unsigned char const ch) noexcept {
                return as_hex ? std::format("0x{:02x}", ch) : std::format("{}", ch);
            };

            std::vector<std::string> elements{};
            elements.reserve(data.size());
            for (auto const ch : data)
                elements.push_back(fmt(ch));

            return join(elements, ", ");
        }


        static auto bytes_to_string(
            BytesView auto const data,
            size_t n,
            bool as_hex = false) noexcept
        -> std::string {
            auto const fmt = [as_hex] (unsigned char const ch) noexcept {
                return as_hex ? std::format("0x{:02x}", ch) : std::format("{}", ch);
            };

            std::vector<std::string> elements{};
            elements.reserve(n);
            for (auto const ch : data.first(n))
                elements.push_back(fmt(ch));

            return join(elements, ", ");
        }

        /// Utworzenie wektora losowych bajtów.
        /// \param n - oczekiwana liczba bajtów.
        /// \return Wektor losowych bajtów.
        static auto random_bytes(size_t n) noexcept
        -> std::vector<unsigned char>;

        /// Wyznaczenie katalogu domowego aktualnego użytkownika.
        static std::string home_dir();

        /// Utworzenie wszystkich katalogów pośrednich, włącznie z ostatnim.
        static bool create_dirs(std::string_view path);

        /// Kompresja ciągłego ciągu bajtów (kontenera).
        // https://github.com/avaneev/lzav
        static inline std::vector<char> compress(BytesView auto const data) {
            auto const max_size =  lzav_compress_bound(static_cast<int>(data.size()));
            std::vector<char> buffer(max_size);
            auto const comp_size = lzav_compress_default(data.data(), buffer.data(), data.size(),max_size);
            if (comp_size < max_size)
                buffer.resize(comp_size);

            // Na początku umieszczamy 4 bajty, będące rozmiarem oryginalnego tekstu.
            // Rozmiar ten jest potrzebny przy dekompresji.
            auto const size = static_cast<u32>(data.size());
            std::vector<char> compressed(sizeof(size) + buffer.size());
            // dodaj oryginalny rozmiar
            std::memcpy(compressed.data(), &size, sizeof(size));
            // dodaj skompresowane dane
            std::memcpy(compressed.data() + sizeof(size), buffer.data(), buffer.size());

            return std::move(compressed);
        }

        /// Dekompresja ciągłego ciągu bajtów (kontenera)
        static std::vector<char> decompress(BytesView auto const data) {
            // Pierwsze 4 bajty zawierają rozmiar oryginalnego tekstu.
            u32 src_size;
            std::memcpy(&src_size, data.data(), sizeof(u32));

            std::vector<char> buffer(src_size);
            auto const size = static_cast<int>(data.size() - sizeof(u32));
            auto const decomp_size = lzav_decompress(data.data() + sizeof(u32), buffer.data(), size, src_size);
            if (decomp_size < 0)
                return {};

            return std::move(buffer);
        }

        template<typename... Args>
        static void print(std::format_string<Args...> fmt, Args... args) {
            std::cout << std::format(fmt, std::forward<Args>(args)...) << std::flush;
        }

        template<typename... Args>
        static void println(std::format_string<Args...> fmt, Args... args) {
            std::cout << std::format(fmt, std::forward<Args>(args)...) << '\n' << std::flush;
        }


        template<typename... Args>
        static void print_error(std::format_string<Args...> fmt, Args... args) {
            std::cerr << std::format(fmt, std::forward<Args>(args)...);
        }

        template<typename... Args>
        static void println_error(std::format_string<Args...> fmt, Args... args) {
            std::cerr << std::format(fmt, std::forward<Args>(args)...) << "\n";;
        }

        template<typename... Args>
        static void println_ptr(auto* ptr, std::format_string<Args...> fmt, Args... args) {
            std::cout << std::format(fmt, std::forward<Args>(args)...) << " [" << ptr << "]\n" << std::flush;
        }
        // static void println_ptr(std::string_view const str, auto* ptr) {
        //     std::cout << str << ", addr: " << ptr << "\n";
        // }

        /// \brief Funkcja opakowująca obiekt funkcyjny, dla której mierzymy czas wykonania.\n
        /// Usage: testowanie funkcji add()\n
        /// \code
        /// auto r = box::execution_timer([]()
        ///     { add(2, 3); },
        ///     1'000); \n
        /// std::print ("czas wykonania: {}\n", r ); \n
        /// \endcode
        /// \param fn Obiekt funkcyjny, dla którego mierzymy czas wykonania,
        /// \param args Parametry, które należy przekazać do obiektu funkcyjnego,
        /// \param n Liczba iteracji (liczba wywołań obiektu funkcyjnego).
        /// \return Średni czas jednego wywołania obiektu funkcyjnego
        template<typename Fn, typename... Args>
        static std::string
        execution_timer(Fn fn, Args&&... args, uint const n = 1'000'000) {
            const auto start = std::chrono::steady_clock::now();
            for (uint i = 0; i < n; i++)
                fn(std::forward<Args>(args)...);
            const auto end = std::chrono::steady_clock::now();
            std::chrono::duration<double> const elapsed = end - start;
            return std::format("{:.10f}s", elapsed.count() / n);
        }
    };
}
