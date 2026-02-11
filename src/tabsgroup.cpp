/**
* @file
* @brief Tab group widget.
*
* @author gnuwimp@gmail.com
* @copyright Released under the GNU General Public License v3.0
*/

#include "tabsgroup.h"

// MKALGAM_ON

#include <FL/Fl_Scroll.H>
#include <FL/Fl_Toggle_Button.H>
#include <FL/fl_draw.H>
#include <algorithm>

namespace flw {
namespace priv {

/*
 *        _______    _          _____                       ____        _   _
 *       |__   __|  | |        / ____|                     |  _ \      | | | |
 *          | | __ _| |__  ___| |  __ _ __ ___  _   _ _ __ | |_) |_   _| |_| |_ ___  _ __
 *          | |/ _` | '_ \/ __| | |_ | '__/ _ \| | | | '_ \|  _ <| | | | __| __/ _ \| '_ \
 *          | | (_| | |_) \__ \ |__| | | | (_) | |_| | |_) | |_) | |_| | |_| || (_) | | | |
 *          |_|\__,_|_.__/|___/\_____|_|  \___/ \__,_| .__/|____/ \__,_|\__|\__\___/|_| |_|
 *      ______                                       | |
 *     |______|                                      |_|
 */

/** @brief Tab button.
* @private
*
* It stores also its accompanying widget,
*/
class _TabsGroupButton : public Fl_Toggle_Button {
public:
    int        tw;     // Width of label when button are on top or bottom.
    Fl_Widget* widget; // Child widget.

    /** @brief Create tab button.
    *
    * @param[in] box_type   Tab box.
    * @param[in] dbox_type  Down box.
    * @param[in] color      Selection color.
    * @param[in] label      Label text.
    * @param[in] widget_    Child widget.
    * @param[in] o          Pointer to parent TabsGroup.
    */
    _TabsGroupButton(Fl_Boxtype box_type, Fl_Boxtype dbox_type, Fl_Color color, const std::string& label, Fl_Widget* widget_, void* o) :
    Fl_Toggle_Button(0, 0, 0, 0) {
        tw     = 0;
        widget = widget_;

        copy_label(label.c_str());
        tooltip("");
        when(FL_WHEN_CHANGED);
        callback(TabsGroup::CallbackButton, o);
        selection_color(color);
        labelfont(flw::PREF_FONT);
        labelsize(flw::PREF_FONTSIZE);
        box(FL_THIN_UP_BOX);
        down_box(FL_NO_BOX);

        if (box_type != FL_MAX_BOXTYPE) {
            box(box_type);
        }

        if (dbox_type != FL_MAX_BOXTYPE) {
            down_box(dbox_type);
        }
    }

};

/** @brief Group widget.
* @private
*/
class _TabsGroup : public Fl_Group {
public:
    /** @brief Create group.
    *
    */
    _TabsGroup(int X, int Y, int W, int H) :
    Fl_Group(X, Y, W, H) {
    }

    /** @brief Resize widget only, not children.
    *
    */
    void resize(int X, int Y, int W, int H) {
        Fl_Widget::resize(X, Y, W, H);
    }
};

} // flw::priv
} // flw

/*
 *      _______    _          _____
 *     |__   __|  | |        / ____|
 *        | | __ _| |__  ___| |  __ _ __ ___  _   _ _ __
 *        | |/ _` | '_ \/ __| | |_ | '__/ _ \| | | | '_ \
 *        | | (_| | |_) \__ \ |__| | | | (_) | |_| | |_) |
 *        |_|\__,_|_.__/|___/\_____|_|  \___/ \__,_| .__/
 *                                                 | |
 *                                                 |_
 */

/** @brief Create tab group.
*
* By default tabs will be set to north/top of widget.
*
* @param[in] X  X pos.
* @param[in] Y  Y pos.
* @param[in] W  Width.
* @param[in] H  Height.
* @param[in] l  Optional label.
*/
flw::TabsGroup::TabsGroup(int X, int Y, int W, int H, const char* l) : Fl_Group(X, Y, W, H, l) {
    end();
    clip_children(1);
    resizable(nullptr);

    _scroll   = new Fl_Scrollbar(0, 0, 0, 0);
    _tabs     = new priv::_TabsGroup(0, 0, 0, 0);
    _active1  = -1;
    _active2  = -1;
    _color    = FL_SELECTION_COLOR;
    _e        = 0;
    _keyboard = true;
    _n        = 0;
    _s        = 0;
    _down_box = FL_MAX_BOXTYPE;
    _up_box   = FL_MAX_BOXTYPE;
    _visible  = 0;
    _w        = 0;
    _width1   = flw::PREF_FONTSIZE * flw::TabsGroup::DEFAULT_VER_TAB_WIDTH;
    _width2   = flw::TabsGroup::DEFAULT_MAX_HOR_TAB_WIDTH;

    _tabs->end();
    _tabs->add(_scroll);
    Fl_Group::add(_tabs);

    _scroll->linesize(4);
    _scroll->value(1, 1, 1, 16);
    _scroll->callback(TabsGroup::CallbackScrollbar, this);
    _tabs->tooltip(TabsGroup::Help());
    _tabs->box(FL_NO_BOX);
    _tabs->clip_children(1);

    tab_pos(Pos::TOP);
    update_pref();
}

