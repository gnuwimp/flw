/**
* @file
* @brief Assorted utility stuff.
*
* @author gnuwimp@gmail.com
* @copyright Released under the GNU General Public License v3.0
*/

#include "flw.h"
#include "waitcursor.h"

// MKALGAM_ON

#include <algorithm>
#include <cstdint>
#include <assert.h>
#include <stdarg.h>
#include <time.h>
#include <FL/Fl_Window.H>
#include <FL/Fl_File_Chooser.H>
#include <FL/Fl_Hold_Browser.H>
#include <FL/Fl_Tooltip.H>
#include <FL/fl_ask.H>
#include <FL/fl_draw.H>
#include <FL/Fl_SVG_Image.H>

#ifdef _WIN32
    #include <FL/x.H>
    #include <windows.h>
#else
    #include <unistd.h>
#endif

#ifdef FLW_USE_PNG
    #include <FL/Fl_PNG_Image.H>
    #include <FL/fl_draw.H>
#endif

/*
 *       __ _
 *      / _| |
 *     | |_| |_      __
 *     |  _| \ \ /\ / /
 *     | | | |\ V  V /
 *     |_| |_| \_/\_/
 *
 *
 */

namespace flw {

std::vector<char*>          PREF_FONTNAMES;
int                         PREF_FIXED_FONT         = FL_COURIER;
std::string                 PREF_FIXED_FONTNAME     = "FL_COURIER";
int                         PREF_FIXED_FONTSIZE     = 14;
int                         PREF_FONT               = FL_HELVETICA;
int                         PREF_FONTSIZE           = 14;
std::string                 PREF_FONTNAME           = "FL_HELVETICA";
double                      PREF_SCALE_VAL          = 1.0;
bool                        PREF_SCALE_ON           = true;
std::string                 PREF_THEME              = "default";

const StringVector PREF_THEMES = {
    "default",
    "gleam",
    "blue gleam",
    "dark gleam",
    "tan gleam",
    "gtk",
    "blue gtk",
    "dark gtk",
    "tan gtk",
    "oxy",
    "tan oxy",
    "plastic",
    "tan plastic",
};

const StringVector PREF_THEMES2 = {
    "default",
    "gleam",
    "blue_gleam",
    "dark_gleam",
    "tan_gleam",
    "gtk",
    "blue_gtk",
    "dark_gtk",
    "tan_gtk",
    "oxy",
    "tan_oxy",
    "plastic",
    "tan_plastic",
};

/*
 *                 _
 *                (_)
 *      _ __  _ __ ___   __
 *     | '_ \| '__| \ \ / /
 *     | |_) | |  | |\ V /
 *     | .__/|_|  |_| \_/
 *     | |
 *     |_|
 */

namespace priv {

static unsigned char _OLD_R[256]  = { 0 };
static unsigned char _OLD_G[256]  = { 0 };
static unsigned char _OLD_B[256]  = { 0 };
static bool          _IS_DARK     = false;
static bool          _SAVED_COLOR = false;
static int           _SCROLLSIZE  = Fl::scrollbar_size();

/** @brief Print to postscript file.
*
* @param[in] ps_filename  Filename.
* @param[in] format       Page format.
* @param[in] layout       Layout.
* @param[in] cb           User callback that draws stuff.
* @param[in] data         User data.
* @param[in] from         From page.
* @param[in] to           To Page.
*
* @return Error string or empty string.
*/
static std::string _print(
    const std::string& ps_filename,
    Fl_Paged_Device::Page_Format format,
    Fl_Paged_Device::Page_Layout layout,
    PrintCallback cb,
    void* data,
    int from,
    int to) {

    bool                      cont = true;
    FILE*                     file = nullptr;
    Fl_PostScript_File_Device printer;
    int                       ph;
    int                       pw;
    std::string               res;

    if ((file = fl_fopen(ps_filename.c_str(), "wb")) == nullptr) {
        return "error: could not open file!";
    }

    printer.begin_job(file, 0, format, layout);

    while (cont == true) {
        if (printer.begin_page() != 0) {
            res = "error: couldn't create new page!";
            goto ERR;
        }

        if (printer.printable_rect(&pw, &ph) != 0) {
            res = "error: couldn't retrieve page size!";
            goto ERR;
        }

        fl_push_clip(0, 0, pw, ph);
        cont = cb(data, pw, ph, from);
        fl_pop_clip();

        if (printer.end_page() != 0) {
            res = "error: couldn't end page!";
            goto ERR;
        }

        if (from > 0) {
            from++;

            if (from > to) {
                cont = false;
            }
        }
    }

ERR:
    printer.end_job();
    fclose(file);
    return res;
}

/** @brief Create additional colors.
*
* Colors are reset every time a new theme has been selected
*
* @param[in] dark  True if a dark theme has been requested.
*/
static void _additional_colors(bool dark) {
    color::BEIGE            = fl_rgb_color(245, 245, 220);
    color::CHOCOLATE        = fl_rgb_color(210, 105,  30);
    color::CRIMSON          = fl_rgb_color(220,  20,  60);
    color::DARKOLIVEGREEN   = fl_rgb_color( 85, 107,  47);
    color::DODGERBLUE       = fl_rgb_color( 30, 144, 255);
    color::FORESTGREEN      = fl_rgb_color( 34, 139,  34);
    color::GOLD             = fl_rgb_color(255, 215,   0);
    color::GRAY             = fl_rgb_color(128, 128, 128);
    color::INDIGO           = fl_rgb_color( 75,   0, 130);
    color::OLIVE            = fl_rgb_color(128, 128,   0);
    color::PINK             = fl_rgb_color(255, 192, 203);
    color::ROYALBLUE        = fl_rgb_color( 65, 105, 225);
    color::SIENNA           = fl_rgb_color(160,  82,  45);
    color::SILVER           = fl_rgb_color(192, 192, 192);
    color::SLATEGRAY        = fl_rgb_color(112, 128, 144);
    color::TEAL             = fl_rgb_color(  0, 128, 128);
    color::TURQUOISE        = fl_rgb_color( 64, 224, 208);
    color::VIOLET           = fl_rgb_color(238, 130, 238);

    if (dark == true) {
        color::BEIGE            = fl_darker(color::BEIGE);
        color::CHOCOLATE        = fl_darker(color::CHOCOLATE);
        color::CRIMSON          = fl_darker(color::CRIMSON);
        color::DARKOLIVEGREEN   = fl_darker(color::DARKOLIVEGREEN);
        color::DODGERBLUE       = fl_darker(color::DODGERBLUE);
        color::FORESTGREEN      = fl_darker(color::FORESTGREEN);
        color::GOLD             = fl_darker(color::GOLD);
        color::GRAY             = fl_darker(color::GRAY);
        color::INDIGO           = fl_darker(color::INDIGO);
        color::OLIVE            = fl_darker(color::OLIVE);
        color::PINK             = fl_darker(color::PINK);
        color::ROYALBLUE        = fl_darker(color::ROYALBLUE);
        color::SIENNA           = fl_darker(color::SIENNA);
        color::SILVER           = fl_darker(color::SILVER);
        color::SLATEGRAY        = fl_darker(color::SLATEGRAY);
        color::TEAL             = fl_darker(color::TEAL);
        color::TURQUOISE        = fl_darker(color::TURQUOISE);
        color::VIOLET           = fl_darker(color::VIOLET);
    }
}

/** @brief Set blue colors.
*/
static void _blue_colors() {
    Fl::set_color(0,   228, 228, 228); // FL_FOREGROUND_COLOR
    Fl::set_color(7,    79,  86,  94); // FL_BACKGROUND2_COLOR
    Fl::set_color(8,   108, 113, 125); // FL_INACTIVE_COLOR
    Fl::set_color(15,  241, 196,  126); // FL_SELECTION_COLOR
    Fl::set_color(56,    0,   0,   0); // FL_BLACK
    Fl::background(48, 56, 65);

    unsigned char r = 0;
    unsigned char g = 0;
    unsigned char b = 0;

    for (int f = 32; f < 49; f++) {
        Fl::set_color(f, r, g, b);

        if (f == 32) {
            r = 0;
            g = 9;
            b = 18;
        }
        else {
            r += 2 + (f < 44);
            g += 2 + (f < 44);
            b += 2 + (f < 44);
        }
    }
}

/** @brief Set dark colors.
*/
static void _dark_colors() {
    Fl::set_color(0,   200, 200, 200); // FL_FOREGROUND_COLOR
    Fl::set_color(7,    64,  64,  64); // FL_BACKGROUND2_COLOR
    Fl::set_color(8,   100, 100, 100); // FL_INACTIVE_COLOR
    Fl::set_color(15,  177, 227, 177); // FL_SELECTION_COLOR
    Fl::set_color(56,    0,   0,   0); // FL_BLACK
    Fl::set_color(49, 43, 43, 43);
    Fl::background(43, 43, 43);

    unsigned char c = 0;

    for (int f = 32; f < 49; f++) {
        Fl::set_color(f, c, c, c);
        c += 2;
        if (f > 40) c++;
    }
}

/** @brief Make default colors darker (for dark themes).
*/
static void _make_default_colors_darker() {
    Fl::set_color(FL_GREEN, fl_darker(Fl::get_color(FL_GREEN)));
    Fl::set_color(FL_DARK_GREEN, fl_darker(Fl::get_color(FL_DARK_GREEN)));
    Fl::set_color(FL_RED, fl_darker(Fl::get_color(FL_RED)));
    Fl::set_color(FL_DARK_RED, fl_darker(Fl::get_color(FL_DARK_RED)));
    Fl::set_color(FL_YELLOW, fl_darker(Fl::get_color(FL_YELLOW)));
    Fl::set_color(FL_DARK_YELLOW, fl_darker(Fl::get_color(FL_DARK_YELLOW)));
    Fl::set_color(FL_BLUE, fl_darker(Fl::get_color(FL_BLUE)));
    Fl::set_color(FL_DARK_BLUE, fl_darker(Fl::get_color(FL_DARK_BLUE)));
    Fl::set_color(FL_CYAN, fl_darker(Fl::get_color(FL_CYAN)));
    Fl::set_color(FL_DARK_CYAN, fl_darker(Fl::get_color(FL_DARK_CYAN)));
    Fl::set_color(FL_MAGENTA, fl_darker(Fl::get_color(FL_MAGENTA)));
    Fl::set_color(FL_DARK_MAGENTA, fl_darker(Fl::get_color(FL_DARK_MAGENTA)));
}

/** @brief Restore original colors.
*/
static void _restore_colors() {
    if (_SAVED_COLOR == true) {
        for (int f = 0; f < 256; f++) {
            Fl::set_color(f, priv::_OLD_R[f], priv::_OLD_G[f], priv::_OLD_B[f]);
        }
    }
}

/** @brief Set colors so they can be restored later.
*/
static void _save_colors() {
    if (_SAVED_COLOR == false) {
        for (int f = 0; f < 256; f++) {
            unsigned char r1, g1, b1;
            Fl::get_color(f, r1, g1, b1);
            priv::_OLD_R[f] = r1;
            priv::_OLD_G[f] = g1;
            priv::_OLD_B[f] = b1;
        }

        _SAVED_COLOR = true;
    }
}

/** @brief Set tan colors.
*/
static void _tan_colors() {
    Fl::set_color(0,     0,   0,   0); // FL_FOREGROUND_COLOR
    Fl::set_color(7,   255, 255, 255); // FL_BACKGROUND2_COLOR
    Fl::set_color(8,    85,  85,  85); // FL_INACTIVE_COLOR
    Fl::set_color(15,  188, 114,  50); // FL_SELECTION_COLOR
    Fl::background(206, 202, 187);
}

/** @brief Load default theme.
* @private
*/
void _load_default() {
    priv::_save_colors();
    priv::_restore_colors();
    priv::_additional_colors(false);
    Fl::scheme("none");
    Fl::redraw();
    flw::PREF_THEME = flw::PREF_THEMES[priv::THEME_DEFAULT];
    _IS_DARK = false;
}

/** @brief Load gleam theme.
* @private
*/
void _load_gleam() {
    priv::_save_colors();
    priv::_restore_colors();
    priv::_additional_colors(false);
    Fl::scheme("gleam");
    Fl::redraw();
    flw::PREF_THEME = flw::PREF_THEMES[priv::THEME_GLEAM];
    _IS_DARK = false;
}

/** @brief Load blue gleam theme.
* @private
*/
void _load_gleam_blue() {
    priv::_save_colors();
    priv::_restore_colors();
    priv::_make_default_colors_darker();
    priv::_blue_colors();
    priv::_additional_colors(true);
    Fl::set_color(255, 101, 117, 125);
    Fl::scheme("gleam");
    Fl::redraw();
    flw::PREF_THEME = flw::PREF_THEMES[priv::THEME_GLEAM_BLUE];
    _IS_DARK = true;
}

/** @brief Load dark gleam theme.
* @private
*/
void _load_gleam_dark() {
    priv::_save_colors();
    priv::_restore_colors();
    priv::_make_default_colors_darker();
    priv::_dark_colors();
    priv::_additional_colors(true);
    Fl::set_color(255, 112, 112, 112);
    Fl::scheme("gleam");
    Fl::redraw();
    flw::PREF_THEME = flw::PREF_THEMES[priv::THEME_GLEAM_DARK];
    _IS_DARK = true;
}

/** @brief Load tan gleam theme.
* @private
*/
void _load_gleam_tan() {
    priv::_save_colors();
    priv::_restore_colors();
    priv::_tan_colors();
    priv::_additional_colors(false);
    Fl::scheme("gleam");
    Fl::redraw();
    flw::PREF_THEME = flw::PREF_THEMES[priv::THEME_GLEAM_TAN];
    _IS_DARK = false;
}

/** @brief Load gtk theme.
* @private
*/
void _load_gtk() {
    priv::_save_colors();
    priv::_restore_colors();
    priv::_additional_colors(false);
    Fl::scheme("gtk+");
    Fl::redraw();
    flw::PREF_THEME = flw::PREF_THEMES[priv::THEME_GTK];
    _IS_DARK = false;
}

/** @brief Load blue gtk theme.
* @private
*/
void _load_gtk_blue() {
    priv::_save_colors();
    priv::_restore_colors();
    priv::_make_default_colors_darker();
    priv::_blue_colors();
    priv::_additional_colors(true);
    Fl::set_color(255, 101, 117, 125);
    Fl::scheme("gtk+");
    Fl::redraw();
    flw::PREF_THEME = flw::PREF_THEMES[priv::THEME_GTK_BLUE];
    _IS_DARK = true;
}

/** @brief Load dark gtk theme.
* @private
*/
void _load_gtk_dark() {
    priv::_save_colors();
    priv::_restore_colors();
    priv::_make_default_colors_darker();
    priv::_dark_colors();
    priv::_additional_colors(true);
    Fl::set_color(255, 112, 112, 112);
    Fl::scheme("gtk+");
    Fl::redraw();
    flw::PREF_THEME = flw::PREF_THEMES[priv::THEME_GTK_DARK];
    _IS_DARK = true;
}

/** @brief Load tan gtk theme.
* @private
*/
void _load_gtk_tan() {
    priv::_save_colors();
    priv::_restore_colors();
    priv::_tan_colors();
    priv::_additional_colors(false);
    Fl::scheme("gtk+");
    Fl::redraw();
    flw::PREF_THEME = flw::PREF_THEMES[priv::THEME_GTK_TAN];
    _IS_DARK = false;
}

/** @brief Load oxy theme.
* @private
*/
void _load_oxy() {
    priv::_save_colors();
    priv::_restore_colors();
    priv::_additional_colors(false);
    Fl::scheme("oxy");
    Fl::redraw();
    flw::PREF_THEME = flw::PREF_THEMES[priv::THEME_OXY];
    _IS_DARK = false;
}

/** @brief Load tan oxy theme.
* @private
*/
void _load_oxy_tan() {
    priv::_save_colors();
    priv::_restore_colors();
    priv::_tan_colors();
    priv::_additional_colors(false);
    Fl::scheme("oxy");
    Fl::redraw();
    flw::PREF_THEME = flw::PREF_THEMES[priv::THEME_OXY_TAN];
    _IS_DARK = false;
}

/** @brief Load plastic theme.
* @private
*/
void _load_plastic() {
    priv::_save_colors();
    priv::_restore_colors();
    priv::_additional_colors(false);
    Fl::scheme("plastic");
    Fl::redraw();
    flw::PREF_THEME = flw::PREF_THEMES[priv::THEME_PLASTIC];
    _IS_DARK = false;
}

/** @brief Load tan plastic theme.
* @private
*/
void _load_plastic_tan() {
    priv::_save_colors();
    priv::_restore_colors();
    priv::_tan_colors();
    priv::_additional_colors(false);
    Fl::scheme("plastic");
    Fl::redraw();
    flw::PREF_THEME = flw::PREF_THEMES[priv::THEME_PLASTIC_TAN];
    _IS_DARK = false;
}

/** @brief Set scrollbar size.
* @private
*/
void _scrollbar() {
    if (flw::PREF_FONTSIZE < 12 || flw::PREF_FONTSIZE > 16) {
        auto f = (double) flw::PREF_FONTSIZE / 14.0;
        auto s = (int) (f * priv::_SCROLLSIZE);
        Fl::scrollbar_size(s);
    }
    else if (priv::_SCROLLSIZE > 0) {
        Fl::scrollbar_size(priv::_SCROLLSIZE);
    }
}

} // flw::priv

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

Fl_Color color::BEIGE            = fl_rgb_color(245, 245, 220);
Fl_Color color::CHOCOLATE        = fl_rgb_color(210, 105,  30);
Fl_Color color::CRIMSON          = fl_rgb_color(220,  20,  60);
Fl_Color color::DARKOLIVEGREEN   = fl_rgb_color( 85, 107,  47);
Fl_Color color::DODGERBLUE       = fl_rgb_color( 30, 144, 255);
Fl_Color color::FORESTGREEN      = fl_rgb_color( 34, 139,  34);
Fl_Color color::GOLD             = fl_rgb_color(255, 215,   0);
Fl_Color color::GRAY             = fl_rgb_color(128, 128, 128);
Fl_Color color::INDIGO           = fl_rgb_color( 75,   0, 130);
Fl_Color color::OLIVE            = fl_rgb_color(128, 128,   0);
Fl_Color color::PINK             = fl_rgb_color(255, 192, 203);
Fl_Color color::ROYALBLUE        = fl_rgb_color( 65, 105, 225);
Fl_Color color::SIENNA           = fl_rgb_color(160,  82,  45);
Fl_Color color::SILVER           = fl_rgb_color(192, 192, 192);
Fl_Color color::SLATEGRAY        = fl_rgb_color(112, 128, 144);
Fl_Color color::TEAL             = fl_rgb_color(  0, 128, 128);
Fl_Color color::TURQUOISE        = fl_rgb_color( 64, 224, 208);
Fl_Color color::VIOLET           = fl_rgb_color(238, 130, 238);

/*
 *          _      _
 *         | |    | |
 *       __| | ___| |__  _   _  __ _
 *      / _` |/ _ \ '_ \| | | |/ _` |
 *     | (_| |  __/ |_) | |_| | (_| |
 *      \__,_|\___|_.__/ \__,_|\__, |
 *                              __/ |
 *                             |___/
 */

/** @brief Print widget sizes and labels.
*
* @param[in] widget     Valid widget.
* @param[in] recursive  True to print recursive if input widget is a group widget.
*
*/
void debug::print(const Fl_Widget* widget, bool recursive) {
    std::string indent;
    debug::print(widget, indent, recursive);
}

/** @brief Print widget sizes and labels.
*
* @param[in] widget     Widget pointer, can be NULL.
* @param[in] indent     Indentation string.
* @param[in] recursive  True to print recursive if input widget is a group widget.
*
*/
void debug::print(const Fl_Widget* widget, std::string& indent, bool recursive) {
    if (widget == nullptr) {
        puts("flw::debug::print() => null widget");
    }
    else {
        printf("%sx=%4d, y=%4d, w=%4d, h=%4d, X=%4d, Y=%4d, %c, \"%s\"\n",
            indent.c_str(),
            widget->x(),
            widget->y(),
            widget->w(),
            widget->h(),
            widget->x() + widget->w(),
            widget->y() + widget->h(),
            widget->visible() ? 'V' : 'H',
            widget->label() ? widget->label() : "NULL"
        );
        auto group = widget->as_group();

        if (group != nullptr && recursive == true) {
            indent += "\t";

            for (int f = 0; f < group->children(); f++) {
                debug::print(group->child(f), indent);
            }

            indent.pop_back();
        }
    }

    fflush(stdout);
}

/** @brief Test value and print to stderr for error.
*
* @param[in] val   Test that value is true.
* @param[in] line  Line number.
* @param[in] func  Function name.
*
* @return Return true if ok.
*/
bool debug::test(bool val, int line, const char* func) {
    if (val == false) {
        fprintf(stderr, "error: test failed at line %d in %s\n", line, func);
        fflush(stderr);
        return false;
    }

    return true;
}

/** @brief Test value and print to stderr for error.
*
* @param[in] ref   Reference string, can be NULL.
* @param[in] val   Test string, can be NULL.
* @param[in] line  Line number.
* @param[in] func  Function name.
*
* @return Return true if ok.
*/
bool debug::test(const char* ref, const char* val, int line, const char* func) {
    if (ref == nullptr && val == nullptr) {
        return true;
    }
    else if (ref == nullptr || val == nullptr || strcmp(ref, val) != 0) {
        fprintf(stderr, "error: test failed '%s' != '%s' at line %d in %s\n", ref ? ref : "NULL", val ? val : "NULL", line, func);
        fflush(stderr);
        return false;
    }

    return true;
}

/** @brief Test value and print to stderr for error.
*
* @param[in] ref   Reference number.
* @param[in] val   Test number.
* @param[in] line  Line number.
* @param[in] func  Function name.
*
* @return Return true if ok.
*/
bool debug::test(int64_t ref, int64_t val, int line, const char* func) {
    if (ref != val) {
        fprintf(stderr, "error: test failed '%lld' != '%lld' at line %d in %s\n", (long long int) ref, (long long int) val, line, func);
        fflush(stderr);
        return false;
    }

    return true;
}

/** @brief Test value and print to stderr for error.
*
* @param[in] ref   Reference number.
* @param[in] val   Test number.
* @param[in] diff  Number range to compare as equal.
* @param[in] line  Line number.
* @param[in] func  Function name.
*
* @return Return true if ok.
*/
bool debug::test(double ref, double val, double diff, int line, const char* func) {
    if (fabs(ref - val) > diff) {
        fprintf(stderr, "error: test failed '%f' != '%f' at line %d in %s\n", ref, val, line, func);
        fflush(stderr);
        return false;
    }

    return true;
}

/*
 *      _       _          _
 *     | |     | |        | |
 *     | | __ _| |__   ___| |
 *     | |/ _` | '_ \ / _ \ |
 *     | | (_| | |_) |  __/ |
 *     |_|\__,_|_.__/ \___|_|
 *
 *
 */

std::string label::BROWSE   = "&Browse";
std::string label::CANCEL   = "&Cancel";
std::string label::CLOSE    = "Cl&ose";
std::string label::DEL      = "&Delete";
std::string label::DISCARD  = "&Discard";
std::string label::MONO     = "&Mono font";
std::string label::NO       = "&No";
std::string label::OK       = "&Ok";
std::string label::PRINT    = "&Print";
std::string label::REGULAR  = "&Regular font";
std::string label::SAVE     = "&Save";
std::string label::SAVE_DOT = "&Save...";
std::string label::SELECT   = "&Select";
std::string label::UPDATE   = "&Update";
std::string label::YES      = "&Yes";

/*
 *
 *
 *      _ __ ___   ___ _ __  _   _
 *     | '_ ` _ \ / _ \ '_ \| | | |
 *     | | | | | |  __/ | | | |_| |
 *     |_| |_| |_|\___|_| |_|\__,_|
 *
 *
 */

namespace menu {

/** @brief Find menu item.
*
* Either by searching for label or data.
*
* @param[in] menu  Menu item owner.
* @param[in] text  Menu item label.
* @param[in] v     Menu item data.
*
* @return Menu item or NULL.
*/
static Fl_Menu_Item* _item(Fl_Menu_* menu, const char* text, void* v = nullptr) {
    const Fl_Menu_Item* item;

    if (v == nullptr) {
        assert(menu && text);
        item = menu->find_item(text);
    }
    else {
        item = menu->find_item_with_user_data(v);
    }
#ifdef DEBUG
    if (item == nullptr) fprintf(stderr, "error: cant find menu item <%s>\n", text);
#endif
    return const_cast<Fl_Menu_Item*>(item);
}

} // flw::menu

/** @brief Enable or disable menu item.
*
* Either by searching for label or data.
*
* @param[in] menu   Menu item owner.
* @param[in] text   Menu item label.
* @param[in] value  On or off.
*/
void menu::enable_item(Fl_Menu_* menu, const char* text, bool value) {
    auto item = _item(menu, text);

    if (item == nullptr) {
        return;
    }

    if (value == true) {
        item->activate();
    }
    else {
        item->deactivate();
    }
}

/** @brief Get menu item.
*
* @param[in] menu  Menu item owner.
* @param[in] text  Menu item label.
*
* @return Item or NULL.
*/
Fl_Menu_Item* menu::get_item(Fl_Menu_* menu, const char* text) {
    return _item(menu, text);
}

/** @brief Get menu item.
*
* @param[in] menu  Menu item owner.
* @param[in] v     Menu item data.
*
* @return Item or NULL.
*/
Fl_Menu_Item* menu::get_item(Fl_Menu_* menu, void* v) {
    return _item(menu, nullptr, v);
}

/** @brief Get checked menu value.
*
* @param[in] menu  Menu item owner.
* @param[in] text  Menu item label.
*
* @return True or false.
*/
bool menu::item_value(Fl_Menu_* menu, const char* text) {
    auto item = _item(menu, text);

    if (item == nullptr) {
        return false;
    }

    return item->value();
}

/** @brief Set checked menu value.
*
* @param[in] menu   Menu item owner.
* @param[in] text   Menu item label.
* @param[in] value  On or off.
*/
void menu::set_item(Fl_Menu_* menu, const char* text, bool value) {
    auto item = _item(menu, text);

    if (item == nullptr) {
        return;
    }

    if (value == true) {
        item->set();
    }
    else {
        item->clear();
    }
}

/** @brief Turn on item for one and of for all other in a group.
*
* @param[in] menu   Menu item owner.
* @param[in] text   Menu item label.
*/
void menu::setonly_item(Fl_Menu_* menu, const char* text) {
    auto item = _item(menu, text);

    if (item == nullptr) {
        return;
    }

    menu->setonly(item);
}

/*
 *            _   _ _
 *           | | (_) |
 *      _   _| |_ _| |
 *     | | | | __| | |
 *     | |_| | |_| | |
 *      \__,_|\__|_|_|
 *
 *
 */

/** @brief Center window on screen or parent.
*
* @param[in] window  Valid window.
* @param[in] parent  Valid parent or NULL.
*/
void util::center_window(Fl_Window* window, Fl_Window* parent) {
    if (parent != nullptr) {
        int x = parent->x() + parent->w() / 2;
        int y = parent->y() + parent->h() / 2;

        window->resize(x - window->w() / 2, y - window->h() / 2, window->w(), window->h());
    }
    else {
        window->resize((Fl::w() / 2) - (window->w() / 2), (Fl::h() / 2) - (window->h() / 2), window->w(), window->h());
    }
}

/** @brief Return time as seconds since 1970.
*
* @return Seconds as a double number.
*/
//
double util::clock() {
#ifdef _WIN32
    struct timeb timeVal;
    ftime(&timeVal);
    return (double) timeVal.time + (double) (timeVal.millitm / 1000.0);
#else
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return (double) (ts.tv_sec) + (ts.tv_nsec / 1000000000.0);
#endif
}

/** @brief Try to count decimals in a number
*
* @param[in] num  Number value.
*
* @return 0 - 8?.
*/
int util::count_decimals(double num) {
    num = fabs(num);

    if (num > 999'999'999'999'999) {
        return 0;
    }

    int    res     = 0;
    char*  end     = 0;
    double inum = static_cast<int64_t>(num);
    double fnum = num - inum;
    char   buffer[400];

    if (num > 9'999'999'999'999) {
        snprintf(buffer, 400, "%.1f", fnum);
    }
    else if (num > 999'999'999'999) {
        snprintf(buffer, 400, "%.2f", fnum);
    }
    else if (num > 99'999'999'999) {
        snprintf(buffer, 400, "%.3f", fnum);
    }
    else if (num > 9'999'999'999) {
        snprintf(buffer, 400, "%.4f", fnum);
    }
    else if (num > 999'999'999) {
        snprintf(buffer, 400, "%.5f", fnum);
    }
    else if (num > 99'999'999) {
        snprintf(buffer, 400, "%.6f", fnum);
    }
    else if (num > 9'999'999) {
        snprintf(buffer, 400, "%.7f", fnum);
    }
    else {
        snprintf(buffer, 400, "%.8f", fnum);
    }

    size_t len = strlen(buffer);
    end = buffer + len - 1;

    while (*end == '0') {
        *end = 0;
        end--;
    }

    res = strlen(buffer) - 2;

    return res;
}

/** @brief Find widget by using the label.
*
* Widgets are searched recursive.\n
* First found widget label is returned.\n
*
* @param[in] group  Group widget.
* @param[in] label  Child label.
*
* @return Widget or NULL.
*/
Fl_Widget* util::find_widget(Fl_Group* group, const std::string& label) {
    for (int f = 0; f < group->children(); f++) {
        auto w = group->child(f);

        if (w->label() != nullptr && label == w->label()) {
            return w;
        }
    }

    for (int f = 0; f < group->children(); f++) {
        auto w = group->child(f);
        auto g = w->as_group();

        if (g != nullptr) {
            w = util::find_widget(g, label);

            if (w != nullptr) {
                return w;
            }
        }
    }

    return nullptr;
}

/** @brief Escape '\', '_', '/', '&' characters.
*
* @param[in] label  Input label.
*
* @return Escaped input string.
*/
std::string util::fix_menu_string(const std::string& label) {
    std::string res = label;

    util::replace_string(res, "\\", "\\\\");
    util::replace_string(res, "_", "\\_");
    util::replace_string(res, "/", "\\/");
    util::replace_string(res, "&", "&&");

    return res;
}

/** @brief Format a string.
*
* @param[in] format  Valid format string, see sprintf() for formatting values.
* @param[in] ...     Additional arguments.
*
* @return Formatted string or empty for any error.
*/
std::string util::format(const std::string& format, ...) {
    if (format == "") {
        return "";
    }

    int         l   = 128;
    int         n   = 0;
    char*       buf = static_cast<char*>(calloc(l, 1));
    std::string res;
    va_list     args;

    va_start(args, format);
    n = vsnprintf(buf, l, format.c_str(), args);
    va_end(args);

    if (n < 0) {
        free(buf);

        return res;
    }

    if (n < l) {
        res = buf;
        free(buf);

        return res;
    }

    free(buf);
    l = n + 1;
    buf = static_cast<char*>(calloc(l, 1));
    if (buf == nullptr) return res;

    va_start(args, format);
    vsnprintf(buf, l, format.c_str(), args);
    va_end(args);
    res = buf;
    free(buf);

    return res;
}

/** @brief Format a decimal number by inserting a thousand delimiter.
*
* Only none decimal numbers are separated.
*
* @param[in] num       Number to format.
* @param[in] decimals  Number of decimals, use -1 to try to count decimals.
* @param[in] del       Thousand separator, default space.
*
* @return Formatted number string.
*/
std::string util::format_double(double num, int decimals, char del) {
    char fr_str[100];

    if (num > 9'007'199'254'740'992.0) {
        return "";
    }

    if (decimals < 0) {
        decimals = util::count_decimals(num);
    }

    if (del < 32) {
        del = 32;
    }

    if (decimals == 0 || decimals > 9) {
        return util::format_int(static_cast<int64_t>(num), del);
    }

    auto fabs_num   = fabs(num + 0.00000001);
    auto int_num    = static_cast<int64_t>(fabs_num);
    auto double_num = static_cast<double>(fabs_num - int_num);

    if (double_num < 0.0000001) {
        double_num = 0.0;
    }

    auto res = util::format_int(int_num, del);
    auto n   = snprintf(fr_str, 100, "%.*f", decimals, double_num);

    if (num < 0.0) {
        res = "-" + res;
    }

    if (n > 0 && n < 100) {
        res += fr_str + 1;
    }

    return res;
}

/** @brief Format an integer by inserting a thousand delimiter.
*
* @param[in] num  Integer to format.
* @param[in] del  Thousand separator, default space.
*
* @return Formatted number string.
*/
std::string util::format_int(int64_t num, char del) {
    auto pos = 0;
    char tmp1[32];
    char tmp2[32];

    if (del < 32) {
        del = 32;
    }
    memset(tmp2, 0, 32);
    snprintf(tmp1, 32, "%lld", (long long int) num);
    auto len = strlen(tmp1);

    for (int f = len - 1, i = 0; f >= 0 && pos < 32; f--, i++) {
        char c = tmp1[f];

        if ((i % 3) == 0 && i > 0 && c != '-') {
            tmp2[pos++] = del;
        }

        tmp2[pos++] = c;
    }

    std::string r = tmp2;
    std::reverse(r.begin(), r.end());
    return r;
}

/** @brief Set icon for widget.
*
* Widget manages image memory and will delete it.\n
*
* @param[in] widget     Widget to set icon for.
* @param[in] svg_image  Valid svg image.
* @param[in] max_size   Max image size (from 16 - 4096).
*
* @return True if image was set.
*
* @image html icons.png
*/
bool util::icon(Fl_Widget* widget, const std::string& svg_image, unsigned max_size) {
    auto svg = (svg_image.length() > 40) ? new Fl_SVG_Image(nullptr, svg_image.c_str()) : nullptr;

    if (svg == nullptr) {
        return false;
    }
    else if (max_size < 16 || max_size > 4096) {
        delete svg;
        return false;
    }

    auto image   = svg->copy();
    auto deimage = image->copy();

    image->scale(max_size, max_size);
    deimage->inactive();
    deimage->scale(max_size, max_size);
    widget->bind_image(image);
    widget->bind_deimage(deimage);
    widget->bind_image(1);
    widget->bind_deimage(1);
    delete svg;

    return true;
}

/** @brief Check if string contains any letter but not control character.
*
* If the string contains at least one control character (1 - 31) it will always return true.\n
* If the string is empty or only whitespaces it will return true.\n
* If the string has whitespaces but at least one letter it will return false.\n
*
* @param[in] string  Input string.
*
* @return True for valid string.
*/
bool util::is_empty(const std::string& string) {
    bool ctrl   = false;
    bool space  = false;
    bool letter = false;

    for (auto c : string) {
        if (c > 0 && c < 32) {
            ctrl = true;
        }
        else if (c == 32) {
            space = true;
        }
        else {
            letter = true;
        }
    }

    if (ctrl == true) {
        return true;
    }
    else if (space == true && letter == false) {
        return true;
    }
    else if (letter == true) {
        return false;
    }
    else {
        return true;
    }
}

/** @brief Set label font properties for a widget.
*
* If it is an group widget it will set properties for all child widgets also (recursive).
*
* @param[in] widget  Valid widget.
* @param[in] font    Font to use.
* @param[in] size    Font size.
*/
void util::labelfont(Fl_Widget* widget, Fl_Font font, int size) {
    widget->labelfont(font);
    widget->labelsize(size);

    auto group = widget->as_group();

    if (group != nullptr) {
        for (auto f = 0; f < group->children(); f++) {
            util::labelfont(group->child(f), font, size);
        }
    }
}

/** @brief Return time stamp.
*
* @return Time in microseconds.
*/
int64_t util::microseconds() {
#if defined(_WIN32)
    LARGE_INTEGER StartingTime;
    LARGE_INTEGER Frequency;

    QueryPerformanceFrequency(&Frequency);
    QueryPerformanceCounter(&StartingTime);

    StartingTime.QuadPart *= 1000000;
    StartingTime.QuadPart /= Frequency.QuadPart;
    return StartingTime.QuadPart;
#else
    timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return (t.tv_sec * 1000000 + t.tv_nsec / 1000);
#endif
}

/** @brief Return time stamp.
*
* @return Time in milliseconds.
*/
int32_t util::milliseconds() {
#if defined(_WIN32)
    LARGE_INTEGER StartingTime;
    LARGE_INTEGER Frequency;

    QueryPerformanceFrequency(&Frequency);
    QueryPerformanceCounter(&StartingTime);

    StartingTime.QuadPart *= 1000000;
    StartingTime.QuadPart /= Frequency.QuadPart;
    return StartingTime.QuadPart / 1000;
#else
    timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return (t.tv_sec * 1000000 + t.tv_nsec / 1000) / 1000;
#endif
}

/** @brief
*
* Must be compiled with FLW_USE_PNG flag and linked with fltk images (fltk-config --ldflags --use-images).\n
* If filename is empty, user will be asked for the name.\n
* If coordinates are 0 it will use all widget.\n
* Might look fuzzy if FLTK scaling is turned on.\
*
* @param[in] window    Window to save.
* @param[in] opt_name  Optional filename.
* @param[in] X         X pos in window.
* @param[in] Y         X pos in window.
* @param[in] W         Window width.
* @param[in] H         Window height.
*
* @return True if an image has been saved.
*/
bool util::png_save(Fl_Window* window, const std::string& opt_name, int X, int Y, int W, int H) {
    auto res = false;

#ifdef FLW_USE_PNG
    auto filename = (opt_name == "") ? fl_file_chooser("Save To PNG File", "All Files (*)\tPNG Files (*.png)", "") : opt_name.c_str();

    if (filename != nullptr) {
        window->make_current();

        if (X == 0 && Y == 0 && W == 0 && H == 0) {
            X = window->x();
            Y = window->y();
            W = window->w();
            H = window->h();
        }

        auto image = fl_read_image(nullptr, X, Y, W, H);

        if (image != nullptr) {
            auto ret = fl_write_png(filename, image, W, H);

            if (ret == 0) {
                res = true;
            }
            else if (ret == -1) {
                fl_alert("%s", "Error: missing libraries!");
            }
            else if (ret == -2) {
                fl_alert("Error: failed to save image to %s!", filename);
            }

            delete []image;
        }
        else {
            fl_alert("%s", "Error: failed to grab image!");
        }
    }
#else
    (void) opt_name;
    (void) window;
    (void) X;
    (void) Y;
    (void) W;
    (void) H;
    fl_alert("Error: flw has not been compiled with FLW_USE_PNG flag!");
#endif

    return res;
}

/** @brief Print user drawing to postscript file.
*
* Callback must return false to abort printing.
*
* @param[in] ps_filename  Filename.
* @param[in] format       Page format.
* @param[in] layout       Layout.
* @param[in] cb           User callback that draws stuff.
* @param[in] data         User data.
*
* @return Error string or empty string.
*/
std::string util::print(const std::string& ps_filename, Fl_Paged_Device::Page_Format format, Fl_Paged_Device::Page_Layout layout, PrintCallback cb, void* data) {
    return priv::_print(ps_filename, format, layout, cb, data, 0, 0);
}

/** @brief Print user drawing to postscript file.
*
* All pages between from and to will be printed but if callback returns false it will be stopped.
*
* @param[in] ps_filename  Filename.
* @param[in] format       Page format.
* @param[in] layout       Layout.
* @param[in] cb           User callback that draws stuff.
* @param[in] data         User data.
* @param[in] from         From page.
* @param[in] to           To Page.
*
* @return Error string or empty string.
*/
std::string util::print(const std::string& ps_filename, Fl_Paged_Device::Page_Format format, Fl_Paged_Device::Page_Layout layout, PrintCallback cb, void* data, int from, int to) {
    if (from < 1 || from > to) {
        return "error: invalid from/to range";
    }

    return priv::_print(ps_filename, format, layout, cb, data, from, to);
}

/** @brief Remove formatting from a browser text line.
*
* Read Fl_Browser::format_char() for browser formatting.\n
* Returns an empty string if exception has been thrown.\n
*
* @param[in] text  Input text.
*
* @return String without control characters.
*/
std::string util::remove_browser_format(const std::string& text) {
    auto res = text;
    auto f   = res.find_last_of("@");

    if (f != std::string::npos) {
        try {
            auto tmp = res.substr(f + 1);

            if (tmp[0] == '.' || tmp[0] == 'l' || tmp[0] == 'm' || tmp[0] == 's' || tmp[0] == 'b' || tmp[0] == 'i' || tmp[0] == 'f' || tmp[0] == 'c' || tmp[0] == 'r' || tmp[0] == 'u' || tmp[0] == '-') {
                res = tmp.substr(1);
            }
            else if (tmp[0] == 'B' || tmp[0] == 'C' || tmp[0] == 'F' || tmp[0] == 'S') {
                auto s = std::string();
                auto e = false;

                tmp = tmp.substr(f + 1);

                for (auto c : tmp) {
                    if (e == false && c >= '0' && c <= '9') {
                    }
                    else {
                        e = true;
                        s += c;
                    }
                }

                res = s;
            }
            else {
                res = res.substr(f);
            }
        }
        catch (...) {
            res = "";
        }
    }

    return res;
}

/** @brief Replace all found strings in input string.
*
* Returns an empty string if exception has been thrown.
*
* @param[in,out] string   Input string.
* @param[in]     find     Find string.
* @param[in]     replace  Replace string.
*
* @return Input string or empty for error.
*/
std::string& util::replace_string(std::string& string, const std::string& find, const std::string& replace) {
    if (find == "") {
        return string;
    }

    try {
        auto res   = std::string();
        auto start = static_cast<size_t>(0);
        auto pos   = static_cast<size_t>(0);

        res.reserve(string.length() + (replace.length() > find.length() ? string.length() * 0.1 : 0));

        while ((pos = string.find(find, start)) != std::string::npos) {
            res   += string.substr(start, pos - start);
            res   += replace;
            pos   += find.length();
            start  = pos;
        }

        res += string.substr(start);
        string.swap(res);
    }
    catch(...) {
        string = "";
    }

    return string;
}

/** @brief Sleep for a while.
*
* @param[in] milli  Number of milliseconds.
*/
void util::sleep(unsigned milli) {
#ifdef _WIN32
    Sleep(milli);
#else
    usleep(milli * 1000);
#endif
}

/** @brief Split a string and return an vector with strings.
*
* @param[in] string  Input string.
* @param[in] split   Split input string on this string.
*
* @return Result vector, empty for error.
*/
flw::StringVector util::split_string(const std::string& string, const std::string& split) {
    auto res = StringVector();

    try {
        if (split != "") {
            auto pos1 = (std::string::size_type) 0;
            auto pos2 = string.find(split);

            while (pos2 != std::string::npos) {
                res.push_back(string.substr(pos1, pos2 - pos1));
                pos1 = pos2 + split.size();
                pos2 = string.find(split, pos1);
            }

            if (pos1 <= string.size()) {
                res.push_back(string.substr(pos1));
            }
        }
    }
    catch(...) {
        res.clear();
    }

    return res;
}

/** @brief Return sub string.
*
* Returns an empty string if exception has been thrown.
*
* @param[in] string  Input string.
* @param[in] pos     Start position.
* @param[in] size    Number of bytes, std::string::npos for all bytes form pos.
*
* @return Sub string.
*/
std::string util::substr(const std::string& string, std::string::size_type pos, std::string::size_type size) {
    try {
        return string.substr(pos, size);
    }
    catch(...) {
        return "";
    }
}

/** @brief Trim whitespace from both ends of string
*
* @param[in,out] string  Input string.
*
* @return Trimmed input string.
*/
std::string util::trim(const std::string& string) {
    auto res = string;
    
    try {
        res.erase(res.begin(), std::find_if(res.begin(), res.end(), [](auto c) { return !std::isspace(c);} ));
        res.erase(std::find_if(res.rbegin(), res.rend(), [](int ch) { return !std::isspace(ch); }).base(), res.end());
        return res;
    }
    catch(...) {
        return string;
    }
}

/** @brief Swap rectangles.
*
* @param[in,out] w1  Widget 1.
* @param[in,out] w2  Widget 2.
*/
void util::swap_rect(Fl_Widget* w1, Fl_Widget* w2) {
    auto r1 = Fl_Rect(w1);
    auto r2 = Fl_Rect(w2);

    w1->resize(r2.x(), r2.y(), r2.w(), r2.h());
    w2->resize(r1.x(), r1.y(), r1.w(), r1.h());
}

/** @brief Convert string to number.
*
* @param[in] string  String to convert.
*
* @return Converted number or INFINITY.
*/
double util::to_double(const std::string& string) {
    try {
        return std::stod(string);
    }
    catch(...) {
        return INFINITY;
    }
}

/** @brief Convert string to an vector of double numbers.
*
* @param[in]  string   String to convert.
* @param[out] numbers  Result vector.
* @param[in]  size     Size of result vector.
*
* @return Number of converted numbers.
*/
size_t util::to_doubles(const std::string& string, double numbers[], size_t size) {
    auto end = (char*) nullptr;
    auto in  = string.c_str();
    auto f   = (size_t) 0;

    errno = 0;

    for (; f < size; f++) {
        numbers[f] = strtod(in, &end);

        if (errno != 0 || in == end) {
            return f;
        }

        in = end;
    }

    return f;
}

/** @brief Convert string to integer.
*
* @param[in] string  String to convert.
* @param[in] def     Default number if exception is thrown.
*
* @return Converted integer or default integer.
*/
int64_t util::to_int(const std::string& string, int64_t def) {
    try {
        return static_cast<int64_t>(std::stoll(string));
    }
    catch(...) {
        return def;
    }
}

/** @brief Convert string to integer.
*
* @param[in] string  String to convert.
* @param[in] def     Default number if exception is thrown.
*
* @return Converted integer or default integer.
*/
long long util::to_long(const std::string& string, long long def) {
    try {
        return std::stoll(string);
    }
    catch(...) {
        return def;
    }
}

/** @brief Zero buffer bytes.
*
* @param[in,out]   mem  Memory to zero.
* @param[in] size  Number of bytes.
*
* @return Input pointer.
*/
void* util::zero_memory(char* mem, size_t size) {
    if (mem == nullptr || size == 0) return mem;
#ifdef _WIN32
    RtlSecureZeroMemory(mem, size);
#else
    auto p = reinterpret_cast<volatile unsigned char*>(mem);

    while (size--) {
        *p = 0;
        p++;
    }
#endif

    return mem;
}

/*
 *      _____      _       _ _______        _
 *     |  __ \    (_)     | |__   __|      | |
 *     | |__) | __ _ _ __ | |_ | | _____  _| |_
 *     |  ___/ '__| | '_ \| __|| |/ _ \ \/ / __|
 *     | |   | |  | | | | | |_ | |  __/>  <| |_
 *     |_|   |_|  |_|_| |_|\__||_|\___/_/\_\\__|
 *
 *
 */

/** @brief Create object.
*
* Line numbers are only used with FL_ALIGN_LEFT.\n
*
* @param[in] filename     Postscript filename.
* @param[in] page_format  Page format.
* @param[in] page_layout  Page layout.
* @param[in] font         Text font.
* @param[in] fontsize     Text size.
* @param[in] align        Alignment for the lines.
* @param[in] wrap         Wrap text or let it be cut if it is to long.
* @param[in] border       Draw a border around page.
* @param[in] line_num     Width of line numbers, 0 to disable (0 - 8).
*/
util::PrintText::PrintText(const std::string& filename,
    Fl_Paged_Device::Page_Format page_format,
    Fl_Paged_Device::Page_Layout page_layout,
    Fl_Font font,
    Fl_Fontsize fontsize,
    Fl_Align align,
    bool wrap,
    bool border,
    int line_num) {

    _align       = FL_ALIGN_INSIDE | FL_ALIGN_TOP;
    _align      |= (align == FL_ALIGN_CENTER || align == FL_ALIGN_RIGHT) ? align : FL_ALIGN_LEFT;
    _border      = border;
    _file        = nullptr;
    _filename    = filename;
    _font        = font;
    _fontsize    = fontsize;
    _line_num    = (align == FL_ALIGN_LEFT && line_num > 0 && line_num < 9) ? line_num : 0;
    _page_format = page_format;
    _page_layout = page_layout;
    _printer     = nullptr;
    _wrap        = wrap;
}

/** @brief Stop printing.
*
*/
util::PrintText::~PrintText() {
    _stop();
}

/** @brief Check and create new page if needed.
*
*/
void util::PrintText::_check_for_new_page() {
    if (_py + _lh > _ph || _page_count == 0) {
        if (_page_count > 0) {
            fl_pop_clip();

            if (_printer->end_page() != 0) {
                throw "error: couldn't end current page";
            }
        }

        if (_printer->begin_page() != 0) {
            throw "error: couldn't create new page!";
        }

        if (_printer->printable_rect(&_pw, &_ph) != 0) {
            throw "error: couldn't retrieve page size!";
        }

        fl_font(_font, _fontsize);
        fl_color(FL_BLACK);
        fl_line_style(FL_SOLID, 1);
        fl_push_clip(0, 0, _pw, _ph);

        if (_border == false) {
            _px = 0;
            _py = 0;
        }
        else {
            fl_rect(0, 0, _pw, _ph);
            _measure_lw_lh("M");

            _px  = _lw;
            _py  = _lh;
            _pw -= _lw * 2;
            _ph -= _lh * 2;
        }

        _page_count++;
    }
}

/** @brief Measure width of text and height of text.
*
* @param[in] text  Line of text.
*/
void util::PrintText::_measure_lw_lh(const std::string& text) {
    _lw = _lh = 0;
    fl_measure(text.c_str(), _lw, _lh, 0);
}

/** @brief Print text to file.
*
* @param[in] text                    Text to print, use only \n for newlines.
* @param[in] replace_tab_with_space  Replace spaces with tab (1 - 16) or 0 to print as it is.
*
* @return Error string or empty string.
*/
std::string util::PrintText::print(const char* text, unsigned replace_tab_with_space) {
    return print(util::split_string(text, "\n"), replace_tab_with_space);
}

/** @brief Print text to file.
*
* @param[in] text                    Text to print, use only \n for newlines.
* @param[in] replace_tab_with_space  Replace spaces with tab (1 - 16) or 0 to print as it is.
*
* @return Error string or empty string.
*/
std::string util::PrintText::print(const std::string& text, unsigned replace_tab_with_space) {
    return print(util::split_string(text.c_str(), "\n"), replace_tab_with_space);
}

/** @brief Print text to file.
*
* @param[in] lines                   Text lines to print.
* @param[in] replace_tab_with_space  Replace spaces with tab (1 - 16) or 0 to print as it is.
*
* @return Error string or empty string.
*/
std::string util::PrintText::print(const StringVector& lines, unsigned replace_tab_with_space) {
    std::string res;
    std::string tab;

    while (replace_tab_with_space > 0 && replace_tab_with_space <= 16) {
        tab += " ";
        replace_tab_with_space--;
    }

    try {
        auto wc = WaitCursor();

        res = _start();

        if (res == "") {
            for (auto& line : lines) {
                if (tab != "") {
                    auto l = line;
                    util::replace_string(l, "\t", "    ");
                    _print_line(l == "" ? " " : l);
                }
                else {
                    _print_line(line == "" ? " " : line);
                }
            }

            res = _stop();
        }
    }
    catch (const char* ex) {
        res = ex;
    }
    catch (...) {
        res = "error: unknown exception!";
    }

    return res;
}

/** @brief Print one line.
*
* @param[in] line  Text line.
*/
void util::PrintText::_print_line(const std::string& line) {
    _line_count++;
    _check_for_new_page();

    if (_line_num > 0) {
        auto num = util::format("%*d: ", _line_num, _line_count);
        _measure_lw_lh(num);
        fl_draw(num.c_str(), _px, _py, _pw, _lh, _align, nullptr, 0);
        _nw = _lw;
    }

    _measure_lw_lh(line);

    if (_wrap == true && _lw > _pw - _nw) {
        _print_wrapped_line(line);
    }
    else {
        fl_draw(line.c_str(), _px + _nw, _py, _pw - _nw, _lh, _align, nullptr, 0);
        _py += _lh;
    }
}

/** @brief Print wrapped line.
*
* @param[in] line  Text line.
*/
void util::PrintText::_print_wrapped_line(const std::string& line) {
    auto p1 = line.c_str();
    auto s1 = std::string();
    auto s2 = std::string();

    while (*p1 != 0) {
        auto cl = fl_wcwidth(p1);

        if (cl > 1) { // Add utf8 character.
            for (auto f = 0; f < cl && *p1 != 0; f++) {
                s1 += *p1;
                p1++;
            }
        }
        else {
            s1 += *p1;
            p1++;
        }

        auto c = s1.back();

        if (c == ' ' || c == '\t' || c == ',' || c == ';' || c == '.') { // Save word break.
            s2 = s1;
        }

        _measure_lw_lh(s1);

        if (_lw >= _pw - _nw) {
            _check_for_new_page();

            if (s2 != "") { // Last word delimiter string, s1 will contain the leftover.
                fl_draw(s2.c_str(), _px + _nw, _py, _pw - _nw, _lh, _align, nullptr, 0);
                s1 = (s2.length() < s1.length()) ? s1.substr(s2.length()) : "";
                s2 = "";
            }
            else {
                std::string s;

                if (s1.length() > 1) { // Remove last char to be sure that it will be inside area.
                    s  = s1.substr(s1.length() - 1);
                    s1 = s1.substr(0, s1.length() - 1);
                }

                fl_draw(s1.c_str(), _px + _nw, _py, _pw - _nw, _lh, _align, nullptr, 0);
                s1 = s;
            }

            _py += _lh;
        }
    }

    if (s1 != "") { // Leftover string.
        _check_for_new_page();
        fl_draw(s1.c_str(), _px + _nw, _py, _pw - _nw, _lh, _align, nullptr, 0);
        _py += _lh;
    }
}

/** @brief Create printer object.
*
*/
std::string util::PrintText::_start() {
    if ((_file = fl_fopen(_filename.c_str(), "wb")) == nullptr) {
        return "error: could not open file!";
    }

    _lh         = 0;
    _line_count = 0;
    _lw         = 0;
    _nw         = 0;
    _page_count = 0;
    _ph         = 0;
    _pw         = 0;
    _px         = 0;
    _py         = 0;
    _printer    = new Fl_PostScript_File_Device();

    _printer->begin_job(_file, 0, _page_format, _page_layout);

    return "";
}

/** @brief End job and delete printer.
*
* @return Error string or empty string.
*/
std::string util::PrintText::_stop() {
    std::string res = "";

    if (_printer != nullptr) {
        if (_page_count > 0) {
            fl_pop_clip();

            if (_printer->end_page() != 0) {
                res = "error: could not end page!";
            }
        }

        _printer->end_job();
        delete _printer;
        fclose(_file);

        _file    = nullptr;
        _printer = nullptr;
    }

    return res;
}

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

namespace theme {

/** @brief Check if a dark theme has been loaded
*
* @return True for dark theme.
*/
bool is_dark() {
    if (flw::PREF_THEME == flw::PREF_THEMES[priv::THEME_GLEAM_BLUE] ||
        flw::PREF_THEME == flw::PREF_THEMES[priv::THEME_GLEAM_DARK] ||
        flw::PREF_THEME == flw::PREF_THEMES[priv::THEME_GTK_BLUE] ||
        flw::PREF_THEME == flw::PREF_THEMES[priv::THEME_GTK_DARK]) {
        return true;
    }
    else {
        return false;
    }
}

/** @brief Load a theme.
*
* Valid theme names are:\n
*   "default"\n
*    "gleam"\n
*    "blue gleam"\n
*    "blue_gleam"\n
*    "dark gleam"\n
*    "dark_gleam"\n
*    "tan gleam"\n
*    "tan_gleam"\n
*    "gtk"\n
*    "blue gtk"\n
*    "blue_gtk"\n
*    "dark gtk"\n
*    "dark_gtk"\n
*    "tan gtk"\n
*    "tan_gtk"\n
*    "oxy"\n
*    "tan oxy"\n
*    "tan_oxy"\n
*    "plastic"\n
*    "tan plastic"\n
*    "tan_plastic"\n

* @param[in] name  Theme name.
*
* @return True if theme has been loaded.
*/
bool load(const std::string& name) {
    if (priv::_SCROLLSIZE == 0) {
        priv::_SCROLLSIZE = Fl::scrollbar_size();
    }

    if (name == flw::PREF_THEMES[priv::THEME_DEFAULT] || name == flw::PREF_THEMES2[priv::THEME_DEFAULT]) {
        priv::_load_default();
    }
    else if (name == flw::PREF_THEMES[priv::THEME_GLEAM] || name == flw::PREF_THEMES2[priv::THEME_GLEAM]) {
        priv::_load_gleam();
    }
    else if (name == flw::PREF_THEMES[priv::THEME_GLEAM_BLUE] || name == flw::PREF_THEMES2[priv::THEME_GLEAM_BLUE]) {
        priv::_load_gleam_blue();
    }
    else if (name == flw::PREF_THEMES[priv::THEME_GLEAM_DARK] || name == flw::PREF_THEMES2[priv::THEME_GLEAM_DARK]) {
        priv::_load_gleam_dark();
    }
    else if (name == flw::PREF_THEMES[priv::THEME_GLEAM_TAN] || name == flw::PREF_THEMES2[priv::THEME_GLEAM_TAN]) {
        priv::_load_gleam_tan();
    }
    else if (name == flw::PREF_THEMES[priv::THEME_GTK] || name == flw::PREF_THEMES2[priv::THEME_GTK]) {
        priv::_load_gtk();
    }
    else if (name == flw::PREF_THEMES[priv::THEME_GTK_BLUE] || name == flw::PREF_THEMES2[priv::THEME_GTK_BLUE]) {
        priv::_load_gtk_blue();
    }
    else if (name == flw::PREF_THEMES[priv::THEME_GTK_DARK] || name == flw::PREF_THEMES2[priv::THEME_GTK_DARK]) {
        priv::_load_gtk_dark();
    }
    else if (name == flw::PREF_THEMES[priv::THEME_GTK_TAN] || name == flw::PREF_THEMES2[priv::THEME_GTK_TAN]) {
        priv::_load_gtk_tan();
    }
    else if (name == flw::PREF_THEMES[priv::THEME_OXY] || name == flw::PREF_THEMES2[priv::THEME_OXY]) {
        priv::_load_oxy();
    }
    else if (name == flw::PREF_THEMES[priv::THEME_OXY_TAN] || name == flw::PREF_THEMES2[priv::THEME_OXY_TAN]) {
        priv::_load_oxy_tan();
    }
    else if (name == flw::PREF_THEMES[priv::THEME_PLASTIC] || name == flw::PREF_THEMES2[priv::THEME_PLASTIC]) {
        priv::_load_plastic();
    }
    else if (name == flw::PREF_THEMES[priv::THEME_PLASTIC_TAN] || name == flw::PREF_THEMES2[priv::THEME_PLASTIC_TAN]) {
        priv::_load_plastic_tan();
    }
    else {
        return false;
    }

    priv::_scrollbar();
    return true;
}

/** @brief Get font value.
*
* First time it is called it will load all available fonts from the system.
*
* @param[in] requested_font  Font name.
*
* @return Found font value or -1.
*/
Fl_Font load_font(const std::string& requested_font) {
    load_fonts();

    auto count = 0;

    for (auto font : flw::PREF_FONTNAMES) {
        auto font2 = util::remove_browser_format(font);

        if (requested_font == font2) {
            return count;
        }

        count++;
    }

    return -1;
}

/** @brief Load fonts to flw font list.
*
* But do it only the first time.
*
* @param[in] iso8859_only  True to only use fonts with ISO8859-1 character set.
*/
void load_fonts(bool iso8859_only) {
    if (flw::PREF_FONTNAMES.size() == 0) {
        auto fonts = Fl::set_fonts((iso8859_only == true) ? nullptr : "-*");

        for (int f = 0; f < fonts; f++) {
            auto attr  = 0;
            auto name1 = Fl::get_font_name(static_cast<Fl_Font>(f), &attr);
            auto name2 = std::string();

            if (attr & FL_BOLD) {
                name2 = std::string("@b");
            }

            if (attr & FL_ITALIC) {
                name2 += std::string("@i");
            }

            name2 += std::string("@.");
            name2 += name1;
            flw::PREF_FONTNAMES.push_back(strdup(name2.c_str()));
            //printf("%3d - %-40s - %-40s\n", f, name1, name2.c_str()); fflush(stdout);
        }
    }
}

/** @brief Load icon before showing window.
*
* Use only windows or linux arguments.\n
* Does not work running in wayland mode.\n
*
* @param[in] win           Window object.
* @param[in] win_resource  Win32 resource (Windows 10/11 only).
* @param[in] xpm_resource  Xpm icon (Linux only).
* @param[in] name          Application name (Linux only).
*/
void load_icon(Fl_Window* win, int win_resource, const char** xpm_resource, const char* name) {
    assert(win);

    if (win->shown() != 0) {
        fl_alert("%s", "Warning: load icon before showing window!");
    }

#if defined(_WIN32)
    win->icon(reinterpret_cast<char*>(LoadIcon(fl_display, MAKEINTRESOURCE(win_resource))));
    (void) name;
    (void) xpm_resource;
    (void) name;
#elif defined(__linux__)
    assert(xpm_resource);

    Fl_Pixmap    pix(xpm_resource);
    Fl_RGB_Image rgb(&pix, Fl_Color(0));

    win->icon(&rgb);
    win->xclass((name != nullptr) ? name : "FLTK");
    (void) win_resource;
#else
    (void) win;
    (void) win_resource;
    (void) xpm_resource;
    (void) name;
#endif
}

/** @brief Load window rectangle from preference.
*
* Some size checks are done to assure that the rectangle can be shown on the desktop.
*
* @param[in] pref      Loaded preference object.
* @param[in] basename  Basename for window size.
*
* @return Rectangle with coordinates or empty.
*/
Fl_Rect load_rect_from_pref(Fl_Preferences& pref, const std::string& basename) {
    int  x, y, w, h;

    pref.get((basename + "x").c_str(), x, 0);
    pref.get((basename + "y").c_str(), y, 0);
    pref.get((basename + "w").c_str(), w, 0);
    pref.get((basename + "h").c_str(), h, 0);

    if (x < 0 || x > Fl::w()) {
        x = 0;
    }

    if (y < 0 || y > Fl::h()) {
        y = 0;
    }

    if (w > Fl::w()) {
        x = 0;
        w = Fl::w();
    }

    if (h > Fl::h()) {
        y = 0;
        h = Fl::h();
    }

    return Fl_Rect(x, y, w, h);
}

/** @brief Load theme, font and scaling values.
*
* Scaling settings has to be restored to same screen.\n
* Also wayland, X11 and windows works somewhat different.\n
* So user has to test and try different settings until one works.\n
*
* @param[in] pref        Loaded preference object.
* @param[in] screen_num  Which screen to set scaling for.
*
* @return Current window scale value.
*/
double load_theme_from_pref(Fl_Preferences& pref, unsigned screen_num) {
    auto val = 0;
    char buffer[4000];

    pref.get("regular_name", buffer, "", 4000);
    std::string name = buffer;

    if (name != "" && name != "FL_HELVETICA") {
        auto font = load_font(name);

        if (font != -1) {
            flw::PREF_FONT     = font;
            flw::PREF_FONTNAME = name;
        }
    }

    pref.get("regular_size", val, flw::PREF_FONTSIZE);

    if (val >= 6 && val <= 72) {
        flw::PREF_FONTSIZE = val;
    }

    pref.get("mono_name", buffer, "", 1000);
    name = buffer;

    if (name != "" && name != "FL_COURIER") {
        auto font = load_font(name);

        if (font != -1) {
            flw::PREF_FIXED_FONT     = font;
            flw::PREF_FIXED_FONTNAME = name;
        }
    }

    pref.get("mono_size", val, flw::PREF_FIXED_FONTSIZE);

    if (val >= 6 && val <= 72) {
        flw::PREF_FIXED_FONTSIZE = val;
    }

    pref.get("theme", buffer, "oxy", 4000);
    load(buffer);
    Fl_Tooltip::font(flw::PREF_FONT);
    Fl_Tooltip::size(flw::PREF_FONTSIZE);
    priv::_scrollbar();

    auto so = 0;
    auto sv = 0.0;
    auto def_scaling = Fl::screen_scale(screen_num);

    pref.get("scaling_on", so, 1);
    pref.get("scaling_value", sv, def_scaling);

    if (sv < 0.5 || sv > 2.0) {
        sv = def_scaling;
    }

    flw::PREF_SCALE_ON  = so;
    flw::PREF_SCALE_VAL = sv;

    if (flw::PREF_SCALE_ON == false) {
        Fl::screen_scale(screen_num, 1.0);
    }
    else {
        Fl::screen_scale(screen_num, sv);
    }

    return flw::PREF_SCALE_VAL;
}

/** @brief Load window size and optional scaling.
*
* Load theme before loading windows preferences.\n
*
* @param[in] pref      Loaded preference object.
* @param[in] basename  Base name in preference file.
* @param[in] window    Window to use.
* @param[in] show      True to show window.
* @param[in] defw      Default width if no width is found.
* @param[in] defh      Default height if no height is found.
*/
void load_win_from_pref(Fl_Preferences& pref, const std::string& basename, Fl_Window* window, bool show, int defw, int defh) {
    int x = 0;
    int y = 0;
    int w = 0;
    int h = 0;

    pref.get((basename + "x").c_str(), x, 80);
    pref.get((basename + "y").c_str(), y, 60);
    pref.get((basename + "w").c_str(), w, defw);
    pref.get((basename + "h").c_str(), h, defh);

    if (x < 0 || x > Fl::w()) {
        x = 0;
    }

    if (y < 0 || y > Fl::h()) {
        y = 0;
    }

#ifdef _WIN32
    if (show == true && window->shown() == 0) { // Show before resize in windows.
        window->show();
    }

    window->resize(x, y, w, h);
#else
    window->resize(x, y, w, h);

    if (show == true && window->shown() == 0) {
        window->show();
    }
#endif
}

/** @brief Parse command line arguments for theme and font values.
*
* @param[in] argc  Number of arguments.
* @param[in] argv  Arguments.
*
* @return True if a theme has been set.
*/
bool parse(int argc, const char** argv) {
    auto res = false;

    for (auto f = 1; f < argc; f++) {
        if (res == false) {
            res = load(argv[f]);
        }

        auto fontsize = atoi(argv[f]);

        if (fontsize >= 6 && fontsize <= 72) {
            flw::PREF_FONTSIZE = fontsize;
        }

        if (std::string("scaleoff") == argv[f] && flw::PREF_SCALE_VAL < 0.1) {
            flw::PREF_SCALE_ON = false;

            if (Fl::first_window() != nullptr) {
                flw::PREF_SCALE_VAL = Fl::screen_scale(Fl::first_window()->screen_num());
                Fl::screen_scale(Fl::first_window()->screen_num(), 1.0);
            }
            else {
                flw::PREF_SCALE_VAL = Fl::screen_scale(0);
                Fl::screen_scale(0, 1.0);
            }
        }
    }

    flw::PREF_FIXED_FONTSIZE = flw::PREF_FONTSIZE;
    Fl_Tooltip::font(flw::PREF_FONT);
    Fl_Tooltip::size(flw::PREF_FONTSIZE);

    return res;
}

/** @brief Save window size.
*
* @param[in] pref      Preference object.
* @param[in] basename  Basename for this rectangle (x, y, w, h are added to basename).
* @param[in] rect      Rectangle to save.
*/
void save_rect_to_pref(Fl_Preferences& pref, const std::string& basename, const Fl_Rect& rect) {
    pref.set((basename + "x").c_str(), rect.x());
    pref.set((basename + "y").c_str(), rect.y());
    pref.set((basename + "w").c_str(), rect.w());
    pref.set((basename + "h").c_str(), rect.h());
}

/** @brief Save theme, font and scaling values.
*
* @param[in] pref Preference object.
*/
void save_theme_to_pref(Fl_Preferences& pref) {
    pref.set("theme", flw::PREF_THEME.c_str());
    pref.set("regular_name", flw::PREF_FONTNAME.c_str());
    pref.set("regular_size", flw::PREF_FONTSIZE);
    pref.set("mono_name", flw::PREF_FIXED_FONTNAME.c_str());
    pref.set("mono_size", flw::PREF_FIXED_FONTSIZE);
    pref.set("scaling_on", flw::PREF_SCALE_ON);
    pref.set("scaling_value", flw::PREF_SCALE_VAL);
}

/** @brief Save window size.
*
* @param[in] pref      Preference object..
* @param[in] basename  Base name in preference file.
* @param[in] window    Window to use.
*/
void save_win_to_pref(Fl_Preferences& pref, const std::string& basename, Fl_Window* window) {
    pref.set((basename + "x").c_str(), window->x());
    pref.set((basename + "y").c_str(), window->y());
    pref.set((basename + "w").c_str(), window->w());
    pref.set((basename + "h").c_str(), window->h());
}

} // flw::theme

} // flw

// MKALGAM_OFF
