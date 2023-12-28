// Copyright gnuwimp@gmail.com
// Released under the GNU General Public License v3.0
// Colors in this file are based on a text file sent to fltk mailing list by Greg Ercolano <erco@seriss.com>

#include "theme.h"
#include "fontdialog.h"
#include "util.h"
#include <FL/Fl_Box.H>
#include <FL/Fl_Return_Button.H>
#include <FL/Fl_Hold_Browser.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Preferences.H>
#include <FL/fl_ask.H>

#ifdef _WIN32
    #include <FL/x.H>
    #include <windows.h>
#elif defined(__linux__)
    #include <FL/Fl_Window.H>
    #include <FL/Fl_Pixmap.H>
    #include <FL/Fl_RGB_Image.H>
#endif

// MKALGAM_ON

namespace flw {
    namespace color {
        Fl_Color RED     = fl_rgb_color(255, 0, 0);
        Fl_Color LIME    = fl_rgb_color(0, 255, 0);
        Fl_Color BLUE    = fl_rgb_color(0, 0, 255);
        Fl_Color YELLOW  = fl_rgb_color(255, 255, 0);
        Fl_Color CYAN    = fl_rgb_color(0, 255, 255);
        Fl_Color MAGENTA = fl_rgb_color(255, 0, 255);
        Fl_Color SILVER  = fl_rgb_color(192, 192, 192);
        Fl_Color GRAY    = fl_rgb_color(128, 128, 128);
        Fl_Color MAROON  = fl_rgb_color(128, 0, 0);
        Fl_Color OLIVE   = fl_rgb_color(128, 128, 0);
        Fl_Color GREEN   = fl_rgb_color(0, 128, 0);
        Fl_Color PURPLE  = fl_rgb_color(128, 0, 128);
        Fl_Color TEAL    = fl_rgb_color(0, 128, 128);
        Fl_Color NAVY    = fl_rgb_color(0, 0, 128);
        Fl_Color BROWN   = fl_rgb_color(210, 105, 30);
        Fl_Color PINK    = fl_rgb_color(255, 192, 203);
        Fl_Color BEIGE   = fl_rgb_color(245, 245, 220);
        Fl_Color AZURE   = fl_rgb_color(240, 255, 250);
    }

    namespace theme {
        static const char* _NAMES[] = {
            "default",
            "oxy",
            "blue oxy",
            "tan oxy",
            "gleam",
            "blue gleam",
            "dark blue gleam",
            "dark gleam",
            "darker gleam",
            "tan gleam",
            "gtk",
            "blue gtk",
            "dark blue gtk",
            "dark gtk",
            "darker gtk",
            "tan gtk",
            "plastic",
            "blue plastic",
            "tan plastic",
            "system"
        };

        enum {
            _NAME_DEFAULT,
            _NAME_OXY,
            _NAME_OXY_BLUE,
            _NAME_OXY_TAN,
            _NAME_GLEAM,
            _NAME_GLEAM_BLUE,
            _NAME_GLEAM_DARK_BLUE,
            _NAME_GLEAM_DARK,
            _NAME_GLEAM_DARKER,
            _NAME_GLEAM_TAN,
            _NAME_GTK,
            _NAME_GTK_BLUE,
            _NAME_GTK_DARK_BLUE,
            _NAME_GTK_DARK,
            _NAME_GTK_DARKER,
            _NAME_GTK_TAN,
            _NAME_PLASTIC,
            _NAME_PLASTIC_BLUE,
            _NAME_PLASTIC_TAN,
            _NAME_SYSTEM,
        };

        static unsigned char _OLD_R[256]  = { 0 };
        static unsigned char _OLD_G[256]  = { 0 };
        static unsigned char _OLD_B[256]  = { 0 };
        static unsigned char _SYS_R[256]  = { 0 };
        static unsigned char _SYS_G[256]  = { 0 };
        static unsigned char _SYS_B[256]  = { 0 };
        static bool          _IS_DARK     = false;
        static bool          _SAVED_COLOR = false;
        static bool          _SAVED_SYS   = false;
        static int           _SCROLLBAR   = 2;
        static std::string   _NAME        = _NAMES[_NAME_DEFAULT];

