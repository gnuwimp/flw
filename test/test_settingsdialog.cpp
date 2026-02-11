// Copyright gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#include "flw.h"

#ifndef FLW_AMALGAM
    #include "settingsdialog.h"
    #include "theme.h"
#endif

using namespace flw;

/*
 *      _______    _    __
 *     |__   __|  | |  /_ |
 *        | | __ _| |__ | |
 *        | |/ _` | '_ \| |
 *        | | (_| | |_) | |
 *        |_|\__,_|_.__/|_|
 *
 *
 */

struct Data1 {
    std::string r1 = "Hello";
    std::string r2 = "RO";
    std::string r3 = "5 1 10 1";
    std::string r4 = "5000000 1000000 10000000 1000000";
    std::string r5 = "123456";
    std::string r6 = "345.123456";
};

class Tab1 : public table::SettingsTable {
public:
    Data1 data;

    //--------------------------------------------------------------------------
    Tab1() : table::SettingsTable(6) {
        set_data();
    }

    //--------------------------------------------------------------------------
    ~Tab1() {
        FLW_BLUE
        FLW_PRINTV(data.r1)
        FLW_PRINTV(data.r2)
        FLW_PRINTV(data.r3)
        FLW_PRINTV(data.r4)
        FLW_PRINTV(data.r5)
        FLW_PRINTV(data.r6)
    }

    //--------------------------------------------------------------------------
    void cell_event(int row, int col, table::Event event) override {
        auto m = util::format("Row: %02d\nColumn: %02d\nEvent: %02d", row, col, (int) event);
        message(col == -1 ? "" : m);
    }

    //--------------------------------------------------------------------------
    void get_data() override {
        data.r1 = cell_value(1, 1);
        data.r2 = cell_value(2, 1);
        data.r3 = cell_value(3, 1);
        data.r4 = cell_value(4, 1);
        data.r5 = cell_value(5, 1);
        data.r6 = cell_value(6, 1);
    }

    //--------------------------------------------------------------------------
    void set_data() override {
        cell_value(1, 0, "Write something");
        cell_value(2, 0, "Read only");
        cell_value(3, 0, "Slider small");
        cell_value(4, 0, "Slider large");
        cell_value(5, 0, "Int");
        cell_value(6, 0, "Double");

        cell_value(1, 1, data.r1);
        cell_value(2, 1, data.r2);
        cell_value(3, 1, data.r3);
        cell_value(4, 1, data.r4);
        cell_value(5, 1, data.r5);
        cell_value(6, 1, data.r6);

        cell_edit(1, 1, true);
        cell_edit(3, 1, true);
        cell_edit(4, 1, true);
        cell_edit(5, 1, true);
        cell_edit(6, 1, true);

        cell_format(5, 1, table::Format::INT_SEP2);
        cell_format(6, 1, table::Format::DEC_2);

        cell_type(3, 1, table::Type::VSLIDER);
        cell_type(4, 1, table::Type::VSLIDER);
        cell_type(5, 1, table::Type::INTEGER);
        cell_type(6, 1, table::Type::NUMBER);
    }

    //--------------------------------------------------------------------------
    void set_def_data() override {
        cell_value(1, 1, "");
        cell_value(2, 1, "RO");
        cell_value(3, 1, "1 1 10 1");
        cell_value(4, 1, "1000000 1000000 10000000 1000000");
        cell_value(5, 1, "0");
        cell_value(6, 1, "0");
    }

    //--------------------------------------------------------------------------
    std::string validate_data(int&) override {
        return "";
    }
};

/*
 *      _______    _    ___
 *     |__   __|  | |  |__ \
 *        | | __ _| |__   ) |
 *        | |/ _` | '_ \ / /
 *        | | (_| | |_) / /_
 *        |_|\__,_|_.__/____|
 *
 *
 */

struct Data2 {
    std::string r1 = "obj";
    std::string r2 = "12231255";
    std::string r3 = "2025-01-20";
    std::string r4 = "Baphomet";
    std::string r5 = "1";
};

class Tab2 : public table::SettingsTable {
public:
    Data2 data;

    //--------------------------------------------------------------------------
    Tab2() : table::SettingsTable(5) {
        set_data();
        cell_choice(4, 1, "Balaam\tBaphomet\tBeelzebub\tBehemoth");
    }

    //--------------------------------------------------------------------------
    ~Tab2() {
        FLW_BLUE
        FLW_PRINTV(data.r1)
        FLW_PRINTV(data.r2)
        FLW_PRINTV(data.r3)
        FLW_PRINTV(data.r4)
        FLW_PRINTV(data.r5)
    }

    //--------------------------------------------------------------------------
    void cell_event(int row, int col, table::Event event) override {
        auto m = util::format("Event: %02d - %02d - %d\n", row, col, (int) event);
        message(col == -1 ? "" : m);
    }

    //--------------------------------------------------------------------------
    void get_data() override {
        data.r1 = cell_value(1, 1);
        data.r2 = cell_value(2, 1);
        data.r3 = cell_value(3, 1);
        data.r4 = cell_value(4, 1);
        data.r5 = cell_value(5, 1);
    }

    //--------------------------------------------------------------------------
    void set_data() override {
        cell_value(1, 0, "Directory");
        cell_value(2, 0, "Color");
        cell_value(3, 0, "Date");
        cell_value(4, 0, "Choice");
        cell_value(5, 0, "Yes/No");

        cell_value(1, 1, data.r1);
        cell_value(2, 1, data.r2);
        cell_value(3, 1, data.r3);
        cell_value(4, 1, data.r4);
        cell_value(5, 1, data.r5);

        cell_type(1, 1, table::Type::DIR);
        cell_type(2, 1, table::Type::COLOR);
        cell_type(3, 1, table::Type::DATE);
        cell_type(4, 1, table::Type::CHOICE);
        cell_type(5, 1, table::Type::BOOLEAN);

        cell_edit(1, 1, true);
        cell_edit(2, 1, true);
        cell_edit(3, 1, true);
        cell_edit(4, 1, true);
        cell_edit(5, 1, true);
    }

    //--------------------------------------------------------------------------
    void set_def_data() override {
        cell_value(1, 1, "src");
        cell_value(2, 1, "0");
        cell_value(3, 1, "1970-01-01");
        cell_value(4, 1, "Balaam");
    }

    //--------------------------------------------------------------------------
    std::string validate_data(int& row) override {
        if (cell_value(3, 1) == "2025-01-20") {
            row = 3;
            return "Error: illegal date!";
        }

        return "";
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

//------------------------------------------------------------------------------
int main(int argc, const char** argv) {
    if (flw::theme::parse(argc, argv) == false) {
        flw::theme::load("oxy");
    }

    //auto pref = Fl_Preferences(Fl_Preferences::USER, "gnuwimp_test", "test_settingsdialog");
    //flw::theme::load_theme_from_pref(pref);
    //flw::dlg::theme(true, true);
    //flw::theme::save_theme_to_pref(pref);

    auto dlg  = SettingsDialog("test_settingsdialog.cpp");
    auto tab1 = new Tab1();
    auto tab2 = new Tab2();

    dlg.add_page("Tab 2", tab2, tab1);
    dlg.insert_page("Tab 1", tab1, tab2);
    dlg.message("Setup for test_settingsdialog.cpp");

    if (dlg.run() == true) {
        FLW_GREEN
    }
    else {
        FLW_RED
    }

    return 0;
}
