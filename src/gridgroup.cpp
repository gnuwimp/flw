// Copyright gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#include "gridgroup.h"
#include "flw.h"

// MKALGAM_ON

namespace flw {

/***
 *                 _            _
 *                (_)          | |
 *      _ __  _ __ ___   ____ _| |_ ___
 *     | '_ \| '__| \ \ / / _` | __/ _ \
 *     | |_) | |  | |\ V / (_| | ||  __/
 *     | .__/|_|  |_| \_/ \__,_|\__\___|
 *     | |
 *     |_|
 */

//------------------------------------------------------------------------------
// Private struct to store layout data and widgets.
//
struct _GridGroupChild {
    Fl_Widget* widget;
    short x;
    short y;
    short w;
    short h;
    short l;
    short r;
    short t;
    short b;

    //------------------------------------------------------------------------------
    _GridGroupChild() {
        set();
        adjust();
    }

    //------------------------------------------------------------------------------
    void adjust(int L = 0, int R = 0, int T = 0, int B = 0) {
        l  = L;
        r  = R;
        t  = T;
        b  = B;
    }

    //------------------------------------------------------------------------------
    void set(Fl_Widget* WIDGET = nullptr, int X = 0, int Y = 0, int W = 0, int H = 0) {
        widget = WIDGET;
        x      = X;
        y      = Y;
        w      = W;
        h      = H;
    }
};

/***
 *       _____      _     _  _____
 *      / ____|    (_)   | |/ ____|
 *     | |  __ _ __ _  __| | |  __ _ __ ___  _   _ _ __
 *     | | |_ | '__| |/ _` | | |_ | '__/ _ \| | | | '_ \
 *     | |__| | |  | | (_| | |__| | | | (_) | |_| | |_) |
 *      \_____|_|  |_|\__,_|\_____|_|  \___/ \__,_| .__/
 *                                                | |
 *                                                |_|
 */

//------------------------------------------------------------------------------
GridGroup::GridGroup(int X, int Y, int W, int H, const char* l) : Fl_Group(X, Y, W, H, l) {
    end();
    clip_children(1);
    resizable(nullptr);
    
    _size = 0;

    for (int f = 0; f < GridGroup::MAX_WIDGETS; f++) {
        _widgets[f] = new _GridGroupChild();
    }
}

//------------------------------------------------------------------------------
GridGroup::~GridGroup() {
    for (int f = 0; f < GridGroup::MAX_WIDGETS; f++) {
        delete _widgets[f];
    }
}

//------------------------------------------------------------------------------
void GridGroup::add(Fl_Widget* widget, int X, int Y, int W, int H) {
    for (int f = 0; f < GridGroup::MAX_WIDGETS; f++) {
        if (_widgets[f]->widget == nullptr) {
            Fl_Group::add(widget);
            _widgets[f]->set(widget, X, Y, W, H);
            return;
        }
    }

    #ifdef DEBUG
        fprintf(stderr, "error: flw::GridGroup::add() too many widgets (label=%s)\n", widget->label());
    #endif
}

//------------------------------------------------------------------------------
void GridGroup::adjust(Fl_Widget* widget, int L, int R, int T, int B) {
    for (int f = 0; f < GridGroup::MAX_WIDGETS; f++) {
        if (_widgets[f]->widget == widget) {
            _widgets[f]->adjust(L, R, T, B);
            return;
        }
    }

    #ifdef DEBUG
        fprintf(stderr, "error: flw::GridGroup::adjust() failed to find widget (label=%s)\n", widget->label());
    #endif
}

//------------------------------------------------------------------------------
void GridGroup::clear() {
    for (int f = 0; f < GridGroup::MAX_WIDGETS; f++) {
        _widgets[f]->set();
    }

    Fl_Group::clear();
}

//------------------------------------------------------------------------------
int GridGroup::handle(int event) {
    if (event == FL_KEYDOWN && Fl::event_key() == FL_Tab) {
        if (children() > 0) {
            Fl_Widget* first   = nullptr;
            Fl_Widget* last    = nullptr;
            Fl_Widget* current = Fl::focus();

            _last_active_widget(&first, &last);

//            puts("");
//            printf("FIRST: %p: %s\n", first ? first : 0, first && first->label() ? first->label() : "NULL");
//            printf("LAST:  %p: %s\n", last ? last : 0, last && last->label() ? last->label() : "NULL");

            if (Fl::event_shift() == 0) {               
                if (first != nullptr && current != nullptr && current == last) {
                    Fl::focus(first);
                    first->redraw();
                    return 1;
                }
            }
            else {
                if (first != nullptr && current != nullptr && current == first) {
                    Fl::focus(last);
                    last->redraw();
                    return 1;
                }
            }
        }
    }
    
    return Fl_Group::handle(event);
}

//------------------------------------------------------------------------------
void GridGroup::_last_active_widget(Fl_Widget** first, Fl_Widget** last) {
    for (int f = 0; f < children(); f++) {
        auto c = child(f);
        auto g = c->as_group();
        
        if (g == nullptr) {
            if (c->active() != 0) {
                *last = c;
            }

            if (*first == nullptr && c->active() != 0) {
                *first = c;
            }
        }
    }
}

//------------------------------------------------------------------------------
void GridGroup::remove(Fl_Widget* widget) {
    for (int f = 0; f < GridGroup::MAX_WIDGETS; f++) {
        if (_widgets[f]->widget == widget) {
            Fl_Group::remove(widget);
            _widgets[f]->set();
            return;
        }
    }

    #ifdef DEBUG
        fprintf(stderr, "error: GridGroup::remove can't find widget\n");
    #endif
}

//------------------------------------------------------------------------------
void GridGroup::resize(int X, int Y, int W, int H) {
    Fl_Widget::resize(X, Y, W, H);

    if (children() == 0 || W == 0 || H == 0) {
        return;
    }

    int size = (_size > 0) ? _size : flw::PREF_FONTSIZE / 2;

    for (int f = 0; f < GridGroup::MAX_WIDGETS; f++) {
        auto* c = _widgets[f];

        if (c->widget != nullptr && c->widget->visible() != 0) {
            int widget_x  = 0;
            int widget_x2 = 0;
            int widget_y  = 0;
            int widget_y2 = 0;
            int widget_w  = 0;
            int widget_h  = 0;

            if (c->x >= 0) {
                widget_x = X + c->x * size;
            }
            else {
                widget_x = X + W + c->x * size;
            }

            if (c->y >= 0) {
                widget_y = Y + c->y * size;
            }
            else {
                widget_y = Y + H + c->y * size;
            }

            if (c->w == 0) {
                widget_x2 = X + W;
            }
            else if (c->w > 0) {
                widget_x2 = widget_x + c->w * size;
            }
            else {
                widget_x2 = X + W + c->w * size;
            }

            if (c->h == 0) {
                widget_y2 = Y + H;
            }
            else if (c->h > 0) {
                widget_y2 = widget_y + c->h * size;
            }
            else {
                widget_y2 = Y + H + c->h * size;
            }

            widget_w = widget_x2 - widget_x;
            widget_h = widget_y2 - widget_y;

            if (widget_w >= 0 && widget_h >= 0) {
                c->widget->resize(widget_x + c->l, widget_y + c->t, widget_w + c->r, widget_h + c->b);
            }
            else {
                c->widget->resize(0, 0, 0, 0);
            }
        }
    }
}

//------------------------------------------------------------------------------
void GridGroup::resize(Fl_Widget* widget, int X, int Y, int W, int H) {
    for (int f = 0; f < GridGroup::MAX_WIDGETS; f++) {
        if (_widgets[f]->widget == widget) {
            _widgets[f]->set(widget, X, Y, W, H);
            return;
        }
    }

    #ifdef DEBUG
        fprintf(stderr, "error: flw::GridGroup::resize() failed to find widget (label=%s)\n", widget->label());
    #endif
}

} // flw

// MKALGAM_OFF
