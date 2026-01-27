/**
* @file
* @brief An input widget with an attached menu button to select input string from.
*
* @author gnuwimp@gmail.com
* @copyright Released under the GNU General Public License v3.0
*/

#ifndef FLW_INPUTMENU_H
#define FLW_INPUTMENU_H

#include "flw.h"

// MKALGAM_ON

#include <FL/Fl_Group.H>
#include <FL/Fl_Menu_Button.H>
#include <FL/Fl_Input.H>

namespace flw {

/*
 *      _____                   _   __  __
 *     |_   _|                 | | |  \/  |
 *       | |  _ __  _ __  _   _| |_| \  / | ___ _ __  _   _
 *       | | | '_ \| '_ \| | | | __| |\/| |/ _ \ '_ \| | | |
 *      _| |_| | | | |_) | |_| | |_| |  | |  __/ | | | |_| |
 *     |_____|_| |_| .__/ \__,_|\__|_|  |_|\___|_| |_|\__,_|
 *                 | |
 *                 |_|
 */

namespace priv {
class _InputMenu;
}

/** @brief Text input widget with a menu button with a history of values.
*
* Use up/down arrow keys in the input widget to switch between current list of strings.\n
* Use ctrl + space to popup menu.\n
* Set a callback function to receive message when string input has changed.\n
* No string is automatically added, that is up to caller.\n
*
* @snippet inputmenu.cpp flw::InputMenu example
* @image html inputmenu.png
*/
class InputMenu : public Fl_Group {
public:
                                InputMenu(int X = 0, int Y = 0, int W = 0, int H = 0, const char* l = nullptr);
    void                        clear();
    Fl_Input*                   input() ///< @brief Get input widget.
                                    { return reinterpret_cast<Fl_Input*>(_input); }
    void                        insert(const std::string& string, unsigned max_list_len);
    Fl_Menu_Button*             menu()
                                    { return _menu; } ///< @brief Get menu widget.
    bool                        menu_visible() const
                                    { return _menu->visible() != 0; } ///< @brief Is menu button visible?
    void                        menu_visible(bool value)
                                    { if (value == true) _menu->show(); else _menu->hide(); resize(x(), y(), w(), h()); } ///< @brief Hide or show menu button.
    StringVector                get_history() const;
    void                        resize(int X, int Y, int W, int H) override;
    void                        update_pref(Fl_Font text_font = flw::PREF_FONT, Fl_Fontsize text_size = flw::PREF_FONTSIZE);
    std::string                 value() const;
    void                        value(const std::string& string);
    void                        values(const StringVector& list, const std::string& input_value);
    void                        values(const StringVector& list, size_t list_index = -1);

private:
    void                        _add(bool insert, const std::string& string, int max_list_len);
    void                        _add(bool insert, const StringVector& list);

    void                        _values(const StringVector& list, const std::string& value);

    static void                 _CallbackInput(Fl_Widget*, void*);
    static void                 _CallbackMenu(Fl_Widget*, void*);

    priv::_InputMenu*           _input; // Input widget.
    Fl_Menu_Button*             _menu;  // Menu button with a list of values to select from.
};

} // flw

// MKALGAM_OFF

#endif
