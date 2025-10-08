/**
* @file
* @brief Table editor widget with data storage.
*
* @author gnuwimp@gmail.com
* @copyright Released under the GNU General Public License v3.0
*/

#include "table.h"

// MKALGAM_ON

#include <stdarg.h>

namespace flw {
namespace table {

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
Table::Table(int rows, int cols, int X, int Y, int W, int H, const char* l) : Editor(X, Y, W, H, l) {
    rows    = rows < 1 ? 1 : rows;
    cols    = cols < 1 ? 1 : cols;

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
StringVector Table::cell_choice(int row, int col) {
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
int Table::_get_int(StringHash& hash, int row, int col, int def) {
    auto value = _get_string(hash, row, col);
    return value != "" ? atoi(value.c_str()) : def;
}

/** @brief Get cell key.
*
* @param[in] row   Row.
* @param[in] col   Column.
*
* @return A string in the form of "rNNcNN.
*/
std::string Table::_get_key(int row, int col) {
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
std::string Table::_get_string(StringHash& hash, int row, int col, const std::string& def) {
    auto search = hash.find(_get_key(row, col));
    return search != hash.end() ? search->second.c_str() : def;
}

/** Delete all data.
*
*/
void Table::reset() {
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
void Table::_set_int(StringHash& hash, int row, int col, int value) {
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
void Table::size(int rows, int cols) {
    if (rows > 0 && cols > 0) {
        reset();
        Editor::size(rows, cols);
    }
}

} // table
} // flw

// MKALGAM_OFF