/** @brief Activate and show button and child widget.
*
* @param[in] widget  Either button widget or its user widget, can be NULL.
*/
void flw::TabsGroup::_activate(Fl_Widget* widget) {
    if (widget == nullptr) {
        return;
    }

    auto count   = 0;
    auto current = _active1;
    auto shown   = true;

    _active1 = -1;

    for (auto button : _widgets) {
        auto b = static_cast<priv::_TabsGroupButton*>(button);

        if (b == widget || b->widget == widget) {
            shown = b->widget->visible();

            _active1 = count;
            _active2 = (current != _active1) ? current : _active2;
            b->value(1);
            b->widget->show();
            b->widget->take_focus();
        }
        else {
            b->value(0);
            b->widget->hide();
        }

        count++;
    }

    do_layout();

    auto but = _active_button();

    if (but == nullptr) {
        return;
    }

    if (is_tabs_horizontal() ==  true) {
        if (but->x() < _scroll->x()) {
            _scroll->value(_active1 + 1);
            do_layout();
        }
        else if (but->x() + but->w() > x() + w()) {
            _scroll->value(_active1 - _visible + 2);
            do_layout();
        }
    }
    else if (is_tabs_vertical() == true) {
        if (but->y() < _scroll->y()) {
            _scroll->value(_active1 + 1);
            do_layout();
        }
        else if (but->y() + but->h() > h() + y()) {
            _scroll->value(_active1 - _visible + 2);
            do_layout();
        }
    }

    _resize_active_widget();
    Fl::redraw();

    if (shown == false) {
        do_callback(FL_REASON_SELECTED);
    }
}

/** @brief Get active tab button.
*
* @return Button or NULL.
*/
Fl_Widget* flw::TabsGroup::_active_button() {
    return _active1 >= 0 && _active1 < static_cast<int>(_widgets.size()) ?
        _widgets[_active1] :
        nullptr;
}

/** @brief Add child widget.
*
* If no after widget is used it will be added last.
*
* @param[in] label    Label for this widget.
* @param[in] widget   Child widget.
* @param[in] after    Add widget after this, optional.
* @param[in] tooltip  Tooltip, optional.
*/
void flw::TabsGroup::add(const std::string& label, Fl_Widget* widget, const Fl_Widget* after, const std::string& tooltip) {
    if (find(widget) != -1) {
        return;
    }

    auto button = new priv::_TabsGroupButton(_up_box, _down_box, _color, label, widget, this);
    auto idx    = (after != nullptr) ? find(after) : static_cast<int>(_widgets.size());

    button->copy_tooltip(tooltip.c_str());

    if (idx < 0 || idx >= static_cast<int>(_widgets.size()) - 1) {
        Fl_Group::add(widget);
        _tabs->add(button);
        _widgets.push_back(button);
    }
    else {
        idx++;
        auto b = static_cast<priv::_TabsGroupButton*>(_widgets[idx]);
        Fl_Group::insert(*widget, b->widget);
        _tabs->insert(*button, b);
        _widgets.insert(_widgets.begin() + idx, button);
    }

    flw::TabsGroup::CallbackButton(button, this);
}

/** @brief Tab button has been activated so switch to new widget.
*
* @param[in] sender  Tab button.
* @param[in] object  This object.
*/
void flw::TabsGroup::CallbackButton(Fl_Widget* sender, void* object) {
    static_cast<TabsGroup*>(object)->_activate(sender);
}

/** @brief Scroll buttons.
*
* @param[in] sender  Scrollbar button.
* @param[in] object  This object.
*/
void flw::TabsGroup::CallbackScrollbar(Fl_Widget*, void* object) {
    auto self = static_cast<TabsGroup*>(object);

    if (self->is_tabs_horizontal() ==  true) {
        self->_resize_top_bottom(self->x(), self->y(), self->w(), self->h());
    }
    else if (self->is_tabs_vertical() == true) {
        self->_resize_left_right(self->x(), self->y(), self->w(), self->h());
    }

    self->redraw();
    self->_tabs->redraw_label(); //!!! Fixes failed button redraws.
}

