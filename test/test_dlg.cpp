// Copyright gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#include "flw.h"

#ifndef FLW_AMALGAM
    #include "datechooser.h"
    #include "dlg.h"
    #include "file.h"
    #include "fontdialog.h"
    #include "postscript.h"
    #include "progressdialog.h"
    #include "svgbutton.h"
    #include "tableeditor.h"
    #include "theme.h"
    #include "waitcursor.h"
#endif

#include "test.h"
#include <FL/Fl.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/fl_ask.H>
#include <FL/fl_draw.H>

using namespace flw;

//------------------------------------------------------------------------------
void test_check() {
    size_t       c = 0;
    StringVector v = { "1Whether", "0tis nobler", "1in the", "0mind to suffer", "1The slings",
                       "0and arrows", "1of outrageous", "0fortune", "1Or to", "0take arms",
                       "1against", "0a sea", "1of troubles", "0And by", "1opposing", "0end them?",
                       "1To die", "0to sleep", "1No more", "0and by a", "1sleep to say", "0we end" };

    for (auto s : dlg::select_checkboxes("flw::dlg::select_checkboxes", v)) {
        printf("%2u = %s => %s\n", (unsigned) c, s.c_str(), v[c].c_str());
        c++;
    }
    fflush(stdout);
}

//------------------------------------------------------------------------------
void test_choice() {
    StringVector v = {"Hello", "World", "How", "Are", "You?"};
    auto v2 = util::split_string(HAMLET_TEXT, "\n");

    //dlg::options(nullptr, true, true);
    printf("line %d was selected\n", dlg::select_choice("flw::dlg::select_choice", "Two line message.\nHello World.", v, 1));
    printf("line %d was selected\n", dlg::select_choice("flw::dlg::select_choice", "One line message.", v2));
    fflush(stdout);
}

//------------------------------------------------------------------------------
void test_date() {
    gnu::Date date = gnu::Date("31/12/1999");

    if (flw::dlg::date("flw::dlg::date()", date)) {
        printf("flw::dlg::date=%s\n", date.format().c_str());
    }
    else {
        printf("No date was selected\n");
    }

    fflush(stdout);
}

