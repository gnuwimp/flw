/**
* @file
* @brief Tab group widget.
*
* @author gnuwimp@gmail.com
* @copyright Released under the GNU General Public License v3.0
*/

#ifndef FLW_TABSGROUP_H
#define FLW_TABSGROUP_H

#include "flw.h"

// MKALGAM_ON

#include <FL/Fl_Rect.H>
#include <FL/Fl_Group.H>

class Fl_Scrollbar;

namespace flw {

/*
 *      _______    _          _____
 *     |__   __|  | |        / ____|
 *        | | __ _| |__  ___| |  __ _ __ ___  _   _ _ __
 *        | |/ _` | '_ \/ __| | |_ | '__/ _ \| | | | '_ \
 *        | | (_| | |_) \__ \ |__| | | | (_) | |_| | |_) |
 *        |_|\__,_|_.__/|___/\_____|_|  \___/ \__,_| .__/
 *                                                 | |
 *                                                 |_|
 */

/** @brief Group widget with a tab bar.
*
* You can move between tabs using alt + left/right.\n
* Switch between two last activated widgets with alt + up/down.\n
* Or use a shortcut key for the first 10 tabs (alt + 1-9/0).\n
* Move tabs by using alt + shift + left/right.\n
*\n
* By default tabs are horizontal on the top.\n
*\n
* If tabs are on the left or right side you can change the width by using the mouse.\n
* If there are to many tabs the scrollbar will be show, either to the left of tabs or on the right side.\n
*\n
* Tab width on top/bottom has a max width, change that with TabsGroup::max_top_width()\n
* If there are to many tabs the scrollbar will be show, either above the tabs or below.\n
*\n
* Do a resize after adding widgets by calling do_layout().\n
*
* @snippet tabsgroup.cpp flw::TabsGroup example
* @image html tabsgroup.png
*/
class TabsGroup : public Fl_Group {
public:
    static const int            DEFAULT_SPACE             =  2;     ///< @brief Default space between buttons.
    static const int            DEFAULT_MAX_HOR_TAB_WIDTH = 12;     ///< @brief Default max tab width for top/bottom tabs.
    static const int            DEFAULT_VER_TAB_WIDTH     = 12;     ///< @brief Default tab width for left/right tabs.
    static const int            HEIGHT                    =  8;     ///< @brief Additional height for tabs.
    static const int            MAX_SPACE                 = 20;     ///< @brief Max space between buttons.
    static const int            MIN_WIDTH                 =  4;     ///< @brief Min width for tab button and container widget.

    /** @brief Positions of tab buttons.
    *
    */
    enum class Pos {
                                TOP,            ///< @brief Place tabs on top side and scrollbar above tabs.
                                BOTTOM,         ///< @brief Place tabs on bottom side and scrollbar below tabs.
                                LEFT,           ///< @brief Place tabs on left side and scrollbar to the left of tabs.
                                RIGHT,          ///< @brief Place tabs on right side and scrollbar to the right of tabs.
                                TOP2,           ///< @brief Place tabs on top side and scrollbar below tabs.
                                BOTTOM2,        ///< @brief Place tabs on bottom side and scrollbar above tabs.
                                LEFT2,          ///< @brief Place tabs on left side and scrollbar to the right of the tabs.
                                RIGHT2,         ///< @brief Place tabs on right side and scrollbar to the left of the tabs.
                                LAST = RIGHT2,  ///< @brief Same as RIGHT2.
    };

