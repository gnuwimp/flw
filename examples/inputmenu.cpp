// Copyright gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#include "flw.h"
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_File_Chooser.H>

int main() {
    Fl::scheme("oxy");
    Fl::screen_scale(0, 1.0);

    // [flw::InputMenu example]

    auto win    = new Fl_Window(Fl::w() / 2 - 320, Fl::h() / 2 - 120, 640, 140, "flw::InputMenu");
    auto input1 = new flw::InputMenu(4, 4, 632, 28);
    auto input2 = new flw::InputMenu(4, 46, 632, 28);
    auto input3 = new flw::InputMenu(4, 88, 632, 28);
    auto list   = std::vector<std::string>{"Item1", "Item2", "Item3", "Item4", "Item5"};

    // Set menu values.
    input1->values(list);

    // Insert new strings when enter is pressed.
    input1->callback([](Fl_Widget* w, void*) {
        auto i = (flw::InputMenu*) w;
        i->insert(i->value(), 5);
    });

    // Set menu values and input value to "Item5".
    input2->values(list, 4);

    // Insert new strings when enter is pressed.
    input2->callback([](Fl_Widget* w, void*) {
        auto i = (flw::InputMenu*) w;
        i->insert(i->value(), 5);
    });

    // Set menu values.
    input3->values(list);

    // Now insert a string first and restrict max number of items to 2.
    input3->insert("First", 2);

    // Insert two more strings, now only "Hello" and "World" is in the menu.
    input3->insert("World", 2);
    input3->insert("Hello", 2);

    // Insert new strings when enter is pressed, now set max items to 3.
    input3->callback([](Fl_Widget* w, void*) {
        auto i = (flw::InputMenu*) w;
        i->insert(i->value(), 3);
    });

    win->resizable(win);
    win->show();
    Fl::run();

    // [flw::InputMenu example]
}
