/**
* @file
* @brief Font selection dialog.
*
* @author gnuwimp@gmail.com
* @copyright Released under the GNU General Public License v3.0
*/

#include "fontdialog.h"
#include "gridgroup.h"
#include "scrollbrowser.h"
#include "svgbutton.h"
#include "theme.h"

// MKALGAM_ON

#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/fl_draw.H>

/*
 *           ______          _   _           _          _
 *          |  ____|        | | | |         | |        | |
 *          | |__ ___  _ __ | |_| |     __ _| |__   ___| |
 *          |  __/ _ \| '_ \| __| |    / _` | '_ \ / _ \ |
 *          | | | (_) | | | | |_| |___| (_| | |_) |  __/ |
 *          |_|  \___/|_| |_|\__|______\__,_|_.__/ \___|_|
 *      ______
 *     |______|
 */

namespace flw {
namespace priv {

static const std::string _FONTDIALOG_LABEL = R"(
ABCDEFGHIJKLMNOPQRSTUVWXYZ /0123456789
abcdefghijklmnopqrstuvwxyz £©µÀÆÖÞßéöÿ
–—‘“”„†•…‰™œŠŸž€ ΑΒΓΔΩαβγδω АБВГДабвгд
∀∂∈ℝ∧∪≡∞ ↑↗↨↻⇣ ┐┼╔╘░►☺♀ ﬁ�⑀₂ἠḂӥẄɐː⍎אԱა

japanese: こんにちは世界
korean: 안녕하세요 세계
tibetan: ཨོཾ་མ་ཎི་པདྨེ་ཧཱུྃ
greek: Γειά σου Κόσμε
ukrainian: Привіт Світ
thai: สวัสดีชาวโลก
amharic: ሰላም ልዑል
braille: ⡌⠁⠧⠑ ⠼⠁⠒  ⡍⠜⠇⠑⠹⠰⠎ ⡣⠕⠌
math: ∮ E⋅da = Q,  n → ∞, ∑ f(i) 2H₂ + O₂ ⇌ 2H₂O, R = 4.7 kΩ

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

/** @brief Example text for current font.
*
*/
class _FontLabel : public Fl_Box {
public:
    int font;
    int size;

    /** @brief
    */
    _FontLabel(int x, int y, int w, int h) : Fl_Box(x, y, w, h, _FONTDIALOG_LABEL.c_str()) {
        font = FL_HELVETICA;
        size = 14;

        align(FL_ALIGN_TOP | FL_ALIGN_LEFT | FL_ALIGN_INSIDE | FL_ALIGN_CLIP);
        box(FL_BORDER_BOX);
        color(FL_BACKGROUND2_COLOR);
    }

    /** @brief
    */
    void draw() override {
        draw_box();
        fl_font((Fl_Font) font, size);
        fl_color(FL_FOREGROUND_COLOR);
        fl_draw(label(), x() + 3, y() + 3, w() - 6, h() - 6, align());
    }
};

} // flw::priv
} // flw

/*
 *      ______          _   _____  _       _
 *     |  ____|        | | |  __ \(_)     | |
 *     | |__ ___  _ __ | |_| |  | |_  __ _| | ___   __ _
 *     |  __/ _ \| '_ \| __| |  | | |/ _` | |/ _ \ / _` |
 *     | | | (_) | | | | |_| |__| | | (_| | | (_) | (_| |
 *     |_|  \___/|_| |_|\__|_____/|_|\__,_|_|\___/ \__, |
 *                                                  __/ |
 *                                                 |___/
 */

/** @brief Create font dialog.
*
* @param[in] font              Selected font.
* @param[in] fontsize          Font size.
* @param[in] title             Dialog title.
* @param[in] limit_to_default  Set to true to only display the default FLTK fonts.
*/
flw::FontDialog::FontDialog(Fl_Font font, Fl_Fontsize fontsize, const std::string& title, bool limit_to_default) :
Fl_Double_Window(0, 0, flw::PREF_FONTSIZE * 64, flw::PREF_FONTSIZE * 36) {
    _create(font, "", fontsize, title, limit_to_default);
}