    explicit                    TabsGroup(int X = 0, int Y = 0, int W = 0, int H = 0, const char* l = nullptr);
    void                        add(const std::string& label, Fl_Widget* widget, const Fl_Widget* after =  nullptr, const std::string& tooltip = "");
    void                        border(int n = 0, int s = 0, int w = 0, int e = 0)
                                    { _n = n; _s = s; _w = w; _e = e; do_layout(); } ///< @brief Set space around active child widget.
    Fl_Widget*                  child(int index) const;
    int                         children() const
                                    { return static_cast<int>(_widgets.size()); } ///< @brief Return number of child widgets (tab buttons are not included).
    void                        clear();
    void                        debug(bool all = true) const;
    void                        disable_keyboard()
                                    { _keyboard = false; } ///< @brief Disable all keyboard shortcuts.
    void                        do_layout()
                                    { TabsGroup::resize(x(), y(), w(), h()); Fl::redraw(); } ///< @brief Resize all widgets.
    void                        enable_keyboard()
                                    { _keyboard = true; } ///< @brief Enable all keyboard shortcuts.
    int                         find(const Fl_Widget* widget) const;
    int                         handle(int event) override;
    void                        hide_tabs();
    void                        insert(const std::string& label, Fl_Widget* widget, const Fl_Widget* before = nullptr, const std::string& tooltip = "");
    bool                        is_tabs_bottom() const
                                    { return _tab_pos == Pos::BOTTOM || _tab_pos == Pos::BOTTOM2; } ///< @brief Is tab buttons on bottom?
    bool                        is_tabs_left() const
                                    { return _tab_pos == Pos::LEFT || _tab_pos == Pos::LEFT2; } ///< @brief Is tab buttons on left side?
    bool                        is_tabs_horizontal() const
                                    { return _tab_pos == Pos::TOP || _tab_pos == Pos::TOP2 || _tab_pos == Pos::BOTTOM || _tab_pos == Pos::BOTTOM2; } ///< @brief Is tab buttons on top or bottom?
    bool                        is_tabs_right() const
                                    { return _tab_pos == Pos::RIGHT || _tab_pos == Pos::RIGHT2; } ///< @brief Is tab buttons on right side?
    bool                        is_tabs_top() const
                                    { return _tab_pos == Pos::TOP || _tab_pos == Pos::TOP2; } ///< @brief Is tab buttons on top side?
    bool                        is_tabs_visible() const
                                    { return _tabs->visible(); } ///< @brief Is tab buttons visible?
    bool                        is_tabs_vertical() const
                                    { return _tab_pos == Pos::LEFT || _tab_pos == Pos::LEFT2 || _tab_pos == Pos::RIGHT || _tab_pos == Pos::RIGHT2; } ///< @brief Is tab buttons on the left or right side?
    void                        max_top_width(unsigned characters = TabsGroup::DEFAULT_MAX_HOR_TAB_WIDTH)
                                    { if (characters >= TabsGroup::MIN_WIDTH && characters <= 100) _width2 = characters; }  ///< @brief Set max width to top/bottom tabs. @param[in] characters  From TabsGroup::MIN_WIDTH to 100, default 10 (characters * fontsize).
    Fl_Widget*                  remove(int index);
    Fl_Widget*                  remove(Fl_Widget* widget)
                                    { return TabsGroup::remove(find(widget)); } ///< @brief Remove widget and return it.
    void                        resize(int X, int Y, int W, int H) override;
    void                        show_tabs();
    void                        sort(bool ascending = true, bool casecompare = false);
    Fl_Widget*                  tab(int index) const
                                    { return child(index); } ///< @brief Return tab widgets. @param[in] index  Child index. @return Widget or NULL.
    int                         tabs() const
                                    { return children(); } ///< @brief Return number of child widgets (tab buttons are not included).
    void                        tab_box(Fl_Boxtype up_box = FL_MAX_BOXTYPE, Fl_Boxtype down_box = FL_MAX_BOXTYPE);
    void                        tab_color(Fl_Color color = FL_SELECTION_COLOR);
    std::string                 tab_label();
    std::string                 tab_label(const Fl_Widget* widget);
    void                        tab_label(const std::string& label, Fl_Widget* widget);
    Pos                         tab_pos() const
                                    { return _tab_pos; } ///< @brief Get position of the tab buttons, @return One of Pos values.
    void                        tab_pos(Pos pos, int space = TabsGroup::DEFAULT_SPACE);
    int                         swap(int from, int to);
    std::string                 tooltip(Fl_Widget* widget) const;
    void                        tooltip(const std::string& label, Fl_Widget* widget);
    void                        update_pref(Fl_Font font = flw::PREF_FONT, Fl_Fontsize fontsize = flw::PREF_FONTSIZE);
    Fl_Widget*                  value() const;
    void                        value(int num);
    void                        value(Fl_Widget* widget)
                                    { _activate(widget); } ///< @brief Activate and show button and child widget. @param[in] widget  Set current widget, can be NULL.

    static void                 CallbackButton(Fl_Widget* sender, void* object);
    static void                 CallbackScrollbar(Fl_Widget* sender, void* object);
    static const char*          Help();

private:
    void                        _activate(Fl_Widget* widget);
    Fl_Widget*                  _active_button();
    void                        _resize_active_widget();
    void                        _resize_left_right(int X, int Y, int W, int H);
    void                        _resize_top_bottom(int X, int Y, int W, int H);

    Fl_Boxtype                  _down_box;  // Box type for tab buttons.
    Fl_Boxtype                  _up_box;    // Box type for tab buttons.
    Fl_Color                    _color;     // Tab color, default FL_SELECTION_COLOR.
    Fl_Group*                   _tabs;      // Area for tab buttons.
    Fl_Rect                     _area;      // Area for widgets.
    Fl_Scrollbar*               _scroll;    // Scrollbar for scrolling tabs if they are to large to fit.
    Pos                         _tab_pos;   // Tab placement.
    WidgetVector                _widgets;   // Vector with _TabsGroupButton objects.
    bool                        _keyboard;  // False to disable keyboard shortcuts.
    bool                        _drag;      // True if drag event.
    int                         _active1;   // Active widget index.
    int                         _active2;   // Previous active widget.
    int                         _e;         // East border around child widget.
    int                         _n;         // North border around child widget.
    int                         _s;         // South border around child widget.
    int                         _space;     // Space between tab buttons.
    int                         _visible;   // Number of visible tabs.
    int                         _w;         // West border around child widget.
    int                         _width1;    // Width of EAST/WEST tab buttons, used also for mouse drags.
    int                         _width2;    // Max width of NORTH/SOUTH buttons in characters.
};

} // flw

// MKALGAM_OFF

#endif
