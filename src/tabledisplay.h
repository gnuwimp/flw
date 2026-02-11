/**
* @file
* @brief Table display widget.
*
* @author gnuwimp@gmail.com
* @copyright Released under the GNU General Public License v3.0
*/

#ifndef FLW_TABLE_DISPLAY_H
#define FLW_TABLE_DISPLAY_H

#include "flw.h"

// MKALGAM_ON

#include <map>
#include <FL/Fl_Group.H>

class Fl_Scrollbar;

namespace flw {

namespace priv {
    class _TableFindDialog;
}

/** @brief Table widgets.
*
*/
namespace table {

/** @brief Select cell/row choices.
*
*/
enum class Select {
    NO,     ///< @brief No selection.
    CELL,   ///< @brief Select cell.
    ROW,    ///< @brief Select row.
};

/** @brief Callback events.
*
* These messages are sent to Fl_Callback and most of them does nothing (up to user).
*/
enum class Event {
    UNDEFINED,      ///< @brief No event.
    CHANGED,        ///< @brief Data has been changed.
    CURSOR,         ///< @brief Cursor has been moved (current cell has been changed).
    COLUMN,         ///< @brief Column header has been clicked.
    COLUMN_CTRL,    ///< @brief Column header has been clicked and ctrl key has been pressed.
    ROW,            ///< @brief Row header has been clicked.
    ROW_CTRL,       ///< @brief Row header has been clicked and ctrl key has been pressed.
    SIZE,           ///< @brief Size has been changed.
    APPEND_ROW,     ///< @brief Append row at current cell, Display::enable_append_keys() must be on.
    APPEND_COLUMN,  ///< @brief Append column at current cell, Display::enable_append_keys() must be on.
    INSERT_ROW,     ///< @brief Insert row at current cell, Display::enable_append_keys() must be on.
    INSERT_COLUMN,  ///< @brief Insert column at current cell, Display::enable_append_keys() must be on.
    DELETE_ROW,     ///< @brief Delete row at current cell, Display::enable_append_keys() must be on.
    DELETE_COLUMN,  ///< @brief Delete column at current cell, Display::enable_append_keys() must be on.
};

/*
 *      _____  _           _
 *     |  __ \(_)         | |
 *     | |  | |_ ___ _ __ | | __ _ _   _
 *     | |  | | / __| '_ \| |/ _` | | | |
 *     | |__| | \__ \ |_) | | (_| | |_| |
 *     |_____/|_|___/ .__/|_|\__,_|\__, |
 *                  | |             __/ |
 *                  |_|            |___/
 */

/** @brief A table display widget that draw only text.
*
* Implement virtual methods to be useful.\n
* At least Display::cell_value(int row, int col) must be implemented to actual show any text.\n
*\n
* Set normal FLTK callback to receive table::Event events.\n
* Use Display::event(), Display::event_col() and Display::event_row() to query the events.\n
*\n
* Keyboard shortcust to append/insert or delete row/columns must be handled in callback.\n
*\n
* To enable column resizing, values sent to Display::cell_width(int col, int width) must be stored.\n
*/
class Display : public Fl_Group {
    friend class priv::_TableFindDialog;

public:
    explicit                    Display(int X = 0, int Y = 0, int W = 0, int H = 0, const char* l = nullptr);
    void                        active_cell(int row = -1, int col = -1, bool show = false);
    virtual Fl_Align            cell_align(int row, int col)
                                        { (void) row; (void) col; return FL_ALIGN_LEFT; } ///< @brief Get alignment for cell.
    virtual Fl_Color            cell_color(int row, int col)
                                        { (void) row; (void) col; return FL_BACKGROUND2_COLOR; } ///< @brief Get background color for cell.
    virtual void                cell_event(int row, int col, Event event)
                                        { (void) row; (void) col; (void) event; } ///< @brief Alternative callback for events.
    virtual Fl_Color            cell_textcolor(int row, int col)
                                    { (void) row; (void) col; return FL_FOREGROUND_COLOR; } ///< @brief Get text color for cell.
    virtual Fl_Font             cell_textfont(int row, int col)
                                        { (void) row; (void) col; return flw::PREF_FONT; } ///< @brief Get font for cell.
    virtual Fl_Fontsize         cell_textsize(int row, int col)
                                        { (void) row; (void) col; return flw::PREF_FONTSIZE; } ///< @brief Get font size for cell.
    virtual std::string         cell_value(int row, int col)
                                        { (void) row; (void) col; return ""; } ///< @brief Get text for cell.
    virtual int                 cell_width(int col)
                                        { (void) col; return flw::PREF_FONTSIZE * 6; } ///< @brief Get column width.
    virtual void                cell_width(int col, int width)
                                        { (void) col; (void) width; } ///< @brief Set column width.
    int                         column() const
                                    { return _curr_col; } ///< @brief Get current column.
    int                         columns() const
                                    { return _cols; } ///< @brief Get column count.
    void                        cmd_copy();
    void                        cmd_find();
    void                        cmd_goto();
    void                        debug() const;
    virtual void                draw() override;
    void                        enable_append_keys(bool val = false)
                                    { _enable_keys = val; } ///< @brief Enable/disable append keys. This is to generate events for cmd key + 'a'/'i'/'d'
    Event                       event() const
                                    { return _event; } ///< @brief Get last event.
    int                         event_col() const
                                    { return _event_col; } ///< @brief Get last event column.
    int                         event_row() const
                                    { return _event_row; } ///< @brief Get last event row.
    void                        expand_last_column(bool expand = false)
                                    { _expand = expand; redraw(); } ///< @brief Expand drawing of last column option.
    virtual int                 handle(int event) override;
    void                        header(bool row = false, bool col = false);
    int                         height() const
                                    { return _height; } ///< @brief Get row height as pixels.
    void                        height(int height)
                                    { if (height >= 6 && height <= 72) _height = height; } ///< @brief Set row height. @param[in] height  From 6 to 72 pixels.
    void                        lines(bool ver = false, bool hor = false);
    virtual void                reset();
    void                        resize_column_width(bool resize = false)
                                    { _resize = resize; } ///< @brief Turn resizing of column width.
    int                         row() const
                                    { return _curr_row; } ///< @brief Get current row.
    int                         rows() const
                                    { return _rows; } ///< @brief Get row count.
    void                        scrollbar(bool ver = true, bool hor = true)
                                    { _disable_ver = !ver; _disable_hor = !hor; redraw(); } ///< @brief Enable/disable scrollbars.
    void                        select_mode(Select select = Select::NO)
                                    { _select = select; } ///< @brief Set select mode.
    void                        show_cell(int row, int col);
    virtual void                size(int rows, int cols);

protected:
    enum class Move {
                                DOWN,
                                FIRST_COL,
                                FIRST_ROW,
                                LAST_COL,
                                LAST_ROW,
                                LEFT,
                                PAGE_DOWN,
                                PAGE_UP,
                                RIGHT,
                                SCROLL_DOWN,
                                SCROLL_LEFT,
                                SCROLL_RIGHT,
                                SCROLL_UP,
                                UP,
    };

