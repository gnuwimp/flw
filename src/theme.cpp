// Copyright 2016 - 2021 gnuwimp@gmail.com
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

// MKALGAM_ON

namespace flw {
    namespace theme {
        static unsigned char _OLD_R[256];
        static unsigned char _OLD_G[256];
        static unsigned char _OLD_B[256];
        static unsigned char _SYS_R[256];
        static unsigned char _SYS_G[256];
        static unsigned char _SYS_B[256];
        static bool          _SAVED_COLOR = false;
        static bool          _SAVED_SYS   = false;

        static const char*   _NAMES[] = {
                "default",
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

        static std::string _NAME = _NAMES[_NAME_DEFAULT];

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
        static void _darker_colors() {
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
        static void _dark_blue_colors() {
            Fl::set_color(0,   255, 255, 255); // FL_FOREGROUND_COLOR
            Fl::set_color(7,    31,  47,  55); // FL_BACKGROUND2_COLOR
            Fl::set_color(8,   108, 113, 125); // FL_INACTIVE_COLOR
            Fl::set_color(15,   68, 138, 255); // FL_SELECTION_COLOR
            Fl::set_color(56,    0,   0,   0); // FL_BLACK
            Fl::background(69, 85, 92);
        }

        //----------------------------------------------------------------------
        static void _dark_colors() {
            Fl::set_color(0,   240, 240, 240); // FL_FOREGROUND_COLOR
            Fl::set_color(7,    55,  55,  55); // FL_BACKGROUND2_COLOR
            Fl::set_color(8,   100, 100, 100); // FL_INACTIVE_COLOR
            Fl::set_color(15,  149,  75,  37); // FL_SELECTION_COLOR
            Fl::set_color(56,    0,   0,   0); // FL_BLACK
            Fl::background(85, 85, 85);
        }

        //----------------------------------------------------------------------
        static void _dark_colors2() {
            Fl::set_color(0,   164, 164, 164); // FL_FOREGROUND_COLOR
            Fl::set_color(7,    16,  16,  16); // FL_BACKGROUND2_COLOR
            Fl::set_color(8,   100, 100, 100); // FL_INACTIVE_COLOR
            Fl::set_color(15,  133,  59,  21); // FL_SELECTION_COLOR
            Fl::set_color(56,    0,   0,   0); // FL_BLACK
            Fl::background(32, 32, 32);
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
                    Fl::set_color(f, theme::_OLD_R[f], theme::_OLD_G[f], theme::_OLD_B[f]);
                }
            }
        }

        //----------------------------------------------------------------------
        static void _restore_sys() {
            if (_SAVED_SYS == true) {
                for (int f = 0; f < 256; f++) {
                    Fl::set_color(f, theme::_SYS_R[f], theme::_SYS_G[f], theme::_SYS_B[f]);
                }
            }
        }

        //----------------------------------------------------------------------
        static void _save_colors() {
            if (_SAVED_COLOR == false) {
                for (int f = 0; f < 256; f++) {
                    unsigned char r1, g1, b1;
                    Fl::get_color(f, r1, g1, b1);
                    theme::_OLD_R[f] = r1;
                    theme::_OLD_G[f] = g1;
                    theme::_OLD_B[f] = b1;
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
                    theme::_SYS_R[f] = r1;
                    theme::_SYS_G[f] = g1;
                    theme::_SYS_B[f] = b1;
                }

                _SAVED_SYS = true;
            }
        }

        //------------------------------------------------------------------------------
        static void _load_default() {
            flw::theme::_save_colors();
            flw::theme::_restore_colors();
            Fl::set_color(FL_SELECTION_COLOR, 0, 120, 200);
            Fl::scheme("none");
            Fl::redraw();
            _NAME = _NAMES[_NAME_DEFAULT];
            flw::PREF_IS_DARK = false;
        }

        //------------------------------------------------------------------------------
        static void _load_gleam() {
            flw::theme::_save_colors();
            flw::theme::_restore_colors();
            Fl::scheme("gleam");
            Fl::set_color(FL_SELECTION_COLOR, 0, 120, 200);
            Fl::redraw();
            _NAME = _NAMES[_NAME_GLEAM];
            flw::PREF_IS_DARK = false;
        }

