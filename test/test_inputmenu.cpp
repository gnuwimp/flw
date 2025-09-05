// Copyright gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#include "flw.h"

#ifndef FLW_AMALGAM
    #include "inputmenu.h"
    #include "gridgroup.h"
#endif

#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Menu_Button.H>
#include <FL/platform.H>

using namespace flw;

class Test : public Fl_Double_Window {
public:
    Test(int W, int H) : Fl_Double_Window(W, H, "test_inputmenu.cpp") {
        TEST = this;

        input1 = new InputMenu(0, 0, 0, 0, "Strings was added by InputMenu::insert(), last string is first in menu.");
        input2 = new InputMenu(0, 0, 0, 0, "Strings was added by a list of strings to InputMenu::values(), first string is first in menu.");
        grid   = new GridGroup(0, 0, W, H);
        hideb  = new Fl_Button(0, 0, 0, 0, "Toggle Menu Buttons");

        add(grid);
        grid->add(input1,   1,   3,  -1,   4);
        grid->add(input2,   1,  10,  -1,   4);
        grid->add(hideb,    1,  15,  -1,   4);
        grid->do_layout();

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
        //input1->update_pref(FL_COURIER, 18);
        
        std::vector<std::string> list;
        list.push_back("History 1");
        list.push_back("History 2");
        list.push_back("History 3");
        list.push_back("History 4&_/\\");
        list.push_back("History 5");
        list.push_back("History 6");
        list.push_back("History 7");
        list.push_back("History 8");
        list.push_back("History 9");
        list.push_back("History 10");
        input2->callback(Test::Callback);
        input2->align(FL_ALIGN_LEFT | FL_ALIGN_TOP);
        input2->values(list, 0);
        input2->update_pref(flw::PREF_FIXED_FONT, flw::PREF_FIXED_FONTSIZE);

        hideb->callback(Test::Callback);
        
        //tooltip("Add string to the menus by entering text in the input field and press enter.");
        callback(Callback);
        resizable(grid);
        size_range(64, 48);
    }

    static void Callback(Fl_Widget* sender, void*) {
        if (sender == TEST) {
            TEST->hide();
        }
        else if (sender == TEST->input1) {
            TEST->input1->insert(TEST->input1->value(), 10);
            TEST->redraw();
        }
        else if (sender == TEST->input2) {
            TEST->input2->insert(TEST->input2->value(), 10);
            TEST->redraw();
        }
        else if (sender == TEST->hideb) {
            TEST->toggle_button(TEST->input1);
            TEST->toggle_button(TEST->input2);
        }
    }

    void toggle_button(InputMenu* w) {
        w->menu_visible(!w->menu_visible());
    }

    Fl_Button*     hideb;
    GridGroup*     grid;
    InputMenu*     input1;
    InputMenu*     input2;
    static Test*   TEST;
};

Test* Test::TEST = nullptr;

int main(int argc, const char** argv) {
    if (flw::theme::parse(argc, argv) == false) {
        flw::theme::load("oxy");
    }

    fl_open_display();
    Test win(800, 400);
    win.show();
    return Fl::run();
}
