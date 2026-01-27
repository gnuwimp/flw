// Copyright gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#include "flw.h"

FL_EXPORT bool fl_disable_wayland = false;

#ifndef FLW_AMALGAM
    #include "chart.h"
    #include "dlg.h"
    #include "theme.h"
    #include "json.h"
#endif

#include <assert.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl.H>
#include <FL/Fl_Preferences.H>

using namespace flw;

#define TEST_RANGE_DAY          "All days (DateRange::DAY)"
#define TEST_RANGE_WEEKDAY      "Weekdays (RANGE::WEEKDAY)"
#define TEST_RANGE_FRIDAY       "Fridays (RANGE::FRIDAY)"
#define TEST_RANGE_SUNDAY       "Sundays (RANGE::SUNDAY)"
#define TEST_RANGE_MONTH        "Months (RANGE::MONTH)"
#define TEST_RANGE_HOUR         "Hour (DateRange::HOUR)"
#define TEST_RANGE_MIN          "Minutes (DateRange::MIN)"
#define TEST_RANGE_SEC          "Seconds (DateRange::SEC)"

#define TEST_SMALL_VALUE1       "Small values 1"
#define TEST_SMALL_VALUE2       "Small values 2"
#define TEST_SMALL_VALUE3       "Small values 3"

#define TEST_LARGE_VALUE1       "Large values 1"
#define TEST_LARGE_VALUE2       "Large values 2"
#define TEST_LARGE_VALUE3       "Large dataset"

#define TEST_RIGHT_10           "Right 10 lines"
#define TEST_RIGHT_NEG          "Right negative values"
#define TEST_RIGHT_SMALL        "Right small"
#define TEST_RIGHT_STEP         "Right + block"
#define TEST_RIGHT_LEFT         "Right and left"

#define TEST_REF_ALL            "Ref all"
#define TEST_REF_ALL_BLOCK      "Ref all + block"
#define TEST_REF_FRIDAY         "Ref friday"
#define TEST_REF_SUNDAY         "Ref sunday"
#define TEST_REF_MONTH          "Ref month"

#define TEST_AREA2              "2 chart areas"
#define TEST_AREA3              "3 chart areas"
#define TEST_AREA4              "4 chart areas"
#define TEST_AREA5              "5 chart areas"

#define TEST_EMPTY              "Empty"
#define TEST_FIRST              TEST_RANGE_DAY
#define TEST_LAST               TEST_EMPTY

std::string TEST = TEST_FIRST;
int         RAND = 0;

//------------------------------------------------------------------------------
chart::PointVector create_serie1(const char* start, const char* stop, double value, double change, const chart::DateRange range, double divide = 0.0) {
    auto close = value;
    auto ch    = 0.0;
    auto date1 = gnu::Date(start);
    auto date2 = gnu::Date(stop);
    auto res   = chart::PointVector();

    while (date1 <= date2) {
        double high = close * 1.02;
        double low  = close * 0.98;

        if (rand() % 3 == 0) {
            high = close * 1.05;
            low  = close * 0.95;
        }
        else if (rand() % 5 == 0) {
            high = close * 1.08;
            low  = close * 0.92;
        }

        if (high < low) {
            double t = high;
            high = low;
            low = t;
        }

        if (divide > 1.0) {
            res.push_back(chart::Point(date1.format(gnu::Date::Format::ISO_TIME), high / divide, low / divide, close / divide));
        }
        else {
            res.push_back(chart::Point(date1.format(gnu::Date::Format::ISO_TIME), high, low, close));
        }

        if (range == chart::DateRange::HOUR) {
            date1.add_seconds(gnu::Date::SECS_PER_HOUR);
        }
        else if (range == chart::DateRange::MIN) {
            date1.add_seconds(60);
        }
        else if (range == chart::DateRange::SEC) {
            date1.add_seconds(1);
        }
        else {
            date1.add_days(1);
        }


        if (rand() % 2 == 0) {
            ch = 1.0 + change / (double) (rand() % 100 + 100.0);
        }
        else {
            ch = 1.0 - change / (double) (rand() % 100 + 100.0);
        }

        close *= ch;
    }

    return res;
}

//------------------------------------------------------------------------------
chart::PointVector create_serie2(const char* start, const char* stop, double num = 1.0, double add = 0.0) {
    auto res   = chart::PointVector();
    auto f     = 1.0;
    auto date1 = gnu::Date(start);
    auto date2 = gnu::Date(stop);

    while (date1 <= date2) {
        res.push_back(chart::Point(date1.format(gnu::Date::Format::ISO_TIME), sinl(f) + num));
        f += num;
        num += add;
        date1.add_days(1);
    }

    return res;
}