//------------------------------------------------------------------------------
void test_font() {
#ifdef _WIN32
    auto dlg = FontDialog("Candara", 15, "flw::FontDialog");
#else
    auto dlg = FontDialog("Nimbus Sans", 15, "flw::FontDialog");
#endif
    //dlg.deactivate_font();
    //dlg.deactivate_fontsize();

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

    if (dlg::font("flw::dlg::font()", font, fontsize, fontname, false) == true) {
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
    dlg::html("flw::dlg::html", html);
}

//------------------------------------------------------------------------------
void test_input() {
    std::string value = "Hello World";
    FLW_PRINTV(dlg::input("flw::dlg::input", TWO_LINE, value), value)
    //FLW_PRINTV(dlg::input("flw::dlg::input", LONG_LINE, value), value)
    FLW_PRINTV(dlg::input("flw::dlg::input", FIVE_LINE, value), value)

    int64_t value2 = 12345678912345; (void) value2;
    FLW_PRINTV(dlg::input_int("flw::dlg::input_int", TWO_LINE, value2), value2)

    double value3 = 123.456; (void) value3;
    FLW_PRINTV(dlg::input_double("flw::dlg::input_double", TWO_LINE, value3), value3)

    std::string value4 = THREE_LINE + THREE_LINE + THREE_LINE;
    FLW_PRINTV(dlg::input_multi("flw::dlg::input_multi", TWO_LINE, value4), value4)
    value4 = THREE_LINE + THREE_LINE + THREE_LINE;
    FLW_PRINTV(dlg::input_multi("flw::dlg::input_multi", LONG_LINE, value4), value4)

    std::string value5 = "Password";
    FLW_PRINTV(dlg::input_secret("flw::dlg::input_secret", TWO_LINE, value5), value5)
}

//------------------------------------------------------------------------------
void test_list() {
    dlg::list("flw::dlg::list", HAMLET_LIST);
    dlg::list("flw::dlg::list - Fixed Font", HAMLET_LIST, true);
    dlg::list_file("flw::dlg::list_file - File", "test/browser.txt", true);
}

//------------------------------------------------------------------------------
void test_message() {
    //dlg::msg_alert("flw::dlg::msg_alert", LONG_LINE);
    //dlg::msg_alert("flw::dlg::msg_alert", LONG_LINE2);

    //dlg::msg("flw::dlg::msg", TWO_LINE);

    //dlg::msg_alert("flw::dlg::msg_alert", TWO_LINE);
    //dlg::msg_alert("flw::dlg::msg_alert", FIVE_LINE);
    //dlg::msg_error("flw::dlg::msg_error", TWO_LINE);

    //FLW_PRINTV(dlg::msg_ask("flw::dlg::msg_ask", "EMPTY", "", "", ""))
    //FLW_PRINTV(dlg::msg_ask("flw::dlg::msg_ask", TWO_LINE, "LEFT:LEFT", "RIGHT:RIGHT", "UP:!UP", "DOWN:DOWN", ""))
    FLW_PRINTV(dlg::msg_ask("flw::dlg::msg_ask", TWO_LINE, "CANCEL:^CANCEL", "DEL:^DEL", "BACK:!^BACK", "No Dark Bg", "CONFIRM:^CONFIRM"))
    FLW_PRINTV(dlg::msg_ask("flw::dlg::msg_ask", TWO_LINE, "One", "", "Two", "Three"))
    FLW_PRINTV(dlg::msg_ask("flw::dlg::msg_ask", TWO_LINE, "", "", "One"))
    FLW_PRINTV(dlg::msg_ask("flw::dlg::msg_ask", TWO_LINE))

    FLW_PRINTV(dlg::msg_warning("flw::dlg::msg_warning", TWO_LINE, "&One", "!&Two"))
    FLW_PRINTV(dlg::msg_warning("flw::dlg::msg_warning", TWO_LINE))
}

//------------------------------------------------------------------------------
bool test_print_a(void*, int pw, int ph, unsigned page) {
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
bool test_print_b(void*, int pw, int ph, unsigned page) {
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
bool test_print_c(void*, int pw, int ph, unsigned page) {
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
void test_print1() {
    {
        system("rm -f print_page_to_file.ps");
        FLW_PRINT(postscript::print_page_to_file("print_page_to_file.ps", Fl_Paged_Device::Page_Format::A4, Fl_Paged_Device::Page_Layout::PORTRAIT, test_print_a, nullptr));
        system("okular print_page_to_file.ps");
        
        system("rm -f print_pages_to_file.ps");
        FLW_PRINT(postscript::print_pages_to_file("print_pages_to_file.ps", Fl_Paged_Device::Page_Format::A4, Fl_Paged_Device::Page_Layout::PORTRAIT, test_print_b, nullptr, 23, 35));
        system("okular print_pages_to_file.ps");
        
        FLW_PRINT(postscript::print_pages_to_file("print_pages_to_file2.ps", Fl_Paged_Device::Page_Format::A4, Fl_Paged_Device::Page_Layout::PORTRAIT, test_print_b, nullptr, 1, 0));
    }
}

//------------------------------------------------------------------------------
void test_print2() {
    {
        system("rm -f output.ps");
        dlg::print_page("flw::dlg::print - One Page", test_print_a);
        system("okular output.ps");
        
        system("rm -f output.ps");
        dlg::print_page("flw::dlg::print - Many Pages", test_print_b, nullptr, 10, 30);
        system("okular output.ps");
        
        system("rm -f output.ps");
        dlg::print_page("flw::dlg::print - Stop On Page 6", test_print_c, nullptr, 1, 11);
        system("okular output.ps");
    }
}

//------------------------------------------------------------------------------
void test_print3() {
    auto r = 0;

    while (r >= 0) {
        system("rm -f output.ps");
        
    #ifdef _WIN32
        dlg::center_fl_message_dialog();
        r = fl_choice_n("%s", "Print Long Lines", "Print Hamlet", "Print UTF8", "Print some test texts.\nSome fonts might have issues.");

        if (r == 0) dlg::print_text("flw::dlg::print_text", LONG_TEXT);
        else if (r == 1) dlg::print_text("flw::dlg::print_text", HAMLET_TEXT);
        else if (r == 2) dlg::print_text("flw::dlg::print_text", UTF8_TEXT);
    #else
        std::string ps = "output.ps";
        auto p = false;

        dlg::center_fl_message_dialog();
        r = fl_choice_n("Print to the default %s file.\nIt will be opened with system default program.\nThere are font issues if you use any other than the normal fltk fonts.\nAnd lot of the utf8 characters does not print well.", "Print Long Lines", "Print Hamlet", "Print UTF8", ps.c_str());

        if (r == 0) p = dlg::print_text("flw::dlg::print_text", LONG_TEXT);
        else if (r == 1) p = dlg::print_text("flw::dlg::print_text", HAMLET_TEXT);
        else if (r == 2) p = dlg::print_text("flw::dlg::print_text", UTF8_TEXT);

        if (p == true && r >= 0) {
            system("okular output.ps");
        }
    #endif
    }
}

//------------------------------------------------------------------------------
void test_print4() {
    {
        auto printer = postscript::PrintText("print_hamlet.ps", Fl_Paged_Device::Page_Format::A4, Fl_Paged_Device::Page_Layout::PORTRAIT, FL_HELVETICA_ITALIC, 14, FL_ALIGN_LEFT, false, false, 0);
        printer.print(HAMLET_TEXT);
    }

    {
        auto printer = postscript::PrintText("print_hamlet_center.ps", Fl_Paged_Device::Page_Format::A4, Fl_Paged_Device::Page_Layout::PORTRAIT, FL_HELVETICA_ITALIC, 14, FL_ALIGN_CENTER, false, false, 0);
        printer.print(HAMLET_TEXT);
    }

    {
        auto printer = postscript::PrintText("print_hamlet_right.ps", Fl_Paged_Device::Page_Format::A4, Fl_Paged_Device::Page_Layout::PORTRAIT, FL_HELVETICA_ITALIC, 14, FL_ALIGN_RIGHT, false, true, 0);
        printer.print(HAMLET_TEXT);
    }

    {
        auto printer = postscript::PrintText("print_long.ps", Fl_Paged_Device::Page_Format::A4, Fl_Paged_Device::Page_Layout::PORTRAIT, FL_COURIER, 14, FL_ALIGN_LEFT, false, true, 0);
        printer.print(LONG_TEXT);
    }

    {
        auto printer = postscript::PrintText("print_long_linenum.ps", Fl_Paged_Device::Page_Format::A4, Fl_Paged_Device::Page_Layout::PORTRAIT, FL_COURIER, 14, FL_ALIGN_LEFT, false, true, 4);
        printer.print(LONG_TEXT);
    }

    {
        auto printer = postscript::PrintText("print_long_wrapped.ps", Fl_Paged_Device::Page_Format::A4, Fl_Paged_Device::Page_Layout::PORTRAIT, FL_COURIER, 14, FL_ALIGN_LEFT, true, false, 0);
        printer.print(LONG_TEXT);
    }

    {
        auto printer = postscript::PrintText("print_long_wrapped_border.ps", Fl_Paged_Device::Page_Format::A4, Fl_Paged_Device::Page_Layout::PORTRAIT, FL_COURIER, 14, FL_ALIGN_LEFT, true, true, 0);
        printer.print(LONG_TEXT);
    }

    {
        auto printer = postscript::PrintText("print_long_wrapped_a3.ps", Fl_Paged_Device::Page_Format::A3, Fl_Paged_Device::Page_Layout::REVERSED, FL_HELVETICA, 16, FL_ALIGN_LEFT, true, true, 0);
        printer.print(LONG_TEXT);
    }

    {
        auto printer = postscript::PrintText("print_long_wrapped_a5.ps", Fl_Paged_Device::Page_Format::A5, Fl_Paged_Device::Page_Layout::LANDSCAPE, FL_TIMES_BOLD_ITALIC, 18, FL_ALIGN_LEFT, true, true, 0);
        printer.print(LONG_TEXT);
    }

    {
        auto printer = postscript::PrintText("print_long_wrapped_border_linenum.ps", Fl_Paged_Device::Page_Format::A4, Fl_Paged_Device::Page_Layout::PORTRAIT, FL_COURIER, 14, FL_ALIGN_LEFT, true, true, 6);
        printer.print(LONG_TEXT);
    }

    {
        auto printer = postscript::PrintText("print_long_wrapped_border_center.ps", Fl_Paged_Device::Page_Format::A4, Fl_Paged_Device::Page_Layout::PORTRAIT, FL_COURIER, 14, FL_ALIGN_CENTER, true, true, 6);
        printer.print(LONG_TEXT, 8);
    }

    {
        auto printer = postscript::PrintText("print_long_wrapped_border_right.ps", Fl_Paged_Device::Page_Format::A4, Fl_Paged_Device::Page_Layout::PORTRAIT, FL_COURIER, 16, FL_ALIGN_RIGHT, true, true, 6);
        printer.print(LONG_TEXT, 8);
    }

    {
        auto printer = postscript::PrintText("print_long_wrapped_border_linenum_24.ps", Fl_Paged_Device::Page_Format::A4, Fl_Paged_Device::Page_Layout::PORTRAIT, FL_COURIER, 24, FL_ALIGN_LEFT, true, true, 6);
        printer.print(LONG_TEXT);
    }

    {
        auto printer = postscript::PrintText("print_long_wrapped_border_linenum_36.ps", Fl_Paged_Device::Page_Format::A4, Fl_Paged_Device::Page_Layout::PORTRAIT, FL_COURIER, 36, FL_ALIGN_LEFT, true, true, 6);
        printer.print(LONG_TEXT);
    }

    {
        auto printer = postscript::PrintText("print_utf8.ps", Fl_Paged_Device::Page_Format::A4, Fl_Paged_Device::Page_Layout::PORTRAIT, FL_COURIER, 14, false, false, 0);
        printer.print(UTF8_TEXT);
    }
}

//------------------------------------------------------------------------------
void test_print5() {
    auto r  = 0;
    auto b1 = gnu::file::read("flw.h");
    auto b2 = gnu::file::read("flw.cpp");

    while (r >= 0 && b1.c_str() && b2.c_str()) {
        system("rm -f output.ps");

    #ifdef _WIN32
        dlg::center_fl_message_dialog();
        r = fl_choice_n("%s", "flw.h", "flw.cpp", nullptr, "Print source code.");

        if (r == 0) dlg::print_text("flw::dlg::print_text", b1.c_str());
        else if (r == 1) dlg::print_text("flw::dlg::print_text", b2.c_str());
    #else
        std::string ps = "output.ps";
        auto p = false;

        dlg::center_fl_message_dialog();
        r = fl_choice_n("%s", "flw.h", "flw.cpp", nullptr, "Print source code.");

        if (r == 0) p = dlg::print_text("flw::dlg::print_text", b1.c_str());
        else if (r == 1) p = dlg::print_text("flw::dlg::print_text", b2.c_str());

        if (p == true && r >= 0) {
            system("okular output.ps");
        }
    #endif
    }
}

//------------------------------------------------------------------------------
void test_progress() {
    {
        auto work  = ProgressDialog("flw::dlg::Progress with a single string message", true, true);
        auto start = util::clock();

        work.start();

        while (true) {
            auto s = util::format("@lRunning forever %.1f sec", util::clock() - start);

            if (work.update(s) == false) {
                break;
            }

            util::sleep(20);
        }
    }

    {
        auto work = ProgressDialog("flw::dlg::Progress with progress bar", true, true, 0.0, 0.0);
        auto val  = 0.0;

        work.range(0.0, 100.0);
        work.resize(0, 0, 640, 480);
        work.start();

        while (true) {
            auto m = std::vector<std::string>();

            m.push_back(util::format("@bWorking from 0.0 to 100.0"));
            m.push_back(util::format("@B95Current value is %.1f", val));

            if (work.update(val, m, 50) == false || work.value() >= 100.0) {
                break;
            }

            util::sleep(20);
            val += 0.2;
        }
    }
}

//------------------------------------------------------------------------------
void test_pwd() {
    std::string password, file;
    bool ret;

    ret = dlg::password("flw::dlg::password", password);
    printf("flw::dlg::password = %s, '%s'\n", ret ? "TRUE" : "FALSE", password.c_str());

    ret = dlg::password_confirm("flw::dlg::password_check", password);
    printf("flw::dlg::password_confirm = %s, '%s'\n", ret ? "TRUE" : "FALSE", password.c_str());

    ret = dlg::password_and_file("flw::dlg::password_with_file", password, file);
    printf("flw::dlg::password_and_file = %s, '%s', '%s'\n", ret ? "TRUE" : "FALSE", password.c_str(), file.c_str());

    ret = dlg::password_confirm_and_file("flw::dlg::password_check_with_file", password, file);
    printf("flw::dlg::password_confirm_and_file = %s, '%s', '%s'\n", ret ? "TRUE" : "FALSE", password.c_str(), file.c_str());

    fflush(stdout);
}

//------------------------------------------------------------------------------
void test_select_string() {
    std::vector<std::string> v;
    char c[100];

    for (int f = 0; f <= 150; f++) {
        snprintf(c, 100, "Hello World %05d", f);
        v.push_back(std::string(c));
    }

    printf("line %d was selected\n", (int) dlg::select_string("flw::dlg::select_string", v, 66));
    printf("line %d was selected\n", (int) dlg::select_string("flw::dlg::select_string", v, "Hello World 00044"));
    //printf("line %d was selected\n", (int) dlg::select_string("flw::dlg::select_string", v, "Hello World 00000"));
    //printf("line %d was selected\n", (int) dlg::select_string("flw::dlg::select_string", v, "Hello World 00150"));
    fflush(stdout);
}

//------------------------------------------------------------------------------
void test_slider() {
    auto v = 12345.0;
    auto r = dlg::slider("flw::dlg::slider", 1, 20'000, v, 0.1);
    printf("ok=%d\nv=%f\n", r, v);
    fflush(stdout);

    v = 66;
    r = dlg::slider("flw::dlg::slider", 6, 666, v);
    printf("ok=%d\nv=%f\n", r, v);

    v = 10'000'000;
    r = dlg::slider("flw::dlg::slider", 0, 1'000'000'000, v, 1'000'000);
    printf("ok=%d\nv=%f\n", r, v);

    v = 10'000'000;
    r = dlg::slider("flw::dlg::slider", 10'000'000, 10'000'001, v, 0.01);
    printf("ok=%d\nv=%f\n", r, v);
}

//------------------------------------------------------------------------------
void test_svg() {
    auto size = flw::PREF_FONTSIZE * 9;
    auto w    = size * 10;
    auto h    = size * 6;
    auto win  = Fl_Double_Window(0, 0, w, h);

    //SVGButton::SPACING = 0; // 8 is the default.

    new SVGButton(size * 0, size * 0, size, size, "icons::ALERT",   icons::ALERT,   false, true, SVGButton::Pos::ABOVE, 3.0);
    new SVGButton(size * 1, size * 0, size, size, "icons::BACK",    icons::BACK,    false, true, SVGButton::Pos::ABOVE, 3.0);
    new SVGButton(size * 2, size * 0, size, size, "icons::CANCEL",  icons::CANCEL,  false, true, SVGButton::Pos::ABOVE, 3.0);
    new SVGButton(size * 3, size * 0, size, size, "icons::CONFIRM", icons::CONFIRM, false, true, SVGButton::Pos::ABOVE, 3.0);
    new SVGButton(size * 4, size * 0, size, size, "icons::DEL",     icons::DEL,     false, true, SVGButton::Pos::ABOVE, 3.0);
    new SVGButton(size * 5, size * 0, size, size, "icons::DOWN",    icons::DOWN,    false, true, SVGButton::Pos::ABOVE, 3.0);
    new SVGButton(size * 6, size * 0, size, size, "icons::EDIT",    icons::EDIT,    false, true, SVGButton::Pos::ABOVE, 3.0);
    new SVGButton(size * 7, size * 0, size, size, "icons::ERR",     icons::ERR,     false, true, SVGButton::Pos::ABOVE, 3.0);
    new SVGButton(size * 8, size * 0, size, size, "icons::FORWARD", icons::FORWARD, false, true, SVGButton::Pos::ABOVE, 3.0);
    new SVGButton(size * 9, size * 0, size, size, "icons::INFO",    icons::INFO,    false, true, SVGButton::Pos::ABOVE, 3.0);
    
    new SVGButton(size * 0, size * 1, size, size, "icons::LEFT",    icons::LEFT,    false, true, SVGButton::Pos::ABOVE, 3.0);
    new SVGButton(size * 1, size * 1, size, size, "icons::PASSWORD",icons::PASSWORD,false, true, SVGButton::Pos::ABOVE, 3.0);
    new SVGButton(size * 2, size * 1, size, size, "icons::PAUSE",   icons::PAUSE,   false, true, SVGButton::Pos::ABOVE, 3.0);
    new SVGButton(size * 3, size * 1, size, size, "icons::PLAY",    icons::PLAY,    false, true, SVGButton::Pos::ABOVE, 3.0);
    new SVGButton(size * 4, size * 1, size, size, "icons::QUESTION",icons::QUESTION,false, true, SVGButton::Pos::ABOVE, 3.0);
    new SVGButton(size * 5, size * 1, size, size, "icons::RIGHT",   icons::RIGHT,   false, true, SVGButton::Pos::ABOVE, 3.0);
    new SVGButton(size * 6, size * 1, size, size, "icons::STOP",    icons::STOP,    false, true, SVGButton::Pos::ABOVE, 3.0);
    new SVGButton(size * 7, size * 1, size, size, "icons::UP",      icons::UP,      false, true, SVGButton::Pos::ABOVE, 3.0);
    new SVGButton(size * 8, size * 1, size, size, "icons::WARNING", icons::WARNING, false, true, SVGButton::Pos::ABOVE, 3.0);
    
    new SVGButton(size * 0, size * 2, size, size, "RIGHT",          icons::ALERT,   false, false, SVGButton::Pos::RIGHT, 4.0);
    new SVGButton(size * 1, size * 2, size, size, "LEFT",           icons::ALERT,   false, false, SVGButton::Pos::LEFT, 4.0);
    new SVGButton(size * 2, size * 2, size, size, "ABOVE",          icons::ALERT,   false, false, SVGButton::Pos::ABOVE, 4.0);
    new SVGButton(size * 3, size * 2, size, size, "BELOW",          icons::ALERT,   false, false, SVGButton::Pos::BELOW, 4.0);
    new SVGButton(size * 4, size * 2, size, size, "UNDER",          icons::ALERT,   false, false, SVGButton::Pos::UNDER, 4.0);
    
    auto b = new SVGButton(w / 2 - 150, h - 300, 300, 300, "WARNING", icons::WARNING, false, false, SVGButton::Pos::ABOVE, 7.0);

    util::labelfont(&win);
    b->deactivate();

    util::center_window(&win);
    win.show();
    Fl::run();
}

//------------------------------------------------------------------------------
void test_text() {
    dlg::text("flw::dlg::text", HAMLET_TEXT);

    std::string t = HAMLET_TEXT;
    printf("flw::dlg::text_edit: %d\n", dlg::text_edit("flw::dlg::text_edit", t, 70, 50));
    if (t != HAMLET_TEXT) printf("text changed!\n%s\n", t.c_str());
    else printf("text NOT changed!\n");
}

//------------------------------------------------------------------------------
void test_theme() {
    flw::dlg::theme(true, true);
}

//------------------------------------------------------------------------------
void test_wait() {
    auto w = Fl_Double_Window(100, 100, 480, 240, "flw::WaitCursor - Press Escape to quit");
    w.show();
    util::center_window(&w);
    WaitCursor wc1;
    WaitCursor wc2;
    WaitCursor wc3;
    Fl::run();
}

//------------------------------------------------------------------------------
int main(int argc, const char** argv) {
    puts("run individual tests with");
    puts("    check      => flw::dlg::select_checkboxes()");
    puts("    choice     => flw::dlg:select_choice()");
    puts("    date       => flw::dlg::date()");
    puts("    font       => flw::FontDialog");
    puts("    html       => flw::dlg::html()");
    puts("    inp        => flw::dlg::input()");
    puts("    list       => flw::dlg::list()");
    puts("    msg        => flw::dlg::msg()");
    puts("    print1     => flw::postscript::print_page_to_file()");
    puts("    print2     => flw::dlg::print_page()");
    puts("    print3     => flw::dlg::print_text()");
    puts("    print4     => flw::postscript::PrintText");
    puts("    print5     => flw::dlg::print_text(flw.c/.h)");
    puts("    progress   => flw::ProgressDialog");
    puts("    pwd        => flw::dlg::password...");
    puts("    slider     => flw::dlg::slider()");
    puts("    string     => flw::dlg::select_string()");
    puts("    svg        => flw::SVGButton");
    puts("    text       => flw::dlg::text()");
    puts("    theme      => flw::dlg::theme()");
    puts("    wait       => flw::WaitCursor");
    fflush(stdout);

    {
        auto pref = Fl_Preferences(Fl_Preferences::USER, "gnuwimp_test", "test_flw_dlg");
        flw::theme::load_theme_from_pref(pref);
    }

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

        if (run == "all") {
            run = "";
        }

        if (run != "") {
            printf("Current: %s\n", run.c_str());
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
            auto pref = Fl_Preferences(Fl_Preferences::USER, "gnuwimp_test", "test_flw_dlg");
            pref.clear();
            flw::theme::save_theme_to_pref(pref);
        }

        if (run == "" || run == "font") {
            test_font();
        }

        if (run == "font2") {
            test_font2();
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

        if (run == "" || run == "inp" || run == "input") {
            test_input();
        }

        if (run == "" || run == "list") {
            test_list();
        }

        if (run == "" || run == "msg" || run == "message") {
            test_message();
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

        if (run == "print4")  {
            test_print4();
        }

        if (run == "print5")  {
            test_print5();
        }

        if (run == "" || run == "progress")  {
            test_progress();
        }

        if (run == "" || run == "pwd")  {
            test_pwd();
        }

        if (run == "" || run == "slider")  {
            test_slider();
        }

        if (run == "" || run == "string") {
            test_select_string();
        }

        if (run == "" || run == "svg") {
            test_svg();
        }

        if (run == "" || run == "text") {
            test_text();
        }

        if (run == "" || run == "wait") {
            test_wait();
        }
    }

    printf("\nTests Finished\n");
    return 0;
}
