// Copyright gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#include "flw.h"

#ifndef FLW_AMALGAM
    #include "gridgroup.h"
    #include "scrollbrowser.h"
#endif

#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Double_Window.H>

using namespace flw;

/*
 *      _______        _
 *     |__   __|      | |
 *        | | ___  ___| |_
 *        | |/ _ \/ __| __|
 *        | |  __/\__ \ |_
 *        |_|\___||___/\__|
 *
 *
 */

//------------------------------------------------------------------------------
class Test : public Fl_Double_Window {
public:
    //--------------------------------------------------------------------------
    Test(int W, int H) : Fl_Double_Window(W, H, "test_scrollbrowser.cpp") {
        TEST = this;

        box     = new Fl_Box(0, 0, 0, 0);
        browser = new ScrollBrowser();
        button1 = new Fl_Button(0, 0, 0, 0, "Toggle Menu");
        button2 = new Fl_Button(0, 0, 0, 0, "Toggle Scrollmode");
        button3 = new Fl_Button(0, 0, 0, 0, "Toggle 3/9 Lines");
        grid    = new GridGroup(0, 0, W, H);

        add(grid);
        grid->add(browser,  1,  1,  40,  -1);
        grid->add(box,     42,  1,  -1,   4);
        grid->add(button1, 42,  6,  -1,   4);
        grid->add(button2, 42, 11,  -1,   4);
        grid->add(button3, 42, 16,  -1,   4);
        grid->do_layout();

        for (int f = 0; f < 200; f++) {
            char buf[100];
            sprintf(buf, "%sRow num %03d", f % 3 == 0 ? "@C88" : f % 2 == 0 ? "@f" : "", f + 1);
            browser->add(buf);
        }

        box->box(FL_BORDER_BOX);
        box->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
        browser->callback(Callback);
        button1->callback(Callback);
        button2->callback(Callback);
        button3->callback(Callback);
        browser->take_focus();

        callback(Callback);
        resizable(grid);
        size_range(64, 48);
    }

    //--------------------------------------------------------------------------
    static void Callback(Fl_Widget* sender, void*) {
        char buf[100];
        *buf = 0;

        if (sender == TEST) {
            TEST->hide();
        }
        else if (sender == TEST->browser) {
            snprintf(buf, 100, "row = %d: %s", TEST->browser->value(), TEST->browser->text2().c_str());
            TEST->box->copy_label(buf);
        }
        else if (sender == TEST->button1) {
            if (TEST->browser->is_menu_enabled() == true) {
                TEST->browser->disable_menu();
            }
            else {
                TEST->browser->enable_menu();
            }
        }
        else if (sender == TEST->button2) {
            if (TEST->browser->is_scrollmode_enabled() == true) {
                TEST->browser->disable_scrollmode();
            }
            else {
                TEST->browser->enable_scrollmode();
            }
        }
        else if (sender == TEST->button3) {
            if (TEST->browser->scroll_lines() == 9) {
                TEST->browser->scroll_lines(3);
            }
            else {
                TEST->browser->scroll_lines(9);
            }
        }
    }

    Fl_Button*     button1;
    Fl_Button*     button2;
    Fl_Button*     button3;
    ScrollBrowser* browser;
    GridGroup*     grid;
    Fl_Box*        box;
    static Test*   TEST;
};

Test* Test::TEST = nullptr;

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

    Test win(800, 480);
    win.show();
    return Fl::run();
}
