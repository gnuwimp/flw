// Copyright 2020 - 2021 gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#include "chart.h"
#include "price.h"
#include "theme.h"
#include "util.h"
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Menu_Bar.H>
#include <math.h>
#include <string.h>
#include <time.h>

using namespace flw;

class Test;

enum {
    DATA_NONE,
    DATA_LEFT_ALL,
    DATA_LEFT_WEEKDAY,
    DATA_LEFT_FRIDAY,
    DATA_LEFT_SUNDAY,
    DATA_LEFT_MONTH,
    DATA_LEFT_SMALL_VALUE,
    DATA_LEFT_LARGE_VALUE,
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
};

int TEST = DATA_LEFT_ALL;
int RAND = 0;

void create_chart(Test* w);
void init_chart(Test* w);
void test1(Chart* chart, const char* label, double start, double change, Date::RANGE range, Chart::TYPE type, bool left, int tick, int width, double min = Chart::MIN, double max = Chart::MAX);
void test2(Chart* chart, double start, double change, const Date::RANGE range);
void test3(Chart* chart, const char* label, const Date::RANGE range, bool block = false);
void test4(Chart* chart, double start, double change);
void test5(Chart* chart, bool three);
void test6(Chart* chart, Date::RANGE what);

class Test : public Fl_Double_Window {
public:
    Fl_Menu_Bar* mb;
    Chart*       chart;

    Test(int W, int H) : Fl_Double_Window(W, H, "test_chart.cpp") {
        end();
        resizable(this);

        mb    = new Fl_Menu_Bar(0, 0, 0, 0);
        chart = new Chart();

        add(mb);
        add(chart);

        mb->add("Debug/Dump", 0, CallbackDebug, this);

        mb->add("&Data/srand(time)", 0, CallbackTest, this, FL_MENU_RADIO);
        mb->add("&Data/srand(666)", 0, CallbackTest, this, FL_MENU_RADIO | FL_MENU_DIVIDER);
        mb->add("&Data/All Days", 0, CallbackTest, this, FL_MENU_RADIO);
        mb->add("&Data/Weekdays (Vertical)", 0, CallbackTest, this, FL_MENU_RADIO);
        mb->add("&Data/Fridays (Clamp)", 0, CallbackTest, this, FL_MENU_RADIO);
        mb->add("&Data/Sundays (Vertical2)", 0, CallbackTest, this, FL_MENU_RADIO);
        mb->add("&Data/Month", 0, CallbackTest, this, FL_MENU_RADIO);
        mb->add("&Data/Small Values (VERTICAL)", 0, CallbackTest, this, FL_MENU_RADIO);
        mb->add("&Data/Large Values (BAR)", 0, CallbackTest, this, FL_MENU_RADIO);
        mb->add("&Data/Large Dataset", 0, CallbackTest, this, FL_MENU_RADIO);
        mb->add("&Data/Right Negative Values", 0, CallbackTest, this, FL_MENU_RADIO);
        mb->add("&Data/Right Small (HORIZONTAL)", 0, CallbackTest, this, FL_MENU_RADIO);
        mb->add("&Data/Left And Right All Days", 0, CallbackTest, this, FL_MENU_RADIO);
        mb->add("&Data/Ref All", 0, CallbackTest, this, FL_MENU_RADIO);
        mb->add("&Data/Ref All + Block", 0, CallbackTest, this, FL_MENU_RADIO);
        mb->add("&Data/Ref Friday", 0, CallbackTest, this, FL_MENU_RADIO);
        mb->add("&Data/Ref Sunday", 0, CallbackTest, this, FL_MENU_RADIO);
        mb->add("&Data/Ref Month", 0, CallbackTest, this, FL_MENU_RADIO);
        mb->add("&Data/2 Chart Areas", 0, CallbackTest, this, FL_MENU_RADIO);
        mb->add("&Data/3 Chart Areas", 0, CallbackTest, this, FL_MENU_RADIO);
        mb->add("&Data/Hour", 0, CallbackTest, this, FL_MENU_RADIO);
        mb->add("&Data/Minutes", 0, CallbackTest, this, FL_MENU_RADIO);
        mb->add("&Data/Seconds", 0, CallbackTest, this, FL_MENU_RADIO);
        mb->add("&Data/None", 0, CallbackTest, this, FL_MENU_RADIO);

        mb->add("&Settings/Horizontal Lines", 0, Callback, this, FL_MENU_TOGGLE | FL_MENU_VALUE);
        mb->add("&Settings/Vertical Lines", 0, Callback, this, FL_MENU_TOGGLE | FL_MENU_VALUE | FL_MENU_DIVIDER);

        ((Fl_Menu_Item*) mb->find_item("&Data/srand(time)"))->set();
        ((Fl_Menu_Item*) mb->find_item("&Data/All Days"))->set();
        flw::util::labelfont(this);
        mb->textsize(flw::PREF_FONTSIZE);

        resize(0, 0, W, H);
        create_chart(this);
        position(0, 0);
        show();
    }

