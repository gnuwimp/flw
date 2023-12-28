// Copyright 2016 - 2021 gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#ifndef FLW_CHART_H
#define FLW_CHART_H

#include "date.h"
#include <FL/Fl_Group.H>
#include <vector>

class Fl_Scrollbar;

// MKALGAM_ON

namespace flw {
    struct ChartArea;
    struct ChartLine;
    struct ChartScale;
    struct Price;

    //--------------------------------------------------------------------------
    // Chart widget that is using dates for the x scale
    //
    class Chart : public Fl_Group {
    public:
        enum class AREA {
                                        A1,
                                        A2,
                                        A3,
                                        SIZE,
        };

        enum class LINE {
                                        L1,
                                        L2,
                                        L3,
                                        L4,
                                        L5,
                                        L6,
                                        L7,
                                        L8,
                                        L9,
                                        L10,
                                        SIZE,
        };

        enum class TYPE {
                                        LINE,
                                        BAR,
                                        VERTICAL,
                                        VERTICAL2,
                                        VERTICAL3,
                                        HORIZONTAL,
                                        HORIZONTAL2,
        };

        static const double             MAX;
        static const double             MIN;

                                        Chart(int X = 0, int Y = 0, int W = 0, int H = 0, const char* l = nullptr);
        virtual                         ~Chart();
        void                            area_size(int area1 = 100, int area2 = 0, int area3 = 0);
        void                            block(const std::vector<flw::Price>& dates);
        void                            clamp(Chart::AREA area, Chart::LINE line, double clamp_min = MIN, double clamp_max = MAX);
        void                            clear();
        void                            debug() const;
        void                            font(Fl_Font font, Fl_Fontsize size)
                                            { _font = font; _fs   = size; }
        void                            draw() override;
        int                             handle(int event) override;
        void                            init(bool calc_dates);
        void                            line(Chart::AREA area, Chart::LINE line, const std::vector<flw::Price>& points, const char* line_label, Chart::TYPE chart_type, Fl_Align line_align, Fl_Color line_color, int line_width);
        void                            margin(int left = 6, int right = 1)
                                            { _margin_left = left; _margin_right = right; redraw(); }
        void                            range(Date::RANGE range = Date::RANGE::DAY);
        void                            resize(int X, int Y, int W, int H) override;
        void                            support_lines(bool hor = false, bool ver = false)
                                            { _hor_lines = hor; _ver_lines = ver; redraw(); }
        void                            tick_width(int tick_width = 3);
        void                            tooltip(bool tooltip = false)
                                            { _tooltip = tooltip; }
        void                            zoom(bool zoom = false)
                                            { _zoom = zoom; }

    private:
        void                            _calc_area_height();
        void                            _calc_area_width();
        void                            _calc_dates();
        void                            _calc_ymin_ymax();
        void                            _calc_yscale();
        void                            _create_tooltip();
        void                            _draw_area(const ChartArea* area);
        void                            _draw_line(const ChartLine* line, const ChartScale* scale, int X, int Y, int W, int H);
        void                            _draw_line_labels(const ChartArea* area);
        void                            _draw_tooltip(const char* label, const ChartArea* area);
        void                            _draw_ver_lines(const ChartArea* area);
        void                            _draw_xlabels();
        void                            _draw_ylabels(int X, double Y1, double Y2, const ChartScale* scale, bool left);
        ChartArea*                      _inside_area(int X, int Y);

        static void                     _CallbackScrollbar(Fl_Widget* widget, void* chart_object);

        Fl_Font                         _font;
        Fl_Fontsize                     _fs;
        Fl_Scrollbar*                   _scroll;
        ChartArea*                      _area;
        ChartArea*                      _areas[(int) Chart::AREA::SIZE];
        bool                            _hor_lines;
        bool                            _tooltip;
        bool                            _ver_lines;
        bool                            _zoom;
        char                            _label[101];
        Date::FORMAT                    _date_format;
        Date::RANGE                     _range;
        std::vector<Price>              _blocks;
        std::vector<Price>              _dates;
        int                             _bottom_space;
        int                             _date_start;
        int                             _margin_left;
        int                             _margin_right;
        int                             _old_height;
        int                             _old_width;
        int                             _tick_width;
        int                             _ticks;
        int                             _top_space;
        int                             _ver_pos[1001];
    };
}

// MKALGAM_OFF

#endif
