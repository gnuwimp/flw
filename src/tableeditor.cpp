// Copyright 2016 - 2021 gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#include "tableeditor.h"
#include "datechooser.h"
#include "dlg.h"
#include <FL/Fl_Scrollbar.H>
#include <FL/fl_draw.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_File_Chooser.H>
#include <FL/Fl_Float_Input.H>
#include <FL/Fl_Input_Choice.H>
#include <FL/Fl_Int_Input.H>
#include <FL/Fl_Secret_Input.H>
#include <FL/Fl_Value_Slider.H>
#include <FL/fl_show_colormap.H>

// MALAGMA_ON

const char* flw::TableEditor::SELECT_DATE = "Select Date";
const char* flw::TableEditor::SELECT_DIR  = "Select Directory";
const char* flw::TableEditor::SELECT_FILE = "Select File";
const char* flw::TableEditor::SELECT_LIST = "Select String";

//------------------------------------------------------------------------------
flw::TableEditor::TableEditor(int X, int Y, int W, int H, const char* l) : TableDisplay(X, Y, W, H, l) {
    clear();
}

//------------------------------------------------------------------------------
void flw::TableEditor::clear() {
    TableDisplay::clear();

    _edit2 = nullptr;
    _edit3 = nullptr;
}

//------------------------------------------------------------------------------
void flw::TableEditor::_delete_current_cell() {
    if (_curr_row > 0 && _curr_col > 0) {
        auto rend = cell_rend(_curr_row, _curr_col);
        auto edit = cell_edit(_curr_row, _curr_col);
        auto set  = false;

        if (edit) {
            switch (rend) {
                case flw::TableEditor::REND::TEXT:
                case flw::TableEditor::REND::SECRET:
                case flw::TableEditor::REND::INPUT_CHOICE:
                case flw::TableEditor::REND::DLG_FILE:
                case flw::TableEditor::REND::DLG_DIR:
                case flw::TableEditor::REND::DLG_LIST:
                    set = cell_value(_curr_row, _curr_col, "");
                    break;

                case flw::TableEditor::REND::INTEGER:
                    set = cell_value(_curr_row, _curr_col, "0");
                    break;

                case flw::TableEditor::REND::NUMBER:
                    set = cell_value(_curr_row, _curr_col, "0.0");
                    break;

                case flw::TableEditor::REND::BOOLEAN:
                    set = cell_value(_curr_row, _curr_col, "0");
                    break;

                case flw::TableEditor::REND::DLG_DATE:
                    set = cell_value(_curr_row, _curr_col, "1970-01-01");
                    break;

                case flw::TableEditor::REND::DLG_COLOR:
                    set = cell_value(_curr_row, _curr_col, "56");
                    break;

                default:
                    break;
            }

            if (set) {
                _event_set(_curr_row, _curr_col, flw::TableEditor::EVENT::CHANGED);
                do_callback();
                redraw();
            }
        }
    }
}

