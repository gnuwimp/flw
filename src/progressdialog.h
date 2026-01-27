/**
* @file
* @brief Progress message dialog.
*
* @author gnuwimp@gmail.com
* @copyright Released under the GNU General Public License v3.0
*/

#ifndef FLW_PROGRESSDIALOG_H
#define FLW_PROGRESSDIALOG_H

#include "flw.h"

// MKALGAM_ON

#include <FL/Fl_Double_Window.H>

class Fl_Button;
class Fl_Hold_Browser;
class Fl_Hor_Fill_Slider;
class Fl_Toggle_Button;

namespace flw {

class GridGroup;

/*
 *      _____                                   _____  _       _
 *     |  __ \                                 |  __ \(_)     | |
 *     | |__) | __ ___   __ _ _ __ ___  ___ ___| |  | |_  __ _| | ___   __ _
 *     |  ___/ '__/ _ \ / _` | '__/ _ \/ __/ __| |  | | |/ _` | |/ _ \ / _` |
 *     | |   | | | (_) | (_| | | |  __/\__ \__ \ |__| | | (_| | | (_) | (_| |
 *     |_|   |_|  \___/ \__, |_|  \___||___/___/_____/|_|\__,_|_|\___/ \__, |
 *                       __/ |                                          __/ |
 *                      |___/                                          |___/
 */

/** @brief A dialog with a message list and a progress bar for time consuming work.
*
* Progress bar is optional.\n
* It has a pause button and an cancel button that can be disabled.\n
*
* @snippet dialog.cpp flw::ProgressDialog example
* @image html progress_dialog.png
*/
class ProgressDialog : public Fl_Double_Window {
public:
                                ProgressDialog(const std::string& title, bool cancel = false, bool pause = false, double min = 0.0, double max = 0.0);
    void                        range(double min, double max);
    void                        start();
    bool                        update(const StringVector& messages, unsigned milli = 100);
    bool                        update(double value, const StringVector& messages, unsigned milli = 100);
    bool                        update(const std::string& message, unsigned milli = 100);
    bool                        update(double value, const std::string& message, unsigned milli = 100);
    double                      value() const;
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

} // flw

// MKALGAM_OFF

#endif
