/**
* @file
* @brief Assorted dialog functions and classes.
*
* @author gnuwimp@gmail.com
* @copyright Released under the GNU General Public License v3.0
*/

#include "settingsdialog.h"
#include "svgbutton.h"

// MKALGAM_ON

/*
 *       _____      _   _   _                 _____  _       _
 *      / ____|    | | | | (_)               |  __ \(_)     | |
 *     | (___   ___| |_| |_ _ _ __   __ _ ___| |  | |_  __ _| | ___   __ _
 *      \___ \ / _ \ __| __| | '_ \ / _` / __| |  | | |/ _` | |/ _ \ / _` |
 *      ____) |  __/ |_| |_| | | | | (_| \__ \ |__| | | (_| | | (_) | (_| |
 *     |_____/ \___|\__|\__|_|_| |_|\__, |___/_____/|_|\__,_|_|\___/ \__, |
 *                                   __/ |                            __/ |
 *                                  |___/                            |___/
 */

/** @brief Create empty settings dialog.
*
* @param[in] title  Dialog title.
* @param[in] W      Width in font units.
* @param[in] H      Height in font units.
*/
flw::SettingsDialog::SettingsDialog(const std::string& title, int W, int H) :
Fl_Double_Window(0, 0, flw::PREF_FONTSIZE * W, flw::PREF_FONTSIZE * H) {
    end();

    _cancel = new Fl_Button(0, 0, 0, 0, labels::CANCEL.c_str());
    _grid   = new flw::GridGroup(0, 0, w(), h());
    _label  = new Fl_Box(0, 0, 0, 0);
    _ok     = new SVGButton(0, 0, 0, 0, labels::OK, icons::CONFIRM, false, false, SVGButton::Pos::RIGHT);
    _reset  = new Fl_Button(0, 0, 0, 0, "Reset to Default");
    _undo   = new Fl_Button(0, 0, 0, 0, "Undo Changes");
    _tabs   = new flw::TabsGroup();
    _ret    = false;
    _run    = false;

    _grid->add(_tabs,      1,   1,  -1, -16);
    _grid->add(_label,     1, -15,  -1,   9);
    _grid->add(_reset,   -76,  -5,  18,   4);
    _grid->add(_undo,    -57,  -5,  18,   4);
    _grid->add(_cancel,  -38,  -5,  18,   4);
    _grid->add(_ok,      -19,  -5,  18,   4);
    add(_grid);

    _cancel->callback(SettingsDialog::Callback, this);
    _cancel->tooltip("Close dialog but save no settings.");
    _label->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE | FL_ALIGN_TOP);
    _label->box(FL_BORDER_BOX);
    _ok->callback(SettingsDialog::Callback, this);
    _ok->tooltip("Close dialog and update all settings.");
    _reset->callback(SettingsDialog::Callback, this);
    _reset->tooltip("Reset all settings to default values in current page.");
    _tabs->callback(SettingsDialog::Callback, this);
    _undo->callback(SettingsDialog::Callback, this);
    _undo->tooltip("Undo all changes to settings in current page.");

    util::labelfont(this);
    callback(SettingsDialog::Callback, this);
    copy_label(title.c_str());
    set_modal();
    resizable(_grid);
    _grid->resize(0, 0, w(), h());
}

/** @brief Add table to dialog.
*
* @param[in] label  Tab label.
* @param[in] table  Table widget.
* @param[in] after  Add page after this page, optional.
*/
void flw::SettingsDialog::add_page(const std::string& label, table::SettingsTable* table, table::SettingsTable* after) {
    _tabs->add(label, table, after);
    table->active_cell();
    table->set_dialog(this);
}

/** @brief Add table to dialog.
*
* @param[in] label   Tab label.
* @param[in] table   Table widget.
* @param[in] before  Insert page before this page, optional.
*/
void flw::SettingsDialog::insert_page(const std::string& label, table::SettingsTable* table, table::SettingsTable* before) {
    _tabs->insert(label, table, before);
    table->active_cell();
    table->set_dialog(this);
}

/** @brief Callback for widgets.
*
* @param[in] w  Widget that caused callback.
* @param[in] o  SettingsDialog.
*/
void flw::SettingsDialog::Callback(Fl_Widget* w, void* o) {
    auto self = static_cast<SettingsDialog*>(o);

    if (w == self) {
    }
    else if (w == self->_tabs) {
        self->message("");
    }
    else if (w == self->_cancel) {
        self->_ret = false;
        self->_run = false;
        self->hide();
    }
    else if (w == self->_ok) {
        for (int f = 0; f < self->_tabs->tabs(); f++) {
            auto row  = 0;
            auto page = static_cast<table::SettingsTable*>(self->_tabs->tab(f));
            auto err  = page->validate_data(row);

            if (err != "") {
                self->_tabs->value(page);

                if (row > 0 && row <= page->rows()) {
                    page->active_cell(row, 1);
                }

                self->message(err, FL_DARK_RED);
                return;
            }
        }

        for (int f = 0; f < self->_tabs->tabs(); f++) {
            auto page = static_cast<table::SettingsTable*>(self->_tabs->tab(f));
            page->get_data();
        }

        self->_ret = true;
        self->_run = false;
    }
    else if (w == self->_reset) {
        auto page = self->_tabs->value();

        if (page != nullptr) {
            static_cast<table::SettingsTable*>(page)->set_def_data();
            page->take_focus();
            Fl::redraw();
        }
    }
    else if (w == self->_undo) {
        auto page = self->_tabs->value();

        if (page != nullptr) {
            static_cast<table::SettingsTable*>(page)->set_data();
            page->take_focus();
            Fl::redraw();
        }
    }
}

/** @brief Set message label.
*
* @param[in] msg    Message string.
* @param[in] color  Label color.
*/
void flw::SettingsDialog::message(const std::string& msg, Fl_Color color) {
    _label->labelcolor(color);
    _label->copy_label(msg.c_str());
}

/** @brief Return one of tab pages or NULL if out of range.
*
* @param[in] num  Tab index.
*
* @return A class derived from table::SettingsTable or NULL.
*/
flw::table::SettingsTable* flw::SettingsDialog::page(int num) {
    return static_cast<table::SettingsTable*>(_tabs->child(num));
}

/** @brief Show dialog.
*
* @return True if Ok button has been pressed.
*/
bool flw::SettingsDialog::run() {
    _ret = false;
    _run = true;
    _tabs->value(_tabs->tab(0));

    util::center_window(this, flw::PREF_WINDOW);
    show();

    while (_run == true) {
        Fl::wait();
        Fl::flush();
    }

    return _ret;
}

// MKALGAM_OFF
