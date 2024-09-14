// Copyright gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#include "flw.h"

#ifndef FLW_AMALGAM
    #include "chart.h"
#endif

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
    DATA_AREA4,
    DATA_AREA5,
    DATA_HOUR,
    DATA_MIN,
    DATA_SEC,
    DATA_LAST,
};

int TEST = DATA_LEFT_ALL;
int RAND = 0;

void callback_timer(void* data);
void create_chart(Test* self);
void test1(Chart* chart, const char* main_label, const char* label, double start, double change, ChartData::RANGE range, ChartLine::TYPE type, Fl_Align align, int tick, int width, double min = INFINITY, double max = INFINITY, double divide = 0.0);
void test2(Chart* chart);
void test3(Chart* chart, const char* label, const ChartData::RANGE range, bool block = false);
void test4(Chart* chart);
void test5(Chart* chart, const char* main_label, int count);
void test6(Chart* chart, ChartData::RANGE range);
void test7(Chart* chart);
void test8(Chart* chart);
void test9(Chart* chart);

//------------------------------------------------------------------------------
class Test : public Fl_Double_Window {
public:
    Fl_Menu_Bar* mb;
    Chart*       chart;

    //--------------------------------------------------------------------------
    Test(int W, int H) : Fl_Double_Window(W, H, "test_chart.cpp") {
        end();

        mb    = new Fl_Menu_Bar(0, 0, 0, 0);
        chart = new Chart();

        add(mb);
        add(chart);

        mb->add("&Test/Autorun", 0, CallbackAuto, this, FL_MENU_DIVIDER);
        mb->add("&Test/save", 0, CallbackSave, this);
        mb->add("&Test/Load", 0, CallbackLoad, this, FL_MENU_DIVIDER);
        mb->add("&Test/srand(time)", 0, CallbackTest, this, FL_MENU_RADIO);
        mb->add("&Test/srand(666)", 0, CallbackTest, this, FL_MENU_RADIO | FL_MENU_DIVIDER);
        mb->add("&Test/All Days (LINE)", 0, CallbackTest, this);
        mb->add("&Test/Step (LINE)", 0, CallbackTest, this);
        mb->add("&Test/10 Colors (LINE)", 0, CallbackTest, this, FL_MENU_DIVIDER);
        mb->add("&Test/Weekdays (BAR)", 0, CallbackTest, this);
        mb->add("&Test/Fridays (BAR_CLAMP)", 0, CallbackTest, this);
        mb->add("&Test/Sundays (BAR_CLAMP)", 0, CallbackTest, this);
        mb->add("&Test/Month", 0, CallbackTest, this, FL_MENU_DIVIDER);
        mb->add("&Test/Very Small Values (LINE)", 0, CallbackTest, this);
        mb->add("&Test/Small Values With Expandable Width (BAR)", 0, CallbackTest, this);
        mb->add("&Test/Small Values (HORIZONTAL)", 0, CallbackTest, this, FL_MENU_DIVIDER);
        mb->add("&Test/Large Values (BAR_HLC)", 0, CallbackTest, this);
        mb->add("&Test/Large Values With Expandable Width (BAR_HLC)", 0, CallbackTest, this);
        mb->add("&Test/3 Lines With Large Dataset", 0, CallbackTest, this, FL_MENU_DIVIDER);
        mb->add("&Test/Right Negative Values (BAR)", 0, CallbackTest, this);
        mb->add("&Test/Right Small (LINE_DOT)", 0, CallbackTest, this);
        mb->add("&Test/Left And Right All Days", 0, CallbackTest, this, FL_MENU_DIVIDER);
        mb->add("&Test/Ref All (BAR_HLC)", 0, CallbackTest, this);
        mb->add("&Test/Ref All (BAR_HLC) + Block", 0, CallbackTest, this);
        mb->add("&Test/Ref Friday (BAR_HLC)", 0, CallbackTest, this);
        mb->add("&Test/Ref Sunday (BAR_HLC)", 0, CallbackTest, this);
        mb->add("&Test/Ref Month (BAR_HLC)", 0, CallbackTest, this, FL_MENU_DIVIDER);
        mb->add("&Test/2 Chart Areas", 0, CallbackTest, this);
        mb->add("&Test/3 Chart Areas", 0, CallbackTest, this);
        mb->add("&Test/4 Chart Areas", 0, CallbackTest, this);
        mb->add("&Test/5 Chart Areas", 0, CallbackTest, this, FL_MENU_DIVIDER);
        mb->add("&Test/Hour", 0, CallbackTest, this);
        mb->add("&Test/Minutes", 0, CallbackTest, this);
        mb->add("&Test/Seconds", 0, CallbackTest, this, FL_MENU_DIVIDER);
        mb->add("&Test/None", 0, CallbackTest, this);
        mb->textfont(flw::PREF_FONT);
        mb->textsize(flw::PREF_FONTSIZE);

        menu::set_item(mb, "&Test/srand(time)", true);
        menu::set_item(mb, "&Test/All Days (LINE)", true);
        mb->textsize(flw::PREF_FONTSIZE);
        util::labelfont(this);

        resize(0, 0, W, H);
        create_chart(this);
        resizable(this);
        size_range(64, 48);
        show();
    }

