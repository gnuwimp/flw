// Copyright gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#include "tableeditor.h"
#include "fontdialog.h"
#include "theme.h"
#include <stdarg.h>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl.H>

using namespace flw;

char* string_format_dup(const char* format, ...) {
    int     n = 0;
    char    buffer[2001];
    va_list args;

    va_start(args, format);
    n = vsnprintf(buffer, 2000, format, args);
    va_end(args);
    if (n < 0 || n == 2000) {
        *buffer = 0;
    }

    return strdup(buffer);
}

#define ROWS 2000
#define COLS 100

class TestTable : public TableEditor {
public:
    TableEditor::FORMAT                 dec[ROWS + 1];
    char*                               values[ROWS + 1][COLS + 1];
    int                                 widths[COLS + 1];
    std::vector<std::string>            choices;

    TestTable() : TableEditor() {
        choices.push_back("Baalberith");
        choices.push_back("Balaam");
        choices.push_back("Baphomet");
        choices.push_back("Beelzebub");
        choices.push_back("Behemoth");
        choices.push_back("Chemosh");
        choices.push_back("Coyote");
        choices.push_back("Euronymous");
        choices.push_back("Fenriz");
        choices.push_back("Gorgo");
        choices.push_back("Haborym");
        choices.push_back("Kali");
        choices.push_back("Marduk");
        choices.push_back("Mastema");
        choices.push_back("Midgard");
        choices.push_back("Moloch");
        choices.push_back("Nergal");
        choices.push_back("Sammael");
        choices.push_back("Shaitan");
        choices.push_back("Thamuz");
        choices.push_back("Typhon");
        choices.push_back("Yen-lo-Wang");

        for (auto r = 0; r <= ROWS; r++) {
            for (auto c = 0; c <= COLS; c++) {
                values[r][c] = 0;
            }
        }

        values[0][0]  = strdup("0,0");
        values[0][1]  = strdup("TEXT");
        values[0][2]  = strdup("INT");
        values[0][3]  = strdup("FLOAT");
        values[0][4]  = strdup("BOOL");
        values[0][5]  = strdup("SEC");
        values[0][6]  = strdup("CHOICE");
        values[0][7]  = strdup("ICHOICE");
        values[0][8]  = strdup("SLIDER");
        values[0][9]  = strdup("VSLIDER");
        values[0][10] = strdup("COLOR");
        values[0][11] = strdup("FILE");
        values[0][12] = strdup("DIR");
        values[0][13] = strdup("DATE");
        values[0][14] = strdup("LIST");

        for (int c = 15; c <= COLS; c++) {
            values[0][c] = string_format_dup("Col %d", c);
        }

        for (int c = 0; c <= COLS; c++) {
            widths[c] = labelsize() * 10;
        }

        for (int r = 1; r <= ROWS; r++) {
            values[r][0]  = string_format_dup("%d", r);
            values[r][1]  = string_format_dup("Hello %d", r);
            values[r][2]  = strdup("6666");
            values[r][3]  = string_format_dup("%.19Lf", (long double) (100000.0) / (long double) 3.1);
            values[r][4]  = strdup("1");
            values[r][5]  = strdup("Password");
            values[r][6]  = (r % 2 == 0) ? strdup("Moloch") : strdup("Beelzebub");
            values[r][7]  = (r % 2 == 0) ? strdup("Behemoth") : strdup("Marduk");
            values[r][8]  = string_format_dup(TableEditor::FormatSlider(rand() % 10 * 10, 0, 100, 10));
            values[r][9]  = string_format_dup(TableEditor::FormatSlider(rand() % 1000 - rand() % 1000, -1000, 1000, 10));
            values[r][10] = string_format_dup("%d", rand() % 256);
            values[r][11] = strdup("Makefile");
            values[r][12] = strdup("obj");
            values[r][13] = string_format_dup("2001-%02d-%02d", rand() % 11 + 1, rand() % 27 + 1);
            values[r][14] = (r % 2 == 0) ? strdup("Coyote") : strdup("Shaitan");
        }

        auto c = 0;
        for (int r = 1; r <= ROWS; r++) {
            if      (c == 0) dec[r] = TableEditor::FORMAT::DEFAULT;
            else if (c == 1) dec[r] = TableEditor::FORMAT::DEC_0;
            else if (c == 2) dec[r] = TableEditor::FORMAT::DEC_1;
            else if (c == 3) dec[r] = TableEditor::FORMAT::DEC_2;
            else if (c == 4) dec[r] = TableEditor::FORMAT::DEC_3;
            else if (c == 5) dec[r] = TableEditor::FORMAT::DEC_4;
            else if (c == 6) dec[r] = TableEditor::FORMAT::DEC_5;
            else if (c == 7) dec[r] = TableEditor::FORMAT::DEC_6;
            else if (c == 8) dec[r] = TableEditor::FORMAT::DEC_7;
            else if (c == 9) {
                dec[r] = TableEditor::FORMAT::DEC_8;
                c = -1;
            }

            c++;
        }

        size(ROWS, COLS);
        // selection_color(FL_RED);
        // set_height(72);
        // set_height(18);
    }

