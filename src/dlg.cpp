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
#include <FL/Fl_Hor_Slider.H>
#include <FL/Fl_Output.H>
#include <FL/Fl_Return_Button.H>
#include <FL/Fl_Secret_Input.H>
#include <FL/Fl_Text_Editor.H>
#include <FL/Fl_Tooltip.H>

namespace flw {
namespace theme {

// duplicated from flw.cpp so it can compile
void _load_default();
void _load_gleam();
void _load_gleam_blue();
void _load_gleam_dark();
void _load_gleam_tan();
void _load_gtk();
void _load_gtk_blue();
void _load_gtk_dark();
void _load_gtk_tan();
void _load_oxy();
void _load_oxy_blue();
void _load_oxy_tan();
void _load_plastic();
void _load_plastic_tan();
void _scrollbar();

} // theme

namespace dlg {

//------------------------------------------------------------------------------
static void _init_printer_formats(Fl_Choice* format, Fl_Choice* layout) {
    format->add("A0 format");
    format->add("A1 format");
    format->add("A2 format");
    format->add("A3 format");
    format->add("A4 format");
    format->add("A5 format");
    format->add("A6 format");
    format->add("A7 format");
    format->add("A8 format");
    format->add("A9 format");
    format->add("B0 format");
    format->add("B1 format");
    format->add("B2 format");
    format->add("B3 format");
    format->add("B4 format");
    format->add("B5 format");
    format->add("B6 format");
    format->add("B7 format");
    format->add("B8 format");
    format->add("B9 format");
    format->add("Executive format");
    format->add("Folio format");
    format->add("Ledger format");
    format->add("Legal format");
    format->add("Letter format");
    format->add("Tabloid format");
    format->tooltip("Select paper format.");
    format->value(4);

    layout->add("Portrait");
    layout->add("Landscape");
    layout->tooltip("Select paper layout.");
    layout->value(0);
}

//------------------------------------------------------------------------------
void center_message_dialog() {
    int X, Y, W, H;
    Fl::screen_xywh(X, Y, W, H);
    fl_message_position(W / 2, H / 2, 1);
}

//------------------------------------------------------------------------------
void panic(std::string message) {
    fl_alert("panic! I have to quit\n%s", message.c_str());
    exit(1);
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
        _grid  = new GridGroup(0, 0, w(), h());
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
        resizable(_grid);
        util::center_window(this, parent);
        _grid->do_layout();
    }

