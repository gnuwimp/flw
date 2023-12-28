// Copyright gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#include "flw.h"

#ifndef FLW_AMALGAM
    #include "splitgroup.h"
#endif

#include <FL/Fl.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Double_Window.H>

using namespace flw;

class Test : public Fl_Double_Window {
public:
    Test(int w, int h) : Fl_Double_Window(w, h, "test_splitgroup.cpp") {
        group  = new SplitGroup(0, 0, w, h);
        group1 = new SplitGroup(0, 0, 0, 0);
        group2 = new SplitGroup(0, 0, 0, 0);

        add(group);
        group->add(group1, SplitGroup::CHILD::FIRST);
        group->add(group2, SplitGroup::CHILD::SECOND);
        group->direction(SplitGroup::DIRECTION::HORIZONTAL);
        group->min_pos(75);
        group->split_pos(150);

        group1->add(new Fl_Button(0, 0, 0, 0, "Left"), SplitGroup::CHILD::FIRST);
        group1->add(new Fl_Button(0, 0, 0, 0, "Hide/Show Left"), SplitGroup::CHILD::SECOND);
        group1->split_pos(200);
        group1->child(SplitGroup::CHILD::SECOND)->callback(Callback1Right, this);

        group2_direction = SplitGroup::DIRECTION::HORIZONTAL;
        group2_child     = SplitGroup::CHILD::SECOND;
        group2_count     = 0;
        group2_pos       = 150;

        group2->add(new Fl_Button(0, 0, 0, 0, "1 Hide SECOND/HORIZONTAL"), SplitGroup::CHILD::FIRST);
        group2->add(new Fl_Button(0, 0, 0, 0, "2 Hide SECOND/HORIZONTAL"), SplitGroup::CHILD::SECOND);
        group2->direction(SplitGroup::DIRECTION::HORIZONTAL);
        group2->min_pos(75);
        group2->split_pos(group2_pos);
        group2->child(SplitGroup::CHILD::FIRST)->callback(Callback2Top, this);
        group2->child(SplitGroup::CHILD::SECOND)->callback(Callback2Top, this);

        group->resize();
        resizable(this);
        size_range(64, 48);
    }

    static void Callback1Right(Fl_Widget*, void* o) {
        auto w = (Test*) o;

        if (w->group1->child(SplitGroup::CHILD::FIRST)->visible()) {
            w->group1->child(SplitGroup::CHILD::FIRST)->hide();
        }
        else {
            w->group1->child(SplitGroup::CHILD::FIRST)->show();
        }

        w->group->resize();
    }

