// Copyright gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#include "tabsgroup.h"

// MKALGAM_ON

#include <assert.h>
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

//------------------------------------------------------------------------------
class _TabsGroupButton : public Fl_Toggle_Button {
public:
    static int                  _BORDER;
    static Fl_Boxtype           _BOXTYPE;
    static Fl_Color             _BOXSELCOLOR;
    static Fl_Color             _BOXCOLOR;
    int                         _tw;

    //--------------------------------------------------------------------------
    explicit _TabsGroupButton(const char* label) : Fl_Toggle_Button(0, 0, 0, 0) {
        _tw = 0;
        copy_label(label);
        copy_tooltip(label);
    }

    //----------------------------------------------------------------------
    void draw() override {
        fl_draw_box(_BOXTYPE, x(), y(), w(), h(), value() ? _TabsGroupButton::_BOXSELCOLOR : _TabsGroupButton::_BOXCOLOR);
        fl_font(flw::PREF_FONT, flw::PREF_FONTSIZE);
        fl_color(value() ? FL_BACKGROUND2_COLOR : FL_FOREGROUND_COLOR);
        fl_draw(label(), x() + 3, y(), w() - 6, h(), FL_ALIGN_LEFT | FL_ALIGN_CLIP);
    }
};

int        _TabsGroupButton::_BORDER      = 6;
Fl_Boxtype _TabsGroupButton::_BOXTYPE     = FL_FLAT_BOX;
Fl_Color   _TabsGroupButton::_BOXSELCOLOR = FL_SELECTION_COLOR;
Fl_Color   _TabsGroupButton::_BOXCOLOR    = FL_DARK1;

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

//------------------------------------------------------------------------------
TabsGroup::TabsGroup(int X, int Y, int W, int H, const char* l) : Fl_Group(X, Y, W, H, l) {
    end();
    clip_children(1);
    resizable(nullptr);
    tooltip(TabsGroup::Help());

    _active = -1;
    _drag   = false;
    _hide   = false;
    _pos    = flw::PREF_FONTSIZE * 10;
    _tabs   = TABS::NORTH;
    _n      = 0;
    _s      = 0;
    _w      = 0;
    _e      = 0;
}

//------------------------------------------------------------------------------
void TabsGroup::add(const std::string& label, Fl_Widget* widget) {
    auto button = new _TabsGroupButton(label.c_str());

    button->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
    button->box(FL_THIN_UP_BOX);
    button->callback(TabsGroup::Callback, this);
    button->when(FL_WHEN_CHANGED);

    Fl_Group::add(widget);
    Fl_Group::add(button);

    if (_active + 1 >= (int) _widgets.size()) {
        _widgets.push_back(widget);
        _buttons.push_back(button);
    }
    else {
        _widgets.insert(_widgets.begin() + _active + 1, widget);
        _buttons.insert(_buttons.begin() + _active + 1, button);
    }

    TabsGroup::Callback(button, this);
}

//------------------------------------------------------------------------------
void TabsGroup::BoxColor(Fl_Color boxcolor) {
    flw::_TabsGroupButton::_BOXCOLOR = boxcolor;
}

//------------------------------------------------------------------------------
void TabsGroup::BoxSelectionColor(Fl_Color boxcolor) {
    flw::_TabsGroupButton::_BOXSELCOLOR = boxcolor;
}

//------------------------------------------------------------------------------
void TabsGroup::BoxType(Fl_Boxtype boxtype) {
    flw::_TabsGroupButton::_BOXTYPE = boxtype;
}

//------------------------------------------------------------------------------
Fl_Widget* TabsGroup::_button() {
    return (_active >= 0 && _active < (int) _buttons.size()) ? _buttons[_active] : nullptr;
}

//------------------------------------------------------------------------------
void TabsGroup::Callback(Fl_Widget* sender, void* object) {
    if (sender) {
        auto button = static_cast<_TabsGroupButton*>(sender);
        auto self   = static_cast<TabsGroup*>(object);
        auto count  = 0;

        self->_active = -1;

        for (auto b : self->_buttons) {
            if (b == button) {
                static_cast<Fl_Button*>(b)->value(1);
                self->_active = count;
                self->_widgets[count]->show();
            }
            else {
                static_cast<Fl_Button*>(b)->value(0);
                self->_widgets[count]->hide();
            }

            count++;
        }

        if (self->value() != nullptr && Fl::focus() != self->value()) {
            self->value()->take_focus();
        }

        self->do_layout();
    }
}

