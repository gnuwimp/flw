/**
* @file
* @brief Table editor widget with data storage.
*
* @author gnuwimp@gmail.com
* @copyright Released under the GNU General Public License v3.0
*/

#include "table.h"
#include "settingsdialog.h"

// MKALGAM_ON

#include <cassert>

/*
 *      _______    _     _
 *     |__   __|  | |   | |
 *        | | __ _| |__ | | ___
 *        | |/ _` | '_ \| |/ _ \
 *        | | (_| | |_) | |  __/
 *        |_|\__,_|_.__/|_|\___|
 *
 *
 */

/** @brief Create table widget.
*
* @param[in] rows  Row count.
* @param[in] cols  Colun count.
* @param[in] X     X pos.
* @param[in] Y     Y pos.
* @param[in] W     Width.
* @param[in] H     Height.
* @param[in] l     Optional label.
*/
flw::table::Table::Table(int rows, int cols, int X, int Y, int W, int H, const char* l) : Editor(X, Y, W, H, l) {
    rows = (rows < 1) ? 1 : rows;
    cols = (cols < 1) ? 1 : cols;

    Table::size(rows, cols);
    lines(true, true);
    header(true, true);
    select_mode(Select::CELL);
    active_cell(1, 1);
    resize_column_width(true);
    expand_last_column(true);
}

/** @brief Get a list of choices.
*
* @param[in] row  Row.
* @param[in] col  Column.
*
* @return Vector with strings.
*/
flw::StringVector flw::table::Table::cell_choice(int row, int col) {
    _cell_choices.clear();
    auto choices = _get_string(_cell_choice, row, col);

    if (choices != "") {
        _cell_choices = util::split_string(choices, "\t");
    }

    return _cell_choices;
}

/** @brief Get int value from hash.
*
* @param[in] hash  Hash.
* @param[in] row   Row.
* @param[in] col   Column.
* @param[in] def   Default value if not found.
*
* @return Found integer or default value.
*/
int flw::table::Table::_get_int(StringHash& hash, int row, int col, int def) {
    auto value = _get_string(hash, row, col);
    auto res   = util::to_int(value, def);

    return static_cast<int>(res);
}

/** @brief Get cell key.
*
* @param[in] row   Row.
* @param[in] col   Column.
*
* @return A string in the form of "rNNcNN.
*/
std::string flw::table::Table::_get_key(int row, int col) {
    char key[100];
    snprintf(key, 100, "r%dc%d", row, col);
    return key;
}

/** @brief Get string value from hash.
*
* @param[in] hash  Hash.
* @param[in] row   Row.
* @param[in] col   Column.
* @param[in] def   Default value if not found.
*
* @return Found string or default value.
*/
std::string flw::table::Table::_get_string(StringHash& hash, int row, int col, const std::string& def) {
    auto search = hash.find(_get_key(row, col));
    return search != hash.end() ? search->second.c_str() : def;
}

/** Delete all data.
*
*/
void flw::table::Table::reset() {
    _cell_align.clear();
    _cell_choice.clear();
    _cell_color.clear();
    _cell_edit.clear();
    _cell_format.clear();
    _cell_textcolor.clear();
    _cell_type.clear();
    _cell_value.clear();
    _cell_width.clear();
}

/** @brief Set int value in hash.
*
* @param[in] hash   Hash.
* @param[in] row    Row.
* @param[in] col    Column.
* @param[in] value  Value to set.
*/
void flw::table::Table::_set_int(StringHash& hash, int row, int col, int value) {
    char value2[100];
    sprintf(value2, "%d", value);
    hash[_get_key(row, col)] = value2;
}

/** Set size of table.
*
* All data is deleted.
*
* @param[in] rows  Rows (> 0).
* @param[in] cols  Columns (> 0).
*/
void flw::table::Table::size(int rows, int cols) {
    if (rows > 0 && cols > 0) {
        reset();
        Editor::size(rows, cols);
    }
}

/*
 *       _____      _   _   _              _______    _     _
 *      / ____|    | | | | (_)            |__   __|  | |   | |
 *     | (___   ___| |_| |_ _ _ __   __ _ ___| | __ _| |__ | | ___
 *      \___ \ / _ \ __| __| | '_ \ / _` / __| |/ _` | '_ \| |/ _ \
 *      ____) |  __/ |_| |_| | | | | (_| \__ \ | (_| | |_) | |  __/
 *     |_____/ \___|\__|\__|_|_| |_|\__, |___/_|\__,_|_.__/|_|\___|
 *                                   __/ |
 *                                  |___/
 */

/** @brief Create settings table.
*
* @param[in] rows  Number of rows (0 - 32'768).
*/
flw::table::SettingsTable::SettingsTable(int rows) :
flw::table::Table(rows, 1) {
    assert(rows >= 0 && rows <= 32'768);

    _dlg = nullptr;
    size(rows, 1);
    cell_width(0, flw::PREF_FONTSIZE * 16);
    cell_width(1, flw::PREF_FONTSIZE * 4);
    draw_inactive_lighter(true);
    expand_last_column(true);
    header(true, false);
    height(flw::PREF_FONTSIZE + 8);
    lines(true, true);
    resize_column_width(true);
    select_mode(table::Select::CELL);
    tooltip("");
}

/** @brief Set message label.
*
* Dialog must have been set in constructor.
*
* @param[in] msg    Message string.
* @param[in] color  Label color.
*/
void flw::table::SettingsTable::message(const std::string& msg, Fl_Color color) {
    if (_dlg == nullptr) {
        return;
    }

    _dlg->message(msg, color);
}

// MKALGAM_OFF
