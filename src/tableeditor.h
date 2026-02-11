/**
* @file
* @brief Table editor widget.
*
* @author gnuwimp@gmail.com
* @copyright Released under the GNU General Public License v3.0
*/

#ifndef FLW_TABLE_EDITOR_H
#define FLW_TABLE_EDITOR_H

#include "tabledisplay.h"

// MKALGAM_ON

namespace flw {
namespace table {

/** @brief Format flags for displaying values.
*
*/
enum class Format {
    DEFAULT,        ///< @brief Use default format for current type.
    INT_DEF,        ///< @brief Default integer.
    INT_SEP1,       ///< @brief Space as thousand separator.
    INT_SEP2,       ///< @brief Single quote as thousand separator.
    DEC_DEF,        ///< @brief Default number.
    DEC_0,          ///< @brief 0 decimals.
    DEC_1,          ///< @brief 1 decimals.
    DEC_2,          ///< @brief 2 decimals.
    DEC_3,          ///< @brief 3 decimals.
    DEC_4,          ///< @brief 4 decimals.
    DEC_5,          ///< @brief 5 decimals.
    DEC_6,          ///< @brief 6 decimals.
    DEC_7,          ///< @brief 7 decimals.
    DEC_8,          ///< @brief 8 decimals.
    DATE_DEF,       ///< @brief Default date, "YYYY-MM-DD".
    DATE_WORLD,     ///< @brief World date, "DD/MM/YYYY".
    DATE_US,        ///< @brief US date, "MM/DD/YYYY".
    SECRET_DEF,     ///< @brief Password with dots.
    SECRET_STAR,    ///< @brief Password with stars.
};

/** @brief All data types.
*
* All values are strings.\n
* Numbers are converted from/to strings.\n
*/
enum class Type {
    TEXT,           ///< @brief Normal text.
    INTEGER,        ///< @brief Integer number.
    NUMBER,         ///< @brief Decimal number.
    BOOLEAN,        ///< @brief Checkbox, use "1" to check it.
    SECRET,         ///< @brief Password text field.
    CHOICE,         ///< @brief String list.
    ICHOICE,        ///< @brief String list with a text input field.
    SLIDER,         ///< @brief Decimal number as a slider.
    VSLIDER,        ///< @brief Decimal number as a slider and number label.
    COLOR,          ///< @brief Color value as an unsigned integer and with a color dialog for editing colors.
    FILE,           ///< @brief File string with a file dialog for selecting new file.
    DIR,            ///< @brief Directory string with a file dialog for selecting new directory.
    DATE,           ///< @brief Date string as "YYYY-MM-DD" with a date dialog for selecting a new date.
    LIST,           ///< @brief Text field with a string list dialog to select a new string.
    MTEXT,          ///< @brief Multiline text field with a text dialog for editing.
};

extern std::string EditColorLabel;
extern std::string EditDateLabel;
extern std::string EditDirLabel;
extern std::string EditFileLabel;
extern std::string EditListLabel;
extern std::string EditTextLabel;

std::string format_slider(double val, double min, double max, double step);
std::string format_slider2(int64_t val, int64_t min, int64_t max, int64_t step);
double      get_slider(const std::string& slider_string, double def = 0.0);
int64_t     get_slider2(const std::string& slider_string, int64_t def = 0);

/*
 *      ______    _ _ _
 *     |  ____|  | (_) |
 *     | |__   __| |_| |_ ___  _ __
 *     |  __| / _` | | __/ _ \| '__|
 *     | |___| (_| | | || (_) | |
 *     |______\__,_|_|\__\___/|_|
 *
 *
 */

/** @brief A table editor widget.
*
* Implement virtual methods to be useful.\n
* At least Display::cell_value(int, int) must be implemented to actual show any text.\n
* And Editor::cell_value(int, int, const std::string&) must be implemented to save edited values.\n
*
* @snippet tableeditor.cpp flw::table::test example
* @snippet tableeditor.cpp flw::table::Editor example
* @image html tableeditor.png
*/
class Editor : public Display {
    using Display::cell_value;

public:
    explicit                    Editor(int X = 0, int Y = 0, int W = 0, int H = 0, const char* l = nullptr);
    void                        send_changed_event_always(bool value)
                                    { _force_events = value; } ///< @brief Send change event even if value has not changed.
    virtual StringVector        cell_choice(int row, int col)
                                    { (void) row; (void) col; return StringVector(); } ///< @brief Get choice string vector.
    virtual bool                cell_edit(int row, int col)
                                    { (void) row; (void) col; return false; } ///< @brief Is cell editable?
    virtual Format              cell_format(int row, int col)
                                    { (void) row; (void) col; return Format::DEFAULT; } ///< @brief Draw options for Editor::Type.
    Fl_Color                    cell_textcolor(int row, int col) override;
    virtual Type                cell_type(int row, int col)
                                    { (void) row; (void) col; return Type::TEXT; } ///< @brief Type of value.
    virtual bool                cell_value(int row, int col, const std::string& value)
                                    { (void) row; (void) col; (void) value; return false; } ///< @brief Set new value. @return True to stop editing.
    void                        cmd_cut();
    void                        cmd_delete();
    void                        cmd_paste();
    bool                        draw_inactive_lighter() const
                                    { return _ro; }
    void                        draw_inactive_lighter(bool ro)
                                    { _ro = ro; }
    int                         handle(int event) override;
    void                        reset() override;

protected:
    void                        _draw_cell(int row, int col, int X, int Y, int W, int H, bool ver, bool hor, bool current = false) override;
    void                        _edit_create();
    void                        _edit_quick(const std::string& key);
    void                        _edit_show_dlg();
    void                        _edit_start(const std::string& key = "");
    void                        _edit_stop(bool save = true);
    int                         _ev_keyboard_down2();
    int                         _ev_mouse_click2();
    int                         _ev_paste();
    std::string                 _get_find_value(int row, int col) override;

    Fl_Widget*                  _edit2;         ///< @brief Only used for Type::INPUT_CHOICE/Fl_Input_Choice editing and for focus reason.
    Fl_Widget*                  _edit3;         ///< @brief Only used for Type::INPUT_CHOICE/Fl_Input_Choice editing and for focus reason.
    bool                        _force_events;  ///< @brief True to force events even if value has not changed.
    bool                        _ro;            ///< @brief True to turn on lighter text for read only cells, unless cell_textcolor() is overriden.
};

} // table
} // flw

// MKALGAM_OFF

#endif