/** @brief Create font dialog.
*
* @param[in] font              Selected font name.
* @param[in] fontsize          Font size.
* @param[in] title             Dialog title.
* @param[in] limit_to_default  Set to true to only display the default FLTK fonts.
*/
flw::FontDialog::FontDialog(const std::string& font, Fl_Fontsize fontsize, const std::string& title, bool limit_to_default) :
Fl_Double_Window(0, 0, flw::PREF_FONTSIZE * 64, flw::PREF_FONTSIZE * 36) {
    _create(0, font, fontsize, title, limit_to_default);
}

/** @brief Turn on font list, active by default.
*
*/
void flw::FontDialog::activate_font() {
    static_cast<Fl_Widget*>(_fonts)->activate();
}

/** @brief Turn on font size list, active by default.
*
*/
void flw::FontDialog::activate_font_size() {
    static_cast<Fl_Widget*>(_fonts)->activate();
}

/** @brief Turn of font list.
*
*/
void flw::FontDialog::deactivate_font() {
    static_cast<Fl_Widget*>(_fonts)->deactivate();
}

/** @brief Turn of font size list.
*
*/
void flw::FontDialog::deactivate_fontsize() {
    static_cast<Fl_Widget*>(_sizes)->deactivate();
}

/** @brief Activate select button.
*
*/
void flw::FontDialog::_activate() {
    if (_fonts->value() == 0 || _sizes->value() == 0 || (_fonts->active() == 0 && _sizes->active() == 0)) {
        _select->deactivate();
    }
    else {
        _select->activate();
    }
}

/** @brief Callback for all widgets.
*
* @param[in] w  Widget that activated callback.
* @param[in] o  This object.
*/
void flw::FontDialog::Callback(Fl_Widget* w, void* o) {
    auto self = static_cast<FontDialog*>(o);

    if (w == self || w == self->_cancel) {
        self->_run = false;
        self->hide();
    }
    else if (w == self->_fonts) {
        auto row = self->_fonts->value();

        if (row > 0) {
            static_cast<priv::_FontLabel*>(self->_label)->font = row - 1;
        }

        self->_activate();
        Fl::redraw();
    }
    else if (w == self->_select) {
        auto row1 = self->_fonts->value();
        auto row2 = self->_sizes->value();

        if (row1 > 0 && row2 > 0) {
            row1--;

            self->_fontname = util::remove_browser_format(util::to_string(flw::PREF_FONTNAMES[row1]));
            self->_font     = row1;
            self->_fontsize = row2 + 5;
            self->_ret      = true;
            self->_run      = false;

            self->hide();
        }
    }
    else if (w == self->_sizes) {
        auto row = self->_sizes->value();

        if (row > 0) {
            static_cast<priv::_FontLabel*>(self->_label)->size = row + 5;
        }

        self->_activate();
        Fl::redraw();
    }
}

