// Copyright gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#include "flw.h"

#ifndef FLW_AMALGAM
    #include "plot.h"
#endif

#include <FL/Fl.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Menu_Bar.H>
#include <time.h>

using namespace flw;

static bool        RAND_666 = false;
static std::string TEST     = "";

void callback_timer(void *data);

#define MENU_10LINES    "10 Lines (Plot::LINE)"
#define MENU_AUTORUN    "Autorun"
#define MENU_BAR        "Bar (Plot::VECTOR)"
#define MENU_DAYS       "Custom Y (Plot::VECTOR)"
#define MENU_LARGE      "Large (Plot::LINE)"
#define MENU_LINE       "Lines (Plot::LINE_WITH_SQUARE)"
#define MENU_MLINE      "1'000 (Plot::LINE)"
#define MENU_MONTHS     "Custom X (Plot::VECTOR && Plot::LINE_DASH)"
#define MENU_NA         "NA"
#define MENU_NP         "Negative && Positive (Plot::SQUARE)"
#define MENU_REF        "Ref (Plot::CIRCLE && Plot::SQUARE)"
#define MENU_SMALL      "Small (Plot::FILLED_CIRCLE && Plot::SQUARE)"
#define MENU_SRAND666   "Srand(666)"
#define MENU_SRANDTIME  "Srand(time)"
#define MENU_VECTOR     "Random pairs (Plot::VECTOR)"
#define MENU_VSMALL     "Very Small (Plot::CIRCLE)"

PointVector create_bar(int y) {
    PointVector res;

    res.push_back(Point(0.0, 0.0));
    res.push_back(Point(0.0, 0.0));

    for (int f = 1; f <= 10; f++) {
        // res.push_back(Point(f, -4.0));
        res.push_back(Point(f, 0.0));
        res.push_back(Point(f, rand() % y));
    }

    res.push_back(Point(11.0, 0.0));
    res.push_back(Point(11.0, 0.0));

    // print(res);
    return res;
}

PointVector create_big_line() {
    PointVector res;

    auto maxy = 1'000;

    for (int f = 1; f <= 1'000; f++) {
        res.push_back(Point(f, rand() % maxy * 2 - maxy));
    }

    return res;
}

PointVector create_days() {
    PointVector res;

    res.push_back(Point(0.0, 0.0));
    res.push_back(Point(0.0, 0.0));

    for (int f = 1; f <= 7; f++) {
        res.push_back(Point(rand() % 100, f));
        res.push_back(Point(0.0, f));
    }

    res.push_back(Point(0, 8));
    res.push_back(Point(0, 8));

    // print(res);
    return res;
}

StringVector create_days2() {
    StringVector res;

    res.push_back("");
    res.push_back("Mon");
    res.push_back("Tue");
    res.push_back("Wednesday");
    res.push_back("Thu");
    res.push_back("Fri");
    res.push_back("Sat");
    res.push_back("Sun");
    return res;
}

PointVector create_line(int x) {
    PointVector res;

    auto maxy = 1'000;

    for (int f = x; f <= 30; f += 2) {
        res.push_back(Point(f, rand() % maxy * 2 - maxy));
    }

    // print(res);
    return res;
}

PointVector create_months() {
    PointVector res;

    res.push_back(Point(0.0, 0.0));
    res.push_back(Point(0.0, 0.0));

    for (int f = 1; f <= 12; f++) {
        res.push_back(Point(f + 0.0, 0.0));
        res.push_back(Point(f + 0.0, rand() % 100));
    }

    res.push_back(Point(13.0, 0.0));
    res.push_back(Point(13.0, 0.0));

    // print(res);
    return res;
}

StringVector create_months2() {
    StringVector res;

    res.push_back("");
    res.push_back("Jan");
    res.push_back("Feb");
    res.push_back("Mar");
    res.push_back("Apr");
    res.push_back("May");
    res.push_back("Jun");
    res.push_back("Jul");
    res.push_back("Aug");
    res.push_back("Sep");
    res.push_back("Oct");
    res.push_back("Nov");
    res.push_back("Dec");
    res.push_back("");
    return res;
}

PointVector create_neg() {
    PointVector res;

    auto maxx = 12'500;
    auto maxy = 25'000;

    for (int f = 0; f < 50; f++) {
        res.push_back(Point(rand() % maxx * 2 - maxx, rand() % maxy * 2 - maxy));
    }

    return res;
}

PointVector create_point(double x, double y) {
    PointVector res;
    res.push_back(Point(x, y));
    return res;
}

