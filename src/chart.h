// Copyright gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#ifndef FLW_CHART_H
#define FLW_CHART_H

#include "date.h"
#include "price.h"

// MKALGAM_ON

#include <FL/Fl_Group.H>
#include <FL/Fl_Menu_Button.H>
#include <FL/Fl_Scrollbar.H>
#include <vector>

namespace flw {
    class Chart;

    namespace chart {
        struct Area;
        struct Line;
        enum class TYPE;

        const int                       MIN_TICK  = 3;
        const int                       MAX_TICK  = 100;
        const size_t                    MAX_AREA  = 3;
        const size_t                    MAX_LINE  = 10;
        const double                    MIN_VAL   = -999'999'999'999'999.0;
        const double                    MAX_VAL   =  999'999'999'999'999.0;

        typedef std::vector<chart::Line> LineVector;
        typedef std::vector<chart::Area> AreaVector;

        size_t                          bsearch(const PriceVector& prices, const Price& key);
        bool                            has_high_low(TYPE chart_type);
        bool                            has_resizable_width(TYPE chart_type);
        bool                            has_time(Date::RANGE date_range);
        bool                            load_data(Chart* chart, std::string filename);
        bool                            save_data(const Chart* chart, std::string filename, double max_diff_high_low = 0.001);

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
            TYPE                        type;
            Fl_Color                    color;
            bool                        visible;
            int                         width;
            double                      clamp_max;
            double                      clamp_min;
            std::string                 label;
            double                      max;
            double                      min;
            PriceVector                 points;

                                        Line();
                                        ~Line()
                                            { clear(); }
            void                        clear();
            void                        debug(int num) const;
            bool                        set(const PriceVector& points, std::string label, TYPE type, Fl_Align align, Fl_Color color, int width, double clamp_min, double clamp_max);
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
            Scale                       left;
            LineVector                  lines;
            int                         percent;
            Scale                       right;
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
        friend bool                     chart::save_data(const Chart*, std::string, double);

    public:
                                        Chart(int X = 0, int Y = 0, int W = 0, int H = 0, const char* l = nullptr);
        bool                            add_line(size_t area_0_to_2, const PriceVector& points, std::string line_label, chart::TYPE chart_type = chart::TYPE::LINE, Fl_Align line_align = FL_ALIGN_LEFT, Fl_Color line_color = FL_BLUE, int line_width = 1, double clamp_min = chart::MIN_VAL, double clamp_max = chart::MAX_VAL);
        bool                            area_size(int area1 = 100, int area2 = 0, int area3 = 0);
        void                            block_dates(const PriceVector& block_dates)
                                            { _block_dates = block_dates; }
        void                            clear();
        Date::FORMAT                    date_format() const
                                            { return _date_format; }
        bool                            date_range(Date::RANGE range = Date::RANGE::DAY);
        void                            debug() const;
        void                            draw() override;
        int                             handle(int event) override;
        bool                            has_time() const
                                            { return chart::has_time(_date_range); }
        void                            init(bool calc_dates);
        bool                            margin(int left_1_to_20 = 6, int right_1_to_20 = 1);
        void                            resize()
                                            { _old_width = _old_height = -1; resize(x(), y(), w(), h()); }
        void                            resize(int X, int Y, int W, int H) override;
        bool                            tick_width(int tick_width_from_3_to_100 = chart::MIN_TICK);
        void                            update_pref();
        void                            view_options(bool line_labels = true, bool hor_lines = true, bool ver_lines = true)
                                            { _view.labels = line_labels; _view.horizontal = hor_lines; _view.vertical = ver_lines; redraw(); }

    private:
        void                            _calc_area_height();
        void                            _calc_area_width();
        void                            _calc_dates();
        void                            _calc_ymin_ymax();
        void                            _calc_yscale();
        void                            _create_tooltip(bool ctrl);
        void                            _draw_area(const chart::Area& area);
        void                            _draw_line(const chart::Line& line, const chart::Scale& scale, int X, int Y, int W, int H);
        void                            _draw_line_labels(const chart::Area& area);
        void                            _draw_tooltip();
        void                            _draw_ver_lines(const chart::Area& area);
        void                            _draw_xlabels();
        void                            _draw_ylabels(int X, double Y1, double Y2, const chart::Scale& scale, bool left);
        chart::Area*                    _inside_area(int X, int Y);

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

        chart::Area*                    _area;
        chart::AreaVector               _areas;
        PriceVector                     _block_dates;
        int                             _bottom_space;
        int                             _cw;
        Date::FORMAT                    _date_format;
        Date::RANGE                     _date_range;
        int                             _date_start;
        PriceVector                     _dates;
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
