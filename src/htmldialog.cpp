/**
* @file
* @brief An html viewer dialog.
*
* @author gnuwimp@gmail.com
* @copyright Released under the GNU General Public License v3.0
*/

#include "dlg.h"
#include "file.h"
#include "gridgroup.h"
#include "htmldialog.h"
#include "inputmenu.h"
#include "svgbutton.h"

// MKALGAM_ON

#include <FL/Fl_Button.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Help_View.H>
#include <FL/Fl_Shared_Image.H>

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

/** @brief Create empty dialog.
*
* @param[in] W  Width in characters.
* @param[in] H  Height in characters.
*/
flw::HTMLDialog::HTMLDialog(int W, int H) :
Fl_Double_Window(0, 0, flw::PREF_FONTSIZE * W,flw::PREF_FONTSIZE * H) {
    end();

    fl_register_images();

    _close   = new Fl_Button(0, 0, 0, 0, labels::CLOSE.c_str());
    _copy    = new Fl_Button(0, 0, 0, 0, "Copy");
    _back    = new SVGButton(0, 0, 0, 0, "", icons::LEFT, false, false, SVGButton::Pos::UNDER);
    _dec     = new SVGButton(0, 0, 0, 0, "", icons::MINUS, false, false, SVGButton::Pos::UNDER);
    _forward = new SVGButton(0, 0, 0, 0, "", icons::RIGHT, false, false, SVGButton::Pos::UNDER);
    _grid    = new GridGroup(0, 0, w(), h());
    _home    = new SVGButton(0, 0, 0, 0, "", icons::HOME, false, false, SVGButton::Pos::UNDER);
    _html    = new Fl_Help_View(0, 0, 0, 0);
    _inc     = new SVGButton(0, 0, 0, 0, "", icons::PLUS, false, false, SVGButton::Pos::UNDER);
    _input   = new InputMenu(0, 0, 0, 0);
    _top     = new SVGButton(0, 0, 0, 0, "", icons::UP, false, false, SVGButton::Pos::UNDER);
    _view    = new SVGButton(0, 0, 0, 0, "", icons::EDIT, false, false, SVGButton::Pos::UNDER);
    _pos     = 0;
    _run     = false;

    _grid->add(_input,      1,  1,  -1,  4);
    _grid->add(_html,       1,  6,  -1, -6);
    _grid->add(_view,     -69, -5,   4,  4);
    _grid->add(_dec,      -64, -5,   4,  4);
    _grid->add(_inc,      -59, -5,   4,  4);
    _grid->add(_back,     -54, -5,   4,  4);
    _grid->add(_forward,  -49, -5,   4,  4);
    _grid->add(_top,      -44, -5,   4,  4);
    _grid->add(_home,     -39, -5,   4,  4);
    _grid->add(_copy,     -34, -5,  16,  4);
    _grid->add(_close,    -17, -5,  16,  4);
    add(_grid);

    _back->callback(HTMLDialog::_Callback, this);
    _back->tooltip("Go back to previous page in history.");
    _close->callback(HTMLDialog::_Callback, this);
    _copy->callback(HTMLDialog::_Callback, this);
    _copy->tooltip("Copy selected text or all text if no text is selected.");
    _dec->callback(HTMLDialog::_Callback, this);
    _dec->tooltip("Decrease font size.");
    _forward->callback(HTMLDialog::_Callback, this);
    _forward->tooltip("Go to next page in history.");
    _html->link(HTMLDialog::_CallbackLink);
    _html->textfont(flw::PREF_FONT);
    _html->user_data(this);
    _home->callback(HTMLDialog::_Callback, this);
    _home->tooltip("Go to to start page.");
    _inc->callback(HTMLDialog::_Callback, this);
    _inc->tooltip("Increase font size.");
    _input->callback(HTMLDialog::_Callback, this);
    _input->tooltip("Enter search text to find text in paragraphs.\nPress ctrl + enter to search from the start.");
    _input->when(FL_WHEN_ENTER_KEY_ALWAYS);
    _top->callback(HTMLDialog::_Callback, this);
    _top->tooltip("Jump to top of page.");
    _view->callback(HTMLDialog::_Callback, this);
    _view->tooltip("View html for current page.");

    flw::util::labelfont(this);
    _input->update_pref(flw::PREF_FONT, flw::PREF_FONTSIZE);
    _set_font_size(flw::PREF_FONTSIZE);
    callback(HTMLDialog::_Callback, this);
    resizable(_grid);
    _grid->do_layout();
    flw::util::center_window(this, flw::PREF_WINDOW);
    _input->take_focus();
    clear_pages();

#ifdef DEBUG
    _input->insert("debug", 20);
#endif
}

/** @brief Add page to internal hash.
*
* To jump between pages use the name in links.\n
* add_page("NAME", HTML1).\n
* <a href="NAME">Link to page</a>.\n
*
* @param[in] name  Name of text.
* @param[in] page  HTML text.
*/
void flw::HTMLDialog::add_page(const std::string& name, const std::string& page) {
    if (_pages.size() == 0) {
        _links1.clear();
        _links2.clear();
        _pages[name] = page;
        _set_page(name, true);
    }
    else {
        _pages[name] = page;
    }
}

