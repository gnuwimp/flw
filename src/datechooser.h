// Copyright 2016 - 2022 gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#ifndef FLW_DATECHOOSER_H
#define FLW_DATECHOOSER_H

#include "date.h"
#include <FL/Fl_Group.H>

class Fl_Box;
class Fl_Button;

// MKALGAM_ON

namespace flw {
    //--------------------------------------------------------------------------
    // An date chooser widget
    // User can navigate with mouse or arrow keys within a month
    // Or use buttons to jump 1 month/1 year/10 years
    //
    class DateChooser : public Fl_Group {
    public:
                                        DateChooser(int X = 0, int Y = 0, int W = 0, int H = 0, const char* l = nullptr);
        void                            draw() override;
        void                            focus();
        Date                            get() const;
        int                             handle(int event) override;
        void                            resize(int X, int Y, int W, int H) override;
        void                            set(const Date& date);


    private:
        static void                     _Callback(Fl_Widget* w, void* o);
        void                            _set_label();

        Fl_Box*                         _month_label;
        Fl_Button*                      _b1;
        Fl_Button*                      _b2;
        Fl_Button*                      _b3;
        Fl_Button*                      _b4;
        Fl_Button*                      _b5;
        Fl_Button*                      _b6;
        Fl_Button*                      _b7;
        Fl_Widget*                      _canvas;
        int                             _h;
        int                             _w;
    };

    namespace dlg {
        bool                            date(const std::string& title, Date& date, Fl_Window* parent);
    }
}

// MKALGAM_OFF

#endif
