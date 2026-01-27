/**
* @file
* @brief Postscript printing.
*
* @author gnuwimp@gmail.com
* @copyright Released under the GNU General Public License v3.0
*/

#ifndef FLW_POSTSCRIPT_H
#define FLW_POSTSCRIPT_H

#include "flw.h"

// MKALGAM_ON

#include <FL/Fl_PostScript.H>

namespace flw {

/** @brief Print text to postscript or drawing from a widget.
*
*/
namespace postscript {

typedef bool (*PageCallback)(void* data, int pw, int ph, unsigned page);    ///< @brief A drawing callback for printing to postscript.

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

/** @brief Print text to postscript file.
*
* @snippet dialog.cpp flw::postscript::PrintText example
* @image html print_text.png
*/
class PrintText {
public:
                                PrintText(const std::string& filename,
                                    Fl_Paged_Device::Page_Format format = Fl_Paged_Device::Page_Format::A4,
                                    Fl_Paged_Device::Page_Layout layout = Fl_Paged_Device::Page_Layout::PORTRAIT,
                                    Fl_Font font = FL_COURIER,
                                    Fl_Fontsize fontsize = 14,
                                    Fl_Align align = FL_ALIGN_LEFT,
                                    bool wrap = true,
                                    bool border = false,
                                    unsigned line_num = 0);
                                ~PrintText();

    Fl_Fontsize                 fontsize() const
                                    { return _fontsize; } ///< @brief Return font size
    int                         page_count() const
                                    { return _page_count; } ///< @brief Return page count.
    std::string                 print(const char* text, unsigned replace_tab_with_space = 0);
    std::string                 print(const std::string& text, unsigned replace_tab_with_space = 0);
    std::string                 print(const StringVector& lines, unsigned replace_tab_with_space = 0);

private:
    void                        _check_for_new_page();
    void                        _measure_lw_lh(const std::string& text);
    void                        _print_line(const std::string& line);
    void                        _print_wrapped_line(const std::string& line);
    std::string                 _start();
    std::string                 _stop();

    Fl_Align                    _align;             // Text align.
    Fl_Font                     _font;              // Text font.
    Fl_Fontsize                 _fontsize;          // Text size.
    Fl_PostScript_File_Device*  _printer;           // Printer device.
    Fl_Paged_Device::Page_Format _page_format;      // Page format.
    Fl_Paged_Device::Page_Layout _page_layout;      // Page layout.
    FILE*                       _file;              // Postscript file handle.
    bool                        _border;            // Border property.
    bool                        _wrap;              // Wrap property.
    int                         _lh;                // Current line height.
    int                         _line_count;        // Line counter.
    int                         _line_num;          // Line number width.
    int                         _lw;                // Current line width.
    int                         _nw;                // Line width if line number is used.
    unsigned                    _page_count;        // Page counter.
    int                         _ph;                // Page height.
    int                         _pw;                // Page width.
    int                         _px;                // Page x pos.
    int                         _py;                // Page y pos.
    std::string                 _filename;          // Postscript filename.
};

    std::string                 print_page_to_file(const std::string& ps_filename, Fl_Paged_Device::Page_Format format, Fl_Paged_Device::Page_Layout layout, PageCallback cb, void* data);
    std::string                 print_pages_to_file(const std::string& ps_filename, Fl_Paged_Device::Page_Format format, Fl_Paged_Device::Page_Layout layout, PageCallback cb, void* data, unsigned from, unsigned to);
} // flw::postscript

namespace dlg {
bool                            print_lines(const std::string& title, const StringVector& text);
void                            print_page(const std::string& title, flw::postscript::PageCallback cb, void* data = nullptr, unsigned from = 1, unsigned to = 1);
bool                            print_text(const std::string& title, const std::string& text);
} // flw::dlg
} // flw

// MKALGAM_OFF

#endif
