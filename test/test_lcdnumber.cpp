// Copyright gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#include "flw.h"

#ifndef FLW_AMALGAM
    #include "lcdnumber.h"
#endif

#include <FL/Fl_Double_Window.H>

class Test : public Fl_Double_Window {
public:
    Test(int W, int H, const char* l) : Fl_Double_Window(W, H, l) {
        int H2 = 14;

        lcd1 = new flw::LcdNumber(0, 0, W, H2 * 5);
        lcd2 = new flw::LcdNumber(0, H2 * 5, W, H2 * 5);
        lcd3 = new flw::LcdNumber(0, H2 * 5 * 2, W, H2 * 5 * 2);

        lcd1->box(FL_EMBOSSED_BOX);
        lcd1->segment_color(FL_FOREGROUND_COLOR);
        lcd1->value("01234.56789");

        lcd2->box(FL_EMBOSSED_BOX);
        lcd2->segment_color(FL_GREEN);
        lcd2->align(FL_ALIGN_RIGHT);
        lcd2->value("-01234567:89");

        lcd3->box(FL_EMBOSSED_BOX);
        lcd3->align(FL_ALIGN_CENTER);
        lcd3->dot_size(16);
        lcd3->segment_color(FL_BLUE);
        lcd3->segment_gap(4);
        lcd3->thickness(8);
        lcd3->unit_gap(8);
        lcd3->unit_h(70);
        lcd3->unit_w(35);
        lcd3->value("0.1.2.3.4.56.889");

        resizable(this);
    }

    flw::LcdNumber* lcd1;
    flw::LcdNumber* lcd2;
    flw::LcdNumber* lcd3;
};

int main(int argc, const char** argv) {
    if (flw::theme::parse(argc, argv) == false) {
        flw::theme::load("oxy");
    }

    Test win(600, 320, "test_lcdnumber.cpp");
    win.show();
    return Fl::run();
}
