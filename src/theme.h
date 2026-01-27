/**
* @file
* @brief Theme and resource.
*
* @author gnuwimp@gmail.com
* @copyright Released under the GNU General Public License v3.0
*/

#ifndef FLW_THEME_H
#define FLW_THEME_H

#include "flw.h"

// MKALGAM_ON

#include <FL/Fl_Preferences.H>
#include <FL/Fl_Rect.H>

namespace flw {

/*
 *                _
 *               | |
 *       ___ ___ | | ___  _ __
 *      / __/ _ \| |/ _ \| '__|
 *     | (_| (_) | | (_) | |
 *      \___\___/|_|\___/|_|
 *
 *
 */

/** @brief Color values.
    Fl_Color BEIGE\n
    Fl_Color CHOCOLATE\n
    Fl_Color CRIMSON\n
    Fl_Color DARKOLIVEGREEN\n
    Fl_Color DODGERBLUE\n
    Fl_Color FORESTGREEN\n
    Fl_Color GOLD\n
    Fl_Color GRAY\n
    Fl_Color INDIGO\n
    Fl_Color OLIVE\n
    Fl_Color PINK\n
    Fl_Color ROYALBLUE\n
    Fl_Color SIENNA\n
    Fl_Color SILVER\n
    Fl_Color SLATEGRAY\n
    Fl_Color TEAL\n
    Fl_Color TURQUOISE\n
    Fl_Color VIOLET\n
*/
namespace color {
    extern Fl_Color             BEIGE;
    extern Fl_Color             CHOCOLATE;
    extern Fl_Color             CRIMSON;
    extern Fl_Color             DARKOLIVEGREEN;
    extern Fl_Color             DODGERBLUE;
    extern Fl_Color             FORESTGREEN;
    extern Fl_Color             GOLD;
    extern Fl_Color             GRAY;
    extern Fl_Color             INDIGO;
    extern Fl_Color             OLIVE;
    extern Fl_Color             PINK;
    extern Fl_Color             ROYALBLUE;
    extern Fl_Color             SIENNA;
    extern Fl_Color             SILVER;
    extern Fl_Color             SLATEGRAY;
    extern Fl_Color             TEAL;
    extern Fl_Color             TURQUOISE;
    extern Fl_Color             VIOLET;
} // flw::color

/*
 *      _   _
 *     | | | |
 *     | |_| |__   ___ _ __ ___   ___
 *     | __| '_ \ / _ \ '_ ` _ \ / _ \
 *     | |_| | | |  __/ | | | | |  __/
 *      \__|_| |_|\___|_| |_| |_|\___|
 *
 *
 */

/** @brief Load different themes and save/load window preferences.
*
* Load theme before loading windows preferences.\n
* Scaling settings are not optimal and works different depending on which desktop are being used.\n
*/
namespace theme {
    bool                        is_dark();
    bool                        load(const std::string& name);
    Fl_Font                     load_font(const std::string& requested_font);
    void                        load_fonts(bool iso8859_only = true);
    void                        load_icon(Fl_Window* win, int win_resource, const char** xpm_resource = nullptr, const char* name = nullptr);
    Fl_Rect                     load_rect_from_pref(Fl_Preferences& pref, const std::string& basename);
    double                      load_theme_from_pref(Fl_Preferences& pref, unsigned screen_num = 0);
    void                        load_win_from_pref(Fl_Preferences& pref, const std::string& basename, Fl_Window* window, bool show = true, int defw = 800, int defh = 600);
    bool                        parse(int argc, const char** argv);
    void                        save_rect_to_pref(Fl_Preferences& pref, const std::string& basename, const Fl_Rect& rect);
    void                        save_theme_to_pref(Fl_Preferences& pref);
    void                        save_win_to_pref(Fl_Preferences& pref, const std::string& basename, Fl_Window* window);
} // flw::theme

namespace dlg {
    void                        theme(bool enable_font = false, bool enable_fixedfont = false);
} // flw::dlg

} // flw

// MKALGAM_OFF

#endif
