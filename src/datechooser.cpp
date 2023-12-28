// Copyright 2016 - 2021 gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#include "datechooser.h"
#include "util.h"
#include <FL/fl_draw.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Repeat_Button.H>
#include <FL/Fl_Return_Button.H>
#include <time.h>

// MKALGAM_ON

namespace flw {
    //--------------------------------------------------------------------------
    class _DateChooserCanvas : public Fl_Widget {
        Date                    _date[7][8];
        char                    _text[7][8][30];
        int                     _col;
        int                     _row;

    public:
        //----------------------------------------------------------------------
        _DateChooserCanvas() : Fl_Widget(0, 0, 0, 0, 0) {
            _row   = 1;
            _col   = 1;

            strncpy(_text[0][0], "Week", 20);
            strncpy(_text[0][1], "Mon", 20);
            strncpy(_text[0][2], "Tue", 20);
            strncpy(_text[0][3], "Wed", 20);
            strncpy(_text[0][4], "Thu", 20);
            strncpy(_text[0][5], "Fri", 20);
            strncpy(_text[0][6], "Sat", 20);
            strncpy(_text[0][7], "Sun", 20);
        }

        //----------------------------------------------------------------------
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
                        ;
                    }
                    else {
                        int v = atoi(t);

                        if (v <= d && col < 3) {
                            d = v;
                            col++;
                        }

                        if (r == _row && c == _col) {
                            bg = FL_SELECTION_COLOR;
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
                    fl_draw(t, x1, y1, w1, h1, FL_ALIGN_CENTER | FL_ALIGN_INSIDE);
                    fl_rect(x1, y1, w1, h1, FL_DARK3);
                }
            }
        }

        //----------------------------------------------------------------------
        Date& get() {
            return _date[_row][_col];
        }

        //----------------------------------------------------------------------
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

        //----------------------------------------------------------------------
        void set_current(int row, int col) {
            if (row != _row || col != _col) {
                _row = row;
                _col = col;
                do_callback();
            }
        }

        //----------------------------------------------------------------------
        void set_date(int row, int col, const Date& date) {
            _date[row][col] = date;
        }

        //----------------------------------------------------------------------
        void set_text(int row, int col, const char* text) {
            strncpy(_text[row][col], text, 30);
        }
    };
}

//------------------------------------------------------------------------------
flw::DateChooser::DateChooser(int X, int Y, int W, int H, const char* l) : Fl_Group(X, Y, W, H, l) {
    end();

    _h           = -1;
    _w           = -1;
    _month_label = new Fl_Box(0, 0, 0, 0, "");
    _canvas      = new _DateChooserCanvas();
    _b6          = new Fl_Repeat_Button(0, 0, 0, 0, "@|<");
    _b1          = new Fl_Repeat_Button(0, 0, 0, 0, "@<<");
    _b2          = new Fl_Repeat_Button(0, 0, 0, 0, "@<");
    _b5          = new Fl_Button(0, 0, 0, 0, "@refresh");
    _b3          = new Fl_Repeat_Button(0, 0, 0, 0, "@>");
    _b4          = new Fl_Repeat_Button(0, 0, 0, 0, "@>>");
    _b7          = new Fl_Repeat_Button(0, 0, 0, 0, "@>|");

    Date date;
    set(date);
    _set_label();

    add(_b1);
    add(_b2);
    add(_b5);
    add(_b3);
    add(_b4);
    add(_b6);
    add(_b7);
    add(_month_label);
    add(_canvas);

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

    flw::util::labelfont(this);
    tooltip("Use arrow keys to navigate\nUse ctrl+left/right to change month");
    flw::DateChooser::resize(X, Y, W, H);
}

