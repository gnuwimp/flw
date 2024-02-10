// Copyright gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#include "flw.h"

#ifndef FLW_AMALGAM
    #include "tabsgroup.h"
#endif

#include <FL/Fl_Button.H>
#include <FL/Fl_Double_Window.H>

using namespace flw;

int BORDER = 0;

class Test : public Fl_Double_Window {
public:
    Test(int W, int H) : Fl_Double_Window(W, H, "test_tabsgroup.cpp") {
        end();
        
        TEST = this;

        auto n = new Fl_Button(0, 0, 0, 0, "TABS::NORTH");
        auto s = new Fl_Button(0, 0, 0, 0, "TABS::SOUTH");
        auto w = new Fl_Button(0, 0, 0, 0, "TABS::WEST");
        auto e = new Fl_Button(0, 0, 0, 0, "TABS::EAST");

        b1 = new Fl_Button(0, 0, 0, 0, "Delete Me");
        b2 = new Fl_Button(0, 0, 0, 0, "Hide/Show");
        b3 = new Fl_Button(0, 0, 0, 0, "Delete Border");
        b4 = new Fl_Button(0, 0, 0, 0, "Set Border");

        n->callback(CallbackNorth, this);
        s->callback(CallbackSouth, this);
        e->callback(CallbackEast, this);
        w->callback(CallbackWest, this);

        tabs = new TabsGroup();
        add(tabs);
        // tabs = new TabsGroup(10, 10, W-20, H-20);
        tabs->add("Delete", b1);
        tabs->add("Hide/Show", b2);
        tabs->add("Delete", b3);
        tabs->add("Border", b4);
        tabs->add("NORTH", n);
        tabs->add("SOUTH", s);
        tabs->add("WEST", w);
        tabs->add("EAST", e);
        tabs->add("START", new Fl_Button(0, 0, 0, 0, TabsGroup::Help()));

//        tabs->set(7);
//        TabsGroup::BoxType(FL_UP_BOX);
//        TabsGroup::BoxType(FL_ENGRAVED_BOX);
//        TabsGroup::BoxType(FL_DIAMOND_UP_BOX);
//        TabsGroup::BoxType(FL_OSHADOW_BOX);
//        TabsGroup::BoxType(FL_ROUND_DOWN_BOX);
//        TabsGroup::BoxType(FL_THIN_UP_BOX);
//        TabsGroup::BoxType(FL_BORDER_BOX);
//        tabs->do_layout();
//        TabsGroup::BoxColor(FL_YELLOW);
//        TabsGroup::BoxSelectionColor(FL_GREEN);

        tabs->child(8)->callback(CallbackStart, this);
//        tabs->color(FL_YELLOW);
//        tabs->box(FL_FLAT_BOX);
        b1->callback(CallbackWidget1, this);
        b2->callback(CallbackWidget2, this);
        b3->callback(CallbackWidget3, this);
        b4->callback(CallbackWidget4, this);

//        color(FL_BLUE);
        resizable(tabs);
        size_range(64, 48);
        CallbackWest(nullptr, nullptr);
    }

    void resize(int X, int Y, int W, int H) override {
        Fl_Double_Window::resize(X, Y, W, H);
//        tabs->resize(10, 10, W - 20, H - 20);
        tabs->resize(0, 0, W, H);
    }

    static void CallbackEast(Fl_Widget*, void*) {
        TEST->tabs->tabs(TabsGroup::TABS::EAST);
        TEST->tabs->do_layout();
    }

    static void CallbackNorth(Fl_Widget*, void*) {
        TEST->tabs->tabs(TabsGroup::TABS::NORTH);
        TEST->tabs->do_layout();
    }

    static void CallbackSouth(Fl_Widget*, void*) {
        TEST->tabs->tabs(TabsGroup::TABS::SOUTH);
        TEST->tabs->do_layout();
    }

    static void CallbackStart(Fl_Widget* widget, void*) {
        TEST->tabs->label("Label Changed", widget);
        TEST->tabs->do_layout();
    }

    static void CallbackWest(Fl_Widget*, void*) {
        TEST->tabs->tabs(TabsGroup::TABS::WEST);
        TEST->tabs->do_layout();
    }

    static void CallbackWidget1(Fl_Widget*, void*) {
        auto w = TEST->tabs->remove(TEST->b1);
        TEST->tabs->do_layout();
        delete w;
    }

    static void CallbackWidget2(Fl_Widget*, void*) {
        if (TEST->tabs->tabs_visible() == false) {
            TEST->tabs->show_tabs();
        }
        else {
            TEST->tabs->hide_tabs();
        }
    }

    static void CallbackWidget3(Fl_Widget*, void*) {
        auto w = TEST->tabs->remove(TEST->b4);
        TEST->tabs->do_layout();
        delete w;
    }

    static void CallbackWidget4(Fl_Widget*, void*) {
        BORDER++;

        if (BORDER == 1) {
            TEST->tabs->border(4, 20, 4, 20);
            TabsGroup::BoxType(FL_UP_BOX);
        }
        else if (BORDER == 2) {
            TEST->tabs->border(0, 0, 20, 20);
            TabsGroup::BoxType(FL_DOWN_BOX);
        }
        else if (BORDER == 3) {
            TEST->tabs->border(20, 20);
            TabsGroup::BoxType(FL_ENGRAVED_BOX);
        }
        else {
            TEST->tabs->border();
            TabsGroup::BoxType();
            BORDER = 0;
        }
    }

    TabsGroup* tabs;
    Fl_Button* b1, *b2, *b3, *b4;
    static Test* TEST;
};

Test* Test::TEST = nullptr;

int main(int argc, const char** argv) {
    if (flw::theme::parse(argc, argv) == false) {
        flw::theme::load("oxy");
    }

    Test win(800, 600);
    win.show();
    return Fl::run();
}
