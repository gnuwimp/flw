// Copyright gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#include "flw.h"

#ifndef FLW_AMALGAM
    #include "gridgroup.h"
    #include "inputmenu.h"
#endif

#include <FL/Fl.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Double_Window.H>

using namespace flw;

class Button : public Fl_Button {
public:
    Button(int X = 0, int Y = 0, int W = 0, int H = 0, const char* L = nullptr) : Fl_Button(X, Y, W, H, L) {
    }
    
    void draw() override {
        if (Fl::focus() == this) {
            color(color::TEAL);
        }
        else {
            color(FL_BACKGROUND_COLOR);
        
        }
        
        Fl_Button::draw();
    }
};

/***
 *      _______        _  __ 
 *     |__   __|      | |/_ |
 *        | | ___  ___| |_| |
 *        | |/ _ \/ __| __| |
 *        | |  __/\__ \ |_| |
 *        |_|\___||___/\__|_|
 *                           
 *                           
 */

class Test1 : public Fl_Double_Window {
public:
    Test1(int W, int H) : Fl_Double_Window(W, H, "Test1 :: test_gridgroup.cpp") {
        end();
        
        b1    = new Button(0, 0, 0, 0, "B&1");
        grid1 = new GridGroup(10, 10, 300, H - 20);

        grid1->add(new Button(0, 0, 0, 0, "MENU"),  0,   0,   0,   2);
        grid1->add(b1,                                 0,   2,   4,   2);
        grid1->add(new Button(0, 0, 0, 0, "B2"),    0,   5,   4,   2);
        grid1->add(new Button(0, 0, 0, 0, "B3"),    0,   8,   4,   2);
        grid1->add(new Button(0, 0, 0, 0, "BOX"),   1,  -3,  -1,   1);
        grid1->add(new Button(0, 0, 0, 0, "B4"),  -10,  -2,   4,   2);
        grid1->add(new Button(0, 0, 0, 0, "B5"),   -5,  -5,  -2,   2);
        grid1->add(new Button(0, 0, 0, 0, "B6"),   -4,   2,   4,   2);
        grid1->add(new Button(0, 0, 0, 0, "B7"),   -4,   5,   4,   2);
        grid1->add(new Button(0, 0, 0, 0, "B8"),   -4,   8,   4,   2);
        grid1->add(new Button(0, 0, 0, 0, "LIST"),  5,   2,  -5,  -4);
        grid1->color(FL_YELLOW);
        grid1->box(FL_FLAT_BOX);
        grid1->size(8);
        util::labelfont(grid1, flw::PREF_FONT, 8);
        
        b2    = new Button(0, 0, 0, 0, "B&2");
        grid2 = new GridGroup(320, 10, W - 330, H - 20);

        grid2->add(new Button(0, 0, 0, 0, "MENU1"),  0,   0,  10,   4);
        grid2->add(new Button(0, 0, 0, 0, "MENU2"), 10,   0,   0,   4);
        grid2->add(new Button(0, 0, 0, 0, "B1"),     0,   4,   4,   4);
        grid2->add(new Button(0, 0, 0, 0, "B2"),     0,   8,   4,   4);
        grid2->add(new Button(0, 0, 0, 0, "B3"),     0,  12,   4,   4);
        grid2->add(new Button(0, 0, 0, 0, "BOX"),    5,  -4,  -5,   4);
        grid2->add(new Button(0, 0, 0, 0, "B4"),     0,  -4,   5,   4);
        grid2->add(new Button(0, 0, 0, 0, "B5"),    -5,  -4,   0,   4);
        grid2->add(new Button(0, 0, 0, 0, "B6"),    -4,   4,   4,   4);
        grid2->add(new Button(0, 0, 0, 0, "B7"),    -4,   8,   4,   4);
        grid2->add(new Button(0, 0, 0, 0, "B8"),    -4,  12,   4,   4);
        grid2->add(b2,                                  5,   4,  -5,  -4);
        grid2->color(FL_RED);
        grid2->box(FL_FLAT_BOX);
        util::labelfont(grid2);
        
        b1->callback(Callback, this);
        b2->callback(Callback, this);

        add(grid1);
        add(grid2);
        resizable(this);
        size_range(64, 48);
        grid1->do_layout();
        grid2->do_layout();
    }