    //--------------------------------------------------------------------------
    static void Callback(Fl_Widget* w, void* o) {
        auto self = static_cast<_DlgHtml*>(o);

        if (w == self || w == self->_close) {
            self->hide();
        }
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

//------------------------------------------------------------------------------
void html(std::string title, const std::string& text, Fl_Window* parent, int W, int H) {
    _DlgHtml dlg(title.c_str(), text.c_str(), parent, W, H);
    dlg.run();
}

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
        _grid  = new GridGroup(0, 0, w(), h());
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
        resizable(_grid);
        util::center_window(this, parent);
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

    //--------------------------------------------------------------------------
    static void Callback(Fl_Widget* w, void* o) {
        auto self = static_cast<_DlgList*>(o);

        if (w == self || w == self->_close) {
            self->hide();
        }
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

//------------------------------------------------------------------------------
void list(std::string title, const StringVector& list, Fl_Window* parent, bool fixed_font, int W, int H) {
    _DlgList dlg(title.c_str(), list, "", parent, fixed_font, W, H);
    dlg.run();
}

//------------------------------------------------------------------------------
void list(std::string title, const std::string& list, Fl_Window* parent, bool fixed_font, int W, int H) {
    auto list2 = util::split_string( list, "\n");
    _DlgList dlg(title.c_str(), list2, "", parent, fixed_font, W, H);
    dlg.run();
}

//------------------------------------------------------------------------------
void list_file(std::string title, std::string file, Fl_Window* parent, bool fixed_font, int W, int H) {
    _DlgList dlg(title.c_str(), flw::StringVector(), file, parent, fixed_font, W, H);
    dlg.run();
}

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
    Fl_Double_Window(0, 0, 10, 10) {
        end();

        _browse    = new Fl_Button(0, 0, 0, 0, "&Browse");
        _cancel    = new Fl_Button(0, 0, 0, 0, PASSWORD_CANCEL);
        _close     = new Fl_Return_Button(0, 0, 0, 0, PASSWORD_OK);
        _file      = new Fl_Output(0, 0, 0, 0, "Key file");
        _grid      = new GridGroup(0, 0, w(), h());
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

        resizable(_grid);
        util::labelfont(this);
        callback(_DlgPassword::Callback, this);
        label(title);
        size(W, H);
        size_range(W, H);
        set_modal();
        util::center_window(this, parent);
        _grid->do_layout();
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

        util::zero_memory(const_cast<char*>(_password1->value()), strlen(_password1->value()));
        util::zero_memory(const_cast<char*>(_password2->value()), strlen(_password2->value()));
        util::zero_memory(const_cast<char*>(_file->value()), strlen(_file->value()));

        return _ret;
    }
};

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

/***
 *           _____  _       _____      _       _   
 *          |  __ \| |     |  __ \    (_)     | |  
 *          | |  | | | __ _| |__) | __ _ _ __ | |_ 
 *          | |  | | |/ _` |  ___/ '__| | '_ \| __|
 *          | |__| | | (_| | |   | |  | | | | | |_ 
 *          |_____/|_|\__, |_|   |_|  |_|_| |_|\__|
 *      ______         __/ |                       
 *     |______|       |___/                        
 */

//------------------------------------------------------------------------------
class _DlgPrint : public Fl_Double_Window {
    Fl_Button*                  _close;
    Fl_Button*                  _file;
    Fl_Button*                  _print;
    Fl_Choice*                  _format;
    Fl_Choice*                  _layout;
    Fl_Hor_Slider*              _from;
    Fl_Hor_Slider*              _to;
    Fl_Widget*                  _widget;
    GridGroup*                  _grid;
    PrintCallback               _cb;

public:
    //--------------------------------------------------------------------------
    _DlgPrint(std::string title, Fl_Window* parent, PrintCallback cb, Fl_Widget* widget, int from, int to) :
    Fl_Double_Window(0, 0, flw::PREF_FONTSIZE * 34, flw::PREF_FONTSIZE * 18) {
        end();

        _close  = new Fl_Button(0, 0, 0, 0, "&Close");
        _file   = new Fl_Button(0, 0, 0, 0, "output.ps");
        _format = new Fl_Choice(0, 0, 0, 0);
        _from   = new Fl_Hor_Slider(0, 0, 0, 0);
        _grid   = new GridGroup(0, 0, w(), h());
        _layout = new Fl_Choice(0, 0, 0, 0);
        _print  = new Fl_Button(0, 0, 0, 0, "&Print");
        _to     = new Fl_Hor_Slider(0, 0, 0, 0);
        _cb     = cb;
        _widget = widget;
        
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
        _from->align(FL_ALIGN_TOP | FL_ALIGN_LEFT);
        _from->callback(_DlgPrint::Callback, this);
        _from->range(from, to);
        _from->value(from);
        _from->tooltip("Start page number.");
        _print->callback(_DlgPrint::Callback, this);
        _to->align(FL_ALIGN_TOP | FL_ALIGN_LEFT);
        _to->callback(_DlgPrint::Callback, this);
        _to->range(from, to);
        _to->value(from);
        _to->tooltip("End page number.");
        
        if (from < 1 || from >= to) {
            _from->deactivate();
            _from->range(0, 1);
            _from->value(0);
            _to->deactivate();
            _to->range(0, 1);
            _to->value(0);
        }
        
        dlg::_init_printer_formats(_format, _layout);
        _DlgPrint::Callback(_from, this);
        _DlgPrint::Callback(_to, this);
        callback(_DlgPrint::Callback, this);
        copy_label(title.c_str());
        size_range(flw::PREF_FONTSIZE * 34, flw::PREF_FONTSIZE * 18);
        set_modal();
        resizable(_grid);
        util::center_window(this, parent);
        _grid->do_layout();
    }

    //--------------------------------------------------------------------------
    static void Callback(Fl_Widget* w, void* o) {
        auto self = static_cast<_DlgPrint*>(o);

        if (w == self) {
            self->hide();
        }
        else if (w == self->_close) {
            self->hide();
        }
        else if (w == self->_file) {
            auto filename = fl_file_chooser("Save To PostScript File", "PostScript Files (*.ps)\tAll Files (*)", self->_file->label());
            
            if (filename != nullptr) {
                self->_file->copy_label(filename);
            }
        }
        else if (w == self->_from || w == self->_to) {
            auto l = util::format("%s page number: %d", (w == self->_from) ? "From" : "To", (int) static_cast<Fl_Hor_Slider*>(w)->value());
            w->copy_label(l.c_str());
            self->redraw();
        }
        else if (w == self->_print) {
            self->print();
        }
    }

    //--------------------------------------------------------------------------
    void print() {
        auto from   = static_cast<int>(_from->value());
        auto to     = static_cast<int>(_to->value());
        auto format = static_cast<Fl_Paged_Device::Page_Format>(_format->value());
        auto layout = (_layout->value() == 0) ? Fl_Paged_Device::Page_Layout::PORTRAIT : Fl_Paged_Device::Page_Layout::LANDSCAPE;
        auto file   = _file->label();
        auto err    = util::print(file, format, layout, _cb, _widget, from, to);
        
        if (err != "") {
            fl_alert("Printing failed!\n%s", err.c_str());
            return;
        }

        hide();
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

//------------------------------------------------------------------------------
void print(std::string title, PrintCallback cb, Fl_Widget* widget, int from, int to, Fl_Window* parent) {
    _DlgPrint dlg(title, parent, cb, widget, from, to);
    dlg.run();
}

/***
 *           _____  _       _____      _       _ _______        _   
 *          |  __ \| |     |  __ \    (_)     | |__   __|      | |  
 *          | |  | | | __ _| |__) | __ _ _ __ | |_ | | _____  _| |_ 
 *          | |  | | |/ _` |  ___/ '__| | '_ \| __|| |/ _ \ \/ / __|
 *          | |__| | | (_| | |   | |  | | | | | |_ | |  __/>  <| |_ 
 *          |_____/|_|\__, |_|   |_|  |_|_| |_|\__||_|\___/_/\_\\__|
 *      ______         __/ |                                        
 *     |______|       |___/                                         
 */

//------------------------------------------------------------------------------
class _DlgPrintText : public Fl_Double_Window {
    Fl_Box*                     _label;
    Fl_Button*                  _close;
    Fl_Button*                  _file;
    Fl_Button*                  _fonts;
    Fl_Button*                  _print;
    Fl_Check_Button*            _border;
    Fl_Check_Button*            _wrap;
    Fl_Choice*                  _align;
    Fl_Choice*                  _format;
    Fl_Choice*                  _layout;
    Fl_Font                     _font;
    Fl_Fontsize                 _fontsize;
    Fl_Hor_Slider*              _line;
    Fl_Hor_Slider*              _tab;
    GridGroup*                  _grid;
    bool                        _ret;
    const StringVector&         _text;
    std::string                 _label2;

public:
    //--------------------------------------------------------------------------
    _DlgPrintText(std::string title, Fl_Window* parent, const StringVector& text) :
    Fl_Double_Window(0, 0, flw::PREF_FONTSIZE * 34, flw::PREF_FONTSIZE * 35),
    _text(text) {
        end();

        _align    = new Fl_Choice(0, 0, 0, 0);
        _border   = new Fl_Check_Button(0, 0, 0, 0, "Print border");
        _close    = new Fl_Button(0, 0, 0, 0, "&Close");
        _file     = new Fl_Button(0, 0, 0, 0, "output.ps");
        _fonts    = new Fl_Button(0, 0, 0, 0, "Courier - 14");
        _format   = new Fl_Choice(0, 0, 0, 0);
        _grid     = new GridGroup(0, 0, w(), h());
        _label    = new Fl_Box(0, 0, 0, 0);
        _layout   = new Fl_Choice(0, 0, 0, 0);
        _line     = new Fl_Hor_Slider(0, 0, 0, 0);
        _print    = new Fl_Button(0, 0, 0, 0, "&Print");
        _tab      = new Fl_Hor_Slider(0, 0, 0, 0);
        _wrap     = new Fl_Check_Button(0, 0, 0, 0, "Wrap lines");
        _ret      = false;
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
        _align->add("Left align");
        _align->add("Center align");
        _align->add("Right align");
        _align->tooltip("Line numbers are only used for left aligned text.");
        _align->value(0);
        _border->tooltip("Print line border around the print area.");
        _close->callback(_DlgPrintText::Callback, this);
        _file->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
        _file->callback(_DlgPrintText::Callback, this);
        _file->tooltip("Select output PostScript file.");
        _fonts->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
        _fonts->callback(_DlgPrintText::Callback, this);
        _fonts->tooltip("Select font to use.");
        _label->align(FL_ALIGN_TOP | FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
        _label->box(FL_BORDER_BOX);
        _label->box(FL_THIN_DOWN_BOX);
        _line->align(FL_ALIGN_TOP | FL_ALIGN_LEFT);
        _line->callback(_DlgPrintText::Callback, this);
        _line->range(0, 6);
        _line->value(0);
        _line->tooltip("Set minimum line number width.\nSet to 0 to disable.");
        _print->callback(_DlgPrintText::Callback, this);
        _tab->align(FL_ALIGN_TOP | FL_ALIGN_LEFT);
        _tab->callback(_DlgPrintText::Callback, this);
        _tab->range(0, 16);
        _tab->value(0);
        _tab->tooltip("Replace tabs with spaces.\nSet to 0 to disable.");
        _wrap->tooltip("Wrap long lines or they will be clipped.");

        dlg::_init_printer_formats(_format, _layout);
        _DlgPrintText::Callback(_line, this);
        _DlgPrintText::Callback(_tab, this);
        callback(_DlgPrintText::Callback, this);
        copy_label(title.c_str());
        size_range(flw::PREF_FONTSIZE * 34, flw::PREF_FONTSIZE * 35);
        set_modal();
        resizable(_grid);
        util::center_window(this, parent);
        _grid->do_layout();
        set_label();
    }

    //--------------------------------------------------------------------------
    static void Callback(Fl_Widget* w, void* o) {
        auto self = static_cast<_DlgPrintText*>(o);

        if (w == self) {
            self->hide();
        }
        else if (w == self->_close) {
            self->hide();
        }
        else if (w == self->_file) {
            auto filename = fl_file_chooser("Save To PostScript File", "PostScript Files (*.ps)\tAll Files (*)", self->_file->label());
            
            if (filename != nullptr) {
                self->_file->copy_label(filename);
            }
        }
        else if (w == self->_fonts) {
            auto dlg = FontDialog(self->_font, self->_fontsize, "Select Print Font", true);

            if (dlg.run() == true) {
                auto l = util::format("%s - %d", dlg.fontname().c_str(), dlg.fontsize());
                self->_fonts->copy_label(l.c_str());
                
                self->_font     = dlg.font();
                self->_fontsize = dlg.fontsize();
            }
        }
        else if (w == self->_line) {
            auto l = util::format("Line number = %d", (int) self->_line->value());
            self->_line->copy_label(l.c_str());
            self->redraw();
        }
        else if (w == self->_print) {
            self->print();
        }
        else if (w == self->_tab) {
            auto l = util::format("Tab replacement = %d", (int) self->_tab->value());
            self->_tab->copy_label(l.c_str());
            self->redraw();
        }
    }

    //--------------------------------------------------------------------------
    void print() {
        auto border  = _border->value();
        auto wrap    = _wrap->value();
        auto line    = static_cast<int>(_line->value());
        auto tab     = static_cast<int>(_tab->value());
        auto format  = static_cast<Fl_Paged_Device::Page_Format>(_format->value());
        auto layout  = (_layout->value() == 0) ? Fl_Paged_Device::Page_Layout::PORTRAIT : Fl_Paged_Device::Page_Layout::LANDSCAPE;
        auto align   = (_align->value() == 0) ? FL_ALIGN_LEFT : (_align->value() == 1) ? FL_ALIGN_CENTER : FL_ALIGN_RIGHT;
        auto file    = _file->label();
        auto printer = PrintText(file, format, layout, _font, _fontsize, align, wrap, border, line);            
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

    //--------------------------------------------------------------------------
    bool run() {
        show();

        while (visible() != 0) {
            Fl::wait();
            Fl::flush();
        }

        return _ret;
    }

    //--------------------------------------------------------------------------
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

//------------------------------------------------------------------------------
bool print_text(std::string title, const std::string& text, Fl_Window* parent) {
    auto lines = util::split_string(text, "\n");
    _DlgPrintText dlg(title, parent, lines);
    return dlg.run();
}

//------------------------------------------------------------------------------
bool print_text(std::string title, const StringVector& text, Fl_Window* parent) {
    _DlgPrintText dlg(title, parent, text);
    return dlg.run();
}

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
        _grid   = new GridGroup(0, 0, w(), h());
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
        resizable(_grid);
        util::center_window(this, parent);
        _grid->do_layout();
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
        _grid   = new GridGroup(0, 0, w(), h());
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
        resizable(_grid);
        util::center_window(this, parent);
        _grid->do_layout();
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
    char* run() {
        show();

        while (visible() != 0) {
            Fl::wait();
            Fl::flush();
        }

        return _res;
    }
};

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
    Fl_Double_Window(0, 0, 10, 10, "Set Theme") {
        end();

        _close       = new Fl_Return_Button(0, 0, 0, 0, "&Close");
        _fixedfont   = new Fl_Button(0, 0, 0, 0, "&Mono font");
        _fixed_label = new Fl_Box(0, 0, 0, 0);
        _font        = new Fl_Button(0, 0, 0, 0, "&Regular font");
        _font_label  = new Fl_Box(0, 0, 0, 0);
        _grid        = new GridGroup(0, 0, w(), h());
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

        resizable(_grid);
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
            FontDialog fd(flw::PREF_FONT, flw::PREF_FONTSIZE, "Select Regular Font");

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

            if (row == theme::THEME_GLEAM) {
                theme::_load_gleam();
            }
            else if (row == theme::THEME_GLEAM_BLUE) {
                theme::_load_gleam_blue();
            }
            else if (row == theme::THEME_GLEAM_DARK) {
                theme::_load_gleam_dark();
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
            else if (row == theme::THEME_GTK_DARK) {
                theme::_load_gtk_dark();
            }
            else if (row == theme::THEME_GTK_TAN) {
                theme::_load_gtk_tan();
            }
            else if (row == theme::THEME_OXY) {
                theme::_load_oxy();
            }
            else if (row == theme::THEME_OXY_TAN) {
                theme::_load_oxy_tan();
            }
            else if (row == theme::THEME_PLASTIC) {
                theme::_load_plastic();
            }
            else if (row == theme::THEME_PLASTIC_TAN) {
                theme::_load_plastic_tan();
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
        size(flw::PREF_FONTSIZE * 30, flw::PREF_FONTSIZE * 28);
        size_range(flw::PREF_FONTSIZE * 20, flw::PREF_FONTSIZE * 14);
        _grid->resize(0, 0, w(), h());
        theme::_scrollbar();

        for (int f = 0; f < theme::THEME_NIL; f++) {
            if (flw::PREF_THEME == flw::PREF_THEMES[f]) {
                _theme->value(f + 1);
                break;
            }

        }

        Fl::redraw();
    }
};

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
    _button   = new Fl_Button(0, 0, 0, 0, "Press to abort");
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
 *           ______          _   _____  _       _             _           _          _
 *          |  ____|        | | |  __ \(_)     | |           | |         | |        | |
 *          | |__ ___  _ __ | |_| |  | |_  __ _| | ___   __ _| |     __ _| |__   ___| |
 *          |  __/ _ \| '_ \| __| |  | | |/ _` | |/ _ \ / _` | |    / _` | '_ \ / _ \ |
 *          | | | (_) | | | | |_| |__| | | (_| | | (_) | (_| | |___| (_| | |_) |  __/ |
 *          |_|  \___/|_| |_|\__|_____/|_|\__,_|_|\___/ \__, |______\__,_|_.__/ \___|_|
 *      ______                                           __/ |
 *     |______|                                         |___/
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

//------------------------------------------------------------------------------
FontDialog::FontDialog(Fl_Font font, Fl_Fontsize fontsize, const std::string& label, bool limit_to_default) :
Fl_Double_Window(0, 0, flw::PREF_FONTSIZE * 64, flw::PREF_FONTSIZE * 36) {
    _create(font, "", fontsize, label, limit_to_default);
}

//------------------------------------------------------------------------------
FontDialog::FontDialog(std::string font, Fl_Fontsize fontsize, std::string label, bool limit_to_default) :
Fl_Double_Window(0, 0, flw::PREF_FONTSIZE * 64, flw::PREF_FONTSIZE * 36) {
    _create(0, font, fontsize, label, limit_to_default);
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
void FontDialog::_create(Fl_Font font, std::string fontname, Fl_Fontsize fontsize, std::string label, bool limit_to_default) {
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