/** @brief Get tab widget.
*
* @param[in] index  Child index.
*
* @return Widget or NULL.
*/
Fl_Widget* flw::TabsGroup::child(int index) const {
    return (index >= 0 && index < static_cast<int>(_widgets.size())) ?
        static_cast<priv::_TabsGroupButton*>(_widgets[index])->widget :
        nullptr;
}

/** @brief Remove all widgets.
*
*/
void flw::TabsGroup::clear() {
    _active1 = -1;
    _active2 = -1;

    _widgets.clear();

    _tabs->remove(_scroll);
    Fl_Group::remove(_tabs);
    _tabs->clear();
    Fl_Group::clear();

    Fl_Group::add(_tabs);
    _tabs->add(_scroll);
    update_pref();
    do_layout();
    Fl::redraw();
}

/** @brief Print some debug info.
*
* @param[in] all  True to print child widgets.
*/
void flw::TabsGroup::debug(bool all) const {
#ifdef DEBUG
    puts("------------------------------------------------------------");
    auto w = ((TabsGroup*) this)->_active_button();
    printf("TabsGroup ==>\n");
    printf("    _tab_pos  = %d\n", (int) _tab_pos);
    printf("    active    = %s\n", w ? w->label() : "NULL");
    printf("    _active1  = %d\n", _active1);
    printf("    _active2  = %d\n", _active2);
    printf("    _drag     = %d\n", _drag);
    printf("    _width1   = %d\n", _width1);
    printf("    _width2   = %d\n", _width2);
    printf("    _keyboard = %d\n", _keyboard);
    printf("    _widgets  = %d\n", static_cast<int>(_widgets.size()));
    printf("    _visible  = %d\n", _visible);
    printf("    _area     = %d - %d :: %d - %d\n", _area.x(), _area.y(), _area.w(), _area.h());
    printf("    scrollbar = %d - %d\n", (int) _scroll->value(), (int) _scroll->maximum());
    printf("    children  = %d\n", children());
    printf("\n");

    if (all == true) {
        flw::debug::print(this->_tabs);
        printf("\n");

        int f = 0;
        for (auto widget : _widgets) {
            auto b = static_cast<priv::_TabsGroupButton*>(widget);
            printf("%2d: %s\n", f++, b->label());
        }
    }

    puts("------------------------------------------------------------");
    fflush(stdout);
#else
    (void) all;
#endif
}

/** @brief Find index for widget.
*
* @param[in] widget  Widget to find.
*
* @return Index or -1 if not found, 0 is the first widget.
*/
int flw::TabsGroup::find(const Fl_Widget* widget) const {
    auto num = 0;

    for (const auto W : _widgets) {
        const auto b = static_cast<priv::_TabsGroupButton*>(W);

        if (b->widget == widget) {
            return num;
        }
        else {
            num++;
        }
    }

    return -1;
}

