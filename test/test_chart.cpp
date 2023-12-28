// Copyright 2016 - 2022 gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#include "chart.h"
#include "price.h"
#include "theme.h"
#include "util.h"
#include <math.h>
#include <string.h>
#include <time.h>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl.H>

using namespace flw;

class Test;

enum {
    DATA_NONE,
    DATA_LEFT_ALL,
    DATA_LEFT_STEP,
    DATA_RIGHT_10,
    DATA_LEFT_WEEKDAY,
    DATA_LEFT_FRIDAY,
    DATA_LEFT_SUNDAY,
    DATA_LEFT_MONTH,
    DATA_LEFT_SMALL_VALUE,
    DATA_LEFT_SMALL_VALUE2,
    DATA_LEFT_SMALL_VALUE3,
    DATA_LEFT_LARGE_VALUE,
    DATA_LEFT_LARGE_VALUE2,
    DATA_LEFT_LARGE_DATASET,
    DATA_RIGHT_ALL,
    DATA_RIGHT_SMALL_VALUE,
    DATA_LEFT_RIGHT_ALL,
    DATA_REF_ALL,
    DATA_REF_ALL_BLOCK,
    DATA_REF_FRIDAY,
    DATA_REF_SUNDAY,
    DATA_REF_MONTH,
    DATA_AREA2,
    DATA_AREA3,
    DATA_HOUR,
    DATA_MIN,
    DATA_SEC,
    DATA_LAST,
};

int TEST = DATA_LEFT_ALL;
int RAND = 0;

void callback_timer(void *data);
void create_chart(Test* w);
void test1(Chart* chart, const char* label, double start, double change, Date::RANGE range, chart::TYPE type, bool left, int tick, int width, double min = chart::MIN_VAL, double max = chart::MAX_VAL);
void test2(Chart* chart, double start, double change, const Date::RANGE range);
void test3(Chart* chart, const char* label, const Date::RANGE range, bool block = false);
void test4(Chart* chart, double start, double change);
void test5(Chart* chart, bool three);
void test6(Chart* chart, Date::RANGE what);
void test7(Chart* chart);
void test8(Chart* chart, const char* label, Date::RANGE range, chart::TYPE type, int tick, int width);
void test9(Chart* chart);

class Test : public Fl_Double_Window {
public:
    Fl_Menu_Bar* mb;
    Chart*       chart;

