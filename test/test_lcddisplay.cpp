// Copyright gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#include "flw.h"

#ifndef FLW_AMALGAM
    #include "lcddisplay.h"
    #include "date.h"
    #include "theme.h"
#endif

#include <FL/Fl_Double_Window.H>

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
    Test(int W, int H, const char* l) : Fl_Double_Window(W, H, l) {
        int H2 = 14;

        lcd1 = new flw::LCDDisplay(0, 0, W, H2 * 5);
        lcd2 = new flw::LCDDisplay(0, H2 * 5, W, H2 * 5);
        lcd3 = new flw::LCDDisplay(0, H2 * 5 * 2, W, H2 * 5 * 2);

        lcd1->box(FL_EMBOSSED_BOX);
        lcd1->color(FL_BLACK);
        lcd1->segment_color(FL_RED);
        lcd1->unit_gap(8);
        lcd1->value("-0123456789.:abcdefghijklmnopqrstuvwxyz");

        lcd2->box(FL_EMBOSSED_BOX);
        lcd2->segment_color(FL_DARK_GREEN);
        lcd2->align(FL_ALIGN_RIGHT);
        lcd2->value("-0123456789 . : ABCDEF");
        lcd2->deactivate();

        lcd3->box(FL_EMBOSSED_BOX);
        lcd3->align(FL_ALIGN_CENTER);
        lcd3->dot_size(16);
        lcd3->segment_color(FL_DARK_BLUE);
        lcd3->segment_gap(4);
        lcd3->thickness(8);
        lcd3->unit_h(90);
        lcd3->unit_w(45);
        lcd3->unit_gap(10);

        resizable(this);
        Test::Timeout(lcd3);
    }

    //--------------------------------------------------------------------------
    static void Timeout(void* o) {
        auto d = gnu::Date();
        auto w = d.format(gnu::Date::Format::TIME_LONG);
        auto t = d.format(gnu::Date::Format::WEEKDAY);

        static_cast<flw::LCDDisplay*>(o)->value(t + " " + w);
        Fl::repeat_timeout(1.0, Test::Timeout, o);
    }

    flw::LCDDisplay* lcd1;
    flw::LCDDisplay* lcd2;
    flw::LCDDisplay* lcd3;
};

/*
 *                      _
 *                     (_)
 *      _ __ ___   __ _ _ _ __
 *     | '_ ` _ \ / _` | | '_ \
 *     | | | | | | (_| | | | | |
 *     |_| |_| |_|\__,_|_|_| |_|
 *
 *
 */

//------------------------------------------------------------------------------
int main(int argc, const char** argv) {
    if (flw::theme::parse(argc, argv) == false) {
        flw::theme::load("oxy");
    }

    Test win(1200, 400, "test_lcddisplay.cpp");
    win.show();
    return Fl::run();
}
