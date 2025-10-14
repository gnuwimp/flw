/**
* @file
* @brief Show a wait cursor as long as WaitCursor object live.
*
* @author gnuwimp@gmail.com
* @copyright Released under the GNU General Public License v3.0
*/

#ifndef FLW_WAITCURSOR_H
#define FLW_WAITCURSOR_H

// MKALGAM_ON

namespace flw {

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

/** @brief Show an wait mouse pointer until object is deleted.
*
*/
class WaitCursor {
    static WaitCursor*          WAITCURSOR;

public:
                                WaitCursor();
                                ~WaitCursor();
};

}

// MKALGAM_OFF

#endif
