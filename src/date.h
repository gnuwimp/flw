/**
* @file
* @brief A date/time class.
*
* @author gnuwimp@gmail.com
* @copyright Released under the GNU General Public License v3.0
*/

#ifndef GNU_DATE_H
#define GNU_DATE_H

// MKALGAM_ON

#include <cstdint>
#include <string>

namespace gnu {

/*
 *      _____        _
 *     |  __ \      | |
 *     | |  | | __ _| |_ ___
 *     | |  | |/ _` | __/ _ \
 *     | |__| | (_| | ||  __/
 *     |_____/ \__,_|\__\___|
 *
 *
 */

/** @brief An date and time class.
*
* Date range is from 1-1-1 00:00:00 to 9999-12-31 23:59:59.\n
* Date with year set to 0 is invalid.\n
*/
class Date {
public:
    /** @brief Use UTC or locak time.
    *
    */
    enum class UTC {
                                ON,     ///< @brief UTC is on.
                                OFF,    ///< @brief UTC is off.
    };

    /** @brief Use US or world date when parsing strings.
    *
    * World is DD/MM and US is MM/DD.
    */
    enum class US {
                                ON,     ///< @brief US date is on.
                                OFF,    ///< @brief World date is ON.
    };

    /** @brief Compare two dates option.
    *
    */
    enum class Compare {
                                YYYYMM,                     ///< @brief Compare using year and month.
                                YYYYMMDD,                   ///< @brief Compare using year and month and day.
                                YYYYMMDDHH,                 ///< @brief Compare using year and month and day and hour.
                                YYYYMMDDHHMM,               ///< @brief Compare using year and month and day and hour and minute.
                                YYYYMMDDHHMMSS,             ///< @brief Compare using year and month and day and hour and minute and seconds.
                                LAST = YYYYMMDDHHMMSS,      ///< @brief Same as YYYYMMDDHHMMSS.
    };

    /** @brief Days in a week.
    *
    */
    enum class Day {
                                INVALID,            ///< @brief Invalid day in week.
                                MONDAY,             ///< @brief Monday.
                                TUESDAY,            ///< @brief Tuesday.
                                WENDSDAY,           ///< @brief Wendsday.
                                THURSDAY,           ///< @brief Thursday.
                                FRIDAY,             ///< @brief Friday.
                                SATURDAY,           ///< @brief Saturday.
                                SUNDAY,             ///< @brief Sunday.
                                LAST = SUNDAY,      ///< @brief Same as SUNDAY.
    };

    /** @brief Format a date string.
    *
    */
    enum class Format {
                                ISO,                        ///< @brief "YYYYMMDD".
                                ISO_LONG,                   ///< @brief "YYYY-MM-DD".
                                ISO_TIME,                   ///< @brief "YYYYMMDD HHMMSS".
                                ISO_TIME_LONG,              ///< @brief "YYYY-MM-DD HH:MM:SS".
                                TIME,                       ///< @brief "HHMMSS".
                                TIME_LONG,                  ///< @brief "HH:MM:SS".
                                US,                         ///< @brief "M/D/YYYY".
                                WORLD,                      ///< @brief "D/M/YYYY".
                                DAY_MONTH_YEAR,             ///< @brief "D December YYYY".
                                DAY_MONTH_YEAR_SHORT,       ///< @brief "D Dec YYYY".
                                WEEKDAY_MONTH_YEAR,         ///< @brief "Sunday December YYYY".
                                WEEKDAY_MONTH_YEAR_SHORT,   ///< @brief "Sun Dec YYYY".
                                WEEKDAY,                    ///< @brief "Sunday".
                                WEEKDAY_SHORT,              ///< @brief "Sun".
                                MONTH,                      ///< @brief "December".
                                MONTH_SHORT,                ///< @brief "Dec".
                                LAST = MONTH_SHORT,         ///< @brief Same as MONTH_SHORT.
    };

    static const int            SECS_PER_HOUR = 3600;
    static const int            SECS_PER_DAY  = 3600 * 24;
    static const int            SECS_PER_WEEK = 3600 * 24 * 7;

