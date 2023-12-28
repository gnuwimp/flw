// Copyright 2021 gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#include "datechooser.h"
#include "theme.h"
#include "util.h"
#include <FL/fl_show_colormap.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Color_Chooser.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_File_Chooser.H>
#include <FL/Fl_Hold_Browser.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Light_Button.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Menu_Button.H>
#include <FL/Fl_Roller.H>
#include <FL/Fl_Round_Button.H>
#include <FL/Fl_Slider.H>
#include <FL/Fl_Tree.H>

using namespace flw;

class Scale : public Fl_Box {
public:
    Scale() : Fl_Box(0, 0, 0, 0) {
        box(FL_BORDER_BOX);
    }

    void draw() override {
        Fl_Box::draw();

        auto H = (double) (h() - 2) / 24.0;
        auto Y = y() + 1;

        for (auto f = 0; f < 24; f++) {
            fl_rectf(x() + 1, Y, w() - 2, (int) H, 32 + f);
            Y += H;
        }
    }
};

class Test : public Fl_Double_Window {
public:
    static Test* TEST;
    Fl_Button*           bg2;
    Fl_Button*           bg;
    Fl_Button*           dark1;
    Fl_Button*           dark2;
    Fl_Button*           dark3;
    Fl_Button*           disabled;
    Fl_Button*           fg;
    Fl_Button*           gray0;
    Fl_Button*           inactive;
    Fl_Button*           light1;
    Fl_Button*           light2;
    Fl_Button*           light3;
    Fl_Button*           selection;
    Fl_Button*           theme;
    Fl_Button*           colormap;

    Fl_Button*           black;
    Fl_Button*           dark_green;
    Fl_Button*           green;
    Fl_Button*           dark_red;
    Fl_Button*           dark_yellow;
    Fl_Button*           red;
    Fl_Button*           yellow;
    Fl_Button*           dark_blue;
    Fl_Button*           dark_cyan;
    Fl_Button*           dark_magenta;
    Fl_Button*           blue;
    Fl_Button*           cyan;
    Fl_Button*           magenta;
    Fl_Button*           white;

    DateChooser*         date;
    Fl_Hold_Browser*     browser;
    Fl_Input*            input;
    Fl_Menu_Bar*         menu;
    Fl_Light_Button*     light;
    Fl_Check_Button*     check;
    Fl_Round_Button*     radio;
    Fl_Slider*           slider;
    Fl_Roller*           roller;
    Fl_Tree*             tree;
    Scale*               scale;

