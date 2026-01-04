/**
* @file
* @brief An input widget with an attached menu button to select input string from.
*
* @author gnuwimp@gmail.com
* @copyright Released under the GNU General Public License v3.0
*/

#include "inputmenu.h"

// MKALGAM_ON

#include <algorithm>

namespace flw {

static constexpr const char* _INPUTMENU_TOOLTIP = "Use up/down arrows to switch between previous values.\nPress ctrl + space to open menu button (if visible).";

/*
 *           _____                   _   __  __
 *          |_   _|                 | | |  \/  |
 *            | |  _ __  _ __  _   _| |_| \  / | ___ _ __  _   _
 *            | | | '_ \| '_ \| | | | __| |\/| |/ _ \ '_ \| | | |
 *           _| |_| | | | |_) | |_| | |_| |  | |  __/ | | | |_| |
 *          |_____|_| |_| .__/ \__,_|\__|_|  |_|\___|_| |_|\__,_|
 *      ______          | |
 *     |______|         |_|
 */


/** @brief Input widget with extended keyboard handling.
* @private.
*/
class _InputMenu : public Fl_Input {
public:
    bool            show_menu;  // If true the open menu button.
    int             index;      // Index in string vextor.
    StringVector    history;    // String list.

    /** @brief Create widget.
    *
    */
    _InputMenu() : Fl_Input(0, 0, 0, 0) {
        index     = -1;
        show_menu = false;
    }