    int                         _cell_height(int Y = -1);
    int                         _cell_width(int col, int X = -1);
    virtual void                _draw_cell(int row, int col, int X, int Y, int W, int H, bool ver, bool hor, bool current = false);
    void                        _draw_row(int row, int W, int Y, int H);
    void                        _draw_text(const char* string, int X, int Y, int W, int H, Fl_Align align);
    int                         _ev_keyboard_down(bool only_append_insert = false);
    int                         _ev_mouse_click();
    int                         _ev_mouse_drag();
    int                         _ev_mouse_move();
    void                        _get_cell_below_mouse(int& row, int& col);
    virtual std::string         _get_find_value(int row, int col)
                                    { return cell_value(row, col); } ///< @brief Get value from cell that is used for dearching text.
    void                        _move_cursor(Move move);
    void                        _update_scrollbars();
    void                        _set_event(int row, int col, Event event);

    static void                 _CallbackHor(Fl_Widget* w, void* v);
    static void                 _CallbackVer(Fl_Widget* w, void* v);

    Event                       _event;                 ///< @brief Last event value
    Select                      _select;                ///< @brief Select mode.
    Fl_Scrollbar*               _hor;                   ///< @brief Horizontal scrollbar.
    Fl_Scrollbar*               _ver;                   ///< @brief Vertical scrollbar.
    Fl_Widget*                  _edit;                  ///< @brief Current edit widget, no widget is created in Display, only in Editor.
    std::string                 _find;                  ///< @brief Last find string.
    bool                        _disable_hor;           ///< @brief True to disable horizontal scrollbar.
    bool                        _disable_ver;           ///< @brief True to disable vertical scrollbar.
    bool                        _drag;                  ///< @brief True if mouse drag is active.
    bool                        _enable_keys;           ///< @brief Enable append/insert/delete shortcuts.
    bool                        _expand;                ///< @brief Expand last column.
    bool                        _resize;                ///< @brief Resize columns.
    bool                        _show_col_header;       ///< @brief Show column header.
    bool                        _show_hor_lines;        ///< @brief Show horizontal lines.
    bool                        _show_row_header;       ///< @brief Show row header.
    bool                        _show_ver_lines;        ///< @brief Show vertical lines.
    int                         _cols;                  ///< @brief Number of columns.
    int                         _curr_col;              ///< @brief Current column.
    int                         _curr_row;              ///< @brief Current row.
    int                         _current_cell[4];       ///< @brief Size and pos of current cell.
    int                         _event_col;             ///< @brief Event column.
    int                         _event_row;             ///< @brief Event row.
    int                         _height;                ///< @brief Row height.
    int                         _resize_col;            ///< @brief Which column to resize.
    int                         _rows;                  ///< @brief Number of rows.
    int                         _start_col;             ///< @brief First visible column.
    int                         _start_row;             ///< @brief First visible row.
};

} // table
} // flw

// MKALGAM_OFF

#endif
