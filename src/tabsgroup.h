// Copyright gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#ifndef FLW_TABSGROUP_H
#define FLW_TABSGROUP_H

#include "flw.h"

// MKALGAM_ON

#include <FL/Fl_Pack.H>
#include <FL/Fl_Scroll.H>

namespace flw {

//------------------------------------------------------------------------------
// Group widget that uses tabs for labels.
// You can move between tabs using alt (or command key on macOS) + left/right.
// Move tabs around by using alt + up/down.
// Use shortcut keys (alt/command + 0-9).
//
// By default tabs are on the NORTH side and tabs size is font size * 10.
// Change width of tabs for the WEST/EAST side with pos().
// If tabs are on the NORTH/SOUTH side you can change the width by using the mouse.
//
// Do a resize after adding widgets by calling do_layout().
//
class TabsGroup : public Fl_Group {
public:
    static const int            DEFAULT_SPACE_PX = 2;
    static int                  MIN_MIN_WIDTH_NS_CH;
    static int                  MIN_MIN_WIDTH_EW_CH;
    
    enum class TABS {
                                NORTH,
                                SOUTH,
                                WEST,
                                EAST,
    };

    explicit                    TabsGroup(int X = 0, int Y = 0, int W = 0, int H = 0, const char* l = nullptr);
    void                        add(std::string label, Fl_Widget* widget, const Fl_Widget* after =  nullptr);
    void                        border(int n = 0, int s = 0, int w = 0, int e = 0)
                                    { _n = n; _s = s; _w = w; _e = e; do_layout(); }
    Fl_Widget*                  child(int num) const;
    int                         children() const
                                    { return (int) _widgets.size(); }
    void                        clear();
    void                        debug() const;
    void                        do_layout()
                                    { TabsGroup::resize(x(), y(), w(), h()); Fl::redraw(); }
    void                        draw() override;
    int                         find(const Fl_Widget* widget) const;
    int                         handle(int event) override;
    void                        hide_tabs();
    void                        insert(std::string label, Fl_Widget* widget, const Fl_Widget* before = nullptr);
    bool                        is_tabs_visible() const
                                    { return _scroll->visible(); }
    std::string                 label(Fl_Widget* widget);
    void                        label(std::string label, Fl_Widget* widget);
    Fl_Widget*                  remove(int num);
    Fl_Widget*                  remove(Fl_Widget* widget)
                                    { return TabsGroup::remove(find(widget)); }
    void                        resize(int X, int Y, int W, int H) override;
    void                        show_tabs();
    void                        sort(bool ascending = true, bool casecompare = false);
    void                        swap(int from, int to);
    TABS                        tabs() const
                                    { return _tabs; }
    void                        tabs(TABS value, int space_max_20 = TabsGroup::DEFAULT_SPACE_PX);
    void                        update_pref(unsigned characters = 10, Fl_Font font = flw::PREF_FONT, Fl_Fontsize fontsize = flw::PREF_FONTSIZE);
    Fl_Widget*                  value() const;
    void                        value(int num);
    void                        value(Fl_Widget* widget)
                                    { value(find(widget)); }

    static void                 Callback(Fl_Widget* sender, void* object);
    static const char*          Help();

private:
    Fl_Widget*                  _active_button();
    void                        _resize_east_west(int X, int Y, int W, int H);
    void                        _resize_north_south(int X, int Y, int W, int H);
    void                        _resize_widgets();

    Fl_Pack*                    _pack;
    Fl_Rect                     _area;
    Fl_Scroll*                  _scroll;
    TABS                        _tabs;
    WidgetVector                _widgets;
    bool                        _drag;
    int                         _active;
    int                         _align;
    int                         _e;
    int                         _n;
    int                         _pos;
    int                         _s;
    int                         _space;
    int                         _w;
};

} // flw

// MKALGAM_OFF

#endif
