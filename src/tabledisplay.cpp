// Copyright 2016 - 2021 gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#include "tabledisplay.h"
#include <FL/Fl_Scrollbar.H>
#include <FL/Fl_Int_Input.H>
#include <FL/Fl_Double_Window.H>
#include <FL/fl_draw.H>

// MALAGMA_ON

namespace flw {
    //--------------------------------------------------------------------------
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

    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------

    //--------------------------------------------------------------------------
    class _TableDisplay_Scrollbar : public Fl_Scrollbar {
    public:
        //----------------------------------------------------------------------
        _TableDisplay_Scrollbar(int X, int Y, int W, int H) : Fl_Scrollbar(X, Y, W, H) {
        }

        //----------------------------------------------------------------------
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

    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------

    //--------------------------------------------------------------------------
    class _TableDisplayCellDialog : public Fl_Double_Window {
        Fl_Int_Input*                   _row;
        Fl_Int_Input*                   _col;
        bool                            _ret;

    public:
        _TableDisplayCellDialog(int row, int col) : Fl_Double_Window(0, 0, 0, 0, "Goto Cell") {
            end();

            _row = new Fl_Int_Input(0, 0, 0, 0, "Row:");
            _col = new Fl_Int_Input(0, 0, 0, 0, "Column:");
            _ret = false;

            add(_row);
            add(_col);

            _row->align(FL_ALIGN_LEFT | FL_ALIGN_TOP);
            _row->callback(_TableDisplayCellDialog::Callback, this);
            _row->labelsize(flw::PREF_FONTSIZE);
            _row->textfont(flw::PREF_FIXED_FONT);
            _row->textsize(flw::PREF_FONTSIZE);
            _row->when(FL_WHEN_ENTER_KEY_ALWAYS);
            _col->align(FL_ALIGN_LEFT | FL_ALIGN_TOP);
            _col->callback(_TableDisplayCellDialog::Callback, this);
            _col->labelsize(flw::PREF_FONTSIZE);
            _col->textfont(flw::PREF_FIXED_FONT);
            _col->textsize(flw::PREF_FONTSIZE);
            _col->when(FL_WHEN_ENTER_KEY_ALWAYS);

            char b[100];
            sprintf(b, "%d", row > 0 ? row : 1);
            _row->value(b);
            sprintf(b, "%d", col > 0 ? col : 1);
            _col->value(b);

            callback(_TableDisplayCellDialog::Callback, this);
            set_modal();
            resizable(this);
            resize(0, 0, flw::PREF_FONTSIZE * 20, flw::PREF_FONTSIZE * 7);
        }

        //----------------------------------------------------------------------
        static void Callback(Fl_Widget* w, void* o) {
            auto dlg = (_TableDisplayCellDialog*) o;

            if (w == dlg) {
                dlg->hide();
            }
            else if (w == dlg->_row) {
                dlg->_ret = true;
                dlg->hide();
            }
            else if (w == dlg->_col) {
                dlg->_ret = true;
                dlg->hide();
            }
        }

        //----------------------------------------------------------------------
        int col() const {
            return *_col->value() >= '0' && *_col->value() <= '9' ? atoi(_col->value()) : 0;
        }

        //----------------------------------------------------------------------
        void resize(int X, int Y, int W, int H) {
            Fl_Double_Window::resize(X, Y, W, H);

            _row->resize(4, flw::PREF_FONTSIZE + 4, W - 8, flw::PREF_FONTSIZE * 2);
            _col->resize(4, flw::PREF_FONTSIZE * 4 + 8, W - 8, flw::PREF_FONTSIZE * 2);
        }

        //----------------------------------------------------------------------
        int row() const {
            return *_row->value() >= '0' && *_row->value() <= '9' ? atoi(_row->value()) : 0;
        }

