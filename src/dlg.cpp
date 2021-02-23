// Copyright 2016 - 2021 gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#include "dlg.h"
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_File_Chooser.H>
#include <FL/Fl_Help_View.H>
#include <FL/Fl_Hold_Browser.H>
#include <FL/Fl_Output.H>
#include <FL/Fl_Return_Button.H>
#include <FL/Fl_Secret_Input.H>
#include <FL/Fl_Text_Display.H>

// MALAGMA_ON

namespace flw {
    namespace dlg {
        //----------------------------------------------------------------------
        class _DlgHtml  : public Fl_Double_Window {
            Fl_Help_View*               _html;
            Fl_Return_Button*           _close;

        public:
            //------------------------------------------------------------------
            _DlgHtml(const char* title, const char* text, Fl_Window* parent, int W, int H) :
            Fl_Double_Window(0, 0, flw::PREF_FONTSIZE * W,flw::PREF_FONTSIZE * H) {
                end();

                _close = new Fl_Return_Button(0, 0, 0, 0, "&Close");
                _html  = new Fl_Help_View(0, 0, 0, 0);

                add(_close);
                add(_html);

                _close->callback(_DlgHtml::Callback, this);
                _close->labelfont(flw::PREF_FONT);
                _close->labelsize(flw::PREF_FONTSIZE);
                _html->textfont(flw::PREF_FONT);
                _html->textsize(flw::PREF_FONTSIZE);
                _html->value(text);

                callback(_DlgHtml::Callback, this);
                copy_label(title);
                set_modal();
                resizable(this);
                util::center_window(this, parent);
            }

            //------------------------------------------------------------------
            static void Callback(Fl_Widget* w, void* o) {
                auto dlg = (_DlgHtml*) o;

                if (w == dlg || w == dlg->_close) {
                    dlg->hide();
                }
            }

            //------------------------------------------------------------------
            void resize(int X, int Y, int W, int H) override {
                Fl_Double_Window::resize(X, Y, W, H);

                _html->resize(4, 4, W - 8, H - flw::PREF_FONTSIZE * 2 - 12);
                _close->resize(W - flw::PREF_FONTSIZE * 10 - 4, H - flw::PREF_FONTSIZE * 2 - 4, flw::PREF_FONTSIZE * 10, flw::PREF_FONTSIZE * 2);
            }

            //------------------------------------------------------------------
            void run() {
                show();

                while (visible()) {
                    Fl::wait();
                    Fl::flush();
                }
            }
        };

        //----------------------------------------------------------------------
        //----------------------------------------------------------------------
        //----------------------------------------------------------------------

        //----------------------------------------------------------------------
        class _DlgList : public Fl_Double_Window {
            Fl_Hold_Browser*            _list;
            Fl_Return_Button*           _close;

        public:
            //------------------------------------------------------------------
            _DlgList(const char* title, const std::vector<std::string>& list, Fl_Window* parent = nullptr, bool fixed_font = false, int W = 50, int H = 20) :
            Fl_Double_Window(0, 0, (fixed_font ? flw::PREF_FIXED_FONTSIZE : flw::PREF_FONTSIZE) * W, (fixed_font ? flw::PREF_FIXED_FONTSIZE : flw::PREF_FONTSIZE) * H) {
                end();

                _close = new Fl_Return_Button(0, 0, 0, 0, "&Close");
                _list  = new Fl_Hold_Browser(0, 0, 0, 0);

                add(_close);
                add(_list);

                _close->callback(_DlgList::Callback, this);
                _close->labelfont(flw::PREF_FONT);
                _close->labelsize(flw::PREF_FONTSIZE);

                resize(0, 0, w(), h());

                for (auto& s : list) {
                    _list->add(s.c_str());
                }

                if (fixed_font) {
                    _list->textfont(flw::PREF_FIXED_FONT);
                    _list->textsize(flw::PREF_FIXED_FONTSIZE);
                }
                else {
                    _list->textfont(flw::PREF_FONT);
                    _list->textsize(flw::PREF_FONTSIZE);
                }

                callback(_DlgList::Callback, this);
                copy_label(title);
                set_modal();
                resizable(this);
                util::center_window(this, parent);
            }