        //----------------------------------------------------------------------
        // Colors are reset every time a new theme has been selected
        //!!! Colors are same for dark and light for now
        //
        static void _colors(bool dark) {
            (void) dark;

            color::AZURE   = fl_rgb_color(240, 255, 250);
            color::BEIGE   = fl_rgb_color(245, 245, 220);
            color::BLUE    = fl_rgb_color(0, 0, 255);
            color::BLUE    = fl_rgb_color(0, 0, 255);
            color::BROWN   = fl_rgb_color(210, 105, 30);
            color::CYAN    = fl_rgb_color(0, 255, 255);
            color::GRAY    = fl_rgb_color(128, 128, 128);
            color::GREEN   = fl_rgb_color(0, 128, 0);
            color::LIME    = fl_rgb_color(0, 255, 0);
            color::MAGENTA = fl_rgb_color(255, 0, 255);
            color::MAROON  = fl_rgb_color(128, 0, 0);
            color::NAVY    = fl_rgb_color(0, 0, 128);
            color::OLIVE   = fl_rgb_color(128, 128, 0);
            color::PINK    = fl_rgb_color(255, 192, 203);
            color::PURPLE  = fl_rgb_color(128, 0, 128);
            color::RED     = fl_rgb_color(255, 0, 0);
            color::SILVER  = fl_rgb_color(192, 192, 192);
            color::TEAL    = fl_rgb_color(0, 128, 128);
            color::YELLOW  = fl_rgb_color(255, 255, 0);
        }

        //----------------------------------------------------------------------
        static void _make_default_colors_darker() {
            Fl::set_color(60,    0,  63,   0); // FL_DARK_GREEN
            Fl::set_color(63,    0, 110,   0); // FL_GREEN
            Fl::set_color(72,   55,   0,   0); // FL_DARK_RED
            Fl::set_color(76,   55,  63,   0); // FL_DARK_YELLOW
            Fl::set_color(88,  110,   0,   0); // FL_RED
            Fl::set_color(95,  140, 140, 100); // FL_YELLOW
            Fl::set_color(136,   0,   0,  55); // FL_DARK_BLUE
            Fl::set_color(140,   0,  63,  55); // FL_DARK_CYAN
            Fl::set_color(152,  55,   0,  55); // FL_DARK_MAGENTA
            Fl::set_color(216,   0,   0, 110); // FL_BLUE
            Fl::set_color(223,   0, 110, 110); // FL_CYAN
            Fl::set_color(248, 110,   0, 110); // FL_MAGENTA
        }

        //----------------------------------------------------------------------
        static void _blue_colors() {
            Fl::set_color(0,     0,   0,   0); // FL_FOREGROUND_COLOR
            Fl::set_color(7,   255, 255, 255); // FL_BACKGROUND2_COLOR
            Fl::set_color(8,   130, 149, 166); // FL_INACTIVE_COLOR
            Fl::set_color(15,  255, 160,  63); // FL_SELECTION_COLOR
            Fl::set_color(255, 244, 244, 244); // FL_WHITE
            Fl::background(170, 189, 206);
        }

        //----------------------------------------------------------------------
        static void _dark_blue_colors() {
            Fl::set_color(0,   255, 255, 255); // FL_FOREGROUND_COLOR
            Fl::set_color(7,    31,  47,  55); // FL_BACKGROUND2_COLOR
            Fl::set_color(8,   108, 113, 125); // FL_INACTIVE_COLOR
            Fl::set_color(15,   68, 138, 255); // FL_SELECTION_COLOR
            Fl::set_color(56,    0,   0,   0); // FL_BLACK
            Fl::background(64, 80, 87);
        }

        //----------------------------------------------------------------------
        static void _dark_colors() {
            Fl::set_color(0,   240, 240, 240); // FL_FOREGROUND_COLOR
            Fl::set_color(7,    55,  55,  55); // FL_BACKGROUND2_COLOR
            Fl::set_color(8,   100, 100, 100); // FL_INACTIVE_COLOR
            Fl::set_color(15,  149,  75,  37); // FL_SELECTION_COLOR
            Fl::set_color(56,    0,   0,   0); // FL_BLACK
            Fl::background(85, 85, 85);
            Fl::background(64, 64, 64);
        }

        //----------------------------------------------------------------------
        static void _darker_colors() {
            Fl::set_color(0,   164, 164, 164); // FL_FOREGROUND_COLOR
            Fl::set_color(7,    16,  16,  16); // FL_BACKGROUND2_COLOR
            Fl::set_color(7,    28,  28,  28); // FL_BACKGROUND2_COLOR
            Fl::set_color(8,   100, 100, 100); // FL_INACTIVE_COLOR
            Fl::set_color(15,  133,  59,  21); // FL_SELECTION_COLOR
            Fl::set_color(56,    0,   0,   0); // FL_BLACK
            Fl::background(32, 32, 32);
            Fl::background(38, 38, 38);
        }

        //----------------------------------------------------------------------
        static void _tan_colors() {
            Fl::set_color(0,     0,   0,   0); // FL_FOREGROUND_COLOR
            Fl::set_color(7,   255, 255, 255); // FL_BACKGROUND2_COLOR
            Fl::set_color(8,    85,  85,  85); // FL_INACTIVE_COLOR
            Fl::set_color(15,  255, 160,  63); // FL_SELECTION_COLOR
            Fl::set_color(255, 244, 244, 244); // FL_WHITE
            Fl::background(206, 202, 187);
        }