//------------------------------------------------------------------------------
void test1(
    chart::Chart* chart,
    std::string main_label,
    std::string label,
    const double start,
    const double change,
    const chart::DateRange range,
    const chart::LineType type,
    Fl_Align align,
    const int tick,
    const int width,
    double min = INFINITY,
    double max = INFINITY,
    double divide = 0.0) {

    auto vec1  = create_serie1("20010101", "20191231", start, change, range, divide);
    auto line1 = chart::Line(vec1, label, type);

    line1.set_align(align).set_color(color::ROYALBLUE).set_width(width);
    chart->area(chart::AreaNum::ONE).add_line(line1);
    chart->area(chart::AreaNum::ONE).set_min_clamp(min);
    chart->area(chart::AreaNum::ONE).set_max_clamp(max);
    chart->set_date_range(range);
    chart->set_tick_width(tick);
    chart->set_main_label(main_label);
    chart->init_new_data();
}

//------------------------------------------------------------------------------
void test2(chart::Chart* chart) {
    auto vec1  = create_serie1("20010101", "20091231", -2000, 3, chart::DateRange::DAY);
    auto vec2  = create_serie1("20010201", "20091130", -4000, 5, chart::DateRange::DAY);
    auto vec3  = chart::Point::Fixed(vec1, 0);
    vec1 = chart::Point::Modify(vec1, chart::Modifier::ADDITION, 1000);
    vec2 = chart::Point::Modify(vec2, chart::Modifier::ADDITION, 2000);
    auto line1 = chart::Line(vec1, "LineType::LINE");
    auto line2 = chart::Line(vec2, "LineType::BAR_HLC", chart::LineType::BAR_HLC);
    auto line3 = chart::Line(vec3, "LineType::EXPAND_HORIZONTAL_FIRST", chart::LineType::EXPAND_HORIZONTAL_FIRST);


    line1.set_color(color::ROYALBLUE).set_width(2);
    line2.set_align(FL_ALIGN_RIGHT).set_color(color::CRIMSON).set_width(2);
    line3.set_color(FL_DARK_GREEN).set_width(3);

    chart->area(chart::AreaNum::ONE).add_line(line1);
    chart->area(chart::AreaNum::ONE).add_line(line2);
    chart->area(chart::AreaNum::ONE).add_line(line3);
    chart->set_date_range(chart::DateRange::DAY);
    chart->set_hor_lines(false);
    chart->set_tick_width(10);
    chart->init_new_data();
}

//------------------------------------------------------------------------------
void test3(chart::Chart* chart, std::string label, const chart::DateRange range, bool block = false) {
    auto vec1 = chart::PointVector();
    auto vec2 = chart::PointVector();
    auto vec3 = chart::PointVector();

    vec1.push_back(chart::Point("20140102", 10)); // Thu    *
    vec1.push_back(chart::Point("20140105", 15)); // Sun    *
    vec1.push_back(chart::Point("20140111", 20)); // Sat    *
    vec1.push_back(chart::Point("20140113", 30)); // Mon |  *
    vec1.push_back(chart::Point("20140116", 25)); // Thu |  *
    vec1.push_back(chart::Point("20140124", 35)); // Fri |  *
    vec1.push_back(chart::Point("20140126", 40)); // Sun    *
    vec1.push_back(chart::Point("20140204", 45)); // Tue
    vec1.push_back(chart::Point("20140318", 50)); // Tue |  *
    vec1.push_back(chart::Point("20140319", 55)); // Wen |  *
    vec1.push_back(chart::Point("20140505", 60)); // Mon
    vec1.push_back(chart::Point("20140510", 30)); // Sat

    if (block == true) {
        vec3.push_back(chart::Point("20140105"));
        vec3.push_back(chart::Point("20140111"));
        vec3.push_back(chart::Point("20140319"));
        chart->set_main_label("Dates 01/05, 01/11, 03/19 has been blocked");
    }

    if (range == chart::DateRange::FRIDAY) {
        vec2 = chart::Point::DayToWeek(vec1, gnu::Date::Day::FRIDAY);
        chart->set_tick_width(25);
    }
    else if (range == chart::DateRange::SUNDAY) {
        vec2 = chart::Point::DayToWeek(vec1, gnu::Date::Day::SUNDAY);
        chart->set_tick_width(25);
    }
    else if (range == chart::DateRange::MONTH) {
        vec2 = chart::Point::DayToMonth(vec1);
        chart->set_tick_width(chart::MAX_TICK);
    }
    else {
        vec2 = vec1;
        chart->set_tick_width(8);
    }

    auto line1 = chart::Line(vec2, label, chart::LineType::HORIZONTAL);
    line1.set_color(color::ROYALBLUE).set_width(8);

    chart->area(chart::AreaNum::ONE).add_line(line1);
    chart->set_date_range(range);
    chart->set_block_dates(vec3);
    chart->init_new_data();
}