/** @brief Handle events.
*
* If tabs are left/right then check for mouse drag events and resize width of buttons.\n
* Check also for key events, tab buttons can be selected with alt + 0-9.\n
* And they can also be moved with alt+shift+left/right keys.\n
*
* @param[in] event  Event value.
*
* @return Either 1 from this widget or value from parent widget.
*/
int flw::TabsGroup::handle(int event) {
    if (is_tabs_vertical() == true) { // Only left/right mode can resize width by using mouse.
        if (event == FL_DRAG) {
            if (_drag == true) {
                auto pos   = 0;
                auto width = flw::PREF_FONTSIZE * flw::TabsGroup::MIN_WIDTH;

                if (is_tabs_left() == true) {
                    pos = Fl::event_x() - x();
                }
                else if (is_tabs_right() == true) {
                    pos = x() + w() - Fl::event_x();
                }

                if (pos != _width1 && pos >= width && pos <= w() - width) {
                    _width1 = pos;
                    do_layout();
                }

                return 1;
            }
        }
        else if (event == FL_LEAVE) {
            _drag = false;
            fl_cursor(FL_CURSOR_DEFAULT);
        }
        else if (event == FL_MOVE) {
            auto event_x = Fl::event_x();

            if (_tab_pos == Pos::LEFT || _tab_pos == Pos::LEFT2) {
                auto pos = x() + _width1;

                if (event_x > (pos - 3) && event_x <= (pos + 3)) {
                    if (_drag == false) {
                        _drag = true;
                        fl_cursor(FL_CURSOR_WE);
                    }

                    return 1;
                }
            }
            else if (_tab_pos == Pos::RIGHT || _tab_pos == Pos::RIGHT2) {
                auto pos = x() + w() - _width1;

                if (event_x > (pos - 3) && event_x <= (pos + 3)) {
                    if (_drag == false) {
                        _drag = true;
                        fl_cursor(FL_CURSOR_WE);
                    }

                    return 1;
                }
            }

            if (_drag == true) {
                _drag = false;
                fl_cursor(FL_CURSOR_DEFAULT);
            }
        }
        else if (event == FL_PUSH) {
            if (_drag == true) {
                return 1;
            }
        }
    }

    if (event == FL_KEYBOARD && _keyboard == true) { // Select tab by using keyboard.
        auto key   = Fl::event_key();
        auto alt   = Fl::event_alt() != 0;
        auto alt2  = alt;
        auto shift = Fl::event_shift() != 0;

#ifdef __APPLE__
        alt2 = Fl::event_command() != 0;
#endif

        if (_widgets.size() < 2) {
        }
        else if (alt2 == true && key >= '0' && key <= '9') { // Select tab from the ten first tabs.
            auto tab = key - '0';
            tab = (tab == 0) ? 9 : tab - 1;

            if (tab < static_cast<int>(_widgets.size())) {
                flw::TabsGroup::CallbackButton(_widgets[tab], this);
            }
            return 1;
        }
        else if (alt == true && shift == true && (key == FL_Left || key == FL_Up)) { // Move tab.
            // Fix for wrapping around tabs.
            if (_active1 - 1 == _active2) {
                _active2 = _active1;
            }
            else if (_active1 == 0 && _widgets.size() > 1) {
                _active2--;
            }

            auto tab = swap(_active1, _active1 - 1);
            flw::TabsGroup::CallbackButton(_widgets[tab], this);
            return 1;
        }
        else if (alt == true && shift == true && (key == FL_Right || key == FL_Down)) { // Move tab.
            // Fix for wrapping around tabs.
            if (_active1 + 1 == _active2) {
                _active2 = _active1;
            }
            else if (_active1 == static_cast<int>(_widgets.size()) - 1 && _widgets.size() > 1) {
                _active2++;
            }

            auto tab = swap(_active1, _active1 + 1);
            flw::TabsGroup::CallbackButton(_widgets[tab], this);
            return 1;
        }
        else if (alt == true && key == FL_Left) { // Switch tab.
            auto tab = (_active1 == 0) ? static_cast<int>(_widgets.size()) - 1 : _active1 - 1;
            flw::TabsGroup::CallbackButton(_widgets[tab], this);
            return 1;
        }
        else if (alt == true && key == FL_Right) { // Switch tab.
            auto tab = (_active1 == static_cast<int>(_widgets.size()) - 1) ? 0 : _active1 + 1;
            flw::TabsGroup::CallbackButton(_widgets[tab], this);
            return 1;
        }
        else if (alt == true && (key == FL_Up || key == FL_Down)) { // Switch between last two tabs.
            auto tab = (_active2 == -1) ? _active1 : _active2;
            flw::TabsGroup::CallbackButton(_widgets[tab], this);
            return 1;
        }
    }
    else if (event == FL_FOCUS) {
        auto widget = value();

        if (widget != nullptr && widget != Fl::focus()) {
            widget->take_focus();
            return 1;
        }
    }

    return Fl_Group::handle(event);
}

/** @brief Get help text.
*
* @return Help text.
*/
const char* flw::TabsGroup::Help() {
    static const char* const HELP =
    "Use alt + left/right to move between tabs.\n"
    "Use alt + up/down to jump between two last widgets.\n"
    "Or alt + [1 - 9, 0] to select tab from 1 - 10.\n"
    "And alt + shift + left/right to move tabs.\n"
    "Tabs on the left/right side can have its width changed by dragging the mouse.";

    return HELP;
}

/** @brief Hide all tab buttons.
*
*/
void flw::TabsGroup::hide_tabs() {
    _tabs->hide();
    do_layout();
}