    static void Callback2Top(Fl_Widget*, void* o) {
        auto w = (Test*) o;

        w->group2_count++;
        // fprintf(stderr, "count=%d, toggle=%s\n", w->group2_count, w->group2_child == SplitGroup::CHILD::FIRST ? "first" : "second");

        if (w->group2_count <= 2) {
            w->group2->toggle(w->group2_child, w->group2_direction, w->group2_pos);
        }
        else if (w->group2_count == 3) {
            w->group2_direction = w->group2_direction == SplitGroup::DIRECTION::HORIZONTAL ? SplitGroup::DIRECTION::VERTICAL : SplitGroup::DIRECTION::HORIZONTAL;
            w->group2->toggle(w->group2_child, w->group2_direction, w->group2_pos);
        }
        else if (w->group2_count <= 5) {
            w->group2->toggle(w->group2_child, w->group2_direction, w->group2_pos);
        }
        else {
            w->group2_child     = w->group2_child == SplitGroup::CHILD::FIRST ? SplitGroup::CHILD::SECOND : SplitGroup::CHILD::FIRST;
            w->group2_direction = SplitGroup::DIRECTION::HORIZONTAL;
            w->group2_count     = 0;

            if (w->group2_child == SplitGroup::CHILD::SECOND && w->group2_count == 0) {
                w->group2_pos = -1;
            }

            w->group2->toggle(w->group2_child, w->group2_direction, w->group2_pos);
        }

        if (w->group2_child == SplitGroup::CHILD::SECOND) {
            if (w->group2_count == 0) {
                w->group2->child(SplitGroup::CHILD::FIRST)->label("1 Hide SECOND/HORIZONTAL (From Start)");
                w->group2->child(SplitGroup::CHILD::SECOND)->label("2 Hide SECOND/HORIZONTAL (From Start)");
            }
            else if (w->group2_count == 1) {
                w->group2->child(SplitGroup::CHILD::FIRST)->label("1 Show SECOND/HORIZONTAL");
                w->group2->child(SplitGroup::CHILD::SECOND)->label("2 Show SECOND/HORIZONTAL");
            }
            else if (w->group2_count == 2) {
                w->group2->child(SplitGroup::CHILD::FIRST)->label("1 Show VERTICAL");
                w->group2->child(SplitGroup::CHILD::SECOND)->label("2 Show VERTICAL");
            }
            else if (w->group2_count == 3) {
                w->group2->child(SplitGroup::CHILD::FIRST)->label("1 Hide SECOND/VERTICAL");
                w->group2->child(SplitGroup::CHILD::SECOND)->label("2 Hide SECOND/VERTICAL");
            }
            else if (w->group2_count == 4) {
                w->group2->child(SplitGroup::CHILD::FIRST)->label("1 Show SECOND/VERTICAL");
                w->group2->child(SplitGroup::CHILD::SECOND)->label("2 Show SECOND/VERTICAL");
            }
            else if (w->group2_count == 5) {
                w->group2->child(SplitGroup::CHILD::FIRST)->label("1 Show HORIZONTAL");
                w->group2->child(SplitGroup::CHILD::SECOND)->label("2 Show HORIZONTAL");
            }
            else {
                w->group2->child(SplitGroup::CHILD::FIRST)->label("ERROR_LEFT");
                w->group2->child(SplitGroup::CHILD::SECOND)->label("ERROR_LEFT");
            }
        }
        else {
            if (w->group2_count == 0) {
                w->group2->child(SplitGroup::CHILD::FIRST)->label("1 Hide FIRST/HORIZONTAL");
                w->group2->child(SplitGroup::CHILD::SECOND)->label("2 Hide FIRST/HORIZONTAL");
            }
            else if (w->group2_count == 1) {
                w->group2->child(SplitGroup::CHILD::FIRST)->label("1 Show FIRST/HORIZONTAL");
                w->group2->child(SplitGroup::CHILD::SECOND)->label("2 Show FIRST/HORIZONTAL");
            }
            else if (w->group2_count == 2) {
                w->group2->child(SplitGroup::CHILD::FIRST)->label("1 Show VERTICAL");
                w->group2->child(SplitGroup::CHILD::SECOND)->label("2 Show VERTICAL");
            }
            else if (w->group2_count == 3) {
                w->group2->child(SplitGroup::CHILD::FIRST)->label("1 Hide FIRST/VERTICAL");
                w->group2->child(SplitGroup::CHILD::SECOND)->label("2 Hide FIRST/VERTICAL");
            }
            else if (w->group2_count == 4) {
                w->group2->child(SplitGroup::CHILD::FIRST)->label("1 Show FIRST/VERTICAL");
                w->group2->child(SplitGroup::CHILD::SECOND)->label("2 Show FIRST/VERTICAL");
            }
            else if (w->group2_count == 5) {
                w->group2->child(SplitGroup::CHILD::FIRST)->label("1 Show HORIZONTAL (Equal Size)");
                w->group2->child(SplitGroup::CHILD::SECOND)->label("2 Show HORIZONTAL (Equal Size)");
            }
            else {
                w->group2->child(SplitGroup::CHILD::FIRST)->label("ERROR_RIGHT");
                w->group2->child(SplitGroup::CHILD::SECOND)->label("ERROR_RIGHT");
            }
        }

        w->group->resize();
        Fl::redraw();
    }

    SplitGroup*           group1;
    SplitGroup*           group2;
    SplitGroup*           group;
    SplitGroup::CHILD     group2_child;
    SplitGroup::DIRECTION group2_direction;
    int                   group2_count;
    int                   group2_pos;
};

int main(int argc, const char** argv) {
    if (flw::theme::parse(argc, argv) == false) {
        flw::theme::load("gtk");
    }

    Test win(800, 480);
    win.show();
    return Fl::run();
}
