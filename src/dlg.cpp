// Copyright gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#include "dlg.h"
#include "scrollbrowser.h"
#include "datechooser.h"

// MKALGAM_ON

#ifdef _WIN32
    #include <windows.h>
#else
#endif

#include <math.h>
#include <FL/Fl_File_Chooser.H>
#include <FL/Fl_Help_View.H>
#include <FL/Fl_Secret_Input.H>
#include <FL/Fl_Output.H>
#include <FL/Fl_Return_Button.H>
#include <FL/Fl_Text_Editor.H>
#include <FL/Fl_Tooltip.H>

namespace flw {
namespace theme {

// duplicated from flw.cpp so it can compile
void _load_default();
void _load_oxy();
void _load_oxy_blue();
void _load_oxy_tan();
void _load_gleam();
void _load_gleam_blue();
void _load_gleam_dark();
void _load_gleam_darker();
void _load_gleam_dark_blue();
void _load_gleam_tan();
void _load_gtk();
void _load_gtk_blue();
void _load_gtk_dark();
void _load_gtk_darker();
void _load_gtk_dark_blue();
void _load_gtk_tan();
void _load_plastic();
void _load_blue_plastic();
void _load_tan_plastic();
void _scrollbar();

} // theme

namespace dlg {

//------------------------------------------------------------------------------
static void* _dlg_zero_memory(void* mem, size_t size) {
    if (mem == nullptr || size == 0) return mem;
#ifdef _WIN32
    RtlSecureZeroMemory(mem, size);
#else
    auto p = static_cast<volatile unsigned char*>(mem);

    while (size--) {
        *p = 0;
        p++;
    }
#endif

    return mem;
}

/***
 *           _____  _       _    _ _             _
 *          |  __ \| |     | |  | | |           | |
 *          | |  | | | __ _| |__| | |_ _ __ ___ | |
 *          | |  | | |/ _` |  __  | __| '_ ` _ \| |
 *          | |__| | | (_| | |  | | |_| | | | | | |
 *          |_____/|_|\__, |_|  |_|\__|_| |_| |_|_|
 *      ______         __/ |
 *     |______|       |___/
 */

//------------------------------------------------------------------------------
class _DlgHtml  : public Fl_Double_Window {
    Fl_Help_View*               _html;
    Fl_Return_Button*           _close;
    GridGroup*                  _grid;
    
public:
    //--------------------------------------------------------------------------
    _DlgHtml(const char* title, const char* text, Fl_Window* parent, int W, int H) :
    Fl_Double_Window(0, 0, flw::PREF_FONTSIZE * W,flw::PREF_FONTSIZE * H) {
        end();

        _close = new Fl_Return_Button(0, 0, 0, 0, "&Close");
        _grid  = new GridGroup();
        _html  = new Fl_Help_View(0, 0, 0, 0);

        _grid->add(_html,    1,  1, -1, -6);
        _grid->add(_close, -17, -5, 16,  4);
        add(_grid);

        _close->callback(_DlgHtml::Callback, this);
        _close->labelfont(flw::PREF_FONT);
        _close->labelsize(flw::PREF_FONTSIZE);
        _html->textfont(flw::PREF_FONT);
        _html->textsize(flw::PREF_FONTSIZE);
        _html->value(text);
        
        callback(_DlgHtml::Callback, this);
        copy_label(title);
        size_range(flw::PREF_FONTSIZE * 24, flw::PREF_FONTSIZE * 12);
        set_modal();
        resizable(this);
        util::center_window(this, parent);
    }

    //--------------------------------------------------------------------------
    static void Callback(Fl_Widget* w, void* o) {
        auto self = static_cast<_DlgHtml*>(o);

        if (w == self || w == self->_close) {
            self->hide();
        }
    }

    //--------------------------------------------------------------------------
    void resize(int X, int Y, int W, int H) override {
        Fl_Double_Window::resize(X, Y, W, H);
        _grid->resize(0, 0, W, H);
    }

    //--------------------------------------------------------------------------
    void run() {
        show();

        while (visible() != 0) {
            Fl::wait();
            Fl::flush();
        }
    }
};

/***
 *           _____  _       _      _     _
 *          |  __ \| |     | |    (_)   | |
 *          | |  | | | __ _| |     _ ___| |_
 *          | |  | | |/ _` | |    | / __| __|
 *          | |__| | | (_| | |____| \__ \ |_
 *          |_____/|_|\__, |______|_|___/\__|
 *      ______         __/ |
 *     |______|       |___/
 */

//------------------------------------------------------------------------------
class _DlgList : public Fl_Double_Window {
    Fl_Return_Button*           _close;
    GridGroup*                  _grid;
    ScrollBrowser*              _list;

public:
    //--------------------------------------------------------------------------
    _DlgList(const char* title, const StringVector& list, std::string file, Fl_Window* parent = nullptr, bool fixed_font = false, int W = 50, int H = 20) :
    Fl_Double_Window(0, 0, (fixed_font ? flw::PREF_FIXED_FONTSIZE : flw::PREF_FONTSIZE) * W, (fixed_font ? flw::PREF_FIXED_FONTSIZE : flw::PREF_FONTSIZE) * H) {
        end();

        _close = new Fl_Return_Button(0, 0, 0, 0, "&Close");
        _grid  = new GridGroup();
        _list  = new ScrollBrowser();

        _grid->add(_list,    1,  1, -1, -6);
        _grid->add(_close, -17, -5, 16,  4);
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
        copy_label(title);
        size_range(flw::PREF_FONTSIZE * 24, flw::PREF_FONTSIZE * 12);
        set_modal();
        resizable(this);
        util::center_window(this, parent);

        if (list.size() > 0) {
            for (const auto& s : list) {
                _list->add(s.c_str());
            }
        }
        else if (file != "") {
            _list->load(file.c_str());
        }
    }

