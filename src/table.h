/**
* @file
* @brief Table editor widget with data storage.
*
* @author gnuwimp@gmail.com
* @copyright Released under the GNU General Public License v3.0
*/

#ifndef FLW_TABLE_H
#define FLW_TABLE_H

#include "tableeditor.h"

// MKALGAM_ON

#include <string>

namespace flw {

namespace table {

/** @brief Table editor widget with embedded data storage.
*
* To show and edit simple tables.
*
* @snippet table.cpp flw::table::Table example
* @image html table.png
*/
class Table : public Editor {
    friend class                _TableChoice;

public:
                                Table(int rows, int cols, int X = 0, int Y = 0, int W = 0, int H = 0, const char* l = nullptr);
    Fl_Align                    cell_align(int row, int col) override
                                    { return static_cast<Fl_Align>(_get_int(_cell_align, row, col, Editor::cell_align(row, col))); } ///< @brief Get cell alignment.
    void                        cell_align(int row, int col, Fl_Align align)
                                    { _set_int(_cell_align, row, col, (int) align); } ///< @brief Set cell alignment.
    StringVector                cell_choice(int row, int col) override;
    void                        cell_choice(int row, int col, const std::string& value)
                                    { _set_string(_cell_choice, row, col, value); } ///< @brief Set cell choice. @param[in] row  Row number. @param[in] col  Column number. @param[in] value  A string with tab between every choice.
    Fl_Color                    cell_color(int row, int col) override
                                    { return (Fl_Color) _get_int(_cell_color, row, col, Editor::cell_color(row, col)); } ///< @brief Get cell color.
    void                        cell_color(int row, int col, Fl_Color color)
                                    { _set_int(_cell_color, row, col, (int) color); } ///< @brief Set cell color.
    bool                        cell_edit(int row, int col) override
                                    { return static_cast<bool>(_get_int(_cell_edit, row, col, 0)); } ///< @brief Is cell editable.
    void                        cell_edit(int row, int col, bool value)
                                    { _set_int(_cell_edit, row, col, static_cast<int>(value)); } ///< @brief Set if cell is editable.
    Format                      cell_format(int row, int col) override
                                    { return static_cast<Format>(_get_int(_cell_format, row, col, static_cast<int>(Format::DEFAULT))); } ///< @brief Get value format.
    void                        cell_format(int row, int col, Format value)
                                    { _set_int(_cell_format, row, col, (int) value); } ///< @brief Set value format.
    Type                        cell_type(int row, int col) override
                                    { return static_cast<Type>(_get_int(_cell_type, row, col, static_cast<int>(Type::TEXT))); } ///< @brief Get value type.
    void                        cell_type(int row, int col, Type type)
                                    { _set_int(_cell_type, row, col, static_cast<int>(type)); } ///< @brief Set value type.
    Fl_Color                    cell_textcolor(int row, int col) override
                                    { return static_cast<Fl_Color>(_get_int(_cell_textcolor, row, col, Editor::cell_textcolor(row, col))); } ///< @brief Get text color.
    void                        cell_textcolor(int row, int col, Fl_Color color)
                                    { _set_int(_cell_textcolor, row, col, static_cast<int>(color)); } ///< @brief Set text color.
    std::string                 cell_value(int row, int col) override
                                    { return _get_string(_cell_value, row, col); } ///< @brief Get cell value.
    bool                        cell_value(int row, int col, const std::string& value) override
                                    { _set_string(_cell_value, row, col, value); return true; } ///< @brief Set cell value.
    int                         cell_width(int col) override
                                    { return _get_int(_cell_width, 0, col, flw::PREF_FONTSIZE * 8); } ///< @brief Get column width.
    void                        cell_width(int col, int width) override
                                    { _set_int(_cell_width, 0, col, width); } ///< @brief Set column width.
    void                        reset();
    virtual void                size(int rows, int cols) override;

protected:
    virtual int                 _get_int(StringHash& hash, int row, int col, int def = 0);
    virtual std::string         _get_key(int row, int col);
    virtual std::string         _get_string(StringHash& hash, int row, int col, const std::string& def = "");
    virtual void                _set_int(StringHash& hash, int row, int col, int value);
    virtual void                _set_string(StringHash& hash, int row, int col, const std::string& value)
                                    { hash[_get_key(row, col)] = value; } ///< @brief Set string value in hash.

    StringHash                  _cell_align;        // Cell alignment values.
    StringHash                  _cell_choice;       // Cell choices.
    StringHash                  _cell_color;        // Cell colors.
    StringHash                  _cell_edit;         // Cell edit status.
    StringHash                  _cell_format;       // Cell value format.
    StringHash                  _cell_textcolor;    // Cell text color.
    StringHash                  _cell_type;         // Cell data type.
    StringHash                  _cell_value;        // Cell values.
    StringHash                  _cell_width;        // Column widths.
    StringVector                _cell_choices;      // Choices for current editor.
};

} // table
} // flw

// MKALGAM_OFF

#endif
