/**
* @file
* @brief A simple layout widget that uses a grid for placing widgets.
*
* @author gnuwimp@gmail.com
* @copyright Released under the GNU General Public License v3.0
*/

#include "gridgroup.h"

// MKALGAM_ON

namespace flw {

/*
 *           _____      _     _  _____                        _____ _     _ _     _
 *          / ____|    (_)   | |/ ____|                      / ____| |   (_) |   | |
 *         | |  __ _ __ _  __| | |  __ _ __ ___  _   _ _ __ | |    | |__  _| | __| |
 *         | | |_ | '__| |/ _` | | |_ | '__/ _ \| | | | '_ \| |    | '_ \| | |/ _` |
 *         | |__| | |  | | (_| | |__| | | | (_) | |_| | |_) | |____| | | | | | (_| |
 *          \_____|_|  |_|\__,_|\_____|_|  \___/ \__,_| .__/ \_____|_| |_|_|_|\__,_|
 *      ______                                        | |
 *     |______|                                       |_|
 */

/** @brief Every widget is stored in a _GridGroupChild.
* @private
*/
struct _GridGroupChild {
    Fl_Widget*                  widget; // Child widget.
    Fl_Widget*                  focus;  // Focus on this widget.
    bool                        g;      // If g is false treat is an normal widget.
    short                       x;      // X pos.
    short                       y;      // Y pos.
    short                       w;      // Width.
    short                       h;      // Height.
    short                       l;      // Adjust left side.
    short                       r;      // Adjust right side.
    short                       t;      // Adjust top side.
    short                       b;      // Adjust bottom side.

    /** @brief Create new child.
    *
    */
    _GridGroupChild(Fl_Widget* WIDGET, Fl_Widget* F, int X, int Y, int W, int H) {
        widget = WIDGET;
        focus = F;
        set(X, Y, W, H);
        adjust();
    }

    /** @brief Adjust its size and position.
    *
    * @param[in] L  Left side.
    * @param[in] R  Right side.
    * @param[in] T  Top side.
    * @param[in] B  Bottom side.
    */
    void adjust(int L = 0, int R = 0, int T = 0, int B = 0) {
        l = L;
        r = R;
        t = T;
        b = B;
    }

