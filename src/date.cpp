// Copyright gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#include "date.h"

// MKALGAM_ON

#include <cstring>
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

    time_t    rawtime  = seconds;
    const tm* timeinfo = (utc == true) ? gmtime(&rawtime) : localtime(&rawtime);

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
    const char* space = strstr(string, " ");
    int  tot   = strlen(string);
    int  tlen  = space ? (int) (space - string) : 0;
    int  len   = space ? tlen : tot;
    int  year  = 1;
    int  month = 1;
    int  day   = 1;
    int  hour  = 0;
    int  min   = 0;
    int  sec   = 0;
    int  Y     = 0;
    int  val[15];

    for (int f = 0; f < 15; f++) {
        val[f] = 0;
    }

    if (len == 10 && string[4] == '-' && string[7] == '-') { // "YYYY-MM-DD"
        us     = false;
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
        us     = false;
        val[4] = string[4] - '0';
        val[5] = string[5] - '0';
        val[6] = string[6] - '0';
        val[7] = string[7] - '0';
    }
    else {
        return Date::InvalidDate();
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
            return Date::InvalidDate();
        }
    }

    year  = val[0] * 1000 + val[1] * 100 + val[2] * 10 + val[3];
    month = val[5] == -1 ? val[4] : val[4] * 10 + val[5];
    day   = val[7] == -1 ? val[6] : val[6] * 10 + val[7];
    hour  = val[8] * 10 + val[9];
    min   = val[10] * 10 + val[11];
    sec   = val[12] * 10 + val[13];

    if (us == true) { // Swap world and US numbers.
        int tmp = month;
        month   = day;
        day     = tmp;
    }

    return Date(year, month, day, hour, min, sec);
}