    Test(int W, int H) : Fl_Double_Window(W, H, "test_chart.cpp") {
        end();

        mb    = new Fl_Menu_Bar(0, 0, 0, 0);
        chart = new Chart();

        add(mb);
        add(chart);
        chart->view_options();
        mb->add("&Data/Autorun", 0, CallbackAuto, this, FL_MENU_DIVIDER);
        mb->add("&Data/save", 0, CallbackSave, this);
        mb->add("&Data/Load", 0, CallbackLoad, this, FL_MENU_DIVIDER);
        mb->add("&Data/srand(time)", 0, CallbackTest, this, FL_MENU_RADIO);
        mb->add("&Data/srand(666)", 0, CallbackTest, this, FL_MENU_RADIO | FL_MENU_DIVIDER);
        mb->add("&Data/All Days (LINE)", 0, CallbackTest, this, FL_MENU_RADIO);
        mb->add("&Data/Step (LINE)", 0, CallbackTest, this, FL_MENU_RADIO);
        mb->add("&Data/10 Colors (LINE)", 0, CallbackTest, this, FL_MENU_RADIO);
        mb->add("&Data/Weekdays (VERTICAL)", 0, CallbackTest, this, FL_MENU_RADIO);
        mb->add("&Data/Fridays (CLAMP_VERTICAL)", 0, CallbackTest, this, FL_MENU_RADIO);
        mb->add("&Data/Sundays (CLAMP_VERTICAL)", 0, CallbackTest, this, FL_MENU_RADIO);
        mb->add("&Data/Month", 0, CallbackTest, this, FL_MENU_RADIO);
        mb->add("&Data/Small Values (VERTICAL)", 0, CallbackTest, this, FL_MENU_RADIO);
        mb->add("&Data/Small Values With Expandable Width (VERTICAL)", 0, CallbackTest, this, FL_MENU_RADIO);
        mb->add("&Data/Small Values (HORIZONTAL)", 0, CallbackTest, this, FL_MENU_RADIO);
        mb->add("&Data/Large Values (BAR)", 0, CallbackTest, this, FL_MENU_RADIO);
        mb->add("&Data/Large Values With Expandable Width (BAR)", 0, CallbackTest, this, FL_MENU_RADIO);
        mb->add("&Data/3 Lines With Large Dataset", 0, CallbackTest, this, FL_MENU_RADIO);
        mb->add("&Data/Right Negative Values (VERTICAL)", 0, CallbackTest, this, FL_MENU_RADIO);
        mb->add("&Data/Right Small (LINE)", 0, CallbackTest, this, FL_MENU_RADIO);
        mb->add("&Data/Left And Right All Days", 0, CallbackTest, this, FL_MENU_RADIO);
        mb->add("&Data/Ref All (BAR)", 0, CallbackTest, this, FL_MENU_RADIO);
        mb->add("&Data/Ref All (BAR) + Block", 0, CallbackTest, this, FL_MENU_RADIO);
        mb->add("&Data/Ref Friday (BAR)", 0, CallbackTest, this, FL_MENU_RADIO);
        mb->add("&Data/Ref Sunday (BAR)", 0, CallbackTest, this, FL_MENU_RADIO);
        mb->add("&Data/Ref Month (BAR)", 0, CallbackTest, this, FL_MENU_RADIO);
        mb->add("&Data/2 Chart Areas", 0, CallbackTest, this, FL_MENU_RADIO);
        mb->add("&Data/3 Chart Areas", 0, CallbackTest, this, FL_MENU_RADIO);
        mb->add("&Data/Hour", 0, CallbackTest, this, FL_MENU_RADIO);
        mb->add("&Data/Minutes", 0, CallbackTest, this, FL_MENU_RADIO);
        mb->add("&Data/Seconds", 0, CallbackTest, this, FL_MENU_RADIO);
        mb->add("&Data/None", 0, CallbackTest, this, FL_MENU_RADIO);
        mb->textfont(flw::PREF_FONT);
        mb->textsize(flw::PREF_FONTSIZE);

        util::menu_item_set(mb, "&Data/srand(time)", true);
        util::menu_item_set(mb, "&Data/All Days (LINE)", true);
        mb->textsize(flw::PREF_FONTSIZE);
        util::labelfont(this);

        resize(0, 0, W, H);
        create_chart(this);
        resizable(this);
        size_range(64, 48);
        show();
    }

    void resize(int X, int Y, int W, int H) override {
        Fl_Double_Window::resize(X, Y, W, H);
        mb->resize(0, 0, W, flw::PREF_FONTSIZE * 2);
        chart->resize(0, flw::PREF_FONTSIZE * 2, W, H - flw::PREF_FONTSIZE * 2);
        // chart->resize(100, 100, W - 200, H - 200);
    }

    static void CallbackAuto(Fl_Widget*, void* v) {
        Test* w = (Test*) v;
        TEST = DATA_NONE;
        w->mb->deactivate();
        Fl::add_timeout(0.5, callback_timer, v);
    }

    static void CallbackLoad(Fl_Widget*, void* v) {
        Test* w = (Test*) v;
        chart::load_data(w->chart, "chart.json");
    }

    static void CallbackSave(Fl_Widget*, void* v) {
        Test* w = (Test*) v;
        chart::save_data(w->chart, "chart.json");
    }

