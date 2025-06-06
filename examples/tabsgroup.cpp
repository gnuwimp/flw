// Copyright gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#include "flw.h"

int main() {
    Fl::scheme("oxy");
    Fl::screen_scale(0, 1.0);

    // [flw::TabsGroup TabsGroup example]

    auto win = new Fl_Window(Fl::w() / 2 - 320, Fl::h() / 2 - 240, 640, 480, "flw::TabsGroup");
    auto tabs = new flw::TabsGroup(0, 0, 640, 480);

    // Add child widgets.
    tabs->add("Hello", new Fl_Button(0, 0, 0, 0, "Hello World 1"));
    tabs->add("XXXXX", new Fl_Button(0, 0, 0, 0, "Hello World 2"));

    // Move tabs from top side to left side.
    tabs->tabs(flw::TabsGroup::TABS::WEST);

    // Change tab button label from "XXXXX" to "World".
    auto x = tabs->child(1);
    tabs->label("World", x);

    // Add widget between the two first widgets.
    tabs->insert("Inserted", new Fl_Button(0, 0, 0, 0, "Hello World 3"), x);

    // Swap positions of "Hello" and "World".
    tabs->swap(0, 2);

    // Change minimum width of tab buttons (10 * flw::PREF_FONTSIZE).
    flw::TabsGroup::MIN_WIDTH_EAST_WEST = 10;

    tabs->do_layout();
    win->resizable(tabs);
    win->show();
    Fl::run();

    // [flw::TabsGroup TabsGroup example]
}
