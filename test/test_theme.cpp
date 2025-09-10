// Copyright gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#include "flw.h"

FL_EXPORT bool fl_disable_wayland = false;

#ifndef FLW_AMALGAM
    #include "datechooser.h"
    #include "dlg.h"
    #include "gridgroup.h"
#endif

#include "icon.xpm"
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Color_Chooser.H>
#include <FL/Fl_Dial.H>
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
#include <FL/fl_show_colormap.H>

using namespace flw;

class Scale : public Fl_Box {
public:
    bool labels = false;
    
    Scale() : Fl_Box(0, 0, 0, 0) {
        box(FL_BORDER_BOX);
    }

    void draw() override {
        Fl_Box::draw();

        auto H = (double) (h() - 2) / 24.0;
        auto Y = y() + 1;

        for (auto f = 0; f < 24; f++) {
            char t[30];
            unsigned char r, g, b;
            Fl::get_color(32 + f, r, g, b);
            snprintf(t, 30, "%d: r=%u, g=%u, b=%u", 32 + f, r, g, b);
            fl_rectf(x() + 1, Y, w() - 2, (int) H, 32 + f);
            fl_color(FL_RED);
            if (labels) fl_draw(t, x(), Y, w(), H, FL_ALIGN_CENTER);
            Y += H;
        }
    }
};

class Test : public Fl_Double_Window {
public:
    static Test*         TEST;
    DateChooser*         date;
    Fl_Button*           bg2;
    Fl_Button*           bg;
    Fl_Button*           colormap;
    Fl_Button*           dark1;
    Fl_Button*           dark2;
    Fl_Button*           dark3;
    Fl_Button*           disabled;
    Fl_Button*           fg;
    Fl_Button*           file;
    Fl_Button*           full;
    Fl_Button*           gray0;
    Fl_Button*           inactive;
    Fl_Button*           light1;
    Fl_Button*           light2;
    Fl_Button*           light3;
    Fl_Button*           selection;
    Fl_Button*           theme;
    Fl_Check_Button*     check;
    Fl_Dial*             dial;
    Fl_Hold_Browser*     browser;
    Fl_Input*            input;
    Fl_Light_Button*     labels;
    Fl_Menu_Bar*         menu;
    Fl_Roller*           roller;
    Fl_Round_Button*     radio;
    Fl_Slider*           slider;
    Fl_Tree*             tree;
    GridGroup*           grid0;
    GridGroup*           grid1;
    GridGroup*           grid2;
    GridGroup*           grid3;
    Scale*               scale;

    Fl_Button*           color_beige;
    Fl_Button*           color_choc;
    Fl_Button*           color_crim;
    Fl_Button*           color_darkog;
    Fl_Button*           color_dodger;
    Fl_Button*           color_forest;
    Fl_Button*           color_gold;
    Fl_Button*           color_gray;
    Fl_Button*           color_indigo;
    Fl_Button*           color_olive;
    Fl_Button*           color_pink;
    Fl_Button*           color_royal;
    Fl_Button*           color_sienna;
    Fl_Button*           color_silver;
    Fl_Button*           color_slate;
    Fl_Button*           color_teal;
    Fl_Button*           color_turq;
    Fl_Button*           color_violet;
    Fl_Button*           def_black;
    Fl_Button*           def_blue;
    Fl_Button*           def_cyan;
    Fl_Button*           def_dark_blue;
    Fl_Button*           def_dark_cyan;
    Fl_Button*           def_dark_green;
    Fl_Button*           def_dark_magenta;
    Fl_Button*           def_dark_red;
    Fl_Button*           def_dark_yellow;
    Fl_Button*           def_green;
    Fl_Button*           def_magenta;
    Fl_Button*           def_red;
    Fl_Button*           def_white;
    Fl_Button*           def_yellow;

    Test() : Fl_Double_Window(0, 0, "test_theme.cpp") {
        TEST = this;
        resizable(this);
        size_range(64, 48);
        menu = nullptr;
    }

