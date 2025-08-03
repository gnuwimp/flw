// Copyright gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#include "flw.h"
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_File_Chooser.H>

#define CALLBACK(X) [](Fl_Widget*, void* o) { X; }

int main() {
    Fl::scheme("oxy");
    Fl::screen_scale(0, 1.0);

    // [flw::RecentMenu example]

    auto win = new Fl_Window(Fl::w() / 2 - 320, Fl::h() / 2 - 240, 640, 480, "flw::RecentMenu");
    auto menu = new Fl_Menu_Bar(0, 0, 640, 24);
    auto recent = new flw::RecentMenu(menu, CALLBACK( // Callback for an item in recent menu.
        puts(static_cast<Fl_Menu_Bar*>(o)->text())
    ), menu);

    menu->add("&File/Add file...", FL_COMMAND + 'o', CALLBACK( // Add file to recent menu.
        auto filename = fl_file_chooser("Select File", "All Files (*)\tExecutable Files (*.exe)", "");

        if (filename != nullptr) {
            static_cast<flw::RecentMenu*>(o)->insert(filename);
        }
    ), recent);

    // After 5 items the oldest will be deleted.
    recent->max_items(5);
    recent->insert("gridgroup.cpp");
    recent->insert("recentmenu.cpp");

    win->resizable(win);
    win->show();
    Fl::run();

    // Save items to preference file, load with flw::RecentMenu::load_pref().
    auto pref = Fl_Preferences(Fl_Preferences::USER, "gnuwimp_test", "example_recentmenu");
    pref.clear();
    recent->save_pref(pref);

    // [flw::RecentMenu example]
}
