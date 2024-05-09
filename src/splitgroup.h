// Copyright gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#ifndef FLW_SPLITGROUP_H
#define FLW_SPLITGROUP_H

// MKALGAM_ON

#include <FL/Fl.H>
#include <FL/Fl_Group.H>

namespace flw {
    //------------------------------------------------------------------------------
    // A split view widget for two child widgets
    // That can be resized by dragging the border between the widgets
    // By default it places widgets in LAYOUT::VERTICAL direction (child widgets get full height)
    // If a widget already exist at specific DIRECTION it will be deleted.
    //
    class SplitGroup : public Fl_Group {
    public:
        enum class CHILD {
                                FIRST,
                                SECOND,
        };

        enum class DIRECTION {
                                HORIZONTAL,
                                VERTICAL,
        };

        explicit                SplitGroup(int X = 0, int Y = 0, int W = 0, int H = 0, const char* l = nullptr);
        void                    add(Fl_Widget* widget, SplitGroup::CHILD child);
        Fl_Widget*              child(SplitGroup::CHILD child)
                                    { return (child == SplitGroup::CHILD::FIRST) ? _widgets[0] : _widgets[1]; }
        void                    clear();
        DIRECTION               direction() const
                                    { return _direction; }
        void                    direction(SplitGroup::DIRECTION direction);
        void                    do_layout()
                                    { SplitGroup::resize(x(), y(), w(), h()); Fl::redraw(); }
        int                     handle(int event) override;
        int                     min_pos() const
                                    { return _min; }
        void                    min_pos(int value)
                                    { _min = value; }
        void                    resize(int X, int Y, int W, int H) override;
        int                     split_pos() const
                                    { return _split_pos; }
        void                    split_pos(int split_pos)
                                    { _split_pos = split_pos; }
        void                    swap()
                                    { auto tmp = _widgets[0]; _widgets[0] = _widgets[1]; _widgets[1] = tmp; do_layout(); }
        void                    toggle(SplitGroup::CHILD child, SplitGroup::DIRECTION direction, int second_size = -1);
        void                    toggle(SplitGroup::CHILD child, int second_size = -1)
                                    { toggle(child, _direction, second_size); }

    private:
        DIRECTION               _direction;
        Fl_Widget*              _widgets[2];
        bool                    _drag;
        int                     _min;
        int                     _split_pos;
    };
}

// MKALGAM_OFF

#endif
