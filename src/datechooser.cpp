/**
* @file
* @brief A date chooser widget.
*
* @author gnuwimp@gmail.com
* @copyright Released under the GNU General Public License v3.0
*/

#include "datechooser.h"
#include "flw.h"

// MKALGAM_ON

#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Repeat_Button.H>
#include <FL/Fl_Return_Button.H>
#include <FL/fl_draw.H>
#include <time.h>

namespace flw {

/*
 *           _____        _        _____ _                                _____
 *          |  __ \      | |      / ____| |                              / ____|
 *          | |  | | __ _| |_ ___| |    | |__   ___   ___  ___  ___ _ __| |     __ _ _ ____   ____ _ ___
 *          | |  | |/ _` | __/ _ \ |    | '_ \ / _ \ / _ \/ __|/ _ \ '__| |    / _` | '_ \ \ / / _` / __|
 *          | |__| | (_| | ||  __/ |____| | | | (_) | (_) \__ \  __/ |  | |___| (_| | | | \ V / (_| \__ \
 *          |_____/ \__,_|\__\___|\_____|_| |_|\___/ \___/|___/\___|_|   \_____\__,_|_| |_|\_/ \__,_|___/
 *      ______
 *     |______|
 */

/** @brief Date panel for DateChooser.
* @private
*/
class _DateChooserCanvas : public Fl_Widget {
    gnu::Date                   _date[7][8];        // One date object for every cell.
    std::string                 _text[7][8];        // Date string for every cell.
    int                         _col;               // Current cell column.
    int                         _row;               // Current cell row.

public:
    /** @brief Create and initiate object.
    *
    */
    _DateChooserCanvas() :
    Fl_Widget(0, 0, 0, 0, 0) {
        _row   = 1;
        _col   = 1;

        _text[0][0] = "Week";
        _text[0][1] = "Mon";
        _text[0][2] = "Tue";
        _text[0][3] = "Wed";
        _text[0][4] = "Thu";
        _text[0][5] = "Fri";
        _text[0][6] = "Sat";
        _text[0][7] = "Sun";
    }

    /** @brief Draw all cells.
    *
    */
    void draw() override {
        int cw  = w() / 8;
        int ch  = h() / 7;
        int d   = 99;
        int col = 0;

        fl_rectf(x(), y(), w(), h(), FL_BACKGROUND_COLOR);

        for (int r = 0; r < 7; r++) {
            for (int c  = 0; c < 8; c++) {
                auto t  = _text[r][c];
                auto x1 = x() + (c * cw) + 1;
                auto y1 = y() + (r * ch) + 1;
                auto w1 = (c == 7) ? x() + w() - x1 : cw - 2;
                auto h1 = (r == 6) ? y() + h() - y1 : ch - 2;
                auto bg = FL_BACKGROUND_COLOR;
                auto fg = FL_FOREGROUND_COLOR;

                if (r == 0 || c == 0) {
                }
                else {
                    int v = atoi(t.c_str());

                    if (v <= d && col < 3) {
                        d = v;
                        col++;
                    }

                    if (r == _row && c == _col) {
                        bg = FL_SELECTION_COLOR;
                        fg = fl_contrast(FL_FOREGROUND_COLOR, bg);
                    }
                    else if (col == 2) {
                        bg = FL_BACKGROUND2_COLOR;
                    }
                    else {
                        bg = FL_DARK2;
                    }
                }

                fl_rectf(x1, y1, w1, h1, bg);
                fl_color(fg);
                fl_font(r == 0 || c == 0 ? FL_HELVETICA_BOLD : FL_HELVETICA, flw::PREF_FONTSIZE);
                fl_draw(t.c_str(), x1, y1, w1, h1, FL_ALIGN_CENTER | FL_ALIGN_INSIDE);
                fl_rect(x1, y1, w1, h1, FL_DARK3);
            }
        }
    }

    /** @brief Get date object from current cell.
    *
    */
    gnu::Date get() {
        return _date[_row][_col];
    }

