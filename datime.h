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
#include <sstream>
#include <utility>
#include <date/date.h>
#include <date/tz.h>

using ZonedTime = date::zoned_time<std::chrono::seconds>;

struct Date {
    int y{}, m{}, d{};
    bool operator==(Date const& rhs) const noexcept {
        return y == rhs.y && m == rhs.m && d == rhs.d;
    }
    bool operator!=(Date const& rhs) const noexcept {
        return !operator==(rhs);
    }
    bool operator<(Date const& rhs) const noexcept {
        if (y < rhs.y) return true;
        if (m < rhs.m) return true;
        if (d < rhs.d) return true;
        return false;
    }
    bool operator>(Date const& rhs) const noexcept {
        if (y > rhs.y) return true;
        if (m > rhs.m) return true;
        if (d > rhs.d) return true;
        return false;
    }

};
struct Time {
    int h{}, m{}, s{};
};

class Datime final {
    date::time_zone const* zone = date::locate_zone("Europe/Warsaw");
    ZonedTime tp_;
public:
    /// Data-czas teraz (now).
    Datime() {
        auto const now = std::chrono::system_clock::now();
        tp_ = date::zoned_time(zone, std::chrono::floor<std::chrono::seconds>(now));
    }
    /// \brief Data-czas z 'timestamp' (liczba sekund od początku epoki).
    /// \param timestamp Liczba sekund od początku epoki.
    explicit Datime(int const timestamp) {
        auto const tp = std::chrono::system_clock::from_time_t(timestamp);
        tp_ = date::make_zoned(zone, std::chrono::floor<std::chrono::seconds>(tp));
    }

    explicit Datime(ZonedTime const tp) : tp_{tp} {}

    /// Data-czas z tekstu (np. 2023-10-23 11:06:21).
    /// \param str - string z datą i godziną
    explicit Datime(std::string const& str) {
        std::stringstream ss{str};
        date::local_time<std::chrono::seconds> tmp;
        date::from_stream(ss, "%F %X", tmp);
        tp_ = make_zoned(zone, tmp);
    }

    /// Data-czas z komponentów.
    Datime(Date const dt, Time const tm)
    : tp_{from_components(dt, tm)}
    {}

    // Domyślne kopiowanie i przekazywanie
    Datime(Datime const&) = default;
    Datime& operator=(Datime const&) = default;
    Datime(Datime&&) = default;
    Datime& operator=(Datime&&) = default;
    ~Datime() = default;

    /// Sprawdza, czy wskazany obiekt określa ten sam punkt w czasie.
    /// \return TRUE, jeśli punkty w czasie są takie same, FALSE w przeciwnym przypadku.
    bool operator==(Datime const &rhs) const noexcept {
        return timestamp() == rhs.timestamp();
    }

    /// Sprawdza, czy wskazany obiekt określa inny punkt w czasie
    /// \return TRUE, jeśli punkty w czasie są różne, FALSE w przeciwnym przypadku.
    bool operator!=(Datime const &rhs) const noexcept {
        return !operator==(rhs);
    }

    /// Sprawdza, czy wskazana data-czas (rhs) jest późniejsza.
    /// \return TRUE, jeśli rhs jest późniejszy, FALSE w przeciwnym przypadku.
    bool operator<(Datime const& rhs) const noexcept {
        return timestamp() < rhs.timestamp();
    }

    /// Sprawdza, czy wskazana data-czas (rhs) jest wcześniejsza.
    /// \return TRUE, jeśli rhs jest wcześniejszy, FALSE w przeciwnym przypadku.
    bool operator>(Datime const& rhs) const noexcept {
        return timestamp() > rhs.timestamp();
    }

    /// Zwraca różnicę jako liczbę minut.
    [[nodiscard]]
    int minutes_from(Datime const& rhs) const noexcept {
        auto const a = date::floor<std::chrono::minutes>(tp_.get_sys_time());
        auto const b = date::floor<std::chrono::minutes>(rhs.tp_.get_sys_time());
        const auto diff = (b - a).count();
        return static_cast<int>(diff);
    }

    /// Obliczenie 'timestamp' (liczba sekund od początku epoki).
    /// \return timestamp
    [[nodiscard]]
    int timestamp() const noexcept {
        return static_cast<int>(tp_.get_sys_time().time_since_epoch().count());
    }

    /// Zmiana czasu na podany, data pozostaje bez zmien.
    Datime& set_time(Time const tm) noexcept {
        namespace chrono = std::chrono;
        const auto t = date::floor<chrono::days>(tp_.get_local_time())
                + chrono::hours(tm.h)
                + chrono::minutes(tm.m)
                + chrono::seconds(tm.s);
        tp_ = make_zoned(zone, t);
        return *this;
    }