        //----------------------------------------------------------------------
        static void _restore_colors() {
            if (_SAVED_COLOR == true) {
                for (int f = 0; f < 256; f++) {
                    Fl::set_color(f, flw::theme::_OLD_R[f], flw::theme::_OLD_G[f], flw::theme::_OLD_B[f]);
                }
            }
        }

        //----------------------------------------------------------------------
        static void _restore_sys() {
            if (_SAVED_SYS == true) {
                for (int f = 0; f < 256; f++) {
                    Fl::set_color(f, flw::theme::_SYS_R[f], flw::theme::_SYS_G[f], flw::theme::_SYS_B[f]);
                }
            }
        }

        //----------------------------------------------------------------------
        static void _save_colors() {
            if (_SAVED_COLOR == false) {
                for (int f = 0; f < 256; f++) {
                    unsigned char r1, g1, b1;
                    Fl::get_color(f, r1, g1, b1);
                    flw::theme::_OLD_R[f] = r1;
                    flw::theme::_OLD_G[f] = g1;
                    flw::theme::_OLD_B[f] = b1;
                }

                _SAVED_COLOR = true;
            }
        }

        //----------------------------------------------------------------------
        static void _save_sys() {
            if (_SAVED_SYS == false) {
                for (int f = 0; f < 256; f++) {
                    unsigned char r1, g1, b1;
                    Fl::get_color(f, r1, g1, b1);
                    flw::theme::_SYS_R[f] = r1;
                    flw::theme::_SYS_G[f] = g1;
                    flw::theme::_SYS_B[f] = b1;
                }

                _SAVED_SYS = true;
            }
        }

        //------------------------------------------------------------------------------
        static void _load_default() {
            flw::theme::_save_colors();
            flw::theme::_restore_colors();
            flw::theme::_colors(false);
            Fl::set_color(FL_SELECTION_COLOR, 0, 120, 200);
            Fl::scheme("none");
            Fl::redraw();
            _NAME = _NAMES[_NAME_DEFAULT];
            _IS_DARK = false;
        }

        //------------------------------------------------------------------------------
        static void _load_oxy() {
            flw::theme::_save_colors();
            flw::theme::_restore_colors();
            flw::theme::_colors(false);
            Fl::scheme("oxy");
            Fl::set_color(FL_SELECTION_COLOR, 0, 120, 200);
            Fl::redraw();
            _NAME = _NAMES[_NAME_OXY];
            _IS_DARK = false;
            _SCROLLBAR = 1;
        }

        //------------------------------------------------------------------------------
        static void _load_oxy_blue() {
            flw::theme::_save_colors();
            flw::theme::_restore_colors();
            flw::theme::_blue_colors();
            flw::theme::_colors(false);
            Fl::scheme("oxy");
            Fl::redraw();
            _NAME = _NAMES[_NAME_OXY_BLUE];
            _IS_DARK = false;
            _SCROLLBAR = 1;
        }

        //------------------------------------------------------------------------------
        static void _load_oxy_tan() {
            flw::theme::_save_colors();
            flw::theme::_restore_colors();
            flw::theme::_tan_colors();
            flw::theme::_colors(false);
            Fl::scheme("oxy");
            Fl::redraw();
            _NAME = _NAMES[_NAME_OXY_TAN];
            _IS_DARK = false;
            _SCROLLBAR = 1;
        }

        //------------------------------------------------------------------------------
        static void _load_gleam() {
            flw::theme::_save_colors();
            flw::theme::_restore_colors();
            flw::theme::_colors(false);
            Fl::scheme("gleam");
            Fl::set_color(FL_SELECTION_COLOR, 0, 120, 200);
            Fl::redraw();
            _NAME = _NAMES[_NAME_GLEAM];
            _IS_DARK = false;
            _SCROLLBAR = 1;
        }

        //------------------------------------------------------------------------------
        static void _load_gleam_blue() {
            flw::theme::_save_colors();
            flw::theme::_restore_colors();
            flw::theme::_blue_colors();
            flw::theme::_colors(false);
            Fl::scheme("gleam");
            Fl::redraw();
            _NAME = _NAMES[_NAME_GLEAM_BLUE];
            _IS_DARK = false;
            _SCROLLBAR = 1;
        }

        //------------------------------------------------------------------------------
        static void _load_gleam_dark() {
            flw::theme::_save_colors();
            flw::theme::_restore_colors();
            flw::theme::_dark_colors();
            flw::theme::_make_default_colors_darker();
            flw::theme::_colors(true);
            Fl::set_color(255, 125, 125, 125);
            Fl::scheme("gleam");
            Fl::redraw();
            _NAME = _NAMES[_NAME_GLEAM_DARK];
            _IS_DARK = true;
            _SCROLLBAR = 1;
        }

