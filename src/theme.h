// Copyright 2016 - 2021 gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#ifndef FLW_THEME_H
#define FLW_THEME_H

#include <string>

// MKALGAM_ON

class Fl_Window;
class Fl_Preferences;

namespace flw {
    namespace theme {
        //----------------------------------------------------------------------
        bool                            is_dark();
        bool                            load(const std::string& name);
        std::string                     name();
        bool                            parse(int argc, const char** argv);
    }

    namespace dlg {
        void                            theme(bool enable_font = false, bool enable_fixedfont = false, Fl_Window* parent = nullptr);
    }

    namespace util {
        void                            pref_load(Fl_Preferences& pref, Fl_Window* window = nullptr, int resource = 0);
        void                            pref_save(Fl_Preferences& pref, Fl_Window* window = nullptr);
    }
}

// MKALGAM_OFF

#endif