//------------------------------------------------------------------------------
void flw::TableEditor::_draw_cell(int row, int col, int X, int Y, int W, int H, bool ver, bool hor, bool current) {
    fl_push_clip(X, Y, W + 1, H);

    auto align     = cell_align(row, col);
    auto textcolor = cell_textcolor(row, col);
    auto textfont  = cell_textfont(row, col);
    auto textsize  = cell_textsize(row, col);
    auto val       = ((TableDisplay*) this)->cell_value(row, col);

    if (row > 0 && col > 0) { // Draw normal cell
        auto format = cell_format(row, col);
        auto rend   = cell_rend(row, col);
        auto color  = cell_color(row, col);
        char buffer[100];

        if (current) {
            color = selection_color();
        }

        fl_rectf(X + 1, Y, W + 1, H, color);

        if (rend == flw::TableEditor::REND::SECRET) {
            fl_font(textfont, textsize);
            fl_color(textcolor);

            if (format == flw::TableEditor::FORMAT::SECRET_DOT) {
                fl_draw("••••••", X + 4, Y + 2, W - 8, H - 4, align, 0, 1);
            }
            else {
                fl_draw("******", X + 4, Y + 2, W - 8, H - 4, align, 0, 1);
            }
        }
        else if (rend == flw::TableEditor::REND::SLIDER) {
            auto curr  = 0.0;
            auto min   = 0.0;
            auto max   = 100.0;
            auto step  = 0.0;
            auto width = 0.0;
            auto range = 0.0;

            if (util::to_doubles(val, &curr, &min, &max, &step) == 4) {
                if ((max - min) > 0.0001) {
                    range = (curr - min) / (max - min);
                }

                width = (int) (range * (W - 3));

                if (width > 0) {
                    fl_draw_box(FL_FLAT_BOX, X + 2, Y + 2, width, H - 3, textcolor);
                }
            }
        }
        else if (rend == flw::TableEditor::REND::DLG_COLOR) {
            fl_draw_box(FL_FLAT_BOX, X + 2, Y + 2, W - 3, H - 3, (Fl_Color) util::to_int(val));
        }
        else if (rend == flw::TableEditor::REND::DLG_DATE) {
            auto        date = Date::FromString(val);
            std::string string;

            if (format == flw::TableEditor::FORMAT::DATE_WORLD) {
                string = date.format(Date::FORMAT::WORLD);
            }
            else if (format == flw::TableEditor::FORMAT::DATE_US) {
                string = date.format(Date::FORMAT::US);
            }
            else {
                string = date.format(Date::FORMAT::ISO_LONG);
            }

            fl_font(textfont, textsize);
            fl_color(textcolor);
            fl_draw(string.c_str(), X + 4, Y + 2, W - 8, H - 4, align, 0, 1);
        }
        else if (rend == flw::TableEditor::REND::BOOLEAN) {
            auto bw  = textsize + 4;
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
                bw -= 6;
                fl_draw_box(FL_ROUND_UP_BOX, x_1 + 3, y_1 + 3, bw, bw, selection_color());
            }
        }
        else if (rend == flw::TableEditor::REND::INTEGER) {
            auto num = util::to_int(val);

            fl_font(textfont, textsize);
            fl_color(textcolor);

            if (format == flw::TableEditor::FORMAT::INT_SEP) {
                auto s = util::format_int(num);
                fl_draw(s.c_str(), X + 4, Y + 2, W - 8, H - 4, align, 0, 1);
            }
            else {
                snprintf(buffer, 100, "%lld", (long long int) num);
                fl_draw(buffer, X + 4, Y + 2, W - 8, H - 4, align, 0, 1);
            }
        }
        else if (rend == flw::TableEditor::REND::NUMBER || rend == flw::TableEditor::REND::VALUE_SLIDER) {
            auto num = util::to_double_l(val);

            if (rend == flw::TableEditor::REND::VALUE_SLIDER) {
                auto curr = 0.0;

                if (util::to_doubles(val, &curr) == 1) {
                    num = curr;
                }
            }

            if (format == flw::TableEditor::FORMAT::DEC_0) {
                snprintf(buffer, 100, "%.0Lf", num);
            }
            else if (format == flw::TableEditor::FORMAT::DEC_1) {
                snprintf(buffer, 100, "%.1Lf", num);
            }
            else if (format == flw::TableEditor::FORMAT::DEC_2) {
                snprintf(buffer, 100, "%.2Lf", num);
            }
            else if (format == flw::TableEditor::FORMAT::DEC_3) {
                snprintf(buffer, 100, "%.3Lf", num);
            }
            else if (format == flw::TableEditor::FORMAT::DEC_4) {
                snprintf(buffer, 100, "%.4Lf", num);
            }
            else if (format == flw::TableEditor::FORMAT::DEC_5) {
                snprintf(buffer, 100, "%.5Lf", num);
            }
            else if (format == flw::TableEditor::FORMAT::DEC_6) {
                snprintf(buffer, 100, "%.6Lf", num);
            }
            else if (format == flw::TableEditor::FORMAT::DEC_7) {
                snprintf(buffer, 100, "%.7Lf", num);
            }
            else if (format == flw::TableEditor::FORMAT::DEC_8) {
                snprintf(buffer, 100, "%.8Lf", num);
            }
            else {
                snprintf(buffer, 100, "%.19Lg", num);
            }

            fl_font(textfont, textsize);
            fl_color(textcolor);
            fl_draw(buffer, X + 4, Y + 2, W - 8, H - 4, align, 0, 1);
        }
        else { // Draw plain text string
            fl_font(textfont, textsize);
            fl_color(textcolor);
            fl_draw(val, X + 4, Y + 2, W - 8, H - 4, align, 0, 1);
        }

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
        // fl_rect(X, Y, W + 1, H + (row == _rows ? 0 : 1), FL_DARK3);
        fl_font(textfont, textsize);
        fl_color(FL_FOREGROUND_COLOR);
        fl_draw(val, X + 4, Y + 2, W - 8, H - 4, align, 0, 1);
    }

    fl_pop_clip();
}