        //------------------------------------------------------------------------------
        static void _load_gleam_darker() {
            flw::theme::_save_colors();
            flw::theme::_restore_colors();
            flw::theme::_darker_colors();
            flw::theme::_make_default_colors_darker();
            flw::theme::_colors(true);
            Fl::set_color(255, 125, 125, 125);
            Fl::scheme("gleam");
            Fl::redraw();
            _NAME = _NAMES[_NAME_GLEAM_DARKER];
            _IS_DARK = true;
            _SCROLLBAR = 1;
        }

        //------------------------------------------------------------------------------
        static void _load_gleam_dark_blue() {
            flw::theme::_save_colors();
            flw::theme::_restore_colors();
            flw::theme::_dark_blue_colors();
            flw::theme::_make_default_colors_darker();
            flw::theme::_colors(true);
            Fl::set_color(255, 101, 117, 125);
            Fl::scheme("gleam");
            Fl::redraw();
            _NAME = _NAMES[_NAME_GLEAM_DARK_BLUE];
            _IS_DARK = true;
            _SCROLLBAR = 1;
        }

        //------------------------------------------------------------------------------
        static void _load_gleam_tan() {
            flw::theme::_save_colors();
            flw::theme::_restore_colors();
            flw::theme::_tan_colors();
            flw::theme::_colors(false);
            Fl::scheme("gleam");
            Fl::redraw();
            _NAME = _NAMES[_NAME_GLEAM_TAN];
            _IS_DARK = false;
            _SCROLLBAR = 1;
        }

        //------------------------------------------------------------------------------
        static void _load_gtk() {
            flw::theme::_save_colors();
            flw::theme::_restore_colors();
            flw::theme::_colors(false);
            Fl::scheme("gtk+");
            Fl::set_color(FL_SELECTION_COLOR, 0, 120, 200);
            Fl::redraw();
            _NAME = _NAMES[_NAME_GTK];
            _IS_DARK = false;
            _SCROLLBAR = 1;
        }

        //------------------------------------------------------------------------------
        static void _load_gtk_blue() {
            flw::theme::_save_colors();
            flw::theme::_restore_colors();
            flw::theme::_blue_colors();
            flw::theme::_colors(false);
            Fl::scheme("gtk+");
            Fl::redraw();
            _NAME = _NAMES[_NAME_GTK_BLUE];
            _IS_DARK = false;
            _SCROLLBAR = 1;
        }

        //------------------------------------------------------------------------------
        static void _load_gtk_dark() {
            flw::theme::_save_colors();
            flw::theme::_restore_colors();
            flw::theme::_dark_colors();
            flw::theme::_make_default_colors_darker();
            flw::theme::_colors(true);
            Fl::set_color(255, 185, 185, 185);
            Fl::scheme("gtk+");
            Fl::redraw();
            _NAME = _NAMES[_NAME_GTK_DARK];
            _IS_DARK = true;
            _SCROLLBAR = 1;
        }

        //------------------------------------------------------------------------------
        static void _load_gtk_darker() {
            flw::theme::_save_colors();
            flw::theme::_restore_colors();
            flw::theme::_darker_colors();
            flw::theme::_make_default_colors_darker();
            flw::theme::_colors(true);
            Fl::set_color(255, 125, 125, 125);
            Fl::scheme("gtk+");
            Fl::redraw();
            _NAME = _NAMES[_NAME_GTK_DARKER];
            _IS_DARK = true;
        }

        //------------------------------------------------------------------------------
        static void _load_gtk_dark_blue() {
            flw::theme::_save_colors();
            flw::theme::_restore_colors();
            flw::theme::_dark_blue_colors();
            flw::theme::_make_default_colors_darker();
            flw::theme::_colors(true);
            Fl::set_color(255, 161, 177, 185);
            Fl::scheme("gtk+");
            Fl::redraw();
            _NAME = _NAMES[_NAME_GTK_DARK_BLUE];
            _IS_DARK = true;
            _SCROLLBAR = 1;
        }

        //------------------------------------------------------------------------------
        static void _load_gtk_tan() {
            flw::theme::_save_colors();
            flw::theme::_restore_colors();
            flw::theme::_tan_colors();
            flw::theme::_colors(false);
            Fl::scheme("gtk+");
            Fl::redraw();
            _NAME = _NAMES[_NAME_GTK_TAN];
            _IS_DARK = false;
            _SCROLLBAR = 1;
        }

