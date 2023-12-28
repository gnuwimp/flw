// Copyright gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#include "tabledisplay.h"
#include "theme.h"
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Menu_Bar.H>
#include <stdarg.h>
#include <FL/Fl_Color_Chooser.H>
#include <FL/Fl.H>

using namespace flw;

#define ROWS 10'000
#define COLS 100

class TestTable : public TableDisplay {
public:
    char                                buffer[200];
    int                                 widths[ROWS + 1];

    TestTable() : TableDisplay() {
        widths[0] = flw::PREF_FONTSIZE * 8;

        for (auto f = 1; f <= ROWS; f++) {
            widths[f] = flw::PREF_FONTSIZE * 12;
        }

        size(ROWS, COLS);
    }

    Fl_Align cell_align(int, int col) override {
        if (col == 0) {
            return FL_ALIGN_RIGHT;
        }
        else if (col == 1) {
            return FL_ALIGN_RIGHT;
        }
        else if (col == 2) {
            return FL_ALIGN_CENTER;
        }
        else {
            return FL_ALIGN_LEFT;
        }
    }

    Fl_Color cell_color(int row, int col) override {
        if (col == 0) {
            return TableDisplay::cell_color(row, col);
        }
        else if (col == 3) {
            return FL_GREEN;
        }
        else if (col == 5) {
            return FL_CYAN;
        }
        else {
            return TableDisplay::cell_color(row, col);
        }
    }

    Fl_Color cell_textcolor(int row, int col) override {
        if (col == 0) {
            return TableDisplay::cell_textcolor(row, col);
        }
        else if (row == this->row() && col == column()) {
            return TableDisplay::cell_textcolor(row, col);
        }
        else if (col == 3) {
            return FL_RED;
        }
        else if (col == 5) {
            return FL_BLUE;
        }
        else {
            return TableDisplay::cell_textcolor(row, col);
        }
    }

    Fl_Font cell_textfont(int row, int col) override {
        if (col == 0) {
            return FL_COURIER_BOLD;
        }
        else {
            return TableDisplay::cell_textfont(row, col);
        }
    }

    Fl_Fontsize cell_textsize(int row, int col) override {
        if (col == 0) {
            return TableDisplay::cell_textsize(row, col) + 4;
        }
        else {
            return TableDisplay::cell_textsize(row, col);
        }
    }

    const char* cell_value(int row, int col) override {
        if (col == 0 && row == 0) {
            return "";
        }
        else if (col == 0) {
            sprintf(buffer, "%d", row);
        }
        else if (row == 0) {
            sprintf(buffer, "%d", col);
        }
        else {
            sprintf(buffer, "%06d - %06d", row, col);
        }

        return buffer;
    }

    int cell_width(int col) override {
        return widths[col];
    }

    void cell_width(int col, int width) override {
        widths[col] = width;
    }
};

class Test : public Fl_Double_Window {
    Fl_Menu_Bar* bar;
    TestTable*   table;

public:
    Test() : Fl_Double_Window(800, 480, "test_tabledisplay.cpp") {
        bar   = new Fl_Menu_Bar(0, 0, 0, 0);
        table = new TestTable();

        bar->add("Select/TableDisplay::SELECT::NO", 0, CallbackSelect, this, FL_MENU_RADIO);
        bar->add("Select/TableDisplay::SELECT::CELL", 0, CallbackSelect, this, FL_MENU_RADIO | FL_MENU_VALUE);
        bar->add("Select/TableDisplay::SELECT::ROW", 0, CallbackSelect, this, FL_MENU_RADIO | FL_MENU_DIVIDER);

        bar->add("Header/No", 0, CallbackHeader, this, FL_MENU_RADIO);
        bar->add("Header/Column", 0, CallbackHeader, this, FL_MENU_RADIO);
        bar->add("Header/Row", 0, CallbackHeader, this, FL_MENU_RADIO);
        bar->add("Header/Both", 0, CallbackHeader, this, FL_MENU_RADIO | FL_MENU_VALUE);

        bar->add("Lines/No", 0, CallbackLines, this, FL_MENU_RADIO);
        bar->add("Lines/Horizontal", 0, CallbackLines, this, FL_MENU_RADIO);
        bar->add("Lines/Vertical", 0, CallbackLines, this, FL_MENU_RADIO);
        bar->add("Lines/Both", 0, CallbackLines, this, FL_MENU_RADIO);

        bar->add("Resize/No", 0, CallbackResize, this, FL_MENU_RADIO);
        bar->add("Resize/Yes", 0, CallbackResize, this, FL_MENU_RADIO);

        bar->add("Expand Last Col/No", 0, CallbackExpand, this, FL_MENU_RADIO);
        bar->add("Expand Last Col/Yes", 0, CallbackExpand, this, FL_MENU_RADIO);

        bar->add("Scrollbar/No", 0, CallbackScrollbar, this, FL_MENU_RADIO);
        bar->add("Scrollbar/Horizontal", 0, CallbackScrollbar, this, FL_MENU_RADIO);
        bar->add("Scrollbar/Vertical", 0, CallbackScrollbar, this, FL_MENU_RADIO);
        bar->add("Scrollbar/Both", 0, CallbackScrollbar, this, FL_MENU_RADIO);

        ((Fl_Menu_Item*) bar->find_item("Header/Both"))->setonly();
        ((Fl_Menu_Item*) bar->find_item("Lines/Both"))->setonly();
        ((Fl_Menu_Item*) bar->find_item("Resize/Yes"))->setonly();
        ((Fl_Menu_Item*) bar->find_item("Expand Last Col/No"))->setonly();
        ((Fl_Menu_Item*) bar->find_item("Scrollbar/Both"))->setonly();
        ((Fl_Menu_Item*) bar->find_item("Select/TableDisplay::SELECT::CELL"))->setonly();

        table->header(true, true);
        table->lines(true, true);
        table->resize_column_width(true);
        table->select_mode(TableDisplay::SELECT::CELL);
        table->resize_column_width(true);
        table->take_focus();
        table->callback(Callback, table);

        // table->header(true, false);
        // table->header(false, false);
        // bar->hide();
        // table->lines(false, false);

        resizable(this);
        size_range(64, 48);
        resize(x(), y(), w(), h());
        bar->textfont(flw::PREF_FONT);
        bar->textsize(flw::PREF_FONTSIZE);
        util::labelfont(this);
    }

