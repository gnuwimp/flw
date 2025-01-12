// Copyright gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#include "flw.h"

#ifndef FLW_AMALGAM
    #include "plot.h"
    #include "dlg.h"
#endif

#include <FL/Fl.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Menu_Bar.H>
#include <assert.h>
#include <time.h>

using namespace flw;

#define TEST_CUSTOM_X   "Custom X (VECTOR && LINE_DASH)"
#define TEST_CUSTOM_Y   "Custom Y (VECTOR)"
#define TEST_EMPTY      "Empty"
#define TEST_LARGE1     "Large numbers"
#define TEST_LARGE2     "Large numbers, small diff"
#define TEST_RANGE1     "Lines (LINE_WITH_SQUARE)"
#define TEST_RANGE2     "Negative && Positive (SQUARE)"
#define TEST_RANGE3     "10 Lines (LINE)"
#define TEST_REF1       "Ref (CIRCLE && SQUARE)"
#define TEST_REF2       "Ref + clamp (CIRCLE && SQUARE)"
#define TEST_SMALL1     "Very small numbers (CIRCLE)"
#define TEST_SMALL2     "Small numbers (FILLED_CIRCLE && SQUARE)"
#define TEST_VECTOR1    "Random pairs (VECTOR)"
#define TEST_VECTOR2    "Bar (VECTOR)"
#define TEST_FIRST      TEST_RANGE1
#define TEST_LAST       TEST_EMPTY

int         RAND = 0;
std::string TEST = TEST_RANGE1;

//------------------------------------------------------------------------------
StringVector create_custom_labels() {
    StringVector res;

    res.push_back("");
    res.push_back("January");
    res.push_back("February");
    res.push_back("Mars_");
    res.push_back("April");
    res.push_back("May");
    res.push_back("June");
    res.push_back("July");
    res.push_back("August");
    res.push_back("September");
    res.push_back("October");
    res.push_back("November");
    res.push_back("December");
    res.push_back("");

    return res;
}

//------------------------------------------------------------------------------
PlotDataVector create_custom_x() {
    PlotDataVector res;

    res.push_back(PlotData(0.0, 0.0));
    res.push_back(PlotData(0.0, 0.0));

    for (int f = 1; f <= 12; f++) {
        res.push_back(PlotData(f + 0.0, 0.0));
        res.push_back(PlotData(f + 0.0, rand() % 100));
    }

    res.push_back(PlotData(13.0, 0.0));
    res.push_back(PlotData(13.0, 0.0));

    return res;
}

//------------------------------------------------------------------------------
PlotDataVector create_large1(int64_t maxx, int64_t maxy) {
    PlotDataVector res;

    auto x = rand() % maxx + maxx;

    for (int f = 0; f < 50; f++) {
#ifdef _WIN32
        res.push_back(PlotData(f * x, (rand() % maxy) * (rand() % maxx)));
#else
        res.push_back(PlotData(f * x, rand() % maxy));
#endif
    }

    return res;
}

//------------------------------------------------------------------------------
PlotDataVector create_line(int x, int maxy = 1'000) {
    PlotDataVector res;

    for (int f = x; f <= 30; f += 2) {
        res.push_back(PlotData(f, rand() % maxy * 2 - maxy));
    }

    return res;
}

//------------------------------------------------------------------------------
PlotDataVector create_point(double x, double y) {
    PlotDataVector res;
    res.push_back(PlotData(x, y));

    return res;
}

//------------------------------------------------------------------------------
PlotDataVector create_random_small(int maxy = 2'440) {
    PlotDataVector res;

    auto maxx = 1'250;

    for (int f = 0; f < 20; f++) {
        res.push_back(PlotData(rand() % maxx, (rand() % maxy) + maxy / 2));
    }

    return res;
}

