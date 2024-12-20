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
#include <string>
#include <sstream>
#include <utility>
#include <date/date.h>
#include <date/tz.h>

namespace bee {
    using ZonedTime = date::zoned_time<std::chrono::seconds>;


    struct date_t {
        int y{}, m{}, d{};
        bool operator==(date_t const& rhs) const noexcept {
            return y == rhs.y && m == rhs.m && d == rhs.d;
        }
        bool operator!=(date_t const& rhs) const noexcept {
            return !operator==(rhs);
        }
        bool operator<(date_t const& rhs) const noexcept {
            if (y < rhs.y) return true;
            if (m < rhs.m) return true;
            if (d < rhs.d) return true;
            return false;
        }
        bool operator>(date_t const& rhs) const noexcept {
            if (y > rhs.y) return true;
            if (m > rhs.m) return true;
            if (d > rhs.d) return true;
            return false;
        }
        [[nodiscard]] std::string to_string() const noexcept {
            return std::format("[.y={}, .m={}, .d={}]", y, m, d);
        }
    };
    struct time_t {
        int h{}, m{}, s{};
        [[nodiscard]] std::string to_string() const noexcept {
            return std::format("[.h={}, .m={}, .s={}]", h, m, s);
        }
    };

    class datime final {
        date::time_zone const* zone = date::locate_zone("Europe/Warsaw");
        ZonedTime tp_;
    public:
        /// Data-czas teraz (now).
        datime() {
            auto const now = std::chrono::system_clock::now();
            tp_ = date::zoned_time(zone, std::chrono::floor<std::chrono::seconds>(now));
        }
        /// \brief Data-czas z 'timestamp' (liczba sekund od początku epoki).
        /// \param timestamp Liczba sekund od początku epoki.
        explicit datime(int const timestamp) {
            auto const tp = std::chrono::system_clock::from_time_t(timestamp);
            tp_ = date::make_zoned(zone, std::chrono::floor<std::chrono::seconds>(tp));
        }

        explicit datime(ZonedTime const tp) : tp_{tp} {}

        /// Data-czas z tekstu (np. 2023-10-23 11:06:21).
        /// \param str - string z datą i godziną
        explicit datime(std::string const& str) {
            std::stringstream ss{str};
            date::local_time<std::chrono::seconds> tmp;
            date::from_stream(ss, "%F %X", tmp);
            tp_ = make_zoned(zone, tmp);
        }

        /// Data-czas z komponentów.
        datime(date_t const dt, time_t const tm)
        : tp_{from_components(dt, tm)}
        {}

        // Domyślne kopiowanie i przekazywanie
        datime(datime const&) = default;
        datime& operator=(datime const&) = default;
        datime(datime&&) = default;
        datime& operator=(datime&&) = default;
        ~datime() = default;

        /// Sprawdza, czy wskazany obiekt określa ten sam punkt w czasie.
        /// \return TRUE, jeśli punkty w czasie są takie same, FALSE w przeciwnym przypadku.
        bool operator==(datime const &rhs) const noexcept {
            return timestamp() == rhs.timestamp();
        }

        /// Sprawdza, czy wskazany obiekt określa inny punkt w czasie
        /// \return TRUE, jeśli punkty w czasie są różne, FALSE w przeciwnym przypadku.
        bool operator!=(datime const &rhs) const noexcept {
            return !operator==(rhs);
        }

        /// Sprawdza, czy wskazana data-czas (rhs) jest późniejsza.
        /// \return TRUE, jeśli rhs jest późniejszy, FALSE w przeciwnym przypadku.
        bool operator<(datime const& rhs) const noexcept {
            return timestamp() < rhs.timestamp();
        }

        /// Sprawdza, czy wskazana data-czas (rhs) jest wcześniejsza.
        /// \return TRUE, jeśli rhs jest wcześniejszy, FALSE w przeciwnym przypadku.
        bool operator>(datime const& rhs) const noexcept {
            return timestamp() > rhs.timestamp();
        }

        /// Zwraca różnicę jako liczbę minut.
        /// \return Liczba (uint) minut pomiedzy punktami-w-czasie.\n
        /// Wartość zawsze dodatnia bez względu na czasową kolejność punktów-w-czasie.
        [[nodiscard]]
        uint minutes_from(datime const& rhs) const noexcept {
            auto const a = date::floor<std::chrono::minutes>(tp_.get_sys_time());
            auto const b = date::floor<std::chrono::minutes>(rhs.tp_.get_sys_time());
            auto const diff = (a < b) ? (b - a).count() : (a - b).count();
            return static_cast<uint>(diff);
        }

