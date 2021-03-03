// Copyright 2021 gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#include "widgets.h"

// MALAGMA_ON

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
flw::ScrollBrowser::ScrollBrowser(int scroll, int X, int Y, int W, int H) : Fl_Hold_Browser(X, Y, W, H) {
    end();
    _scroll = scroll > 0 ? scroll : 9;
    _move   = false;

    tooltip("Right click a row or press ctrl + 'c' to copy current line");
}

//------------------------------------------------------------------------------
void flw::ScrollBrowser::copy_line_to_clipboard() const {
    if (value()) {
        auto s = ScrollBrowser::RemoveFormat(text(value()));
        Fl::copy(s.c_str(), s.length(), 2);
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
        auto key = Fl::event_key();

        if (Fl::event_ctrl() && key == 'c') {
            copy_line_to_clipboard();
            return 1;
        }
        else if (_move && key == FL_Page_Up) {
            auto line = value();

            if (line - _scroll < 1) {
                value(1);
            }
            else {
                value(line - _scroll);
                topline(line - _scroll);
            }
            return 1;
        }
        else if (_move && key == FL_Page_Down) {
            auto line = value();

            if (line + _scroll > size()) {
                value(size());
            }
            else {
                value(line + _scroll);
                bottomline(line + _scroll);
            }

            return 1;
        }
    }
    else if (event == FL_PUSH) {
        if (Fl::event_button() == FL_RIGHT_MOUSE) {
            copy_line_to_clipboard();
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

// MALAGMA_OFF