    /** @brief Replace child widget.
    *
    */
    void set(int X, int Y, int W, int H) {
        x = X;
        y = Y;
        w = W;
        h = H;
    }
};

/*
 *       _____      _     _  _____
 *      / ____|    (_)   | |/ ____|
 *     | |  __ _ __ _  __| | |  __ _ __ ___  _   _ _ __
 *     | | |_ | '__| |/ _` | | |_ | '__/ _ \| | | | '_ \
 *     | |__| | |  | | (_| | |__| | | | (_) | |_| | |_) |
 *      \_____|_|  |_|\__,_|\_____|_|  \___/ \__,_| .__/
 *                                                | |
 *                                                |_|
 */

/** @brief Create new GridGroup.
*
* @param[in] X  X pos.
* @param[in] Y  Y pos.
* @param[in] W  Width.
* @param[in] H  Height.
* @param[in] l  Optional label.
*/
GridGroup::GridGroup(int X, int Y, int W, int H, const char* l) : Fl_Group(X, Y, W, H, l) {
    end();
    clip_children(1);
    resizable(nullptr);

    _size = 0;
}

/** @brief Delete widget.
*
* GridGroup deleted child data.\n
* Parent Fl_Group delete all child widgets.\n
*/
GridGroup::~GridGroup() {
    for (auto v : _widgets) {
        delete static_cast<_GridGroupChild*>(v);
    }
}

/** @brief Add child widget.
*
* X and Y can use negative values.\n
* Then it will calculate positions from right side or from the bottom.\n
* With default values GridGroup::add(WIDGET, 1, 1, 10, 4) will resize widget to (14/2, 14/2, 14 / 2 * 10, 14 / 2 * 4)\n
* \n
* W and H can use negative values.\n
* Then it will adjust width or height from right side or from the bottom.\n
*
* @param[in] widget  Widget to add.
* @param[in] X       X pos using grid coordinates .
* @param[in] Y       Y pos using grid coordinates.
* @param[in] W       Width in grid coordinates.
* @param[in] H       Height in grid coordinates.
* @param[in] F       Use this widget for focus (this was added so InputMenu::input() would get focus).
*/
void GridGroup::add(Fl_Widget* widget, int X, int Y, int W, int H, Fl_Widget* F) {
    _widgets.push_back(new _GridGroupChild(widget, F, X, Y, W, H));
    Fl_Group::add(widget);
}

/** @brief Adjust size and position of a child widget.
*
* @param[in] widget  Child widget.
* @param[in] L       Left side.
* @param[in] R       Right side.
* @param[in] T       Top side.
* @param[in] B       Bottom side.
    */
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

/** @brief Delete all child widgets,
*
* Use this method, not Fl_Group::clear().
*/
void GridGroup::clear() {
    _widgets.clear();
    Fl_Group::clear();
}

/** @brief Handle message.
*
* Take care of moving from widget to widget with tab key.
*
* @param[in] event  Event value.
*
* @return 1 if new widget gets the focus.
*/
int GridGroup::handle(int event) {
    if (event == FL_KEYDOWN && Fl::event_key() == FL_Tab) {
        if (children() > 0) {
            Fl_Widget* first   = nullptr;
            Fl_Widget* last    = nullptr;
            Fl_Widget* current = Fl::focus();

            _last_active_widget(&first, &last);

            //puts("");
            //printf("FIRST: %p: %s\n", first ? first : 0, first && first->label() ? first->label() : "NULL");
            //printf("LAST:  %p: %s\n", last ? last : 0, last && last->label() ? last->label() : "NULL");

            if (Fl::event_shift() == 0 && first != nullptr && current != nullptr && current == last) {
                Fl::focus(first);
                redraw();
                return 1;
            }
            else if (Fl::event_shift() != 0 && first != nullptr && current != nullptr && current == first) {
                Fl::focus(last);
                redraw();
                return 1;
            }
            
            //printf("FOCUS:  %p: %s\n", Fl::focus() ? Fl::focus() : 0, Fl::focus() && Fl::focus()->label() ? Fl::focus()->label() : "NULL");
        }
    }

    return Fl_Group::handle(event);
}

/** @brief Set first and last active widget.
*
* @param[out] first  Set last active widget.
* @param[out] last   Set first active widget.
*/
void GridGroup::_last_active_widget(Fl_Widget** first, Fl_Widget** last) {
    for (auto& v : _widgets) {
        auto child = static_cast<_GridGroupChild*>(v);
        auto g = child->widget->as_group();

        if (g == nullptr || child->focus != nullptr) {
            if (child->widget->active() != 0 && child->focus != nullptr) {
                *last = child->focus;
            }
            else if (child->widget->active() != 0) {
                *last = child->widget;
            }

            if (*first == nullptr && child->widget->active() != 0 && child->focus != nullptr) {
                *first = child->focus;
            }
            else if (*first == nullptr && child->widget->active() != 0) {
                *first = child->widget;
            }
        }
    }
}

/** @brief Remove widget from group.
*
* @param[in] widget  Widget to remove.
*
* @return If input widget is found then it is returned or nullptr.
*/
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

/** @brief Resize widget and all child widgets,
*
* @param[in] X  X pos.
* @param[in] Y  Y pos.
* @param[in] W  Width.
* @param[in] H  Height.
*/
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

/** @brief Resize child widget.
*
* @param[in] widget  Widget to resize.
* @param[in] X       X pos using grid coordinates .
* @param[in] Y       Y pos using grid coordinates.
* @param[in] W       Width in grid coordinates.
* @param[in] H       height in grid coordinates.
*/
void GridGroup::resize(Fl_Widget* widget, int X, int Y, int W, int H) {
    for (auto& v : _widgets) {
        auto child = static_cast<_GridGroupChild*>(v);

        if (child->widget == widget) {
            child->set(X, Y, W, H);
            return;
        }
    }

    #ifdef DEBUG
        fprintf(stderr, "error: flw::GridGroup::resize() failed to find widget (label=%s)\n", widget->label());
    #endif
}

} // flw

// MKALGAM_OFF