/** @brief Callback for all widgets.
*
*/
void flw::HTMLDialog::_Callback(Fl_Widget* w, void* o) {
    auto self = static_cast<HTMLDialog*>(o);

    if (w == self || w == self->_close) {
        self->_run = false;
        self->hide();
    }
    else if (w == self->_back) {
        self->_go_back();
        self->_input->take_focus();
    }
    else if (w == self->_copy) {
        self->_copy_text();
    }
    else if (w == self->_dec) {
        self->_set_font_size(self->_html->labelsize() - 2);
    }
    else if (w == self->_forward) {
        self->_go_forward();
        self->_input->take_focus();
    }
    else if (w == self->_home) {
        self->_go_home();
        self->_input->take_focus();
    }
    else if (w == self->_inc) {
        self->_set_font_size(self->_html->labelsize() + 2);
        self->_input->take_focus();
    }
    else if (w == self->_input) {
        self->_find();
    }
    else if (w == self->_top) {
        self->_pos = 0;
        self->_html->topline(0);
        self->_input->take_focus();
    }
    else if (w == self->_view) {
        dlg::text("HTML", util::to_string(self->_html->value()));
    }

    self->_update_buttons();
}


/** @brief Call _go_link().
*
*/
const char* flw::HTMLDialog::_CallbackLink(Fl_Widget* w, const char* uri) {
    return static_cast<HTMLDialog*>(w->user_data())->_go_link(uri);
}

/** @brief Clear all embedded texts and history.
*
*/
void flw::HTMLDialog::clear_pages() {
    _pages.clear();
    _links1.clear();
    _links2.clear();
    _update_buttons();
}

/** @brief Copy text to clipboard.
*
*/
void flw::HTMLDialog::_copy_text() {
    if (_html->text_selected() == 0) {
        _html->select_all();
        _html->copy();
        _html->clear_selection();
    }
    else {
        _html->copy();
    }
}

/** @brief Print debug info.
*
*/
void flw::HTMLDialog::_debug() const {
#ifdef DEBUG
    puts("\nflw::HTMLDialog:");
    printf("    pos     = %d\n", _pos);
    printf("    dir     = %s\n", _html->directory());
    printf("    file    = %s\n", _html->filename());

    auto e = 1;

    printf("============= %d Pages\n", (int) _pages.size());
    for (auto& t : _pages) {
        printf("      %05d = %s\n", (int) t.second.length(), t.first.c_str());
    }

    printf("============= History <---\n");
    e = 1;
    for (auto& l : _links1) {
        printf("         %02d = %s\n", e++, l.c_str());
    }

    printf("============= History --->\n");
    e = (int) (_links1.size() + _links2.size());
    for (auto& l : _links2) {
        printf("         %02d = %s\n", e--, l.c_str());
    }

    puts("");
    fflush(stdout);
#endif
}

/** @brief Find text in document.
*
* Enter "debug" to print debug info to stdout.
*/
void flw::HTMLDialog::_find() {
    if (_input->value() == "") {
        return;
    }
#ifdef DEBUG
    else if (_input->value() == std::string("debug")) {
        _debug();
        return;
    }
#endif

    if (Fl::event_ctrl() != 0) {
        _pos = 0;
    }

    _pos = _html->find(_input->value().c_str(), _pos + 1);
    _input->insert(_input->value(), 20);
}

/** @brief Go back to previous page in history.
*
*/
void flw::HTMLDialog::_go_back() {
    if (_links1.size() < 2) {
        return;
    }

    if (_pages.size() == 0) {
        _links2.push_back(_links1.back());
        _links1.pop_back();

        auto l = _links1.back();
        _links1.pop_back();

        auto tmp = _links2; // Forward history will be deleted so save it first and restore it after load.
        _html->load(l.c_str());
        _links2 = tmp;
    }
    else {
        _links2.push_back(_links1.back());
        _links1.pop_back(); // Remove current/old page.

        auto l = _links1.back();
        _links1.pop_back(); // Remove new page because it will be added again.
        _set_page(l, false); // Load new page and do not clear forward history.
    }
}

/** @brief Go to next page in history.
*
*/
void flw::HTMLDialog::_go_forward() {
    if (_links2.size() == 0) {
        return;
    }

    if (_pages.size() == 0) {
        auto l = _links2.back();
        _links2.pop_back();

        auto tmp = _links2; // Forward history will be deleted so save it first and restore it after load.
        _html->load(l.c_str());
        _links2 = tmp;
    }
    else {
        auto l = _links2.back();
        _links2.pop_back(); // Remove last page.
        _set_page(l, false); // And set text and add it to _list1 again.
    }
}

/** @brief Go to start page or file.
*
*/
void flw::HTMLDialog::_go_home() {
    if (_links1.size() == 0) {
        return;
    }

    if (_pages.size() == 0) {
        _html->load(_links1[0].c_str());
    }
    else {
        _set_page(_links1[0], true);
    }
}