    //--------------------------------------------------------------------------
    void resize(int X, int Y, int W, int H) override {
        Fl_Double_Window::resize(X, Y, W, H);
        mb->resize(0, 0, W, flw::PREF_FONTSIZE * 2);
        chart->resize(0, flw::PREF_FONTSIZE * 2, W, H - flw::PREF_FONTSIZE * 2);
//         chart->resize(100, 100, W - 200, H - 200);
    }

    //--------------------------------------------------------------------------
    static void CallbackAuto(Fl_Widget*, void* v) {
        auto self = static_cast<Test*>(v);
        TEST = DATA_NONE;
        self->mb->deactivate();
        Fl::add_timeout(0.5, callback_timer, v);
    }

    //--------------------------------------------------------------------------
    static void CallbackLoad(Fl_Widget*, void* v) {
        auto self = static_cast<Test*>(v);
        self->chart->load_json("chart.json");
    }

    //--------------------------------------------------------------------------
    static void CallbackSave(Fl_Widget*, void* v) {
        auto self = static_cast<Test*>(v);
        self->chart->save_json("chart.json");
    }

    //--------------------------------------------------------------------------
    static void CallbackTest(Fl_Widget*, void* v) {
        auto self = static_cast<Test*>(v);

        if (strcmp(self->mb->text(), "srand(time)") == 0)                                          RAND = 0;
        else if (strcmp(self->mb->text(), "srand(666)") == 0)                                      RAND = 666;
        else if (strcmp(self->mb->text(), "All Days (LINE)") == 0)                                 TEST = DATA_LEFT_ALL;
        else if (strcmp(self->mb->text(), "Step (LINE)") == 0)                                     TEST = DATA_LEFT_STEP;
        else if (strcmp(self->mb->text(), "10 Colors (LINE)") == 0)                                TEST = DATA_RIGHT_10;
        else if (strcmp(self->mb->text(), "Weekdays (BAR)") == 0)                                  TEST = DATA_LEFT_WEEKDAY;
        else if (strcmp(self->mb->text(), "Fridays (BAR_CLAMP)") == 0)                             TEST = DATA_LEFT_FRIDAY;
        else if (strcmp(self->mb->text(), "Sundays (BAR_CLAMP)") == 0)                             TEST = DATA_LEFT_SUNDAY;
        else if (strcmp(self->mb->text(), "Month") == 0)                                           TEST = DATA_LEFT_MONTH;
        else if (strcmp(self->mb->text(), "Very Small Values (LINE)") == 0)                        TEST = DATA_LEFT_SMALL_VALUE;
        else if (strcmp(self->mb->text(), "Small Values With Expandable Width (BAR)") == 0)        TEST = DATA_LEFT_SMALL_VALUE2;
        else if (strcmp(self->mb->text(), "Small Values (HORIZONTAL)") == 0)                       TEST = DATA_LEFT_SMALL_VALUE3;
        else if (strcmp(self->mb->text(), "Large Values (BAR_HLC)") == 0)                          TEST = DATA_LEFT_LARGE_VALUE;
        else if (strcmp(self->mb->text(), "Large Values With Expandable Width (BAR_HLC)") == 0)    TEST = DATA_LEFT_LARGE_VALUE2;
        else if (strcmp(self->mb->text(), "3 Lines With Large Dataset") == 0)                      TEST = DATA_LEFT_LARGE_DATASET;
        else if (strcmp(self->mb->text(), "Right Negative Values (BAR)") == 0)                     TEST = DATA_RIGHT_ALL;
        else if (strcmp(self->mb->text(), "Right Small (LINE_DOT)") == 0)                          TEST = DATA_RIGHT_SMALL_VALUE;
        else if (strcmp(self->mb->text(), "Left And Right All Days") == 0)                         TEST = DATA_LEFT_RIGHT_ALL;
        else if (strcmp(self->mb->text(), "Ref All (BAR_HLC)") == 0)                               TEST = DATA_REF_ALL;
        else if (strcmp(self->mb->text(), "Ref All (BAR_HLC) + Block") == 0)                       TEST = DATA_REF_ALL_BLOCK;
        else if (strcmp(self->mb->text(), "Ref Friday (BAR_HLC)") == 0)                            TEST = DATA_REF_FRIDAY;
        else if (strcmp(self->mb->text(), "Ref Sunday (BAR_HLC)") == 0)                            TEST = DATA_REF_SUNDAY;
        else if (strcmp(self->mb->text(), "Ref Month (BAR_HLC)") == 0)                             TEST = DATA_REF_MONTH;
        else if (strcmp(self->mb->text(), "2 Chart Areas") == 0)                                   TEST = DATA_AREA2;
        else if (strcmp(self->mb->text(), "3 Chart Areas") == 0)                                   TEST = DATA_AREA3;
        else if (strcmp(self->mb->text(), "4 Chart Areas") == 0)                                   TEST = DATA_AREA4;
        else if (strcmp(self->mb->text(), "5 Chart Areas") == 0)                                   TEST = DATA_AREA5;
        else if (strcmp(self->mb->text(), "Hour") == 0)                                            TEST = DATA_HOUR;
        else if (strcmp(self->mb->text(), "Minutes") == 0)                                         TEST = DATA_MIN;
        else if (strcmp(self->mb->text(), "Seconds") == 0)                                         TEST = DATA_SEC;
        else                                                                                       TEST = DATA_NONE;

        create_chart(self);
    }
};