            //--------------------------------------------------------------------------
            static void Callback(Fl_Widget* w, void* o) {
                auto dlg = (_DlgList*) o;

                if (w == dlg || w == dlg->_close) {
                    dlg->hide();
                }
            }

            //------------------------------------------------------------------
            void resize(int X, int Y, int W, int H) override {
                Fl_Double_Window::resize(X, Y, W, H);

                _list->resize(4, 4, W - 8, H - flw::PREF_FONTSIZE * 2 - 12);
                _close->resize(W - flw::PREF_FONTSIZE * 10 - 4, H - flw::PREF_FONTSIZE * 2 - 4, flw::PREF_FONTSIZE * 10, flw::PREF_FONTSIZE * 2);
            }

            //--------------------------------------------------------------------------
            void run() {
                show();

                while (visible()) {
                    Fl::wait();
                    Fl::flush();
                }
            }
        };

        //----------------------------------------------------------------------
        //----------------------------------------------------------------------
        //----------------------------------------------------------------------

        //----------------------------------------------------------------------
        class _DlgSelect : public Fl_Double_Window {
            Fl_Button*                      _close;
            Fl_Button*                      _cancel;
            Fl_Browser*                     _list;
            Fl_Input*                       _filter;
            const std::vector<std::string>& _strings;

