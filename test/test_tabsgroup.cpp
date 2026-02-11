// Copyright gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#include "flw.h"

#ifndef FLW_AMALGAM
    #include "dlg.h"
    #include "tabsgroup.h"
    #include "theme.h"
#endif

//FL_EXPORT bool fl_disable_wayland = true;

#include <assert.h>
#include <FL/Fl_Button.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Text_Editor.H>

using namespace flw;

class Test1;
class Test2;
void update_pref();
void update_tabs(Fl_Boxtype boxtype = FL_MAX_BOXTYPE, Fl_Boxtype dboxtype = FL_MAX_BOXTYPE);

int    BORDER = 0;
Test1* TEST1 = nullptr;
Test2* TEST2 = nullptr;

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

//----------------------------------------------------------------------------------
class Test1 : public Fl_Double_Window {
public:
    //------------------------------------------------------------------------------
    Test1(int W, int H) : Fl_Double_Window(W, H, "Test1 :: test_tabsgroup.cpp") {
        end();

        TEST1   = this;
        boxtype = FL_MAX_BOXTYPE;

        //tabs = new TabsGroup(50, 50, W - 100, H - 100);
        tabs = new TabsGroup(0, 0, W, H);

        n  = new Fl_Button(0, 0, 0, 0, "Pos::TOP");
        s  = new Fl_Button(0, 0, 0, 0, "Pos::BOTTOM");
        w  = new Fl_Button(0, 0, 0, 0, "Pos::LEFT");
        e  = new Fl_Button(0, 0, 0, 0, "Pos::RIGHT");
        b1 = new Fl_Button(0, 0, 0, 0, "Delete Me");
        b2 = new Fl_Button(0, 0, 0, 0, "Hide/Show");
        b3 = new Fl_Button(0, 0, 0, 0, "Delete Border");
        b4 = new Fl_Button(0, 0, 0, 0, "Set Border");
        b5 = new Fl_Button(0, 0, 0, 0, "Debug");
        b6 = new Fl_Button(0, 0, 0, 0, "Change Tab Label");
        b7 = new Fl_Button(0, 0, 0, 0, "Clear Some Labels");
        b8 = new Fl_Button(0, 0, 0, 0, "Toggle boxtype");
        b9 = new Fl_Button(0, 0, 0, 0, "Theme");

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
        b8->callback(Callback, this);
        b9->callback(Callback, this);

        add(tabs);

        tabs->add("Delete", b1, nullptr, "Testing tooltip in TabsGroup::add()");
        tabs->add("Hide/Show", b2);
        tabs->add("Delete", b3);
        tabs->add("Border", b4);
        tabs->add("Debug", b5);
        tabs->add("Label", b6);
        tabs->add("TOP", n);
        tabs->add("BOTTOM", s);
        tabs->add("LEFT", w);
        tabs->add("RIGHT", e);
        tabs->add("Clear", b7);
        tabs->add("Box", b8);
        tabs->add("Theme", b9);
        tabs->add("Start", new Fl_Button(0, 0, 0, 0, TabsGroup::Help()));
        tabs->do_layout();
        //tabs->value(0);
        //tabs->value(n);
        //tabs->value(nullptr);
        tabs->tooltip("Testing tooltip in TabsGroup::tooltip()", b2);

        resizable(tabs);
        size_range(64, 48);
        Callback(w, nullptr);

        //tabs->color(FL_YELLOW);
        //tabs->value(7);
        //tabs->update_pref(23);
    }

