// Copyright gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#include "toolgroup.h"
#include "flw.h"

// MKALGAM_ON

namespace flw {

/***
 *        _______          _  _____                        _____ _     _ _     _ 
 *       |__   __|        | |/ ____|                      / ____| |   (_) |   | |
 *          | | ___   ___ | | |  __ _ __ ___  _   _ _ __ | |    | |__  _| | __| |
 *          | |/ _ \ / _ \| | | |_ | '__/ _ \| | | | '_ \| |    | '_ \| | |/ _` |
 *          | | (_) | (_) | | |__| | | | (_) | |_| | |_) | |____| | | | | | (_| |
 *          |_|\___/ \___/|_|\_____|_|  \___/ \__,_| .__/ \_____|_| |_|_|_|\__,_|
 *      ______                                     | |                           
 *     |______|                                    |_|                           
 */

//------------------------------------------------------------------------------
// Private struct to store layout data and widgets.
//
struct _ToolGroupChild {
    Fl_Widget* widget;
    short      size;

    //------------------------------------------------------------------------------
    _ToolGroupChild() {
        set();
    }

    //------------------------------------------------------------------------------
    void set(Fl_Widget* WIDGET = nullptr, int SIZE = 0) {
        widget = WIDGET;
        size   = SIZE;
    }
};

/***
 *      _______          _ _
 *     |__   __|        | | |
 *        | | ___   ___ | | |__   __ _ _ __
 *        | |/ _ \ / _ \| | '_ \ / _` | '__|
 *        | | (_) | (_) | | |_) | (_| | |
 *        |_|\___/ \___/|_|_.__/ \__,_|_|
 *
 *
 */

//------------------------------------------------------------------------------
ToolGroup::ToolGroup(DIRECTION direction, int X, int Y, int W, int H, const char* l) : Fl_Group(X, Y, W, H, l) {
    end();
    clip_children(1);
    resizable(nullptr);

    _direction = direction;
    _expand    = false;

    for (int f = 0; f < ToolGroup::MAX_WIDGETS; f++) {
        _widgets[f] = new _ToolGroupChild();
    }
}

//------------------------------------------------------------------------------
ToolGroup::~ToolGroup() {
    for (int f = 0; f < ToolGroup::MAX_WIDGETS; f++) {
        delete _widgets[f];
    }
}

//------------------------------------------------------------------------------
void ToolGroup::add(Fl_Widget* widget, int SIZE) {
    for (int f = 0; f < ToolGroup::MAX_WIDGETS; f++) {
        if (_widgets[f]->widget == nullptr) {
            Fl_Group::add(widget);
            _widgets[f]->set(widget, SIZE);
            return;
        }
    }
}

//------------------------------------------------------------------------------
void ToolGroup::clear() {
    for (int f = 0; f < ToolGroup::MAX_WIDGETS; f++) {
        _widgets[f]->set();
    }

    Fl_Group::clear();
}

//------------------------------------------------------------------------------
void ToolGroup::remove(Fl_Widget* widget) {
    for (int f = 0; f < ToolGroup::MAX_WIDGETS; f++) {
        if (_widgets[f]->widget == widget) {
            Fl_Group::remove(widget);
            _widgets[f]->set();
            return;
        }
    }
}

//------------------------------------------------------------------------------
void ToolGroup::resize(const int X, const int Y, const int W, const int H) {
    Fl_Widget::resize(X, Y, W, H);

    if (children() == 0 || W == 0 || H == 0 || visible() == 0) {
        return;
    }

    auto leftover = (_direction == DIRECTION::HORIZONTAL) ? W : H;
    auto count    = 0;
    auto last     = 0;
    auto avg      = 0;
    auto xpos     = X;
    auto ypos     = Y;

    for (int f = 0; f < ToolGroup::MAX_WIDGETS; f++) {
        auto c = *_widgets[f];

        if (c.widget != nullptr) {
            last = f;

            if (c.size > 0) {
                leftover -= (c.size * flw::PREF_FONTSIZE);
            }
            else {
                count++;
            }
        }
    }

    if (count > 0) {
        avg = leftover / count;
    }

    for (int f = 0; f < ToolGroup::MAX_WIDGETS; f++) {
        auto c = *_widgets[f];

        if (c.widget != nullptr) {
            if (_direction == DIRECTION::HORIZONTAL) {
                if (_expand == true && f == last) {
                    c.widget->resize(xpos, Y, X + W - xpos, H);
                }
                else if (c.size > 0) {
                    c.widget->resize(xpos, Y, c.size * flw::PREF_FONTSIZE, H);
                    xpos += flw::PREF_FONTSIZE * c.size;
                }
                else {
                    c.widget->resize(xpos, Y, avg, H);
                    xpos += avg;
                }
            }
            else {
                if (_expand == true && f == last) {
                    c.widget->resize(X, ypos, W, Y + H - ypos);
                }
                else if (c.size > 0) {
                    c.widget->resize(X, ypos, W, c.size * flw::PREF_FONTSIZE);
                    ypos += flw::PREF_FONTSIZE * c.size;
                }
                else {
                    c.widget->resize(X, ypos, W, avg);
                    ypos += avg;
                }
            }
        }
    }
}

//------------------------------------------------------------------------------
void ToolGroup::resize(Fl_Widget* widget, int SIZE) {
    for (int f = 0; f < ToolGroup::MAX_WIDGETS; f++) {
        if (_widgets[f]->widget == widget) {
            _widgets[f]->set(widget, SIZE);
            return;
        }
    }
}

} // flw

// MKALGAM_OFF
