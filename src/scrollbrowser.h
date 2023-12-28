// Copyright 2021 - 2022 gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#ifndef FLW_SCROLLBROWSER_H
#define FLW_SCROLLBROWSER_H

#include "util.h"
#include <FL/Fl_Hold_Browser.H>

class Fl_Menu_Button;

// MKALGAM_ON

namespace flw {
    //--------------------------------------------------------------------------
    class ScrollBrowser : public Fl_Hold_Browser {
    public:
                                        ScrollBrowser(const ScrollBrowser&) = delete;
                                        ScrollBrowser(ScrollBrowser&&) = delete;
        ScrollBrowser&                  operator=(const ScrollBrowser&) = delete;
        ScrollBrowser&                  operator=(ScrollBrowser&&) = delete;

                                        ScrollBrowser(int scroll = 9, int X = 0, int Y = 0, int W = 0, int H = 0, const char* l = nullptr);
        void                            enable_menu(bool menu)
                                            { _flag_menu = menu; }
        void                            enable_pagemove(bool move)
                                            { _flag_move = move; }
        int                             handle(int event) override;
        Fl_Menu_Button*                 menu()
                                            { return _menu; }
        void                            update_pref(Fl_Font text_font = flw::PREF_FONT, Fl_Fontsize text_size = flw::PREF_FONTSIZE);

        static void                     Callback(Fl_Widget*, void*);
        static std::string              RemoveFormat(const char* text);

    private:
        Fl_Menu_Button*                 _menu;
        bool                            _flag_menu;
        bool                            _flag_move;
        int                             _scroll;
    };
}

// MKALGAM_OFF

#endif
