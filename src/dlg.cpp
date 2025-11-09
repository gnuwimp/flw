/**
* @file
* @brief Assorted dialog functions and classes.
*
* @author gnuwimp@gmail.com
* @copyright Released under the GNU General Public License v3.0
*/

#include "dlg.h"
#include "icons.h"
#include "scrollbrowser.h"
#include "datechooser.h"

// MKALGAM_ON

#ifdef _WIN32
    #include <windows.h>
#else
#endif

#include <math.h>
#include <FL/Fl_File_Chooser.H>
#include <FL/Fl_Float_Input.H>
#include <FL/Fl_Help_View.H>
#include <FL/Fl_Hor_Slider.H>
#include <FL/Fl_Hor_Value_Slider.H>
#include <FL/Fl_Int_Input.H>
#include <FL/Fl_Multiline_Input.H>
#include <FL/Fl_Output.H>
#include <FL/Fl_Return_Button.H>
#include <FL/Fl_Scroll.H>
#include <FL/Fl_Secret_Input.H>
#include <FL/Fl_Text_Editor.H>
#include <FL/Fl_Tooltip.H>

namespace flw {

/** @brief Private code.
*
*/
namespace priv {

static Fl_Window*   PARENT              = nullptr;
static bool         ACTIVE_BUTTON_COLOR = false;

/* @private */ void _load_default();
/* @private */ void _load_gleam();
/* @private */ void _load_gleam_blue();
/* @private */ void _load_gleam_dark();
/* @private */ void _load_gleam_tan();
/* @private */ void _load_gtk();
/* @private */ void _load_gtk_blue();
/* @private */ void _load_gtk_dark();
/* @private */ void _load_gtk_tan();
/* @private */ void _load_oxy();
/* @private */ void _load_oxy_blue();
/* @private */ void _load_oxy_tan();
/* @private */ void _load_plastic();
/* @private */ void _load_plastic_tan();
/* @private */ void _scrollbar();

/** @brief Initiate printer formats and layouts.
*
* @param[in] format  Choice widget with page names.
* @param[in] layout  Choice widget with layout names.
*/
static void _init_printer_formats(Fl_Choice* format, Fl_Choice* layout) {
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

/*
 *           ____        _   _
 *          |  _ \      | | | |
 *          | |_) |_   _| |_| |_ ___  _ __
 *          |  _ <| | | | __| __/ _ \| '_ \
 *          | |_) | |_| | |_| || (_) | | | |
 *          |____/ \__,_|\__|\__\___/|_| |_|
 *      ______
 *     |______|
 */

/** @brief A button for dialogs.
*
*/
class _Button : public Fl_Button {
    Fl_Color                    _color;     // Optional color.
    bool                        _return;    // If true then it is and return button.
    std::string                 _icon;      // Icon name.
    std::string                 _label;     // Label without spaces that has been appended.

public:
    /** @brief Create button.
    */
    _Button(int X, int Y, int W, int H, const std::string& l, const std::string& icon, bool ret) : Fl_Button(X, Y, W, H) {
        _return = ret;
        _color  = -1;

        set_icon(l, icon);
    }

    /** @brief Change background color if active.
    */
    void draw() override {
        if (Fl::focus() == this && priv::ACTIVE_BUTTON_COLOR == true) {
            if (_color == static_cast<Fl_Color>(-1)) {
                color(fl_darker(FL_BACKGROUND_COLOR));
            }
            else {
                color(_color);
            }
        }
        else {
            color(FL_BACKGROUND_COLOR);
        }

        Fl_Button::draw();
    }

    /** @brief Take care of enter presses.
    */
    int handle(int event) override {
        if (_return == false) {
        }
        else if (event == FL_SHORTCUT && (Fl::event_key() == FL_Enter || Fl::event_key() == FL_KP_Enter)) {
            simulate_key_action();
            do_callback(FL_REASON_SELECTED);
            return 1;
        }

        return Fl_Button::handle(event);
    }

    /** @brief Get icon name.
    */
    std::string icon() {
        return _icon;
    }

    /** @brief Is this a return button?
    */
    bool is_return() {
        return _return;
    }

    /** @brief Get icon name.
    */
    std::string label2() {
        return _label;
    }

    /** @brief Set highlight color.
    */
    void set_color(Fl_Color color) {
        _color = color;
    }

    /** @brief Set label and create optional icon.
    */
    void set_icon(const std::string& l, const std::string& icon) {
        _icon = icon;

        if (icon != "") {
            copy_label((l + "  ").c_str());
            util::icon(this, icon, flw::PREF_FIXED_FONTSIZE * 1.5);
            align(FL_ALIGN_CENTER | FL_ALIGN_INSIDE | FL_ALIGN_CLIP | FL_ALIGN_TEXT_NEXT_TO_IMAGE);
        }
        else {
            copy_label(l.c_str());
            align(FL_ALIGN_CENTER | FL_ALIGN_INSIDE | FL_ALIGN_CLIP);
        }
    }

    /** @brief Set return option.
    */
    void set_return(bool ret) {
        _return = ret;
    }
};

} // flw::priv

/** @brief Create an button with an icon.
*
* Insert first in a label a "!" character first in label to use the icons::BACK icon.\n
* Insert first in a label a "#" character first in label to use the icons::CANCEL icon.\n
* Insert first in a label a "%" character first in label to use the icons::DEL icon.\n
* Insert first in a label "@" character first in label to use the icons::CONFIRM icon.\n
* Icon option character will be removed from the label.\n
*
* @param[in] X X pos.
* @param[in] Y Y pos.
* @param[in] W Width.
* @param[in] H Height.
* @param[in] l Label.
*
* @see util::icon() to see how they look.\n
*/
Fl_Button* dlg::button(int X, int Y, int W, int H, const std::string& l) {
    auto t  = util::trim(l);
    auto l2 = t.c_str();

    if (*l2 == 0) {
        return new Fl_Button(X, Y, W, H);
    }

    const char* l3 = l2 + 1;

    if (*l2 == '!') {
        return new priv::_Button(X, Y, W, H, l3, icons::BACK, true);
    }
    else if (*l2 == '#') {
        return new priv::_Button(X, Y, W, H, l3, icons::CANCEL, false);
    }
    else if (*l2 == '%') {
        return new priv::_Button(X, Y, W, H, l3, icons::DEL, false);
    }
    else if (*l2 == '@') {
        return new priv::_Button(X, Y, W, H, l3, icons::CONFIRM, false);
    }
    else {
        return new priv::_Button(X, Y, W, H, l, "", false);
    }
}

/** @brief Create an button with an icon.
*
* @param[in] X     X pos.
* @param[in] Y     Y pos.
* @param[in] W     Width.
* @param[in] H     Height.
* @param[in] l     Label.
* @param[in] icon  Optional svg icon.
* @param[in] ret   If true then it will be activated with enter.
*/
Fl_Button* dlg::button(int X, int Y, int W, int H, const std::string& l, const std::string& icon, bool ret) {
    return new priv::_Button(X, Y, W, H, l, icon, ret);
}

/** @brief Put fl_message dialogs at the centre of the desktop.
*
*/
void dlg::center_fl_message_dialog() {
    int X, Y, W, H;
    Fl::screen_xywh(X, Y, W, H);
    fl_message_position(W / 2, H / 2, 1);
}

/** @brief Set dialog options.
*
* Some desktops can't place windows at a specific position.
*
* @param[in] parent               Center on this window, NULL to center on desktop.
* @param[in] active_button_color  Change background color for active button.
*/
void dlg::options(Fl_Window* parent, bool active_button_color) {
    priv::PARENT              = parent;
    priv::ACTIVE_BUTTON_COLOR = active_button_color;
}

/** @brief Show an dialog with message and then exit the program.
*
* @param[in] message  Message string.
*/
void dlg::panic(const std::string& message) {
    dlg::msg_error("Application Error", util::format("PANIC! I have to quit\n%s", message.c_str()));
    exit(1);
}

/*
 *           _____        _        _____ _                               _____  _
 *          |  __ \      | |      / ____| |                             |  __ \| |
 *          | |  | | __ _| |_ ___| |    | |__   ___   ___  ___  ___ _ __| |  | | | __ _
 *          | |  | |/ _` | __/ _ \ |    | '_ \ / _ \ / _ \/ __|/ _ \ '__| |  | | |/ _` |
 *          | |__| | (_| | ||  __/ |____| | | | (_) | (_) \__ \  __/ |  | |__| | | (_| |
 *          |_____/ \__,_|\__\___|\_____|_| |_|\___/ \___/|___/\___|_|  |_____/|_|\__, |
 *      ______                                                                     __/ |
 *     |______|                                                                   |___/
 */

namespace priv {

/** @brief An date chooser dialog.
*
*/
class _DateChooserDlg : public Fl_Double_Window {
    gnu::Date&                  _value;         // In/out date value.
    DateChooser*                _date_chooser;  // Date widget.
    Fl_Button*                  _cancel;        // Cancel button.
    Fl_Button*                  _ok;            // Ok button.
    GridGroup*                  _grid;          // Layout widget.
    bool                        _res;           // Return value.
    bool                        _run;           // Run flag.

public:
    /** @brief Create window.
    *
    * @param[in]      title  Dialog title.
    * @param[in,out]  date   Date value.
    * @param[in]      W      Width.
    * @param[in]      H      Height.
    */
    _DateChooserDlg(const std::string& title, gnu::Date& date, int W, int H) :
    Fl_Double_Window(0, 0, flw::PREF_FONTSIZE * W, flw::PREF_FONTSIZE * H),
    _value(date) {
        end();

        _cancel       = new Fl_Button(0, 0, 0, 0, label::CANCEL.c_str());
        _date_chooser = new DateChooser(0, 0, 0, 0);
        _grid         = new GridGroup(0, 0, w(), h());
        _ok           = new Fl_Return_Button(0, 0, 0, 0, label::OK.c_str());
        _res          = false;
        _run          = false;

        _grid->add(_date_chooser,   0,   0,   0,  -6);
        _grid->add(_cancel,       -34,  -5,  16,   4);
        _grid->add(_ok,           -17,  -5,  16,   4);
        add(_grid);

        _cancel->callback(Callback, this);
        _date_chooser->focus();
        _date_chooser->set(_value);
        _grid->do_layout();
        _ok->callback(Callback, this);

        util::labelfont(this);
        callback(Callback, this);
        copy_label(title.c_str());
        resizable(_grid);
        set_modal();
    }

    /** @brief Callback for all widgets.
    *
    */
    static void Callback(Fl_Widget* w, void* o) {
        auto self = static_cast<_DateChooserDlg*>(o);

        if (w == self || w == self->_cancel) {
            self->_run = false;
            self->hide();
        }
        else if (w == self->_ok) {
            self->_res = true;
            self->_run = false;
            self->hide();
        }
    }