//------------------------------------------------------------------------------
static Date::DAY _date_weekday(int year, int month, int day) {
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
Date::Date(bool utc) {
    int y, m, d, ho, mi, se;
    _date_from_time(::time(nullptr), utc, y, m, d, ho, mi, se);
    set(y, m, d, ho, mi, se);
}

//------------------------------------------------------------------------------
Date::Date(const Date& other) {
    set(other);
}

//------------------------------------------------------------------------------
Date::Date(Date&& other) {
    set(other);
}

//------------------------------------------------------------------------------
Date::Date(int year, int month, int day, int hour, int min, int sec) {
    _year = _month = _day = 0;
    _hour = _min = _sec = 0;
    set(year, month, day, hour, min, sec);
}

//------------------------------------------------------------------------------
Date& Date::operator=(const Date& date) {
    set(date);
    return *this;
}

//------------------------------------------------------------------------------
Date& Date::operator=(Date&& date) {
    set(date);
    return *this;
}

//------------------------------------------------------------------------------
bool Date::add_days(const int days) {
    if (days) {
        int daym = _date_days(_year, _month);

        if (daym > 0) {
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

                    d = _date_days(y, m);

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

                    daym = _date_days(y, m);

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
    }

    return false;
}

//------------------------------------------------------------------------------
bool Date::add_months(const int months) {
    if (months) {
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

        const int days = _date_days(y, m);

        if (days > 0) {
            _year  = y;
            _month = m;

            if (_day > days) {
                _day = days;
            }

            return true;
        }
    }

    return false;
}

//------------------------------------------------------------------------------
bool Date::add_seconds(const int64_t seconds) {
    if (seconds) {
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

    return false;
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
bool Date::Compare(const Date& a, const Date& b) {
    return a.compare(b) < 0;
}

//------------------------------------------------------------------------------
Date& Date::day(int day) {
    if (day > 0 && day <= _date_days(_year, _month)) {
        _day = day;
    }

    return *this;
}

//------------------------------------------------------------------------------
Date& Date::day_last() {
    _day = month_days();
    return *this;
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

    if (format == Date::FORMAT::ISO_LONG) {
        n = snprintf(tmp, 100, "%04d-%02d-%02d", _year, _month, _day);
    }
    else if (format == Date::FORMAT::US) {
        n = snprintf(tmp, 100, "%d/%d/%04d", _month, _day, _year);
    }
    else if (format == Date::FORMAT::WORLD) {
        n = snprintf(tmp, 100, "%d/%d/%04d", _day, _month, _year);
    }
    else if (format == Date::FORMAT::NAME) {
        n = snprintf(tmp, 100, "%04d %s %d", _year, month_name_short(), _day);
    }
    else if (format == Date::FORMAT::NAME_LONG) {
        n = snprintf(tmp, 100, "%04d %s %d", _year, month_name(), _day);
    }
    else if (format == Date::FORMAT::NAME_TIME) {
        n = snprintf(tmp, 100, "%04d %s %d - %02d%02d%02d", _year, month_name_short(), _day, _hour, _min, _sec);
    }
    else if (format == Date::FORMAT::NAME_TIME_LONG) {
        n = snprintf(tmp, 100, "%04d %s %d - %02d:%02d:%02d", _year, month_name(), _day, _hour, _min, _sec);
    }
    else if (format == Date::FORMAT::YEAR_MONTH) {
        n = snprintf(tmp, 100, "%04d %s", _year, month_name_short());
    }
    else if (format == Date::FORMAT::YEAR_MONTH_LONG) {
        n = snprintf(tmp, 100, "%04d %s", _year, month_name());
    }
    else if (format == Date::FORMAT::ISO_TIME) {
        n = snprintf(tmp, 100, "%04d%02d%02d %02d%02d%02d", _year, _month, _day, _hour, _min, _sec);
    }
    else if (format == Date::FORMAT::ISO_TIME_LONG) {
        n = snprintf(tmp, 100, "%04d-%02d-%02d %02d:%02d:%02d", _year, _month, _day, _hour, _min, _sec);
    }
    else if (format == Date::FORMAT::TIME_LONG) {
        n = snprintf(tmp, 100, "%02d:%02d:%02d", _hour, _min, _sec);
    }
    else if (format == Date::FORMAT::TIME) {
        n = snprintf(tmp, 100, "%02d%02d%02d", _hour, _min, _sec);
    }
    else { // Date::FORMAT::ISO
        n = snprintf(tmp, 100, "%04d%02d%02d", _year, _month, _day);
    }

    if (n < 0 || n >= 100) {
        *tmp = 0;
    }

    return tmp;
}

//------------------------------------------------------------------------------
std::string Date::FormatSecToISO(int64_t seconds, bool utc) {
    const time_t rawtime  = (time_t) seconds;
    const tm*    timeinfo = (utc == true) ? gmtime(&rawtime) : localtime(&rawtime);
    char         buffer[100];

    if (timeinfo == nullptr) {
        return "";
    }

    snprintf(buffer, 100, "%04d-%02d-%02d %02d:%02d:%02d", timeinfo->tm_year + 1900, timeinfo->tm_mon + 1, timeinfo->tm_mday, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
    return buffer;
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
// Invalid date returns a Date set to (0, 0, 0)
//
Date Date::FromString(const char* buffer, bool us) {
    if (buffer == nullptr) {
        return Date::InvalidDate();
    }
    else {
         return _date_parse(buffer, us);
    }
}

//------------------------------------------------------------------------------
Date Date::FromTime(int64_t seconds, bool utc) {
    int y, m, d, ho, mi, se;
    _date_from_time(seconds, utc, y, m, d, ho, mi, se);
    return Date(y, m, d, ho, mi, se);
}

//------------------------------------------------------------------------------
Date& Date::hour(int hour) {
    if (hour >= 0 && hour <= 23) {
        _hour = hour;
    }

    return *this;
}

//------------------------------------------------------------------------------
Date Date::InvalidDate() {
    Date date;
    date._year = date._month = date._day = date._hour = date._min = date._sec = 0;
    return date;
}

//------------------------------------------------------------------------------
bool Date::is_leapyear() const {
    return _date_is_leapyear(_year);
}

//------------------------------------------------------------------------------
Date& Date::minute(int min) {
    if (min >= 0 && min <= 59) {
        _min = min;
    }

    return *this;
}

//------------------------------------------------------------------------------
Date& Date::month(int month) {
    if (month >= 1 && month <= 12) {
        _month = month;
    }

    return *this;
}

//------------------------------------------------------------------------------
int Date::month_days() const {
    return _date_days(_year, _month);
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
void Date::print() const {
    auto string = format(Date::FORMAT::ISO_TIME_LONG);
    printf("Date| %s\n", string.c_str());
    fflush(stdout);
}

//------------------------------------------------------------------------------
Date& Date::second(int sec) {
    if (sec >= 0 && sec <= 59) {
        _sec = sec;
    }

    return *this;
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
Date& Date::set(int year, int month, int day, int hour, int min, int sec) {
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
        auto yday  = yearday();

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
Date& Date::weekday(Date::DAY day) {
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
const char* Date::weekday_name() const {
    return _DATE_WEEKDAYS[(int) _date_weekday(_year, _month, _day)];
}

//------------------------------------------------------------------------------
const char* Date::weekday_name_short() const {
    return _DATE_WEEKDAYS_SHORT[(int) _date_weekday(_year, _month, _day)];
}

//------------------------------------------------------------------------------
Date& Date::year(int year) {
    if (year >= 1 && year <= 9999) {
        _year = year;
    }

    return *this;
}

//------------------------------------------------------------------------------
int Date::yearday() const {
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

} // flw

// MKALGAM_OFF
