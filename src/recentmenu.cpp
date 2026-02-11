/**
* @file
* @brief A class to help managing recent items in a menu.
*
* @author gnuwimp@gmail.com
* @copyright Released under the GNU General Public License v3.0
*/

#include "recentmenu.h"

// MKALGAM_ON

/*
 *      _____                     _   __  __
 *     |  __ \                   | | |  \/  |
 *     | |__) |___  ___ ___ _ __ | |_| \  / | ___ _ __  _   _
 *     |  _  // _ \/ __/ _ \ '_ \| __| |\/| |/ _ \ '_ \| | | |
 *     | | \ \  __/ (_|  __/ | | | |_| |  | |  __/ | | | |_| |
 *     |_|  \_\___|\___\___|_| |_|\__|_|  |_|\___|_| |_|\__,_|
 *
 *
 */

/** @brief Create new recent menu item.
*
* RecentMenu creates a sub menu that manages some child items.\n
* Like recently opened files.\n
*\n
* Set max number of items with RecentMenu::max_items().\n
* Oldest item will be removed if threshold has been reached.\n
*
* @param[in] menu         Menu object.
* @param[in] callback     Callback function.
* @param[in] userdata     Optional userdata.
* @param[in] base_label   Base label, default "&File/Open recent".
* @param[in] clear_label  Clear label, default "/Clear" which ends up in "&File/Open recent/Clear".
*/
flw::util::RecentMenu::RecentMenu(Fl_Menu_* menu, Fl_Callback* callback, void* userdata, const std::string& base_label, const std::string& clear_label) {
    _menu     = menu;
    _callback = callback;
    _base     = base_label;
    _clear    = clear_label;
    _user     = userdata;
    _max      = 10;

    _menu->add((_base + _clear).c_str(), 0, RecentMenu::_Callback, this, FL_MENU_DIVIDER);
}

/** @brief Add item.
*
* @param[in] item    String to add, if empty all items will be deleted.
* @param[in] append  True to append, false to insert.
*/
void flw::util::RecentMenu::_add(const std::string& item, bool append) {
    if (item == "") {
        _items.clear();
    }
    else if (append == true) {
        _add_string(_items, _max, item);
    }
    else {
        _insert_string(_items, _max, item);
    }

    auto index = _menu->find_index(_base.c_str());

    if (index >= 0) {
        _menu->clear_submenu(index);
        _menu->add((_base + _clear).c_str(), 0, RecentMenu::_Callback, this, FL_MENU_DIVIDER);

    }

    for (const auto& f : _items) {
        _menu->add((_base + "/" + flw::util::fix_menu_string(f)).c_str(), 0, _callback, _user);
    }
}

/** @brief Append item.
*
* @param[in] items     Vector with menu labels.
* @param[in] max_size  Max items.
* @param[in] string    Label string.
*
* @return Number of items.
*/
size_t flw::util::RecentMenu::_add_string(StringVector& items, size_t max_size, const std::string& string) {
    for (auto it = items.begin(); it != items.end(); ++it) {
        if (*it == string) {
            items.erase(it);
            break;
        }
    }

    items.push_back(string);

    while (items.size() > max_size) {
        items.erase(items.begin());
    }

    return items.size();
}

/** @brief Delete all child items.
*
* @param[in] o  This object.
*/
void flw::util::RecentMenu::_Callback(Fl_Widget*, void* o) {
    static_cast<RecentMenu*>(o)->_add("", false);
}

/** @brief Insert item
*
* @param[in] items     Vector with menu labels.
* @param[in] max_size  Max items.
* @param[in] string    Label string.
*
* @return Number of items.
*/
size_t flw::util::RecentMenu::_insert_string(StringVector& items, size_t max_size, const std::string& string) {
    for (auto it = items.begin(); it != items.end(); ++it) {
        if (*it == string) {
            items.erase(it);
            break;
        }
    }

    items.insert(items.begin(), string);

    while (items.size() > max_size) {
        items.pop_back();
    }

    return items.size();
}

/** @brief Add menu list.
*
* @param[in] names  Vector with menu strings.
*/
void flw::util::RecentMenu::items(const StringVector& names) {
    auto res = StringVector();

    for (const auto& name : names) {
        _add(name, true);
    }
}

/** @brief Load items from a preference file.
*
* The key will be set to "base_name/index".\n
* Max 100 items can be loaded.\n
*
* @param[in] pref       Application preference.
* @param[in] base_name  Base name in preference.
*/
void flw::util::RecentMenu::load_pref(Fl_Preferences& pref, const std::string& base_name) {
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

/** @brief Save items to a preference file.
*
* The key will be set to "base_name/index".\n
* Max 100 items can be saved.\n
*
* @param[in] pref       Application preference.
* @param[in] base_name  Base name in preference.
*/
void flw::util::RecentMenu::save_pref(Fl_Preferences& pref, const std::string& base_name) {
    auto index = 1;

    for (const auto& s : _items) {
        pref.set(flw::util::format("%s%d", base_name.c_str(), index++).c_str(), s.c_str());
    }

    pref.set(flw::util::format("%s%d", base_name.c_str(), index++).c_str(), "");
}

// MKALGAM_OFF