    static void Callback(Fl_Widget* w, void* v) {
        Test1* self = (Test1*) v;
        
        if (w == self->b1) {
            delete self->grid1->remove(self->b1);
            self->b1 = nullptr;
            self->grid1->do_layout();
        }
        else if (w == self->b2) {
            delete self->grid2->remove(self->b2);
            self->b2 = nullptr;
            self->grid2->do_layout();
        }
    }

    GridGroup* grid1;
    GridGroup* grid2;
    Fl_Button* b1;
    Fl_Button* b2;
};

/***
 *      _______        _   ___  
 *     |__   __|      | | |__ \ 
 *        | | ___  ___| |_   ) |
 *        | |/ _ \/ __| __| / / 
 *        | |  __/\__ \ |_ / /_ 
 *        |_|\___||___/\__|____|
 *                              
 *                              
 */

#define ADJUST "Toggle GridGroup::Adjust()\nFor GRID2"
#define DEACT  "Deactivate\nor\nActivate\nbuttons"

class Test2 : public Fl_Double_Window {
public:
    Test2(int W, int H) : Fl_Double_Window(W, H, "Test2 :: test_gridgroup.cpp") {
        end();
        
        grid   = new GridGroup(0, 0, W, H, "GRID");
        toggle = true;

        grid->add(new Button(0, 0, 0, 0, "&TOP"),       0,   0,   0,   4);
        grid->add(new Button(0, 0, 0, 0, DEACT),        0,   4,  16,   0);
        grid->add(new Button(0, 0, 0, 0, ADJUST),      16, -16,   0,   0);
        grid->add(new Button(0, 0, 0, 0, "RIGHT3"),   -20,  12,   0, -18);
        grid->add(new Button(0, 0, 0, 0, "RIGHT2"),   -10,   8,   0,   4);
        grid->add(new Button(0, 0, 0, 0, "RIGHT1"),   -20,   4,  10,   4);
        util::find_widget(grid, ADJUST)->tooltip("Press to toggle adjust on grid 2");
        util::find_widget(grid, ADJUST)->callback(Callback1, this);
        util::find_widget(grid, "RIGHT1")->callback(Callback2, this);
        util::find_widget(grid, "RIGHT2")->callback(Callback2, this);
        util::find_widget(grid, "RIGHT3")->callback(Callback2, this);
        util::find_widget(grid, DEACT)->callback(Callback3, this);
        util::find_widget(grid, "&TOP")->callback(Callback2, this);
        util::find_widget(grid, "&TOP")->take_focus();
        //util::find_widget(grid, "&TOP")->deactivate();
        util::labelfont(grid);
        
        grid2  = new GridGroup(0, 0, 0, 0, "GRID2");
        //auto box1 = new Button(0, 0, 0, 0, "b1");
        auto box1 = new flw::InputMenu(0, 0, 0, 0, "i1");
        box1->insert("History a", 10);
        box1->insert("History b", 10);
        box1->input()->label("INPUT");

        auto box2 = new Button(0, 0, 0, 0, "&b2");
        auto box3 = new Button(0, 0, 0, 0, "b3");
        auto box4 = new Button(0, 0, 0, 0, "b4");
        //box1->align(FL_ALIGN_CENTER | FL_ALIGN_INSIDE | FL_ALIGN_CLIP);
        //box1->box(FL_BORDER_BOX);
        //box1->tooltip("not adjusted\nb1, b2, b3, b4 are slight overlapping");
        box2->align(FL_ALIGN_CENTER | FL_ALIGN_INSIDE | FL_ALIGN_CLIP);
        box2->box(FL_BORDER_BOX);
        box2->tooltip("adjusted 1 pixel left and right\nb1, b2, b3, b4 are slight overlapping");
        box3->align(FL_ALIGN_CENTER | FL_ALIGN_INSIDE | FL_ALIGN_CLIP);
        box3->box(FL_BORDER_BOX);
        box3->tooltip("adjusted 1 pixel top and bottom\nb1, b2, b3, b4 are slight overlapping\ndelete b1, b2, b4");
        box4->align(FL_ALIGN_CENTER | FL_ALIGN_INSIDE | FL_ALIGN_CLIP);
        box4->box(FL_BORDER_BOX);
        box4->tooltip("adjusted 1 pixel left and right and top and bottom\nb1, b2, b3, b4 are slight overlapping");
        
        grid2->add(box1,  0,   0,  40,  30, box1->input());
        grid2->add(box2, 40,   0,   0,  30);
        grid2->add(box3,  0,  30,  40,   0);
        grid2->add(box4, 40,  30,   0,   0);
        
        grid2->adjust(box2, -1, 1,  0, 0);
        grid2->adjust(box3,  0, 0, -1, 1);
        grid2->adjust(box4, -1, 1, -1, 1);
        
        util::labelfont(grid2);
        box1->callback(Callback2, this);
        box2->callback(Callback2, this);
        box3->callback(Callback4, this);
        box4->callback(Callback2, this);
        grid->add(grid2, 18,   8, -22, -18);

        grid->do_layout();
        tooltip("Tab between widgets works only with current group.");
        add(grid);
        resizable(grid);
        size_range(64, 48);
    }

