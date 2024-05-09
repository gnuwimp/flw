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
struct _ToolGroupChild {
    Fl_Widget*                  widget;
    short                       size;

    //--------------------------------------------------------------------------
    _ToolGroupChild(Fl_Widget* WIDGET, int SIZE) {
        set(WIDGET, SIZE);
    }

    //--------------------------------------------------------------------------
    void set(Fl_Widget* WIDGET, int SIZE) {
        widget = WIDGET;
        size   = SIZE;
    }
};


/***
 *      _______          _  _____                       
 *     |__   __|        | |/ ____|                      
 *        | | ___   ___ | | |  __ _ __ ___  _   _ _ __  
 *        | |/ _ \ / _ \| | | |_ | '__/ _ \| | | | '_ \ 
 *        | | (_) | (_) | | |__| | | | (_) | |_| | |_) |
 *        |_|\___/ \___/|_|\_____|_|  \___/ \__,_| .__/ 
 *                                               | |    
 *                                               |_|    
 */

//------------------------------------------------------------------------------
ToolGroup::ToolGroup(DIRECTION direction, int X, int Y, int W, int H, const char* l) : Fl_Group(X, Y, W, H, l) {
    end();
    clip_children(1);
    resizable(nullptr);

    _direction = direction;
    _expand    = false;
}

//------------------------------------------------------------------------------
ToolGroup::~ToolGroup() {
    for (auto v : _widgets) {
        delete static_cast<_ToolGroupChild*>(v);
    }
}

//------------------------------------------------------------------------------
void ToolGroup::add(Fl_Widget* widget, int SIZE) {
    _widgets.push_back(new _ToolGroupChild(widget, SIZE));
    Fl_Group::add(widget);
}

//------------------------------------------------------------------------------
void ToolGroup::clear() {
    _widgets.clear();
    Fl_Group::clear();
}

//------------------------------------------------------------------------------
Fl_Widget* ToolGroup::remove(Fl_Widget* widget) {
    for (auto it = _widgets.begin(); it != _widgets.end(); it++) {
        auto child = static_cast<_ToolGroupChild*>(*it);
        
        if (child->widget == widget) {
            Fl_Group::remove(widget);
            _widgets.erase(it);
            delete child;
            return widget;
        }
    }

    #ifdef DEBUG
        fprintf(stderr, "error: ToolGroup::remove can't find widget\n");
    #endif
    
    return nullptr;
}

//------------------------------------------------------------------------------
void ToolGroup::resize(const int X, const int Y, const int W, const int H) {
    Fl_Widget::resize(X, Y, W, H);

    if (children() == 0 || W == 0 || H == 0 || visible() == 0) {
        return;
    }

    auto leftover = (_direction == DIRECTION::HORIZONTAL) ? W : H;
    auto count    = 0;
    auto last     = static_cast<Fl_Widget*>(nullptr);
    auto avg      = 0;
    auto xpos     = X;
    auto ypos     = Y;
    
    for (auto v : _widgets) {
        auto child = static_cast<_ToolGroupChild*>(v);

        last = child->widget;

        if (child->size > 0) {
            leftover -= (child->size * flw::PREF_FONTSIZE);
        }
        else {
            count++;
        }
    }

    if (count > 0) {
        avg = leftover / count;
    }

    for (auto v : _widgets) {
        auto child = static_cast<_ToolGroupChild*>(v);
        
        if (child->widget != nullptr) {
            if (_direction == DIRECTION::HORIZONTAL) {
                if (_expand == true && child->widget == last) {
                    child->widget->resize(xpos, Y, X + W - xpos, H);
                }
                else if (child->size > 0) {
                    child->widget->resize(xpos, Y, child->size * flw::PREF_FONTSIZE, H);
                    xpos += flw::PREF_FONTSIZE * child->size;
                }
                else {
                    child->widget->resize(xpos, Y, avg, H);
                    xpos += avg;
                }
            }
            else {
                if (_expand == true && child->widget == last) {
                    child->widget->resize(X, ypos, W, Y + H - ypos);
                }
                else if (child->size > 0) {
                    child->widget->resize(X, ypos, W, child->size * flw::PREF_FONTSIZE);
                    ypos += flw::PREF_FONTSIZE * child->size;
                }
                else {
                    child->widget->resize(X, ypos, W, avg);
                    ypos += avg;
                }
            }
        }
    }
}

//------------------------------------------------------------------------------
void ToolGroup::resize(Fl_Widget* widget, int SIZE) {
    for (auto v : _widgets) {
        auto child = static_cast<_ToolGroupChild*>(v);
        if (child->widget == widget) {
            child->set(widget, SIZE);
            return;
        }
    }
}

} // flw

// MKALGAM_OFF