        //------------------------------------------------------------------------------
        static void _load_plastic() {
            flw::theme::_save_colors();
            flw::theme::_restore_colors();
            flw::theme::_colors(false);
            Fl::set_color(FL_SELECTION_COLOR, 0, 120, 200);
            Fl::scheme("plastic");
            Fl::redraw();
            _NAME = _NAMES[_NAME_PLASTIC];
            _IS_DARK = false;
            _SCROLLBAR = 2;
        }

        //------------------------------------------------------------------------------
        static void _load_blue_plastic() {
            flw::theme::_save_colors();
            flw::theme::_restore_colors();
            flw::theme::_blue_colors();
            flw::theme::_colors(false);
            Fl::scheme("plastic");
            Fl::redraw();
            _NAME = _NAMES[_NAME_PLASTIC_BLUE];
            _IS_DARK = false;
            _SCROLLBAR = 2;
        }

        //------------------------------------------------------------------------------
        static void _load_tan_plastic() {
            flw::theme::_save_colors();
            flw::theme::_restore_colors();
            flw::theme::_tan_colors();
            flw::theme::_colors(false);
            Fl::scheme("plastic");
            Fl::redraw();
            _NAME = _NAMES[_NAME_PLASTIC_TAN];
            _IS_DARK = false;
            _SCROLLBAR = 2;
        }

        //------------------------------------------------------------------------------
        static void _load_system() {
            flw::theme::_save_colors();
            flw::theme::_restore_colors();
            flw::theme::_colors(false);
            Fl::scheme("gtk+");

            if (theme::_SAVED_SYS) {
              flw::theme::_restore_sys();
            }
            else {
              Fl::get_system_colors();
              flw::theme::_save_sys();
            }

            Fl::redraw();
            _NAME = _NAMES[_NAME_SYSTEM];
            _IS_DARK = false;
            _SCROLLBAR = 1;
        }

        //------------------------------------------------------------------------------
        static void _scrollbar() {
            auto s = (flw::PREF_FONTSIZE > flw::PREF_FIXED_FONTSIZE) ? flw::PREF_FONTSIZE : flw::PREF_FIXED_FONTSIZE;
            Fl::scrollbar_size(s + _SCROLLBAR);
        }
    }

    namespace dlg {
        //----------------------------------------------------------------------
        class _DlgTheme : public Fl_Double_Window {
            Fl_Box*                     _font_label;
            Fl_Box*                     _fixedfont_label;
            Fl_Browser*                 _theme;
            Fl_Button*                  _close;
            Fl_Button*                  _font;
            Fl_Button*                  _fixedfont;
            int                         _theme_row;

        public:
            //------------------------------------------------------------------
            _DlgTheme(bool enable_font, bool enable_fixedfont, Fl_Window* parent) : Fl_Double_Window(0, 0, 0, 0, "Set Theme") {
                end();

                _close           = new Fl_Return_Button(0, 0, 0, 0, "&Close");
                _fixedfont       = new Fl_Button(0, 0, 0, 0, "F&ixed Font");
                _fixedfont_label = new Fl_Box(0, 0, 0, 0);
                _font            = new Fl_Button(0, 0, 0, 0, "&Font");
                _font_label      = new Fl_Box(0, 0, 0, 0);
                _theme           = new Fl_Hold_Browser(0, 0, 0, 0);
                _theme_row       = 0;

                add(_theme);
                add(_fixedfont);
                add(_font);
                add(_close);
                add(_fixedfont_label);
                add(_font_label);

                if (enable_font == false) {
                  _font->deactivate();
                }

                if (enable_fixedfont == false) {
                  _fixedfont->deactivate();
                }

                _close->callback(Callback, this);
                _font_label->color(FL_BACKGROUND2_COLOR);
                _font_label->box(FL_DOWN_BOX);
                _font->callback(Callback, this);
                _font_label->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
                _fixedfont_label->color(FL_BACKGROUND2_COLOR);
                _fixedfont_label->box(FL_DOWN_BOX);
                _fixedfont->callback(Callback, this);
                _fixedfont_label->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
                _theme->callback(Callback, this);
                _theme->textfont(flw::PREF_FONT);

                for (int f = 0; f <= flw::theme::_NAME_SYSTEM; f++) {
                    _theme->add(theme::_NAMES[f]);
                }

                resizable(this);
                callback(Callback, this);
                set_modal();
                update_pref();
                util::center_window(this, parent);
            }

