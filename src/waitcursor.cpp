/**
* @file
* @brief Show a wait cursor as long as WaitCursor object live.
*
* @author gnuwimp@gmail.com
* @copyright Released under the GNU General Public License v3.0
*/

#include "waitcursor.h"

// MKALGAM_ON

#include <FL/fl_draw.H>
#include <FL/Fl.H>

/*
 *     __          __   _ _    _____
 *     \ \        / /  (_) |  / ____|
 *      \ \  /\  / /_ _ _| |_| |    _   _ _ __ ___  ___  _ __
 *       \ \/  \/ / _` | | __| |   | | | | '__/ __|/ _ \| '__|
 *        \  /\  / (_| | | |_| |___| |_| | |  \__ \ (_) | |
 *         \/  \/ \__,_|_|\__|\_____\__,_|_|  |___/\___/|_|
 *
 *
 */

flw::WaitCursor* flw::WaitCursor::WAITCURSOR = nullptr;

/** @brief Create waitcursor.
*
* If wait cursor has been created already this will no nothing.\n
* Only one is allowed at the same time.\n
*/
flw::WaitCursor::WaitCursor() {
    if (WaitCursor::WAITCURSOR == nullptr) {
        WaitCursor::WAITCURSOR = this;
        fl_cursor(FL_CURSOR_WAIT);
        Fl::redraw();
        Fl::flush();
    }
}

/** @brief Restore cursor.
*
*/
flw::WaitCursor::~WaitCursor() {
    if (WaitCursor::WAITCURSOR == this) {
        WaitCursor::WAITCURSOR = nullptr;
        fl_cursor(FL_CURSOR_DEFAULT);
    }
}

// MKALGAM_OFF
