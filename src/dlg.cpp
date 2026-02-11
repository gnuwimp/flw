/**
* @file
* @brief Assorted dialog functions and classes.
*
* @author gnuwimp@gmail.com
* @copyright Released under the GNU General Public License v3.0
*/

#include "dlg.h"
#include "gridgroup.h"
#include "scrollbrowser.h"
#include "svgbutton.h"

// MKALGAM_ON

#include <cmath>
#include <FL/Fl_File_Chooser.H>
#include <FL/Fl_Float_Input.H>
#include <FL/Fl_Help_View.H>
#include <FL/Fl_Hor_Value_Slider.H>
#include <FL/Fl_Int_Input.H>
#include <FL/Fl_Multiline_Input.H>
#include <FL/Fl_Output.H>
#include <FL/Fl_Return_Button.H>
#include <FL/Fl_Scroll.H>
#include <FL/Fl_Secret_Input.H>
#include <FL/Fl_Text_Editor.H>

namespace flw {
namespace priv {

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


/** @brief HTML dialog.
* @private
*/
class _DlgHtml : public Fl_Double_Window {
    Fl_Help_View*               _html;  // HTML widget.
    SVGButton*                  _close; // Close button.
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

        _close = new SVGButton(0, 0, 0, 0, labels::CLOSE, icons::BACK, true);
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
        util::center_window(this, flw::PREF_WINDOW);
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

/** @brief Dialog with a browser list widget.
*
*/
class _DlgList : public Fl_Double_Window {
    SVGButton*                  _close;     // Close button.
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

