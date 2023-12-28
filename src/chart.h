// Copyright 2016 - 2022 gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#ifndef FLW_CHART_H
#define FLW_CHART_H

#include "date.h"
#include "price.h"
#include <FL/Fl_Group.H>
#include <vector>

class Fl_Scrollbar;
class Fl_Menu_Button;

// MKALGAM_ON

namespace flw {
    class Chart;

    namespace chart {
        class Area;
        class Line;
        enum class TYPE;

        extern const double             MAX_VAL;
        extern const double             MIN_VAL;
        extern const int                MAX_TICK;
        extern const int                MIN_TICK;
        extern const size_t             MAX_AREA;
        extern const size_t             MAX_LINE;

        typedef std::vector<chart::Line> LineVector;
        typedef std::vector<chart::Area> AreaVector;

        size_t                          bsearch(const flw::PriceVector& prices, const flw::Price& key);
        bool                            has_high_low(flw::chart::TYPE chart_type);
        bool                            has_resizable_width(flw::chart::TYPE chart_type);
        bool                            has_time(flw::Date::RANGE date_range);
        bool                            load_data(flw::Chart* chart, std::string filename);
        bool                            save_data(const flw::Chart* chart, std::string filename);

        //----------------------------------------------------------------------
        enum class TYPE {
                                        LINE,
                                        BAR,
                                        VERTICAL,
                                        CLAMP_VERTICAL,
                                        HORIZONTAL,
                                        EXPAND_VERTICAL,
                                        EXPAND_HORIZONTAL,
                                        LAST = EXPAND_HORIZONTAL,
        };

        //----------------------------------------------------------------------
        // Data for one chart line
        //
        struct Line {
            Fl_Align                    align;
            flw::chart::TYPE            type;
            Fl_Color                    color;
            bool                        visible;
            int                         width;
            double                      clamp_max;
            double                      clamp_min;
            std::string                 label;
            double                      max;
            double                      min;
            flw::PriceVector            points;

                                        Line();
                                        ~Line()
                                            { clear(); }
            void                        clear();
            void                        debug(int num) const;
            bool                        set(const flw::PriceVector& points, std::string label, flw::chart::TYPE type, Fl_Align align, Fl_Color color, int width, double clamp_min, double clamp_max);
        };

        //----------------------------------------------------------------------
        // Each chart area can have an scale on the left and right side
        //
        struct Scale {
            double                      max;
            double                      min;
            double                      pixel;
            double                      tick;

                                        Scale();
            void                        calc(int height);
            void                        clear();
            void                        debug(const char* name) const;
            double                      diff() const
                                            { return max - min; }
            void                        fix_height();
        };

        //----------------------------------------------------------------------
        // Data for one chart area, currently max 3 areas in one widget can be shown at the same time
        // All are using the same date range
        //
        struct Area {
            size_t                      count;
            double                      h;
            flw::chart::Scale           left;
            flw::chart::LineVector      lines;
            int                         percent;
            flw::chart::Scale           right;
            size_t                      selected;
            double                      w;
            double                      x;
            double                      y;

                                        Area();
            void                        clear(bool clear_data);
            void                        debug(int num) const;
            int                         x2() const
                                            { return (int) (x + w); }
            int                         y2() const
                                            { return (int) (y + h); }
        };
    }

    //--------------------------------------------------------------------------
    // Chart widget that is using dates for the x scale
    // It has an popup menu for a few settings and option to save chart to png image
    // It can also use date + hours/minutes/seconds
    // Use block_dates() to set an date list that removes those dates
    // Line types BAR, VERTICAL and CLAMP_VERTICAL can use line width of -1 which means it will expand to tick width - 1
    //
    class Chart : public Fl_Group {
        friend bool                     flw::chart::load_data(flw::Chart* chart, std::string filename);
        friend bool                     flw::chart::save_data(const flw::Chart* chart, std::string filename);

    public:
                                        Chart(int X = 0, int Y = 0, int W = 0, int H = 0, const char* l = nullptr);
        bool                            add_line(size_t area_0_to_2, const flw::PriceVector& points, std::string line_label, flw::chart::TYPE chart_type = flw::chart::TYPE::LINE, Fl_Align line_align = FL_ALIGN_LEFT, Fl_Color line_color = FL_BLUE, int line_width = 1, double clamp_min = flw::chart::MIN_VAL, double clamp_max = flw::chart::MAX_VAL);
        bool                            area_size(int area1 = 100, int area2 = 0, int area3 = 0);
        void                            block_dates(const flw::PriceVector& block_dates)
                                            { _block_dates = block_dates; }
        void                            clear();
        Date::FORMAT                    date_format() const
                                            { return _date_format; }
        bool                            date_range(flw::Date::RANGE range = Date::RANGE::DAY);
        void                            debug() const;
        void                            draw() override;
        int                             handle(int event) override;
        bool                            has_time() const
                                            { return flw::chart::has_time(_date_range); }
        void                            init(bool calc_dates);
        bool                            margin(int left_1_to_20 = 6, int right_1_to_20 = 1);
        void                            resize()
                                            { _old_width = _old_height = -1; resize(x(), y(), w(), h()); }
        void                            resize(int X, int Y, int W, int H) override;
        bool                            tick_width(int tick_width_from_3_to_100 = flw::chart::MIN_TICK);
        void                            update_pref();
        void                            view_options(bool labels = true, bool hor = true, bool ver = true)
                                            { _view.labels = labels; _view.horizontal = hor; _view.vertical = ver; redraw(); }

    private:
        void                            _calc_area_height();
        void                            _calc_area_width();
        void                            _calc_dates();
        void                            _calc_ymin_ymax();
        void                            _calc_yscale();
        void                            _create_tooltip(bool ctrl);
        void                            _draw_area(const flw::chart::Area& area);
        void                            _draw_line(const flw::chart::Line& line, const flw::chart::Scale& scale, int X, int Y, int W, int H);
        void                            _draw_line_labels(const flw::chart::Area& area);
        void                            _draw_tooltip();
        void                            _draw_ver_lines(const flw::chart::Area& area);
        void                            _draw_xlabels();
        void                            _draw_ylabels(int X, double Y1, double Y2, const flw::chart::Scale& scale, bool left);
        flw::chart::Area*               _inside_area(int X, int Y);

        static void                     _CallbackDebug(Fl_Widget* widget, void* chart_object);
        static void                     _CallbackReset(Fl_Widget* widget, void* chart_object);
        static void                     _CallbackSavePng(Fl_Widget* widget, void* chart_object);
        static void                     _CallbackScrollbar(Fl_Widget* widget, void* chart_object);
        static void                     _CallbackToggle(Fl_Widget* widget, void* chart_object);

        struct {
            bool                        horizontal;
            bool                        labels;
            bool                        vertical;
        }                               _view;

        flw::chart::Area*               _area;
        flw::chart::AreaVector          _areas;
        flw::PriceVector                _block_dates;
        int                             _bottom_space;
        int                             _cw;
        flw::Date::FORMAT               _date_format;
        flw::Date::RANGE                _date_range;
        int                             _date_start;
        flw::PriceVector                _dates;
        int                             _margin_left;
        int                             _margin_right;
        Fl_Menu_Button*                 _menu;
        int                             _old_height;
        int                             _old_width;
        Fl_Scrollbar*                   _scroll;
        int                             _tick_width;
        int                             _ticks;
        std::string                     _tooltip;
        int                             _top_space;
        int                             _ver_pos[101];
    };
}

// MKALGAM_OFF

#endif
