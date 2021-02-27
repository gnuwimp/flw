// Copyright 2016 - 2021 gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#ifndef FLW_DLG_H
#define FLW_DLG_H

#include "util.h"

class Fl_Window;

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
    }
}

// MALAGMA_OFF

#endif