        //----------------------------------------------------------------------
        bool run(Fl_Window* parent) {
            flw::util::center_window(this, parent);
            show();

            while (visible()) {
                Fl::wait();
                Fl::flush();
            }

            return _ret;
        }
    };
}

//------------------------------------------------------------------------------
flw::TableDisplay::TableDisplay(int X, int Y, int W, int H, const char* l) : Fl_Group(X, Y, W, H, l) {
    end();

    _hor  = new _TableDisplay_Scrollbar(0, 0, 0, 0);
    _ver  = new _TableDisplay_Scrollbar(0, 0, 0, 0);
    _edit = nullptr;

    add(_ver);
    add(_hor);

    _hor->callback(&_CallbackVer, this);
    _hor->linesize(10);
    _hor->type(FL_HORIZONTAL);
    _ver->callback(&_CallbackHor, this);
    _ver->linesize(10);

    box(FL_BORDER_BOX);
    color(FL_BACKGROUND_COLOR);
    selection_color(FL_SELECTION_COLOR);
    clip_children(1);
    flw::util::labelfont(this);
    clear();
}

//------------------------------------------------------------------------------
void flw::TableDisplay::active_cell(int row, int col, bool show) {
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

    if (send) {
        _event_set(_curr_row, _curr_col, flw::TableDisplay::EVENT::CURSOR);
        do_callback();
    }

    redraw();

    if (show &&
        _curr_row > 0 &&
        _curr_col > 0) {

        show_cell(_curr_row, _curr_col);
    }
}

//------------------------------------------------------------------------------
void flw::TableDisplay::_CallbackVer(Fl_Widget*, void* o) {
    auto table = (flw::TableDisplay*) o;

    table->_start_col = table->_hor->value();
    table->redraw();
}

//------------------------------------------------------------------------------
 void flw::TableDisplay::_CallbackHor(Fl_Widget*, void* o) {
    auto table = (TableDisplay*) o;

    table->_start_row = table->_ver->value();
    table->redraw();
}

//------------------------------------------------------------------------------
int flw::TableDisplay::_cell_height(int Y) {
    auto y2 = y() + h() - _hor->h();

    if (Y + _height >= y2) {
        return y2 - Y;
    }
    else {
        return _height;
    }
}

//------------------------------------------------------------------------------
int flw::TableDisplay::_cell_width(int col, int X) {
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

//------------------------------------------------------------------------------
void flw::TableDisplay::clear() {
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
    _event           = flw::TableDisplay::EVENT::UNDEFINED;
    _event_col       = -1;
    _event_row       = -1;
    _expand          = false;
    _height          = flw::PREF_FONTSIZE * 2;
    _resize          = false;
    _resize_col      = -1;
    _rows            = 0;
    _select          = flw::TableDisplay::SELECT::NO;
    _show_col_header = false;
    _show_hor_lines  = false;
    _show_row_header = false;
    _show_ver_lines  = false;
    _start_col       = 1;
    _start_row       = 1;

    redraw();
}

//------------------------------------------------------------------------------
void flw::TableDisplay::draw() {
    if (_edit) { // Have to be resized before parent drawing
        _edit->resize(_current_cell[0], _current_cell[1], _current_cell[2] + 1, _current_cell[3] + 1);
    }

    _update_scrollbars();
    Fl_Group::draw();

    if (_ver->visible() && _hor->visible()) {
        fl_push_clip(x() + 1, y() + 1, w() - 2 - _ver->w(), h() - 2 - _hor->h());
    }
    else if (_ver->visible()) {
        fl_push_clip(x() + 1, y() + 1, w() - 2 - _ver->w(), h() - 2);
    }
    else if (_hor->visible()) {
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
        else if (r == 0) { // After header start with first used row
            r = _start_row - 1;
        }
    }

    if (_edit) { // Make sure border are consistent around edit widget
        fl_color(fl_lighter(FL_FOREGROUND_COLOR));
        fl_rect(_current_cell[0], _current_cell[1], _current_cell[2] + 1, _current_cell[3] + 1);
    }

    fl_pop_clip();
}

//------------------------------------------------------------------------------
void flw::TableDisplay::_draw_cell(int row, int col, int X, int Y, int W, int H, bool ver, bool hor, bool current) {
    fl_push_clip(X, Y, W + 1, H);

    auto align    = cell_align(row, col);
    auto textfont = cell_textfont(row, col);
    auto textsize = cell_textsize(row, col);
    auto val      = cell_value(row, col);
    auto space    = (align & FL_ALIGN_RIGHT) ? 6 : 4;

    if (row > 0 && col > 0) { // Draw normal cell
        auto color     = cell_color(row, col);
        auto textcolor = cell_textcolor(row, col);

        if (current) {
            color = selection_color();
        }

        fl_color(color);
        fl_rectf(X + 1, Y, W + 1, H);
        fl_font(textfont, textsize);
        fl_color(textcolor);
        fl_draw(val, X + space, Y + 2, W - space * 2, H - 4, align, 0, 1);

        fl_color(FL_DARK3);

        if (ver) {
            fl_line(X, Y, X, Y + H);
            fl_line(X + W, Y, X + W, Y + H);
        }

        if (hor) {
            fl_line(X, Y, X + W, Y);
            fl_line(X, Y + H - (row == _rows ? 1 : 0), X + W, Y + H - (row == _rows ? 1 : 0));
        }
    }
    else { // Draw header cell
        fl_draw_box(FL_THIN_UP_BOX, X, Y, W + 1, H + (row == _rows ? 0 : 1), FL_BACKGROUND_COLOR);
        fl_font(textfont, textsize);
        fl_color(FL_FOREGROUND_COLOR);
        fl_draw(val, X + space, Y + 2, W - space * 2, H - 4, align, 0, 1);
    }

    fl_pop_clip();
}

//------------------------------------------------------------------------------
void flw::TableDisplay::_draw_row(int row, int W, int Y, int H) {
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
                _draw_cell(row, c, x1, Y, cw, H, _show_ver_lines, _show_hor_lines, _select != flw::TableDisplay::SELECT::NO);
            }
        }
        else if (row > 0 && row == _curr_row && _select == flw::TableDisplay::SELECT::ROW) {
            _draw_cell(row, c, x1, Y, cw, H, _show_ver_lines, _show_hor_lines, true);
        }
        else {
            _draw_cell(row, c, x1, Y, cw, H, _show_ver_lines, _show_hor_lines, false);
        }