    Test() : Fl_Double_Window(1000, 700, "test_theme.cpp") {
        TEST         = this;

        check        = new Fl_Check_Button(0, 0, 0, 0, "Fl_Check_Button");
        colormap     = new Fl_Button(0, 0, 0, 0, "Color Map");
        date         = new DateChooser(0, 0, 0, 0);
        disabled     = new Fl_Button(0, 0, 0, 0, "deactivated");
        light        = new Fl_Light_Button(0, 0, 0, 0, "File Chooser");
        menu         = new Fl_Menu_Bar(0, 0, 0, 0);
        radio        = new Fl_Round_Button(0, 0, 0, 0, "Fl_Round_Button");
        roller       = new Fl_Roller(0, 0, 0, 0, "");
        slider       = new Fl_Slider(0, 0, 0, 0);
        theme        = new Fl_Button(0, 0, 0, 0, "Theme");

        browser      = new Fl_Hold_Browser(0, 0, 0, 0);
        input        = new Fl_Input(0, 0, 0, 0);
        tree         = new Fl_Tree(0, 0, 0, 0);
        scale        = new Scale();

        bg           = new Fl_Button(0, 0, 0, 0, "FL_BACKGROUND_COLOR (49)");
        bg2          = new Fl_Button(0, 0, 0, 0, "FL_BACKGROUND2_COLOR (7)");
        dark1        = new Fl_Button(0, 0, 0, 0, "FL_DARK1 (47)");
        dark2        = new Fl_Button(0, 0, 0, 0, "FL_DARK2 (45)");
        dark3        = new Fl_Button(0, 0, 0, 0, "FL_DARK3 (39)");
        fg           = new Fl_Button(0, 0, 0, 0, "FL_FOREGROUND_COLOR (0)");
        gray0        = new Fl_Button(0, 0, 0, 0, "FL_GRAY0 (32)");
        inactive     = new Fl_Button(0, 0, 0, 0, "FL_INACTIVE_COLOR (8)");
        light1       = new Fl_Button(0, 0, 0, 0, "FL_LIGHT1 (50)");
        light2       = new Fl_Button(0, 0, 0, 0, "FL_LIGHT2 (52)");
        light3       = new Fl_Button(0, 0, 0, 0, "FL_LIGHT3 (54)");
        selection    = new Fl_Button(0, 0, 0, 0, "FL_SELECTION_COLOR (15)");

        black        = new Fl_Button(0, 0, 0, 0, "FL_BLACK (56)");
        dark_green   = new Fl_Button(0, 0, 0, 0, "FL_DARK_GREEN (60)");
        green        = new Fl_Button(0, 0, 0, 0, "FL_GREEN (63)");
        dark_red     = new Fl_Button(0, 0, 0, 0, "FL_DARK_RED (72)");
        dark_yellow  = new Fl_Button(0, 0, 0, 0, "FL_DARK_YELLOW (76)");
        red          = new Fl_Button(0, 0, 0, 0, "FL_RED (88)");
        yellow       = new Fl_Button(0, 0, 0, 0, "FL_YELLOW (95)");
        dark_blue    = new Fl_Button(0, 0, 0, 0, "FL_DARK_BLUE (36)");
        dark_cyan    = new Fl_Button(0, 0, 0, 0, "FL_DARK_CYAN (40)");
        dark_magenta = new Fl_Button(0, 0, 0, 0, "FL_DARK_MAGENTA (52)");
        blue         = new Fl_Button(0, 0, 0, 0, "FL_BLUE (16)");
        cyan         = new Fl_Button(0, 0, 0, 0, "FL_CYAN (23)");
        magenta      = new Fl_Button(0, 0, 0, 0, "FL_MAGENTA (48)");
        white        = new Fl_Button(0, 0, 0, 0, "FL_WHITE (55)");

        add(menu);
        add(theme);
        add(disabled);
        add(radio);
        add(check);
        add(light);
        add(slider);
        add(roller);
        add(input);
        add(browser);
        add(date);

        add(gray0);
        add(dark1);
        add(dark2);
        add(dark3);
        add(light1);
        add(light2);
        add(light3);
        add(fg);
        add(bg);
        add(bg2);
        add(inactive);
        add(selection);

        add(black);
        add(dark_green);
        add(green);
        add(dark_red);
        add(dark_yellow);
        add(red);
        add(yellow);
        add(dark_blue);
        add(dark_cyan);
        add(dark_magenta);
        add(blue);
        add(cyan);
        add(magenta);
        add(white);

        button(bg, FL_BACKGROUND_COLOR);
        button(bg2, FL_BACKGROUND2_COLOR);
        button(dark1, FL_DARK1);
        button(dark2, FL_DARK2);
        button(dark3, FL_DARK3);
        button(fg, FL_FOREGROUND_COLOR);
        button(gray0, FL_GRAY0);
        button(inactive, FL_INACTIVE_COLOR);
        button(light1, FL_LIGHT1);
        button(light2, FL_LIGHT2);
        button(light3, FL_LIGHT3);
        button(selection, FL_SELECTION_COLOR);

        button(black, FL_BLACK);
        button(dark_green, FL_DARK_GREEN);
        button(green, FL_GREEN);
        button(dark_red, FL_DARK_RED);
        button(dark_yellow, FL_DARK_YELLOW);
        button(red, FL_RED);
        button(yellow, FL_YELLOW);
        button(dark_blue, FL_DARK_BLUE);
        button(dark_cyan, FL_DARK_CYAN);
        button(dark_magenta, FL_DARK_MAGENTA);
        button(blue, FL_BLUE);
        button(cyan, FL_CYAN);
        button(magenta, FL_MAGENTA);
        button(white, FL_WHITE);

        colormap->callback(CallbackColorMap);
        disabled->deactivate();
        input->value("default");
        slider->type(4);
        theme->callback(CallbackTheme);
        browser->add("FL_BACKGROUND2_COLOR");
        browser->add("FL_BACKGROUND_COLOR");
        browser->add("FL_BLACK");
        browser->add("FL_FOREGROUND_COLOR");
        browser->add("FL_INACTIVE_COLOR");
        browser->add("FL_SELECTION_COLOR");
        browser->add("FL_WHITE");
        light->callback(CallbackFile);
        menu->add("File/Open");
        menu->add("File/Save");
        menu->add("Edit/Copy");
        menu->add("Edit/Cut");
        menu->add("Edit/Paste");
        tree->add("Widget Colors/FL_BACKGROUND_COLOR");
        tree->add("Widget Colors/FL_BACKGROUND2_COLOR");
        tree->add("Widget Colors/FL_DARK1");
        tree->add("Widget Colors/FL_DARK2");
        tree->add("Widget Colors/FL_DARK3");
        tree->add("Widget Colors/FL_FOREGROUND_COLOR");
        tree->add("Widget Colors/FL_GRAY0");
        tree->add("Widget Colors/FL_INACTIVE_COLOR");
        tree->add("Widget Colors/FL_LIGHT1");
        tree->add("Widget Colors/FL_LIGHT2");
        tree->add("Widget Colors/FL_LIGHT3");
        tree->add("Widget Colors/FL_SELECTION_COLOR");
        tree->add("Named COlors/FL_BLACK");
        tree->add("Named COlors/FL_DARK_GREEN");
        tree->add("Named COlors/FL_GREEN");
        tree->add("Named COlors/FL_DARK_RED");
        tree->add("Named COlors/FL_DARK_YELLOW");
        tree->add("Named COlors/FL_RED");
        tree->add("Named COlors/FL_YELLOW");
        tree->add("Named COlors/FL_DARK_BLUE");
        tree->add("Named COlors/FL_DARK_CYAN");
        tree->add("Named COlors/FL_DARK_MAGENTA");
        tree->add("Named COlors/FL_BLUE");
        tree->add("Named COlors/FL_CYAN");
        tree->add("Named COlors/FL_MAGENTA");
        tree->add("Named COlors/FL_WHITE");

        resizable(this);
    }

