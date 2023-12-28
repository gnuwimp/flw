// Copyright gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#ifndef FLW_DATE_H
#define FLW_DATE_H

#include <string>

// MKALGAM_ON

namespace flw {
    //--------------------------------------------------------------------------
    // An date and time class
    // Date range is from 1-1-1 00:00:00 to 9999-12-31 23:59:59
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
                                        US,
                                        WORLD,
                                        NAME,
                                        NAME_LONG,
                                        YEAR_MONTH,
                                        YEAR_MONTH_LONG,
                                        LAST = YEAR_MONTH_LONG,
        };

        enum class RANGE {
                                        DAY,
                                        WEEKDAY,
                                        FRIDAY,
                                        SUNDAY,
                                        MONTH,
                                        HOUR,
                                        MIN,
                                        SEC,
                                        LAST = SEC,
        };

        static const int                SECS_PER_HOUR;
        static const int                SECS_PER_DAY;
        static const int                SECS_PER_WEEK;

                                        Date(bool utc = false);
                                        Date(const Date& other);
                                        Date(Date&&);
                                        Date(int year, int month, int day, int hour = 0, int min = 0, int sec = 0);
        Date&                           operator=(const Date& other);
        Date&                           operator=(Date&&);
        bool                            operator<(const Date& other) const
                                            { return compare(other) < 0 ? true : false; }
        bool                            operator<=(const Date& other) const
                                            { return compare(other) <= 0 ? true : false; }
        bool                            operator>(const Date& other) const
                                            { return compare(other) > 0 ? true : false; }
        bool                            operator>=(const Date& other) const
                                            { return compare(other) >= 0 ? true : false; }
        bool                            operator==(const Date& other) const
                                            { return compare(other) == 0 ? true : false; }
        bool                            operator!=(const Date& other) const
                                            { return compare(other) != 0 ? true : false; }
        bool                            add_days(int days);
        bool                            add_months(int months);
        bool                            add_seconds(int64_t seconds);
        int                             compare(const Date& other, Date::COMPARE flag = Date::COMPARE::YYYYMMDDHHMMSS) const;
        int                             day() const
                                            { return _day; }
        Date&                           day(int day);
        Date&                           day_last();
        int                             diff_days(const Date& other) const;
        int                             diff_months(const Date& other) const;
        int                             diff_seconds(const Date& other) const;
        std::string                     format(Date::FORMAT format = Date::FORMAT::ISO) const;
        int                             hour() const
                                            { return _hour; }
        Date&                           hour(int hour);
        bool                            is_leapyear() const;
        int                             minute() const
                                            { return _min; }
        Date&                           minute(int min);
        int                             month() const
                                            { return _month; }
        Date&                           month(int month);
        int                             month_days() const;
        const char*                     month_name() const;
        const char*                     month_name_short() const;
        void                            print() const;
        int                             second() const
                                            { return _sec; }
        Date&                           second(int sec);
        Date&                           set(const Date& other);
        Date&                           set(int year, int month, int day, int hour = 0, int min = 0, int sec = 0);
        int64_t                         time() const;
        int                             week() const;
        Date::DAY                       weekday() const;
        Date&                           weekday(Date::DAY weekday);
        const char*                     weekday_name() const;
        const char*                     weekday_name_short() const;
        int                             year() const
                                            { return _year; }
        Date&                           year(int year);
        int                             yearday() const;

        static int                      Compare(const void* a, const void* b);
        static bool                     Compare(const Date& a, const Date& b);
        static void                     Del(void* self);
        static Date                     FromString(const char* string, bool us = false);
        static Date                     FromTime(int64_t seconds, bool utc = false);

    private:
        short                           _year;
        char                            _month;
        char                            _day;
        char                            _hour;
        char                            _min;
        char                            _sec;
    };
}

// MKALGAM_OFF

#endif