    void create() {
        browser      = new Fl_Hold_Browser(0, 0, 0, 0);
        check        = new Fl_Check_Button(0, 0, 0, 0, "Fl_Check_Button");
        colormap     = new Fl_Button(0, 0, 0, 0, "Color Map");
        date         = new DateChooser(0, 0, 0, 0);
        dial         = new Fl_Dial(0, 0, 0, 0);
        disabled     = new Fl_Button(0, 0, 0, 0, "Deactivated");
        file         = new Fl_Button(0, 0, 0, 0, "File chooser");
        full         = new Fl_Button(0, 0, 0, 0, TEST->fullscreen_active() ? "Fullscreen Off" : "Fullscreen");
        grid0        = new GridGroup();
        grid1        = new GridGroup();
        grid2        = new GridGroup();
        grid3        = new GridGroup();
        input        = new Fl_Input(0, 0, 0, 0);
        labels       = new Fl_Light_Button(0, 0, 0, 0, "Scale labels");
        menu         = new Fl_Menu_Bar(0, 0, 0, 0);
        radio        = new Fl_Round_Button(0, 0, 0, 0, "Fl_Round_Button");
        roller       = new Fl_Roller(0, 0, 0, 0, "");
        scale        = new Scale();
        slider       = new Fl_Slider(0, 0, 0, 0);
        theme        = new Fl_Button(0, 0, 0, 0, "&Theme");
        tree         = new Fl_Tree(0, 0, 0, 0);
    
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

        def_black        = new Fl_Button(0, 0, 0, 0, "FL_BLACK (56)");
        def_blue         = new Fl_Button(0, 0, 0, 0, "FL_BLUE (16)");
        def_cyan         = new Fl_Button(0, 0, 0, 0, "FL_CYAN (23)");
        def_dark_blue    = new Fl_Button(0, 0, 0, 0, "FL_DARK_BLUE (36)");
        def_dark_cyan    = new Fl_Button(0, 0, 0, 0, "FL_DARK_CYAN (40)");
        def_dark_green   = new Fl_Button(0, 0, 0, 0, "FL_DARK_GREEN (60)");
        def_dark_magenta = new Fl_Button(0, 0, 0, 0, "FL_DARK_MAGENTA (52)");
        def_dark_red     = new Fl_Button(0, 0, 0, 0, "FL_DARK_RED (72)");
        def_dark_yellow  = new Fl_Button(0, 0, 0, 0, "FL_DARK_YELLOW (76)");
        def_green        = new Fl_Button(0, 0, 0, 0, "FL_GREEN (63)");
        def_magenta      = new Fl_Button(0, 0, 0, 0, "FL_MAGENTA (48)");
        def_red          = new Fl_Button(0, 0, 0, 0, "FL_RED (88)");
        def_white        = new Fl_Button(0, 0, 0, 0, "FL_WHITE (55)");
        def_yellow       = new Fl_Button(0, 0, 0, 0, "FL_YELLOW (95)");

        color_beige  = new Fl_Button(0, 0, 0, 0, "color::BEIGE");
        color_choc   = new Fl_Button(0, 0, 0, 0, "color::CHOCOLATE");
        color_crim   = new Fl_Button(0, 0, 0, 0, "color::CRIMSON");
        color_darkog = new Fl_Button(0, 0, 0, 0, "color::DARKOLIVEGREEN");
        color_dodger = new Fl_Button(0, 0, 0, 0, "color::DODGERBLUE");
        color_forest = new Fl_Button(0, 0, 0, 0, "color::FORESTGREEN");
        color_gold   = new Fl_Button(0, 0, 0, 0, "color::GOLD");
        color_gray   = new Fl_Button(0, 0, 0, 0, "color::GRAY");
        color_indigo = new Fl_Button(0, 0, 0, 0, "color::INDIGO");
        color_olive  = new Fl_Button(0, 0, 0, 0, "color::OLIVE");
        color_pink   = new Fl_Button(0, 0, 0, 0, "color::PINK");
        color_royal  = new Fl_Button(0, 0, 0, 0, "color::ROYALBLUE");
        color_sienna = new Fl_Button(0, 0, 0, 0, "color::SIENNA");
        color_silver = new Fl_Button(0, 0, 0, 0, "color::SILVER");
        color_slate  = new Fl_Button(0, 0, 0, 0, "color::SLATEGRAY");
        color_teal   = new Fl_Button(0, 0, 0, 0, "color::TEAL");
        color_turq   = new Fl_Button(0, 0, 0, 0, "color::TURQUOISE");
        color_violet = new Fl_Button(0, 0, 0, 0, "color::VIOLET");

        add(menu);
        add(grid0);
        add(grid1);
        add(grid2);
        add(grid3);

        grid0->add(theme,           0,   1,  31,  4);
        grid0->add(colormap,        0,   6,  31,  4);
        grid0->add(full,            0,  11,  31,  4);
        grid0->add(disabled,        0,  16,  31,  4);
        grid0->add(radio,           0,  21,  31,  4);
        grid0->add(check,           0,  26,  31,  4);
        grid0->add(file,            0,  31,  31,  4);
        grid0->add(slider,          0,  36,   4, 20);
        grid0->add(roller,          5,  36,   4, 20);
        grid0->add(dial,           10,  36,  20, 20);
        grid0->add(labels,          0,  57,  31,  4);
        grid0->add(scale,           0,  62,  31, -1);
        
        grid0->add(input,          33,   1,  -1,  4);
        grid0->add(browser,        33,   6,  -1, 20);
        grid0->add(date,           33,  27,  -1, 48);
        grid0->add(tree,           33,  76,  -1, -1);

        grid1->add(bg,              0,   3,   0,  4);
        grid1->add(bg2,             0,  10,   0,  4);
        grid1->add(dark1,           0,  17,   0,  4);
        grid1->add(dark2,           0,  24,   0,  4);
        grid1->add(dark3,           0,  31,   0,  4);
        grid1->add(fg,              0,  38,   0,  4);
        grid1->add(gray0,           0,  45,   0 , 4);
        grid1->add(inactive,        0,  52,   0,  4);
        grid1->add(light1,          0,  59,   0,  4);
        grid1->add(light2,          0,  66,   0,  4);
        grid1->add(light3,          0,  73,   0,  4);
        grid1->add(selection,       0,  80,   0,  4);
        
        grid2->add(def_black,       0,   3,   0,  4);
        grid2->add(def_white,       0,  10,   0,  4);
        grid2->add(def_red,         0,  17,   0,  4);
        grid2->add(def_dark_red,    0,  24,   0,  4);
        grid2->add(def_green,       0,  31,   0,  4);
        grid2->add(def_dark_green,  0,  38,   0,  4);
        grid2->add(def_blue,        0,  45,   0,  4);
        grid2->add(def_dark_blue,   0,  52,   0,  4);
        grid2->add(def_yellow,      0,  59,   0,  4);
        grid2->add(def_dark_yellow, 0,  66,   0,  4);
        grid2->add(def_cyan,        0,  73,   0,  4);
        grid2->add(def_dark_cyan,   0,  80,   0,  4);
        grid2->add(def_magenta,     0,  87,   0,  4);
        grid2->add(def_dark_magenta,0,  94,   0,  4);

        grid3->add(color_beige,     0,   3,   0,  4);
        grid3->add(color_choc,      0,  10,   0,  4);
        grid3->add(color_crim,      0,  17,   0,  4);
        grid3->add(color_darkog,    0,  24,   0,  4);
        grid3->add(color_dodger,    0,  31,   0,  4);
        grid3->add(color_forest,    0,  38,   0,  4);
        grid3->add(color_gold,      0,  45,   0,  4);
        grid3->add(color_gray,      0,  52,   0,  4);
        grid3->add(color_indigo,    0,  59,   0,  4);
        grid3->add(color_olive,     0,  66,   0,  4);
        grid3->add(color_pink,      0,  73,   0,  4);
        grid3->add(color_royal,     0,  80,   0,  4);
        grid3->add(color_sienna,    0,  87,   0,  4);
        grid3->add(color_silver,    0,  94,   0,  4);
        grid3->add(color_slate,     0, 101,   0,  4);
        grid3->add(color_teal,      0, 108,   0,  4);
        grid3->add(color_turq,      0, 115,   0,  4);
        grid3->add(color_violet,    0, 122,   0,  4);
        
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

        button(def_black, FL_BLACK);
        button(def_dark_green, FL_DARK_GREEN);
        button(def_green, FL_GREEN);
        button(def_dark_red, FL_DARK_RED);
        button(def_dark_yellow, FL_DARK_YELLOW);
        button(def_red, FL_RED);
        button(def_yellow, FL_YELLOW);
        button(def_dark_blue, FL_DARK_BLUE);
        button(def_dark_cyan, FL_DARK_CYAN);
        button(def_dark_magenta, FL_DARK_MAGENTA);
        button(def_blue, FL_BLUE);
        button(def_cyan, FL_CYAN);
        button(def_magenta, FL_MAGENTA);
        button(def_white, FL_WHITE);

        button2(color_beige, color::BEIGE);
        button2(color_choc, color::CHOCOLATE);
        button2(color_crim, color::CRIMSON);
        button2(color_darkog, color::DARKOLIVEGREEN);
        button2(color_dodger, color::DODGERBLUE);
        button2(color_forest, color::FORESTGREEN);
        button2(color_gold, color::GOLD);
        button2(color_gray, color::GRAY);
        button2(color_indigo, color::INDIGO);
        button2(color_olive, color::OLIVE);
        button2(color_pink, color::PINK);
        button2(color_royal, color::ROYALBLUE);
        button2(color_sienna, color::SIENNA);
        button2(color_silver, color::SILVER);
        button2(color_slate, color::SLATEGRAY);
        button2(color_teal, color::TEAL);
        button2(color_turq, color::TURQUOISE);
        button2(color_violet, color::VIOLET);

        browser->add("FL_BACKGROUND2_COLOR");
        browser->add("FL_BACKGROUND_COLOR");
        browser->add("FL_BLACK");
        browser->add("FL_FOREGROUND_COLOR");
        browser->add("FL_INACTIVE_COLOR");
        browser->add("FL_SELECTION_COLOR");
        browser->add("FL_WHITE");
        colormap->callback(CallbackColorMap);
        dial->type(FL_FILL_DIAL);
        dial->value(0.5);
        disabled->deactivate();
        input->value(flw::PREF_THEME.c_str());
        file->callback(CallbackFile);
        full->callback(CallbackFullscreen);
        labels->callback(CallbackLabels, this);
        slider->type(4);
        theme->callback(CallbackTheme);
        theme->tooltip("Select a theme.");
        tree->connectorcolor(FL_FOREGROUND_COLOR);

        menu->add("File/Open");
        menu->add("File/Save");
        menu->add("Edit/Copy");
        menu->add("Edit/Cut");
        menu->add("Edit/Paste");
        menu->add("Test/Toggle on", 0, nullptr, nullptr, FL_MENU_TOGGLE);
        menu->add("Test/Toggle off", 0, nullptr, nullptr, FL_MENU_TOGGLE);
        menu->add("Test/Enabled");
        menu->add("Test/Disabled");
        menu::set_item(menu, "Test/Toggle on", true);
        menu::set_item(menu, "Test/Toggle off", true);
        menu::set_item(menu, "Test/Toggle off", false);
        menu::enable_item(menu, "Test/Enabled", false);
        menu::enable_item(menu, "Test/Disabled", false);
        menu::enable_item(menu, "Test/Enabled", true);
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

    static void CallbackFullscreen(Fl_Widget*, void*) {
        if (TEST->fullscreen_active()) {
            TEST->full->label("Fullscreen");
            TEST->fullscreen_off();
        }
        else {
            TEST->full->label("Fullscreen Off");
            TEST->fullscreen();
        }
    }

    static void CallbackLabels(Fl_Widget* w, void* o) {
        auto self = static_cast<Test*>(o);
        self->scale->labels = static_cast<Fl_Light_Button*>(w)->value();
        Fl::redraw();
    }

    static void CallbackTheme(Fl_Widget*, void*) {
        flw::dlg::theme(true, true, TEST);
        TEST->clear();
        TEST->create();
        TEST->update_pref();
        TEST->input->value(flw::PREF_THEME.c_str());
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

        Fl::get_color(*c, r, g, b);

        if (fl_color_chooser("Select Color", r, g, b, 1)) {
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
        Fl_Double_Window::resize(X, Y, W, H);
        
        if (menu != nullptr) {
            auto fs = flw::PREF_FONTSIZE;

            menu->resize    (0,        0,       W,        fs * 2);
            grid0->resize   (fs,       fs * 2,  fs * 55,  H - fs * 3);
            grid1->resize   (fs * 57,  fs * 2,  fs * 20,  H - fs * 3);
            grid2->resize   (fs * 80,  fs * 2,  fs * 20,  H - fs * 3);
            grid3->resize   (fs * 101,  fs * 2,  fs * 20,  H - fs * 3);
        }
    }

    void update_pref() {
        util::labelfont(this);
        input->textfont(flw::PREF_FONT);
        input->textsize(flw::PREF_FONTSIZE);
        browser->textfont(flw::PREF_FONT);
        browser->textsize(flw::PREF_FONTSIZE);
        menu->textsize(flw::PREF_FONTSIZE);
        tree->item_labelfont(flw::PREF_FIXED_FONT);
        tree->item_labelsize(flw::PREF_FIXED_FONTSIZE);
        make_tree();
        size(w(), h());
        Fl::redraw();
    }
};

Test* Test::TEST = nullptr;

int main(int argc, const char** argv) {
    Test win;

    auto pref = Fl_Preferences(Fl_Preferences::USER, "gnuwimp_test", "test_theme");
    flw::theme::parse(argc, argv);
    flw::theme::load_icon(&win, 666, icon_xpm, "Test Theme");
    flw::theme::load_theme_from_pref(pref);
    flw::theme::load_win_from_pref(pref, "gui.", &win, true);
    win.create();
    win.update_pref();
    Fl::run();
    pref.clear();
    flw::theme::save_theme_to_pref(pref);
    flw::theme::save_win_to_pref(pref, "gui.", &win);

    return 0;
}
