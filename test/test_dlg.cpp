// Copyright gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#include "flw.h"

#ifndef FLW_AMALGAM
    #include "datechooser.h"
    #include "dlg.h"
    #include "file.h"
    #include "waitcursor.h"
#endif

#include "test.h"
#include <FL/Fl.H>
#include <FL/fl_ask.H>

using namespace flw;

//------------------------------------------------------------------------------
void test_abort() {
    {
        auto dialog = dlg::AbortDialog("dlg::AbortDialog");

        dialog.show("This will never stop until you press the button\nHello World");

        while (dialog.check() == false) {
        }
    }

    {
        auto dialog = dlg::AbortDialog("", -100.0, 100.0);
        auto count  = -100.0;

        dialog.show("This will take some time");

        while (dialog.check() == false && count <= 100.0) {
            dialog.value(count);
            util::sleep(100);
            count += 1.0;
        }
    }
}

//------------------------------------------------------------------------------
void test_check() {
    StringVector v = { "1Whether", "0tis nobler", "1in the", "0mind to suffer", "1The slings", 
                       "0and arrows", "1of outrageous", "0fortune", "1Or to", "0take arms", 
                       "1against", "0a sea", "1of troubles", "0And by", "1opposing", "0end them?", 
                       "1To die", "0to sleep", "1No more", "0and by a", "1sleep to say", "0we end" };
    size_t       c = 0;
    
    for (auto s : dlg::check("dlg::check", v)) {
        printf("%2u = %s => %s\n", (unsigned) c, s.c_str(), v[c].c_str());
        c++;
    }
    fflush(stdout);
}

//------------------------------------------------------------------------------
void test_choice() {
    StringVector v = {"Hello", "World", "How", "Are", "You?"};

    printf("line %d was selected\n", dlg::choice("dlg::choice", v, 1));
    fflush(stdout);
}

//------------------------------------------------------------------------------
void test_date() {
    gnu::Date date;

    if (dlg::date("dlg::date", date, nullptr)) {
        printf("dlg::date=%s\n", date.format().c_str());
    }
    else {
        printf("No date was selected\n");
    }

    fflush(stdout);
}

//------------------------------------------------------------------------------
void test_font() {
#ifdef _WIN32
    auto dlg = dlg::FontDialog("Candara", 15, "dlg::FontDialog");
#else
    auto dlg = dlg::FontDialog("Nimbus Sans", 15, "dlg::FontDialog");
#endif
//            dlg.deactivate_font();
//            dlg.deactivate_fontsize();

    if (dlg.run() == true) {
        printf("font set=%s, %d\n", dlg.fontname().c_str(),  dlg.fontsize());
        flw::PREF_FONT           = dlg.font();
        flw::PREF_FONTSIZE       = dlg.fontsize();
        flw::PREF_FIXED_FONTSIZE = dlg.fontsize();
    }
    else {
        printf("aborted font dialog=%s, %d\n", dlg.fontname().c_str(),  dlg.fontsize());
    }
}

//------------------------------------------------------------------------------
void test_font2() {
    Fl_Font     font     = FL_COURIER;
    Fl_Fontsize fontsize = 16;
    std::string fontname;

    if (dlg::font(font, fontsize, fontname, false) == true) {
        printf("selected: font=%d, fontsize=%d, fontname=%s\n", font, fontsize, fontname.c_str());
    }
    else {
        printf("aborted font dialog\n");
    }
}

//------------------------------------------------------------------------------
void test_font_big() {
#ifdef _WIN32
    auto font = theme::load_font("Calibri italic");
#else
    auto font = theme::load_font("Kinnari bold italic");
#endif

    if (font != -1) {
        flw::PREF_FONT     = font;
        flw::PREF_FONTSIZE = 20;
    }

#ifdef _WIN32
    font = theme::load_font("Lucida Console");
#else
    font = theme::load_font("mono bold italic");
#endif

    if (font != -1) {
        flw::PREF_FIXED_FONT     = font;
        flw::PREF_FIXED_FONTSIZE = 20;
    }
}

