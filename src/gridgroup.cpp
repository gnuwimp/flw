// Copyright 2019 - 2021 gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#include "gridgroup.h"
#include <FL/Fl.H>

// MKALGAM_ON

namespace flw {
    //------------------------------------------------------------------------------
    // Private struct to store layout data and widgets
    //
    struct _GridGroupChild {
        Fl_Widget* widget;
        int x;
        int y;
        int w;
        int h;

        _GridGroupChild() {
            widget = nullptr;
            x = 0;
            y = 0;
            w = 0;
            h = 0;
        }
    };
}

//------------------------------------------------------------------------------
flw::GridGroup::GridGroup(int X, int Y, int W, int H, const char* l) : Fl_Group(X, Y, W, H, l) {
    end();
    clip_children(1);

    _size = labelsize();

    for (int f = 0; f < flw::GridGroup::MAX_WIDGETS; f++) {
        _widgets[f] = new _GridGroupChild();
    }
}

//------------------------------------------------------------------------------
flw::GridGroup::~GridGroup() {
    for (int f = 0; f < flw::GridGroup::MAX_WIDGETS; f++) {
        delete _widgets[f];
    }
}

//------------------------------------------------------------------------------
void flw::GridGroup::add(Fl_Widget* widget, int x, int y, int w, int h) {
    for (int f = 0; f < flw::GridGroup::MAX_WIDGETS; f++) {
        if (_widgets[f]->widget == nullptr) {
            Fl_Group::add(widget);

            _widgets[f]->widget = widget;
            _widgets[f]->x      = x;
            _widgets[f]->y      = y;
            _widgets[f]->w      = w;
            _widgets[f]->h      = h;

            return;
        }
    }

    #ifdef DEBUG
        fprintf(stderr, "error: flw::GridGroup::add too many widgets\n");
    #endif
}

//------------------------------------------------------------------------------
void flw::GridGroup::clear() {
    for (int f = 0; f < flw::GridGroup::MAX_WIDGETS; f++) {
        _widgets[f]->widget = nullptr;
        _widgets[f]->x      = 0;
        _widgets[f]->y      = 0;
        _widgets[f]->w      = 0;
        _widgets[f]->h      = 0;
    }

    Fl_Group::clear();
}

//------------------------------------------------------------------------------
void flw::GridGroup::remove(Fl_Widget* widget) {
    for (int f = 0; f < flw::GridGroup::MAX_WIDGETS; f++) {
        if (_widgets[f]->widget == widget) {
            Fl_Group::remove(widget);

            _widgets[f]->widget = nullptr;
            _widgets[f]->x      = 0;
            _widgets[f]->y      = 0;
            _widgets[f]->w      = 0;
            _widgets[f]->h      = 0;

            return;
        }
    }

    #ifdef DEBUG
        fprintf(stderr, "error: flw::GridGroup::remove can't find widget\n");
    #endif
}

//------------------------------------------------------------------------------
void flw::GridGroup::resize() {
    resize(x(), y(), w(), h());
    redraw();
}


//------------------------------------------------------------------------------
void flw::GridGroup::resize(int X, int Y, int W, int H) {
    Fl_Widget::resize(X, Y, W, H);

    if (W == 0 || H == 0) {
        return;
    }

    #ifdef DEBUG
        // fprintf(stderr, "\n%-20s (x=%4d y=%4d)  (w=%4d h=%4d)  (x2=%4d y2=%4d)\n", "GridGroup: ", X, Y, W, H, X + W, Y + H);
        // fprintf(stderr, "%-20s (%4s, %4s)  (%4s, %4s)  (%4s, %4s) <=> (%3s, %3s, %3s, %3s)\n", "", "x", "y", "w", "h", "x+w", "y+h", "lx", "ly", "lw", "lh");
    #endif

    for (int f = 0; f < flw::GridGroup::MAX_WIDGETS; f++) {
        _GridGroupChild* widget = _widgets[f];

        if (widget->widget) {
            int widget_x  = 0;
            int widget_x2 = 0;
            int widget_y  = 0;
            int widget_y2 = 0;
            int widget_w  = 0;
            int widget_h  = 0;

            if (widget->x >= 0) {
                widget_x = X + widget->x * _size;
            }
            else {
                widget_x = X + W + widget->x * _size;
            }

            if (widget->y >= 0) {
                widget_y = Y + widget->y * _size;
            }
            else {
                widget_y = Y + H + widget->y * _size;
            }

            if (widget->w == 0) {
                widget_x2 = X + W;
            }
            else if (widget->w > 0) {
                widget_x2 = widget_x + widget->w * _size;
            }
            else {
                widget_x2 = X + W + widget->w * _size;
            }

            if (widget->h == 0) {
                widget_y2 = Y + H;
            }
            else if (widget->h > 0) {
                widget_y2 = widget_y + widget->h * _size;
            }
            else {
                widget_y2 = Y + H + widget->h * _size;
            }

            widget_w = widget_x2 - widget_x;
            widget_h = widget_y2 - widget_y;

            // if (widget_w >= _size && widget_h >= _size && widget_x + widget_w >= _size && widget_y + widget_h >= _size) {
            if (widget_w >= 0 && widget_h >= 0) {
                widget->widget->resize(widget_x, widget_y, widget_w, widget_h);
            }
            else {
                widget->widget->resize(widget_x, widget_y, 0, 0);
            }

            #ifdef DEBUG
                // fprintf(stderr, "%-20s (%4d, %4d)  (%4d, %4d)  (%4d, %4d) <=> (%3d, %3d, %3d, %3d)\n", widget->widget->label() ? widget->widget->label() : "", widget_x, widget_y, widget_w, widget_h, widget_x + widget_w, widget_y + widget_h, widget->x, widget->y, widget->w, widget->h);
            #endif
        }
    }
}

// MKALGAM_OFF