/** @brief Load text from file or from internal texts.
*
* @param[in] link  The link.
*
* @return Input link or nullptr if text have been set.
*/
const char* flw::HTMLDialog::_go_link(const char* link) {
    if (_pages.size() > 0) {
        auto link2 = std::string(link);
        auto path  = link2.rfind("/");
        auto key   = (path != std::string::npos) ? link2.substr(path + 1) : link2;

        if (_pages.find(key) != _pages.end()) {
            _set_page(key, true);
            return nullptr;
        }

        auto e = util::file_ext(link, true);

        if (e == "" || e == "html" || e == "htm") {
            _links1.push_back(link);
            _links2.clear();
            _update_buttons();
        }

        return link;
    }
    else {
        auto e = util::file_ext(link, true);

        if (e == "html" || e == "htm") {
            if (_links1.size() == 0) {
                _links1.push_back(link);
            }
            else if (_links1.size() > 0 && _links1.back() != link) {
                _links1.push_back(link);
            }

            _links2.clear();
        }

        _update_buttons();
        return link;
    }
}

/** @brief Jump back/forward with mouse buttons 4 and 5.
*
* @param[in] event  FLTK event.
*/
int flw::HTMLDialog::handle(int event) {
    if (event == FL_PUSH) {
        if (Fl::event_button4() != 0 && _back->active() != 0) {
            _go_back();
            _update_buttons();
        }
        else if (Fl::event_button5() != 0 && _forward->active() != 0) {
            _go_forward();
            _update_buttons();
        }
    }

    return Fl_Double_Window::handle(event);
}

/** @brief Load file, can't be used with embedded pages.
*
* @param[in] file  HTML file.
*/
void flw::HTMLDialog::load_page(const std::string& file) {
    clear_pages();
    _html->load(gnu::file::File(file, true).c_str());
    _update_buttons();
}

/** @brief Run dialog.
*
* @param[in] modal  True to show dialog in modal mode.
*/
void flw::HTMLDialog::run(bool modal) {
    _run = true;

    if (modal == true) {
        set_modal();
    }

    show();

    while (_run == true) {
        Fl::wait();
        Fl::flush();
    }

    clear_modal_states();
}

/** @brief Set new font size between 10 and 36.
*
* And change scroll speed.
*/
void flw::HTMLDialog::_set_font_size(int size) {
    if (size > 36) {
        size = 36;
    }
    else if (size < 10) {
        size = 10;
    }

    _html->scrollbar()->linesize(size * 8);
    _html->labelsize(size);
    _html->textsize(size);
}

/** @brief Set HTML text.
*
* @param[in] page          Document name used in HTMLDialog::add_page().
* @param[in] clear_links2  True to clear forward links.
*/
void flw::HTMLDialog::_set_page(const std::string& page, bool clear_links2) {
    auto find = _pages.find(page);

    if (find != _pages.end()) {
        _html->value(find->second.c_str());
    }
    else { // This probably generates an error message.
        _html->load(page.c_str());
    }

    if (_links1.size() == 0) { // Always add first page.
        _links1.push_back(page);
    }
    else if (_links1.size() > 0 && _links1.back() != page) { // Add only a new page if is different from current page.
        _links1.push_back(page);
    }

    if (clear_links2 == true) { // Always clear forward list if a link has been clicked.
        _links2.clear();
    }

   _update_buttons();
}

/** @brief Enable or disable some buttons.
*
*/
void flw::HTMLDialog::_update_buttons() {
    if (_links1.size() == 0) {
        _home->deactivate();
    }
    else if (_links1.back() == _links1.front()) {
        _home->deactivate();
    }
    else {
        _home->activate();
    }

    if (_links1.size() < 2) {
        _back->deactivate();
    }
    else {
        _back->activate();
    }

    if (_links2.size() == 0) {
        _forward->deactivate();
    }
    else {
        _forward->activate();
    }

    if (_html->labelsize() <= 10) {
        _dec->deactivate();
    }
    else {
        _dec->activate();
    }

    if (_html->labelsize() >= 36) {
        _inc->deactivate();
    }
    else {
        _inc->activate();
    }

    if (_html->title() != nullptr) {
        copy_label(_html->title());
    }
    else {
        copy_label("");
    }
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

/** @brief Show a html text in a dialog in modal mode.
*
* @see flw::HTMLDialog.\n
*
* @param[in] html  HTML text.
* @param[in] W     Width in characters.
* @param[in] H     Height in characters.
*
* @snippet dialog.cpp flw::dlg::html example
*/
void flw::dlg::html(const std::string& html, int W, int H) {
    HTMLDialog dlg(W, H);

    dlg.add_page("home", html);
    dlg.run();
}

/** @brief Show a html file in a dialog in modal mode.
*
* @see flw::HTMLDialog.\n
*
* @param[in] file  HTML file.
* @param[in] W     Width in characters.
* @param[in] H     Height in characters.
*/
void flw::dlg::html_file(const std::string& file, int W, int H) {
    HTMLDialog dlg(W, H);

    dlg.load_page(file);
    dlg.run();
}

// MKALGAM_OFF