/** @brief Insert child widget.
*
* If "before" widget is NULL then the widget will be inserted first.
*
* @param[in] label    Label for this widget.
* @param[in] widget   Child widget.
* @param[in] before   Insert before this, optional.
* @param[in] tooltip  Tab tooltip, optional.
*/
void flw::TabsGroup::insert(const std::string& label, Fl_Widget* widget, const Fl_Widget* before, const std::string& tooltip) {
    if (find(widget) != -1) {
        return;
    }

    auto button = new priv::_TabsGroupButton(_up_box, _down_box, _color, label, widget, this);
    auto idx    = (before != nullptr) ? find(before) : 0;

    button->copy_tooltip(tooltip.c_str());

    if (idx >= static_cast<int>(_widgets.size())) {
        Fl_Group::add(widget);
        _tabs->add(button);
        _widgets.push_back(button);
    }
    else {
        auto b = static_cast<priv::_TabsGroupButton*>(_widgets[idx]);
        Fl_Group::insert(*widget, b->widget);
        _tabs->insert(*button, b);
        _widgets.insert(_widgets.begin() + idx, button);
    }

    flw::TabsGroup::CallbackButton(button, this);
    do_layout();
}

/** @brief Remove child widget.
*
* @param[in] index  Child index.
*
* @return Removed child widget or NULL.
*/
Fl_Widget* flw::TabsGroup::remove(int index) {
    if (index < 0 || index >= static_cast<int>(_widgets.size())) {
        return nullptr;
    }

    auto button = static_cast<priv::_TabsGroupButton*>(_widgets[index]);
    auto res    = button->widget;

    _widgets.erase(_widgets.begin() + index);
    remove(res);
    _tabs->remove(button);
    delete button;

    if (index < _active1) {
        _active1--;
    }
    else if (_active1 == static_cast<int>(_widgets.size())) {
        _active1 = static_cast<int>(_widgets.size()) - 1;
    }

    do_layout();
    flw::TabsGroup::CallbackButton(_active_button(), this);

    return res;
}

/** @brief Resize group.
*
*/
void flw::TabsGroup::resize(int X, int Y, int W, int H) {
    Fl_Widget::resize(X, Y, W, H);

    if (W == 0 || H == 0) {
        return;
    }

    if (_tabs->visible() == 0) {
        _area = Fl_Rect(X + _w, Y + _n, W - _w - _e, H - _n - _s);
    }
    else {
        fl_font(flw::PREF_FONT, flw::PREF_FONTSIZE);

        if (is_tabs_horizontal() == true) {
            _resize_top_bottom(X, Y, W, H);
        }
        else if (is_tabs_vertical() == true) {
            _resize_left_right(X, Y, W, H);
        }
    }

    _resize_active_widget();
    Fl::redraw();
}

/** @brief Resize child widget (only the visible one).
*
*/
void flw::TabsGroup::_resize_active_widget() {
    auto but = static_cast<priv::_TabsGroupButton*>(_active_button());

    if (but == nullptr) {
        return;
    }

    if (Fl_Rect(but->widget) != _area) {
        but->widget->resize(_area.x(), _area.y(), _area.w(), _area.h());
        Fl::redraw();
    }
}

/** @brief Resize tab buttons if they are on the left or right side.
*
*/
void flw::TabsGroup::_resize_left_right(int X, int Y, int W, int H) {
    auto height = flw::PREF_FONTSIZE + flw::TabsGroup::HEIGHT;
    auto pack_h = (height + _space) * static_cast<int>(_widgets.size()) - _space;
    auto scroll = 0;

    _visible = static_cast<int>(H / (height + _space));

    if (_width1 < flw::PREF_FONTSIZE * flw::TabsGroup::MIN_WIDTH) { // Set min size for widgets on the left.
        _width1 = flw::PREF_FONTSIZE * flw::TabsGroup::MIN_WIDTH;
    }
    else if (_width1 > W - flw::PREF_FONTSIZE * flw::TabsGroup::MIN_WIDTH) { // Set min size for widgets on the right.
        _width1 = W - flw::PREF_FONTSIZE * flw::TabsGroup::MIN_WIDTH;
    }
    if (pack_h > H) {
        auto size = static_cast<int>(_widgets.size() - _visible + 2);

        scroll = Fl::scrollbar_size();
        _scroll->range(1, size > 0 ? size : 1);
        _scroll->show();
    }
    else {
        _scroll->value(1);
        _scroll->resize(0, 0, 0, 0);
        _scroll->hide();
    }

    if (is_tabs_left() == true) {
        _tabs->resize(X, Y, _width1, H);
        _area = Fl_Rect(X + _width1 + _w, Y + _n, W - _width1 - _w - _e, H - _n - _s);

        if (_tab_pos == Pos::LEFT) {
            _scroll->resize(_tabs->x(), _tabs->y(), scroll, _tabs->h());
        }
        else if (_tab_pos == Pos::LEFT2) {
            _scroll->resize(_tabs->x() + _width1 - scroll, _tabs->y(), scroll, _tabs->h());
        }
    }
    else if (is_tabs_right() == true) {
        _tabs->resize(X + W - _width1, Y, _width1, H);
        _area = Fl_Rect(X + _w, Y + _n, W - _width1 - _w - _e, H - _n - _s);

        if (_tab_pos == Pos::RIGHT) {
            _scroll->resize(_tabs->x() + _tabs->w() - scroll, _tabs->y(), scroll, _tabs->h());
        }
        else if (_tab_pos == Pos::RIGHT2) {
            _scroll->resize(_tabs->x(), _tabs->y(), scroll, _tabs->h());
        }
    }

    auto ypos = -(_scroll->value() - 1);
    ypos *= height + _space;
    ypos += _tabs->y();

    for (auto widget : _widgets) {
        auto b  = static_cast<priv::_TabsGroupButton*>(widget);

        b->tw = 0;
        b->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE | FL_ALIGN_CLIP);

        if (_tab_pos == Pos::LEFT) {
            b->resize(_tabs->x() + scroll, ypos, _tabs->w() - scroll, height);
        }
        else if (_tab_pos == Pos::LEFT2) {
            b->resize(_tabs->x(), ypos, _tabs->w() - scroll, height);
        }
        else if (_tab_pos == Pos::RIGHT) {
            b->resize(_tabs->x(), ypos, _tabs->w() - scroll, height);
        }
        else if (_tab_pos == Pos::RIGHT2) {
            b->resize(_tabs->x() + scroll, ypos, _tabs->w() - scroll, height);
        }

        ypos += height + _space;
    }
}