    void resize(int X, int Y, int W, int H) override {
        Fl_Double_Window::resize(X, Y, W, H);
        mb->resize(0, 0, W, flw::PREF_FONTSIZE * 2);
        chart->resize(0, flw::PREF_FONTSIZE * 2, W, H - flw::PREF_FONTSIZE * 2);
        // chart->resize(100, 100, W - 200, H - 200);
    }

    static void Callback(Fl_Widget*, void* v) {
        init_chart((Test*) v);
    }

    static void CallbackDebug(Fl_Widget*, void* v) {
        Test* w = (Test*) v;
        w->chart->debug();
    }

    static void CallbackTest(Fl_Widget*, void* v) {
        Test* w = (Test*) v;

        if (strcmp(w->mb->text(), "srand(time)") == 0)                      RAND = 0;
        else if (strcmp(w->mb->text(), "srand(666)") == 0)                  RAND = 666;
        else if (strcmp(w->mb->text(), "All Days") == 0)                    TEST = DATA_LEFT_ALL;
        else if (strcmp(w->mb->text(), "Weekdays (Vertical)") == 0)         TEST = DATA_LEFT_WEEKDAY;
        else if (strcmp(w->mb->text(), "Fridays (Clamp)") == 0)             TEST = DATA_LEFT_FRIDAY;
        else if (strcmp(w->mb->text(), "Sundays (Vertical2)") == 0)         TEST = DATA_LEFT_SUNDAY;
        else if (strcmp(w->mb->text(), "Month") == 0)                       TEST = DATA_LEFT_MONTH;
        else if (strcmp(w->mb->text(), "Small Values (VERTICAL)") == 0)     TEST = DATA_LEFT_SMALL_VALUE;
        else if (strcmp(w->mb->text(), "Large Values (BAR)") == 0)          TEST = DATA_LEFT_LARGE_VALUE;
        else if (strcmp(w->mb->text(), "Large Dataset") == 0)               TEST = DATA_LEFT_LARGE_DATASET;
        else if (strcmp(w->mb->text(), "Right Negative Values") == 0)       TEST = DATA_RIGHT_ALL;
        else if (strcmp(w->mb->text(), "Right Small (HORIZONTAL)") == 0)    TEST = DATA_RIGHT_SMALL_VALUE;
        else if (strcmp(w->mb->text(), "Left And Right All Days") == 0)     TEST = DATA_LEFT_RIGHT_ALL;
        else if (strcmp(w->mb->text(), "Ref All") == 0)                     TEST = DATA_REF_ALL;
        else if (strcmp(w->mb->text(), "Ref All + Block") == 0)             TEST = DATA_REF_ALL_BLOCK;
        else if (strcmp(w->mb->text(), "Ref Friday") == 0)                  TEST = DATA_REF_FRIDAY;
        else if (strcmp(w->mb->text(), "Ref Sunday") == 0)                  TEST = DATA_REF_SUNDAY;
        else if (strcmp(w->mb->text(), "Ref Month") == 0)                   TEST = DATA_REF_MONTH;
        else if (strcmp(w->mb->text(), "2 Chart Areas") == 0)               TEST = DATA_AREA2;
        else if (strcmp(w->mb->text(), "3 Chart Areas") == 0)               TEST = DATA_AREA3;
        else if (strcmp(w->mb->text(), "Hour") == 0)                        TEST = DATA_HOUR;
        else if (strcmp(w->mb->text(), "Minutes") == 0)                     TEST = DATA_MIN;
        else if (strcmp(w->mb->text(), "Seconds") == 0)                     TEST = DATA_SEC;
        else                                                                TEST = DATA_NONE;

        create_chart(w);
    }
};

