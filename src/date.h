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
                                NAME,
                                NAME_LONG,
                                NAME_TIME,
                                NAME_TIME_LONG,
                                YEAR_MONTH,
                                YEAR_MONTH_LONG,
                                LAST = YEAR_MONTH_LONG,
    };

    static const int            SECS_PER_HOUR  = 3600;
    static const int            SECS_PER_DAY   = 3600 * 24;
    static const int            SECS_PER_WEEK  = 3600 * 24 * 7;

    explicit                    Date(bool utc = false);
                                Date(const Date& other);
                                Date(Date&&);
                                Date(int year, int month, int day, int hour = 0, int min = 0, int sec = 0);
    Date&                       operator=(const Date& other);
    Date&                       operator=(Date&&);
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
    int                         compare(const Date& other, Date::COMPARE flag = Date::COMPARE::YYYYMMDDHHMMSS) const;
    int                         day() const
                                    { return _day; }
    Date&                       day(int day);
    Date&                       day_last();
    int                         diff_days(const Date& other) const;
    int                         diff_months(const Date& other) const;
    int                         diff_seconds(const Date& other) const;
    std::string                 format(Date::FORMAT format = Date::FORMAT::ISO) const;
    int                         hour() const
                                    { return _hour; }
    Date&                       hour(int hour);
    bool                        is_invalid() const
                                    { return _year == 0 || _month == 0 || _day == 0; }
    bool                        is_leapyear() const;
    int                         minute() const
                                    { return _min; }
    Date&                       minute(int min);
    int                         month() const
                                    { return _month; }
    Date&                       month(int month);
    int                         month_days() const;
    const char*                 month_name() const;
    const char*                 month_name_short() const;
    void                        print() const;
    int                         second() const
                                    { return _sec; }
    Date&                       second(int sec);
    Date&                       set(const Date& other);
    Date&                       set(int year, int month, int day, int hour = 0, int min = 0, int sec = 0);
    int64_t                     time() const;
    int                         week() const;
    Date::DAY                   weekday() const;
    Date&                       weekday(Date::DAY weekday);
    const char*                 weekday_name() const;
    const char*                 weekday_name_short() const;
    int                         year() const
                                    { return _year; }
    Date&                       year(int year);
    int                         yearday() const;

    static bool                 Compare(const Date& a, const Date& b);
    static std::string          FormatSecToISO(int64_t seconds, bool utc = false);
    static Date                 FromString(const char* string, bool us = false);
    static Date                 FromTime(int64_t seconds, bool utc = false);
    static Date                 InvalidDate();

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