        x1 += cw;

        if (x1 >= x2) {
            break;
        }
        else if (c == 0) { // After header start with first used column
            c = _start_col - 1;
        }
    }

}

//------------------------------------------------------------------------------
int flw::TableDisplay::_ev_keyboard_down() {
    auto key   = Fl::event_key();
    auto text  = std::string(Fl::event_text());
    auto cmd   = Fl::event_command() != 0;
    auto shift = Fl::event_shift() != 0;

    // printf("key=%d <%s>, alt=%d, cmd=%d, shift=%d\n", key, text.c_str(), alt, cmd, shift); fflush(stdout);

    if (cmd && key == FL_Up) {
        move_cursor((int) _TABLEDISPLAY_MOVE::SCROLL_UP);
        return 1;
    }
    else if (key == FL_Up) {
        move_cursor((int) _TABLEDISPLAY_MOVE::UP);
        return 1;
    }
    else if (cmd && key == FL_Down) {
        move_cursor((int) _TABLEDISPLAY_MOVE::SCROLL_DOWN);
        return 1;
    }
    else if (key == FL_Down) {
        move_cursor((int) _TABLEDISPLAY_MOVE::DOWN);
        return 1;
    }
    else if (cmd && key == FL_Left) {
        move_cursor((int) _TABLEDISPLAY_MOVE::SCROLL_LEFT);
        return 1;
    }
    else if (key == FL_Left || (key == FL_Tab && shift)) {
        move_cursor((int) _TABLEDISPLAY_MOVE::LEFT);
        return 1;
    }
    else if (cmd && key == FL_Right) {
        move_cursor((int) _TABLEDISPLAY_MOVE::SCROLL_RIGHT);
        return 1;
    }
    else if (key == FL_Right || key == FL_Tab) {
        move_cursor((int) _TABLEDISPLAY_MOVE::RIGHT);
        return 1;
    }
    else if (cmd && key == FL_Page_Up) {
        move_cursor((int) _TABLEDISPLAY_MOVE::FIRST_ROW);
        return 1;
    }
    else if (key == FL_Page_Up) {
        move_cursor((int) _TABLEDISPLAY_MOVE::PAGE_UP);
        return 1;
    }
    else if (cmd && key == FL_Page_Down) {
        move_cursor((int) _TABLEDISPLAY_MOVE::LAST_ROW);
        return 1;
    }
    else if (key == FL_Page_Down) {
        move_cursor((int) _TABLEDISPLAY_MOVE::PAGE_DOWN);
        return 1;
    }
    else if (key == FL_Home) {
        move_cursor((int) _TABLEDISPLAY_MOVE::FIRST_COL);
        return 1;
    }
    else if (key == FL_End) {
        move_cursor((int) _TABLEDISPLAY_MOVE::LAST_COL);
        return 1;
    }
    else if (cmd && key == 'c') {
        auto val = cell_value(_curr_row, _curr_col);

        Fl::copy(val, strlen(val), 1);
        return 1;
    }
    else if (cmd && key == 'g') {
        auto dlg = _TableDisplayCellDialog(_curr_row, _curr_col);

        if (dlg.run(window())) {
            active_cell(dlg.row(), dlg.col(), true);
        }

        return 1;
    }

    return 0;
}

