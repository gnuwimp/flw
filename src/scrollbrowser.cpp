// Copyright gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#include "scrollbrowser.h"

// MKALGAM_ON

namespace flw {
    static const std::string _SCROLLBROWSER_MENU_ALL  = "Copy All Lines";
    static const std::string _SCROLLBROWSER_MENU_LINE = "Copy Current Line";
    static const std::string _SCROLLBROWSER_TOOLTIP   = "Right click to show the menu";
}

//------------------------------------------------------------------------------
flw::ScrollBrowser::ScrollBrowser(int scroll, int X, int Y, int W, int H, const char* l) : Fl_Hold_Browser(X, Y, W, H, l) {
    end();

    _menu      = new Fl_Menu_Button(0, 0, 0, 0);
    _scroll    = (scroll > 0) ? scroll : 9;
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
    auto label = std::string((menu != nullptr) ? menu : "");
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