    static void CallbackTest(Fl_Widget*, void* v) {
        Test* w = (Test*) v;

        if (strcmp(w->mb->text(), "srand(time)") == 0)                                          RAND = 0;
        else if (strcmp(w->mb->text(), "srand(666)") == 0)                                      RAND = 666;
        else if (strcmp(w->mb->text(), "All Days (LINE)") == 0)                                 TEST = DATA_LEFT_ALL;
        else if (strcmp(w->mb->text(), "Step (LINE)") == 0)                                     TEST = DATA_LEFT_STEP;
        else if (strcmp(w->mb->text(), "10 Colors (LINE)") == 0)                                TEST = DATA_RIGHT_10;
        else if (strcmp(w->mb->text(), "Weekdays (VERTICAL)") == 0)                             TEST = DATA_LEFT_WEEKDAY;
        else if (strcmp(w->mb->text(), "Fridays (CLAMP_VERTICAL)") == 0)                        TEST = DATA_LEFT_FRIDAY;
        else if (strcmp(w->mb->text(), "Sundays (CLAMP_VERTICAL)") == 0)                        TEST = DATA_LEFT_SUNDAY;
        else if (strcmp(w->mb->text(), "Month") == 0)                                           TEST = DATA_LEFT_MONTH;
        else if (strcmp(w->mb->text(), "Small Values (VERTICAL)") == 0)                         TEST = DATA_LEFT_SMALL_VALUE;
        else if (strcmp(w->mb->text(), "Small Values With Expandable Width (VERTICAL)") == 0)   TEST = DATA_LEFT_SMALL_VALUE2;
        else if (strcmp(w->mb->text(), "Small Values (HORIZONTAL)") == 0)                       TEST = DATA_LEFT_SMALL_VALUE3;
        else if (strcmp(w->mb->text(), "Large Values (BAR)") == 0)                              TEST = DATA_LEFT_LARGE_VALUE;
        else if (strcmp(w->mb->text(), "Large Values With Expandable Width (BAR)") == 0)        TEST = DATA_LEFT_LARGE_VALUE2;
        else if (strcmp(w->mb->text(), "3 Lines With Large Dataset") == 0)                      TEST = DATA_LEFT_LARGE_DATASET;
        else if (strcmp(w->mb->text(), "Right Negative Values (VERTICAL)") == 0)                TEST = DATA_RIGHT_ALL;
        else if (strcmp(w->mb->text(), "Right Small (LINE)") == 0)                              TEST = DATA_RIGHT_SMALL_VALUE;
        else if (strcmp(w->mb->text(), "Left And Right All Days") == 0)                         TEST = DATA_LEFT_RIGHT_ALL;
        else if (strcmp(w->mb->text(), "Ref All (BAR)") == 0)                                   TEST = DATA_REF_ALL;
        else if (strcmp(w->mb->text(), "Ref All (BAR) + Block") == 0)                           TEST = DATA_REF_ALL_BLOCK;
        else if (strcmp(w->mb->text(), "Ref Friday (BAR)") == 0)                                TEST = DATA_REF_FRIDAY;
        else if (strcmp(w->mb->text(), "Ref Sunday (BAR)") == 0)                                TEST = DATA_REF_SUNDAY;
        else if (strcmp(w->mb->text(), "Ref Month (BAR)") == 0)                                 TEST = DATA_REF_MONTH;
        else if (strcmp(w->mb->text(), "2 Chart Areas") == 0)                                   TEST = DATA_AREA2;
        else if (strcmp(w->mb->text(), "3 Chart Areas") == 0)                                   TEST = DATA_AREA3;
        else if (strcmp(w->mb->text(), "Hour") == 0)                                            TEST = DATA_HOUR;
        else if (strcmp(w->mb->text(), "Minutes") == 0)                                         TEST = DATA_MIN;
        else if (strcmp(w->mb->text(), "Seconds") == 0)                                         TEST = DATA_SEC;
        else                                                                                    TEST = DATA_NONE;

        create_chart(w);
    }
};

void callback_timer(void *data) {
    auto test = (Test*) data;

    TEST++;

    if (TEST == DATA_LAST) {
        TEST = DATA_LEFT_ALL;
        util::menu_item_set(test->mb, "&Data/All Days (LINE)", true);
        test->mb->activate();
        Fl::remove_timeout(callback_timer, data);
    }
    else {
        Fl::repeat_timeout(2.0, callback_timer, data);
    }

    create_chart(test);
}

