// Copyright 2021 gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#include "widgets.h"
#include "theme.h"
#include "util.h"
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Double_Window.H>

using namespace flw;

class Test : public Fl_Double_Window {
public:
    Test(int W, int H) : Fl_Double_Window(W, H, "test_widgets.cpp") {
        TEST = this;


        {
            box = new Fl_Box(0, 0, 0, 0);
            box->box(FL_BORDER_BOX);
            box->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
        }

        {
            browser = new ScrollBrowser();

            for (int f = 0; f < 200; f++) {
                char buf[100];
                sprintf(buf, "%sRow num %03d", f % 3 == 0 ? "@C88" : f % 2 == 0 ? "@f" : "", f + 1);
                browser->add(buf);
            }
//            browser->enable_menu(false);
//            browser->enable_pagemove(false);
            browser->callback(Callback);
        }

        {
            input1 = new InputMenu(0, 0, 0, 0, "enter new string or select from menu then press enter in input to update history");
            input1->insert("History a", 10);
            input1->insert("History b", 10);
            input1->insert("History 1", 10);
            input1->insert("History 2", 10);
            input1->insert("History 3", 10);
            input1->insert("History 4", 10);
            input1->insert("History 5", 10);
            input1->insert("History 6", 10);
            input1->insert("History 7", 10);
            input1->insert("History 8", 10);
            input1->insert("History 9", 10);
            input1->insert("History 10 &_/\\", 10);
            input1->value("");
            input1->align(FL_ALIGN_LEFT | FL_ALIGN_TOP);
            input1->callback(Test::Callback);
            input1->take_focus();
        }

        {
            input2 = new InputMenu(0, 0, 0, 0, "InputMenu::set() version - first string in list is first string in history");
            std::vector<std::string> list;
            list.push_back("History 1");
            list.push_back("History 2");
            list.push_back("History 3");
            list.push_back("History 4&_/\\");
            input2->callback(Test::Callback);
            input2->align(FL_ALIGN_LEFT | FL_ALIGN_TOP);
            input2->set(list);
            input2->update_pref(flw::PREF_FIXED_FONT, flw::PREF_FIXED_FONTSIZE);
        }

        {
            hideb = new Fl_Button(0, 0, 0, 0, "Toggle Menu Buttons");
            hideb->callback(Test::Callback);
        }

        callback(Callback);
        resizable(this);
        size_range(64, 48);
    }

    static void Callback(Fl_Widget* sender, void*) {
        char buf[100];
        *buf = 0;

        if (sender == TEST) {
            if (Fl::event() == FL_SHORTCUT && Fl::event_key() == FL_Escape) {
                return;
            }
            else {
                TEST->hide();
            }
        }
        else if (sender == TEST->browser) {
            snprintf(buf, 100, "row=%d", TEST->browser->value());
        }
        else if (sender == TEST->input1) {
            snprintf(buf, 100, "inserted %s", TEST->input1->value());
            TEST->input1->insert(TEST->input1->value(), 10);
            TEST->redraw();
        }
        else if (sender == TEST->input2) {
            snprintf(buf, 100, "inserted %s", TEST->input2->value());
            TEST->input2->insert(TEST->input2->value(), 4);
            TEST->redraw();
        }
        else if (sender == TEST->hideb) {
            TEST->toggle_button(TEST->input1);
            TEST->toggle_button(TEST->input2);
            TEST->size(TEST->w(), TEST->h());
            snprintf(buf, 100, "toggled InputMenu's menu button");
        }

        TEST->box->copy_label(buf);
    }

    void toggle_button(InputMenu* w) {
        if (w->menu()->visible()) {
            w->menu()->hide();
        }
        else {
            w->menu()->show();
        }
    }

    void resize(int X, int Y, int W, int H) override {
        Fl_Double_Window::resize(X, Y, W, H);
        box->resize(4, 4, W - 8, flw::PREF_FONTSIZE * 2 - 8);
        browser->resize(4, flw::PREF_FONTSIZE * 2, 200, H - flw::PREF_FONTSIZE * 2 - 4);
        input1->resize(208, flw::PREF_FONTSIZE * 3 + 8, W - 212, flw::PREF_FONTSIZE * 2);
        input2->resize(208, flw::PREF_FONTSIZE * 6 + 12, W - 212, flw::PREF_FONTSIZE * 2);
        hideb->resize(208, flw::PREF_FONTSIZE * 8 + 16, W - 212, flw::PREF_FONTSIZE * 2);
    }

    ScrollBrowser* browser;
    InputMenu*     input1;
    InputMenu*     input2;
    Fl_Button*     hideb;
    Fl_Box*        box;
    static Test*   TEST;
};

Test* Test::TEST = nullptr;

int main(int argc, const char** argv) {
    if (flw::theme::parse(argc, argv) == false) {
        flw::theme::load("gtk");
    }

    Test win(800, 480);
    win.show();
    return Fl::run();
}
