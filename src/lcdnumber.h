// Copyright (C) 2006 z_hossain
// Released under the GNU General Public License v3.0

#ifndef FLW_LCDNUMBER_H
#define FLW_LCDNUMBER_H

#include <FL/Fl_Box.H>
#include <FL/Fl.H>

// MKALGAM_ON

namespace flw
                                    {
    //--------------------------------------------------------------------------
    // A 7 segment number label widget
    //
    class LcdNumber : public Fl_Box
                                        {
    public:
                                        LcdNumber(int X = 0, int Y = 0, int W = 0, int H = 0, const char* l = nullptr);
        void                            draw() override;
        void                            value(const char* value);

        Fl_Align                        align() const
                                            { return _align; }
        void                            align(Fl_Align align)
                                            { _align = align; Fl::redraw(); }
        int                             dot_size() const
                                            { return _dot_size; }
        void                            dot_size(int size)
                                            { _dot_size = size; Fl::redraw(); }
        Fl_Color                        segment_color() const
                                            { return _seg_color; }
        void                            segment_color(Fl_Color color)
                                            { _seg_color = color; Fl::redraw(); }
        int                             segment_gap() const
                                            { return _seg_gap; }
        void                            segment_gap(int gap)
                                            { _seg_gap = gap; Fl::redraw(); }
        int                             thickness() const
                                            { return _thick; }
        void                            thickness(int thickness)
                                            { _thick = thickness; Fl::redraw(); }
        int                             unit_gap()
                                            { return _unit_gap; }
        void                            unit_gap(int gap)
                                            { _unit_gap = gap; Fl::redraw(); }
        int                             unit_h() const
                                            { return _unit_h; }
        void                            unit_h(int height)
                                            { _unit_h = height; Fl::redraw(); }
        int                             unit_w() const
                                            { return _unit_w; }
        void                            unit_w(int width)
                                            { _unit_w = width; Fl::redraw(); }
        const char*                     value() const
                                            { return _value; }

    private:
        void                            _draw_seg(uchar a, int x, int y, int w, int h);

        Fl_Align                        _align;
        Fl_Color                        _seg_color;
        char                            _value[100];
        int                             _dot_size;
        int                             _seg_gap;
        int                             _thick;
        int                             _unit_gap;
        int                             _unit_h;
        int                             _unit_w;
    };
}

// MKALGAM_OFF

#endif
