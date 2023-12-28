// Copyright gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#ifndef FLW_TABSGROUP_H
#define FLW_TABSGROUP_H

#include "flw.h"

// MKALGAM_ON

#include <FL/Fl_Group.H>

namespace flw {

//--------------------------------------------------------------------------
// Group widget that uses tabs for labels.
// You can move between tabs using alt (or command key on macOS) + left/right.
// Move tabs around by using alt + up/down.
// Use shortcut keys (alt/command + 0-9).
//
class TabsGroup : public Fl_Group {
public:
    enum class TABS {
                                NORTH,
                                SOUTH,
                                WEST,
                                EAST,
    };

                                TabsGroup(int X = 0, int Y = 0, int W = 0, int H = 0, const char* l = nullptr);
    void                        add(const std::string& label, Fl_Widget* widget);
    Fl_Widget*                  child(int num) const;
    int                         children() const
                                    { return (int) _widgets.size(); }
    int                         find(Fl_Widget* widget) const;
    int                         handle(int event) override;
    void                        hide_tabs()
                                    { _hide_tab_buttons(true); }
    void                        label(const std::string& label, Fl_Widget* widget);
    Fl_Widget*                  remove(int num);
    Fl_Widget*                  remove(Fl_Widget* widget)
                                    { return remove(find(widget)); }
    void                        resize()
                                    { Fl::redraw(); resize(x(), y(), w(), h()); }
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
    Fl_Widget*                  value() const;
    void                        value(int num);
    void                        value(Fl_Widget* widget)
                                    { value(find(widget)); }

    static void                 BoxColor(Fl_Color boxcolor);
    static void                 BoxSelectionColor(Fl_Color boxcolor);
    static void                 BoxType(Fl_Boxtype boxtype);
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
    int                         _pos;
};

} // flw

// MKALGAM_OFF

#endif
