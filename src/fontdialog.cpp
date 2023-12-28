// Copyright 2021 - 2022 gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#include "fontdialog.h"
#include "util.h"
#include "widgets.h"
#include <FL/fl_ask.H>
#include <FL/fl_draw.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Hold_Browser.H>

// MKALGAM_ON

namespace flw {
    namespace dlg {
        static std::vector<char*>   _FONTDIALOG_NAMES;
        static const std::string    _FONTDIALOG_LABEL = R"(
ABCDEFGHIJKLMNOPQRSTUVWXYZ /0123456789
abcdefghijklmnopqrstuvwxyz £©µÀÆÖÞßéöÿ
–—‘“”„†•…‰™œŠŸž€ ΑΒΓΔΩαβγδω АБВГДабвгд
∀∂∈ℝ∧∪≡∞ ↑↗↨↻⇣ ┐┼╔╘░►☺♀ ﬁ�⑀₂ἠḂӥẄɐː⍎אԱა

Hello world, Καλημέρα κόσμε, コンニチハ

math: ∮ E⋅da = Q,  n → ∞, ∑ f(i) 2H₂ + O₂ ⇌ 2H₂O, R = 4.7 kΩ
korean: 구두의
greek: Οὐχὶ ταὐτὰ παρίσταταί μοι γιγνώσκειν
russian: Зарегистрируйтесь сейчас
thai: แผ่นดินฮั่นเสื่อมโทรมแสนสังเวช
amharic: ሰማይ አይታረስ ንጉሥ አይከሰስ
braille: ⡌⠁⠧⠑ ⠼⠁⠒  ⡍⠜⠇⠑⠹⠰⠎ ⡣⠕⠌

“There is nothing else than now.
There is neither yesterday, certainly,
nor is there any tomorrow.
How old must you be before you know that?
There is only now, and if now is only two days,
then two days is your life and everything in it will be in proportion.
This is how you live a life in two days.
And if you stop complaining and asking for what you never will get,
you will have a good life.
A good life is not measured by any biblical span.”
― Ernest Hemingway, For Whom the Bell Tolls
)";

        //----------------------------------------------------------------------
        class _FontDialogLabel : public Fl_Box {
        public:
            int font;
            int size;

            //------------------------------------------------------------------
            _FontDialogLabel(int x, int y, int w, int h) : Fl_Box(x, y, w, h, _FONTDIALOG_LABEL.c_str()) {
                font = FL_HELVETICA;
                size = 14;

                align(FL_ALIGN_TOP | FL_ALIGN_LEFT | FL_ALIGN_INSIDE | FL_ALIGN_CLIP);
                box(FL_BORDER_BOX);
                color(FL_BACKGROUND2_COLOR);
            }

            //------------------------------------------------------------------
            void draw() override {
                draw_box();
                fl_font((Fl_Font) font, size);
                fl_color(FL_FOREGROUND_COLOR);
                fl_draw(label(), x() + 3, y() + 3, w() - 6, h() - 6, align());
            }
        };
    }
}

//------------------------------------------------------------------------------
flw::dlg::FontDialog::FontDialog(Fl_Font font, Fl_Fontsize fontsize, const std::string& label) :
Fl_Double_Window(0, 0, flw::PREF_FONTSIZE * 60, flw::PREF_FONTSIZE * 36) {
    _create(font, "", fontsize, label);
}

//------------------------------------------------------------------------------
flw::dlg::FontDialog::FontDialog(std::string font, Fl_Fontsize fontsize, std::string label) :
Fl_Double_Window(0, 0, flw::PREF_FONTSIZE * 60, flw::PREF_FONTSIZE * 36) {
    _create(0, font, fontsize, label);
}

//------------------------------------------------------------------------------
void flw::dlg::FontDialog::_activate() {
    if (_fonts->value() == 0 || _sizes->value() == 0) {
        _select->deactivate();
    }
    else {
        _select->activate();
    }
}

//------------------------------------------------------------------------------
void flw::dlg::FontDialog::Callback(Fl_Widget* w, void* o) {
    FontDialog* dlg = (FontDialog*) o;

    if (w == dlg) {
        dlg->hide();
    }
    else if (w == dlg->_cancel) {
        dlg->hide();
    }
    else if (w == dlg->_fonts) {
        auto row = dlg->_fonts->value();

        if (row > 0) {
            ((_FontDialogLabel*) dlg->_label)->font = row - 1;
        }

        dlg->_activate();
        Fl::redraw();
    }
    else if (w == dlg->_select) {
        auto row1 = dlg->_fonts->value();
        auto row2 = dlg->_sizes->value();

        if (row1 > 0 && row2 > 0) {
            row1--;

            dlg->_fontname = ScrollBrowser::RemoveFormat(_FONTDIALOG_NAMES[row1]);
            dlg->_font     = row1;
            dlg->_fontsize = row2 + 5;
            dlg->_ret      = true;

            dlg->hide();
        }
    }
    else if (w == dlg->_sizes) {
        auto row = dlg->_sizes->value();

        if (row > 0) {
            ((_FontDialogLabel*) dlg->_label)->size = row + 5;
        }

        dlg->_activate();
        Fl::redraw();
    }
}