void create_chart(Test* w) {
    if (RAND == 0) {
        srand(time(nullptr));
    }
    else {
        srand(RAND);
    }

    w->chart->clear();

    if (TEST == DATA_LEFT_ALL)                test1(w->chart, "All Days (LINE)", 100.0, 2.0, Date::RANGE::DAY, chart::TYPE::LINE, true, 3, 2);
    else if (TEST == DATA_LEFT_STEP)          test8(w->chart, "Step (LINE)", Date::RANGE::DAY, chart::TYPE::LINE, 80, 4);
    else if (TEST == DATA_RIGHT_10)           test9(w->chart);
    else if (TEST == DATA_LEFT_WEEKDAY)       test1(w->chart, "Weekdays (VERTICAL)", 100.0, 2.0, Date::RANGE::WEEKDAY, chart::TYPE::VERTICAL, true, 11, 6);
    else if (TEST == DATA_LEFT_FRIDAY)        test1(w->chart, "Fridays (CLAMP_VERTICAL)", 100.0, 1.0, Date::RANGE::FRIDAY, chart::TYPE::CLAMP_VERTICAL, true, 5, 3, -50.0, 200.0);
    else if (TEST == DATA_LEFT_SUNDAY)        test1(w->chart, "Sundays (CLAMP_VERTICAL)", 100.0, 2.0, Date::RANGE::SUNDAY, chart::TYPE::CLAMP_VERTICAL, true, 5, 3);
    else if (TEST == DATA_LEFT_MONTH)         test1(w->chart, "Month", 100.0, 2.0, Date::RANGE::MONTH, chart::TYPE::LINE, true, 10, 3);
    else if (TEST == DATA_LEFT_SMALL_VALUE)   test1(w->chart, "Small Values (VERTICAL)", 0.1, 1.0, Date::RANGE::DAY, chart::TYPE::VERTICAL, true, 5, 4);
    else if (TEST == DATA_LEFT_SMALL_VALUE2)  test1(w->chart, "Small Values (VERTICAL) -1", 0.1, 1.0, Date::RANGE::DAY, chart::TYPE::VERTICAL, true, 5, -1);
    else if (TEST == DATA_LEFT_LARGE_VALUE)   test1(w->chart, "Large Values (BAR)", 1000000000.0, 3.0, Date::RANGE::DAY, chart::TYPE::BAR, true, 10, 6);
    else if (TEST == DATA_LEFT_LARGE_VALUE2)  test1(w->chart, "Large Values (BAR)", 1000000000.0, 3.0, Date::RANGE::DAY, chart::TYPE::BAR, true, 10, -1);
    else if (TEST == DATA_LEFT_SMALL_VALUE3)  test1(w->chart, "Small Values (HORIZONTAL))", 0.1, 1.0, Date::RANGE::DAY, chart::TYPE::HORIZONTAL, true, 15, 2);
    else if (TEST == DATA_LEFT_LARGE_DATASET) test4(w->chart, 1000.0, 0.5);
    else if (TEST == DATA_RIGHT_ALL)          test1(w->chart, "Right Negative Values (VERTICAL)", -50000.0, 2.0, Date::RANGE::DAY, chart::TYPE::VERTICAL, false, 3, 1);
    else if (TEST == DATA_RIGHT_SMALL_VALUE)  test7(w->chart);
    else if (TEST == DATA_LEFT_RIGHT_ALL)     test2(w->chart, 1000.0, 3.0, Date::RANGE::DAY);
    else if (TEST == DATA_REF_ALL)            test3(w->chart, "Ref Data All", Date::RANGE::DAY);
    else if (TEST == DATA_REF_ALL_BLOCK)      test3(w->chart, "Ref Data All + Block", Date::RANGE::DAY, true);
    else if (TEST == DATA_REF_FRIDAY)         test3(w->chart, "Ref Data Friday", Date::RANGE::FRIDAY);
    else if (TEST == DATA_REF_SUNDAY)         test3(w->chart, "Ref Data Sunday", Date::RANGE::SUNDAY);
    else if (TEST == DATA_REF_MONTH)          test3(w->chart, "Ref Data Month", Date::RANGE::MONTH);
    else if (TEST == DATA_AREA2)              test5(w->chart, false);
    else if (TEST == DATA_AREA3)              test5(w->chart, true);
    else if (TEST == DATA_HOUR)               test6(w->chart, Date::RANGE::HOUR);
    else if (TEST == DATA_MIN)                test6(w->chart, Date::RANGE::MIN);
    else if (TEST == DATA_SEC)                test6(w->chart, Date::RANGE::SEC);
}

