// Copyright gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#include "date.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// MKALGAM_ON

namespace flw {
    static int          _DATE_DAYS_MONTH[]      = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    static int          _DATE_DAYS_MONTH_LEAP[] = {0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    static const char*  _DATE_WEEKDAYS[]        = {"", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday", ""};
    static const char*  _DATE_WEEKDAYS_SHORT[]  = {"", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun", ""};
    static const char*  _DATE_MONTHS[]          = {"", "January", "February", "Mars", "April", "May", "June", "July", "August", "September", "October", "November", "December", ""};
    static const char*  _DATE_MONTHS_SHORT[]    = {"", "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec", ""};

    const int Date::SECS_PER_HOUR = 3600;
    const int Date::SECS_PER_DAY  = 3600 * 24;
    const int Date::SECS_PER_WEEK = 3600 * 24 * 7;

    //------------------------------------------------------------------------------
    static int _date_days(int year, int month) {
        if (year < 1 || year > 9999 || month < 1 || month > 12) {
            return 0;
        }
        else if ((year % 4 == 0 && year % 100 != 0) || year % 400 == 0) {
            return _DATE_DAYS_MONTH_LEAP[month];
        }

        return _DATE_DAYS_MONTH[month];
    }

    //------------------------------------------------------------------------------
    static void _date_from_time(int64_t seconds, bool utc, int& year, int& month, int& day, int& hour, int& min, int& sec) {
        year = month = day = 1;
        hour = min = sec = 0;

        if (seconds < 0) {
            return;
        }

        auto rawtime  = (time_t) seconds;
        auto timeinfo = utc ? gmtime(&rawtime) : localtime(&rawtime);

        if (timeinfo == nullptr) {
            return;
        }

        year  = timeinfo->tm_year + 1900;
        month = timeinfo->tm_mon + 1;
        day   = timeinfo->tm_mday;
        hour  = timeinfo->tm_hour;
        min   = timeinfo->tm_min;
        sec   = timeinfo->tm_sec;
    }

    //------------------------------------------------------------------------------
    static Date _date_parse(const char* string, bool us) {
        auto space = strstr(string, " ");
        int  tot   = strlen(string);
        int  tlen  = space ? (int) (space - string) : 0;
        int  len   = space ? tlen : tot;
        int  year  = 1;
        int  month = 1;
        int  day   = 1;
        int  hour  = 0;
        int  min   = 0;
        int  sec   = 0;
        bool iso   = false;
        int  Y     = 0;
        int  val[15];

        for (int f = 0; f < 15; f++) {
            val[f] = 0;
        }

        if (len == 10 && string[4] == '-' && string[7] == '-') { // "YYYY-MM-DD"
            iso    = true;
            val[4] = string[5] - '0';
            val[5] = string[6] - '0';
            val[6] = string[8] - '0';
            val[7] = string[9] - '0';
        }
        else if (len == 8 && string[1] == '/' && string[3] == '/') { // "D/M/YYYY"
            Y      = 4;
            val[4] = string[2] - '0';
            val[5] = -1;
            val[6] = string[0] - '0';
            val[7] = -1;
        }
        else if (len == 9 && string[1] == '/' && string[4] == '/') { // "D/MM/YYYY"
            Y      = 5;
            val[4] = string[2] - '0';
            val[5] = string[3] - '0';
            val[6] = string[0] - '0';
            val[7] = -1;
        }
        else if (len == 9 && string[2] == '/' && string[4]) { // "DD/M/YYYY"
            Y      = 5;
            val[4] = string[3] - '0';
            val[5] = -1;
            val[6] = string[0] - '0';
            val[7] = string[1] - '0';
        }
        else if (len == 10 && string[2] == '/' && string[5] == '/') { // "DD/MM/YYYY"
            Y      = 6;
            val[4] = string[3] - '0';
            val[5] = string[4] - '0';
            val[6] = string[0] - '0';
            val[7] = string[1] - '0';
        }
        else if (len == 8) { // "YYYYMMDD"
            iso    = true;
            val[4] = string[4] - '0';
            val[5] = string[5] - '0';
            val[6] = string[6] - '0';
            val[7] = string[7] - '0';
        }
        else {
            return Date(1, 1, 1);
        }

        val[0] = string[Y] - '0';
        val[1] = string[Y + 1] - '0';
        val[2] = string[Y + 2] - '0';
        val[3] = string[Y + 3] - '0';

        if (tlen && tot - tlen >= 9) {
            val[8]  = string[tlen + 1] - '0';
            val[9]  = string[tlen + 2] - '0';
            val[10] = string[tlen + 4] - '0';
            val[11] = string[tlen + 5] - '0';
            val[12] = string[tlen + 7] - '0';
            val[13] = string[tlen + 8] - '0';
        }
        else if (tlen && tot - tlen >= 7) {
            val[8]  = string[tlen + 1] - '0';
            val[9]  = string[tlen + 2] - '0';
            val[10] = string[tlen + 3] - '0';
            val[11] = string[tlen + 4] - '0';
            val[12] = string[tlen + 5] - '0';
            val[13] = string[tlen + 6] - '0';
        }

        // printf("(%2d, %2d, %2d, %2d)  (%2d, %2d)  (%2d, %2d)  (%2d, %2d, %2d, %2d, %2d, %2d)  <%s>\n", val[0], val[1], val[2], val[3], val[4], val[5], val[6], val[7], val[8], val[9], val[10], val[11], val[12], val[13], string);
        // fflush(stdout);

        for (int f = 0; f < 15; f++) {
            if ((f == 5 || f == 7) && val[f] == -1) {
                ;
            }
            else if (val[f] < 0 || val[f] > 9) {
                return Date(1, 1, 1);
            }
        }

        year  = val[0] * 1000 + val[1] * 100 + val[2] * 10 + val[3];
        month = val[5] == -1 ? val[4] : val[4] * 10 + val[5];
        day   = val[7] == -1 ? val[6] : val[6] * 10 + val[7];
        hour  = val[8] * 10 + val[9];
        min   = val[10] * 10 + val[11];
        sec   = val[12] * 10 + val[13];

        if (iso == false && us) { // Swap world and us numbers
            int tmp = month;
            month   = day;
            day     = tmp;
        }

        return Date(year, month, day, hour, min, sec);
    }

    //------------------------------------------------------------------------------
    static flw::Date::DAY _date_weekday(int year, int month, int day) {
        if (year > 0 && year < 10000 && month > 0 && month < 13 && day > 0 && day <= _date_days(year, month)) {
            int start = 0;
            int y1    = year - 1;
            int pre   = ((year < 1582) || ((year == 1582) && (month <= 10)));

            if (pre) {
                start = 6 + y1 + (y1 / 4);
            }
            else {
                start = 1 + y1 + (y1 / 4) - (y1 / 100) + (y1 / 400);
            }

            for (int i = 1; i < month; i++) {
                int days = _date_days(year, i);

                if (days) {
                    start += days;
                }
                else {
                    return flw::Date::DAY::INVALID;
                }
            }

            start = start % 7;
            start = start == 0 ? 7 : start;

            for (int i = 2; i <= day; i++) {
                start++;

                if (start > 7) {
                    start = 1;
                }
            }

            if (start < 1 || start > 7) {
                return flw::Date::DAY::INVALID;
            }
            else {
                return (flw::Date::DAY) start;
            }
        }

        return flw::Date::DAY::INVALID;
    }

    //------------------------------------------------------------------------------
    static bool _date_is_leapyear(int year) {
        if (year < 1 || year > 9999) {
            return false;
        }
        else if ((year % 4 == 0 && year % 100 != 0) || year % 400 == 0) {
            return true;
        }
        else {
           return false;
        }
    }
}

//------------------------------------------------------------------------------
flw::Date::Date(bool utc) {
    int year, month, day, hour, min, sec;
    _date_from_time(::time(nullptr), utc, year, month, day, hour, min, sec);
    set(year, month, day, hour, min, sec);
}

//------------------------------------------------------------------------------
flw::Date::Date(const Date& other) {
    set(other);
}

//------------------------------------------------------------------------------
flw::Date::Date(Date&& other) {
    set(other);
}

//------------------------------------------------------------------------------
flw::Date::Date(int year, int month, int day, int hour, int min, int sec) {
    _year = _month = _day = 1;
    _hour = _min = _sec = 0;
    set(year, month, day, hour, min, sec);
}

//------------------------------------------------------------------------------
flw::Date& flw::Date::operator=(const Date& date) {
    set(date);
    return *this;
}

//------------------------------------------------------------------------------
flw::Date& flw::Date::operator=(Date&& date) {
    set(date);
    return *this;
}

//------------------------------------------------------------------------------
bool flw::Date::add_days(const int days) {
    if (days) {
        int daym = _date_days(_year, _month);

        if (daym > 0) {
            const int inc   = days > 0 ? 1 : -1;
            int       year  = _year;
            int       month = _month;
            int       day   = _day;

            for (int f = 0; f < abs(days); f++) {
                day += inc;

                if (inc < 0 && day == 0) {
                    month--;

                    if (month == 0) {
                        month = 12;
                        year--;

                        if (year < 1) {
                            return false;
                        }
                    }

                    day = _date_days(year, month);

                    if (day == 0) {
                        return false;
                    }
                }
                else if (inc > 0 && day > daym) {
                    day = 1;
                    month++;

                    if (month == 13) {
                        month = 1;
                        year++;

                        if (year > 9999) {
                            return false;
                        }
                    }

                    daym = _date_days(year, month);

                    if (daym == 0) {
                        return false;
                    }
                }
            }

            _year  = year;
            _month = month;
            _day   = day;

            return true;
        }
    }

    return false;
}

//------------------------------------------------------------------------------
bool flw::Date::add_months(const int months) {
    if (months) {
        const int inc   = months > 0 ? 1 : -1;
        int       month = _month;
        int       year  = _year;

        for (int f = 0; f < abs(months); f++) {
            month += inc;

            if (month == 0) {
                month = 12;
                year--;

                if (year < 1) {
                    return false;
                }
            }
            else if (month > 12) {
                month = 1;
                year++;

                if (year > 9999) {
                    return false;
                }
            }
        }

        const int days = _date_days(year, month);

        if (days > 0) {
            _year  = year;
            _month = month;

            if (_day > days) {
                _day = days;
            }

            return true;
        }
    }

    return false;
}

//------------------------------------------------------------------------------
bool flw::Date::add_seconds(const int64_t seconds) {
    if (seconds) {
        const int inc  = seconds > 0 ? 1 : -1;
        int       hour = _hour;
        int       min  = _min;
        int       sec  = _sec;

        for (int64_t f = 0; f < llabs(seconds); f++) {
            sec += inc;

            if (inc < 0 && sec == -1) {
                min--;

                if (min == -1) {
                    min = 59;
                    hour--;

                    if (hour == -1) {
                        hour = 23;

                        if (add_days(-1) == false) {
                            return false;
                        }
                    }
                }

                sec = 59;
            }
            else if (inc > 0 && sec == 60) {
                min++;

                if (min == 60) {
                    min = 0;
                    hour++;

                    if (hour == 24) {
                        hour = 0;

                        if (add_days(1) == false) {
                            return false;
                        }
                    }
                }

                sec = 0;
            }
        }

        _hour = hour;
        _min  = min;
        _sec  = sec;

        return true;
    }

    return false;
}

//------------------------------------------------------------------------------
int flw::Date::compare(const Date& other, flw::Date::COMPARE flag) const {
    if (_year < other._year) {
        return -1;
    }
    else if (_year > other._year) {
        return 1;
    }

    if (_month < other._month) {
        return -1;
    }
    else if (_month > other._month) {
        return 1;
    }

    if (flag >= flw::Date::COMPARE::YYYYMMDD) {
        if (_day < other._day) {
            return -1;
        }
        else if (_day > other._day) {
            return 1;
        }
    }

    if (flag >= flw::Date::COMPARE::YYYYMMDDHH) {
        if (_hour < other._hour) {
            return -1;
        }
        else if (_hour > other._hour) {
            return 1;
        }
    }

    if (flag >= flw::Date::COMPARE::YYYYMMDDHHMM) {
        if (_min < other._min) {
            return -1;
        }
        else if (_min > other._min) {
            return 1;
        }
    }

    if (flag >= flw::Date::COMPARE::YYYYMMDDHHMMSS) {
        if (_sec < other._sec) {
            return -1;
        }
        else if (_sec > other._sec) {
            return 1;
        }
    }

    return 0;
}

//------------------------------------------------------------------------------
int flw::Date::Compare(const void* a, const void* b) {
    Date* A = *(Date**) a;
    Date* B = *(Date**) b;
    return A->compare(*B);
}

//------------------------------------------------------------------------------
bool flw::Date::Compare(const Date& a, const Date& b) {
    return a.compare(b) < 0;
}

//------------------------------------------------------------------------------
flw::Date& flw::Date::day(int day) {
    if (day > 0 && day <= _date_days(_year, _month)) {
        _day = day;
    }

    return *this;
}

//------------------------------------------------------------------------------
flw::Date& flw::Date::day_last() {
    _day = month_days();
    return *this;
}

//------------------------------------------------------------------------------
void flw::Date::Del(void* self) {
    if (self) {
        delete (Date*) self;
    }
}

//------------------------------------------------------------------------------
int flw::Date::diff_days(const Date& date) const {
    Date d(date);
    int  res = 0;

    if (compare(d, flw::Date::COMPARE::YYYYMMDD) < 0) {
        while (compare(d, flw::Date::COMPARE::YYYYMMDD) != 0) {
            d.add_days(-1);
            res++;
        }
    }
    else if (compare(d, flw::Date::COMPARE::YYYYMMDD) > 0) {
        while (compare(d, flw::Date::COMPARE::YYYYMMDD) != 0) {
            d.add_days(1);
            res--;
        }
    }

    return res;
}

//------------------------------------------------------------------------------
int flw::Date::diff_months(const Date& date) const {
    Date d(date);
    int  res = 0;

    if (compare(d, flw::Date::COMPARE::YYYYMM) < 0) {
        while (compare(d, flw::Date::COMPARE::YYYYMM)) {
            d.add_months(-1);
            res++;
        }
    }
    else if (compare(d, flw::Date::COMPARE::YYYYMM) > 0) {
        while (compare(d, flw::Date::COMPARE::YYYYMM)) {
            d.add_months(1);
            res--;
        }
    }

    return res;
}

//------------------------------------------------------------------------------
int flw::Date::diff_seconds(const Date& date) const {
    int64_t unix1 = time();
    int64_t unix2 = date.time();

    if (unix1 >= 0 && unix2 >= 0) {
        return unix2 - unix1;
    }

    return 0;
}

//------------------------------------------------------------------------------
std::string flw::Date::format(flw::Date::FORMAT format) const {
    char tmp[100];
    int  n = 0;

    if (format == flw::Date::FORMAT::ISO_LONG) {
        n = snprintf(tmp, 100, "%04d-%02d-%02d", _year, _month, _day);
    }
    else if (format == flw::Date::FORMAT::US) {
        n = snprintf(tmp, 100, "%d/%d/%04d", _month, _day, _year);
    }
    else if (format == flw::Date::FORMAT::WORLD) {
        n = snprintf(tmp, 100, "%d/%d/%04d", _day, _month, _year);
    }
    else if (format == flw::Date::FORMAT::NAME) {
        n = snprintf(tmp, 100, "%04d %s %d", _year, month_name_short(), _day);
    }
    else if (format == flw::Date::FORMAT::NAME_LONG) {
        n = snprintf(tmp, 100, "%04d %s %d", _year, month_name(), _day);
    }
    else if (format == flw::Date::FORMAT::YEAR_MONTH) {
        n = snprintf(tmp, 100, "%04d %s", _year, month_name_short());
    }
    else if (format == flw::Date::FORMAT::YEAR_MONTH_LONG) {
        n = snprintf(tmp, 100, "%04d %s", _year, month_name());
    }
    else if (format == flw::Date::FORMAT::ISO_TIME) {
        n = snprintf(tmp, 100, "%04d%02d%02d %02d%02d%02d", _year, _month, _day, _hour, _min, _sec);
    }
    else if (format == flw::Date::FORMAT::ISO_TIME_LONG) {
        n = snprintf(tmp, 100, "%04d-%02d-%02d %02d:%02d:%02d", _year, _month, _day, _hour, _min, _sec);
    }
    else {
        n = snprintf(tmp, 100, "%04d%02d%02d", _year, _month, _day);
    }

    if (n < 0 || n >= 100) {
        *tmp = 0;
    }

    return tmp;
}

//------------------------------------------------------------------------------
// Create date object by parsing a string buffer
// Valid formats for dates are:
// "19991224"
// "19991224"
// "1999-12-24"
// "24/12/1999" "world formar"
// "12/24/1999" "us format"
// And optional time after date with one space between them:
// "000000"
// "00:00:00"
// Invalid date returns a Date set to (1, 1, 1)
//
flw::Date flw::Date::FromString(const char* buffer, bool us) {
    if (buffer == nullptr) {
        return Date(1, 1, 1);
    }
    else {
         return _date_parse(buffer, us);
    }
}

//------------------------------------------------------------------------------
flw::Date flw::Date::FromTime(int64_t seconds, bool utc) {
    int year, month, day, hour, min, sec;
    _date_from_time(seconds, utc, year, month, day, hour, min, sec);
    return Date(year, month, day, hour, min, sec);
}

//------------------------------------------------------------------------------
flw::Date& flw::Date::hour(int hour) {
    if (hour >= 0 && hour <= 23) {
        _hour = hour;
    }

    return *this;
}

//------------------------------------------------------------------------------
bool flw::Date::is_leapyear() const {
    return _date_is_leapyear(_year);
}

//------------------------------------------------------------------------------
flw::Date& flw::Date::minute(int min) {
    if (min >= 0 && min <= 59) {
        _min = min;
    }

    return *this;
}

//------------------------------------------------------------------------------
flw::Date& flw::Date::month(int month) {
    if (month >= 1 && month <= 12) {
        _month = month;
    }

    return *this;
}

//------------------------------------------------------------------------------
int flw::Date::month_days() const {
    return _date_days(_year, _month);
}

//------------------------------------------------------------------------------
const char* flw::Date::month_name() const {
    return _DATE_MONTHS[(int) _month];
}

//------------------------------------------------------------------------------
const char* flw::Date::month_name_short() const {
    return _DATE_MONTHS_SHORT[(int) _month];
}

//------------------------------------------------------------------------------
void flw::Date::print() const {
    auto string = format(flw::Date::FORMAT::ISO_TIME_LONG);
    printf("Date| %s\n", string.c_str());
    fflush(stdout);
}

//------------------------------------------------------------------------------
flw::Date& flw::Date::second(int sec) {
    if (sec >= 0 && sec <= 59) {
        _sec = sec;
    }

    return *this;
}

//------------------------------------------------------------------------------
flw::Date& flw::Date::set(const Date& date) {
    _year  = date._year;
    _month = date._month;
    _day   = date._day;
    _hour  = date._hour;
    _min   = date._min;
    _sec   = date._sec;

    return *this;
}

//------------------------------------------------------------------------------
flw::Date& flw::Date::set(int year, int month, int day, int hour, int min, int sec) {
    if (year < 1 || year > 9999 ||
        month < 1 || month > 12 ||
        day < 1 || day > _date_days(year, month) ||
        hour < 0 || hour > 23 ||
        min < 0 || min > 59 ||
        sec < 0 || sec > 59) {
        return *this;
    }
    else {
        _year  = year;
        _month = month;
        _day   = day;
        _hour  = hour;
        _min   = min;
        _sec   = sec;

        return *this;
    }
}

//------------------------------------------------------------------------------
int64_t flw::Date::time() const {
    tm t;

    if (_year < 1970) {
        return -1;
    }

    memset(&t, '\0', sizeof(t));
    t.tm_year = _year - 1900;
    t.tm_mon  = _month - 1;
    t.tm_mday = _day;
    t.tm_hour = _hour;
    t.tm_min  = _min;
    t.tm_sec  = _sec;
    return mktime(&t);
}

//------------------------------------------------------------------------------
int flw::Date::week() const {
    flw::Date::DAY wday  = _date_weekday(_year, _month, _day);
    flw::Date::DAY wday1 = _date_weekday(_year, 1, 1);

    if (wday != flw::Date::DAY::INVALID && wday1 != flw::Date::DAY::INVALID) {
        auto week  = 0;
        auto y1    = _year - 1;
        auto leap  = _date_is_leapyear(_year);
        auto leap1 = _date_is_leapyear(y1);
        auto yday  = yearday();

        if (yday <= (8 - (int) wday1) && wday1 > flw::Date::DAY::THURSDAY) {
            if (wday1 == flw::Date::DAY::FRIDAY || (wday1 == flw::Date::DAY::SATURDAY && leap1)) {
                week = 53;
            }
            else {
                week = 52;
            }
        }
        else {
            auto days = leap ? 366 : 365;

            if ((days - yday) < (4 - (int) wday)) {
                week = 1;
            }
            else {
                days = yday + (7 - (int) wday) + ((int) wday1 - 1);
                days = days / 7;

                if (wday1 > flw::Date::DAY::THURSDAY) {
                    days--;
                }

                week = days;
            }
        }

        if (week > 0 && week < 54) {
            return week;
        }
    }

    return 0;
}

//------------------------------------------------------------------------------
flw::Date::DAY flw::Date::weekday() const {
    return _date_weekday(_year, _month, _day);
}

//------------------------------------------------------------------------------
flw::Date& flw::Date::weekday(flw::Date::DAY day) {
    if (weekday() < day) {
        while (weekday() < day) {
            add_days(1);
        }
    }
    else if (weekday() > day) {
        while (weekday() > day) {
            add_days(-1);
        }
    }

    return *this;
}

//------------------------------------------------------------------------------
const char* flw::Date::weekday_name() const {
    return _DATE_WEEKDAYS[(int) _date_weekday(_year, _month, _day)];
}

//------------------------------------------------------------------------------
const char* flw::Date::weekday_name_short() const {
    return _DATE_WEEKDAYS_SHORT[(int) _date_weekday(_year, _month, _day)];
}

//------------------------------------------------------------------------------
flw::Date& flw::Date::year(int year) {
    if (year >= 1 && year <= 9999) {
        _year = year;
    }

    return *this;
}

//------------------------------------------------------------------------------
int flw::Date::yearday() const {
    auto res  = 0;
    auto leap = _date_is_leapyear(_year);

    for (auto m = 1; m < _month && m < 13; m++) {
        if (leap) {
            res += _DATE_DAYS_MONTH_LEAP[m];
        }
        else {
            res += _DATE_DAYS_MONTH[m];
        }
    }

    return res + _day;
}

// MKALGAM_OFF