    static void CallbackButton(Fl_Widget* w, void*) {
        TEST->color(w, (size_t) w->user_data());
    }

    static void CallbackColorMap(Fl_Widget*, void*) {
        auto c = fl_show_colormap(0);
        auto r = (unsigned char) 0;
        auto g = (unsigned char) 0;
        auto b = (unsigned char) 0;
        char buf[200];

        sprintf(buf, "Choose color for %d", c);
        Fl::get_color(c, r, g, b);

        if (fl_color_chooser(buf, r, g, b, 1)) {
            Fl::set_color(c,r, g, b);
            fflush(stdout);
            Fl::redraw();
            printf("color %3d was set to %3d, %3d, %3d\n", c, r, g, b);
        }
    }

    static void CallbackFile(Fl_Widget*, void*) {
        fl_file_chooser("fl_file_chooser", nullptr, nullptr, 0);
    }

    static void CallbackTheme(Fl_Widget*, void*) {
        flw::dlg::theme(true, true, TEST);
        TEST->update_pref();
        TEST->input->value(flw::theme::name().c_str());
    }

    void button(Fl_Button* button, Fl_Color color) {
        button->align(FL_ALIGN_LEFT | FL_ALIGN_TOP);
        button->box(FL_BORDER_BOX);
        button->color(color);
        button->callback(CallbackButton);
        button->user_data((void*) (size_t) color);
    }

    void color(Fl_Widget* button, Fl_Color color) {
        auto r = (unsigned char) 0;
        auto g = (unsigned char) 0;
        auto b = (unsigned char) 0;
        char buf[200];

        Fl::get_color((int) color, r, g, b);
        sprintf(buf, "Choose color for %d", color);

        if (fl_color_chooser(buf, r, g, b, 1)) {
            sprintf(buf, "color %d, r=%d, g=%d, b=%d", (int) color, r, g, b);
            button->copy_label(buf);
            Fl::set_color(color, r, g, b);
            Fl::redraw();
        };
    }