            //------------------------------------------------------------------
            static void Callback(Fl_Widget* w, void* o) {
                auto dlg = (_DlgTheme*) o;

                if (w == dlg) {
                    dlg->hide();
                }
                else if (w == dlg->_fixedfont) {
                    flw::dlg::FontDialog fd(flw::PREF_FIXED_FONT, flw::PREF_FIXED_FONTSIZE, "Select Monospaced Font");

                    if (fd.run(Fl::first_window()) == true) {
                        flw::PREF_FIXED_FONT     = fd.font();
                        flw::PREF_FIXED_FONTSIZE = fd.fontsize();
                        flw::PREF_FIXED_FONTNAME = fd.fontname();

                        if (dlg->_font->active() == 0) {
                            flw::PREF_FONTSIZE = flw::PREF_FIXED_FONTSIZE;
                        }

                        dlg->update_pref();
                    }
                }
                else if (w == dlg->_font) {
                    flw::dlg::FontDialog fd(flw::PREF_FONT, flw::PREF_FONTSIZE, "Select Font");

                    if (fd.run(Fl::first_window()) == true) {
                        flw::PREF_FONT     = fd.font();
                        flw::PREF_FONTSIZE = fd.fontsize();
                        flw::PREF_FONTNAME = fd.fontname();

                        if (dlg->_fixedfont->active() == 0) {
                            flw::PREF_FIXED_FONTSIZE = flw::PREF_FONTSIZE;
                        }

                        dlg->update_pref();
                    }
                }
                else if (w == dlg->_theme) {
                    auto row = dlg->_theme->value() - 1;

                    if (row == flw::theme::_NAME_OXY) {
                        flw::theme::_load_oxy();
                    }
                    else if (row == flw::theme::_NAME_OXY_BLUE) {
                        flw::theme::_load_oxy_blue();
                    }
                    else if (row == flw::theme::_NAME_OXY_TAN) {
                        flw::theme::_load_oxy_tan();
                    }
                    else if (row == flw::theme::_NAME_GLEAM) {
                        flw::theme::_load_gleam();
                    }
                    else if (row == flw::theme::_NAME_GLEAM_BLUE) {
                        flw::theme::_load_gleam_blue();
                    }
                    else if (row == flw::theme::_NAME_GLEAM_DARK_BLUE) {
                        flw::theme::_load_gleam_dark_blue();
                    }
                    else if (row == flw::theme::_NAME_GLEAM_DARK) {
                        flw::theme::_load_gleam_dark();
                    }
                    else if (row == flw::theme::_NAME_GLEAM_DARKER) {
                        flw::theme::_load_gleam_darker();
                    }
                    else if (row == flw::theme::_NAME_GLEAM_TAN) {
                        flw::theme::_load_gleam_tan();
                    }
                    else if (row == flw::theme::_NAME_GTK) {
                        flw::theme::_load_gtk();
                    }
                    else if (row == flw::theme::_NAME_GTK_BLUE) {
                        flw::theme::_load_gtk_blue();
                    }
                    else if (row == flw::theme::_NAME_GTK_DARK_BLUE) {
                        flw::theme::_load_gtk_dark_blue();
                    }
                    else if (row == flw::theme::_NAME_GTK_DARK) {
                        flw::theme::_load_gtk_dark();
                    }
                    else if (row == flw::theme::_NAME_GTK_DARKER) {
                        flw::theme::_load_gtk_darker();
                    }
                    else if (row == flw::theme::_NAME_GTK_TAN) {
                        flw::theme::_load_gtk_tan();
                    }
                    else if (row == flw::theme::_NAME_PLASTIC) {
                        flw::theme::_load_plastic();
                    }
                    else if (row == flw::theme::_NAME_PLASTIC_BLUE) {
                        flw::theme::_load_blue_plastic();
                    }
                    else if (row == flw::theme::_NAME_PLASTIC_TAN) {
                        flw::theme::_load_tan_plastic();
                    }
                    else if (row == flw::theme::_NAME_SYSTEM) {
                        flw::theme::_load_system();
                    }
                    else {
                        flw::theme::_load_default();
                    }

                    dlg->update_pref();
                }
                else if (w == dlg->_close) {
                    dlg->hide();
                }
            }

            //------------------------------------------------------------------
            void resize(int X, int Y, int W, int H) override {
                auto fs = flw::PREF_FONTSIZE;

                Fl_Double_Window::resize(X, Y, W, H);

                _theme->resize           (4,                 4,                  W - 8,     H - fs * 6 - 24);
                _font_label->resize      (4,                 H - fs * 6 - 16,    W - 8,     fs * 2);
                _fixedfont_label->resize (4,                 H - fs * 4 - 12,    W - 8,     fs * 2);
                _font->resize            (W - fs * 24 - 12,  H - fs * 2 - 4,     fs * 8,    fs * 2);
                _fixedfont->resize       (W - fs * 16 - 8,   H - fs * 2 - 4,     fs * 8,    fs * 2);
                _close->resize           (W - fs * 8 - 4,    H - fs * 2 - 4,     fs * 8,    fs * 2);
            }

            //------------------------------------------------------------------
            void run() {
                show();

                while (visible()) {
                    Fl::wait();
                    Fl::flush();
                }
            }

