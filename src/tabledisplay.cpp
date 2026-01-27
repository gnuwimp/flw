/**
* @file
* @brief Table display widget.
*
* @author gnuwimp@gmail.com
* @copyright Released under the GNU General Public License v3.0
*/

#include "dlg.h"
#include "gridgroup.h"
#include "tabledisplay.h"

// MKALGAM_ON

#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Int_Input.H>
#include <FL/Fl_Return_Button.H>
#include <FL/Fl_Scrollbar.H>
#include <FL/fl_ask.H>
#include <FL/fl_draw.H>

namespace flw {
namespace table {

static constexpr const char* HELP_TEXT = "Press CTRL + 'g' to show go to cell dialog.\nPress CTRL + 'f' to show find text dialog.";

/*
 *            _____                _ _ _
 *           / ____|              | | | |
 *          | (___   ___ _ __ ___ | | | |__   __ _ _ __
 *           \___ \ / __| '__/ _ \| | | '_ \ / _` | '__|
 *           ____) | (__| | | (_) | | | |_) | (_| | |
 *          |_____/ \___|_|  \___/|_|_|_.__/ \__,_|_|
 *      ______
 *     |______|
 */

/** @brief Scrollbar (private).
* @private
*
*/
class _Scrollbar : public Fl_Scrollbar {
public:
    /** @brief
    *
    */
    _Scrollbar(int X, int Y, int W, int H) : Fl_Scrollbar(X, Y, W, H) {
    }

    /** @brief
    *
    */
    int handle(int event) override {
        if (event == FL_FOCUS ||
            event == FL_KEYUP ||
            event == FL_KEYDOWN ||
            event == FL_SHORTCUT) {

            return 0;
        }

        return Fl_Scrollbar::handle(event);
    }
};

/*
 *           _____       _        _____  _       _
 *          / ____|     | |      |  __ \(_)     | |
 *         | |  __  ___ | |_ ___ | |  | |_  __ _| | ___   __ _
 *         | | |_ |/ _ \| __/ _ \| |  | | |/ _` | |/ _ \ / _` |
 *         | |__| | (_) | || (_) | |__| | | (_| | | (_) | (_| |
 *          \_____|\___/ \__\___/|_____/|_|\__,_|_|\___/ \__, |
 *      ______                                            __/ |
 *     |______|                                          |___/
 */

/** @brief Goto cell dialog (private).
* @private
*
*/
class _GotoDialog : public Fl_Double_Window {
    Fl_Int_Input*                   _row;   // Row number input.
    Fl_Int_Input*                   _col;   // Column number input.
    GridGroup*                      _grid;  // Layout widget.
    bool                            _ret;   // Return value.
    bool                            _run;   // Run condition.

public:
    /** @brief
    *
    */
    _GotoDialog(int row, int col) :
    Fl_Double_Window(0, 0, 10, 10, "Goto Cell") {
        end();

        _col  = new Fl_Int_Input(0, 0, 0, 0, "Column:");
        _grid = new GridGroup(0, 0, w(), h());
        _row  = new Fl_Int_Input(0, 0, 0, 0, "Row:");
        _ret  = false;
        _run  = false;

        _grid->add(_row,   1,  3,  -1,   4);
        _grid->add(_col,   1, 10,  -1,   4);
        add(_grid);

        _row->align(FL_ALIGN_LEFT | FL_ALIGN_TOP);
        _row->callback(_GotoDialog::Callback, this);
        _row->labelsize(flw::PREF_FONTSIZE);
        _row->textfont(flw::PREF_FIXED_FONT);
        _row->textsize(flw::PREF_FONTSIZE);
        _row->when(FL_WHEN_ENTER_KEY_ALWAYS);
        _col->align(FL_ALIGN_LEFT | FL_ALIGN_TOP);
        _col->callback(_GotoDialog::Callback, this);
        _col->labelsize(flw::PREF_FONTSIZE);
        _col->textfont(flw::PREF_FIXED_FONT);
        _col->textsize(flw::PREF_FONTSIZE);
        _col->when(FL_WHEN_ENTER_KEY_ALWAYS);

        char b[100];
        sprintf(b, "%d", row > 0 ? row : 1);
        _row->value(b);
        sprintf(b, "%d", col > 0 ? col : 1);
        _col->value(b);

        callback(_GotoDialog::Callback, this);
        set_modal();
        resizable(_grid);
        resize(0, 0, flw::PREF_FONTSIZE * 16, flw::PREF_FONTSIZE * 8);
    }

    /** @brief
    *
    */
    static void Callback(Fl_Widget* w, void* o) {
        auto self = static_cast<_GotoDialog*>(o);

        if (w == self) {
            self->_run = false;
            self->hide();
        }
        else if (w == self->_row || w == self->_col) {
            self->_ret = true;
            self->_run = false;
            self->hide();
        }
    }

    /** @brief
    *
    */
    int col() const {
        return (*_col->value() >= '0' && *_col->value() <= '9') ? atoi(_col->value()) : 0;
    }