    /** @brief Run dialog.
    *
    */
    bool run() {
        _run = true;
        util::center_window(this, priv::PARENT);
        show();

        while (_run == true) {
            Fl::wait();
            Fl::flush();
        }

        if (_res == true) {
            _value = _date_chooser->get();
        }

        return _res;
    }
};

} // flw::priv

/** @brief Show an date chooser dialog.
*
* @param[in]     title   Dialog title.
* @param[in,out] date    Input and output date.
* @param[in]     W       Width in characters.
* @param[in]     H       Height in characters.
*
* @return True if ok has been pressed and date has been updated.
*
* @snippet dialog.cpp flw::dlg::date example
* @image html date_dialog.png
*/
bool dlg::date(const std::string& title, gnu::Date& date, int W, int H) {
    priv::_DateChooserDlg dlg(title, date, W, H);
    return dlg.run();
}

/*
 *           _____  _       _    _ _             _
 *          |  __ \| |     | |  | | |           | |
 *          | |  | | | __ _| |__| | |_ _ __ ___ | |
 *          | |  | | |/ _` |  __  | __| '_ ` _ \| |
 *          | |__| | | (_| | |  | | |_| | | | | | |
 *          |_____/|_|\__, |_|  |_|\__|_| |_| |_|_|
 *      ______         __/ |
 *     |______|       |___/
 */

namespace priv {

/** @brief HTML dialog.
* @private
*/
class _DlgHtml : public Fl_Double_Window {
    Fl_Help_View*               _html;  // HTML widget.
    Fl_Return_Button*           _close; // Close button.
    GridGroup*                  _grid;  // layout widget.
    bool                        _run;   // Run flag.

public:
    /** @brief Create window.
    *
    * @param[in] title   Dialog title.
    * @param[in] text    HTML text.
    * @param[in] W       Width in characters.
    * @param[in] H       Height in characters.
    */
    _DlgHtml(const std::string& title, const std::string& text, int W, int H) :
    Fl_Double_Window(0, 0, flw::PREF_FONTSIZE * W,flw::PREF_FONTSIZE * H) {
        end();

        _close = new Fl_Return_Button(0, 0, 0, 0, label::CLOSE.c_str());
        _grid  = new GridGroup(0, 0, w(), h());
        _html  = new Fl_Help_View(0, 0, 0, 0);
        _run   = false;

        _grid->add(_html,    1,  1, -1, -6);
        _grid->add(_close, -17, -5, 16,  4);
        add(_grid);

        _close->callback(_DlgHtml::Callback, this);
        _close->labelfont(flw::PREF_FONT);
        _close->labelsize(flw::PREF_FONTSIZE);
        _html->textfont(flw::PREF_FONT);
        _html->textsize(flw::PREF_FONTSIZE);
        _html->value(text.c_str());

        callback(_DlgHtml::Callback, this);
        copy_label(title.c_str());
        set_modal();
        resizable(_grid);
        util::center_window(this, priv::PARENT);
        _grid->do_layout();
    }

    /** @brief Callback for all widgets.
    *
    */
    static void Callback(Fl_Widget* w, void* o) {
        auto self = static_cast<_DlgHtml*>(o);

        if (w == self || w == self->_close) {
            self->_run = false;
            self->hide();
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
};

} // flw::priv

/** @brief Show a html view dialog.
*
* Only basic html 2 tags are supported.
*
* @param[in] title   Dialog title.
* @param[in] text    HTML text.
* @param[in] W       Width in characters.
* @param[in] H       Height in characters.
*
* @snippet dialog.cpp flw::dlg::html example
* @image html html_dialog.png
*/
void dlg::html(const std::string& title, const std::string& text, int W, int H) {
    priv::_DlgHtml dlg(title, text, W, H);
    dlg.run();
}

/*
 *           _____  _       _      _     _
 *          |  __ \| |     | |    (_)   | |
 *          | |  | | | __ _| |     _ ___| |_
 *          | |  | | |/ _` | |    | / __| __|
 *          | |__| | | (_| | |____| \__ \ |_
 *          |_____/|_|\__, |______|_|___/\__|
 *      ______         __/ |
 *     |______|       |___/
 */

namespace priv {

/** @brief Dialog with a browser list widget.
*
*/
class _DlgList : public Fl_Double_Window {
    Fl_Return_Button*           _close;     // Close button.
    GridGroup*                  _grid;      // Layout widget.
    ScrollBrowser*              _list;      // List widget.
    bool                        _run;       // Run flag.

public:
    /** @brief Create window.
    *
    * Only one of list and file can be used at the same time.
    *
    * @param[in] title       Dialog title.
    * @param[in] list        String list.
    * @param[in] file        String file.
    * @param[in] fixed_font  Width in characters.
    * @param[in] W           Width in characters.
    * @param[in] H           Height in characters.
    */
    _DlgList(const std::string& title, const StringVector& list, const std::string& file, bool fixed_font = false, int W = 50, int H = 20) :
    Fl_Double_Window(0, 0, (fixed_font ? flw::PREF_FIXED_FONTSIZE : flw::PREF_FONTSIZE) * W, (fixed_font ? flw::PREF_FIXED_FONTSIZE : flw::PREF_FONTSIZE) * H) {
        end();

        _close = new Fl_Return_Button(0, 0, 0, 0, label::CLOSE.c_str());
        _grid  = new GridGroup(0, 0, w(), h());
        _list  = new ScrollBrowser();
        _run   = false;

        _grid->add(_list,     1,   1,  -1,  -6);
        _grid->add(_close,  -17,  -5,  16,   4);
        add(_grid);

        _close->callback(_DlgList::Callback, this);
        _close->labelfont(flw::PREF_FONT);
        _close->labelsize(flw::PREF_FONTSIZE);
        _list->take_focus();

        if (fixed_font == true) {
            _list->textfont(flw::PREF_FIXED_FONT);
            _list->textsize(flw::PREF_FIXED_FONTSIZE);
        }
        else {
            _list->textfont(flw::PREF_FONT);
            _list->textsize(flw::PREF_FONTSIZE);
        }

        callback(_DlgList::Callback, this);
        copy_label(title.c_str());
        set_modal();
        resizable(_grid);
        util::center_window(this, priv::PARENT);
        _grid->do_layout();

        if (list.size() > 0) {
            for (const auto& s : list) {
                _list->add(s.c_str());
            }
        }
        else if (file != "") {
            _list->load(file.c_str());
        }
    }

    /** @brief Callback for all widgets.
    *
    */
    static void Callback(Fl_Widget* w, void* o) {
        auto self = static_cast<_DlgList*>(o);

        if (w == self || w == self->_close) {
            self->_run = false;
            self->hide();
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
};

} // flw::priv

/** @brief Show a dialog with a list of strings.
*
* @see Fl_Browser::format_char() for formatting characters.\n
*
* @param[in] title       Dialog title.
* @param[in] list        List of strings.
* @param[in] fixed_font  True to use a fixed font.
* @param[in] W           Width in characters.
* @param[in] H           Height in characters.
*
* @snippet dialog.cpp flw::dlg::list example
* @image html list_dialog.png
*/
void dlg::list(const std::string& title, const StringVector& list, bool fixed_font, int W, int H) {
    priv::_DlgList dlg(title, list, "", fixed_font, W, H);
    dlg.run();
}

/** @brief Show a dialog with a browser/list widget.
*
* @see Fl_Browser::format_char() for formatting characters.\n
* This function splits a string on newline.\n
*
* @param[in] title       Dialog title.
* @param[in] list        String list.
* @param[in] fixed_font  True to use a fixed font.
* @param[in] W           Width in characters.
* @param[in] H           Height in characters.
*/
void dlg::list(const std::string& title, const std::string& list, bool fixed_font, int W, int H) {
    auto list2 = util::split_string( list, "\n");
    priv::_DlgList dlg(title, list2, "", fixed_font, W, H);
    dlg.run();
}

/** @brief Show a dialog with a browser/list widget.
*
* @see Fl_Browser::format_char() for formatting characters.\n
* This function loads a list of string from a file.\n
*
* @param[in] title       Dialog title.
* @param[in] file        Filename to load.
* @param[in] fixed_font  True to use a fixed font.
* @param[in] W           Width in characters.
* @param[in] H           Height in characters.
*/
void dlg::list_file(const std::string& title, const std::string& file, bool fixed_font, int W, int H) {
    priv::_DlgList dlg(title, flw::StringVector(), file, fixed_font, W, H);
    dlg.run();
}

/*
 *           _____  _       __  __
 *          |  __ \| |     |  \/  |
 *          | |  | | | __ _| \  / |___  __ _
 *          | |  | | |/ _` | |\/| / __|/ _` |
 *          | |__| | | (_| | |  | \__ \ (_| |
 *          |_____/|_|\__, |_|  |_|___/\__, |
 *      ______         __/ |            __/ |
 *     |______|       |___/            |___/
 */

namespace priv {

/** @brief Message dialog.
* @private
*/
class _DlgMsg : public Fl_Double_Window {
    Fl_Box*                     _label;     // Message label.
    priv::_Button*              _b1;        // First button on the right.
    priv::_Button*              _b2;        // Second button.
    priv::_Button*              _b3;        // Third button.
    priv::_Button*              _b4;        // Fourth button.
    priv::_Button*              _b5;        // Fifth button.
    Fl_Button*                  _icon;      // Icon label.
    Fl_Input*                   _input;     // Input widget.
    GridGroup*                  _grid;      // layout widget.
    bool                        _run;       // Run flag.
    int                         _escape;    // > 1 and dialog can't be closed with escape.
    std::string                 _res;       // Used button label.

public:
    /** @brief Create window.
    *
    * @param[in] type   Type of dialog.
    * @param[in] title  Dialog title.
    * @param[in] text   Message text.
    * @param[in] b1     right button label.
    * @param[in] b2     Second button label.
    * @param[in] b3     Third button label.
    * @param[in] b4     Fourth button label.
    * @param[in] b5     Fifth button label.
    * @param[in] W      Width in characters.
    * @param[in] H      Height in characters.
    * @param[in] value  Optional input.
    * @param[in] input  Input widget.
    */
    _DlgMsg(
        const std::string& type,
        const std::string& title,
        const std::string& message,
        const std::string& b1,
        const std::string& b2,
        const std::string& b3,
        const std::string& b4,
        const std::string& b5,
        int W,
        int H,
        const char* value = nullptr,
        const std::string& input = ""
    ) : Fl_Double_Window(0, 0, flw::PREF_FONTSIZE * W,flw::PREF_FONTSIZE * H) {
        end();

        _b1     = static_cast<priv::_Button*>(dlg::button(0, 0, 0, 0, b1));
        _b2     = static_cast<priv::_Button*>(dlg::button(0, 0, 0, 0, b2));
        _b3     = static_cast<priv::_Button*>(dlg::button(0, 0, 0, 0, b3));
        _b4     = static_cast<priv::_Button*>(dlg::button(0, 0, 0, 0, b4));
        _b5     = static_cast<priv::_Button*>(dlg::button(0, 0, 0, 0, b5));
        _grid   = new GridGroup(0, 0, w(), h());
        _icon   = new Fl_Button(0, 0, 0, 0);
        _label  = new Fl_Box(0, 0, 0, 0);
        _run    = false;
        _escape = 0;

        auto multi = false;
        auto xpos  = 0;
        auto count = 0;

        if (input == "Fl_Int_Input") {
            _input = new Fl_Int_Input(0, 0, 0, 0);
        }
        else if (input == "Fl_Float_Input") {
            _input = new Fl_Float_Input(0, 0, 0, 0);
        }
        else if (input == "Fl_Multiline_Input") {
            _input = new Fl_Multiline_Input(0, 0, 0, 0);
            multi  = true;
        }
        else if (input == "Fl_Secret_Input") {
            _input = new Fl_Secret_Input(0, 0, 0, 0);
        }
        else {
            _input = new Fl_Input(0, 0, 0, 0);
        }

        // Create top left icon.

        util::icon(_icon, type, flw::PREF_FONTSIZE * 4);

        // Fix width depending on how many buttons there will be.

        count = count + (b1 != "");
        count = count + (b2 != "");
        count = count + (b3 != "");
        count = count + (b4 != "");
        count = count + (b5 != "");
        xpos  = -count * 21;

        // Add optional input widget.

        if (value != nullptr) {
            if (multi == true) {
                _grid->add(_input, 12, 8, -1, -6);
            }
            else {
                _grid->add(_input, 12, -10, -1, 4);
            }

            _input->value(value);
            _input->box(FL_BORDER_BOX);
        }
        else {
            _input->hide();
        }

        // Add buttons, start from the left so moving between buttons are in right order.

        if (b5 != "") {
            _grid->add(_b5, xpos, -5, 20, 4);
            xpos += 21;
        }
        else {
            _b5->hide();
        }

        if (b4 != "") {
            _grid->add(_b4, xpos, -5, 20, 4);
            xpos += 21;
        }
        else {
            _b4->hide();
        }

        if (b3 != "") {
            _grid->add(_b3, xpos, -5, 20, 4);
            xpos += 21;
        }
        else {
            _b3->hide();
        }

        if (b2 != "") {
            _grid->add(_b2, xpos, -5, 20, 4);
            xpos += 21;
        }
        else {
            _b2->hide();
        }

        if (b1 != "") {
            _grid->add(_b1, xpos, -5, 20, 4);
            xpos += 21;
        }
        else {
            _b1->hide();
        }

        // Count number of visible buttons.

        _escape += _b1->visible();
        _escape += _b2->visible();
        _escape += _b3->visible();
        _escape += _b4->visible();
        _escape += _b5->visible();

        // Add label and icon.

        if (_input->visible() == 0) {
            _grid->add(_label, 12,  1, -1, -6);
        }
        else if (multi == true) {
            _grid->add(_label, 12,  1, -1, 6);
        }
        else {
            _grid->add(_label, 12,  1, -1, -11);
        }

        _grid->add(_icon, 1, 1, 10, 10);
        add(_grid);

        _b1->callback(_DlgMsg::Callback, this);
        _b2->callback(_DlgMsg::Callback, this);
        _b3->callback(_DlgMsg::Callback, this);
        _b4->callback(_DlgMsg::Callback, this);
        _b5->callback(_DlgMsg::Callback, this);
        _icon->box(FL_BORDER_BOX);
        _label->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE | FL_ALIGN_TOP | FL_ALIGN_CLIP);
        _label->box(FL_BORDER_BOX);
        _label->copy_label(message.c_str());