        /// Zwraca różnicę pomiędzy datami jako liczbę sekund.
        /// \return Liczba (uint) sekund pomiedzy punktami-w-czasie.\n
        /// Wartość zawsze dodatnia bez względu na czasową kolejność punktów-w-czasie.
        [[nodiscard]]
        uint seconds_from(datime const& rhs) const noexcept {
            auto const a = date::floor<std::chrono::seconds>(tp_.get_sys_time());
            auto const b = date::floor<std::chrono::seconds>(rhs.tp_.get_sys_time());
            auto const diff = (a < b) ? (b - a).count() : (a - b).count();
            return static_cast<uint>(diff);
        }

        /// Obliczenie 'timestamp' (liczba sekund od początku epoki).
        /// \return timestamp
        [[nodiscard]]
        int timestamp() const noexcept {
            return static_cast<int>(tp_.get_sys_time().time_since_epoch().count());
        }

        /// Zmiana czasu na podany, data pozostaje bez zmien.
        datime& set_time(time_t const tm) noexcept {
            namespace chrono = std::chrono;
            const auto t = date::floor<chrono::days>(tp_.get_local_time())
                    + chrono::hours(tm.h)
                    + chrono::minutes(tm.m)
                    + chrono::seconds(tm.s);
            tp_ = make_zoned(zone, t);
            return *this;
        }

        /// Wyzerowanie sekund z ewentualnym zaokrągleniem minut.
        datime& clear_seconds() noexcept {
            namespace chrono = std::chrono;
            auto const days = date::floor<chrono::days>(tp_.get_local_time());
            date::hh_mm_ss hms{tp_.get_local_time() - days};
            const auto t = date::floor<chrono::days>(tp_.get_local_time())
                    + hms.hours()
                    + hms.minutes()
                    + chrono::seconds(hms.seconds().count() >= 30 ? 60 : 0);
            tp_ = make_zoned(zone, t);
            return *this;
        }

        /// Wyzerowanie czasu.
        datime& clear_time() noexcept {
            namespace chrono = std::chrono;
            auto t = date::floor<chrono::days>(tp_.get_local_time())
                    + chrono::hours(0)
                    + chrono::minutes(0)
                    + chrono::seconds(0);
            tp_ = make_zoned(zone, t);
            return *this;
        }

        /// Nowa data odpowiadająca początkowi dnia.
        /// Aktualny obiekt pozostaje niezmieniony.
        [[nodiscard]] datime beginning_day() const noexcept {
            auto dt = datime{tp_};
            dt.clear_time();
            return dt;
        }

        /// Nowa data odpowiadająca końcowi dnia.
        /// Aktualny obiekt pozostaje niezmieniony,
        [[nodiscard]] datime end_day() const noexcept {
            auto dt = datime{tp_};
            dt.set_time({23,59,59});
            return dt;
        }

        /// Wyznaczenie składników daty (bez czasu).
        [[nodiscard]] date_t date_components() const noexcept {
            namespace chrono = std::chrono;
            auto const days = date::floor<chrono::days>(tp_.get_local_time());
            date::year_month_day const ymd{days};
            auto const year = static_cast<int>(ymd.year());
            auto const month = static_cast<int>(static_cast<unsigned>(ymd.month()));
            auto const day = static_cast<int>(static_cast<unsigned>(ymd.day()));
            return {year, month, day};
        }

        /// Sprawdzenie, czy to ten sam dzień.
        [[nodiscard]] bool is_same_day(datime const& rhs) const noexcept {
            return date_components() == rhs.date_components();
        }

        /// Wyznaczenie składników czasu (bez daty).
        [[nodiscard]] time_t time_components() const noexcept {
            namespace chrono = std::chrono;
            auto const days = date::floor<chrono::days>(tp_.get_local_time());
            date::hh_mm_ss const hms{tp_.get_local_time() - days};
            auto hour = static_cast<int>(hms.hours().count());
            auto min = static_cast<int>(hms.minutes().count());
            auto sec = static_cast<int>(hms.seconds().count());
            return {hour, min, sec};
        }
        /// Wyznaczenie składników daty i czasu.
        [[nodiscard]] std::pair<date_t,time_t> components() const noexcept {
            namespace chrono = std::chrono;
            auto const days = date::floor<chrono::days>(tp_.get_local_time());

            date::year_month_day const ymd{days};
            auto const year = static_cast<int>(ymd.year());
            auto const month = static_cast<int>(static_cast<unsigned>(ymd.month()));
            auto const day = static_cast<int>(static_cast<unsigned>(ymd.day()));

            date::hh_mm_ss const hms{tp_.get_local_time() - days};
            auto const hour = static_cast<int>(hms.hours().count());
            auto const min = static_cast<int>(hms.minutes().count());
            auto const sec = static_cast<int>(hms.seconds().count());

            return {{year, month, day}, {hour, min, sec}};
        }

