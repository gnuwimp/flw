// Copyright 2021 gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#include "tabsgroup.h"
#include "theme.h"
#include "util.h"
#include <FL/Fl_Button.H>
#include <FL/Fl_Double_Window.H>

using namespace flw;

class Test : public Fl_Double_Window {
public:
    Test(int W, int H) : Fl_Double_Window(W, H, "test_tabsgroup.cpp") {
        TEST = this;

        auto n = new Fl_Button(0, 0, 0, 0, "TABS::NORTH");
        auto s = new Fl_Button(0, 0, 0, 0, "TABS::SOUTH");
        auto w = new Fl_Button(0, 0, 0, 0, "TABS::WEST");
        auto e = new Fl_Button(0, 0, 0, 0, "TABS::EAST");

        b1 = new Fl_Button(0, 0, 0, 0, "Delete 1");
        b2 = new Fl_Button(0, 0, 0, 0, "Widget 2");
        b3 = new Fl_Button(0, 0, 0, 0, "Delete 4");
        b4 = new Fl_Button(0, 0, 0, 0, "Widget 4");

        n->callback(CallbackNorth, this);
        s->callback(CallbackSouth, this);
        e->callback(CallbackEast, this);
        w->callback(CallbackWest, this);

        tabs = new TabsGroup(0, 0, W, H);
        // tabs = new TabsGroup(10, 10, W-20, H-20);
        tabs->add("Widget 1", b1);
        tabs->add("Widget 2", b2);
        tabs->add("Widget 3", b3);
        tabs->add("Widget 4", b4);
        tabs->add("NORTH", n);
        tabs->add("SOUTH", s);
        tabs->add("WEST", w);
        tabs->add("EAST", e);
        tabs->add("START", new Fl_Button(0, 0, 0, 0, "Use alt+left/right to move between tabs\nOr alt+[1-9] to select tab\nAnd alt+shift+left/right to move tabs"));
        // tabs->set(7);
        // TabsGroup::BoxType(FL_UP_BOX);
        // TabsGroup::BoxType(FL_ENGRAVED_BOX);
        // TabsGroup::BoxType(FL_DIAMOND_UP_BOX);
        // TabsGroup::BoxType(FL_OSHADOW_BOX);
        // TabsGroup::BoxType(FL_ROUND_DOWN_BOX);
        // TabsGroup::BoxType(FL_THIN_UP_BOX);
        // TabsGroup::BoxType(FL_BORDER_BOX);
        tabs->resize();
        // TabsGroup::BoxColor(FL_YELLOW);
        // TabsGroup::BoxSelectionColor(FL_GREEN);

        tabs->child(8)->callback(CallbackStart, this);
        b1->callback(CallbackWidget1, this);
        b3->callback(CallbackWidget3, this);

        resizable(this);
    }

    static void CallbackEast(Fl_Widget*, void*) {
        TEST->tabs->tabs(TabsGroup::TABS::EAST);
        TEST->tabs->resize();
    }

    static void CallbackNorth(Fl_Widget*, void*) {
        TEST->tabs->tabs(TabsGroup::TABS::NORTH);
        TEST->tabs->resize();
    }

    static void CallbackSouth(Fl_Widget*, void*) {
        TEST->tabs->tabs(TabsGroup::TABS::SOUTH);
        TEST->tabs->resize();
    }

    static void CallbackStart(Fl_Widget* widget, void*) {
        TEST->tabs->label("Label Changed", widget);
        TEST->tabs->resize();
    }

    static void CallbackWest(Fl_Widget*, void*) {
        TEST->tabs->tabs(TabsGroup::TABS::WEST);
        TEST->tabs->resize();
    }

    static void CallbackWidget1(Fl_Widget*, void*) {
        auto w = TEST->tabs->remove(TEST->b1);
        TEST->tabs->resize();
        delete w;
    }

    static void CallbackWidget3(Fl_Widget*, void*) {
        auto w = TEST->tabs->remove(TEST->b4);
        TEST->tabs->resize();
        delete w;
    }

    TabsGroup* tabs;
    Fl_Button* b1, *b2, *b3, *b4;
    static Test* TEST;
};

Test* Test::TEST = nullptr;

int main(int argc, const char** argv) {
    if (flw::theme::parse(argc, argv) == false) {
        flw::theme::load("gtk");
    }

    Test win(640, 480);
    win.show();
    return Fl::run();
}
