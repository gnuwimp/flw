// Copyright gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#include "flw.h"

#ifndef FLW_AMALGAM
    #include "tabsgroup.h"
    #include "dlg.h"
#endif

#include <assert.h>
#include <FL/Fl_Button.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Text_Editor.H>

using namespace flw;

int BORDER = 0;

/*
 *      _______        _  __
 *     |__   __|      | |/_ |
 *        | | ___  ___| |_| |
 *        | |/ _ \/ __| __| |
 *        | |  __/\__ \ |_| |
 *        |_|\___||___/\__|_|
 *
 *
 */

class Test1 : public Fl_Double_Window {
public:
    Test1(int W, int H) : Fl_Double_Window(W, H, "Test1 :: test_tabsgroup.cpp") {
        end();

        TEST = this;

//        tabs = new TabsGroup(50, 50, W - 100, H - 100);
        tabs = new TabsGroup(0, 0, W, H);

        n  = new Fl_Button(0, 0, 0, 0, "Pos::NORTH");
        s  = new Fl_Button(0, 0, 0, 0, "Pos::SOUTH");
        w  = new Fl_Button(0, 0, 0, 0, "Pos::WEST");
        e  = new Fl_Button(0, 0, 0, 0, "Pos::EAST");
        b1 = new Fl_Button(0, 0, 0, 0, "Delete Me");
        b2 = new Fl_Button(0, 0, 0, 0, "Hide/Show");
        b3 = new Fl_Button(0, 0, 0, 0, "Delete Border");
        b4 = new Fl_Button(0, 0, 0, 0, "Set Border");
        b5 = new Fl_Button(0, 0, 0, 0, "Debug");
        b6 = new Fl_Button(0, 0, 0, 0, "Change Tab Label");
        b7 = new Fl_Button(0, 0, 0, 0, "Clear Some Labels");

        n->callback(Callback, this);
        s->callback(Callback, this);
        e->callback(Callback, this);
        w->callback(Callback, this);
        b1->callback(Callback, this);
        b2->callback(Callback, this);
        b3->callback(Callback, this);
        b4->callback(Callback, this);
        b5->callback(Callback, this);
        b6->callback(Callback, this);
        b7->callback(Callback, this);

        add(tabs);

        tabs->add("Delete", b1, nullptr, "Testing tooltip in TabsGroup::add()");
        tabs->add("Hide/Show", b2);
        tabs->add("Delete", b3);
        tabs->add("Border", b4);
        tabs->add("Debug", b5);
        tabs->add("Label", b6);
        tabs->add("N", n);
        tabs->add("S", s);
        tabs->add("W", w);
        tabs->add("E", e);
        tabs->add("CLEAR", b7);
        tabs->add("START", new Fl_Button(0, 0, 0, 0, TabsGroup::Help()));
        tabs->box(FL_BORDER_BOX);
        tabs->do_layout();
        tabs->tooltip("Testing tooltip in TabsGroup::tooltip()", b2);

        resizable(tabs);
        size_range(64, 48);
        Callback(w, nullptr);

//        tabs->color(FL_YELLOW);
//        tabs->value(7);
//        tabs->update_pref(23);
    }