        public:
            //------------------------------------------------------------------
            _DlgSelect(const char* title, Fl_Window* parent, const std::vector<std::string>& strings, int selected_string_index, std::string selected_string, bool fixed_font, int W, int H) :
            Fl_Double_Window(0, 0, (fixed_font ? flw::PREF_FIXED_FONTSIZE : flw::PREF_FONTSIZE) * W, (fixed_font ? flw::PREF_FIXED_FONTSIZE : flw::PREF_FONTSIZE) * H),
            _strings(strings) {
                end();

                _filter = new Fl_Input(0, 0, 0, 0);
                _close  = new Fl_Return_Button(0, 0, 0, 0, "&Select");
                _cancel = new Fl_Button(0, 0, 0, 0, "&Cancel");
                _list   = new Fl_Hold_Browser(0, 0, 0, 0);

                add(_filter);
                add(_list);
                add(_cancel);
                add(_close);

                _cancel->callback(Callback, this);
                _close->callback(Callback, this);
                _filter->callback(Callback, this);
                _filter->tooltip("Enter text to filter rows that macthes the text");
                _filter->when(FL_WHEN_CHANGED);
                _list->callback(Callback, this);
                _list->tooltip("Use Page Up or Page Down in list to scroll faster");

                if (fixed_font) {
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

                resize(0, 0, w(), h());

                {
                    auto r = 0;
                    auto f = 0;

                    for (auto& string : _strings) {
                        _list->add(string.c_str());

                        if (string == selected_string) {
                            r = f + 1;
                        }

                        f++;
                    }

                    if (selected_string_index && selected_string_index <= (int) _strings.size()) {
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

                flw::util::labelfont(this);
                callback(Callback, this);
                copy_label(title);
                set_modal();
                resizable(this);
                util::center_window(this, parent);
                activate_button();
                _filter->take_focus();
            }

            //------------------------------------------------------------------
            void activate_button() {
                if (_list->value() == 0) {
                    _close->deactivate();
                }
                else {
                    _close->activate();
                }
            }

            //------------------------------------------------------------------
            static void Callback(Fl_Widget* w, void* o) {
                auto dlg = (_DlgSelect*) o;

                if (w == dlg || w == dlg->_cancel) {
                    dlg->_list->deselect();
                    dlg->hide();
                }
                else if (w == dlg->_filter) {
                    dlg->filter(dlg->_filter->value());
                    dlg->activate_button();
                }
                else if (w == dlg->_list) {
                    dlg->activate_button();

                    if (Fl::event_clicks() > 0 && dlg->_close->active()) {
                        Fl::event_clicks(0);
                        dlg->hide();
                    }
                }
                else if (w == dlg->_close) {
                    dlg->hide();
                }
            }

            //------------------------------------------------------------------
            void filter(const char* filter) {
                _list->clear();

                for (auto& string : _strings) {
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
            int handle(int event) override {
                if (event == FL_KEYDOWN) {
                    if (Fl::event_key() == FL_Enter) {
                        if (_list->value()) {
                            hide();
                        }

                        return 1;
                    }
                    else if (Fl::event_key() == FL_Tab) {
                        if (Fl::focus() == _list) {
                            _filter->take_focus();
                        }
                        else {
                            _list->take_focus();
                        }

                        return 1;
                    }
                    else if (Fl::event_key() == FL_Page_Up) {
                        _list->value(_list->value() - 10);
                        _list->show(_list->value());
                        return 1;
                    }
                    else if (Fl::event_key() == FL_Page_Down) {
                        _list->value(_list->value() + 10);
                        _list->show(_list->value());
                        return 1;
                    }
                }

                return Fl_Double_Window::handle(event);
            }

            //------------------------------------------------------------------
            void resize(int X, int Y, int W, int H) override {
                Fl_Double_Window::resize(X, Y, W, H);

                _filter->resize(4, 4, W - 8, flw::PREF_FONTSIZE * 2);
                _list->resize(4, flw::PREF_FONTSIZE * 2  + 8, W - 8, H - flw::PREF_FONTSIZE * 4 - 16);
                _cancel->resize(W - flw::PREF_FONTSIZE * 20 - 8, H - flw::PREF_FONTSIZE * 2 - 4, flw::PREF_FONTSIZE * 10, flw::PREF_FONTSIZE * 2);
                _close->resize(W - flw::PREF_FONTSIZE * 10 - 4, H - flw::PREF_FONTSIZE * 2 - 4, flw::PREF_FONTSIZE * 10, flw::PREF_FONTSIZE * 2);
            }

            //------------------------------------------------------------------
            int run() {
                show();

                while (visible()) {
                    Fl::wait();
                    Fl::flush();
                }

                auto row = _list->value();

                if (row) {
                    auto selected = _list->text(row);

                    for (int f = 0; f < (int) _strings.size(); f++) {
                        auto& string = _strings[f];

                        if (string == selected) {
                            return f + 1;
                        }
                    }
                }

                return 0;
            }
        };

        //----------------------------------------------------------------------
        //----------------------------------------------------------------------
        //----------------------------------------------------------------------

        //----------------------------------------------------------------------
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
            _DlgPassword::TYPE          _mode;
            bool                        _ret;

        public:
            //------------------------------------------------------------------
            _DlgPassword(const char* title, Fl_Window* parent, _DlgPassword::TYPE mode) :
            Fl_Double_Window(0, 0, 0, 0) {
                end();

                _password1 = new Fl_Secret_Input(0, 0, 0, 0, "Password");
                _password2 = new Fl_Secret_Input(0, 0, 0, 0, "Enter Password Again");
                _browse    = new Fl_Button(0, 0, 0, 0, "&Browse");
                _cancel    = new Fl_Button(0, 0, 0, 0, "&Cancel");
                _close     = new Fl_Return_Button(0, 0, 0, 0, "&Ok");
                _file      = new Fl_Output(0, 0, 0, 0, "Key File");
                _mode      = mode;
                _ret       = false;

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

                if (_mode == _DlgPassword::TYPE::ASK_PASSWORD) {
                    _password2->hide();
                    _browse->hide();
                    _file->hide();
                    add(_password1);
                    resize(0, 0, 30 * flw::PREF_FONTSIZE, 5 * flw::PREF_FONTSIZE + 12);
                }
                else if (_mode == _DlgPassword::TYPE::CONFIRM_PASSWORD) {
                    _browse->hide();
                    _file->hide();
                    add(_password1);
                    add(_password2);
                    resize(0, 0, 30 * flw::PREF_FONTSIZE, 8 * flw::PREF_FONTSIZE + 20);
                }
                else if (_mode == _DlgPassword::TYPE::ASK_PASSWORD_AND_KEYFILE) {
                    _password2->hide();
                    add(_password1);
                    add(_file);
                    add(_browse);
                    resize(0, 0, 40 * flw::PREF_FONTSIZE, 8 * flw::PREF_FONTSIZE + 20);
                }
                else if (_mode == _DlgPassword::TYPE::CONFIRM_PASSWORD_AND_KEYFILE) {
                    add(_password1);
                    add(_password2);
                    add(_file);
                    add(_browse);
                    resize(0, 0, 40 * flw::PREF_FONTSIZE, 11 * flw::PREF_FONTSIZE + 24);
                }

                add(_cancel);
                add(_close);

                flw::util::labelfont(this);
                callback(_DlgPassword::Callback, this);
                label(title);
                set_modal();
                resizable(this);
                util::center_window(this, parent);
            }

            //------------------------------------------------------------------
            static void Callback(Fl_Widget* w, void* o) {
                auto dlg = (_DlgPassword*) o;

                if (w == dlg) {
                    ;
                }
                else if (w == dlg->_password1) {
                    dlg->check();
                }
                else if (w == dlg->_password2) {
                    dlg->check();
                }
                else if (w == dlg->_browse) {
                    auto filename = fl_file_chooser("Select Key File", nullptr, nullptr, 0);

                    if (filename) {
                        dlg->_file->value(filename);
                    }
                    else {
                        dlg->_file->value("");
                    }
                }
                else if (w == dlg->_cancel) {
                    dlg->_ret = false;
                    dlg->hide();
                }
                else if (w == dlg->_close) {
                    dlg->_ret = true;
                    dlg->hide();
                }
            }

            //------------------------------------------------------------------
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

            //------------------------------------------------------------------
            void resize(int X, int Y, int W, int H) override {
                Fl_Double_Window::resize(X, Y, W, H);

                if (_mode == _DlgPassword::TYPE::ASK_PASSWORD) {
                    _password1->resize(4, flw::PREF_FONTSIZE + 4, W - 8, flw::PREF_FONTSIZE * 2);
                }
                else if (_mode == _DlgPassword::TYPE::CONFIRM_PASSWORD) {
                    _password1->resize(4, flw::PREF_FONTSIZE + 4, W - 8, flw::PREF_FONTSIZE * 2);
                    _password2->resize(4, flw::PREF_FONTSIZE * 4 + 12, W - 8, flw::PREF_FONTSIZE * 2);
                }
                else if (_mode == _DlgPassword::TYPE::ASK_PASSWORD_AND_KEYFILE) {
                    _password1->resize(4, flw::PREF_FONTSIZE + 4, W - 8, flw::PREF_FONTSIZE * 2);
                    _file->resize(4, flw::PREF_FONTSIZE * 4 + 12, W - flw::PREF_FONTSIZE * 10 - 12, flw::PREF_FONTSIZE * 2);
                    _browse->resize(W - flw::PREF_FONTSIZE * 10 - 4, flw::PREF_FONTSIZE * 4 + 12, flw::PREF_FONTSIZE * 10, flw::PREF_FONTSIZE * 2);
                }
                else if (_mode == _DlgPassword::TYPE::CONFIRM_PASSWORD_AND_KEYFILE) {
                    _password1->resize(4, flw::PREF_FONTSIZE + 4, W - 8, flw::PREF_FONTSIZE * 2);
                    _password2->resize(4, flw::PREF_FONTSIZE * 4 + 12, W - 8, flw::PREF_FONTSIZE * 2);
                    _file->resize(4, flw::PREF_FONTSIZE * 7 + 16, W - flw::PREF_FONTSIZE * 10 - 12, flw::PREF_FONTSIZE * 2);
                    _browse->resize(W - flw::PREF_FONTSIZE * 10 - 4, flw::PREF_FONTSIZE * 7 + 16, flw::PREF_FONTSIZE * 10, flw::PREF_FONTSIZE * 2);
                }

                _cancel->resize(W - flw::PREF_FONTSIZE * 20 - 8, H - flw::PREF_FONTSIZE * 2 - 4, flw::PREF_FONTSIZE * 10, flw::PREF_FONTSIZE * 2);
                _close->resize(W - flw::PREF_FONTSIZE * 10 - 4, H - flw::PREF_FONTSIZE * 2 - 4, flw::PREF_FONTSIZE * 10, flw::PREF_FONTSIZE * 2);
            }

            //------------------------------------------------------------------
            bool run(std::string& password, std::string& file) {
                show();

                while (visible()) {
                    Fl::wait();
                    Fl::flush();
                }

                if (_ret) {
                    file = _file->value();
                    password = _password1->value();
                }

                util::zero_memory((void*) _password1->value(), strlen(_password1->value()));
                util::zero_memory((void*) _password2->value(), strlen(_password2->value()));
                util::zero_memory((void*) _file->value(), strlen(_file->value()));

                return _ret;
            }
        };

        //----------------------------------------------------------------------
        //----------------------------------------------------------------------
        //----------------------------------------------------------------------

        //----------------------------------------------------------------------
        class _DlgText : public Fl_Double_Window {
            Fl_Return_Button*           _close;
            Fl_Text_Buffer*             _buffer;
            Fl_Text_Display*            _text;

        public:
            //------------------------------------------------------------------
            _DlgText(const char* title, const char* text, Fl_Window* parent, bool fixed_font, int W, int H) :
            Fl_Double_Window(0, 0, (fixed_font ? flw::PREF_FIXED_FONTSIZE : flw::PREF_FONTSIZE) * W, (fixed_font ? flw::PREF_FIXED_FONTSIZE : flw::PREF_FONTSIZE) * H) {
                end();

                _close   = new Fl_Return_Button(0, 0, 0, 0, "&Close");
                _text = new Fl_Text_Display(0, 0, w(), h());
                _buffer  = new Fl_Text_Buffer();

                add(_close);
                add(_text);

                _buffer->text(text);
                _close->callback(_DlgText::Callback, this);
                _close->labelfont(flw::PREF_FONT);
                _close->labelsize(flw::PREF_FONTSIZE);
                _text->linenumber_align(FL_ALIGN_RIGHT);
                _text->linenumber_bgcolor(FL_BACKGROUND_COLOR);
                _text->linenumber_fgcolor(FL_FOREGROUND_COLOR);
                _text->buffer(_buffer);

                if (fixed_font) {
                    _text->linenumber_font(flw::PREF_FIXED_FONT);
                    _text->linenumber_size(flw::PREF_FIXED_FONTSIZE);
                    _text->linenumber_width(flw::PREF_FIXED_FONTSIZE * 3);
                    _text->textfont(flw::PREF_FIXED_FONT);
                    _text->textsize(flw::PREF_FIXED_FONTSIZE);
                }
                else {
                    _text->linenumber_font(flw::PREF_FONT);
                    _text->linenumber_size(flw::PREF_FONTSIZE);
                    _text->linenumber_width(flw::PREF_FONTSIZE * 3);
                    _text->textfont(flw::PREF_FONT);
                    _text->textsize(flw::PREF_FONTSIZE);
                }

                callback(_DlgText::Callback, this);
                copy_label(title);
                set_modal();
                resizable(this);
                util::center_window(this, parent);
            }

            //------------------------------------------------------------------
            ~_DlgText() {
               _text->buffer(nullptr);
                delete _buffer;
            }

            //------------------------------------------------------------------
            static void Callback(Fl_Widget* w, void* o) {
                auto dlg = (_DlgText*) o;

                if (w == dlg || w == dlg->_close) {
                    dlg->hide();
                }
            }

            //------------------------------------------------------------------
            void resize(int X, int Y, int W, int H) override {
                Fl_Double_Window::resize(X, Y, W, H);

                _text->resize(4, 4, W - 8, H - flw::PREF_FONTSIZE * 2 - 12);
                _close->resize(W - flw::PREF_FONTSIZE * 10 - 4, H - flw::PREF_FONTSIZE * 2 - 4, flw::PREF_FONTSIZE * 10, flw::PREF_FONTSIZE * 2);
            }

            //------------------------------------------------------------------
            void run() {
                show();

                while (visible()) {
                    Fl::wait();
                    Fl::flush();
                }
            }
        };
    }
}

//------------------------------------------------------------------------------
void flw::dlg::html(const std::string& title, const std::string& text, Fl_Window* parent, int W, int H) {
    _DlgHtml dlg(title.c_str(), text.c_str(), parent, W, H);
    dlg.run();
}

//------------------------------------------------------------------------------
void flw::dlg::list(const std::string& title, const std::vector<std::string>& list, Fl_Window* parent, bool fixed_font, int W, int H) {
    _DlgList dlg(title.c_str(), list, parent, fixed_font, W, H);
    dlg.run();
}

//------------------------------------------------------------------------------
void flw::dlg::list(const std::string& title, const std::string& list, Fl_Window* parent, bool fixed_font, int W, int H) {
    auto list2 = flw::util::split( list, "\n");
    _DlgList dlg(title.c_str(), list2, parent, fixed_font, W, H);
    dlg.run();
}

//------------------------------------------------------------------------------
void flw::dlg::panic(std::string message) {
    fl_alert("panic! I have to quit\n%s", message.c_str());
    exit(1);
}

//------------------------------------------------------------------------------
bool flw::dlg::password1(const std::string& title, std::string& password, Fl_Window* parent) {
    std::string file;
    _DlgPassword dlg(title.c_str(), parent, _DlgPassword::TYPE::ASK_PASSWORD);
    return dlg.run(password, file);
}

//------------------------------------------------------------------------------
bool flw::dlg::password2(const std::string& title, std::string& password, Fl_Window* parent) {
    std::string file;
    _DlgPassword dlg(title.c_str(), parent, _DlgPassword::TYPE::CONFIRM_PASSWORD);
    return dlg.run(password, file);
}

//------------------------------------------------------------------------------
bool flw::dlg::password3(const std::string& title, std::string& password, std::string& file, Fl_Window* parent) {
    _DlgPassword dlg(title.c_str(), parent, _DlgPassword::TYPE::ASK_PASSWORD_AND_KEYFILE);
    return dlg.run(password, file);
}

//------------------------------------------------------------------------------
bool flw::dlg::password4(const std::string& title, std::string& password, std::string& file, Fl_Window* parent) {
    _DlgPassword dlg(title.c_str(), parent, _DlgPassword::TYPE::CONFIRM_PASSWORD_AND_KEYFILE);
    return dlg.run(password, file);
}

//------------------------------------------------------------------------------
int flw::dlg::select(const std::string& title, const std::vector<std::string>& list, int selected_row, Fl_Window* parent, bool fixed_font, int W, int H) {
    _DlgSelect dlg(title.c_str(), parent, list, selected_row, "", fixed_font, W, H);
    return dlg.run();
}

//------------------------------------------------------------------------------
int flw::dlg::select(const std::string& title, const std::vector<std::string>& list, const std::string& selected_row, Fl_Window* parent, bool fixed_font, int W, int H) {
    _DlgSelect dlg(title.c_str(), parent, list, 0, selected_row, fixed_font, W, H);
    return dlg.run();
}

//------------------------------------------------------------------------------
void flw::dlg::text(const std::string& title, const std::string& text, Fl_Window* parent, bool fixed_font, int W, int H) {
    _DlgText dlg(title.c_str(), text.c_str(), parent, fixed_font, W, H);
    dlg.run();
}

// MALAGMA_OFF
