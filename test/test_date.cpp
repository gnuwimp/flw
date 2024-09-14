// Copyright gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#include "flw.h"

#ifndef FLW_AMALGAM
    #include "date.h"
#endif

#include <assert.h>
#include <string.h>

using namespace flw;

//#define FLW_ASSERT(X,Y) assert((Y) == (X));

void error() {
    fprintf(stderr, "error()\n"); fflush(stderr);

    Date date1(1, 1, 1);

    { // Failure
        date1 = Date::FromString("20030229", false);
        FLW_ASSERT(1, date1.is_invalid())
        FLW_ASSERT("00000000", date1.format().c_str())

        date1 = Date::FromString("20o30229", false);
        FLW_ASSERT("00000000", date1.format().c_str())

        date1 = Date::FromString("20030 29", false);
        FLW_ASSERT("00000000", date1.format().c_str())

        date1 = Date::FromString("20030232", false);
        FLW_ASSERT("00000000", date1.format().c_str())

        date1 = Date::FromString("20031313", false);
        FLW_ASSERT("00000000", date1.format().c_str())

        date1 = Date::FromString("20040229 0102O3", false);
        FLW_ASSERT("00000000", date1.format().c_str())

        date1 = Date::FromString("20040229 310203", false);
        FLW_ASSERT("00000000", date1.format().c_str())

        date1 = Date::FromString("20040229 210260", false);
        FLW_ASSERT("00000000", date1.format().c_str())

        date1 = Date::FromString("20040229 210259", false);
        FLW_ASSERT("20040229", date1.format().c_str())

        date1.set(2000, 1, 32);
        date1.set(2000, 1, 0);
        date1.set(2000, 0, 2);
        date1.set(2000, 13, 2);
        date1.set(0, 13, 2);
        date1.set(10000, 13, 2);
        FLW_ASSERT("20040229", date1.format().c_str())

        date1.year(2000);
        date1.year(0);
        date1.year(10000);
        date1.month(2);
        date1.month(0);
        date1.month(13);
        date1.day(29);
        date1.day(0);
        date1.day(32);
        date1.hour(6);
        date1.hour(24);
        date1.minute(33);
        date1.minute(60);
        date1.second(45);
        date1.second(60);

        FLW_ASSERT(2000, date1.year())
        FLW_ASSERT(2, date1.month())
            FLW_ASSERT(29, date1.day())
        FLW_ASSERT(6, date1.hour())
        FLW_ASSERT(33, date1.minute())
        FLW_ASSERT(45, date1.second())
    }
}

void basic() {
    fprintf(stderr, "basic()\n"); fflush(stderr);

    Date date(1, 1, 1);

    { // ISO format
        date = Date::FromString("20040229", false);
        FLW_ASSERT("20040229", date.format().c_str())

        date = Date::FromString("1801-12-31", false);
        FLW_ASSERT("18011231", date.format().c_str())

        date = Date::FromString("20120330", false);
        FLW_ASSERT("20120330", date.format().c_str())
    }

    { // Test for parse bug
        date = Date::FromString("20121015", false);
        FLW_ASSERT("20121015", date.format().c_str())
    }

    { // ISO format + time
        date = Date::FromString("20040229 010203", false);
        FLW_ASSERT("20040229 010203", date.format(Date::FORMAT::ISO_TIME).c_str())

        date = Date::FromString("1801-12-31 23:59:59", false);
        FLW_ASSERT("1801-12-31 23:59:59", date.format(Date::FORMAT::ISO_TIME_LONG).c_str())

        date = Date::FromString("2111-12-18 12:13:14", false);
        FLW_ASSERT("2111-12-18 12:13:14", date.format(Date::FORMAT::ISO_TIME_LONG).c_str())
    }

    { // World format
        date = Date::FromString("1/2/2020", false);
        FLW_ASSERT("20200201", date.format().c_str())

        date = Date::FromString("2/12/2020", false);
        FLW_ASSERT("20201202", date.format().c_str())

        date = Date::FromString("24/3/2020", false);
        FLW_ASSERT("20200324", date.format().c_str())

        date = Date::FromString("25/12/2020", false);
        FLW_ASSERT("20201225", date.format().c_str())
    }

    { // US format
        date = Date::FromString("1/2/2020", true);
        FLW_ASSERT("20200102", date.format().c_str())

        date = Date::FromString("2/12/2020", true);
        FLW_ASSERT("20200212", date.format().c_str())

        date = Date::FromString("12/3/2020", true);
        FLW_ASSERT("20201203", date.format().c_str())

        date = Date::FromString("11/13/2020", true);
        FLW_ASSERT("20201113", date.format().c_str())
    }

    { // Format
        date = Date::FromString("24/12/1999 111213", false);
        FLW_ASSERT("19991224", date.format(Date::FORMAT::ISO).c_str())
        FLW_ASSERT("1999-12-24", date.format(Date::FORMAT::ISO_LONG).c_str())
        FLW_ASSERT("19991224 111213", date.format(Date::FORMAT::ISO_TIME).c_str())
        FLW_ASSERT("1999-12-24 11:12:13", date.format(Date::FORMAT::ISO_TIME_LONG).c_str())
        FLW_ASSERT("12/24/1999", date.format(Date::FORMAT::US).c_str())
        FLW_ASSERT("24/12/1999", date.format(Date::FORMAT::WORLD).c_str())
        FLW_ASSERT("1999 Dec 24", date.format(Date::FORMAT::NAME).c_str())
        FLW_ASSERT("1999 December 24", date.format(Date::FORMAT::NAME_LONG).c_str())
        FLW_ASSERT("1999 Dec", date.format(Date::FORMAT::YEAR_MONTH).c_str())
        FLW_ASSERT("1999 December", date.format(Date::FORMAT::YEAR_MONTH_LONG).c_str())
    }
}