        //------------------------------------------------------------------------------
        static void _load_gleam_blue() {
            flw::theme::_save_colors();
            flw::theme::_restore_colors();
            flw::theme::_blue_colors();
            Fl::scheme("gleam");
            Fl::redraw();
            _NAME = _NAMES[_NAME_GLEAM_BLUE];
            flw::PREF_IS_DARK = false;
        }

        //------------------------------------------------------------------------------
        static void _load_gleam_dark() {
            flw::theme::_save_colors();
            flw::theme::_restore_colors();
            flw::theme::_dark_colors();
            flw::theme::_darker_colors();
            Fl::set_color(255, 125, 125, 125);
            Fl::scheme("gleam");
            Fl::redraw();
            _NAME = _NAMES[_NAME_GLEAM_DARK];
            flw::PREF_IS_DARK = true;
        }

        //------------------------------------------------------------------------------
        static void _load_gleam_darker() {
            flw::theme::_save_colors();
            flw::theme::_restore_colors();
            flw::theme::_dark_colors2();
            flw::theme::_darker_colors();
            Fl::set_color(255, 125, 125, 125);
            Fl::scheme("gleam");
            Fl::redraw();
            _NAME = _NAMES[_NAME_GLEAM_DARKER];
            flw::PREF_IS_DARK = true;
        }

        //------------------------------------------------------------------------------
        static void _load_gleam_dark_blue() {
            flw::theme::_save_colors();
            flw::theme::_restore_colors();
            flw::theme::_dark_blue_colors();
            flw::theme::_darker_colors();
            Fl::set_color(255, 101, 117, 125);
            Fl::scheme("gleam");
            Fl::redraw();
            _NAME = _NAMES[_NAME_GLEAM_DARK_BLUE];
            flw::PREF_IS_DARK = true;
        }

        //------------------------------------------------------------------------------
        static void _load_gleam_tan() {
            flw::theme::_save_colors();
            flw::theme::_restore_colors();
            flw::theme::_tan_colors();
            Fl::scheme("gleam");
            Fl::redraw();
            _NAME = _NAMES[_NAME_GLEAM_TAN];
            flw::PREF_IS_DARK = false;
        }

        //------------------------------------------------------------------------------
        static void _load_gtk() {
            flw::theme::_save_colors();
            flw::theme::_restore_colors();
            Fl::scheme("gtk+");
            Fl::set_color(FL_SELECTION_COLOR, 0, 120, 200);
            Fl::redraw();
            _NAME = _NAMES[_NAME_GTK];
            flw::PREF_IS_DARK = false;
        }

        //------------------------------------------------------------------------------
        static void _load_gtk_blue() {
            flw::theme::_save_colors();
            flw::theme::_restore_colors();
            flw::theme::_blue_colors();
            Fl::scheme("gtk+");
            Fl::redraw();
            _NAME = _NAMES[_NAME_GTK_BLUE];
            flw::PREF_IS_DARK = false;
        }

        //------------------------------------------------------------------------------
        static void _load_gtk_dark() {
            flw::theme::_save_colors();
            flw::theme::_restore_colors();
            flw::theme::_dark_colors();
            flw::theme::_darker_colors();
            Fl::set_color(255, 185, 185, 185);
            Fl::scheme("gtk+");
            Fl::redraw();
            _NAME = _NAMES[_NAME_GTK_DARK];
            flw::PREF_IS_DARK = true;
        }

        //------------------------------------------------------------------------------
        static void _load_gtk_darker() {
            flw::theme::_save_colors();
            flw::theme::_restore_colors();
            flw::theme::_dark_colors2();
            flw::theme::_darker_colors();
            Fl::set_color(255, 125, 125, 125);
            Fl::scheme("gtk+");
            Fl::redraw();
            _NAME = _NAMES[_NAME_GTK_DARKER];
            flw::PREF_IS_DARK = true;
        }

        //------------------------------------------------------------------------------
        static void _load_gtk_dark_blue() {
            flw::theme::_save_colors();
            flw::theme::_restore_colors();
            flw::theme::_dark_blue_colors();
            flw::theme::_darker_colors();
            Fl::set_color(255, 161, 177, 185);
            Fl::scheme("gtk+");
            Fl::redraw();
            _NAME = _NAMES[_NAME_GTK_DARK_BLUE];
            flw::PREF_IS_DARK = true;
        }

