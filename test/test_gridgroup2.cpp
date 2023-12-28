// Copyright 2019 - 2021 gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#include "gridgroup.h"
#include "theme.h"
#include <FL/Fl_Button.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl.H>

using namespace flw;

// ---------------------------------------------
// |TOP________________________________________|
// |VERTICAL|                      RIGHT1      |
// |VERTICAL| CENTER______________       RIGHT2|
// |VERTICAL| CENTER______________ RIGHT3______|
// |VERTICAL| CENTER______________ RIGHT3______|
// |VERTICAL| CENTER______________ RIGHT3______|
// |VERTICAL| CENTER______________ RIGHT3______|
// |VERTICAL| CENTER______________ RIGHT3______|
// |VERTICAL|                      RIGHT3______|
// |VERTICAL|BOTTOM____________________________|
// ---------------------------------------------
//
class Test : public Fl_Double_Window {
public:
    Test(int W, int H) : Fl_Double_Window(W, H, "test_gridgroup2.cpp") {
        grid = new GridGroup(0, 0, W, H);

        //                                                 X   Y    W   H
        grid->add(new Fl_Button(0, 0, 0, 0, "TOP"),        0,  0,   0,  2);
        grid->add(new Fl_Button(0, 0, 0, 0, "VERTICAL"),   0,  2,   8,  0);
        grid->add(new Fl_Button(0, 0, 0, 0, "BOTTOM"),     8, -8,   0,  0);
        grid->add(new Fl_Button(0, 0, 0, 0, "RIGHT1"),  -10,  2,   5,  2);
        grid->add(new Fl_Button(0, 0, 0, 0, "RIGHT2"),   -5,  4,   0,  2);
        grid->add(new Fl_Button(0, 0, 0, 0, "RIGHT3"),  -10,  6,   0, -9);
        grid->add(new Fl_Button(0, 0, 0, 0, "CENTER"),     9,  4, -11, -9);
        grid->size(10);
        grid->resize();
        resizable(grid);
        size_range(64, 48);
    }

    GridGroup* grid;
};

int main(int argc, const char** argv) {
    if (flw::theme::parse(argc, argv) == false) {
        flw::theme::load("gtk");
    }

    Test win(800, 480);
    win.show();
    return Fl::run();
}
