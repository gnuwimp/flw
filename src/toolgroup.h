// Copyright gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#ifndef FLW_TOOLGROUP_H
#define FLW_TOOLGROUP_H

// MKALGAM_ON

#include <FL/Fl_Group.H>
#include <FL/Fl_Rect.H>

namespace flw {

struct _ToolGroupChild;

//--------------------------------------------------------------------------
class ToolGroup : public Fl_Group {
public:
    enum class DIRECTION {
                                HORIZONTAL,
                                VERTICAL,
    };

    static const int            MAX_WIDGETS = 50;

    explicit                    ToolGroup(DIRECTION direction = DIRECTION::HORIZONTAL, int X = 0, int Y = 0, int W = 0, int H = 0, const char* l = nullptr);
    virtual                     ~ToolGroup();
    void                        add(Fl_Widget* widget, int SIZE = 0);
    void                        clear();
    DIRECTION                   direction() const
                                    { return _direction; }
    void                        direction(DIRECTION direction)
                                    { _direction = direction; }
    void                        do_layout()
                                    { resize(x(), y(), w(), h()); redraw(); }
    bool                        expand_last() const
                                    { return _expand; }
    void                        expand_last(bool value)
                                    { _expand = value; }
    void                        remove(Fl_Widget* widget);
    void                        resize(int X, int Y, int W, int H) override;
    void                        resize(Fl_Widget* widget, int SIZE = 0);

private:
    DIRECTION                   _direction;
    _ToolGroupChild*            _widgets[MAX_WIDGETS];
    bool                        _expand;
};

}

// MKALGAM_OFF

#endif
