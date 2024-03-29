// Copyright gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#ifndef FLW_TABSGROUP_H
#define FLW_TABSGROUP_H

#include "flw.h"

// MKALGAM_ON

#include <FL/Fl_Group.H>
#include <FL/Fl_Rect.H>

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
// Do a resize after adding widgets by calling do_layout() or resize().
//
class TabsGroup : public Fl_Group {
public:
    enum class TABS {
                                NORTH,
                                SOUTH,
                                WEST,
                                EAST,
    };

    explicit                    TabsGroup(int X = 0, int Y = 0, int W = 0, int H = 0, const char* l = nullptr);
    void                        add(const std::string& label, Fl_Widget* widget, bool force_append = false);
    void                        border(int n = 0, int s = 0, int w = 0, int e = 0)
                                    { _n = n; _s = s; _w = w; _e = e; do_layout(); }
    Fl_Widget*                  child(int num) const;
    int                         children() const
                                    { return (int) _widgets.size(); }
    void                        do_layout()
                                    { TabsGroup::resize(x(), y(), w(), h()); Fl::redraw(); }
    int                         find(Fl_Widget* widget) const;
    int                         handle(int event) override;
    void                        hide_tabs()
                                    { _hide_tab_buttons(true); }
    void                        label(std::string label, Fl_Widget* widget);
    Fl_Widget*                  remove(int num);
    Fl_Widget*                  remove(Fl_Widget* widget)
                                    { return remove(find(widget)); }
    void                        resize(int X, int Y, int W, int H) override;
    void                        show_tabs()
                                    { _hide_tab_buttons(false); }
    void                        swap(int from, int to);
    TABS                        tabs()
                                    { return _tabs; }
    void                        tabs(TABS value)
                                    { _tabs = value; }
    bool                        tabs_visible() const
                                    { return _hide == false; }
    void                        update_pref(int pos = 10, Fl_Font font = flw::PREF_FONT, Fl_Fontsize fontsize = flw::PREF_FONTSIZE);
    Fl_Widget*                  value() const;
    void                        value(int num);
    void                        value(Fl_Widget* widget)
                                    { value(find(widget)); }

    static void                 BoxColor(Fl_Color boxcolor = FL_DARK1);
    static void                 BoxSelectionColor(Fl_Color boxcolor = FL_SELECTION_COLOR);
    static void                 BoxType(Fl_Boxtype boxtype = FL_FLAT_BOX);
    static void                 Callback(Fl_Widget* sender, void* object);
    static const char*          Help();

private:
    Fl_Widget*                  _button();
    void                        _hide_tab_buttons(bool hide);

    TABS                        _tabs;
    WidgetVector                _buttons;
    WidgetVector                _widgets;
    bool                        _drag;
    bool                        _hide;
    int                         _active;
    int                         _e;
    int                         _n;
    int                         _s;
    int                         _pos;
    int                         _w;
};

} // flw

// MKALGAM_OFF

#endif
