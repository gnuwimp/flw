/**
* @file
* @brief A class to help managing recent items in a menu.
*
* @author gnuwimp@gmail.com
* @copyright Released under the GNU General Public License v3.0
*/

#ifndef FLW_RECENTMENU_H
#define FLW_RECENTMENU_H

#include "flw.h"

// MKALGAM_ON

#include <FL/Fl_Menu_.H>
#include <FL/Fl_Preferences.H>

namespace flw {


/***
 *      _____                     _   __  __
 *     |  __ \                   | | |  \/  |
 *     | |__) |___  ___ ___ _ __ | |_| \  / | ___ _ __  _   _
 *     |  _  // _ \/ __/ _ \ '_ \| __| |\/| |/ _ \ '_ \| | | |
 *     | | \ \  __/ (_|  __/ | | | |_| |  | |  __/ | | | |_| |
 *     |_|  \_\___|\___\___|_| |_|\__|_|  |_|\___|_| |_|\__,_|
 *
 *
 */

/** @brief A class to help managing recent used items in a menu.
*
* Strings can be saved with RecentMenu::save_pref() and loaded with RecentMenu::load_pref().\n
* By default max 10 items can be used.\n
* After that oldest item will be removed from list before adding or inserting a new item.\n
* Use RecentMenu::max_items() to change that.\n
*
* @snippet recentmenu.cpp flw::RecentMenu example
* @image html recentmenu.png
*/
class RecentMenu {
public:
                                    RecentMenu(Fl_Menu_* menu, Fl_Callback* callback, void* userdata = nullptr, const std::string& base_label = "&File/Open recent", const std::string& clear_label = "/Clear");
    void                            append(const std::string& item)
                                        { return _add(item, true); } ///< @brief Append item last in menu. @param[in] item  Menu label.
    void                            insert(const std::string& item)
                                        { return _add(item, false); } ///< @brief Insert item first in menu. @param[in] item  Menu label.
    StringVector                    items() const
                                        { return _items; } ///< @brief Number of items.
    size_t                          max_items() const
                                        { return _max; } ///< @brief Get max number of items.
    void                            max_items(size_t max)
                                        { if (max > 0 && max <= 100) _max = max; } ///< @brief Set max number of items. @param[in] max  From 1 to 100.
    Fl_Menu_*                       menu()
                                        { return _menu; } ///< @brief Get menu object.
    void                            load_pref(Fl_Preferences& pref, const std::string& base_name = "files");
    void                            save_pref(Fl_Preferences& pref, const std::string& base_name = "files");
    void*                           user_data()
                                        { return _user; } ///< @brief Get user data.
    void                            user_data(void* data)
                                        { _user = data; } ///< @brief Set user data.

    static void                     CallbackClear(Fl_Widget*, void* o);

private:
    void                            _add(const std::string& item, bool append);
    size_t                          _add_string(StringVector& items, size_t max_size, const std::string& string);
    size_t                          _insert_string(StringVector& items, size_t max_size, const std::string& string);

    std::string                     _base;
    Fl_Callback*                    _callback;
    std::string                     _clear;
    StringVector                    _items;
    size_t                          _max;
    Fl_Menu_*                       _menu;
    void*                           _user;
};
}

// MKALGAM_OFF

#endif