//------------------------------------------------------------------------------
void test4(chart::Chart* chart) {
    auto vec1  = create_serie1("18000101", "20191231", 1000.0, 0.5, chart::DateRange::DAY);
    auto vec2  = create_serie1("18100101", "20291231", 1000.0, 0.5, chart::DateRange::DAY);
    auto vec3  = create_serie1("18200101", "19991231", 1000.0, 0.5, chart::DateRange::DAY);
    auto line1 = chart::Line(vec1);
    auto line2 = chart::Line(vec2);
    auto line3 = chart::Line(vec3);

    line1.set_label("18000101 - 20191231").set_type(chart::LineType::LINE).set_color(color::ROYALBLUE).set_width(1);
    line2.set_label("18100101 - 20291231").set_type(chart::LineType::LINE).set_color(FL_GREEN).set_width(1);
    line3.set_label("18200101 - 19991231").set_type(chart::LineType::LINE).set_color(color::CRIMSON).set_width(1);

    chart->area(chart::AreaNum::ONE).add_line(line1);
    chart->area(chart::AreaNum::ONE).add_line(line2);
    chart->area(chart::AreaNum::ONE).add_line(line3);
    chart->set_date_range(chart::DateRange::DAY);
    chart->set_tick_width(3);
    chart->init_new_data();
}

//------------------------------------------------------------------------------
void test5(chart::Chart* chart, std::string main_label, const int count) {
    auto vec1  = create_serie1("20010101", "20091231", 1000.0, 2.0, chart::DateRange::DAY);
    auto vec2  = create_serie1("20010201", "20091130", 1500000.0, 2.0, chart::DateRange::DAY);
    auto vec3  = create_serie1("20010101", "20091231", 3000000, 5.0, chart::DateRange::DAY);
    auto vec4  = chart::Point::MovingAverage(vec3, 20);
    auto line1 = chart::Line(vec1, "LineType::BAR_HLC", chart::LineType::BAR_HLC);
    auto line2 = chart::Line(vec2, "LineType::LINE");
    auto line3 = chart::Line(vec3, "LineType::BAR_CLAMP", chart::LineType::BAR_CLAMP);
    auto line4 = chart::Line(vec4, "Moving Average");

    line1.set_color(color::ROYALBLUE).set_width(3);
    line2.set_color(color::CRIMSON).set_width(3);
    line3.set_color(color::OLIVE).set_width(3);
    line4.set_color(color::VIOLET).set_width(3);

    if (count == 2) {
        line2.set_align(FL_ALIGN_RIGHT);
        line3.set_width(0);

        chart->set_area_size(70, 30, 0, 0, 0);
        chart->area(chart::AreaNum::ONE).add_line(line1);
        chart->area(chart::AreaNum::ONE).add_line(line2);
        chart->area(chart::AreaNum::TWO).add_line(line3);
        chart->area(chart::AreaNum::TWO).add_line(line4);
        chart->area(chart::AreaNum::TWO).set_min_clamp(0);
    }
    else if (count == 3) {
        chart->set_area_size(40, 40, 20);
        chart->area(chart::AreaNum::ONE).add_line(line1);
        chart->area(chart::AreaNum::TWO).add_line(line2);
        chart->area(chart::AreaNum::THREE).add_line(line3);
        chart->area(chart::AreaNum::THREE).add_line(line4);
        chart->area(chart::AreaNum::THREE).set_min_clamp(0);
    }
    else if (count == 4) {
        chart->set_area_size(30, 30, 20, 20);
        chart->area(chart::AreaNum::ONE).add_line(line1);
        chart->area(chart::AreaNum::TWO).add_line(line2);
        chart->area(chart::AreaNum::THREE).add_line(line3);
        chart->area(chart::AreaNum::FOUR).add_line(line4);
        chart->area(chart::AreaNum::FOUR).set_min_clamp(0);
    }
    else if (count == 5) {
        line4.set_align(FL_ALIGN_RIGHT);
        chart->set_area_size(20, 20, 20, 20, 20);
        chart->area(chart::AreaNum::ONE).add_line(line1);
        chart->area(chart::AreaNum::TWO).add_line(line2);
        chart->area(chart::AreaNum::THREE).add_line(line3);
        chart->area(chart::AreaNum::FOUR).add_line(line1);
        chart->area(chart::AreaNum::FIVE).add_line(line4);
        chart->area(chart::AreaNum::FIVE).set_min_clamp(0);
    }

    chart->set_hor_lines(false);
    chart->set_date_range(chart::DateRange::DAY);
    chart->set_tick_width(4);
    chart->set_main_label(main_label);
    chart->init_new_data();
}

//------------------------------------------------------------------------------
void test6(chart::Chart* chart, const chart::DateRange range) {
    auto vec1  = create_serie1("20200912 000000", "20200916 230000", 100.0, 0.5, range);
    auto line1 = chart::Line(vec1);

    if (range == chart::DateRange::HOUR) {
        line1.set_label("DateRange::HOUR");
    }
    else if (range == chart::DateRange::MIN) {
        line1.set_label("DateRange::MIN");
    }
    else if (range == chart::DateRange::SEC) {
        line1.set_label("DateRange::SEC");
    }

    line1.set_type(chart::LineType::LINE).set_color(color::ROYALBLUE).set_width(4);

    chart->area(chart::AreaNum::ONE).add_line(line1);
    chart->set_date_range(range);
    chart->set_tick_width(25);
    chart->init_new_data();
}