//------------------------------------------------------------------------------
int flw::TableDisplay::_ev_mouse_click () {
    if (Fl::event_button1() && _drag) {
        return 1;
    }
    else {
        auto row         = 0;
        auto col         = 0;
        auto current_row = _curr_row;
        auto current_col = _curr_col;

        _get_cell_below_mouse(row, col);

        if (_edit == nullptr) {
            Fl::focus(this);
        }

        if (row == 0 && col >= 1) { // Mouse click on top header cells
            _event_set(row, col, Fl::event_ctrl() ? flw::TableDisplay::EVENT::COLUMN_CTRL : flw::TableDisplay::EVENT::COLUMN);
            do_callback();
        }
        else if (col == 0 && row >= 1) { // Mouse click on left header cells
            _event_set(row, col, Fl::event_ctrl() ? flw::TableDisplay::EVENT::ROW_CTRL : flw::TableDisplay::EVENT::ROW);
            do_callback();
        }
        else if (row == -1 || col == -1) { // Mouse click outside cell
            if (row == -1 && _hor->visible() && Fl::event_y() >= _hor->y()) { // Don't deselect if clicked on scrollbar
                ;
            }
            else if (col == -1 && _ver->visible() && Fl::event_x() >= _ver->x()) { // Don't deselect if clicked on scrollbar
                ;
            }
            else { // If clicked in whitespace then deselect cell
                active_cell(-1, -1);
                return 0;
            }
        }
        else if (row >= 1 && col >= 1 && (row != current_row || col != current_col) && _select != flw::TableDisplay::SELECT::NO) { // Set new current cell and send event
            active_cell(row, col);
        }

        return 2;
    }
}

//------------------------------------------------------------------------------
int flw::TableDisplay::_ev_mouse_drag() {
    if (_drag) {
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
    }

    return 2;
}

//------------------------------------------------------------------------------
int flw::TableDisplay::_ev_mouse_move() {
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

    if (_drag) { // Set cursor to the default cursor only if it was set to another cursor before
        _drag = false;
        fl_cursor(FL_CURSOR_DEFAULT);
    }

    _resize_col = -1;
    return 2;
}

//------------------------------------------------------------------------------
void flw::TableDisplay::_get_cell_below_mouse(int& row, int& col) {
    row = -1;
    col = -1;

    auto my = Fl::event_y();
    auto mx = Fl::event_x();

    if (!((_ver->visible() && mx >= _ver->x()) || (_hor->visible() && my >= _hor->y()))) { // Dont click on scrollbars
        if (_show_col_header && (my - y()) < _height) {
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

        if (_show_row_header && (mx - x()) < _cell_width(0)) {
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
                else if (mx > x1 && (mx < x1 + cw || c == _cols)) {
                    col = c;
                    break;
                }

                x1 += cw;
            }
        }
    }
}

//------------------------------------------------------------------------------
int flw::TableDisplay::handle(int event) {
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

    if (ret == 0 || ret == 1) {
        return ret;
    }
    else {
        return Fl_Group::handle(event);
    }
}