    /** @brief Handle events.
    *
    * Open menu button with ctrl + space.\n
    * Or use up/down arrows to shift through available items.\n
    */
    int handle(int event) override {
        if (event == FL_KEYBOARD) {
            auto key = Fl::event_key();

            if (Fl::event_ctrl() != 0 && key == ' ') {
                if (history.size() > 0) {
                    show_menu = true;
                    do_callback();
                }

                return 1;
            }
            else if (key == FL_Up && history.size() > 0) {
                if (index <= 0) {
                    value("");
                    index = -1;
                }
                else {
                    index--;
                    show_menu = false;
                    value(history[index].c_str());
                }

                return 1;
            }
            else if (key == FL_Down && history.size() > 0 && index < static_cast<int>(history.size()) - 1) {
                index++;

                value(history[index].c_str());
                show_menu = false;
                return 1;
            }
            else {
                show_menu = false;
            }
        }

        return Fl_Input::handle(event);
    }
};

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

/** @brief Create new InputMenu widget.
*
* Menu button is empty.\n
* Text field is empty.\n
*
* @param[in] X  X pos.
* @param[in] Y  Y pos.
* @param[in] W  Width.
* @param[in] H  Height.
* @param[in] l  Optional label.
*/
InputMenu::InputMenu(int X, int Y, int W, int H, const char* l) : Fl_Group(X, Y, W, H, l) {
    end();

    _input = new flw::_InputMenu();
    _menu  = new Fl_Menu_Button(0, 0, 0, 0);

    Fl_Group::add(_input);
    Fl_Group::add(_menu);

    _input->callback(InputMenu::_CallbackInput, this);
    _input->when(FL_WHEN_ENTER_KEY_ALWAYS);
    _menu->callback(InputMenu::_CallbackMenu, this);
    _menu->tooltip(_INPUTMENU_TOOLTIP);
    tooltip(_INPUTMENU_TOOLTIP);
    update_pref();
    resize(X, Y, W, H);
}

/** @brief Popup menu buttom or do an callback.
*
*/
void InputMenu::_CallbackInput(Fl_Widget*, void* o) {
    auto self = static_cast<InputMenu*>(o);

    if (self->_input->show_menu == true) {
        if (self->_menu->visible() != 0) {
            self->_menu->popup();
        }
    }
    else {
        self->do_callback();
    }
}

/** @brief Menu has been selected so copy value from menu to input.
*
*/
void InputMenu::_CallbackMenu(Fl_Widget*, void* o) {
    auto self  = static_cast<InputMenu*>(o);
    auto index = self->_menu->find_index(self->_menu->text());

    if (index >= 0 && index < static_cast<int>(self->_input->history.size())) {
        self->_input->value(self->_input->history[index].c_str());
        self->_input->index = index;
    }

    self->_input->take_focus();
}

/** @brief Clear all data.
*
*/
void InputMenu::clear() {
    _menu->clear();
    _input->history.clear();
    _input->index = -1;
}

/** @brief Get current list of string.
*
* @return String vector.
*/
flw::StringVector InputMenu::get_history() const {
    return _input->history;
}

/** @brief Insert string into menu.
*
* If list is too large the oldest items are removed.
*
* @param[in] string        String to insert.
* @param[in] max_list_len  Max number of strings in menu.
*/
void InputMenu::insert(const std::string& string, unsigned max_list_len) {
    for (auto it = _input->history.begin(); it != _input->history.end(); ++it) {
        if (*it == string) {
            _input->history.erase(it);
            break;
        }
    }

    _input->history.insert(_input->history.begin(), string);

    while (static_cast<unsigned>(_input->history.size()) > max_list_len) {
        _input->history.pop_back();
    }

    _menu->clear();

    for (const auto& s : _input->history) {
        _menu->add(flw::util::fix_menu_string(s.c_str()).c_str());
    }

    _input->index = -1;
    _input->value(string.c_str());
    _input->insert_position(string.length(), 0);
}

/** @brief Resize widget
*
* @param[in] X  X pos.
* @param[in] Y  Y pos.
* @param[in] W  Width.
* @param[in] H  Height.
*/
void InputMenu::resize(int X, int Y, int W, int H) {
    Fl_Widget::resize(X, Y, W, H);

    if (_menu->visible() != 0) {
        auto mw = flw::PREF_FONTSIZE;
        _input->resize(X, Y, W - flw::PREF_FONTSIZE - mw, H);
        _menu->resize(X + W - flw::PREF_FONTSIZE - mw, Y, flw::PREF_FONTSIZE + mw, H);
    }
    else {
        _input->resize(X, Y, W, H);
    }
}

/** @brief Update font properties of input and menu widgets.
*
* Label font are using values from flw::PREF_FONT.
*
* @param[in] text_font  New font for text input and menu.
* @param[in] text_size  New font size text input and menu.
*/
void InputMenu::update_pref(Fl_Font text_font, Fl_Fontsize text_size) {
    labelfont(flw::PREF_FONT);
    labelsize(flw::PREF_FONTSIZE);
    _input->textfont(text_font);
    _input->textsize(text_size);
    _menu->textfont(text_font);
    _menu->textsize(text_size);
}

/** @brief Get input value.
*
* @return Input string.
*/
std::string InputMenu::value() const {
    return flw::util::to_string(_input->value());
}

/** @brief Set input value.
*
* @param[in] string  String value.
*/
void InputMenu::value(const std::string& string) {
    _input->value(string.c_str());
}

/** @brief Set a list of string and input value.
*
* @param[in] list         List of strings.
* @param[in] input_value  Input string.
*/
void InputMenu::values(const StringVector& list, const std::string& input_value) {
    _values(list, input_value);
}

/** @brief Set a list of string and an optional input value.
*
* @param[in] list        List of strings.
* @param[in] list_index  Optional index in list to set input string, -1 to set input to empty string.
*/
void InputMenu::values(const StringVector& list, size_t list_index) { 
    _values(list, list.size() > list_index ? list[list_index] : "");
}

/** @brief Set new menu list and optional input string.
*
* @param[in] menu_list    List of strings for the menu.
* @param[in] input_value  Text input value.
*/
void InputMenu::_values(const StringVector& menu_list, const std::string& input_value) {
    clear();
    _input->history = menu_list;

    for (const auto& s : _input->history) {
        _menu->add(flw::util::fix_menu_string(s.c_str()).c_str());
    }

    _input->value(input_value.c_str());
    _input->insert_position(input_value.length(), 0);
}

} // flw

// MKALGAM_OFF