//------------------------------------------------------------------------------
void test7(chart::Chart* chart) {
    auto vec1  = chart::PointVector();
    auto date1 = gnu::Date("20010101");
    auto date2 = gnu::Date("20021231");
    auto p     = -3.5;

    while (date1 <= date2) {
        vec1.push_back(chart::Point(date1.format(gnu::Date::Format::ISO).c_str(), p));
        date1.add_days(1);

        if (rand() % 3 == 1) {
            p -= rand() % 5 / 300.0;
        }
        else {
            p += rand() % 5 / 100.0;
        }
    }

    auto line1 = chart::Line(vec1, "LineType::LINE_DOT)", chart::LineType::LINE_DOT);
    line1.set_align(FL_ALIGN_RIGHT).set_color(color::ROYALBLUE).set_width(2);

    chart->area(chart::AreaNum::ONE).add_line(line1);
    chart->set_date_range(chart::DateRange::DAY);
    chart->set_tick_width(6);
    chart->init_new_data();
}

//------------------------------------------------------------------------------
void test8(chart::Chart* chart) {
    auto vec1  = create_serie1("20010101", "20011231", 10.0, 1.0, chart::DateRange::DAY);
    auto start = 1.0;
    auto even  = 0;

    for (auto& p : vec1) {
        p.close = start;
        p.high  = start;
        p.low   = start;

        if (even % 2 == 0) {
            if (rand() % 3 == 0) {
                start  -= 2.0;
            }
            else {
                start  += 2.0;
            }
        }

        even++;
    }

    auto line1 = chart::Line(vec1, "LineType::LINE + blocked dates", chart::LineType::LINE).set_color(color::ROYALBLUE).set_width(4).set_align(FL_ALIGN_RIGHT);
    auto vec2  = chart::PointVector();

    vec2.push_back(chart::Point("20010128"));
    vec2.push_back(chart::Point("20010129"));
    vec2.push_back(chart::Point("20010201"));
    vec2.push_back(chart::Point("20010202"));
    vec2.push_back(chart::Point("20010205"));
    vec2.push_back(chart::Point("20010206"));

    chart->area(chart::AreaNum::ONE).add_line(line1);
    chart->set_date_range(chart::DateRange::DAY);
    chart->set_tick_width(25);
    chart->set_block_dates(vec2);
    chart->init_new_data();
}

//------------------------------------------------------------------------------
void test9(chart::Chart* chart) {
    auto vec1   = create_serie2("20010101", "20021231");
    auto vec2   = create_serie2("20010101", "20021231", 2.0);
    auto vec3   = create_serie2("20010101", "20021231", 3.0);
    auto vec4   = create_serie2("20010101", "20021231", 4.0);
    auto vec5   = create_serie2("20010101", "20021231", 5.0);
    auto vec6   = create_serie2("20010101", "20021231", 6.0);
    auto vec7   = create_serie2("20010101", "20021231", 7.0);
    auto vec8   = create_serie2("20010101", "20021231", 8.0);
    auto vec9   = create_serie2("20010101", "20021231", 9.0);
    auto vec10  = create_serie2("20010101", "20021231", 10.0);
    auto line1  = chart::Line(vec1, "One", chart::LineType::LINE_DOT);
    auto line2  = chart::Line(vec2, "Two");
    auto line3  = chart::Line(vec3, "Three");
    auto line4  = chart::Line(vec4, "Four", chart::LineType::LINE_DOT);
    auto line5  = chart::Line(vec5, "Five");
    auto line6  = chart::Line(vec6, "Six");
    auto line7  = chart::Line(vec7, "Seven");
    auto line8  = chart::Line(vec8, "Eight", chart::LineType::LINE_DOT);
    auto line9  = chart::Line(vec9, "Nine");
    auto line10 = chart::Line(vec10, "Ten");
    auto line11 = chart::Line(chart::Line(chart::PointVector(), "Empty"));

    line1.set_align(FL_ALIGN_RIGHT).set_color(color::ROYALBLUE).set_width(4);
    line2.set_align(FL_ALIGN_RIGHT).set_color(color::CRIMSON).set_width(4);
    line3.set_align(FL_ALIGN_RIGHT).set_color(color::GOLD).set_width(4);
    line4.set_align(FL_ALIGN_RIGHT).set_color(color::TURQUOISE).set_width(4);
    line5.set_align(FL_ALIGN_RIGHT).set_color(color::VIOLET).set_width(4);
    line6.set_align(FL_ALIGN_RIGHT).set_color(color::PINK).set_width(2);
    line7.set_align(FL_ALIGN_RIGHT).set_color(color::OLIVE).set_width(4);
    line8.set_align(FL_ALIGN_RIGHT).set_color(color::CHOCOLATE).set_width(4);
    line9.set_align(FL_ALIGN_RIGHT).set_color(color::PINK).set_width(4);
    line10.set_align(FL_ALIGN_RIGHT).set_color(color::TEAL).set_width(4);
    line11.set_align(FL_ALIGN_RIGHT).set_color(color::TEAL).set_width(4);

    chart->area(chart::AreaNum::ONE).add_line(line1);
    chart->area(chart::AreaNum::ONE).add_line(line2);
    chart->area(chart::AreaNum::ONE).add_line(line3);
    chart->area(chart::AreaNum::ONE).add_line(line4);
    chart->area(chart::AreaNum::ONE).add_line(line5);
    chart->area(chart::AreaNum::ONE).add_line(line6);
    chart->area(chart::AreaNum::ONE).add_line(line7);
    chart->area(chart::AreaNum::ONE).add_line(line8);
    chart->area(chart::AreaNum::ONE).add_line(line9);
    assert(chart->area(chart::AreaNum::ONE).add_line(line10) == true);
    assert(chart->area(chart::AreaNum::ONE).add_line(line10) == false);
    chart->area(chart::AreaNum::ONE).add_line(line11);
    chart->area(chart::AreaNum::ONE).set_min_clamp(-1);
    chart->area(chart::AreaNum::ONE).set_max_clamp(12);
    chart->set_date_range(chart::DateRange::DAY);
    chart->set_tick_width(30);
    chart->init_new_data();
}