PriceVector create_serie(const char* start, const char* stop, double value, double change, const Date::RANGE range) {
    auto price = value;
    auto ch    = 0.0;
    auto date1 = Date::FromString(start);
    auto date2 = Date::FromString(stop);
    auto res   = PriceVector();

    while (date1 <= date2) {
        double high = price * 1.02;
        double low  = price * 0.98;

        if (rand() % 3 == 0) {
            high = price * 1.05;
            low  = price * 0.95;
        }
        else if (rand() % 5 == 0) {
            high = price * 1.08;
            low  = price * 0.92;
        }

        if (high < low) {
            double t = high;
            high = low;
            low = t;
        }

        if (range == Date::RANGE::HOUR) {
            res.push_back(Price(date1.format(Date::FORMAT::ISO_TIME).c_str(), high, low, price));
            date1.add_seconds(Date::SECS_PER_HOUR);
            // fprintf(stderr, "    date1=%s\n", date1.format(Date::FORMAT::ISO_TIME).c_str());
        }
        else if (range == Date::RANGE::MIN) {
            res.push_back(Price(date1.format(Date::FORMAT::ISO_TIME).c_str(), high, low, price));
            date1.add_seconds(60);
        }
        else if (range == Date::RANGE::SEC) {
            res.push_back(Price(date1.format(Date::FORMAT::ISO_TIME).c_str(), high, low, price));
            date1.add_seconds(1);
        }
        else {
            res.push_back(Price(date1.format(Date::FORMAT::ISO).c_str(), high, low, price));
            date1.add_days(1);
        }

        if (rand() % 2 == 0) {
            ch = 1.0 + change / (double) (rand() % 100 + 100.0);
        }
        else {
            ch = 1.0 - change / (double) (rand() % 100 + 100.0);
        }

        price *= ch;
        // fprintf(stderr, "  date2=%s\n", date2.format(s, 50, Date::FORMAT::ISO_TIME));
    }

    return res;
}

PriceVector create_serie2(const char* start, const char* stop, double d = 1.0) {
    auto res   = PriceVector();
    auto f     = 1.0;
    auto date1 = Date::FromString(start);
    auto date2 = Date::FromString(stop);

    while (date1 <= date2) {
        res.push_back(Price(date1.format(Date::FORMAT::ISO).c_str(), sinl(f) + d));
        f += d;
        date1.add_days(1);
    }

    return res;
}

void test1(Chart* chart, const char* label, const double start, const double change, const Date::RANGE range, const chart::TYPE type, const bool left, const int tick, const int width, const double min, const double max) {
    PriceVector prices1 = create_serie("20010101", "20191231", start, change, range);

    if (left == true) {
        chart->add_line(0, prices1, label, type, FL_ALIGN_LEFT, color::BLUE, width, min, max);

        if (start > 100000000.0) {
            chart->margin(10, 1);
        }
        else if (start > 1000000.0) {
            chart->margin(8, 1);
        }
        else {
            chart->margin(6, 1);
        }
    }
    else {
        chart->add_line(0, prices1, label, type, FL_ALIGN_RIGHT, color::BLUE, width, min, max);
        chart->margin(3, 6);
    }

    chart->date_range(range);
    chart->tick_width(tick);
    chart->init(true);
}

void test2(Chart* chart, const double start, const double change, const Date::RANGE range) {
    PriceVector prices1 = create_serie("20010101", "20091231", start, change, range);
    PriceVector prices2 = create_serie("20010201", "20091130", start, change, range);

    chart->add_line(0, prices1, "Line Chart", chart::TYPE::LINE, FL_ALIGN_LEFT, color::BLUE, 2);
    chart->add_line(0, prices2, "Bar Chart", chart::TYPE::BAR, FL_ALIGN_RIGHT, color::RED, 2);
    chart->margin(6, 6);
    chart->date_range(range);
    chart->tick_width(10);
    chart->init(true);
}

void test3(Chart* chart, const char* label, const Date::RANGE range, const bool block) {
    PriceVector prices1;
    PriceVector prices2;
    PriceVector prices3;

    prices1.push_back(Price("20140102", 10)); // Thu    *
    prices1.push_back(Price("20140105", 15)); // Sun    *
    prices1.push_back(Price("20140111", 20)); // Sat    *
    prices1.push_back(Price("20140113", 30)); // Mon |  *
    prices1.push_back(Price("20140116", 25)); // Thu |  *
    prices1.push_back(Price("20140124", 35)); // Fri |  *
    prices1.push_back(Price("20140126", 40)); // Sun    *
    prices1.push_back(Price("20140204", 45)); // Tue
    prices1.push_back(Price("20140318", 50)); // Tue |  *
    prices1.push_back(Price("20140319", 55)); // Wen |  *
    prices1.push_back(Price("20140505", 60)); // Mon
    prices1.push_back(Price("20140510", 30)); // Sat

    if (block == true) {
        prices3.push_back(Price("20140105"));
        prices3.push_back(Price("20140111"));
        prices3.push_back(Price("20140319"));
    }

    if (range == Date::RANGE::FRIDAY) {
        prices2 = Price::DayToWeek(prices1, Date::DAY::FRIDAY);
        chart->tick_width(25);
    }
    else if (range == Date::RANGE::SUNDAY) {
        prices2 = Price::DayToWeek(prices1, Date::DAY::SUNDAY);
        chart->tick_width(25);
    }
    else if (range == Date::RANGE::MONTH) {
        prices2 = Price::DayToMonth(prices1);
        chart->tick_width(50);
    }
    else {
        prices2 = prices1;
        chart->tick_width(6);
    }

    chart->add_line(0, prices2, label, chart::TYPE::BAR, FL_ALIGN_LEFT, color::BLUE, 6);
    chart->date_range(range);
    chart->margin(6, 1);
    chart->block_dates(prices3);
    chart->init(true);
}

