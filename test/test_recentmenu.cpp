// Copyright gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#include <FL/Fl_Return_Button.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Preferences.H>
#include "recentmenu.h"

//------------------------------------------------------------------------------
class Test : public Fl_Double_Window {
    Fl_Button*          add_button;
    flw::RecentMenu*    add_recent;
    int                 count;
    Fl_Button*          insert_button;
    flw::RecentMenu*    insert_recent;
    Fl_Menu_Bar*        menu;

public:
    //--------------------------------------------------------------------------
    Test() : Fl_Double_Window(640, 480, "flCalc") {
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
        insert_recent = new flw::RecentMenu(menu, CallbackInsert, this);
        menu->add("&File/Save",                        FL_COMMAND + 's',           Test::Callback, this);
        add_recent = new flw::RecentMenu(menu, CallbackAdd, this, "&File/Open recent (add)");
        menu->add("&File/Close",                       0,                          Test::Callback, this);
        menu->add("&File/Quit",                        FL_COMMAND + 'q',           Test::Callback, this);

        resizable(this);
        size_range(320, 240);
        callback(Test::Callback, this);
        show();

        auto pref = Fl_Preferences(Fl_Preferences::USER, "gnuwimp", "test_recentmenu");
        insert_recent->load_pref(pref);
        add_recent->load_pref(pref, "files2");
    }

    //--------------------------------------------------------------------------
    ~Test() {
        auto pref = Fl_Preferences(Fl_Preferences::USER, "gnuwimp", "test_recentmenu");
        pref.clear();
        insert_recent->save_pref(pref);
        add_recent->save_pref(pref, "files2");
    }

    //--------------------------------------------------------------------------
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

    //--------------------------------------------------------------------------
    static void CallbackAdd(Fl_Widget* w, void* o) {
        auto self = (Test*) o;
        self->add_button->copy_label(self->menu->text());
    }

    //--------------------------------------------------------------------------
    static void CallbackInsert(Fl_Widget* w, void* o) {
        auto self = (Test*) o;
        self->insert_button->copy_label(self->menu->text());
    }

    //--------------------------------------------------------------------------
    void make_menu(flw::RecentMenu* recent, bool append) {
        auto label = flw::util::format("/this/is/a/file/number & _%d_", ++count);
        if (append == false) recent->insert(label);
        else recent->append(label);
    }

    //--------------------------------------------------------------------------
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
        flw::theme::load("gtk");
    }
#endif

    Test app;
    Fl::run();
    return 0;
}
