// Copyright 2021 gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#ifndef FLW_WORKDIALOG_H
#define FLW_WORKDIALOG_H

#include "util.h"
#include <FL/Fl_Double_Window.H>

class Fl_Button;
class Fl_Hold_Browser;
class Fl_Toggle_Button;

// MALAGMA_ON

namespace flw {
        //----------------------------------------------------------------------
        class WorkDialog : public Fl_Double_Window {
        public:
                                        WorkDialog(const char* title, Fl_Window* parent, bool cancel, bool pause, int W = 40, int H = 10);
            void                        resize(int X, int Y, int W, int H) override;
            bool                        run(double update_time, const std::vector<std::string>& messages);
            bool                        run(double update_time, const std::string& message);

            static void                 Callback(Fl_Widget* w, void* o);

        private:
            Fl_Button*                  _cancel;
            Fl_Hold_Browser*            _label;
            Fl_Toggle_Button*           _pause;
            bool                        _ret;
            double                      _last;
            std::string                 _message;
        };
}

// MALAGMA_OFF

#endif