void create_chart(Test* w) {
    if (RAND == 0)
        srand(time(nullptr));
    else
        srand(RAND);

    w->chart->clear();

    if (TEST == DATA_LEFT_ALL)                test1(w->chart, "All Days", 100.0, 2.0, Date::RANGE::DAY, Chart::TYPE::LINE, true, 3, 1);
    else if (TEST == DATA_LEFT_WEEKDAY)       test1(w->chart, "Weekdays (VERTICAL)", 100.0, 2.0, Date::RANGE::WEEKDAY, Chart::TYPE::VERTICAL, true, 3, 1);
    else if (TEST == DATA_LEFT_FRIDAY)        test1(w->chart, "Fridays (VERTICAL2 & CLAMP)", 100.0, 1.0, Date::RANGE::FRIDAY, Chart::TYPE::VERTICAL2, true, 5, 3, -50.0, 200.0);
    else if (TEST == DATA_LEFT_SUNDAY)        test1(w->chart, "Sundays (VERTICAL2)", 100.0, 2.0, Date::RANGE::SUNDAY, Chart::TYPE::VERTICAL2, true, 5, 3);
    else if (TEST == DATA_LEFT_MONTH)         test1(w->chart, "Month", 100.0, 2.0, Date::RANGE::MONTH, Chart::TYPE::LINE, true, 10, 3);
    else if (TEST == DATA_LEFT_SMALL_VALUE)   test1(w->chart, "All Days Small Values (VERTICAL)", 0.1, 1.0, Date::RANGE::DAY, Chart::TYPE::VERTICAL, true, 5, 4);
    else if (TEST == DATA_LEFT_LARGE_VALUE)   test1(w->chart, "All Days Large Values (BAR)", 100000000.0, 3.0, Date::RANGE::DAY, Chart::TYPE::BAR, true, 10, 2);
    else if (TEST == DATA_LEFT_LARGE_DATASET) test4(w->chart, 1000.0, 0.5);
    else if (TEST == DATA_RIGHT_ALL)          test1(w->chart, "Right Negative Values (VERTICAL)", -50000.0, 2.0, Date::RANGE::DAY, Chart::TYPE::VERTICAL, false, 3, 1);
    else if (TEST == DATA_RIGHT_SMALL_VALUE)  test1(w->chart, "Right Small Negative Values (HORIZONTAL)", -2.0, 1.0, Date::RANGE::DAY, Chart::TYPE::HORIZONTAL, false, 5, 2);
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

    init_chart(w);
}

std::vector<Price> create_serie(const char* start, const char* stop, double value, double change, const Date::RANGE range) {
    double      price = value;
    double      ch    = 0.0;
    Date        date1 = Date::FromString(start);
    Date        date2 = Date::FromString(stop);
    std::vector<Price> res;

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

        if (rand() % 2 == 0)
            ch = 1.0 + change / (double) (rand() % 100 + 100.0);
        else
            ch = 1.0 - change / (double) (rand() % 100 + 100.0);

        price *= ch;
        // fprintf(stderr, "  date2=%s\n", date2.format(s, 50, Date::FORMAT::ISO_TIME));
    }

    return res;
}

void init_chart(Test* w) {
    bool h = false;
    bool v = false;

    v = w->mb->find_item("&Settings/Vertical Lines")->value();
    h = w->mb->find_item("&Settings/Horizontal Lines")->value();

    w->chart->support_lines(h, v);
    w->chart->zoom(true);
    w->chart->tooltip(true);
    w->chart->init(true);
    // w->chart->debug();

    fflush(stderr);
}