PointVector create_random_small() {
    PointVector res;

    auto maxx = 1'250;
    auto maxy = 2'440;

    for (int f = 0; f < 20; f++) {
        res.push_back(Point(rand() % maxx, rand() % maxy));
    }

    return res;
}

PointVector create_random_large() {
    PointVector res;

    auto maxx =  5'250'100;
    auto maxy = 222'125'000;
    auto x    = rand() % maxx + maxx;

    for (int f = 0; f < 50; f++) {
#ifdef _WIN32
        res.push_back(Point(f * x, (rand() % maxy) * (rand() % maxx)));
#else
        res.push_back(Point(f * x, rand() % maxy));
#endif
    }

    // print(res);
    return res;
}

PointVector create_random_very_small() {
    PointVector res;

    auto maxx = 123'250;
    auto maxy = 222'125'000;

    for (int f = 0; f < 100; f++) {
#ifdef _WIN32
        res.push_back(Point((double) (rand() % maxx) / (double) (maxx * maxx), (double) ((rand() * rand()) % maxy) / (double) (maxy * maxy)));
#else
        res.push_back(Point((double) (rand() % maxx) / (double) (maxx * maxx), (double) (rand() % maxy) / (double) (maxx * maxx)));
#endif
    }

    return res;
}

PointVector create_ref() {
    PointVector res;

    for (int f = -10; f <= 10; f++) {
        res.push_back(Point(f * 10, f * 10));
    }

    return res;
}

PointVector create_vector() {
    PointVector res;

    auto maxx = 100;
    auto maxy = 100;

    for (int f = 0; f <= 20; f++) {
        auto p = Point((double) (rand() % maxx), (double) (rand() % maxy));
        auto x = (rand() % 20) - 10;
        auto y = (rand() % 20) - 10;

        res.push_back(p);
        res.push_back(Point(p.x + x, p.y += y));
    }

    // print(res);
    return res;
}

class Test : public Fl_Double_Window {
public:
    Test(int W, int H) : Fl_Double_Window(W, H, "test_plot.cpp") {
        menu = new Fl_Menu_Bar(0, 0, 0, 0);
        plot = new Plot(0, 0, W, H);

        add(menu);
        add(plot);

        menu->add("&Data/" MENU_AUTORUN, 0, Callback, this, FL_MENU_DIVIDER);
        menu->add("&Data/" MENU_SRAND666, 0, Callback, this);
        menu->add("&Data/" MENU_SRANDTIME, 0, Callback, this, FL_MENU_DIVIDER);
        menu->add("&Data/save", 0, CallbackSave, this);
        menu->add("&Data/Load", 0, CallbackLoad, this, FL_MENU_DIVIDER);
        menu->add("&Data/" MENU_10LINES, 0, Callback, this);
        menu->add("&Data/" MENU_REF, 0, Callback, this);
        menu->add("&Data/" MENU_VSMALL, 0, Callback, this);
        menu->add("&Data/" MENU_SMALL, 0, Callback, this);
        menu->add("&Data/" MENU_LARGE, 0, Callback, this);
        menu->add("&Data/" MENU_NP, 0, Callback, this);
        menu->add("&Data/" MENU_LINE, 0, Callback, this);
        menu->add("&Data/" MENU_MLINE, 0, Callback, this);
        menu->add("&Data/" MENU_VECTOR, 0, Callback, this);
        menu->add("&Data/" MENU_BAR, 0, Callback, this);
        menu->add("&Data/" MENU_MONTHS, 0, Callback, this);
        menu->add("&Data/" MENU_DAYS, 0, Callback, this);
        menu->textfont(flw::PREF_FONT);
        menu->textsize(flw::PREF_FONTSIZE);

        resizable(this);
        size_range(64, 48);
        show();
        resize(300, 100, W, H);
    }

    static void Callback(Fl_Widget* sender, void* object) {
        auto self = (Test*) object;

        if (sender == self->menu) {
            auto s = (self->menu->text() != nullptr) ? std::string(self->menu->text()) : std::string();
            self->run_menu(s);
        }
    }

    static void CallbackLoad(Fl_Widget*, void* v) {
        Test* w = (Test*) v;
        Plot::Load(w->plot, "plot.json");
    }

    static void CallbackSave(Fl_Widget*, void* v) {
        Test* w = (Test*) v;
        Plot::Save(w->plot, "plot.json");
    }

    int handle(int event) override {
        return Fl_Double_Window::handle(event);
    }

