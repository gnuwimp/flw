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
        void _load_system();
        void _scrollbar();
    }

    namespace dlg {
        //----------------------------------------------------------------------
        void* zero_memory(void* mem, size_t size) {
            if (mem == nullptr || size == 0) return mem;
        #ifdef _WIN32
            RtlSecureZeroMemory(mem, size);
        #else
            auto p = (volatile unsigned char*) mem;

            while (size--) {
                *p = 0;
                p++;
            }
        #endif

            return mem;
        }

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
                size_range(flw::PREF_FONTSIZE * 24, flw::PREF_FONTSIZE * 12);
                set_modal();
                resizable(this);
                flw::util::center_window(this, parent);
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

                _html->resize(4, 4, W - 8, H - flw::PREF_FONTSIZE * 2 - 16);
                _close->resize(W - flw::PREF_FONTSIZE * 8 - 4, H - flw::PREF_FONTSIZE * 2 - 4, flw::PREF_FONTSIZE * 8, flw::PREF_FONTSIZE * 2);
            }

            //------------------------------------------------------------------
            void run() {
                show();

                while (visible() != 0) {
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
            flw::ScrollBrowser*         _list;
            Fl_Return_Button*           _close;

        public:
            //------------------------------------------------------------------
            _DlgList(const char* title, const StringVector& list, std::string file, Fl_Window* parent = nullptr, bool fixed_font = false, int W = 50, int H = 20) :
            Fl_Double_Window(0, 0, (fixed_font ? flw::PREF_FIXED_FONTSIZE : flw::PREF_FONTSIZE) * W, (fixed_font ? flw::PREF_FIXED_FONTSIZE : flw::PREF_FONTSIZE) * H) {
                end();

                _close = new Fl_Return_Button(0, 0, 0, 0, "&Close");
                _list  = new flw::ScrollBrowser();

                add(_close);
                add(_list);

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
                flw::util::center_window(this, parent);

                if (list.size() > 0) {
                    for (auto& s : list) {
                        _list->add(s.c_str());
                    }
                }
                else if (file != "") {
                    _list->load(file.c_str());
                }
            }

            //------------------------------------------------------------------
            static void Callback(Fl_Widget* w, void* o) {
                auto dlg = (_DlgList*) o;

                if (w == dlg || w == dlg->_close) {
                    dlg->hide();
                }
            }

            //------------------------------------------------------------------
            void resize(int X, int Y, int W, int H) override {
                Fl_Double_Window::resize(X, Y, W, H);

                _list->resize(4, 4, W - 8, H - flw::PREF_FONTSIZE * 2 - 16);
                _close->resize(W - flw::PREF_FONTSIZE * 8 - 4, H - flw::PREF_FONTSIZE * 2 - 4, flw::PREF_FONTSIZE * 8, flw::PREF_FONTSIZE * 2);
            }

            //------------------------------------------------------------------
            void run() {
                show();

                while (visible() != 0) {
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
            Fl_Button*                  _close;
            Fl_Button*                  _cancel;
            ScrollBrowser*              _list;
            Fl_Input*                   _filter;
            const StringVector&         _strings;

        public:
            //------------------------------------------------------------------
            _DlgSelect(const char* title, Fl_Window* parent, const StringVector& strings, int selected_string_index, std::string selected_string, bool fixed_font, int W, int H) :
            Fl_Double_Window(0, 0, ((fixed_font == true) ? flw::PREF_FIXED_FONTSIZE : flw::PREF_FONTSIZE) * W, ((fixed_font == true) ? flw::PREF_FIXED_FONTSIZE : flw::PREF_FONTSIZE) * H),
            _strings(strings) {
                end();

                _filter = new Fl_Input(0, 0, 0, 0);
                _close  = new Fl_Return_Button(0, 0, 0, 0, "&Select");
                _cancel = new Fl_Button(0, 0, 0, 0, "&Cancel");
                _list   = new flw::ScrollBrowser(0, 0, 0, 0);

                add(_filter);
                add(_list);
                add(_cancel);
                add(_close);

                _cancel->callback(_DlgSelect::Callback, this);
                _close->callback(_DlgSelect::Callback, this);
                _filter->callback(_DlgSelect::Callback, this);
                _filter->tooltip("Enter text to filter rows that macthes the text\nPress tab to switch focus between input and list widget.");
                _filter->when(FL_WHEN_CHANGED);
                _list->callback(_DlgSelect::Callback, this);
                _list->tooltip("Use Page Up or Page Down in list to scroll faster");

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

                    for (auto& string : _strings) {
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
                flw::util::center_window(this, parent);
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

            //------------------------------------------------------------------
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

            //------------------------------------------------------------------
            void resize(int X, int Y, int W, int H) override {
                Fl_Double_Window::resize(X, Y, W, H);

                _filter->resize(4, 4, W - 8, flw::PREF_FONTSIZE * 2);
                _list->resize(4, flw::PREF_FONTSIZE * 2  + 8, W - 8, H - flw::PREF_FONTSIZE * 4 - 20);
                _cancel->resize(W - flw::PREF_FONTSIZE * 16 - 8, H - flw::PREF_FONTSIZE * 2 - 4, flw::PREF_FONTSIZE * 8, flw::PREF_FONTSIZE * 2);
                _close->resize(W - flw::PREF_FONTSIZE * 8 - 4, H - flw::PREF_FONTSIZE * 2 - 4, flw::PREF_FONTSIZE * 8, flw::PREF_FONTSIZE * 2);
            }

            //------------------------------------------------------------------
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

        const char* PASSWORD_CANCEL = "Cancel";
        const char* PASSWORD_OK     = "Ok";

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
                _cancel    = new Fl_Button(0, 0, 0, 0, flw::dlg::PASSWORD_CANCEL);
                _close     = new Fl_Return_Button(0, 0, 0, 0, flw::dlg::PASSWORD_OK);
                _file      = new Fl_Output(0, 0, 0, 0, "Key File");
                _mode      = mode;
                _ret       = false;

                add(_password1);
                add(_password2);
                add(_file);
                add(_browse);
                add(_cancel);
                add(_close);

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

                auto W = 40 * flw::PREF_FONTSIZE;
                auto H = 11 * flw::PREF_FONTSIZE + 28;

                if (_mode == _DlgPassword::TYPE::ASK_PASSWORD) {
                    _password2->hide();
                    _browse->hide();
                    _file->hide();
                    W = 30 * flw::PREF_FONTSIZE;
                    H = 5 * flw::PREF_FONTSIZE + 16;
                }
                else if (_mode == _DlgPassword::TYPE::CONFIRM_PASSWORD) {
                    _browse->hide();
                    _file->hide();
                    W = 30 * flw::PREF_FONTSIZE;
                    H = 8 * flw::PREF_FONTSIZE + 24;
                }
                else if (_mode == _DlgPassword::TYPE::ASK_PASSWORD_AND_KEYFILE) {
                    _password2->hide();
                    W = 40 * flw::PREF_FONTSIZE;
                    H = 8 * flw::PREF_FONTSIZE + 24;
                }

                util::labelfont(this);
                callback(_DlgPassword::Callback, this);
                label(title);
                size(W, H);
                size_range(W, H);
                set_modal();
                resizable(this);
                flw::util::center_window(this, parent);
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
                    _file->resize(4, flw::PREF_FONTSIZE * 4 + 12, W - flw::PREF_FONTSIZE * 8 - 12, flw::PREF_FONTSIZE * 2);
                    _browse->resize(W - flw::PREF_FONTSIZE * 8 - 4, flw::PREF_FONTSIZE * 4 + 12, flw::PREF_FONTSIZE * 8, flw::PREF_FONTSIZE * 2);
                }
                else if (_mode == _DlgPassword::TYPE::CONFIRM_PASSWORD_AND_KEYFILE) {
                    _password1->resize(4, flw::PREF_FONTSIZE + 4, W - 8, flw::PREF_FONTSIZE * 2);
                    _password2->resize(4, flw::PREF_FONTSIZE * 4 + 12, W - 8, flw::PREF_FONTSIZE * 2);
                    _file->resize(4, flw::PREF_FONTSIZE * 7 + 16, W - flw::PREF_FONTSIZE * 8 - 12, flw::PREF_FONTSIZE * 2);
                    _browse->resize(W - flw::PREF_FONTSIZE * 8 - 4, flw::PREF_FONTSIZE * 7 + 16, flw::PREF_FONTSIZE * 8, flw::PREF_FONTSIZE * 2);
                }

                _cancel->resize(W - flw::PREF_FONTSIZE * 16 - 8, H - flw::PREF_FONTSIZE * 2 - 4, flw::PREF_FONTSIZE * 8, flw::PREF_FONTSIZE * 2);
                _close->resize(W - flw::PREF_FONTSIZE * 8 - 4, H - flw::PREF_FONTSIZE * 2 - 4, flw::PREF_FONTSIZE * 8, flw::PREF_FONTSIZE * 2);
            }

            //------------------------------------------------------------------
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

                dlg::zero_memory((void*) _password1->value(), strlen(_password1->value()));
                dlg::zero_memory((void*) _password2->value(), strlen(_password2->value()));
                dlg::zero_memory((void*) _file->value(), strlen(_file->value()));

                return _ret;
            }
        };

        //----------------------------------------------------------------------
        //----------------------------------------------------------------------
        //----------------------------------------------------------------------

        //----------------------------------------------------------------------
        class _DlgText : public Fl_Double_Window {
            Fl_Button*                  _cancel;
            Fl_Button*                  _close;
            Fl_Button*                  _save;
            Fl_Text_Buffer*             _buffer;
            Fl_Text_Display*            _text;
            bool                        _edit;
            char*                       _res;

        public:
            //------------------------------------------------------------------
            _DlgText(const char* title, const char* text, bool edit, Fl_Window* parent, int W, int H) :
            Fl_Double_Window(0, 0, flw::PREF_FONTSIZE * W, flw::PREF_FONTSIZE * H) {
                end();

                _cancel = new Fl_Button(0, 0, 0, 0, "C&ancel");
                _close  = (edit == false) ? new Fl_Return_Button(0, 0, 0, 0, "&Close") : new Fl_Button(0, 0, 0, 0, "&Close");
                _save   = new Fl_Button(0, 0, 0, 0, "&Save");
                _text   = (edit == false) ? new Fl_Text_Display(0, 0, 0, 0) : new Fl_Text_Editor(0, 0, 0, 0);
                _buffer = new Fl_Text_Buffer();
                _edit   = edit;
                _res    = nullptr;

                add(_save);
                add(_cancel);
                add(_close);
                add(_text);

                _buffer->text(text);
                _cancel->callback(_DlgText::Callback, this);
                _cancel->tooltip("Close and abort all changes.");
                _close->callback(_DlgText::Callback, this);
                _close->tooltip("Close and update text.");
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
                flw::util::center_window(this, parent);
            }

            //------------------------------------------------------------------
            ~_DlgText() {
               _text->buffer(nullptr);
                delete _buffer;
            }

            //------------------------------------------------------------------
            static void Callback(Fl_Widget* w, void* o) {
                auto dlg = (_DlgText*) o;

                if (w == dlg || w == dlg->_cancel) {
                    dlg->hide();
                }
                else if (w == dlg->_save) {
                    auto filename = fl_file_chooser("Select Destination File", nullptr, nullptr, 0);

                    if (filename != nullptr && dlg->_buffer->savefile(filename) != 0) {
                        fl_alert("error: failed to save text to %s", filename);
                    }
                }
                else if (w == dlg->_close) {
                    if (dlg->_edit == true) {
                        dlg->_res = dlg->_buffer->text();
                    }

                    dlg->hide();
                }
            }

            //------------------------------------------------------------------
            void resize(int X, int Y, int W, int H) override {
                Fl_Double_Window::resize(X, Y, W, H);

                _text->resize(4, 4, W - 8, H - flw::PREF_FONTSIZE * 2 - 16);

                if (_cancel->visible() != 0) {
                    _save->resize(W - flw::PREF_FONTSIZE * 24 - 12, H - flw::PREF_FONTSIZE * 2 - 4, flw::PREF_FONTSIZE * 8, flw::PREF_FONTSIZE * 2);
                    _cancel->resize(W - flw::PREF_FONTSIZE * 16 - 8, H - flw::PREF_FONTSIZE * 2 - 4, flw::PREF_FONTSIZE * 8, flw::PREF_FONTSIZE * 2);
                }
                else {
                    _save->resize(W - flw::PREF_FONTSIZE * 16 - 8, H - flw::PREF_FONTSIZE * 2 - 4, flw::PREF_FONTSIZE * 8, flw::PREF_FONTSIZE * 2);
                }

                _close->resize(W - flw::PREF_FONTSIZE * 8 - 4, H - flw::PREF_FONTSIZE * 2 - 4, flw::PREF_FONTSIZE * 8, flw::PREF_FONTSIZE * 2);
            }

            //------------------------------------------------------------------
            char* run() {
                show();

                while (visible() != 0) {
                    Fl::wait();
                    Fl::flush();
                }

                return _res;
            }
        };

        //----------------------------------------------------------------------
        //----------------------------------------------------------------------
        //----------------------------------------------------------------------

        //----------------------------------------------------------------------
        class _DlgTheme : public Fl_Double_Window {
            Fl_Box*                     _font_label;
            Fl_Box*                     _fixedfont_label;
            Fl_Browser*                 _theme;
            Fl_Button*                  _close;
            Fl_Button*                  _font;
            Fl_Button*                  _fixedfont;
            int                         _theme_row;

        public:
            //------------------------------------------------------------------
            _DlgTheme(bool enable_font, bool enable_fixedfont, Fl_Window* parent) : Fl_Double_Window(0, 0, 0, 0, "Set Theme") {
                end();

                _close           = new Fl_Return_Button(0, 0, 0, 0, "&Close");
                _fixedfont       = new Fl_Button(0, 0, 0, 0, "F&ixed Font");
                _fixedfont_label = new Fl_Box(0, 0, 0, 0);
                _font            = new Fl_Button(0, 0, 0, 0, "&Font");
                _font_label      = new Fl_Box(0, 0, 0, 0);
                _theme           = new Fl_Hold_Browser(0, 0, 0, 0);
                _theme_row       = 0;

                add(_theme);
                add(_fixedfont);
                add(_font);
                add(_close);
                add(_fixedfont_label);
                add(_font_label);

                if (enable_font == false) {
                  _font->deactivate();
                }

                if (enable_fixedfont == false) {
                  _fixedfont->deactivate();
                }

                _close->callback(Callback, this);
                _font_label->color(FL_BACKGROUND2_COLOR);
                _font_label->box(FL_DOWN_BOX);
                _font->callback(Callback, this);
                _font_label->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
                _fixedfont_label->color(FL_BACKGROUND2_COLOR);
                _fixedfont_label->box(FL_DOWN_BOX);
                _fixedfont->callback(Callback, this);
                _fixedfont_label->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
                _theme->callback(Callback, this);
                _theme->textfont(flw::PREF_FONT);

                for (size_t f = 0; f < 100; f++) {
                    auto theme = flw::PREF_THEMES[f];
                    if (theme != nullptr) _theme->add(theme);
                    else break;
                }

                resizable(this);
                callback(Callback, this);
                set_modal();
                update_pref();
                util::center_window(this, parent);
            }

            //------------------------------------------------------------------
            static void Callback(Fl_Widget* w, void* o) {
                auto dlg = (_DlgTheme*) o;

                if (w == dlg) {
                    dlg->hide();
                }
                else if (w == dlg->_fixedfont) {
                    flw::dlg::FontDialog fd(flw::PREF_FIXED_FONT, flw::PREF_FIXED_FONTSIZE, "Select Monospaced Font");

                    if (fd.run(Fl::first_window()) == true) {
                        flw::PREF_FIXED_FONT     = fd.font();
                        flw::PREF_FIXED_FONTSIZE = fd.fontsize();
                        flw::PREF_FIXED_FONTNAME = fd.fontname();

                        if (dlg->_font->active() == 0) {
                            flw::PREF_FONTSIZE = flw::PREF_FIXED_FONTSIZE;
                        }

                        dlg->update_pref();
                    }
                }
                else if (w == dlg->_font) {
                    flw::dlg::FontDialog fd(flw::PREF_FONT, flw::PREF_FONTSIZE, "Select Font");

                    if (fd.run(Fl::first_window()) == true) {
                        flw::PREF_FONT     = fd.font();
                        flw::PREF_FONTSIZE = fd.fontsize();
                        flw::PREF_FONTNAME = fd.fontname();

                        if (dlg->_fixedfont->active() == 0) {
                            flw::PREF_FIXED_FONTSIZE = flw::PREF_FONTSIZE;
                        }

                        dlg->update_pref();
                    }
                }
                else if (w == dlg->_theme) {
                    auto row = dlg->_theme->value() - 1;

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
                    else if (row == theme::THEME_SYSTEM) {
                        theme::_load_system();
                    }
                    else {
                        theme::_load_default();
                    }

                    dlg->update_pref();
                }
                else if (w == dlg->_close) {
                    dlg->hide();
                }
            }

            //------------------------------------------------------------------
            void resize(int X, int Y, int W, int H) override {
                auto fs = flw::PREF_FONTSIZE;

                Fl_Double_Window::resize(X, Y, W, H);

                _theme->resize           (4,                 4,                  W - 8,     H - fs * 6 - 24);
                _font_label->resize      (4,                 H - fs * 6 - 16,    W - 8,     fs * 2);
                _fixedfont_label->resize (4,                 H - fs * 4 - 12,    W - 8,     fs * 2);
                _font->resize            (W - fs * 24 - 12,  H - fs * 2 - 4,     fs * 8,    fs * 2);
                _fixedfont->resize       (W - fs * 16 - 8,   H - fs * 2 - 4,     fs * 8,    fs * 2);
                _close->resize           (W - fs * 8 - 4,    H - fs * 2 - 4,     fs * 8,    fs * 2);
            }

            //------------------------------------------------------------------
            void run() {
                show();

                while (visible()) {
                    Fl::wait();
                    Fl::flush();
                }
            }

            //------------------------------------------------------------------
            void update_pref() {
                util::labelfont(this);
                _font_label->copy_label(flw::util::format("%s - %d", flw::PREF_FONTNAME.c_str(), flw::PREF_FONTSIZE).c_str());
                _fixedfont_label->copy_label(flw::util::format("%s - %d", flw::PREF_FIXED_FONTNAME.c_str(), flw::PREF_FIXED_FONTSIZE).c_str());
                _fixedfont_label->labelfont(flw::PREF_FIXED_FONT);
                _fixedfont_label->labelsize(flw::PREF_FIXED_FONTSIZE);
                _theme->textsize(flw::PREF_FONTSIZE);
                size(flw::PREF_FONTSIZE * 32, flw::PREF_FONTSIZE * 30);
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
    }
}

//------------------------------------------------------------------------------
void flw::dlg::html(std::string title, const std::string& text, Fl_Window* parent, int W, int H) {
    _DlgHtml dlg(title.c_str(), text.c_str(), parent, W, H);
    dlg.run();
}

//------------------------------------------------------------------------------
void flw::dlg::list(std::string title, const StringVector& list, Fl_Window* parent, bool fixed_font, int W, int H) {
    _DlgList dlg(title.c_str(), list, "", parent, fixed_font, W, H);
    dlg.run();
}

//------------------------------------------------------------------------------
void flw::dlg::list(std::string title, const std::string& list, Fl_Window* parent, bool fixed_font, int W, int H) {
    auto list2 = flw::util::split( list, "\n");
    _DlgList dlg(title.c_str(), list2, "", parent, fixed_font, W, H);
    dlg.run();
}

//------------------------------------------------------------------------------
void flw::dlg::list_file(std::string title, std::string file, Fl_Window* parent, bool fixed_font, int W, int H) {
    _DlgList dlg(title.c_str(), flw::StringVector(), file, parent, fixed_font, W, H);
    dlg.run();
}

//------------------------------------------------------------------------------
void flw::dlg::panic(std::string message) {
    fl_alert("panic! I have to quit\n%s", message.c_str());
    exit(1);
}

//------------------------------------------------------------------------------
bool flw::dlg::password1(std::string title, std::string& password, Fl_Window* parent) {
    std::string file;
    _DlgPassword dlg(title.c_str(), parent, _DlgPassword::TYPE::ASK_PASSWORD);
    return dlg.run(password, file);
}

//------------------------------------------------------------------------------
bool flw::dlg::password2(std::string title, std::string& password, Fl_Window* parent) {
    std::string file;
    _DlgPassword dlg(title.c_str(), parent, _DlgPassword::TYPE::CONFIRM_PASSWORD);
    return dlg.run(password, file);
}

//------------------------------------------------------------------------------
bool flw::dlg::password3(std::string title, std::string& password, std::string& file, Fl_Window* parent) {
    _DlgPassword dlg(title.c_str(), parent, _DlgPassword::TYPE::ASK_PASSWORD_AND_KEYFILE);
    return dlg.run(password, file);
}

//------------------------------------------------------------------------------
bool flw::dlg::password4(std::string title, std::string& password, std::string& file, Fl_Window* parent) {
    _DlgPassword dlg(title.c_str(), parent, _DlgPassword::TYPE::CONFIRM_PASSWORD_AND_KEYFILE);
    return dlg.run(password, file);
}

//------------------------------------------------------------------------------
int flw::dlg::select(std::string title, const StringVector& list, int selected_row, Fl_Window* parent, bool fixed_font, int W, int H) {
    _DlgSelect dlg(title.c_str(), parent, list, selected_row, "", fixed_font, W, H);
    return dlg.run();
}

//------------------------------------------------------------------------------
int flw::dlg::select(std::string title, const StringVector& list, const std::string& selected_row, Fl_Window* parent, bool fixed_font, int W, int H) {
    _DlgSelect dlg(title.c_str(), parent, list, 0, selected_row, fixed_font, W, H);
    return dlg.run();
}

//------------------------------------------------------------------------------
void flw::dlg::text(std::string title, const std::string& text, Fl_Window* parent, int W, int H) {
    _DlgText dlg(title.c_str(), text.c_str(), false, parent, W, H);
    dlg.run();
}

//------------------------------------------------------------------------------
bool flw::dlg::text_edit(std::string title, std::string& text, Fl_Window* parent, int W, int H) {
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
void flw::dlg::theme(bool enable_font, bool enable_fixedfont, Fl_Window* parent) {
    auto dlg = dlg::_DlgTheme(enable_font, enable_fixedfont, parent);
    dlg.run();
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// To disable progress set min and max to 0.0
//
flw::dlg::AbortDialog::AbortDialog(double min, double max) : Fl_Double_Window(0, 0, 0, 0, "Working...") {
    _button   = new Fl_Button(0, 0, 0, 0, "Press To Abort");
    _progress = new Fl_Hor_Fill_Slider(0, 0, 0, 0);
    _abort    = false;
    _last     = 0;

    add(_button);
    add(_progress);

    auto W = flw::PREF_FONTSIZE * 32;
    auto H = 0;

    if (min < max && fabs(max - min) > 0.001) {
        _progress->range(min, max);
        _progress->value(min);
        H = flw::PREF_FONTSIZE * 6 + 12;
    }
    else {
        _progress->hide();
        H = flw::PREF_FONTSIZE * 4 + 8;
    }

    _button->callback(AbortDialog::Callback, this);
    _button->labelfont(flw::PREF_FONT);
    _button->labelsize(flw::PREF_FONTSIZE);
    _progress->color(FL_SELECTION_COLOR);

    size(W, H);
    size_range(W, H);
    callback(AbortDialog::Callback, this);
    set_modal();
}

//------------------------------------------------------------------------------
void flw::dlg::AbortDialog::Callback(Fl_Widget* w, void* o) {
    auto dlg = (AbortDialog*) o;

    if (w == dlg->_button) {
        dlg->_abort = true;
    }
}

//------------------------------------------------------------------------------
bool flw::dlg::AbortDialog::check(int milliseconds) {
    auto now = flw::util::milliseconds();

    if (now - _last > milliseconds) {
        _last = now;
        Fl::check();
    }

    return _abort;
}

//------------------------------------------------------------------------------
bool flw::dlg::AbortDialog::check(double value, double min, double max, int milliseconds) {
    auto now = flw::util::milliseconds();

    if (now - _last > milliseconds) {
        _progress->value(value);
        _progress->range(min, max);
        _last = now;
        Fl::check();
    }

    return _abort;
}

//------------------------------------------------------------------------------
void flw::dlg::AbortDialog::range(double min, double max) {
    _progress->range(min, max);
}

//------------------------------------------------------------------
void flw::dlg::AbortDialog::resize(int X, int Y, int W, int H) {
    Fl_Double_Window::resize(X, Y, W, H);

    if (_progress->visible() != 0) {
        _button->resize(4, 4, W - 8, H - flw::PREF_FONTSIZE * 2 - 12);
        _progress->resize(4, H - flw::PREF_FONTSIZE * 2 - 4, W - 8, flw::PREF_FONTSIZE * 2);
    }
    else {
        _button->resize(4, 4, W - 8, H - 8);
    }
}

//------------------------------------------------------------------------------
void flw::dlg::AbortDialog::show(const std::string& label, Fl_Window* parent) {
    _abort = false;
    _last  = 0;

    _button->copy_label(label.c_str());
    flw::util::center_window(this, parent);
    Fl_Double_Window::show();
    Fl::flush();
}

//------------------------------------------------------------------------------
void flw::dlg::AbortDialog::value(double value) {
    _progress->value(value);
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

namespace flw {
    namespace dlg {
        static const std::string    _FONTDIALOG_LABEL = R"(
ABCDEFGHIJKLMNOPQRSTUVWXYZ /0123456789
abcdefghijklmnopqrstuvwxyz £©µÀÆÖÞßéöÿ
–—‘“”„†•…‰™œŠŸž€ ΑΒΓΔΩαβγδω АБВГДабвгд
∀∂∈ℝ∧∪≡∞ ↑↗↨↻⇣ ┐┼╔╘░►☺♀ ﬁ�⑀₂ἠḂӥẄɐː⍎אԱა

Hello world, Καλημέρα κόσμε, コンニチハ

math: ∮ E⋅da = Q,  n → ∞, ∑ f(i) 2H₂ + O₂ ⇌ 2H₂O, R = 4.7 kΩ
korean: 구두의
greek: Οὐχὶ ταὐτὰ παρίσταταί μοι γιγνώσκειν
russian: Зарегистрируйтесь сейчас
thai: แผ่นดินฮั่นเสื่อมโทรมแสนสังเวช
amharic: ሰማይ አይታረስ ንጉሥ አይከሰስ
braille: ⡌⠁⠧⠑ ⠼⠁⠒  ⡍⠜⠇⠑⠹⠰⠎ ⡣⠕⠌

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

        //----------------------------------------------------------------------
        class _FontDialogLabel : public Fl_Box {
        public:
            int font;
            int size;

            //------------------------------------------------------------------
            _FontDialogLabel(int x, int y, int w, int h) : Fl_Box(x, y, w, h, _FONTDIALOG_LABEL.c_str()) {
                font = FL_HELVETICA;
                size = 14;

                align(FL_ALIGN_TOP | FL_ALIGN_LEFT | FL_ALIGN_INSIDE | FL_ALIGN_CLIP);
                box(FL_BORDER_BOX);
                color(FL_BACKGROUND2_COLOR);
            }

            //------------------------------------------------------------------
            void draw() override {
                draw_box();
                fl_font((Fl_Font) font, size);
                fl_color(FL_FOREGROUND_COLOR);
                fl_draw(label(), x() + 3, y() + 3, w() - 6, h() - 6, align());
            }
        };
    }
}

//------------------------------------------------------------------------------
flw::dlg::FontDialog::FontDialog(Fl_Font font, Fl_Fontsize fontsize, const std::string& label) :
Fl_Double_Window(0, 0, flw::PREF_FONTSIZE * 60, flw::PREF_FONTSIZE * 36) {
    _create(font, "", fontsize, label);
}

//------------------------------------------------------------------------------
flw::dlg::FontDialog::FontDialog(std::string font, Fl_Fontsize fontsize, std::string label) :
Fl_Double_Window(0, 0, flw::PREF_FONTSIZE * 60, flw::PREF_FONTSIZE * 36) {
    _create(0, font, fontsize, label);
}

//------------------------------------------------------------------------------
void flw::dlg::FontDialog::_activate() {
    if (_fonts->value() == 0 || _sizes->value() == 0) {
        _select->deactivate();
    }
    else {
        _select->activate();
    }
}

//------------------------------------------------------------------------------
void flw::dlg::FontDialog::Callback(Fl_Widget* w, void* o) {
    FontDialog* dlg = (FontDialog*) o;

    if (w == dlg) {
        dlg->hide();
    }
    else if (w == dlg->_cancel) {
        dlg->hide();
    }
    else if (w == dlg->_fonts) {
        auto row = dlg->_fonts->value();

        if (row > 0) {
            ((_FontDialogLabel*) dlg->_label)->font = row - 1;
        }

        dlg->_activate();
        Fl::redraw();
    }
    else if (w == dlg->_select) {
        auto row1 = dlg->_fonts->value();
        auto row2 = dlg->_sizes->value();

        if (row1 > 0 && row2 > 0) {
            row1--;

            dlg->_fontname = util::remove_browser_format(flw::PREF_FONTNAMES[row1]);
            dlg->_font     = row1;
            dlg->_fontsize = row2 + 5;
            dlg->_ret      = true;

            dlg->hide();
        }
    }
    else if (w == dlg->_sizes) {
        auto row = dlg->_sizes->value();

        if (row > 0) {
            ((_FontDialogLabel*) dlg->_label)->size = row + 5;
        }

        dlg->_activate();
        Fl::redraw();
    }
}

//------------------------------------------------------------------------------
void flw::dlg::FontDialog::_create(Fl_Font font, std::string fontname, Fl_Fontsize fontsize, std::string label) {
    end();

    _cancel   = new Fl_Button(0, 0, 0, 0, "&Cancel");
    _fonts    = new flw::ScrollBrowser(12);
    _label    = new flw::dlg::_FontDialogLabel(0, 0, 0, 0);
    _select   = new Fl_Button(0, 0, 0, 0, "&Select");
    _sizes    = new flw::ScrollBrowser(6);
    _font     = -1;
    _fontsize = -1;
    _ret      = false;

    add(_sizes);
    add(_fonts);
    add(_select);
    add(_cancel);
    add(_label);

    _cancel->callback(flw::dlg::FontDialog::Callback, this);
    _cancel->labelfont(flw::PREF_FONT);
    _cancel->labelsize(flw::PREF_FONTSIZE);
    _fonts->callback(flw::dlg::FontDialog::Callback, this);
    _fonts->textsize(flw::PREF_FONTSIZE);
    _fonts->when(FL_WHEN_CHANGED);
    ((_FontDialogLabel*) _label)->font = font;
    ((_FontDialogLabel*) _label)->size = fontsize;
    _select->callback(flw::dlg::FontDialog::Callback, this);
    _select->labelfont(flw::PREF_FONT);
    _select->labelsize(flw::PREF_FONTSIZE);
    _sizes->callback(flw::dlg::FontDialog::Callback, this);
    _sizes->textsize(flw::PREF_FONTSIZE);
    _sizes->when(FL_WHEN_CHANGED);

    size(w(), h());
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
        ((_FontDialogLabel*) _label)->font = fontsize;
    }
    else {
        _sizes->value(14 - 5);
        _sizes->middleline(14 - 5);
        ((_FontDialogLabel*) _label)->font = 14;
    }

    if (fontname != "") {
        _select_name(fontname);
    }
    else if (font >= 0 && font < _fonts->size()) {
        _fonts->value(font + 1);
        _fonts->middleline(font + 1);
        ((_FontDialogLabel*) _label)->font = font;
    }
    else {
        _fonts->value(1);
        _fonts->middleline(1);
    }

    resizable(this);
    copy_label(label.c_str());
    callback(flw::dlg::FontDialog::Callback, this);
    size_range(flw::PREF_FONTSIZE * 38, flw::PREF_FONTSIZE * 12);
    set_modal();
    _fonts->take_focus();
}

//------------------------------------------------------------------------------
void flw::dlg::FontDialog::resize(int X, int Y, int W, int H) {
    int fs = flw::PREF_FONTSIZE;

    Fl_Double_Window::resize(X, Y, W, H);

    _fonts->resize  (4,                 4,                  fs * 25,            H - fs * 2 - 16);
    _sizes->resize  (fs * 25 + 8,       4,                  fs * 6,             H - fs * 2 - 16);
    _label->resize  (fs * 31 + 12,      4,                  W - fs * 31 - 16,   H - fs * 2 - 16);
    _cancel->resize (W - fs * 16 - 8,   H - fs * 2 - 4,     fs * 8,             fs * 2);
    _select->resize (W - fs * 8 - 4,    H - fs * 2 - 4,     fs * 8,             fs * 2);
}

//------------------------------------------------------------------------------
bool flw::dlg::FontDialog::run(Fl_Window* parent) {
    _ret = false;

    _activate();
    flw::util::center_window(this, parent);
    show();

    while (visible() != 0) {
        Fl::wait();
        Fl::flush();
    }

    return _ret;
}

//------------------------------------------------------------------------------
void flw::dlg::FontDialog::_select_name(std::string fontname) {
    auto count = 1;

    for (auto font : flw::PREF_FONTNAMES) {
        auto font_without_style = util::remove_browser_format(font);

        if (fontname == font_without_style) {
            _fonts->value(count);
            _fonts->middleline(count);
            ((_FontDialogLabel*) _label)->font = count - 1;
            return;
        }

        count++;
    }

    _fonts->value(1);
    ((_FontDialogLabel*) _label)->font = 0;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
flw::dlg::WorkDialog::WorkDialog(const char* title, Fl_Window* parent, bool cancel, bool pause, int W, int H) : Fl_Double_Window(0, 0, W * flw::PREF_FONTSIZE, H * flw::PREF_FONTSIZE) {
    end();

    _cancel = new Fl_Button(0, 0, 0, 0, "Cancel");
    _pause  = new Fl_Toggle_Button(0, 0, 0, 0, "Pause");
    _label  = new Fl_Hold_Browser(0, 0, 0, 0);
    _ret    = true;
    _last   = 0.0;

    add(_label);
    add(_pause);
    add(_cancel);

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
    flw::util::center_window(this, parent);
    show();
}

//------------------------------------------------------------------------------
void flw::dlg::WorkDialog::Callback(Fl_Widget* w, void* o) {
    auto dlg = (flw::dlg::WorkDialog*) o;

    if (w == dlg) {
    }
    else if (w == dlg->_cancel) {
        dlg->_ret = false;
    }
    else if (w == dlg->_pause) {
        bool cancel = dlg->_cancel->active();
        dlg->_cancel->deactivate();
        dlg->_pause->label("C&ontinue");

        while (dlg->_pause->value() != 0) {
            flw::util::sleep(10);
            Fl::check();
        }

        dlg->_pause->label("&Pause");

        if (cancel) {
            dlg->_cancel->activate();
        }
    }
}

//------------------------------------------------------------------------------
void flw::dlg::WorkDialog::resize(int X, int Y, int W, int H) {
    Fl_Double_Window::resize(X, Y, W, H);

    _label->resize(4, 4, W - 8, H - flw::PREF_FONTSIZE * 2 - 16);
    _pause->resize(W - flw::PREF_FONTSIZE * 16 - 8, H - flw::PREF_FONTSIZE * 2 - 4, flw::PREF_FONTSIZE * 8, flw::PREF_FONTSIZE * 2);
    _cancel->resize(W - flw::PREF_FONTSIZE * 8 - 4, H - flw::PREF_FONTSIZE * 2 - 4, flw::PREF_FONTSIZE * 8, flw::PREF_FONTSIZE * 2);
}

//------------------------------------------------------------------------------
bool flw::dlg::WorkDialog::run(double update_time, const StringVector& messages) {
    auto now = flw::util::clock();

    if (now - _last > update_time) {
        _label->clear();

        for (auto& s : messages) {
            _label->add(s.c_str());
        }

        _last = now;
        Fl::check();
        Fl::flush();
    }

    return _ret;
}

//------------------------------------------------------------------------------
bool flw::dlg::WorkDialog::run(double update_time, const std::string& message) {
    auto now = flw::util::clock();

    if (now - _last > update_time) {
        _label->clear();
        _label->add(message.c_str());
        _last = now;
        Fl::check();
        Fl::flush();
    }

    return _ret;
}

// MKALGAM_OFF