//------------------------------------------------------------------------------
Fl_Widget* TabsGroup::child(int num) const {
    return (num >= 0 && num < (int) _widgets.size()) ? _widgets[num] : nullptr;
}

//------------------------------------------------------------------------------
int TabsGroup::find(Fl_Widget* widget) const{
    auto num = 0;

    for (const auto w : _widgets) {
        if (w == widget) {
            return num;
        }
        else {
            num++;
        }
    }

    return -1;
}

//------------------------------------------------------------------------------
int TabsGroup::handle(int event) {
    if (_widgets.size() == 0) {
        return Fl_Group::handle(event);
    }

    if (_tabs == TABS::WEST || _tabs == TABS::EAST) {
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

    if (_buttons.size() > 1) {
        if (event == FL_KEYBOARD) {
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

                if (tab < (int) _buttons.size()) {
                    TabsGroup::Callback(_buttons[tab], this);
                }
                return 1;
            }
            else if (alt == true && shift == true && key == FL_Left) {
                swap(_active, _active - 1);
                TabsGroup::Callback(_button(), this);
                return 1;
            }
            else if (alt == true && shift == true && key == FL_Right) {
                swap(_active, _active + 1);
                TabsGroup::Callback(_button(), this);
                return 1;
            }
            else if (alt == true && key == FL_Left) {
                _active = _active == 0 ? (int) _widgets.size() - 1 : _active - 1;
                TabsGroup::Callback(_button(), this);
                return 1;
            }
            else if (alt == true && key == FL_Right) {
                _active = _active == (int) _widgets.size() - 1 ? 0 : _active + 1;
                TabsGroup::Callback(_button(), this);
                return 1;
            }
        }
    }

    if (event == FL_FOCUS) {
        if (value() != nullptr && Fl::focus() != value()) {
            value()->take_focus();
            return 1;
        }
    }

    return Fl_Group::handle(event);
}

//------------------------------------------------------------------------------
void TabsGroup::_hide_tab_buttons(bool hide) {
    _hide = hide;

    for (auto b : _buttons) {
        if (_hide == true) {
            b->hide();
        }
        else {
            b->show();
        }
    }

    do_layout();
}

//------------------------------------------------------------------------------
const char* TabsGroup::Help() {
    static const char* const HELP =
    "Use alt + left/right to move between tabs.\n"
    "Or alt (command key) + [1 - 9] to select tab.\n"
    "And alt + shift + left/right to move tabs.\n"
    "Tabs on the left/right side can have its width changed by dragging the mouse.";
    return HELP;
}

//------------------------------------------------------------------------------
void TabsGroup::label(const std::string& label, Fl_Widget* widget) {
    auto num = find(widget);

    if (num != -1) {
        _buttons[num]->copy_label(label.c_str());
        _buttons[num]->copy_tooltip(label.c_str());
    }
}

//------------------------------------------------------------------------------
Fl_Widget* TabsGroup::remove(int num) {
    if (num >= 0 && num < (int) _widgets.size()) {
        auto w = _widgets[num];
        auto b = _buttons[num];

        _widgets.erase(_widgets.begin() + num);
        _buttons.erase(_buttons.begin() + num);

        remove(w);
        remove(b);
        delete b;

        if (num < _active) {
            _active--;
        }
        else if (_active == (int) _widgets.size()) {
            _active = (int) _widgets.size() - 1;
        }

        TabsGroup::Callback(_button(), this);
        return w;
    }
    else {
        return nullptr;
    }
}