void compare() {
    fprintf(stderr, "compare()\n"); fflush(stderr);

    Date date1;
    Date date2(date1);
    Date date3;

    { // Compare date
        FLW_ASSERT(true, date1 == date2)
        FLW_ASSERT(true, date1 <= date2)
        FLW_ASSERT(true, date1 >= date2)

        date1.month(1);
        date2.month(3);
        FLW_ASSERT(true, date1 != date2)
        FLW_ASSERT(true, date1 < date2)
        FLW_ASSERT(true, date2 > date1)

        date2 = date1;
        FLW_ASSERT(true, date1 == date2)
    }

    { // Compare date and time
        date1.set(2076, 8, 23, 10, 20, 30);
        date2.set(2076, 8, 23, 10, 20, 30);
        date3.set(2076, 8, 23, 10, 20, 31);
        FLW_ASSERT(true, date1 == date2)
        FLW_ASSERT(true, date1 <= date2)
        FLW_ASSERT(true, date1 >= date2)
    }

    { // Compare month
        date1.set(2076, 8, 23, 10, 20, 30);
        date2.set(2076, 8, 24, 10, 20, 30);
        FLW_ASSERT(true, date1 != date2)
        FLW_ASSERT(true, date1.compare(date2, Date::COMPARE::YYYYMM) == 0)
    }
}

