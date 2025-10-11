// Copyright gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#include "flw.h"

int main() {
    Fl::scheme("oxy");
    Fl::screen_scale(0, 1.0);
    srand(time(0));

    // [flw::table::Table example]

    auto win     = new Fl_Double_Window(Fl::w() / 2 - 300, Fl::h() / 2 - 100, 600, 200, "flw::table::Table");
    auto table   = new flw::table::Table(5, 4, 0, 0, 600, 200);
    auto strings = std::vector<std::string>{"Red", "Yellow", "Green", "Blue", "Cyan", "Magenta"};

    // Set table data.
    for (int row = 1; row <= 5; row++) {
        table->cell_value(row, 0, flw::util::format("%d", row));

        table->cell_edit(row, 1, true);
        table->cell_value(row, 1, strings[rand() % strings.size()]);

        table->cell_type(row, 2, flw::table::Type::CHOICE);
        table->cell_edit(row, 2, true);
        table->cell_value(row, 2, strings[rand() % strings.size()]);
        table->cell_choice(row, 2, "Red\tYellow\tGreen\tBlue\tCyan\tMagenta");

        table->cell_edit(row, 3, true);
        table->cell_type(row, 3, flw::table::Type::INTEGER);
        table->cell_value(row, 3, flw::util::format("%d", rand() % 256));

        table->cell_edit(row, 4, true);
        table->cell_type(row, 4, flw::table::Type::COLOR);
        table->cell_value(row, 4, flw::util::format("%d", rand() % 256));
    }

    // Set new width of row column.
    table->cell_width(0, 50);

    // Set header columns.
    table->cell_value(0, 1, "TEXT");
    table->cell_value(0, 2, "CHOICE");
    table->cell_value(0, 3, "INTEGER");
    table->cell_value(0, 4, "COLOR");

    win->resizable(table);
    win->show();
    Fl::run();

    // [flw::table::Table example]
}