    /** @brief Select current cell/date.
    *
    * Either with a with a mouse click.\n
    * Or arrow keys.\n
    */
    int handle(int event) override {
        if (event == FL_PUSH) {
            take_focus();

            auto cw = (int) (w() / 8);
            auto ch = (int) (h() / 7);

            for (auto r = 1; r < 7; r++) {
                for (auto c = 1; c < 8; c++) {
                    auto x1 = (int) (x() + (c * cw));
                    auto y1 = (int) (y() + (r * ch));

                    if (Fl::event_x() >= x1 && Fl::event_x() < x1 + cw && Fl::event_y() >= y1 && Fl::event_y() < y1 + ch) {
                        _row = r;
                        _col = c;
                        do_callback();
                        redraw();
                        break;
                    }
                }
            }

            return 1;
        }
        else if (event == FL_FOCUS) {
            return 1;
        }
        else if (event == FL_KEYDOWN) {
            if (Fl::event_command() == 0) {
                int num = Fl::event_key();

                if (num == FL_Up) {
                    if (_row > 1) {
                        _row--;
                        redraw();
                        do_callback();
                    }

                    return 1;
                }
                else if (num == FL_Down) {
                    if (_row < 6) {
                        _row++;
                        redraw();
                        do_callback();
                    }

                    return 1;
                }
                else if (num == FL_Left) {
                    if (_col > 1) {
                        _col--;
                        redraw();
                        do_callback();
                    }

                    return 1;
                }
                else if (num == FL_Right) {
                    if (_col < 7) {
                        _col++;
                        redraw();
                        do_callback();
                    }

                    return 1;
                }
            }
        }

        return Fl_Widget::handle(event);
    }

    /** @brief Set current cell and do callback.
    *
    */
    void set_current(int row, int col) {
        if (row != _row || col != _col) {
            _row = row;
            _col = col;
            do_callback();
        }
    }

    /** @brief Set date object.
    *
    */
    void set_date(int row, int col, const gnu::Date& date) {
        _date[row][col] = date;
    }

