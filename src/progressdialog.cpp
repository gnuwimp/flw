/**
* @file
* @brief Progress message dialog.
*
* @author gnuwimp@gmail.com
* @copyright Released under the GNU General Public License v3.0
*/

#include "gridgroup.h"
#include "progressdialog.h"

// MKALGAM_ON

#include <cmath>
#include <FL/Fl_Hold_Browser.H>
#include <FL/Fl_Hor_Fill_Slider.H>
#include <FL/Fl_Toggle_Button.H>

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
flw::ProgressDialog::ProgressDialog(const std::string& title, bool cancel, bool pause, double min, double max) :
Fl_Double_Window(0, 0, flw::PREF_FONTSIZE * 40, flw::PREF_FONTSIZE * 12) {
    end();

    _cancel   = new Fl_Button(0, 0, 0, 0, labels::CANCEL.c_str());
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

    _cancel->callback(ProgressDialog::Callback, this);
    _label->box(FL_BORDER_BOX);
    _label->textfont(flw::PREF_FONT);
    _label->textsize(flw::PREF_FONTSIZE);
    _pause->callback(ProgressDialog::Callback, this);

    if (cancel == false) {
        _cancel->deactivate();
    }

    if (pause == false) {
        _pause->deactivate();
    }

    util::labelfont(this);
    callback(ProgressDialog::Callback, this);
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
void flw::ProgressDialog::Callback(Fl_Widget* w, void* o) {
    auto self = static_cast<ProgressDialog*>(o);

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
* @param[in] min  Min progress bar value.
* @param[in] max  Max progress bar value.
*/
void flw::ProgressDialog::range(double min, double max) {
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

/** @brief Show window.
*
*/
void flw::ProgressDialog::start() {
    util::center_window(this, flw::PREF_WINDOW);
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
bool flw::ProgressDialog::update(double value, const StringVector& messages, unsigned milli) {
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
bool flw::ProgressDialog::update(const StringVector& messages, unsigned milli) {
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
bool flw::ProgressDialog::update(double value, const std::string& message, unsigned milli) {
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
bool flw::ProgressDialog::update(const std::string& message, unsigned milli) {
    return update(0.0, message, milli);
}

/** @brief Return progress bar value.
*
*/
double flw::ProgressDialog::value() const {
    return _progress->value();
}

/** @brief Set progress bar value.
*
* @param[in] value  Number from progress min to progress max.
*/
void flw::ProgressDialog::value(double value) {
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

// MKALGAM_OFF