void test4(Chart* chart, const double start, const double change) {
    PriceVector prices1 = create_serie("18000101", "20191231", start, change, Date::RANGE::DAY);
    PriceVector prices2 = create_serie("18100101", "20291231", start, change, Date::RANGE::DAY);
    PriceVector prices3 = create_serie("18200101", "19991231", start, change, Date::RANGE::DAY);

    chart->add_line(0, prices1, "18000101 - 20191231", chart::TYPE::LINE, FL_ALIGN_LEFT, color::BLUE, 1);
    chart->add_line(0, prices2, "18100101 - 20291231", chart::TYPE::LINE, FL_ALIGN_LEFT, FL_GREEN, 1);
    chart->add_line(0, prices3, "18200101 - 19991231", chart::TYPE::LINE, FL_ALIGN_LEFT, color::RED, 1);
    chart->margin(6, 1);
    chart->date_range(Date::RANGE::DAY);
    chart->tick_width(3);
    chart->init(true);
}

void test5(Chart* chart, const bool three) {
    PriceVector prices1 = create_serie("20010101", "20091231", 1000.0, 2.0, Date::RANGE::DAY);
    PriceVector prices2 = create_serie("20010201", "20091130", 1500000.0, 2.0, Date::RANGE::DAY);
    PriceVector prices3 = create_serie("20010101", "20091231", 3000000, 5.0, Date::RANGE::DAY);
    PriceVector prices4;

    if (three == true) {
        prices4 = Price::MovingAverage(prices1, 20);
        chart->area_size(40, 40, 20);
        chart->add_line(0, prices1, "Bar Chart", chart::TYPE::BAR, FL_ALIGN_LEFT, color::BLUE, 3);
        chart->add_line(0, prices4, "AVG", chart::TYPE::LINE, FL_ALIGN_LEFT, color::GREEN, 3);
        chart->add_line(1, prices2, "Line Chart", chart::TYPE::LINE, FL_ALIGN_LEFT, color::RED, 3);
        chart->add_line(2, prices3, "Volume", chart::TYPE::CLAMP_VERTICAL, FL_ALIGN_LEFT, color::GREEN, 3, 0.0);
        chart->margin(7, 1);
    }
    else {
        prices4 = Price::MovingAverage(prices3, 20);
        chart->area_size(70, 30, 0);
        chart->add_line(0, prices1, "Bar Chart", chart::TYPE::BAR, FL_ALIGN_LEFT, color::BLUE, -1);
        chart->add_line(0, prices2, "Line Chart", chart::TYPE::LINE, FL_ALIGN_RIGHT, color::RED, 3);
        chart->add_line(1, prices3, "Volume", chart::TYPE::CLAMP_VERTICAL, FL_ALIGN_LEFT, color::GREEN, -1);
        chart->add_line(1, prices4, "AVG", chart::TYPE::LINE, FL_ALIGN_LEFT, color::BLUE, 3);
        chart->margin(7, 7);
    }

    chart->date_range(Date::RANGE::DAY);
    chart->tick_width(4);
    chart->init(true);
}

void test6(Chart* chart, const Date::RANGE range) {
    PriceVector prices1 = create_serie("20201012 000000", "20201016 230000", 100.0, 0.5, range);
    // fprintf(stderr, "size=%d\n", prices1.size());

    if (range == Date::RANGE::HOUR) {
        chart->add_line(0, prices1, "Hour", chart::TYPE::BAR, FL_ALIGN_LEFT, color::BLUE, 4);
        chart->margin(6, 1);
    }
    else if (range == Date::RANGE::MIN) {
        chart->add_line(0, prices1, "Minutes", chart::TYPE::BAR, FL_ALIGN_LEFT, color::BLUE, 4);
        chart->margin(6, 1);
    }
    else if (range == Date::RANGE::SEC) {
        chart->add_line(0, prices1, "Seconds", chart::TYPE::BAR, FL_ALIGN_LEFT, color::BLUE, 4);
        chart->margin(6, 1);
    }

    chart->date_range(range);
    chart->tick_width(8);
    chart->init(true);
}