//------------------------------------------------------------------------------
void callback_timer(void* data) {
    auto self = static_cast<Test*>(data);

    TEST++;

    if (TEST == DATA_LAST) {
        Fl::remove_timeout(callback_timer, data);
        self->mb->activate();
        TEST = DATA_LEFT_ALL;
    }
    else {
        Fl::repeat_timeout(2.0, callback_timer, data);
    }

    create_chart(self);
}

//------------------------------------------------------------------------------
void create_chart(Test* self) {
    if (RAND == 0) srand(time(nullptr));
    else           srand(RAND);

    self->chart->clear();

    if (TEST == DATA_LEFT_ALL)                test1(self->chart, "First Test", "All Days (LINE)", 100.0, 2.0, ChartData::RANGE::DAY, ChartLine::TYPE::LINE, FL_ALIGN_LEFT, 3, 2);
    else if (TEST == DATA_LEFT_STEP)          test8(self->chart);
    else if (TEST == DATA_RIGHT_10)           test9(self->chart);
    else if (TEST == DATA_LEFT_WEEKDAY)       test1(self->chart, "", "Weekdays (BAR)", 100.0, 2.0, ChartData::RANGE::WEEKDAY, ChartLine::TYPE::BAR, FL_ALIGN_LEFT, 11, 6);
    else if (TEST == DATA_LEFT_FRIDAY)        test1(self->chart, "", "Fridays (BAR_CLAMP)", 100.0, 1.0, ChartData::RANGE::FRIDAY, ChartLine::TYPE::BAR_CLAMP, FL_ALIGN_LEFT, 5, 3, -50.0, 200.0);
    else if (TEST == DATA_LEFT_SUNDAY)        test1(self->chart, "", "Sundays (BAR_CLAMP)", 100.0, 2.0, ChartData::RANGE::SUNDAY, ChartLine::TYPE::BAR_CLAMP, FL_ALIGN_LEFT, 5, 3);
    else if (TEST == DATA_LEFT_MONTH)         test1(self->chart, "", "Month", 100.0, 2.0, ChartData::RANGE::MONTH, ChartLine::TYPE::LINE, FL_ALIGN_LEFT, 10, 3);
    else if (TEST == DATA_LEFT_SMALL_VALUE)   test1(self->chart, "", "Very Small Values (LINE)", 100.0, 3.0, ChartData::RANGE::DAY, ChartLine::TYPE::LINE, FL_ALIGN_LEFT, 5, 4, INFINITY, INFINITY, 10000000.0);
    else if (TEST == DATA_LEFT_SMALL_VALUE2)  test1(self->chart, "", "Small Values (Expandable BAR)", 0.1, 1.0, ChartData::RANGE::DAY, ChartLine::TYPE::BAR, FL_ALIGN_LEFT, 5, 0);
    else if (TEST == DATA_LEFT_LARGE_VALUE)   test1(self->chart, "", "Large Values (BAR_HLC)", 1000000000.0, 3.0, ChartData::RANGE::DAY, ChartLine::TYPE::BAR_HLC, FL_ALIGN_LEFT, 10, 6);
    else if (TEST == DATA_LEFT_LARGE_VALUE2)  test1(self->chart, "", "Large Values (BAR_HLC)", 1000000000.0, 3.0, ChartData::RANGE::DAY, ChartLine::TYPE::BAR_HLC, FL_ALIGN_LEFT, 10, 0);
    else if (TEST == DATA_LEFT_SMALL_VALUE3)  test1(self->chart, "", "Small Values (HORIZONTAL))", 0.1, 1.0, ChartData::RANGE::DAY, ChartLine::TYPE::HORIZONTAL, FL_ALIGN_LEFT, 15, 2);
    else if (TEST == DATA_LEFT_LARGE_DATASET) test4(self->chart);
    else if (TEST == DATA_RIGHT_ALL)          test1(self->chart, "", "Right Negative Values (BAR)", -50000.0, 2.0, ChartData::RANGE::DAY, ChartLine::TYPE::BAR, FL_ALIGN_RIGHT, 3, 1);
    else if (TEST == DATA_RIGHT_SMALL_VALUE)  test7(self->chart);
    else if (TEST == DATA_LEFT_RIGHT_ALL)     test2(self->chart);
    else if (TEST == DATA_REF_ALL)            test3(self->chart, "Ref Data All", ChartData::RANGE::DAY);
    else if (TEST == DATA_REF_ALL_BLOCK)      test3(self->chart, "Ref Data All + Block", ChartData::RANGE::DAY, true);
    else if (TEST == DATA_REF_FRIDAY)         test3(self->chart, "Ref Data Friday", ChartData::RANGE::FRIDAY);
    else if (TEST == DATA_REF_SUNDAY)         test3(self->chart, "Ref Data Sunday", ChartData::RANGE::SUNDAY);
    else if (TEST == DATA_REF_MONTH)          test3(self->chart, "Ref Data Month", ChartData::RANGE::MONTH);
    else if (TEST == DATA_AREA2)              test5(self->chart, "Two Chart Areas", 2);
    else if (TEST == DATA_AREA3)              test5(self->chart, "Three Chart Areas", 3);
    else if (TEST == DATA_AREA4)              test5(self->chart, "Four Chart Areas", 4);
    else if (TEST == DATA_AREA5)              test5(self->chart, "Five Chart Areas", 5);
    else if (TEST == DATA_HOUR)               test6(self->chart, ChartData::RANGE::HOUR);
    else if (TEST == DATA_MIN)                test6(self->chart, ChartData::RANGE::MIN);
    else if (TEST == DATA_SEC)                test6(self->chart, ChartData::RANGE::SEC);

    self->chart->take_focus();
}

