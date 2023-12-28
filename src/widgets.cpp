// Copyright 2021 gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#include "widgets.h"

// MKALGAM_ON

namespace flw {
    static const std::string _INPUTMENU_TOOLTIP       = "Use up/down arrows to switch between previous values\nPress ctrl + space to open menu button (if visible)";
    static const std::string _SCROLLBROWSER_MENU_ALL  = "Copy All Lines";
    static const std::string _SCROLLBROWSER_MENU_LINE = "Copy Current Line";
    static const std::string _SCROLLBROWSER_TOOLTIP   = "Right click to show the menu";

    //--------------------------------------------------------------------------
    class _InputMenu : public Fl_Input {
    public:

        bool                            show_menu;
        int                             index;
        std::vector<std::string>        history;

        //----------------------------------------------------------------------
        _InputMenu() : Fl_Input(0, 0, 0, 0) {
            tooltip(_INPUTMENU_TOOLTIP.c_str());
            show_menu = false;
        }

        //----------------------------------------------------------------------
        int handle(int event) override {
            if (event == FL_KEYBOARD) {
                auto key = Fl::event_key();

                if (Fl::event_ctrl() && key == ' ') {
                    if (history.size() > 0) {
                        show_menu = true;
                        do_callback();
                    }

                    return 1;
                }
                else if (key == FL_Up && history.size()) {
                    if (index <= 0) {
                        value("");
                        index = -1;
                    }
                    else {
                        index--;

                        if (index < 0) {
                            index = 0;
                        }

                        value(history[index].c_str());
                        show_menu = false;
                    }

                    return 1;
                }
                else if (key == FL_Down && history.size() && index < (int) history.size() - 1) {
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
std::vector<std::string> flw::InputMenu::get_history() const {
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
void flw::InputMenu::set(const std::vector<std::string>& list, bool copy_first_to_input) {
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

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
flw::ScrollBrowser::ScrollBrowser(int scroll, int X, int Y, int W, int H, const char* l) : Fl_Hold_Browser(X, Y, W, H, l) {
    end();

    _menu      = new Fl_Menu_Button(0, 0, 0, 0);
    _scroll    = scroll > 0 ? scroll : 9;
    _flag_move = true;
    _flag_menu = true;

    ((Fl_Group*) this)->add(_menu);
    _menu->add(_SCROLLBROWSER_MENU_LINE.c_str(), 0, ScrollBrowser::Callback, this);
    _menu->add(_SCROLLBROWSER_MENU_ALL.c_str(), 0, ScrollBrowser::Callback, this);
    _menu->type(Fl_Menu_Button::POPUP3);
    tooltip(_SCROLLBROWSER_TOOLTIP.c_str());
    update_pref();
}

//------------------------------------------------------------------------------
void flw::ScrollBrowser::Callback(Fl_Widget*, void* o) {
    auto self  = (ScrollBrowser*) o;
    auto menu  = self->_menu->text();
    auto label = std::string(menu != nullptr ? menu : "");
    auto clip  = std::string();

    clip.reserve(self->size() * 40 + 100);

    if (label == _SCROLLBROWSER_MENU_LINE) {
        if (self->value() != 0) {
            clip = ScrollBrowser::RemoveFormat(self->text(self->value()));
        }
    }
    else if (label == _SCROLLBROWSER_MENU_ALL) {
        for (auto f = 1; f <= self->size(); f++) {
            auto s = ScrollBrowser::RemoveFormat(self->text(f));
            clip += s;
            clip += "\n";
        }
    }

    if (clip != "") {
        Fl::copy(clip.c_str(), clip.length(), 2);
    }
}

//------------------------------------------------------------------------------
int flw::ScrollBrowser::handle(int event) {
    if (event == FL_MOUSEWHEEL) {
        if (Fl::event_dy() > 0) {
            topline(topline() + _scroll);
        }
        else if (Fl::event_dy() < 0) {
            topline(topline() - _scroll);
        }

        return 1;
    }
    else if (event == FL_KEYBOARD) {
        if (_flag_move == true) {
            auto key = Fl::event_key();

            if (key == FL_Page_Up) {
                auto line = value();

                if (line == 1) {
                }
                else if (line - _scroll < 1) {
                    value(1);
                    do_callback();
                }
                else {
                    value(line - _scroll);
                    topline(line - _scroll);
                    do_callback();
                }

                return 1;
            }
            else if (key == FL_Page_Down) {
                auto line = value();

                if (line == size()) {
                }
                else if (line + _scroll > size()) {
                    value(size());
                    do_callback();
                }
                else {
                    value(line + _scroll);
                    bottomline(line + _scroll);
                    do_callback();
                }

                return 1;
            }
        }
    }
    else if (event == FL_PUSH) {
        if (_flag_menu == true && Fl::event_button() == FL_RIGHT_MOUSE) {
            _menu->popup();
            return 1;
        }
    }

    return Fl_Hold_Browser::handle(event);
}

//------------------------------------------------------------------------------
std::string flw::ScrollBrowser::RemoveFormat(const char* text) {
    auto res = std::string(text);
    auto f   = res.find_last_of("@");

    if (f != std::string::npos) {
        auto tmp = res.substr(f + 1);

        if (tmp[0] == '.' || tmp[0] == 'l' || tmp[0] == 'm' || tmp[0] == 's' || tmp[0] == 'b' || tmp[0] == 'i' || tmp[0] == 'f' || tmp[0] == 'c' || tmp[0] == 'r' || tmp[0] == 'u' || tmp[0] == '-') {
            res = tmp.substr(1);
        }
        else if (tmp[0] == 'B' || tmp[0] == 'C' || tmp[0] == 'F' || tmp[0] == 'S') {
            auto s = std::string();
            auto e = false;

            tmp = tmp.substr(f + 1);

            for (auto c : tmp) {
                if (e == false && c >= '0' && c <= '9') {
                }
                else {
                    e = true;
                    s += c;
                }
            }

            res = s;
        }
        else {
            res = res.substr(f);
        }
    }

    return res;
}

//------------------------------------------------------------------------------
void flw::ScrollBrowser::update_pref(Fl_Font text_font, Fl_Fontsize text_size) {
    labelfont(flw::PREF_FONT);
    labelsize(flw::PREF_FONTSIZE);
    textfont(text_font);
    textsize(text_size);
}

// MKALGAM_OFF