//------------------------------------------------------------------------------
void test_font_small() {
#ifdef _WIN32
    auto font = theme::load_font("MS Serif bold");
#else
    auto font = theme::load_font("Kinnari bold italic");
#endif

    if (font != -1) {
        flw::PREF_FONT     = font;
        flw::PREF_FONTSIZE = 12;
    }

#ifdef _WIN32
    font = theme::load_font("Fira Code bold");
#else
    font = theme::load_font("mono bold italic");
#endif

    if (font != -1) {
        flw::PREF_FIXED_FONT     = font;
        flw::PREF_FIXED_FONTSIZE = 12;
    }
}

//------------------------------------------------------------------------------
void test_html() {
    auto html = "<h1>Hello World</h1><ul><li>Hello<li>World</ul>This is some text<br>And more text";
    dlg::html("dlg::html", html);
}

//------------------------------------------------------------------------------
void test_list() {
    dlg::list("dlg::list", HAMLET_LIST);
    dlg::list("dlg::list - Fixed Font", HAMLET_LIST, nullptr, true);
    dlg::list_file("dlg::list_file - File", "test/browser.txt", nullptr, true);
}

//------------------------------------------------------------------------------
void test_print() {
    auto r = 0;

    while (r >= 0) {
    #ifdef _WIN32
        dlg::center_message_dialog();
        r = fl_choice_n("%s", "Print Long Lines", "Print Hamlet", "Print UTF8", "Print some test texts.\nSome fonts might have issues.");

        if (r == 0) dlg::print_text("dlg::print_text", LONG_TEXT);
        else if (r == 1) dlg::print_text("dlg::print_text", HAMLET_TEXT);
        else if (r == 2) dlg::print_text("dlg::print_text", UTF8_TEXT);
    #else
        std::string ps = "output.ps";
        auto p = false;

        dlg::center_message_dialog();
        r = fl_choice_n("Print to the default %s file.\nIt will be opened with system default program.\nThere are font issues if you use any other than the normal fltk fonts.\nAnd lot of the utf8 characters does not print well.", "Print Long Lines", "Print Hamlet", "Print UTF8", ps.c_str());

        if (r == 0) p = dlg::print_text("dlg::print_text", LONG_TEXT);
        else if (r == 1) p = dlg::print_text("dlg::print_text", HAMLET_TEXT);
        else if (r == 2) p = dlg::print_text("dlg::print_text", UTF8_TEXT);

        if (p == true && r >= 0 && gnu::File(ps).size > 0) {
            auto f = system((std::string("open ") + ps).c_str());
            (void) f;
        }
    #endif
    }
}

