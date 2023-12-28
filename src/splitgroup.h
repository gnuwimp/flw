// Copyright 2016 - 2021 gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#ifndef FLW_SPLITGROUP_H
#define FLW_SPLITGROUP_H

#include <FL/Fl_Group.H>

// MKALGAM_ON

namespace flw {
    //--------------------------------------------------------------------------
    // A split view widget for two child widgets
    // That can be resized by dragging the border between the widgets
    // By default it places widgets in LAYOUT::VERTICAL direction (child widgets get full height)
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
                                        SplitGroup(int X = 0, int Y = 0, int W = 0, int H = 0, const char* l = nullptr);
        void                            add(Fl_Widget* widget, SplitGroup::CHILD child);
        void                            direction(SplitGroup::DIRECTION direction);
        void                            clear();
        int                             handle(int event) override;
        void                            resize(int X, int Y, int W, int H) override;
        void                            toggle(SplitGroup::CHILD child, SplitGroup::DIRECTION direction, int second_size = -1);

        inline Fl_Widget*               child(SplitGroup::CHILD child) { return child == SplitGroup::CHILD::FIRST ? _widgets[0] : _widgets[1]; }
        inline DIRECTION                direction() const { return _direction; }
        inline int                      min_pos() const { return _min; }
        inline void                     min_pos(int value) { _min = value; }
        inline void                     resize() { SplitGroup::resize(x(), y(), w(), h()); }
        inline int                      split_pos() const { return _split_pos; }
        inline void                     split_pos(int split_pos) { _split_pos = split_pos; }
        inline void                     toggle(SplitGroup::CHILD child, int second_size = -1) { toggle(child, _direction, second_size); }

    private:
        Fl_Widget*                      _widgets[2];
        DIRECTION                       _direction;
        bool                            _drag;
        int                             _min;
        int                             _split_pos;
    };
}

// MKALGAM_OFF

#endif