    static void Callback(Fl_Widget* w, void*) {
        if (w == TEST->e) {
            TEST->tabs->tab_pos(TabsGroup::Pos::RIGHT, 8);
            TEST->tabs->do_layout();
        }
        else if (w == TEST->n) {
            TEST->tabs->tab_pos(TabsGroup::Pos::TOP);
            TEST->tabs->do_layout();
        }
        else if (w == TEST->s) {
            TEST->tabs->tab_pos(TabsGroup::Pos::BOTTOM, 8);
            TEST->tabs->do_layout();
        }
        else if (w == TEST->w) {
            TEST->tabs->tab_pos(TabsGroup::Pos::LEFT);
            TEST->tabs->do_layout();
        }
        else if (w == TEST->b1) {
            auto w = TEST->tabs->remove(TEST->b1);
            TEST->tabs->do_layout();
            delete w;
        }
        else if (w == TEST->b2) {
            if (TEST->tabs->is_tabs_visible() == false) TEST->tabs->show_tabs();
            else TEST->tabs->hide_tabs();
        }
        else if (w == TEST->b3) {
            auto w = TEST->tabs->remove(TEST->b4);
            TEST->tabs->do_layout();
            delete w;
        }
        else if (w == TEST->b4) {
            BORDER++;

            if (BORDER == 1) {
                TEST->tabs->border(4, 20, 4, 20);
            }
            else if (BORDER == 2) {
                TEST->tabs->border(0, 0, 20, 20);
            }
            else if (BORDER == 3) {
                TEST->tabs->border(20, 20);
            }
            else {
                TEST->tabs->border();
                BORDER = 0;
            }
        }
        else if (w == TEST->b5) {
            TEST->tabs->debug();
        }
        else if (w == TEST->b6) {
            TEST->tabs->label("This Is A New Label", TEST->b6);
            TEST->tabs->do_layout();
        }
        else if (w == TEST->b7) {
            TEST->tabs->label("", TEST->b1);
            TEST->tabs->label("", TEST->b2);
            TEST->tabs->label("", TEST->b3);
            TEST->tabs->label("", TEST->b4);
            TEST->tabs->label("", TEST->b5);
            TEST->tabs->label("", TEST->b6);
            TEST->tabs->label("", TEST->b7);
            TEST->tabs->do_layout();
        }
    }

    TabsGroup*      tabs;
    Fl_Button*      e;
    Fl_Button*      n;
    Fl_Button*      s;
    Fl_Button*      w;
    Fl_Button*      b1;
    Fl_Button*      b2;
    Fl_Button*      b3;
    Fl_Button*      b4;
    Fl_Button*      b5;
    Fl_Button*      b6;
    Fl_Button*      b7;
    static Test1*   TEST;
};

Test1* Test1::TEST = nullptr;

/*
 *      _______        _   ___
 *     |__   __|      | | |__ \
 *        | | ___  ___| |_   ) |
 *        | |/ _ \/ __| __| / /
 *        | |  __/\__ \ |_ / /_
 *        |_|\___||___/\__|____|
 *
 *
 */

class Editor : public Fl_Text_Editor {
public:
    Fl_Text_Buffer* b;

    Editor(const char* t) : Fl_Text_Editor(0, 0, 0, 0) {
        b = new Fl_Text_Buffer();
        buffer(b);
        b->text(t);
    }

    ~Editor() {
        buffer(nullptr);
        delete b;
    }
};

