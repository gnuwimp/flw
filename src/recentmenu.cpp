// Copyright gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#include "recentmenu.h"

// MKALGAM_ON

namespace flw {
    //--------------------------------------------------------------------------
    RecentMenu::RecentMenu(Fl_Menu_* menu, Fl_Callback* file_callback, void* userdata, std::string base_label, std::string clear_label) {
        _menu     = menu;
        _callback = file_callback;
        _base     = base_label;
        _clear    = clear_label;
        _user     = userdata;
        _max      = 10;

        _menu->add((_base + _clear).c_str(), 0, RecentMenu::CallbackClear, this, FL_MENU_DIVIDER);
    }

    //--------------------------------------------------------------------------
    void RecentMenu::_add(std::string file, bool append) {
        if (file == "") {
            _files.clear();
        }
        else if (append == true) {
            util::add_string(_files, _max, file);
        }
        else {
            util::insert_string(_files, _max, file);
        }

        auto index = _menu->find_index(_base.c_str());

        if (index >= 0) {
            _menu->clear_submenu(index);
            _menu->add((_base + _clear).c_str(), 0, RecentMenu::CallbackClear, this, FL_MENU_DIVIDER);

        }

        for (const auto& file : _files) {
            _menu->add((_base + "/" + flw::util::fix_menu_string(file)).c_str(), 0, _callback, _user);
        }
    }

    //--------------------------------------------------------------------------
    void RecentMenu::CallbackClear(Fl_Widget*, void* o) {
        auto self = (RecentMenu*) o;
        self->_add("", false);
    }

    //--------------------------------------------------------------------------
    void RecentMenu::load_pref(Fl_Preferences& pref, std::string base_name) {
        auto index = 1;
        char buffer[1000];

        while (index <= 100) {
            auto key = flw::util::format("%s%d", base_name.c_str(), index++);

            pref.get(key.c_str(), buffer, "", 1000);

            if (*buffer == 0) {
                break;
            }

            _add(buffer, true);
        }
    }

    //--------------------------------------------------------------------------
    void RecentMenu::save_pref(Fl_Preferences& pref, std::string base_name) {
        auto index = 1;

        for (auto& s : _files) {
            pref.set(flw::util::format("%s%d", base_name.c_str(), index++).c_str(), s.c_str());
        }

        pref.set(flw::util::format("%s%d", base_name.c_str(), index++).c_str(), "");
    }
}

// MKALGAM_OFF