    explicit                    Date(Date::UTC utc = Date::UTC::OFF);
                                Date(int year, int month, int day, int hour = 0, int min = 0, int sec = 0);
    explicit                    Date(int64_t unix_time, Date::UTC utc = Date::UTC::OFF);
    explicit                    Date(const std::string& date, Date::US us = Date::US::OFF);
    bool                        operator<(const Date& other) const
                                    { return compare(other) < 0 ? true : false; } ///< @brief Compare dates.
    bool                        operator<=(const Date& other) const
                                    { return compare(other) <= 0 ? true : false; } ///< @brief Compare dates.
    bool                        operator>(const Date& other) const
                                    { return compare(other) > 0 ? true : false; } ///< @brief Compare dates.
    bool                        operator>=(const Date& other) const
                                    { return compare(other) >= 0 ? true : false; } ///< @brief Compare dates.
    bool                        operator==(const Date& other) const
                                    { return compare(other) == 0 ? true : false; } ///< @brief Compare dates.
    bool                        operator!=(const Date& other) const
                                    { return compare(other) != 0 ? true : false; } ///< @brief Compare dates.
    bool                        add_days(int days);
    bool                        add_months(int months);
    bool                        add_seconds(int64_t seconds);
    bool                        add_years(int years)
                                    { return add_months(years * 12); } ///< @brief Add years. @param[in] years  Valid number of years. @return True if ok.
    Date&                       clear()
                                    { _year = _month = _day = _hour = _min = _sec = 0; return *this; } ///< @brief Set all fields to 0.
    Date&                       clear_time()
                                    { _hour = _min = _sec = 0; return *this; } ///< @brief Set hour, minutes and seconds to 0.
    int                         compare(const Date& other, Compare flag = Compare::YYYYMMDDHHMMSS) const;
    int                         day() const
                                    { return _day; } ///< @brief Get day in month.
    int                         days_in_month() const;
    int                         days_into_year() const;
    void                        debug() const;
    int                         diff_days(const Date& other) const;
    int                         diff_months(const Date& other) const;
    int64_t                     diff_seconds(const Date& other) const;
    std::string                 format(Format format = Format::ISO) const;
    int                         hour() const
                                    { return _hour; } ///< @brief Get hour.
    bool                        is_invalid() const
                                    { return _year == 0 || _month == 0 || _day == 0; } ///< @brief Test if date is invalid.
    bool                        is_leapyear() const;
    int                         minute() const
                                    { return _min; } ///< @brief Get minute.
    int                         month() const
                                    { return _month; } ///< @brief Get month number
    const char*                 month_name() const;
    const char*                 month_name_short() const;
    int                         second() const
                                    { return _sec; } ///< @brief Get seconds.
    Date&                       set(const Date& other);
    Date&                       set(const std::string& date, Date::US us = Date::US::OFF);
    Date&                       set(int year, int month, int day, int hour = 0, int min = 0, int sec = 0);
    Date&                       set_day(int day);
    Date&                       set_day_to_last_in_month()
                                    { _day = days_in_month(); return *this; } ///< @brief Set day to last day in current month.
    Date&                       set_hour(int hour);
    Date&                       set_minute(int min);
    Date&                       set_month(int month);
    Date&                       set_second(int sec);
    Date&                       set_weekday(Day weekday);
    Date&                       set_year(int year);
    int64_t                     time() const;
    int                         week() const;
    Day                         weekday() const;
    const char*                 weekday_name() const;
    const char*                 weekday_name_short() const;
    int                         year() const
                                    { return _year; } ///< @brief Get current year.

    static inline bool          CompareDates(const Date& a, const Date& b)
                                    { return a.compare(b) < 0; } ///< @brief Compare two dates.

private:
    short                       _year;      ///< @brief Year from 1 - 9999.
    char                        _month;     ///< @brief Month from 1 - 12.
    char                        _day;       ///< @brief Day from 1 - 31.
    char                        _hour;      ///< @brief Hour from 0 - 23.
    char                        _min;       ///< @brief Minute from 0 - 59.
    char                        _sec;       ///< @brief Second from 0 - 59.
};

} // gnu

// MKALGAM_OFF

#endif // GNU_DATE_H