class Test2 : public Fl_Double_Window {
public:
    Test2(int W, int H) : Fl_Double_Window(W, H, "Test2 :: test_tabsgroup.cpp") {
        end();

        TEST   = this;
        shrink = false;
        tabs   = new TabsGroup(0, 0, W, H);
        b1     = new Fl_Button(0, 0, 0, 0, "Add");
        b2     = new Fl_Button(0, 0, 0, 0, "Insert first");
        b11    = new Fl_Button(0, 0, 0, 0, "Insert before");
        b3     = new Fl_Button(0, 0, 0, 0, "Add last");
        b12    = new Fl_Button(0, 0, 0, 0, "Add after");
        b4     = new Fl_Button(0, 0, 0, 0, "Delete all");
        b10    = new Fl_Button(0, 0, 0, 0, "Delete curr");
        b13    = new Fl_Button(0, 0, 0, 0, "Asc");
        b14    = new Fl_Button(0, 0, 0, 0, "Desc");
        b5     = new Fl_Button(0, 0, 0, 0, "&N");
        b6     = new Fl_Button(0, 0, 0, 0, "&S");
        b7     = new Fl_Button(0, 0, 0, 0, "&W");
        b8     = new Fl_Button(0, 0, 0, 0, "&E");
        b9     = new Fl_Button(0, 0, 0, 0, "&Debug");
        b15    = new Fl_Button(0, 0, 0, 0, "Theme");
        b16    = new Fl_Button(0, 0, 0, 0, "Hide");
        b17    = new Fl_Button(0, 0, 0, 0, "Size");

        add(b1);
        add(b2);
        add(b11);
        add(b3);
        add(b12);
        add(b4);
        add(b10);
        add(b13);
        add(b14);
        add(b5);
        add(b6);
        add(b7);
        add(b8);
        add(b9);
        add(b15);
        add(b16);
        add(b17);
        add(tabs);

        b1->callback(Test2::Callback, this);
        b2->callback(Test2::Callback, this);
        b3->callback(Test2::Callback, this);
        b4->callback(Test2::Callback, this);
        b5->callback(Test2::Callback, this);
        b6->callback(Test2::Callback, this);
        b7->callback(Test2::Callback, this);
        b8->callback(Test2::Callback, this);
        b9->callback(Test2::Callback, this);
        b10->callback(Test2::Callback, this);
        b11->callback(Test2::Callback, this);
        b12->callback(Test2::Callback, this);
        b13->callback(Test2::Callback, this);
        b14->callback(Test2::Callback, this);
        b15->callback(Test2::Callback, this);
        b16->callback(Test2::Callback, this);
        b17->callback(Test2::Callback, this);
        tabs->tab_pos(TabsGroup::Pos::WEST);
        tabs->disable_keyboard();
        tabs->enable_keyboard();
        //tabs->tabs(TabsGroup::Pos::NORTH);

        resizable(tabs);
        size_range(64, 48);
        flw::util::labelfont(this);
        create(16, "add last", true);
    }

    static void Callback(Fl_Widget* w, void*) {
        if (w == TEST->b1) {
            TEST->create(32, "add last", false);
            //TEST->create(4, "add last", true);
        }
        else if (w == TEST->b2) {
            TEST->create(1, "insert first");
        }
        else if (w == TEST->b11) {
            TEST->create(1, "insert before");
        }
        else if (w == TEST->b3) {
            TEST->create(1, "add last");
        }
        else if (w == TEST->b12) {
            TEST->create(1, "add after");
        }
        else if (w == TEST->b4) {
            TEST->tabs->clear();
        }
        else if (w == TEST->b10) {
            delete TEST->tabs->remove(TEST->tabs->value());
        }
        else if (w == TEST->b13) {
            TEST->tabs->sort(true, false);
        }
        else if (w == TEST->b14) {
            TEST->tabs->sort(false, false);
        }
        else if (w == TEST->b5) {
            TEST->tabs->tab_pos(TabsGroup::Pos::NORTH);
        }
        else if (w == TEST->b6) {
            TEST->tabs->tab_pos(TabsGroup::Pos::SOUTH);
        }
        else if (w == TEST->b7) {
            TEST->tabs->tab_pos(TabsGroup::Pos::WEST);
        }
        else if (w == TEST->b8) {
            TEST->tabs->tab_pos(TabsGroup::Pos::EAST);
        }
        else if (w == TEST->b9) {
//            flw::debug::print(TEST);
            TEST->tabs->debug(false);
            Fl::redraw();
        }
        else if (w == TEST->b15) {
            flw::dlg::theme(true);
            flw::util::labelfont(TEST);
            TEST->tabs->update_pref();
            TEST->size(TEST->w(), TEST->h());
        }
        else if (w == TEST->b16) {
            if (TEST->tabs->is_tabs_visible()) {
                TEST->tabs->hide_tabs();
                TEST->b16->label("Show");
            }
            else {
                TEST->tabs->show_tabs();
                TEST->b16->label("Hide");
            }
        }
        else if (w == TEST->b17) {
            TEST->shrink = !TEST->shrink;
            TEST->size(TEST->w(), TEST->h());
        }

        TEST->tabs->take_focus();
    }