//------------------------------------------------------------------------------
void test_print1() {
    {
        auto printer = PrintText("..", Fl_Paged_Device::Page_Format::A4, Fl_Paged_Device::Page_Layout::PORTRAIT, FL_HELVETICA_ITALIC, 14, FL_ALIGN_LEFT, false, false, 0);
        FLW_PRINT(printer.print(HAMLET_TEXT))
    }

    {
        auto printer = PrintText("print_hamlet.ps", Fl_Paged_Device::Page_Format::A4, Fl_Paged_Device::Page_Layout::PORTRAIT, FL_HELVETICA_ITALIC, 14, FL_ALIGN_LEFT, false, false, 0);
        printer.print(HAMLET_TEXT);
    }

    {
        auto printer = PrintText("print_hamlet_center.ps", Fl_Paged_Device::Page_Format::A4, Fl_Paged_Device::Page_Layout::PORTRAIT, FL_HELVETICA_ITALIC, 14, FL_ALIGN_CENTER, false, false, 0);
        printer.print(HAMLET_TEXT);
    }

    {
        auto printer = PrintText("print_hamlet_right.ps", Fl_Paged_Device::Page_Format::A4, Fl_Paged_Device::Page_Layout::PORTRAIT, FL_HELVETICA_ITALIC, 14, FL_ALIGN_RIGHT, false, true, 0);
        printer.print(HAMLET_TEXT);
    }

    {
        auto printer = PrintText("print_long.ps", Fl_Paged_Device::Page_Format::A4, Fl_Paged_Device::Page_Layout::PORTRAIT, FL_COURIER, 14, FL_ALIGN_LEFT, false, true, 0);
        printer.print(LONG_TEXT);
    }

    {
        auto printer = PrintText("print_long_linenum.ps", Fl_Paged_Device::Page_Format::A4, Fl_Paged_Device::Page_Layout::PORTRAIT, FL_COURIER, 14, FL_ALIGN_LEFT, false, true, 4);
        printer.print(LONG_TEXT);
    }

    {
        auto printer = PrintText("print_long_wrapped.ps", Fl_Paged_Device::Page_Format::A4, Fl_Paged_Device::Page_Layout::PORTRAIT, FL_COURIER, 14, FL_ALIGN_LEFT, true, false, 0);
        printer.print(LONG_TEXT);
    }

    {
        auto printer = PrintText("print_long_wrapped_border.ps", Fl_Paged_Device::Page_Format::A4, Fl_Paged_Device::Page_Layout::PORTRAIT, FL_COURIER, 14, FL_ALIGN_LEFT, true, true, 0);
        printer.print(LONG_TEXT);
    }

    {
        auto printer = PrintText("print_long_wrapped_a3.ps", Fl_Paged_Device::Page_Format::A3, Fl_Paged_Device::Page_Layout::REVERSED, FL_HELVETICA, 16, FL_ALIGN_LEFT, true, true, 0);
        printer.print(LONG_TEXT);
    }

    {
        auto printer = PrintText("print_long_wrapped_a5.ps", Fl_Paged_Device::Page_Format::A5, Fl_Paged_Device::Page_Layout::LANDSCAPE, FL_TIMES_BOLD_ITALIC, 18, FL_ALIGN_LEFT, true, true, 0);
        printer.print(LONG_TEXT);
    }

    {
        auto printer = PrintText("print_long_wrapped_border_linenum.ps", Fl_Paged_Device::Page_Format::A4, Fl_Paged_Device::Page_Layout::PORTRAIT, FL_COURIER, 14, FL_ALIGN_LEFT, true, true, 6);
        printer.print(LONG_TEXT);
    }

    {
        auto printer = PrintText("print_long_wrapped_border_center.ps", Fl_Paged_Device::Page_Format::A4, Fl_Paged_Device::Page_Layout::PORTRAIT, FL_COURIER, 14, FL_ALIGN_CENTER, true, true, 6);
        printer.print(LONG_TEXT, 8);
    }

    {
        auto printer = PrintText("print_long_wrapped_border_right.ps", Fl_Paged_Device::Page_Format::A4, Fl_Paged_Device::Page_Layout::PORTRAIT, FL_COURIER, 16, FL_ALIGN_RIGHT, true, true, 6);
        printer.print(LONG_TEXT, 8);
    }

    {
        auto printer = PrintText("print_long_wrapped_border_linenum_24.ps", Fl_Paged_Device::Page_Format::A4, Fl_Paged_Device::Page_Layout::PORTRAIT, FL_COURIER, 24, FL_ALIGN_LEFT, true, true, 6);
        printer.print(LONG_TEXT);
    }

    {
        auto printer = PrintText("print_long_wrapped_border_linenum_36.ps", Fl_Paged_Device::Page_Format::A4, Fl_Paged_Device::Page_Layout::PORTRAIT, FL_COURIER, 36, FL_ALIGN_LEFT, true, true, 6);
        printer.print(LONG_TEXT);
    }

    {
        auto printer = PrintText("print_utf8.ps", Fl_Paged_Device::Page_Format::A4, Fl_Paged_Device::Page_Layout::PORTRAIT, FL_COURIER, 14, false, false, 0);
        printer.print(UTF8_TEXT);
    }
}

//------------------------------------------------------------------------------
void test_print2() {
    auto r  = 0;
    auto b1 = gnu::File::Read("flw.h");
    auto b2 = gnu::File::Read("flw.cpp");

    while (r >= 0 && b1.p && b2.p) {
    #ifdef _WIN32
        dlg::center_message_dialog();
        r = fl_choice_n("%s", "flw.h", "flw.cpp", nullptr, "Print source code.");

        if (r == 0) dlg::print_text("dlg::print_text", b1.p);
        else if (r == 1) dlg::print_text("dlg::print_text", b2.p);
    #else
        std::string ps = "output.ps";
        auto p = false;

        dlg::center_message_dialog();
        r = fl_choice_n("%s", "flw.h", "flw.cpp", nullptr, "Print source code.");

        if (r == 0) p = dlg::print_text("dlg::print_text", b1.p);
        else if (r == 1) p = dlg::print_text("dlg::print_text", b2.p);

        if (p == true && r >= 0 && gnu::File(ps).size > 0) {
            auto f = system((std::string("open ") + ps).c_str());
            (void) f;
        }
    #endif
    }
}

