// Copyright gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#include "flw.h"

#ifndef FLW_AMALGAM
    #include "toolgroup.h"
#endif

#include <FL/Fl_Button.H>
#include <FL/Fl_Double_Window.H>

using namespace flw;

#define CB(X) [](Fl_Widget* t, void* o) { auto T = reinterpret_cast<ToolGroup*>(t); T->X(T->X()); }

class Test : public Fl_Double_Window {
public:
    Test(int W, int H) : Fl_Double_Window(W, H, "test_toolgroup.cpp") {
        end();
        
        tool_h = new ToolGroup();
        tool_v = new ToolGroup(ToolGroup::DIRECTION::VERTICAL);
        t17    = new Fl_Button(0, 0, 0, 0, "Toggle\nExpand");
        t21    = new Fl_Button(0, 0, 0, 0, "EQUAL H / RM");
        t23    = new Fl_Button(0, 0, 0, 0, "H3");
        t26    = new Fl_Button(0, 0, 0, 0, "H1");
        t27    = new Fl_Button(0, 0, 0, 0, "Toggle\nExpand");
        
        add(tool_h);
        add(tool_v);
        
        tool_h->add(new Fl_Button(0, 0, 0, 0, "EQUAL W"));
        tool_h->add(new Fl_Button(0, 0, 0, 0, "EQUAL W"));
        tool_h->add(new Fl_Button(0, 0, 0, 0, "W4"), 4);
        tool_h->add(new Fl_Button(0, 0, 0, 0, "EQUAL W"));
        tool_h->add(new Fl_Button(0, 0, 0, 0, "EQUAL W"));
        tool_h->add(new Fl_Button(0, 0, 0, 0, "W2"), 2);
        tool_h->add(t17);
        tool_h->expand_last(true);
        tool_h->color(FL_YELLOW);
        tool_h->box(FL_FLAT_BOX);
        
        tool_v->add(t21);
        tool_v->add(new Fl_Button(0, 0, 0, 0, "EQUAL H"));
        tool_v->add(t23, 3);
        tool_v->add(new Fl_Button(0, 0, 0, 0, "EQUAL H"));
        tool_v->add(new Fl_Button(0, 0, 0, 0, "EQUAL H"));
        tool_v->add(t26, 1);
        tool_v->add(t27);
        tool_v->expand_last(true);
        tool_v->color(FL_YELLOW);
        tool_v->box(FL_FLAT_BOX);
        
        t17->callback(Callback, this);
        t21->callback(Callback, this);
        t27->callback(Callback, this);
        resizable(this);
        size_range(120, 120);
    }
    
    static void Callback(Fl_Widget* w, void* o) {
        auto t = static_cast<Test*>(o);
        
        if (w == t->t17) {
            t->tool_h->expand_last(!t->tool_h->expand_last());
            t->tool_h->do_layout();
        }
        else if (w == t->t21) {
            delete t->tool_v->remove(w);
            delete t->tool_v->remove(t->t23);
            delete t->tool_v->remove(t->t26);
            t->tool_v->do_layout();
            t->t21 = nullptr;
            t->t23 = nullptr;
            t->t26 = nullptr;
        }
        else if (w == t->t27) {
            t->tool_v->expand_last(!t->tool_v->expand_last());
            t->tool_v->do_layout();
        }
    }
    
    void resize(int X, int Y, int W, int H) override {
        Fl_Double_Window::resize(X, Y, W, H);

        if (r.w() != W || r.h() != H) {
//            tool_h->resize(80, 10, W - 80, 60);
//            tool_v->resize(80, 70, 120, H - 70);
            tool_h->resize(0, 0, W, 60);
            tool_v->resize(0, 60, 120, H - 60);
            r = Fl_Rect(this);
        }
    }

    ToolGroup*  tool_h;
    ToolGroup*  tool_v;
    Fl_Button*  t17;
    Fl_Button*  t21;
    Fl_Button*  t23;
    Fl_Button*  t26;
    Fl_Button*  t27;
    Fl_Rect     r;
};

int main(int argc, const char** argv) {
    if (flw::theme::parse(argc, argv) == false) {
        flw::theme::load("oxy");
    }

    {
        Test win(812, 612);
        win.show();
        Fl::run();
    }

    puts("DONE");
    return 0;
}