            //------------------------------------------------------------------
            void update_pref() {
                util::labelfont(this);
                _font_label->copy_label(flw::util::format("%s - %d", flw::PREF_FONTNAME.c_str(), flw::PREF_FONTSIZE).c_str());
                _fixedfont_label->copy_label(flw::util::format("%s - %d", flw::PREF_FIXED_FONTNAME.c_str(), flw::PREF_FIXED_FONTSIZE).c_str());
                _fixedfont_label->labelfont(flw::PREF_FIXED_FONT);
                _fixedfont_label->labelsize(flw::PREF_FIXED_FONTSIZE);
                _theme->textsize(flw::PREF_FONTSIZE);
                size(flw::PREF_FONTSIZE * 32, flw::PREF_FONTSIZE * 30);
                theme::_scrollbar();

                for (int f = 0; f <= flw::theme::_NAME_SYSTEM; f++) {
                    if (theme::_NAME == flw::theme::_NAMES[f]) {
                        _theme->value(f + 1);
                        break;
                    }

                }

                Fl::redraw();
            }
        };
    }
}

//------------------------------------------------------------------------------
void flw::dlg::theme(bool enable_font, bool enable_fixedfont, Fl_Window* parent) {
    auto dlg = dlg::_DlgTheme(enable_font, enable_fixedfont, parent);
    dlg.run();
}

//------------------------------------------------------------------------------
bool flw::theme::is_dark() {
    if (theme::_NAME == flw::theme::_NAMES[_NAME_GLEAM_DARK_BLUE] ||
        flw::theme::_NAME == flw::theme::_NAMES[_NAME_GLEAM_DARK] ||
        flw::theme::_NAME == flw::theme::_NAMES[_NAME_GLEAM_DARKER] ||
        flw::theme::_NAME == flw::theme::_NAMES[_NAME_GTK_DARK_BLUE] ||
        flw::theme::_NAME == flw::theme::_NAMES[_NAME_GTK_DARK] ||
        flw::theme::_NAME == flw::theme::_NAMES[_NAME_GTK_DARKER]) {
        return true;
    }
    else {
        return false;
    }
}

//------------------------------------------------------------------------------
bool flw::theme::load(std::string name) {
    if (name == flw::theme::_NAMES[_NAME_DEFAULT]) {
        flw::theme::_load_default();
    }
    else if (name == flw::theme::_NAMES[_NAME_OXY]) {
        flw::theme::_load_oxy();
    }
    else if (name == flw::theme::_NAMES[_NAME_OXY_BLUE]) {
        flw::theme::_load_oxy_blue();
    }
    else if (name == flw::theme::_NAMES[_NAME_OXY_TAN]) {
        flw::theme::_load_oxy_tan();
    }
    else if (name == flw::theme::_NAMES[_NAME_GLEAM]) {
        flw::theme::_load_gleam();
    }
    else if (name == flw::theme::_NAMES[_NAME_GLEAM_BLUE]) {
        flw::theme::_load_gleam_blue();
    }
    else if (name == flw::theme::_NAMES[_NAME_GLEAM_DARK_BLUE]) {
        flw::theme::_load_gleam_dark_blue();
    }
    else if (name == flw::theme::_NAMES[_NAME_GLEAM_DARK]) {
        flw::theme::_load_gleam_dark();
    }
    else if (name == flw::theme::_NAMES[_NAME_GLEAM_DARKER]) {
        flw::theme::_load_gleam_darker();
    }
    else if (name == flw::theme::_NAMES[_NAME_GLEAM_TAN]) {
        flw::theme::_load_gleam_tan();
    }
    else if (name == flw::theme::_NAMES[_NAME_GTK]) {
        flw::theme::_load_gtk();
    }
    else if (name == flw::theme::_NAMES[_NAME_GTK_BLUE]) {
        flw::theme::_load_gtk_blue();
    }
    else if (name == flw::theme::_NAMES[_NAME_GTK_DARK_BLUE]) {
        flw::theme::_load_gtk_dark_blue();
    }
    else if (name == flw::theme::_NAMES[_NAME_GTK_DARK]) {
        flw::theme::_load_gtk_dark();
    }
    else if (name == flw::theme::_NAMES[_NAME_GTK_DARKER]) {
        flw::theme::_load_gtk_darker();
    }
    else if (name == flw::theme::_NAMES[_NAME_GTK_TAN]) {
        flw::theme::_load_gtk_tan();
    }
    else if (name == flw::theme::_NAMES[_NAME_PLASTIC]) {
        flw::theme::_load_plastic();
    }
    else if (name == flw::theme::_NAMES[_NAME_PLASTIC_BLUE]) {
        flw::theme::_load_blue_plastic();
    }
    else if (name == flw::theme::_NAMES[_NAME_PLASTIC_TAN]) {
        flw::theme::_load_tan_plastic();
    }
    else {
        return false;
    }

    theme::_scrollbar();
    return true;
}


