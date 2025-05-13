// Copyright gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#include "date.h"

// MKALGAM_ON

#include <cstring>
#include <cassert>
#include <ctime>

namespace gnu {

/***
 *      _____      _            _
 *     |  __ \    (_)          | |
 *     | |__) | __ ___   ____ _| |_ ___
 *     |  ___/ '__| \ \ / / _` | __/ _ \
 *     | |   | |  | |\ V / (_| | ||  __/
 *     |_|   |_|  |_| \_/ \__,_|\__\___|
 *
 *
 */

static int          _DATE_DAYS_MONTH[]      = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
static int          _DATE_DAYS_MONTH_LEAP[] = {0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
static const char*  _DATE_WEEKDAYS[]        = {"", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday", ""};
static const char*  _DATE_WEEKDAYS_SHORT[]  = {"", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun", ""};
static const char*  _DATE_MONTHS[]          = {"", "January", "February", "Mars", "April", "May", "June", "July", "August", "September", "October", "November", "December", ""};
static const char*  _DATE_MONTHS_SHORT[]    = {"", "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec", ""};

//------------------------------------------------------------------------------
static int _date_days_in_month(int year, int month) {
    if (year < 1 || year > 9999 || month < 1 || month > 12) {
        return 0;
    }
    else if ((year % 4 == 0 && year % 100 != 0) || year % 400 == 0) {
        return _DATE_DAYS_MONTH_LEAP[month];
    }

    return _DATE_DAYS_MONTH[month];
}

//------------------------------------------------------------------------------
static void _date_from_time(int64_t seconds, Date::UTC utc, int& year, int& month, int& day, int& hour, int& min, int& sec) {
    year = month = day = 1;
    hour = min = sec = 0;

    if (seconds < 0) {
        return;
    }

    time_t    rawtime  = seconds;
    const tm* timeinfo = (utc == Date::UTC::ON) ? gmtime(&rawtime) : localtime(&rawtime);

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
static Date::DAY _date_weekday(int year, int month, int day) {
    if (year > 0 && year < 10000 && month > 0 && month < 13 && day > 0 && day <= _date_days_in_month(year, month)) {
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
            int days = _date_days_in_month(year, i);

            if (days) {
                start += days;
            }
            else {
                return Date::DAY::INVALID;
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
            return Date::DAY::INVALID;
        }
        else {
            return (Date::DAY) start;
        }
    }

    return Date::DAY::INVALID;
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
Date::Date(Date::UTC utc) {
    int y, m, d, ho, mi, se;
    _year = _month = _day = _hour = _min = _sec = 0;
    _date_from_time(::time(nullptr), utc, y, m, d, ho, mi, se);
    set(y, m, d, ho, mi, se);
}

//------------------------------------------------------------------------------
Date::Date(int year, int month, int day, int hour, int min, int sec) {
    _year = _month = _day = _hour = _min = _sec = 0;
    set(year, month, day, hour, min, sec);
}

//------------------------------------------------------------------------------
Date::Date(int64_t unix_time, Date::UTC utc) {
    int y, m, d, ho, mi, se;
    _date_from_time(unix_time, utc, y, m, d, ho, mi, se);
    set(y, m, d, ho, mi, se);
}


//------------------------------------------------------------------------------
// Create date object by parsing a string buffer
// Valid formats for dates are:
// "19991224"
// "1999-12-24"
// "24/12/1999" world format
// "12/24/1999" us format
// And optional time after date with at least one space between them:
// "0000"
// "000000"
// "00:00:00"
// "00:00"
// Invalid input sets date to 0
//
Date::Date(const std::string& date, Date::US us) {
    _year = _month = _day = _hour = _min = _sec = 0;
    set(date, us);
}

//------------------------------------------------------------------------------
bool Date::add_days(const int days) {
    if (days == 0) {
        return 0;
    }

    int daym = _date_days_in_month(_year, _month);

    if (daym == 0) {
        return false;
    }

    int inc = days > 0 ? 1 : -1;
    int y   = _year;
    int m   = _month;
    int d   = _day;

    for (int f = 0; f < abs(days); f++) {
        d += inc;

        if (inc < 0 && d == 0) {
            m--;

            if (m == 0) {
                m = 12;
                y--;

                if (y < 1) {
                    return false;
                }
            }

            d = _date_days_in_month(y, m);

            if (d == 0) {
                return false;
            }
        }
        else if (inc > 0 && d > daym) {
            d = 1;
            m++;

            if (m == 13) {
                m = 1;
                y++;

                if (y > 9999) {
                    return false;
                }
            }

            daym = _date_days_in_month(y, m);

            if (daym == 0) {
                return false;
            }
        }
    }

    _year  = y;
    _month = m;
    _day   = d;

    return true;
}

//------------------------------------------------------------------------------
bool Date::add_months(const int months) {
    if (months == 0) {
        return false;
    }

    int inc = months > 0 ? 1 : -1;
    int m   = _month;
    int y   = _year;

    for (int f = 0; f < abs(months); f++) {
        m += inc;

        if (m == 0) {
            m = 12;
            y--;

            if (y < 1) {
                return false;
            }
        }
        else if (m > 12) {
            m = 1;
            y++;

            if (y > 9999) {
                return false;
            }
        }
    }

    const int days = _date_days_in_month(y, m);

    if (days <= 0) {
        return false;
    }

    _year  = y;
    _month = m;

    if (_day > days) {
        _day = days;
    }

    return true;
}

//------------------------------------------------------------------------------
bool Date::add_seconds(const int64_t seconds) {
    if (seconds == 0) {
        return false;
    }

    int inc = seconds > 0 ? 1 : -1;
    int h   = _hour;
    int m   = _min;
    int s   = _sec;

    for (int64_t f = 0; f < llabs(seconds); f++) {
        s += inc;

        if (inc < 0 && s == -1) {
            m--;

            if (m == -1) {
                m = 59;
                h--;

                if (h == -1) {
                    h = 23;

                    if (add_days(-1) == false) {
                        return false;
                    }
                }
            }

            s = 59;
        }
        else if (inc > 0 && s == 60) {
            m++;

            if (m == 60) {
                m = 0;
                h++;

                if (h == 24) {
                    h = 0;

                    if (add_days(1) == false) {
                        return false;
                    }
                }
            }

            s = 0;
        }
    }

    _hour = h;
    _min  = m;
    _sec  = s;

    return true;
}

//------------------------------------------------------------------------------
int Date::compare(const Date& other, Date::COMPARE flag) const {
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

    if (flag >= Date::COMPARE::YYYYMMDD) {
        if (_day < other._day) {
            return -1;
        }
        else if (_day > other._day) {
            return 1;
        }
    }

    if (flag >= Date::COMPARE::YYYYMMDDHH) {
        if (_hour < other._hour) {
            return -1;
        }
        else if (_hour > other._hour) {
            return 1;
        }
    }

    if (flag >= Date::COMPARE::YYYYMMDDHHMM) {
        if (_min < other._min) {
            return -1;
        }
        else if (_min > other._min) {
            return 1;
        }
    }

    if (flag >= Date::COMPARE::YYYYMMDDHHMMSS) {
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
int Date::days_in_month() const {
    return _date_days_in_month(_year, _month);
}

//------------------------------------------------------------------------------
int Date::days_into_year() const {
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

//------------------------------------------------------------------------------
void Date::debug() const {
    printf("Date| %s\n", format(Date::FORMAT::ISO_TIME_LONG).c_str());
    fflush(stdout);
}

//------------------------------------------------------------------------------
int Date::diff_days(const Date& date) const {
    Date d(date);
    int  res = 0;

    if (compare(d, Date::COMPARE::YYYYMMDD) < 0) {
        while (compare(d, Date::COMPARE::YYYYMMDD) != 0) {
            d.add_days(-1);
            res++;
        }
    }
    else if (compare(d, Date::COMPARE::YYYYMMDD) > 0) {
        while (compare(d, Date::COMPARE::YYYYMMDD) != 0) {
            d.add_days(1);
            res--;
        }
    }

    return res;
}

//------------------------------------------------------------------------------
int Date::diff_months(const Date& date) const {
    Date d(date);
    int  res = 0;

    if (compare(d, Date::COMPARE::YYYYMM) < 0) {
        while (compare(d, Date::COMPARE::YYYYMM)) {
            d.add_months(-1);
            res++;
        }
    }
    else if (compare(d, Date::COMPARE::YYYYMM) > 0) {
        while (compare(d, Date::COMPARE::YYYYMM)) {
            d.add_months(1);
            res--;
        }
    }

    return res;
}

//------------------------------------------------------------------------------
int Date::diff_seconds(const Date& date) const {
    int64_t unix1 = time();
    int64_t unix2 = date.time();

    if (unix1 >= 0 && unix2 >= 0) {
        return unix2 - unix1;
    }

    return 0;
}

//------------------------------------------------------------------------------
std::string Date::format(Date::FORMAT format) const {
    char tmp[100];
    int  n = 0;

    if (format == Date::FORMAT::ISO) {
        n = snprintf(tmp, 100, "%04d%02d%02d", _year, _month, _day);
    }
    else if (format == Date::FORMAT::ISO_LONG) {
        n = snprintf(tmp, 100, "%04d-%02d-%02d", _year, _month, _day);
    }
    else if (format == Date::FORMAT::ISO_TIME) {
        n = snprintf(tmp, 100, "%04d%02d%02d %02d%02d%02d", _year, _month, _day, _hour, _min, _sec);
    }
    else if (format == Date::FORMAT::ISO_TIME_LONG) {
        n = snprintf(tmp, 100, "%04d-%02d-%02d %02d:%02d:%02d", _year, _month, _day, _hour, _min, _sec);
    }
    else if (format == Date::FORMAT::TIME) {
        n = snprintf(tmp, 100, "%02d%02d%02d", _hour, _min, _sec);
    }
    else if (format == Date::FORMAT::TIME_LONG) {
        n = snprintf(tmp, 100, "%02d:%02d:%02d", _hour, _min, _sec);
    }
    else if (format == Date::FORMAT::US) {
        n = snprintf(tmp, 100, "%d/%d/%04d", _month, _day, _year);
    }
    else if (format == Date::FORMAT::WORLD) {
        n = snprintf(tmp, 100, "%d/%d/%04d", _day, _month, _year);
    }
    else if (format == Date::FORMAT::DAY_MONTH_YEAR) {
        n = snprintf(tmp, 100, "%d %s %04d", _day, month_name(), _year);
    }
    else if (format == Date::FORMAT::DAY_MONTH_YEAR_SHORT) {
        n = snprintf(tmp, 100, "%d %s, %04d", _day, month_name_short(), _year);
    }
    else if (format == Date::FORMAT::WEEKDAY_MONTH_YEAR) {
        n = snprintf(tmp, 100, "%s %d %s %04d", weekday_name(), _day, month_name(), _year);
    }
    else if (format == Date::FORMAT::WEEKDAY_MONTH_YEAR_SHORT) {
        n = snprintf(tmp, 100, "%s, %d %s, %04d", weekday_name_short(), _day, month_name_short(), _year);
    }

    if (n < 0 || n >= 100) {
        *tmp = 0;
    }

    return tmp;
}

//------------------------------------------------------------------------------
bool Date::is_leapyear() const {
    return _date_is_leapyear(_year);
}

//------------------------------------------------------------------------------
const char* Date::month_name() const {
    return _DATE_MONTHS[(int) _month];
}

//------------------------------------------------------------------------------
const char* Date::month_name_short() const {
    return _DATE_MONTHS_SHORT[(int) _month];
}

//------------------------------------------------------------------------------
Date& Date::set(const Date& date) {
    _year  = date._year;
    _month = date._month;
    _day   = date._day;
    _hour  = date._hour;
    _min   = date._min;
    _sec   = date._sec;

    return *this;
}

//------------------------------------------------------------------------------
Date& Date::set(const std::string& date, Date::US us) {
    auto str1 = date.c_str();
    auto len1 = static_cast<int>(date.length());
    auto str2 = strstr(str1, " ");

    while (str2 != nullptr && *str2 == ' ') { // Find possible time string.
        str2++;

        if (*str2 == 0) {
            str2 = nullptr;
        }
    }

    int      len2    = (str2 != nullptr) ? strlen(str2) : 0;
    int      year    = 0;
    int      month   = 0;
    int      day     = 0;
    int      hour    = 0;
    int      min     = 0;
    int      sec     = 0;
    int      Y       = 0;
    bool     time    = false;
    unsigned val[14] = { 0 };
    unsigned EMPTY   = -1;

    if (len1 >= 10 && str1[4] == '-' && str1[7] == '-' && str1[10] <= ' ') { // "YYYY-MM-DD"
        us     = US::OFF;
        val[4] = str1[5] - '0';
        val[5] = str1[6] - '0';
        val[6] = str1[8] - '0';
        val[7] = str1[9] - '0';
    }
    else if (len1 >= 8 && str1[1] == '/' && str1[3] == '/' && str1[8] <= ' ') { // "D/M/YYYY"
        Y      = 4;
        val[4] = str1[2] - '0';
        val[5] = EMPTY;
        val[6] = str1[0] - '0';
        val[7] = EMPTY;
    }
    else if (len1 >= 9 && str1[1] == '/' && str1[4] == '/' && str1[9] <= ' ') { // "D/MM/YYYY"
        Y      = 5;
        val[4] = str1[2] - '0';
        val[5] = str1[3] - '0';
        val[6] = str1[0] - '0';
        val[7] = EMPTY;
    }
    else if (len1 >= 9 && str1[2] == '/' && str1[4] && str1[9] <= ' ') { // "DD/M/YYYY"
        Y      = 5;
        val[4] = str1[3] - '0';
        val[5] = EMPTY;
        val[6] = str1[0] - '0';
        val[7] = str1[1] - '0';
    }
    else if (len1 >= 10 && str1[2] == '/' && str1[5] == '/' && str1[10] <= ' ') { // "DD/MM/YYYY"
        Y      = 6;
        val[4] = str1[3] - '0';
        val[5] = str1[4] - '0';
        val[6] = str1[0] - '0';
        val[7] = str1[1] - '0';
    }
    else if (len1 >= 8 && str1[8] <= ' ') { // "YYYYMMDD"
        us     = US::OFF;
        val[4] = str1[4] - '0';
        val[5] = str1[5] - '0';
        val[6] = str1[6] - '0';
        val[7] = str1[7] - '0';
    }
    else {
        return *this;
    }

    val[0] = str1[Y] - '0';
    val[1] = str1[Y + 1] - '0';
    val[2] = str1[Y + 2] - '0';
    val[3] = str1[Y + 3] - '0';

    if (str2 != nullptr && str2[0] >= '0' && str2[0] <= '9') {
        if (len2 >= 4 && str2[4] <= ' ') {  // "HHMM"
            time    = true;
            val[8]  = str2[0] - '0';
            val[9]  = str2[1] - '0';
            val[10] = str2[2] - '0';
            val[11] = str2[3] - '0';
        }
        else if (len2 >= 5 && str2[2] == ':' && str2[5] <= ' ') { // "HH:MM"
            time    = true;
            val[8]  = str2[0] - '0';
            val[9]  = str2[1] - '0';
            val[10] = str2[3] - '0';
            val[11] = str2[4] - '0';
        }
        else if (len2 >= 6 && str2[6] <= ' ') {  // "HHMMSS"
            time    = true;
            val[8]  = str2[0] - '0';
            val[9]  = str2[1] - '0';
            val[10] = str2[2] - '0';
            val[11] = str2[3] - '0';
            val[12] = str2[4] - '0';
            val[13] = str2[5] - '0';
        }
        else if (len2 >= 8 && str2[2] == ':' && str2[5] == ':' && str2[8] <= ' ') { // "HH:MM:SS"
            time    = true;
            val[8]  = str2[0] - '0';
            val[9]  = str2[1] - '0';
            val[10] = str2[3] - '0';
            val[11] = str2[4] - '0';
            val[12] = str2[6] - '0';
            val[13] = str2[7] - '0';
        }

        for (int f = 8; f < 14 && time == true; f++) { // Check time and disable if any error.
            if (val[f] > 9) {
                time = false;
            }
        }

        if (time == true) {
            hour = val[8] * 10 + val[9];
            min  = val[10] * 10 + val[11];
            sec  = val[12] * 10 + val[13];

            if (hour < 0 || hour > 23) {
                time = false;
            }
            else if (min < 0 || min > 59) {
                time = false;
            }
            else if (sec < 0 || sec > 59) {
                time = false;
            }
        }
    }

//     printf("Y(%2u, %2u, %2u, %2u)  M(%2u, %2u)  D(%2u, %2u)  T(%2u, %2u, %2u, %2u, %2u, %2u),  len1=%2u, len2=%2u,  <%s>,  <%s>\n", val[0], val[1], val[2], val[3], val[4], val[5], val[6], val[7], val[8], val[9], val[10], val[11], val[12], val[13], len1, len2, str1, str2 ? str2 : "NULL");
//     fflush(stdout);

    for (int f = 0; f < 8; f++) { // Check date.
        if (f == 5 && val[5] == EMPTY) {
        }
        else if (f == 7 && val[7] == EMPTY) {
        }
        else if (val[f] > 9) {
            return *this;
        }
    }

    year  = val[0] * 1000 + val[1] * 100 + val[2] * 10 + val[3];
    month = (val[5] == EMPTY) ? val[4] : val[4] * 10 + val[5];
    day   = (val[7] == EMPTY) ? val[6] : val[6] * 10 + val[7];

    if (us == US::ON) { // Swap world and US numbers.
        int tmp = month;
        month   = day;
        day     = tmp;
    }

    if (time == true) {
        set(year, month, day, hour, min, sec);
    }
    else {
        set(year, month, day);
    }

    return *this;
}

//------------------------------------------------------------------------------
Date& Date::set(int year, int month, int day, int hour, int min, int sec) {
    if (year < 1 || year > 9999 ||
        month < 1 || month > 12 ||
        day < 1 || day > _date_days_in_month(year, month) ||
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
Date& Date::set_day(int day) {
    if (day > 0 && day <= _date_days_in_month(_year, _month)) {
        _day = day;
    }

    return *this;
}

//------------------------------------------------------------------------------
Date& Date::set_hour(int hour) {
    if (hour >= 0 && hour <= 23) {
        _hour = hour;
    }

    return *this;
}

//------------------------------------------------------------------------------
Date& Date::set_minute(int min) {
    if (min >= 0 && min <= 59) {
        _min = min;
    }

    return *this;
}

//------------------------------------------------------------------------------
Date& Date::set_month(int month) {
    if (_day >= 1 &&
        _day <= _date_days_in_month(_year, month) &&
        month >= 1 && month <= 12) {
        _month = month;
    }

    return *this;
}

//------------------------------------------------------------------------------
Date& Date::set_second(int sec) {
    if (sec >= 0 && sec <= 59) {
        _sec = sec;
    }

    return *this;
}

//------------------------------------------------------------------------------
Date& Date::set_weekday(Date::DAY day) {
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
Date& Date::set_year(int year) {
    if (year >= 1 && year <= 9999) {
        _year = year;
    }

    return *this;
}

//------------------------------------------------------------------------------
int64_t Date::time() const {
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
int Date::week() const {
    Date::DAY wday  = _date_weekday(_year, _month, _day);
    Date::DAY wday1 = _date_weekday(_year, 1, 1);

    if (wday != Date::DAY::INVALID && wday1 != Date::DAY::INVALID) {
        auto w     = 0;
        auto y1    = _year - 1;
        auto leap  = _date_is_leapyear(_year);
        auto leap1 = _date_is_leapyear(y1);
        auto yday  = days_into_year();

        if (yday <= (8 - (int) wday1) && wday1 > Date::DAY::THURSDAY) {
            if (wday1 == Date::DAY::FRIDAY || (wday1 == Date::DAY::SATURDAY && leap1)) {
                w = 53;
            }
            else {
                w = 52;
            }
        }
        else {
            auto days = leap ? 366 : 365;

            if ((days - yday) < (4 - (int) wday)) {
                w = 1;
            }
            else {
                days = yday + (7 - (int) wday) + ((int) wday1 - 1);
                days = days / 7;

                if (wday1 > Date::DAY::THURSDAY) {
                    days--;
                }

                w = days;
            }
        }

        if (w > 0 && w < 54) {
            return w;
        }
    }

    return 0;
}

//------------------------------------------------------------------------------
Date::DAY Date::weekday() const {
    return _date_weekday(_year, _month, _day);
}

//------------------------------------------------------------------------------
const char* Date::weekday_name() const {
    return _DATE_WEEKDAYS[(int) _date_weekday(_year, _month, _day)];
}

//------------------------------------------------------------------------------
const char* Date::weekday_name_short() const {
    return _DATE_WEEKDAYS_SHORT[(int) _date_weekday(_year, _month, _day)];
}

} // flw

// MKALGAM_OFF