/** @brief Resize tab buttons if they are on the top or bottom side.
*
*/
void flw::TabsGroup::_resize_top_bottom(int X, int Y, int W, int H) {
    auto height = flw::PREF_FONTSIZE + flw::TabsGroup::HEIGHT;
    auto pack_w = 0;
    auto scroll = 0;
    auto width  = 0;
    auto minw   = flw::PREF_FONTSIZE * flw::TabsGroup::MIN_WIDTH + _space;
    auto maxw   = flw::PREF_FONTSIZE * _width2 + _space;

    _visible = 0;
    fl_font(labelfont(), labelsize());

    for (auto widget : _widgets) { // Check if horizontal scrollbar has to be visible.
        auto b  = static_cast<priv::_TabsGroupButton*>(widget);
        auto th = 0;

        b->tw = 0;
        fl_measure(b->label(), b->tw, th);

        if (b->tw + flw::PREF_FONTSIZE > width) {
            width = b->tw + flw::PREF_FONTSIZE;
        }
    }

    if (width < minw) {
        width = minw;
    }
    else if (width > maxw) {
        width = maxw;
    }

    pack_w   = (width + _space) * static_cast<int>(_widgets.size()) - _space;
    _visible = static_cast<int>(W / (width + _space));

    if (pack_w - _space > W) {
        auto size = static_cast<int>(_widgets.size() - _visible + 2);

        scroll = Fl::scrollbar_size();
        _scroll->show();
        _scroll->range(1, size > 0 ? size : 1);
    }

    else {
        _scroll->value(1);
        _scroll->resize(0, 0, 0, 0);
        _scroll->hide();
    }

    if (is_tabs_top() == true) {
        _tabs->resize(X, Y, W, height + scroll);
        _area = Fl_Rect(X + _w, Y + height + scroll + _n, W - _w - _e, H - height - scroll - _n - _s);

        if (_tab_pos == Pos::TOP) {
            _scroll->resize(_tabs->x(), _tabs->y(), _tabs->w(), scroll);
        }
        else if (_tab_pos == Pos::TOP2) {
            _scroll->resize(_tabs->x(), _tabs->y() + height, _tabs->w(), scroll);
        }
    }
    else if (is_tabs_bottom() == true) {
        _tabs->resize(X, Y + H - height - scroll, W, height + scroll);
        _area = Fl_Rect(X + _w, Y + _n, W - _w - _e, H - height - scroll - _n - _s);

        if (_tab_pos == Pos::BOTTOM) {
            _scroll->resize(_tabs->x(), _tabs->y() + _tabs->h() - scroll, _tabs->w(), scroll);
        }
        else if (_tab_pos == Pos::BOTTOM2) {
            _scroll->resize(_tabs->x(), _tabs->y(), _tabs->w(), scroll);
        }
    }

    auto xpos = -(_scroll->value() - 1);
    xpos *= width + _space;
    xpos += _tabs->x();

    for (auto widget : _widgets) {
        auto b  = static_cast<priv::_TabsGroupButton*>(widget);

        if (b->tw >= width) {
            b->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE | FL_ALIGN_CLIP);
        }
        else {
            b->align(FL_ALIGN_CENTER | FL_ALIGN_INSIDE | FL_ALIGN_CLIP);
        }

        if (_tab_pos == Pos::TOP) {
            b->resize(xpos, _tabs->y() + scroll, width, height);
        }
        else if (_tab_pos == Pos::TOP2) {
            b->resize(xpos, _tabs->y(), width, height);
        }
        else if (_tab_pos == Pos::BOTTOM) {
            b->resize(xpos, _tabs->y() + _tabs->h() - height - scroll, width, height);
        }
        else if (_tab_pos == Pos::BOTTOM2) {
            b->resize(xpos, _tabs->y() + _tabs->h() - height, width, height);
        }

        xpos += width + _space;
    }
}