    void resize(int X, int Y, int W, int H) override {
        Fl_Double_Window::resize(X, Y, W, H);
        menu->resize(0, 0, W, flw::PREF_FONTSIZE * 2);
        plot->resize(0, flw::PREF_FONTSIZE * 2, W, H - flw::PREF_FONTSIZE * 2);
//        plot->resize(50, 50, W - 100, H - 100);
    }

    void run_menu(std::string s) {
        if (s == MENU_SRAND666) {
            RAND_666 = true;
        }
        else if (s == MENU_SRANDTIME) {
            RAND_666 = false;
        }
        else if (s == MENU_AUTORUN) {
            TEST = "";
            menu->deactivate();
            Fl::add_timeout(0.5, callback_timer, this);
        }
        else if (s == "" || s == MENU_10LINES) {
            if (RAND_666) srand(666); else srand(time(nullptr));
            plot->clear();
            plot->add_line(create_line(-10), Plot::LINE, 3, "1 Plot::LINE", color::CRIMSON);
            plot->add_line(create_line(-9), Plot::LINE, 3, "2 Plot::LINE", color::OLIVE);
            plot->add_line(create_line(-8), Plot::LINE, 3, "3 Plot::LINE", color::TEAL);
            plot->add_line(create_line(-7), Plot::LINE, 3, "4 Plot::LINE", color::GOLD);
            plot->add_line(create_line(-6), Plot::LINE, 3, "5 Plot::LINE", color::TURQUOISE);
            plot->add_line(create_line(-5), Plot::LINE, 3, "6 Plot::LINE", color::ROYALBLUE);
            plot->add_line(create_line(-4), Plot::LINE, 3, "7 Plot::LINE", color::PINK);
            plot->add_line(create_line(-3), Plot::LINE, 3, "8 Plot::LINE", color::OLIVE);
            plot->add_line(create_line(-2), Plot::LINE, 3, "9 Plot::LINE", color::CHOCOLATE);
            plot->add_line(create_line(-1), Plot::LINE, 3, "10 Plot::LINE", color::FORESTGREEN);
            plot->labels(MENU_10LINES, MENU_10LINES);
            plot->label_colors(color::CRIMSON, color::TEAL);
            plot->layout();
        }
        else if (s == MENU_BAR) {
            if (RAND_666) srand(666); else srand(time(nullptr));
            plot->clear();
            plot->add_line(create_bar(100), Plot::VECTOR, 21, "Plot::VECTOR", color::TEAL);
            plot->labels(MENU_BAR, MENU_BAR);
            plot->label_colors(color::CRIMSON, color::TEAL);
            plot->layout();
        }
        else if (s=="1" || s == MENU_DAYS) {
            if (RAND_666) srand(666); else srand(time(nullptr));
            plot->clear();
            plot->add_line(create_days(), Plot::VECTOR, 21, "Plot::VECTOR", color::TEAL);
            plot->custom_ylabels_for_points0(create_days2());
            plot->labels(MENU_DAYS, "Sum");
            plot->label_colors(color::CRIMSON, color::TEAL);
            plot->layout();
        }
        else if (s == MENU_LARGE) {
            if (RAND_666) srand(666); else srand(time(nullptr));
            plot->clear();
            plot->add_line(create_random_large(), Plot::LINE, 3, "Plot::LINE", color::TEAL);
            plot->add_line(create_random_large(), Plot::LINE_DASH, 3, "Plot::LINE_DASH", color::OLIVE);
            plot->add_line(create_random_large(), Plot::LINE_DOT, 3, "Plot::LINE_DOT", color::CRIMSON);
            plot->labels(MENU_LARGE, MENU_LARGE);
            plot->layout();
        }
        else if (s == MENU_LINE) {
            if (RAND_666) srand(666); else srand(time(nullptr));
            plot->clear();
            plot->add_line(create_line(-10), Plot::LINE_WITH_SQUARE, 1, "Plot::LINE_WITH_SQUARE", color::TEAL);
            plot->labels(MENU_LINE, MENU_LINE);
            plot->label_colors(color::CRIMSON, color::TEAL);
            plot->layout();
        }
        else if (s == MENU_MLINE) {
            if (RAND_666) srand(666); else srand(time(nullptr));
            plot->clear();
            plot->add_line(create_big_line(), Plot::LINE, 1, "Plot::LINE", color::TEAL);
            plot->labels("", MENU_MLINE);
            plot->label_colors(color::CRIMSON, color::TEAL);
            plot->layout();
        }
        else if (s=="1" || s == MENU_MONTHS) {
            if (RAND_666) srand(666); else srand(time(nullptr));
            plot->clear();
            plot->add_line(create_months(), Plot::VECTOR, 21, "Plot::VECTOR", color::TEAL);
            plot->add_line(create_months(), Plot::LINE_DASH, 3, "Plot::LINE_DASH", color::CRIMSON);
            plot->custom_xlabels_for_points0(create_months2());
            plot->labels(MENU_MONTHS, "Y-Sum");
            plot->label_colors(color::CRIMSON, color::TEAL);
            plot->layout();
        }
        else if (s == MENU_NP) {
            if (RAND_666) srand(666); else srand(time(nullptr));
            plot->clear();
            plot->add_line(create_neg(), Plot::SQUARE, 7, "Plot::SQUARE", color::TEAL);
            plot->labels(MENU_NP, MENU_NP);
            plot->label_colors(color::CRIMSON, color::TEAL);
            plot->layout();
        }
        else if (s == MENU_SMALL) {
            if (RAND_666) srand(666); else srand(time(nullptr));
            plot->clear();
            plot->add_line(create_random_small(), Plot::FILLED_CIRCLE, 21, "Plot::FILLED_CIRCLE", color::TEAL);
            plot->add_line(create_random_small(), Plot::SQUARE, 7, "Plot::SQUARE", color::CRIMSON);
            plot->labels(MENU_SMALL, MENU_SMALL);
            plot->layout();
        }
        else if (s == MENU_VECTOR) {
            if (RAND_666) srand(666); else srand(time(nullptr));
            plot->clear();
            plot->add_line(create_vector(), Plot::VECTOR, 2, "Plot::VECTOR", color::TEAL);
            plot->labels(MENU_VECTOR, MENU_VECTOR);
            plot->layout();
        }
        else if (s == MENU_VSMALL) {
            if (RAND_666) srand(666); else srand(time(nullptr));
            plot->clear();
            plot->add_line(create_random_very_small(), Plot::CIRCLE, 9, "Plot::CIRCLE", color::TEAL);
            plot->labels(MENU_VSMALL, MENU_VSMALL);
            plot->layout();
        }
        else {
            if (RAND_666) srand(666); else srand(time(nullptr));
            plot->clear();
            plot->add_line(create_ref(), Plot::CIRCLE, 11, "Plot::CIRCLE", color::TEAL);
            plot->add_line(create_point(0.0, 0.0), Plot::SQUARE, 7, "Plot::SQUARE", color::CRIMSON);
            plot->labels(MENU_REF, MENU_REF);
            plot->label_colors(color::CRIMSON, color::TEAL);
            plot->layout();
        }
    }

