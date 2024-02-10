// Copyright gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#ifndef FLW_GRIDGROUP_H
#define FLW_GRIDGROUP_H

// MKALGAM_ON

#include <FL/Fl_Group.H>
#include <FL/Fl_Rect.H>

namespace flw {

struct _GridGroupChild;

//------------------------------------------------------------------------------
// A simple layout widget that uses a grid for placing widgets.
// One cell is set to flw::PREF_FONTSIZE / 2.
// Override by calling size() method (min 4, max 72 pixels).
//
class GridGroup : public Fl_Group {
public:
    static const int            MAX_WIDGETS = 100;

    explicit                    GridGroup(int X = 0, int Y = 0, int W = 0, int H = 0, const char* l = nullptr);
    virtual                     ~GridGroup();
    void                        add(Fl_Widget* widget, int X, int Y, int W, int H);
    void                        adjust(Fl_Widget* widget, int L = 0, int R = 0, int T = 0, int B = 0);
    void                        clear();
    void                        do_layout()
                                    { resize(x(), y(), w(), h()); redraw(); }
    int                         handle(int event) override;
    void                        remove(Fl_Widget* widget);
    void                        resize(int X, int Y, int W, int H) override;
    void                        resize(Fl_Widget* widget, int X, int Y, int W, int H);
    int                         size() const
                                    { return _size; }
    void                        size(int size)
                                    { _size = (size >= 4 && size <= 72) ? size : 0; }
    
private:
    void                        _last_active_widget(Fl_Widget** first, Fl_Widget** last);
    
    _GridGroupChild*            _widgets[MAX_WIDGETS];
    int                         _size;
};

} // flw

// MKALGAM_OFF

#endif
