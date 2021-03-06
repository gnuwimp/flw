// Copyright 2021 gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#include "waitcursor.h"
#include <FL/fl_draw.H>

// MALAGMA_ON

flw::WaitCursor* flw::WaitCursor::WAITCURSOR = nullptr;

//------------------------------------------------------------------------------
flw::WaitCursor::WaitCursor() {
    if (WaitCursor::WAITCURSOR == nullptr) {
        WaitCursor::WAITCURSOR = this;
        fl_cursor(FL_CURSOR_WAIT);
        Fl::redraw();
        Fl::flush();
    }
}

//------------------------------------------------------------------------------
flw::WaitCursor::~WaitCursor() {
    if (WaitCursor::WAITCURSOR == this) {
        WaitCursor::WAITCURSOR = nullptr;
        fl_cursor(FL_CURSOR_DEFAULT);
    }
}

// MALAGMA_OFF