    //--------------------------------------------------------------------------
    static void Callback(Fl_Widget* w, void* o) {
        auto self = static_cast<_DlgList*>(o);

        if (w == self || w == self->_close) {
            self->hide();
        }
    }

    //--------------------------------------------------------------------------
    void resize(int X, int Y, int W, int H) override {
        Fl_Double_Window::resize(X, Y, W, H);
        _grid->resize(0, 0, W, H);
    }

    //--------------------------------------------------------------------------
    void run() {
        show();

        while (visible() != 0) {
            Fl::wait();
            Fl::flush();
        }
    }
};

/***
 *           _____  _        _____      _           _
 *          |  __ \| |      / ____|    | |         | |
 *          | |  | | | __ _| (___   ___| | ___  ___| |_
 *          | |  | | |/ _` |\___ \ / _ \ |/ _ \/ __| __|
 *          | |__| | | (_| |____) |  __/ |  __/ (__| |_
 *          |_____/|_|\__, |_____/ \___|_|\___|\___|\__|
 *      ______         __/ |
 *     |______|       |___/
 */

//------------------------------------------------------------------------------
class _DlgSelect : public Fl_Double_Window {
    Fl_Button*                  _cancel;
    Fl_Button*                  _close;
    Fl_Input*                   _filter;
    GridGroup*                  _grid;
    ScrollBrowser*              _list;
    const StringVector&         _strings;

public:
    //--------------------------------------------------------------------------
    _DlgSelect(const char* title, Fl_Window* parent, const StringVector& strings, int selected_string_index, std::string selected_string, bool fixed_font, int W, int H) :
    Fl_Double_Window(0, 0, ((fixed_font == true) ? flw::PREF_FIXED_FONTSIZE : flw::PREF_FONTSIZE) * W, ((fixed_font == true) ? flw::PREF_FIXED_FONTSIZE : flw::PREF_FONTSIZE) * H),
    _strings(strings) {
        end();

        _cancel = new Fl_Button(0, 0, 0, 0, "&Cancel");
        _close  = new Fl_Return_Button(0, 0, 0, 0, "&Select");
        _filter = new Fl_Input(0, 0, 0, 0);
        _grid   = new GridGroup();
        _list   = new ScrollBrowser(0, 0, 0, 0);

        _grid->add(_filter,   1,  1, -1,  4);
        _grid->add(_list,     1,  6, -1, -6);
        _grid->add(_cancel, -34, -5, 16,  4);
        _grid->add(_close,  -17, -5, 16,  4);
        add(_grid);

        _cancel->callback(_DlgSelect::Callback, this);
        _close->callback(_DlgSelect::Callback, this);
        _filter->callback(_DlgSelect::Callback, this);
        _filter->tooltip("Enter text to filter rows that macthes the text.\nPress tab to switch focus between input and list widget.");
        _filter->when(FL_WHEN_CHANGED);
        _list->callback(_DlgSelect::Callback, this);
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
        callback(_DlgSelect::Callback, this);
        copy_label(title);
        activate_button();
        size_range(flw::PREF_FONTSIZE * 24, flw::PREF_FONTSIZE * 12);
        set_modal();
        resizable(this);
        util::center_window(this, parent);
    }

    //--------------------------------------------------------------------------
    void activate_button() {
        if (_list->value() == 0) {
            _close->deactivate();
        }
        else {
            _close->activate();
        }
    }

    //--------------------------------------------------------------------------
    static void Callback(Fl_Widget* w, void* o) {
        auto self = static_cast<_DlgSelect*>(o);

        if (w == self || w == self->_cancel) {
            self->_list->deselect();
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
                self->hide();
            }
        }
        else if (w == self->_close) {
            self->hide();
        }
    }

    //--------------------------------------------------------------------------
    void filter(const char* filter) {
        _list->clear();

        for (const auto& string : _strings) {
            if (*filter == 0) {
                _list->add(string.c_str());
            }
            else if (strstr(string.c_str(), filter) != nullptr) {
                _list->add(string.c_str());
            }
        }

        _list->value(1);
    }