        //------------------------------------------------------------------------------
        static void _load_gtk_tan() {
            flw::theme::_save_colors();
            flw::theme::_restore_colors();
            flw::theme::_tan_colors();
            Fl::scheme("gtk+");
            Fl::redraw();
            _NAME = _NAMES[_NAME_GTK_TAN];
            flw::PREF_IS_DARK = false;
        }

        //------------------------------------------------------------------------------
        static void _load_plastic() {
            flw::theme::_save_colors();
            flw::theme::_restore_colors();
            Fl::set_color(FL_SELECTION_COLOR, 0, 120, 200);
            Fl::scheme("plastic");
            Fl::redraw();
            _NAME = _NAMES[_NAME_PLASTIC];
            flw::PREF_IS_DARK = false;
        }

        //------------------------------------------------------------------------------
        static void _load_blue_plastic() {
            flw::theme::_save_colors();
            flw::theme::_restore_colors();
            flw::theme::_blue_colors();
            Fl::scheme("plastic");
            Fl::redraw();
            _NAME = _NAMES[_NAME_PLASTIC_BLUE];
            flw::PREF_IS_DARK = false;
        }

        //------------------------------------------------------------------------------
        static void _load_tan_plastic() {
            flw::theme::_save_colors();
            flw::theme::_restore_colors();
            flw::theme::_tan_colors();
            Fl::scheme("plastic");
            Fl::redraw();
            _NAME = _NAMES[_NAME_PLASTIC_TAN];
            flw::PREF_IS_DARK = false;
        }