//------------------------------------------------------------------------------
void flw::TableDisplay::header(bool row, bool col) {
    _show_row_header = row;
    _show_col_header = col;
    redraw();
}

//------------------------------------------------------------------------------
void flw::TableDisplay::lines(bool ver, bool hor) {
    _show_ver_lines = ver;
    _show_hor_lines = hor;
    redraw();
}

//------------------------------------------------------------------------------
void flw::TableDisplay::move_cursor(int pos) {
    if (_edit == nullptr && _rows > 0 && _cols > 0 && _select != flw::TableDisplay::SELECT::NO) {
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
                switch ((_TABLEDISPLAY_MOVE) pos) {
                    case _TABLEDISPLAY_MOVE::FIRST_ROW:
                        r = 1;
                        break;

                    case _TABLEDISPLAY_MOVE::UP:
                        r = r > 1 ? r - 1 : 1;
                        break;

                    case _TABLEDISPLAY_MOVE::SCROLL_UP:
                        r = r > 8 ? r - 8 : 1;
                        break;

                    case _TABLEDISPLAY_MOVE::PAGE_UP:
                        r = _start_row - range > 0 ? _start_row - range : 1;
                        _start_row = r;
                        break;

                    case _TABLEDISPLAY_MOVE::DOWN:
                        r = r < _rows ? r + 1 : _rows;
                        break;

                    case _TABLEDISPLAY_MOVE::SCROLL_DOWN:
                        r = r < _rows - 8 ? r + 8 : _rows;
                        break;

                    case _TABLEDISPLAY_MOVE::PAGE_DOWN:
                        r = _start_row + range <= _rows ? _start_row + range : _rows;
                        _start_row = r;
                        break;

                    case _TABLEDISPLAY_MOVE::LAST_ROW:
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
            switch ((_TABLEDISPLAY_MOVE) pos) {
                case _TABLEDISPLAY_MOVE::LEFT:
                    c = c > 1 ? c - 1 : 1;
                    break;

                case _TABLEDISPLAY_MOVE::SCROLL_LEFT:
                    c = c > 4 ? c - 4 : 1;
                    break;

                case _TABLEDISPLAY_MOVE::RIGHT:
                    c = c < _cols ? c + 1 : _cols;
                    break;

                case _TABLEDISPLAY_MOVE::SCROLL_RIGHT:
                    c = c < _cols - 4 ? c + 4 : _cols;
                    break;

                case _TABLEDISPLAY_MOVE::FIRST_COL:
                    c = 1;
                    break;

                case _TABLEDISPLAY_MOVE::LAST_COL:
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

//------------------------------------------------------------------------------
void flw::TableDisplay::show_cell(int row, int col) {
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

//------------------------------------------------------------------------------
void flw::TableDisplay::size(int rows, int cols) {
    if (rows > -1 && _cols > -1) {
        _rows      = rows;
        _cols      = cols;
        _curr_row  = 1;
        _curr_col  = 1;
        _start_row = 1;
        _start_col = 1;

        _event_set(_curr_row, _curr_col, flw::TableDisplay::EVENT::SIZE);
        do_callback();
    }

    redraw();
}

//------------------------------------------------------------------------------
void flw::TableDisplay::_update_scrollbars() {
    if (_rows > 0 && _cols > 0) {
        if (_disable_hor) {
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


        if (_disable_ver) {
            _ver->hide();
        }
        else {
            auto rows = (h() - Fl::scrollbar_size() - (_show_col_header ? _height : 0)) / _height;

            if (_rows > rows) {
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

    if (_ver->visible() && _hor->visible()) {
        _ver->resize(x() + w() - Fl::scrollbar_size() - 1, y() + 1, Fl::scrollbar_size(), h() - Fl::scrollbar_size() - 2);
        _hor->resize(x() + 1, y() + h() - Fl::scrollbar_size() - 1, w() - Fl::scrollbar_size() - 2, Fl::scrollbar_size());
    }
    else if (_ver->visible()) {
        _ver->resize(x() + w() - Fl::scrollbar_size() - 1, y() + 1, Fl::scrollbar_size(), h() - 2);
        _hor->resize(0, 0, 0, 0);
    }
    else if (_hor->visible()) {
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

// MALAGMA_OFF
