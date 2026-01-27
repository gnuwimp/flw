// Copyright gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#include "flw.h"

#ifndef FLW_AMALGAM
    #include "table.h"
    #include "theme.h"
#endif

#include <FL/Fl.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Double_Window.H>

using namespace flw;

static const std::string CHOICES = "monster\tdaemon\theller\tmatch\thellion\tfiend\tdickens\tdeuce\tmorning star\tfriction match\tdemon\tdaystar\togre\tdaimon\tthe Tempter";
static const int         ROWS1   =  1'000;
static const int         COLS1   =     15;
static const int         ROWS2   = 10'000;
static const int         COLS2   =     30;
static const int         ROWS3   =     10;
static const int         COLS3   =      5;

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
    table::Table*   table;
    Fl_Button*      button1;
    Fl_Button*      button2;

public:
    //------------------------------------------------------------------------------
    Test(int W, int H) : Fl_Double_Window(W, H, "test_table.cpp") {
        button1 = new Fl_Button(0, 0, W / 2, 28, "Set size to 10, 5");
        button2 = new Fl_Button(W / 2, 0, W / 2, 28, "Set size to 10000, 30");
        table   = new table::Table(ROWS1, COLS1, 0, 28, W, H - 28);

        add(table);
        add(button1);
        add(button2);
        create(1000, 15);
        table->labelfont(FL_COURIER);
        button1->callback(Callback1, this);
        button2->callback(Callback2, this);
        resizable(table);
        size_range(64, 48);
    }

    //------------------------------------------------------------------------------
    static void Callback1(Fl_Widget* w, void* o) {
        (void) w;
        Test* t = (Test*) o;
        t->table->size(ROWS3, COLS3);
        t->create(ROWS3, COLS3);
    }

    //------------------------------------------------------------------------------
    static void Callback2(Fl_Widget* w, void* o) {
        (void) w;
        Test* t = (Test*) o;
        t->table->size(ROWS2, COLS2);
        t->create(ROWS2, COLS2);
    }

    //------------------------------------------------------------------------------
    void create(int rows, int cols) {
        for (int r = 0; r <= rows; r++) {
            table->cell_value(r, 0, util::format("Row %d", r));
            table->cell_value(r, 1, util::format("Text %d", r));
            table->cell_value(r, 2, util::format("%d", r + 10000));
            table->cell_value(r, 3, util::format("%d.123", r + 1000));
            table->cell_value(r, 4, rand() % 3 == 1 ? "0" : "1");
            table->cell_value(r, 5, "daemon");
            table->cell_value(r, 6, util::format("%d 0 200 10", rand() % 200));
            table->cell_value(r, 7, util::format("20%02d-%02d-%02d", rand() % 25 + 1, rand() % 11 + 1, rand() % 20 + 1));
            table->cell_value(r, 8, util::format("%d", rand() % 256));
            table->cell_value(r, 9, "Makefile");
            table->cell_value(r, 10, "morning star");
            table->cell_value(r, 11, util::format("%d 0 1000 %d", rand() % 1000, rand() % 100 + 1));
            table->cell_value(r, 12, "hellion");

            for (int c = 13; c <= cols; c++) {
                table->cell_value(r, c, util::format("%d,%d", r, c));
            }

            for (int c = 0; c <= cols; c++) {
                if (c == 4) {
                    table->cell_align(r, c, FL_ALIGN_CENTER);
                }
                else if (c == 6) {
                    table->cell_align(r, c, FL_ALIGN_RIGHT);
                }

                if (c < 20) {
                    table->cell_edit(r, c, true);
                }
            }

            table->cell_type(r, 1, table::Type::TEXT);
            table->cell_type(r, 2, table::Type::INTEGER);
            table->cell_type(r, 3, table::Type::NUMBER);
            table->cell_type(r, 4, table::Type::BOOLEAN);
            table->cell_type(r, 5, table::Type::CHOICE);
            table->cell_type(r, 6, table::Type::VSLIDER);
            table->cell_type(r, 7, table::Type::DATE);
            table->cell_type(r, 8, table::Type::COLOR);
            table->cell_type(r, 9, table::Type::FILE);
            table->cell_type(r, 10, table::Type::ICHOICE);
            table->cell_type(r, 11, table::Type::SLIDER);
            table->cell_type(r, 12, table::Type::LIST);

            table->cell_format(r, 2, table::Format::INT_SEP);
            table->cell_format(r, 3, table::Format::DEC_2);

            table->cell_choice(r, 5, CHOICES);
            table->cell_choice(r, 10, CHOICES);
            table->cell_choice(r, 12, CHOICES);

            table->cell_textcolor(r, 6, r % 2 == 0 ? FL_RED : FL_GREEN);
            table->cell_textcolor(r, 11, FL_RED);
            table->cell_color(r, 1, FL_YELLOW);
        }

        table->cell_width(0, 90);
        table->cell_width(1, 70);
        table->cell_width(2, 80);
        table->cell_width(3, 90);
        table->cell_width(4, 50);
        table->cell_width(5, 90);
        table->cell_width(6, 90);
        table->cell_width(7, 100);
        table->cell_width(8, 60);
        table->cell_width(9, 150);
        table->cell_width(10, 120);
        table->cell_width(11, 120);
        table->cell_width(12, 120);

        table->cell_value(0, 1, "TEXT");
        table->cell_value(0, 2, "INT");
        table->cell_value(0, 3, "FLOAT");
        table->cell_value(0, 4, "BOOL");
        table->cell_value(0, 5, "CHOICE");
        table->cell_value(0, 6, "VSLIDER");
        table->cell_value(0, 7, "DATE");
        table->cell_value(0, 8, "COLOR");
        table->cell_value(0, 9, "FILE");
        table->cell_value(0, 10, "ICHOICE");
        table->cell_value(0, 11, "SLIDER");
        table->cell_value(0, 12, "LIST");

        for (int c = 13; c <= cols; c++) {
            if (c < 20) {
                table->cell_value(0, c, "?");
            }
            else {
                table->cell_value(0, c, "R/O");
            }
        }
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

    Test win(1200, 800);
    win.show();
    return Fl::run();
}
