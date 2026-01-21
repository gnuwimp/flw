/**
* @file
* @brief A slightly extended version of Fl_Hold_Browser.
*
* @author gnuwimp@gmail.com
* @copyright Released under the GNU General Public License v3.0
*/

#ifndef FLW_SCROLLBROWSER_H
#define FLW_SCROLLBROWSER_H

#include "flw.h"

// MKALGAM_ON

#include <FL/Fl_Hold_Browser.H>
#include <FL/Fl_Menu_Button.H>

namespace flw {


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

/** @brief An extended Fl_Hold_Browser with some additional functionalities.
*
* Number of lines that are scrolled with mouse or keyboard can be changed.\n
* If browser has the focus you can scroll and change current line using page up/down.\n
* Current line will always will be visible if page up/down is used.\n
*\n
* Selected line or all lines can be copied to clipboard.\n
*/
class ScrollBrowser : public Fl_Hold_Browser {
public:
                                ScrollBrowser(int lines = 9, int X = 0, int Y = 0, int W = 0, int H = 0, const char* l = nullptr);
    void                        disable_menu()
                                    { _flag_menu = false; } ///< @brief Disable popup menu.
    void                        disable_scrollmode()
                                    { _flag_move = false; } ///< @brief Use default Fl_Hold_Browser scroll mode.
    void                        enable_menu()
                                    { _flag_menu = true; } ///< @brief Turn on menu.
    void                        enable_scrollmode()
                                    { _flag_move = true; } ///< @brief Turn on scroll mode.
    bool                        is_menu_enabled() const
                                    { return _flag_menu; } ///< @brief Is menu on?
    bool                        is_scrollmode_enabled() const
                                    { return _flag_move; } ///< @brief Is scroll mode on?
    int                         handle(int event) override;
    Fl_Menu_Button*             menu()
                                    { return _menu; } ///< @brief Get menu object.
    int                         scroll_lines() const
                                    { return _scroll; } ///< @brief Get number of lines for the scroll mode.
    void                        scroll_lines(int lines)
                                    { if (lines > 0) _scroll = lines; } ///< @brief Set number of lines for the scroll mode.
    std::string                 text2() const
                                    { return util::remove_browser_format(util::to_string(text(value()))); } ///< @brief Get text from current line without formatting characters.
    std::string                 text2(int line) const
                                    { return util::remove_browser_format(util::to_string(text(line))); } ///< @brief Get text from line without formatting characters.
    void                        update_pref(Fl_Font text_font = flw::PREF_FONT, Fl_Fontsize text_size = flw::PREF_FONTSIZE);

    static void                 Callback(Fl_Widget*, void*);

private:
    Fl_Menu_Button*             _menu;
    bool                        _flag_menu;
    bool                        _flag_move;
    int                         _scroll;
};

} // flw

// MKALGAM_OFF

#endif
