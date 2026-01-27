/**
* @file
* @brief Postscript printing.
*
* @author gnuwimp@gmail.com
* @copyright Released under the GNU General Public License v3.0
*/

#include "dlg.h"
#include "fontdialog.h"
#include "gridgroup.h"
#include "postscript.h"
#include "waitcursor.h"

// MKALGAM_ON

#include <FL/Fl_Box.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_File_Chooser.H>
#include <FL/Fl_Hor_Slider.H>
#include <FL/fl_draw.H>

namespace flw {
namespace priv {

/** @brief Initiate printer formats and layouts.
*
* @param[in] format  Choice widget with page names.
* @param[in] layout  Choice widget with layout names.
*/
static void _postscript_init_formats(Fl_Choice* format, Fl_Choice* layout) {
    format->add("A0 Format");
    format->add("A1 Format");
    format->add("A2 Format");
    format->add("A3 Format");
    format->add("A4 Format");
    format->add("A5 Format");
    format->add("A6 Format");
    format->add("A7 Format");
    format->add("A8 Format");
    format->add("A9 Format");
    format->add("B0 Format");
    format->add("B1 Format");
    format->add("B2 Format");
    format->add("B3 Format");
    format->add("B4 Format");
    format->add("B5 Format");
    format->add("B6 Format");
    format->add("B7 Format");
    format->add("B8 Format");
    format->add("B9 Format");
    format->add("Executive Format");
    format->add("Folio Format");
    format->add("Ledger Format");
    format->add("Legal Format");
    format->add("Letter Format");
    format->add("Tabloid Format");
    format->tooltip("Select paper format.");
    format->value(4);

    layout->add("Portrait");
    layout->add("Landscape");
    layout->tooltip("Select paper layout.");
    layout->value(0);
}

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
static std::string _postscript_to_file(
    const std::string&           ps_filename,
    Fl_Paged_Device::Page_Format format,
    Fl_Paged_Device::Page_Layout layout,
    postscript::PageCallback     cb,
    void*                        data,
    unsigned                     from,
    unsigned                     to) {

    auto run     = true;
    auto file    = (FILE*) nullptr;
    auto printer = Fl_PostScript_File_Device();
    auto ph      = 0;
    auto pw      = 0;
    auto res     = std::string();

    if ((file = fl_fopen(ps_filename.c_str(), "wb")) == nullptr) {
        return "Error: could not open file!";
    }

    printer.begin_job(file, 0, format, layout);

    while (run == true) {
        if (printer.begin_page() != 0) {
            res = "Error: couldn't create new page!";
            goto ERR;
        }

        if (printer.printable_rect(&pw, &ph) != 0) {
            res = "Error: couldn't retrieve page size!";
            goto ERR;
        }

        fl_push_clip(0, 0, pw, ph);
        run = cb(data, pw, ph, from);
        fl_pop_clip();

        if (printer.end_page() != 0) {
            res = "Error: couldn't end page!";
            goto ERR;
        }

        from++;

        if (from > to) {
            run = false;
        }
    }

ERR:
    printer.end_job();
    fclose(file);
    return res;
}

/*
 *           _____  _       _____      _       _
 *          |  __ \| |     |  __ \    (_)     | |
 *          | |  | | | __ _| |__) | __ _ _ __ | |_
 *          | |  | | |/ _` |  ___/ '__| | '_ \| __|
 *          | |__| | | (_| | |   | |  | | | | | |_
 *          |_____/|_|\__, |_|   |_|  |_|_| |_|\__|
 *      ______         __/ |
 *     |______|       |___/
 */

/** @brief A print dialog.
*
*/
class _DlgPrint : public Fl_Double_Window {
    Fl_Button*                  _close;     // Close button.
    Fl_Button*                  _file;      // Select destination file.
    Fl_Button*                  _print;     // Print to file.
    Fl_Choice*                  _format;    // Page format list.
    Fl_Choice*                  _layout;    // Page layout list.
    Fl_Hor_Slider*              _from;      // From page number.
    Fl_Hor_Slider*              _to;        // To page number.
    GridGroup*                  _grid;      // Layout widget.
    postscript::PageCallback    _cb;        // User callback for drawing a page.
    bool                        _run;       // Run flag.
    void*                       _data;      // User data.

public:
    /** @brief Create window.
    *
    * Only one of list and file can be used at the same time.
    *
    * @param[in] title   Dialog title.
    * @param[in] cb      Callback drawing funtion.
    * @param[in] data    Data for callback function.
    * @param[in] from    From page number.
    * @param[in] to      To page number.
    */
    _DlgPrint(const std::string& title, postscript::PageCallback cb, void* data, unsigned from, unsigned to) :
    Fl_Double_Window(0, 0, flw::PREF_FONTSIZE * 34, flw::PREF_FONTSIZE * 18) {
        end();

        _close  = new Fl_Button(0, 0, 0, 0, labels::CLOSE.c_str());
        _file   = new Fl_Button(0, 0, 0, 0, "output.ps");
        _format = new Fl_Choice(0, 0, 0, 0);
        _from   = new Fl_Hor_Slider(0, 0, 0, 0);
        _grid   = new GridGroup(0, 0, w(), h());
        _layout = new Fl_Choice(0, 0, 0, 0);
        _print  = new Fl_Button(0, 0, 0, 0, labels::PRINT.c_str());
        _to     = new Fl_Hor_Slider(0, 0, 0, 0);
        _cb     = cb;
        _data   = data;
        _run    = false;

        _grid->add(_from,     1,   3,  -1,   4);
        _grid->add(_to,       1,  10,  -1,   4);
        _grid->add(_format,   1,  15,  -1,   4);
        _grid->add(_layout,   1,  20,  -1,   4);
        _grid->add(_file,     1,  25,  -1,   4);
        _grid->add(_print,  -34,  -5,  16,   4);
        _grid->add(_close,  -17,  -5,  16,   4);
        add(_grid);

        util::labelfont(this);
        _close->callback(_DlgPrint::Callback, this);
        _file->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
        _file->callback(_DlgPrint::Callback, this);
        _file->tooltip("Select output PostScript file.");
        _format->textfont(flw::PREF_FONT);
        _format->textsize(flw::PREF_FONTSIZE);
        _from->align(FL_ALIGN_TOP | FL_ALIGN_LEFT);
        _from->callback(_DlgPrint::Callback, this);
        _from->color(FL_BACKGROUND2_COLOR);
        _from->range(from, to);
        _from->precision(0);
        _from->value(from);
        _from->tooltip("Start page number.");
        _layout->textfont(flw::PREF_FONT);
        _layout->textsize(flw::PREF_FONTSIZE);
        _print->callback(_DlgPrint::Callback, this);
        _to->align(FL_ALIGN_TOP | FL_ALIGN_LEFT);
        _to->callback(_DlgPrint::Callback, this);
        _to->color(FL_BACKGROUND2_COLOR);
        _to->range(from, to);
        _to->precision(0);
        _to->value(to);
        _to->tooltip("End page number.");

        if (from < 1 || from > to) {
            _from->deactivate();
            _from->range(0, 1);
            _from->value(0);
            _to->deactivate();
            _to->range(0, 1);
            _to->value(0);
        }
        else if (from == to) {
            _from->deactivate();
            _to->deactivate();
        }

        priv::_postscript_init_formats(_format, _layout);
        _DlgPrint::Callback(_from, this);
        _DlgPrint::Callback(_to, this);
        callback(_DlgPrint::Callback, this);
        copy_label(title.c_str());
        set_modal();
        resizable(_grid);
        util::center_window(this, flw::PREF_WINDOW);
        _grid->do_layout();
    }

