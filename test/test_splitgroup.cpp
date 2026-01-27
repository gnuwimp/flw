// Copyright gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#include "flw.h"

#ifndef FLW_AMALGAM
    #include "splitgroup.h"
    #include "theme.h"
#endif

#include <FL/Fl.H>
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
    Test(int W, int H) : Fl_Double_Window(W, H, "test_splitgroup.cpp") {
        b1     = new Fl_Button(0, 0, 0, 0, "Swap Widgets");
        b2     = new Fl_Button(0, 0, 0, 0, "Hide");
        b3     = new Fl_Button(0, 0, 0, 0, "Show");
        b4     = new Fl_Button(0, 0, 0, 0, "Swap Pos");
        group  = new SplitGroup();
        group1 = new SplitGroup();
        group2 = new SplitGroup();

        add(b1);
        add(b2);
        add(b3);
        add(b4);
        add(group);

        group->add(group1, true);
        group->add(group2, false);
        group->pos(SplitGroup::Pos::HORIZONTAL);
        group->min_split_pos(100);

        group1->add(new Fl_Button(0, 0, 0, 0, "A"), true);
        group1->add(new Fl_Button(0, 0, 0, 0, "B"), false);

        group2->add(new Fl_Button(0, 0, 0, 0, "C"), true);
        group2->add(new Fl_Button(0, 0, 0, 0, "D"), false);
        group2->pos(SplitGroup::Pos::HORIZONTAL);
        group2->min_split_pos(10);

        //group1->split_pos(100);
        group->do_layout();

        b1->callback(Callback, this);
        b2->callback(Callback, this);
        b3->callback(Callback, this);
        b4->callback(Callback, this);

        resizable(group);
        size_range(64, 48);
    }

    //--------------------------------------------------------------------------
    static void Callback(Fl_Widget* b, void* o) {
        auto w = (Test*) o;

        if (b == w->b1) {
            w->group1->show_child(true);
            w->group1->show_child(false);
            w->group1->swap();

            w->group2->show_child(true);
            w->group2->show_child(false);
            w->group2->swap();
        }
        else if (b == w->b2) {
            w->group1->hide_child(true);
            w->group2->hide_child(true);
        }
        else if (b == w->b3) {
            w->group1->show_child(true);
            w->group2->show_child(true);
        }
        else if (b == w->b4) {
            w->group->pos(w->group->pos() == SplitGroup::Pos::HORIZONTAL ? SplitGroup::Pos::VERTICAL : SplitGroup::Pos::HORIZONTAL);
            w->group1->pos(w->group1->pos() == SplitGroup::Pos::HORIZONTAL ? SplitGroup::Pos::VERTICAL : SplitGroup::Pos::HORIZONTAL);
            w->group2->pos(w->group2->pos() == SplitGroup::Pos::HORIZONTAL ? SplitGroup::Pos::VERTICAL : SplitGroup::Pos::HORIZONTAL);
        }
    }

    //--------------------------------------------------------------------------
    void resize(int X, int Y, int W, int H) {
        Fl_Double_Window::resize(X, Y, W, H);
        b1->resize(0, 0, 110, 28);
        b2->resize(114, 0, 110, 28);
        b3->resize(228, 0, 110, 28);
        b4->resize(342, 0, 110, 28);
        group->resize(0, 28, W, H - 28);
    }

    Fl_Button*      b1;
    Fl_Button*      b2;
    Fl_Button*      b3;
    Fl_Button*      b4;
    SplitGroup*     group;
    SplitGroup*     group1;
    SplitGroup*     group2;
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

    Test win(800, 480);
    win.show();

    return Fl::run();
}
