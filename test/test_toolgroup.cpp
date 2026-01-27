// Copyright gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#include "flw.h"

#ifndef FLW_AMALGAM
    #include "theme.h"
    #include "toolgroup.h"
#endif

#include <FL/Fl_Button.H>
#include <FL/Fl_Double_Window.H>

using namespace flw;

#define CB(X) [](Fl_Widget* t, void* o) { auto T = reinterpret_cast<ToolGroup*>(t); T->X(T->X()); }

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
    Test(int W, int H) : Fl_Double_Window(W, H, "test_toolgroup.cpp") {
        end();
        
        tool_h = new ToolGroup();
        tool_v = new ToolGroup();
        rem    = new Fl_Button(0, 0, 0, 0, "Remove B3 and B6");
        tnew   = new Fl_Button(0, 0, 0, 0, "New");
        
        add(tool_h);
        add(tool_v);
        add(rem);
        
        t11 = (Fl_Button*) tool_h->add(new Fl_Button(0, 0, 0, 0, "Fixed"));
        t12 = (Fl_Button*) tool_h->add(new Fl_Button(0, 0, 0, 0, "Equal"));
        t13 = (Fl_Button*) tool_h->add(new Fl_Button(0, 0, 0, 0, "Replace Me"));
        tool_h->add(new Fl_Button(0, 0, 0, 0, "Button"));
        tool_h->add(new Fl_Button(0, 0, 0, 0, "Button"));
        tool_h->add(new Fl_Button(0, 0, 0, 0, "Button"), 4);
        t17 = (Fl_Button*) tool_h->add(new Fl_Button(0, 0, 0, 0, "Toggle\nExpand"));
        tool_h->expand_last(true);
        tool_h->color(FL_YELLOW);
        tool_h->box(FL_FLAT_BOX);
        
        t25 = (Fl_Button*) tool_v->add(new Fl_Button(0, 0, 0, 0, "Fixed"));
        t21 = (Fl_Button*) tool_v->add(new Fl_Button(0, 0, 0, 0, "Equal"));
        tool_v->add(new Fl_Button(0, 0, 0, 0, "Button"));
        t23 = (Fl_Button*) tool_v->add(new Fl_Button(0, 0, 0, 0, "B3"), 4);
        tool_v->add(new Fl_Button(0, 0, 0, 0, "Button"));
        t26 = (Fl_Button*) tool_v->add(new Fl_Button(0, 0, 0, 0, "B6"), 2);
        t27 = (Fl_Button*) tool_v->add(new Fl_Button(0, 0, 0, 0, "Toggle\nExpand"));
        tool_v->expand_last(true);
        tool_v->pos(ToolGroup::Pos::VERTICAL);
        tool_v->color(FL_YELLOW);
        tool_v->box(FL_FLAT_BOX);
        
        rem->callback(Callback, this);
        t11->callback(Callback, this);
        t12->callback(Callback, this);
        t13->callback(Callback, this);
        t17->callback(Callback, this);
        t21->callback(Callback, this);
        t25->callback(Callback, this);
        t27->callback(Callback, this);
        resizable(this);
        size_range(120, 120);
        //tool_h->clear();
    }
    
    //--------------------------------------------------------------------------
    ~Test() {
        delete tnew;
    }
    
    //--------------------------------------------------------------------------
    static void Callback(Fl_Widget* w, void* o) {
        auto t = static_cast<Test*>(o);
        
        if (w == t->rem) {
            delete t->tool_v->remove(t->t23);
            delete t->tool_v->remove(t->t26);
            t->t23 = nullptr;
            t->t26 = nullptr;
            t->tool_v->do_layout();
        }
        else if (w == t->t11) {
            t->tool_h->size(6);
        }
        else if (w == t->t12) {
            t->tool_h->size(0);
        }
        else if (w == t->t13 && t->tnew != nullptr) {
            delete t->tool_h->replace(t->t13, t->tnew);
            t->tnew = nullptr;
            t->tool_h->do_layout();
        }
        else if (w == t->t17) {
            t->tool_h->expand_last(!t->tool_h->expand_last());
            t->tool_h->do_layout();
        }
        else if (w == t->t21) {
            t->tool_v->size(0);
        }
        else if (w == t->t25) {
            t->tool_v->size(4);
        }
        else if (w == t->t27) {
            t->tool_v->expand_last(!t->tool_v->expand_last());
            t->tool_v->do_layout();
        }
    }
    
    //--------------------------------------------------------------------------
    void resize(int X, int Y, int W, int H) override {
        Fl_Double_Window::resize(X, Y, W, H);

        //tool_h->resize(80, 10, W - 80, 60);
        //tool_v->resize(80, 70, 120, H - 70);
        //rem->resize(200, 70, W - 200, H - 70);
        tool_h->resize(0, 0, W, 60);
        tool_v->resize(0, 60, 120, H - 60);
        rem->resize(120, 60, W - 120, H - 60);
    }

    ToolGroup*  tool_h;
    ToolGroup*  tool_v;
    Fl_Button*  t11;
    Fl_Button*  t12;
    Fl_Button*  t13;
    Fl_Button*  t17;
    Fl_Button*  t21;
    Fl_Button*  t23;
    Fl_Button*  t25;
    Fl_Button*  t26;
    Fl_Button*  t27;
    Fl_Button*  tnew;
    Fl_Button*  rem;
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

    {
        Test win(1200, 800);
        win.show();
        Fl::run();
    }

    puts("DONE");
    return 0;
}