    /// Wyzerowanie sekund z ewentualnym zaokrągleniem minut.
    Datime& clear_seconds() noexcept {
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
    Datime& clear_time() noexcept {
        namespace chrono = std::chrono;
        auto t = date::floor<chrono::days>(tp_.get_local_time())
                + chrono::hours(0)
                + chrono::minutes(0)
                + chrono::seconds(0);
        tp_ = make_zoned(zone, t);
        return *this;
    }

    /// Początek dnia dla daty.
    Datime& beginning_day() noexcept {
        return clear_time();
    }

    /// Koniec dnia dla daty.
    Datime& end_day() noexcept {
        return set_time({23,59,59});
    }
    /// Wyznaczenie składników daty (bez czasu).
    [[nodiscard]] Date date_components() const noexcept {
        namespace chrono = std::chrono;
        auto days = date::floor<chrono::days>(tp_.get_local_time());
        date::year_month_day ymd{days};
        auto const year = static_cast<int>(ymd.year());
        auto const month = static_cast<int>(static_cast<unsigned>(ymd.month()));
        auto const day = static_cast<int>(static_cast<unsigned>(ymd.day()));
        return {year, month, day};
    }

    /// Sprawdzenie, czy to ten sam dzień.
    [[nodiscard]] bool is_same_day(Datime const& rhs) const noexcept {
        return date_components() == rhs.date_components();
    }

    /// Wyznaczenie składników czasu (bez daty).
    [[nodiscard]] Time time_components() const noexcept {
        namespace chrono = std::chrono;
        auto const days = date::floor<chrono::days>(tp_.get_local_time());
        date::hh_mm_ss const hms{tp_.get_local_time() - days};
        auto hour = static_cast<int>(hms.hours().count());
        auto min = static_cast<int>(hms.minutes().count());
        auto sec = static_cast<int>(hms.seconds().count());
        return {hour, min, sec};
    }
    /// Wyznaczenie składników daty i czasu.
    [[nodiscard]] std::tuple<Date, Time> components() const noexcept {
        namespace chrono = std::chrono;
        auto const days = date::floor<chrono::days>(tp_.get_local_time());

        date::year_month_day const ymd{days};
        auto const year = static_cast<int>(ymd.year());
        auto const month = static_cast<int>(static_cast<unsigned>(ymd.month()));
        auto const day = static_cast<int>(static_cast<unsigned>(ymd.day()));
        Date const dt{year, month, day};

        date::hh_mm_ss const hms{tp_.get_local_time() - days};
        auto hour = static_cast<int>(hms.hours().count());
        auto min = static_cast<int>(hms.minutes().count());
        auto sec = static_cast<int>(hms.seconds().count());
        Time const tm{hour, min, sec};

        return {dt, tm};
    }

    /// Utworzenie nowej data-czasu przez dodanie wskazanej liczby dni.
    /// \param n - liczba dnie do dodania (może być ujemna)
    /// \return Nowa data-czas
    [[nodiscard]] Datime add_days(int const n) const noexcept {
        namespace chrono = std::chrono;
        auto const days = date::floor<chrono::days>(tp_.get_local_time());
        date::hh_mm_ss const hms{tp_.get_local_time() - days};
        auto const added = days + chrono::days(n);
        auto const secs = chrono::floor<chrono::seconds>(added)
                + hms.hours()
                + hms.minutes()
                + hms.seconds();
        return Datime(make_zoned(zone, secs));
    }

    [[nodiscard]]
    Datime next_day() const noexcept {
        return add_days(1);
    }

    [[nodiscard]]
    Datime  prev_day() const noexcept {
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
    std::pair<Datime,Datime> week_range() const noexcept {
        auto const today_idx = static_cast<int>(week_day());
        return {add_days(-today_idx + 1), add_days(7 - today_idx)};
    }

    /// Lokalna data-czas w postaci tekstu.
    /// \return Tekst z lokalną datą-czasem.
    [[nodiscard]]
    std::string  str() const noexcept {
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
    ZonedTime from_components(Date const dt, Time const tm) const noexcept {
        namespace chrono = std::chrono;
        date::year_month_day const ymd = date::year(dt.y) / dt.m / dt.d;
        auto const days = static_cast<date::local_days>(ymd);
        auto t = days + chrono::hours(tm.h) + chrono::minutes(tm.m) + chrono::seconds(tm.s);
        return make_zoned(zone, t);
    }
};
