// Copyright gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#ifndef FLW_SCROLLBROWSER_H
#define FLW_SCROLLBROWSER_H

#include "flw.h"

// MKALGAM_ON

#include <FL/Fl_Hold_Browser.H>
#include <FL/Fl_Menu_Button.H>

namespace flw {


/***
 *       _____                _ _ ____                                  
 *      / ____|              | | |  _ \                                 
 *     | (___   ___ _ __ ___ | | | |_) |_ __ _____      _____  ___ _ __ 
 *      \___ \ / __| '__/ _ \| | |  _ <| '__/ _ \ \ /\ / / __|/ _ \ '__|
 *      ____) | (__| | | (_) | | | |_) | | | (_) \ V  V /\__ \  __/ |   
 *     |_____/ \___|_|  \___/|_|_|____/|_|  \___/ \_/\_/ |___/\___|_|   
 *                                                                      
 *                                                                      
 */

//------------------------------------------------------------------------------
class ScrollBrowser : public Fl_Hold_Browser {
public:
                                ScrollBrowser(const ScrollBrowser&) = delete;
                                ScrollBrowser(ScrollBrowser&&) = delete;
    ScrollBrowser&              operator=(const ScrollBrowser&) = delete;
    ScrollBrowser&              operator=(ScrollBrowser&&) = delete;

    explicit                    ScrollBrowser(int scroll = 9, int X = 0, int Y = 0, int W = 0, int H = 0, const char* l = nullptr);
    bool                        enable_menu() const
                                    { return _flag_menu; }
    void                        enable_menu(bool menu)
                                    { _flag_menu = menu; }
    bool                        enable_pagemove() const
                                    { return _flag_move; }
    void                        enable_pagemove(bool move)
                                    { _flag_move = move; }
    int                         handle(int event) override;
    Fl_Menu_Button*             menu()
                                    { return _menu; }
    std::string                 text2() const
                                    { return util::remove_browser_format(util::to_string(text(value()))); }
    std::string                 text2(int line) const
                                    { return util::remove_browser_format(util::to_string(text(line))); }
    void                        update_pref()
                                    { update_pref(flw::PREF_FONT, flw::PREF_FONTSIZE); }
    void                        update_pref(Fl_Font text_font, Fl_Fontsize text_size);

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
