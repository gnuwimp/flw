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

    return Fl_Hold_Browser::handle(event);
}

// MALAGMA_OFF
