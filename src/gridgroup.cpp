// Copyright gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#include "gridgroup.h"
#include "flw.h"

// MKALGAM_ON

namespace flw {

/***
 *           _____      _     _  _____                        _____ _     _ _     _ 
 *          / ____|    (_)   | |/ ____|                      / ____| |   (_) |   | |
 *         | |  __ _ __ _  __| | |  __ _ __ ___  _   _ _ __ | |    | |__  _| | __| |
 *         | | |_ | '__| |/ _` | | |_ | '__/ _ \| | | | '_ \| |    | '_ \| | |/ _` |
 *         | |__| | |  | | (_| | |__| | | | (_) | |_| | |_) | |____| | | | | | (_| |
 *          \_____|_|  |_|\__,_|\_____|_|  \___/ \__,_| .__/ \_____|_| |_|_|_|\__,_|
 *      ______                                        | |                           
 *     |______|                                       |_|                           
 */

//------------------------------------------------------------------------------
struct _GridGroupChild {
    Fl_Widget*                  widget;
    short                       x;
    short                       y;
    short                       w;
    short                       h;
    short                       l;
    short                       r;
    short                       t;
    short                       b;

    //--------------------------------------------------------------------------
    _GridGroupChild(Fl_Widget* WIDGET, int X, int Y, int W, int H) {
        set(WIDGET, X, Y, W, H);
        adjust();
    }

    //--------------------------------------------------------------------------
    void adjust(int L = 0, int R = 0, int T = 0, int B = 0) {
        l = L;
        r = R;
        t = T;
        b = B;
    }

    //--------------------------------------------------------------------------
    void set(Fl_Widget* WIDGET, int X, int Y, int W, int H) {
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
}

//------------------------------------------------------------------------------
GridGroup::~GridGroup() {
    for (auto v : _widgets) {
        delete static_cast<_GridGroupChild*>(v);
    }
}

//------------------------------------------------------------------------------
void GridGroup::add(Fl_Widget* widget, int X, int Y, int W, int H) {
    _widgets.push_back(new _GridGroupChild(widget, X, Y, W, H));
    Fl_Group::add(widget);
}

//------------------------------------------------------------------------------
void GridGroup::adjust(Fl_Widget* widget, int L, int R, int T, int B) {
    for (auto& v : _widgets) {
        auto child = static_cast<_GridGroupChild*>(v);
        
        if (child->widget == widget) {
            child->adjust(L, R, T, B);
            return;
        }
    }

    #ifdef DEBUG
        fprintf(stderr, "error: flw::GridGroup::adjust() failed to find widget (label=%s)\n", widget->label());
    #endif
}

//------------------------------------------------------------------------------
void GridGroup::clear() {
    _widgets.clear();
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
Fl_Widget* GridGroup::remove(Fl_Widget* widget) {
    for (auto it = _widgets.begin(); it != _widgets.end(); it++) {
        auto child = static_cast<_GridGroupChild*>(*it);
        
        if (child->widget == widget) {
            delete child;
            Fl_Group::remove(widget);
            _widgets.erase(it);
            return widget;
        }
    }

    #ifdef DEBUG
        fprintf(stderr, "error: GridGroup::remove can't find widget\n");
    #endif
    
    return nullptr;
}

//------------------------------------------------------------------------------
void GridGroup::resize(int X, int Y, int W, int H) {
    Fl_Widget::resize(X, Y, W, H);

    if (children() == 0 || W == 0 || H == 0 || visible() == 0) {
        return;
    }

    int size = (_size > 0) ? _size : flw::PREF_FONTSIZE / 2;

    for (const auto& v : _widgets) {
        auto child = static_cast<_GridGroupChild*>(v);

        if (child->widget != nullptr && child->widget->visible() != 0) {
            int widget_x  = 0;
            int widget_x2 = 0;
            int widget_y  = 0;
            int widget_y2 = 0;
            int widget_w  = 0;
            int widget_h  = 0;

            if (child->x >= 0) {
                widget_x = X + child->x * size;
            }
            else {
                widget_x = X + W + child->x * size;
            }

            if (child->y >= 0) {
                widget_y = Y + child->y * size;
            }
            else {
                widget_y = Y + H + child->y * size;
            }

            if (child->w == 0) {
                widget_x2 = X + W;
            }
            else if (child->w > 0) {
                widget_x2 = widget_x + child->w * size;
            }
            else {
                widget_x2 = X + W + child->w * size;
            }

            if (child->h == 0) {
                widget_y2 = Y + H;
            }
            else if (child->h > 0) {
                widget_y2 = widget_y + child->h * size;
            }
            else {
                widget_y2 = Y + H + child->h * size;
            }

            widget_w = widget_x2 - widget_x;
            widget_h = widget_y2 - widget_y;

            if (widget_w >= 0 && widget_h >= 0) {
                child->widget->resize(widget_x + child->l, widget_y + child->t, widget_w + child->r, widget_h + child->b);
            }
            else {
                child->widget->resize(0, 0, 0, 0);
            }
        }
    }
}

//------------------------------------------------------------------------------
void GridGroup::resize(Fl_Widget* widget, int X, int Y, int W, int H) {
    for (auto& v : _widgets) {
        auto child = static_cast<_GridGroupChild*>(v);
        
        if (child->widget == widget) {
            child->set(widget, X, Y, W, H);
            return;
        }
    }

    #ifdef DEBUG
        fprintf(stderr, "error: flw::GridGroup::resize() failed to find widget (label=%s)\n", widget->label());
    #endif
}

} // flw

// MKALGAM_OFF
