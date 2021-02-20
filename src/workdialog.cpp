// Copyright 2021 gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#include "workdialog.h"
#include "util.h"
#include <FL/Fl_Toggle_Button.H>
#include <FL/Fl_Hold_Browser.H>

// MALAGMA_ON

//------------------------------------------------------------------------------
flw::WorkDialog::WorkDialog(const char* title, Fl_Window* parent, bool cancel, bool pause, int W, int H) : Fl_Double_Window(0, 0, W * flw::PREF_FONTSIZE, H * flw::PREF_FONTSIZE) {
    end();

    _cancel = new Fl_Button(0, 0, 0, 0, "Cancel");
    _pause  = new Fl_Toggle_Button(0, 0, 0, 0, "Pause");
    _label  = new Fl_Hold_Browser(0, 0, 0, 0);
    _ret    = true;
    _last   = 0.0;

    add(_label);
    add(_pause);
    add(_cancel);

    _cancel->callback(flw::WorkDialog::Callback, this);
    _label->box(FL_BORDER_BOX);
    _label->textfont(flw::PREF_FONT);
    _label->textsize(flw::PREF_FONTSIZE);
    _pause->callback(flw::WorkDialog::Callback, this);

    if (cancel == false) {
        _cancel->deactivate();
    }

    if (pause == false) {
        _pause->deactivate();
    }

    flw::util::labelfont(this);
    callback(flw::WorkDialog::Callback, this);
    copy_label(title);
    set_modal();
    resizable(this);
    util::center_window(this, parent);
    show();
}

//------------------------------------------------------------------------------
void flw::WorkDialog::Callback(Fl_Widget* w, void* o) {
    auto dlg = (flw::WorkDialog*) o;

    if (w == dlg) {
    }
    else if (w == dlg->_cancel) {
        dlg->_ret = false;
    }
    else if (w == dlg->_pause) {
        bool cancel = dlg->_cancel->active();
        dlg->_cancel->deactivate();
        dlg->_pause->label("C&ontinue");

        while (dlg->_pause->value()) {
            util::time_sleep(10);
            Fl::check();
        }

        dlg->_pause->label("&Pause");

        if (cancel) {
            dlg->_cancel->activate();
        }
    }
}

//------------------------------------------------------------------------------
void flw::WorkDialog::resize(int X, int Y, int W, int H) {
    Fl_Double_Window::resize(X, Y, W, H);

    _label->resize(4, 4, W - 8, H - flw::PREF_FONTSIZE * 2 - 12);
    _pause->resize(W - flw::PREF_FONTSIZE * 20 - 8, H - flw::PREF_FONTSIZE * 2 - 4, flw::PREF_FONTSIZE * 10, flw::PREF_FONTSIZE * 2);
    _cancel->resize(W - flw::PREF_FONTSIZE * 10 - 4, H - flw::PREF_FONTSIZE * 2 - 4, flw::PREF_FONTSIZE * 10, flw::PREF_FONTSIZE * 2);
}

//------------------------------------------------------------------------------
bool flw::WorkDialog::run(double update_time, const std::vector<std::string>& messages) {
    auto now = util::time();

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
bool flw::WorkDialog::run(double update_time, const std::string& message) {
    auto now = util::time();

    if (now - _last > update_time) {
        _label->clear();
        _label->add(message.c_str());
        _last = now;
        Fl::check();
        Fl::flush();
    }

    return _ret;
}

// MALAGMA_OFF
