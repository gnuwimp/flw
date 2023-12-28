// Copyright gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#include "inputmenu.h"
#include <FL/Fl_Input.H>
#include <FL/Fl_Menu_Button.H>

// MKALGAM_ON

namespace flw {
    static const std::string _INPUTMENU_TOOLTIP = "Use up/down arrows to switch between previous values\nPress ctrl + space to open menu button (if visible)";

    //--------------------------------------------------------------------------
    class _InputMenu : public Fl_Input {
    public:

        bool            show_menu;
        int             index;
        StringVector    history;

        //----------------------------------------------------------------------
        _InputMenu() : Fl_Input(0, 0, 0, 0) {
            tooltip(_INPUTMENU_TOOLTIP.c_str());

            index     = -1;
            show_menu = false;
        }

        //----------------------------------------------------------------------
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
                else if (key == FL_Down && history.size() > 0 && index < (int) history.size() - 1) {
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
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
flw::InputMenu::InputMenu(int X, int Y, int W, int H, const char* l) : Fl_Group(X, Y, W, H, l) {
    end();

    _input = new flw::_InputMenu();
    _menu  = new Fl_Menu_Button(0, 0, 0, 0);

    Fl_Group::add(_input);
    Fl_Group::add(_menu);

    _input->callback(InputMenu::Callback, this);
    _input->when(FL_WHEN_ENTER_KEY_ALWAYS);
    _menu->callback(InputMenu::Callback, this);
    update_pref();
}

//------------------------------------------------------------------------------
void flw::InputMenu::Callback(Fl_Widget* sender, void* self) {
    auto w = (InputMenu*) self;

    if (sender == w->_input) {
        if (w->_input->show_menu) {
            if (w->_menu->visible()) {
                w->_menu->popup();
            }
        }
        else {
            w->do_callback();
        }
    }
    else if (sender == w->_menu) {
        auto index = w->_menu->find_index(w->_menu->text());

        if (index >= 0 && index < (int) w->_input->history.size()) {
            w->_input->value(w->_input->history[index].c_str());
            w->_input->index = index;
        }

        w->_input->take_focus();
    }
}

//------------------------------------------------------------------------------
void flw::InputMenu::clear() {
    _menu->clear();
    _input->history.clear();
    _input->index = -1;
}

//------------------------------------------------------------------------------
flw::StringVector flw::InputMenu::get_history() const {
    return _input->history;
}

//------------------------------------------------------------------------------
void flw::InputMenu::insert(const std::string& string, int max_list_len) {
    for (auto it = _input->history.begin(); it != _input->history.end(); ++it) {
        if (*it == string) {
            _input->history.erase(it);
            break;
        }
    }

    _input->history.insert(_input->history.begin(), string);

    while ((int) _input->history.size() > max_list_len) {
        _input->history.pop_back();
    }

    _menu->clear();

    for (auto& s : _input->history) {
        _menu->add(flw::util::fix_menu_string(s.c_str()).c_str());
    }

    _input->index = -1;
    _input->value(string.c_str());
    _input->position(string.length(), 0);
}

//------------------------------------------------------------------------------
void flw::InputMenu::resize(int X, int Y, int W, int H) {
    Fl_Group::resize(X, Y, W, H);

    if (_menu->visible()) {
        _input->resize(X, Y, W - flw::PREF_FONTSIZE * 2, H);
        _menu->resize(X + W - flw::PREF_FONTSIZE * 2, Y, flw::PREF_FONTSIZE * 2, H);
    }
    else {
        _input->resize(X, Y, W, H);
    }
}

//------------------------------------------------------------------------------
void flw::InputMenu::set(const StringVector& list, bool copy_first_to_input) {
    clear();
    _input->history = list;

    for (auto& s : _input->history) {
        _menu->add(flw::util::fix_menu_string(s.c_str()).c_str());
    }

    if (list.size() && copy_first_to_input) {
        auto s = list.front();
        _input->value(s.c_str());
        _input->position(s.length(), 0);
    }
}

//------------------------------------------------------------------------------
void flw::InputMenu::update_pref(Fl_Font text_font, Fl_Fontsize text_size) {
    labelfont(flw::PREF_FONT);
    labelsize(flw::PREF_FONTSIZE);
    _input->textfont(text_font);
    _input->textsize(text_size);
    _menu->labelfont(text_font);
    _menu->labelsize(text_size);
    _menu->textfont(text_font);
    _menu->textsize(text_size);
}

//------------------------------------------------------------------------------
const char* flw::InputMenu::value() const {
    return _input->value();
}

//------------------------------------------------------------------------------
void flw::InputMenu::value(const char* string) {
    _input->value(string ? string : "");
}

// MKALGAM_OFF