//------------------------------------------------------------------------------
void flw::TableEditor::_edit_create() {
    auto rend      = cell_rend(_curr_row, _curr_col);
    auto align     = cell_align(_curr_row, _curr_col);
    auto color     = cell_color(_curr_row, _curr_col);
    auto textcolor = FL_FOREGROUND_COLOR;
    auto textfont  = cell_textfont(_curr_row, _curr_col);
    auto textsize  = cell_textsize(_curr_row, _curr_col);
    auto val       = ((TableDisplay*) this)->cell_value(_curr_row, _curr_col);

    if (rend == flw::TableEditor::REND::TEXT ||
        rend == flw::TableEditor::REND::INTEGER ||
        rend == flw::TableEditor::REND::NUMBER ||
        rend == flw::TableEditor::REND::SECRET) {

        auto w = (Fl_Input*) nullptr;

        if (rend == flw::TableEditor::REND::TEXT) {
            w = new Fl_Input(0, 0, 0, 0);
        }
        else if (rend == flw::TableEditor::REND::INTEGER) {
            w = new Fl_Int_Input(0, 0, 0, 0);
        }
        else if (rend == flw::TableEditor::REND::NUMBER) {
            w = new Fl_Float_Input(0, 0, 0, 0);
        }
        else if (rend == flw::TableEditor::REND::SECRET) {
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
        w->position(0, strlen(val));

        _edit = w;
    }
    else if (rend == flw::TableEditor::REND::BOOLEAN) {
        auto w = new Fl_Check_Button(0, 0, 0, 0);

        w->box(FL_BORDER_BOX);
        w->color(color);
        w->labelcolor(textcolor);
        w->labelsize(textsize);
        w->value(*val && val[0] == '1' ? 1 : 0);

        _edit = w;
    }
    else if (rend == flw::TableEditor::REND::SLIDER) {
        auto w    = (Fl_Slider*) nullptr;
        auto curr = 0.0;
        auto min  = 0.0;
        auto max  = 0.0;
        auto step = 0.0;

        if (util::to_doubles(val, &curr, &min, &max, &step) == 4) {
            w = new Fl_Slider(0, 0, 0, 0);
            w->color(color);
            w->selection_color(textcolor);
            w->range(min, max);
            w->step(step);
            w->value(curr);
            w->type(FL_HOR_FILL_SLIDER);
            w->box(FL_BORDER_BOX);

            _edit = w;
        }
    }
    else if (rend == flw::TableEditor::REND::VALUE_SLIDER) {
        auto w    = (Fl_Slider*) nullptr;
        auto curr = 0.0;
        auto min  = 0.0;
        auto max  = 0.0;
        auto step = 0.0;

        if (util::to_doubles(val, &curr, &min, &max, &step) == 4) {
            w = new Fl_Value_Slider(0, 0, 0, 0);
            w->color(color);
            w->selection_color(textcolor);
            w->range(min, max);
            w->step(step);
            w->value(curr);
            w->type(FL_HOR_FILL_SLIDER);
            w->box(FL_BORDER_BOX);
            ((Fl_Value_Slider*) w)->textsize(textsize * 0.8);

            _edit = w;
        }
    }
    else if (rend == flw::TableEditor::REND::CHOICE) {
        auto choices = cell_choice(_curr_row, _curr_col);

        if (choices.size()) {
            auto w      = new Fl_Choice(0, 0, 0, 0);
            auto select = (std::size_t) 0;

            w->align(align);
            w->box(FL_BORDER_BOX);
            w->down_box(FL_BORDER_BOX);
            w->labelsize(textsize);
            w->textfont(textfont);
            w->textsize(textsize);

            for (std::size_t f = 0; f < choices.size(); f++) {
                auto& s = choices[f];

                w->add(s.c_str());

                if (s == val) {
                    select = f;
                }
            }

            w->value(select);
            _edit = w;
        }
    }
    else if (rend == flw::TableEditor::REND::INPUT_CHOICE) {
        auto choices = cell_choice(_curr_row, _curr_col);

        if (choices.size()) {
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

            for (auto& s : choices) {
                w->add(s.c_str());
            }

            if (*val) {
                w->value(val);
                w->input()->position(0, strlen(val));
            }

            _edit  = w;
            _edit2 = w->input();
            _edit3 = w->menubutton();
        }
    }

    if (_edit) {
        add(_edit);

        if (_edit2) {
            Fl::focus(_edit2);
        }
        else {
            Fl::focus(_edit);
        }
    }
}

//------------------------------------------------------------------------------
void flw::TableEditor::_edit_quick(const char* key) {
    auto rend = cell_rend(_curr_row, _curr_col);
    auto val  = ((TableDisplay*) this)->cell_value(_curr_row, _curr_col);
    char buffer[100];

    if (rend == flw::TableEditor::REND::INTEGER) {
        auto num = util::to_int(val);

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

        if (strcmp(val, buffer) != 0 && cell_value(_curr_row, _curr_col, buffer)) {
            _event_set(_curr_row, _curr_col, flw::TableEditor::EVENT::CHANGED);
            do_callback();
        }
    }
    else if (rend == flw::TableEditor::REND::NUMBER) {
        auto num = util::to_double(val);

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

        if (strcmp(val, buffer) != 0 && cell_value(_curr_row, _curr_col, buffer)) {
            _event_set(_curr_row, _curr_col, flw::TableEditor::EVENT::CHANGED);
            do_callback();
        }
    }
    else if (rend == flw::TableEditor::REND::DLG_DATE) {
        auto date = Date::FromString(val);

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

        auto string = date.format(Date::FORMAT::ISO_LONG);

        if (cell_value(_curr_row, _curr_col, string.c_str())) {
            _event_set(_curr_row, _curr_col, flw::TableEditor::EVENT::CHANGED);
            do_callback();
        }
    }
    else if (rend == flw::TableEditor::REND::SLIDER || rend == flw::TableEditor::REND::VALUE_SLIDER) {
        auto curr = 0.0;
        auto min  = 0.0;
        auto max  = 0.0;
        auto step = 0.0;

        if (util::to_doubles(val, &curr, &min, &max, &step) == 4) {
            if (strcmp(key, "+") == 0) {
                curr += step;
            }
            else if (strcmp(key, "++") == 0) {
                curr += (step * 10);
            }
            else if (strcmp(key, "+++") == 0) {
                curr += (step * 100);
            }
            else if (strcmp(key, "-") == 0) {
                curr -= step;
            }
            else if (strcmp(key, "--") == 0) {
                curr -= (step * 10);
            }
            else if (strcmp(key, "---") == 0) {
                curr -= (step * 100);
            }

            if (curr < min) {
                curr = min;
            }
            else if (curr > max) {
                curr = max;
            }

            auto val2 = FormatSlider(curr, min, max, step);

            if (strcmp(val, val2) != 0 && cell_value(_curr_row, _curr_col, val2)) {
                _event_set(_curr_row, _curr_col, flw::TableEditor::EVENT::CHANGED);
                do_callback();
            }
        }
    }
}

//------------------------------------------------------------------------------
void flw::TableEditor::_edit_show() {
    auto rend = cell_rend(_curr_row, _curr_col);
    auto val  = ((TableDisplay*) this)->cell_value(_curr_row, _curr_col);
    char buffer[100];

    if (rend == flw::TableEditor::REND::DLG_COLOR) {
        auto color1 = (int) util::to_int(val);
        auto color2 = (int) fl_show_colormap((Fl_Color) color1);

        snprintf(buffer, 20, "%d", color2);

        if ((color1 != color2) && cell_value(_curr_row, _curr_col, buffer)) {
            _event_set(_curr_row, _curr_col, flw::TableEditor::EVENT::CHANGED);
            do_callback();
        }
    }
    else if (rend == flw::TableEditor::REND::DLG_FILE) {
        auto result = fl_file_chooser(flw::TableEditor::SELECT_FILE, "", val, 0);

        if (result && strcmp(val, result) != 0 && cell_value(_curr_row, _curr_col, result)) {
            _event_set(_curr_row, _curr_col, flw::TableEditor::EVENT::CHANGED);
            do_callback();
        }
    }
    else if (rend == flw::TableEditor::REND::DLG_DIR) {
        auto result = fl_dir_chooser(flw::TableEditor::SELECT_DIR, val);

        if (result && strcmp(val, result) != 0 && cell_value(_curr_row, _curr_col, result)) {
            _event_set(_curr_row, _curr_col, flw::TableEditor::EVENT::CHANGED);
            do_callback();
        }
    }
    else if (rend == flw::TableEditor::REND::DLG_DATE) {
        auto date1  = Date::FromString(val);
        auto date2  = Date(date1);
        auto result = flw::dlg::date(flw::TableEditor::SELECT_DATE, date1, top_window());
        auto string = date1.format(Date::FORMAT::ISO_LONG);

        if (result && date1 != date2 && cell_value(_curr_row, _curr_col, string.c_str())) {
            _event_set(_curr_row, _curr_col, flw::TableEditor::EVENT::CHANGED);
            do_callback();
        }
    }
    else if (rend == flw::TableEditor::REND::DLG_LIST) {
        auto choices = cell_choice(_curr_row, _curr_col);

        if (choices.size()) {
            auto row = dlg::select(flw::TableEditor::SELECT_LIST, choices, val);

            if (row) {
                auto& string = choices[row - 1];

                if (string != val && cell_value(_curr_row, _curr_col, string.c_str())) {
                    _event_set(_curr_row, _curr_col, flw::TableEditor::EVENT::CHANGED);
                    do_callback();
                }
            }
        }
    }
}

//------------------------------------------------------------------------------
void flw::TableEditor::_edit_start(const char* key) {
    if (_select != flw::TableEditor::SELECT::NO && _edit == nullptr && _curr_row >= 1 && _curr_col >= 1 && cell_edit(_curr_row, _curr_col)) {
        Fl::event_clicks(0);
        Fl::event_is_click(0);

        flw::TableEditor::REND rend = cell_rend(_curr_row, _curr_col);

        if (*key) {
            _edit_quick(key);
        }
        else if (rend == flw::TableEditor::REND::DLG_COLOR ||
                 rend == flw::TableEditor::REND::DLG_FILE ||
                 rend == flw::TableEditor::REND::DLG_DIR ||
                 rend == flw::TableEditor::REND::DLG_DATE ||
                 rend == flw::TableEditor::REND::DLG_LIST) {
            _edit_show();
        }
        else {
            _edit_create();
        }

        redraw();
    }
}

//------------------------------------------------------------------------------
void flw::TableEditor::_edit_stop(bool save) {
    if (_edit) {
        auto rend = cell_rend(_curr_row, _curr_col);
        auto val  = ((TableDisplay*) this)->cell_value(_curr_row, _curr_col);
        auto stop = true;

        if (save) {
            if (rend == flw::TableEditor::REND::TEXT ||
                rend == flw::TableEditor::REND::INTEGER ||
                rend == flw::TableEditor::REND::NUMBER ||
                rend == flw::TableEditor::REND::SECRET) {

                auto input = (Fl_Input*) _edit;
                auto val2  = input->value();
                char buffer[100];

                if (rend == flw::TableEditor::REND::INTEGER) {
                    snprintf(buffer, 100, "%d", (int) util::to_int(val2));
                    val2 = buffer;
                }
                else if (rend == flw::TableEditor::REND::NUMBER) {
                    auto num = util::to_double(val2);
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
            else if (rend == flw::TableEditor::REND::BOOLEAN) {
                auto button = (Fl_Check_Button*) _edit;
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
            else if (rend == flw::TableEditor::REND::SLIDER || rend == flw::TableEditor::REND::VALUE_SLIDER) {
                auto slider = (Fl_Slider*) _edit;
                auto val2   = FormatSlider(slider->value(), slider->minimum(), slider->maximum(), slider->step());

                if (strcmp(val, val2) == 0) {
                    stop = true;
                    save = false;
                }
                else {
                    stop = cell_value(_curr_row, _curr_col, val2);
                }
            }
            else if (rend == flw::TableEditor::REND::CHOICE) {
                auto choice = (Fl_Choice*) _edit;
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
            else if (rend == flw::TableEditor::REND::INPUT_CHOICE) {
                auto input_choice = (Fl_Input_Choice*) _edit;
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

        if (stop) {
            remove(_edit);
            delete _edit;

            _edit            = nullptr;
            _edit2           = nullptr;
            _edit3           = nullptr;
            _current_cell[0] = 0;
            _current_cell[1] = 0;
            _current_cell[2] = 0;
            _current_cell[3] = 0;

            if (save) {
                _event_set(_curr_row, _curr_col, flw::TableEditor::EVENT::CHANGED);
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
int flw::TableEditor::_ev_keyboard_down() {
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
        else if (cmd && key == FL_BackSpace) {
            _delete_current_cell();
            return 1;
        }
        else if (key == FL_Delete) {
            _delete_current_cell();
        }
        else if (cmd && key == 'x') {
            auto val = ((TableDisplay*) this)->cell_value(_curr_row, _curr_col);

            Fl::copy(val, strlen(val), 1);
            _delete_current_cell();
            return 1;
        }
        else if (cmd && key == 'v') {
            Fl::paste(*this, 1);
            return 1;
        }
        else if (alt && shift && (key == '+' || text == "+" || key == FL_KP + '+')) { // !!! Problems with shift as it changes key depending on layout
            _edit_start("+++");
            return 1;
        }
        else if (alt && (key == '+' || text == "+" || key == FL_KP + '+')) {
            _edit_start("++");
            return 1;
        }
        else if (key == '+' || text == "+" || key == FL_KP + '+') {
            _edit_start("+");
            return 1;
        }
        else if (alt && shift && (key == '-' || text == "-" || key == FL_KP + '-')) { // !!! Problems with shift as it changes key depending on layout
            _edit_start("---");
            return 1;
        }
        else if (alt && (key == '-' || text == "-" || key == FL_KP + '-')) {
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
int flw::TableEditor::_ev_mouse_click () {
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
int flw::TableEditor::_ev_paste() {
    auto text = Fl::event_text();

    if (_curr_row > 0 && _curr_col > 0 && text && *text) {
        auto        rend = cell_rend(_curr_row, _curr_col);
        auto        val  = ((TableDisplay*) this)->cell_value(_curr_row, _curr_col);
        char        buffer[100];
        std::string string;

        switch(rend) {
            case flw::TableEditor::REND::CHOICE:
            case flw::TableEditor::REND::DLG_LIST:
            case flw::TableEditor::REND::SLIDER:
            case flw::TableEditor::REND::VALUE_SLIDER:
                return 1;

            case flw::TableEditor::REND::DLG_DIR:
            case flw::TableEditor::REND::DLG_FILE:
            case flw::TableEditor::REND::INPUT_CHOICE:
            case flw::TableEditor::REND::SECRET:
            case flw::TableEditor::REND::TEXT:
                break;

            case flw::TableEditor::REND::BOOLEAN:
                if (strcmp("1", text) == 0 || strcmp("true", text) == 0) {
                    text = "1";
                }
                else if (strcmp("0", text) == 0 || strcmp("false", text) == 0) {
                    text = "0";
                }
                else {
                    return 1;
                }

            case flw::TableEditor::REND::DLG_COLOR:
            case flw::TableEditor::REND::INTEGER:
                if (*text < '0' || *text > '9') {
                    return 1;
                }
                else {
                    auto num = util::to_int(text);
                    snprintf(buffer, 100, "%lld", (long long int) num);
                    text = buffer;

                    if (rend == flw::TableEditor::REND::DLG_COLOR && (num < 0 || num > 255)) {
                        return 1;
                    }
                    else {
                        break;
                    }
                }

            case flw::TableEditor::REND::NUMBER: {
                if (*text < '0' || *text > '9') {
                    return 1;
                }
                else {
                    auto num = util::to_double(text);
                    snprintf(buffer, 100, "%f", num);
                    text = buffer;
                    break;
                }
            }

            case flw::TableEditor::REND::DLG_DATE: {
                auto date = Date::FromString(text);

                if (date.year() == 1 && date.month() == 1 && date.day() == 1) {
                    return 1;
                }
                else {
                    string = date.format(Date::FORMAT::ISO_LONG);
                    text = string.c_str();
                    break;
                }
            }
        }

        if (strcmp(val, text) != 0 && cell_value(_curr_row, _curr_col, text)) {
            _event_set(_curr_row, _curr_col, flw::TableEditor::EVENT::CHANGED);
            do_callback();
            redraw();
        }
    }

    return 1;
}

//------------------------------------------------------------------------------
const char* flw::TableEditor::FormatSlider(double val, double min, double max, double step) {
    static char result[200];

    snprintf(result, 200, "%.4f %.4f %.4f %.4f", val, min, max, step);
    return result;
}

//------------------------------------------------------------------------------
int flw::TableEditor::handle(int event) {
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
            ret = _ev_keyboard_down();
        }
        else if (event == FL_PUSH) {
            ret = _ev_mouse_click();
        }
    }

    if (ret == 1) {
        return ret;
    }
    else {
        return TableDisplay::handle(event);
    }
}

// MALAGMA_OFF