    /** @brief
    *
    */
    int row() const {
        return (*_row->value() >= '0' && *_row->value() <= '9') ? atoi(_row->value()) : 0;
    }

    /** @brief
    *
    */
    bool run(Fl_Window* parent) {
        _run = true;
        flw::util::center_window(this, parent);
        show();

        while (_run == true) {
            Fl::wait();
            Fl::flush();
        }

        return _ret;
    }
};

/*
 *           ______ _           _ _____  _       _
 *          |  ____(_)         | |  __ \(_)     | |
 *          | |__   _ _ __   __| | |  | |_  __ _| | ___   __ _
 *          |  __| | | '_ \ / _` | |  | | |/ _` | |/ _ \ / _` |
 *          | |    | | | | | (_| | |__| | | (_| | | (_) | (_| |
 *          |_|    |_|_| |_|\__,_|_____/|_|\__,_|_|\___/ \__, |
 *      ______                                            __/ |
 *     |______|                                          |___/
 */

/** @brief Find text dialog (private).
* @private
*
*/
class _FindDialog : public Fl_Double_Window {
    Fl_Input*                   _find;      // Find input.
    Fl_Button*                  _next;      // Jump to next find.
    Fl_Button*                  _prev;      // Jump to prev find.
    Fl_Button*                  _close;     // Close dialog.
    GridGroup*                  _grid;      // Layout widget.
    Display*                    _table;     // Display widget.
    bool                        _repeat;    // True to repeat.
    bool                        _run;       // Run condition.

public:
    /** @brief
    *
    */
    _FindDialog(Display* table) :
    Fl_Double_Window(0, 0, 10, 10, "Table - Find Text") {
        end();

        _close  = new Fl_Button(0, 0, 0, 0, labels::CLOSE.c_str());
        _grid   = new GridGroup(0, 0, w(), h());
        _next   = new Fl_Return_Button(0, 0, 0, 0, labels::NEXT.c_str());
        _prev   = new Fl_Button(0, 0, 0, 0, labels::PREV.c_str());
        _find   = new Fl_Input(0, 0, 0, 0, "Find:");
        _table  = table;
        _repeat = true;
        _run    = false;

        _grid->add(_find,     8,  1,  -1,   4);
        _grid->add(_prev,   -51, -5,  16,   4);
        _grid->add(_next,   -34, -5,  16,   4);
        _grid->add(_close,  -17, -5,  16,   4);
        add(_grid);

        _close->callback(_FindDialog::Callback, this);
        _close->labelsize(flw::PREF_FONTSIZE);
        _next->callback(_FindDialog::Callback, this);
        _next->labelsize(flw::PREF_FONTSIZE);
        _prev->callback(_FindDialog::Callback, this);
        _prev->labelsize(flw::PREF_FONTSIZE);
        _find->align(FL_ALIGN_LEFT);
        _find->callback(_FindDialog::Callback, this);
        _find->labelsize(flw::PREF_FONTSIZE);
        _find->textfont(flw::PREF_FIXED_FONT);
        _find->textsize(flw::PREF_FONTSIZE);
        _find->value(_table->_find.c_str());
        _find->when(FL_WHEN_ENTER_KEY_ALWAYS);

        callback(_FindDialog::Callback, this);
        set_modal();
        resizable(_grid);
        resize(0, 0, flw::PREF_FONTSIZE * 35, flw::PREF_FONTSIZE * 6);
    }

    /** @brief
    *
    */
    static void Callback(Fl_Widget* w, void* o) {
        auto self = static_cast<_FindDialog*>(o);

        if (w == self) {
            self->_run = false;
            self->hide();
        }
        else if (w == self->_close) {
            self->_table->_find = self->_find->value();
            self->_run = false;
            self->hide();
        }
        else if (w == self->_next) {
            self->find(true);
        }
        else if (w == self->_prev) {
            self->find(false);
        }
        else if (w == self->_find) {
            self->find(self->_repeat);
        }
    }

    /** @brief Search for text.
    *
    * @param[in] next  True to search for next string.
    */
    void find(bool next) {
        auto find = util::to_string(_find->value());

        _repeat = next;

        if (find == "") {
            fl_beep(FL_BEEP_ERROR);
            return;
        }

        auto row = _table->row();
        auto col = _table->column();

        if (next == true) {
            if (row < 1 || col < 1) {
                row = 1;
                col = 1;
            }
            else {
                col++;
            }
        }
        else {
            if (row < 1 || col < 1) {
                row = _table->rows();
                col = _table->columns();
            }
            else {
                col--;
            }
        }

    AGAIN:
        if (next == true) {
            for (int r = row; r <= _table->rows(); r++) {
                for (int c = col; c <= _table->columns(); c++) {
                    auto v = _table->_get_find_value(r, c);
                    auto p = v.find(find);

                    if (p != std::string::npos) {
                        _table->active_cell(r, c, true);
                        _table->_find = find;
                        return;
                    }
                }

                col = 1;
            }

            if (dlg::msg_ask("Table", util::format("Unable to find <%s>!\nWould you like to try again from the beginning?", find.c_str()), labels::NO, labels::YES) == labels::YES) {
                col = row = 1;
                goto AGAIN;
            }
            else {
                hide();
            }
        }
        else {
            for (int r = row; r >= 1; r--) {
                for (int c = col; c >= 1; c--) {
                    auto v = _table->_get_find_value(r, c);

                    if (v.find(find) != std::string::npos) {
                        _table->active_cell(r, c, true);
                        _table->_find = find;
                        return;
                    }
                }

                col = _table->columns();
            }

            if (dlg::msg_ask("Table", util::format("Unable to find <%s>!\nWould you like to try again from the end?", find.c_str()), labels::NO, labels::YES) == labels::YES) {
                row = _table->rows();
                col = _table->columns();
                goto AGAIN;
            }
            else {
                hide();
            }
        }
    }

