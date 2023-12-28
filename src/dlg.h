// Copyright gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#ifndef FLW_DLG_H
#define FLW_DLG_H

#include "util.h"

// MKALGAM_ON

#include <FL/Fl_Button.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Hold_Browser.H>
#include <FL/Fl_Hor_Fill_Slider.H>
#include <FL/Fl_Toggle_Button.H>

namespace flw {
    namespace dlg {
        //----------------------------------------------------------------------
        extern const char*              PASSWORD_CANCEL;
        extern const char*              PASSWORD_OK;

        void                            html(std::string title, const std::string& text, Fl_Window* parent = nullptr, int W = 40, int H = 23);
        void                            list(std::string title, const StringVector& list, Fl_Window* parent = nullptr, bool fixed_font = false, int W = 40, int H = 23);
        void                            list(std::string title, const std::string& list, Fl_Window* parent = nullptr, bool fixed_font = false, int W = 40, int H = 23);
        void                            list_file(std::string title, std::string file, Fl_Window* parent = nullptr, bool fixed_font = false, int W = 40, int H = 23);
        void                            panic(std::string message);
        bool                            password1(std::string title, std::string& password, Fl_Window* parent = nullptr);
        bool                            password2(std::string title, std::string& password, Fl_Window* parent = nullptr);
        bool                            password3(std::string title, std::string& password, std::string& file, Fl_Window* parent = nullptr);
        bool                            password4(std::string title, std::string& password, std::string& file, Fl_Window* parent = nullptr);
        int                             select(std::string title, const StringVector& list, int select_row, Fl_Window* parent = nullptr, bool fixed_font = false, int W = 40, int H = 23);
        int                             select(std::string title, const StringVector& list, const std::string& select_row, Fl_Window* parent = nullptr, bool fixed_font = false, int W = 40, int H = 23);
        void                            text(std::string title, const std::string& text, Fl_Window* parent = nullptr, int W = 40, int H = 23);
        bool                            text_edit(std::string title, std::string& text, Fl_Window* parent = nullptr, int W = 40, int H = 23);

        //----------------------------------------------------------------------
        class AbortDialog : public Fl_Double_Window {
        public:
                                        AbortDialog(const AbortDialog&) = delete;
                                        AbortDialog(AbortDialog&&) = delete;
            AbortDialog&                operator=(const AbortDialog&) = delete;
            AbortDialog&                operator=(AbortDialog&&) = delete;

                                        AbortDialog(double min = 0.0, double max = 0.0);
            bool                        check(int milliseconds = 200);
            bool                        check(double value, double min, double max, int milliseconds = 200);
            bool                        aborted()
                                            { return _abort; }
            void                        range(double min, double max);
            void                        resize(int X, int Y, int W, int H) override;
            void                        show(const std::string& label, Fl_Window* parent = nullptr);
            void                        value(double value);


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
            bool                        run(double update_time, const StringVector& messages);
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

// MKALGAM_OFF

#endif
