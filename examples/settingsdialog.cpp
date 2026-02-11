// Copyright gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#include "flw.h"

// [flw::table::SettingsTable example]

// Data for dialog.

struct Data {
    std::string     r1;
    int64_t         r2;
    int             r3;
    gnu::file::File r4;
    std::string     r5;
    std::string     r6;

    Data(const std::string& d1 = "world", int64_t d2 = 0, int d3 = 1, const std::string& d4 = ".", const std::string& d5 = "Hello", const std::string& d6 = "1970-01-01") {
        r1 = d1;
        r2 = d2;
        r3 = d3;
        r4 = gnu::file::File(d4, true);
        r5 = d5;
        r6 = d6;
    }
};

// Implement virtual methods in flw::table::SettingsTable to get and set data.

class SettingsTable : public flw::table::SettingsTable {
    Data& _data;

public:
    SettingsTable(Data& data) : flw::table::SettingsTable(6), _data(data) {
        // Set header labels for all rows.
        cell_value(1, 0, "Write 'world'");
        cell_value(2, 0, "Integer");
        cell_value(3, 0, "Count");
        cell_value(4, 0, "Directory");
        cell_value(5, 0, "Choice");
        cell_value(6, 0, "Date");

        // All rows are editable.
        cell_edit(1, 1, true);
        cell_edit(2, 1, true);
        cell_edit(3, 1, true);
        cell_edit(4, 1, true);
        cell_edit(5, 1, true);
        cell_edit(6, 1, true);

        // Set data types.
        cell_type(1, 1, flw::table::Type::TEXT);
        cell_type(2, 1, flw::table::Type::INTEGER);
        cell_type(3, 1, flw::table::Type::VSLIDER);
        cell_type(4, 1, flw::table::Type::DIR);
        cell_type(5, 1, flw::table::Type::CHOICE);
        cell_type(6, 1, flw::table::Type::DATE);

        // Set array for the choice data type.
        cell_choice(5, 1, "Hello\tWorld\tHow\tAre\tYou?");

        // Display integer witha thousand separator.
        cell_format(2, 1, flw::table::Format::INT_SEP2);

        // Decrease row header width somewhat, although the column width can be resized with mouse.
        cell_width(0, flw::PREF_FONTSIZE * 10);

        set_data();
    }

    // Optional label message.

    void cell_event(int row, int col, flw::table::Event event) override {
        if (event != flw::table::Event::CURSOR || col == 0) {
        }
        else if (row == 1) {
            message("Only 'world' is accepted.");
        }
        else if (row == 4) {
            message("Select a valid directory.");
        }
        else {
            message("");
        }
    }

    // Data are updated when validate_data() has accepted data.

    void get_data() override {
        _data.r1 = cell_value(1, 1);
        _data.r2 = flw::util::to_int(cell_value(2, 1));
        _data.r3 = flw::table::get_slider2(cell_value(3, 1));
        _data.r4 = gnu::file::File(cell_value(4, 1));
        _data.r5 = cell_value(5, 1);
        _data.r6 = cell_value(6, 1);
    }

    // Copy data to table, any changes can be rolled back with "Undo Changes".

    void set_data() override {
        set_data(_data);
    }

    void set_data(Data& data) {
        cell_value(1, 1, data.r1);
        cell_value(2, 1, flw::util::format("%lld", (long long) data.r2));
        cell_value(3, 1, flw::table::format_slider2(data.r3, 1, 10, 1));
        cell_value(4, 1, data.r4.filename());
        cell_value(5, 1, data.r5);
        cell_value(6, 1, data.r6);
    }

    // Reset data to default state in table but don't overwrite input data.

    void set_def_data() override {
        Data data;
        set_data(data);
        message("All settings was reset to default values.");
    }

    // An empty strings must be returned to close the dialog and call get_data().

    std::string validate_data(int& row) override {
        if (cell_value(1, 1) != "world") {
            row = 1;
            return "Error: not 'world'";
        }
        else if (gnu::file::File(cell_value(4, 1)).is_dir() == false) {
            row = 4;
            return "Error: not a directory";
        }
        else {
            return "";
        }
    }
};

// [flw::table::SettingsTable example]

int main() {
    Fl::scheme("oxy");
    Fl::screen_scale(0, 1.0);

    // [flw::SettingsDialog example]

    auto data = Data("hello", 123456, 5, "settingsdialog.cpp", "World", "1996-02-29");
    auto dlg  = new flw::SettingsDialog("flw::SettingsDialog");
    auto page = new SettingsTable(data);

    dlg->add_page("Page 1", page);
    dlg->message("Setup example for flw::SettingsDialog.");

    if (dlg->run() == true) {
        printf("Settings was updated.\n");
    }
    else {
        printf("Settings was NOT updated.\n");
    }

    printf("Row 1 = %s\n", data.r1.c_str());
    printf("Row 2 = %lld\n", (long long) data.r2);
    printf("Row 3 = %d\n", data.r3);
    printf("Row 4 = %s\n", data.r4.c_str());
    printf("Row 5 = %s\n", data.r5.c_str());
    printf("Row 6 = %s\n", data.r6.c_str());

    // [flw::SettingsDialog example]

    return 0;
}
