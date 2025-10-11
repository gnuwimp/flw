// Copyright gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#include "flw.h"

// [flw::table::test example]

// Implement virtual methods in flw::table::Editor to get and update data.

class TestTable : public flw::table::Editor {
public:
    static constexpr int ROWS    = 100;
    static constexpr int COLUMNS =  10;

    flw::StringVector choices;
    int               widths[COLUMNS + 1];
    std::string       values[ROWS + 1][COLUMNS + 1];

    TestTable(int X, int Y, int W, int H) : flw::table::Editor(X, Y, W, H) {
        header(true, true);
        lines(true, true);
        select_mode(flw::table::Select::CELL);
        resize_column_width(true);
        expand_last_column(true);

        choices.push_back("Basic");
        choices.push_back("C/C++");
        choices.push_back("Java");
        choices.push_back("Kotlin");
        choices.push_back("Lua");
        choices.push_back("Pascal");
        choices.push_back("Perl");
        choices.push_back("Python");
        choices.push_back("Rust");

        // Set header labels for all columns.
        values[0][1]  = "TEXT";
        values[0][2]  = "INTEGER";
        values[0][3]  = "NUMBER";
        values[0][4]  = "BOOLEAN";
        values[0][5]  = "CHOICE";
        values[0][6]  = "SLIDER";
        values[0][7]  = "COLOR";
        values[0][8]  = "FILE";
        values[0][9]  = "DATE";
        values[0][10] = "LIST";

        for (int c = 1; c <= COLUMNS; c++) {
            widths[c] = labelsize() * 8;
        }

        // Set wwidth of row number column.
        widths[0] = labelsize() * 4;

        srand(time(0));

        // Create values for all rows.
        for (int r = 1; r <= ROWS; r++) {
            values[r][0]  = flw::util::format("%d", r);
            values[r][1]  = "Hello";
            values[r][2]  = flw::util::format("%d", rand() % 1000);
            values[r][3]  = flw::util::format("%d.%d", rand() % 1000, rand() % 10);
            values[r][4]  = "1";
            values[r][5]  = choices[rand() % choices.size()];
            values[r][6]  = flw::table::format_slider(rand() % 1000, 0, 1000, 20);
            values[r][7]  = flw::util::format("%d", rand() % 256);
            values[r][8]  = "Makefile";
            values[r][9]  = flw::util::format("2001-%02d-%02d", rand() % 11 + 1, rand() % 27 + 1);
            values[r][10] = choices[rand() % choices.size()];
        }

        size(ROWS, COLUMNS);
    }

    Fl_Align cell_align(int, int col) override {
        return col == 4 ? FL_ALIGN_CENTER : FL_ALIGN_LEFT;
    }

    std::vector<std::string> cell_choice(int, int) override {
        return choices;
    }

    Fl_Color cell_color(int row, int col) override {
        return col == 4 ? FL_YELLOW : flw::table::Display::cell_color(row, col);
    }

    bool cell_edit(int, int) override {
        return true;
    }

    flw::table::Format cell_format(int, int col) override {
        return col == 3 ? flw::table::Format::DEC_2 : flw::table::Format::DEFAULT;
    }

    Fl_Color cell_textcolor(int row, int col) override {
        return col == 1 ? FL_DARK_GREEN : flw::table::Display::cell_textcolor(row, col);
    }

    Fl_Font cell_textfont(int row, int col) override {
        return col == 2 || col == 3 ?  FL_COURIER : flw::table::Display::cell_textfont(row, col);
    }

    Fl_Fontsize cell_textsize(int row, int col) override {
        return flw::table::Display::cell_textsize(row, col);
    }

    flw::table::Type cell_type(int row, int col) override {
        if (col ==  2)  return flw::table::Type::INTEGER;
        if (col ==  3)  return flw::table::Type::NUMBER;
        if (col ==  4)  return flw::table::Type::BOOLEAN;
        if (col ==  5)  return flw::table::Type::CHOICE;
        if (col ==  6)  return flw::table::Type::SLIDER;
        if (col ==  7)  return flw::table::Type::COLOR;
        if (col ==  8)  return flw::table::Type::FILE;
        if (col ==  9)  return flw::table::Type::DATE;
        if (col ==  10) return flw::table::Type::LIST;
        else            return flw::table::Editor::cell_type(row, col);
    }

    std::string cell_value(int row, int col) override {
        return values[row][col];
    }

    bool cell_value(int row, int col, const std::string& value) override {
        values[row][col] = value;
        return true;
    }

    int cell_width(int col) override {
        return widths[col];
    }

    void cell_width(int col, int width) override {
        widths[col] = width;
    }
};

// [flw::table::test example]

int main() {
    Fl::scheme("oxy");
    Fl::screen_scale(0, 1.0);

    // [flw::table::Editor example]

    auto win   = new Fl_Double_Window(Fl::w() / 2 - 700, Fl::h() / 2 - 250, 1400, 500, "flw::table::Editor");
    auto table = new TestTable(0, 0, 1400, 500);

    win->resizable(table);
    win->show();
    Fl::run();

    // [flw::table::Editor example]
}
