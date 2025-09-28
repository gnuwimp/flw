/**
* @file
* @brief Assorted dialog functions and classes.
*
* @author gnuwimp@gmail.com
* @copyright Released under the GNU General Public License v3.0
*/

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

/** @brief Assorted dialog functions and classes.
*/
namespace dlg {

/*
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
bool                            date(const std::string& title, gnu::Date& date, Fl_Window* parent = nullptr, int W = 33, int H = 21);
bool                            font(Fl_Font& font, Fl_Fontsize& fontsize, std::string& fontname, bool limit_to_default = false);
void                            html(const std::string& title, const std::string& text, Fl_Window* parent = nullptr, int W = 40, int H = 23);
void                            list(const std::string& title, const StringVector& list, bool fixed_font = false, Fl_Window* parent = nullptr, int W = 40, int H = 23);
void                            list(const std::string& title, const std::string& list, bool fixed_font = false, Fl_Window* parent = nullptr, int W = 40, int H = 23);
void                            list_file(const std::string& title, const std::string& file, bool fixed_font = false, Fl_Window* parent = nullptr, int W = 40, int H = 23);
void                            panic(const std::string& message);
bool                            password(const std::string& title, std::string& password, Fl_Window* parent = nullptr);
bool                            password_confirm(const std::string& title, std::string& password, Fl_Window* parent = nullptr);
bool                            password_confirm_and_file(const std::string& title, std::string& password, std::string& file, Fl_Window* parent = nullptr);
bool                            password_and_file(const std::string& title, std::string& password, std::string& file, Fl_Window* parent = nullptr);
void                            print(const std::string& title, PrintCallback cb, void* data = nullptr, int from = 1, int to = 0, Fl_Window* parent = nullptr);
bool                            print_text(const std::string& title, const std::string& text, Fl_Window* parent = nullptr);
bool                            print_text(const std::string& title, const StringVector& text, Fl_Window* parent = nullptr);
StringVector                    select_checkboxes(const std::string& title, const StringVector& list, Fl_Window* parent = nullptr);
int                             select_choice(const std::string& title, const StringVector& list, int selected = 0, Fl_Window* parent = nullptr);
int                             select_string(const std::string& title, const StringVector& list, int select_row, bool fixed_font = false, Fl_Window* parent = nullptr, int W = 40, int H = 23);
int                             select_string(const std::string& title, const StringVector& list, const std::string& select_row, bool fixed_font = false, Fl_Window* parent = nullptr, int W = 40, int H = 23);
bool                            slider(const std::string& title, double min, double max, double& value, double step = 1.0, Fl_Window* parent = nullptr);
void                            text(const std::string& title, const std::string& text, Fl_Window* parent = nullptr, int W = 40, int H = 23);
bool                            text_edit(const std::string& title, std::string& text, Fl_Window* parent = nullptr, int W = 40, int H = 23);
void                            theme(bool enable_font = false, bool enable_fixedfont = false, Fl_Window* parent = nullptr);

/**
 *      ______          _   
 *     |  ____|        | |  
 *     | |__ ___  _ __ | |_ 
 *     |  __/ _ \| '_ \| __|
 *     | | | (_) | | | | |_ 
 *     |_|  \___/|_| |_|\__|
 *                          
 *                          
 */

/** @brief A dialog for selecting font and font size.
*
* Font::LoadFonts() will be called automatically (or do it manually).\n
* It is only needed once.\n
* Call Font::DeleteFonts() before app exit (this is unnecessary, only for keeping memory sanitizers satisfied).\n
*
* @snippet dialog.cpp flw::dlg::Font example
* @image html font_dialog.png
*/
class Font : public Fl_Double_Window {
public:
                                Font(Fl_Font font, Fl_Fontsize fontsize, const std::string& title = "Select Font", bool limit_to_default = false);
                                Font(const std::string& font, Fl_Fontsize fontsize, const std::string& title = "Select Font", bool limit_to_default = false);
    void                        activate_font()
                                    { static_cast<Fl_Widget*>(_fonts)->activate(); } ///< @brief Turn on font list, active by default.
    void                        activate_font_size()
                                    { static_cast<Fl_Widget*>(_fonts)->activate(); } ///< @brief Turn on font size list, active by default.
    void                        deactivate_font()
                                    { static_cast<Fl_Widget*>(_fonts)->deactivate(); } ///< @brief Turn of font list.
    void                        deactivate_fontsize()
                                    { static_cast<Fl_Widget*>(_sizes)->deactivate(); } ///< @brief Turn of font size list.
    int                         font()
                                    { return _font; } ///< @brief Return selected font.
    std::string                 fontname()
                                    { return _fontname; } ///< @brief Return selected font name.
    int                         fontsize()
                                    { return _fontsize; } ///< @brief Return selected font size.
    bool                        run(Fl_Window* parent = nullptr);

private:
    void                        _activate();
    void                        _create(Fl_Font font, const std::string& fontname, Fl_Fontsize fontsize, const std::string& label, bool limit_to_default);
    void                        _select_name(const std::string& font_name);

    static void                 Callback(Fl_Widget* w, void* o);

    Fl_Box*                     _label;     // Sample text label.
    Fl_Button*                  _cancel;    // Cancel button.
    Fl_Button*                  _select;    // Select and close dialog button.
    GridGroup*                  _grid;      // Layout manager.
    ScrollBrowser*              _fonts;     // Font list.
    ScrollBrowser*              _sizes;     // Font size list.
    bool                        _ret;       // Return value.
    bool                        _run;       // Run flag.
    int                         _font;      // Selected font.
    int                         _fontsize;  // Selected font size.
    std::string                 _fontname;  // Selected font name.
};

/**
 *      _____
 *     |  __ \
 *     | |__) | __ ___   __ _ _ __ ___  ___ ___
 *     |  ___/ '__/ _ \ / _` | '__/ _ \/ __/ __|
 *     | |   | | | (_) | (_| | | |  __/\__ \__ \
 *     |_|   |_|  \___/ \__, |_|  \___||___/___/
 *                       __/ |
 *                      |___/
 */

/** @brief A dialog with a message list and a progress bar for time consuming work.
*
* Progress bar is optional.\n
* It has a pause button and an cancel button that can be disabled.\n
*
* @snippet dialog.cpp flw::dlg::Progress example
* @image html progress_dialog.png
*/
class Progress : public Fl_Double_Window {
public:
                                Progress(const std::string& title, bool cancel = false, bool pause = false, double min = 0.0, double max = 0.0);
    void                        range(double min, double max);
    void                        start(Fl_Window* parent = nullptr);
    bool                        update(const StringVector& messages, unsigned milli = 100);
    bool                        update(double value, const StringVector& messages, unsigned milli = 100);
    bool                        update(const std::string& message, unsigned milli = 100);
    bool                        update(double value, const std::string& message, unsigned milli = 100);
    double                      value() const
                                    { return _progress->value(); } ///< @brief Return progress bar value.
    void                        value(double value);

private:
    static void                 Callback(Fl_Widget* w, void* o);

    Fl_Button*                  _cancel;    // Cancel work button.
    Fl_Hold_Browser*            _label;     // Message label.
    Fl_Hor_Fill_Slider*         _progress;  // Optional pprogress bar.
    Fl_Toggle_Button*           _pause;     // Pause button.
    GridGroup*                  _grid;      // Layout manager.
    bool                        _ret;       // Return value from update().
    unsigned                    _last;      // Last refresh window time.
};

} // dlg
} // flw

// MKALGAM_OFF

#endif
