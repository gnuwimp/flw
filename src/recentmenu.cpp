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
            _add_string(_files, _max, file);
        }
        else {
            _insert_string(_files, _max, file);
        }

        auto index = _menu->find_index(_base.c_str());

        if (index >= 0) {
            _menu->clear_submenu(index);
            _menu->add((_base + _clear).c_str(), 0, RecentMenu::CallbackClear, this, FL_MENU_DIVIDER);

        }

        for (const auto& f : _files) {
            _menu->add((_base + "/" + flw::util::fix_menu_string(f)).c_str(), 0, _callback, _user);
        }
    }

    //------------------------------------------------------------------------------
    size_t RecentMenu::_add_string(StringVector& in, size_t max_size, std::string string) {
        for (auto it = in.begin(); it != in.end(); ++it) {
            if (*it == string) {
                in.erase(it);
                break;
            }
        }

        in.push_back(string);

        while (in.size() > max_size) in.erase(in.begin());
        return in.size();
    }

    //--------------------------------------------------------------------------
    void RecentMenu::CallbackClear(Fl_Widget*, void* o) {
        auto self = static_cast<RecentMenu*>(o);
        self->_add("", false);
    }

    //------------------------------------------------------------------------------
    size_t RecentMenu::_insert_string(StringVector& in, size_t max_size, std::string string) {
        for (auto it = in.begin(); it != in.end(); ++it) {
            if (*it == string) {
                in.erase(it);
                break;
            }
        }

        in.insert(in.begin(), string);
        while (in.size() > max_size) in.pop_back();
        return (int) in.size();
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

        for (const auto& s : _files) {
            pref.set(flw::util::format("%s%d", base_name.c_str(), index++).c_str(), s.c_str());
        }

        pref.set(flw::util::format("%s%d", base_name.c_str(), index++).c_str(), "");
    }
}

// MKALGAM_OFF