    /** @brief
    *
    */
    void run(Fl_Window* parent) {
        _run = true;
        flw::util::center_window(this, parent);
        show();

        while (_run == true) {
            Fl::wait();
            Fl::flush();
        }
    }
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

/** @brief Create table display widget.
*
* @param[in] X  X pos.
* @param[in] Y  Y pos.
* @param[in] W  Width.
* @param[in] H  Height.
* @param[in] l  Optional label.
*/
Display::Display(int X, int Y, int W, int H, const char* l) : Fl_Group(X, Y, W, H, l) {
    end();

    _hor  = new _Scrollbar(0, 0, 0, 0);
    _ver  = new _Scrollbar(0, 0, 0, 0);
    _edit = nullptr;

    add(_ver);
    add(_hor);

    _hor->callback(&_CallbackVer, this);
    _hor->linesize(10);
    _hor->type(FL_HORIZONTAL);
    _ver->callback(&_CallbackHor, this);
    _ver->linesize(10);

    resizable(nullptr);
    box(FL_BORDER_BOX);
    color(FL_BACKGROUND_COLOR);
    selection_color(FL_SELECTION_COLOR);
    tooltip(table::HELP_TEXT);
    clip_children(1);
    util::labelfont(this);
    reset();
}

/** @brief Set active cell.
*
* @param[in] row   Row number.
* @param[in] col   Column number.
* @param[in] show  True to show cell.
*/
void Display::active_cell(int row, int col, bool show) {
    auto send = false;

    if (_rows == 0 || row < 1 || row > _rows || _cols == 0 || col < 1 || col > _cols) {
        row = -1;
        col = -1;
    }

    if (_curr_row != row || _curr_col != col) {
        send = true;
    }

    _curr_row = row;
    _curr_col = col;

    if (send == true) {
        _set_event(_curr_row, _curr_col, Event::CURSOR);
        do_callback();
    }

    redraw();

    if (show == true && _curr_row > 0 && _curr_col > 0) {
        show_cell(_curr_row, _curr_col);
    }
}

/** @brief Callback for vertical scrollbar
*
*/
void Display::_CallbackVer(Fl_Widget*, void* o) {
    auto table = static_cast<Display*>(o);

    table->_start_col = table->_hor->value();
    table->redraw();
}

/** @brief Callback for horizontal scrollbar
*
*/
void Display::_CallbackHor(Fl_Widget*, void* o) {
    auto table = static_cast<Display*>(o);

    table->_start_row = table->_ver->value();
    table->redraw();
}

/** @brief Get cell height
*
* @param[in] Y  Y value.
*
* @return Cell height.
*/
int Display::_cell_height(int Y) {
    auto y2 = y() + h() - _hor->h();

    if (Y + _height >= y2) {
        return y2 - Y;
    }
    else {
        return _height;
    }
}

/** @brief Get cell width
*
* @param[in] col  Column index.
* @param[in] X    X pos.
*
* @return Cell width.
*/
int Display::_cell_width(int col, int X) {
    auto cw  = cell_width(col);
    auto min = _height;

    if (cw < min) {
        cw = min;
    }
    else if (cw > 3840) {
        cw = 3840;
    }

    if (X >= 0 && _expand && col == _cols) {
        return x() + w() - _ver->w() - X;
    }
    else {
        auto x2 = x() + w() - _ver->w();

        if (X + cw >= x2) {
            return x2 - X;
        }
    }

    return cw;
}

/** @brief Copy string from cell to clipboard.
*
*/
void Display::cmd_copy() {
    auto val = cell_value(_curr_row, _curr_col);
    Fl::copy(val.c_str(), static_cast<int>(val.length()), 1);
}

/** @brief Display find dialog and search for a string.
*
*/
void Display::cmd_find() {
    auto dlg = _FindDialog(this);
    dlg.run(window());
}

/** @brief Display goto cell dialog.
*
*/
void Display::cmd_goto() {
    auto dlg = _GotoDialog(_curr_row, _curr_col);

    if (dlg.run(window()) == true) {
        active_cell(dlg.row(), dlg.col(), true);
    }
}

/** @brief Print debug info to stdout.
*
*/
void Display::debug() const {
#ifdef DEBUG
    printf("flw::Display:\n");
    printf("    rows            = %4d\n", _rows);
    printf("    cols            = %4d\n", _cols);
    printf("    curr_row        = %4d\n", _curr_row);
    printf("    curr_col        = %4d\n", _curr_col);
    printf("    start_row       = %4d\n", _start_row);
    printf("    start_col       = %4d\n", _start_col);
    printf("    enable_keys     = %4d\n", _enable_keys);
    printf("    event           = %4d\n", (int) _event);
    printf("    event_row       = %4d\n", _event_row);
    printf("    event_col       = %4d\n", _event_col);
    printf("    height          = %4d\n", _height);
    printf("    resize_col      = %4d\n", _resize_col);
    printf("    show_col_header = %4d\n", _show_col_header);
    printf("    show_hor_lines  = %4d\n", _show_hor_lines);
    printf("    show_row_header = %4d\n", _show_row_header);
    printf("    show_ver_lines  = %4d\n", _show_ver_lines);
    printf("    resize          = %4d\n", _resize);
    printf("    expand          = %4d\n", _expand);
    printf("    disable_hor     = %4d\n", _disable_hor);
    printf("    disable_ver     = %4d\n", _disable_ver);
#endif
}

/** @brief Draw all cells.
*
*/
void Display::draw() {
    if (_edit != nullptr) { // Have to be resized before parent drawing.
        _edit->resize(_current_cell[0], _current_cell[1], _current_cell[2] + 1, _current_cell[3] + 1);
    }

    _update_scrollbars();
    Fl_Group::draw();

    if (_ver->visible() != 0 && _hor->visible() != 0) {
        fl_push_clip(x() + 1, y() + 1, w() - 2 - _ver->w(), h() - 2 - _hor->h());
    }
    else if (_ver->visible() != 0) {
        fl_push_clip(x() + 1, y() + 1, w() - 2 - _ver->w(), h() - 2);
    }
    else if (_hor->visible() != 0) {
        fl_push_clip(x() + 1, y() + 1, w() - 2, h() - 2 - _hor->h());
    }
    else {
        fl_push_clip(x() + 1, y() + 1, w() - 2, h() - 2);
    }

    auto w1 = w() - _ver->w();
    auto y1 = y();
    auto y2 = y() + h() - _hor->h();

    for (auto r = (_show_col_header ? 0 : _start_row); r <= _rows; r++) {
        auto ch = _cell_height(y1);

        _draw_row(r, w1, y1, ch);
        y1 += _height;

        if (y1 >= y2) {
            break;
        }
        else if (r == 0) { // After header start with first used row.
            r = _start_row - 1;
        }
    }

    if (_edit != nullptr) { // Make sure border are consistent around edit widget.
        fl_color(fl_lighter(FL_FOREGROUND_COLOR));
        fl_rect(_current_cell[0], _current_cell[1], _current_cell[2] + 1, _current_cell[3] + 1);
    }

    fl_pop_clip();
}

/** @brief Draw one cell
*
* @param[in] row      Row index.
* @param[in] col      Column index.
* @param[in] X        X pos.
* @param[in] Y        Y pos.
* @param[in] W        Cell width.
* @param[in] H        Cell height.
* @param[in] ver      Draw vertical line.
* @param[in] hor      Draw horizontal line.
* @param[in] current  True if cell is the current cell.
*/
void Display::_draw_cell(int row, int col, int X, int Y, int W, int H, bool ver, bool hor, bool current) {
    fl_push_clip(X, Y, W + 1, H);

    auto align    = cell_align(row, col);
    auto textfont = cell_textfont(row, col);
    auto textsize = cell_textsize(row, col);
    auto val      = cell_value(row, col);
    auto space    = (align & FL_ALIGN_RIGHT) ? 6 : 4;

    if (row > 0 && col > 0) { // Draw normal cell.
        auto color     = cell_color(row, col);
        auto textcolor = cell_textcolor(row, col);

        if (current == true) {
            color     = selection_color();
            textcolor = fl_contrast(FL_FOREGROUND_COLOR, color);
        }

        fl_color(color);
        fl_rectf(X + 1, Y, W + 1, H);
        fl_font(textfont, textsize);
        fl_color(textcolor);
        _draw_text(val.c_str(), X + space, Y + 2, W - space * 2, H - 4, align);
        fl_color(FL_DARK3);

        if (ver == true) {
            fl_line(X, Y, X, Y + H);
            fl_line(X + W, Y, X + W, Y + H);
        }

        if (hor == true) {
            fl_line(X, Y, X + W, Y);
            fl_line(X, Y + H - (row == _rows ? 1 : 0), X + W, Y + H - (row == _rows ? 1 : 0));
        }
    }
    else { // Draw header cell.
        fl_draw_box(FL_THIN_UP_BOX, X, Y, W + 1, H + (row == _rows ? 0 : 1), FL_BACKGROUND_COLOR);
        fl_font(textfont, textsize);
        fl_color(FL_FOREGROUND_COLOR);
        _draw_text(val.c_str(), X + space, Y + 2, W - space * 2, H - 4, align);
    }

    fl_pop_clip();
}

/** @brief Draw a row.
*
* @param[in] row  Row index.
* @param[in] W    Width.
* @param[in] Y    Y pos.
* @param[in] H    Cell height.
*/
void Display::_draw_row(int row, int W, int Y, int H) {
    auto x1 = x();
    auto x2 = x() + W;

    for (auto c = (_show_row_header ? 0 : _start_col); c <= _cols; c++) {
        auto cw = _cell_width(c, x1);

        if (row > 0 && c > 0 && row == _curr_row && c == _curr_col) {
            _current_cell[0] = x1;
            _current_cell[1] = Y;
            _current_cell[2] = cw;
            _current_cell[3] = H;

            if (_edit == nullptr) {
                _draw_cell(row, c, x1, Y, cw, H, _show_ver_lines, _show_hor_lines, _select != Select::NO);
            }
        }
        else if (row > 0 && row == _curr_row && _select == Select::ROW) {
            _draw_cell(row, c, x1, Y, cw, H, _show_ver_lines, _show_hor_lines, true);
        }
        else {
            _draw_cell(row, c, x1, Y, cw, H, _show_ver_lines, _show_hor_lines, false);
        }

        x1 += cw;

        if (x1 >= x2) {
            break;
        }
        else if (c == 0) { // After header start with first used column.
            c = _start_col - 1;
        }
    }

}

/** @brief Draw text.
*
* @param[in] string  String to draw.
* @param[in] X       X pos.
* @param[in] Y       Y pos.
* @param[in] W       Width.
* @param[in] H       Height.
* @param[in] align   Align text option.
*/
void Display::_draw_text(const char* string, int X, int Y, int W, int H, Fl_Align align) {
    if (align == FL_ALIGN_CENTER || align == FL_ALIGN_RIGHT) {
        if (fl_width(string) > W) {
            align = FL_ALIGN_LEFT;
        }
    }

    fl_draw(string, X, Y, W, H, align);
}

/** @brief Take care of keyboard events.
*
* @param[in] only_append_insert  True to only use append and insert.
*
* @return 1 if event is handled, 0 if not.
*/
int Display::_ev_keyboard_down(bool only_append_insert) {
    auto key   = Fl::event_key();
    auto cmd   = Fl::event_command() != 0;
    auto shift = Fl::event_shift() != 0;

    //printf("key=%d, %c, cmd=%d, shift=%d\n", key, key, cmd, shift); fflush(stdout);

    if (only_append_insert == true) {
        if (_enable_keys == false) {
        }
        else if (cmd == true && key == 'a') {
            _set_event(_curr_row, _curr_col, (shift == true) ? Event::APPEND_COLUMN : Event::APPEND_ROW);
            do_callback();
            return 1;
        }
        else if (cmd == true && key == 'i') {
            _set_event(_curr_row, _curr_col, (shift == true) ? Event::INSERT_COLUMN : Event::INSERT_ROW);
            do_callback();
            return 1;
        }
        else if (cmd == true && key == 'd') {
            _set_event(_curr_row, _curr_col, (shift == true) ? Event::DELETE_COLUMN : Event::DELETE_ROW);
            do_callback();
            return 1;
        }
    }
    else if (cmd == true && key == FL_Up && shift == false) {
        _move_cursor(Move::SCROLL_UP);
        return 1;
    }
    else if (key == FL_Up && shift == false) {
        _move_cursor(Move::UP);
        return 1;
    }
    else if (cmd == true && key == FL_Down && shift == false) {
        _move_cursor(Move::SCROLL_DOWN);
        return 1;
    }
    else if (key == FL_Down && shift == false) {
        _move_cursor(Move::DOWN);
        return 1;
    }
    else if (cmd == true && key == FL_Left && shift == false) {
        _move_cursor(Move::SCROLL_LEFT);
        return 1;
    }
    else if ((key == FL_Left && shift == false) || (key == FL_Tab && shift == true)) {
        _move_cursor(Move::LEFT);
        return 1;
    }
    else if (cmd == true && key == FL_Right && shift == false) {
        _move_cursor(Move::SCROLL_RIGHT);
        return 1;
    }
    else if ((key == FL_Right && shift == false) || (key == FL_Tab && shift == false)) {
        _move_cursor(Move::RIGHT);
        return 1;
    }
    else if (cmd == true && key == FL_Page_Up && shift == false) {
        _move_cursor(Move::FIRST_ROW);
        return 1;
    }
    else if (key == FL_Page_Up && shift == false) {
        _move_cursor(Move::PAGE_UP);
        return 1;
    }
    else if (cmd == true && key == FL_Page_Down && shift == false) {
        _move_cursor(Move::LAST_ROW);
        return 1;
    }
    else if (key == FL_Page_Down && shift == false) {
        _move_cursor(Move::PAGE_DOWN);
        return 1;
    }
    else if (key == FL_Home && shift == false) {
        _move_cursor(Move::FIRST_COL);
        return 1;
    }
    else if (key == FL_End && shift == false) {
        _move_cursor(Move::LAST_COL);
        return 1;
    }
    else if (cmd == true && key == 'c' && shift == false) {
        cmd_copy();
        return 1;
    }
    else if (cmd == true && key == 'g' && shift == false) {
        cmd_goto();
        return 1;
    }
    else if (cmd == true && key == 'f' && shift == false) {
        cmd_find();
        return 1;
    }
    else if (_enable_keys == false) {
    }
    else if (cmd == true && key == 'a') {
        _set_event(_curr_row, _curr_col, (shift == true) ? Event::APPEND_COLUMN : Event::APPEND_ROW);
        do_callback();
        return 1;
    }
    else if (cmd == true && key == 'i') {
        _set_event(_curr_row, _curr_col, (shift == true) ? Event::INSERT_COLUMN : Event::INSERT_ROW);
        do_callback();
        return 1;
    }
    else if (cmd == true && key == 'd') {
        _set_event(_curr_row, _curr_col, (shift == true) ? Event::DELETE_COLUMN : Event::DELETE_ROW);
        do_callback();
        return 1;
    }

    return 0;
}

/** @brief Mouse click events.
*
* If drag is under way return 1.
*
* @return 1 if event is handled, 0 if not.
*/
int Display::_ev_mouse_click () {
    if (Fl::event_button1() && _drag == true) {
        return 1;
    }

    auto r  = 0;
    auto c  = 0;
    auto cr = _curr_row;
    auto cc = _curr_col;

    _get_cell_below_mouse(r, c);

    if (_edit == nullptr) {
        Fl::focus(this);
    }

    if (r == 0 && c >= 1) { // Mouse click on top header cells
        _set_event(r, c, (Fl::event_ctrl() != 0) ? Event::COLUMN_CTRL : Event::COLUMN);
        do_callback();
    }
    else if (c == 0 && r >= 1) { // Mouse click on left header cells
        _set_event(r, c, (Fl::event_ctrl() != 0) ? Event::ROW_CTRL : Event::ROW);
        do_callback();
    }
    else if (r == -1 || c == -1) { // Mouse click outside cell
        if (r == -1 && _hor->visible() != 0 && Fl::event_y() >= _hor->y()) { // Don't deselect if clicked on scrollbar
            ;
        }
        else if (c == -1 && _ver->visible() != 0 && Fl::event_x() >= _ver->x()) { // Don't deselect if clicked on scrollbar
            ;
        }
        else { // If clicked in whitespace then deselect cell
            active_cell(-1, -1);
            return 0;
        }
    }
    else if (r >= 1 && c >= 1 && (r != cr || c != cc) && _select != Select::NO) { // Set new current cell and send event
        active_cell(r, c);
    }

    return 2;
}

/** @brief Mouse drag events.
*
* If drag is under way return 1.
*
* @return Return 2.
*/
int Display::_ev_mouse_drag() {
    if (_drag == false) {
        return 2;
    }

    auto xpos  = Fl::event_x();
    auto currx = x();

    if (_show_row_header && _resize_col == 0) {
        if ((xpos - currx) >= 10) {
            cell_width(_resize_col, xpos - currx);
            redraw();
        }
    }
    else {
        if (_show_row_header) {
            currx += _cell_width(0);
        }

        for (auto c = _start_col; c < _resize_col; c++) {
            currx += _cell_width(c);
        }

        if ((xpos - currx) >= 10) {
            cell_width(_resize_col, xpos - currx);
            redraw();
        }
    }

    return 2;
}

/** @brief Mouse move events.
*
* @return 0, 1 and 2.
*/
int Display::_ev_mouse_move() {
    auto mx = Fl::event_x();
    auto x1 = x() + (_show_row_header ? _cell_width(0) : 0);
    auto x2 = x() + w() - _ver->w();

    if (_resize) {
        if (_show_row_header && mx > (x1 - 3) && mx < (x1 + 4)) {
            _resize_col = 0;
            _drag       = true;

            fl_cursor(FL_CURSOR_WE);
            return 1;
        }
        else {
            for (auto c = _start_col; c <= _cols; c++) {
                x1 += _cell_width(c, x1);

                if (x1 >= x2) {
                    break;
                }
                else if (mx > (x1 - 3) && mx < (x1 + 4)) { // Change cursor to show that it is possible to change column width
                    _resize_col = c;
                    _drag       = true;

                    fl_cursor(FL_CURSOR_WE);
                    return 1;
                }
            }
        }
    }

    if (_drag == true) { // Set cursor to the default cursor only if it was set to another cursor before
        _drag = false;
        fl_cursor(FL_CURSOR_DEFAULT);
    }

    _resize_col = -1;
    return 2;
}

/** @brief Get cell index for cell below mouse cursor.
*
* @param[out] row  Row index.
* @param[out] col  Column index.
*/
void Display::_get_cell_below_mouse(int& row, int& col) {
    row = -1;
    col = -1;

    auto my = Fl::event_y();
    auto mx = Fl::event_x();

    if (!((_ver->visible() != 0 && mx >= _ver->x()) || (_hor->visible() != 0 && my >= _hor->y()))) { // Dont click on scrollbars
        if (_show_col_header == true && (my - y()) < _height) {
            row = 0;
        }
        else {
            auto y1 = y() + (_show_col_header ? _height : 0);
            auto y2 = y() + h();

            for (auto r = _start_row; r <= _rows; r++) {
                if (y1 > y2) {
                    break;
                }
                else if (my > y1 && my < y1 + _height) {
                    row = r;
                    break;
                }

                y1 += _height;
            }
        }

        if (_show_row_header == true && (mx - x()) < _cell_width(0)) {
            col = 0;
        }
        else {
            auto x1 = x() + (_show_row_header ? _cell_width(0) : 0);
            auto x2 = x() + w();

            for (auto c = _start_col; c <= _cols; c++) {
                auto cw = _cell_width(c);
                if (x1 > x2) {
                    break;
                }
                else if (mx > x1 && (mx < (x1 + cw) || (_expand == true && c == _cols))) {
                    col = c;
                    break;
                }

                x1 += cw;
            }
        }
    }
}

/** @brief Handle events.
*
* @param[in] event Event value.
*
* @return 1 if event is handled, 0 if not.
*/
int Display::handle(int event) {
    auto ret = 2;

    if (_rows > 0 && _cols > 0) {
        if (event == FL_FOCUS) {
            ret = 1;
        }
        else if (event == FL_LEAVE) {
            fl_cursor(FL_CURSOR_DEFAULT);
        }
        else if (event == FL_DRAG) {
            ret = _ev_mouse_drag();
        }
        else if (event == FL_KEYDOWN) {
            ret = _ev_keyboard_down();
        }
        else if (event == FL_MOVE) {
            ret = _ev_mouse_move();
        }
        else if (event == FL_PUSH) {
            ret = _ev_mouse_click();
        }
    }
    else if (event == FL_KEYDOWN) {
        ret = _ev_keyboard_down(true);
    }
    else if (event == FL_PUSH) {
        ret = _ev_mouse_click();
    }

    if (ret == 0 || ret == 1) {
        return ret;
    }
    else {
        return Fl_Group::handle(event);
    }
}

/** @brief Set show/hide header options
*
* @param[in] row  Show row header.
* @param[in] col  Show column header.
*/
void Display::header(bool row, bool col) {
    _show_row_header = row;
    _show_col_header = col;
    redraw();
}

/** @brief Set show/hide cell border line options
*
* @param[in] ver  Show vertical lines.
* @param[in] hor  Show horizontal lines.
*/
void Display::lines(bool ver, bool hor) {
    _show_ver_lines = ver;
    _show_hor_lines = hor;
    redraw();
}

/** @brief Move cursor (active cell).
*
* @param[in] move  Move option.
*/
void Display::_move_cursor(Move move) {
    if (_edit == nullptr && _rows > 0 && _cols > 0 && _select != Select::NO) {
        auto r     = _curr_row;
        auto c     = _curr_col;
        auto range = (int) ((h() - _hor->h() - (_show_row_header ? _height : 0)) / _height);

        if (r < 1) {
            r = 1;
        }
        else if (r > _rows) {
            r = _rows;
        }
        else {
            if (range > 0) {
                switch (move) {
                    case Move::FIRST_ROW:
                        r = 1;
                        break;

                    case Move::UP:
                        r = r > 1 ? r - 1 : 1;
                        break;

                    case Move::SCROLL_UP:
                        r = r > 8 ? r - 8 : 1;
                        break;

                    case Move::PAGE_UP:
                        r = _start_row - range > 0 ? _start_row - range : 1;
                        _start_row = r;
                        break;

                    case Move::DOWN:
                        r = r < _rows ? r + 1 : _rows;
                        break;

                    case Move::SCROLL_DOWN:
                        r = r < _rows - 8 ? r + 8 : _rows;
                        break;

                    case Move::PAGE_DOWN:
                        r = _start_row + range <= _rows ? _start_row + range : _rows;
                        _start_row = r;
                        break;

                    case Move::LAST_ROW:
                        r = _rows;
                        break;

                    default:
                        break;
                }
            }
        }

        if (c < 1) {
            c = 1;
        }
        else if (c > _cols) {
            c = _cols;
        }
        else {
            switch (move) {
                case Move::LEFT:
                    c = c > 1 ? c - 1 : 1;
                    break;

                case Move::SCROLL_LEFT:
                    c = c > 4 ? c - 4 : 1;
                    break;

                case Move::RIGHT:
                    c = c < _cols ? c + 1 : _cols;
                    break;

                case Move::SCROLL_RIGHT:
                    c = c < _cols - 4 ? c + 4 : _cols;
                    break;

                case Move::FIRST_COL:
                    c = 1;
                    break;

                case Move::LAST_COL:
                    c = _cols;
                    break;

                default:
                    break;
            }
        }

        if (r > 0 && r <= _rows && c > 0 && c <= _cols) {
            active_cell(r, c, true);
        }
    }
}

/** @brief Clear all values.
*
* Scrollbars are on.\n
* Headers, lines, resizing and expanding are off.\n
* Append shortcuts are disabled.\n
*/
void Display::reset() {
    _cols            = 0;
    _curr_col        = -1;
    _curr_row        = -1;
    _current_cell[0] = 0;
    _current_cell[1] = 0;
    _current_cell[2] = 0;
    _current_cell[3] = 0;
    _disable_hor     = false;
    _disable_ver     = false;
    _drag            = false;
    _edit            = nullptr;
    _enable_keys     = false;
    _event           = Event::UNDEFINED;
    _event_col       = -1;
    _event_row       = -1;
    _expand          = false;
    _height          = flw::PREF_FONTSIZE * 2;
    _resize          = false;
    _resize_col      = -1;
    _rows            = 0;
    _select          = Select::NO;
    _show_col_header = false;
    _show_hor_lines  = false;
    _show_row_header = false;
    _show_ver_lines  = false;
    _start_col       = 1;
    _start_row       = 1;
    _find            = "";
    redraw();
}

/** @brief Show cell.
*
* @param[in] row  Row index.
* @param[in] col  Column index.
*/
void Display::show_cell(int row, int col) {
    if (_rows > 0 && row > 0 && row <= _rows) {
        auto rc = (h() - Fl::scrollbar_size() - (_show_col_header ? _height : 0)) / _height;

        if (row <= _start_row) {
            _start_row = row;
        }
        else if (row >= _start_row + rc) {
            _start_row = row - rc + 1;
        }
    }

    if (_cols > 0 && col > 0 && col <= _cols) {
        if (col <= _start_col) {
            _start_col = col;
        }
        else {
            LOOP:

            if (_start_col < col) {
                auto x1 = x() + (_show_row_header ? _cell_width(0) : 0);
                auto x2 = x() + w() - Fl::scrollbar_size();

                for (auto c = _start_col; c <= col; c++) {
                    auto cw = _cell_width(c);

                    if (x1 + cw >= x2) {
                        if (col - c < 20) {
                            _start_col++;
                        }
                        else {
                            _start_col = col - 20;
                        }

                        goto LOOP;
                    }

                    x1 += cw;
                }
            }
        }
    }

    redraw();
}

/** @brief Set table size.
*
* @param[in] rows  Number of rows.
* @param[in] cols  Number of columns.
*/
void Display::size(int rows, int cols) {
    if (rows < 0 || _cols < 0) {
        return;
    }

    _rows      = rows;
    _cols      = cols;
    _curr_row  = 1;
    _curr_col  = 1;
    _start_row = 1;
    _start_col = 1;

    _set_event(_curr_row, _curr_col, Event::SIZE);
    do_callback();
    redraw();
}

/** @brief Show/hide scrollbars and fix size.
*
*/
void Display::_update_scrollbars() {
    if (_rows > 0 && _cols > 0) {
        if (_disable_hor == true) {
            _hor->hide();
        }
        else {
            auto cols  = 0;
            auto width = _show_row_header ? _cell_width(0) : 0;

            for (auto c = 1; c <= _cols; c++) {
                width += _cell_width(c);

                if (width > (w() - Fl::scrollbar_size())) {
                    break;
                }
                else {
                    cols++;
                }
            }

            if (_cols > cols) {
                _hor->slider_size(0.2);
                _hor->range(1, _cols);
                _hor->show();
            }
            else {
                _hor->hide();

                if (_start_col > 0) {
                    _start_col = 1;
                }
            }
        }


        if (_disable_ver == true) {
            _ver->hide();
        }
        else {
            auto r = (h() - Fl::scrollbar_size() - (_show_col_header ? _height : 0)) / _height;

            if (_rows > r) {
                _ver->slider_size(0.2);
                _ver->range(1, _rows);
                _ver->show();
            }
            else {
                _ver->hide();

                if (_start_row > 0) {
                    _start_row = 1;
                }
            }
        }
    }
    else {
        _ver->hide();
        _hor->hide();
    }

    if (_ver->visible() != 0 && _hor->visible() != 0) {
        _ver->resize(x() + w() - Fl::scrollbar_size() - 1, y() + 1, Fl::scrollbar_size(), h() - Fl::scrollbar_size() - 2);
        _hor->resize(x() + 1, y() + h() - Fl::scrollbar_size() - 1, w() - Fl::scrollbar_size() - 2, Fl::scrollbar_size());
    }
    else if (_ver->visible() != 0) {
        _ver->resize(x() + w() - Fl::scrollbar_size() - 1, y() + 1, Fl::scrollbar_size(), h() - 2);
        _hor->resize(0, 0, 0, 0);
    }
    else if (_hor->visible() != 0) {
        _hor->resize(x() + 1, y() + h() - Fl::scrollbar_size() - 1, w() - 2, Fl::scrollbar_size());
        _ver->resize(0, 0, 0, 0);
    }
    else {
        _ver->resize(0, 0, 0, 0);
        _hor->resize(0, 0, 0, 0);
    }

    _ver->Fl_Valuator::value(_start_row);
    _hor->Fl_Valuator::value(_start_col);
}

} // table
} // flw

// MKALGAM_OFF