//------------------------------------------------------------------------------
bool test_print3a(void*, int pw, int ph, int page) {
    char b[100];
    snprintf(b, 100, "%d", page);

    fl_font(FL_COURIER, 36);
    fl_color(FL_BLACK);
    fl_line_style(FL_SOLID, 1);
    fl_rect(0, 0, pw, ph);
    fl_draw(b, 0, 0, pw, ph, FL_ALIGN_INSIDE | FL_ALIGN_CENTER, nullptr, 0);

    return true;
}

//------------------------------------------------------------------------------
bool test_print3b(void* data, int pw, int ph, int) {
    auto c = static_cast<int*>(data);
    char b[100];
    snprintf(b, 100, "%d", *c);

    fl_font(FL_COURIER, 36);
    fl_color(FL_BLACK);
    fl_line_style(FL_SOLID, 1);
    fl_rect(0, 0, pw, ph);
    fl_draw(b, 0, 0, pw, ph, FL_ALIGN_INSIDE | FL_ALIGN_CENTER, nullptr, 0);
    (*c)--;
    return *c > 0;
}

//------------------------------------------------------------------------------
bool test_print3c(void*, int pw, int ph, int page) {
    char b[100];
    snprintf(b, 100, "%d", page);

    fl_font(FL_COURIER, 36);
    fl_color(FL_BLACK);
    fl_line_style(FL_SOLID, 1);
    fl_rect(0, 0, pw, ph);
    fl_draw(b, 0, 0, pw, ph, FL_ALIGN_INSIDE | FL_ALIGN_CENTER, nullptr, 0);

    return false;
}

//------------------------------------------------------------------------------
bool test_print3d(void*, int pw, int ph, int page) {
    char b[100];
    snprintf(b, 100, "%d", page);

    fl_font(FL_COURIER, 36);
    fl_color(FL_BLACK);
    fl_line_style(FL_SOLID, 5);
    fl_rect(0, 0, pw, ph);
    fl_draw(b, 0, 0, pw, ph, FL_ALIGN_INSIDE | FL_ALIGN_CENTER, nullptr, 0);

    return page != 6;
}

//------------------------------------------------------------------------------
void test_print3() {
    int b = 33;
    dlg::print("dlg::print - Pages Between From And To", test_print3a, nullptr, 5, 10);
    dlg::print("dlg::print - Many Pages", test_print3b, &b);
    dlg::print("dlg::print - One Page", test_print3c);
    dlg::print("dlg::print - Stop On Page 6", test_print3d, nullptr, 1, 11);
}

//------------------------------------------------------------------------------
void test_pwd() {
    std::string password, file;
    bool ret;

    ret = dlg::password1("dlg::password1", password);
    printf("dlg::password1=%d, '%s'\n", ret, password.c_str());

    ret = dlg::password2("dlg::password2", password);
    printf("dlg::password2=%d, '%s'\n", ret, password.c_str());

    ret = dlg::password3("dlg::password3", password, file);
    printf("dlg::password3=%d, '%s', '%s'\n", ret, password.c_str(), file.c_str());

    dlg::PASSWORD_CANCEL = "custom &cancel";
    dlg::PASSWORD_OK = "custom &ok";
    ret = dlg::password4("dlg::password4", password, file);
    printf("dlg::password4=%d, '%s', '%s'\n", ret, password.c_str(), file.c_str());

    fflush(stdout);
}

//------------------------------------------------------------------------------
void test_select() {
    std::vector<std::string> v;
    char c[100];

    for (int f = 1; f <= 150; f++) {
        snprintf(c, 100, "Hello World %05d", f);
        v.push_back(std::string(c));
    }

    printf("line %d was selected\n", (int) dlg::select("dlg::select", v, 66));
    printf("line %d was selected\n", (int) dlg::select("dlg::select", v, "Hello World 00044"));
    fflush(stdout);
}

