// Copyright 2021 gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#ifndef FLW_ABORTDIALOG_H
#define FLW_ABORTDIALOG_H

#include <FL/Fl_Double_Window.H>
#include <string>

class Fl_Button;

// MALAGMA_ON

namespace flw {
    //--------------------------------------------------------------------------
    class AbortDialog : public Fl_Double_Window {
    public:
                                        AbortDialog(const AbortDialog&) = delete;
                                        AbortDialog(AbortDialog&&) = delete;
        AbortDialog&                    operator=(const AbortDialog&) = delete;
        AbortDialog&                    operator=(AbortDialog&&) = delete;

                                        AbortDialog();
        bool                            abort(int milliseconds = 200);
        void                            show(const std::string& label, Fl_Window* parent = nullptr);

        inline bool                     aborted() { return _abort; }

        static void                     Callback(Fl_Widget* w, void* o);

    private:
        Fl_Button*                      _button;
        bool                            _abort;
        int64_t                         _last;
    };
}

// MALAGMA_OFF

#endif
