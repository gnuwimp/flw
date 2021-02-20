// Copyright 2021 gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#include "abortdialog.h"
#include "util.h"
#include <FL/Fl_Button.H>

// MALAGMA_ON

//------------------------------------------------------------------------------
flw::AbortDialog::AbortDialog() : Fl_Double_Window(0, 0, 0, 0, "Working...") {
    _button = new Fl_Button(0, 0, 0, 0, "Press To Abort");
    _abort  = false;
    _last   = 0;

    add(_button);

    _button->callback(flw::AbortDialog::Callback, this);
    _button->labelfont(flw::PREF_FONT);
    _button->labelsize(flw::PREF_FONTSIZE);

    callback(flw::AbortDialog::Callback, this);
    set_modal();
}

//------------------------------------------------------------------------------
void flw::AbortDialog::Callback(Fl_Widget* w, void* o) {
    auto dlg = (AbortDialog*) o;

    if (w == dlg->_button) {
        dlg->_abort = true;
    }
}

//------------------------------------------------------------------------------
bool flw::AbortDialog::abort(int milliseconds) {
    auto now = flw::util::time_milli();

    if (now - _last > milliseconds) {
        Fl::check();
        _last = now;
    }

    return _abort;
}

//------------------------------------------------------------------------------
void flw::AbortDialog::show(const std::string& label, Fl_Window* parent) {
    _abort = false;
    _last  = 0;

    resize(0, 0, flw::PREF_FONTSIZE * 32, flw::PREF_FONTSIZE * 6);
    _button->copy_label(label.c_str());
    _button->resize(7, 7, w() - 14, h() - 14);
    flw::util::center_window(this, parent);
    Fl_Double_Window::show();
    Fl::flush();
}

// MALAGMA_OFF