    ~TestTable() {
        for (auto r = 0; r <= ROWS; r++) {
            for (auto c = 0; c <= COLS; c++) {
                free(values[r][c]);
            }
        }
    }

    Fl_Align cell_align(int row, int col) override {
        if (col == 2) {
            return FL_ALIGN_RIGHT;
        }
        else if (col == 3) {
            return FL_ALIGN_RIGHT;
        }
        else if (col == 4) {
            if (row == 1) {
                return FL_ALIGN_LEFT;
            }
            else if (row == 2) {
                return FL_ALIGN_CENTER;
            }
            else if (row == 3) {
                return FL_ALIGN_RIGHT;
            }
            else {
                return FL_ALIGN_CENTER;
            }
        }
        else if (col == 9) {
            return FL_ALIGN_RIGHT;
        }
        else if (col == 15) {
            return FL_ALIGN_CENTER;
        }

        return TableDisplay::cell_align(row, col);
    }

    std::vector<std::string> cell_choice(int row, int col) override {
        (void) row;
        (void) col;

        return choices;
    }

    Fl_Color cell_color(int row, int col) override {
        if (col == 4) {
            return FL_YELLOW;
        }
        else if (col == 9) {
            return FL_RED;
        }
        else if (col == 17) {
            return FL_YELLOW;
        }
        else {
            return TableDisplay::cell_color(row, col);
        }
    }

    bool cell_edit(int row, int col) override {
        (void) row;

        if (col == 18) {
            return false;
        }

        return true;
    }

    TableEditor::FORMAT cell_format(int row, int col) override {
        if (col == 2 && row % 2 == 0) {
            return TableEditor::FORMAT::INT_SEP;
        }
        else if (col == 3) {
            return dec[row];
        }
        else if (col == 5 && row % 2 == 0) {
            return TableEditor::FORMAT::SECRET_DOT;
        }
        else if (col == 9 && row % 2 == 0) {
            return TableEditor::FORMAT::DEC_3;
        }
        else if (col == 13 && row % 2 == 0) {
            return TableEditor::FORMAT::DATE_WORLD;
        }
        else {
            return TableEditor::FORMAT::DEFAULT;
        }
    }

    Fl_Color cell_textcolor(int row, int col) override {
        if (row == this->row() && col == column()) {
            return TableDisplay::cell_textcolor(row, col);
        }
        else if (col == 1) {
            return FL_GREEN;
        }
        else if (col == 6) {
            return FL_RED;
        }
        else if (col == 7) {
            return FL_GREEN;
        }
        else if (col == 9) {
            return FL_BLUE;
        }
        else {
            return TableDisplay::cell_textcolor(row, col);
        }
    }

    Fl_Font cell_textfont(int row, int col) override {
        if (col == 2 || col == 3) {
            return FL_COURIER;
        }
        else {
            return TableDisplay::cell_textfont(row, col);
        }
    }

    Fl_Fontsize cell_textsize(int row, int col) override {
        if (col > 50) {
            return flw::PREF_FONTSIZE * 1.5;
        }
        else if (col > 15) {
            return flw::PREF_FONTSIZE * 0.5;
        }
        else {
            return TableDisplay::cell_textsize(row, col);
        }
    }

    TableEditor::REND cell_rend(int row, int col) override {
        if (col ==  1) {
            return TableEditor::REND::TEXT;
        }
        else if (col ==  2) {
            return TableEditor::REND::INTEGER;
        }
        else if (col ==  3) {
            return TableEditor::REND::NUMBER;
        }
        else if (col ==  4) {
            return TableEditor::REND::BOOLEAN;
        }
        else if (col ==  5) {
            return TableEditor::REND::SECRET;
        }
        else if (col ==  6) {
            return TableEditor::REND::CHOICE;
        }
        else if (col ==  7) {
            return TableEditor::REND::INPUT_CHOICE;
        }
        else if (col ==  8) {
            return TableEditor::REND::SLIDER;
        }
        else if (col ==  9) {
            return TableEditor::REND::VALUE_SLIDER;
        }
        else if (col == 10) {
            return TableEditor::REND::DLG_COLOR;
        }
        else if (col == 11) {
            return TableEditor::REND::DLG_FILE;
        }
        else if (col == 12) {
            return TableEditor::REND::DLG_DIR;
        }
        else if (col == 13) {
            return TableEditor::REND::DLG_DATE;
        }
        else if (col == 14) {
            return TableEditor::REND::DLG_LIST;
        }
        else {
            return TableEditor::cell_rend(row, col);
        }
    }

