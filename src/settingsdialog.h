/**
* @file
* @brief Assorted dialog functions and classes.
*
* @author gnuwimp@gmail.com
* @copyright Released under the GNU General Public License v3.0
*/

#ifndef FLW_SETTINGSDIALOG_H
#define FLW_SETTINGSDIALOG_H

#include "flw.h"
#include "gridgroup.h"
#include "tabsgroup.h"
#include "table.h"

// MKALGAM_ON

#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Double_Window.H>

namespace flw {

namespace table {
class SettingsTable;
}

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

/** @brief A dialog for application settings.
*
* Add one or more flw::table::SettingsTable objects to the dialog.\n
*\n
* The dialog has 4 buttons.\n
* "Ok", "Cancel", "Reset to Default" and "Undo Changes".\n
*\n
* "Ok" will first call table::SettingsTable::validate_data() for all pages.\n
* It will stop for first error and select that page.\n
* If validation is successful it will call table::SettingsTable::get_data() to set updated data and close the dialog.\n
*\n
* "Reset to Default" will call table::SettingsTable::set_def_data() to restore default data for current page.\n
* "Undo Changes" will call table::SettingsTable::set_data() to restore data to initial values for current page.\n
*\n
* "Cancel" will close dialog without updating any data.\n
*\n
* It is up to SettingsTable class to update values.\n
*
* @snippet settingsdialog.cpp flw::table::SettingsTable example
* @snippet settingsdialog.cpp flw::SettingsDialog example
* @image html settingsdialog.png
*/
class SettingsDialog : public Fl_Double_Window {
public:
                                SettingsDialog(const std::string& title, int W = 45, int H = 34);
    void                        add_page(const std::string& label, table::SettingsTable* table, table::SettingsTable* after = nullptr);
    void                        insert_page(const std::string& label, table::SettingsTable* table, table::SettingsTable* before = nullptr);
    void                        message(const std::string& msg, Fl_Color color = FL_FOREGROUND_COLOR);
    table::SettingsTable*       page(int num);
    bool                        run();

private:
    static void                 Callback(Fl_Widget* w, void* o);

    Fl_Box*                     _label;     // Message label.
    Fl_Button*                  _cancel;    // Cancel work button.
    Fl_Button*                  _ok;        // Close dialog and update settings.
    Fl_Button*                  _reset;     // Reset data to default.
    Fl_Button*                  _undo;      // Restore data from start.
    GridGroup*                  _grid;      // Layout manager.
    TabsGroup*                  _tabs;      // Tab pages.
    bool                        _ret;       // Return value from update().
    bool                        _run;       // Last refresh window time.
};

} // flw

// MKALGAM_OFF

#endif