    Plot*        plot;
    Fl_Menu_Bar* menu;
};

void callback_timer(void *data) {
    auto test = (Test*) data;

    if (TEST == MENU_DAYS) {
        TEST = MENU_10LINES;
        test->menu->activate();
        Fl::remove_timeout(callback_timer, data);
    }
    else {
        if (TEST == "") {
            TEST = MENU_10LINES;
        }
        else if (TEST == MENU_10LINES) {
            TEST = MENU_REF;
        }
        else if (TEST == MENU_REF) {
            TEST = MENU_SMALL;
        }
        else if (TEST == MENU_VSMALL) {
            TEST = MENU_LARGE;
        }
        else if (TEST == MENU_SMALL) {
            TEST = MENU_VSMALL;
        }
        else if (TEST == MENU_LARGE) {
            TEST = MENU_NP;
        }
        else if (TEST == MENU_NP) {
            TEST = MENU_LINE;
        }
        else if (TEST == MENU_LINE) {
            TEST = MENU_MLINE;
        }
        else if (TEST == MENU_MLINE) {
            TEST = MENU_VECTOR;
        }
        else if (TEST == MENU_VECTOR) {
            TEST = MENU_BAR;
        }
        else if (TEST == MENU_BAR) {
            TEST = MENU_MONTHS;
        }
        else if (TEST == MENU_MONTHS) {
            TEST = MENU_DAYS;
        }

        Fl::repeat_timeout(2.0, callback_timer, data);
    }

    test->run_menu(TEST);
}

int main(int argc, const char** argv) {
    if (flw::theme::parse(argc, argv) == false) {
        flw::theme::load("oxy");
    }

    Test win(1000, 600);
    Test::Callback(win.menu, &win);
    return Fl::run();
}
