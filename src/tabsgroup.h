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

#include <FL/Fl_Pack.H>
#include <FL/Fl_Scroll.H>

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
* Only one child widget is visible at one time.\n
* You can move between tabs using alt + left/right.\n
* Switch between two last activated widgets with alt + up/down.\n
* Or use a shortcut key for the first 10 tabs (alt + 1-9/0).\n
* Move tabs around by using alt + shift + left/right.\n
*\n
* By default tabs are on the TABS::NORTH side and tabs size is font size * 10.\n
* If tabs are on the TABS::EAST or TABS::WEST side you can change the width by using the mouse.\n
*\n
* Do a resize after adding widgets by calling do_layout().\n
*
* @snippet tabsgroup.cpp flw::TabsGroup example
* @image html tabsgroup.png
*/
class TabsGroup : public Fl_Group {
public:
    static const int            DEFAULT_SPACE_PX = 2;   ///< @brief Default space between buttons.
    static int                  MIN_WIDTH_NORTH_SOUTH;  ///< @brief Minimum width of top/bottom buttons, default 4 (4 * flw::PREF_FONTSIZE).
    static int                  MIN_WIDTH_EAST_WEST;    ///< @brief Minimum width of left/right buttons, default 4 (4 * flw::PREF_FONTSIZE).

    /** @brief Positions of tab buttons.
    *
    */
    enum class TABS {
                                NORTH,          ///< @brief Top of container.
                                TOP = NORTH,    ///< @brief Top of container.
                                SOUTH,          ///< @brief Bottom of container.
                                BOTTOM = SOUTH, ///< @brief Bottom of container.
                                WEST,           ///< @brief On the left side of container.
                                LEFT = WEST,    ///< @brief On the left side of container.
                                EAST,           ///< @brief On the right side of the container.
                                RIGHT = EAST,   ///< @brief On the right side of the container.
    };

    explicit                    TabsGroup(int X = 0, int Y = 0, int W = 0, int H = 0, const char* l = nullptr);
    void                        activate(Fl_Widget* widget)
                                    { _activate(widget, false); } ///< @brief Activate and show button and child widget.
    void                        add(const std::string& label, Fl_Widget* widget, const Fl_Widget* after =  nullptr);
    void                        border(int n = 0, int s = 0, int w = 0, int e = 0)
                                    { _n = n; _s = s; _w = w; _e = e; do_layout(); } ///< @brief Set border around active child widget.
    Fl_Widget*                  child(int index) const;
    int                         children() const
                                    { return (int) _widgets.size(); } ///< @brief Return number of child widgets (tab buttons are not included).
    void                        clear();
    void                        debug(bool all = true) const;
    void                        disable_keyboard()
                                    { _disable_k = true; } ///< @brief Disable all keyboard shortcuts.
    void                        do_layout()
                                    { TabsGroup::resize(x(), y(), w(), h()); Fl::redraw(); } ///< @brief Resize all widgets.
    void                        draw() override;
    void                        enable_keyboard()
                                    { _disable_k = false; } ///< @brief Enable all keyboard shortcuts.
    int                         find(const Fl_Widget* widget) const;
    int                         handle(int event) override;
    void                        hide_tabs();
    void                        insert(const std::string& label, Fl_Widget* widget, const Fl_Widget* before = nullptr);
    bool                        is_tabs_visible() const
                                    { return _scroll->visible(); } ///< @brief Is tab buttons visible?
    std::string                 label(Fl_Widget* widget);
    void                        label(const std::string& label, Fl_Widget* widget);
    Fl_Widget*                  remove(int index);
    Fl_Widget*                  remove(Fl_Widget* widget)
                                    { return TabsGroup::remove(find(widget)); } ///< @brief Remove widget and return it.
    void                        resize(int X, int Y, int W, int H) override;
    void                        show_tabs();
    void                        sort(bool ascending = true, bool casecompare = false);
    int                         swap(int from, int to);
    TABS                        tabs() const
                                    { return _tabs; } ///< @brief Pos of the tab buttons (TABS::NORTH, TABS::SOUTH, TABS::EAST, TABS::WEST).
    void                        tabs(TABS value, int space_max_20 = TabsGroup::DEFAULT_SPACE_PX);
    void                        tooltip(const std::string& label, Fl_Widget* widget);
    void                        update_pref(unsigned characters = 10, Fl_Font font = flw::PREF_FONT, Fl_Fontsize fontsize = flw::PREF_FONTSIZE);
    Fl_Widget*                  value() const;
    void                        value(int num);
    void                        value(Fl_Widget* widget)
                                    { value(find(widget)); } ///< @brief Set active child widget.

    static void                 Callback(Fl_Widget* sender, void* object);
    static const char*          Help();

private:
    void                        _activate(Fl_Widget* widget, bool kludge);
    Fl_Widget*                  _active_button();
    void                        _resize_active_widget();
    void                        _resize_east_west(int X, int Y, int W, int H);
    void                        _resize_north_south(int X, int Y, int W, int H);

    Fl_Align                    _align;     // Align value for tab button labels.
    Fl_Pack*                    _pack;      // Tab button container.
    Fl_Rect                     _area;      // Area for tab buttons.
    Fl_Scroll*                  _scroll;    // Scroll container for the pack widget.
    TABS                        _tabs;      // Tab placement.
    WidgetVector                _widgets;   // Vector with _TabsGroupButton objects.
    bool                        _drag;      // True if drag event.
    bool                        _disable_k; // True to disable keyboard shortcuts.
    int                         _active1;   // Active widget index.
    int                         _active2;   // Previous active widget.
    int                         _e;         // East border around child widget.
    int                         _n;         // North border around child widget.
    int                         _pos;       // Position or width of east/west tab buttons.
    int                         _s;         // South border around child widget.
    int                         _space;     // Space between tab buttons.
    int                         _w;         // West border around child widget.
};

} // flw

// MKALGAM_OFF

#endif