//------------------------------------------------------------------------------
void flw::DateChooser::_Callback(Fl_Widget* w, void* o) {
    auto dc = (DateChooser*) o;
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
        dt = Date::FromTime(::time(0));
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

//------------------------------------------------------------------------------
void flw::DateChooser::draw() {
    _month_label->labelfont(FL_HELVETICA_BOLD);
    Fl_Group::draw();
}

//------------------------------------------------------------------------------
void flw::DateChooser::focus() {
    _canvas->take_focus();
}

//------------------------------------------------------------------------------
flw::Date flw::DateChooser::get() const {
    auto canvas = (flw::_DateChooserCanvas*) _canvas;
    return canvas->get();
}

//------------------------------------------------------------------------------
int flw::DateChooser::handle(int event) {
    if (event == FL_KEYDOWN) {
        if (Fl::event_command()) {
            Date dt = get();

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

//------------------------------------------------------------------------------
void flw::DateChooser::resize(int X, int Y, int W, int H) {
    Fl_Widget::resize(X, Y, W, H);

    if (W == _w && H == _h) {
        return;
    }

    int w1 = W / 8;
    int w2 = W - (w1 * 6);
    int fs = flw::PREF_FONTSIZE;

    _b6->resize(X, Y, w1, fs * 2);
    _b1->resize(X + w1, Y, w1, fs * 2);
    _b2->resize(X + w1 * 2, Y, w1, fs * 2);

    _b3->resize(X + w1 * 3 + w2, Y, w1, fs * 2);
    _b4->resize(X + w1 * 4 + w2, Y, w1, fs * 2);
    _b7->resize(X + w1 * 5 + w2, Y, w1, fs * 2);

    _b5->resize(X + w1 * 3, Y, w2, fs * 2);

    _month_label->resize(X, Y + fs * 2, W, fs * 2);
    _canvas->resize(X, Y + fs * 4, W, H - fs * 4);

    _w = W;
    _h = H;
}

//------------------------------------------------------------------------------
void flw::DateChooser::set(const Date& date) {
    auto canvas = (flw::_DateChooserCanvas*) _canvas;
    auto date2  = date;

    if (date2.year() < 2 && date2.month() < 2) {
        date2.month(2);
    }

    auto  start_cell  = 0;
    auto first_date   = Date(date2.year(), date2.month(), 1);
    auto current_date = Date();
    char tmp[30];

    // Optimize where to start with the 1:st in every month

    start_cell    = (int) first_date.weekday() - 1;
    start_cell    = start_cell + first_date.month_days() < 32 ? start_cell + 7 : start_cell;
    current_date  = first_date;

    current_date.add_days(-start_cell);

    for (auto r = 1; r <= 6; r++) {
        snprintf(tmp, 30, "%02d", current_date.week());
        canvas->set_text(r, 0, tmp);

        for (auto c = 1; c <= 7; c++) {
            snprintf(tmp, 30, "%02d", current_date.day());
            canvas->set_text(r, c, tmp);
            canvas->set_date(r, c, current_date);

            if (current_date.month() == date2.month() && current_date.day() == date2.day()) {
                canvas->set_current(r, c);
            }

            current_date.add_days(1);
        }
    }

    _set_label();
    redraw();
}

//------------------------------------------------------------------------------
void flw::DateChooser::_set_label() {
    auto canvas = (flw::_DateChooserCanvas*) _canvas;
    auto date   = canvas->get();
    auto string = date.format(Date::FORMAT::YEAR_MONTH_LONG);

    _month_label->copy_label(string.c_str());
}

namespace flw {
    namespace dlg {
        //----------------------------------------------------------------------
        class _DlgDate : public Fl_Double_Window {
            DateChooser*            _date_chooser;
            Date&                   _value;
            Fl_Button*              _cancel;
            Fl_Button*              _ok;
            bool                    _res;

        public:
            //------------------------------------------------------------------
            _DlgDate(const char* title, Date& date) : Fl_Double_Window(0, 0, 0, 0), _value(date) {
                end();

                _date_chooser = new DateChooser();
                _ok           = new Fl_Return_Button(0, 0, 0, 0, "&Ok");
                _cancel       = new Fl_Button(0, 0, 0, 0, "&Cancel");
                _res          = false;

                add(_date_chooser);
                add(_ok);
                add(_cancel);

                _cancel->callback(Callback, this);
                _date_chooser->focus();
                _date_chooser->set(_value);
                _ok->callback(Callback, this);

                flw::util::labelfont(this);
                callback(Callback, this);
                copy_label(title);
                set_modal();
                resizable(this);
            }

            //------------------------------------------------------------------
            static void Callback(Fl_Widget* w, void* o) {
                _DlgDate* dlg = (_DlgDate*) o;

                if (w == dlg) {
                    ;
                }
                else if (w == dlg->_cancel) {
                    dlg->hide();
                }
                else if (w == dlg->_ok) {
                    dlg->hide();
                    dlg->_res = true;
                }
            }

            //------------------------------------------------------------------
            void resize(int X, int Y, int W, int H) override {
                auto fs = flw::PREF_FONTSIZE;

                Fl_Double_Window::resize(X, Y, W, H);

                _date_chooser->resize (4,                 4,                  W - 8,    H - fs * 2 - 16);
                _cancel->resize       (W - fs * 16 - 8,   H - fs * 2 - 4,     fs * 8,   fs * 2);
                _ok->resize           (W - fs * 8 - 4,    H - fs * 2 - 4,     fs * 8,   fs * 2);
            }

            //------------------------------------------------------------------
            bool run(Fl_Window* parent) {
                size(flw::PREF_FONTSIZE * 34, flw::PREF_FONTSIZE * 24);
                util::center_window(this, parent);
                show();

                while (visible()) {
                    Fl::wait();
                    Fl::flush();
                }

                if (_res) {
                    _value = _date_chooser->get();
                }

                return _res;
            }
        };

        //----------------------------------------------------------------------
        bool date(const std::string& title, flw::Date& date, Fl_Window* parent) {
            _DlgDate dlg(title.c_str(), date);
            return dlg.run(parent);
        }
    }
}


// MKALGAM_OFF
