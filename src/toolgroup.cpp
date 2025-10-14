/**
* @file
* @brief Toolbar group widget.
*
* @author gnuwimp@gmail.com
* @copyright Released under the GNU General Public License v3.0
*/

#include "toolgroup.h"
#include "flw.h"

// MKALGAM_ON

namespace flw {
/*
 *        _______          _  _____                        _____ _     _ _     _
 *       |__   __|        | |/ ____|                      / ____| |   (_) |   | |
 *          | | ___   ___ | | |  __ _ __ ___  _   _ _ __ | |    | |__  _| | __| |
 *          | |/ _ \ / _ \| | | |_ | '__/ _ \| | | | '_ \| |    | '_ \| | |/ _` |
 *          | | (_) | (_) | | |__| | | | (_) | |_| | |_) | |____| | | | | | (_| |
 *          |_|\___/ \___/|_|\_____|_|  \___/ \__,_| .__/ \_____|_| |_|_|_|\__,_|
 *      ______                                     | |
 *     |______|                                    |_|
 */

/** @brief Child object.
* @private
*/
struct _ToolGroupChild {
    Fl_Widget*                  widget;
    int                         size;

    /** @brief Create new widget object.
    *
    */
    _ToolGroupChild(Fl_Widget* WIDGET, int SIZE) {
        set(WIDGET);
        set(SIZE);
    }

    /** @brief Set new widget and size.
    *
    */
    void set(Fl_Widget* WIDGET) {
        widget = WIDGET;
    }

    /** @brief Set new size.
    *
    */
    void set(int SIZE) {
        size = SIZE;
    }
};


/*
 *      _______          _  _____
 *     |__   __|        | |/ ____|
 *        | | ___   ___ | | |  __ _ __ ___  _   _ _ __
 *        | |/ _ \ / _ \| | | |_ | '__/ _ \| | | | '_ \
 *        | | (_) | (_) | | |__| | | | (_) | |_| | |_) |
 *        |_|\___/ \___/|_|\_____|_|  \___/ \__,_| .__/
 *                                               | |
 *                                               |_|
 */

/** @brief Create tool group.
*
* @param[in] X    X pos.
* @param[in] Y    Y pos.
* @param[in] W    Width.
* @param[in] H    Height.
* @param[in] l    Optional label.
*/
ToolGroup::ToolGroup(int X, int Y, int W, int H, const char* l) : Fl_Group(X, Y, W, H, l) {
    end();
    clip_children(1);
    resizable(nullptr);

    _pos    = Pos::HORIZONTAL;
    _expand = false;
}

/** @brief Delete all widget data.
*
*/
ToolGroup::~ToolGroup() {
    clear();
}

/** @brief Add child widget.
*
* All horizontal widget have same height.\n
* All vertical widget have same width.\n
*
* @param[in] widget  Child widget.
* @param[in] size    Widget size, number of font units, 0 for equal size.
*
* @return Input widget or NULL if it has been added already.
*/
Fl_Widget* ToolGroup::add(Fl_Widget* widget, unsigned size) {
    if (find(widget) != children()) {
        return nullptr;
    }

    _widgets.push_back(new _ToolGroupChild(widget, size));
    Fl_Group::add(widget);

    return widget;
}

/** @brief Delete all widgets.
*
*/
void ToolGroup::clear() {
    for (auto v : _widgets) {
        delete static_cast<_ToolGroupChild*>(v);
    }

    _widgets.clear();
    Fl_Group::clear();
}

/** @brief Remove child widget.
*
* @param[in] widget  Valid child widget.
*
* @return If found return input child widget, otherwise NULL.
*/
Fl_Widget* ToolGroup::remove(Fl_Widget* widget) {
    if (find(widget) == children()) {
        return nullptr;
    }

    for (auto it = _widgets.begin(); it != _widgets.end(); it++) {
        auto child = static_cast<_ToolGroupChild*>(*it);

        if (child->widget == widget) {
            Fl_Group::remove(widget);
            _widgets.erase(it);
            delete child;

            return widget;
        }
    }

    return nullptr;
}

/** @brief Remove child widget.
*
* @param[in] widget  Valid child widget.
*
* @return If found return input child widget, otherwise NULL.
*/
Fl_Widget* ToolGroup::replace(Fl_Widget* old_widget, Fl_Widget* new_widget) {
    if (find(old_widget) == children()) {
        return nullptr;
    }

    for (auto it = _widgets.begin(); it != _widgets.end(); it++) {
        auto child = static_cast<_ToolGroupChild*>(*it);

        if (child->widget == old_widget) {
            insert(*new_widget, old_widget);
            child->set(new_widget);
            Fl_Group::remove(old_widget);

            return old_widget;
        }
    }

    return nullptr;
}

/** @brief Resize widget and all child widgets.
*
*/
void ToolGroup::resize(const int X, const int Y, const int W, const int H) {
    Fl_Widget::resize(X, Y, W, H);

    if (children() == 0 || W == 0 || H == 0 || visible() == 0) {
        return;
    }

    auto leftover = (_pos == Pos::HORIZONTAL) ? W : H;
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
            if (_pos == Pos::HORIZONTAL) {
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
            else if (_pos == Pos::VERTICAL) {
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

/** @brief Resize child widget.
*
* All horizontal widget have same height.\n
* All vertical widget have same width.\n
*
* @param[in] widget  Child widget.
* @param[in] size    Widget size, number of font units, 0 for equal size.
*/
void ToolGroup::size(Fl_Widget* widget, unsigned size) {
    for (auto v : _widgets) {
        auto child = static_cast<_ToolGroupChild*>(v);

        if (child->widget == widget) {
            child->set(size);
            return;
        }
    }
}

/** @brief Resize all child widgets.
*
* All horizontal widget have same height.\n
* All vertical widget have same width.\n
*
* @param[in] widget  Child widget.
* @param[in] size    Widget size, number of font units, 0 for equal size.
*/
void ToolGroup::size(unsigned size) {
    for (auto v : _widgets) {
        auto child = static_cast<_ToolGroupChild*>(v);

        child->set(size);
    }

    do_layout();
}

} // flw

// MKALGAM_OFF