//------------------------------------------------------------------------------
void test10(chart::Chart* chart) {
    auto vec1   = create_serie2("20010101", "20041231", 321'000'000'000, 0.01);
    auto line1  = chart::Line(vec1, "LineType::LINE_DOT", chart::LineType::LINE_DOT);

    line1.set_align(FL_ALIGN_RIGHT).set_color(FL_BLUE).set_width(4);

    chart->area(chart::AreaNum::ONE).add_line(line1);
    chart->set_date_range(chart::DateRange::DAY);
    chart->set_tick_width(10);
    chart->init_new_data();
}

/*
 *      _______        _
 *     |__   __|      | |
 *        | | ___  ___| |_
 *        | |/ _ \/ __| __|
 *        | |  __/\__ \ |_
 *        |_|\___||___/\__|
 *
 *
 */

//------------------------------------------------------------------------------
class Test : public Fl_Double_Window {
public:
    Fl_Menu_Bar*  menu;
    chart::Chart* chart;

    //--------------------------------------------------------------------------
    Test(int W, int H) : Fl_Double_Window(W, H, "test_chart.cpp") {
        end();
        resizable(this);

        menu  = new Fl_Menu_Bar(0, 0, 0, 0);
        chart = new chart::Chart();

        auto pref = Fl_Preferences(Fl_Preferences::USER, "gnuwimp_test", "test_chart");
        flw::theme::load_theme_from_pref(pref);
        flw::theme::load_win_from_pref(pref, "gui.", this, true);

        add(menu);
        add(chart);

        menu->add("&Test/Autorun",                0, Test::CallbackAuto, this, FL_MENU_DIVIDER);
        menu->add("&Test/save",                   0, Test::CallbackSave, this);
        menu->add("&Test/Load",                   0, Test::CallbackLoad, this, FL_MENU_DIVIDER);

        menu->add("&Test/" TEST_RANGE_DAY,        0, Test::CallbackTest, this);
        menu->add("&Test/" TEST_RANGE_WEEKDAY,    0, Test::CallbackTest, this);
        menu->add("&Test/" TEST_RANGE_FRIDAY,     0, Test::CallbackTest, this);
        menu->add("&Test/" TEST_RANGE_SUNDAY,     0, Test::CallbackTest, this);
        menu->add("&Test/" TEST_RANGE_MONTH,      0, Test::CallbackTest, this);
        menu->add("&Test/" TEST_RANGE_HOUR,       0, Test::CallbackTest, this);
        menu->add("&Test/" TEST_RANGE_MIN,        0, Test::CallbackTest, this);
        menu->add("&Test/" TEST_RANGE_SEC,        0, Test::CallbackTest, this, FL_MENU_DIVIDER);
        menu->add("&Test/" TEST_SMALL_VALUE1,     0, Test::CallbackTest, this);
        menu->add("&Test/" TEST_SMALL_VALUE2,     0, Test::CallbackTest, this);
        menu->add("&Test/" TEST_SMALL_VALUE3,     0, Test::CallbackTest, this, FL_MENU_DIVIDER);
        menu->add("&Test/" TEST_LARGE_VALUE1,     0, Test::CallbackTest, this);
        menu->add("&Test/" TEST_LARGE_VALUE2,     0, Test::CallbackTest, this);
        menu->add("&Test/" TEST_LARGE_VALUE3,     0, Test::CallbackTest, this, FL_MENU_DIVIDER);
        menu->add("&Test/" TEST_RIGHT_10,         0, Test::CallbackTest, this);
        menu->add("&Test/" TEST_RIGHT_NEG,        0, Test::CallbackTest, this);
        menu->add("&Test/" TEST_RIGHT_SMALL,      0, Test::CallbackTest, this);
        menu->add("&Test/" TEST_RIGHT_STEP,       0, Test::CallbackTest, this);
        menu->add("&Test/" TEST_RIGHT_LEFT,       0, Test::CallbackTest, this, FL_MENU_DIVIDER);
        menu->add("&Test/" TEST_REF_ALL,          0, Test::CallbackTest, this);
        menu->add("&Test/" TEST_REF_ALL_BLOCK,    0, Test::CallbackTest, this);
        menu->add("&Test/" TEST_REF_FRIDAY,       0, Test::CallbackTest, this);
        menu->add("&Test/" TEST_REF_SUNDAY,       0, Test::CallbackTest, this);
        menu->add("&Test/" TEST_REF_MONTH,        0, Test::CallbackTest, this, FL_MENU_DIVIDER);
        menu->add("&Test/" TEST_AREA2,            0, Test::CallbackTest, this);
        menu->add("&Test/" TEST_AREA3,            0, Test::CallbackTest, this);
        menu->add("&Test/" TEST_AREA4,            0, Test::CallbackTest, this);
        menu->add("&Test/" TEST_AREA5,            0, Test::CallbackTest, this, FL_MENU_DIVIDER);
        menu->add("&Test/" TEST_EMPTY,            0, Test::CallbackTest, this);

        menu->add("&Settings/srand(time)",        0, Test::CallbackRandTime, this, FL_MENU_RADIO);
        menu->add("&Settings/srand(666)",         0, Test::CallbackRand666, this, FL_MENU_RADIO | FL_MENU_DIVIDER);
        menu->add("&Settings/Theme...",           0, Test::CallbackTheme, this, FL_MENU_DIVIDER);
        menu->add("&Settings/Font 10",            0, Test::Callback10, this);
        menu->add("&Settings/Font 14",            0, Test::Callback14, this);
        menu->add("&Settings/Font 24",            0, Test::Callback24, this, FL_MENU_DIVIDER);
        menu->add("&Settings/Alt Font 0.6",       0, Test::CallbackA06, this);
        menu->add("&Settings/Alt Font 0.8",       0, Test::CallbackA08, this);
        menu->add("&Settings/Alt Font 1.0",       0, Test::CallbackA10, this, FL_MENU_DIVIDER);
        menu->add("&Settings/Debug",              0, Test::CallbackDebug, this);

        menu->textfont(flw::PREF_FONT);
        menu->textsize(flw::PREF_FONTSIZE);
        menu::set_item(menu, "&Settings/srand(time)", true);
        //menu::set_item(menu, "&Settings/srand(666)", true); RAND = 666; TEST = TEST_RIGHT_10;
        util::labelfont(this);
        flw::PREF_WINDOW = this;

        resize(300, 300, W, H);
        show();
        create_chart();
    }

    //--------------------------------------------------------------------------
    ~Test() {
        auto pref = Fl_Preferences(Fl_Preferences::USER, "gnuwimp_test", "test_chart");
        flw::theme::save_theme_to_pref(pref);
        flw::theme::save_win_to_pref(pref, "gui.", this);
    }

    //--------------------------------------------------------------------------
    static void Callback10(Fl_Widget*, void* v) {
        auto self = static_cast<Test*>(v);
        flw::PREF_FONTSIZE = 10;
        flw::PREF_FIXED_FONTSIZE = 10;
        self->update_pref();
    }

    //--------------------------------------------------------------------------
    static void Callback14(Fl_Widget*, void* v) {
        auto self = static_cast<Test*>(v);
        flw::PREF_FONTSIZE = 14;
        flw::PREF_FIXED_FONTSIZE = 14;
        self->update_pref();
    }

    //--------------------------------------------------------------------------
    static void Callback24(Fl_Widget*, void* v) {
        auto self = static_cast<Test*>(v);
        flw::PREF_FONTSIZE = 24;
        flw::PREF_FIXED_FONTSIZE = 24;
        self->update_pref();
    }

    //--------------------------------------------------------------------------
    static void CallbackA06(Fl_Widget*, void* v) {
        auto self = static_cast<Test*>(v);
        self->chart->set_alt_size(0.6);
        self->update_pref();
    }

    //--------------------------------------------------------------------------
    static void CallbackA08(Fl_Widget*, void* v) {
        auto self = static_cast<Test*>(v);
        self->chart->set_alt_size(0.8);
        self->update_pref();
    }

    //--------------------------------------------------------------------------
    static void CallbackA10(Fl_Widget*, void* v) {
        auto self = static_cast<Test*>(v);
        self->chart->set_alt_size(1.0);
        self->update_pref();
    }

    //--------------------------------------------------------------------------
    static void CallbackAuto(Fl_Widget*, void* v) {
        auto self = static_cast<Test*>(v);
        TEST = "";
        self->menu->deactivate();
        self->chart->disable_menu();
        Fl::add_timeout(0.5, Test::CallbackTimer, v);
    }

    //--------------------------------------------------------------------------
    static void CallbackDebug(Fl_Widget*, void* v) {
        auto self = static_cast<Test*>(v);
        flw::PREF_FONTSIZE = 24;
        flw::PREF_FIXED_FONTSIZE = 24;
        self->update_pref();
        self->size(800, 592);
        TEST = TEST_LARGE_VALUE2;
        self->create_chart();
    }

    //--------------------------------------------------------------------------
    static void CallbackLoad(Fl_Widget*, void* v) {
        auto self = static_cast<Test*>(v);
        self->chart->load_json("chart.json");
    }

    //--------------------------------------------------------------------------
    static void CallbackRand666(Fl_Widget*, void* v) {
        RAND = 666;
        static_cast<Test*>(v)->create_chart();
    }

    //--------------------------------------------------------------------------
    static void CallbackRandTime(Fl_Widget*, void* v) {
        RAND = 0;
        static_cast<Test*>(v)->create_chart();
    }

    //--------------------------------------------------------------------------
    static void CallbackSave(Fl_Widget*, void* v) {
        auto self = static_cast<Test*>(v);
        self->chart->save_json("chart.json");
    }

    //--------------------------------------------------------------------------
    static void CallbackTest(Fl_Widget*, void* v) {
        auto self = static_cast<Test*>(v);
        TEST = self->menu->text();
        self->create_chart();
    }

    //------------------------------------------------------------------------------
    static void CallbackTimer(void* data) {
        auto self = static_cast<Test*>(data);

        if (TEST == TEST_LAST) {
            Fl::remove_timeout(CallbackTimer, data);
            self->menu->activate();
            self->chart->enable_menu();
            TEST = TEST_FIRST;
        }
        else {
            if (TEST == "")                         TEST = TEST_RANGE_DAY;
            else if (TEST == TEST_RANGE_DAY)        TEST = TEST_RANGE_WEEKDAY;
            else if (TEST == TEST_RANGE_WEEKDAY)    TEST = TEST_RANGE_FRIDAY;
            else if (TEST == TEST_RANGE_FRIDAY)     TEST = TEST_RANGE_SUNDAY;
            else if (TEST == TEST_RANGE_SUNDAY)     TEST = TEST_RANGE_MONTH;
            else if (TEST == TEST_RANGE_MONTH)      TEST = TEST_RANGE_HOUR;
            else if (TEST == TEST_RANGE_HOUR)       TEST = TEST_RANGE_MIN;
            else if (TEST == TEST_RANGE_MIN)        TEST = TEST_RANGE_SEC;
            else if (TEST == TEST_RANGE_SEC)        TEST = TEST_SMALL_VALUE1;

            else if (TEST == TEST_SMALL_VALUE1)     TEST = TEST_SMALL_VALUE2;
            else if (TEST == TEST_SMALL_VALUE2)     TEST = TEST_SMALL_VALUE3;
            else if (TEST == TEST_SMALL_VALUE3)     TEST = TEST_LARGE_VALUE1;

            else if (TEST == TEST_LARGE_VALUE1)     TEST = TEST_LARGE_VALUE2;
            else if (TEST == TEST_LARGE_VALUE2)     TEST = TEST_LARGE_VALUE3;
            else if (TEST == TEST_LARGE_VALUE3)     TEST = TEST_RIGHT_10;

            else if (TEST == TEST_RIGHT_10)         TEST = TEST_RIGHT_NEG;
            else if (TEST == TEST_RIGHT_NEG)        TEST = TEST_RIGHT_SMALL;
            else if (TEST == TEST_RIGHT_SMALL)      TEST = TEST_RIGHT_STEP;
            else if (TEST == TEST_RIGHT_STEP)       TEST = TEST_RIGHT_LEFT;
            else if (TEST == TEST_RIGHT_LEFT)       TEST = TEST_REF_ALL;

            else if (TEST == TEST_REF_ALL)          TEST = TEST_REF_ALL_BLOCK;
            else if (TEST == TEST_REF_ALL_BLOCK)    TEST = TEST_REF_FRIDAY;
            else if (TEST == TEST_REF_FRIDAY)       TEST = TEST_REF_SUNDAY;
            else if (TEST == TEST_REF_SUNDAY)       TEST = TEST_REF_MONTH;
            else if (TEST == TEST_REF_MONTH)        TEST = TEST_AREA2;

            else if (TEST == TEST_AREA2)            TEST = TEST_AREA3;
            else if (TEST == TEST_AREA3)            TEST = TEST_AREA4;
            else if (TEST == TEST_AREA4)            TEST = TEST_AREA5;
            else if (TEST == TEST_AREA5)            TEST = TEST_EMPTY;

            Fl::repeat_timeout(2.0, CallbackTimer, data);
        }

        self->create_chart();
    }

    //--------------------------------------------------------------------------
    static void CallbackTheme(Fl_Widget*, void* v) {
        auto self = static_cast<Test*>(v);
        flw::dlg::theme(true, true);
        self->update_pref();
    }

    //--------------------------------------------------------------------------
    void create_chart() {
        if (RAND == 0) {
            srand(time(nullptr));
        }
        else {
            srand(RAND);
        }

        chart->reset();


        if (TEST == TEST_RANGE_DAY)             test1(chart, TEST, "LineType::LINE", 100.0, 2.0, chart::DateRange::DAY, chart::LineType::LINE, FL_ALIGN_LEFT, 3, 2);
        else if (TEST == TEST_RANGE_WEEKDAY)    test1(chart, TEST, "LineType::BAR", 100.0, 2.0, chart::DateRange::WEEKDAY, chart::LineType::BAR, FL_ALIGN_LEFT, 25, 6);
        else if (TEST == TEST_RANGE_FRIDAY)     test1(chart, TEST, "LineType::BAR_CLAMP", 100.0, 1.0, chart::DateRange::FRIDAY, chart::LineType::BAR_CLAMP, FL_ALIGN_LEFT, 25, 6, -50.0, 200.0);
        else if (TEST == TEST_RANGE_SUNDAY)     test1(chart, TEST, "LineType::BAR_CLAMP", 100.0, 2.0, chart::DateRange::SUNDAY, chart::LineType::BAR_CLAMP, FL_ALIGN_LEFT, 25, 6);
        else if (TEST == TEST_RANGE_MONTH)      test1(chart, TEST, "LineType::BAR_HLC", 100.0, 2.0, chart::DateRange::MONTH, chart::LineType::BAR_HLC, FL_ALIGN_LEFT, 40, 6);
        else if (TEST == TEST_RANGE_HOUR)       test6(chart, chart::DateRange::HOUR);
        else if (TEST == TEST_RANGE_MIN)        test6(chart, chart::DateRange::MIN);
        else if (TEST == TEST_RANGE_SEC)        test6(chart, chart::DateRange::SEC);

        else if (TEST == TEST_SMALL_VALUE1)     test1(chart, TEST, "LineType::LINE", 100.0, 2.5, chart::DateRange::DAY, chart::LineType::LINE, FL_ALIGN_LEFT, 5, 4, INFINITY, INFINITY, 10000000.0);
        else if (TEST == TEST_SMALL_VALUE2)     test1(chart, TEST, "LineType::BAR_HLC", 0.1, 1.0, chart::DateRange::DAY, chart::LineType::BAR_HLC, FL_ALIGN_LEFT, 15, 4);
        else if (TEST == TEST_SMALL_VALUE3)     test1(chart, TEST, "LineType::HORIZONTAL", 0.1, 1.0, chart::DateRange::DAY, chart::LineType::HORIZONTAL, FL_ALIGN_LEFT, 15, 2);

        else if (TEST == TEST_LARGE_VALUE1)     test1(chart, TEST, "LineType::BAR_HLC", 1000000000.0, 3.0, chart::DateRange::DAY, chart::LineType::BAR_HLC, FL_ALIGN_LEFT, 10, 6);
        else if (TEST == TEST_LARGE_VALUE2)     test10(chart);
        else if (TEST == TEST_LARGE_VALUE3)     test4(chart);

        else if (TEST == TEST_RIGHT_10)         test9(chart);
        else if (TEST == TEST_RIGHT_NEG)        test1(chart, TEST, "LineType::BAR", -50000.0, 2.0, chart::DateRange::DAY, chart::LineType::BAR, FL_ALIGN_RIGHT, 3, 1);
        else if (TEST == TEST_RIGHT_SMALL)      test7(chart);
        else if (TEST == TEST_RIGHT_STEP)       test8(chart);
        else if (TEST == TEST_RIGHT_LEFT)       test2(chart);

        else if (TEST == TEST_REF_ALL)          test3(chart, "Ref All", chart::DateRange::DAY);
        else if (TEST == TEST_REF_ALL_BLOCK)    test3(chart, "Ref All + Block", chart::DateRange::DAY, true);
        else if (TEST == TEST_REF_FRIDAY)       test3(chart, "Ref Friday", chart::DateRange::FRIDAY);
        else if (TEST == TEST_REF_SUNDAY)       test3(chart, "Ref Sunday", chart::DateRange::SUNDAY);
        else if (TEST == TEST_REF_MONTH)        test3(chart, "Ref Month", chart::DateRange::MONTH);

        else if (TEST == TEST_AREA2)            test5(chart, "Two Chart Areas", 2);
        else if (TEST == TEST_AREA3)            test5(chart, "Three Chart Areas", 3);
        else if (TEST == TEST_AREA4)            test5(chart, "Four Chart Areas", 4);
        else if (TEST == TEST_AREA5)            test5(chart, "Five Chart Areas", 5);

        else if (TEST == TEST_EMPTY)            { chart->set_main_label("Empty"); chart->init_new_data(); }

        chart->take_focus();
        Fl::redraw();
    }

    //--------------------------------------------------------------------------
    void resize(int X, int Y, int W, int H) override {
        Fl_Double_Window::resize(X, Y, W, H);
        menu->resize(0, 0, W, flw::PREF_FONTSIZE * 2);
        chart->resize(0, flw::PREF_FONTSIZE * 2, W, H - flw::PREF_FONTSIZE * 2);
         //chart->resize(100, 100, W - 200, H - 200);
    }

    //--------------------------------------------------------------------------
    void update_pref() {
        menu->textfont(flw::PREF_FONT);
        menu->textsize(flw::PREF_FONTSIZE);
        chart->update_pref();
        chart->init();
        resize(x(), y(), w(), h());
        Fl::redraw();
    }
};

//------------------------------------------------------------------------------
int main(int argc, const char** argv) {
    if (flw::theme::parse(argc, argv) == false) {
        flw::theme::load("oxy");
    }

    //Fl::screen_scale(0, 1.0);

    {
        Test win(1200, 800);
        Fl::run();
    }

    return 0;
}