    const char* cell_value(int row, int col) override {
        return values[row][col] ? values[row][col] : "!";
    }

    bool cell_value(int row, int col, const char* value) override {
        if (col == 9) {
            fprintf(stderr, "%f\n", atof(value));
            fflush(stderr);
        }

        if (strcmp(value, "hello") == 0) {
            return false;
        }

        free(values[row][col]);
        values[row][col] = strdup(value);
        return true;
    }

    int cell_width(int col) override {
        return widths[col];
    }

    void cell_width(int col, int width) override {
        widths[col] = width;
    }
};

class Test : public Fl_Double_Window {
public:
    Fl_Menu_Bar* bar;
    TestTable*   table;

    Test() : Fl_Double_Window(800, 480, "test_tableeditor.cpp") {
        bar   = new Fl_Menu_Bar(0, 0, 0, 0);
        table = new TestTable();

        bar->add("Debug/Clear", 0, Test::CallbackDebugClear, this);

        bar->add("Size/Shrink", 0, Test::CallbackShrink, this);
        bar->add("Event/Always", 0, Test::CallbackEvent, this, FL_MENU_TOGGLE);

        table->header(true, true);
        table->lines(true, true);
        table->resize_column_width(true);
        table->select_mode(TableDisplay::SELECT::CELL);
        table->resize_column_width(true);
        table->take_focus();
        table->callback(Test::Callback, table);

        // table->header(true, false);
        // table->header(false, false);
        // bar->hide();
        // table->lines(false, false);

        resizable(this);
        resize(x(), y(), w(), h());
        size_range(64, 48);
        bar->textfont(flw::PREF_FONT);
        bar->textsize(flw::PREF_FONTSIZE);
        util::labelfont(this);
    }

    void resize(int X, int Y, int W, int H) {
        Fl_Double_Window::resize(X, Y, W, H);
        bar->resize(0, 0, W, flw::PREF_FONTSIZE * 2);
        table->resize(0, flw::PREF_FONTSIZE * 2, W, H - flw::PREF_FONTSIZE * 2);
        // table->resize(100, 100, W - 200, H - 200);
    }

    static void Callback(Fl_Widget*, void* v) {
        auto table = (TableEditor*) v;

        if (table->event() == TableDisplay::EVENT::UNDEFINED) {
            fprintf(stderr, "Callback: row=%4d, col=%4d  =>  TableDisplay::EVENT::UNDEFINED\n", table->event_row(), table->event_col());
        }
        else if (table->event() == TableDisplay::EVENT::CHANGED) {
            fprintf(stderr, "Callback: row=%4d, col=%4d  =>  TableDisplay::EVENT::CHANGED\n", table->event_row(), table->event_col());
        }
        else if (table->event() == TableDisplay::EVENT::CURSOR) {
            fprintf(stderr, "Callback: row=%4d, col=%4d  =>  TableDisplay::EVENT::CURSOR\n", table->event_row(), table->event_col());
        }
        else if (table->event() == TableDisplay::EVENT::COLUMN) {
            fprintf(stderr, "Callback: row=%4d, col=%4d  =>  TableDisplay::EVENT::COLUMN\n", table->event_row(), table->event_col());
        }
        else if (table->event() == TableDisplay::EVENT::COLUMN_CTRL) {
            fprintf(stderr, "Callback: row=%4d, col=%4d  =>  TableDisplay::EVENT::COLUMN_CTRL\n", table->event_row(), table->event_col());
        }
        else if (table->event() == TableDisplay::EVENT::ROW) {
            fprintf(stderr, "Callback: row=%4d, col=%4d  =>  TableDisplay::EVENT::ROW\n", table->event_row(), table->event_col());
        }
        else if (table->event() == TableDisplay::EVENT::ROW_CTRL) {
            fprintf(stderr, "Callback: row=%4d, col=%4d  =>  TableDisplay::EVENT::ROW_CTRL\n", table->event_row(), table->event_col());
        }

        fflush(stderr);
    }

    static void CallbackDebugClear(Fl_Widget*, void* v) {
        auto w = (Test*) v;

        w->table->clear();
    }

    static void CallbackShrink(Fl_Widget*, void* v) {
        auto w = (Test*) v;

        w->table->size(10, 5);
    }

    static void CallbackEvent(Fl_Widget*, void* v) {
        auto w    = (Test*) v;
        auto item = (Fl_Menu_Item*) w->bar->find_item("Event/Always");

        w->table->send_changed_event_always(item->value());
    }
};

int main(int argc, const char** argv) {
    if (flw::theme::parse(argc, argv) == false) {
        flw::theme::load("gtk");
    }

    Test win;
    win.show();
    return Fl::run();
}
