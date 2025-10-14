/**
* @file
* @brief Table editor widget.
*
* @author gnuwimp@gmail.com
* @copyright Released under the GNU General Public License v3.0
*/

#include "tableeditor.h"
#include "datechooser.h"
#include "dlg.h"
#include "file.h"

// MKALGAM_ON

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
#include <FL/Fl_Color_Chooser.H>

namespace flw {
namespace table {

static const std::string DEC_SMALL      = "-";
static const std::string DEC_MEDIUM     = "--";
static const std::string DEC_LARGE      = "---";
static const std::string INC_SMALL      = "+";
static const std::string INC_MEDIUM     = "++";
static const std::string INC_LARGE      = "+++";
std::string              EditColorLabel = "Set Color";
std::string              EditDateLabel  = "Select Date";
std::string              EditDirLabel   = "Select Directory";
std::string              EditFileLabel  = "Select File";
std::string              EditListLabel  = "Select String";
std::string              EditTextLabel  = "Edit Text";

/** @brief Format a 4 number string for the slider widgets.
*
* @param[in] val   Current value.
* @param[in] min   Min value.
* @param[in] max   Max value.
* @param[in] step  Step value.
*
* @return String with numbers ex. "12.5000 10.0000 20.0000 1.0000".
*/
std::string format_slider(double val, double min, double max, double step) {
    static char result[2000];

    snprintf(result, 2000, "%.4f %.4f %.4f %.4f", val, min, max, step);
    return result;
}

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

/** @brief Create table editor widget.
*
* @param[in] X  X pos.
* @param[in] Y  Y pos.
* @param[in] W  Width.
* @param[in] H  Height.
* @param[in] l  Optional label.
*/
Editor::Editor(int X, int Y, int W, int H, const char* l) : Display(X, Y, W, H, l) {
    Editor::reset();
}

/** @brief Delete value and copy value to clipboard.
*
*/
void Editor::cmd_cut() {
    auto val = cell_value(_curr_row, _curr_col);

    Fl::copy(val.c_str(), static_cast<int>(val.length()), 1);
    cmd_delete();
}

/** @brief Delete value.
*
* For some values it will set data to a default value for that type.\n
* Empty string or 0 for numbers and so on.\n
*
*/
void Editor::cmd_delete() {
    if (_curr_row > 0 && _curr_col > 0) {
        auto edit = cell_edit(_curr_row, _curr_col);

        if (edit == true) {
            auto type = cell_type(_curr_row, _curr_col);
            auto set  = false;

            switch (type) {
                case Type::TEXT:
                case Type::SECRET:
                case Type::ICHOICE:
                case Type::FILE:
                case Type::DIR:
                case Type::LIST:
                case Type::MTEXT:
                    set = cell_value(_curr_row, _curr_col, "");
                    break;

                case Type::INTEGER:
                    set = cell_value(_curr_row, _curr_col, "0");
                    break;

                case Type::NUMBER:
                    set = cell_value(_curr_row, _curr_col, "0.0");
                    break;

                case Type::BOOLEAN:
                    set = cell_value(_curr_row, _curr_col, "0");
                    break;

                case Type::DATE:
                    set = cell_value(_curr_row, _curr_col, "1970-01-01");
                    break;

                case Type::COLOR:
                    set = cell_value(_curr_row, _curr_col, "56");
                    break;

                default:
                    break;
            }

            if (set == true) {
                _set_event(_curr_row, _curr_col, Event::CHANGED);
                do_callback();
                redraw();
            }
        }
    }
}

/** @brief Paste value from clipboard into current cell.
*
*/
void Editor::cmd_paste() {
    Fl::paste(*this, 1);
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
void Editor::_draw_cell(int row, int col, int X, int Y, int W, int H, bool ver, bool hor, bool current) {
    fl_push_clip(X, Y, W + 1, H);

    auto align     = cell_align(row, col);
    auto textcolor = cell_textcolor(row, col);
    auto textfont  = cell_textfont(row, col);
    auto textsize  = cell_textsize(row, col);
    auto val       = cell_value(row, col);

    if (row < 0 || col < 0) {
    }
    else if (row == 0 || col == 0) { // Draw header cell.
        fl_draw_box(FL_THIN_UP_BOX, X, Y, W + 1, H + (row == _rows ? 0 : 1), FL_BACKGROUND_COLOR);
        fl_font(textfont, textsize);
        fl_color(FL_FOREGROUND_COLOR);
        _draw_text(val.c_str(), X + 4, Y + 2, W - 8, H - 4, align);
    }
    else { // Draw normal cell.
        auto format = cell_format(row, col);
        auto type   = cell_type(row, col);
        auto color  = cell_color(row, col);
        char buffer[100];

        if (current == true) {
            color     = selection_color();
            textcolor = fl_contrast(FL_FOREGROUND_COLOR, color);
        }

        fl_rectf(X + 1, Y, W + 1, H, color);

        if (type == Type::SECRET) {
            fl_font(textfont, textsize);
            fl_color(textcolor);

            if (format == Format::SECRET_DOT) {
                fl_draw("••••••", X + 4, Y + 2, W - 8, H - 4, align, 0, 1);
            }
            else {
                fl_draw("******", X + 4, Y + 2, W - 8, H - 4, align, 0, 1);
            }
        }
        else if (type == Type::SLIDER) {
            double nums[4];

            if (util::to_doubles(val, nums, 4) == 4) {
                auto range = 0.0;

                if ((nums[2] - nums[1]) > 0.0001) {
                    range = (nums[0] - nums[1]) / (nums[2] - nums[1]);
                }

                auto width = static_cast<int>(range * (W - 3));

                if (width > 0) {
                    fl_draw_box(FL_FLAT_BOX, X + 2, Y + 2, width, H - 3, textcolor);
                }
            }
        }
        else if (type == Type::COLOR) {
            fl_draw_box(FL_FLAT_BOX, X + 2, Y + 2, W - 3, H - 3, static_cast<Fl_Color>(util::to_int(val, 0)));
        }
        else if (type == Type::DATE) {
            auto date   = gnu::Date(val);
            auto string = std::string();

            if (format == Format::DATE_WORLD) {
                string = date.format(gnu::Date::Format::WORLD);
            }
            else if (format == Format::DATE_US) {
                string = date.format(gnu::Date::Format::US);
            }
            else {
                string = date.format(gnu::Date::Format::ISO_LONG);
            }

            fl_font(textfont, textsize);
            fl_color(textcolor);
            _draw_text(string.c_str(), X + 4, Y + 2, W - 8, H - 4, align);
        }
        else if (type == Type::BOOLEAN) {
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

            if (*val.c_str() == '1') {
                Fl_Rect r(x_1, y_1, bw - 1, bw - 1);
                fl_draw_check(r, selection_color());
            }
        }
        else if (type == Type::INTEGER) {
            auto num = util::to_int(val, 0);

            fl_font(textfont, textsize);
            fl_color(textcolor);

            if (format == Format::INT_SEP) {
                auto s = util::format_int(num);
                _draw_text(s.c_str(), X + 4, Y + 2, W - 8, H - 4, align);
            }
            else {
                snprintf(buffer, 100, "%lld", (long long int) num);
                _draw_text(buffer, X + 4, Y + 2, W - 8, H - 4, align);
            }
        }
        else if (type == Type::NUMBER || type == Type::VSLIDER) {
            auto num = util::to_double(val, 0.0);

            if (type == Type::VSLIDER) {
                double nums[1];

                if (util::to_doubles(val, nums, 1) == 1) {
                    num = nums[0];
                }
            }

            if (format == Format::DEC_0) {
                snprintf(buffer, 100, "%.0f", num);
            }
            else if (format == Format::DEC_1) {
                snprintf(buffer, 100, "%.1f", num);
            }
            else if (format == Format::DEC_2) {
                snprintf(buffer, 100, "%.2f", num);
            }
            else if (format == Format::DEC_3) {
                snprintf(buffer, 100, "%.3f", num);
            }
            else if (format == Format::DEC_4) {
                snprintf(buffer, 100, "%.4f", num);
            }
            else if (format == Format::DEC_5) {
                snprintf(buffer, 100, "%.5f", num);
            }
            else if (format == Format::DEC_6) {
                snprintf(buffer, 100, "%.6f", num);
            }
            else if (format == Format::DEC_7) {
                snprintf(buffer, 100, "%.7f", num);
            }
            else if (format == Format::DEC_8) {
                snprintf(buffer, 100, "%.8f", num);
            }
            else {
                snprintf(buffer, 100, "%f", num);
            }

            fl_font(textfont, textsize);
            fl_color(textcolor);
            _draw_text(buffer, X + 4, Y + 2, W - 8, H - 4, align);
        }
        else if (type == Type::MTEXT) {
            auto tmp = std::string();

            for (auto c : val) {
                if (tmp.length() == 30) {
                    break;
                }
                else if ((c == 10 || c == 13 || c == 9) && tmp == "") {
                }
                else if ((c == 10 || c == 13 || c == 9) && tmp != "") {
                    break;
                }
                else {
                    tmp += c;
                }
            }

            if (tmp != val) {
                tmp += "...";
            }

            fl_font(textfont, textsize);
            fl_color(textcolor);
            _draw_text(tmp.c_str(), X + 4, Y + 2, W - 8, H - 4, align);
        }
        else { // Draw plain text string.
            fl_font(textfont, textsize);
            fl_color(textcolor);
            _draw_text(val.c_str(), X + 4, Y + 2, W - 8, H - 4, align);
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

    fl_pop_clip();
}

/** @brief Create edit widget.
*
*/
void Editor::_edit_create() {
    auto rend      = cell_type(_curr_row, _curr_col);
    auto align     = cell_align(_curr_row, _curr_col);
    auto color     = cell_color(_curr_row, _curr_col);
    auto textcolor = FL_FOREGROUND_COLOR;
    auto textfont  = cell_textfont(_curr_row, _curr_col);
    auto textsize  = cell_textsize(_curr_row, _curr_col);
    auto val       = cell_value(_curr_row, _curr_col);

    if (rend == Type::TEXT ||
        rend == Type::INTEGER ||
        rend == Type::NUMBER ||
        rend == Type::SECRET) {

        auto w = (Fl_Input*) nullptr;

        if (rend == Type::TEXT) {
            w = new Fl_Input(0, 0, 0, 0);
        }
        else if (rend == Type::INTEGER) {
            w = new Fl_Int_Input(0, 0, 0, 0);
        }
        else if (rend == Type::NUMBER) {
            w = new Fl_Float_Input(0, 0, 0, 0);
        }
        else if (rend == Type::SECRET) {
            w = new Fl_Secret_Input(0, 0, 0, 0);
        }

        w->align(align);
        w->box(FL_BORDER_BOX);
        w->color(color);
        w->labelsize(textsize);
        w->textcolor(textcolor);
        w->textfont(textfont);
        w->textsize(textsize);
        w->value(val.c_str());
        w->insert_position(0, static_cast<int>(val.length()));

        _edit = w;
    }
    else if (rend == Type::BOOLEAN) {
        auto w = new Fl_Check_Button(0, 0, 0, 0);

        w->box(FL_BORDER_BOX);
        w->color(color);
        w->labelcolor(textcolor);
        w->labelsize(textsize);
        w->value(*val.c_str() == '1' ? 1 : 0);

        _edit = w;
    }
    else if (rend == Type::SLIDER) {
        auto w = (Fl_Slider*) nullptr;
        double nums[4];

        if (util::to_doubles(val, nums, 4) == 4) {
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
    else if (rend == Type::VSLIDER) {
        auto w = (Fl_Slider*) nullptr;
        double nums[4];

        if (util::to_doubles(val, nums, 4) == 4) {
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
    else if (rend == Type::CHOICE) {
        auto choices = cell_choice(_curr_row, _curr_col);

        if (choices.size() > 0) {
            auto w      = new Fl_Choice(0, 0, 0, 0);
            auto select = (size_t) 0;

            w->align(align);
            w->box(FL_BORDER_BOX);
            w->down_box(FL_BORDER_BOX);
            w->labelsize(textsize);
            w->textfont(textfont);
            w->textsize(textsize);

            for (size_t f = 0; f < choices.size(); f++) {
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
    else if (rend == Type::ICHOICE) {
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

            if (val != "") {
                w->value(val.c_str());
                w->input()->insert_position(0, static_cast<int>(val.length()));
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

/** @brief Edit a value without creating a widget.
*
* Does not work for all types.
*
* @param[in] key  Value size change hint.
*/
void Editor::_edit_quick(const std::string& key) {
    auto rend = cell_type(_curr_row, _curr_col);
    auto val  = cell_value(_curr_row, _curr_col);
    char buffer[100];

    if (rend == Type::INTEGER) {
        auto num = util::to_int(val, 0);

        if (key == table::INC_SMALL) {
            num++;
        }
        else if (key == table::INC_MEDIUM) {
            num += 10;
        }
        else if (key == table::INC_LARGE) {
            num += 100;
        }
        else if (key == table::DEC_SMALL) {
            num--;
        }
        else if (key == table::DEC_MEDIUM) {
            num -= 10;
        }
        else if (key == table::DEC_LARGE) {
            num -= 100;
        }

        snprintf(buffer, 100, "%lld", static_cast<long long int>(num));

        if ((_force_events == true || val != buffer) && cell_value(_curr_row, _curr_col, buffer) == true) {
            _set_event(_curr_row, _curr_col, Event::CHANGED);
            do_callback();
        }
    }
    else if (rend == Type::NUMBER) {
        auto num = util::to_double(val, 0.0);

        if (key == table::INC_SMALL) {
            num += 0.1;
        }
        else if (key == table::INC_MEDIUM) {
            num += 1.0;
        }
        else if (key == table::INC_LARGE) {
            num += 10.0;
        }
        else if (key == table::DEC_SMALL) {
            num -= 0.1;
        }
        else if (key == table::DEC_MEDIUM) {
            num -= 1.0;
        }
        else if (key == table::DEC_LARGE) {
            num -= 10.0;
        }

        snprintf(buffer, 100, "%f", num);

        if ((_force_events == true || val != buffer) && cell_value(_curr_row, _curr_col, buffer) == true) {
            _set_event(_curr_row, _curr_col, Event::CHANGED);
            do_callback();
        }
    }
    else if (rend == Type::COLOR) {
        auto color = static_cast<int>(util::to_int(val, 0));

        if (key == table::INC_SMALL) {
            color++;
        }
        else if (key == table::INC_MEDIUM) {
            color += 8;
        }
        else if (key == table::INC_LARGE) {
            color += 24;
        }
        else if (key == table::DEC_SMALL) {
            color--;
        }
        else if (key == table::DEC_MEDIUM) {
            color -= 8;
        }
        else if (key == table::DEC_LARGE) {
            color -= 24;
        }

        color = color % 256;
        snprintf(buffer, 100, "%d", color);

        if ((_force_events == true || val != buffer) && cell_value(_curr_row, _curr_col, buffer) == true) {
            _set_event(_curr_row, _curr_col, Event::CHANGED);
            do_callback();
        }
    }
    else if (rend == Type::DATE) {
        auto date = gnu::Date(val);

        if (key == table::INC_SMALL) {
            date.add_days(1);
        }
        else if (key == table::INC_MEDIUM) {
            date.add_months(1);
        }
        else if (key == table::INC_LARGE) {
            date.add_years(1);
        }
        else if (key == table::DEC_SMALL) {
            date.add_days(-1);
        }
        else if (key == table::DEC_MEDIUM) {
            date.add_months(-1);
        }
        else if (key == table::DEC_LARGE) {
            date.add_years(-1);
        }

        auto string = date.format(gnu::Date::Format::ISO_LONG);

        if ((_force_events == true || string != val) && cell_value(_curr_row, _curr_col, string.c_str()) == true) {
            _set_event(_curr_row, _curr_col, Event::CHANGED);
            do_callback();
        }
    }
    else if (rend == Type::SLIDER || rend == Type::VSLIDER) {
        double nums[4];

        if (util::to_doubles(val, nums, 4) == 4) {
            if (key == table::INC_SMALL) {
                nums[0] += nums[3];
            }
            else if (key == table::INC_MEDIUM) {
                nums[0] += (nums[3] * 10);
            }
            else if (key == table::INC_LARGE) {
                nums[0] += (nums[3] * 100);
            }
            else if (key == table::DEC_SMALL) {
                nums[0] -= nums[3];
            }
            else if (key == table::DEC_MEDIUM) {
                nums[0] -= (nums[3] * 10);
            }
            else if (key == table::DEC_LARGE) {
                nums[0] -= (nums[3] * 100);
            }

            if (nums[0] < nums[1]) {
                nums[0] = nums[1];
            }
            else if (nums[0] > nums[2]) {
                nums[0] = nums[2];
            }

            auto val2 = table::format_slider(nums[0], nums[1], nums[2], nums[3]);

            if ((_force_events == true || val != val2) && cell_value(_curr_row, _curr_col, val2) == true) {
                _set_event(_curr_row, _curr_col, Event::CHANGED);
                do_callback();
            }
        }
    }
}

/** @brief Show edit dialog.
*
*/
void Editor::_edit_show_dlg() {
    auto rend = cell_type(_curr_row, _curr_col);
    auto val  = cell_value(_curr_row, _curr_col);

    if (rend == Type::COLOR) {
        auto red    = (uchar) 0;
        auto green  = (uchar) 0;
        auto blue   = (uchar) 0;
        auto color1 = static_cast<Fl_Color>(util::to_int(val, 0));

        Fl::get_color(color1, red, green, blue);

        if (fl_color_chooser(table::EditColorLabel.c_str(), red, green, blue, 2) != 0) {
            auto color2 = fl_rgb_color(red, green, blue);
            char buffer[100];

            snprintf(buffer, 100, "%u", color2);

            if ((_force_events == true || color1 != color2) && cell_value(_curr_row, _curr_col, buffer) == true) {
                _set_event(_curr_row, _curr_col, Event::CHANGED);
                do_callback();
            }
        }

    }
    else if (rend == Type::FILE) {
        auto result = util::to_string(fl_file_chooser(table::EditFileLabel.c_str(), "", val.c_str(), 0));

        if (result != "" && (_force_events == true || val != result) && cell_value(_curr_row, _curr_col, result) == true) {
            _set_event(_curr_row, _curr_col, Event::CHANGED);
            do_callback();
        }
    }
    else if (rend == Type::DIR) {
        auto result = util::to_string(fl_dir_chooser(table::EditDirLabel.c_str(), val.c_str()));

        if (result != "" && (_force_events == true || val != result) && cell_value(_curr_row, _curr_col, result) == true) {
            _set_event(_curr_row, _curr_col, Event::CHANGED);
            do_callback();
        }
    }
    else if (rend == Type::DATE) {
        auto date1  = gnu::Date(val);
        auto date2  = gnu::Date(date1);
        auto result = flw::dlg::date(table::EditDateLabel, date1, top_window());
        auto string = date1.format(gnu::Date::Format::ISO_LONG);

        if ((_force_events == true || (result == true && date1 != date2)) && cell_value(_curr_row, _curr_col, string.c_str()) == true) {
            _set_event(_curr_row, _curr_col, Event::CHANGED);
            do_callback();
        }
    }
    else if (rend == Type::LIST) {
        auto choices = cell_choice(_curr_row, _curr_col);

        if (choices.size() > 0) {
            auto row = dlg::select_string(table::EditListLabel.c_str(), choices, val);

            if (row >= 0) {
                const auto& string = choices[row];

                if ((_force_events == true || string != val) && cell_value(_curr_row, _curr_col, string.c_str()) == true) {
                    _set_event(_curr_row, _curr_col, Event::CHANGED);
                    do_callback();
                }
            }
        }
    }
    else if (rend == Type::MTEXT) {
        auto val2 = val;

        if (dlg::text_edit(table::EditTextLabel.c_str(), val2, top_window()) == true) {
            if ((_force_events == true || val != val2) && cell_value(_curr_row, _curr_col, val2.c_str()) == true) {
                _set_event(_curr_row, _curr_col, Event::CHANGED);
                do_callback();
            }
        }
    }
}

/** @brief Start editing.
*
* @param[in] key  If set then do an quick edit.
*/
void Editor::_edit_start(const std::string& key) {
    if (_select != Select::NO && _edit == nullptr && _curr_row >= 1 && _curr_col >= 1 && cell_edit(_curr_row, _curr_col)) {
        Fl::event_clicks(0);
        Fl::event_is_click(0);

        Type type = cell_type(_curr_row, _curr_col);

        if (key != "") {
            _edit_quick(key);
        }
        else if (type == Type::COLOR ||
                 type == Type::FILE ||
                 type == Type::DIR ||
                 type == Type::DATE ||
                 type == Type::LIST ||
                 type == Type::MTEXT) {
            _edit_show_dlg();
        }
        else {
            _edit_create();
        }

        redraw();
    }
}

/** @brief Stop editing.
*
* @param[in] save  True to try to send data to cell_value().
*/
void Editor::_edit_stop(bool save) {
    if (_edit != nullptr) {
        auto type = cell_type(_curr_row, _curr_col);
        auto val  = cell_value(_curr_row, _curr_col);
        auto stop = true;

        if (save == true) {
            if (type == Type::TEXT ||
                type == Type::INTEGER ||
                type == Type::NUMBER ||
                type == Type::SECRET) {

                auto input = static_cast<Fl_Input*>(_edit);
                auto val2  = util::to_string(input->value());
                char buffer[100];

                if (type == Type::INTEGER) {
                    snprintf(buffer, 100, "%lld", static_cast<long long int>(util::to_int(val2, 0)));
                    val2 = buffer;
                }
                else if (type == Type::NUMBER) {
                    auto num = util::to_double(val2, 0.0);
                    snprintf(buffer, 100, "%f", num);
                    val2 = buffer;
                }

                if (val == val2) {
                    stop = true;
                    save = false;
                }
                else {
                    stop = cell_value(_curr_row, _curr_col, val2);
                }
            }
            else if (type == Type::BOOLEAN) {
                auto button = static_cast<Fl_Check_Button*>(_edit);
                auto val2   = std::string("0");

                if (button->value() != 0) {
                    val2 = "1";
                }

                if (val == val2) {
                    stop = true;
                    save = false;
                }
                else {
                    stop = cell_value(_curr_row, _curr_col, val2);
                }
            }
            else if (type == Type::SLIDER || type == Type::VSLIDER) {
                auto slider = static_cast<Fl_Slider*>(_edit);
                auto val2   = table::format_slider(slider->value(), slider->minimum(), slider->maximum(), slider->step());

                if (val == val2) {
                    stop = true;
                    save = false;
                }
                else {
                    stop = cell_value(_curr_row, _curr_col, val2);
                }
            }
            else if (type == Type::CHOICE) {
                auto choice = static_cast<Fl_Choice*>(_edit);
                auto val2   = util::to_string(choice->text());

                if (val == val2) {
                    stop = true;
                    save = false;
                }
                else {
                    stop = cell_value(_curr_row, _curr_col, val2);
                }
            }
            else if (type == Type::ICHOICE) {
                auto input_choice = static_cast<Fl_Input_Choice*>(_edit);
                auto val2         = util::to_string(input_choice->value());

                if (val == val2) {
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

            if (_force_events == true || save == true) {
                _set_event(_curr_row, _curr_col, Event::CHANGED);
                do_callback();
            }

            Fl::check();
            Fl::focus(this);
            fl_cursor(FL_CURSOR_DEFAULT);
            redraw();
        }
        else { // cell_value() has not alloed data to be saved.
            fl_beep();
        }
    }
}

/** @brief Change or delete value without edit widgets.
*
* Use +/- to change value (+/- 1 for integers, days for dates).\n
* Use alt + "+|-" to change value (+/-10 for integers, months for dates).\n
* Use alt + shift + "+|-" to change value (+/-100 for integers, years for dates).\n
*
* @return 1 if event was used or 0.
*/
int Editor::_ev_keyboard_down2() {
    auto key   = Fl::event_key();
    auto text  = std::string(Fl::event_text());
    auto alt   = Fl::event_alt() != 0;
    auto cmd   = Fl::event_command() != 0;
    auto shift = Fl::event_shift() != 0;

    //printf("key=%d <%s>, alt=%d, cmd=%d, shift=%d\n", key, text.c_str(), alt, cmd, shift); fflush(stdout);

    if (_edit != nullptr) {
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
            cmd_delete();
            return 1;
        }
        else if (key == FL_Delete) {
            cmd_delete();
        }
        else if (cmd == true && key == 'x') {
            cmd_cut();
            return 1;
        }
        else if (cmd == true && key == 'v') {
            cmd_paste();
            return 1;
        }
        else if (alt == true && shift == true && (key == '+' || text == table::INC_SMALL || key == FL_KP + '+')) { // !!! Problems with shift as it changes key depending on layout.
            _edit_start(table::INC_LARGE);
            return 1;
        }
        else if (alt == true && (key == '+' || text == table::INC_SMALL || key == FL_KP + '+')) {
            _edit_start(table::INC_MEDIUM);
            return 1;
        }
        else if (key == '+' || text == table::INC_SMALL || key == FL_KP + '+') {
            _edit_start(table::INC_SMALL);
            return 1;
        }
        else if (alt == true && shift == true && (key == '-' || text == table::DEC_SMALL || key == FL_KP + '-')) { // !!! Problems with shift as it changes key depending on layout.
            _edit_start(table::DEC_LARGE);
            return 1;
        }
        else if (alt == true && (key == '-' || text == table::DEC_SMALL || key == FL_KP + '-')) {
            _edit_start(table::DEC_MEDIUM);
            return 1;
        }
        else if (key == '-' || text == table::DEC_SMALL || key == FL_KP + '-') {
            _edit_start(table::DEC_SMALL);
            return 1;
        }
    }

    return 0;
}

/** @brief Start or stop editing by using mouse click.
*
* @return 1 if used or 2.
*/
int Editor::_ev_mouse_click2() {
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

/** @brief Paste text from clipboard.
*
* Not all text can be used with current data type.
*
* @return Always 1?
*/
int Editor::_ev_paste() {
    auto text = util::to_string(Fl::event_text());

    if (_curr_row > 0 && _curr_col > 0 && text != "") {
        auto        type = cell_type(_curr_row, _curr_col);
        auto        val  = cell_value(_curr_row, _curr_col);
        char        buffer[100];
        std::string string;

        switch(type) {
            case Type::CHOICE:
            case Type::LIST:
            case Type::SLIDER:
            case Type::VSLIDER:
                return 1;

            case Type::DIR:
            case Type::FILE:
            case Type::ICHOICE:
            case Type::SECRET:
            case Type::TEXT:
            case Type::MTEXT:
                break;

            case Type::BOOLEAN: {
                if (text == "1" || text == "true") {
                    text = "1";
                }
                else if (text == "0" || text == "false") {
                    text = "0";
                }
                else {
                    return 1;
                }

                break;
            }

            case Type::COLOR:
            case Type::INTEGER: {
                if (text[0] < '0' || text[0] > '9') {
                    return 1;
                }
                else {
                    auto num = util::to_int(text, 0);
                    snprintf(buffer, 100, "%lld", static_cast<long long int>(num));
                    text = buffer;

                    if (type == Type::COLOR && (num < 0 || num > 255)) {
                        return 1;
                    }
                }

                break;
            }

            case Type::NUMBER: {
                if (text[0] < '0' || text[0] > '9') {
                    return 1;
                }
                else {
                    auto num = util::to_double(text, 0.0);
                    snprintf(buffer, 100, "%f", num);
                    text = buffer;
                }

                break;
            }

            case Type::DATE: {
                auto date = gnu::Date(text);

                if (date.year() == 1 && date.month() == 1 && date.day() == 1) {
                    return 1;
                }
                else {
                    string = date.format(gnu::Date::Format::ISO_LONG);
                    text = string.c_str();
                }

                break;
            }
        }

        if ((_force_events == true || val != text) && cell_value(_curr_row, _curr_col, text) == true) {
            _set_event(_curr_row, _curr_col, Event::CHANGED);
            do_callback();
            redraw();
        }
    }

    return 1;
}

/** @brief Hack for searching text with Type::SLIDER or Type::VSLIDER.
*
* @param[in] row  Row number.
* @param[in] col  Column number.
*
* @return Cell string or first number in case of SLIDER types.
*/
std::string Editor::_get_find_value(int row, int col) {
    auto type = cell_type(row, col);
    auto val  = cell_value(row, col);

    if (type == Type::SLIDER || type == Type::VSLIDER) {
        auto pos = val.find(" ");

        if (pos != std::string::npos) {
            val = val.substr(0, pos);
        }
    }

    return val;
}

/** @brief Handle events.
*
* @param[in] event Event value.
*
* @return 1 if event is handled, 0 if not.
*/
int Editor::handle(int event) {
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
        return Display::handle(event);
    }
}

/** @brief Clear all values.
*
*/
void Editor::reset() {
    Display::reset();

    _force_events = false;
    _edit2        = nullptr;
    _edit3        = nullptr;
}

} // table
} // flw

// MKALGAM_OFF