//------------------------------------------------------------------------------
// Load icon before showing window
//
void flw::theme::load_icon(Fl_Window* win, int win_resource, const char** xpm_resource, const char* name) {
    assert(win);

    if (win->shown() != 0) {
        fl_alert("%s", "warning: load icon before showing window!");
    }

#if defined(_WIN32)
    win->icon((char*) LoadIcon(fl_display, MAKEINTRESOURCE(win_resource)));
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

//------------------------------------------------------------------------------
// Load theme and font data
//
void flw::theme::load_theme_pref(Fl_Preferences& pref) {
    auto val = 0;
    char buffer[4000];

    pref.get("fontname", buffer, "", 4000);

    if (*buffer != 0 && strcmp("FL_HELVETICA", buffer) != 0) {
        auto font = dlg::FontDialog::LoadFont(buffer);

        if (font != -1) {
            flw::PREF_FONT     = font;
            flw::PREF_FONTNAME = buffer;
        }
    }

    pref.get("fontsize", val, flw::PREF_FONTSIZE);

    if (val >= 6 && val <= 72) {
        flw::PREF_FONTSIZE = val;
    }

    pref.get("fixedfontname", buffer, "", 1000);

    if (*buffer != 0 && strcmp("FL_COURIER", buffer) != 0) {
        auto font = dlg::FontDialog::LoadFont(buffer);

        if (font != -1) {
            flw::PREF_FIXED_FONT     = font;
            flw::PREF_FIXED_FONTNAME = buffer;
        }
    }

    pref.get("fixedfontsize", val, flw::PREF_FIXED_FONTSIZE);

    if (val >= 6 && val <= 72) {
        flw::PREF_FIXED_FONTSIZE = val;
    }

    pref.get("theme", buffer, "gtk", 4000);
    flw::theme::load(buffer);
}

//------------------------------------------------------------------------------
// Load window size
//
void flw::theme::load_win_pref(Fl_Preferences& pref, Fl_Window* window, bool show, int defw, int defh, std::string basename) {
    assert(window);

    int  x, y, w, h, f;

    pref.get((basename + "x").c_str(), x, 80);
    pref.get((basename + "y").c_str(), y, 60);
    pref.get((basename + "w").c_str(), w, defw);
    pref.get((basename + "h").c_str(), h, defh);
    pref.get((basename + "fullscreen").c_str(), f, 0);

    if (w == 0 || h == 0) {
        w = 800;
        h = 600;
    }

    if (x + w <= 0 || y + h <= 0 || x >= Fl::w() || y >= Fl::h()) {
        x = 80;
        y = 60;
    }

    if (show == true && window->shown() == 0) {
        window->resize(x, y, w, h);
        window->show();
    }
    else {
        window->resize(x, y, w, h);
    }

    if (f == 1) {
        window->fullscreen();
    }
}

//------------------------------------------------------------------------------
std::string flw::theme::name() {
    return _NAME;
}

//------------------------------------------------------------------------------
bool flw::theme::parse(int argc, const char** argv) {
    auto res = false;

    for (auto f = 1; f < argc; f++) {
        if (res == false) {
            res = flw::theme::load(argv[f]);
        }

        auto fontsize = util::to_int(argv[f]);

        if (fontsize >= 6 && fontsize <= 72) {
            flw::PREF_FONTSIZE = fontsize;
        }
    }

    flw::PREF_FIXED_FONTSIZE = flw::PREF_FONTSIZE;
    return res;
}

//------------------------------------------------------------------------------
// Save theme and font data
//
void flw::theme::save_theme_pref(Fl_Preferences& pref) {
    pref.set("theme", flw::theme::name().c_str());
    pref.set("fontname", flw::PREF_FONTNAME.c_str());
    pref.set("fontsize", flw::PREF_FONTSIZE);
    pref.set("fixedfontname", flw::PREF_FIXED_FONTNAME.c_str());
    pref.set("fixedfontsize", flw::PREF_FIXED_FONTSIZE);
}

//------------------------------------------------------------------------------
// Save window size
//
void flw::theme::save_win_pref(Fl_Preferences& pref, Fl_Window* window, std::string basename) {
    assert(window);

    pref.set((basename + "x").c_str(), window->x());
    pref.set((basename + "y").c_str(), window->y());
    pref.set((basename + "w").c_str(), window->w());
    pref.set((basename + "h").c_str(), window->h());
    pref.set((basename + "fullscreen").c_str(), window->fullscreen_active() ? 1 : 0);
}

// MKALGAM_OFF
