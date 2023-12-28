// Copyright 2019 - 2021 gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#include "gridgroup.h"
#include "theme.h"
#include "util.h"
#include <FL/Fl_Button.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl.H>

using namespace flw;

class Test : public Fl_Double_Window {
public:
    Test(int W, int H) : Fl_Double_Window(W, H, "test_gridgroup1.cpp") {
        grid1 = new GridGroup(100, 100, 200, 180);
        grid1->size(flw::PREF_FONTSIZE / 4);
        grid1->color(FL_YELLOW);
        grid1->box(FL_FLAT_BOX);
        grid1->add(new Fl_Button(0, 0, 0, 0, "MENU"), 0, 0, 0, 2);
        b1 = new Fl_Button(0, 0, 0, 0, "B&1");
        grid1->add(b1, 0, 3, 4, 2);
        grid1->add(new Fl_Button(0, 0, 0, 0, "B2"), 0, 9, 4, 2);
        grid1->add(new Fl_Button(0, 0, 0, 0, "B3"), 0, 12, 4, 2);
        grid1->add(new Fl_Button(0, 0, 0, 0, "BOX"), 1, -3, -11, 2);
        grid1->add(new Fl_Button(0, 0, 0, 0, "B4"), -10, -3, 4, 2);
        grid1->add(new Fl_Button(0, 0, 0, 0, "B5"), -5, -3, -1, 2);
        grid1->add(new Fl_Button(0, 0, 0, 0, "B6"), -4, 3, 4, 2);
        grid1->add(new Fl_Button(0, 0, 0, 0, "B7"), -4, 7, 4, 2);
        grid1->add(new Fl_Button(0, 0, 0, 0, "B8"), -4, 10, 4, 2);
        grid1->add(new Fl_Button(0, 0, 0, 0, "LIST"), 5, 2, -5, -4);
        grid1->size(10);

        grid2 = new GridGroup(300, 280, W - 300, H - 280);
        grid2->size(flw::PREF_FONTSIZE / 2);
        grid2->color(FL_RED);
        grid2->box(FL_FLAT_BOX);
        grid2->add(new Fl_Button(0, 0, 0, 0, "MENU1"), 0, 0, 10, 2);
        grid2->add(new Fl_Button(0, 0, 0, 0, "MENU2"), 10, 0, 0, 2);
        grid2->add(new Fl_Button(0, 0, 0, 0, "B1"), 0, 3, 4, 2);
        grid2->add(new Fl_Button(0, 0, 0, 0, "B2"), 0, 9, 4, 2);
        grid2->add(new Fl_Button(0, 0, 0, 0, "B3"), 0, 12, 4, 2);
        grid2->add(new Fl_Button(0, 0, 0, 0, "BOX"), 1, -3, -11, 2);
        grid2->add(new Fl_Button(0, 0, 0, 0, "B4"), -10, -3, 4, 2);
        grid2->add(new Fl_Button(0, 0, 0, 0, "B5"), -5, -3, -1, 2);
        grid2->add(new Fl_Button(0, 0, 0, 0, "B6"), -4, 3, 4, 2);
        grid2->add(new Fl_Button(0, 0, 0, 0, "B7"), -4, 7, 4, 2);
        grid2->add(new Fl_Button(0, 0, 0, 0, "B8"), -4, 10, 4, 2);
        b2 = new Fl_Button(0, 0, 0, 0, "&REMOVE");
        grid2->add(b2, 5, 2, -5, -4);
        grid2->size(20);

        resizable(this);
        size_range(64, 48);
        b1->callback(Callback1, this);
        b2->callback(Callback2, this);
        flw::theme::labelfont(grid1);
        flw::theme::labelfont(grid2);
        grid1->resize();
        grid2->resize();
    }

    void resize(int X, int Y, int W, int H) {
        Fl_Double_Window::resize(X, Y, W, H);
        grid1->resize(100, 100, 200, 180);
        grid2->resize(300, 280, W - 300, H - 280);
    }

    static void Callback1(Fl_Widget*, void* v) {
        Test* w = (Test*) v;

        w->grid1->remove(w->b1);
        w->b1 = nullptr;
        w->grid1->resize();
    }

    static void Callback2(Fl_Widget*, void* v) {
        Test* w = (Test*) v;

        w->grid2->remove(w->b2);
        w->b2 = nullptr;
        w->grid2->resize();
    }

    GridGroup* grid1;
    GridGroup* grid2;
    Fl_Button* b1;
    Fl_Button* b2;
};

int main(int argc, const char** argv) {
    if (flw::theme::parse(argc, argv) == false) {
        flw::theme::load("gtk");
    }

    Test win(800, 480);
    win.show();
    return Fl::run();
}