//------------------------------------------------------------------------------
void TabsGroup::resize(int X, int Y, int W, int H) {
    assert(_buttons.size() == _widgets.size());
    
    Fl_Widget::resize(X, Y, W, H);

    if (children() == 0 || W == 0 || H == 0) {
        return;
    }
    
    auto height = flw::PREF_FONTSIZE + 8;

    if (_hide == true) {
        for (auto w : _widgets) { // Resize widgets when tabs are hidden
            if (w->visible() == 0) {
            }
            else {
                w->resize(X + _w, Y + _n, W - _w - _e, H - _n - _s);
            }
        }

        return;
    }

    if (_tabs == TABS::NORTH || _tabs == TABS::SOUTH) {
        auto space = 4;
        auto x     = 0;
        auto w     = 0;
        auto th    = 0;

        fl_font(flw::PREF_FONT, flw::PREF_FONTSIZE);

        for (auto widget : _buttons) { // Calc total width of buttons
            auto b = static_cast<_TabsGroupButton*>(widget);
            b->_tw = 0;
            fl_measure(b->label(), b->_tw, th);

            b->_tw += _TabsGroupButton::_BORDER;
            w      += b->_tw + space;
        }

        if (w > W) { // If width is too large then divide equal
            w = (W - (_buttons.size() * 2)) / _buttons.size();
            space = 2;
        }
        else {
            w = 0;
        }

        size_t count = 0;

        for (auto widget : _buttons) { // Resize buttons
            auto b  = static_cast<_TabsGroupButton*>(widget);
            auto bw = (w != 0) ? w : b->_tw;

            if (space == 2 && count == _buttons.size() - 1) {
                bw = W - x;
            }

            if (_tabs == TABS::NORTH) {
                b->resize(X + x, Y, bw, height);
            }
            else {
                b->resize(X + x, Y + H - height, bw, height);
            }

            x += bw;
            x += space;
            count++;
        }
    }
    else { // TABS::WEST, TABS::EAST
        auto y        = Y;
        auto h        = height;
        auto shrinked = false;
        auto space    = 3;

        if ((h + space) * (int) _buttons.size() > H) { // Shrink button height if any falls outside area
            h = (H - _buttons.size()) / _buttons.size();
            shrinked = true;
        }

        if (_pos < flw::PREF_FONTSIZE * space) { // Set min size for widgets on the left
            _pos = flw::PREF_FONTSIZE * space;
        }
        else if (_pos > W - flw::PREF_FONTSIZE * 8) { // Set min size for widgets on the right
            _pos = W - flw::PREF_FONTSIZE * 8;
        }

        for (auto b : _buttons) {
            if (_tabs == TABS::WEST) {
                b->resize(X, y, _pos, h);
                y += h + (shrinked == false ? space : 1);
            }
            else {
                b->resize(X + W - _pos, y, _pos, h);
                y += h + (shrinked == false ? space : 1);
            }
        }
    }

    for (auto w : _widgets) { // Resize widgets
        if (w->visible() == 0) {
        }
        else if (_tabs == TABS::NORTH) {
            w->resize(X + _w, Y + height + _n, W - _w - _e, H - height - _n - _s);
        }
        else if (_tabs == TABS::SOUTH) {
            w->resize(X + _w, Y + _n, W - _w - _e, H - height - _n - _s);
        }
        else if (_tabs == TABS::WEST) {
            w->resize(X + _pos + _w, Y + _n, W - _pos - _w - _e, H - _n - _s);
        }
        else if (_tabs == TABS::EAST) {
            w->resize(X + _w, Y + _n, W - _pos - _w - _e, H - _n - _s);
        }
    }
}

//------------------------------------------------------------------------------
void TabsGroup::swap(int from, int to) {
    auto last = (int) _widgets.size() - 1;

    if (_widgets.size() < 2 || to < -1 || to > (int) _widgets.size()) {
        return;
    }

    bool active = (_active == from);

    if (from == 0 && to == -1) {
        auto widget = _widgets[0];
        auto button = _buttons[0];

        for (int f = 1; f <= last; f++) {
            _widgets[f - 1] = _widgets[f];
            _buttons[f - 1] = _buttons[f];
        }

        from           = last;
        _widgets[from] = widget;
        _buttons[from] = button;
    }
    else if (from == last && to == (int) _widgets.size()) {
        auto widget = _widgets[last];
        auto button = _buttons[last];

        for (int f = last - 1; f >= 0; f--) {
            _widgets[f + 1] = _widgets[f];
            _buttons[f + 1] = _buttons[f];
        }

        from           = 0;
        _widgets[from] = widget;
        _buttons[from] = button;
    }
    else {
        auto widget = _widgets[from];
        auto button = _buttons[from];

        _widgets[from] = _widgets[to];
        _buttons[from] = _buttons[to];
        _widgets[to]   = widget;
        _buttons[to]   = button;
        from           = to;
    }

    if (active == true) {
        _active = from;
    }

    do_layout();
}

//------------------------------------------------------------------------------
Fl_Widget* TabsGroup::value() const {
    return (_active >= 0 && _active < (int) _widgets.size()) ? _widgets[_active] : nullptr;
}

//------------------------------------------------------------------------------
void TabsGroup::value(int num) {
    if (num >= 0 && num < (int) _buttons.size()) {
        TabsGroup::Callback(_buttons[num], this);
    }
}

} // flw

// MKALGAM_OFF