        // Make sure return buttons has focus (this is a visual issue).

        if (_b1->is_return() == true) {
            _b1->take_focus();
        }
        else if (_b2->is_return() == true) {
            _b2->take_focus();
        }
        else if (_b3->is_return() == true) {
            _b3->take_focus();
        }
        else if (_b4->is_return() == true) {
            _b4->take_focus();
        }
        else if (_b5->is_return() == true) {
            _b5->take_focus();
        }
        else {
            _icon->take_focus();
        }

        if (_escape == 1) {
            // Only one button so turn off eventual highlight color.

            _b1->set_color(FL_BACKGROUND_COLOR);
            _b2->set_color(FL_BACKGROUND_COLOR);
            _b3->set_color(FL_BACKGROUND_COLOR);
            _b4->set_color(FL_BACKGROUND_COLOR);
            _b5->set_color(FL_BACKGROUND_COLOR);

            // Make sure one button is a return button.

            if (_b1->visible() != 0 && _b1->icon() == "") {
                _b1->set_icon(_b1->label(), icons::BACK);
                _b1->set_return(true);
            }
            else if (_b2->visible() != 0 && _b2->icon() == "") {
                _b2->set_icon(_b2->label(), icons::BACK);
                _b2->set_return(true);
            }
            else if (_b3->visible() != 0 && _b3->icon() == "") {
                _b3->set_icon(_b3->label(), icons::BACK);
                _b3->set_return(true);
            }
            else if (_b4->visible() != 0 && _b4->icon() == "") {
                _b4->set_icon(_b4->label(), icons::BACK);
                _b4->set_return(true);
            }
            else if (_b5->visible() != 0 && _b5->icon() == "") {
                _b5->set_icon(_b5->label(), icons::BACK);
                _b5->set_return(true);
            }
        }

        // Input widget alsways has the focus.

        if (_input->visible() != 0) {
            _input->take_focus();
        }

        callback(_DlgMsg::Callback, this);
        copy_label(title.c_str());
        set_modal();
        resizable(_grid);
        util::labelfont(this);
        util::center_window(this, priv::PARENT);
        _grid->do_layout();
    }

    /** @brief Callback for all widgets.
    *
    */
    static void Callback(Fl_Widget* w, void* o) {
        auto self = static_cast<_DlgMsg*>(o);

        if (w == self && self->_escape == 1) {
            self->_run = false;
            self->hide();
        }
        else if (w == self->_b1 || w == self->_b2 || w == self->_b3 || w == self->_b4 || w == self->_b5) {
            self->_run = false;
            self->hide();
            self->_res = util::trim(w->label());
        }
    }

    /** @brief Get input value.
    *
    */
    std::string input() {
        return _input->value();
    }

