/**
* @file
* @brief Font selection dialog.
*
* @author gnuwimp@gmail.com
* @copyright Released under the GNU General Public License v3.0
*/

#ifndef FLW_FONTDIALOG_H
#define FLW_FONTDIALOG_H

#include "flw.h"

// MKALGAM_ON

#include <FL/Fl_Double_Window.H>

class Fl_Box;
class Fl_Button;

namespace flw {

class GridGroup;
class ScrollBrowser;
class SVGButton;

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

/** @brief A dialog for selecting font and font size.
*
* flw::theme::load_fonts() will be called automatically.\n
*
* @snippet dialog.cpp flw::FontDialog example
* @image html font_dialog.png
*/
class FontDialog : public Fl_Double_Window {
public:
                                FontDialog(Fl_Font font, Fl_Fontsize fontsize, const std::string& title = "Select Font", bool limit_to_default = false);
                                FontDialog(const std::string& font, Fl_Fontsize fontsize, const std::string& title = "Select Font", bool limit_to_default = false);
    void                        activate_font();
    void                        activate_font_size();
    void                        deactivate_font();
    void                        deactivate_fontsize();
    int                         font()
                                    { return _font; } ///< @brief Return selected font.
    std::string                 fontname()
                                    { return _fontname; } ///< @brief Return selected font name.
    int                         fontsize()
                                    { return _fontsize; } ///< @brief Return selected font size.
    bool                        run();

private:
    void                        _activate();
    void                        _create(Fl_Font font, const std::string& fontname, Fl_Fontsize fontsize, const std::string& label, bool limit_to_default);
    void                        _select_name(const std::string& font_name);

    static void                 Callback(Fl_Widget* w, void* o);

    Fl_Box*                     _label;     ///< @brief Sample text label.
    Fl_Button*                  _cancel;    ///< @brief Cancel button.
    SVGButton*                  _select;    ///< @brief Select and close dialog button.
    GridGroup*                  _grid;      ///< @brief Layout manager.
    ScrollBrowser*              _fonts;     ///< @brief Font list.
    ScrollBrowser*              _sizes;     ///< @brief Font size list.
    bool                        _ret;       ///< @brief Return value.
    bool                        _run;       ///< @brief Run flag.
    int                         _font;      ///< @brief Selected font.
    int                         _fontsize;  ///< @brief Selected font size.
    std::string                 _fontname;  ///< @brief Selected font name.
};

namespace dlg {
    bool                        font(const std::string& title, Fl_Font& font, Fl_Fontsize& fontsize, std::string& fontname, bool limit_to_default = false);
}

} // flw

// MKALGAM_OFF

#endif