/** @brief Create font dialog.
*
* @param[in] font              Selected font.
* @param[in] fontname          Font name.
* @param[in] fontsize          Font size.
* @param[in] title             Sample text.
* @param[in] limit_to_default  Set to true to only display the default FLTK fonts.
*/
void flw::FontDialog::_create(Fl_Font font, const std::string& fontname, Fl_Fontsize fontsize, const std::string& title, bool limit_to_default) {
    end();

    _cancel   = new SVGButton(0, 0, 0, 0, labels::CANCEL);
    _fonts    = new ScrollBrowser(12);
    _grid     = new GridGroup();
    _label    = new priv::_FontLabel(0, 0, 0, 0);
    _select   = new SVGButton(0, 0, 0, 0, labels::SELECT, icons::BACK, true);
    _sizes    = new ScrollBrowser(6);
    _font     = -1;
    _fontsize = -1;
    _ret      = false;
    _run      = false;

    _grid->add(_fonts,    1,   1,  50,  -6);
    _grid->add(_sizes,   52,   1,  12,  -6);
    _grid->add(_label,   65,   1,  -1,  -6);
    _grid->add(_cancel, -34,  -5,  16,   4);
    _grid->add(_select, -17,  -5,  16,   4);
    add(_grid);

    _cancel->callback(FontDialog::Callback, this);
    _cancel->labelfont(flw::PREF_FONT);
    _cancel->labelsize(flw::PREF_FONTSIZE);
    _fonts->box(FL_BORDER_BOX);
    _fonts->callback(FontDialog::Callback, this);
    _fonts->textsize(flw::PREF_FONTSIZE);
    _fonts->when(FL_WHEN_CHANGED);
    static_cast<priv::_FontLabel*>(_label)->font = font;
    static_cast<priv::_FontLabel*>(_label)->size = fontsize;
    _select->callback(FontDialog::Callback, this);
    _select->labelfont(flw::PREF_FONT);
    _select->labelsize(flw::PREF_FONTSIZE);
    _sizes->box(FL_BORDER_BOX);
    _sizes->callback(FontDialog::Callback, this);
    _sizes->textsize(flw::PREF_FONTSIZE);
    _sizes->when(FL_WHEN_CHANGED);

    flw::theme::load_fonts();

    auto count = 0;

    for (auto name : flw::PREF_FONTNAMES) {
        if (limit_to_default == true && count == 12) {
            break;
        }

        _fonts->add(name);
        count++;
    }

    for (auto f = 6; f <= 72; f++) {
        char buf[50];
        snprintf(buf, 50, "@r%2d  ", f);
        _sizes->add(buf);
    }

    if (fontsize >= 6 && fontsize <= 72) {
        _sizes->value(fontsize - 5);
        _sizes->middleline(fontsize - 5);
        static_cast<priv::_FontLabel*>(_label)->font = fontsize;
    }
    else {
        _sizes->value(14 - 5);
        _sizes->middleline(14 - 5);
        static_cast<priv::_FontLabel*>(_label)->font = 14;
    }

    if (fontname != "") {
        _select_name(fontname);
    }
    else if (font >= 0 && font < _fonts->size()) {
        _fonts->value(font + 1);
        _fonts->middleline(font + 1);
        static_cast<priv::_FontLabel*>(_label)->font = font;
    }
    else {
        _fonts->value(1);
        _fonts->middleline(1);
    }

    resizable(_grid);
    copy_label(title.c_str());
    callback(FontDialog::Callback, this);
    set_modal();
    _fonts->take_focus();
    _grid->resize(0, 0, w(), h());
}

/** @brief Show dialog in modal mode.
*
* @return True if select button has been pressed.
*/
bool flw::FontDialog::run() {
    _ret = false;
    _run = true;

    _activate();
    util::center_window(this, flw::PREF_WINDOW);
    show();

    while (_run == true) {
        Fl::wait();
        Fl::flush();
    }

    return _ret;
}

/** @brief Try to select font by using the name.
*
* If font cant be found then the first font is selected.
*
* @param[in] fontname  Name of font to select.
*/
void flw::FontDialog::_select_name(const std::string& fontname) {
    auto count = 1;

    for (auto f : flw::PREF_FONTNAMES) {
        auto font_without_style = util::remove_browser_format(util::to_string(f));

        if (fontname == font_without_style) {
            _fonts->value(count);
            _fonts->middleline(count);
            static_cast<priv::_FontLabel*>(_label)->font = count - 1;
            return;
        }

        count++;
    }

    _fonts->value(1);
    static_cast<priv::_FontLabel*>(_label)->font = 0;
}

/** @brief Create and show font dialog.
*
* Input values are not updated if user press cancel.
*
* @param[in]     title             Dialog title.
* @param[in,out] font              Font index.
* @param[in,out] fontsize          Font size.
* @param[out]    fontname          Font name.
* @param[in]     limit_to_default  True to use only FLTK fonts.
*
* @return True if user selected a font.
*
* @snippet dialog.cpp flw::dlg::font example
* @image html font_dialog.png
*/
bool flw::dlg::font(const std::string& title, Fl_Font& font, Fl_Fontsize& fontsize, std::string& fontname, bool limit_to_default) {
    auto dlg = flw::FontDialog(font, fontsize, title, limit_to_default);

    if (dlg.run() == false) {
        return false;
    }

    font     = dlg.font();
    fontsize = dlg.fontsize();
    fontname = dlg.fontname();

    return true;
}

// MKALGAM_OFF