    /** @brief Run dialog.
    *
    */
    std::string run() {
        if (_b1->visible() == 0 && _b2->visible() == 0 && _b3->visible() == 0 && _b4->visible() == 0 && _b5->visible() == 0) {
            return "";
        }

        _run = true;
        show();

        while (_run == true) {
            Fl::wait();
            Fl::flush();
        }

        return _res;
    }
};

} // flw::priv

/** @brief Show an text input dialog.
*
* @param[in]     title    Dialog title.
* @param[in]     message  Message text.
* @param[in,out] value    Text line to edit.
* @param[in]     W        Width in characters.
* @param[in]     H        Height in characters.
*
* @return flw::label::OK or flw::label::CANCEL.
*
* @image html dlg_input.png
*/
std::string dlg::input(const std::string& title, const std::string& message, std::string& value, int W, int H) {
    priv::_DlgMsg dlg(icons::EDIT, title, message, label::OK, label::CANCEL, "", "", "", W, H, value.c_str(), "Fl_Input");
    auto res = dlg.run();

    if (res == label::OK) {
        value = dlg.input();
    }

    return res;
}

/** @brief Show an decimal input dialog.
*
* If the number is invalid, the input number will be returned.
*
* @param[in]     title    Dialog title.
* @param[in]     message  Message text.
* @param[in,out] value    Decimal number to edit.
* @param[in]     W        Width in characters.
* @param[in]     H        Height in characters.
*
* @return flw::label::OK or flw::label::CANCEL.
*/
std::string dlg::input_double(const std::string& title, const std::string& message, double& value, int W, int H) {
    char num[500];
    snprintf(num, 500, "%g", value);

    priv::_DlgMsg dlg(icons::EDIT, title, message, label::OK, label::CANCEL, "", "", "", W, H, num, "Fl_Float_Input");
    auto res = dlg.run();

    if (res == label::OK) {
        auto tmp = value;
        tmp = util::to_double(dlg.input());

        if (std::isinf(tmp) == false) {
            value = tmp;
        }
    }

    return res;
}

/** @brief Show an integer input dialog.
*
* If the number is invalid, the input number will be returned.
*
* @param[in]     title    Dialog title.
* @param[in]     message  Message text.
* @param[in,out] value    Integer number to edit.
* @param[in]     W        Width in characters.
* @param[in]     H        Height in characters.
*
* @return flw::label::OK or flw::label::CANCEL.
*/
std::string dlg::input_int(const std::string& title, const std::string& message, int64_t& value, int W, int H) {
    char num[100];
    snprintf(num, 100, "%lld", static_cast<long long int>(value));

    priv::_DlgMsg dlg(icons::EDIT, title, message, label::OK, label::CANCEL, "", "", "", W, H, num, "Fl_Int_Input");
    auto res = dlg.run();

    if (res == label::OK) {
        value = util::to_int(dlg.input(), value);
    }

    return res;
}

/** @brief Show an multiline input dialog.
*
* Don't edit to many lines of text because there are no scrollbars.\n
* @see To edit longer texts use dlg::text_edit()
*
* @param[in]     title    Dialog title.
* @param[in]     message  Message text.
* @param[in,out] value    Multi line text to edit.
* @param[in]     W        Width in characters.
* @param[in]     H        Height in characters.
*
* @return flw::label::OK or flw::label::CANCEL.
*
* @image html dlg_input_multi.png
*/
std::string dlg::input_multi(const std::string& title, const std::string& message, std::string& value, int W, int H) {
    priv::_DlgMsg dlg(icons::EDIT, title, message, label::OK, label::CANCEL, "", "", "", W, H, value.c_str(), "Fl_Multiline_Input");
    auto res = dlg.run();

    if (res == label::OK) {
        value = dlg.input();
    }

    return res;
}

/** @brief Show an secret text input dialog.
*
* @param[in]     title    Dialog title.
* @param[in]     message  Message text.
* @param[in,out] value    Secret text to edit.
* @param[in]     W        Width in characters.
* @param[in]     H        Height in characters.
*
* @return flw::label::OK or flw::label::CANCEL.
*
* @image html dlg_input_secret.png
*/
std::string dlg::input_secret(const std::string& title, const std::string& message, std::string& value, int W, int H) {
    priv::_DlgMsg dlg(icons::PASSWORD, title, message, label::OK, label::CANCEL, "", "", "", W, H, value.c_str(), "Fl_Secret_Input");
    auto res = dlg.run();

    if (res == label::OK) {
        value = dlg.input();
    }

    return res;
}

/** @brief Show an dialog with a message icon.
*
* @param[in] title    Dialog title.
* @param[in] message  Message text.
* @param[in] W        Width in characters.
* @param[in] H        Height in characters.
*
* @image html dlg_msg.png
*/
void dlg::msg(const std::string& title, const std::string& message, int W, int H) {
    priv::_DlgMsg dlg(icons::INFO, title, message, label::CLOSE, "", "", "", "", W, H);
    dlg.run();
}

/** @brief Show an dialog with an alert icon.
*
* @param[in] title    Dialog title.
* @param[in] message  Message text.
* @param[in] W        Width in characters.
* @param[in] H        Height in characters.
*
* @image html dlg_msg_alert.png
*/
void dlg::msg_alert(const std::string& title, const std::string& message, int W, int H) {
    priv::_DlgMsg dlg(icons::ALERT, title, message, label::CLOSE, "", "", "", "", W, H);
    dlg.run();
}

/** @brief Show an dialog with a question icon.
*
* You can show 1 to 5 buttons to choose from.\n
* If you use more than three buttons you must increase width and height.\n
* @see dlg::button() on how to add a icon.\n
* Any button label can be empty and it will be hidden.\n
*
* @param[in] title    Dialog title.
* @param[in] message  Message text.
* @param[in] b1       First button label on the right side.
* @param[in] b2       Second button label.
* @param[in] b3       Third button label.
* @param[in] b4       Fourth button label.
* @param[in] b5       Fifth button label.
* @param[in] W        Width in characters.
* @param[in] H        Height in characters.
*
* @return Activated button label without the optional icon character.
*
* @image html dlg_msg_ask.png
*/
std::string dlg::msg_ask(const std::string& title, const std::string& message, const std::string& b1, const std::string& b2, const std::string& b3, const std::string& b4, const std::string& b5, int W, int H) {
    priv::_DlgMsg dlg(icons::QUESTION, title, message, b1, b2, b3, b4, b5, W, H);
    return dlg.run();
}

/** @brief Show an dialog with an error icon.
*
* @param[in] title    Dialog title.
* @param[in] message  Message text.
* @param[in] W        Width in characters.
* @param[in] H        Height in characters.
*/
void dlg::msg_error(const std::string& title, const std::string& message, int W, int H) {
    priv::_DlgMsg dlg(icons::ERR, title, message, label::CLOSE, "", "", "", "", W, H);
    dlg.run();
}

/** @brief Show an dialog with a warning icon and 1 or more buttons.
*
* @see dlg::button() on how to add a icon.\n
* Any button label can be empty and it will be hidden.\n
*
* @param[in] title    Dialog title.
* @param[in] message  Message text.
* @param[in] b1       Right button label.
* @param[in] b2       Left button label.
* @param[in] W        Width in characters.
* @param[in] H        Height in characters.
*
* @return Activated button label without the optional icon character.
*/
std::string dlg::msg_warning(const std::string& title, const std::string& message, const std::string& b1, const std::string& b2, int W, int H) {
    priv::_DlgMsg dlg(icons::WARNING, title, message, b1, b2, "", "", "", W, H);
    return dlg.run();
}

/*
 *           _____  _       _____                                    _
 *          |  __ \| |     |  __ \                                  | |
 *          | |  | | | __ _| |__) |_ _ ___ _____      _____  _ __ __| |
 *          | |  | | |/ _` |  ___/ _` / __/ __\ \ /\ / / _ \| '__/ _` |
 *          | |__| | | (_| | |  | (_| \__ \__ \\ V  V / (_) | | | (_| |
 *          |_____/|_|\__, |_|   \__,_|___/___/ \_/\_/ \___/|_|  \__,_|
 *      ______         __/ |
 *     |______|       |___/
 */

namespace priv {

/** @brief A password dialog.
*
*/
class _DlgPassword : public Fl_Double_Window {
public:
    enum class Mode {
                                PASSWORD,
                                PASSWORD_CONFIRM,
                                PASSWORD_CONFIRM_WITH_FILE,
                                PASSWORD_WITH_FILE,
    };

private:
    Fl_Button*                  _browse;        // Browse file button.
    Fl_Button*                  _cancel;        // Cancel button.
    Fl_Button*                  _close;         // Close button.
    Fl_Input*                   _file;          // File input.
    Fl_Input*                   _password1;     // Password input 1.
    Fl_Input*                   _password2;     // Password input 2 (confirm).
    GridGroup*                  _grid;          // Layout widget.
    Mode                        _mode;          // There are 4 modes.
    bool                        _ret;           // Return value.
    bool                        _run;           // Run flag.

public:
    /** @brief Create window.
    *
    * Only one of list and file can be used at the same time.
    *
    * @param[in] title   Dialog title.
    * @param[in] mode    Which dialog mode should be used.
    */
    _DlgPassword(const std::string& title, Mode mode) :
    Fl_Double_Window(0, 0, 10, 10) {
        end();

        _browse    = new Fl_Button(0, 0, 0, 0, label::BROWSE.c_str());
        _cancel    = new Fl_Button(0, 0, 0, 0);
        _close     = new Fl_Return_Button(0, 0, 0, 0);
        _file      = new Fl_Output(0, 0, 0, 0, "Key file");
        _grid      = new GridGroup(0, 0, w(), h());
        _password1 = new Fl_Secret_Input(0, 0, 0, 0, "Password");
        _password2 = new Fl_Secret_Input(0, 0, 0, 0, "Enter password again");
        _mode      = mode;
        _ret       = false;
        _run       = false;

        _grid->add(_password1,  1,   3,  -1,  4);
        _grid->add(_password2,  1,  10,  -1,  4);
        _grid->add(_file,       1,  17,  -1,  4);
        _grid->add(_browse,   -51,  -5,  16,  4);
        _grid->add(_cancel,   -34,  -5,  16,  4);
        _grid->add(_close,    -17,  -5,  16,  4);
        add(_grid);

        _browse->callback(_DlgPassword::Callback, this);
        _cancel->callback(_DlgPassword::Callback, this);
        _cancel->copy_label(label::CANCEL.c_str());
        _close->callback(_DlgPassword::Callback, this);
        _close->copy_label(label::OK.c_str());
        _close->deactivate();
        _file->align(FL_ALIGN_TOP | FL_ALIGN_LEFT);
        _file->textfont(flw::PREF_FIXED_FONT);
        _file->textsize(flw::PREF_FONTSIZE);
        _file->tooltip("Select optional key file.");
        _password1->align(FL_ALIGN_TOP | FL_ALIGN_LEFT);
        _password1->callback(_DlgPassword::Callback, this);
        _password1->textfont(flw::PREF_FIXED_FONT);
        _password1->textsize(flw::PREF_FONTSIZE);
        _password1->when(FL_WHEN_CHANGED);
        _password2->align(FL_ALIGN_TOP | FL_ALIGN_LEFT);
        _password2->callback(_DlgPassword::Callback, this);
        _password2->textfont(flw::PREF_FIXED_FONT);
        _password2->textsize(flw::PREF_FONTSIZE);
        _password2->when(FL_WHEN_CHANGED);

        auto W = flw::PREF_FONTSIZE * 40;
        auto H = flw::PREF_FONTSIZE * 13.5;

        if (_mode == Mode::PASSWORD) {
            _password2->hide();
            _browse->hide();
            _file->hide();
            H = flw::PREF_FONTSIZE * 6.5;
        }
        else if (_mode == Mode::PASSWORD_CONFIRM) {
            _browse->hide();
            _file->hide();
            H = flw::PREF_FONTSIZE * 10;
        }
        else if (_mode == Mode::PASSWORD_WITH_FILE) {
            _password2->hide();
            _grid->resize(_file, 1, 10, -1, 4);
            H = flw::PREF_FONTSIZE * 10;
        }

        resizable(_grid);
        util::labelfont(this);
        callback(_DlgPassword::Callback, this);
        copy_label(title.c_str());
        size(W, H);
        set_modal();
        util::center_window(this, priv::PARENT);
        _grid->do_layout();
    }

    /** @brief Callback for all widgets.
    *
    */
    static void Callback(Fl_Widget* w, void* o) {
        auto self = static_cast<_DlgPassword*>(o);

        if (w == self || w == self->_cancel) {
            self->_ret = false;
            self->_run = false;
            self->hide();
        }
        else if (w == self->_password1) {
            self->check();
        }
        else if (w == self->_password2) {
            self->check();
        }
        else if (w == self->_browse) {
            auto filename = fl_file_chooser("Select Key File", nullptr, nullptr, 0);

            if (filename) {
                self->_file->value(filename);
            }
            else {
                self->_file->value("");
            }

            self->check();
        }
        else if (w == self->_close) {
            self->_ret = true;
            self->_run = false;
            self->hide();
        }
    }

    /** @brief Enable close button.
    *
    */
    void check() {
        auto p1 = _password1->value();
        auto p2 = _password2->value();
        auto f  = _file->value();

        if (_mode == Mode::PASSWORD) {
            if (strlen(p1) > 0) {
                _close->activate();
            }
            else {
                _close->deactivate();
            }
        }
        else if (_mode == Mode::PASSWORD_CONFIRM) {
            if (strlen(p1) > 0 && strcmp(p1, p2) == 0) {
                _close->activate();
            }
            else {
                _close->deactivate();
            }
        }
        else if (_mode == Mode::PASSWORD_CONFIRM_WITH_FILE) {
            if (strlen(p1) > 0 && strcmp(p1, p2) == 0) {
                _close->activate();
            }
            else if (strlen(p1) == 0 && strlen(p2) == 0 && strlen(f) > 0) {
                _close->activate();
            }
            else {
                _close->deactivate();
            }
        }
        else if (_mode == Mode::PASSWORD_WITH_FILE) {
            if (strlen(p1) > 0) {
                _close->activate();
            }
            else if (strlen(f) > 0) {
                _close->activate();
            }
            else {
                _close->deactivate();
            }
        }
    }

