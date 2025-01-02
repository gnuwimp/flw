// Copyright gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#ifndef FLW_TABLEDISPLAY_H
#define FLW_TABLEDISPLAY_H

#include "flw.h"

// MKALGAM_ON

#include <map>
#include <FL/Fl_Group.H>
#include <FL/Fl_Scrollbar.H>

namespace flw {

/***
 *      _______    _     _      _____  _           _
 *     |__   __|  | |   | |    |  __ \(_)         | |
 *        | | __ _| |__ | | ___| |  | |_ ___ _ __ | | __ _ _   _
 *        | |/ _` | '_ \| |/ _ \ |  | | / __| '_ \| |/ _` | | | |
 *        | | (_| | |_) | |  __/ |__| | \__ \ |_) | | (_| | |_| |
 *        |_|\__,_|_.__/|_|\___|_____/|_|___/ .__/|_|\__,_|\__, |
 *                                          | |             __/ |
 *                                          |_|            |___/
 */

//------------------------------------------------------------------------------
// A table display widget
//
class TableDisplay : public Fl_Group {
    friend class _TableDisplayFindDialog;

public:
    static constexpr const char* HELP_TEXT = "Press ctrl + 'g' to show go to cell dialog.\nPress ctrl + 'f' to show find text dialog.";

    enum class SELECT {
                                NO,
                                CELL,
                                ROW,
    };

    enum class EVENT { // These messages are sent to Fl_Callback and most of them does nothing (up to user).
                                UNDEFINED,
                                CHANGED,        // Data has been changed.
                                CURSOR,         // Cursor has been moved (cell has been changed).
                                COLUMN,         // Column header has been clicked.
                                COLUMN_CTRL,    // Column header has been clicked and ctrl key has been pressed.
                                ROW,            // Row header has been clicked.
                                ROW_CTRL,       // Row header has been clicked and ctrl key has been pressed.
                                SIZE,           // Size has been changed.
                                APPEND_ROW,     // Append row at current cell.
                                APPEND_COLUMN,  // Append column at current cell.
                                INSERT_ROW,     // Insert row at current cell.
                                INSERT_COLUMN,  // Insert column at current cell.
                                DELETE_ROW,     // Delete row at current cell.
                                DELETE_COLUMN,  // Delete column at current cell.
    };

    explicit                    TableDisplay(int X = 0, int Y = 0, int W = 0, int H = 0, const char* l = nullptr);
    void                        active_cell(int row = -1, int col = -1, bool show = false);
    virtual Fl_Align            cell_align(int row, int col)
                                        { (void) row; (void) col; return FL_ALIGN_LEFT; }
    virtual Fl_Color            cell_color(int row, int col)
                                        { (void) row; (void) col; return FL_BACKGROUND2_COLOR; }
    virtual Fl_Color            cell_textcolor(int row, int col)
                                    { (void) row; (void) col; return FL_FOREGROUND_COLOR; }
    virtual Fl_Font             cell_textfont(int row, int col)
                                        { (void) row; (void) col; return flw::PREF_FONT; }
    virtual Fl_Fontsize         cell_textsize(int row, int col)
                                        { (void) row; (void) col; return flw::PREF_FONTSIZE; }
    virtual const char*         cell_value(int row, int col)
                                        { (void) row; (void) col; return ""; }
    virtual int                 cell_width(int col)
                                        { (void) col; return flw::PREF_FONTSIZE * 6; }
    virtual void                cell_width(int col, int width)
                                        { (void) col; (void) width; }
    virtual void                clear();
    int                         column() const
                                    { return _curr_col; }
    int                         columns() const
                                    { return _cols; }
    virtual void                draw() override;
    void                        expand_last_column(bool expand = false)
                                    { _expand = expand; redraw(); }
    TableDisplay::EVENT         event() const
                                    { return _event; }
    int                         event_col() const
                                    { return _event_col; }
    int                         event_row() const
                                    { return _event_row; }
    virtual int                 handle(int event) override;
    void                        header(bool row = false, bool col = false);
    int                         height() const
                                    { return _height; }
    void                        height(int height)
                                    { _height = height; }
    void                        lines(bool ver = false, bool hor = false);
    void                        resize_column_width(bool resize = false)
                                    { _resize = resize; }
    int                         row() const
                                    { return _curr_row; }
    int                         rows() const
                                    { return _rows; }
    void                        scrollbar(bool ver = true, bool hor = true)
                                    { _disable_ver = !ver; _disable_hor = !hor; redraw(); }
    void                        select_mode(TableDisplay::SELECT select = TableDisplay::SELECT::NO)
                                    { _select = select; }
    void                        show_cell(int row, int col);
    virtual void                size(int rows, int cols);

protected:
    enum class _TABLEDISPLAY_MOVE {
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
    int                         _ev_keyboard_down();
    int                         _ev_mouse_click();
    int                         _ev_mouse_drag();
    int                         _ev_mouse_move();
    void                        _get_cell_below_mouse(int& row, int& col);
    void                        _move_cursor(_TABLEDISPLAY_MOVE move);
    void                        _update_scrollbars();
    void                        _set_event(int row, int col, TableDisplay::EVENT event)
                                    { _event_row = row; _event_col = col; _event = event; }

    static void                 _CallbackHor(Fl_Widget* w, void* v);
    static void                 _CallbackVer(Fl_Widget* w, void* v);

    TableDisplay::EVENT         _event;
    TableDisplay::SELECT        _select;
    Fl_Scrollbar*               _hor;
    Fl_Scrollbar*               _ver;
    Fl_Widget*                  _edit;
    std::string                 _find;
    bool                        _disable_hor;
    bool                        _disable_ver;
    bool                        _drag;
    bool                        _expand;
    bool                        _resize;
    bool                        _show_col_header;
    bool                        _show_hor_lines;
    bool                        _show_row_header;
    bool                        _show_ver_lines;
    int                         _cols;
    int                         _curr_col;
    int                         _curr_row;
    int                         _current_cell[4];
    int                         _event_col;
    int                         _event_row;
    int                         _height;
    int                         _resize_col;
    int                         _rows;
    int                         _start_col;
    int                         _start_row;
};

} // flw

// MKALGAM_OFF

#endif
