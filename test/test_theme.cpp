// Copyright 2021 - 2022 gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#include "datechooser.h"
#include "theme.h"
#include "util.h"
#include "icon.xpm"
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
    Fl_Button*           color_red;
    Fl_Button*           color_lime;
    Fl_Button*           color_yellow;
    Fl_Button*           color_cyan;
    Fl_Button*           color_magenta;
    Fl_Button*           color_maroon;
    Fl_Button*           color_olive;
    Fl_Button*           color_green;
    Fl_Button*           color_purple;
    Fl_Button*           color_teal;
    Fl_Button*           color_navy;
    Fl_Button*           color_brown;
    Fl_Button*           color_pink;
    Fl_Button*           color_beige;
    Fl_Button*           color_azure;
    Fl_Button*           color_blue;

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

    Test() : Fl_Double_Window(0, 0, "test_theme.cpp") {
        TEST = this;
        resizable(this);
        size_range(64, 48);
        menu = nullptr;
    }

    void create() {
        begin();

        check        = new Fl_Check_Button(0, 0, 0, 0, "Fl_Check_Button");
        colormap     = new Fl_Button(0, 0, 0, 0, "Color Map");
        date         = new DateChooser(0, 0, 0, 0);
        disabled     = new Fl_Button(0, 0, 0, 0, "Deactivated");
        light        = new Fl_Light_Button(0, 0, 0, 0, "File Chooser");
        menu         = new Fl_Menu_Bar(0, 0, 0, 0);
        radio        = new Fl_Round_Button(0, 0, 0, 0, "Fl_Round_Button");
        roller       = new Fl_Roller(0, 0, 0, 0, "");
        slider       = new Fl_Slider(0, 0, 0, 0);
        theme        = new Fl_Button(0, 0, 0, 0, "&Theme");

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

        color_red     = new Fl_Button(0, 0, 0, 0, "color::RED");
        color_lime    = new Fl_Button(0, 0, 0, 0, "color::LIME");
        color_yellow  = new Fl_Button(0, 0, 0, 0, "color::YELLOW");
        color_cyan    = new Fl_Button(0, 0, 0, 0, "color::CYAN");
        color_magenta = new Fl_Button(0, 0, 0, 0, "color::MAGENTA");
        color_maroon  = new Fl_Button(0, 0, 0, 0, "color::MARRON");
        color_olive   = new Fl_Button(0, 0, 0, 0, "color::OLIVE");
        color_green   = new Fl_Button(0, 0, 0, 0, "color::GREEN");
        color_purple  = new Fl_Button(0, 0, 0, 0, "color::PURPLE");
        color_teal    = new Fl_Button(0, 0, 0, 0, "color::TEAL");
        color_navy    = new Fl_Button(0, 0, 0, 0, "color::NAVY");
        color_brown   = new Fl_Button(0, 0, 0, 0, "color::BROWN");
        color_pink    = new Fl_Button(0, 0, 0, 0, "color::PINK");
        color_beige   = new Fl_Button(0, 0, 0, 0, "color::BEIGE");
        color_azure   = new Fl_Button(0, 0, 0, 0, "color::AZURE");
        color_blue    = new Fl_Button(0, 0, 0, 0, "color::BLUE");

        end();

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

        button2(color_red, color::RED);
        button2(color_lime, color::LIME);
        button2(color_yellow, color::YELLOW);
        button2(color_cyan, color::CYAN);
        button2(color_magenta, color::MAGENTA);
        button2(color_maroon, color::MAROON);
        button2(color_olive, color::OLIVE);
        button2(color_green, color::GREEN);
        button2(color_purple, color::PURPLE);
        button2(color_teal, color::TEAL);
        button2(color_navy, color::NAVY);
        button2(color_brown, color::BROWN);
        button2(color_pink, color::PINK);
        button2(color_beige, color::BEIGE);
        button2(color_azure, color::AZURE);
        button2(color_blue, color:: BLUE);

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
        menu->add("Test/Toggle on", 0, nullptr, nullptr, FL_MENU_TOGGLE);
        menu->add("Test/Toggle off", 0, nullptr, nullptr, FL_MENU_TOGGLE);
        menu->add("Test/Enabled");
        menu->add("Test/Disabled");

        util::menu_item_set(menu, "Test/Toggle on", true);
        util::menu_item_set(menu, "Test/Toggle off", true);
        util::menu_item_set(menu, "Test/Toggle off", false);
        util::menu_item_enable(menu, "Test/Enabled", false);
        util::menu_item_enable(menu, "Test/Disabled", false);
        util::menu_item_enable(menu, "Test/Enabled", true);
    }

    static void CallbackButton(Fl_Widget* w, void*) {
        TEST->color(w, (size_t) w->user_data());
    }

    static void CallbackButton2(Fl_Widget* w, void*) {
        TEST->color2(w, w->user_data());
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
        TEST->clear();
        TEST->create();
        TEST->update_pref();
        TEST->input->value(flw::theme::name().c_str());
        TEST->button2(TEST->color_red, color::RED);
        TEST->button2(TEST->color_lime, color::LIME);
        TEST->button2(TEST->color_yellow, color::YELLOW);
        TEST->button2(TEST->color_cyan, color::CYAN);
        TEST->button2(TEST->color_magenta, color::MAGENTA);
        TEST->button2(TEST->color_maroon, color::MAROON);
        TEST->button2(TEST->color_olive, color::OLIVE);
        TEST->button2(TEST->color_green, color::GREEN);
        TEST->button2(TEST->color_purple, color::PURPLE);
        TEST->button2(TEST->color_teal, color::TEAL);
        TEST->button2(TEST->color_navy, color::NAVY);
        TEST->button2(TEST->color_brown, color::BROWN);
        TEST->button2(TEST->color_pink, color::PINK);
        TEST->button2(TEST->color_beige, color::BEIGE);
        TEST->button2(TEST->color_azure, color::AZURE);
        TEST->button2(TEST->color_blue, color::BLUE);
    }

    void button(Fl_Button* button, Fl_Color color) {
        button->align(FL_ALIGN_LEFT | FL_ALIGN_TOP);
        button->box(FL_BORDER_BOX);
        button->color(color);
        button->callback(CallbackButton);
        button->user_data((void*) (size_t) color);
    }

    void button2(Fl_Button* button, Fl_Color& color) {
        button->align(FL_ALIGN_LEFT | FL_ALIGN_TOP);
        button->box(FL_BORDER_BOX);
        button->color(color);
        button->callback(CallbackButton2);
        button->user_data(&color);
    }

    void color(Fl_Widget* button, Fl_Color color) {
        auto r = (unsigned char) 0;
        auto g = (unsigned char) 0;
        auto b = (unsigned char) 0;
        char buf[200];

        Fl::get_color((int) color, r, g, b);
        sprintf(buf, "Fl_Color=%d", color);

        if (fl_color_chooser(buf, r, g, b, 1)) {
            sprintf(buf, "color %d, r=%d, g=%d, b=%d", (int) color, r, g, b);
            button->copy_label(buf);
            Fl::set_color(color, r, g, b);
            Fl::redraw();
        };
    }

    void color2(Fl_Widget* button, void* color) {
        auto r = (unsigned char) 0;
        auto g = (unsigned char) 0;
        auto b = (unsigned char) 0;
        auto c = (Fl_Color*) color;
        char buf[200];

        FLW_PRINT(color, c)
        Fl::get_color(*c, r, g, b);

        if (fl_color_chooser("Select", r, g, b, 1)) {
            sprintf(buf, "r=%d, g=%d, b=%d", r, g, b);
            *c = fl_rgb_color(r, g, b);
            button->copy_label(buf);
            button->color(*c);
            Fl::redraw();
        };
    }

    void make_tree() {
        tree->clear();
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
        tree->add("Named Colors/FL_BLACK");
        tree->add("Named Colors/FL_DARK_GREEN");
        tree->add("Named Colors/FL_GREEN");
        tree->add("Named Colors/FL_DARK_RED");
        tree->add("Named Colors/FL_DARK_YELLOW");
        tree->add("Named Colors/FL_RED");
        tree->add("Named Colors/FL_YELLOW");
        tree->add("Named Colors/FL_DARK_BLUE");
        tree->add("Named Colors/FL_DARK_CYAN");
        tree->add("Named Colors/FL_DARK_MAGENTA");
        tree->add("Named Colors/FL_BLUE");
        tree->add("Named Colors/FL_CYAN");
        tree->add("Named Colors/FL_MAGENTA");
        tree->add("Named Colors/FL_WHITE");
    }

    void resize(int X, int Y, int W, int H) override {
        auto fs = flw::PREF_FONTSIZE;

        Fl_Double_Window::resize(X, Y, W, H);

        if (menu == nullptr) {
            return;
        }

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

        fg->resize       (W - fs * 48 - 8, fs * 3 + 4,   fs * 15, fs * 2);
        bg->resize       (W - fs * 48 - 8, fs * 6 + 8,   fs * 15, fs * 2);
        bg2->resize      (W - fs * 48 - 8, fs * 9 + 12,  fs * 15, fs * 2);
        inactive->resize (W - fs * 48 - 8, fs * 12 + 16, fs * 15, fs * 2);
        selection->resize(W - fs * 48 - 8, fs * 15 + 20, fs * 15, fs * 2);
        gray0->resize    (W - fs * 48 - 8, fs * 18 + 24, fs * 15, fs * 2);
        dark3->resize    (W - fs * 48 - 8, fs * 21 + 28, fs * 15, fs * 2);
        dark2->resize    (W - fs * 48 - 8, fs * 24 + 32, fs * 15, fs * 2);
        dark1->resize    (W - fs * 48 - 8, fs * 27 + 36, fs * 15, fs * 2);
        light1->resize   (W - fs * 48 - 8, fs * 30 + 40, fs * 15, fs * 2);
        light2->resize   (W - fs * 48 - 8, fs * 33 + 44, fs * 15, fs * 2);
        light3->resize   (W - fs * 48 - 8, fs * 36 + 48, fs * 15, fs * 2);

        black->resize        (W - fs * 32 - 4, fs * 3 + 4,   fs * 15, fs * 2);
        dark_green->resize   (W - fs * 32 - 4, fs * 6 + 8,   fs * 15, fs * 2);
        green->resize        (W - fs * 32 - 4, fs * 9 + 12,  fs * 15, fs * 2);
        dark_red->resize     (W - fs * 32 - 4, fs * 12 + 16, fs * 15, fs * 2);
        dark_yellow->resize  (W - fs * 32 - 4, fs * 15 + 20, fs * 15, fs * 2);
        red->resize          (W - fs * 32 - 4, fs * 18 + 24, fs * 15, fs * 2);
        yellow->resize       (W - fs * 32 - 4, fs * 21 + 28, fs * 15, fs * 2);
        dark_blue->resize    (W - fs * 32 - 4, fs * 24 + 32, fs * 15, fs * 2);
        dark_cyan->resize    (W - fs * 32 - 4, fs * 27 + 36, fs * 15, fs * 2);
        dark_magenta->resize (W - fs * 32 - 4, fs * 30 + 40, fs * 15, fs * 2);
        blue->resize         (W - fs * 32 - 4, fs * 33 + 44, fs * 15, fs * 2);
        cyan->resize         (W - fs * 32 - 4, fs * 36 + 48, fs * 15, fs * 2);
        magenta->resize      (W - fs * 32 - 4, fs * 39 + 52, fs * 15, fs * 2);
        white->resize        (W - fs * 32 - 4, fs * 42 + 56, fs * 15, fs * 2);

        auto y = fs * 3 + 4;
        color_azure->resize   (W - fs * 16 - 4, y, fs * 15, fs * 2); y += fs * 3 + 4;
        color_beige->resize   (W - fs * 16 - 4, y, fs * 15, fs * 2); y += fs * 3 + 4;
        color_blue->resize    (W - fs * 16 - 4, y, fs * 15, fs * 2); y += fs * 3 + 4;
        color_brown->resize   (W - fs * 16 - 4, y, fs * 15, fs * 2); y += fs * 3 + 4;
        color_cyan->resize    (W - fs * 16 - 4, y, fs * 15, fs * 2); y += fs * 3 + 4;
        color_green->resize   (W - fs * 16 - 4, y, fs * 15, fs * 2); y += fs * 3 + 4;
        color_lime->resize    (W - fs * 16 - 4, y, fs * 15, fs * 2); y += fs * 3 + 4;
        color_magenta->resize (W - fs * 16 - 4, y, fs * 15, fs * 2); y += fs * 3 + 4;
        color_maroon->resize  (W - fs * 16 - 4, y, fs * 15, fs * 2); y += fs * 3 + 4;
        color_navy->resize    (W - fs * 16 - 4, y, fs * 15, fs * 2); y += fs * 3 + 4;
        color_olive->resize   (W - fs * 16 - 4, y, fs * 15, fs * 2); y += fs * 3 + 4;
        color_pink->resize    (W - fs * 16 - 4, y, fs * 15, fs * 2); y += fs * 3 + 4;
        color_purple->resize  (W - fs * 16 - 4, y, fs * 15, fs * 2); y += fs * 3 + 4;
        color_red->resize     (W - fs * 16 - 4, y, fs * 15, fs * 2); y += fs * 3 + 4;
        color_teal->resize    (W - fs * 16 - 4, y, fs * 15, fs * 2); y += fs * 3 + 4;
        color_yellow->resize  (W - fs * 16 - 4, y, fs * 15, fs * 2); y += fs * 3 + 4;
    }

    void update_pref() {
        flw::theme::labelfont(this);
        input->textfont(flw::PREF_FONT);
        input->textsize(flw::PREF_FONTSIZE);
        browser->textfont(flw::PREF_FONT);
        browser->textsize(flw::PREF_FONTSIZE);
        menu->textsize(flw::PREF_FONTSIZE);
        tree->item_labelsize(flw::PREF_FONTSIZE);
        make_tree();
        size(w(), h());
        Fl::redraw();
    }
};

Test* Test::TEST = nullptr;

int main(int argc, const char** argv) {
    Test win;

    auto pref = Fl_Preferences(Fl_Preferences::USER, "gnuwimp", "test_theme");
    flw::theme::parse(argc, argv);
    flw::theme::load_icon(&win, 666, icon_xpm, "Test Theme");
    flw::theme::load_theme_pref(pref);
    flw::theme::load_win_pref(pref, &win);
    win.create();
    win.update_pref();
    Fl::run();
    pref.clear();
    flw::theme::save_theme_pref(pref);
    flw::theme::save_win_pref(pref, &win);

    return 0;
}