    void resize(int X, int Y, int W, int H) override {
        auto fs = flw::PREF_FONTSIZE;

        Fl_Double_Window::resize(X, Y, W, H);

        menu->resize(0, 0, W, fs * 2);

        theme->resize   (4, fs * 2 + 4, fs * 10, fs * 2);
        colormap->resize(4, fs * 4 + 8, fs * 10, fs * 2);
        disabled->resize(4, fs * 6 + 12, fs * 10, fs * 2);
        radio->resize   (4, fs * 8 + 16, fs * 10, fs * 2);
        check->resize   (4, fs * 10 + 20, fs * 10, fs * 2);
        light->resize   (4, fs * 12 + 24, fs * 10, fs * 2);
        slider->resize  (4, fs * 14 + 28, fs * 2, fs * 12);
        roller->resize  (fs * 2 + 8, fs * 14 + 28, fs * 2, fs * 12);
        scale->resize   (4, fs * 26 + 34, fs * 10, H - fs * 26 - 40);

        input->resize  (fs * 10 + 8, fs * 2 + 4, fs * 24, fs * 2);
        browser->resize(fs * 10 + 8, fs * 4 + 8, fs * 24, fs * 10);
        date->resize   (fs * 10 + 8, fs * 14 + 12, fs * 24, fs * 20);
        tree->resize   (fs * 10 + 8, fs * 34 + 16, fs * 24, H - fs * 34 - 20);

        fg->resize       (W - fs * 32 - 8, fs * 3 + 4,   fs * 16, fs * 2);
        bg->resize       (W - fs * 32 - 8, fs * 6 + 8,   fs * 16, fs * 2);
        bg2->resize      (W - fs * 32 - 8, fs * 9 + 12,  fs * 16, fs * 2);
        inactive->resize (W - fs * 32 - 8, fs * 12 + 16, fs * 16, fs * 2);
        selection->resize(W - fs * 32 - 8, fs * 15 + 20, fs * 16, fs * 2);
        gray0->resize    (W - fs * 32 - 8, fs * 18 + 24, fs * 16, fs * 2);
        dark3->resize    (W - fs * 32 - 8, fs * 21 + 28, fs * 16, fs * 2);
        dark2->resize    (W - fs * 32 - 8, fs * 24 + 32, fs * 16, fs * 2);
        dark1->resize    (W - fs * 32 - 8, fs * 27 + 36, fs * 16, fs * 2);
        light1->resize   (W - fs * 32 - 8, fs * 30 + 40, fs * 16, fs * 2);
        light2->resize   (W - fs * 32 - 8, fs * 33 + 44, fs * 16, fs * 2);
        light3->resize   (W - fs * 32 - 8, fs * 36 + 48, fs * 16, fs * 2);

        black->resize        (W - fs * 16 - 4, fs * 3 + 4,   fs * 16, fs * 2);
        dark_green->resize   (W - fs * 16 - 4, fs * 6 + 8,   fs * 16, fs * 2);
        green->resize        (W - fs * 16 - 4, fs * 9 + 12,  fs * 16, fs * 2);
        dark_red->resize     (W - fs * 16 - 4, fs * 12 + 16, fs * 16, fs * 2);
        dark_yellow->resize  (W - fs * 16 - 4, fs * 15 + 20, fs * 16, fs * 2);
        red->resize          (W - fs * 16 - 4, fs * 18 + 24, fs * 16, fs * 2);
        yellow->resize       (W - fs * 16 - 4, fs * 21 + 28, fs * 16, fs * 2);
        dark_blue->resize    (W - fs * 16 - 4, fs * 24 + 32, fs * 16, fs * 2);
        dark_cyan->resize    (W - fs * 16 - 4, fs * 27 + 36, fs * 16, fs * 2);
        dark_magenta->resize (W - fs * 16 - 4, fs * 30 + 40, fs * 16, fs * 2);
        blue->resize         (W - fs * 16 - 4, fs * 33 + 44, fs * 16, fs * 2);
        cyan->resize         (W - fs * 16 - 4, fs * 36 + 48, fs * 16, fs * 2);
        magenta->resize      (W - fs * 16 - 4, fs * 39 + 52, fs * 16, fs * 2);
        white->resize        (W - fs * 16 - 4, fs * 42 + 56, fs * 16, fs * 2);

        Fl::redraw();
    }

    void update_pref() {
        flw::util::labelfont(this);
        input->textfont(flw::PREF_FONT);
        input->textsize(flw::PREF_FONTSIZE);
        browser->textfont(flw::PREF_FONT);
        browser->textsize(flw::PREF_FONTSIZE);
        size(w(), h());
        Fl::redraw();
    }
};

Test* Test::TEST = nullptr;

int main(int argc, const char** argv) {
    flw::theme::parse(argc, argv);

    Test win;
    win.show();
    flw::util::center_window(&win);
    return Fl::run();
}
