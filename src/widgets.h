// Copyright 2021 gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#ifndef FLW_WORKDIALOG_H
#define FLW_WORKDIALOG_H

#include "util.h"
#include <FL/Fl_Hold_Browser.H>

class Fl_Button;
class Fl_Hold_Browser;
class Fl_Toggle_Button;

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
        int                             handle(int event) override;

    private:
        int                             _scroll;
    };
}

// MALAGMA_OFF

#endif