    void resize(int X, int Y, int W, int H) override {
        auto b = 0;
        Fl_Double_Window::resize(X, Y, W, H);
        bar->resize(0, 0, W, flw::PREF_FONTSIZE * 2);
        table->resize(b, flw::PREF_FONTSIZE * 2 + b, W - b * 2, H - flw::PREF_FONTSIZE * 2 - b * 2);
    }

    static void Callback(Fl_Widget*, void* v) {
        TableDisplay* table = (TableDisplay*) v;

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

    static void CallbackExpand(Fl_Widget*, void* v) {
        Test* w = (Test*) v;

        if (w->bar->find_item("Expand Last Col/No")->value())
            w->table->expand_last_column(false);
        else if (w->bar->find_item("Expand Last Col/Yes")->value())
            w->table->expand_last_column(true);
        w->table->redraw();
    }

    static void CallbackHeader(Fl_Widget*, void* v) {
        Test* w = (Test*) v;

        if (w->bar->find_item("Header/No")->value()) {
            w->table->header(false, false);
        }
        else if (w->bar->find_item("Header/Column")->value()) {
            w->table->header(false, true);
        }
        else if (w->bar->find_item("Header/Row")->value()) {
            w->table->header(true, false);
        }
        else if (w->bar->find_item("Header/Both")->value()) {
            w->table->header(true, true);
        }

        w->table->redraw();
    }


    static void CallbackLines(Fl_Widget*, void* v) {
        Test* w = (Test*) v;

        if (w->bar->find_item("Lines/No")->value()) {
            w->table->lines(false, false);
        }
        else if (w->bar->find_item("Lines/Horizontal")->value()) {
            w->table->lines(false, true);
        }
        else if (w->bar->find_item("Lines/Vertical")->value()) {
            w->table->lines(true, false);
        }
        else if (w->bar->find_item("Lines/Both")->value()) {
            w->table->lines(true, true);
        }

        w->table->redraw();
    }

    static void CallbackResize(Fl_Widget*, void* v) {
        Test* w = (Test*) v;

        if (w->bar->find_item("Resize/No")->value()) {
            w->table->resize_column_width(false);
        }
        else if (w->bar->find_item("Resize/Yes")->value()) {
            w->table->resize_column_width(true);
        }

        w->table->redraw();
    }

    static void CallbackScrollbar(Fl_Widget*, void* v) {
        Test* w = (Test*) v;

        if (w->bar->find_item("Scrollbar/No")->value()) {
            w->table->scrollbar(false, false);
        }
        else if (w->bar->find_item("Scrollbar/Horizontal")->value()) {
            w->table->scrollbar(false, true);
        }
        else if (w->bar->find_item("Scrollbar/Vertical")->value()) {
            w->table->scrollbar(true, false);
        }
        else if (w->bar->find_item("Scrollbar/Both")->value()) {
            w->table->scrollbar(true, true);
        }

        w->table->redraw();
    }

    static void CallbackSelect(Fl_Widget*, void* v) {
        Test* w = (Test*) v;

        if (w->bar->find_item("Select/TableDisplay::SELECT::NO")->value()) {
            w->table->select_mode(TableDisplay::SELECT::NO);
        }
        else if (w->bar->find_item("Select/TableDisplay::SELECT::CELL")->value()) {
            w->table->select_mode(TableDisplay::SELECT::CELL);
        }
        else if (w->bar->find_item("Select/TableDisplay::SELECT::ROW")->value()) {
            w->table->select_mode(TableDisplay::SELECT::ROW);
        }

        w->table->redraw();
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
