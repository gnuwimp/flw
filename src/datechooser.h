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

/***
 *      _____        _        _____ _
 *     |  __ \      | |      / ____| |
 *     | |  | | __ _| |_ ___| |    | |__   ___   ___  ___  ___ _ __
 *     | |  | |/ _` | __/ _ \ |    | '_ \ / _ \ / _ \/ __|/ _ \ '__|
 *     | |__| | (_| | ||  __/ |____| | | | (_) | (_) \__ \  __/ |
 *     |_____/ \__,_|\__\___|\_____|_| |_|\___/ \___/|___/\___|_|
 *
 *
 */

//------------------------------------------------------------------------------
// An date chooser widget
// User can navigate with mouse or arrow keys within a month
// Or use buttons to jump 1 month/1 year/10 years
//
class DateChooser : public GridGroup {
public:
    explicit                    DateChooser(int X = 0, int Y = 0, int W = 0, int H = 0, const char* l = nullptr);
    void                        draw() override;
    void                        focus();
    Date                        get() const;
    int                         handle(int event) override;
    void                        set(const Date& date);


private:
    static void                 _Callback(Fl_Widget* w, void* o);
    void                        _set_label();

    Fl_Box*                     _month_label;
    Fl_Button*                  _b1;
    Fl_Button*                  _b2;
    Fl_Button*                  _b3;
    Fl_Button*                  _b4;
    Fl_Button*                  _b5;
    Fl_Button*                  _b6;
    Fl_Button*                  _b7;
    Fl_Widget*                  _canvas;
    ToolGroup*                  _buttons;
    int                         _h;
    int                         _w;
};

namespace dlg {

/***
 *       __                  _   _
 *      / _|                | | (_)
 *     | |_ _   _ _ __   ___| |_ _  ___  _ __  ___
 *     |  _| | | | '_ \ / __| __| |/ _ \| '_ \/ __|
 *     | | | |_| | | | | (__| |_| | (_) | | | \__ \
 *     |_|  \__,_|_| |_|\___|\__|_|\___/|_| |_|___/
 *
 *
 */

bool                            date(const std::string& title, Date& date, Fl_Window* parent);

} // dlg
} // flw

// MKALGAM_OFF

#endif