        _close = new SVGButton(0, 0, 0, 0, labels::CLOSE, icons::BACK, true);
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
        util::center_window(this, flw::PREF_WINDOW);
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

/** @brief Message dialog.
* @private
*/
class _DlgMsg : public Fl_Double_Window {
    Fl_Box*                     _label;     // Message label.
    SVGButton*                  _b1;        // First button on the right.
    SVGButton*                  _b2;        // Second button.
    SVGButton*                  _b3;        // Third button.
    SVGButton*                  _b4;        // Fourth button.
    SVGButton*                  _b5;        // Fifth button.
    Fl_Button*                  _icon;      // Icon label.
    Fl_Input*                   _input;     // Input widget.
    GridGroup*                  _grid;      // layout widget.
    bool                        _run;       // Run flag.
    int                         _escape;    // Number of visible buttons, > 1 and dialog can't be closed with escape.
    std::string                 _res;       // Used button label.

public:
    /** @brief Create window.
    *
    * @param[in] type     Type of dialog.
    * @param[in] title    Dialog title.
    * @param[in] message  Message text.
    * @param[in] b1       Right button label.
    * @param[in] b2       Second button label.
    * @param[in] b3       Third button label.
    * @param[in] b4       Fourth button label.
    * @param[in] b5       Fifth button label.
    * @param[in] W        Width in characters.
    * @param[in] H        Height in characters.
    * @param[in] value    Optional input.
    * @param[in] input    Input widget.
    */
    _DlgMsg(
        const std::string&  type,
        const std::string&  title,
        const std::string&  message,
        const std::string&  b1,
        const std::string&  b2,
        const std::string&  b3,
        const std::string&  b4,
        const std::string&  b5,
        int                 W,
        int                 H,
        const char*         value = nullptr,
        const std::string&  input = ""
    ) : Fl_Double_Window(0, 0, flw::PREF_FONTSIZE * W,flw::PREF_FONTSIZE * H) {
        end();

        _b1     = new SVGButton(0, 0, 0, 0, b1);
        _b2     = new SVGButton(0, 0, 0, 0, b2);
        _b3     = new SVGButton(0, 0, 0, 0, b3);
        _b4     = new SVGButton(0, 0, 0, 0, b4);
        _b5     = new SVGButton(0, 0, 0, 0, b5);
        _grid   = new GridGroup(0, 0, w(), h());
        _icon   = new Fl_Button(0, 0, 0, 0);
        _label  = new Fl_Box(0, 0, 0, 0);
        _run    = false;
        _escape = 0;

        auto multi = false;
        auto xpos  = 0;
        auto count = 0;
        auto fixed = false;

        if (input == "Fl_Int_Input") {
            fixed  = true;
            _input = new Fl_Int_Input(0, 0, 0, 0);
        }
        else if (input == "Fl_Float_Input") {
            fixed  = true;
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
            _input->textfont(fixed == true ? flw::PREF_FIXED_FONT : flw::PREF_FONT);
            _input->textsize(flw::PREF_FONTSIZE);
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

        if (_b1->has_return() == true) {
            _b1->take_focus();
        }
        else if (_b2->has_return() == true) {
            _b2->take_focus();
        }
        else if (_b3->has_return() == true) {
            _b3->take_focus();
        }
        else if (_b4->has_return() == true) {
            _b4->take_focus();
        }
        else if (_b5->has_return() == true) {
            _b5->take_focus();
        }
        else {
            _icon->take_focus();
        }

        if (_escape == 1) { // Only one button so turn off eventual highlight color.
            _b1->set_dark(false);
            _b2->set_dark(false);
            _b3->set_dark(false);
            _b4->set_dark(false);
            _b5->set_dark(false);

            // Make sure one button is a return button.

            if (_b1->visible() != 0 && _b1->has_image() == false) {
                _b1->set_icon(_b1->label(), icons::BACK);
                _b1->set_return(true);
            }
            else if (_b2->visible() != 0 && _b2->has_image() == false) {
                _b2->set_icon(_b2->label(), icons::BACK);
                _b2->set_return(true);
            }
            else if (_b3->visible() != 0 && _b3->has_image() == false) {
                _b3->set_icon(_b3->label(), icons::BACK);
                _b3->set_return(true);
            }
            else if (_b4->visible() != 0 && _b4->has_image() == false) {
                _b4->set_icon(_b4->label(), icons::BACK);
                _b4->set_return(true);
            }
            else if (_b5->visible() != 0 && _b5->has_image() == false) {
                _b5->set_icon(_b5->label(), icons::BACK);
                _b5->set_return(true);
            }
        }
        else if (value != nullptr) { // Make ok button a return button (input dialogs have only cancel and ok).
            _b1->set_icon(_b1->label(), icons::BACK);
            _b1->set_return(true);
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
        _grid->do_layout();

        { // Try to resize label and window if it is to wide or if there are to many buttons (only valid for msg_ask()).
            fl_font(flw::PREF_FONT, flw::PREF_FONTSIZE);
            auto width  = 0;
            auto height = 0;

            for (const auto& line : flw::util::split_string(message, "\n")) {
                auto tmp = fl_width(line.c_str());

                if (tmp > _label->w() && tmp > width) {
                    width = tmp;
                }

                height += fl_height();
            }

            if (width > _label->w()) {
                w(w() + width - _label->w() + flw::PREF_FONTSIZE);

                if (w() > Fl::w()) {
                    w(Fl::w());
                }

                _grid->resize(0, 0, w(), h());
                _grid->do_layout();
            }

            if (height > _label->h()) {
                h(h() + height - _label->h() + flw::PREF_FONTSIZE);

                if (h() > Fl::h()) {
                    h(Fl::h());
                }

                _grid->resize(0, 0, w(), h());
                _grid->do_layout();
            }

            if (_b5->visible() != 0 && _b5->x() < _label->x()) {
                w(_b5->w() * 6);
                _grid->resize(0, 0, w(), h());
                _grid->do_layout();
            }
            else if (_b4->visible() != 0 && _b4->x() < _label->x()) {
                w(_b4->w() * 5);
                _grid->resize(0, 0, w(), h());
                _grid->do_layout();
            }
        }

        util::center_window(this, flw::PREF_WINDOW);
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

/** @brief A password dialog.
*
*/
class _DlgPassword : public Fl_Double_Window {
public:
    /** @brief Dialog has four different views.
    *
    */
    enum class Mode {
                                PASSWORD,                       ///< @brief Show only one password input.
                                PASSWORD_CONFIRM,               ///< @brief Show two inputs, password must be the same to enable ok button.
                                PASSWORD_CONFIRM_WITH_FILE,     ///< @brief Two inputs and one file input.
                                PASSWORD_WITH_FILE,             ///< @brief One input and one file input.
    };

private:
    Fl_Button*                  _browse;        ///< @brief Browse file button.
    Fl_Button*                  _cancel;        ///< @brief Cancel button.
    Fl_Button*                  _show;          ///< @brief Toggle show password button.
    Fl_Box*                     _caps;          ///< @brief Show caps on text.
    Fl_Input*                   _file;          ///< @brief File input.
    Fl_Input*                   _password1;     ///< @brief Password input 1.
    Fl_Input*                   _password2;     ///< @brief Password input 2 (confirm).
    GridGroup*                  _grid;          ///< @brief Layout widget.
    SVGButton*                  _close;         ///< @brief Close button.
    Mode                        _mode;          ///< @brief There are 4 modes.
    bool                        _ret;           ///< @brief Return value.
    bool                        _run;           ///< @brief Run flag.

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

        _browse    = new Fl_Button(0, 0, 0, 0, labels::BROWSE.c_str());
        _cancel    = new Fl_Button(0, 0, 0, 0, "");
        _caps      = new Fl_Box(0, 0, 0, 0, "");
        _close     = new SVGButton(0, 0, 0, 0, "", icons::BACK, true);
        _file      = new Fl_Output(0, 0, 0, 0, "Key file");
        _grid      = new GridGroup(0, 0, w(), h());
        _password1 = new Fl_Secret_Input(0, 0, 0, 0, "Password");
        _password2 = new Fl_Secret_Input(0, 0, 0, 0, "Enter password again");
        _show      = new Fl_Button(0, 0, 0, 0, "&Show");
        _mode      = mode;
        _ret       = false;
        _run       = false;

        _grid->add(_password1,  1,   3,  -1,  4);
        _grid->add(_password2,  1,  10,  -1,  4);
        _grid->add(_file,       1,  17,  -1,  4);
        _grid->add(_caps,       1,  -5,  10,  4);
        _grid->add(_browse,   -68,  -5,  16,  4);
        _grid->add(_show,     -51,  -5,  16,  4);
        _grid->add(_cancel,   -34,  -5,  16,  4);
        _grid->add(_close,    -17,  -5,  16,  4);
        add(_grid);

        _browse->callback(_DlgPassword::Callback, this);
        _browse->tooltip("Show file dialog and select a key file.");
        _cancel->callback(_DlgPassword::Callback, this);
        _cancel->copy_label(labels::CANCEL.c_str());
        _caps->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE | FL_ALIGN_CLIP);
        _caps->labelcolor(FL_DARK_RED);
        _close->callback(_DlgPassword::Callback, this);
        _close->copy_label(labels::OK.c_str());
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
        _show->callback(_DlgPassword::Callback, this);
        _show->tooltip("Show or hide password.");

        auto W = flw::PREF_FONTSIZE * 42;
        auto H = flw::PREF_FONTSIZE * 13.5;

        if (_mode == Mode::PASSWORD) {
            _password2->hide();
            _browse->hide();
            _file->hide();
            W = flw::PREF_FONTSIZE * 40;
            H = flw::PREF_FONTSIZE * 6.5;
        }
        else if (_mode == Mode::PASSWORD_CONFIRM) {
            _browse->hide();
            _file->hide();
            W = flw::PREF_FONTSIZE * 40;
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
        util::center_window(this, flw::PREF_WINDOW);
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
        else if (w == self->_close) {
            self->_ret = true;
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
        else if (w == self->_show) {
            if (self->_show->label() == std::string("&Hide")) {
                self->_show->label("&Show");
                self->_password1->input_type(FL_SECRET_INPUT);
                self->_password2->input_type(FL_SECRET_INPUT);
                self->_password1->take_focus();
            }
            else {
                self->_show->label("&Hide");
                self->_password1->input_type(FL_NORMAL_INPUT);
                self->_password2->input_type(FL_NORMAL_INPUT);
                self->_password1->take_focus();
            }

            Fl::redraw();
        }
    }

    /** @brief Enable close button.
    *
    */
    void check() {
        auto p1 = _password1->value();
        auto p2 = _password2->value();
        auto f  = _file->value();

        if (Fl::event_state() & FL_CAPS_LOCK) {
            _caps->label("Caps On");
        }
        else {
            _caps->label("");
        }

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

/** @brief A dialog with check buttons.
* @private
*/
class _DlgSelectCheckBoxes : public Fl_Double_Window {
    Fl_Button*                  _all;           // Turn on all check buttons.
    Fl_Button*                  _cancel;        // Cancel dialog.
    Fl_Button*                  _invert;        // Invert all check buttons.
    Fl_Button*                  _none;          // Uncheck all check buttons.
    Fl_Scroll*                  _scroll;        // Scroller for check buttons.
    GridGroup*                  _grid;          // Main layout widget.
    SVGButton*                  _close;         // Close dialog.
    WidgetVector                _checkbuttons;  // Vector with check buttons.
    bool                        _ret;           // Return value from run.
    bool                        _run;           // Run flag.
    const StringVector&         _labels;        // Labels for check buttons.

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
        _cancel = new Fl_Button(0, 0, 0, 0, labels::CANCEL.c_str());
        _close  = new SVGButton(0, 0, 0, 0, labels::OK, icons::BACK, true);
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
        util::center_window(this, flw::PREF_WINDOW);
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

/** @brief A dialog with a choice widget.
* @private
*/
class _DlgSelectChoice : public Fl_Double_Window {
    Fl_Box*                     _label;     // Message label.
    Fl_Button*                  _cancel;    // Cancel button.
    Fl_Button*                  _icon;      // Icon label.
    Fl_Choice*                  _choice;    // Choice widget with user strings to select from.
    GridGroup*                  _grid;      // Layout widget.
    SVGButton*                  _ok;        // Ok button.
    bool                        _run;       // Run flag.
    int                         _ret;       // Return value from run.

public:
    /** @brief Create window.
    *
    * @param[in] title     Dialog title.
    * @param[in] message   Message string.
    * @param[in] strings   Strings for the choce widget.
    * @param[in] selected  Selected string index.
    */
    _DlgSelectChoice(const std::string& title, const std::string& message, const StringVector& strings, int selected) :
    Fl_Double_Window(0, 0, flw::PREF_FONTSIZE * 40, flw::PREF_FONTSIZE * 9.5) {
        end();

        _cancel = new Fl_Button(0, 0, 0, 0, labels::CANCEL.c_str());
        _choice = new Fl_Choice(0, 0, 0, 0);
        _grid   = new GridGroup(0, 0, w(), h());
        _icon   = new Fl_Button(0, 0, 0, 0);
        _label  = new Fl_Box(0, 0, 0, 0);
        _ok     = new SVGButton(0, 0, 0, 0, labels::OK, icons::BACK, true, false);
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
        _choice->textfont(flw::PREF_FONT);
        _choice->textsize(flw::PREF_FONTSIZE);
        _choice->value(selected);
        util::icon(_icon, icons::RIGHT, flw::PREF_FONTSIZE * 4);
        _icon->box(FL_BORDER_BOX);
        _label->align(FL_ALIGN_INSIDE | FL_ALIGN_LEFT | FL_ALIGN_TOP | FL_ALIGN_CLIP);
        _label->copy_label(message.c_str());
        _label->box(FL_BORDER_BOX);
        _ok->callback(_DlgSelectChoice::Callback, this);

        util::labelfont(this);
        callback(_DlgSelectChoice::Callback, this);
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

/** @brief Dialog with a list of string to select from.
*
*/
class _DlgSelectString : public Fl_Double_Window {
    Fl_Button*                  _cancel;        // Cancel dialog button.
    Fl_Input*                   _filter;        // Enter text to filter strings.
    GridGroup*                  _grid;          // Layout widget.
    SVGButton*                  _select;        // Select string and close button.
    ScrollBrowser*              _list;          // String list widget.
    bool                        _run;           // Run flag;
    const StringVector&         _strings;       // Input strings.

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

        _cancel = new Fl_Button(0, 0, 0, 0, labels::CANCEL.c_str());
        _filter = new Fl_Input(0, 0, 0, 0);
        _grid   = new GridGroup(0, 0, w(), h());
        _list   = new ScrollBrowser(9, 0, 0, 0, 0);
        _select = new SVGButton(0, 0, 0, 0, labels::SELECT, icons::BACK, true);
        _run    = false;

        _grid->add(_filter,   1,  1, -1,  4);
        _grid->add(_list,     1,  6, -1, -6);
        _grid->add(_cancel, -34, -5, 16,  4);
        _grid->add(_select, -17, -5, 16,  4);
        add(_grid);

        _cancel->callback(_DlgSelectString::Callback, this);
        _filter->callback(_DlgSelectString::Callback, this);
        _filter->tooltip("Enter text to filter rows that macthes the text.\nPress tab to switch focus between input and list widget.");
        _filter->when(FL_WHEN_CHANGED);
        _list->callback(_DlgSelectString::Callback, this);
        _list->tooltip("Use Page Up or Page Down in list to scroll faster,");
        _select->callback(_DlgSelectString::Callback, this);

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
                    r = f;
                }

                f++;
            }

            if (selected_string_index >= 0 && selected_string_index < static_cast<int>(_strings.size())) {
                _list->value(selected_string_index + 1);
                _list->middleline(selected_string_index + 1);
            }
            else {
                _list->value(r + 1);
                _list->middleline(r + 1);
            }
        }

        _filter->take_focus();
        util::labelfont(this);
        callback(_DlgSelectString::Callback, this);
        copy_label(title.c_str());
        activate_button();
        set_modal();
        resizable(_grid);
        util::center_window(this, flw::PREF_WINDOW);
        _grid->do_layout();
    }

    /** @brief Activate close button if a string is selected.
    *
    */
    void activate_button() {
        if (_list->value() == 0) {
            _select->deactivate();
        }
        else {
            _select->activate();
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
        else if (w == self->_select) {
            self->_run = false;
            self->hide();
        }
        else if (w == self->_filter) {
            self->filter(self->_filter->value());
            self->activate_button();
        }
        else if (w == self->_list) {
            self->activate_button();

            if (Fl::event_clicks() > 0 && self->_select->active()) {
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

/** @brief Set a number dialog that uses a slider.
*
*/
class _DlgSlider : public Fl_Double_Window {
    Fl_Button*                  _cancel;    // Cancel button.
    Fl_Hor_Value_Slider*        _slider;    // Number slider.
    GridGroup*                  _grid;      // Layout widget.
    SVGButton*                  _ok;        // Close dialog and set new value.
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

        _cancel = new Fl_Button(0, 0, 0, 0, labels::CANCEL.c_str());
        _grid   = new GridGroup(0, 0, w(), h());
        _ok     = new SVGButton(0, 0, 0, 0, labels::OK, icons::BACK, true);
        _slider = new Fl_Hor_Value_Slider(0, 0, 0, 0);
        _ret    = false;
        _run    = false;

        _grid->add(_slider,   1,  1, -1,  4);
        _grid->add(_cancel, -34, -5, 16,  4);
        _grid->add(_ok,     -17, -5, 16,  4);
        add(_grid);

        _cancel->callback(_DlgSlider::Callback, this);
        _ok->callback(_DlgSlider::Callback, this);
        _slider->align(FL_ALIGN_LEFT    );
        _slider->callback(_DlgSlider::Callback, this);
        _slider->range(min, max);
        _slider->value(value);
        _slider->step(step);
        _slider->textfont(flw::PREF_FONT);
        _slider->textsize(flw::PREF_FONTSIZE);

        util::labelfont(this);
        callback(_DlgSlider::Callback, this);
        copy_label(title.c_str());
        set_modal();
        resizable(_grid);
        util::center_window(this, flw::PREF_WINDOW);
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
        else if (w == self->_ok) {
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

/** @brief Show text in a dialog.
*
*/
class _DlgText : public Fl_Double_Window {
    Fl_Button*                  _cancel;    // Cancel button.
    Fl_Button*                  _save;      // Save text button.
    Fl_Text_Buffer*             _buffer;    // Text buffer.
    Fl_Text_Display*            _text;      // Text display.
    GridGroup*                  _grid;      // Layout widget.
    SVGButton*                  _close;     // Close/update button.
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
        _cancel = new Fl_Button(0, 0, 0, 0, labels::CANCEL.c_str());
        _close  = new SVGButton(0, 0, 0, 0, (edit == true) ? labels::UPDATE : labels::CLOSE, icons::BACK, true);
        _grid   = new GridGroup(0, 0, w(), h());
        _save   = new Fl_Button(0, 0, 0, 0, labels::SAVE_DOT.c_str());
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
        util::center_window(this, flw::PREF_WINDOW);
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
} // flw

/*
 *       __ _                 _ _
 *      / _| |         _ _   | | |
 *     | |_| |_      _(_|_)__| | | __ _
 *     |  _| \ \ /\ / /   / _` | |/ _` |
 *     | | | |\ V  V / _ | (_| | | (_| |
 *     |_| |_| \_/\_/ (_|_)__,_|_|\__, |
 *                                 __/ |
 *                                |___/
 */

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
void flw::dlg::html(const std::string& title, const std::string& text, int W, int H) {
    priv::_DlgHtml dlg(title, text, W, H);
    dlg.run();
}

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
void flw::dlg::list(const std::string& title, const StringVector& list, bool fixed_font, int W, int H) {
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
void flw::dlg::list(const std::string& title, const std::string& list, bool fixed_font, int W, int H) {
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
void flw::dlg::list_file(const std::string& title, const std::string& file, bool fixed_font, int W, int H) {
    priv::_DlgList dlg(title, flw::StringVector(), file, fixed_font, W, H);
    dlg.run();
}

/** @brief Show an text input dialog.
*
* It will try to increase window width and height if message is to large.\n
*
* @param[in]     title    Dialog title.
* @param[in]     message  Message text.
* @param[in,out] value    Text line to edit.
* @param[in]     W        Width in characters.
* @param[in]     H        Height in characters.
*
* @return flw::labels::OK or flw::labels::CANCEL.
*
* @image html dlg_input.png
*/
std::string flw::dlg::input(const std::string& title, const std::string& message, std::string& value, int W, int H) {
    priv::_DlgMsg dlg(icons::EDIT, title, message, labels::OK, labels::CANCEL, "", "", "", W, H, value.c_str(), "Fl_Input");
    auto res = dlg.run();

    if (res == labels::OK) {
        value = dlg.input();
    }

    return res;
}

/** @brief Show an decimal input dialog.
*
* It will try to increase window width and height if message is to large.\n
* If the number is invalid, the input number will be returned.\n
*
* @param[in]     title    Dialog title.
* @param[in]     message  Message text.
* @param[in,out] value    Decimal number to edit.
* @param[in]     W        Width in characters.
* @param[in]     H        Height in characters.
*
* @return flw::labels::OK or flw::labels::CANCEL.
*/
std::string flw::dlg::input_double(const std::string& title, const std::string& message, double& value, int W, int H) {
    char num[500];
    snprintf(num, 500, "%g", value);

    priv::_DlgMsg dlg(icons::EDIT, title, message, labels::OK, labels::CANCEL, "", "", "", W, H, num, "Fl_Float_Input");
    auto res = dlg.run();

    if (res == labels::OK) {
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
* It will try to increase window width and height if message is to large.\n
* If the number is invalid, the input number will be returned.\n
*
* @param[in]     title    Dialog title.
* @param[in]     message  Message text.
* @param[in,out] value    Integer number to edit.
* @param[in]     W        Width in characters.
* @param[in]     H        Height in characters.
*
* @return flw::labels::OK or flw::labels::CANCEL.
*/
std::string flw::dlg::input_int(const std::string& title, const std::string& message, int64_t& value, int W, int H) {
    char num[100];
    snprintf(num, 100, "%lld", static_cast<long long int>(value));

    priv::_DlgMsg dlg(icons::EDIT, title, message, labels::OK, labels::CANCEL, "", "", "", W, H, num, "Fl_Int_Input");
    auto res = dlg.run();

    if (res == labels::OK) {
        value = util::to_int(dlg.input(), value);
    }

    return res;
}

/** @brief Show an multiline input dialog.
*
* Don't edit to many lines of text because there are no scrollbars.\n
* It will try to increase window width and height if message is to large.\n
*
* @see To edit longer texts use dlg::text_edit()
*
* @param[in]     title    Dialog title.
* @param[in]     message  Message text.
* @param[in,out] value    Multi line text to edit.
* @param[in]     W        Width in characters.
* @param[in]     H        Height in characters.
*
* @return flw::labels::OK or flw::labels::CANCEL.
*
* @image html dlg_input_multi.png
*/
std::string flw::dlg::input_multi(const std::string& title, const std::string& message, std::string& value, int W, int H) {
    priv::_DlgMsg dlg(icons::EDIT, title, message, labels::OK, labels::CANCEL, "", "", "", W, H, value.c_str(), "Fl_Multiline_Input");
    auto res = dlg.run();

    if (res == labels::OK) {
        value = dlg.input();
    }

    return res;
}

/** @brief Show an secret text input dialog.
*
* It will try to increase window width and height if message is to large.\n
*
* @param[in]     title    Dialog title.
* @param[in]     message  Message text.
* @param[in,out] value    Secret text to edit.
* @param[in]     W        Width in characters.
* @param[in]     H        Height in characters.
*
* @return flw::labels::OK or flw::labels::CANCEL.
*
* @image html dlg_input_secret.png
*/
std::string flw::dlg::input_secret(const std::string& title, const std::string& message, std::string& value, int W, int H) {
    priv::_DlgMsg dlg(icons::PASSWORD, title, message, labels::OK, labels::CANCEL, "", "", "", W, H, value.c_str(), "Fl_Secret_Input");
    auto res = dlg.run();

    if (res == labels::OK) {
        value = dlg.input();
    }

    return res;
}

/** @brief Show an dialog with a message icon.
*
* It will try to increase window width and height if message is to large.\n
*
* @param[in] title    Dialog title.
* @param[in] message  Message text.
* @param[in] W        Width in characters.
* @param[in] H        Height in characters.
*
* @image html dlg_msg.png
*/
void flw::dlg::msg(const std::string& title, const std::string& message, int W, int H) {
    priv::_DlgMsg dlg(icons::INFO, title, message, labels::CLOSE, "", "", "", "", W, H);
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
void flw::dlg::msg_alert(const std::string& title, const std::string& message, int W, int H) {
    priv::_DlgMsg dlg(icons::ALERT, title, message, labels::CLOSE, "", "", "", "", W, H);
    dlg.run();
}

/** @brief Show an dialog with a question icon.
*
* You can show 1 to 5 buttons to choose from.\n
* Any button label can be empty and it will be hidden.\n
*
* @see dlg::button() on how to add a icon.\n
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
std::string flw::dlg::msg_ask(const std::string& title, const std::string& message, const std::string& b1, const std::string& b2, const std::string& b3, const std::string& b4, const std::string& b5, int W, int H) {
    priv::_DlgMsg dlg(icons::QUESTION, title, message, b1, b2, b3, b4, b5, W, H);
    return dlg.run();
}

/** @brief Show an dialog with an error icon.\n
*
* @param[in] title    Dialog title.
* @param[in] message  Message text.
* @param[in] W        Width in characters.
* @param[in] H        Height in characters.
*/
void flw::dlg::msg_error(const std::string& title, const std::string& message, int W, int H) {
    priv::_DlgMsg dlg(icons::ERR, title, message, labels::CLOSE, "", "", "", "", W, H);
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
std::string flw::dlg::msg_warning(const std::string& title, const std::string& message, const std::string& b1, const std::string& b2, int W, int H) {
    priv::_DlgMsg dlg(icons::WARNING, title, message, b1, b2, "", "", "", W, H);
    return dlg.run();
}

/** @brief Get password from user.
*
* @param[in]  title     Dialog title.
* @param[out] password  User password.
*
* @return True if ok has been pressed and password argument has been set.
*/
bool flw::dlg::password(const std::string& title, std::string& password) {
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
bool flw::dlg::password_confirm(const std::string& title, std::string& password) {
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
bool flw::dlg::password_confirm_and_file(const std::string& title, std::string& password, std::string& file) {
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
bool flw::dlg::password_and_file(const std::string& title, std::string& password, std::string& file) {
    priv::_DlgPassword dlg(title.c_str(), priv::_DlgPassword::Mode::PASSWORD_WITH_FILE);
    return dlg.run(password, file);
}

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
flw::StringVector flw::dlg::select_checkboxes(const std::string& title, const StringVector& list) {
    priv::_DlgSelectCheckBoxes dlg(title, list);
    return dlg.run();
}

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
int flw::dlg::select_choice(const std::string& title, const std::string& message, const StringVector& list, int selected) {
    priv::_DlgSelectChoice dlg(title, message, list, selected);
    return dlg.run();
}

/** @brief Show a dialog with strings to select from.
*
* There is also an input widget to filter strings in list.\n
* The returned index is always the original index in input vector.\n
*
* @param[in] title         Dialog title.
* @param[in] list          Vector with strings.
* @param[in] selected_row  Selected row in vector.
* @param[in] fixed_font    True to use a fixed font.
* @param[in] W             Width in characters.
* @param[in] H             Height in characters.
*
* @return Selected index in input vector.
*/
int flw::dlg::select_string(const std::string& title, const StringVector& list, int selected_row, bool fixed_font, int W, int H) {
    priv::_DlgSelectString dlg(title.c_str(), list, selected_row, "", fixed_font, W, H);
    return dlg.run();
}

/** @brief Show a dialog with strings to select from.
*
* There is also an input widget to filter strings in list.\n
* The returned index is always the original index in input vector.\n
*
* @param[in] title         Dialog title.
* @param[in] list          Vector with strings.
* @param[in] selected_row  Selected row in vector.
* @param[in] fixed_font    True to use a fixed font.
* @param[in] W             Width in characters.
* @param[in] H             Height in characters.
*
* @return Selected index in input vector.
*
* @snippet dialog.cpp flw::dlg::select example
* @image html select_string_dialog.png
*/
int flw::dlg::select_string(const std::string& title, const StringVector& list, const std::string& selected_row, bool fixed_font, int W, int H) {
    priv::_DlgSelectString dlg(title.c_str(), list, -1, selected_row, fixed_font, W, H);
    return dlg.run();
}

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
bool flw::dlg::slider(const std::string& title, double min, double max, double& value, double step) {
    priv::_DlgSlider dlg(title.c_str(), min, max, value, step);
    return dlg.run();
}

/** @brief Show text in a dialog.
*
* @param[in] title   Dialog title.
* @param[in] text    String list.
* @param[in] W       Width in characters.
* @param[in] H       Height in characters.
*/
void flw::dlg::text(const std::string& title, const std::string& text, int W, int H) {
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
bool flw::dlg::text_edit(const std::string& title, std::string& text, int W, int H) {
    auto dlg = priv::_DlgText(title.c_str(), text.c_str(), true, W, H);
    auto res = dlg.run();

    if (res == nullptr) {
        return false;
    }

    text = res;
    free(res);
    return true;
}

/** @brief Put fl_message dialogs at the centre of the desktop.
*
*/
void flw::dlg::center_fl_message_dialog() {
    int X, Y, W, H;
    Fl::screen_xywh(X, Y, W, H);
    fl_message_position(W / 2, H / 2, 1);
}

/** @brief Show an dialog with message and then exit the program.
*
* @param[in] message  Message string.
*/
void flw::dlg::panic(const std::string& message) {
    dlg::msg_error("Application Error", util::format("PANIC! I have to quit\n%s", message.c_str()));
    exit(1);
}

// MKALGAM_OFF
