/**
* @file
* @brief A slightly extended version of Fl_Hold_Browser.
*
* @author gnuwimp@gmail.com
* @copyright Released under the GNU General Public License v3.0
*/

#include "scrollbrowser.h"

// MKALGAM_ON

namespace flw {
namespace priv {

static const std::string _SCROLLBROWSER_MENU_ALL  = "Copy all Lines";
static const std::string _SCROLLBROWSER_MENU_LINE = "Copy Current Line";
static const std::string _SCROLLBROWSER_TOOLTIP   = "Right click to show the menu.";

} // flw::priv
} // flw

/*
 *       _____                _ _ ____
 *      / ____|              | | |  _ \
 *     | (___   ___ _ __ ___ | | | |_) |_ __ _____      _____  ___ _ __
 *      \___ \ / __| '__/ _ \| | |  _ <| '__/ _ \ \ /\ / / __|/ _ \ '__|
 *      ____) | (__| | | (_) | | | |_) | | | (_) \ V  V /\__ \  __/ |
 *     |_____/ \___|_|  \___/|_|_|____/|_|  \___/ \_/\_/ |___/\___|_|
 *
 *
 */

/** @brief Create widget.
*
* @param[in] lines  Number of lines to scroll with page up/down.
* @param[in] X      X pos.
* @param[in] Y      Y pos.
* @param[in] W      Width.
* @param[in] H      Height.
* @param[in] l      Optional label.
*/
flw::ScrollBrowser::ScrollBrowser(int lines, int X, int Y, int W, int H, const char* l) : Fl_Hold_Browser(X, Y, W, H, l) {
    end();

    _menu      = new Fl_Menu_Button(0, 0, 0, 0);
    _flag_move = true;
    _flag_menu = true;

    scroll_lines(lines);
    static_cast<Fl_Group*>(this)->add(_menu);
    _menu->add(priv::_SCROLLBROWSER_MENU_LINE.c_str(), 0, ScrollBrowser::Callback, this);
    _menu->add(priv::_SCROLLBROWSER_MENU_ALL.c_str(), 0, ScrollBrowser::Callback, this);
    _menu->type(Fl_Menu_Button::POPUP3);
    tooltip(priv::_SCROLLBROWSER_TOOLTIP.c_str());
    update_pref();
}

/** @brief Callback form the menu.
*
*/
void flw::ScrollBrowser::Callback(Fl_Widget*, void* o) {
    auto self  = static_cast<ScrollBrowser*>(o);
    auto txt   = self->_menu->text();
    auto label = std::string((txt != nullptr) ? txt : "");
    auto clip  = std::string();

    clip.reserve(self->size() * 40 + 100);

    if (label == priv::_SCROLLBROWSER_MENU_LINE) {
        if (self->value() > 0) {
            clip = util::remove_browser_format(util::to_string(self->text(self->value())));
        }
    }
    else if (label == priv::_SCROLLBROWSER_MENU_ALL) {
        for (auto f = 1; f <= self->size(); f++) {
            auto s = util::remove_browser_format(util::to_string(self->text(f)));
            clip += s;
            clip += "\n";
        }
    }

    if (clip != "") {
        Fl::copy(clip.c_str(), clip.length(), 2);
    }
}

/** @brief Take care of scroll and menu events.
*
* Handle mouse wheel and keyboard scrolling.\n
* Or show menu.\n
*
*/
int flw::ScrollBrowser::handle(int event) {
    if (event == FL_MOUSEWHEEL) {
        if (_flag_move == true) {
            #ifdef DEBUG
                if (_scroll == 0) puts("warning: Scrollbrowser::scroll_lines() is 0");
            #endif

            if (Fl::event_dy() > 0) {
                topline(topline() + _scroll);
            }
            else if (Fl::event_dy() < 0) {
                topline(topline() - _scroll);
            }

            return 1;
        }
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

/** @brief Update font preferences.
*
* @param[in] text_font  Text font fo the list, label and menu.
* @param[in] text_size  Text size for the list, label and menu.
*/
void flw::ScrollBrowser::update_pref(Fl_Font text_font, Fl_Fontsize text_size) {
    labelfont(flw::PREF_FONT);
    labelsize(flw::PREF_FONTSIZE);
    textfont(text_font);
    textsize(text_size);
    _menu->textfont(text_font);
    _menu->textsize(text_size);
}

// MKALGAM_OFF
