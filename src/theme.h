// Copyright 2016 - 2022 gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#ifndef FLW_THEME_H
#define FLW_THEME_H

#include <string>
#include <FL/fl_draw.H>

// MKALGAM_ON

class Fl_Preferences;
class Fl_Window;

namespace flw {
    extern bool                         PREF_IS_DARK;
    extern int                          PREF_FIXED_FONT;
    extern std::string                  PREF_FIXED_FONTNAME;
    extern int                          PREF_FIXED_FONTSIZE;
    extern int                          PREF_FONT;
    extern int                          PREF_FONTSIZE;
    extern std::string                  PREF_FONTNAME;

    //--------------------------------------------------------------------------
    // Load different themes
    //
    namespace theme {
        bool                            is_dark();
        void                            labelfont(Fl_Widget* widget);
        bool                            load(std::string name);
        void                            load_icon(Fl_Window* win, int win_resource, const char** xpm_resource = nullptr, const char* name = nullptr);
        std::string                     name();
        bool                            parse(int argc, const char** argv);
        void                            pref_load(Fl_Preferences& pref, Fl_Window* window = nullptr);
        void                            pref_save(Fl_Preferences& pref, Fl_Window* window = nullptr);
    }

    //--------------------------------------------------------------------------
    // Drawing colors
    //
    namespace color {
        extern Fl_Color                 AZURE;
        extern Fl_Color                 BEIGE;
        extern Fl_Color                 BLUE;
        extern Fl_Color                 BROWN;
        extern Fl_Color                 CYAN;
        extern Fl_Color                 GRAY;
        extern Fl_Color                 GREEN;
        extern Fl_Color                 LIME;
        extern Fl_Color                 MAGENTA;
        extern Fl_Color                 MAROON;
        extern Fl_Color                 NAVY;
        extern Fl_Color                 OLIVE;
        extern Fl_Color                 PINK;
        extern Fl_Color                 PURPLE;
        extern Fl_Color                 RED;
        extern Fl_Color                 SILVER;
        extern Fl_Color                 TEAL;
        extern Fl_Color                 YELLOW;
    }

    //--------------------------------------------------------------------------
    // Show theme selection dialog
    //
    namespace dlg {
        void                            theme(bool enable_font = false, bool enable_fixedfont = false, Fl_Window* parent = nullptr);
    }
}

// MKALGAM_OFF

#endif