//------------------------------------------------------------------------------
void flw::dlg::FontDialog::_create(Fl_Font font, std::string fontname, Fl_Fontsize fontsize, std::string label) {
    end();

    _cancel   = new Fl_Button(0, 0, 0, 0, "&Cancel");
    _fonts    = new flw::ScrollBrowser(12);
    _label    = new flw::dlg::_FontDialogLabel(0, 0, 0, 0);
    _select   = new Fl_Button(0, 0, 0, 0, "&Select");
    _sizes    = new flw::ScrollBrowser(6);
    _font     = -1;
    _fontsize = -1;
    _ret      = false;

    add(_sizes);
    add(_fonts);
    add(_select);
    add(_cancel);
    add(_label);

    _cancel->callback(flw::dlg::FontDialog::Callback, this);
    _cancel->labelfont(flw::PREF_FONT);
    _cancel->labelsize(flw::PREF_FONTSIZE);
    _fonts->callback(flw::dlg::FontDialog::Callback, this);
    _fonts->textsize(flw::PREF_FONTSIZE);
    _fonts->when(FL_WHEN_CHANGED);
    ((_FontDialogLabel*) _label)->font = font;
    ((_FontDialogLabel*) _label)->size = fontsize;
    _select->callback(flw::dlg::FontDialog::Callback, this);
    _select->labelfont(flw::PREF_FONT);
    _select->labelsize(flw::PREF_FONTSIZE);
    _sizes->callback(flw::dlg::FontDialog::Callback, this);
    _sizes->textsize(flw::PREF_FONTSIZE);
    _sizes->when(FL_WHEN_CHANGED);

    size(w(), h());
    FontDialog::LoadFonts();

    for (auto name : _FONTDIALOG_NAMES) {
        _fonts->add(name);
    }

    for (auto f = 6; f <= 72; f++) {
        char buf[50];
        snprintf(buf, 50, "@r%2d  ", f);
        _sizes->add(buf);
    }

    if (fontsize >= 6 && fontsize <= 72) {
        _sizes->value(fontsize - 5);
        _sizes->middleline(fontsize - 5);
        ((_FontDialogLabel*) _label)->font = fontsize;
    }
    else {
        _sizes->value(14 - 5);
        _sizes->middleline(14 - 5);
        ((_FontDialogLabel*) _label)->font = 14;
    }

    if (fontname != "") {
        _select_name(fontname);
    }
    else if (font >= 0 && font < _fonts->size()) {
        _fonts->value(font + 1);
        _fonts->middleline(font + 1);
        ((_FontDialogLabel*) _label)->font = font;
    }
    else {
        _fonts->value(1);
        _fonts->middleline(1);
    }

    resizable(this);
    copy_label(label.c_str());
    callback(flw::dlg::FontDialog::Callback, this);
    size_range(flw::PREF_FONTSIZE * 38, flw::PREF_FONTSIZE * 12);
    set_modal();
    _fonts->take_focus();
}

//------------------------------------------------------------------------------
void flw::dlg::FontDialog::DeleteFonts() {
    for (auto f : _FONTDIALOG_NAMES) {
        free(f);
    }

    _FONTDIALOG_NAMES.clear();
}

//------------------------------------------------------------------------------
int flw::dlg::FontDialog::LoadFont(std::string requested_font) {
    FontDialog::LoadFonts();

    auto count = 0;

    for (auto font : _FONTDIALOG_NAMES) {
        auto font2 = ScrollBrowser::RemoveFormat(font);

        if (requested_font == font2) {
            return count;
        }

        count++;
    }

    return -1;
}

//------------------------------------------------------------------------------
// Load fonts only once in the program
//
void flw::dlg::FontDialog::LoadFonts(bool iso8859_only) {
    if (_FONTDIALOG_NAMES.size() == 0) {
        auto fonts = Fl::set_fonts(iso8859_only ? nullptr : "-*");

        for (int f = 0; f < fonts; f++) {
            auto attr  = 0;
            auto name1 = Fl::get_font_name((Fl_Font) f, &attr);
            auto name2 = std::string();

            if (attr & FL_BOLD) {
                name2 = std::string("@b");
            }

            if (attr & FL_ITALIC) {
                name2 += std::string("@i");
            }

            name2 += std::string("@.");
            name2 += name1;
            _FONTDIALOG_NAMES.push_back(strdup(name2.c_str()));
            // printf("%3d - %-40s - %-40s\n", f, name1, name2.c_str()); fflush(stdout);
        }
    }
}

//------------------------------------------------------------------------------
void flw::dlg::FontDialog::resize(int X, int Y, int W, int H) {
    int fs = flw::PREF_FONTSIZE;

    Fl_Double_Window::resize(X, Y, W, H);

    _fonts->resize  (4,                 4,                  fs * 25,            H - fs * 2 - 16);
    _sizes->resize  (fs * 25 + 8,       4,                  fs * 6,             H - fs * 2 - 16);
    _label->resize  (fs * 31 + 12,      4,                  W - fs * 31 - 16,   H - fs * 2 - 16);
    _cancel->resize (W - fs * 16 - 8,   H - fs * 2 - 4,     fs * 8,             fs * 2);
    _select->resize (W - fs * 8 - 4,    H - fs * 2 - 4,     fs * 8,             fs * 2);
}

//------------------------------------------------------------------------------
bool flw::dlg::FontDialog::run(Fl_Window* parent) {
    _ret = false;

    _activate();
    flw::util::center_window(this, parent);
    show();

    while (visible() != 0) {
        Fl::wait();
        Fl::flush();
    }

    return _ret;
}

//------------------------------------------------------------------------------
void flw::dlg::FontDialog::_select_name(std::string fontname) {
    auto count = 1;

    for (auto font : _FONTDIALOG_NAMES) {
        auto font_without_style = ScrollBrowser::RemoveFormat(font);

        if (fontname == font_without_style) {
            _fonts->value(count);
            _fonts->middleline(count);
            ((_FontDialogLabel*) _label)->font = count - 1;
            return;
        }

        count++;
    }

    _fonts->value(1);
    ((_FontDialogLabel*) _label)->font = 0;
}

// MKALGAM_OFF
