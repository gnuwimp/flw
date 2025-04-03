// Copyright gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#ifndef FLW_DLG_H
#define FLW_DLG_H

#include "flw.h"
#include "date.h"
#include "scrollbrowser.h"
#include "gridgroup.h"

// MKALGAM_ON

#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Hold_Browser.H>
#include <FL/Fl_Hor_Fill_Slider.H>
#include <FL/Fl_Toggle_Button.H>

namespace flw {
namespace dlg {

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

extern const char*              PASSWORD_CANCEL;
extern const char*              PASSWORD_OK;

void                            center_message_dialog();
StringVector                    check(std::string title, const StringVector& list, Fl_Window* parent = nullptr);
int                             choice(std::string title, const StringVector& list, int selected = 0, Fl_Window* parent = nullptr);
bool                            font(Fl_Font& font, Fl_Fontsize& fontsize, std::string& fontname, bool limit_to_default = false);
void                            html(std::string title, const std::string& text, Fl_Window* parent = nullptr, int W = 40, int H = 23);
void                            list(std::string title, const StringVector& list, Fl_Window* parent = nullptr, bool fixed_font = false, int W = 40, int H = 23);
void                            list(std::string title, const std::string& list, Fl_Window* parent = nullptr, bool fixed_font = false, int W = 40, int H = 23);
void                            list_file(std::string title, std::string file, Fl_Window* parent = nullptr, bool fixed_font = false, int W = 40, int H = 23);
void                            panic(std::string message);
bool                            password(std::string title, std::string& password, Fl_Window* parent = nullptr);
bool                            password_check(std::string title, std::string& password, Fl_Window* parent = nullptr);
bool                            password_check_with_file(std::string title, std::string& password, std::string& file, Fl_Window* parent = nullptr);
bool                            password_with_file(std::string title, std::string& password, std::string& file, Fl_Window* parent = nullptr);
void                            print(std::string title, PrintCallback cb, void* data = nullptr, int from = 1, int to = 0, Fl_Window* parent = nullptr);
bool                            print_text(std::string title, const std::string& text, Fl_Window* parent = nullptr);
bool                            print_text(std::string title, const StringVector& text, Fl_Window* parent = nullptr);
int                             select(std::string title, const StringVector& list, int select_row, Fl_Window* parent = nullptr, bool fixed_font = false, int W = 40, int H = 23);
int                             select(std::string title, const StringVector& list, const std::string& select_row, Fl_Window* parent = nullptr, bool fixed_font = false, int W = 40, int H = 23);
bool                            slider(std::string title, double min, double max, double& value, double step = 1.0, Fl_Window* parent = nullptr);
void                            text(std::string title, const std::string& text, Fl_Window* parent = nullptr, int W = 40, int H = 23);
bool                            text_edit(std::string title, std::string& text, Fl_Window* parent = nullptr, int W = 40, int H = 23);
void                            theme(bool enable_font = false, bool enable_fixedfont = false, Fl_Window* parent = nullptr);

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
class AbortDialog : public Fl_Double_Window {
    using Fl_Double_Window::show;

public:
                                AbortDialog(const AbortDialog&) = delete;
                                AbortDialog(AbortDialog&&) = delete;
    AbortDialog&                operator=(const AbortDialog&) = delete;
    AbortDialog&                operator=(AbortDialog&&) = delete;

    explicit                    AbortDialog(std::string label = "", double min = 0.0, double max = 0.0);
    bool                        check(int milliseconds = 200);
    bool                        check(double value, double min, double max, int milliseconds = 200);
    bool                        aborted()
                                    { return _abort; }
    void                        range(double min, double max);
    void                        resize(int X, int Y, int W, int H) override
                                    { Fl_Double_Window::resize(X, Y, W, H); _grid->resize(0, 0, W, H); }
    void                        show(const std::string& label, Fl_Window* parent = nullptr);
    void                        value(double value);


    static void                 Callback(Fl_Widget* w, void* o);

private:
    Fl_Button*                  _button;
    Fl_Hor_Fill_Slider*         _progress;
    GridGroup*                  _grid;
    bool                        _abort;
    int64_t                     _last;
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
// Dialog for selecting font and font size.
// FontDialog::LoadFonts() will be called automatically (or do it manually).
// It is only needed once.
// Call FontDialog::DeleteFonts() before app exit (this is unnecessarily, only for keeping memory sanitizers satisfied).
//
class FontDialog : public Fl_Double_Window {
public:
                                FontDialog(const FontDialog&) = delete;
                                FontDialog(FontDialog&&) = delete;
    FontDialog&                 operator=(const FontDialog&) = delete;
    FontDialog&                 operator=(FontDialog&&) = delete;

                                FontDialog(Fl_Font font, Fl_Fontsize fontsize, const std::string& label, bool limit_to_default = false);
                                FontDialog(std::string font, Fl_Fontsize fontsize, std::string label, bool limit_to_default = false);
    void                        activate_font()
                                    { static_cast<Fl_Widget*>(_fonts)->activate(); }
    void                        deactivate_font()
                                    { static_cast<Fl_Widget*>(_fonts)->deactivate(); }
    void                        deactivate_fontsize()
                                    { static_cast<Fl_Widget*>(_sizes)->deactivate(); }
    int                         font()
                                    { return _font; }
    std::string                 fontname()
                                    { return _fontname; }
    int                         fontsize()
                                    { return _fontsize; }
    void                        resize(int X, int Y, int W, int H) override
                                    { Fl_Double_Window::resize(X, Y, W, H); _grid->resize(0, 0, W, H); }
    bool                        run(Fl_Window* parent = nullptr);

    static void                 Callback(Fl_Widget* w, void* o);

private:
    void                        _activate();
    void                        _create(Fl_Font font, std::string fontname, Fl_Fontsize fontsize, std::string label, bool limit_to_default);
    void                        _select_name(std::string font_name);

    Fl_Box*                     _label;
    Fl_Button*                  _cancel;
    Fl_Button*                  _select;
    GridGroup*                  _grid;
    ScrollBrowser*              _fonts;
    ScrollBrowser*              _sizes;
    bool                        _ret;
    int                         _font;
    int                         _fontsize;
    std::string                 _fontname;
};

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
class WorkDialog : public Fl_Double_Window {
public:
                                WorkDialog(const char* title, Fl_Window* parent, bool cancel, bool pause, int W = 40, int H = 10);
    void                        resize(int X, int Y, int W, int H) override
                                    { Fl_Double_Window::resize(X, Y, W, H); _grid->resize(0, 0, W, H); }
    bool                        run(double update_time, const StringVector& messages);
    bool                        run(double update_time, const std::string& message);

    static void                 Callback(Fl_Widget* w, void* o);

private:
    Fl_Button*                  _cancel;
    Fl_Hold_Browser*            _label;
    Fl_Toggle_Button*           _pause;
    GridGroup*                  _grid;
    bool                        _ret;
    double                      _last;
    std::string                 _message;
};

} // dlg
} // flw

// MKALGAM_OFF

#endif
