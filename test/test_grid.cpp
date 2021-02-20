// Copyright 2019 - 2021 gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#include "grid.h"
#include "theme.h"
#include <FL/Fl_Button.H>
#include <FL/Fl_Double_Window.H>

using namespace flw;

#ifdef DEBUG
    namespace flw {
        void grid_test_array();
    }
#endif

static const char* CHOICES = "monster\tdaemon\theller\tmatch\thellion\tfiend\tdickens\tdeuce\tmorning star\tfriction match\tdemon\tdaystar\togre\tdaimon\tthe Tempter";

class Test : public Fl_Double_Window {
    Grid*      grid;
    Fl_Button* button1;
    Fl_Button* button2;

public:
    Test(int W, int H) : Fl_Double_Window(W, H, "test_grid.cpp") {
        button1 = new Fl_Button(0, 0, W / 2, 28, "Set size to 10, 5");
        button2 = new Fl_Button(W / 2, 0, W / 2, 28, "Set size to 10000, 30");
        grid    = new Grid(1000, 15, 0, 28, W, H - 28);

        add(grid);
        add(button1);
        add(button2);
        create(1000, 15);
        grid->labelfont(FL_COURIER);
        button1->callback(Callback1, this);
        button2->callback(Callback2, this);
        resizable(grid);
    }

    static void Callback1(Fl_Widget* w, void* o) {
        (void) w;
        Test* t = (Test*) o;
        t->grid->size(10, 5);
        t->create(10, 5);
    }

    static void Callback2(Fl_Widget* w, void* o) {
        (void) w;
        Test* t = (Test*) o;
        t->grid->size(10000, 30);
        t->create(10000, 30);
    }

    void create(int rows, int cols) {
        for (int r = 0; r <= rows; r++) {
            grid->cell_value2(r, 0, "Row %d", r);
            grid->cell_value2(r, 1, "Text %d", r);
            grid->cell_value2(r, 2, "%d", r + 10000);
            grid->cell_value2(r, 3, "%d.123", r + 1000);
            grid->cell_value(r, 4, "1");
            grid->cell_value(r, 5, "daemon");
            grid->cell_value2(r, 6, "%d 0 200 10", rand() % 200);
            grid->cell_value2(r, 7, "2001-04-%02d", rand() % 20 + 1);
            grid->cell_value2(r, 8, r % 2 == 0 ? "63" : "248");
            grid->cell_value(r, 9, "Makefile");
            grid->cell_value(r, 10, "morning star");
            grid->cell_value2(r, 11, "%d 0 1000 %d", rand() % 1000, rand() % 100 + 1);
            grid->cell_value2(r, 12, "hellion");

            for (int c = 13; c < cols; c++) {
                grid->cell_value2(r, c, "%d,%d", r, c);
            }

            for (int c = 0; c <= 12; c++) {
                if (c == 4) {
                    grid->cell_align(r, c, FL_ALIGN_CENTER);
                }
                else if (c == 6) {
                    grid->cell_align(r, c, FL_ALIGN_RIGHT);
                }

                grid->cell_edit(r, c, true);
            }

            grid->cell_rend(r, 1, TableEditor::REND::TEXT);
            grid->cell_rend(r, 2, TableEditor::REND::INTEGER);
            grid->cell_rend(r, 3, TableEditor::REND::NUMBER);
            grid->cell_rend(r, 4, TableEditor::REND::BOOLEAN);
            grid->cell_rend(r, 5, TableEditor::REND::CHOICE);
            grid->cell_rend(r, 6, TableEditor::REND::VALUE_SLIDER);
            grid->cell_rend(r, 7, TableEditor::REND::DLG_DATE);
            grid->cell_rend(r, 8, TableEditor::REND::DLG_COLOR);
            grid->cell_rend(r, 9, TableEditor::REND::DLG_FILE);
            grid->cell_rend(r, 10, TableEditor::REND::INPUT_CHOICE);
            grid->cell_rend(r, 11, TableEditor::REND::SLIDER);
            grid->cell_rend(r, 12, TableEditor::REND::DLG_LIST);

            grid->cell_format(r, 2, TableEditor::FORMAT::INT_SEP);
            grid->cell_format(r, 3, TableEditor::FORMAT::DEC_2);

            grid->cell_choice(r, 5, CHOICES);
            grid->cell_choice(r, 10, CHOICES);
            grid->cell_choice(r, 12, CHOICES);

            grid->cell_textcolor(r, 6, r % 2 == 0 ? FL_RED : FL_GREEN);
            grid->cell_textcolor(r, 11, FL_RED);
            grid->cell_color(r, 1, FL_YELLOW);
        }

        grid->cell_width(0, 60);
        grid->cell_width(1, 60);
        grid->cell_width(2, 70);
        grid->cell_width(3, 90);
        grid->cell_width(4, 50);
        grid->cell_width(5, 80);
        grid->cell_width(6, 90);
        grid->cell_width(7, 100);
        grid->cell_width(8, 60);
        grid->cell_width(9, 150);
        grid->cell_width(10, 100);
        grid->cell_width(11, 120);
        grid->cell_width(12, 50);

        grid->cell_value(0, 1, "TEXT");
        grid->cell_value(0, 2, "INT");
        grid->cell_value(0, 3, "FLOAT");
        grid->cell_value(0, 4, "BOOL");
        grid->cell_value(0, 5, "CHOICE");
        grid->cell_value(0, 6, "VSLIDER");
        grid->cell_value(0, 7, "DATE");
        grid->cell_value(0, 8, "COLOR");
        grid->cell_value(0, 9, "FILE");
        grid->cell_value(0, 10, "ICHOICE");
        grid->cell_value(0, 11, "SLIDER");
        grid->cell_value(0, 12, "LIST");

        for (int c = 13; c < cols; c++) {
            grid->cell_value(0, c, "?");
        }
    }
};

int main(int argc, const char** argv) {
    if (flw::theme::parse(argc, argv) == false) {
        flw::theme::load("gtk");
    }

    Test win(800, 480);
    win.show();
    return Fl::run();
}
