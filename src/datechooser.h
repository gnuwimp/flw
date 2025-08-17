// Copyright gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#ifndef FLW_DATECHOOSER_H
#define FLW_DATECHOOSER_H

#include "date.h"
#include "gridgroup.h"
#include "toolgroup.h"

// MKALGAM_ON

#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Group.H>

namespace flw {

/*
 *      _____        _        _____ _
 *     |  __ \      | |      / ____| |
 *     | |  | | __ _| |_ ___| |    | |__   ___   ___  ___  ___ _ __
 *     | |  | |/ _` | __/ _ \ |    | '_ \ / _ \ / _ \/ __|/ _ \ '__|
 *     | |__| | (_| | ||  __/ |____| | | | (_) | (_) \__ \  __/ |
 *     |_____/ \__,_|\__\___|\_____|_| |_|\___/ \___/|___/\___|_|
 *
 *
 */

/** @brief An date chooser widget.
*
* User can navigate with mouse or arrow keys within a month.\n
* Or use buttons to jump 1 month/1 year/10 years forward or backward in time.\n
*
* @image html date_dialog.png
*/
class DateChooser : public GridGroup {
public:
    explicit                    DateChooser(int X = 0, int Y = 0, int W = 0, int H = 0, const char* l = nullptr);
    void                        draw() override;
    void                        focus();
    gnu::Date                   get() const;
    int                         handle(int event) override;
    void                        set(const gnu::Date& date);


private:
    static void                 _Callback(Fl_Widget* w, void* o);
    void                        _set_label();

    Fl_Box*                     _month_label;   // Date label "dayname DD monthname YYYY".
    Fl_Button*                  _b1;            // Jump one year back.
    Fl_Button*                  _b2;            // One month back.
    Fl_Button*                  _b3;            // Next month.
    Fl_Button*                  _b4;            // Next year.
    Fl_Button*                  _b5;            // Goto todays date.
    Fl_Button*                  _b6;            // 10 years back.
    Fl_Button*                  _b7;            // 10 years forward.
    Fl_Widget*                  _canvas;        // _DateChooserCanvas (month view).
    ToolGroup*                  _buttons;       // Toolbar for the buttobs.
};

}

// MKALGAM_OFF

#endif
