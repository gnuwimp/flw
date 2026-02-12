/**
* @file
* @brief A date chooser widget.
*
* @author gnuwimp@gmail.com
* @copyright Released under the GNU General Public License v3.0
*/

#ifndef FLW_DATECHOOSER_H
#define FLW_DATECHOOSER_H

#include "date.h"
#include "gridgroup.h"

// MKALGAM_ON

class Fl_Box;
class Fl_Button;

namespace flw {

class ToolGroup;

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
* flw::dlg::date() creates and shows an dialog with a datechooser widget.\n
*
* @snippet dialog.cpp flw::dlg::date example
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

    Fl_Box*                     _month_label;   ///< @brief Date label "dayname DD monthname YYYY".
    Fl_Button*                  _b1;            ///< @brief Jump one year back.
    Fl_Button*                  _b2;            ///< @brief One month back.
    Fl_Button*                  _b3;            ///< @brief Next month.
    Fl_Button*                  _b4;            ///< @brief Next year.
    Fl_Button*                  _b5;            ///< @brief Goto todays date.
    Fl_Button*                  _b6;            ///< @brief 10 years back.
    Fl_Button*                  _b7;            ///< @brief 10 years forward.
    Fl_Widget*                  _canvas;        ///< @brief _DateChooserCanvas (month view).
    ToolGroup*                  _buttons;       ///< @brief Toolbar for the buttobs.
};

namespace dlg {
    bool                        date(const std::string& title, gnu::Date& date, int W = 33, int H = 21);
} // flw::dlg
} // flw

// MKALGAM_OFF

#endif