/***
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
    Fl_Menu_Bar* menu;
    Plot*        plot;

    Test(int W, int H) : Fl_Double_Window(W, H, "test_plot.cpp") {
        menu = new Fl_Menu_Bar(0, 0, 0, 0);
        plot = new Plot(0, 0, W, H);

        add(menu);
        add(plot);

        menu->add("&Test/Autorun",              0, Test::CallbackAuto, this, FL_MENU_DIVIDER);
        menu->add("&Test/Save",                 0, Test::CallbackSave, this);
        menu->add("&Test/Load",                 0, Test::CallbackLoad, this, FL_MENU_DIVIDER);

        menu->add("&Test/" TEST_RANGE1,         0, Test::CallbackTest, this);
        menu->add("&Test/" TEST_RANGE2,         0, Test::CallbackTest, this);
        menu->add("&Test/" TEST_RANGE3,         0, Test::CallbackTest, this, FL_MENU_DIVIDER);
        menu->add("&Test/" TEST_REF1,           0, Test::CallbackTest, this);
        menu->add("&Test/" TEST_REF2,           0, Test::CallbackTest, this, FL_MENU_DIVIDER);
        menu->add("&Test/" TEST_LARGE1,         0, Test::CallbackTest, this);
        menu->add("&Test/" TEST_LARGE2,         0, Test::CallbackTest, this, FL_MENU_DIVIDER);
        menu->add("&Test/" TEST_SMALL1,         0, Test::CallbackTest, this);
        menu->add("&Test/" TEST_SMALL2,         0, Test::CallbackTest, this, FL_MENU_DIVIDER);
        menu->add("&Test/" TEST_VECTOR1,        0, Test::CallbackTest, this);
        menu->add("&Test/" TEST_VECTOR2,        0, Test::CallbackTest, this, FL_MENU_DIVIDER);
        menu->add("&Test/" TEST_CUSTOM_X,       0, Test::CallbackTest, this);
        menu->add("&Test/" TEST_CUSTOM_Y,       0, Test::CallbackTest, this, FL_MENU_DIVIDER);
        menu->add("&Test/" TEST_EMPTY,          0, Test::CallbackTest, this);

        menu->add("&Settings/srand(time)",      0, Test::CallbackRandTime, this, FL_MENU_RADIO);
        menu->add("&Settings/srand(666)",       0, Test::CallbackRand666, this, FL_MENU_RADIO | FL_MENU_DIVIDER);
        menu->add("&Settings/Theme...",         0, Test::CallbackTheme, this);
        menu->add("&Settings/Clear labels",     0, Test::CallbackLabels, this, FL_MENU_DIVIDER);
        menu->add("&Settings/Font 10",          0, Test::Callback10, this);
        menu->add("&Settings/Font 14",          0, Test::Callback14, this);
        menu->add("&Settings/Font 24",          0, Test::Callback24, this);
//        menu->add("&Settings/Debug",            0, Test::CallbackDebug, this);

        menu->textfont(flw::PREF_FONT);
        menu->textsize(flw::PREF_FONTSIZE);
        menu::set_item(menu, "&Settings/srand(time)", true);
        create_plot();

        resizable(this);
        size_range(64, 48);
        show();
        resize(300, 100, W, H);
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
    static void CallbackAuto(Fl_Widget*, void* v) {
        auto self = static_cast<Test*>(v);
        TEST = "";
        self->menu->deactivate();
        self->plot->disable_menu();
        Fl::add_timeout(0.5, Test::CallbackTimer, v);
    }

    //--------------------------------------------------------------------------
    static void CallbackDebug(Fl_Widget*, void* v) {
        auto self = static_cast<Test*>(v);
        flw::PREF_FONTSIZE = 24;
        flw::PREF_FIXED_FONTSIZE = 24;
        self->update_pref();
        self->size(800, 600);
        TEST = TEST_LARGE2;
        self->create_plot();
    }

    //--------------------------------------------------------------------------
    static void CallbackRand666(Fl_Widget*, void* v) {
        RAND = 666;
        static_cast<Test*>(v)->create_plot();
    }

    //--------------------------------------------------------------------------
    static void CallbackLabels(Fl_Widget*, void* v) {
        auto self = static_cast<Test*>(v);
        self->plot->set_label("");
        self->plot->xscale().set_label("");
        self->plot->yscale().set_label("");
        self->plot->update_pref();
        self->plot->resize();
    }

    //--------------------------------------------------------------------------
    static void CallbackLoad(Fl_Widget*, void* v) {
        auto self = static_cast<Test*>(v);
        self->plot->load_json("plot.json");
    }

    //--------------------------------------------------------------------------
    static void CallbackRandTime(Fl_Widget*, void* v) {
        RAND = 0;
        static_cast<Test*>(v)->create_plot();
    }

    //--------------------------------------------------------------------------
    static void CallbackSave(Fl_Widget*, void* v) {
        auto self = static_cast<Test*>(v);
        self->plot->save_json("plot.json");
    }

    //--------------------------------------------------------------------------
    static void CallbackTest(Fl_Widget*, void* v) {
        auto self = (Test*) v;
        TEST = util::to_string(self->menu->text());
        self->create_plot();
    }

    //--------------------------------------------------------------------------
    static void CallbackTheme(Fl_Widget*, void* v) {
        auto self = static_cast<Test*>(v);
        dlg::theme(true, true);
        self->update_pref();
    }

    //--------------------------------------------------------------------------
    static void CallbackTimer(void* v) {
        auto self = static_cast<Test*>(v);

        if (TEST == TEST_LAST) {
            TEST = TEST_FIRST;
            self->menu->activate();
            self->plot->enable_menu();
            Fl::remove_timeout(Test::CallbackTimer, v);
        }
        else {
            if (TEST == "")                 TEST = TEST_RANGE1;
            else if (TEST == TEST_RANGE1)   TEST = TEST_RANGE2;
            else if (TEST == TEST_RANGE2)   TEST = TEST_RANGE3;
            else if (TEST == TEST_RANGE3)   TEST = TEST_REF1;
            else if (TEST == TEST_REF1)     TEST = TEST_REF2;
            else if (TEST == TEST_REF2)     TEST = TEST_LARGE1;
            else if (TEST == TEST_LARGE1)   TEST = TEST_LARGE2;
            else if (TEST == TEST_LARGE2)   TEST = TEST_SMALL1;
            else if (TEST == TEST_SMALL1)   TEST = TEST_SMALL2;
            else if (TEST == TEST_SMALL2)   TEST = TEST_VECTOR1;
            else if (TEST == TEST_VECTOR1)  TEST = TEST_VECTOR2;
            else if (TEST == TEST_VECTOR2)  TEST = TEST_CUSTOM_X;
            else if (TEST == TEST_CUSTOM_X) TEST = TEST_CUSTOM_Y;
            else if (TEST == TEST_CUSTOM_Y) TEST = TEST_EMPTY;

            Fl::repeat_timeout(2.0, CallbackTimer, v);
        }

        self->create_plot();
    }

    //--------------------------------------------------------------------------
    void create_plot() {
        if (RAND == 0) {
            srand(time(nullptr));
        }
        else {
            srand(RAND);
        }

        plot->reset();

        if (TEST == TEST_CUSTOM_X) {
            plot->add_line(PlotLine(create_custom_x(), "TYPE::VECTOR", PlotLine::TYPE::VECTOR, color::TEAL, 21));
            plot->add_line(PlotLine(create_custom_x(), "TYPE::LINE_DASH", PlotLine::TYPE::LINE_DASH, color::CRIMSON, 3));
            plot->xscale().set_custom_labels(create_custom_labels());
            plot->xscale().set_label("Custom _X_ Labels For First Data Serie");
            plot->xscale().set_color(color::CRIMSON);
            plot->yscale().set_label("Custom X Labels");
            plot->yscale().set_color(color::TEAL);
            plot->set_label("Custom X Labels");
        }
        else if (TEST == TEST_CUSTOM_Y) {
            PlotDataVector vec;

            vec.push_back(PlotData(0.0, 0.0));
            vec.push_back(PlotData(0.0, 0.0));

            for (int f = 1; f <= 12; f++) {
                vec.push_back(PlotData(rand() % 100, f));
                vec.push_back(PlotData(0.0, f));
            }

            vec.push_back(PlotData(0, 8));
            vec.push_back(PlotData(0, 8));
            
            plot->add_line(PlotLine(vec, "TYPE__VECTOR", PlotLine::TYPE::VECTOR, color::TEAL, 21));
            plot->yscale().set_custom_labels(create_custom_labels());
            plot->xscale().set_label("Custom _Y_ Labels");
            plot->xscale().set_color(color::CRIMSON);
            plot->yscale().set_label("Custom _Y_ Labels  For First Data Serie");
            plot->yscale().set_color(color::TEAL);
            plot->set_label("Custom _Y_ Labels");
            plot->set_max_y(13);
        }
        else if (TEST == TEST_RANGE1) {
            auto vec1 = create_line(-10);
            auto vec2 = PlotData::Modify(vec1, PlotData::MODIFY::MULTIPLICATION, PlotData::TARGET::Y, -1.0);
            
            plot->add_line(PlotLine(vec1, "TYPE::LINE_WITH_SQUARE", PlotLine::TYPE::LINE_WITH_SQUARE, FL_BLUE, 2));
            plot->add_line(PlotLine(vec2, "Inverted Line", PlotLine::TYPE::LINE_DOT, FL_RED, 3));
            plot->xscale().set_label("X Label");
            plot->xscale().set_color(FL_DARK_GREEN);
            plot->yscale().set_label("Y Label");
            plot->yscale().set_color(FL_DARK_GREEN);
        }
        else if (TEST == TEST_RANGE2) {
            PlotDataVector vec;

            auto maxx = 12'500;
            auto maxy = 25'000;

            for (int f = 0; f < 50; f++) {
                vec.push_back(PlotData(rand() % maxx * 2 - maxx, rand() % maxy * 2 - maxy));
            }
            
            plot->add_line(PlotLine(vec, "TYPE::SQUARE", PlotLine::TYPE::SQUARE, color::TEAL, 7));
            plot->xscale().set_label(TEST_RANGE2);
            plot->xscale().set_color(color::CRIMSON);
            plot->yscale().set_label(TEST_RANGE2);
            plot->yscale().set_color(color::TEAL);
        }
        else if (TEST == TEST_RANGE3) {
            plot->set_label(TEST_RANGE3);
            plot->add_line(PlotLine(create_line(-10, 1000), "1 TYPE::LINE", PlotLine::TYPE::LINE, color::CRIMSON, 3));
            plot->add_line(PlotLine(create_line(-9, 900), "2 TYPE::LINE", PlotLine::TYPE::LINE, color::OLIVE, 3));
            plot->add_line(PlotLine(create_line(-8, 800), "3 TYPE::LINE", PlotLine::TYPE::LINE, color::TEAL, 3));
            plot->add_line(PlotLine(create_line(-7, 700), "4 TYPE::LINE", PlotLine::TYPE::LINE, color::GOLD, 3));
            plot->add_line(PlotLine(create_line(-6, 600), "5 TYPE::LINE", PlotLine::TYPE::LINE, color::TURQUOISE, 3));
            plot->add_line(PlotLine(create_line(-5, 500), "6 TYPE::LINE", PlotLine::TYPE::LINE, color::ROYALBLUE, 3));
            plot->add_line(PlotLine(create_line(-4, 400), "7 TYPE::LINE", PlotLine::TYPE::LINE, color::PINK, 3));
            plot->add_line(PlotLine(create_line(-3, 300), "8 TYPE::LINE", PlotLine::TYPE::LINE, color::OLIVE, 3));
            plot->add_line(PlotLine(create_line(-2, 200), "9 TYPE::LINE", PlotLine::TYPE::LINE, color::CHOCOLATE, 3));
            assert(plot->add_line(PlotLine(create_line(-1, 100), "10 TYPE::LINE", PlotLine::TYPE::LINE, color::FORESTGREEN, 3)) == true);
            assert(plot->add_line(PlotLine(create_line(-1, 900), "10 TYPE::LINE", PlotLine::TYPE::LINE, color::FORESTGREEN, 3)) == false);
            plot->xscale().set_label(TEST_RANGE3);
            plot->xscale().set_color(color::CRIMSON);
            plot->yscale().set_label(TEST_RANGE3);
            plot->yscale().set_color(color::TEAL);
        }
        else if (TEST == TEST_EMPTY) {
            plot->xscale().set_label("X Labels");
            plot->yscale().set_label("Y Labels");
            plot->set_label("Empty");
        }
        else if (TEST == TEST_LARGE1) {
            plot->add_line(PlotLine(create_large1(27'250'100, 1'422'125'000), "TYPE::LINE", PlotLine::TYPE::LINE, color::TEAL, 2));
            plot->add_line(PlotLine(create_large1(20'250'100, 2'222'125'000), "TYPE::LINE_DASH", PlotLine::TYPE::LINE_DASH, color::OLIVE, 4));
            plot->add_line(PlotLine(create_large1(35'250'100, 1'822'125'000), "TYPE::LINE_DOT", PlotLine::TYPE::LINE_DOT, color::CRIMSON, 6));
            plot->xscale().set_label(TEST_LARGE1);
            plot->yscale().set_label(TEST_LARGE1);
        }
        else if (TEST == TEST_LARGE2) {
            PlotDataVector vec;

            const double v = 321'000'000'000;

            for (int f = 1; f <= 20; f++) {
                auto x = 1.0 / static_cast<double>((rand() % 10) + 1);
                auto y = 1.0 / static_cast<double>((rand() % 10) + 1);
                vec.push_back(PlotData(v + x, v - y));
            }

            vec.push_back(PlotData(INFINITY, 0.0));
            vec.push_back(PlotData(9323372036854775807.0, 0.0));
            
            plot->add_line(PlotLine(vec, "TYPE::LINE", PlotLine::TYPE::FILLED_CIRCLE, color::TEAL, 10));
            plot->xscale().set_label("");
            plot->xscale().set_color(FL_DARK_GREEN);
            plot->yscale().set_label(TEST_LARGE2);
            plot->yscale().set_color(FL_RED);
            plot->set_min_x(v - 1.5);
            plot->set_max_x(v + 1.5);
            plot->set_min_y(v - 1.5);
            plot->set_max_y(v + 1.5);
            plot->set_label("Large Values With Clamp Values For X And Y");
        }
        else if (TEST == TEST_REF1 || TEST == TEST_REF2) {
            PlotDataVector vec;

            for (int f = -10; f <= 10; f++) {
                vec.push_back(PlotData(f * 10, f * 10));
            }

            plot->add_line(PlotLine(vec, "TYPE::CIRCLE", PlotLine::TYPE::CIRCLE, color::TEAL, 21));
            plot->add_line(PlotLine(create_point(0.0, 0.0), "TYPE::SQUARE", PlotLine::TYPE::SQUARE, color::CRIMSON, 15));
            plot->xscale().set_label(TEST_REF1);
            plot->xscale().set_color(color::CRIMSON);
            plot->yscale().set_label(TEST_REF1);
            plot->yscale().set_color(color::TEAL);
            
            if (TEST == TEST_REF1) {
                plot->set_label("Reference Values");
            }
            else {
                plot->set_label("Reference Values With Clamp");
                plot->set_min_x(-120);
                plot->set_max_x(120);
                plot->set_min_y(-120);
                plot->set_max_y(120);
            }
        }
        else if (TEST == TEST_SMALL1) {
            PlotDataVector vec;

            auto maxx = 2'00'000;
            auto maxy = 2'00'000;

            for (int f = 0; f < 10; f++) {
            #ifdef _WIN32
                vec.push_back(PlotData((double) (rand() % maxx) / (double) (maxx * maxx), (double) ((rand() * rand()) % maxy) / (double) (maxy * maxy)));
            #else
                vec.push_back(PlotData((double) (rand() % maxx) / (double) (maxx * maxx), (double) (rand() % maxy) / (double) (maxx * maxx)));
            #endif
            }

            plot->add_line(PlotLine(vec, "TYPE::CIRCLE", PlotLine::TYPE::CIRCLE, color::TEAL, 11));
            plot->xscale().set_label(TEST_SMALL1);
            plot->yscale().set_label(TEST_SMALL1);
        }
        else if (TEST == TEST_SMALL2) {
            plot->add_line(PlotLine(create_random_small(), "TYPE::FILLED_CIRCLE", PlotLine::TYPE::FILLED_CIRCLE, FL_BLUE, 21));
            plot->add_line(PlotLine(create_random_small(1'000), "TYPE::SQUARE", PlotLine::TYPE::SQUARE, FL_RED, 12));
            plot->xscale().set_label(TEST_SMALL2);
            plot->yscale().set_label(TEST_SMALL2);
        }
        else if (TEST == TEST_VECTOR1) {
            PlotDataVector vec;

            auto maxx = 100;
            auto maxy = 100;

            for (int f = 0; f <= 20; f++) {
                auto p = PlotData((double) (rand() % maxx), (double) (rand() % maxy));
                auto x = (rand() % 20) - 10;
                auto y = (rand() % 20) - 10;

                vec.push_back(p);
                vec.push_back(PlotData(p.x + x, p.y += y));
            }

            plot->add_line(PlotLine(vec, "TYPE::VECTOR", PlotLine::TYPE::VECTOR, color::TEAL, 2));
            plot->xscale().set_label(TEST_VECTOR1);
            plot->yscale().set_label(TEST_VECTOR1);
        }
        else if (TEST == TEST_VECTOR2) {
            PlotDataVector vec;

            vec.push_back(PlotData(0.0, 0.0));
            vec.push_back(PlotData(0.0, 0.0));

            for (int f = 1; f <= 10; f++) {
                vec.push_back(PlotData(f, 0.0));
                vec.push_back(PlotData(f, rand() % 100));
            }

            vec.push_back(PlotData(11.0, 0.0));
            vec.push_back(PlotData(11.0, 0.0));
            
            plot->add_line(PlotLine(vec, "TYPE::VECTOR", PlotLine::TYPE::VECTOR, color::TEAL, 21));
            plot->xscale().set_label(TEST_VECTOR2);
            plot->xscale().set_color(color::CRIMSON);
            plot->yscale().set_label(TEST_VECTOR2);
            plot->yscale().set_color(color::TEAL);
        }

        plot->init_new_data();
    }

    //--------------------------------------------------------------------------
    int handle(int event) override {
        return Fl_Double_Window::handle(event);
    }

    //--------------------------------------------------------------------------
    void resize(int X, int Y, int W, int H) override {
        Fl_Double_Window::resize(X, Y, W, H);
        menu->resize(0, 0, W, flw::PREF_FONTSIZE * 2);
        plot->resize(0, flw::PREF_FONTSIZE * 2, W, H - flw::PREF_FONTSIZE * 2);
//        plot->resize(100, 100, W - 150, H - 150);
    }

    //--------------------------------------------------------------------------
    void update_pref() {
        menu->textfont(flw::PREF_FONT);
        menu->textsize(flw::PREF_FONTSIZE);
        plot->update_pref();
        plot->init_new_data();
        resize(x(), y(), w(), h());
        Fl::redraw();
    }
};

//------------------------------------------------------------------------------
int main(int argc, const char** argv) {
    if (flw::theme::parse(argc, argv) == false) {
        flw::theme::load("oxy");
    }

    Test win(1200, 800);
    return Fl::run();
}
