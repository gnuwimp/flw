// Copyright gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#include "flw.h"

//------------------------------------------------------------------------------
void font_example1() {
    // [flw::dlg::FontDialog example]

    // Create and run font dialog.
    auto fontdialog = flw::dlg::FontDialog(FL_HELVETICA, 14);

    if (fontdialog.run() == true) {
        printf("font=%d, fontsize=%d, fontname=%s\n", fontdialog.font(),  fontdialog.fontsize(), fontdialog.fontname().c_str());

        // Set global regular font.
        flw::PREF_FONT = fontdialog.font();
        flw::PREF_FONTSIZE = fontdialog.fontsize();
    }
    else {
        printf("no font selected\n");
    }

    // [flw::dlg::FontDialog example]
}

//------------------------------------------------------------------------------
void font_example2() {
    // [flw::dlg::font example]

    Fl_Font font = FL_HELVETICA;
    Fl_Fontsize fontsize = 14;
    std::string fontname;

    if (flw::dlg::font(font, fontsize, fontname) == true) {
        printf("font=%d, fontsize=%d, fontname=%s\n", font,  fontsize, fontname.c_str());

        // Set global regular font.
        flw::PREF_FONT = font;
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
    flw::dlg::list("flw::dlg::list()", "@LHello\nWorld\nHow\nAre\nYou?\n");
    flw::dlg::list("flw::dlg::list()", list);

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
// [flw::dlg::print_callback example]
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
// [flw::dlg::print_callback example]

//------------------------------------------------------------------------------
void print_example1() {
    // [flw::dlg::print example]

    // Print pages using a user callback for drawing.
    flw::dlg::print("flw::dlg::print() - Print User Callback", print_callback, nullptr, 1, 10);

    // [flw::dlg::print example]
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
void select_checkboxes_example() {
    // [flw::dlg::select_checkboxes example]

    flw::StringVector checks = { "1Hello", "1World", "0End", };

    // Show dialog with three check buttons.
    checks = flw::dlg::select_checkboxes("flw::dlg::select_checkboxes()", checks);

    // If user has pressed ok 'checks' will contain the updated values.
    for (auto& s : checks) {
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
    int index = flw::dlg::select_choice("flw::dlg::select_choice()", choices, 1);
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

    auto input_output_value = 666.66;
    auto ok = flw::dlg::slider("flw::dlg::slider()", 600, 700, input_output_value, 0.01);

    if (ok == true) {
        printf("new value=%.2f\n", input_output_value);
    }
    else {
        printf("unchanged value=%.2f\n", input_output_value);
    }

    // [flw::dlg::slider example]
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
Font          =  10
Fontsize      =  14
Fontname      = serif italic
FixedFont     = 178
FixedFontsize =  13
FixedFontname = Lucida Console
*/
    // [flw::dlg::theme example]
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
void work_example() {
    // [flw::dlg::WorkDialog example]

    auto workdialog = flw::dlg::WorkDialog("flw::dlg::WorkDialog Example", true, true, 0.0, 100.0);
    auto count  = 0.0;

    workdialog.start();

    while (count <= 100.0) {
        char buf[200];

        flw::util::sleep(10);
        count += 0.1;
        sprintf(buf, "Counting 0 to 100\nCurrent value is %.1f\nPress button to interrupt", count);

        if (workdialog.update(count, buf) == false) {
            break;
        }
    }

    if (count >= 100.0) {
        puts("done");
    }
    else {
        printf("aborted at %.1f\n", workdialog.value());
    }

    // [flw::dlg::WorkDialog example]
}

//------------------------------------------------------------------------------
int main() {
    flw::theme::load("oxy");
    Fl::screen_scale(0, 1.0);

    theme_example();
    font_example1();
    font_example2();
    html_example();
    list_example();
    password_example();
    print_example1();
    print_example2();
    select_checkboxes_example();
    select_choice_example();
    select_string_example();
    slider_example();
    text_example();
    work_example();
}
