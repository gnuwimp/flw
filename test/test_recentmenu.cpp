// Copyright gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#include "flw.h"

#ifndef FLW_AMALGAM
    #include "recentmenu.h"
    #include "theme.h"
#endif

#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Preferences.H>
#include <FL/Fl_Return_Button.H>

//------------------------------------------------------------------------------
class Test : public Fl_Double_Window {
    Fl_Button*                  add_button;
    Fl_Button*                  insert_button;
    Fl_Menu_Bar*                menu;
    flw::util::RecentMenu*      add_recent;
    flw::util::RecentMenu*      insert_recent;
    int                         count;

public:
    //------------------------------------------------------------------------------
    Test() : Fl_Double_Window(640, 480, "test_recentmenu.cpp") {
        end();

        menu          = new Fl_Menu_Bar(0, 0, 0, 0);
        add_button    = new Fl_Return_Button(0, 0, 0, 0, "Add Item");
        insert_button = new Fl_Return_Button(0, 0, 0, 0, "Insert Item");
        count         = 0;

        add(menu);
        add(add_button);
        add(insert_button);

        add_button->callback(Test::Callback, this);
        insert_button->callback(Test::Callback, this);
        menu->callback(Test::Callback, this);

        menu->add("&File/Open...",                     FL_COMMAND + 'o',           Test::Callback, this);
        insert_recent = new flw::util::RecentMenu(menu, CallbackInsert, this);
        insert_recent->max_items(5);
        add_recent = new flw::util::RecentMenu(menu, CallbackAdd, this, "&File/Open recent (add)");
        add_recent->max_items(5);
        menu->add("&File/Clear",                       0,                          Test::Callback2, this);
        menu->add("&File/Save",                        FL_COMMAND + 's',           Test::Callback, this);
        menu->add("&File/Close",                       0,                          Test::Callback, this);
        menu->add("&File/Quit",                        FL_COMMAND + 'q',           Test::Callback, this);

        resizable(this);
        size_range(320, 240);
        callback(Test::Callback, this);
        show();

        auto pref = Fl_Preferences(Fl_Preferences::USER, "gnuwimp_test", "test_recentmenu");
        insert_recent->load_pref(pref);
        add_recent->load_pref(pref, "files2");
    }

    //------------------------------------------------------------------------------
    ~Test() {
        auto pref = Fl_Preferences(Fl_Preferences::USER, "gnuwimp_test", "test_recentmenu");
        pref.clear();
        insert_recent->save_pref(pref);
        add_recent->save_pref(pref, "files2");
        
        for (auto& insert : insert_recent->items()) {
            FLW_PRINTV(insert)
            (void) insert;
        }
    }

    //------------------------------------------------------------------------------
    static void Callback(Fl_Widget* w, void* o) {
        auto self = (Test*) o;

        if (w == self) {
            self->hide();
        }
        else if (w == self->add_button) {
            self->make_menu(self->add_recent, true);
        }
        else if (w == self->insert_button) {
            self->make_menu(self->insert_recent, false);
        }
    }

    //------------------------------------------------------------------------------
    static void Callback2(Fl_Widget*, void* o) {
        auto self = (Test*) o;
        self->insert_recent->clear();
        self->add_recent->clear();
    }

    //------------------------------------------------------------------------------
    static void CallbackAdd(Fl_Widget*, void* o) {
        auto self = (Test*) o;
        self->add_button->copy_label(self->menu->text());
    }

    //------------------------------------------------------------------------------
    static void CallbackInsert(Fl_Widget*, void* o) {
        auto self = (Test*) o;
        self->insert_button->copy_label(self->menu->text());
    }

    //------------------------------------------------------------------------------
    void make_menu(flw::util::RecentMenu* recent, bool append) {
        auto label = flw::util::format("/this/is/a/file/number & _%d_", ++count);
        if (append == false) recent->insert(label);
        else recent->append(label);
    }

    //------------------------------------------------------------------------------
    void resize(int X, int Y, int W, int H) override {
        auto w = W / 2;

        Fl_Double_Window::resize(X, Y, W, H);
        menu->resize(0, 0, W, 28);
        insert_button->resize(0, 28, w, H - 28);
        add_button->resize(w, 28, w, H - 28);
    }
};

//------------------------------------------------------------------------------
int main() {
#ifdef FLW_THEME
    if (flw::theme::parse(argc, argv) == false) {
        flw::theme::load("oxy");
    }
#endif

    Test app;
    Fl::run();
    return 0;
}
