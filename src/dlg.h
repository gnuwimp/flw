// Copyright 2016 - 2021 gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#ifndef FLW_DLG_H
#define FLW_DLG_H

#include "util.h"
#include <FL/Fl_Double_Window.H>

class Fl_Button;
class Fl_Hold_Browser;
class Fl_Toggle_Button;
class Fl_Hor_Fill_Slider;

// MALAGMA_ON

namespace flw {
    namespace dlg {
        //----------------------------------------------------------------------
        extern const char*              PASSWORD_CANCEL;
        extern const char*              PASSWORD_OK;

        void                            html(const std::string& title, const std::string& text, Fl_Window* parent = nullptr, int W = 40, int H = 23);
        void                            list(const std::string& title, const std::vector<std::string>& list, Fl_Window* parent = nullptr, bool fixed_font = false, int W = 40, int H = 23);
        void                            list(const std::string& title, const std::string& list, Fl_Window* parent = nullptr, bool fixed_font = false, int W = 40, int H = 23);
        void                            panic(std::string message);
        bool                            password1(const std::string& title, std::string& password, Fl_Window* parent = nullptr);
        bool                            password2(const std::string& title, std::string& password, Fl_Window* parent = nullptr);
        bool                            password3(const std::string& title, std::string& password, std::string& file, Fl_Window* parent = nullptr);
        bool                            password4(const std::string& title, std::string& password, std::string& file, Fl_Window* parent = nullptr);
        int                             select(const std::string& title, const std::vector<std::string>& list, int select_row, Fl_Window* parent = nullptr, bool fixed_font = false, int W = 40, int H = 23);
        int                             select(const std::string& title, const std::vector<std::string>& list, const std::string& select_row, Fl_Window* parent = nullptr, bool fixed_font = false, int W = 40, int H = 23);
        void                            text(const std::string& title, const std::string& text, Fl_Window* parent = nullptr, bool fixed_font = false, int W = 40, int H = 23);

        //----------------------------------------------------------------------
        class AbortDialog : public Fl_Double_Window {
        public:
                                        AbortDialog(const AbortDialog&) = delete;
                                        AbortDialog(AbortDialog&&) = delete;
            AbortDialog&                operator=(const AbortDialog&) = delete;
            AbortDialog&                operator=(AbortDialog&&) = delete;

                                        AbortDialog(double min = 0.0, double max = 0.0);
            bool                        abort(int milliseconds = 200);
            void                        range(double min, double max);
            void                        show(const std::string& label, Fl_Window* parent = nullptr);
            void                        value(double value);

            inline bool                 aborted() { return _abort; }

            static void                 Callback(Fl_Widget* w, void* o);

        private:
            Fl_Button*                  _button;
            Fl_Hor_Fill_Slider*         _progress;
            bool                        _abort;
            int64_t                     _last;
        };

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
}

// MALAGMA_OFF

#endif
