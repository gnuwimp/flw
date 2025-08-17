// Copyright gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#include "flw.h"
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_File_Chooser.H>

int main() {
    Fl::scheme("oxy");
    Fl::screen_scale(0, 1.0);

    // [flw::RecentMenu example]

    #define CALLBACK(X) [](Fl_Widget*, void* o) { X; }

    Fl_Preferences   pref   = Fl_Preferences(Fl_Preferences::USER, "gnuwimp_test", "example_recentmenu");
    Fl_Window*       win    = new Fl_Window(Fl::w() / 2 - 320, Fl::h() / 2 - 240, 640, 480, "flw::RecentMenu");
    Fl_Menu_Bar*     menu   = new Fl_Menu_Bar(0, 0, 640, 28);
    flw::RecentMenu* recent = new flw::RecentMenu(menu, CALLBACK( // Callback for an item in recent menu.
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
    
    // Load menu items from preference file.
    recent->load_pref(pref);
    
    // Add two items if nothing are stored in preference file.
    if (recent->items().size() == 0) {
        recent->insert("gridgroup.cpp");
        recent->insert("recentmenu.cpp");
    }

    win->resizable(win);
    win->show();
    Fl::run();

    // Save items to preference file.
    pref.clear();
    recent->save_pref(pref);

    // [flw::RecentMenu example]
}
