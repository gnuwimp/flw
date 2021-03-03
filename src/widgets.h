// Copyright 2021 gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#ifndef FLW_SCROLLBROWSER_H
#define FLW_SCROLLBROWSER_H

#include "util.h"
#include <FL/Fl_Hold_Browser.H>

// MALAGMA_ON

namespace flw {
    //--------------------------------------------------------------------------
    class ScrollBrowser : public Fl_Hold_Browser {
    public:
                                        ScrollBrowser(const ScrollBrowser&) = delete;
                                        ScrollBrowser(ScrollBrowser&&) = delete;
        ScrollBrowser&                  operator=(const ScrollBrowser&) = delete;
        ScrollBrowser&                  operator=(ScrollBrowser&&) = delete;

                                        ScrollBrowser(int scroll = 9, int X = 0, int Y = 0, int W = 0, int H = 0);
        void                            copy_line_to_clipboard() const;
        int                             handle(int event) override;

        void                            activate_page_move(bool move) { _move = move; }

        static std::string              RemoveFormat(const char* text);

    private:
        bool                            _move;
        int                             _scroll;
    };
}

// MALAGMA_OFF

#endif
