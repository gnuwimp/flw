// Copyright gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#include "flw.h"

#ifndef FLW_AMALGAM
    #include "tableeditor.h"
    #include "theme.h"
#endif

#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Menu_Bar.H>

using namespace flw;

const std::string HAMLET_TEXT = R"(
To be, or not to be: that is the question:
Whether 'tis nobler in the mind to suffer
The slings and arrows of outrageous fortune,
Or to take arms against a sea of troubles,
And by opposing end them? To die: to sleep;
No more; and by a sleep to say we end
The heart-ache and the thousand natural shocks
That flesh is heir to, 'tis a consummation
Devoutly to be wish'd. To die, to sleep;
To sleep: perchance to dream: ay, there's the rub;
For in that sleep of death what dreams may come
When we have shuffled off this mortal coil,
)";

#define ROWS 2'000
#define COLS   100

/*
 *      _______        _ _______    _     _
 *     |__   __|      | |__   __|  | |   | |
 *        | | ___  ___| |_ | | __ _| |__ | | ___
 *        | |/ _ \/ __| __|| |/ _` | '_ \| |/ _ \
 *        | |  __/\__ \ |_ | | (_| | |_) | |  __/
 *        |_|\___||___/\__||_|\__,_|_.__/|_|\___|
 *
 *
 */

//----------------------------------------------------------------------------------
class TestTable : public table::Editor {
public:
    flw::StringVector   choices;
    int                 widths[COLS + 1];
    std::string         values[ROWS + 1][COLS + 1];
    int                 start_free;

