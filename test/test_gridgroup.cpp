// Copyright gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#include "flw.h"

#ifndef FLW_AMALGAM
    #include "gridgroup.h"
#endif

#include <FL/Fl.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Double_Window.H>

using namespace flw;

class Button : public Fl_Button {
public:
    Button(int X = 0, int Y = 0, int W = 0, int H = 0, const char* L = nullptr) : Fl_Button(X, Y, W, H, L) {
    }
    
    void draw() override {
        if (Fl::focus() == this) {
            color(FL_GREEN);
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
    Test1(int W, int H) : Fl_Double_Window(W, H, "test_gridgroup.cpp - Test1") {
        end();
        
        b1    = new Button(0, 0, 0, 0, "B&1");
        grid1 = new GridGroup(10, 10, 300, H - 20);

//                                                     X    Y    W    H
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
        
        b2    = new Button(0, 0, 0, 0, "&REMOVE");
        grid2 = new GridGroup(320, 10, W - 330, H - 20);
//                                                      X    Y    W    H
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
        
        b1->callback(Callback1, this);
        b2->callback(Callback2, this);

        add(grid1);
        add(grid2);
        resizable(this);
        size_range(64, 48);
        grid1->do_layout();
        grid2->do_layout();
    }

    static void Callback1(Fl_Widget*, void* v) {
        Test1* w = (Test1*) v;

        w->grid1->remove(w->b1);
        w->b1 = nullptr;
        w->grid1->do_layout();
    }

    static void Callback2(Fl_Widget*, void* v) {
        Test1* w = (Test1*) v;

        w->grid2->remove(w->b2);
        w->b2 = nullptr;
        w->grid2->do_layout();
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

class Test2 : public Fl_Double_Window {
public:
    Test2(int W, int H) : Fl_Double_Window(W, H, "test_gridgroup.cpp - Test2") {
        end();
        
        grid   = new GridGroup(0, 0, W, H, "GRID");
        toggle = true;
//                                                         X    Y    W    H
        grid->add(new Button(0, 0, 0, 0, "&TOP"),        0,   0,   0,   4);
        grid->add(new Button(0, 0, 0, 0, "&DEACTIVATE"),   0,   4,  16,   0);
        grid->add(new Button(0, 0, 0, 0, "BOTTOM"),    16, -16,   0,   0);
        grid->add(new Button(0, 0, 0, 0, "RIGHT1"),   -20,   4,  10,   4);
        grid->add(new Button(0, 0, 0, 0, "RIGHT2"),   -10,   8,   0,   4);
        grid->add(new Button(0, 0, 0, 0, "RIGHT3"),   -20,  12,   0, -18);
        util::widget(grid, "BOTTOM")->tooltip("Press to toggle adjust on grid 2");
        util::widget(grid, "BOTTOM")->callback(Callback1, this);
        util::widget(grid, "RIGHT1")->callback(Callback2, this);
        util::widget(grid, "RIGHT2")->callback(Callback2, this);
        util::widget(grid, "RIGHT3")->callback(Callback2, this);
        util::widget(grid, "&DEACTIVATE")->callback(Callback3, this);
        util::widget(grid, "&TOP")->callback(Callback2, this);
        util::widget(grid, "&TOP")->take_focus();
//        util::widget(grid, "&TOP")->deactivate();
        util::labelfont(grid);
        
        grid2  = new GridGroup(0, 0, 0, 0, "GRID2");
        auto box1 = new Button(0, 0, 0, 0, "b1");
        auto box2 = new Button(0, 0, 0, 0, "&b2");
        auto box3 = new Button(0, 0, 0, 0, "b3");
        auto box4 = new Button(0, 0, 0, 0, "b4");
        box1->align(FL_ALIGN_CENTER | FL_ALIGN_INSIDE | FL_ALIGN_CLIP);
        box1->box(FL_BORDER_BOX);
        box1->tooltip("not adjusted\nb1, b2, b3, b4 are slight overlapping");
        box2->align(FL_ALIGN_CENTER | FL_ALIGN_INSIDE | FL_ALIGN_CLIP);
        box2->box(FL_BORDER_BOX);
        box2->tooltip("adjusted 1 pixel left and right\nb1, b2, b3, b4 are slight overlapping");
        box3->align(FL_ALIGN_CENTER | FL_ALIGN_INSIDE | FL_ALIGN_CLIP);
        box3->box(FL_BORDER_BOX);
        box3->tooltip("adjusted 1 pixel top and bottom\nb1, b2, b3, b4 are slight overlapping");
        box4->align(FL_ALIGN_CENTER | FL_ALIGN_INSIDE | FL_ALIGN_CLIP);
        box4->box(FL_BORDER_BOX);
        box4->tooltip("adjusted 1 pixel left and right and top and bottom\nb1, b2, b3, b4 are slight overlapping");
        grid2->add(box1,  0,   0,  40,  30);
        grid2->add(box2, 40,   0,   0,  30);
        grid2->add(box3,  0,  30,  40,   0);
        grid2->add(box4, 40,  30,   0,   0);
        grid2->adjust(box2, -1, 1,  0, 0);
        grid2->adjust(box3,  0, 0, -1, 1);
        grid2->adjust(box4, -1, 1, -1, 1);
        util::labelfont(grid2);
        util::widget(grid2, "b1")->callback(Callback2, this);
        util::widget(grid2, "&b2")->callback(Callback2, this);
        util::widget(grid2, "b3")->callback(Callback2, this);
        util::widget(grid2, "b4")->callback(Callback2, this);
        grid->add(grid2, 18,   8, -22, -18);

        grid->do_layout();
        add(grid);
        resizable(grid);
        size_range(64, 48);
    }

    static void Callback1(Fl_Widget* w, void* v) {
        printf("Callback1(%s)\n", w->label());
        
        Test2* t = (Test2*) v;
        t->toggle = !t->toggle;

        if (t->toggle) {
            t->grid2->adjust(util::widget(t->grid, "&b2"), -1, 1,  0, 0);
            t->grid2->adjust(util::widget(t->grid, "b3"),  0, 0, -1, 1);
            t->grid2->adjust(util::widget(t->grid, "b4"), -1, 1, -1, 1);
        }
        else {
            t->grid2->adjust(util::widget(t->grid, "&b2"));
            t->grid2->adjust(util::widget(t->grid, "b3"));
            t->grid2->adjust(util::widget(t->grid, "b4"));
        }
        
        t->grid2->do_layout();
    }
    
    static void Callback2(Fl_Widget* w, void*) {
        printf("Callback2(%s)\n", w->label());
    }

    static void Callback3(Fl_Widget* w, void* v) {
        printf("Callback3(%s)\n", w->label());
        
        Test2* t = (Test2*) v;
        
        if (util::widget(t, "&TOP")->active()) {
            util::widget(t, "&TOP")->deactivate();
            util::widget(t, "b1")->deactivate();
        }
        else {
            util::widget(t, "&TOP")->activate();
            util::widget(t, "b1")->activate();
        }
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
        flw::theme::load("gtk");
    }

    Test1 win1(800, 680);
    Test2 win2(800, 480);
    win1.show();
    win2.show();
    return Fl::run();
}
