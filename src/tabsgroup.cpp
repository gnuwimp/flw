/**
* @file
* @brief Tab group widget.
*
* @author gnuwimp@gmail.com
* @copyright Released under the GNU General Public License v3.0
*/

#include "tabsgroup.h"

// MKALGAM_ON

#include <assert.h>
#include <algorithm>
#include <FL/Fl_Toggle_Button.H>
#include <FL/fl_draw.H>

namespace flw {

/***
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
*
* It stores also its accompanying widget,
*/
class _TabsGroupButton : public Fl_Toggle_Button {
public:
    int                         tw;     // Width of label when button are on top or bottom.
    Fl_Widget*                  widget; // Child widget.

    /** @brief Create tab button.
    *
    * @param[in] align   Label align.
    * @param[in] label   Label text.
    * @param[in] widget  Child widget.
    * @param[in] o       Pointer to parent TabsGroup.
    */
    explicit _TabsGroupButton(Fl_Align align, const std::string& label, Fl_Widget* widget, void* o) :
    Fl_Toggle_Button(0, 0, 0, 0) {
        tw           = 0;
        this->widget = widget;

        this->align(align);
        copy_label(label.c_str());
        tooltip("");
        when(FL_WHEN_CHANGED);
        callback(TabsGroup::Callback, o);
        selection_color(FL_SELECTION_COLOR);
        labelfont(flw::PREF_FONT);
        labelsize(flw::PREF_FONTSIZE);
    }
};

/***
 *      _______    _          _____
 *     |__   __|  | |        / ____|
 *        | | __ _| |__  ___| |  __ _ __ ___  _   _ _ __
 *        | |/ _` | '_ \/ __| | |_ | '__/ _ \| | | | '_ \
 *        | | (_| | |_) \__ \ |__| | | | (_) | |_| | |_) |
 *        |_|\__,_|_.__/|___/\_____|_|  \___/ \__,_| .__/
 *                                                 | |
 *                                                 |_|
 */

int TabsGroup::MIN_WIDTH_NORTH_SOUTH = 4;
int TabsGroup::MIN_WIDTH_EAST_WEST = 4;

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
TabsGroup::TabsGroup(int X, int Y, int W, int H, const char* l) : Fl_Group(X, Y, W, H, l) {
    end();
    clip_children(1);
    resizable(nullptr);
    tooltip(TabsGroup::Help());

    _scroll = new Fl_Scroll(X, Y, W, H);
    _pack   = new Fl_Pack(X, Y, W, H);
    _active = -1;
    _n      = 0;
    _s      = 0;
    _w      = 0;
    _e      = 0;
    _align  = 0;

    _pack->end();
    _scroll->box(FL_NO_BOX);
    _scroll->add(_pack);
    Fl_Group::add(_scroll);
    tabs(TABS::NORTH);
    update_pref();
}

/** @brief Add child widget.
*
* If no after widget is used it will be added last.
*
* @param[in] label   Label for this widget.
* @param[in] widget  Child widget.
* @param[in] after   Add widget after this, optional.
*/
void TabsGroup::add(const std::string& label, Fl_Widget* widget, const Fl_Widget* after) {
    assert(widget);

    auto button = new _TabsGroupButton(_align, label, widget, this);
    auto idx    = (after != nullptr) ? find(after) : (int) _widgets.size();

    if (idx < 0 || idx >= (int) _widgets.size() - 1) {
        Fl_Group::add(widget);
        _pack->add(button);
        _widgets.push_back(button);
    }
    else {
        idx++;
        auto b = static_cast<_TabsGroupButton*>(_widgets[idx]);
        Fl_Group::insert(*widget, b->widget);
        _pack->insert(*button, b);
        _widgets.insert(_widgets.begin() + idx, button);
    }

    TabsGroup::Callback(button, this);
    do_layout();
}

/** @brief Get active tab button.
*
* @return Button or NULL.
*/
Fl_Widget* TabsGroup::_active_button() {
    return (_active >= 0 && _active < (int) _widgets.size()) ? _widgets[_active] : nullptr;
}

/** @brief Tab button has been activated so switch to new widget.
*
* @param[in] sender  Tab button.
* @param[in] object  This object.
*/
void TabsGroup::Callback(Fl_Widget* sender, void* object) {
    auto self   = static_cast<TabsGroup*>(object);
    auto count  = 0;

    self->_active = -1;

    for (auto widget : self->_widgets) {
        auto b = static_cast<_TabsGroupButton*>(widget);

        if (b == sender) {
            self->_active = count;
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

    self->_resize_widgets();
}

/** @brief Get child widget.
*
* @param[in] index  Child index.
*
* @return Widget or NULL.
*/
Fl_Widget* TabsGroup::child(int index) const {
    return (index >= 0 && index < (int) _widgets.size()) ? static_cast<_TabsGroupButton*>(_widgets[index])->widget : nullptr;
}

/** @brief Remove all widgets.
*
*/
void TabsGroup::clear() {
    _scroll->remove(_pack);
    _scroll->clear();
    _pack->clear();
    Fl_Group::remove(_scroll);
    Fl_Group::clear();
    Fl_Group::add(_scroll);
    _widgets.clear();
    _scroll->add(_pack);
    _active = -1;
    update_pref();
    Fl::redraw();
}

/** @brief Print some debug info.
*
*/
void TabsGroup::debug() const {
#ifdef DEBUG
    printf("TabsGroup ==>\n");
    printf("    _active  = %d\n", _active);
    printf("    _drag    = %d\n", _drag);
    printf("    _pos     = %d\n", _pos);
    printf("    _widgets = %d\n", (int) _widgets.size());
    printf("    tabs     = %s\n", _scroll->visible() ? "visible" : "hidden");
    printf("    children = %d\n", children());
    printf("    scroll   = %d\n", _scroll->children());
    printf("\n");

    auto count = 0;

    for (auto b : _widgets) {
        printf("    widget[%02d] = %s\n", count++, b->label());
    }

    printf("\n");
    flw::debug::print(this);
    printf("TabsGroup <==\n");
    fflush(stdout);
#endif
}

/** @brief Redraw widget.
*
* Force scroll widget to redraw (is it needed?).
*/
void TabsGroup::draw() {
    _scroll->redraw();
    Fl_Group::draw();
}

/** @brief Find index for widget.
*
* @param[in] widget  Widget to find.
*
* @return Index or -1 if not found.
*/
int TabsGroup::find(const Fl_Widget* widget) const {
    auto num = 0;

    for (const auto W : _widgets) {
        const auto b = static_cast<_TabsGroupButton*>(W);

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
* If tabs are west/east then check for mouse drag events and resize width of buttons.\n
* Check also for key events, tab buttons can be selected with alt + 0-9.\n
* And they can also be moved with alt+shift+left/right keys.\n
*
* @param[in] event  Event value.
*
* @return Either 1 from this widget or value from parent widget.
*/
int TabsGroup::handle(int event) {
    if (_widgets.size() == 0) {
        return Fl_Group::handle(event);
    }

    if (_tabs == TABS::WEST || _tabs == TABS::EAST) { // Only left/right mode can resize width by using mouse.
        if (event == FL_DRAG) {
            if (_drag == true) {
                auto pos = 0;

                if (_tabs == TABS::WEST) {
                    pos = Fl::event_x() - x();
                }
                else {
                    pos = x() + w() - Fl::event_x();
                }

                if (pos != _pos) {
                    _pos = pos;
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

            if (_tabs == TABS::WEST) {
                auto pos = x() + _pos;

                if (event_x > (pos - 3) && event_x <= (pos + 3)) {
                    if (_drag == false) {
                        _drag = true;
                        fl_cursor(FL_CURSOR_WE);
                    }

                    return 1;
                }
            }
            else {
                auto pos = x() + w() - _pos;

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

    if (_widgets.size() > 1) {
        if (event == FL_KEYBOARD) { // Select tab by using keyboard.
            auto key   = Fl::event_key();
            auto alt   = Fl::event_alt() != 0;
            auto alt2  = alt;
            auto shift = Fl::event_shift() != 0;

#ifdef __APPLE__
            alt2 = Fl::event_command() != 0;
#endif

            if (alt2 == true && key >= '0' && key <= '9') {
                auto tab = key - '0';
                tab = (tab == 0) ? 9 : tab - 1;

                if (tab < (int) _widgets.size()) {
                    TabsGroup::Callback(_widgets[tab], this);
                }
                return 1;
            }
            else if (alt == true && shift == true && key == FL_Left) {
                swap(_active, _active - 1);
                TabsGroup::Callback(_active_button(), this);
                return 1;
            }
            else if (alt == true && shift == true && key == FL_Right) {
                swap(_active, _active + 1);
                TabsGroup::Callback(_active_button(), this);
                return 1;
            }
            else if (alt == true && key == FL_Left) {
                _active = _active == 0 ? (int) _widgets.size() - 1 : _active - 1;
                TabsGroup::Callback(_active_button(), this);
                return 1;
            }
            else if (alt == true && key == FL_Right) {
                _active = _active == (int) _widgets.size() - 1 ? 0 : _active + 1;
                TabsGroup::Callback(_active_button(), this);
                return 1;
            }
        }
    }

    if (event == FL_FOCUS) {
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
const char* TabsGroup::Help() {
    static const char* const HELP =
    "Use alt + left/right to move between tabs.\n"
    "Or alt (command key) + [1 - 9] to select tab.\n"
    "And alt + shift + left/right to move tabs.\n"
    "Tabs on the left/right side can have its width changed by dragging the mouse.";

    return HELP;
}

/** @brief Hide all tab buttons.
*
*/
void TabsGroup::hide_tabs() {
    _scroll->hide();
    do_layout();
}

/** @brief Insert child widget.
*
* If "before" widget is NULL then the widget will be inserted first.
*
* @param[in] label   Label for this widget.
* @param[in] widget  Child widget.
* @param[in] before  Insert before this, optional.
*/
void TabsGroup::insert(const std::string& label, Fl_Widget* widget, const Fl_Widget* before) {
    auto button = new _TabsGroupButton(_align, label, widget, this);
    auto idx    = (before != nullptr) ? find(before) : 0;

    if (idx >= (int) _widgets.size()) {
        Fl_Group::add(widget);
        _pack->add(button);
        _widgets.push_back(button);
    }
    else {
        auto b = static_cast<_TabsGroupButton*>(_widgets[idx]);
        Fl_Group::insert(*widget, b->widget);
        _pack->insert(*button, b);
        _widgets.insert(_widgets.begin() + idx, button);
    }

    TabsGroup::Callback(button, this);
    do_layout();
}

/** @brief Get tab button label.
*
* @param[in] widget  Child widget to get tab button label for.
*
* @return Label string or empty string.
*/
std::string TabsGroup::label(Fl_Widget* widget) {
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
void TabsGroup::label(const std::string& label, Fl_Widget* widget) {
    auto num = find(widget);

    if (num == -1) {
        return;
    }

    _widgets[num]->copy_label(label.c_str());
}

/** @brief Remove child widget.
*
* @param[in] index  Child index.
*
* @return Removed child widget or NULL.
*/
Fl_Widget* TabsGroup::remove(int index) {
    if (index < 0 || index >= (int) _widgets.size()) {
        return nullptr;
    }

    auto button = static_cast<_TabsGroupButton*>(_widgets[index]);
    auto res    = button->widget;

    _widgets.erase(_widgets.begin() + index);
    remove(res);
    _scroll->remove(button);
    delete button;

    if (index < _active) {
        _active--;
    }
    else if (_active == (int) _widgets.size()) {
        _active = (int) _widgets.size() - 1;
    }

    do_layout();
    TabsGroup::Callback(_active_button(), this);

    return res;
}

/** @brief Resize group.
*
*/
void TabsGroup::resize(int X, int Y, int W, int H) {
    Fl_Widget::resize(X, Y, W, H);

    if (W == 0 || H == 0) {
        return;
    }

    if (_scroll->visible() == 0) {
        _area = Fl_Rect(X + _w, Y + _n, W - _w - _e, H - _n - _s);
    }
    else {
        fl_font(flw::PREF_FONT, flw::PREF_FONTSIZE);

        if (_tabs == TABS::NORTH || _tabs == TABS::SOUTH) { // Resize buttons, either on top/bottom or left/right.
            _resize_north_south(X, Y, W, H);
        }
        else {
            _resize_east_west(X, Y, W, H);
        }

    }

    _resize_widgets();
}

/** @brief Resize tab buttons if they are on the left or right side.
*
*/
void TabsGroup::_resize_east_west(int X, int Y, int W, int H) {
    auto height = flw::PREF_FONTSIZE + 8;
    auto pack_h = (height + _space) * (int) _widgets.size() - _space;
    auto scroll = 0;

    if (_pos < flw::PREF_FONTSIZE * TabsGroup::MIN_WIDTH_EAST_WEST) { // Set min size for widgets on the left
        _pos = flw::PREF_FONTSIZE * TabsGroup::MIN_WIDTH_EAST_WEST;
    }
    else if (_pos > W - flw::PREF_FONTSIZE * TabsGroup::MIN_WIDTH_EAST_WEST) { // Set min size for widgets on the right
        _pos = W - flw::PREF_FONTSIZE * TabsGroup::MIN_WIDTH_EAST_WEST;
    }

    if (pack_h > H) {
        scroll = (_scroll->scrollbar_size() == 0) ? Fl::scrollbar_size() : _scroll->scrollbar_size();
    }

    for (auto b : _widgets) {
        b->size(0, height);
    }

    if (_tabs == TABS::WEST) {
        _scroll->resize(X, Y, _pos, H);
        _pack->resize(X, Y, _pos - scroll, pack_h);
        _area = Fl_Rect(X + _pos + _w, Y + _n, W - _pos - _w - _e, H - _n - _s);
    }
    else { // TABS::EAST.
        _scroll->resize(X + W - _pos, Y, _pos, H);
        _pack->resize(X + W - _pos, Y, _pos - scroll, pack_h);
        _area = Fl_Rect(X + _w, Y + _n, W - _pos - _w - _e, H - _n - _s);
    }
}

/** @brief Resize tab buttons if they are on the top or bottom side.
*
*/
void TabsGroup::_resize_north_south(int X, int Y, int W, int H) {
    auto height = flw::PREF_FONTSIZE + 8;
    auto scroll = 0;
    auto pack_w = 0;

    for (auto widget : _widgets) { // Check if scrollbar has to be visible.
        auto b  = static_cast<_TabsGroupButton*>(widget);
        auto th = 0;

        b->tw = 0;
        fl_measure(b->label(), b->tw, th);

        if (b->tw < flw::PREF_FONTSIZE * TabsGroup::MIN_WIDTH_NORTH_SOUTH) {
            b->tw = flw::PREF_FONTSIZE * TabsGroup::MIN_WIDTH_NORTH_SOUTH;
        }

        b->tw += flw::PREF_FONTSIZE;
        pack_w += b->tw + _space;
    }

    if (pack_w - _space > W) {
        scroll = (_scroll->scrollbar_size() == 0) ? Fl::scrollbar_size() : _scroll->scrollbar_size();
    }

    for (auto widget : _widgets) {
        auto b  = static_cast<_TabsGroupButton*>(widget);
        b->size(b->tw, 0);
    }

    if (_tabs == TABS::NORTH) {
        _scroll->resize(X, Y, W, height + scroll);
        _pack->resize(X, Y, pack_w, height);
        _area = Fl_Rect(X + _w, Y + height + scroll + _n, W - _w - _e, H - height - scroll - _n - _s);
    }
    else { // TABS::SOUTH.
        _scroll->resize(X, Y + H - height - scroll, W, height + scroll);
        _pack->resize(X, Y + H - height - scroll, pack_w, height);
        _area = Fl_Rect(X + _w, Y + _n, W - _w - _e, H - height - scroll - _n - _s);
    }
}

/** @brief Resize child widget (only the visible one).
*
*/
void TabsGroup::_resize_widgets() {
    for (auto w : _widgets) { // Resize tabs widgets.
        auto b = static_cast<_TabsGroupButton*>(w);

        if (b->widget->visible() != 0) {
            b->widget->resize(_area.x(), _area.y(), _area.w(), _area.h());
        }
    }

    Fl::redraw();
}

/** @brief Show tab buttons.
*
*/
void TabsGroup::show_tabs() {
    _scroll->show();
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
void TabsGroup::sort(bool ascending, bool casecompare) {
    auto pack = const_cast<Fl_Widget**>(_pack->array());
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

    for (int f = 0; f < _pack->children(); f++) {
        pack[f] = _widgets[f];

        if (_widgets[f] == butt) {
            _active = f;
        }
    }

    do_layout();
}

/** @brief Swap two tab buttons/widgets.
*
* @param[in] from  From index.
* @param[in] to    To index.
*
*/
void TabsGroup::swap(int from, int to) {
    auto last = (int) _widgets.size() - 1;

    if (_widgets.size() < 2 || to < -1 || to > (int) _widgets.size()) {
        return;
    }

    auto active = (_active == from);
    auto pack   = const_cast<Fl_Widget**>(_pack->array());

    if (from == 0 && to == -1) {
        auto widget = _widgets[0];

        for (int f = 1; f <= last; f++) {
            _widgets[f - 1] = _widgets[f];
            pack[f - 1]     = pack[f];
        }

        from           = last;
        pack[from]     = widget;
        _widgets[from] = widget;
    }
    else if (from == last && to == (int) _widgets.size()) {
        auto widget = _widgets[last];

        for (int f = last - 1; f >= 0; f--) {
            _widgets[f + 1] = _widgets[f];
            pack[f + 1]     = pack[f];
        }

        from           = 0;
        pack[from]     = widget;
        _widgets[from] = widget;
    }
    else {
        auto widget = _widgets[from];

        pack[from]     = pack[to];
        pack[to]       = widget;
        _widgets[from] = _widgets[to];
        _widgets[to]   = widget;
        from           = to;
    }

    if (active == true) {
        _active = from;
    }

    do_layout();
}

/** @brief Set where tab buttons should be.
*
* @param[in] tabs          North/south/west/east.
* @param[in] space_max_20  Max 20 pixel between tab buttons.
*
*/
void TabsGroup::tabs(TABS tabs, int space_max_20) {
    _tabs  = tabs;
    _space = (space_max_20 >= 0 && space_max_20 <= 20) ? space_max_20 : TabsGroup::DEFAULT_SPACE_PX;
    _align = FL_ALIGN_LEFT | FL_ALIGN_INSIDE | FL_ALIGN_CLIP;

    if (_tabs == TABS::NORTH || _tabs == TABS::SOUTH) {
        _align = FL_ALIGN_CENTER | FL_ALIGN_INSIDE | FL_ALIGN_CLIP;
        _scroll->type(Fl_Scroll::HORIZONTAL);
        _pack->type(Fl_Pack::HORIZONTAL);
    }
    else {
        _pack->type(Fl_Pack::VERTICAL);
        _scroll->type(Fl_Scroll::VERTICAL);
    }

    for (auto widget : _widgets) {
        widget->align(_align);
    }

    _pack->spacing(_space);
    do_layout();

    auto w = value();

    if (w != nullptr) {
        w->take_focus();
    }
}

/** @brief Update font preferences.
*
* @param[in] characters  Number of characters for west/east label width.
* @param[in] font        Font to use.
* @param[in] fontsize    Size of font.
*/
void TabsGroup::update_pref(unsigned characters, Fl_Font font, Fl_Fontsize fontsize) {
    _drag = false;
    _pos  = fontsize * characters;

    for (auto widget : _widgets) {
        widget->labelfont(font);
        widget->labelsize(fontsize);
    }
}

/** @brief Get current active widget.
*
* @return Widget or NULL.
*/
Fl_Widget* TabsGroup::value() const {
    return (_active >= 0 && _active < (int) _widgets.size()) ? static_cast<_TabsGroupButton*>(_widgets[_active])->widget : nullptr;
}

/** @brief Set active widget.
*
* @param[in] num  Valid index.
*/
void TabsGroup::value(int num) {
    if (num >= 0 && num < (int) _widgets.size()) {
        TabsGroup::Callback(_widgets[num], this);
    }
}

} // flw

// MKALGAM_OFF