void test1(Chart* chart, const char* label, const double start, const double change, const Date::RANGE range, const Chart::TYPE type, const bool left, const int tick, const int width, const double min, const double max) {
    std::vector<Price> arr1 = create_serie("20010101", "20191231", start, change, range);

    if (left) {
        chart->line(Chart::AREA::A1, Chart::LINE::L1, arr1, label, type, FL_ALIGN_LEFT, FL_BLUE, width);
        chart->margin(start > 1000000.0 ? 8 : 6, 1);
    }
    else {
        chart->line(Chart::AREA::A1, Chart::LINE::L1, arr1, label, type, FL_ALIGN_RIGHT, FL_BLUE, width);
        chart->margin(3, 6);
    }

    chart->clamp(Chart::AREA::A1, Chart::LINE::L1, min, max);
    chart->range(range);
    chart->tick_width(tick);
}

void test2(Chart* chart, const double start, const double change, const Date::RANGE range) {
    std::vector<Price> arr1 = create_serie("20010101", "20091231", start, change, range);
    std::vector<Price> arr2 = create_serie("20010201", "20091130", start, change, range);

    chart->line(Chart::AREA::A1, Chart::LINE::L1, arr1, "Line Chart", Chart::TYPE::LINE, FL_ALIGN_LEFT, FL_BLUE, 2);
    chart->line(Chart::AREA::A1, Chart::LINE::L2, arr2, "Bar Chart", Chart::TYPE::BAR, FL_ALIGN_RIGHT, FL_RED, 2);
    chart->margin(6, 6);
    chart->range(range);
    chart->tick_width(10);
}

void test3(Chart* chart, const char* label, const Date::RANGE range, const bool block) {
    std::vector<Price> arr1;
    std::vector<Price> arr2;
    std::vector<Price> arr3;

    arr1.push_back(Price("20140102", 10)); // Thu    *
    arr1.push_back(Price("20140105", 15)); // Sun    *
    arr1.push_back(Price("20140111", 20)); // Sat    *
    arr1.push_back(Price("20140113", 30)); // Mon |  *
    arr1.push_back(Price("20140116", 25)); // Thu |  *
    arr1.push_back(Price("20140124", 35)); // Fri |  *
    arr1.push_back(Price("20140126", 40)); // Sun    *
    arr1.push_back(Price("20140204", 45)); // Tue
    arr1.push_back(Price("20140318", 50)); // Tue |  *
    arr1.push_back(Price("20140319", 55)); // Wen |  *
    arr1.push_back(Price("20140505", 60)); // Mon
    arr1.push_back(Price("20140510", 30)); // Sat

    if (block) {
        arr3.push_back(Price("20140105"));
        arr3.push_back(Price("20140111"));
        arr3.push_back(Price("20140319"));
    }

    if (range == Date::RANGE::FRIDAY) {
        arr2 = Price::DayToWeek(arr1, Date::DAY::FRIDAY);
        chart->tick_width(25);
    }
    else if (range == Date::RANGE::SUNDAY) {
        arr2 = Price::DayToWeek(arr1, Date::DAY::SUNDAY);
        chart->tick_width(25);
    }
    else if (range == Date::RANGE::MONTH) {
        arr2 = Price::DayToMonth(arr1);
        chart->tick_width(50);
    }
    else {
        arr2 = arr1;
        chart->tick_width(6);
    }

    chart->line(Chart::AREA::A1, Chart::LINE::L1, arr2, label, Chart::TYPE::BAR, FL_ALIGN_LEFT, FL_BLUE, 6);
    chart->range(range);
    chart->margin(6, 1);
    chart->block(arr3);
}

void test4(Chart* chart, const double start, const double change) {
    std::vector<Price> arr1 = create_serie("18000101", "20191231", start, change, Date::RANGE::DAY);
    std::vector<Price> arr2 = create_serie("18100101", "20291231", start, change, Date::RANGE::DAY);
    std::vector<Price> arr3 = create_serie("18200101", "19991231", start, change, Date::RANGE::DAY);

    chart->line(Chart::AREA::A1, Chart::LINE::L1, arr1, "18000101 - 20191231", Chart::TYPE::LINE, FL_ALIGN_LEFT, FL_BLUE, 1);
    chart->line(Chart::AREA::A1, Chart::LINE::L2, arr2, "18100101 - 20291231", Chart::TYPE::LINE, FL_ALIGN_LEFT, FL_GREEN, 1);
    chart->line(Chart::AREA::A1, Chart::LINE::L3, arr3, "18200101 - 19991231", Chart::TYPE::LINE, FL_ALIGN_LEFT, FL_RED, 1);
    chart->margin(6, 1);
    chart->range(Date::RANGE::DAY);
    chart->tick_width(3);
}