    void create(int num, std::string what, bool clear = false) {
        if (clear == true) {
            tabs->clear();
        }

        auto count = tabs->children();
        auto first = (Fl_Widget*) nullptr;

        srand(time(nullptr));

        for (int f = 1; f <= num; f++) {
            char l[100];
            char l2[100];
            snprintf(l, 100, "Widget %02d", ++count);
            snprintf(l2, 100, "%06d", count);
            auto t = new Editor(l);
            auto r = rand() % 4;

            if (first == nullptr) {
                first = t;
            }

            if (r == 0) {
                snprintf(l, 100, "Button %02d", count);
            }
            else if (r == 1) {
                snprintf(l, 100, "button %02d", count);
            }
            else if (r == 2) {
                snprintf(l, 100, "Wider button %02d", count);
            }
            else if (r == 3) {
                snprintf(l, 100, "wider button %02d", count);
            }

            if (what == "add last") {
                tabs->add(l, t);
            }
            else if (what == "add after") {
                tabs->add(l, t, TEST->tabs->value());
            }
            else if (what == "insert first") {
                tabs->insert(l, t);
            }
            else if (what == "insert before") {
                tabs->insert(l, t, TEST->tabs->value());
            }
            else {
                assert(false);
            }

            tabs->tooltip(l2, t);
        }

        tabs->do_layout();
    }

    void resize(int X, int Y, int W, int H) override {
        auto fs = flw::PREF_FONTSIZE;

        Fl_Double_Window::resize(X, Y, W, H);

        if (shrink == false) {
            tabs->resize(0, fs * 2, W, H - fs * 2);
        }
        else {
            tabs->resize(50, 50, W - 100, H - 100);
        }

        b1->resize(0, 0, 3 * fs, fs * 2);
        b2->resize(fs * 3, 0, 7 * fs, fs * 2);
        b11->resize(fs * 10, 0, 7 * fs, fs * 2);
        b3->resize(fs * 17, 0, 7 * fs, fs * 2);
        b12->resize(fs * 24, 0, 7 * fs, fs * 2);
        b4->resize(fs * 31, 0, 7 * fs, fs * 2);
        b10->resize(fs * 38, 0, 7 * fs, fs * 2);
        b13->resize(fs * 45, 0, 3 * fs, fs * 2);
        b14->resize(fs * 48, 0, 3 * fs, fs * 2);
        b5->resize(fs * 51, 0, 3 * fs, fs * 2);
        b6->resize(fs * 54, 0, 3 * fs, fs * 2);
        b7->resize(fs * 57, 0, 3 * fs, fs * 2);
        b8->resize(fs * 60, 0, 3 * fs, fs * 2);
        b9->resize(fs * 63, 0, 4 * fs, fs * 2);
        b15->resize(fs * 67, 0, 4 * fs, fs * 2);
        b16->resize(fs * 71, 0, 4 * fs, fs * 2);
        b17->resize(fs * 74, 0, 4 * fs, fs * 2);
    }

    TabsGroup*      tabs;
    Fl_Button*      b1;
    Fl_Button*      b2;
    Fl_Button*      b3;
    Fl_Button*      b4;
    Fl_Button*      b5;
    Fl_Button*      b6;
    Fl_Button*      b7;
    Fl_Button*      b8;
    Fl_Button*      b9;
    Fl_Button*      b10;
    Fl_Button*      b11;
    Fl_Button*      b12;
    Fl_Button*      b13;
    Fl_Button*      b14;
    Fl_Button*      b15;
    Fl_Button*      b16;
    Fl_Button*      b17;
    bool            shrink;
    static Test2*   TEST;
};

Test2* Test2::TEST = nullptr;

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

int main(int argc, const char** argv) {
    if (flw::theme::parse(argc, argv) == false) {
        flw::theme::load("oxy");
    }

//    flw::PREF_FONTSIZE = 10;

    {
        Test1 test1(800, 600);
        test1.show();

        Test2 test2(1100, 600);
        test2.show();

        Fl::run();
    }

    puts("DONE");
    return 0;
}
