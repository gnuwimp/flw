/**
* @file
* @brief An html viewer dialog.
*
* @author gnuwimp@gmail.com
* @copyright Released under the GNU General Public License v3.0
*/

#ifndef FLW_HTMLDIALOG_H
#define FLW_HTMLDIALOG_H

#include "flw.h"

// MKALGAM_ON

#include <FL/Fl_Double_Window.H>

class Fl_Button;
class Fl_Help_View;
class Fl_Input;

namespace flw {

class GridGroup;
class InputMenu;
class SVGButton;

/*
 *      _    _ _______ __  __ _      _____  _       _
 *     | |  | |__   __|  \/  | |    |  __ \(_)     | |
 *     | |__| |  | |  | \  / | |    | |  | |_  __ _| | ___   __ _
 *     |  __  |  | |  | |\/| | |    | |  | | |/ _` | |/ _ \ / _` |
 *     | |  | |  | |  | |  | | |____| |__| | | (_| | | (_) | (_| |
 *     |_|  |_|  |_|  |_|  |_|______|_____/|_|\__,_|_|\___/ \__, |
 *                                                           __/ |
 *                                                          |___/
 */

/** @brief A simple html dialog.
*
* This is similar to Fl_Help_Dialog class.\n
*\n
* This is primarily used to show pages that have been embedded in your program.\n
* Embedd texts with HTMLDialog::add_page(name, html)\n
*\n
* It can load a html file, but don't mix files and embedded pages.\n
*\n
* Links does have a history so user can jump back or forward.\n
* The first page that is set or loaded will be the home page.\n
*\n
* Supported external links might by opened by supported programs.\n
*\n
* Font size can be changed.\n
*\n
* Call HTMLDialog::run() to show it in modal mode.\n
*\n
* Only basic html 2 tags are supported.\n
*\n
* @see https://www.fltk.org/doc-1.4/classFl__Help__View.html#details\n
*
* @snippet dialog.cpp flw::HTMLDialog example
* @image html html_dialog.png
*/
class HTMLDialog : public Fl_Double_Window {
public:
                                HTMLDialog(int W = 60, int H = 50);
                                ~HTMLDialog()
                                    { _html->value(""); } // This fixed memory issues.
    void                        add_page(const std::string& name, const std::string& page);
    void                        clear_pages();
    int                         handle(int event) override;
    void                        load_page(const std::string& file);
    void                        run(bool modal = true);

private:
    void                        _copy_text();
    void                        _debug() const;
    void                        _find();
    void                        _go_back();
    void                        _go_forward();
    void                        _go_home();
    const char*                 _go_link(const char* uri);
    void                        _set_font_size(int size);
    void                        _set_page(const std::string& page, bool clear_links2);
    void                        _update_buttons();

    static void                 _Callback(Fl_Widget* w, void* o);
    static const char*          _CallbackLink(Fl_Widget*, const char* uri);

    Fl_Button*                  _close;         ///< @brief Close button.
    Fl_Button*                  _copy;          ///< @brief Copy text button.
    Fl_Help_View*               _html;          ///< @brief HTML widget.
    InputMenu*                  _input;         ///< @brief Search text.
    bool                        _run;           ///< @brief Run flag for modal dialog.
    flw::GridGroup*             _grid;          ///< @brief layout widget.
    flw::SVGButton*             _back;          ///< @brief Jump back to previous page.
    flw::SVGButton*             _forward;       ///< @brief Jump to next page.
    flw::SVGButton*             _dec;           ///< @brief Decrease font size.
    flw::SVGButton*             _home;          ///< @brief Jump to first page.
    flw::SVGButton*             _inc;           ///< @brief Increase font size.
    flw::SVGButton*             _top;           ///< @brief Jump to top.
    flw::SVGButton*             _view;          ///< @brief View current html code.
    flw::StringHash             _pages;         ///< @brief Hashes with name and html strings.
    flw::StringVector           _links1;        ///< @brief Back history.
    flw::StringVector           _links2;        ///< @brief Forward history.
    int                         _pos;           ///< @brief Find position.
};

namespace dlg {

void                            html(const std::string& text, int W = 60, int H = 50);
void                            html_file(const std::string& file, int W = 60, int H = 50);

} // flw::dlg
} // flw

// MKALGAM_OFF

#endif