/** @brief Show tab buttons.
*
*/
void flw::TabsGroup::show_tabs() {
    _tabs->show();
    do_layout();
}

/** @brief Sort tab buttons using the labels.
*
* Sort widget array then set pointers manually in the pack widget.
*
* @param[in] ascending    True to sort ascending, false for descending order.
* @param[in] casecompare  True use case compare.
*
*/
void flw::TabsGroup::sort(bool ascending, bool casecompare) {
    auto pack = const_cast<Fl_Widget**>(_tabs->array());
    auto butt = _active_button();

    if (ascending == true && casecompare == true) {
        std::sort(_widgets.begin(), _widgets.end(), [](const Fl_Widget* a, const Fl_Widget* b) { return strcmp(a->label(), b->label()) < 0; });
    }
    else if (ascending == true && casecompare == false) {
        std::sort(_widgets.begin(), _widgets.end(), [](const Fl_Widget* a, const Fl_Widget* b) { return fl_utf_strcasecmp(a->label(), b->label()) < 0; });
    }
    else if (ascending == false && casecompare == true) {
        std::sort(_widgets.begin(), _widgets.end(), [](const Fl_Widget* a, const Fl_Widget* b) { return strcmp(b->label(), a->label()) < 0; });
    }
    else if (ascending == false && casecompare == false) {
        std::sort(_widgets.begin(), _widgets.end(), [](const Fl_Widget* a, const Fl_Widget* b) { return fl_utf_strcasecmp(b->label(), a->label()) < 0; });
    }

    for (int f = 1; f < _tabs->children(); f++) {
        pack[f] = _widgets[f - 1];

        if (_widgets[f - 1] == butt) {
            _active1 = f - 1;
        }
    }

    _activate(butt);
}

/** @brief Swap two tab buttons/widgets.
*
* @param[in] from  From index.
* @param[in] to    To index.
*
* @return Active index or -1.
*/
int flw::TabsGroup::swap(int from, int to) {
    auto last = static_cast<int>(_widgets.size()) - 1;

    if (_widgets.size() < 2 || to < -1 || to > static_cast<int>(_widgets.size())) {
        return _active1;
    }

    auto active = (_active1 == from);
    auto pack   = const_cast<Fl_Widget**>(_tabs->array()); // Ignore widget 0 which is the scrollbar.

    if (from == 0 && to == -1) { // Move button from top to last.
        auto widget = _widgets[0];

        for (int f = 1; f <= last; f++) {
            _widgets[f - 1] = _widgets[f];
            pack[f] = pack[f + 1];
        }

        pack[last + 1] = widget;
        _widgets[last] = widget;

        if (active == true) {
            _active1 = last;
        }

        util::swap_rect(_widgets[last - 1], _widgets[last]);
    }
    else if (from == last && to == static_cast<int>(_widgets.size())) { // Move button from last to first.
        auto widget = _widgets[last];

        for (int f = last; f > 0; f--) {
            _widgets[f] = _widgets[f - 1];
            pack[f + 1] = pack[f];
        }

        pack[1] = widget;
        _widgets[0] = widget;

        if (active == true) {
            _active1 = 0;
        }

        util::swap_rect(_widgets[0], _widgets[1]);
    }
    else { // Swap two buttons.
        auto widget = _widgets[from];

        pack[from + 1] = pack[to + 1];
        pack[to + 1] = widget;
        _widgets[from] = _widgets[to];
        _widgets[to] = widget;

        if (active == true) {
            _active1 = to;
        }

        util::swap_rect(_widgets[from], _widgets[to]);
    }

    return _active1;
}

