// Copyright gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#include "flw.h"

//------------------------------------------------------------------------------
void date_example() {
    // [flw::dlg::date example]

    auto date = gnu::Date("31/12/1999");

    if (flw::dlg::date("flw::dlg::date()", date) == true) {
        printf("Date = %s\n", date.format(gnu::Date::Format::ISO_LONG).c_str());
    }
    else {
        printf("No date was selected\n");
    }
/*
Date = 1999-12-31
*/

    // [flw::dlg::date example]
}

//------------------------------------------------------------------------------
void font_example1() {
    // [flw::FontDialog example]

    // Create and run font dialog.
    auto font = flw::FontDialog(FL_HELVETICA, 14);

    if (font.run() == true) {
        printf("font=%d, fontsize=%d, fontname=%s\n", font.font(),  font.fontsize(), font.fontname().c_str());

        // Set global regular font.
        flw::PREF_FONT     = font.font();
        flw::PREF_FONTSIZE = font.fontsize();
    }
    else {
        printf("no font selected\n");
    }

    // [flw::FontDialog example]
}

//------------------------------------------------------------------------------
void font_example2() {
    // [flw::dlg::font example]

    auto font     = FL_HELVETICA;
    auto fontsize = 14;
    auto fontname = std::string();

    if (flw::dlg::font("flw::dlg::font()", font, fontsize, fontname) == true) {
        printf("font=%d, fontsize=%d, fontname=%s\n", font,  fontsize, fontname.c_str());

        // Set global regular font.
        flw::PREF_FONT     = font;
        flw::PREF_FONTSIZE = fontsize;
    }

    // [flw::dlg::font example]
}

//------------------------------------------------------------------------------
void html_example() {
    // [flw::dlg::html example]

    flw::dlg::html("flw::dlg::html()", "<h1>Hello World</h1><ul><li>Hello<li>World</ul>This is some text<br>And more text");

    // [flw::dlg::html example]
}

//------------------------------------------------------------------------------
void list_example() {
    // [flw::dlg::list example]

    flw::StringVector list = {"@LHello", "World", "How", "Are", "You?"};

    // Use fixed font for the items and split input string on newline.
    flw::dlg::list("flw::dlg::list()", "@LHello\nWorld\nHow\nAre\nYou?\n", true);

    // Use a vector of std::string objects.
    flw::dlg::list("flw::dlg::list()", list);

    // Load a text file.
    flw::dlg::list_file("flw::dlg::list_file()", "Makefile");

    // [flw::dlg::list example]
}

//------------------------------------------------------------------------------
void password_example() {
    // [flw::dlg::password example]

    std::string password, file;

    if (flw::dlg::password_confirm_and_file("flw::dlg::password_confirm_and_file()", password, file) == true) {
        printf("password='%s', file='%s'\n", password.c_str(), file.c_str());
    }
    else {
        printf("no password was set\n");
    }

    // [flw::dlg::password example]
}

//------------------------------------------------------------------------------
// [flw::dlg::print_page_callback example]
bool print_callback(void*, int pw, int ph, int page) {
    char b[100];
    snprintf(b, 100, "%d", page);

    fl_font(FL_COURIER, 36);
    fl_color(FL_BLACK);
    fl_line_style(FL_SOLID, 5);
    fl_rect(0, 0, pw, ph);
    fl_draw(b, 0, 0, pw, ph, FL_ALIGN_INSIDE | FL_ALIGN_CENTER, nullptr, 0);

    // Print only until page 5.
    return page < 5;
}
// [flw::dlg::print_page_callback example]

//------------------------------------------------------------------------------
void print_example1() {
    // [flw::dlg::print_page example]

    // Print pages using a user callback for drawing.
    flw::dlg::print_page("flw::dlg::print_page() - Print User Callback", print_callback, nullptr, 1, 10);

    // [flw::dlg::print_page example]
}

//------------------------------------------------------------------------------
void print_example2() {
    // [flw::dlg::print_text example]

const std::string TEXT = R"(To be, or not to be: that is the question:
Whether 'tis nobler in the mind to suffer
The slings and arrows of outrageous fortune,
Or to take arms against a sea of troubles,
And by opposing end them? To die: to sleep;

- William Shakespeare, Hamlet
)";

    flw::dlg::print_text("flw::dlg::print_text()", TEXT);

    // [flw::dlg::print_text example]
}