//------------------------------------------------------------------------------
void test_slider() {
    auto v = 12345.0;
    auto r = dlg::slider("dlg::slider", 1, 20'000, v, 0.1);
    printf("ok=%d\nv=%f\n", r, v);
    fflush(stdout);

    v = 66;
    r = dlg::slider("dlg::slider", 6, 666, v);
    printf("ok=%d\nv=%f\n", r, v);

    v = 10'000'000;
    r = dlg::slider("dlg::slider", 0, 1'000'000'000, v, 1'000'000);
    printf("ok=%d\nv=%f\n", r, v);

    v = 10'000'000;
    r = dlg::slider("dlg::slider", 10'000'000, 10'000'001, v, 0.01);
    printf("ok=%d\nv=%f\n", r, v);
}

//------------------------------------------------------------------------------
void test_text() {
    dlg::text("dlg::text", HAMLET_TEXT);

    std::string t = HAMLET_TEXT;
    printf("dlg::text_edit: %d\n", dlg::text_edit("dlg::text_edit", t, nullptr, 70, 50));
    if (t != HAMLET_TEXT) printf("text changed!\n%s\n", t.c_str());
    else printf("text NOT changed!\n");
}

//------------------------------------------------------------------------------
void test_theme() {
    dlg::theme(true, true);
}

//------------------------------------------------------------------------------
void test_wait() {
    auto w = Fl_Double_Window(100, 100, 480, 240, "WaitCursor - Press Escape to quit");
    w.show();
    util::center_window(&w);
    WaitCursor wc1;
    WaitCursor wc2;
    WaitCursor wc3;
    Fl::run();
}

//------------------------------------------------------------------------------
void test_work() {
    {
        auto work  = dlg::WorkDialog("dlg::WorkDialog", nullptr, true, true);
        auto start = util::clock();

        while (true) {
            auto s = util::format("@bRunning for = %.1f sec", util::clock() - start);

            if (work.run(0.1, s) == false) {
                break;
            }

            util::sleep(20);
        }
    }

    {
        auto work = dlg::WorkDialog("dlg::WorkDialog", nullptr, false, false, 60, 10);
        auto stop = util::clock() + 5.0;

        while (true) {
            auto m = std::vector<std::string>();

            m.push_back(util::format("@bHello World"));
            m.push_back(util::format("Quitting in %.1f sec", stop - util::clock()));
            m.push_back(util::format("@B95Quitting in %.1f sec", stop - util::clock()));

            if (work.run(0.1, m) == false) {
                break;
            }
            else if (util::clock() - stop > 0.0) {
                break;
            }

            util::sleep(20);
        }
    }
}

//------------------------------------------------------------------------------
int main(int argc, const char** argv) {
    theme::parse(argc, argv);

    puts("run individual tests with");
    puts("    abort");
    puts("    check");
    puts("    choice");
    puts("    date");
    puts("    font");
    puts("    html");
    puts("    list");
    puts("    print");
    puts("    print1");
    puts("    print2");
    puts("    pwd");
    puts("    select");
    puts("    slider");
    puts("    text");
    puts("    theme");
    puts("    wait");
    puts("    work");
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
            test_font_small();
            run = "";
        }
        else if (run == "loadbig") {
            test_font_big();
            run = "";
        }

        if (run == "" || run == "theme") {
            test_theme();
        }

        if (run == "" || run == "font") {
            test_font();
        }

        if (run == "font2") {
            test_font2();
        }

        if (run == "" || run == "abort") {
            test_abort();
        }

        if (run == "" || run == "check") {
            test_check();
        }

        if (run == "" || run == "choice") {
            test_choice();
        }

        if (run == "" || run == "date") {
            test_date();
        }

        if (run == "" || run == "html") {
            test_html();
        }

        if (run == "" || run == "list") {
            test_list();
        }

        if (run == "" || run == "print")  {
            test_print();
        }

        if (run == "print1")  {
            test_print1();
        }

        if (run == "print2")  {
            test_print2();
        }

        if (run == "print3")  {
            test_print3();
        }

        if (run == "" || run == "select") {
            test_select();
        }

        if (run == "" || run == "slider")  {
            test_slider();
        }

        if (run == "" || run == "pwd")  {
            test_pwd();
        }

        if (run == "" || run == "text") {
            test_text();
        }

        if (run == "" || run == "wait") {
            test_wait();
        }

        if (run == "" || run == "work")  {
            test_work();
        }
    }

    printf("\nTests Finished\n");
    return 0;
}
