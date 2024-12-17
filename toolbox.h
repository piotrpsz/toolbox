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
#pragma once

/*------- include files:
-------------------------------------------------------------------*/
#include <string>
#include <vector>
#include <concepts>
#include <algorithm>
#include <ranges>
#include <format>
#include <span>
#include <sstream>

namespace bee {
    class box {
        static constexpr auto DECIMAL_POINT = ',';
        static constexpr auto THOUSAND_SEPARATOR = '.';
        static constexpr auto DIGITS_AFTER_DECIMAL_POINT = 2;
    public:

        /// Konwersja tekstu z liczbą całkowitą na liczbę.
        /// \param sv View tekstu zawierającego liczbę,
        /// \param base System, w którem liczba jest prezentowana w tekście (domyślnie 10)
        /// \return Opcjonalnie wyznaczona liczba
        static auto to_int(
            std::string_view sv,
            int base = 10) noexcept
        -> std::optional<int>;

        /// Konwersja liczby całkowitej na tekst.
        /// Formatownie obejmuje separatory tysięcy..
        /// \param value Wartość do sformatowania,
        /// \param separator Znak oddzielający grupy tysięcy.
        /// \return Tekst ze sformatowaną liczbą całkowitą.
        static auto to_string(
            std::integral auto value,
            char separator = THOUSAND_SEPARATOR) noexcept
        -> std::string;

        /// Konwersja liczby zmienno-przecinkowej na tekst.
        /// Formatownie obejmuje separatory tysięcy w liczbie przed przecinkiem.
        /// Określić można również liczbę cyfr po przecinku.
        /// \param value Wartość do sformatowania,
        /// \param n Liczba cyfr po kropce (zaokrąglenie)
        /// \param point Znak oddzielający część całkowitą od ułamka.
        /// \param separator Znak oddzielający grupy tysięcy.
        /// \return Tekst ze sformatowaną liczbą.
        static auto to_string(
            std::floating_point auto value,
            int n = DIGITS_AFTER_DECIMAL_POINT,
            char point = DECIMAL_POINT,
            char separator = THOUSAND_SEPARATOR) noexcept
        -> std::string;

        /// Komwersja wektora bajtów (unsigned char) na tekst.
        /// \param bytes Span bajtów do konwersji,
        /// \param n Liczba bajtów do konwersji (-1 dla wszystkich).
        static auto to_string(
            std::span<unsigned char const> bytes,
            int n = -1) noexcept
        -> std::string;

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

        /// Usunięcie zamykającyh (końcowych) białych znaków (z prawej strony).
        /// \param s Tekst, z którego należy usunąć białe znaki
        /// \return Tekst bez zamykających białych znaków.
        static std::string trim_right(std::string s) noexcept {
            s.erase(std::find_if(s.rbegin(), s.rend(), is_not_space).base(), s.end());
            return s;
        }

        /// Usunięcie początkowych i końcowych białych znaków (z obu stron).
        /// \param s Tekst, z którego należy usunąć białe znaki
        /// \return Tekst bez początkowych i końcowych białych znaków.
        static std::string trim(std::string s) noexcept {
            return trim_left(trim_right(std::move(s)));
        }

        /// Podział przysłanego stringa na wektor stringów. \n
        /// Wyodrębnianie stringów składowych odbywa się po napotkaniu delimiter'a.
        /// \param text - string do podziału,
        /// \param delimiter - znak sygnalizujący podział,
        /// \return Wektor stringów.
        static auto split(
            std::string const& text,
            char delimiter) noexcept
        -> std::vector<std::string>;

        /// Tworzy string będący złączeniem stringów przysłanych w wektorze. \n
        /// Łączone stringi rozdzielone są przysłanym delimiter'em.
        /// \param data Wektor stringów do połączenia,
        /// \param delimiter Znak wstawiany pomiędzy łączonymi stringami.
        /// \return String jako suma połączonych stringów.
        static auto join(
            std::span<std::string> data,
            std::string_view delimiter = ",") noexcept
        -> std::string;

        static auto bytes_to_string(
            std::span<unsigned char> data,
            bool as_hex = false) noexcept
        -> std::string;

        /// Utworzenie wektora losowych bajtów.
        /// \param n - oczekiwana liczba bajtów.
        /// \return Wektor losowych bajtów.
        static auto random_bytes(int n) noexcept
        -> std::vector<unsigned char>;

        /// \brief Funkcja opakowująca obiekt funkcyjny, dla której mierzymy czas wykonania.\n
        /// usage: testowanie funkcji add()\n
        /// auto r = box::execution_timer([]() { add(2, 3); }, 10 ); \n
        /// std::print ("czas wykonania: {}\n", r ); \n
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
