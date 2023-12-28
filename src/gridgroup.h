// Copyright 2019 - 2021 gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#ifndef FLW_GRIDGROUP_H
#define FLW_GRIDGROUP_H

#include <FL/Fl_Group.H>

// MKALGAM_ON

namespace flw {
    struct _GridGroupChild;

    //--------------------------------------------------------------------------
    // A simple layout widget that uses a grid for placing widgets
    // One cell is set to labelsize() at start, use set_size() to change it
    //
    class GridGroup : public Fl_Group {
    public:
        static const int                MAX_WIDGETS = 100;

                                        GridGroup(int X = 0, int Y = 0, int W = 0, int H = 0, const char* l = nullptr);
        virtual                         ~GridGroup();
        void                            add(Fl_Widget* widget, int X, int Y, int W, int H);
        void                            clear();
        void                            remove(Fl_Widget* widget);
        void                            resize();
        void                            resize(int X, int Y, int W, int H) override;

        inline int                      size() const { return _size; }
        inline void                     size(int size) { _size = size; }

    private:
        _GridGroupChild*                _widgets[MAX_WIDGETS];
        int                             _size;
    };
}

// MKALGAM_OFF

#endif