/** @brief Set tab button boxtype.
*
* @param[in] up_box    Use FL_MAX_BOXTYPE to reset to default.
* @param[in] down_box  Use FL_MAX_BOXTYPE to reset to default.
*/
void flw::TabsGroup::tab_box(Fl_Boxtype up_box, Fl_Boxtype down_box) {
    _up_box   = up_box;
    _down_box = down_box;

    for (auto widget : _widgets) {
        auto b = static_cast<priv::_TabsGroupButton*>(widget);

        if (_up_box == FL_MAX_BOXTYPE) {
            b->box(FL_THIN_UP_BOX);
        }
        else {
            b->box(_up_box);
        }

        if (_down_box == FL_MAX_BOXTYPE) {
            b->down_box(FL_NO_BOX);
        }
        else {
            b->down_box(_down_box);
        }
    }
}

/** @brief Set tab selection color.
*
* @param[in] color  Color, if different than FL_SELECTION_COLOR it will not change when theme changes.
*/
void flw::TabsGroup::tab_color(Fl_Color color) {
    _color = color;

    for (auto widget : _widgets) {
        auto b = static_cast<priv::_TabsGroupButton*>(widget);
        b->selection_color(_color);
    }
}

/** @brief Get current tab button label.
*
* @return Label string or empty string.
*/
std::string flw::TabsGroup::tab_label() {
    return tab_label(value());
}

/** @brief Get tab button label.
*
* @param[in] widget  Child widget to get tab button label for.
*
* @return Label string or empty string.
*/
std::string flw::TabsGroup::tab_label(const Fl_Widget* widget) {
    auto num = find(widget);

    if (num == -1) {
        return "";
    }

    return _widgets[num]->label();
}

/** @brief Set tab button label.
*
* @param[in] label   New label.
* @param[in] widget  Child widget to set tab button label for.
*/
void flw::TabsGroup::tab_label(const std::string& label, Fl_Widget* widget) {
    auto num = find(widget);

    if (num == -1) {
        return;
    }

    _widgets[num]->copy_label(label.c_str());
}

/** @brief Set on which side the tab buttons should be.
*
* @param[in] pos    One of Pos values.
* @param[in] space  Space between tab buttons (0 - flw::TabsGroup::MAX_SPACE pixels).
*/
void flw::TabsGroup::tab_pos(Pos pos, int space) {
    _tab_pos = pos;
    _space   = (space >= 0 && space <= flw::TabsGroup::MAX_SPACE) ? space : flw::TabsGroup::DEFAULT_SPACE;

    if (is_tabs_horizontal() == true) {
        _scroll->type(FL_HORIZONTAL);
    }
    else if (is_tabs_vertical() == true) {
        _scroll->type(FL_VERTICAL);
    }

    _scroll->value(1);
    _activate(_active_button());
}

/** @brief Get tab button tooltip.
*
* @param[in] widget  Child widget to get tooltip for.
*
* @return Tooltip label.
*/
std::string flw::TabsGroup::tooltip(Fl_Widget* widget) const {
    auto num = find(widget);

    if (num == -1) {
        return "";
    }

    return flw::util::to_string(_widgets[num]->tooltip());
}

/** @brief Set tab button tooltip.
*
* @param[in] tooltip  New tooltip.
* @param[in] widget   Child widget to set tab button tooltip for.
*/
void flw::TabsGroup::tooltip(const std::string& tooltip, Fl_Widget* widget) {
    auto num = find(widget);

    if (num == -1) {
        return;
    }

    _widgets[num]->copy_tooltip(tooltip.c_str());
}

/** @brief Update font preferences.
*
* @param[in] font      Font to use.
* @param[in] fontsize  Size of font.
*/
void flw::TabsGroup::update_pref(Fl_Font font, Fl_Fontsize fontsize) {
    _drag = false;

    labelfont(font);
    labelsize(fontsize);
    _width1 = flw::PREF_FONTSIZE * flw::TabsGroup::DEFAULT_VER_TAB_WIDTH;
    _width2 = flw::TabsGroup::DEFAULT_MAX_HOR_TAB_WIDTH;

    for (auto widget : _widgets) {
        widget->labelfont(font);
        widget->labelsize(fontsize);
    }
}

/** @brief Get current active widget.
*
* @return Widget or NULL.
*/
Fl_Widget* flw::TabsGroup::value() const {
    return _active1 >= 0 && _active1 < static_cast<int>(_widgets.size()) ?
        static_cast<priv::_TabsGroupButton*>(_widgets[_active1])->widget :
        nullptr;
}

/** @brief Set active widget.
*
* @param[in] num  Valid index.
*/
void flw::TabsGroup::value(int num) {
    if (num >= 0 && num < static_cast<int>(_widgets.size())) {
        _activate(_widgets[num]);
    }
}

// MKALGAM_OFF