    /** @brief Set Date text.
    *
    */
    void set_text(int row, int col, const std::string& text) {
        _text[row][col] = text;
    }
};

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

/** @brief Create date chooser widget.
*
* @param[in] X  X pos.
* @param[in] Y  Y pos.
* @param[in] W  Width.
* @param[in] H  Height.
* @param[in] l  Label.
*/
flw::DateChooser::DateChooser(int X, int Y, int W, int H, const char* l) :
GridGroup(X, Y, W, H, l) {
    end();

    _b1          = new Fl_Repeat_Button(0, 0, 0, 0, "@<<");
    _b2          = new Fl_Repeat_Button(0, 0, 0, 0, "@<");
    _b3          = new Fl_Repeat_Button(0, 0, 0, 0, "@>");
    _b4          = new Fl_Repeat_Button(0, 0, 0, 0, "@>>");
    _b5          = new Fl_Button(0, 0, 0, 0, "@refresh");
    _b6          = new Fl_Repeat_Button(0, 0, 0, 0, "@|<");
    _b7          = new Fl_Repeat_Button(0, 0, 0, 0, "@>|");
    _buttons     = new ToolGroup();
    _canvas      = new _DateChooserCanvas();
    _month_label = new Fl_Box(0, 0, 0, 0, "");

    gnu::Date date;
    set(date);
    _set_label();

    _buttons->add(_b6);
    _buttons->add(_b1);
    _buttons->add(_b2);
    _buttons->add(_b5, 8);
    _buttons->add(_b3);
    _buttons->add(_b4);
    _buttons->add(_b7);
    _buttons->expand_last(true);
    add(_buttons,     0, 0, 0, 4);
    add(_month_label, 0, 4, 0, 4);
    add(_canvas,      0, 8, 0, 0);

    _b1->callback(flw::DateChooser::_Callback, this);
    _b1->tooltip("Previous year");
    _b2->callback(flw::DateChooser::_Callback, this);
    _b2->tooltip("Previous month");
    _b3->callback(flw::DateChooser::_Callback, this);
    _b3->tooltip("Next month");
    _b4->callback(flw::DateChooser::_Callback, this);
    _b4->tooltip("Next year");
    _b5->callback(flw::DateChooser::_Callback, this);
    _b5->tooltip("Today");
    _b6->callback(flw::DateChooser::_Callback, this);
    _b6->tooltip("10 years in the past");
    _b7->callback(flw::DateChooser::_Callback, this);
    _b7->tooltip("10 years in the future");
    _canvas->callback(flw::DateChooser::_Callback, this);
    _month_label->box(FL_UP_BOX);

    util::labelfont(this);
    tooltip("Use arrow keys to navigate\nUse ctrl+left/right to change month");
    resizable(this);
    do_layout();
}

/** @brief Callback for all widgets.
*
* Select new date cell.
*
* @param[in] w  Widget.
* @param[in] o  DateChooser widget..
*/
void flw::DateChooser::_Callback(Fl_Widget* w, void* o) {
    auto dc = static_cast<DateChooser*>(o);
    auto dt = dc->get();

    if (w == dc->_b6) {
        dt.add_months(-120);
        dc->set(dt);
    }
    else if (w == dc->_b1) {
        dt.add_months(-12);
        dc->set(dt);
    }
    else if (w == dc->_b2) {
        dt.add_months(-1);
        dc->set(dt);
    }
    else if (w == dc->_b5) {
        dt = gnu::Date(::time(0));
        dc->set(dt);
    }
    else if (w == dc->_b3) {
        dt.add_months(1);
        dc->set(dt);
    }
    else if (w == dc->_b4) {
        dt.add_months(12);
        dc->set(dt);
    }
    else if (w == dc->_b7) {
        dt.add_months(120);
        dc->set(dt);
    }
    else if (w == dc->_canvas) {
        dc->_set_label();
        dc->do_callback();
    }

    dc->_canvas->take_focus();
}

/** @brief Draw cells.
*
*/
void flw::DateChooser::draw() {
    _month_label->labelfont(FL_HELVETICA_BOLD);
    Fl_Group::draw();
}

/** @brief Take focus.
*
*/
void flw::DateChooser::focus() {
    _canvas->take_focus();
}

/** @brief Get current date.
*
* @return Date object.
*/
gnu::Date flw::DateChooser::get() const {
    return static_cast<flw::_DateChooserCanvas*>(_canvas)->get();
}

/** @brief Change current cell by keyboard.
*
*/
int flw::DateChooser::handle(int event) {
    if (event == FL_KEYDOWN) {
        if (Fl::event_command()) {
            gnu::Date dt = get();

            if (Fl::event_key() == FL_Left) {
                dt.add_months(-1);
                set(dt);
                return 1;
            }
            else if (Fl::event_key() == FL_Right) {
                dt.add_months(+1);
                set(dt);
                return 1;
            }
        }
    }

    return Fl_Group::handle(event);
}

/** @brief Set current date.
*
* @param[in] date  Date object.
*/
void flw::DateChooser::set(const gnu::Date& date) {
    auto arg    = date;
    auto canvas = static_cast<flw::_DateChooserCanvas*>(_canvas);

    if (arg.is_invalid() == true) {
        arg = gnu::Date();
    }
    else if (arg.year() < 2 && arg.month() < 2) {
        arg.set_month(2);
    }

    auto start_cell   = 0;
    auto first_date   = gnu::Date(arg.year(), arg.month(), 1);
    auto current_date = gnu::Date();
    char tmp[30];

    // Optimize where to start with the 1:st in every month

    start_cell    = static_cast<int>(first_date.weekday()) - 1;
    start_cell    = start_cell + first_date.days_in_month() < 32 ? start_cell + 7 : start_cell;
    current_date  = first_date;

    current_date.add_days(-start_cell);

    for (auto r = 1; r <= 6; r++) {
        snprintf(tmp, 30, "%02d", current_date.week());
        canvas->set_text(r, 0, tmp);

        for (auto c = 1; c <= 7; c++) {
            snprintf(tmp, 30, "%02d", current_date.day());
            canvas->set_text(r, c, tmp);
            canvas->set_date(r, c, current_date);

            if (current_date.month() == arg.month() && current_date.day() == arg.day()) {
                canvas->set_current(r, c);
            }

            current_date.add_days(1);
        }
    }

    _set_label();
    redraw();
}

/** @brief Set month label.
*
*/
void flw::DateChooser::_set_label() {
    auto canvas = static_cast<flw::_DateChooserCanvas*>(_canvas);
    auto date   = canvas->get();
    auto string = date.format(gnu::Date::Format::WEEKDAY_MONTH_YEAR);

    _month_label->copy_label(string.c_str());
}

} // flw

// MKALGAM_OFF