        /// Nowa data-czas po dodaniu wskazanej liczby lat.
        [[nodiscard]] datime add_year(int const years_number) const {
            auto [dt, tm] = components();
            dt.y += years_number;
            return datime(from_components(dt, tm));
        }

        /// Nowa data-czas po dodaniu wskazanej liczby miesięcy.
        [[nodiscard]] datime add_month(int const months_number) const {
            auto [dt, tm] = components();
            dt.m += months_number;
            return datime(from_components(dt, tm));
        }

        /// Nowa data-czas po dodaniu wskazanej liczby dni.
        [[nodiscard]] datime add_days(int const days_number) const noexcept {
            auto [dt, tm] = components();
            dt.d += days_number;
            return datime(from_components(dt, tm));
        }

        /// Utworzenie nowego obiektu 'datime' przez dodanie wskazanej liczby godzin.
        [[nodiscard]] datime add_hours(int const hrs) const noexcept {
            namespace chrono = std::chrono;
            const auto days = chrono::floor<chrono::days>(tp_.get_local_time());
            date::hh_mm_ss const hms{tp_.get_local_time() - days};
            auto const secs = chrono::floor<chrono::seconds>(days)
                    + (hms.hours() + chrono::hours(hrs))
                    + hms.minutes()
                    + hms.seconds();
            return datime(make_zoned(zone, secs));
        }

        /// Utworzenie nowego obiektu 'datime' przez dodanie wskazanej liczby minut.
        [[nodiscard]] datime add_minutes(int const min) const noexcept {
            namespace chrono = std::chrono;
            const auto days = chrono::floor<chrono::days>(tp_.get_local_time());
            date::hh_mm_ss const hms{tp_.get_local_time() - days};
            auto const secs = chrono::floor<chrono::seconds>(days)
                    + hms.hours()
                    + (hms.minutes() + chrono::minutes(min))
                    + hms.seconds();
            return datime(make_zoned(zone, secs));
        }

        /// Utworzenie nowego obiektu 'datime' przez dodanie wskazanej liczby sekond.
        [[nodiscard]] datime add_seconds(int const sec) const noexcept {
            namespace chrono = std::chrono;
            const auto days = chrono::floor<chrono::days>(tp_.get_local_time());
            date::hh_mm_ss const hms{tp_.get_local_time() - days};
            auto const secs = chrono::floor<chrono::seconds>(days)
                    + hms.hours()
                    + hms.minutes()
                    + (hms.seconds() + chrono::seconds(sec));
            return datime(make_zoned(zone, secs));
        }

        /// Nowa data oznaczjąca dzień następny.
        [[nodiscard]]
        datime next_day() const noexcept {
            return add_days(1);
        }

        /// Nowa data oznaczająca dzień poprzedni.
        [[nodiscard]]
        datime prev_day() const noexcept {
            return add_days(-1);
        }

        /// Wyznaczenie numeru dnia w ramach tygodnia.
        /// Poniedziałek: 1 ... Niedziela: 7
        [[nodiscard]]
        uint week_day() const noexcept {
            namespace chrono = std::chrono;
            auto const wd = date::weekday(chrono::floor<chrono::days>(tp_.get_local_time()));
            return wd.iso_encoding();
        }

        /// Wyznaczenie pary obiektów data-czas określających
        /// początek i koniec tygodnia.
        [[nodiscard]]
        std::pair<datime,datime> week_range() const noexcept {
            auto const today_idx = static_cast<int>(week_day());
            return {add_days(-today_idx + 1), add_days(7 - today_idx)};
        }

        /// Lokalna data-czas w postaci tekstu.
        /// \return Tekst z lokalną datą-czasem.
        [[nodiscard]]
        std::string  to_string() const noexcept {
            std::stringstream ss{};
            ss << tp_.get_local_time();
            return ss.str();
        }

    private:
        /// Wyznaczenie lokalnego punktu-w-czasie ze składników.
        /// \param  dt Data
        /// \param tm Czas
        /// \return Lokalny punkt-w-czasie.
        [[nodiscard]]
        ZonedTime from_components(date_t const dt, time_t const tm) const noexcept {
            namespace chrono = std::chrono;
            date::year_month_day const ymd = date::year(dt.y) / dt.m / dt.d;
            auto const days = static_cast<date::local_days>(ymd);
            const auto t = days
                + chrono::hours(tm.h)
                + chrono::minutes(tm.m)
                + chrono::seconds(tm.s);
            return make_zoned(zone, t);
        }
    };
}