void set() {
    fprintf(stderr, "set()\n"); fflush(stderr);

    Date date1;
    Date date2(date1);
    Date date3;

    { // Set/get
        date1 = Date::FromTime(0);
        FLW_ASSERT(1970, date1.year())
        FLW_ASSERT(1, date1.month())
        FLW_ASSERT(1, date1.day())
        FLW_ASSERT(0, date1.time())

        date1 = Date::FromTime(18 * 3600 + 60);
        FLW_ASSERT(1970, date1.year())
        FLW_ASSERT(1, date1.month())
        FLW_ASSERT(1, date1.day())
        // FLW_ASSERT(19, date1.hour()) // For UTC+
        FLW_ASSERT(1, date1.minute())
        FLW_ASSERT(0, date1.second())
        FLW_ASSERT(18 * 3600 + 60, date1.time())
    }

    { // Add/goto
        date2.set(2000, 2, 29);

        FLW_ASSERT(false, date2.add_days(-3000 * 365))
        FLW_ASSERT(false, date2.add_months(-3000 * 12))

        date2.add_days(-7);
        FLW_ASSERT("2000-02-22", date2.format(Date::FORMAT::ISO_LONG).c_str())

        date2.add_days(14);
        FLW_ASSERT("2000-03-07", date2.format(Date::FORMAT::ISO_LONG).c_str())

        FLW_ASSERT(true, date2.add_months(-1))
        FLW_ASSERT("2000-02-07", date2.format(Date::FORMAT::ISO_LONG).c_str())

        FLW_ASSERT(true, date2.add_months(12))
        FLW_ASSERT("2001-02-07", date2.format(Date::FORMAT::ISO_LONG).c_str())

        date1.set(1999, 12, 24);
        date1.weekday(Date::DAY::SUNDAY);
        FLW_ASSERT(1999, date1.year())
        FLW_ASSERT(12, date1.month())
        FLW_ASSERT(26, date1.day())

        date1.weekday(Date::DAY::MONDAY);
        FLW_ASSERT(1999, date1.year())
        FLW_ASSERT(12, date1.month())
        FLW_ASSERT(20, date1.day())

        FLW_ASSERT(true, date1.add_seconds(4.5 * Date::SECS_PER_DAY))
        FLW_ASSERT("1999-12-24 12:00:00", date1.format(Date::FORMAT::ISO_TIME_LONG).c_str())

        FLW_ASSERT(true, date1.add_seconds(-Date::SECS_PER_HOUR))
        FLW_ASSERT("1999-12-24 11:00:00", date1.format(Date::FORMAT::ISO_TIME_LONG).c_str())

        date1 = Date::FromString("20201016 180000", false);
        FLW_ASSERT("2020-10-16 18:00:00", date1.format(Date::FORMAT::ISO_TIME_LONG).c_str())
        FLW_ASSERT(true, date1.add_seconds(Date::SECS_PER_HOUR))
        FLW_ASSERT("2020-10-16 19:00:00", date1.format(Date::FORMAT::ISO_TIME_LONG).c_str())

        date1 = Date::FromString("9999-12-31 23:00:00", false);
        FLW_ASSERT("9999-12-31 23:00:00", date1.format(Date::FORMAT::ISO_TIME_LONG).c_str())
        FLW_ASSERT(true, date1.add_seconds(1800))
        FLW_ASSERT("9999-12-31 23:30:00", date1.format(Date::FORMAT::ISO_TIME_LONG).c_str())
        FLW_ASSERT(true, date1.add_seconds(1799))
        FLW_ASSERT("9999-12-31 23:59:59", date1.format(Date::FORMAT::ISO_TIME_LONG).c_str())
        FLW_ASSERT(false, date1.add_seconds(1))
        FLW_ASSERT("9999-12-31 23:59:59", date1.format(Date::FORMAT::ISO_TIME_LONG).c_str())

        date1 = Date::FromString("0001-01-01 01:00:00", false);
        FLW_ASSERT("0001-01-01 01:00:00", date1.format(Date::FORMAT::ISO_TIME_LONG).c_str())
        FLW_ASSERT(true, date1.add_seconds(-1800))
        FLW_ASSERT("0001-01-01 00:30:00", date1.format(Date::FORMAT::ISO_TIME_LONG).c_str())
        FLW_ASSERT(true, date1.add_seconds(-1800))
        FLW_ASSERT("0001-01-01 00:00:00", date1.format(Date::FORMAT::ISO_TIME_LONG).c_str())
        FLW_ASSERT(false, date1.add_seconds(-1))
        FLW_ASSERT("0001-01-01 00:00:00", date1.format(Date::FORMAT::ISO_TIME_LONG).c_str())
    }

    { // Diff
        date1.set(2000, 1, 1);
        date2.set(2001, 1, 1);
        date3.set(2001, 1, 2);

        FLW_ASSERT(12, date1.diff_months(date2))
        FLW_ASSERT(-12, date2.diff_months(date1))
        FLW_ASSERT(0, date2.diff_months(date2))

        FLW_ASSERT(366, date1.diff_days(date2))
        FLW_ASSERT(-366, date2.diff_days(date1))
        FLW_ASSERT(0, date2.diff_days(date2))

        FLW_ASSERT(Date::SECS_PER_DAY, date2.diff_seconds(date3))
        FLW_ASSERT(-Date::SECS_PER_DAY, date3.diff_seconds(date2))
    }

    { // Weeks
        date1.set(2004, 2, 29);
        FLW_ASSERT(9, date1.week())
        FLW_ASSERT((int) Date::DAY::SUNDAY, (int) date1.weekday())

        date1.set(2015, 12, 21);
        FLW_ASSERT(52, date1.week())
        FLW_ASSERT((int) Date::DAY::MONDAY, (int) date1.weekday())

        date1.set(2015, 12, 28);
        FLW_ASSERT(53, date1.week())
        FLW_ASSERT((int) Date::DAY::MONDAY, (int) date1.weekday())

        date1.set(2016, 1, 5);
        FLW_ASSERT(1, date1.week())
        FLW_ASSERT((int) Date::DAY::TUESDAY, (int) date1.weekday())
    }

    {
        date1.set(2000, 02, 29);

        FLW_ASSERT(60, date1.yearday())
        FLW_ASSERT(true, date1.is_leapyear())
    }
}

int main() {
    {
        Date today;

        fprintf(stderr, "today is %s\n", today.format(Date::FORMAT::ISO_TIME_LONG).c_str());
        error();
        basic();
        compare();
        set();
    }

//    flw::Date d = true;
    flw::Date d(true);
    printf("d(true)=%s\n", d.format().c_str());

    printf("Test Finished\n");
    return 0;
}
