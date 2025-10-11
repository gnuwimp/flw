// Copyright gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#include "flw.h"

int main() {
    Fl::scheme("oxy");
    Fl::screen_scale(0, 1.0);

    // [flw::TabsGroup example]

    auto* win  = new Fl_Window(Fl::w() / 2 - 320, Fl::h() / 2 - 240, 640, 480, "flw::TabsGroup");
    auto* tabs = new flw::TabsGroup(0, 0, 640, 480);

    // Add child widgets.
    tabs->add("Hello", new Fl_Button(0, 0, 0, 0, "Hello World 1"));
    tabs->add("XXXXX", new Fl_Button(0, 0, 0, 0, "Hello World 2"));

    // Add many tabs.
    for (int f = 1; f <= 40; f++) {
        tabs->add(flw::util::format("Tab %02d", f), new Fl_Button(0, 0, 0, 0, "Hello World"));
    }

    // Move tabs from default top side to left side.
    tabs->tab_pos(flw::TabsGroup::Pos::LEFT);

    // Change tab button label from "XXXXX" to "World".
    tabs->tab_label("World", tabs->child(1));

    // Add widget between the two first widgets.
    tabs->insert("Inserted", new Fl_Button(0, 0, 0, 0, "Hello World 3"), tabs->child(1));

    // Swap positions of "Hello" and "World".
    tabs->swap(0, 2);
    tabs->do_layout();
    tabs->value(tabs->child(0));

    win->resizable(tabs);
    win->show();
    Fl::run();

    // [flw::TabsGroup example]
}