void test5(Chart* chart, const bool three) {
    std::vector<Price> arr1 = create_serie("20010101", "20091231", 1000.0, 2.0, Date::RANGE::DAY);
    std::vector<Price> arr2 = create_serie("20010201", "20091130", 1000.0, 1.0, Date::RANGE::DAY);
    std::vector<Price> arr3 = create_serie("20010101", "20091231", 3000000, 5.0, Date::RANGE::DAY);
    std::vector<Price> arr4;

    if (three) {
        arr4 = Price::MovingAverage(arr1, 20);
        chart->area_size(40, 40, 20);
        chart->line(Chart::AREA::A1, Chart::LINE::L1, arr1, "Bar Chart", Chart::TYPE::BAR, FL_ALIGN_LEFT, FL_BLUE, 3);
        chart->line(Chart::AREA::A1, Chart::LINE::L2, arr4, "AVG", Chart::TYPE::LINE, FL_ALIGN_LEFT, FL_DARK_GREEN, 3);
        chart->line(Chart::AREA::A2, Chart::LINE::L1, arr2, "Foo", Chart::TYPE::LINE, FL_ALIGN_LEFT, FL_RED, 3);
        chart->line(Chart::AREA::A3, Chart::LINE::L1, arr3, "Volume", Chart::TYPE::VERTICAL2, FL_ALIGN_LEFT, FL_DARK_GREEN, 3);
        chart->clamp(Chart::AREA::A3, Chart::LINE::L1, 0.0);
        chart->margin(7, 1);
    }
    else {
        arr4 = Price::MovingAverage(arr3, 20);
        chart->area_size(70, 30, 0);
        chart->line(Chart::AREA::A1, Chart::LINE::L1, arr1, "Bar Chart", Chart::TYPE::BAR, FL_ALIGN_LEFT, FL_BLUE, 3);
        chart->line(Chart::AREA::A1, Chart::LINE::L2, arr2, "Foo", Chart::TYPE::LINE, FL_ALIGN_RIGHT, FL_RED, 3);
        chart->line(Chart::AREA::A2, Chart::LINE::L1, arr3, "Volume", Chart::TYPE::VERTICAL2, FL_ALIGN_LEFT, FL_DARK_GREEN, 3);
        chart->line(Chart::AREA::A2, Chart::LINE::L2, arr4, "AVG", Chart::TYPE::LINE, FL_ALIGN_LEFT, FL_BLUE, 3);
        chart->margin(7, 6);
    }

    chart->range(Date::RANGE::DAY);
    chart->tick_width(4);
}

void test6(Chart* chart, const Date::RANGE range) {
    std::vector<Price> arr1 = create_serie("20201012 000000", "20201016 230000", 100.0, 0.5, range);
    // fprintf(stderr, "size=%d\n", arr1.size());

    if (range == Date::RANGE::HOUR) {
        chart->line(Chart::AREA::A1, Chart::LINE::L1, arr1, "Hour", Chart::TYPE::BAR, FL_ALIGN_LEFT, FL_BLUE, 3);
        chart->margin(6, 1);
    }
    else if (range == Date::RANGE::MIN) {
        chart->line(Chart::AREA::A1, Chart::LINE::L1, arr1, "Minutes", Chart::TYPE::BAR, FL_ALIGN_LEFT, FL_BLUE, 3);
        chart->margin(6, 1);
    }
    else if (range == Date::RANGE::SEC) {
        chart->line(Chart::AREA::A1, Chart::LINE::L1, arr1, "Seconds", Chart::TYPE::BAR, FL_ALIGN_LEFT, FL_BLUE, 3);
        chart->margin(6, 1);
    }

    chart->range(range);
    chart->tick_width(4);
}

int main(int argc, const char** argv) {
    if (flw::theme::parse(argc, argv) == false) {
        flw::theme::load("gtk");
    }

    {
        srand(time(0));
        Test win(800, 480);
        Fl::run();
    }

    return 0;
}