//------------------------------------------------------------------------------
void print_example3() {
    // [flw::postscript::PrintText example]

const std::string TEXT = R"(To be, or not to be: that is the question:
Whether 'tis nobler in the mind to suffer
The slings and arrows of outrageous fortune,
Or to take arms against a sea of troubles,
And by opposing end them? To die: to sleep;

- William Shakespeare, Hamlet
)";

    // Create printer object.
    auto printer = flw::postscript::PrintText(
        "text.ps",                                  // Output file.
        Fl_Paged_Device::Page_Format::A4,           // A4 page format.
        Fl_Paged_Device::Page_Layout::PORTRAIT,     // Portrait format.
        FL_COURIER,                                 // Font to use.
        14,                                         // Font size.
        FL_ALIGN_LEFT,                              // Align text to the left.
        false,                                      // Do not wrap text.
        true,                                       // Print a border.
        2                                           // Print line number with a width of 2.
    );

    // Now print to file.
    printer.print(TEXT);

    // [flw::postscript::PrintText example]
}

//------------------------------------------------------------------------------
void progress_example() {
    // [flw::ProgressDialog example]

    auto progress = flw::ProgressDialog("flw::ProgressDialog Example", true, true, 0.0, 100.0);
    auto count    = 0.0;

    progress.start();

    while (count <= 100.0) {
        char buf[200];

        flw::util::sleep(10);
        count += 0.1;
        sprintf(buf, "Counting 0 to 100\nCurrent value is %.1f\nPress button to interrupt", count);

        if (progress.update(count, buf) == false) {
            break;
        }
    }

    if (count >= 100.0) {
        puts("done");
    }
    else {
        printf("aborted at %.1f\n", progress.value());
    }

    // [flw::ProgressDialog example]
}

//------------------------------------------------------------------------------
void select_checkboxes_example() {
    // [flw::dlg::select_checkboxes example]

    flw::StringVector checkboxes = { "1Hello", "1World", "0End", };

    // Show dialog with three check buttons.
    checkboxes = flw::dlg::select_checkboxes("flw::dlg::select_checkboxes()", checkboxes);

    // If user has pressed ok 'checks' will contain the updated values.
    for (auto& s : checkboxes) {
        printf("%s\n", s.c_str());
    }
/*
0Hello
0World
1End
*/
    // [flw::dlg::select_checkboxes example]
}

//------------------------------------------------------------------------------
void select_choice_example() {
    // [flw::dlg::select_choice example]

    flw::StringVector choices = {"Hello", "World", "How", "Are", "You?"};
    int index = flw::dlg::select_choice("flw::dlg::select_choice()", "Select one of the strings.", choices, 1);
    printf("selected index was %d\n", index);

    // [flw::dlg::select_choice example]
}

//------------------------------------------------------------------------------
void select_string_example() {
    // [flw::dlg::select example]

    flw::StringVector list = {"Hello", "World", "How", "Are", "You?"};

    // Show dialog with at list of strings and selected row with "World".
    int index = flw::dlg::select_string("flw::dlg::select_string()", list, "World");
    printf("selected index was %d\n", index);

    // [flw::dlg::select example]
}

//------------------------------------------------------------------------------
void slider_example() {
    // [flw::dlg::slider example]

    auto value = 666.66;
    auto res   = flw::dlg::slider("flw::dlg::slider()", 600, 700, value, 0.01);

    if (res == true) {
        printf("new value=%.2f\n", value);
    }
    else {
        printf("unchanged value=%.2f\n", value);
    }

    // [flw::dlg::slider example]
}

//------------------------------------------------------------------------------
void text_example() {
    // [flw::dlg::text_edit example]

    std::string text = "Hello\nWorld\n";

    if (flw::dlg::text_edit("flw::dlg::text_edit()", text) == true) {
        printf("Text changed:\n%s\n", text.c_str());
    }
    else {
        printf("Text not changed:\n%s\n", text.c_str());
    }
/*
Text changed:
Hello
New
World
*/

    // [flw::dlg::text_edit example]
}

//------------------------------------------------------------------------------
void theme_example() {
    // [flw::dlg::theme example]

    flw::dlg::theme(true, true);

    printf("Font          = %3d\n", flw::PREF_FONT);
    printf("Fontsize      = %3d\n", flw::PREF_FONTSIZE);
    printf("Fontname      = %s\n", flw::PREF_FONTNAME.c_str());
    printf("FixedFont     = %3d\n", flw::PREF_FIXED_FONT);
    printf("FixedFontsize = %3d\n", flw::PREF_FIXED_FONTSIZE);
    printf("FixedFontname = %s\n", flw::PREF_FIXED_FONTNAME.c_str());
/*
Font          =   0
Fontsize      =  14
Fontname      = sans
FixedFont     =   4
FixedFontsize =  14
FixedFontname = mono
*/
    // [flw::dlg::theme example]
}

//------------------------------------------------------------------------------
int main() {
    flw::theme::load("oxy");
    Fl::screen_scale(0, 1.0);

    theme_example();
    date_example();
    font_example1();
    font_example2();
    html_example();
    list_example();
    password_example();
    print_example1();
    print_example2();
    print_example3();
    progress_example();
    select_checkboxes_example();
    select_choice_example();
    select_string_example();
    slider_example();
    text_example();
}