void test7(Chart* chart) {
    auto prices1 = PriceVector();
    auto date1   = Date::FromString("20010101");
    auto date2   = Date::FromString("20021231");
    auto p       = -3.5;

    while (date1 <= date2) {
        prices1.push_back(Price(date1.format(Date::FORMAT::ISO).c_str(), p));
        date1.add_days(1);

        if (rand() % 3 == 1) {
            p -= rand() % 5 / 300.0;
        }
        else {
            p += rand() % 5 / 100.0;
        }
    }

    chart->add_line(0, prices1, "Small (LINE)", chart::TYPE::LINE, FL_ALIGN_RIGHT, color::BLUE, 2);
    chart->margin(3, 6);
    chart->date_range(Date::RANGE::DAY);
    chart->tick_width(6);
    chart->init(true);
}

void test8(Chart* chart, const char* label, const Date::RANGE range, const chart::TYPE type, const int tick, const int width) {
    auto prices1 = create_serie("20010101", "20011231", 10.0, 1.0, Date::RANGE::DAY);
    auto start   = 1.0;
    auto even    = 0;

    for (auto& p : prices1) {
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

    chart->add_line(0, prices1, label, type, FL_ALIGN_LEFT, color::BLUE, width);
    chart->margin(6, 1);
    chart->date_range(range);
    chart->tick_width(tick);
    chart->init(true);
}

void test9(Chart* chart) {
    PriceVector prices1  = create_serie2("20010101", "20021231");
    PriceVector prices2  = create_serie2("20010101", "20021231", 2.0);
    PriceVector prices3  = create_serie2("20010101", "20021231", 3.0);
    PriceVector prices4  = create_serie2("20010101", "20021231", 4.0);
    PriceVector prices5  = create_serie2("20010101", "20021231", 5.0);
    PriceVector prices6  = create_serie2("20010101", "20021231", 6.0);
    PriceVector prices7  = create_serie2("20010101", "20021231", 7.0);
    PriceVector prices8  = create_serie2("20010101", "20021231", 8.0);
    PriceVector prices9  = create_serie2("20010101", "20021231", 9.0);
    PriceVector prices10 = create_serie2("20010101", "20021231", 10.0);

    chart->add_line(0, prices1, "One", chart::TYPE::LINE, FL_ALIGN_RIGHT, color::BLUE, 4);
    chart->add_line(0, prices2, "Two", chart::TYPE::LINE, FL_ALIGN_RIGHT, color::RED, 4);
    chart->add_line(0, prices3, "Three", chart::TYPE::LINE, FL_ALIGN_RIGHT, color::LIME, 4);
    chart->add_line(0, prices4, "Four", chart::TYPE::LINE, FL_ALIGN_RIGHT, color::MAGENTA, 4);
    chart->add_line(0, prices5, "Five", chart::TYPE::LINE, FL_ALIGN_RIGHT, color::CYAN, 4);
    chart->add_line(0, prices6, "Six", chart::TYPE::LINE, FL_ALIGN_RIGHT, color::YELLOW, 2);
    chart->add_line(0, prices7, "Seven", chart::TYPE::LINE, FL_ALIGN_RIGHT, color::OLIVE, 4);
    chart->add_line(0, prices8, "Eight", chart::TYPE::LINE, FL_ALIGN_RIGHT, color::BROWN, 4);
    chart->add_line(0, prices9, "Nine", chart::TYPE::LINE, FL_ALIGN_RIGHT, color::PINK, 4);
    chart->add_line(0, prices10, "Ten", chart::TYPE::LINE, FL_ALIGN_RIGHT, color::TEAL, 4);
    chart->margin(3, 5);
    chart->date_range(Date::RANGE::DAY);
    chart->tick_width(30);
    chart->init(true);
}

int main(int argc, const char** argv) {
    if (flw::theme::parse(argc, argv) == false) {
        flw::theme::load("gtk");
    }

    {
        srand(time(0));
        Test win(960, 540);
        Fl::run();
    }

    return 0;
}