    /** @brief Run dialog.
    *
    */
    bool run(std::string& password, std::string& file) {
        _run = true;
        show();

        while (_run == true) {
            Fl::wait();
            Fl::flush();
        }

        if (_ret == true) {
            file = _file->value();
            password = _password1->value();
        }

        util::zero_memory(const_cast<char*>(_password1->value()), strlen(_password1->value()));
        util::zero_memory(const_cast<char*>(_password2->value()), strlen(_password2->value()));
        util::zero_memory(const_cast<char*>(_file->value()), strlen(_file->value()));

        return _ret;
    }
};

} // flw::priv

/** @brief Get password from user.
*
* @param[in]  title     Dialog title.
* @param[out] password  User password.
*
* @return True if ok has been pressed and password argument has been set.
*/
bool dlg::password(const std::string& title, std::string& password) {
    std::string file;
    priv::_DlgPassword dlg(title.c_str(), priv::_DlgPassword::Mode::PASSWORD);
    return dlg.run(password, file);
}

/** @brief Get confirmed password from user.
*
* @param[in]  title     Dialog title.
* @param[out] password  User password.
*
* @return True if ok has been pressed and password argument has been set.
*/
bool dlg::password_confirm(const std::string& title, std::string& password) {
    std::string file;
    priv::_DlgPassword dlg(title.c_str(), priv::_DlgPassword::Mode::PASSWORD_CONFIRM);
    return dlg.run(password, file);
}

/** @brief Get confirmed password and optional key file from user.
*
* Passwords can be empty if a file has been choosen.
*
* @param[in]  title     Dialog title.
* @param[out] password  User password.
* @param[out] file      User file.
*
* @return True if ok has been pressed and password and/or file argument has been set.
*
* @snippet dialog.cpp flw::dlg::password example
* @image html password_dialog.png
*/
bool dlg::password_confirm_and_file(const std::string& title, std::string& password, std::string& file) {
    priv::_DlgPassword dlg(title.c_str(), priv::_DlgPassword::Mode::PASSWORD_CONFIRM_WITH_FILE);
    return dlg.run(password, file);
}

/** @brief Get password and optional key file from user.
*
* Password can be empty if a file has been choosen.
*
* @param[in]  title     Dialog title.
* @param[out] password  User password.
* @param[out] file      User file.
*
* @return True if ok has been pressed and password and/or file argument has been set.
*/
bool dlg::password_and_file(const std::string& title, std::string& password, std::string& file) {
    priv::_DlgPassword dlg(title.c_str(), priv::_DlgPassword::Mode::PASSWORD_WITH_FILE);
    return dlg.run(password, file);
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

namespace priv {

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
    PrintCallback               _cb;        // User callback for drawing a page.
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
    _DlgPrint(const std::string& title, PrintCallback cb, void* data, int from, int to) :
    Fl_Double_Window(0, 0, flw::PREF_FONTSIZE * 34, flw::PREF_FONTSIZE * 18) {
        end();

        _close  = new Fl_Button(0, 0, 0, 0, label::CLOSE.c_str());
        _file   = new Fl_Button(0, 0, 0, 0, "output.ps");
        _format = new Fl_Choice(0, 0, 0, 0);
        _from   = new Fl_Hor_Slider(0, 0, 0, 0);
        _grid   = new GridGroup(0, 0, w(), h());
        _layout = new Fl_Choice(0, 0, 0, 0);
        _print  = new Fl_Button(0, 0, 0, 0, label::PRINT.c_str());
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

        priv::_init_printer_formats(_format, _layout);
        _DlgPrint::Callback(_from, this);
        _DlgPrint::Callback(_to, this);
        callback(_DlgPrint::Callback, this);
        copy_label(title.c_str());
        set_modal();
        resizable(_grid);
        util::center_window(this, priv::PARENT);
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
        auto err    = (from == 0) ? util::print(file, format, layout, _cb, _data) : util::print(file, format, layout, _cb, _data, from, to);

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

} // flw::priv

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
* @snippet dialog.cpp flw::dlg::print example
* @snippet dialog.cpp flw::dlg::print_callback example
* @image html print_dialog.png
*/
void dlg::print(const std::string& title, PrintCallback cb, void* data, int from, int to) {
    priv::_DlgPrint dlg(title, cb, data, from, to);
    dlg.run();
}

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

namespace priv {

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
        _close    = new Fl_Button(0, 0, 0, 0, label::CLOSE.c_str());
        _file     = new Fl_Button(0, 0, 0, 0, "output.ps");
        _fonts    = new Fl_Button(0, 0, 0, 0, "Courier - 14");
        _format   = new Fl_Choice(0, 0, 0, 0);
        _grid     = new GridGroup(0, 0, w(), h());
        _label    = new Fl_Box(0, 0, 0, 0);
        _layout   = new Fl_Choice(0, 0, 0, 0);
        _line     = new Fl_Hor_Slider(0, 0, 0, 0);
        _print    = new Fl_Button(0, 0, 0, 0, label::PRINT.c_str());
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

        priv::_init_printer_formats(_format, _layout);
        _DlgPrintText::Callback(_line, this);
        _DlgPrintText::Callback(_tab, this);
        callback(_DlgPrintText::Callback, this);
        copy_label(title.c_str());
        set_modal();
        resizable(_grid);
        util::center_window(this, priv::PARENT);
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
            auto dlg = dlg::Font(self->_font, self->_fontsize, "Select Print Font", true);

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
        auto printer = util::PrintText(file, format, layout, _font, _fontsize, align, wrap, border, line);
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
bool dlg::print_text(const std::string& title, const std::string& text) {
    auto lines = util::split_string(text, "\n");
    priv::_DlgPrintText dlg(title, lines);

    return dlg.run();
}

/** @brief Show print dialog for printing text to postscript.
*
* @param[in]  title  Dialog title.
* @param[out] text   Text strings.
*
* @return True if printing was successful.
*/
bool dlg::print_text(const std::string& title, const StringVector& text) {
    priv::_DlgPrintText dlg(title, text);
    return dlg.run();
}

/*
 *           _____  _        _____      _           _    _____ _               _    ____
 *          |  __ \| |      / ____|    | |         | |  / ____| |             | |  |  _ \
 *          | |  | | | __ _| (___   ___| | ___  ___| |_| |    | |__   ___  ___| | _| |_) | _____  _____  ___
 *          | |  | | |/ _` |\___ \ / _ \ |/ _ \/ __| __| |    | '_ \ / _ \/ __| |/ /  _ < / _ \ \/ / _ \/ __|
 *          | |__| | | (_| |____) |  __/ |  __/ (__| |_| |____| | | |  __/ (__|   <| |_) | (_) >  <  __/\__ \
 *          |_____/|_|\__, |_____/ \___|_|\___|\___|\__|\_____|_| |_|\___|\___|_|\_\____/ \___/_/\_\___||___/
 *      ______         __/ |
 *     |______|       |___/
 */

namespace priv {

/** @brief A dialog with check buttons.
* @private
*/
class _DlgSelectCheckBoxes : public Fl_Double_Window {
    Fl_Button*                  _all;           // Turn on all check buttons.
    Fl_Button*                  _cancel;        // Cancel dialog.
    Fl_Button*                  _close;         // Close dialog.
    Fl_Button*                  _invert;        // Invert all check buttons.
    Fl_Button*                  _none;          // Uncheck all check buttons.
    Fl_Scroll*                  _scroll;        // Scroller for check buttons.
    GridGroup*                  _grid;          // Main layout widget.
    const StringVector&         _labels;        // Labels for check buttons.
    WidgetVector                _checkbuttons;  // Vector with check buttons.
    bool                        _ret;           // Return value from run.
    bool                        _run;           // Run flag.

public:
    /** @brief Create window.
    *
    * @param[in] title    Dialog title.
    * @param[in] strings  Strings for the checkboxes.
    */
    _DlgSelectCheckBoxes(const std::string& title, const StringVector& strings) :
    Fl_Double_Window(0, 0, flw::PREF_FONTSIZE * 36, flw::PREF_FONTSIZE * 20),
    _labels(strings) {
        end();

        _all    = new Fl_Button(0, 0, 0, 0, "All On");
        _cancel = new Fl_Button(0, 0, 0, 0, label::CANCEL.c_str());
        _close  = new Fl_Return_Button(0, 0, 0, 0, label::OK.c_str());
        _grid   = new GridGroup(0, 0, w(), h());
        _invert = new Fl_Button(0, 0, 0, 0, "Invert");
        _none   = new Fl_Button(0, 0, 0, 0, "All Off");
        _scroll = new Fl_Scroll(0, 0, 0, 0);
        _ret    = false;
        _run    = false;

        _grid->add(_scroll,   1,  1, -1, -7);
        _grid->add(_invert, -67, -5, 10,  4);
        _grid->add(_none,   -56, -5, 10,  4);
        _grid->add(_all,    -45, -5, 10,  4);
        _grid->add(_cancel, -34, -5, 16,  4);
        _grid->add(_close,  -17, -5, 16,  4);
        add(_grid);

        for (auto& l : _labels) {
            auto b = new Fl_Check_Button(0, 0, 0, 0, l.c_str() + 1);
            b->value(*l.c_str() == '1');
            _checkbuttons.push_back(b);
            _scroll->add(b);
        }

        _all->callback(_DlgSelectCheckBoxes::Callback, this);
        _cancel->callback(_DlgSelectCheckBoxes::Callback, this);
        _close->callback(_DlgSelectCheckBoxes::Callback, this);
        _invert->callback(_DlgSelectCheckBoxes::Callback, this);
        _none->callback(_DlgSelectCheckBoxes::Callback, this);
        _scroll->box(FL_BORDER_BOX);

        util::labelfont(this);
        callback(_DlgSelectCheckBoxes::Callback, this);
        copy_label(title.c_str());
        set_modal();
        resizable(_grid);
        util::center_window(this, priv::PARENT);
    }

    /** @brief Callback for all widgets.
    *
    */
    static void Callback(Fl_Widget* w, void* o) {
        auto self = static_cast<_DlgSelectCheckBoxes*>(o);

        if (w == self || w == self->_cancel) {
            self->_run = false;
            self->hide();
        }
        else if (w == self->_close) {
            self->_ret = true;
            self->_run = false;
            self->hide();
        }
        else if (w == self->_all) {
            for (auto b : self->_checkbuttons) {
                static_cast<Fl_Check_Button*>(b)->value(1);
            }
        }
        else if (w == self->_none) {
            for (auto b : self->_checkbuttons) {
                static_cast<Fl_Check_Button*>(b)->value(0);
            }
        }
        else if (w == self->_invert) {
            for (auto b : self->_checkbuttons) {
                static_cast<Fl_Check_Button*>(b)->value(static_cast<Fl_Check_Button*>(b)->value() ? 0 : 1);
            }
        }
    }

    /** @brief Resize all widgets.
    *
    */
    void resize(int X, int Y, int W, int H) override {
        Fl_Double_Window::resize(X, Y, W, H);
        _grid->resize(0, 0, W, H);

        auto y = _scroll->y() + 4;

        for (auto b : _checkbuttons) {
            b->resize(_scroll->x() + 4, y, _scroll->w() - Fl::scrollbar_size() - 8, flw::PREF_FONTSIZE * 2);
            y += flw::PREF_FONTSIZE * 2;
        }
    }

    /** @brief Show dialog.
    *
    */
    StringVector run() {
        StringVector res;

        _run = true;
        show();

        while (_run == true) {
            Fl::wait();
            Fl::flush();
        }

        if (_ret == true) {
            for (auto b : _checkbuttons) {
                res.push_back(std::string((static_cast<Fl_Check_Button*>(b)->value() == 0) ? "0" : "1") + b->label());
            }
        }

        return res;
    }
};

} // flw::priv

/** @brief Show a dialog with checkboxes.
*
* Each string in string list should start with a '0' or '1' which indicates if it should be checked or not.\n
* Then after '0/1' comes the label.\n
* Example: "1Hello World" for an checked item with label "Hello World".\n
*
* @param[in] title   Dialog title.
* @param[in] list    List with strings for the check buttons.
*
* @return If 'Ok' has been pressed it will return the new list with current checks or an empty vector if canceled.
*
* @snippet dialog.cpp flw::dlg::select_checkboxes example
* @image html select_checkboxes_dialog.png
*/
StringVector dlg::select_checkboxes(const std::string& title, const StringVector& list) {
    priv::_DlgSelectCheckBoxes dlg(title, list);
    return dlg.run();
}

/*
 *           _____  _        _____      _           _    _____ _           _
 *          |  __ \| |      / ____|    | |         | |  / ____| |         (_)
 *          | |  | | | __ _| (___   ___| | ___  ___| |_| |    | |__   ___  _  ___ ___
 *          | |  | | |/ _` |\___ \ / _ \ |/ _ \/ __| __| |    | '_ \ / _ \| |/ __/ _ \
 *          | |__| | | (_| |____) |  __/ |  __/ (__| |_| |____| | | | (_) | | (_|  __/
 *          |_____/|_|\__, |_____/ \___|_|\___|\___|\__|\_____|_| |_|\___/|_|\___\___|
 *      ______         __/ |
 *     |______|       |___/
 */

namespace priv {

/** @brief A dialog with choice widget.
* @private
*/
class _DlgSelectChoice : public Fl_Double_Window {
    Fl_Box*                     _label;     // Message label.
    priv::_Button*              _cancel;    // Cancel button.
    priv::_Button*              _ok;        // Ok button.
    Fl_Button*                  _icon;      // Icon label.
    Fl_Choice*                  _choice;    // Choice widget with user strings to select from.
    GridGroup*                  _grid;      // Layout widget.
    bool                        _run;       // Run flag.
    int                         _ret;       // Return value from run.

public:
    /** @brief Create window.
    *
    * @param[in] title     Dialog title.
    * @param[in] strings   Strings for the choce widget.
    * @param[in] selected  Selected string index.
    */
    _DlgSelectChoice(const std::string& title, const std::string& message, const StringVector& strings, int selected) :
    Fl_Double_Window(0, 0, flw::PREF_FONTSIZE * 40, flw::PREF_FONTSIZE * 9.5) {
        end();

        _cancel = static_cast<priv::_Button*>(dlg::button(0, 0, 0, 0, label::CANCEL));
        _choice = new Fl_Choice(0, 0, 0, 0);
        _grid   = new GridGroup(0, 0, w(), h());
        _icon   = new Fl_Button(0, 0, 0, 0);
        _label  = new Fl_Box(0, 0, 0, 0);
        _ok     = static_cast<priv::_Button*>(dlg::button(0, 0, 0, 0, label::OK, icons::BACK, true));
        _ret    = -1;
        _run    = false;

        _grid->add(_icon,     1,   1, 10,  10);
        _grid->add(_label,   12,   1, -1, -11);
        _grid->add(_choice,  12, -10, -1,   4);
        _grid->add(_cancel, -34,  -5, 16,   4);
        _grid->add(_ok,     -17,  -5, 16,   4);
        add(_grid);

        for (const auto& string : strings) {
            _choice->add(string.c_str());
        }

        _cancel->callback(_DlgSelectChoice::Callback, this);
        _cancel->set_color(FL_BACKGROUND_COLOR);
        _choice->textfont(flw::PREF_FONT);
        _choice->textsize(flw::PREF_FONTSIZE);
        _choice->value(selected);
        util::icon(_icon, icons::RIGHT, flw::PREF_FONTSIZE * 4);
        _icon->box(FL_BORDER_BOX);
        _label->align(FL_ALIGN_INSIDE | FL_ALIGN_LEFT | FL_ALIGN_TOP | FL_ALIGN_CLIP);
        _label->copy_label(message.c_str());
        _label->box(FL_BORDER_BOX);
        _ok->callback(_DlgSelectChoice::Callback, this);
        _ok->set_color(FL_BACKGROUND_COLOR);

        util::labelfont(this);
        callback(_DlgSelectChoice::Callback, this);
        copy_label(title.c_str());
        set_modal();
        resizable(_grid);
        util::center_window(this, priv::PARENT);
        _grid->do_layout();
    }

    /** @brief Callback for all widgets.
    *
    */
    static void Callback(Fl_Widget* w, void* o) {
        auto self = static_cast<_DlgSelectChoice*>(o);

        if (w == self || w == self->_cancel) {
            self->_run = false;
            self->hide();
        }
        else if (w == self->_ok) {
            self->_ret = self->_choice->value();
            self->_run = false;
            self->hide();
        }
    }

    /** @brief Run dialog.
    *
    */
    int run() {
        _run = true;
        show();

        while (_run == true) {
            Fl::wait();
            Fl::flush();
        }

        return _ret;
    }
};

} // flw::priv

/** @brief Show a dialog with a choice widget that contains input strings to select from.
*
* @param[in] title     Dialog title.
* @param[in] message   Message string.
* @param[in] list      List with strings.
* @param[in] selected  Index in string list to select.
*
* @return Selected index or -1.
*
* @snippet dialog.cpp flw::dlg::select_choice example
* @image html select_choice_dialog.png
*/
int dlg::select_choice(const std::string& title, const std::string& message, const StringVector& list, int selected) {
    priv::_DlgSelectChoice dlg(title, message, list, selected);
    return dlg.run();
}

/*
 *           _____  _        _____      _           _    _____ _        _
 *          |  __ \| |      / ____|    | |         | |  / ____| |      (_)
 *          | |  | | | __ _| (___   ___| | ___  ___| |_| (___ | |_ _ __ _ _ __   __ _
 *          | |  | | |/ _` |\___ \ / _ \ |/ _ \/ __| __|\___ \| __| '__| | '_ \ / _` |
 *          | |__| | | (_| |____) |  __/ |  __/ (__| |_ ____) | |_| |  | | | | | (_| |
 *          |_____/|_|\__, |_____/ \___|_|\___|\___|\__|_____/ \__|_|  |_|_| |_|\__, |
 *      ______         __/ |                                                     __/ |
 *     |______|       |___/                                                     |___/
 */

namespace priv {

/** @brief Dialog with a list of string to select from.
*
*/
class _DlgSelectString : public Fl_Double_Window {
    Fl_Button*                  _cancel;        // Cancel dialog button.
    Fl_Button*                  _close;         // Close dialog button.
    Fl_Input*                   _filter;        // Enter text to filter strings.
    GridGroup*                  _grid;          // Layout widget.
    ScrollBrowser*              _list;          // String list widget.
    const StringVector&         _strings;       // Input strings.
    bool                        _run;           // Run flag;

public:
    /** @brief Create window.
    *
    * @param[in] title                  Dialog title.
    * @param[in] strings                String list.
    * @param[in] selected_string_index  Initial selected string.
    * @param[in] selected_string        Initial selected string.
    * @param[in] fixed_font             String list.
    * @param[in] W                      Width in characters.
    * @param[in] H                      Height in characters.
    */
    _DlgSelectString(const std::string& title, const StringVector& strings, int selected_string_index, const std::string& selected_string, bool fixed_font, int W, int H) :
    Fl_Double_Window(0, 0, ((fixed_font == true) ? flw::PREF_FIXED_FONTSIZE : flw::PREF_FONTSIZE) * W, ((fixed_font == true) ? flw::PREF_FIXED_FONTSIZE : flw::PREF_FONTSIZE) * H),
    _strings(strings) {
        end();

        _cancel = new Fl_Button(0, 0, 0, 0, label::CANCEL.c_str());
        _close  = new Fl_Return_Button(0, 0, 0, 0, label::SELECT.c_str());
        _filter = new Fl_Input(0, 0, 0, 0);
        _grid   = new GridGroup(0, 0, w(), h());
        _list   = new ScrollBrowser(0, 0, 0, 0);
        _run    = false;

        _grid->add(_filter,   1,  1, -1,  4);
        _grid->add(_list,     1,  6, -1, -6);
        _grid->add(_cancel, -34, -5, 16,  4);
        _grid->add(_close,  -17, -5, 16,  4);
        add(_grid);

        _cancel->callback(_DlgSelectString::Callback, this);
        _close->callback(_DlgSelectString::Callback, this);
        _filter->callback(_DlgSelectString::Callback, this);
        _filter->tooltip("Enter text to filter rows that macthes the text.\nPress tab to switch focus between input and list widget.");
        _filter->when(FL_WHEN_CHANGED);
        _list->callback(_DlgSelectString::Callback, this);
        _list->tooltip("Use Page Up or Page Down in list to scroll faster,");

        if (fixed_font == true) {
            _filter->textfont(flw::PREF_FIXED_FONT);
            _filter->textsize(flw::PREF_FIXED_FONTSIZE);
            _list->textfont(flw::PREF_FIXED_FONT);
            _list->textsize(flw::PREF_FIXED_FONTSIZE);
        }
        else {
            _filter->textfont(flw::PREF_FONT);
            _filter->textsize(flw::PREF_FONTSIZE);
            _list->textfont(flw::PREF_FONT);
            _list->textsize(flw::PREF_FONTSIZE);
        }

        {
            auto r = 0;
            auto f = 0;

            for (const auto& string : _strings) {
                _list->add(string.c_str());

                if (string == selected_string) {
                    r = f + 1;
                }

                f++;
            }

            if (selected_string_index > 0 && selected_string_index <= (int) _strings.size()) {
                _list->value(selected_string_index);
                _list->middleline(selected_string_index);
            }
            else if (r > 0) {
                _list->value(r);
                _list->middleline(r);
            }
            else {
                _list->value(1);
            }
        }

        _filter->take_focus();
        util::labelfont(this);
        callback(_DlgSelectString::Callback, this);
        copy_label(title.c_str());
        activate_button();
        set_modal();
        resizable(_grid);
        util::center_window(this, priv::PARENT);
        _grid->do_layout();
    }

    /** @brief Activate close button if a string is selected.
    *
    */
    void activate_button() {
        if (_list->value() == 0) {
            _close->deactivate();
        }
        else {
            _close->activate();
        }
    }

    /** @brief Callback for all widgets.
    *
    */
    static void Callback(Fl_Widget* w, void* o) {
        auto self = static_cast<_DlgSelectString*>(o);

        if (w == self || w == self->_cancel) {
            self->_list->deselect();
            self->_run = false;
            self->hide();
        }
        else if (w == self->_close) {
            self->_run = false;
            self->hide();
        }
        else if (w == self->_filter) {
            self->filter(self->_filter->value());
            self->activate_button();
        }
        else if (w == self->_list) {
            self->activate_button();

            if (Fl::event_clicks() > 0 && self->_close->active()) {
                Fl::event_clicks(0);
                self->_run = false;
                self->hide();
            }
        }
    }

    /** @brief Filter strings from input list.
    *
    */
    void filter(const std::string& filter) {
        _list->clear();

        for (const auto& string : _strings) {
            if (filter == "") {
                _list->add(string.c_str());
            }
            else if (string.find(filter) != std::string::npos) {
                _list->add(string.c_str());
            }
        }

        _list->value(1);
    }

    /** @brief Tab key changes focus between list and input.
    *
    */
    int handle(int event) override {
        if (event == FL_KEYDOWN) {
            if (Fl::event_key() == FL_Enter) {
                if (_list->value() > 0) {
                    _run = false;
                    hide();
                }

                return 1;
            }
            else if (Fl::event_key() == FL_Tab) {
                if (Fl::focus() == _list || Fl::focus() == _list->menu()) {
                    _filter->take_focus();
                }
                else {
                    _list->take_focus();
                }

                return 1;
            }
        }

        return Fl_Double_Window::handle(event);
    }

    /** @brief Run dialog.
    *
    */
    int run() {
        _run = true;
        show();

        while (_run == true) {
            Fl::wait();
            Fl::flush();
        }

        auto row = _list->value();

        if (row > 0) {
            auto selected = _list->text(row);

            for (int f = 0; f < static_cast<int>(_strings.size()); f++) {
                const auto& string = _strings[f];

                if (string == selected) {
                    return f;
                }
            }
        }

        return -1;
    }
};

} // flw::priv

/** @brief Show a dialog with strings to select from.
*
* There is also an input widget to filter strings in list.\n
* The returned index is always the original index in input vector.\n
*
* @param[in] title         Dialog title.
* @param[in] list          Filename to load.
* @param[in] selected_row  Selected row index.
* @param[in] fixed_font    True to use a fixed font.
* @param[in] W             Width in characters.
* @param[in] H             Height in characters.
*
* @return Selected index (from 0 to list.size() - 1) or -1.
*/
int dlg::select_string(const std::string& title, const StringVector& list, int selected_row, bool fixed_font, int W, int H) {
    priv::_DlgSelectString dlg(title.c_str(), list, selected_row, "", fixed_font, W, H);
    return dlg.run();
}

/** @brief Show a dialog with strings to select from.
*
* There is also an input widget to filter strings in list.\n
* The returned index is always the original index in input vector.\n
*
* @param[in] title         Dialog title.
* @param[in] list          Filename to load.
* @param[in] selected_row  Selected string.
* @param[in] fixed_font    True to use a fixed font.
* @param[in] W             Width in characters.
* @param[in] H             Height in characters.
*
* @return Selected index (from 0 to list.size() - 1) or -1.
*
* @snippet dialog.cpp flw::dlg::select example
* @image html select_string_dialog.png
*/
int dlg::select_string(const std::string& title, const StringVector& list, const std::string& selected_row, bool fixed_font, int W, int H) {
    priv::_DlgSelectString dlg(title.c_str(), list, 0, selected_row, fixed_font, W, H);
    return dlg.run();
}

/*
 *           _____  _        _____ _ _     _
 *          |  __ \| |      / ____| (_)   | |
 *          | |  | | | __ _| (___ | |_  __| | ___ _ __
 *          | |  | | |/ _` |\___ \| | |/ _` |/ _ \ '__|
 *          | |__| | | (_| |____) | | | (_| |  __/ |
 *          |_____/|_|\__, |_____/|_|_|\__,_|\___|_|
 *      ______         __/ |
 *     |______|       |___/
 */

namespace priv {

/** @brief Set a number dialog that uses a slider.
*
*/
class _DlgSlider : public Fl_Double_Window {
    Fl_Button*                  _cancel;    // Cancel button.
    Fl_Button*                  _close;     // Close button.
    Fl_Hor_Value_Slider*        _slider;    // Number slider.
    GridGroup*                  _grid;      // Layout widget.
    bool                        _ret;       // Return flag.
    bool                        _run;       // Run flag.
    double&                     _value;     // Number value.

public:
    /** @brief Create window.
    *
    * @param[in] title   Dialog title.
    * @param[in] min     Minimum value.
    * @param[in] max     Maximum value.
    * @param[in] value   Start value.
    * @param[in] step    Step between every slider change.
    */
    _DlgSlider(const std::string& title, double min, double max, double& value, double step) :
    Fl_Double_Window(0, 0, flw::PREF_FONTSIZE * 40, flw::PREF_FONTSIZE * 6),
    _value(value) {
        end();

        _cancel = new Fl_Button(0, 0, 0, 0, label::CANCEL.c_str());
        _slider = new Fl_Hor_Value_Slider(0, 0, 0, 0);
        _close  = new Fl_Return_Button(0, 0, 0, 0, label::OK.c_str());
        _grid   = new GridGroup(0, 0, w(), h());
        _ret    = false;
        _run    = false;

        _grid->add(_slider,   1,  1, -1,  4);
        _grid->add(_cancel, -34, -5, 16,  4);
        _grid->add(_close,  -17, -5, 16,  4);
        add(_grid);

        _cancel->callback(_DlgSlider::Callback, this);
        _slider->align(FL_ALIGN_LEFT    );
        _slider->callback(_DlgSlider::Callback, this);
        _slider->range(min, max);
        _slider->value(value);
        _slider->step(step);
        _slider->textfont(flw::PREF_FONT);
        _slider->textsize(flw::PREF_FONTSIZE);
        _close->callback(_DlgSlider::Callback, this);

        util::labelfont(this);
        callback(_DlgSlider::Callback, this);
        copy_label(title.c_str());
        set_modal();
        resizable(_grid);
        util::center_window(this, priv::PARENT);
        _grid->do_layout();
        _slider->value_width((max >= 100'000) ? flw::PREF_FONTSIZE * 10 : flw::PREF_FONTSIZE * 6);
    }

    /** @brief Callback for all widgets.
    *
    */
    static void Callback(Fl_Widget* w, void* o) {
        auto self = static_cast<_DlgSlider*>(o);

        if (w == self || w == self->_cancel) {
            self->_run = false;
            self->hide();
        }
        else if (w == self->_close) {
            self->_ret = true;
            self->_run = false;
            self->_value = self->_slider->value();
            self->hide();
        }
    }

    /** @brief Run dialog.
    *
    */
    int run() {
        _run = true;
        show();

        while (_run == true) {
            Fl::wait();
            Fl::flush();
        }

        return _ret;
    }
};

} // flw::priv

/** @brief Show a dialog and set a number with a slider.
*
* @param[in]     title   Dialog title.
* @param[in]     min     Minimum value.
* @param[in]     max     Maximum value.
* @param[in,out] value   Start value.
* @param[in]     step    Step between every slider change.
*
* @return True if ok has been pressed and value has been updated.
*
* @snippet dialog.cpp flw::dlg::slider example
* @image html slider_dialog.png
*/
bool dlg::slider(const std::string& title, double min, double max, double& value, double step) {
    priv::_DlgSlider dlg(title.c_str(), min, max, value, step);
    return dlg.run();
}

/*
 *           _____  _    _______        _
 *          |  __ \| |  |__   __|      | |
 *          | |  | | | __ _| | _____  _| |_
 *          | |  | | |/ _` | |/ _ \ \/ / __|
 *          | |__| | | (_| | |  __/>  <| |_
 *          |_____/|_|\__, |_|\___/_/\_\\__|
 *      ______         __/ |
 *     |______|       |___/
 */

namespace priv {

/** @brief Show text in a dialog.
*
*/
class _DlgText : public Fl_Double_Window {
    Fl_Button*                  _cancel;    // Cancel button.
    Fl_Button*                  _close;     // Close button.
    Fl_Button*                  _save;      // Save text button.
    Fl_Text_Buffer*             _buffer;    // Text buffer.
    Fl_Text_Display*            _text;      // Text display.
    GridGroup*                  _grid;      // Layout widget.
    bool                        _edit;      // Allow editing.
    bool                        _run;       // Run flag.
    char*                       _res;       // Result text.

public:
    /** @brief Create window.
    *
    * @param[in] title   Dialog title.
    * @param[in] text    Text.
    * @param[in] edit    True to allow editing.
    * @param[in] W       Width in characters.
    * @param[in] H       Height in characters.
    */
    _DlgText(const std::string& title, const char* text, bool edit, int W, int H) :
    Fl_Double_Window(0, 0, flw::PREF_FONTSIZE * W, flw::PREF_FONTSIZE * H) {
        end();

        _buffer = new Fl_Text_Buffer();
        _cancel = new Fl_Button(0, 0, 0, 0, label::CANCEL.c_str());
        _close  = new Fl_Return_Button(0, 0, 0, 0, (edit == true) ? label::UPDATE.c_str() : label::CLOSE.c_str());
        _grid   = new GridGroup(0, 0, w(), h());
        _save   = new Fl_Button(0, 0, 0, 0, label::SAVE_DOT.c_str());
        _text   = (edit == false) ? new Fl_Text_Display(0, 0, 0, 0) : new Fl_Text_Editor(0, 0, 0, 0);
        _edit   = edit;
        _res    = nullptr;
        _run    = false;

        if (edit == false) {
            _cancel->hide();
            _grid->add(_text,     1,   1,  -1,  -6);
            _grid->add(_save,   -34,  -5,  16,   4);
            _grid->add(_close,  -17,  -5,  16,   4);
        }
        else {
            _grid->add(_text,     1,   1,  -1,  -6);
            _grid->add(_save,   -51,  -5,  16,   4);
            _grid->add(_cancel, -34,  -5,  16,   4);
            _grid->add(_close,  -17,  -5,  16,   4);
        }

        add(_grid);

        _buffer->text(text);
        _cancel->callback(_DlgText::Callback, this);
        _cancel->tooltip("Close and abort all changes.");
        _close->callback(_DlgText::Callback, this);
        _close->tooltip(_edit == true ? "Update text and close window." : "Close window.");
        _save->callback(_DlgText::Callback, this);
        _save->tooltip("Save text to file.");
        _text->buffer(_buffer);
        _text->linenumber_align(FL_ALIGN_RIGHT);
        _text->linenumber_bgcolor(FL_BACKGROUND_COLOR);
        _text->linenumber_fgcolor(FL_FOREGROUND_COLOR);
        _text->linenumber_font(flw::PREF_FIXED_FONT);
        _text->linenumber_size(flw::PREF_FIXED_FONTSIZE);
        _text->linenumber_width(flw::PREF_FIXED_FONTSIZE * 3);
        _text->take_focus();
        _text->textfont(flw::PREF_FIXED_FONT);
        _text->textsize(flw::PREF_FIXED_FONTSIZE);
        util::labelfont(this);

        callback(_DlgText::Callback, this);
        copy_label(title.c_str());
        set_modal();
        resizable(_grid);
        util::center_window(this, priv::PARENT);
        _grid->do_layout();
    }

    /** @brief Cleanup.
    *
    */
    ~_DlgText() {
       _text->buffer(nullptr);
        delete _buffer;
    }

    /** @brief Callback for all widgets.
    *
    */
    static void Callback(Fl_Widget* w, void* o) {
        auto self = static_cast<_DlgText*>(o);

        if (w == self || w == self->_cancel) {
            self->_run = false;
            self->hide();
        }
        else if (w == self->_save) {
            auto filename = fl_file_chooser("Select Destination File", nullptr, nullptr, 0);

            if (filename != nullptr && self->_buffer->savefile(filename) != 0) {
                dlg::msg_alert("Save Error", util::format("Failed to save text to %s", filename));
            }
        }
        else if (w == self->_close) {
            if (self->_edit == true) {
                self->_res = self->_buffer->text();
            }

            self->_run = false;
            self->hide();
        }
    }

    /** @brief Run dialog.
    *
    */
    char* run() {
        _run = true;
        show();

        while (_run == true) {
            Fl::wait();
            Fl::flush();
        }

        return _res;
    }
};

} // flw::priv

/** @brief Show text in a dialog.
*
* @param[in] title   Dialog title.
* @param[in] text    String list.
* @param[in] W       Width in characters.
* @param[in] H       Height in characters.
*/
void dlg::text(const std::string& title, const std::string& text, int W, int H) {
    priv::_DlgText dlg(title.c_str(), text.c_str(), false, W, H);
    dlg.run();
}

/** @brief Show and edit text in a dialog.
*
* @param[in]     title   Dialog title.
* @param[in,out] text    String list.
* @param[in]     W       Width in characters.
* @param[in]     H       Height in characters.
*
* @return True if ok has been pressed and text has been updated.
*
* @snippet dialog.cpp flw::dlg::text_edit example
* @image html text_edit_dialog.png
*/
bool dlg::text_edit(const std::string& title, std::string& text, int W, int H) {
    auto dlg = priv::_DlgText(title.c_str(), text.c_str(), true, W, H);
    auto res = dlg.run();

    if (res == nullptr) {
        return false;
    }

    text = res;
    free(res);
    return true;
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

namespace priv {

/** @brief Select a theme dialog.
*
*/
class _DlgTheme : public Fl_Double_Window {
    Fl_Box*                     _fixed_label;   // Fixed font name and example.
    Fl_Box*                     _font_label;    // Regular font name and example.
    Fl_Browser*                 _theme;         // Theme list.
    Fl_Button*                  _close;         // Close button.
    Fl_Button*                  _fixedfont;     // Select fixed font.
    Fl_Button*                  _font;          // Select regular font.
    Fl_Check_Button*            _scale;         // Turn on/off FLTK scaling.
    Fl_Slider*                  _scale_val;     // Scale value.
    GridGroup*                  _grid;          // Layout widget.
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

        _close       = new Fl_Return_Button(0, 0, 0, 0, label::CLOSE.c_str());
        _fixedfont   = new Fl_Button(0, 0, 0, 0, label::MONO.c_str());
        _fixed_label = new Fl_Box(0, 0, 0, 0);
        _font        = new Fl_Button(0, 0, 0, 0, label::REGULAR.c_str());
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
        util::center_window(this, priv::PARENT);
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
            dlg::Font fd(flw::PREF_FIXED_FONT, flw::PREF_FIXED_FONTSIZE, "Select Monospaced Font");

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
            dlg::Font fd(flw::PREF_FONT, flw::PREF_FONTSIZE, "Select Regular Font");

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

            if (row == priv::THEME_GLEAM) {
                priv::_load_gleam();
            }
            else if (row == priv::THEME_GLEAM_BLUE) {
                priv::_load_gleam_blue();
            }
            else if (row == priv::THEME_GLEAM_DARK) {
                priv::_load_gleam_dark();
            }
            else if (row == priv::THEME_GLEAM_TAN) {
                priv::_load_gleam_tan();
            }
            else if (row == priv::THEME_GTK) {
                priv::_load_gtk();
            }
            else if (row == priv::THEME_GTK_BLUE) {
                priv::_load_gtk_blue();
            }
            else if (row == priv::THEME_GTK_DARK) {
                priv::_load_gtk_dark();
            }
            else if (row == priv::THEME_GTK_TAN) {
                priv::_load_gtk_tan();
            }
            else if (row == priv::THEME_OXY) {
                priv::_load_oxy();
            }
            else if (row == priv::THEME_OXY_TAN) {
                priv::_load_oxy_tan();
            }
            else if (row == priv::THEME_PLASTIC) {
                priv::_load_plastic();
            }
            else if (row == priv::THEME_PLASTIC_TAN) {
                priv::_load_plastic_tan();
            }
            else {
                priv::_load_default();
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
        priv::_scrollbar();

        for (int f = 0; f < priv::THEME_NIL; f++) {
            if (flw::PREF_THEME == flw::PREF_THEMES[f]) {
                _theme->value(f + 1);
                break;
            }
        }

        Fl::redraw();
    }
};

} // flw::priv

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
void dlg::theme(bool enable_font, bool enable_fixedfont) {
    auto dlg = priv::_DlgTheme(enable_font, enable_fixedfont);
    dlg.run();
}

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

/*
 *      ______          _
 *     |  ____|        | |
 *     | |__ ___  _ __ | |_
 *     |  __/ _ \| '_ \| __|
 *     | | | (_) | | | | |_
 *     |_|  \___/|_| |_|\__|
 *
 *
 */

/** @brief Create font dialog.
*
* @param[in] font              Selected font.
* @param[in] fontsize          Font size.
* @param[in] title             Dialog title.
* @param[in] limit_to_default  Set to true to only display the default FLTK fonts.
*/
dlg::Font::Font(Fl_Font font, Fl_Fontsize fontsize, const std::string& title, bool limit_to_default) :
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
dlg::Font::Font(const std::string& font, Fl_Fontsize fontsize, const std::string& title, bool limit_to_default) :
Fl_Double_Window(0, 0, flw::PREF_FONTSIZE * 64, flw::PREF_FONTSIZE * 36) {
    _create(0, font, fontsize, title, limit_to_default);
}

/** @brief Activate select button.
*
*/
void dlg::Font::_activate() {
    if (_fonts->value() == 0 || _sizes->value() == 0 || (_fonts->active() == 0 && _sizes->active() == 0)) {
        _select->deactivate();
    }
    else {
        _select->activate();
    }
}

/** @brief Callbakc for all widgets.
*
* @param[in] w  Widget.
* @param[in] p  Font.
*/
void dlg::Font::Callback(Fl_Widget* w, void* o) {
    auto self = static_cast<Font*>(o);

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
* @param[in] limit_to_default  Set to true to only display the default FLTK font.
*/
void dlg::Font::_create(Fl_Font font, const std::string& fontname, Fl_Fontsize fontsize, const std::string& title, bool limit_to_default) {
    end();

    _cancel   = new Fl_Button(0, 0, 0, 0, label::CANCEL.c_str());
    _fonts    = new ScrollBrowser(12);
    _grid     = new GridGroup();
    _label    = new priv::_FontLabel(0, 0, 0, 0);
    _select   = new Fl_Button(0, 0, 0, 0, label::SELECT.c_str());
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

    _cancel->callback(Font::Callback, this);
    _cancel->labelfont(flw::PREF_FONT);
    _cancel->labelsize(flw::PREF_FONTSIZE);
    _fonts->box(FL_BORDER_BOX);
    _fonts->callback(Font::Callback, this);
    _fonts->textsize(flw::PREF_FONTSIZE);
    _fonts->when(FL_WHEN_CHANGED);
    static_cast<priv::_FontLabel*>(_label)->font = font;
    static_cast<priv::_FontLabel*>(_label)->size = fontsize;
    _select->callback(Font::Callback, this);
    _select->labelfont(flw::PREF_FONT);
    _select->labelsize(flw::PREF_FONTSIZE);
    _sizes->box(FL_BORDER_BOX);
    _sizes->callback(Font::Callback, this);
    _sizes->textsize(flw::PREF_FONTSIZE);
    _sizes->when(FL_WHEN_CHANGED);

    theme::load_fonts();

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
    callback(Font::Callback, this);
    set_modal();
    _fonts->take_focus();
    _grid->resize(0, 0, w(), h());
}

/** @brief Show dialog.
*
* @return True if select button has been pressed.
*/
bool dlg::Font::run() {
    _ret = false;
    _run = true;

    _activate();
    util::center_window(this, priv::PARENT);
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
void dlg::Font::_select_name(const std::string& fontname) {
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

/** @brief Show font dialog.
*
* Input values are not update if user press cancel.
*
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
bool dlg::font(Fl_Font& font, Fl_Fontsize& fontsize, std::string& fontname, bool limit_to_default) {
    auto dlg = dlg::Font(font, fontsize, "Select Font", limit_to_default);

    if (dlg.run() == false) {
        return false;
    }

    font     = dlg.font();
    fontsize = dlg.fontsize();
    fontname = dlg.fontname();

    return true;
}

/*
 *      _____
 *     |  __ \
 *     | |__) | __ ___   __ _ _ __ ___  ___ ___
 *     |  ___/ '__/ _ \ / _` | '__/ _ \/ __/ __|
 *     | |   | | | (_) | (_| | | |  __/\__ \__ \
 *     |_|   |_|  \___/ \__, |_|  \___||___/___/
 *                       __/ |
 *                      |___/
 */

/** @brief Create progress dialog.
*
* But dont show it.\n
* If min and max progress values are equal then the progress bar will be hidden.\n
*
* @param[in] title   Title string.
* @param[in] cancel  True to enable cancel button.
* @param[in] pause   True to enable pause button.
* @param[in] min     Min progress bar value.
* @param[in] max     Max progress bar value.
*/
dlg::Progress::Progress(const std::string& title, bool cancel, bool pause, double min, double max) :
Fl_Double_Window(0, 0, flw::PREF_FONTSIZE * 40, flw::PREF_FONTSIZE * 12) {
    end();

    _cancel   = new Fl_Button(0, 0, 0, 0, label::CANCEL.c_str());
    _grid     = new GridGroup(0, 0, w(), h());
    _label    = new Fl_Hold_Browser(0, 0, 0, 0);
    _pause    = new Fl_Toggle_Button(0, 0, 0, 0, "&Pause");
    _progress = new Fl_Hor_Fill_Slider(0, 0, 0, 0);
    _ret      = true;
    _last     = 0;

    _grid->add(_label,     1,   1,  -1, -11);
    _grid->add(_progress,  1, -10,  -1,   4);
    _grid->add(_pause,   -34,  -5,  16,   4);
    _grid->add(_cancel,  -17,  -5,  16,   4);
    add(_grid);
    range(min, max);

    _cancel->callback(Progress::Callback, this);
    _label->box(FL_BORDER_BOX);
    _label->textfont(flw::PREF_FONT);
    _label->textsize(flw::PREF_FONTSIZE);
    _pause->callback(Progress::Callback, this);

    if (cancel == false) {
        _cancel->deactivate();
    }

    if (pause == false) {
        _pause->deactivate();
    }

    util::labelfont(this);
    callback(Progress::Callback, this);
    copy_label(title.c_str());
    set_modal();
    resizable(_grid);
    _grid->resize(0, 0, w(), h());
}

/** @brief Callback for widgets.
*
* Pause is running a loop until it has unpaused.
*
* @param[in] w  Widget that caused callback.
* @param[in] o  Progress.
*/
void dlg::Progress::Callback(Fl_Widget* w, void* o) {
    auto self = static_cast<Progress*>(o);

    if (w == self) {
    }
    else if (w == self->_cancel) {
        self->_ret = false;
    }
    else if (w == self->_pause) {
        bool cancel = self->_cancel->active();
        self->_cancel->deactivate();

        while (self->_pause->value() != 0) {
            util::sleep(20);
            Fl::check();
        }

        if (cancel == true) {
            self->_cancel->activate();
        }
    }
}

/** @brief Set new progress bar range.
*
* @param[in] min     Min progress bar value.
* @param[in] max     Max progress bar value.
*/
void dlg::Progress::range(double min, double max) {
    if (min < max && fabs(max - min) > 0.001) {
        _progress->show();
        _progress->range(min, max);
        _progress->value(min);
        _grid->resize(_label, 1, 1, -1, -11);
    }
    else {
        _progress->hide();
        _grid->resize(_label, 1, 1, -1, -6);
    }
}

/** @brief Show window centered.
*
* If parent is NULL then windows is centered on screen.\n
* Otherwise it is centered on parent window.
* Some window managers might ignore window positions.
*/
void dlg::Progress::start() {
    util::center_window(this, priv::PARENT);
    Fl_Double_Window::show();
}

/** @brief Update work dialog window.
*
* @param[in] value     Progress bar value.
* @param[in] messages  Message list.
* @param[in] milli     Milliseconds between updating the window.
*
* @return False is cancel has been pressed, otherwise true.
*/
bool dlg::Progress::update(double value, const StringVector& messages, unsigned milli) {
    auto now = static_cast<unsigned>(util::milliseconds());

    if (now - _last > milli) {
        _progress->value(value);
        _label->clear();

        for (const auto& s : messages) {
            _label->add(s.c_str());
        }

        _last = now;
        Fl::check();
        Fl::flush();
    }

    return _ret;
}

/** @brief Update work dialog window.
*
* @param[in] messages  Message list.
* @param[in] milli     Milliseconds between updating the window.
*
* @return False is cancel has been pressed, otherwise true.
*/
bool dlg::Progress::update(const StringVector& messages, unsigned milli) {
    return update(0.0, messages, milli);
}

/** @brief Update work dialog window.
*
* @param[in] value    Progress bar value.
* @param[in] message  Message string.
* @param[in] milli    Milliseconds between updating the window.
*
* @return False is cancel has been pressed, otherwise true.
*/
bool dlg::Progress::update(double value, const std::string& message, unsigned milli) {
    auto messages = std::vector<std::string>();
    messages.push_back(message);

    return update(value, messages, milli);
}

/** @brief Update work dialog window.
*
* @param[in] message  Message string.
* @param[in] milli    Milliseconds between updating the window.
*
* @return False is cancel has been pressed, otherwise true.
*/
bool dlg::Progress::update(const std::string& message, unsigned milli) {
    return update(0.0, message, milli);
}

/** @brief Set progress bar value.
*
* @param[in] value  Number from progress min to progress max.
*/
void dlg::Progress::value(double value) {
    if (value < _progress->minimum()) {
        _progress->value(_progress->minimum());
    }
    else if (value > _progress->maximum()) {
        _progress->value(_progress->maximum());
    }
    else {
        _progress->value(value);
    }
}

} // flw

// MKALGAM_OFF
