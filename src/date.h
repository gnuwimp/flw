// Copyright gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#ifndef GNU_DATE_H
#define GNU_DATE_H

// MKALGAM_ON

#include <cstdint>
#include <string>

namespace gnu {

/***
 *      _____        _
 *     |  __ \      | |
 *     | |  | | __ _| |_ ___
 *     | |  | |/ _` | __/ _ \
 *     | |__| | (_| | ||  __/
 *     |_____/ \__,_|\__\___|
 *
 *
 */

//------------------------------------------------------------------------------
// An date and time class.
// Date range is from 1-1-1 00:00:00 to 9999-12-31 23:59:59.
// Date with year set to 0 is invalid.
//
class Date {
public:
    enum class UTC {
                                ON,
                                OFF,
    };

    enum class US {
                                ON,
                                OFF,
    };

    enum class COMPARE {
                                YYYYMM,
                                YYYYMMDD,
                                YYYYMMDDHH,
                                YYYYMMDDHHMM,
                                YYYYMMDDHHMMSS,
                                LAST = YYYYMMDDHHMMSS,
    };

    enum class DAY {
                                INVALID,
                                MONDAY,
                                TUESDAY,
                                WENDSDAY,
                                THURSDAY,
                                FRIDAY,
                                SATURDAY,
                                SUNDAY,
                                LAST = SUNDAY,
    };

    enum class FORMAT {
                                ISO,
                                ISO_LONG,
                                ISO_TIME,
                                ISO_TIME_LONG,
                                TIME,
                                TIME_LONG,
                                US,
                                WORLD,
                                DAY_MONTH_YEAR,
                                DAY_MONTH_YEAR_SHORT,
                                WEEKDAY_MONTH_YEAR,
                                WEEKDAY_MONTH_YEAR_SHORT,
                                LAST = WEEKDAY_MONTH_YEAR_SHORT,
    };

    static const int            SECS_PER_HOUR = 3600;
    static const int            SECS_PER_DAY  = 3600 * 24;
    static const int            SECS_PER_WEEK = 3600 * 24 * 7;

    explicit                    Date(Date::UTC utc = Date::UTC::OFF);
                                Date(int year, int month, int day, int hour = 0, int min = 0, int sec = 0);
    explicit                    Date(int64_t unix_time, Date::UTC utc = Date::UTC::OFF);
    explicit                    Date(const std::string& date, Date::US us = Date::US::OFF);
    bool                        operator<(const Date& other) const
                                    { return compare(other) < 0 ? true : false; }
    bool                        operator<=(const Date& other) const
                                    { return compare(other) <= 0 ? true : false; }
    bool                        operator>(const Date& other) const
                                    { return compare(other) > 0 ? true : false; }
    bool                        operator>=(const Date& other) const
                                    { return compare(other) >= 0 ? true : false; }
    bool                        operator==(const Date& other) const
                                    { return compare(other) == 0 ? true : false; }
    bool                        operator!=(const Date& other) const
                                    { return compare(other) != 0 ? true : false; }
    bool                        add_days(int days);
    bool                        add_months(int months);
    bool                        add_seconds(int64_t seconds);
    bool                        add_years(int years)
                                    { return add_months(years * 12); }
    Date&                       clear()
                                    { _year = _month = _day = _hour = _min = _sec = 0; return *this; }
    Date&                       clear_time()
                                    { _hour = _min = _sec = 0; return *this; }
    int                         compare(const Date& other, Date::COMPARE flag = Date::COMPARE::YYYYMMDDHHMMSS) const;
    int                         day() const
                                    { return _day; }
    int                         days_in_month() const;
    int                         days_into_year() const;
    void                        debug() const;
    int                         diff_days(const Date& other) const;
    int                         diff_months(const Date& other) const;
    int                         diff_seconds(const Date& other) const;
    std::string                 format(Date::FORMAT format = Date::FORMAT::ISO) const;
    int                         hour() const
                                    { return _hour; }
    bool                        is_invalid() const
                                    { return _year == 0 || _month == 0 || _day == 0; }
    bool                        is_leapyear() const;
    int                         minute() const
                                    { return _min; }
    int                         month() const
                                    { return _month; }
    const char*                 month_name() const;
    const char*                 month_name_short() const;
    int                         second() const
                                    { return _sec; }
    Date&                       set(const Date& other);
    Date&                       set(const std::string& date, Date::US us = Date::US::OFF);
    Date&                       set(int year, int month, int day, int hour = 0, int min = 0, int sec = 0);
    Date&                       set_day(int day);
    Date&                       set_day_to_last_in_month()
                                    { _day = days_in_month(); return *this; }
    Date&                       set_hour(int hour);
    Date&                       set_minute(int min);
    Date&                       set_month(int month);
    Date&                       set_second(int sec);
    Date&                       set_weekday(Date::DAY weekday);
    Date&                       set_year(int year);
    int64_t                     time() const;
    int                         week() const;
    Date::DAY                   weekday() const;
    const char*                 weekday_name() const;
    const char*                 weekday_name_short() const;
    int                         year() const
                                    { return _year; }

    static inline bool          Compare(const Date& a, const Date& b)
                                    { return a.compare(b) < 0; }

private:
    short                       _year;
    char                        _month;
    char                        _day;
    char                        _hour;
    char                        _min;
    char                        _sec;
};

} // gnu

// MKALGAM_OFF

#endif // GNU_DATE_H