    //------------------------------------------------------------------------------
    static void Callback(Fl_Widget* w, void*) {
        if (w == TEST1->e) {
            TEST1->tabs->tab_pos(TabsGroup::Pos::RIGHT, 8);
            TEST1->tabs->do_layout();
        }
        else if (w == TEST1->n) {
            TEST1->tabs->tab_pos(TabsGroup::Pos::TOP);
            TEST1->tabs->do_layout();
        }
        else if (w == TEST1->s) {
            TEST1->tabs->tab_pos(TabsGroup::Pos::BOTTOM, 8);
            TEST1->tabs->do_layout();
        }
        else if (w == TEST1->w) {
            TEST1->tabs->tab_pos(TabsGroup::Pos::LEFT);
            TEST1->tabs->do_layout();
        }
        else if (w == TEST1->b1) {
            auto w = TEST1->tabs->remove(TEST1->b1);
            TEST1->tabs->do_layout();
            delete w;
        }
        else if (w == TEST1->b2) {
            if (TEST1->tabs->is_tabs_visible() == false) TEST1->tabs->show_tabs();
            else TEST1->tabs->hide_tabs();
        }
        else if (w == TEST1->b3) {
            auto w = TEST1->tabs->remove(TEST1->b4);
            TEST1->tabs->do_layout();
            delete w;
        }
        else if (w == TEST1->b4) {
            BORDER++;

            if (BORDER == 1) {
                TEST1->tabs->border(4, 20, 4, 20);
            }
            else if (BORDER == 2) {
                TEST1->tabs->border(0, 0, 20, 20);
            }
            else if (BORDER == 3) {
                TEST1->tabs->border(20, 20);
            }
            else {
                TEST1->tabs->border();
                BORDER = 0;
            }
        }
        else if (w == TEST1->b5) {
            TEST1->tabs->debug();
        }
        else if (w == TEST1->b6) {
            TEST1->tabs->tab_label("This Is A New Label", TEST1->b6);
            TEST1->tabs->do_layout();
        }
        else if (w == TEST1->b7) {
            TEST1->tabs->tab_label("", TEST1->b1);
            TEST1->tabs->tab_label("", TEST1->b2);
            TEST1->tabs->tab_label("", TEST1->b3);
            TEST1->tabs->tab_label("", TEST1->b4);
            TEST1->tabs->tab_label("", TEST1->b5);
            TEST1->tabs->tab_label("", TEST1->b6);
            TEST1->tabs->tab_label("", TEST1->b7);
            TEST1->tabs->do_layout();
        }
        else if (w == TEST1->b8) {
            if (TEST1->boxtype == FL_MAX_BOXTYPE) {
                TEST1->tabs->tab_box(FL_BORDER_BOX, FL_BORDER_BOX);
                TEST1->boxtype = FL_BORDER_BOX;
                update_tabs(FL_BORDER_BOX, FL_BORDER_BOX);
            }
            else if (TEST1->boxtype == FL_BORDER_BOX) {
                TEST1->tabs->tab_box(FL_FLAT_BOX, FL_FLAT_BOX);
                TEST1->boxtype = FL_FLAT_BOX;
                update_tabs(FL_FLAT_BOX, FL_FLAT_BOX);
            }
            //else if (TEST1->boxtype == FL_FLAT_BOX) {
                //TEST1->tabs->tab_box(FL_THIN_UP_BOX, FL_MAX_BOXTYPE);
                //TEST1->boxtype = FL_THIN_UP_BOX;
                //update_tabs(FL_THIN_UP_BOX, FL_MAX_BOXTYPE);
            //}
            else {
                TEST1->tabs->tab_box();
                TEST1->boxtype = FL_MAX_BOXTYPE;
                update_tabs(FL_MAX_BOXTYPE, FL_MAX_BOXTYPE);
            }

            Fl::redraw();
        }
        else if (w == TEST1->b9) {
            flw::dlg::theme(true);
            flw::util::labelfont(TEST1);
            TEST1->tabs->update_pref();
            TEST1->tabs->do_layout();
            update_pref();
            Fl::redraw();
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
    Fl_Button*      b8;
    Fl_Button*      b9;
    Fl_Boxtype      boxtype;
};

/*
 *      ______    _ _ _
 *     |  ____|  | (_) |
 *     | |__   __| |_| |_ ___  _ __
 *     |  __| / _` | | __/ _ \| '__|
 *     | |___| (_| | | || (_) | |
 *     |______\__,_|_|\__\___/|_|
 *
 *
 */

static int COUNT = 0;

//----------------------------------------------------------------------------------
class Editor : public Fl_Text_Editor {
public:
    Fl_Text_Buffer* b;

    //------------------------------------------------------------------------------
    Editor(const char* t) : Fl_Text_Editor(0, 0, 0, 0) {
        b = new Fl_Text_Buffer();
        buffer(b);
        b->text(t);
    }

    //------------------------------------------------------------------------------
    ~Editor() {
        buffer(nullptr);
        delete b;
    }

    //------------------------------------------------------------------------------
    void resize(int X, int Y, int W, int H) override {
        Fl_Text_Editor::resize(X, Y, W, H);
        COUNT++;
        FLW_PRINTV(COUNT)
    }
};

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

//----------------------------------------------------------------------------------
class Test2 : public Fl_Double_Window {
public:
    //------------------------------------------------------------------------------
    Test2(int W, int H) : Fl_Double_Window(W, H, "Test2 :: test_tabsgroup.cpp") {
        end();

        TEST2  = this;
        shrink = false;
        tabs   = new TabsGroup(0, 0, W, H);
        b1     = new Fl_Button(0, 0, 0, 0, "&Add");
        b2     = new Fl_Button(0, 0, 0, 0, "Insert first");
        b11    = new Fl_Button(0, 0, 0, 0, "Insert before");
        b3     = new Fl_Button(0, 0, 0, 0, "Add last");
        b12    = new Fl_Button(0, 0, 0, 0, "Add after");
        b4     = new Fl_Button(0, 0, 0, 0, "Delete all");
        b10    = new Fl_Button(0, 0, 0, 0, "Delete curr");
        b13    = new Fl_Button(0, 0, 0, 0, "Asc");
        b14    = new Fl_Button(0, 0, 0, 0, "Desc");
        b5     = new Fl_Button(0, 0, 0, 0, "&Top");
        b51    = new Fl_Button(0, 0, 0, 0, "Top2");
        b6     = new Fl_Button(0, 0, 0, 0, "&Bot");
        b61    = new Fl_Button(0, 0, 0, 0, "Bot2");
        b7     = new Fl_Button(0, 0, 0, 0, "&Left");
        b71    = new Fl_Button(0, 0, 0, 0, "Left2");
        b8     = new Fl_Button(0, 0, 0, 0, "&Rig");
        b81    = new Fl_Button(0, 0, 0, 0, "Rig2");
        b9     = new Fl_Button(0, 0, 0, 0, "&Debug");
        b16    = new Fl_Button(0, 0, 0, 0, "Hide");
        b17    = new Fl_Button(0, 0, 0, 0, "S&ize");
        inp    = new Fl_Input(0, 0, 0, 0);

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
        add(b51);
        add(b6);
        add(b61);
        add(b7);
        add(b71);
        add(b8);
        add(b81);
        add(b9);
        add(b16);
        add(b17);
        add(tabs);
        add(inp);

        b1->callback(Test2::Callback, this);
        b2->callback(Test2::Callback, this);
        b3->callback(Test2::Callback, this);
        b4->callback(Test2::Callback, this);
        b5->callback(Test2::Callback, this);
        b51->callback(Test2::Callback, this);
        b6->callback(Test2::Callback, this);
        b61->callback(Test2::Callback, this);
        b7->callback(Test2::Callback, this);
        b71->callback(Test2::Callback, this);
        b8->callback(Test2::Callback, this);
        b81->callback(Test2::Callback, this);
        b9->callback(Test2::Callback, this);
        b10->callback(Test2::Callback, this);
        b11->callback(Test2::Callback, this);
        b12->callback(Test2::Callback, this);
        b13->callback(Test2::Callback, this);
        b14->callback(Test2::Callback, this);
        b16->callback(Test2::Callback, this);
        b17->callback(Test2::Callback, this);
        tabs->callback(Test2::Callback, this);
        tabs->disable_keyboard();
        tabs->enable_keyboard();
        //tabs->max_top_width(5);
        tabs->tab_pos(TabsGroup::Pos::LEFT);
        tabs->tab_color(FL_GREEN);
        //tabs->tab_pos(TabsGroup::Pos::LEFT, 20);
        //tabs->tab_pos(TabsGroup::Pos::BOTTOM, 20);
        tabs->do_layout();

        resizable(tabs);
        size_range(64, 48);
        flw::util::labelfont(this);
        create(16, "add last", true);
        //tabs->debug(true);
    }

    //------------------------------------------------------------------------------
    static void Callback(Fl_Widget* w, void*) {
        if (w == TEST2->tabs) {
            auto tabs = (TabsGroup*) w;
            printf("callback with reason %d, label = %s\n", Fl::callback_reason(), tabs->tab_label().c_str());
        }
        else if (w == TEST2->b1) {
            TEST2->create(32, "add last", false);
            //TEST2->create(4, "add last", true);
        }
        else if (w == TEST2->b2) {
            TEST2->create(1, "insert first");
        }
        else if (w == TEST2->b11) {
            TEST2->create(1, "insert before");
        }
        else if (w == TEST2->b3) {
            TEST2->create(1, "add last");
        }
        else if (w == TEST2->b12) {
            TEST2->create(1, "add after");
        }
        else if (w == TEST2->b4) {
            TEST2->tabs->clear();
        }
        else if (w == TEST2->b10) {
            delete TEST2->tabs->remove(TEST2->tabs->value());
        }
        else if (w == TEST2->b13) {
            TEST2->tabs->sort(true, false);
        }
        else if (w == TEST2->b14) {
            TEST2->tabs->sort(false, false);
        }
        else if (w == TEST2->b5) {
            TEST2->tabs->tab_pos(TabsGroup::Pos::TOP);
        }
        else if (w == TEST2->b51) {
            TEST2->tabs->tab_pos(TabsGroup::Pos::TOP2);
        }
        else if (w == TEST2->b6) {
            TEST2->tabs->tab_pos(TabsGroup::Pos::BOTTOM);
        }
        else if (w == TEST2->b61) {
            TEST2->tabs->tab_pos(TabsGroup::Pos::BOTTOM2);
        }
        else if (w == TEST2->b7) {
            TEST2->tabs->tab_pos(TabsGroup::Pos::LEFT);
        }
        else if (w == TEST2->b71) {
            TEST2->tabs->tab_pos(TabsGroup::Pos::LEFT2);
        }
        else if (w == TEST2->b8) {
            TEST2->tabs->tab_pos(TabsGroup::Pos::RIGHT);
        }
        else if (w == TEST2->b81) {
            TEST2->tabs->tab_pos(TabsGroup::Pos::RIGHT2);
        }
        else if (w == TEST2->b9) {
            //flw::debug::print(TEST);
            TEST2->tabs->debug(true);
            Fl::redraw();
            Fl::flush();
        }
        else if (w == TEST2->b16) {
            if (TEST2->tabs->is_tabs_visible()) {
                TEST2->tabs->hide_tabs();
                TEST2->b16->label("Show");
            }
            else {
                TEST2->tabs->show_tabs();
                TEST2->b16->label("Hide");
            }
        }
        else if (w == TEST2->b17) {
            TEST2->shrink = !TEST2->shrink;
            TEST2->size(TEST2->w(), TEST2->h());
        }

        TEST2->tabs->take_focus();
    }

    //------------------------------------------------------------------------------
    void create(int num, std::string what, bool clear = false) {
        if (clear == true) {
            tabs->clear();
        }

        auto count = tabs->children();
        auto first = (Fl_Widget*) nullptr;

        srand(time(nullptr));

        for (int f = 1; f <= num; f++) {
            char l[100];
            snprintf(l, 100, "Widget %02d", ++count);
            auto t = new Editor(l);
            auto r = rand() % 4;

            if (first == nullptr) {
                first = t;
            }

            if (r == 0) {
                snprintf(l, 100, "123456789012345678901234567890 %05d", count);
            }
            else {
                snprintf(l, 100, "Button %02d", count);
            }

            if (what == "add last") {
                tabs->add(l, t, nullptr, l);
            }
            else if (what == "add after") {
                tabs->add(l, t, TEST2->tabs->value(), l);
            }
            else if (what == "insert first") {
                tabs->insert(l, t, nullptr, l);
            }
            else if (what == "insert before") {
                tabs->insert(l, t, TEST2->tabs->value(), l);
            }
            else {
                assert(false);
            }
        }

        tabs->do_layout();
    }

    //------------------------------------------------------------------------------
    void resize(int X, int Y, int W, int H) override {
        auto fs = flw::PREF_FONTSIZE;

        Fl_Double_Window::resize(X, Y, W, H);

        if (shrink == false) {
            tabs->resize(0, fs * 2, W, H - fs * 4);
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
        b51->resize(fs * 54, 0, 3 * fs, fs * 2);
        b6->resize(fs * 57, 0, 3 * fs, fs * 2);
        b61->resize(fs * 60, 0, 3 * fs, fs * 2);
        b7->resize(fs * 63, 0, 3 * fs, fs * 2);
        b71->resize(fs * 66, 0, 3 * fs, fs * 2);
        b8->resize(fs * 69, 0, 3 * fs, fs * 2);
        b81->resize(fs * 72, 0, 3 * fs, fs * 2);
        b9->resize(fs * 75, 0, 4 * fs, fs * 2);
        b16->resize(fs * 79, 0, 4 * fs, fs * 2);
        b17->resize(fs * 83, 0, 4 * fs, fs * 2);
        inp->resize(0, H - fs * 2, W, fs * 2);
    }

    TabsGroup*      tabs;
    Fl_Button*      b1;
    Fl_Button*      b2;
    Fl_Button*      b3;
    Fl_Button*      b4;
    Fl_Button*      b5;
    Fl_Button*      b51;
    Fl_Button*      b6;
    Fl_Button*      b61;
    Fl_Button*      b7;
    Fl_Button*      b71;
    Fl_Button*      b8;
    Fl_Button*      b81;
    Fl_Button*      b9;
    Fl_Button*      b10;
    Fl_Button*      b11;
    Fl_Button*      b12;
    Fl_Button*      b13;
    Fl_Button*      b14;
    Fl_Button*      b16;
    Fl_Button*      b17;
    Fl_Input*       inp;
    bool            shrink;
};

//----------------------------------------------------------------------------------
void update_pref() {
    flw::util::labelfont(TEST2);
    TEST2->tabs->update_pref();
    TEST2->resize(TEST2->x(), TEST2->y(), TEST2->w(), TEST2->h());
    TEST2->tabs->do_layout();
}

//----------------------------------------------------------------------------------
void update_tabs(Fl_Boxtype boxtype, Fl_Boxtype dboxtype) {
    TEST2->tabs->tab_box(boxtype, dboxtype);
}

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

//----------------------------------------------------------------------------------
int main(int argc, const char** argv) {
    if (flw::theme::parse(argc, argv) == false) {
        flw::theme::load("oxy");
    }

    Fl::screen_scale(0, 1.0);
    //flw::PREF_FONTSIZE = 10;

    {
        Test1 test1(800, 600);
        test1.show();

        Test2 test2(1200, 600);
        test2.show();

        Fl::run();
    }

    puts("DONE");
    return 0;
}