    static void Callback1(Fl_Widget* w, void* v) {
        printf("Callback1(%s)\n", w->label());
        
        Test2* t = (Test2*) v;
        t->toggle = !t->toggle;

        if (t->toggle) {
            if (util::find_widget(t->grid, "&b2") == nullptr) {
                t->grid2->adjust(util::find_widget(t->grid, "b3"),  0, 0, -1, 1);
            }
            else {
                t->grid2->adjust(util::find_widget(t->grid, "&b2"), -1, 1,  0, 0);
                t->grid2->adjust(util::find_widget(t->grid, "b3"),  0, 0, -1, 1);
                t->grid2->adjust(util::find_widget(t->grid, "b4"), -1, 1, -1, 1);
            }
        }
        else {
            if (util::find_widget(t->grid, "&b2") == nullptr) {
                t->grid2->adjust(util::find_widget(t->grid, "b3"));
            }
            else {
                t->grid2->adjust(util::find_widget(t->grid, "&b2"));
                t->grid2->adjust(util::find_widget(t->grid, "b3"));
                t->grid2->adjust(util::find_widget(t->grid, "b4"));
            }
        }
        
        t->grid2->do_layout();
    }
    
    static void Callback2(Fl_Widget* w, void*) {
        printf("Callback2(%s)\n", w->label());
    }

    static void Callback3(Fl_Widget* w, void* v) {
        printf("Callback3(%s)\n", w->label());
        
        Test2* t = (Test2*) v;
        
        if (util::find_widget(t, "&TOP")->active()) {
            util::find_widget(t, "&TOP")->deactivate();
            util::find_widget(t, "b3")->deactivate();
        }
        else {
            util::find_widget(t, "&TOP")->activate();
            util::find_widget(t, "b3")->activate();
        }
    }

    static void Callback4(Fl_Widget* w, void* v) {
        Test2* t = (Test2*) v;
        int i = 0;
        
        while (t->grid2->children() > 1) {
            auto w2 = t->grid2->child(i);
            
            if (w2 != w) {
                delete t->grid2->remove(w2);
            }
            else {
                i = 1;
            }
        }
        
        t->grid2->do_layout();
    }

    int handle(int event) override {
        if (event == FL_KEYUP && Fl::event_key() == 'p') {
            auto f = Fl::focus();            
            if (f) printf("FOCUS: %p: %s\n", f, f->label());
        }
        
        return Fl_Double_Window::handle(event);
    }

    GridGroup*  grid;
    GridGroup*  grid2;
    bool        toggle;
};

/***
 *                      _       
 *                     (_)      
 *      _ __ ___   __ _ _ _ __  
 *     | '_ ` _ \ / _` | | '_ \ 
 *     | | | | | | (_| | | | | |
 *     |_| |_| |_|\__,_|_|_| |_|
 *                              
 *                              
 */

int main(int argc, const char** argv) {
    if (flw::theme::parse(argc, argv) == false) {
        flw::theme::load("oxy");
    }

    {
        Test1 win1(800, 680);
        Test2 win2(800, 480);
        win1.show();
        win2.show();
        Fl::run();
    }
    
    puts("DONE");
    return 0;
}