    TestTable() : Editor() {
        start_free = 17;

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

        for (int c = start_free; c <= COLS; c++) {
            values[0][c] = util::format("Col %d", c);
        }

        values[0][0]  = "0,0";
        values[0][1]  = "TEXT";
        values[0][2]  = "INTEGER";
        values[0][3]  = "NUMBER";
        values[0][4]  = "BOOLEAN";
        values[0][5]  = "SECRET";
        values[0][6]  = "CHOICE";
        values[0][7]  = "ICHOICE";
        values[0][8]  = "SLIDER";
        values[0][9]  = "VSLIDER";
        values[0][10] = "COLOR";
        values[0][11] = "FILE";
        values[0][12] = "DIR";
        values[0][13] = "DATE";
        values[0][14] = "LIST";
        values[0][15] = "MTEXT";
        values[0][16] = "VSLIDER";
        values[0][start_free + 1] = "READONLY";

        for (int r = 1; r <= ROWS; r++) {
            values[r][0]  = util::format("%d", r);
            values[r][1]  = util::format("Hello %d", r);
            values[r][2]  = util::format("%d", rand() % 1'000'000);
            values[r][3]  = util::format("%f", (double) (rand() % 100'000) * 3.456321);
            if (rand() % 10 == 0) values[r][3] = "inf";
            values[r][4]  = rand() % 2 == 1 ? "1" : "0";
            if (rand() % 2 == 0) values[r][4] += values[r][1];
            values[r][5]  = "Password";
            values[r][6]  = (r % 2 == 0) ? "Moloch" : "Beelzebub";
            values[r][7]  = (r % 2 == 0) ? "Behemoth" : "Marduk";
            values[r][8]  = table::format_slider(rand() % 10, -10, 10, 0.1);
            values[r][9]  = table::format_slider2((rand() % 1'000) * 1'000'000, -1'000'000'000, 1'000'000'000, 1'000'000);
#ifdef _WIN32
            values[r][10] = util::format("%u", rand() * rand() * 4);
#else
            values[r][10] = util::format("%u", rand() * 2);
#endif
            values[r][11] = "Makefile";
            values[r][12] = "obj";
            values[r][13] = util::format("2001-%02d-%02d", rand() % 11 + 1, rand() % 27 + 1);
            values[r][14] = (r % 2 == 0) ? "Coyote" : "Shaitan";
            values[r][15] = HAMLET_TEXT;
            values[r][16] = table::format_slider(rand() % 10, 0.0, 10.0, 0.01);
            
            values[r][18] = util::format("%04d - %04d", r, 18);
        }

        size(ROWS, COLS);
        resize_col();
        draw_inactive_lighter(true);
         //selection_color(FL_RED);
         //set_height(72);
         //set_height(18);
    }

    //------------------------------------------------------------------------------
    Fl_Align cell_align(int row, int col) override {
        if (col == 2) {
            return FL_ALIGN_RIGHT;
        }
        else if (col == 3) {
            return FL_ALIGN_RIGHT;
        }
        else if (col == 4) {
            if (row % 3 == 0) {
                return FL_ALIGN_RIGHT;
            }
            else if (row % 2 == 0) {
                return FL_ALIGN_CENTER;
            }
            else {
                return FL_ALIGN_LEFT;
            }
        }
        else if (col == 9 || col == 16) {
            return FL_ALIGN_RIGHT;
        }
        else if (col == start_free) {
            return FL_ALIGN_CENTER;
        }

        return table::Display::cell_align(row, col);
    }

    //------------------------------------------------------------------------------
    std::vector<std::string> cell_choice(int row, int col) override {
        (void) row;
        (void) col;

        return choices;
    }

    //------------------------------------------------------------------------------
    Fl_Color cell_color(int row, int col) override {
        if (col == 4 || col == 9 || col == start_free) {
            return FL_YELLOW;
            //return FL_BACKGROUND2_COLOR;
        }
        else {
            return table::Display::cell_color(row, col);
        }
    }

    //------------------------------------------------------------------------------
    bool cell_edit(int row, int col) override {
        (void) row;

        if (col == start_free + 1) {
            return false;
        }

        return true;
    }

    //------------------------------------------------------------------------------
    table::Format cell_format(int row, int col) override {
        if (col == 2 && row % 2 == 0) {
            return table::Format::INT_SEP1;
            //return table::Format::DEFAULT;
        }
        else if (col == 2 && row % 3 == 0) {
            return table::Format::INT_SEP2;
            //return table::Format::DEFAULT;
        }
        else if (col == 3) {
            return table::Format::DEC_3;
        }
        else if (col == 5) {
            if (row % 2 == 0) return table::Format::SECRET_DEF;
            else if (row % 3 == 0) return table::Format::SECRET_STAR;
            else return table::Format::DEFAULT;
        }
        else if (col == 8) {
            return table::Format::DEC_1;
        }
        else if (col == 9) {
            return table::Format::INT_SEP2;
        }
        else if (col == 13 && row % 2 == 0) {
            return table::Format::DATE_WORLD;
        }
        else if (col == 16) {
            return table::Format::DEC_2;
        }
        else {
            return table::Format::DEFAULT;
        }
    }

    //------------------------------------------------------------------------------
    Fl_Color cell_textcolor(int row, int col) override {
        //return FL_DARK_RED;

        if (col == 1) {
            return FL_GREEN;
        }
        else if (col == 6) {
            return FL_RED;
        }
        else if (col == 7) {
            return FL_GREEN;
        }
        else if (col == 8) {
            return FL_DARK_GREEN;
        }
        else if (col == 9) {
            return FL_BLUE;
        }
        else if (col == 16) {
            return FL_DARK_RED;
        }
        else {
            return table::Editor::cell_textcolor(row, col);
        }
    }

    //------------------------------------------------------------------------------
    Fl_Font cell_textfont(int row, int col) override {
        //return FL_COURIER_BOLD_ITALIC;

        if (col == 2 || col == 3) {
            return flw::PREF_FIXED_FONT;
        }
        else if (col == 8 || col == 9 || col == 16) {
            return flw::PREF_FIXED_FONT + FL_BOLD;
        }
        else {
            return table::Display::cell_textfont(row, col);
        }
    }

    //------------------------------------------------------------------------------
    Fl_Fontsize cell_textsize(int row, int col) override {
        if (col == 2 || col == 3 || col == 8 || col == 9 || col == 16) {
            return flw::PREF_FIXED_FONTSIZE;
        }
        else if (col > 50) {
            return flw::PREF_FONTSIZE * 1.5;
        }
        else if (col == 18) {
            return table::Display::cell_textsize(row, col);
        }
        else if (col >= start_free) {
            return 10;
        }
        else {
            return table::Display::cell_textsize(row, col);
        }
    }

    //------------------------------------------------------------------------------
    table::Type cell_type(int row, int col) override {
        if (col ==  1) {
            return table::Type::TEXT;
        }
        else if (col ==  2) {
            return table::Type::INTEGER;
        }
        else if (col ==  3) {
            return table::Type::NUMBER;
        }
        else if (col ==  4) {
            return table::Type::BOOLEAN;
        }
        else if (col ==  5) {
            return table::Type::SECRET;
        }
        else if (col ==  6) {
            return table::Type::CHOICE;
        }
        else if (col ==  7) {
            return table::Type::ICHOICE;
        }
        else if (col ==  8) {
            return table::Type::SLIDER;
        }
        else if (col ==  9) {
            return table::Type::VSLIDER;
        }
        else if (col == 10) {
            return table::Type::COLOR;
        }
        else if (col == 11) {
            return table::Type::FILE;
        }
        else if (col == 12) {
            return table::Type::DIR;
        }
        else if (col == 13) {
            return table::Type::DATE;
        }
        else if (col == 14) {
            return table::Type::LIST;
        }
        else if (col == 15) {
            return table::Type::MTEXT;
        }
        else if (col == 16) {
            return table::Type::VSLIDER;
        }
        else {
            return Editor::cell_type(row, col);
        }
    }

    //------------------------------------------------------------------------------
    std::string cell_value(int row, int col) override {
        return values[row][col];
    }

    //------------------------------------------------------------------------------
    bool cell_value(int row, int col, const std::string& value) override {
        if (col == 8 || col == 9 || col == 16) {
            printf("cell_value(%d): %f\n", col, atof(value.c_str()));
            fflush(stdout);
        }

        if (value.find("hello") != std::string::npos) {
            return false;
        }

        values[row][col] = value;
        return true;
    }

    //------------------------------------------------------------------------------
    int cell_width(int col) override {
        return widths[col];
    }

    //------------------------------------------------------------------------------
    void cell_width(int col, int width) override {
        widths[col] = width;
    }

    //------------------------------------------------------------------------------
    void resize_col() {
        for (int c = 0; c <= COLS; c++) {
            widths[c] = flw::PREF_FONTSIZE * 10;
        }
    }
};

/*
 *      _______        _
 *     |__   __|      | |
 *        | | ___  ___| |_
 *        | |/ _ \/ __| __|
 *        | |  __/\__ \ |_
 *        |_|\___||___/\__|
 *
 *
 */

//----------------------------------------------------------------------------------
class Test : public Fl_Double_Window {
public:
    Fl_Menu_Bar* bar;
    TestTable*   table;

    //------------------------------------------------------------------------------
    Test() : Fl_Double_Window(1200, 800, "test_tableeditor.cpp") {
        end();
        resizable(this);

        bar   = new Fl_Menu_Bar(0, 0, 0, 0);
        table = new TestTable();

        auto pref = Fl_Preferences(Fl_Preferences::USER, "gnuwimp_test", "test_tableeditor");
        flw::theme::load_theme_from_pref(pref);
        flw::theme::load_win_from_pref(pref, "gui.", this, true);

        bar->add("Size/10 rows && 5 columns", 0, Test::CallbackShrink, this);
        bar->add("Size/Clear", 0, Test::CallbackClear, this);
        bar->add("Event/Always", 0, Test::CallbackEvent, this, FL_MENU_TOGGLE);

        bar->add("Select/Select::NO", 0, CallbackSelect, this, FL_MENU_RADIO);
        bar->add("Select/Select::CELL", 0, CallbackSelect, this, FL_MENU_RADIO | FL_MENU_VALUE);
        bar->add("Select/Select::ROW", 0, CallbackSelect, this, FL_MENU_RADIO | FL_MENU_DIVIDER);

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

        bar->add("Debug/Table", 0, CallbackDebug, this);
        bar->add("Theme", 0, Test::CallbackTheme, this);

        add(bar);
        add(table);

        table->header(true, true);
        table->lines(true, true);
        table->select_mode(table::Select::CELL);
        table->resize_column_width(true);
        table->take_focus();
        table->callback(Test::Callback, table);

        flw::menu::setonly_item(bar, "Header/Both");
        flw::menu::setonly_item(bar, "Lines/Both");
        flw::menu::setonly_item(bar, "Resize/Yes");
        flw::menu::setonly_item(bar, "Expand Last Col/No");
        flw::menu::setonly_item(bar, "Scrollbar/Both");
        flw::menu::setonly_item(bar, "Select/Select::CELL");

         //table->header(true, false);
         //table->header(false, false);
         //bar->hide();
         //table->lines(false, false);

        resize(x(), y(), w(), h());
        size_range(64, 48);
        update_pref();
    }

    //--------------------------------------------------------------------------
    ~Test() {
        auto pref = Fl_Preferences(Fl_Preferences::USER, "gnuwimp_test", "test_tableeditor");
        flw::theme::save_theme_to_pref(pref);
        flw::theme::save_win_to_pref(pref, "gui.", this);
    }

    //------------------------------------------------------------------------------
    static void Callback(Fl_Widget*, void* v) {
        auto table = (table::Editor*) v;

        if (table->event() == table::Event::UNDEFINED) {
            printf("Callback: row=%4d, col=%4d  =>  table::Event::UNDEFINED\n", table->event_row(), table->event_col());
        }
        else if (table->event() == table::Event::CHANGED) {
            printf("Callback: row=%4d, col=%4d  =>  table::Event::CHANGED\n", table->event_row(), table->event_col());
        }
        else if (table->event() == table::Event::CURSOR) {
            printf("Callback: row=%4d, col=%4d  =>  table::Event::CURSOR\n", table->event_row(), table->event_col());
        }
        else if (table->event() == table::Event::COLUMN) {
            printf("Callback: row=%4d, col=%4d  =>  table::Event::COLUMN\n", table->event_row(), table->event_col());
        }
        else if (table->event() == table::Event::COLUMN_CTRL) {
            printf("Callback: row=%4d, col=%4d  =>  table::Event::COLUMN_CTRL\n", table->event_row(), table->event_col());
        }
        else if (table->event() == table::Event::ROW) {
            printf("Callback: row=%4d, col=%4d  =>  table::Event::ROW\n", table->event_row(), table->event_col());
        }
        else if (table->event() == table::Event::ROW_CTRL) {
            printf("Callback: row=%4d, col=%4d  =>  table::Event::ROW_CTRL\n", table->event_row(), table->event_col());
        }
        else if (table->event() == table::Event::SIZE) {
            printf("Callback: rows=%4d, columns=%4d => table::Event::SIZE\n", table->rows(), table->columns());
        }
        else if (table->event() == table::Event::APPEND_ROW) {
            printf("Callback: row=   %4d => table::Event::APPEND_ROW\n", table->event_row());
        }
        else if (table->event() == table::Event::APPEND_COLUMN) {
            printf("Callback: column=%4d => table::Event::APPEND_COLUMN\n", table->event_col());
        }
        else if (table->event() == table::Event::INSERT_ROW) {
            printf("Callback: row=   %4d => table::Event::INSERT_ROW\n", table->event_row());
        }
        else if (table->event() == table::Event::INSERT_COLUMN) {
            printf("Callback: column=%4d => table::Event::INSERT_COLUMN\n", table->event_col());
        }
        else if (table->event() == table::Event::DELETE_ROW) {
            printf("Callback: row=   %4d => table::Event::DELETE_ROW\n", table->event_row());
        }
        else if (table->event() == table::Event::DELETE_COLUMN) {
            printf("Callback: column=%4d => table::Event::DELETE_COLUMN\n", table->event_col());
        }

        fflush(stdout);
    }

    //------------------------------------------------------------------------------
    static void CallbackClear(Fl_Widget*, void* v) {
        auto w = (Test*) v;

        w->table->reset();
        flw::menu::setonly_item(w->bar, "Header/No");
        flw::menu::setonly_item(w->bar, "Lines/No");
        flw::menu::setonly_item(w->bar, "Resize/No");
        flw::menu::setonly_item(w->bar, "Expand Last Col/No");
        flw::menu::setonly_item(w->bar, "Scrollbar/Both");
        flw::menu::setonly_item(w->bar, "Select/Select::NO");
    }

    //------------------------------------------------------------------------------
    static void CallbackDebug(Fl_Widget*, void* v) {
        Test* w = (Test*) v;

        w->table->debug();
    }

    //------------------------------------------------------------------------------
    static void CallbackEvent(Fl_Widget*, void* v) {
        auto w    = (Test*) v;
        auto item = (Fl_Menu_Item*) w->bar->find_item("Event/Always");

        w->table->send_changed_event_always(item->value());
    }

    //------------------------------------------------------------------------------
    static void CallbackExpand(Fl_Widget*, void* v) {
        Test* w = (Test*) v;

        if (w->bar->find_item("Expand Last Col/No")->value()) {
            w->table->expand_last_column(false);
        }
        else if (w->bar->find_item("Expand Last Col/Yes")->value()) {
            w->table->expand_last_column(true);
        }

        w->table->redraw();
    }

    //------------------------------------------------------------------------------
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

    //------------------------------------------------------------------------------
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

    //------------------------------------------------------------------------------
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

    //------------------------------------------------------------------------------
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

    //------------------------------------------------------------------------------
    static void CallbackSelect(Fl_Widget*, void* v) {
        Test* w = (Test*) v;

        if (w->bar->find_item("Select/Select::NO")->value()) {
            w->table->select_mode(table::Select::NO);
        }
        else if (w->bar->find_item("Select/Select::CELL")->value()) {
            w->table->select_mode(table::Select::CELL);
        }
        else if (w->bar->find_item("Select/Select::ROW")->value()) {
            w->table->select_mode(table::Select::ROW);
        }

        w->table->redraw();
    }

    //------------------------------------------------------------------------------
    static void CallbackShrink(Fl_Widget*, void* v) {
        auto w = (Test*) v;

        w->table->size(10, 5);
    }

    //--------------------------------------------------------------------------
    static void CallbackTheme(Fl_Widget*, void* v) {
        auto self = static_cast<Test*>(v);
        flw::dlg::theme(true, true);
        self->update_pref();
    }

    //------------------------------------------------------------------------------
    void resize(int X, int Y, int W, int H) {
        Fl_Double_Window::resize(X, Y, W, H);
        bar->resize(0, 0, W, flw::PREF_FONTSIZE * 2);
        table->resize(0, flw::PREF_FONTSIZE * 2, W, H - flw::PREF_FONTSIZE * 2);
         //table->resize(100, 100, W - 200, H - 200);
    }

    //--------------------------------------------------------------------------
    void update_pref() {
        util::labelfont(this);
        bar->textfont(flw::PREF_FONT);
        bar->textsize(flw::PREF_FONTSIZE);
        table->resize_col();
        resize(x(), y(), w(), h());
        Fl::redraw();
    }
};

/*
 *                      _
 *                     (_)
 *      _ __ ___   __ _ _ _ __
 *     | '_ ` _ \ / _` | | '_ \
 *     | | | | | | (_| | | | | |
 *     |_| |_| |_|\__,_|_|_| |_|
 *
 *
 */

//----------------------------------------------------------------------------------
int main(int argc, const char** argv) {
    if (flw::theme::parse(argc, argv) == false) {
        flw::theme::load("oxy");
    }

    Test win;
    win.show();
    return Fl::run();
}
