/**
* @file
* @brief Theme functions.
*
* @author gnuwimp@gmail.com
* @copyright Released under the GNU General Public License v3.0
*/

#include "fontdialog.h"
#include "gridgroup.h"
#include "svgbutton.h"
#include "theme.h"

// MKALGAM_ON

#include <FL/Fl_Box.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Hold_Browser.H>
#include <FL/Fl_Pixmap.H>
#include <FL/Fl_Return_Button.H>
#include <FL/Fl_Tooltip.H>
#include <FL/fl_ask.H>

#ifdef _WIN32
    #include <FL/x.H>
#endif

namespace flw {
namespace priv {

static unsigned char _THEME_OLD_R[256]  = { 0 };
static unsigned char _THEME_OLD_G[256]  = { 0 };
static unsigned char _THEME_OLD_B[256]  = { 0 };
static bool          _THEME_IS_DARK     = false;
static bool          _THEME_SAVED_COLOR = false;
static int           _THEME_SCROLLSIZE  = Fl::scrollbar_size();

const StringVector _THEME_ALT = { // Must be synced with flw::PREF_THEMES.
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

enum { // Must be synced with flw::PREF_THEMES.
    _THEME_DEFAULT,
    _THEME_GLEAM,
    _THEME_GLEAM_BLUE,
    _THEME_GLEAM_DARK,
    _THEME_GLEAM_TAN,
    _THEME_GTK,
    _THEME_GTK_BLUE,
    _THEME_GTK_DARK,
    _THEME_GTK_TAN,
    _THEME_OXY,
    _THEME_OXY_TAN,
    _THEME_PLASTIC,
    _THEME_PLASTIC_TAN,
    _THEME_NIL,
};

/** @brief Create additional colors.
*
* Colors are reset every time a new theme has been selected
*
* @param[in] dark  True if a dark theme has been requested.
*/
static void _theme_additional_colors(bool dark) {
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
static void _theme_blue_colors() {
    Fl::set_color ( 0, 255, 255, 255);  // FL_FOREGROUND_COLOR
    Fl::set_color ( 7,  79,  86,  94);  // FL_BACKGROUND2_COLOR
    Fl::set_color ( 8, 108, 113, 125);  // FL_INACTIVE_COLOR
    Fl::set_color (15, 172, 140,  90);  // FL_SELECTION_COLOR
    Fl::background(36,  44,  53);

    unsigned char r = 0;
    unsigned char g = 9;
    unsigned char b = 18;

    Fl::set_color(32, r, g, b);

    for (int f = 33; f < 49; f++) {
        r += 2;
        g += 2;
        b += 2;
        Fl::set_color(f, r, g, b);
    }
}

/** @brief Set dark colors.
*/
static void _theme_dark_colors() {
    Fl::set_color ( 0, 255, 255, 255); // FL_FOREGROUND_COLOR
    Fl::set_color ( 7,  64,  64,  64); // FL_BACKGROUND2_COLOR
    Fl::set_color ( 8, 100, 100, 100); // FL_INACTIVE_COLOR
    Fl::set_color (15, 107, 138, 107); // FL_SELECTION_COLOR
    Fl::background(36,  36,  36);

    unsigned char c = 0;

    for (int f = 32; f < 49; f++) {
        Fl::set_color(f, c, c, c);
        c += 2;
    }
}

/** @brief Make default colors darker (for dark themes).
*/
static void _theme_make_default_colors_darker() {
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
static void _theme_restore_colors() {
    if (priv::_THEME_SAVED_COLOR == false) {
        return;
    }

    for (int f = 0; f < 256; f++) {
        Fl::set_color(f, priv::_THEME_OLD_R[f], priv::_THEME_OLD_G[f], priv::_THEME_OLD_B[f]);
    }
}

/** @brief Set colors so they can be restored later.
*/
static void _theme_save_colors() {
    if (priv::_THEME_SAVED_COLOR == true) {
        return;
    }

    for (int f = 0; f < 256; f++) {
        unsigned char r1, g1, b1;
        Fl::get_color(f, r1, g1, b1);
        priv::_THEME_OLD_R[f] = r1;
        priv::_THEME_OLD_G[f] = g1;
        priv::_THEME_OLD_B[f] = b1;
    }

    priv::_THEME_SAVED_COLOR = true;
}

/** @brief Set tan colors.
*/
static void _theme_tan_colors() {
    Fl::set_color(0,     0,   0,   0); // FL_FOREGROUND_COLOR
    Fl::set_color(7,   255, 255, 255); // FL_BACKGROUND2_COLOR
    Fl::set_color(8,    85,  85,  85); // FL_INACTIVE_COLOR
    Fl::set_color(15,  188, 114,  50); // FL_SELECTION_COLOR
    Fl::background(206, 202, 187);
}

/** @brief Load default theme.
* @private
*/
void _theme_load_default() {
    priv::_theme_save_colors();
    priv::_theme_restore_colors();
    priv::_theme_additional_colors(false);
    Fl::scheme("none");
    Fl::redraw();
    flw::PREF_THEME = flw::PREF_THEMES[priv::_THEME_DEFAULT];
    priv::_THEME_IS_DARK = false;
}

/** @brief Load gleam theme.
* @private
*/
void _theme_load_gleam() {
    priv::_theme_save_colors();
    priv::_theme_restore_colors();
    priv::_theme_additional_colors(false);
    Fl::scheme("gleam");
    Fl::redraw();
    flw::PREF_THEME = flw::PREF_THEMES[priv::_THEME_GLEAM];
    priv::_THEME_IS_DARK = false;
}

/** @brief Load blue gleam theme.
* @private
*/
void _theme_load_gleam_blue() {
    priv::_theme_save_colors();
    priv::_theme_restore_colors();
    priv::_theme_make_default_colors_darker();
    priv::_theme_blue_colors();
    priv::_theme_additional_colors(true);
    Fl::set_color(255, 101, 117, 125);
    Fl::scheme("gleam");
    Fl::redraw();
    flw::PREF_THEME = flw::PREF_THEMES[priv::_THEME_GLEAM_BLUE];
    priv::_THEME_IS_DARK = true;
}

/** @brief Load dark gleam theme.
* @private
*/
void _theme_load_gleam_dark() {
    priv::_theme_save_colors();
    priv::_theme_restore_colors();
    priv::_theme_make_default_colors_darker();
    priv::_theme_dark_colors();
    priv::_theme_additional_colors(true);
    Fl::set_color(255, 112, 112, 112);
    Fl::scheme("gleam");
    Fl::redraw();
    flw::PREF_THEME = flw::PREF_THEMES[priv::_THEME_GLEAM_DARK];
    priv::_THEME_IS_DARK = true;
}

/** @brief Load tan gleam theme.
* @private
*/
void _theme_load_gleam_tan() {
    priv::_theme_save_colors();
    priv::_theme_restore_colors();
    priv::_theme_tan_colors();
    priv::_theme_additional_colors(false);
    Fl::scheme("gleam");
    Fl::redraw();
    flw::PREF_THEME = flw::PREF_THEMES[priv::_THEME_GLEAM_TAN];
    priv::_THEME_IS_DARK = false;
}

/** @brief Load gtk theme.
* @private
*/
void _theme_load_gtk() {
    priv::_theme_save_colors();
    priv::_theme_restore_colors();
    priv::_theme_additional_colors(false);
    Fl::scheme("gtk+");
    Fl::redraw();
    flw::PREF_THEME = flw::PREF_THEMES[priv::_THEME_GTK];
    priv::_THEME_IS_DARK = false;
}

/** @brief Load blue gtk theme.
* @private
*/
void _theme_load_gtk_blue() {
    priv::_theme_save_colors();
    priv::_theme_restore_colors();
    priv::_theme_make_default_colors_darker();
    priv::_theme_blue_colors();
    priv::_theme_additional_colors(true);
    Fl::set_color(255, 101, 117, 125);
    Fl::scheme("gtk+");
    Fl::redraw();
    flw::PREF_THEME = flw::PREF_THEMES[priv::_THEME_GTK_BLUE];
    priv::_THEME_IS_DARK = true;
}

/** @brief Load dark gtk theme.
* @private
*/
void _theme_load_gtk_dark() {
    priv::_theme_save_colors();
    priv::_theme_restore_colors();
    priv::_theme_make_default_colors_darker();
    priv::_theme_dark_colors();
    priv::_theme_additional_colors(true);
    Fl::set_color(255, 112, 112, 112);
    Fl::scheme("gtk+");
    Fl::redraw();
    flw::PREF_THEME = flw::PREF_THEMES[priv::_THEME_GTK_DARK];
    priv::_THEME_IS_DARK = true;
}

/** @brief Load tan gtk theme.
* @private
*/
void _theme_load_gtk_tan() {
    priv::_theme_save_colors();
    priv::_theme_restore_colors();
    priv::_theme_tan_colors();
    priv::_theme_additional_colors(false);
    Fl::scheme("gtk+");
    Fl::redraw();
    flw::PREF_THEME = flw::PREF_THEMES[priv::_THEME_GTK_TAN];
    priv::_THEME_IS_DARK = false;
}

/** @brief Load oxy theme.
* @private
*/
void _theme_load_oxy() {
    priv::_theme_save_colors();
    priv::_theme_restore_colors();
    priv::_theme_additional_colors(false);
    Fl::scheme("oxy");
    Fl::redraw();
    flw::PREF_THEME = flw::PREF_THEMES[priv::_THEME_OXY];
    priv::_THEME_IS_DARK = false;
}

/** @brief Load tan oxy theme.
* @private
*/
void _theme_load_oxy_tan() {
    priv::_theme_save_colors();
    priv::_theme_restore_colors();
    priv::_theme_tan_colors();
    priv::_theme_additional_colors(false);
    Fl::scheme("oxy");
    Fl::redraw();
    flw::PREF_THEME = flw::PREF_THEMES[priv::_THEME_OXY_TAN];
    priv::_THEME_IS_DARK = false;
}

/** @brief Load plastic theme.
* @private
*/
void _theme_load_plastic() {
    priv::_theme_save_colors();
    priv::_theme_restore_colors();
    priv::_theme_additional_colors(false);
    Fl::scheme("plastic");
    Fl::redraw();
    flw::PREF_THEME = flw::PREF_THEMES[priv::_THEME_PLASTIC];
    priv::_THEME_IS_DARK = false;
}

/** @brief Load tan plastic theme.
* @private
*/
void _theme_load_plastic_tan() {
    priv::_theme_save_colors();
    priv::_theme_restore_colors();
    priv::_theme_tan_colors();
    priv::_theme_additional_colors(false);
    Fl::scheme("plastic");
    Fl::redraw();
    flw::PREF_THEME = flw::PREF_THEMES[priv::_THEME_PLASTIC_TAN];
    priv::_THEME_IS_DARK = false;
}

/** @brief Set scrollbar size.
* @private
*/
void _theme_scrollbar() {
    if (flw::PREF_FONTSIZE < 12 || flw::PREF_FONTSIZE > 16) {
        auto f = static_cast<double>(flw::PREF_FONTSIZE / 14.0);
        auto s = static_cast<int>(f * priv::_THEME_SCROLLSIZE);
        Fl::scrollbar_size(s);
    }
    else if (priv::_THEME_SCROLLSIZE > 0) {
        Fl::scrollbar_size(priv::_THEME_SCROLLSIZE);
    }
}

/*
 *           _____  _    _______ _
 *          |  __ \| |  |__   __| |
 *          | |  | | | __ _| |  | |__   ___ _ __ ___   ___
 *          | |  | | |/ _` | |  | '_ \ / _ \ '_ ` _ \ / _ \
 *          | |__| | | (_| | |  | | | |  __/ | | | | |  __/
 *          |_____/|_|\__, |_|  |_| |_|\___|_| |_| |_|\___|
 *      ______         __/ |
 *     |______|       |___/
 */

/** @brief Select a theme dialog.
*
*/
class _DlgTheme : public Fl_Double_Window {
    Fl_Box*                     _fixed_label;   // Fixed font name and example.
    Fl_Box*                     _font_label;    // Regular font name and example.
    Fl_Button*                  _fixedfont;     // Select fixed font.
    Fl_Button*                  _font;          // Select regular font.
    Fl_Check_Button*            _scale;         // Turn on/off FLTK scaling.
    Fl_Hold_Browser*            _theme;         // Theme list.
    Fl_Slider*                  _scale_val;     // Scale value.
    GridGroup*                  _grid;          // Layout widget.
    SVGButton*                  _close;         // Close button.
    bool                        _run;           // Run flag.
    int                         _theme_row;     // Row index in theme list.

public:
    /** @brief Create window.
    *
    * @param[in] enable_font       Enable selecting regular font.
    * @param[in] enable_fixedfont  Enable selecting fixed font.
    */
    _DlgTheme(bool enable_font, bool enable_fixedfont) :
    Fl_Double_Window(0, 0, 10, 10, "Set Theme") {
        end();

        _close       = new SVGButton(0, 0, 0, 0, labels::CLOSE, icons::BACK, true);
        _fixedfont   = new Fl_Button(0, 0, 0, 0, labels::MONO.c_str());
        _fixed_label = new Fl_Box(0, 0, 0, 0);
        _font        = new Fl_Button(0, 0, 0, 0, labels::REGULAR.c_str());
        _font_label  = new Fl_Box(0, 0, 0, 0);
        _grid        = new GridGroup(0, 0, w(), h());
        _scale       = new Fl_Check_Button(0, 0, 0, 0, "!! Use Scaling");
        _scale_val   = new Fl_Slider(0, 0, 0, 0);
        _theme       = new Fl_Hold_Browser(0, 0, 0, 0);
        _theme_row   = 0;
        _run         = false;

        _grid->add(_theme,         1,   1,  -1, -21);
        _grid->add(_font_label,    1, -20,  -1,   4);
        _grid->add(_fixed_label,   1, -15,  -1,   4);
        _grid->add(_scale,         1, -10,  19,   4);
        _grid->add(_scale_val,    25, -10,  -1,   4);
        _grid->add(_font,        -51,  -5,  16,   4);
        _grid->add(_fixedfont,   -34,  -5,  16,   4);
        _grid->add(_close,       -17,  -5,  16,   4);
        add(_grid);

        if (enable_font == false) {
          _font->deactivate();
        }

        if (enable_fixedfont == false) {
          _fixedfont->deactivate();
        }

        _close->callback(_DlgTheme::Callback, this);
        _fixed_label->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
        _fixed_label->box(FL_BORDER_BOX);
        _fixed_label->color(FL_BACKGROUND2_COLOR);
        _fixed_label->tooltip("Default fixed font.");
        _fixedfont->callback(_DlgTheme::Callback, this);
        _fixedfont->tooltip("Set default fixed font.");
        _font->callback(_DlgTheme::Callback, this);
        _font->tooltip("Set default font.");
        _font_label->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
        _font_label->box(FL_BORDER_BOX);
        _font_label->color(FL_BACKGROUND2_COLOR);
        _font_label->tooltip("Default font.");
        _scale->callback(_DlgTheme::Callback, this);
        _scale->tooltip(
            "Turn on/off FLTK scaling for HiDPI screens.\n"
            "This work somewhat different depending on what desktop it is running on.\n"
            "Save settings and restart application."
        );
        _scale->value(flw::PREF_SCALE_ON);
        _scale_val->range(0.5, 2.0);
        _scale_val->step(0.05);
        _scale_val->value(flw::PREF_SCALE_VAL);
        _scale_val->type(FL_HORIZONTAL);
        _scale_val->callback(_DlgTheme::Callback, this);
        _scale_val->align(FL_ALIGN_LEFT);
        _scale_val->tooltip("Set scaling factor.");
        _theme->box(FL_BORDER_BOX);
        _theme->callback(_DlgTheme::Callback, this);
        _theme->textfont(flw::PREF_FONT);

        for (auto& name : flw::PREF_THEMES) {
            _theme->add(name.c_str());
        }

        if (Fl::screen_scaling_supported() == 0) {
            _scale->value(0);
            _scale->deactivate();
            _scale_val->deactivate();
        }

        _DlgTheme::Callback(_scale_val, this);
        resizable(_grid);
        callback(_DlgTheme::Callback, this);
        set_modal();
        update_pref();
        util::center_window(this, flw::PREF_WINDOW);
    }

    /** @brief Callback for all widgets.
    *
    */
    static void Callback(Fl_Widget* w, void* o) {
        auto self = static_cast<_DlgTheme*>(o);

        if (w == self || w == self->_close) {
            self->_run = false;
            self->hide();
        }
        else if (w == self->_fixedfont) {
            flw::FontDialog fd(flw::PREF_FIXED_FONT, flw::PREF_FIXED_FONTSIZE, "Select Monospaced Font");

            if (fd.run() == true) {
                flw::PREF_FIXED_FONT     = fd.font();
                flw::PREF_FIXED_FONTSIZE = fd.fontsize();
                flw::PREF_FIXED_FONTNAME = fd.fontname();

                if (self->_font->active() == 0) {
                    flw::PREF_FONTSIZE = flw::PREF_FIXED_FONTSIZE;
                }

                self->update_pref();
            }
        }
        else if (w == self->_font) {
            flw::FontDialog fd(flw::PREF_FONT, flw::PREF_FONTSIZE, "Select Regular Font");

            if (fd.run() == true) {
                flw::PREF_FONT     = fd.font();
                flw::PREF_FONTSIZE = fd.fontsize();
                flw::PREF_FONTNAME = fd.fontname();
                Fl_Tooltip::font(flw::PREF_FONT);
                Fl_Tooltip::size(flw::PREF_FONTSIZE);

                if (self->_fixedfont->active() == 0) {
                    flw::PREF_FIXED_FONTSIZE = flw::PREF_FONTSIZE;
                }

                self->update_pref();

                #if defined(__linux__)
                    self->hide(); // !!! Wayland/KDE ignores resizing of window unless the windows does a hide and show, no idea why.
                    self->show();
                #endif
            }
        }
        else if (w == self->_theme) {
            auto row = self->_theme->value() - 1;

            if (row == priv::_THEME_GLEAM) {
                priv::_theme_load_gleam();
            }
            else if (row == priv::_THEME_GLEAM_BLUE) {
                priv::_theme_load_gleam_blue();
            }
            else if (row == priv::_THEME_GLEAM_DARK) {
                priv::_theme_load_gleam_dark();
            }
            else if (row == priv::_THEME_GLEAM_TAN) {
                priv::_theme_load_gleam_tan();
            }
            else if (row == priv::_THEME_GTK) {
                priv::_theme_load_gtk();
            }
            else if (row == priv::_THEME_GTK_BLUE) {
                priv::_theme_load_gtk_blue();
            }
            else if (row == priv::_THEME_GTK_DARK) {
                priv::_theme_load_gtk_dark();
            }
            else if (row == priv::_THEME_GTK_TAN) {
                priv::_theme_load_gtk_tan();
            }
            else if (row == priv::_THEME_OXY) {
                priv::_theme_load_oxy();
            }
            else if (row == priv::_THEME_OXY_TAN) {
                priv::_theme_load_oxy_tan();
            }
            else if (row == priv::_THEME_PLASTIC) {
                priv::_theme_load_plastic();
            }
            else if (row == priv::_THEME_PLASTIC_TAN) {
                priv::_theme_load_plastic_tan();
            }
            else {
                priv::_theme_load_default();
            }

            self->update_pref();
        }
        else if (w == self->_scale) {
            flw::PREF_SCALE_ON = self->_scale->value();
        }
        else if (w == self->_scale_val) {
            flw::PREF_SCALE_VAL = self->_scale_val->value();
            self->_scale_val->copy_label(util::format("%.2f", flw::PREF_SCALE_VAL).c_str());
        }
    }

    /** @brief Run dialog.
    *
    */
    void run() {
        _run = true;
        show();

        while (_run == true) {
            Fl::wait();
            Fl::flush();
        }
    }

    /** @brief Update widget fonts and looks.
    *
    */
    void update_pref() {
        size(flw::PREF_FONTSIZE * 30, flw::PREF_FONTSIZE * 32);
        _grid->resize(0, 0, w(), h());

        Fl_Tooltip::font(flw::PREF_FONT);
        Fl_Tooltip::size(flw::PREF_FONTSIZE);
        util::labelfont(this);
        _font_label->copy_label(util::format("%s - %d", flw::PREF_FONTNAME.c_str(), flw::PREF_FONTSIZE).c_str());
        _fixed_label->copy_label(util::format("%s - %d", flw::PREF_FIXED_FONTNAME.c_str(), flw::PREF_FIXED_FONTSIZE).c_str());
        _fixed_label->labelfont(flw::PREF_FIXED_FONT);
        _fixed_label->labelsize(flw::PREF_FIXED_FONTSIZE);
        _theme->textfont(flw::PREF_FONT);
        _theme->textsize(flw::PREF_FONTSIZE);
        priv::_theme_scrollbar();

        for (int f = 0; f < priv::_THEME_NIL; f++) {
            if (flw::PREF_THEME == flw::PREF_THEMES[f]) {
                _theme->value(f + 1);
                break;
            }
        }

        Fl::redraw();
    }
};

} // flw::priv
} // flw

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

Fl_Color flw::color::BEIGE            = fl_rgb_color(245, 245, 220);
Fl_Color flw::color::CHOCOLATE        = fl_rgb_color(210, 105,  30);
Fl_Color flw::color::CRIMSON          = fl_rgb_color(220,  20,  60);
Fl_Color flw::color::DARKOLIVEGREEN   = fl_rgb_color( 85, 107,  47);
Fl_Color flw::color::DODGERBLUE       = fl_rgb_color( 30, 144, 255);
Fl_Color flw::color::FORESTGREEN      = fl_rgb_color( 34, 139,  34);
Fl_Color flw::color::GOLD             = fl_rgb_color(255, 215,   0);
Fl_Color flw::color::GRAY             = fl_rgb_color(128, 128, 128);
Fl_Color flw::color::INDIGO           = fl_rgb_color( 75,   0, 130);
Fl_Color flw::color::OLIVE            = fl_rgb_color(128, 128,   0);
Fl_Color flw::color::PINK             = fl_rgb_color(255, 192, 203);
Fl_Color flw::color::ROYALBLUE        = fl_rgb_color( 65, 105, 225);
Fl_Color flw::color::SIENNA           = fl_rgb_color(160,  82,  45);
Fl_Color flw::color::SILVER           = fl_rgb_color(192, 192, 192);
Fl_Color flw::color::SLATEGRAY        = fl_rgb_color(112, 128, 144);
Fl_Color flw::color::TEAL             = fl_rgb_color(  0, 128, 128);
Fl_Color flw::color::TURQUOISE        = fl_rgb_color( 64, 224, 208);
Fl_Color flw::color::VIOLET           = fl_rgb_color(238, 130, 238);

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


/** @brief Check if a dark theme has been loaded
*
* @return True for dark theme.
*/
bool flw::theme::is_dark() {
    if (flw::PREF_THEME == flw::PREF_THEMES[priv::_THEME_GLEAM_BLUE] ||
        flw::PREF_THEME == flw::PREF_THEMES[priv::_THEME_GLEAM_DARK] ||
        flw::PREF_THEME == flw::PREF_THEMES[priv::_THEME_GTK_BLUE] ||
        flw::PREF_THEME == flw::PREF_THEMES[priv::_THEME_GTK_DARK]) {
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
bool flw::theme::load(const std::string& name) {
    if (priv::_THEME_SCROLLSIZE == 0) {
        priv::_THEME_SCROLLSIZE = Fl::scrollbar_size();
    }

    if (name == flw::PREF_THEMES[priv::_THEME_DEFAULT] || name == priv::_THEME_ALT[priv::_THEME_DEFAULT]) {
        priv::_theme_load_default();
    }
    else if (name == flw::PREF_THEMES[priv::_THEME_GLEAM] || name == priv::_THEME_ALT[priv::_THEME_GLEAM]) {
        priv::_theme_load_gleam();
    }
    else if (name == flw::PREF_THEMES[priv::_THEME_GLEAM_BLUE] || name == priv::_THEME_ALT[priv::_THEME_GLEAM_BLUE]) {
        priv::_theme_load_gleam_blue();
    }
    else if (name == flw::PREF_THEMES[priv::_THEME_GLEAM_DARK] || name == priv::_THEME_ALT[priv::_THEME_GLEAM_DARK]) {
        priv::_theme_load_gleam_dark();
    }
    else if (name == flw::PREF_THEMES[priv::_THEME_GLEAM_TAN] || name == priv::_THEME_ALT[priv::_THEME_GLEAM_TAN]) {
        priv::_theme_load_gleam_tan();
    }
    else if (name == flw::PREF_THEMES[priv::_THEME_GTK] || name == priv::_THEME_ALT[priv::_THEME_GTK]) {
        priv::_theme_load_gtk();
    }
    else if (name == flw::PREF_THEMES[priv::_THEME_GTK_BLUE] || name == priv::_THEME_ALT[priv::_THEME_GTK_BLUE]) {
        priv::_theme_load_gtk_blue();
    }
    else if (name == flw::PREF_THEMES[priv::_THEME_GTK_DARK] || name == priv::_THEME_ALT[priv::_THEME_GTK_DARK]) {
        priv::_theme_load_gtk_dark();
    }
    else if (name == flw::PREF_THEMES[priv::_THEME_GTK_TAN] || name == priv::_THEME_ALT[priv::_THEME_GTK_TAN]) {
        priv::_theme_load_gtk_tan();
    }
    else if (name == flw::PREF_THEMES[priv::_THEME_OXY] || name == priv::_THEME_ALT[priv::_THEME_OXY]) {
        priv::_theme_load_oxy();
    }
    else if (name == flw::PREF_THEMES[priv::_THEME_OXY_TAN] || name == priv::_THEME_ALT[priv::_THEME_OXY_TAN]) {
        priv::_theme_load_oxy_tan();
    }
    else if (name == flw::PREF_THEMES[priv::_THEME_PLASTIC] || name == priv::_THEME_ALT[priv::_THEME_PLASTIC]) {
        priv::_theme_load_plastic();
    }
    else if (name == flw::PREF_THEMES[priv::_THEME_PLASTIC_TAN] || name == priv::_THEME_ALT[priv::_THEME_PLASTIC_TAN]) {
        priv::_theme_load_plastic_tan();
    }
    else {
        return false;
    }

    priv::_theme_scrollbar();
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
Fl_Font flw::theme::load_font(const std::string& requested_font) {
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
* But it does it only once.\n
*
* @param[in] iso8859_only  True to only use fonts with ISO8859-1 character set.
*/
void flw::theme::load_fonts(bool iso8859_only) {
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
* @param[in] xpm_resource  Xpm icon (Linux/X11 only).
* @param[in] name          Application name (Linux only).
*/
void flw::theme::load_icon(Fl_Window* win, int win_resource, const char** xpm_resource, const char* name) {
    if (win->shown() != 0) {
        fl_alert("%s", "Warning: load icon before showing window!");
    }

#if defined(_WIN32)
    win->icon(reinterpret_cast<char*>(LoadIcon(fl_display, MAKEINTRESOURCE(win_resource))));
    (void) name;
    (void) xpm_resource;
    (void) name;
#elif defined(__linux__)
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
Fl_Rect flw::theme::load_rect_from_pref(Fl_Preferences& pref, const std::string& basename) {
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
double flw::theme::load_theme_from_pref(Fl_Preferences& pref, unsigned screen_num) {
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
    priv::_theme_scrollbar();

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
* Set resizable() widget before calling this.\n
*
* @param[in] pref      Loaded preference object.
* @param[in] basename  Base name in preference file.
* @param[in] window    Window to use.
* @param[in] show      True to show window.
* @param[in] defw      Default width if no width is found.
* @param[in] defh      Default height if no height is found.
*/
void flw::theme::load_win_from_pref(Fl_Preferences& pref, const std::string& basename, Fl_Window* window, bool show, int defw, int defh) {
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
bool flw::theme::parse(int argc, const char** argv) {
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
void flw::theme::save_rect_to_pref(Fl_Preferences& pref, const std::string& basename, const Fl_Rect& rect) {
    pref.set((basename + "x").c_str(), rect.x());
    pref.set((basename + "y").c_str(), rect.y());
    pref.set((basename + "w").c_str(), rect.w());
    pref.set((basename + "h").c_str(), rect.h());
}

/** @brief Save theme, font and scaling values.
*
* @param[in] pref Preference object.
*/
void flw::theme::save_theme_to_pref(Fl_Preferences& pref) {
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
void flw::theme::save_win_to_pref(Fl_Preferences& pref, const std::string& basename, Fl_Window* window) {
    pref.set((basename + "x").c_str(), window->x());
    pref.set((basename + "y").c_str(), window->y());
    pref.set((basename + "w").c_str(), window->w());
    pref.set((basename + "h").c_str(), window->h());
}

/** @brief Show a theme and font selection dialog.
*
* Current theme is selected.\n
* Following font values are updated.\n
* flw::PREF_FONT.\n
* flw::PREF_FONTSIZE.\n
* flw::PREF_FONTNAME.\n
* flw::PREF_FIXED_FONT.\n
* flw::PREF_FIXED_FONTSIZE.\n
* flw::PREF_FIXED_FONTNAME.\n
* flw::PREF_SCALE_ON.\n
* flw::PREF_SCALE_VAL.\n
*
* @param[in] enable_font       Enable selecting regular font.
* @param[in] enable_fixedfont  Enable selecting fixed font.
*
* @snippet dialog.cpp flw::dlg::theme example
* @image html theme_dialog.png
*/
void flw::dlg::theme(bool enable_font, bool enable_fixedfont) {
    priv::_DlgTheme(enable_font, enable_fixedfont).run();
}

// MKALGAM_OFF