    /** @brief Callback for all widgets.
    *
    */
    static void Callback(Fl_Widget* w, void* o) {
        auto self = static_cast<_DlgPrint*>(o);

        if (w == self || w == self->_close) {
            self->_run = false;
            self->hide();
        }
        else if (w == self->_file) {
            auto filename = fl_file_chooser("Save To PostScript File", "PostScript Files (*.ps)\tAll Files (*)", self->_file->label());

            if (filename != nullptr) {
                self->_file->copy_label(filename);
            }
        }
        else if (w == self->_from || w == self->_to) {
            auto l = util::format("%s page: %d", (w == self->_from) ? "From" : "To", (int) static_cast<Fl_Hor_Slider*>(w)->value());
            w->copy_label(l.c_str());
            self->redraw();
        }
        else if (w == self->_print) {
            self->print();
        }
    }

    /** @brief Callback for all widgets.
    *
    */
    void print() {
        auto from   = static_cast<int>(_from->value());
        auto to     = static_cast<int>(_to->value());
        auto format = static_cast<Fl_Paged_Device::Page_Format>(_format->value());
        auto layout = (_layout->value() == 0) ? Fl_Paged_Device::Page_Layout::PORTRAIT : Fl_Paged_Device::Page_Layout::LANDSCAPE;
        auto file   = _file->label();
        auto err    = (from >= to) ? postscript::print_page_to_file(file, format, layout, _cb, _data) : postscript::print_pages_to_file(file, format, layout, _cb, _data, from, to);

        if (err != "") {
            dlg::msg_alert("Printing Error", util::format("Printing failed!\n%s", err.c_str()));
            return;
        }

        _run = false;
        hide();
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
};

/*
 *           _____  _       _____      _       _ _______        _
 *          |  __ \| |     |  __ \    (_)     | |__   __|      | |
 *          | |  | | | __ _| |__) | __ _ _ __ | |_ | | _____  _| |_
 *          | |  | | |/ _` |  ___/ '__| | '_ \| __|| |/ _ \ \/ / __|
 *          | |__| | | (_| | |   | |  | | | | | |_ | |  __/>  <| |_
 *          |_____/|_|\__, |_|   |_|  |_|_| |_|\__||_|\___/_/\_\\__|
 *      ______         __/ |
 *     |______|       |___/
 */

/** @brief Print text dialog.
*
*/
class _DlgPrintText : public Fl_Double_Window {
    Fl_Box*                     _label;         // Label with information about the text.
    Fl_Button*                  _close;         // Close button.
    Fl_Button*                  _file;          // Select postscript result file.
    Fl_Button*                  _fonts;         // Select font button.
    Fl_Button*                  _print;         // Print text button.
    Fl_Check_Button*            _border;        // Turn on printing a border around the page.
    Fl_Check_Button*            _wrap;          // Wrap text lines option.
    Fl_Choice*                  _align;         // Align text option.
    Fl_Choice*                  _format;        // Page format.
    Fl_Choice*                  _layout;        // Page layout.
    Fl_Font                     _font;          // Font to use.
    Fl_Fontsize                 _fontsize;      // Font size to use.
    Fl_Hor_Slider*              _line;          // Line number width (optional).
    Fl_Hor_Slider*              _tab;           // Replace tabs with spaces (optional).
    GridGroup*                  _grid;          // Layout widget.
    bool                        _ret;           // Return value.
    bool                        _run;           // Run flag.
    const StringVector&         _text;          // Text strings to print.
    std::string                 _label2;        // Text info.

public:
    /** @brief Create window.
    *
    * @param[in] title   Dialog title.
    * @param[in] text    Text to print.
    */
    _DlgPrintText(const std::string& title, const StringVector& text) :
    Fl_Double_Window(0, 0, flw::PREF_FONTSIZE * 34, flw::PREF_FONTSIZE * 35),
    _text(text) {
        end();

        _align    = new Fl_Choice(0, 0, 0, 0);
        _border   = new Fl_Check_Button(0, 0, 0, 0, "Print border");
        _close    = new Fl_Button(0, 0, 0, 0, labels::CLOSE.c_str());
        _file     = new Fl_Button(0, 0, 0, 0, "output.ps");
        _fonts    = new Fl_Button(0, 0, 0, 0, "Courier - 14");
        _format   = new Fl_Choice(0, 0, 0, 0);
        _grid     = new GridGroup(0, 0, w(), h());
        _label    = new Fl_Box(0, 0, 0, 0);
        _layout   = new Fl_Choice(0, 0, 0, 0);
        _line     = new Fl_Hor_Slider(0, 0, 0, 0);
        _print    = new Fl_Button(0, 0, 0, 0, labels::PRINT.c_str());
        _tab      = new Fl_Hor_Slider(0, 0, 0, 0);
        _wrap     = new Fl_Check_Button(0, 0, 0, 0, "Wrap lines");
        _ret      = false;
        _run      = false;
        _font     = FL_COURIER;
        _fontsize = 14;

        _grid->add(_border,   1,   1,  -1,   4);
        _grid->add(_wrap,     1,   6,  -1,   4);
        _grid->add(_line,     1,  13,  -1,   4);
        _grid->add(_tab,      1,  20,  -1,   4);
        _grid->add(_format,   1,  25,  -1,   4);
        _grid->add(_layout,   1,  30,  -1,   4);
        _grid->add(_align,    1,  35,  -1,   4);
        _grid->add(_fonts,    1,  40,  -1,   4);
        _grid->add(_file,     1,  45,  -1,   4);
        _grid->add(_label,    1,  50,  -1,   13);
        _grid->add(_print,  -34,  -5,  16,   4);
        _grid->add(_close,  -17,  -5,  16,   4);
        add(_grid);

        util::labelfont(this);
        _align->add("Left Align");
        _align->add("Center Align");
        _align->add("Right Align");
        _align->tooltip("Line numbers are only used for left aligned text.");
        _align->value(0);
        _align->textfont(flw::PREF_FONT);
        _align->textsize(flw::PREF_FONTSIZE);
        _border->tooltip("Print line border around the print area.");
        _close->callback(_DlgPrintText::Callback, this);
        _file->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
        _file->callback(_DlgPrintText::Callback, this);
        _file->tooltip("Select output PostScript file.");
        _fonts->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
        _fonts->callback(_DlgPrintText::Callback, this);
        _fonts->tooltip("Select font to use.");
        _format->textfont(flw::PREF_FONT);
        _format->textsize(flw::PREF_FONTSIZE);
        _label->align(FL_ALIGN_TOP | FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
        _label->box(FL_BORDER_BOX);
        _label->box(FL_THIN_DOWN_BOX);
        _layout->textfont(flw::PREF_FONT);
        _layout->textsize(flw::PREF_FONTSIZE);
        _line->align(FL_ALIGN_TOP | FL_ALIGN_LEFT);
        _line->callback(_DlgPrintText::Callback, this);
        _line->color(FL_BACKGROUND2_COLOR);
        _line->range(0, 6);
        _line->precision(0);
        _line->value(0);
        _line->tooltip("Set minimum line number width.\nSet to 0 to disable.");
        _print->callback(_DlgPrintText::Callback, this);
        _tab->align(FL_ALIGN_TOP | FL_ALIGN_LEFT);
        _tab->callback(_DlgPrintText::Callback, this);
        _tab->color(FL_BACKGROUND2_COLOR);
        _tab->range(0, 16);
        _tab->precision(0);
        _tab->value(0);
        _tab->tooltip("Replace tabs with spaces.\nSet to 0 to disable.");
        _wrap->tooltip("Wrap long lines or they will be clipped.");

        priv::_postscript_init_formats(_format, _layout);
        _DlgPrintText::Callback(_line, this);
        _DlgPrintText::Callback(_tab, this);
        callback(_DlgPrintText::Callback, this);
        copy_label(title.c_str());
        set_modal();
        resizable(_grid);
        util::center_window(this, flw::PREF_WINDOW);
        _grid->do_layout();
        set_label();
    }

    /** @brief Callback for all widgets.
    *
    */
    static void Callback(Fl_Widget* w, void* o) {
        auto self = static_cast<_DlgPrintText*>(o);

        if (w == self || w == self->_close) {
            self->_run = false;
            self->hide();
        }
        else if (w == self->_file) {
            auto filename = fl_file_chooser("Save To PostScript File", "PostScript Files (*.ps)\tAll Files (*)", self->_file->label());

            if (filename != nullptr) {
                self->_file->copy_label(filename);
            }
        }
        else if (w == self->_fonts) {
            auto dlg = flw::FontDialog(self->_font, self->_fontsize, "Select Print Font", true);

            if (dlg.run() == true) {
                auto l = util::format("%s - %d", dlg.fontname().c_str(), dlg.fontsize());
                self->_fonts->copy_label(l.c_str());

                self->_font     = dlg.font();
                self->_fontsize = dlg.fontsize();
            }
        }
        else if (w == self->_line) {
            auto l = util::format("Line number: %d", (int) self->_line->value());
            self->_line->copy_label(l.c_str());
            self->redraw();
        }
        else if (w == self->_print) {
            self->print();
        }
        else if (w == self->_tab) {
            auto l = util::format("Tab replacement: %d", (int) self->_tab->value());
            self->_tab->copy_label(l.c_str());
            self->redraw();
        }
    }

    /** @brief Run print.
    *
    */
    void print() {
        auto border  = _border->value();
        auto wrap    = _wrap->value();
        auto line    = static_cast<int>(_line->value());
        auto tab     = static_cast<int>(_tab->value());
        auto format  = static_cast<Fl_Paged_Device::Page_Format>(_format->value());
        auto layout  = (_layout->value() == 0) ? Fl_Paged_Device::Page_Layout::PORTRAIT : Fl_Paged_Device::Page_Layout::LANDSCAPE;
        auto align   = (_align->value() == 0) ? FL_ALIGN_LEFT : (_align->value() == 1) ? FL_ALIGN_CENTER : FL_ALIGN_RIGHT;
        auto file    = _file->label();
        auto printer = postscript::PrintText(file, format, layout, _font, _fontsize, align, wrap, border, line);
        auto err     = printer.print(_text, tab);

        if (err == "") {
            auto s = _label2;
            s += util::format("\n%d page%s was printed.", printer.page_count(), printer.page_count() > 1 ? "s" : "");
            _label->copy_label(s.c_str());
            _ret = true;
        }
        else {
            auto s = _label2;
            s += util::format("\nPrinting failed!\n%s", err.c_str());
            _label->copy_label(s.c_str());
            _ret = false;
        }

        redraw();
    }

    /** @brief Run dialog.
    *
    */
    bool run() {
        _run = true;
        show();

        while (_run == true) {
            Fl::wait();
            Fl::flush();
        }

        return _ret;
    }

    /** @brief Create and set text info.
    *
    */
    void set_label() {
        auto len = 0;

        for (auto& s : _text) {
            auto l = fl_utf_nb_char((unsigned char*) s.c_str(), (int) s.length());

            if (l > len) {
                len = l;
            }
        }

        _label2 = util::format("Text contains %u lines.\nMax line length are %u characters.", (unsigned) _text.size(), (unsigned) len);
        _label->copy_label(_label2.c_str());
    }
};

} // flw::priv
} // flw

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
flw::postscript::PrintText::PrintText(
    const std::string&              filename,
    Fl_Paged_Device::Page_Format    page_format,
    Fl_Paged_Device::Page_Layout    page_layout,
    Fl_Font                         font,
    Fl_Fontsize                     fontsize,
    Fl_Align                        align,
    bool                            wrap,
    bool                            border,
    unsigned                        line_num) {

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
flw::postscript::PrintText::~PrintText() {
    _stop();
}

/** @brief Check and create new page if needed.
*
*/
void flw::postscript::PrintText::_check_for_new_page() {
    if (_py + _lh > _ph || _page_count == 0) {
        if (_page_count > 0) {
            fl_pop_clip();

            if (_printer->end_page() != 0) {
                throw "Error: couldn't end current page!";
            }
        }

        if (_printer->begin_page() != 0) {
            throw "Error: couldn't create new page!";
        }

        if (_printer->printable_rect(&_pw, &_ph) != 0) {
            throw "Error: couldn't retrieve page size!";
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
void flw::postscript::PrintText::_measure_lw_lh(const std::string& text) {
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
std::string flw::postscript::PrintText::print(const char* text, unsigned replace_tab_with_space) {
    return print(util::split_string(text, "\n"), replace_tab_with_space);
}

/** @brief Print text to file.
*
* @param[in] text                    Text to print, use only \n for newlines.
* @param[in] replace_tab_with_space  Replace spaces with tab (1 - 16) or 0 to print as it is.
*
* @return Error string or empty string.
*/
std::string flw::postscript::PrintText::print(const std::string& text, unsigned replace_tab_with_space) {
    return print(util::split_string(text.c_str(), "\n"), replace_tab_with_space);
}

/** @brief Print text to file.
*
* @param[in] lines                   Text lines to print.
* @param[in] replace_tab_with_space  Replace spaces with tab (1 - 16) or 0 to print as it is.
*
* @return Error string or empty string.
*/
std::string flw::postscript::PrintText::print(const StringVector& lines, unsigned replace_tab_with_space) {
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
        res = "Error: unknown exception!";
    }

    return res;
}

/** @brief Print one line.
*
* @param[in] line  Text line.
*/
void flw::postscript::PrintText::_print_line(const std::string& line) {
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
void flw::postscript::PrintText::_print_wrapped_line(const std::string& line) {
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
std::string flw::postscript::PrintText::_start() {
    if ((_file = fl_fopen(_filename.c_str(), "wb")) == nullptr) {
        return "Error: could not open file!";
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
std::string flw::postscript::PrintText::_stop() {
    std::string res = "";

    if (_printer != nullptr) {
        if (_page_count > 0) {
            fl_pop_clip();

            if (_printer->end_page() != 0) {
                res = "Error: could not end page!";
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
 *                      _                 _       _
 *                     | |               (_)     | |
 *      _ __   ___  ___| |_ ___  ___ _ __ _ _ __ | |_
 *     | '_ \ / _ \/ __| __/ __|/ __| '__| | '_ \| __|
 *     | |_) | (_) \__ \ |_\__ \ (__| |  | | |_) | |_
 *     | .__/ \___/|___/\__|___/\___|_|  |_| .__/ \__|
 *     | |                                 | |
 *     |_|                                 |_|
 */


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
std::string flw::postscript::print_page_to_file(const std::string& ps_filename, Fl_Paged_Device::Page_Format format, Fl_Paged_Device::Page_Layout layout, PageCallback cb, void* data) {
    return priv::_postscript_to_file(ps_filename, format, layout, cb, data, 1, 1);
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
std::string flw::postscript::print_pages_to_file(const std::string& ps_filename, Fl_Paged_Device::Page_Format format, Fl_Paged_Device::Page_Layout layout, PageCallback cb, void* data, unsigned from, unsigned to) {
    if (from < 1 || from > to) {
        return "Error: invalid from/to range!";
    }

    return priv::_postscript_to_file(ps_filename, format, layout, cb, data, from, to);
}

/*
 *          _ _
 *         | | |
 *       __| | | __ _
 *      / _` | |/ _` |
 *     | (_| | | (_| |
 *      \__,_|_|\__, |
 *               __/ |
 *              |___/
 */

/** @brief Show print dialog for printing text to postscript.
*
* @param[in]  title  Dialog title.
* @param[out] text   Text strings.
*
* @return True if printing was successful.
*/
bool flw::dlg::print_lines(const std::string& title, const StringVector& text) {
    return priv::_DlgPrintText(title, text).run();
}

/** @brief Show print dialog for custom drawings to postscript.
*
* Callback must be used for drawing.
*
* @param[in]  title  Dialog title.
* @param[out] cb     Printer callback.
* @param[out] data   Data for callback.
* @param[out] from   From page.
* @param[out] to     To Page.
*
* @snippet dialog.cpp flw::dlg::print_page example
* @snippet dialog.cpp flw::dlg::print_page_callback example
* @image html print_dialog.png
*/
void flw::dlg::print_page(const std::string& title, flw::postscript::PageCallback cb, void* data, unsigned from, unsigned to) {
    priv::_DlgPrint(title, cb, data, from, to).run();
}

/** @brief Show print dialog for printing text to postscript.
*
* String will be splitted on "\n".
*
* @param[in]  title  Dialog title.
* @param[out] text   Text string.
*
* @return True if printing was successful.
*
* @snippet dialog.cpp flw::dlg::print_text example
* @image html print_text_dialog.png
* @image html print_text.png
*/
bool flw::dlg::print_text(const std::string& title, const std::string& text) {
    auto lines = util::split_string(text, "\n");
    return priv::_DlgPrintText(title, lines).run();
}


// MKALGAM_OFF