//------------------------------------------------------------------------------
ChartDataVector create_serie1(const char* start, const char* stop, double value, double change, const ChartData::RANGE range, double divide = 0.0) {
    auto close = value;
    auto ch    = 0.0;
    auto date1 = Date::FromString(start);
    auto date2 = Date::FromString(stop);
    auto res   = ChartDataVector();

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
            res.push_back(ChartData(date1.format(Date::FORMAT::ISO_TIME), high / divide, low / divide, close / divide));
        }
        else {
            res.push_back(ChartData(date1.format(Date::FORMAT::ISO_TIME), high, low, close));
        }

        if (range == ChartData::RANGE::HOUR) {
            date1.add_seconds(Date::SECS_PER_HOUR);
        }
        else if (range == ChartData::RANGE::MIN) {
            date1.add_seconds(60);
        }
        else if (range == ChartData::RANGE::SEC) {
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
ChartDataVector create_serie2(const char* start, const char* stop, double d = 1.0) {
    auto res   = ChartDataVector();
    auto f     = 1.0;
    auto date1 = Date::FromString(start);
    auto date2 = Date::FromString(stop);

    while (date1 <= date2) {
        res.push_back(ChartData(date1.format(Date::FORMAT::ISO_TIME), sinl(f) + d));
        f += d;
        date1.add_days(1);
    }

    return res;
}

//------------------------------------------------------------------------------
void test1(Chart* chart, const char* main_label, const char* label, const double start, const double change, const ChartData::RANGE range, const ChartLine::TYPE type, Fl_Align align, const int tick, const int width, const double min, const double max, const double divide) {
    auto vec1  = create_serie1("20010101", "20191231", start, change, range, divide);
    auto line1 = ChartLine(vec1, label, type);

    line1.set_align(align).set_color(color::ROYALBLUE).set_width(width).set_clamp(min, max);
    chart->add_line(ChartArea::NUM::ONE, line1);

    if (start > 100000000.0) {
        chart->set_margin(10);
    }
    else if (start > 1000000.0) {
        chart->set_margin(8);
    }
    else if (divide > 100.0) {
        chart->set_margin(10);
    }
    else {
        chart->set_margin(Chart::DEF_MARGIN);
    }

    chart->set_date_range(range);
    chart->set_tick_width(tick);
    chart->set_label(main_label);
    chart->init(true);
}

//------------------------------------------------------------------------------
void test2(Chart* chart) {
    auto vec1  = create_serie1("20010101", "20091231", -2000, 3, ChartData::RANGE::DAY);
    auto vec2  = create_serie1("20010201", "20091130", -4000, 5, ChartData::RANGE::DAY);
    auto vec3  = ChartData::Fixed(vec1, 0);
    vec1 = ChartData::Modify(vec1, ChartData::MODIFY::ADDITION, 1000);
    vec2 = ChartData::Modify(vec2, ChartData::MODIFY::ADDITION, 2000);
    auto line1 = ChartLine(vec1, "Line Chart");
    auto line2 = ChartLine(vec2, "Bar Chart", ChartLine::TYPE::BAR_HLC);
    auto line3 = ChartLine(vec3, "ZERO", ChartLine::TYPE::EXPAND_HORIZONTAL);

    
    line1.set_color(color::ROYALBLUE).set_width(2);
    line2.set_align(FL_ALIGN_RIGHT).set_color(color::CRIMSON).set_width(2);
    line3.set_color(FL_GREEN).set_width(3);

    chart->add_line(ChartArea::NUM::ONE, line1);
    chart->add_line(ChartArea::NUM::ONE, line2);
    chart->add_line(ChartArea::NUM::ONE, line3);
    chart->set_margin(Chart::DEF_MARGIN);
    chart->set_date_range(ChartData::RANGE::DAY);
    chart->set_tick_width(10);
    chart->init(true);
}

//------------------------------------------------------------------------------
void test3(Chart* chart, const char* label, const ChartData::RANGE range, const bool block) {
    auto vec1 = ChartDataVector();
    auto vec2 = ChartDataVector();
    auto vec3 = ChartDataVector();

    vec1.push_back(ChartData("20140102", 10)); // Thu    *
    vec1.push_back(ChartData("20140105", 15)); // Sun    *
    vec1.push_back(ChartData("20140111", 20)); // Sat    *
    vec1.push_back(ChartData("20140113", 30)); // Mon |  *
    vec1.push_back(ChartData("20140116", 25)); // Thu |  *
    vec1.push_back(ChartData("20140124", 35)); // Fri |  *
    vec1.push_back(ChartData("20140126", 40)); // Sun    *
    vec1.push_back(ChartData("20140204", 45)); // Tue
    vec1.push_back(ChartData("20140318", 50)); // Tue |  *
    vec1.push_back(ChartData("20140319", 55)); // Wen |  *
    vec1.push_back(ChartData("20140505", 60)); // Mon
    vec1.push_back(ChartData("20140510", 30)); // Sat

    if (block == true) {
        vec3.push_back(ChartData("20140105"));
        vec3.push_back(ChartData("20140111"));
        vec3.push_back(ChartData("20140319"));
    }

    if (range == ChartData::RANGE::FRIDAY) {
        vec2 = ChartData::DayToWeek(vec1, Date::DAY::FRIDAY);
        chart->set_tick_width(25);
    }
    else if (range == ChartData::RANGE::SUNDAY) {
        vec2 = ChartData::DayToWeek(vec1, Date::DAY::SUNDAY);
        chart->set_tick_width(25);
    }
    else if (range == ChartData::RANGE::MONTH) {
        vec2 = ChartData::DayToMonth(vec1);
        chart->set_tick_width(Chart::MAX_TICK);
    }
    else {
        vec2 = vec1;
        chart->set_tick_width(6);
    }

    auto line1 = ChartLine(vec2, label, ChartLine::TYPE::BAR_HLC);
    line1.set_color(color::ROYALBLUE).set_width(6);

    chart->add_line(ChartArea::NUM::ONE, line1);
    chart->set_date_range(range);
    chart->set_margin(Chart::DEF_MARGIN);
    chart->block_dates(vec3);
    chart->init(true);
}

//------------------------------------------------------------------------------
void test4(Chart* chart) {
    auto vec1  = create_serie1("18000101", "20191231", 1000.0, 0.5, ChartData::RANGE::DAY);
    auto vec2  = create_serie1("18100101", "20291231", 1000.0, 0.5, ChartData::RANGE::DAY);
    auto vec3  = create_serie1("18200101", "19991231", 1000.0, 0.5, ChartData::RANGE::DAY);
    auto line1 = ChartLine(vec1);
    auto line2 = ChartLine(vec2);
    auto line3 = ChartLine(vec3);

    line1.set_label("18000101 - 20191231").set_type(ChartLine::TYPE::LINE).set_color(color::ROYALBLUE).set_width(1);
    line2.set_label("18100101 - 20291231").set_type(ChartLine::TYPE::LINE).set_color(FL_GREEN).set_width(1);
    line3.set_label("18200101 - 19991231").set_type(ChartLine::TYPE::LINE).set_color(color::CRIMSON).set_width(1);

    chart->add_line(ChartArea::NUM::ONE, line1);
    chart->add_line(ChartArea::NUM::ONE, line2);
    chart->add_line(ChartArea::NUM::ONE, line3);
    chart->set_margin(Chart::DEF_MARGIN);
    chart->set_date_range(ChartData::RANGE::DAY);
    chart->set_tick_width(3);
    chart->init(true);
}

//------------------------------------------------------------------------------
void test5(Chart* chart, const char* main_label, const int count) {
    auto vec1  = create_serie1("20010101", "20091231", 1000.0, 2.0, ChartData::RANGE::DAY);
    auto vec2  = create_serie1("20010201", "20091130", 1500000.0, 2.0, ChartData::RANGE::DAY);
    auto vec3  = create_serie1("20010101", "20091231", 3000000, 5.0, ChartData::RANGE::DAY);
    auto vec4  = ChartData::MovingAverage(vec3, 20);
    auto line1 = ChartLine(vec1, "Bar HLC Chart", ChartLine::TYPE::BAR_HLC);
    auto line2 = ChartLine(vec2, "Line Chart");
    auto line3 = ChartLine(vec3, "Volume", ChartLine::TYPE::BAR_CLAMP);
    auto line4 = ChartLine(vec4, "AVG");

    line1.set_color(color::ROYALBLUE).set_width(3);
    line2.set_color(color::CRIMSON).set_width(3);
    line3.set_color(color::OLIVE).set_width(3).set_clamp(0.0);
    line4.set_color(color::VIOLET).set_width(3);

    if (count == 2) {
        line2.set_align(FL_ALIGN_RIGHT);
        line3.set_width(0);

        chart->set_area_size(70, 30, 0);
        chart->add_line(ChartArea::NUM::ONE, line1);
        chart->add_line(ChartArea::NUM::ONE, line2);
        chart->add_line(ChartArea::NUM::TWO, line3);
        chart->add_line(ChartArea::NUM::TWO, line4);
    }
    else if (count == 3) {
        chart->set_area_size(40, 40, 20);
        chart->add_line(ChartArea::NUM::ONE, line1);
        chart->add_line(ChartArea::NUM::TWO, line2);
        chart->add_line(ChartArea::NUM::THREE, line3);
        chart->add_line(ChartArea::NUM::THREE, line4);
    }
    else if (count == 4) {
        chart->set_area_size(30, 30, 20, 20);
        chart->add_line(ChartArea::NUM::ONE, line1);
        chart->add_line(ChartArea::NUM::TWO, line2);
        chart->add_line(ChartArea::NUM::THREE, line3);        
        chart->add_line(ChartArea::NUM::FOUR, line4);
    }
    else if (count == 5) {
        line4.set_align(FL_ALIGN_RIGHT);
        chart->set_area_size(20, 20, 20, 20, 20);
        chart->add_line(ChartArea::NUM::ONE, line1);
        chart->add_line(ChartArea::NUM::TWO, line2);
        chart->add_line(ChartArea::NUM::THREE, line3);        
        chart->add_line(ChartArea::NUM::FOUR, line1);
        chart->add_line(ChartArea::NUM::FIVE, line4);
    }

    chart->set_margin(7);
    chart->set_date_range(ChartData::RANGE::DAY);
    chart->set_tick_width(4);
    chart->set_label(main_label);
    chart->init(true);
}

//------------------------------------------------------------------------------
void test6(Chart* chart, const ChartData::RANGE range) {
    auto vec1  = create_serie1("20201012 000000", "20201016 230000", 100.0, 0.5, range);
    auto line1 = ChartLine(vec1);

    if (range == ChartData::RANGE::HOUR) {
        line1.set_label("Hours");
    }
    else if (range == ChartData::RANGE::MIN) {
        line1.set_label("Minutes");
    }
    else if (range == ChartData::RANGE::SEC) {
        line1.set_label("Seconds");
    }

    line1.set_type(ChartLine::TYPE::BAR_HLC).set_color(color::ROYALBLUE).set_width(4);

    chart->add_line(ChartArea::NUM::ONE, line1);
    chart->set_margin(Chart::DEF_MARGIN);
    chart->set_date_range(range);
    chart->set_tick_width(8);
    chart->init(true);
}

//------------------------------------------------------------------------------
void test7(Chart* chart) {
    auto vec1  = ChartDataVector();
    auto date1 = Date::FromString("20010101");
    auto date2 = Date::FromString("20021231");
    auto p     = -3.5;

    while (date1 <= date2) {
        vec1.push_back(ChartData(date1.format(Date::FORMAT::ISO).c_str(), p));
        date1.add_days(1);

        if (rand() % 3 == 1) {
            p -= rand() % 5 / 300.0;
        }
        else {
            p += rand() % 5 / 100.0;
        }
    }

    auto line1 = ChartLine(vec1, "Right Small (LINE_DOT)", ChartLine::TYPE::LINE_DOT);
    line1.set_align(FL_ALIGN_RIGHT).set_color(color::ROYALBLUE).set_width(2);

    chart->add_line(ChartArea::NUM::ONE, line1);
    chart->set_margin(Chart::DEF_MARGIN);
    chart->set_date_range(ChartData::RANGE::DAY);
    chart->set_tick_width(6);
    chart->init(true);
}

//------------------------------------------------------------------------------
void test8(Chart* chart) {
    auto vec1  = create_serie1("20010101", "20011231", 10.0, 1.0, ChartData::RANGE::DAY);
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

    auto line1 = ChartLine(vec1, "Step (LINE)", ChartLine::TYPE::LINE).set_color(color::ROYALBLUE).set_width(4);

    chart->add_line(ChartArea::NUM::ONE, line1);
    chart->set_margin(Chart::DEF_MARGIN);
    chart->set_date_range(ChartData::RANGE::DAY);
    chart->set_tick_width(25);
    chart->init(true);
}

//------------------------------------------------------------------------------
void test9(Chart* chart) {
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
    auto line1  = ChartLine(vec1, "One (Clamp)");
    auto line2  = ChartLine(vec2, "Two");
    auto line3  = ChartLine(vec3, "Three");
    auto line4  = ChartLine(vec4, "Four");
    auto line5  = ChartLine(vec5, "Five");
    auto line6  = ChartLine(vec6, "Six");
    auto line7  = ChartLine(vec7, "Seven");
    auto line8  = ChartLine(vec8, "Eight");
    auto line9  = ChartLine(vec9, "Nine");
    auto line10 = ChartLine(vec10, "Ten");

    line1.set_align(FL_ALIGN_RIGHT).set_color(color::ROYALBLUE).set_width(4).set_clamp(-1, 12);
    line2.set_align(FL_ALIGN_RIGHT).set_color(color::CRIMSON).set_width(4);
    line3.set_align(FL_ALIGN_RIGHT).set_color(color::GOLD).set_width(4);
    line4.set_align(FL_ALIGN_RIGHT).set_color(color::TURQUOISE).set_width(4);
    line5.set_align(FL_ALIGN_RIGHT).set_color(color::VIOLET).set_width(4);
    line6.set_align(FL_ALIGN_RIGHT).set_color(color::PINK).set_width(2);
    line7.set_align(FL_ALIGN_RIGHT).set_color(color::OLIVE).set_width(4);
    line8.set_align(FL_ALIGN_RIGHT).set_color(color::CHOCOLATE).set_width(4);
    line9.set_align(FL_ALIGN_RIGHT).set_color(color::PINK).set_width(4);
    line10.set_align(FL_ALIGN_RIGHT).set_color(color::TEAL).set_width(4);

    chart->add_line(ChartArea::NUM::ONE, line1);
    chart->add_line(ChartArea::NUM::ONE, line2);
    chart->add_line(ChartArea::NUM::ONE, line3);
    chart->add_line(ChartArea::NUM::ONE, line4);
    chart->add_line(ChartArea::NUM::ONE, line5);
    chart->add_line(ChartArea::NUM::ONE, line6);
    chart->add_line(ChartArea::NUM::ONE, line7);
    chart->add_line(ChartArea::NUM::ONE, line8);
    chart->add_line(ChartArea::NUM::ONE, line9);
    chart->add_line(ChartArea::NUM::ONE, line10);
    chart->set_margin(5);
    chart->set_date_range(ChartData::RANGE::DAY);
    chart->set_tick_width(30);
    chart->init(true);
}

//------------------------------------------------------------------------------
int main(int argc, const char** argv) {
    if (flw::theme::parse(argc, argv) == false) {
        flw::theme::load("oxy");
    }

    {
        Test win(960, 540);
        Fl::run();
    }

    return 0;
}
