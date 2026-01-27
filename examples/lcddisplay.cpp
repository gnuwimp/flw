// Copyright gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#include "flw.h"

// [flw::LCDDisplay::timeout example]

static void timeout(void* o) {
    static_cast<flw::LCDDisplay*>(o)->value(gnu::Date().format(gnu::Date::Format::ISO_TIME_LONG));
    Fl::repeat_timeout(1.0, timeout, o);
}

// [flw::LCDDisplay::timeout example]

int main() {
    Fl::scheme("oxy");
    Fl::screen_scale(0, 1.0);

    // [flw::LCDDisplay example]

    auto win  = new Fl_Window(Fl::w() / 2 - 700, Fl::h() / 2 - 150, 1400, 300, "flw::LCDDisplay");
    auto lcd1 = new flw::LCDDisplay(0, 0, 1400, 150);
    auto lcd2 = new flw::LCDDisplay(0, 150, 1400, 150);

    lcd1->box(FL_BORDER_BOX);
    lcd1->align(FL_ALIGN_CENTER);
    lcd1->color(FL_BLACK);
    lcd1->segment_color(FL_RED);
    lcd1->thickness(8);
    lcd1->dot_size(10);
    lcd1->unit_h(100);
    lcd1->unit_w(50);
    lcd1->unit_gap(12);

    lcd2->align(FL_ALIGN_CENTER);
    lcd2->unit_gap(12);
    lcd2->value("-0123456789.:abcdefghijklmnopqrstuvwxyz");

    timeout(lcd1);
    win->resizable(win);
    win->show();
    Fl::run();

    // [flw::LCDDisplay example]
}
