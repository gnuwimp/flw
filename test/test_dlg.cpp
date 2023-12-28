// Copyright 2016 - 2021 gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#include "datechooser.h"
#include "fontdialog.h"
#include "dlg.h"
#include "theme.h"
#include "waitcursor.h"
#include "test.h"

using namespace flw;

int main(int argc, const char** argv) {
    theme::parse(argc, argv);

    puts("run individual test with");
    puts("    theme");
    puts("    font");
    puts("    abort");
    puts("    wait");
    puts("    html");
    puts("    list");
    puts("    select");
    puts("    date");
    puts("    pwd");
    puts("    work");
    puts("    work");
    puts("    text");
    fflush(stdout);

    for (int f = 0; f < argc; f++) {
        std::string run = "";

         if (argc > 1) {
            if (f == 0) {
                continue;
            }
            else {
                run = argv[f];
            }
         }

        if (run == "loadsmall") {
            #ifdef _WIN32
                auto font = flw::dlg::FontDialog::LoadFont("MS Serif bold");
            #else
                auto font = flw::dlg::FontDialog::LoadFont("Kinnari bold italic");
            #endif

            if (font != -1) {
                flw::PREF_FONT     = font;
                flw::PREF_FONTSIZE = 12;
            }

            #ifdef _WIN32
                font = flw::dlg::FontDialog::LoadFont("Fira Code bold");
            #else
                font = flw::dlg::FontDialog::LoadFont("mono bold italic");
            #endif

            if (font != -1) {
                flw::PREF_FIXED_FONT     = font;
                flw::PREF_FIXED_FONTSIZE = 12;
            }

        }

        if (run == "loadbig") {
            #ifdef _WIN32
                auto font = flw::dlg::FontDialog::LoadFont("Calibri italic");
            #else
                auto font = flw::dlg::FontDialog::LoadFont("Kinnari bold italic");
            #endif

            if (font != -1) {
                flw::PREF_FONT     = font;
                flw::PREF_FONTSIZE = 20;
            }

            #ifdef _WIN32
                font = flw::dlg::FontDialog::LoadFont("Lucida Console");
            #else
                font = flw::dlg::FontDialog::LoadFont("mono bold italic");
            #endif

            if (font != -1) {
                flw::PREF_FIXED_FONT     = font;
                flw::PREF_FIXED_FONTSIZE = 20;
            }

        }

        if (run == "" || run == "theme") {
            dlg::theme(true, true);
        }

        if (run == "" || run == "font") {
            auto dlg = flw::dlg::FontDialog("Roboto", 14, "Test Font");
            dlg.run();
            printf("font=%s, %d\n", dlg.fontname().c_str(),  dlg.fontsize());
        }

        if (run == "" || run == "abort") {
            auto dialog = flw::dlg::AbortDialog();

            dialog.show("This will never stop until you press the button\nHello World");

            while (dialog.abort() == false) {
            }
        }

        if (run == "" || run == "abort") {
            auto dialog = flw::dlg::AbortDialog(-100.0, 100.0);
            auto count  = -100.0;

            dialog.show("This will take some time");

            while (dialog.abort() == false && count <= 100.0) {
                dialog.value(count);
                flw::util::time_sleep(100);
                count += 1.0;
            }
        }

        if (run == "" || run == "wait") {
            auto w = Fl_Double_Window(100, 100, 480, 240, "WaitCursor - Press Escape to quit");
            w.show();
            flw::util::center_window(&w);
            WaitCursor wc1;
            WaitCursor wc2;
            WaitCursor wc3;
            Fl::run();
        }

        if (run == "" || run == "html") { // flw::dlg::html()
            auto html = "<h1>Hello World</h1><ul><li>Hello<li>World</ul>This is some text<br>And more text";
            dlg::html("HTML Dialog", html);
        }

        if (run == "" || run == "list") {
            dlg::list("List View", HAMLET_TEXT);
            dlg::list("List View", HAMLET_LIST, nullptr, true); // !!! does not show all rows with fixed font
        }

        if (run == "" || run == "select") {
            std::vector<std::string> v;
            char c[100];

            for (int f = 1; f <= 150; f++) {
                snprintf(c, 100, "Hello World %05d", f);
                v.push_back(std::string(c));
            }

            printf("line %d was selected\n", (int) dlg::select("flw::dlg::select", v, 66));
            printf("line %d was selected\n", (int) dlg::select("flw::dlg::select", v, "Hello World 00044"));
            fflush(stdout);
        }

        if (run == "" || run == "date") { // flw::dlg::date()
            Date date;

            if (flw::dlg::date("Select Date", date, nullptr)) {
                printf("flw::dlg::date=%s\n", date.format().c_str());
            }
            else {
                printf("No date was selected\n");
            }

            fflush(stdout);
        }

        if (run == "" || run == "pwd")  {
            std::string password, file;
            bool ret;

            ret = flw::dlg::password1("test_dlg.cpp - flw::dlg::password1", password);
            printf("flw::dlg::password1=%d, '%s'\n", ret, password.c_str());
            util::zero_memory((char*) password.data());

            ret = flw::dlg::password2("test_dlg.cpp - flw::dlg::password2", password);
            printf("flw::dlg::password2=%d, '%s'\n", ret, password.c_str());
            util::zero_memory((char*) password.data());

            // flw::dlg::PASSWORD_CANCEL = "cancel";
            // flw::dlg::PASSWORD_OK = "ok";

            ret = flw::dlg::password3("test_dlg.cpp - flw::dlg::password3", password, file);
            printf("flw::dlg::password3=%d, '%s', '%s'\n", ret, password.c_str(), file.c_str());
            util::zero_memory((char*) password.data());
            util::zero_memory((char*) file.data());

            ret = flw::dlg::password4("test_dlg.cpp - flw::dlg::password4", password, file);
            printf("flw::dlg::password4=%d, '%s', '%s'\n", ret, password.c_str(), file.c_str());
            util::zero_memory((char*) password.data());
            util::zero_memory((char*) file.data());

            fflush(stdout);
        }

        if (run == "" || run == "work")  { // flw::dlg::work()
            auto work  = flw::dlg::WorkDialog("WorkDialog", nullptr, true, true);
            auto start = util::time();

            while (true) {
                auto s = util::format("@bRunning for = %.1f sec", util::time() - start);

                if (work.run(0.1, s) == false) {
                    break;
                }

                util::time_sleep(20);
            }
        }

        if (run == "" || run == "work") { // flw::dlg::work()
            auto work = flw::dlg::WorkDialog("WorkDialog", nullptr, false, false, 60, 10);
            auto stop = util::time() + 5.0;

            while (true) {
                auto m = std::vector<std::string>();

                m.push_back(util::format("@bHello World"));
                m.push_back(util::format("Quitting in %.1f sec", stop - util::time()));
                m.push_back(util::format("@B95Quitting in %.1f sec", stop - util::time()));

                if (work.run(0.1, m) == false) {
                    break;
                }
                else if (util::time() - stop > 0.0) {
                    break;
                }

                util::time_sleep(20);
            }
        }

        if (run == "" || run == "text") { // flw::dlg::text()
            flw::dlg::text("test_dlg.cpp - flw::dlg::text", HAMLET_TEXT);
            flw::dlg::text("test_dlg.cpp - flw::dlg::text", HAMLET_TEXT, nullptr, true, 70, 50);
        }
    }

    printf("\nTests Finished\n");
    return 0;
}