        //------------------------------------------------------------------------------
        static void _load_system() {
            flw::theme::_save_colors();
            flw::theme::_restore_colors();
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
            flw::PREF_IS_DARK = false;
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

                for (int f = 0; f <= theme::_NAME_SYSTEM; f++) {
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

                    if (fd.run(Fl::first_window())) {
                        flw::PREF_FIXED_FONT     = fd.font();
                        flw::PREF_FIXED_FONTSIZE = fd.fontsize();
                        flw::PREF_FIXED_FONTNAME = fd.fontname();
                        dlg->update_pref();
                    }
                }
                else if (w == dlg->_font) {
                    flw::dlg::FontDialog fd(flw::PREF_FONT, flw::PREF_FONTSIZE, "Select Font");

                    if (fd.run(Fl::first_window())) {
                        flw::PREF_FONT     = fd.font();
                        flw::PREF_FONTSIZE = fd.fontsize();
                        flw::PREF_FONTNAME = fd.fontname();
                        dlg->update_pref();
                    }
                }
                else if (w == dlg->_theme) {
                    auto row = dlg->_theme->value() - 1;

                    if (row == theme::_NAME_GLEAM) {
                        theme::_load_gleam();
                    }
                    else if (row == theme::_NAME_GLEAM_BLUE) {
                        theme::_load_gleam_blue();
                    }
                    else if (row == theme::_NAME_GLEAM_DARK_BLUE) {
                        theme::_load_gleam_dark_blue();
                    }
                    else if (row == theme::_NAME_GLEAM_DARK) {
                        theme::_load_gleam_dark();
                    }
                    else if (row == theme::_NAME_GLEAM_DARKER) {
                        theme::_load_gleam_darker();
                    }
                    else if (row == theme::_NAME_GLEAM_TAN) {
                        theme::_load_gleam_tan();
                    }
                    else if (row == theme::_NAME_GTK) {
                        theme::_load_gtk();
                    }
                    else if (row == theme::_NAME_GTK_BLUE) {
                        theme::_load_gtk_blue();
                    }
                    else if (row == theme::_NAME_GTK_DARK_BLUE) {
                        theme::_load_gtk_dark_blue();
                    }
                    else if (row == theme::_NAME_GTK_DARK) {
                        theme::_load_gtk_dark();
                    }
                    else if (row == theme::_NAME_GTK_DARKER) {
                        theme::_load_gtk_darker();
                    }
                    else if (row == theme::_NAME_GTK_TAN) {
                        theme::_load_gtk_tan();
                    }
                    else if (row == theme::_NAME_PLASTIC) {
                        theme::_load_plastic();
                    }
                    else if (row == theme::_NAME_PLASTIC_BLUE) {
                        theme::_load_blue_plastic();
                    }
                    else if (row == theme::_NAME_PLASTIC_TAN) {
                        theme::_load_tan_plastic();
                    }
                    else if (row == theme::_NAME_SYSTEM) {
                        theme::_load_system();
                    }
                    else {
                        theme::_load_default();
                    }

                    Fl::redraw();
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
                flw::util::labelfont(this);
                _font_label->copy_label(flw::util::format("%s - %d", flw::PREF_FONTNAME.c_str(), flw::PREF_FONTSIZE).c_str());
                _fixedfont_label->copy_label(flw::util::format("%s - %d", flw::PREF_FIXED_FONTNAME.c_str(), flw::PREF_FIXED_FONTSIZE).c_str());
                _fixedfont_label->labelfont(flw::PREF_FIXED_FONT);
                _fixedfont_label->labelsize(flw::PREF_FIXED_FONTSIZE);
                _theme->textsize(flw::PREF_FONTSIZE);
                size(flw::PREF_FONTSIZE * 32, flw::PREF_FONTSIZE * 30);

                for (int f = 0; f <= theme::_NAME_SYSTEM; f++) {
                    if (theme::_NAME == theme::_NAMES[f]) {
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
bool flw::theme::is_dark() {
    if (theme::_NAME == theme::_NAMES[_NAME_GLEAM_DARK_BLUE] ||
        theme::_NAME == theme::_NAMES[_NAME_GLEAM_DARK] ||
        theme::_NAME == theme::_NAMES[_NAME_GLEAM_DARKER] ||
        theme::_NAME == theme::_NAMES[_NAME_GTK_DARK_BLUE] ||
        theme::_NAME == theme::_NAMES[_NAME_GTK_DARK] ||
        theme::_NAME == theme::_NAMES[_NAME_GTK_DARKER]) {
        return true;
    }
    else {
        return false;
    }
}

//------------------------------------------------------------------------------
bool flw::theme::load(const std::string& name) {
    if (name == theme::_NAMES[_NAME_DEFAULT]) {
        theme::_load_default();
    }
    else if (name == theme::_NAMES[_NAME_GLEAM]) {
        theme::_load_gleam();
    }
    else if (name == theme::_NAMES[_NAME_GLEAM_BLUE]) {
        theme::_load_gleam_blue();
    }
    else if (name == theme::_NAMES[_NAME_GLEAM_DARK_BLUE]) {
        theme::_load_gleam_dark_blue();
    }
    else if (name == theme::_NAMES[_NAME_GLEAM_DARK]) {
        theme::_load_gleam_dark();
    }
    else if (name == theme::_NAMES[_NAME_GLEAM_DARKER]) {
        theme::_load_gleam_darker();
    }
    else if (name == theme::_NAMES[_NAME_GLEAM_TAN]) {
        theme::_load_gleam_tan();
    }
    else if (name == theme::_NAMES[_NAME_GTK]) {
        theme::_load_gtk();
    }
    else if (name == theme::_NAMES[_NAME_GTK_BLUE]) {
        theme::_load_gtk_blue();
    }
    else if (name == theme::_NAMES[_NAME_GTK_DARK_BLUE]) {
        theme::_load_gtk_dark_blue();
    }
    else if (name == theme::_NAMES[_NAME_GTK_DARK]) {
        theme::_load_gtk_dark();
    }
    else if (name == theme::_NAMES[_NAME_GTK_DARKER]) {
        theme::_load_gtk_darker();
    }
    else if (name == theme::_NAMES[_NAME_GTK_TAN]) {
        theme::_load_gtk_tan();
    }
    else if (name == theme::_NAMES[_NAME_PLASTIC]) {
        theme::_load_plastic();
    }
    else if (name == theme::_NAMES[_NAME_PLASTIC_BLUE]) {
        theme::_load_blue_plastic();
    }
    else if (name == theme::_NAMES[_NAME_PLASTIC_TAN]) {
        theme::_load_tan_plastic();
    }
    else {
        return false;
    }

    return true;
}

//------------------------------------------------------------------------------
std::string flw::theme::name() {
    return _NAME;
}

//------------------------------------------------------------------------------
bool flw::theme::parse(int argc, const char** argv) {
    auto res = false;

    for (auto f = 0; f < argc; f++) {
        auto arg = std::string(argv[f]);

        if (res == false && f) {
            res = flw::theme::load(arg);
        }

        if (arg == "8") {
            flw::PREF_FONTSIZE = 8;
        }
        else if (arg == "9") {
            flw::PREF_FONTSIZE = 9;
        }
        else if (arg == "10") {
            flw::PREF_FONTSIZE = 10;
        }
        else if (arg == "12") {
            flw::PREF_FONTSIZE = 12;
        }
        else if (arg == "14") {
            flw::PREF_FONTSIZE = 14;
        }
        else if (arg == "16") {
            flw::PREF_FONTSIZE = 16;
        }
        else if (arg == "18") {
            flw::PREF_FONTSIZE = 18;
        }
        else if (arg == "20") {
            flw::PREF_FONTSIZE = 20;
        }
        else if (arg == "24") {
            flw::PREF_FONTSIZE = 24;
        }
        else if (arg == "28") {
            flw::PREF_FONTSIZE = 28;
        }
        else if (arg == "36") {
            flw::PREF_FONTSIZE = 36;
        }
    }

    flw::PREF_FIXED_FONTSIZE = flw::PREF_FONTSIZE;
    return res;
}

//------------------------------------------------------------------------------
void flw::dlg::theme(bool enable_font, bool enable_fixedfont, Fl_Window* parent) {
    auto dlg = dlg::_DlgTheme(enable_font, enable_fixedfont, parent);
    dlg.run();
}

//------------------------------------------------------------------------------
// Load gui preferences and if window is set resize and show it
//
void flw::util::load_pref(Fl_Preferences& pref, Fl_Window* window) {
    auto val = 0;
    char buffer[4000];

    if (window != nullptr) {
        int x, y, w, h, f;

        pref.get("gui.x", x, 80);
        pref.get("gui.y", y, 60);
        pref.get("gui.w", w, 800);
        pref.get("gui.h", h, 600);
        pref.get("gui.fullscreen", f, 0);

        window->resize(x, y, w, h);
        window->show();
        window->resize(x, y, w, h);

        if (f == 1) {
            window->fullscreen();
        }
    }

    pref.get("gui.fontname", buffer, "", 4000);

    if (*buffer != 0 && strcmp("FL_HELVETICA", buffer) != 0) {
        auto font = dlg::FontDialog::LoadFont(buffer);

        if (font != -1) {
            flw::PREF_FONT     = font;
            flw::PREF_FONTNAME = buffer;
        }
    }

    pref.get("gui.fontsize", val, flw::PREF_FONTSIZE);

    if (val >= 6 && val <= 72) {
        flw::PREF_FONTSIZE = val;
    }

    pref.get("gui.fixedfontname", buffer, "", 1000);

    if (*buffer != 0 && strcmp("FL_COURIER", buffer) != 0) {
        auto font = dlg::FontDialog::LoadFont(buffer);

        if (font != -1) {
            flw::PREF_FIXED_FONT     = font;
            flw::PREF_FIXED_FONTNAME = buffer;
        }
    }

    pref.get("gui.fixedfontsize", val, flw::PREF_FIXED_FONTSIZE);

    if (val >= 6 && val <= 72) {
        flw::PREF_FIXED_FONTSIZE = val;
    }

    pref.get("gui.theme", buffer, "gtk", 4000);
    theme::load(buffer);
}

//------------------------------------------------------------------------------
// Save theme and fontnames and optionally window size
//
void flw::util::save_pref(Fl_Preferences& pref, Fl_Window* window) {
    if (window != nullptr) {
        pref.set("gui.x", window->x());
        pref.set("gui.y", window->y());
        pref.set("gui.w", window->w());
        pref.set("gui.h", window->h());
        pref.set("gui.fullscreen", window->fullscreen_active() ? 1 : 0);
    }

    pref.set("gui.theme", flw::theme::name().c_str());
    pref.set("gui.fontname", flw::PREF_FONTNAME.c_str());
    pref.set("gui.fontsize", flw::PREF_FONTSIZE);
    pref.set("gui.fixedfontname", flw::PREF_FIXED_FONTNAME.c_str());
    pref.set("gui.fixedfontsize", flw::PREF_FIXED_FONTSIZE);
}

// MKALGAM_OFF