    //--------------------------------------------------------------------------
    // Tab key changes focus between list and input
    //
    int handle(int event) override {
        if (event == FL_KEYDOWN) {
            if (Fl::event_key() == FL_Enter) {
                if (_list->value() > 0) {
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

    //--------------------------------------------------------------------------
    void resize(int X, int Y, int W, int H) override {
        Fl_Double_Window::resize(X, Y, W, H);
        _grid->resize(0, 0, W, H);
    }

    //--------------------------------------------------------------------------
    int run() {
        show();

        while (visible() != 0) {
            Fl::wait();
            Fl::flush();
        }

        auto row = _list->value();

        if (row > 0) {
            auto selected = _list->text(row);

            for (int f = 0; f < (int) _strings.size(); f++) {
                const auto& string = _strings[f];

                if (string == selected) {
                    return f + 1;
                }
            }
        }

        return 0;
    }
};

/***
 *           _____  _       _____                                    _
 *          |  __ \| |     |  __ \                                  | |
 *          | |  | | | __ _| |__) |_ _ ___ _____      _____  _ __ __| |
 *          | |  | | |/ _` |  ___/ _` / __/ __\ \ /\ / / _ \| '__/ _` |
 *          | |__| | | (_| | |  | (_| \__ \__ \\ V  V / (_) | | | (_| |
 *          |_____/|_|\__, |_|   \__,_|___/___/ \_/\_/ \___/|_|  \__,_|
 *      ______         __/ |
 *     |______|       |___/
 */

const char* PASSWORD_CANCEL = "&Cancel";
const char* PASSWORD_OK     = "&Ok";

//------------------------------------------------------------------------------
class _DlgPassword : public Fl_Double_Window {
public:
    enum class TYPE {
                                ASK_PASSWORD,
                                ASK_PASSWORD_AND_KEYFILE,
                                CONFIRM_PASSWORD,
                                CONFIRM_PASSWORD_AND_KEYFILE,
    };

private:
    Fl_Button*                  _browse;
    Fl_Button*                  _cancel;
    Fl_Button*                  _close;
    Fl_Input*                   _file;
    Fl_Input*                   _password1;
    Fl_Input*                   _password2;
    GridGroup*                  _grid;
    _DlgPassword::TYPE          _mode;
    bool                        _ret;

public:
    //--------------------------------------------------------------------------
    _DlgPassword(const char* title, Fl_Window* parent, _DlgPassword::TYPE mode) :
    Fl_Double_Window(0, 0, 0, 0) {
        end();

        _browse    = new Fl_Button(0, 0, 0, 0, "&Browse");
        _cancel    = new Fl_Button(0, 0, 0, 0, PASSWORD_CANCEL);
        _close     = new Fl_Return_Button(0, 0, 0, 0, PASSWORD_OK);
        _file      = new Fl_Output(0, 0, 0, 0, "Key file");
        _grid      = new GridGroup();
        _password1 = new Fl_Secret_Input(0, 0, 0, 0, "Password");
        _password2 = new Fl_Secret_Input(0, 0, 0, 0, "Enter password again");
        _mode      = mode;
        _ret       = false;

        _grid->add(_password1,  1,   3,  -1,  4);
        _grid->add(_password2,  1,  10,  -1,  4);
        _grid->add(_file,       1,  17,  -1,  4);
        _grid->add(_browse,   -51,  -5,  16,  4);
        _grid->add(_cancel,   -34,  -5,  16,  4);
        _grid->add(_close,    -17,  -5,  16,  4);
        add(_grid);

        _browse->callback(_DlgPassword::Callback, this);
        _cancel->callback(_DlgPassword::Callback, this);
        _close->callback(_DlgPassword::Callback, this);
        _close->deactivate();
        _file->align(FL_ALIGN_TOP | FL_ALIGN_LEFT);
        _file->textfont(flw::PREF_FIXED_FONT);
        _file->textsize(flw::PREF_FONTSIZE);
        _file->tooltip("Select optional key file");
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

        auto W = flw::PREF_FONTSIZE * 35;
        auto H = flw::PREF_FONTSIZE * 13.5;

        if (_mode == _DlgPassword::TYPE::ASK_PASSWORD) {
            _password2->hide();
            _browse->hide();
            _file->hide();
            H = flw::PREF_FONTSIZE * 6.5;
        }
        else if (_mode == _DlgPassword::TYPE::CONFIRM_PASSWORD) {
            _browse->hide();
            _file->hide();
            H = flw::PREF_FONTSIZE * 10;
        }
        else if (_mode == _DlgPassword::TYPE::ASK_PASSWORD_AND_KEYFILE) {
            _password2->hide();
            _grid->resize(_file, 1, 10, -1, 4);
            H = flw::PREF_FONTSIZE * 10;
        }

        resizable(this);
        util::labelfont(this);
        callback(_DlgPassword::Callback, this);
        label(title);
        size(W, H);
        size_range(W, H);
        set_modal();
        util::center_window(this, parent);
    }

    //--------------------------------------------------------------------------
    static void Callback(Fl_Widget* w, void* o) {
        auto self = static_cast<_DlgPassword*>(o);

        if (w == self) {
            ;
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
        }
        else if (w == self->_cancel) {
            self->_ret = false;
            self->hide();
        }
        else if (w == self->_close) {
            self->_ret = true;
            self->hide();
        }
    }

    //--------------------------------------------------------------------------
    void check() {
        auto p1 = _password1->value();
        auto p2 = _password2->value();

        if (_mode == _DlgPassword::TYPE::ASK_PASSWORD ||
            _mode == _DlgPassword::TYPE::ASK_PASSWORD_AND_KEYFILE) {

            if (strlen(p1)) {
                _close->activate();
            }
            else {
                _close->deactivate();
            }
        }
        else if (_mode == _DlgPassword::TYPE::CONFIRM_PASSWORD ||
                 _mode == _DlgPassword::TYPE::CONFIRM_PASSWORD_AND_KEYFILE) {

            if (strlen(p1) && strcmp(p1, p2) == 0) {
                _close->activate();
            }
            else {
                _close->deactivate();
            }
        }
    }

    //--------------------------------------------------------------------------
    void resize(int X, int Y, int W, int H) override {
        Fl_Double_Window::resize(X, Y, W, H);
        _grid->resize(0, 0, W, H);
    }
    
    //--------------------------------------------------------------------------
    bool run(std::string& password, std::string& file) {
        show();

        while (visible() != 0) {
            Fl::wait();
            Fl::flush();
        }

        if (_ret) {
            file = _file->value();
            password = _password1->value();
        }

        _dlg_zero_memory(const_cast<char*>(_password1->value()), strlen(_password1->value()));
        _dlg_zero_memory(const_cast<char*>(_password2->value()), strlen(_password2->value()));
        _dlg_zero_memory(const_cast<char*>(_file->value()), strlen(_file->value()));

        return _ret;
    }
};

/***
 *           _____  _    _______        _
 *          |  __ \| |  |__   __|      | |
 *          | |  | | | __ _| | _____  _| |_
 *          | |  | | |/ _` | |/ _ \ \/ / __|
 *          | |__| | | (_| | |  __/>  <| |_
 *          |_____/|_|\__, |_|\___/_/\_\\__|
 *      ______         __/ |
 *     |______|       |___/
 */

//------------------------------------------------------------------------------
class _DlgText : public Fl_Double_Window {
    Fl_Button*                  _cancel;
    Fl_Button*                  _close;
    Fl_Button*                  _save;
    Fl_Text_Buffer*             _buffer;
    Fl_Text_Display*            _text;
    GridGroup*                  _grid;
    bool                        _edit;
    char*                       _res;

public:
    //--------------------------------------------------------------------------
    _DlgText(const char* title, const char* text, bool edit, Fl_Window* parent, int W, int H) :
    Fl_Double_Window(0, 0, flw::PREF_FONTSIZE * W, flw::PREF_FONTSIZE * H) {
        end();

        _buffer = new Fl_Text_Buffer();
        _cancel = new Fl_Button(0, 0, 0, 0, "C&ancel");
        _close  = (edit == false) ? new Fl_Return_Button(0, 0, 0, 0, "&Close") : new Fl_Button(0, 0, 0, 0, "&Update");
        _grid   = new GridGroup();
        _save   = new Fl_Button(0, 0, 0, 0, "&Save");
        _text   = (edit == false) ? new Fl_Text_Display(0, 0, 0, 0) : new Fl_Text_Editor(0, 0, 0, 0);
        _edit   = edit;
        _res    = nullptr;

        _grid->add(_text,     1,   1,  -1,  -6);
        _grid->add(_cancel, -51,  -5,  16,   4);
        _grid->add(_save,   -34,  -5,  16,   4);
        _grid->add(_close,  -17,  -5,  16,   4);
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

        if (edit == false) {
            _cancel->hide();
        }

        callback(_DlgText::Callback, this);
        copy_label(title);
        size_range(flw::PREF_FONTSIZE * 24, flw::PREF_FONTSIZE * 12);
        set_modal();
        resizable(this);
        util::center_window(this, parent);
    }

    //--------------------------------------------------------------------------
    ~_DlgText() {
       _text->buffer(nullptr);
        delete _buffer;
    }

    //--------------------------------------------------------------------------
    static void Callback(Fl_Widget* w, void* o) {
        auto self = static_cast<_DlgText*>(o);

        if (w == self || w == self->_cancel) {
            self->hide();
        }
        else if (w == self->_save) {
            auto filename = fl_file_chooser("Select Destination File", nullptr, nullptr, 0);

            if (filename != nullptr && self->_buffer->savefile(filename) != 0) {
                fl_alert("error: failed to save text to %s", filename);
            }
        }
        else if (w == self->_close) {
            if (self->_edit == true) {
                self->_res = self->_buffer->text();
            }

            self->hide();
        }
    }

    //--------------------------------------------------------------------------
    void resize(int X, int Y, int W, int H) override {
        Fl_Double_Window::resize(X, Y, W, H);
        _grid->resize(0, 0, W, H);
    }

    //--------------------------------------------------------------------------
    char* run() {
        show();

        while (visible() != 0) {
            Fl::wait();
            Fl::flush();
        }

        return _res;
    }
};

/***
 *           _____  _    _______ _
 *          |  __ \| |  |__   __| |
 *          | |  | | | __ _| |  | |__   ___ _ __ ___   ___
 *          | |  | | |/ _` | |  | '_ \ / _ \ '_ ` _ \ / _ \
 *          | |__| | | (_| | |  | | | |  __/ | | | | |  __/
 *          |_____/|_|\__, |_|  |_| |_|\___|_| |_| |_|\___|
 *      ______         __/ |
 *     |______|       |___/
 */

//------------------------------------------------------------------------------
class _DlgTheme : public Fl_Double_Window {
    Fl_Box*                     _fixed_label;
    Fl_Box*                     _font_label;
    Fl_Browser*                 _theme;
    Fl_Button*                  _close;
    Fl_Button*                  _fixedfont;
    Fl_Button*                  _font;
    GridGroup*                  _grid;
    int                         _theme_row;

public:
    //--------------------------------------------------------------------------
    _DlgTheme(bool enable_font, bool enable_fixedfont, Fl_Window* parent) :
    Fl_Double_Window(0, 0, 0, 0, "Set Theme") {
        end();

        _close       = new Fl_Return_Button(0, 0, 0, 0, "&Close");
        _fixedfont   = new Fl_Button(0, 0, 0, 0, "F&ixed Font");
        _fixed_label = new Fl_Box(0, 0, 0, 0);
        _font        = new Fl_Button(0, 0, 0, 0, "&Font");
        _font_label  = new Fl_Box(0, 0, 0, 0);
        _grid        = new GridGroup();
        _theme       = new Fl_Hold_Browser(0, 0, 0, 0);
        _theme_row   = 0;

        _grid->add(_theme,         1,   1,  -1, -16);
        _grid->add(_font_label,    1, -15,  -1,   4);
        _grid->add(_fixed_label,   1, -10,  -1,   4);
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

        _close->callback(Callback, this);
        _fixed_label->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
        _fixed_label->box(FL_BORDER_BOX);
        _fixed_label->color(FL_BACKGROUND2_COLOR);
        _fixedfont->callback(Callback, this);
        _font->callback(Callback, this);
        _font_label->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
        _font_label->box(FL_BORDER_BOX);
        _font_label->color(FL_BACKGROUND2_COLOR);
        _theme->box(FL_BORDER_BOX);
        _theme->callback(Callback, this);
        _theme->textfont(flw::PREF_FONT);

        for (size_t f = 0; f < 100; f++) {
            auto t = flw::PREF_THEMES[f];

            if (t != nullptr) {
                _theme->add(t);
            }
            else {
                break;
            }
        }

        resizable(this);
        callback(Callback, this);
        set_modal();
        update_pref();
        util::center_window(this, parent);
    }

    //--------------------------------------------------------------------------
    static void Callback(Fl_Widget* w, void* o) {
        auto self = static_cast<_DlgTheme*>(o);

        if (w == self) {
            self->hide();
        }
        else if (w == self->_fixedfont) {
            FontDialog fd(flw::PREF_FIXED_FONT, flw::PREF_FIXED_FONTSIZE, "Select Monospaced Font");

            if (fd.run(Fl::first_window()) == true) {
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
            FontDialog fd(flw::PREF_FONT, flw::PREF_FONTSIZE, "Select Font");

            if (fd.run(Fl::first_window()) == true) {
                flw::PREF_FONT     = fd.font();
                flw::PREF_FONTSIZE = fd.fontsize();
                flw::PREF_FONTNAME = fd.fontname();

                if (self->_fixedfont->active() == 0) {
                    flw::PREF_FIXED_FONTSIZE = flw::PREF_FONTSIZE;
                }

                self->update_pref();
            }
        }
        else if (w == self->_theme) {
            auto row = self->_theme->value() - 1;

            if (row == theme::THEME_OXY) {
                theme::_load_oxy();
            }
            else if (row == theme::THEME_OXY_BLUE) {
                theme::_load_oxy_blue();
            }
            else if (row == theme::THEME_OXY_TAN) {
                theme::_load_oxy_tan();
            }
            else if (row == theme::THEME_GLEAM) {
                theme::_load_gleam();
            }
            else if (row == theme::THEME_GLEAM_BLUE) {
                theme::_load_gleam_blue();
            }
            else if (row == theme::THEME_GLEAM_DARK_BLUE) {
                theme::_load_gleam_dark_blue();
            }
            else if (row == theme::THEME_GLEAM_DARK) {
                theme::_load_gleam_dark();
            }
            else if (row == theme::THEME_GLEAM_DARKER) {
                theme::_load_gleam_darker();
            }
            else if (row == theme::THEME_GLEAM_TAN) {
                theme::_load_gleam_tan();
            }
            else if (row == theme::THEME_GTK) {
                theme::_load_gtk();
            }
            else if (row == theme::THEME_GTK_BLUE) {
                theme::_load_gtk_blue();
            }
            else if (row == theme::THEME_GTK_DARK_BLUE) {
                theme::_load_gtk_dark_blue();
            }
            else if (row == theme::THEME_GTK_DARK) {
                theme::_load_gtk_dark();
            }
            else if (row == theme::THEME_GTK_DARKER) {
                theme::_load_gtk_darker();
            }
            else if (row == theme::THEME_GTK_TAN) {
                theme::_load_gtk_tan();
            }
            else if (row == theme::THEME_PLASTIC) {
                theme::_load_plastic();
            }
            else if (row == theme::THEME_PLASTIC_BLUE) {
                theme::_load_blue_plastic();
            }
            else if (row == theme::THEME_PLASTIC_TAN) {
                theme::_load_tan_plastic();
            }
            else {
                theme::_load_default();
            }

            self->update_pref();
        }
        else if (w == self->_close) {
            self->hide();
        }
    }

    //--------------------------------------------------------------------------
    void resize(int X, int Y, int W, int H) override {
        Fl_Double_Window::resize(X, Y, W, H);
        _grid->resize(0, 0, W, H);
    }

    //--------------------------------------------------------------------------
    void run() {
        show();

        while (visible()) {
            Fl::wait();
            Fl::flush();
        }
    }

    //--------------------------------------------------------------------------
    void update_pref() {
        Fl_Tooltip::font(flw::PREF_FONT);
        Fl_Tooltip::size(flw::PREF_FONTSIZE);
        util::labelfont(this);
        _font_label->copy_label(util::format("%s - %d", flw::PREF_FONTNAME.c_str(), flw::PREF_FONTSIZE).c_str());
        _fixed_label->copy_label(util::format("%s - %d", flw::PREF_FIXED_FONTNAME.c_str(), flw::PREF_FIXED_FONTSIZE).c_str());
        _fixed_label->labelfont(flw::PREF_FIXED_FONT);
        _fixed_label->labelsize(flw::PREF_FIXED_FONTSIZE);
        _theme->textsize(flw::PREF_FONTSIZE);
        size(flw::PREF_FONTSIZE * 30, flw::PREF_FONTSIZE * 34);
        size_range(flw::PREF_FONTSIZE * 20, flw::PREF_FONTSIZE * 24);
        theme::_scrollbar();

        for (int f = 0; f <= theme::THEME_SYSTEM; f++) {
            if (flw::PREF_THEME == flw::PREF_THEMES[f]) {
                _theme->value(f + 1);
                break;
            }

        }

        Fl::redraw();
    }
};

/***
 *       __                  _   _
 *      / _|                | | (_)
 *     | |_ _   _ _ __   ___| |_ _  ___  _ __  ___
 *     |  _| | | | '_ \ / __| __| |/ _ \| '_ \/ __|
 *     | | | |_| | | | | (__| |_| | (_) | | | \__ \
 *     |_|  \__,_|_| |_|\___|\__|_|\___/|_| |_|___/
 *
 *
 */

//------------------------------------------------------------------------------
void html(std::string title, const std::string& text, Fl_Window* parent, int W, int H) {
    _DlgHtml dlg(title.c_str(), text.c_str(), parent, W, H);
    dlg.run();
}

//------------------------------------------------------------------------------
void list(std::string title, const StringVector& list, Fl_Window* parent, bool fixed_font, int W, int H) {
    _DlgList dlg(title.c_str(), list, "", parent, fixed_font, W, H);
    dlg.run();
}

//------------------------------------------------------------------------------
void list(std::string title, const std::string& list, Fl_Window* parent, bool fixed_font, int W, int H) {
    auto list2 = util::split( list, "\n");
    _DlgList dlg(title.c_str(), list2, "", parent, fixed_font, W, H);
    dlg.run();
}

//------------------------------------------------------------------------------
void list_file(std::string title, std::string file, Fl_Window* parent, bool fixed_font, int W, int H) {
    _DlgList dlg(title.c_str(), flw::StringVector(), file, parent, fixed_font, W, H);
    dlg.run();
}

//------------------------------------------------------------------------------
void panic(std::string message) {
    fl_alert("panic! I have to quit\n%s", message.c_str());
    exit(1);
}

//------------------------------------------------------------------------------
bool password1(std::string title, std::string& password, Fl_Window* parent) {
    std::string file;
    _DlgPassword dlg(title.c_str(), parent, _DlgPassword::TYPE::ASK_PASSWORD);
    return dlg.run(password, file);
}

//------------------------------------------------------------------------------
bool password2(std::string title, std::string& password, Fl_Window* parent) {
    std::string file;
    _DlgPassword dlg(title.c_str(), parent, _DlgPassword::TYPE::CONFIRM_PASSWORD);
    return dlg.run(password, file);
}

//------------------------------------------------------------------------------
bool password3(std::string title, std::string& password, std::string& file, Fl_Window* parent) {
    _DlgPassword dlg(title.c_str(), parent, _DlgPassword::TYPE::ASK_PASSWORD_AND_KEYFILE);
    return dlg.run(password, file);
}

//------------------------------------------------------------------------------
bool password4(std::string title, std::string& password, std::string& file, Fl_Window* parent) {
    _DlgPassword dlg(title.c_str(), parent, _DlgPassword::TYPE::CONFIRM_PASSWORD_AND_KEYFILE);
    return dlg.run(password, file);
}

//------------------------------------------------------------------------------
int select(std::string title, const StringVector& list, int selected_row, Fl_Window* parent, bool fixed_font, int W, int H) {
    _DlgSelect dlg(title.c_str(), parent, list, selected_row, "", fixed_font, W, H);
    return dlg.run();
}

//------------------------------------------------------------------------------
int select(std::string title, const StringVector& list, const std::string& selected_row, Fl_Window* parent, bool fixed_font, int W, int H) {
    _DlgSelect dlg(title.c_str(), parent, list, 0, selected_row, fixed_font, W, H);
    return dlg.run();
}

//------------------------------------------------------------------------------
void text(std::string title, const std::string& text, Fl_Window* parent, int W, int H) {
    _DlgText dlg(title.c_str(), text.c_str(), false, parent, W, H);
    dlg.run();
}

//------------------------------------------------------------------------------
bool text_edit(std::string title, std::string& text, Fl_Window* parent, int W, int H) {
    auto dlg = _DlgText(title.c_str(), text.c_str(), true, parent, W, H);
    auto res = dlg.run();

    if (res == nullptr) {
        return false;
    }

    text = res;
    free(res);
    return true;
}

//------------------------------------------------------------------------------
void theme(bool enable_font, bool enable_fixedfont, Fl_Window* parent) {
    auto dlg = _DlgTheme(enable_font, enable_fixedfont, parent);
    dlg.run();
}

/***
 *               _                _   _____  _       _
 *         /\   | |              | | |  __ \(_)     | |
 *        /  \  | |__   ___  _ __| |_| |  | |_  __ _| | ___   __ _
 *       / /\ \ | '_ \ / _ \| '__| __| |  | | |/ _` | |/ _ \ / _` |
 *      / ____ \| |_) | (_) | |  | |_| |__| | | (_| | | (_) | (_| |
 *     /_/    \_\_.__/ \___/|_|   \__|_____/|_|\__,_|_|\___/ \__, |
 *                                                            __/ |
 *                                                           |___/
 */

//------------------------------------------------------------------------------
// To disable progress set min and max to 0.0
//
AbortDialog::AbortDialog(double min, double max) :
Fl_Double_Window(0, 0, 0, 0, "Working...") {
    _button   = new Fl_Button(0, 0, 0, 0, "Press To Abort");
    _grid     = new GridGroup();
    _progress = new Fl_Hor_Fill_Slider(0, 0, 0, 0);
    _abort    = false;
    _last     = 0;

    _grid->add(_button,     1,   1,   -1,  -6);
    _grid->add(_progress,   1,  -5,   -1,   4);
    add(_grid);

    auto W = flw::PREF_FONTSIZE * 32;
    auto H = flw::PREF_FONTSIZE * 12;

    if (min < max && fabs(max - min) > 0.001) {
        _progress->range(min, max);
        _progress->value(min);
    }
    else {
        _progress->hide();
        _grid->resize(_button, 1, 1, -1, -1);
    }

    _button->callback(AbortDialog::Callback, this);
    _button->labelfont(flw::PREF_FONT);
    _button->labelsize(flw::PREF_FONTSIZE);
    _progress->color(FL_SELECTION_COLOR);

    resizable(this);
    size(W, H);
    size_range(W, H);
    callback(AbortDialog::Callback, this);
    set_modal();
}

//------------------------------------------------------------------------------
void AbortDialog::Callback(Fl_Widget* w, void* o) {
    auto self = static_cast<AbortDialog*>(o);

    if (w == self->_button) {
        self->_abort = true;
    }
}

//------------------------------------------------------------------------------
bool AbortDialog::check(int milliseconds) {
    auto now = util::milliseconds();

    if (now - _last > milliseconds) {
        _last = now;
        Fl::check();
    }

    return _abort;
}

//------------------------------------------------------------------------------
bool AbortDialog::check(double value, double min, double max, int milliseconds) {
    auto now = util::milliseconds();

    if (now - _last > milliseconds) {
        _progress->value(value);
        _progress->range(min, max);
        _last = now;
        Fl::check();
    }

    return _abort;
}

//------------------------------------------------------------------------------
void AbortDialog::range(double min, double max) {
    _progress->range(min, max);
}

//------------------------------------------------------------------------------
void AbortDialog::show(const std::string& label, Fl_Window* parent) {
    _abort = false;
    _last  = 0;

    _button->copy_label(label.c_str());
    util::center_window(this, parent);
    Fl_Double_Window::show();
    Fl::flush();
}

//------------------------------------------------------------------------------
void AbortDialog::value(double value) {
    _progress->value(value);
}

/***
 *      ______          _   _____  _       _
 *     |  ____|        | | |  __ \(_)     | |
 *     | |__ ___  _ __ | |_| |  | |_  __ _| | ___   __ _
 *     |  __/ _ \| '_ \| __| |  | | |/ _` | |/ _ \ / _` |
 *     | | | (_) | | | | |_| |__| | | (_| | | (_) | (_| |
 *     |_|  \___/|_| |_|\__|_____/|_|\__,_|_|\___/ \__, |
 *                                                  __/ |
 *                                                 |___/
 */

static const std::string _FONTDIALOG_LABEL = R"(
ABCDEFGHIJKLMNOPQRSTUVWXYZ /0123456789
abcdefghijklmnopqrstuvwxyz £©µÀÆÖÞßéöÿ
–—‘“”„†•…‰™œŠŸž€ ΑΒΓΔΩαβγδω АБВГДабвгд
∀∂∈ℝ∧∪≡∞ ↑↗↨↻⇣ ┐┼╔╘░►☺♀ ﬁ�⑀₂ἠḂӥẄɐː⍎אԱა

japanese: こんにちは世界
korean: 안녕하세요 세계
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

//------------------------------------------------------------------------------
class _FontDialogLabel : public Fl_Box {
public:
    int font;
    int size;

    //--------------------------------------------------------------------------
    _FontDialogLabel(int x, int y, int w, int h) : Fl_Box(x, y, w, h, _FONTDIALOG_LABEL.c_str()) {
        font = FL_HELVETICA;
        size = 14;

        align(FL_ALIGN_TOP | FL_ALIGN_LEFT | FL_ALIGN_INSIDE | FL_ALIGN_CLIP);
        box(FL_BORDER_BOX);
        color(FL_BACKGROUND2_COLOR);
    }

    //--------------------------------------------------------------------------
    void draw() override {
        draw_box();
        fl_font((Fl_Font) font, size);
        fl_color(FL_FOREGROUND_COLOR);
        fl_draw(label(), x() + 3, y() + 3, w() - 6, h() - 6, align());
    }
};

//------------------------------------------------------------------------------
FontDialog::FontDialog(Fl_Font font, Fl_Fontsize fontsize, const std::string& label) :
Fl_Double_Window(0, 0, flw::PREF_FONTSIZE * 64, flw::PREF_FONTSIZE * 36) {
    _create(font, "", fontsize, label);
}

//------------------------------------------------------------------------------
FontDialog::FontDialog(std::string font, Fl_Fontsize fontsize, std::string label) :
Fl_Double_Window(0, 0, flw::PREF_FONTSIZE * 64, flw::PREF_FONTSIZE * 36) {
    _create(0, font, fontsize, label);
}

//------------------------------------------------------------------------------
void FontDialog::_activate() {
    if (_fonts->value() == 0 || _sizes->value() == 0 || (_fonts->active() == 0 && _sizes->active() == 0)) {
        _select->deactivate();
    }
    else {
        _select->activate();
    }
}

//------------------------------------------------------------------------------
void FontDialog::Callback(Fl_Widget* w, void* o) {
    auto self = static_cast<FontDialog*>(o);

    if (w == self) {
        self->hide();
    }
    else if (w == self->_cancel) {
        self->hide();
    }
    else if (w == self->_fonts) {
        auto row = self->_fonts->value();

        if (row > 0) {
            static_cast<_FontDialogLabel*>(self->_label)->font = row - 1;
        }

        self->_activate();
        Fl::redraw();
    }
    else if (w == self->_select) {
        auto row1 = self->_fonts->value();
        auto row2 = self->_sizes->value();

        if (row1 > 0 && row2 > 0) {
            row1--;

            self->_fontname = util::remove_browser_format(flw::PREF_FONTNAMES[row1]);
            self->_font     = row1;
            self->_fontsize = row2 + 5;
            self->_ret      = true;

            self->hide();
        }
    }
    else if (w == self->_sizes) {
        auto row = self->_sizes->value();

        if (row > 0) {
            static_cast<_FontDialogLabel*>(self->_label)->size = row + 5;
        }

        self->_activate();
        Fl::redraw();
    }
}

//------------------------------------------------------------------------------
void FontDialog::_create(Fl_Font font, std::string fontname, Fl_Fontsize fontsize, std::string label) {
    end();

    _cancel   = new Fl_Button(0, 0, 0, 0, "&Cancel");
    _fonts    = new ScrollBrowser(12);
    _grid     = new GridGroup();
    _label    = new _FontDialogLabel(0, 0, 0, 0);
    _select   = new Fl_Button(0, 0, 0, 0, "&Select");
    _sizes    = new ScrollBrowser(6);
    _font     = -1;
    _fontsize = -1;
    _ret      = false;

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
    static_cast<_FontDialogLabel*>(_label)->font = font;
    static_cast<_FontDialogLabel*>(_label)->size = fontsize;
    _select->callback(FontDialog::Callback, this);
    _select->labelfont(flw::PREF_FONT);
    _select->labelsize(flw::PREF_FONTSIZE);
    _sizes->box(FL_BORDER_BOX);
    _sizes->callback(FontDialog::Callback, this);
    _sizes->textsize(flw::PREF_FONTSIZE);
    _sizes->when(FL_WHEN_CHANGED);

    theme::load_fonts();

    for (auto name : flw::PREF_FONTNAMES) {
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
        static_cast<_FontDialogLabel*>(_label)->font = fontsize;
    }
    else {
        _sizes->value(14 - 5);
        _sizes->middleline(14 - 5);
        static_cast<_FontDialogLabel*>(_label)->font = 14;
    }

    if (fontname != "") {
        _select_name(fontname);
    }
    else if (font >= 0 && font < _fonts->size()) {
        _fonts->value(font + 1);
        _fonts->middleline(font + 1);
        static_cast<_FontDialogLabel*>(_label)->font = font;
    }
    else {
        _fonts->value(1);
        _fonts->middleline(1);
    }

    resizable(this);
    copy_label(label.c_str());
    callback(FontDialog::Callback, this);
    size_range(flw::PREF_FONTSIZE * 38, flw::PREF_FONTSIZE * 12);
    set_modal();
    _fonts->take_focus();
    _grid->do_layout();
}

//------------------------------------------------------------------------------
bool FontDialog::run(Fl_Window* parent) {
    _ret = false;

    _activate();
    util::center_window(this, parent);
    show();

    while (visible() != 0) {
        Fl::wait();
        Fl::flush();
    }

    return _ret;
}

//------------------------------------------------------------------------------
void FontDialog::_select_name(std::string fontname) {
    auto count = 1;

    for (auto f : flw::PREF_FONTNAMES) {
        auto font_without_style = util::remove_browser_format(f);

        if (fontname == font_without_style) {
            _fonts->value(count);
            _fonts->middleline(count);
            static_cast<_FontDialogLabel*>(_label)->font = count - 1;
            return;
        }

        count++;
    }

    _fonts->value(1);
    static_cast<_FontDialogLabel*>(_label)->font = 0;
}

/***
 *     __          __        _    _____  _       _
 *     \ \        / /       | |  |  __ \(_)     | |
 *      \ \  /\  / /__  _ __| | _| |  | |_  __ _| | ___   __ _
 *       \ \/  \/ / _ \| '__| |/ / |  | | |/ _` | |/ _ \ / _` |
 *        \  /\  / (_) | |  |   <| |__| | | (_| | | (_) | (_| |
 *         \/  \/ \___/|_|  |_|\_\_____/|_|\__,_|_|\___/ \__, |
 *                                                        __/ |
 *                                                       |___/
 */

//------------------------------------------------------------------------------
WorkDialog::WorkDialog(const char* title, Fl_Window* parent, bool cancel, bool pause, int W, int H) :
Fl_Double_Window(0, 0, W * flw::PREF_FONTSIZE, H * flw::PREF_FONTSIZE) {
    end();

    _cancel = new Fl_Button(0, 0, 0, 0, "Cancel");
    _grid   = new GridGroup();
    _label  = new Fl_Hold_Browser(0, 0, 0, 0);
    _pause  = new Fl_Toggle_Button(0, 0, 0, 0, "Pause");
    _ret    = true;
    _last   = 0.0;

    _grid->add(_label,    1,   1,  -1,  -6);
    _grid->add(_pause,  -34,  -5,  16,   4);
    _grid->add(_cancel, -17,  -5,  16,   4);
    add(_grid);

    _cancel->callback(WorkDialog::Callback, this);
    _label->box(FL_BORDER_BOX);
    _label->textfont(flw::PREF_FONT);
    _label->textsize(flw::PREF_FONTSIZE);
    _pause->callback(WorkDialog::Callback, this);

    if (cancel == false) {
        _cancel->deactivate();
    }

    if (pause == false) {
        _pause->deactivate();
    }

    util::labelfont(this);
    callback(WorkDialog::Callback, this);
    copy_label(title);
    size_range(flw::PREF_FONTSIZE * 24, flw::PREF_FONTSIZE * 12);
    set_modal();
    resizable(this);
    util::center_window(this, parent);
    show();
}

//------------------------------------------------------------------------------
void WorkDialog::Callback(Fl_Widget* w, void* o) {
    auto self = static_cast<WorkDialog*>(o);

    if (w == self) {
    }
    else if (w == self->_cancel) {
        self->_ret = false;
    }
    else if (w == self->_pause) {
        bool cancel = self->_cancel->active();
        self->_cancel->deactivate();
        self->_pause->label("C&ontinue");

        while (self->_pause->value() != 0) {
            util::sleep(10);
            Fl::check();
        }

        self->_pause->label("&Pause");

        if (cancel) {
            self->_cancel->activate();
        }
    }
}

//------------------------------------------------------------------------------
bool WorkDialog::run(double update_time, const StringVector& messages) {
    auto now = util::clock();

    if ((now - _last) > update_time) {
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

//------------------------------------------------------------------------------
bool WorkDialog::run(double update_time, const std::string& message) {
    auto now = util::clock();

    if ((now - _last) > update_time) {
        _label->clear();
        _label->add(message.c_str());
        _last = now;
        Fl::check();
        Fl::flush();
    }

    return _ret;
}

} // dlg
} // flw

// MKALGAM_OFF
