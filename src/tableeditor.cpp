// Copyright gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#include "tableeditor.h"
#include "datechooser.h"
#include "dlg.h"

// MKALGAM_ON

#include <assert.h>
#include <errno.h>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_File_Chooser.H>
#include <FL/Fl_Float_Input.H>
#include <FL/Fl_Input_Choice.H>
#include <FL/Fl_Int_Input.H>
#include <FL/Fl_Secret_Input.H>
#include <FL/Fl_Value_Slider.H>
#include <FL/fl_draw.H>
#include <FL/fl_show_colormap.H>

namespace flw {

/***
 *                 _            _       
 *                (_)          | |      
 *      _ __  _ __ ___   ____ _| |_ ___ 
 *     | '_ \| '__| \ \ / / _` | __/ _ \
 *     | |_) | |  | |\ V / (_| | ||  __/
 *     | .__/|_|  |_| \_/ \__,_|\__\___|
 *     | |                              
 *     |_|                              
 */

//----------------------------------------------------------------------
int _tableeditor_to_doubles(std::string string, double numbers[], size_t size) {
    auto end = (char*) nullptr;
    auto in  = string.c_str();
    auto f   = (size_t) 0;

    errno = 0;

    for (; f < size; f++) {
        numbers[f] = strtod(in, &end);
        if (errno != 0 || in == end) return f;
        in = end;
    }

    return f;
}
//----------------------------------------------------------------------
int64_t _tableeditor_to_int(std::string string, int64_t def) {
    try { return std::stoll(string.c_str(), 0, 0); }
    catch (...) { return def; }
}

//----------------------------------------------------------------------
// Return number of set integers
//
int _tableeditor_to_ints(std::string string, int64_t numbers[], size_t size) {
    auto end = (char*) nullptr;
    auto in  = string.c_str();
    auto f   = (size_t) 0;

    errno = 0;

    for (; f < size; f++) {
        numbers[f] = strtoll(in, &end, 10);
        if (errno != 0 || in == end) return f;
        in = end;
    }

    return f;
}

/***
 *      _______    _     _      ______    _ _ _             
 *     |__   __|  | |   | |    |  ____|  | (_) |            
 *        | | __ _| |__ | | ___| |__   __| |_| |_ ___  _ __ 
 *        | |/ _` | '_ \| |/ _ \  __| / _` | | __/ _ \| '__|
 *        | | (_| | |_) | |  __/ |___| (_| | | || (_) | |   
 *        |_|\__,_|_.__/|_|\___|______\__,_|_|\__\___/|_|   
 *                                                          
 *                                                          
 */

const char* TableEditor::SELECT_DATE = "Select Date";
const char* TableEditor::SELECT_DIR  = "Select Directory";
const char* TableEditor::SELECT_FILE = "Select File";
const char* TableEditor::SELECT_LIST = "Select String";

//------------------------------------------------------------------------------
TableEditor::TableEditor(int X, int Y, int W, int H, const char* l) : TableDisplay(X, Y, W, H, l) {
    TableEditor::clear();
}

//------------------------------------------------------------------------------
void TableEditor::clear() {
    TableDisplay::clear();

    _send_changed_event_always = false;
    _edit2 = nullptr;
    _edit3 = nullptr;
}

//------------------------------------------------------------------------------
void TableEditor::_delete_current_cell() {
    if (_curr_row > 0 && _curr_col > 0) {
        auto edit = cell_edit(_curr_row, _curr_col);

        if (edit == true) {
            auto rend = cell_rend(_curr_row, _curr_col);
            auto set  = false;

            switch (rend) {
                case TableEditor::REND::TEXT:
                case TableEditor::REND::SECRET:
                case TableEditor::REND::INPUT_CHOICE:
                case TableEditor::REND::DLG_FILE:
                case TableEditor::REND::DLG_DIR:
                case TableEditor::REND::DLG_LIST:
                    set = cell_value(_curr_row, _curr_col, "");
                    break;

                case TableEditor::REND::INTEGER:
                    set = cell_value(_curr_row, _curr_col, "0");
                    break;

                case TableEditor::REND::NUMBER:
                    set = cell_value(_curr_row, _curr_col, "0.0");
                    break;

                case TableEditor::REND::BOOLEAN:
                    set = cell_value(_curr_row, _curr_col, "0");
                    break;

                case TableEditor::REND::DLG_DATE:
                    set = cell_value(_curr_row, _curr_col, "1970-01-01");
                    break;

                case TableEditor::REND::DLG_COLOR:
                    set = cell_value(_curr_row, _curr_col, "56");
                    break;

                default:
                    break;
            }

            if (set == true) {
                _set_event(_curr_row, _curr_col, TableEditor::EVENT::CHANGED);
                do_callback();
                redraw();
            }
        }
    }
}

//------------------------------------------------------------------------------
void TableEditor::_draw_cell(int row, int col, int X, int Y, int W, int H, bool ver, bool hor, bool current) {
    fl_push_clip(X, Y, W + 1, H);

    auto align     = cell_align(row, col);
    auto textcolor = cell_textcolor(row, col);
    auto textfont  = cell_textfont(row, col);
    auto textsize  = cell_textsize(row, col);
    auto val       = static_cast<TableDisplay*>(this)->cell_value(row, col);

    assert(val);

    if (row > 0 && col > 0) { // Draw normal cell
        auto format = cell_format(row, col);
        auto rend   = cell_rend(row, col);
        auto color  = cell_color(row, col);
        char buffer[100];

        if (current == true) {
            color     = selection_color();
            textcolor = fl_contrast(FL_FOREGROUND_COLOR, color);
        }

        fl_rectf(X + 1, Y, W + 1, H, color);

        if (rend == TableEditor::REND::SECRET) {
            fl_font(textfont, textsize);
            fl_color(textcolor);

            if (format == TableEditor::FORMAT::SECRET_DOT) {
                fl_draw("••••••", X + 4, Y + 2, W - 8, H - 4, align, 0, 1);
            }
            else {
                fl_draw("******", X + 4, Y + 2, W - 8, H - 4, align, 0, 1);
            }
        }
        else if (rend == TableEditor::REND::SLIDER) {
            double nums[4];

            if (_tableeditor_to_doubles(val, nums, 4) == 4) {
                auto range = 0.0;

                if ((nums[2] - nums[1]) > 0.0001) {
                    range = (nums[0] - nums[1]) / (nums[2] - nums[1]);
                }

                auto width = (int) (range * (W - 3));

                if (width > 0) {
                    fl_draw_box(FL_FLAT_BOX, X + 2, Y + 2, width, H - 3, textcolor);
                }
            }
        }
        else if (rend == TableEditor::REND::DLG_COLOR) {
            fl_draw_box(FL_FLAT_BOX, X + 2, Y + 2, W - 3, H - 3, (Fl_Color) _tableeditor_to_int(val, 0));
        }
        else if (rend == TableEditor::REND::DLG_DATE) {
            auto        date = gnu::Date::FromString(val);
            std::string string;

            if (format == TableEditor::FORMAT::DATE_WORLD) {
                string = date.format(gnu::Date::FORMAT::WORLD);
            }
            else if (format == TableEditor::FORMAT::DATE_US) {
                string = date.format(gnu::Date::FORMAT::US);
            }
            else {
                string = date.format(gnu::Date::FORMAT::ISO_LONG);
            }

            fl_font(textfont, textsize);
            fl_color(textcolor);
            _draw_text(string.c_str(), X + 4, Y + 2, W - 8, H - 4, align);
        }
        else if (rend == TableEditor::REND::BOOLEAN) {
            auto bw  = textsize;
            auto y_1 = Y + (H / 2) - (bw / 2);
            auto x_1 = 0;

            if (align == FL_ALIGN_RIGHT) {
                x_1 = X + W - bw - 6;
            }
            else if (align == FL_ALIGN_CENTER) {
                x_1 = X + (W / 2) - (bw / 2);
            }
            else if (align == FL_ALIGN_LEFT) {
                x_1 = X + 6;
            }

            fl_draw_box(FL_DOWN_BOX, x_1, y_1, bw, bw, FL_WHITE);

            if (*val == '1') {
                Fl_Rect r(x_1, y_1, bw - 1, bw - 1);
                fl_draw_check(r, selection_color());
            }
        }
        else if (rend == TableEditor::REND::INTEGER) {
            auto num = _tableeditor_to_int(val, 0);

            fl_font(textfont, textsize);
            fl_color(textcolor);

            if (format == TableEditor::FORMAT::INT_SEP) {
                auto s = util::format_int(num);
                _draw_text(s.c_str(), X + 4, Y + 2, W - 8, H - 4, align);
            }
            else {
                snprintf(buffer, 100, "%lld", (long long int) num);
                _draw_text(buffer, X + 4, Y + 2, W - 8, H - 4, align);
            }
        }
        else if (rend == TableEditor::REND::NUMBER || rend == TableEditor::REND::VALUE_SLIDER) {
            auto num = util::to_double(val, 0.0);

            if (rend == TableEditor::REND::VALUE_SLIDER) {
                double nums[1];

                if (_tableeditor_to_doubles(val, nums, 1) == 1) {
                    num = nums[0];
                }
            }

            if (format == TableEditor::FORMAT::DEC_0) {
                snprintf(buffer, 100, "%.0f", num);
            }
            else if (format == TableEditor::FORMAT::DEC_1) {
                snprintf(buffer, 100, "%.1f", num);
            }
            else if (format == TableEditor::FORMAT::DEC_2) {
                snprintf(buffer, 100, "%.2f", num);
            }
            else if (format == TableEditor::FORMAT::DEC_3) {
                snprintf(buffer, 100, "%.3f", num);
            }
            else if (format == TableEditor::FORMAT::DEC_4) {
                snprintf(buffer, 100, "%.4f", num);
            }
            else if (format == TableEditor::FORMAT::DEC_5) {
                snprintf(buffer, 100, "%.5f", num);
            }
            else if (format == TableEditor::FORMAT::DEC_6) {
                snprintf(buffer, 100, "%.6f", num);
            }
            else if (format == TableEditor::FORMAT::DEC_7) {
                snprintf(buffer, 100, "%.7f", num);
            }
            else if (format == TableEditor::FORMAT::DEC_8) {
                snprintf(buffer, 100, "%.8f", num);
            }
            else {
                snprintf(buffer, 100, "%f", num);
            }

            fl_font(textfont, textsize);
            fl_color(textcolor);
            _draw_text(buffer, X + 4, Y + 2, W - 8, H - 4, align);
        }
        else { // Draw plain text string
            fl_font(textfont, textsize);
            fl_color(textcolor);
            _draw_text(val, X + 4, Y + 2, W - 8, H - 4, align);
        }

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
    else { // Draw header cell
        fl_draw_box(FL_THIN_UP_BOX, X, Y, W + 1, H + (row == _rows ? 0 : 1), FL_BACKGROUND_COLOR);
        fl_font(textfont, textsize);
        fl_color(FL_FOREGROUND_COLOR);
        _draw_text(val, X + 4, Y + 2, W - 8, H - 4, align);
    }

    fl_pop_clip();
}

//------------------------------------------------------------------------------
void TableEditor::_edit_create() {
    auto rend      = cell_rend(_curr_row, _curr_col);
    auto align     = cell_align(_curr_row, _curr_col);
    auto color     = cell_color(_curr_row, _curr_col);
    auto textcolor = FL_FOREGROUND_COLOR;
    auto textfont  = cell_textfont(_curr_row, _curr_col);
    auto textsize  = cell_textsize(_curr_row, _curr_col);
    auto val       = static_cast<TableDisplay*>(this)->cell_value(_curr_row, _curr_col);

    assert(val);

    if (rend == TableEditor::REND::TEXT ||
        rend == TableEditor::REND::INTEGER ||
        rend == TableEditor::REND::NUMBER ||
        rend == TableEditor::REND::SECRET) {

        auto w = (Fl_Input*) nullptr;

        if (rend == TableEditor::REND::TEXT) {
            w = new Fl_Input(0, 0, 0, 0);
        }
        else if (rend == TableEditor::REND::INTEGER) {
            w = new Fl_Int_Input(0, 0, 0, 0);
        }
        else if (rend == TableEditor::REND::NUMBER) {
            w = new Fl_Float_Input(0, 0, 0, 0);
        }
        else if (rend == TableEditor::REND::SECRET) {
            w = new Fl_Secret_Input(0, 0, 0, 0);
        }

        w->align(align);
        w->box(FL_BORDER_BOX);
        w->color(color);
        w->labelsize(textsize);
        w->textcolor(textcolor);
        w->textfont(textfont);
        w->textsize(textsize);
        w->value(val);
        w->insert_position(0, strlen(val));

        _edit = w;
    }
    else if (rend == TableEditor::REND::BOOLEAN) {
        auto w = new Fl_Check_Button(0, 0, 0, 0);

        w->box(FL_BORDER_BOX);
        w->color(color);
        w->labelcolor(textcolor);
        w->labelsize(textsize);
        w->value(*val && val[0] == '1' ? 1 : 0);

        _edit = w;
    }
    else if (rend == TableEditor::REND::SLIDER) {
        auto w = (Fl_Slider*) nullptr;
        double nums[4];

        if (_tableeditor_to_doubles(val, nums, 4) == 4) {
            w = new Fl_Slider(0, 0, 0, 0);
            w->color(color);
            w->selection_color(textcolor);
            w->range(nums[1], nums[2]);
            w->step(nums[3]);
            w->value(nums[0]);
            w->type(FL_HOR_FILL_SLIDER);
            w->box(FL_BORDER_BOX);

            _edit = w;
        }
    }
    else if (rend == TableEditor::REND::VALUE_SLIDER) {
        auto w = (Fl_Slider*) nullptr;
        double nums[4];

        if (_tableeditor_to_doubles(val, nums, 4) == 4) {
            w = new Fl_Value_Slider(0, 0, 0, 0);
            w->color(color);
            w->selection_color(textcolor);
            w->range(nums[1], nums[2]);
            w->step(nums[3]);
            w->value(nums[0]);
            w->type(FL_HOR_FILL_SLIDER);
            w->box(FL_BORDER_BOX);
            static_cast<Fl_Value_Slider*>(w)->textsize(textsize * 0.8);

            _edit = w;
        }
    }
    else if (rend == TableEditor::REND::CHOICE) {
        auto choices = cell_choice(_curr_row, _curr_col);

        if (choices.size() > 0) {
            auto w      = new Fl_Choice(0, 0, 0, 0);
            auto select = (std::size_t) 0;

            w->align(align);
            w->box(FL_BORDER_BOX);
            w->down_box(FL_BORDER_BOX);
            w->labelsize(textsize);
            w->textfont(textfont);
            w->textsize(textsize);

            for (std::size_t f = 0; f < choices.size(); f++) {
                const auto& s = choices[f];

                w->add(s.c_str());

                if (s == val) {
                    select = f;
                }
            }

            w->value(select);
            _edit = w;
        }
    }
    else if (rend == TableEditor::REND::INPUT_CHOICE) {
        auto choices = cell_choice(_curr_row, _curr_col);

        if (choices.size() > 0) {
            auto w = new Fl_Input_Choice(0, 0, 0, 0);

            w->box(FL_NO_BOX);
            w->down_box(FL_BORDER_BOX);
            w->input()->box(FL_BORDER_BOX);
            w->input()->textfont(textfont);
            w->input()->textsize(textsize);
            w->labelsize(textsize);
            w->menubutton()->box(FL_BORDER_BOX);
            w->menubutton()->textfont(textfont);
            w->menubutton()->textsize(textsize);
            w->textsize(textsize);

            for (const auto& s : choices) {
                w->add(s.c_str());
            }

            if (*val) {
                w->value(val);
                w->input()->insert_position(0, strlen(val));
            }

            _edit  = w;
            _edit2 = w->input();
            _edit3 = w->menubutton();
        }
    }

    if (_edit != nullptr) {
        add(_edit);

        if (_edit2 != nullptr) {
            Fl::focus(_edit2);
        }
        else {
            Fl::focus(_edit);
        }
    }
}

//------------------------------------------------------------------------------
void TableEditor::_edit_quick(const char* key) {
    auto rend = cell_rend(_curr_row, _curr_col);
    auto val  = static_cast<TableDisplay*>(this)->cell_value(_curr_row, _curr_col);
    char buffer[100];

    assert(val);

    if (rend == TableEditor::REND::INTEGER) {
        auto num = _tableeditor_to_int(val, 0);

        if (strcmp(key, "+") == 0) {
            num++;
        }
        else if (strcmp(key, "++") == 0) {
            num += 10;
        }
        else if (strcmp(key, "+++") == 0) {
            num += 100;
        }
        else if (strcmp(key, "-") == 0) {
            num--;
        }
        else if (strcmp(key, "--") == 0) {
            num -= 10;
        }
        else if (strcmp(key, "---") == 0) {
            num -= 100;
        }

        snprintf(buffer, 100, "%lld", (long long int) num);

        if ((_send_changed_event_always == true || strcmp(val, buffer) != 0) && cell_value(_curr_row, _curr_col, buffer) == true) {
            _set_event(_curr_row, _curr_col, TableEditor::EVENT::CHANGED);
            do_callback();
        }
    }
    else if (rend == TableEditor::REND::NUMBER) {
        auto num = util::to_double(val, 0.0);

        if (strcmp(key, "+") == 0) {
            num += 0.1;
        }
        else if (strcmp(key, "++") == 0) {
            num += 1.0;
        }
        else if (strcmp(key, "+++") == 0) {
            num += 10.0;
        }
        else if (strcmp(key, "-") == 0) {
            num -= 0.1;
        }
        else if (strcmp(key, "--") == 0) {
            num -= 1.0;
        }
        else if (strcmp(key, "---") == 0) {
            num -= 10.0;
        }

        snprintf(buffer, 100, "%f", num);

        if ((_send_changed_event_always == true || strcmp(val, buffer) != 0) && cell_value(_curr_row, _curr_col, buffer) == true) {
            _set_event(_curr_row, _curr_col, TableEditor::EVENT::CHANGED);
            do_callback();
        }
    }
    else if (rend == TableEditor::REND::DLG_DATE) {
        auto date = gnu::Date::FromString(val);

        if (strcmp(key, "+") == 0) {
            date.add_days(1);
        }
        else if (strcmp(key, "++") == 0) {
            date.add_months(1);
        }
        else if (strcmp(key, "+++") == 0) {
            date.year(date.year() + 1);
        }
        else if (strcmp(key, "-") == 0) {
            date.add_days(-1);
        }
        else if (strcmp(key, "--") == 0) {
            date.add_months(-1);
        }
        else if (strcmp(key, "---") == 0) {
            date.year(date.year() - 1);
        }

        auto string = date.format(gnu::Date::FORMAT::ISO_LONG);

        if ((_send_changed_event_always == true || string != val) && cell_value(_curr_row, _curr_col, string.c_str()) == true) {
            _set_event(_curr_row, _curr_col, TableEditor::EVENT::CHANGED);
            do_callback();
        }
    }
    else if (rend == TableEditor::REND::SLIDER || rend == TableEditor::REND::VALUE_SLIDER) {
        double nums[4];

        if (_tableeditor_to_doubles(val, nums, 4) == 4) {
            if (strcmp(key, "+") == 0) {
                nums[0] += nums[3];
            }
            else if (strcmp(key, "++") == 0) {
                nums[0] += (nums[3] * 10);
            }
            else if (strcmp(key, "+++") == 0) {
                nums[0] += (nums[3] * 100);
            }
            else if (strcmp(key, "-") == 0) {
                nums[0] -= nums[3];
            }
            else if (strcmp(key, "--") == 0) {
                nums[0] -= (nums[3] * 10);
            }
            else if (strcmp(key, "---") == 0) {
                nums[0] -= (nums[3] * 100);
            }

            if (nums[0] < nums[1]) {
                nums[0] = nums[1];
            }
            else if (nums[0] > nums[2]) {
                nums[0] = nums[2];
            }

            auto val2 = FormatSlider(nums[0], nums[1], nums[2], nums[3]);

            if ((_send_changed_event_always == true || strcmp(val, val2) != 0) && cell_value(_curr_row, _curr_col, val2) == true) {
                _set_event(_curr_row, _curr_col, TableEditor::EVENT::CHANGED);
                do_callback();
            }
        }
    }
}

//------------------------------------------------------------------------------
void TableEditor::_edit_show() {
    auto rend = cell_rend(_curr_row, _curr_col);
    auto val  = static_cast<TableDisplay*>(this)->cell_value(_curr_row, _curr_col);

    assert(val);

    if (rend == TableEditor::REND::DLG_COLOR) {
        auto color1 = (int) _tableeditor_to_int(val, 0);
        auto color2 = (int) fl_show_colormap((Fl_Color) color1);
        char buffer[100];

        snprintf(buffer, 20, "%d", color2);

        if ((_send_changed_event_always == true || color1 != color2) && cell_value(_curr_row, _curr_col, buffer) == true) {
            _set_event(_curr_row, _curr_col, TableEditor::EVENT::CHANGED);
            do_callback();
        }
    }
    else if (rend == TableEditor::REND::DLG_FILE) {
        auto result = fl_file_chooser(TableEditor::SELECT_FILE, "", val, 0);

        if ((_send_changed_event_always == true || (result != nullptr && strcmp(val, result) != 0)) && cell_value(_curr_row, _curr_col, result) == true) {
            _set_event(_curr_row, _curr_col, TableEditor::EVENT::CHANGED);
            do_callback();
        }
    }
    else if (rend == TableEditor::REND::DLG_DIR) {
        auto result = fl_dir_chooser(TableEditor::SELECT_DIR, val);

        if ((_send_changed_event_always == true || (result != nullptr && strcmp(val, result) != 0)) && cell_value(_curr_row, _curr_col, result) == true) {
            _set_event(_curr_row, _curr_col, TableEditor::EVENT::CHANGED);
            do_callback();
        }
    }
    else if (rend == TableEditor::REND::DLG_DATE) {
        auto date1  = gnu::Date::FromString(val);
        auto date2  = gnu::Date(date1);
        auto result = flw::dlg::date(TableEditor::SELECT_DATE, date1, top_window());
        auto string = date1.format(gnu::Date::FORMAT::ISO_LONG);

        if ((_send_changed_event_always == true || (result == true && date1 != date2)) && cell_value(_curr_row, _curr_col, string.c_str()) == true) {
            _set_event(_curr_row, _curr_col, TableEditor::EVENT::CHANGED);
            do_callback();
        }
    }
    else if (rend == TableEditor::REND::DLG_LIST) {
        auto choices = cell_choice(_curr_row, _curr_col);

        if (choices.size() > 0) {
            auto row = dlg::select(TableEditor::SELECT_LIST, choices, val);

            if (row > 0) {
                const auto& string = choices[row - 1];

                if ((_send_changed_event_always == true || string != val) && cell_value(_curr_row, _curr_col, string.c_str()) == true) {
                    _set_event(_curr_row, _curr_col, TableEditor::EVENT::CHANGED);
                    do_callback();
                }
            }
        }
    }
}

//------------------------------------------------------------------------------
void TableEditor::_edit_start(const char* key) {
    if (_select != TableEditor::SELECT::NO && _edit == nullptr && _curr_row >= 1 && _curr_col >= 1 && cell_edit(_curr_row, _curr_col)) {
        Fl::event_clicks(0);
        Fl::event_is_click(0);

        TableEditor::REND rend = cell_rend(_curr_row, _curr_col);

        if (*key != 0) {
            _edit_quick(key);
        }
        else if (rend == TableEditor::REND::DLG_COLOR ||
                 rend == TableEditor::REND::DLG_FILE ||
                 rend == TableEditor::REND::DLG_DIR ||
                 rend == TableEditor::REND::DLG_DATE ||
                 rend == TableEditor::REND::DLG_LIST) {
            _edit_show();
        }
        else {
            _edit_create();
        }

        redraw();
    }
}

//------------------------------------------------------------------------------
void TableEditor::_edit_stop(bool save) {
    if (_edit) {
        auto rend = cell_rend(_curr_row, _curr_col);
        auto val  = static_cast<TableDisplay*>(this)->cell_value(_curr_row, _curr_col);
        auto stop = true;

        if (save == true) {
            if (rend == TableEditor::REND::TEXT ||
                rend == TableEditor::REND::INTEGER ||
                rend == TableEditor::REND::NUMBER ||
                rend == TableEditor::REND::SECRET) {

                auto input = static_cast<Fl_Input*>(_edit);
                auto val2  = input->value();
                char buffer[100];

                if (rend == TableEditor::REND::INTEGER) {
                    snprintf(buffer, 100, "%d", (int) _tableeditor_to_int(val2, 0));
                    val2 = buffer;
                }
                else if (rend == TableEditor::REND::NUMBER) {
                    auto num = util::to_double(val2, 0.0);
                    snprintf(buffer, 100, "%f", num);
                    val2 = buffer;
                }

                if (val2 == 0) {
                    stop = true;
                    save = false;
                }
                else if (strcmp(val, val2) == 0) {
                    stop = true;
                    save = false;
                }
                else {
                    stop = cell_value(_curr_row, _curr_col, val2);
                }
            }
            else if (rend == TableEditor::REND::BOOLEAN) {
                auto button = static_cast<Fl_Check_Button*>(_edit);
                auto val2   = "0";

                if (button->value())
                    val2 = "1";

                if (strcmp(val, val2) == 0) {
                    stop = true;
                    save = false;
                }
                else {
                    stop = cell_value(_curr_row, _curr_col, val2);
                }
            }
            else if (rend == TableEditor::REND::SLIDER || rend == TableEditor::REND::VALUE_SLIDER) {
                auto slider = static_cast<Fl_Slider*>(_edit);
                auto val2   = FormatSlider(slider->value(), slider->minimum(), slider->maximum(), slider->step());

                if (strcmp(val, val2) == 0) {
                    stop = true;
                    save = false;
                }
                else {
                    stop = cell_value(_curr_row, _curr_col, val2);
                }
            }
            else if (rend == TableEditor::REND::CHOICE) {
                auto choice = static_cast<Fl_Choice*>(_edit);
                auto val2   = choice->text();

                if (val2 == 0) {
                    stop = true;
                    save = false;
                }
                else if (strcmp(val, val2) == 0) {
                    stop = true;
                    save = false;
                }
                else {
                    stop = cell_value(_curr_row, _curr_col, val2);
                }
            }
            else if (rend == TableEditor::REND::INPUT_CHOICE) {
                auto input_choice = static_cast<Fl_Input_Choice*>(_edit);
                auto val2         = input_choice->value();

                if (val2 == 0) {
                    stop = true;
                    save = false;
                }
                else if (strcmp(val, val2) == 0) {
                    stop = true;
                    save = false;
                }
                else {
                    stop = cell_value(_curr_row, _curr_col, val2);
                }
            }
        }

        if (stop == true) {
            remove(_edit);
            delete _edit;

            _edit            = nullptr;
            _edit2           = nullptr;
            _edit3           = nullptr;
            _current_cell[0] = 0;
            _current_cell[1] = 0;
            _current_cell[2] = 0;
            _current_cell[3] = 0;

            if (_send_changed_event_always == true || save == true) {
                _set_event(_curr_row, _curr_col, TableEditor::EVENT::CHANGED);
                do_callback();
            }

            Fl::check();
            Fl::focus(this);
            fl_cursor(FL_CURSOR_DEFAULT);
            redraw();
        }
        else {
            fl_beep();
        }
    }
}

//------------------------------------------------------------------------------
// Change value without editing
// Use +/- to change value (+/- 1 for integers)
// Use alt + "+|-" to change value (+/-10 for integers)
// Use alt + shift + "+|-" to change value (+/-100 for integers)
//
int TableEditor::_ev_keyboard_down2() {
    auto key   = Fl::event_key();
    auto text  = std::string(Fl::event_text());
    auto alt   = Fl::event_alt() != 0;
    auto cmd   = Fl::event_command() != 0;
    auto shift = Fl::event_shift() != 0;

    // printf("key=%d <%s>, alt=%d, cmd=%d, shift=%d\n", key, text.c_str(), alt, cmd, shift); fflush(stdout);

    if (_edit) {
        if (key == FL_Enter || key == FL_KP_Enter) {
            _edit_stop();
            return 1;
        }
        else if (key == FL_Escape) {
            _edit_stop(false);
            return 1;
        }
    }
    else {
        if (key == FL_Enter || key == FL_KP_Enter) {
            _edit_start();
            return 1;
        }
        else if (cmd == true && key == FL_BackSpace) {
            _delete_current_cell();
            return 1;
        }
        else if (key == FL_Delete) {
            _delete_current_cell();
        }
        else if (cmd == true && key == 'x') {
            auto val = static_cast<TableDisplay*>(this)->cell_value(_curr_row, _curr_col);

            Fl::copy(val, strlen(val), 1);
            _delete_current_cell();
            return 1;
        }
        else if (cmd == true && key == 'v') {
            Fl::paste(*this, 1);
            return 1;
        }
        else if (alt == true && shift == true && (key == '+' || text == "+" || key == FL_KP + '+')) { // !!! Problems with shift as it changes key depending on layout
            _edit_start("+++");
            return 1;
        }
        else if (alt == true && (key == '+' || text == "+" || key == FL_KP + '+')) {
            _edit_start("++");
            return 1;
        }
        else if (key == '+' || text == "+" || key == FL_KP + '+') {
            _edit_start("+");
            return 1;
        }
        else if (alt == true && shift == true && (key == '-' || text == "-" || key == FL_KP + '-')) { // !!! Problems with shift as it changes key depending on layout
            _edit_start("---");
            return 1;
        }
        else if (alt == true && (key == '-' || text == "-" || key == FL_KP + '-')) {
            _edit_start("--");
            return 1;
        }
        else if (key == '-' || text == "-" || key == FL_KP + '-') {
            _edit_start("-");
            return 1;
        }
    }

    return 0;
}

//------------------------------------------------------------------------------
int TableEditor::_ev_mouse_click2() {
    auto row         = 0;
    auto col         = 0;
    auto current_row = _curr_row;
    auto current_col = _curr_col;

    _get_cell_below_mouse(row, col);

    if (_edit && (row != current_row || col != current_col)) {
        _edit_stop();
        return 1;
    }
    else if (row == current_row && col == current_col && Fl::event_clicks() == 1 && Fl::event_is_click()) {
        _edit_start();
        return 1;
    }

    return 2;
}

//------------------------------------------------------------------------------
int TableEditor::_ev_paste() {
    auto text = Fl::event_text();

    if (_curr_row > 0 && _curr_col > 0 && text && *text) {
        auto        rend = cell_rend(_curr_row, _curr_col);
        auto        val  = static_cast<TableDisplay*>(this)->cell_value(_curr_row, _curr_col);
        char        buffer[100];
        std::string string;

        switch(rend) {
            case TableEditor::REND::CHOICE:
            case TableEditor::REND::DLG_LIST:
            case TableEditor::REND::SLIDER:
            case TableEditor::REND::VALUE_SLIDER:
                return 1;

            case TableEditor::REND::DLG_DIR:
            case TableEditor::REND::DLG_FILE:
            case TableEditor::REND::INPUT_CHOICE:
            case TableEditor::REND::SECRET:
            case TableEditor::REND::TEXT:
                break;

            case TableEditor::REND::BOOLEAN:
                if (strcmp("1", text) == 0 || strcmp("true", text) == 0) {
                    text = "1";
                }
                else if (strcmp("0", text) == 0 || strcmp("false", text) == 0) {
                    text = "0";
                }
                else {
                    return 1;
                }

            case TableEditor::REND::DLG_COLOR:
            case TableEditor::REND::INTEGER:
                if (*text < '0' || *text > '9') {
                    return 1;
                }
                else {
                    auto num = _tableeditor_to_int(text, 0);
                    snprintf(buffer, 100, "%lld", (long long int) num);
                    text = buffer;

                    if (rend == TableEditor::REND::DLG_COLOR && (num < 0 || num > 255)) {
                        return 1;
                    }
                    else {
                        break;
                    }
                }

            case TableEditor::REND::NUMBER: {
                if (*text < '0' || *text > '9') {
                    return 1;
                }
                else {
                    auto num = util::to_double(text, 0.0);
                    snprintf(buffer, 100, "%f", num);
                    text = buffer;
                    break;
                }
            }

            case TableEditor::REND::DLG_DATE: {
                auto date = gnu::Date::FromString(text);

                if (date.year() == 1 && date.month() == 1 && date.day() == 1) {
                    return 1;
                }
                else {
                    string = date.format(gnu::Date::FORMAT::ISO_LONG);
                    text = string.c_str();
                    break;
                }
            }
        }

        if ((_send_changed_event_always == true || strcmp(val, text) != 0) && cell_value(_curr_row, _curr_col, text) == true) {
            _set_event(_curr_row, _curr_col, TableEditor::EVENT::CHANGED);
            do_callback();
            redraw();
        }
    }

    return 1;
}

//------------------------------------------------------------------------------
const char* TableEditor::FormatSlider(double val, double min, double max, double step) {
    static char result[200];

    snprintf(result, 200, "%.4f %.4f %.4f %.4f", val, min, max, step);
    return result;
}

//------------------------------------------------------------------------------
int TableEditor::handle(int event) {
    auto ret = 2;

    if (_rows > 0 && _cols > 0) {
        if (event == FL_UNFOCUS) {
            if (_edit && Fl::focus() && Fl::focus()->parent()) {
                if ((_edit2 == nullptr && Fl::focus() != _edit) || (_edit2 && Fl::focus() != _edit2 && Fl::focus() != _edit3)) {
                    _edit_stop();
                }
            }
        }
        else if (event == FL_PASTE) {
            ret = _ev_paste();
        }
        else if (event == FL_KEYDOWN) {
            ret = _ev_keyboard_down2();
        }
        else if (event == FL_PUSH) {
            ret = _ev_mouse_click2();
        }
    }

    if (ret == 1) {
        return ret;
    }
    else {
        return TableDisplay::handle(event);
    }
}

} // flw

// MKALGAM_OFF
