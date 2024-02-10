// Copyright gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#ifndef FLW_CHART_H
#define FLW_CHART_H

#include "date.h"

// MKALGAM_ON

#include <FL/Fl_Group.H>
#include <FL/Fl_Menu_Button.H>
#include <FL/Fl_Scrollbar.H>
#include <vector>

namespace flw {

/***
 *      _____      _
 *     |  __ \    (_)
 *     | |__) | __ _  ___ ___
 *     |  ___/ '__| |/ __/ _ \
 *     | |   | |  | | (_|  __/
 *     |_|   |_|  |_|\___\___|
 *
 *
 */

//------------------------------------------------------------------------------
// Chart data
//
struct Price {
    std::string                 date;
    double                      close;
    double                      high;
    double                      low;
    double                      vol;

                                Price();
    explicit                    Price(const std::string& date_value, double value = 0.0);
                                Price(const std::string& date, double high, double low, double close, double vol = 0.0);
    bool                        operator<(const Price& price) const { return date < price.date; }
    bool                        operator<=(const Price& price) const { return date <= price.date; }
    bool                        operator==(const Price& price) const { return date == price.date; }
    bool                        operator!=(const Price& price) const { return date != price.date; }
};

struct ChartArea;
struct ChartLine;
struct ChartScale;

typedef std::vector<Price>      PriceVector;
typedef std::vector<ChartLine>  LineVector;
typedef std::vector<ChartArea>  AreaVector;

/***
 *       _____ _                _
 *      / ____| |              | |
 *     | |    | |__   __ _ _ __| |_
 *     | |    | '_ \ / _` | '__| __|
 *     | |____| | | | (_| | |  | |_
 *      \_____|_| |_|\__,_|_|   \__|
 *
 *
 */

//------------------------------------------------------------------------------
// Chart widget that is using dates for the x scale
// It has an popup menu for a few settings and option to save chart to png image
// It can also use date + hours/minutes/seconds
// Use block_dates() to set an date list that removes those dates
// ChartLine types BAR, VERTICAL and CLAMP_VERTICAL can use line width of -1 which means it will expand to tick width - 1
//
class Chart : public Fl_Group {
public:
    //--------------------------------------------------------------------------
    enum TYPE {
                                LINE,
                                BAR,
                                VERTICAL,
                                CLAMP_VERTICAL,
                                HORIZONTAL,
                                EXPAND_VERTICAL,
                                EXPAND_HORIZONTAL,
                                LAST = EXPAND_HORIZONTAL,
    };

    static const int            MIN_TICK  = 3;
    static const int            MAX_TICK  = 100;
    static const size_t         MAX_AREA  = 3;
    static const size_t         MAX_LINE  = 10;
    static constexpr double     MIN_VAL = -999'999'999'999'999.0;
    static constexpr double     MAX_VAL =  999'999'999'999'999.0;

    explicit                    Chart(int X = 0, int Y = 0, int W = 0, int H = 0, const char* l = nullptr);
    bool                        add_line(size_t area_0_to_2, const PriceVector& points, std::string line_label, Chart::TYPE chart_type = Chart::TYPE::LINE, Fl_Align line_align = FL_ALIGN_LEFT, Fl_Color line_color = FL_BLUE, int line_width = 1, double clamp_min = Chart::MIN_VAL, double clamp_max = Chart::MAX_VAL);
    bool                        area_size(int area1 = 100, int area2 = 0, int area3 = 0);
    void                        block_dates(const PriceVector& block_dates)
                                    { _block_dates = block_dates; }
    void                        clear();
    Date::FORMAT                date_format() const
                                    { return _date_format; }
    bool                        date_range(Date::RANGE range = Date::RANGE::DAY);
    void                        debug() const;
    void                        draw() override;
    int                         handle(int event) override;
    bool                        has_time() const;
    void                        init(bool calc_dates);
    void                        layout()
                                    { _old_width = _old_height = -1; resize(x(), y(), w(), h()); }
    bool                        margin(int left_1_to_20 = 6, int right_1_to_20 = 1);
    void                        resize(int X, int Y, int W, int H) override;
    bool                        tick_width(int tick_width_from_3_to_100 = Chart::MIN_TICK);
    void                        update_pref();
    void                        view_options(bool line_labels = true, bool hor_lines = true, bool ver_lines = true)
                                    { _view.labels = line_labels; _view.horizontal = hor_lines; _view.vertical = ver_lines; redraw(); }

    static bool                 Load(Chart* chart, std::string filename);
    static bool                 Save(const Chart* chart, std::string filename, double max_diff_high_low = 0.001);

private:
    void                        _calc_area_height();
    void                        _calc_area_width();
    void                        _calc_dates();
    void                        _calc_ymin_ymax();
    void                        _calc_yscale();
    void                        _create_tooltip(bool ctrl);
    void                        _draw_area(const ChartArea& area);
    void                        _draw_line(const ChartLine& line, const ChartScale& scale, int X, int Y, int W, int H);
    void                        _draw_line_labels(const ChartArea& area);
    void                        _draw_tooltip();
    void                        _draw_ver_lines(const ChartArea& area);
    void                        _draw_xlabels();
    void                        _draw_ylabels(int X, double Y1, double Y2, const ChartScale& scale, bool left);
    ChartArea*                  _inside_area(int X, int Y);

    static void                 _CallbackDebug(Fl_Widget* widget, void* chart_object);
    static void                 _CallbackReset(Fl_Widget* widget, void* chart_object);
    static void                 _CallbackSavePng(Fl_Widget* widget, void* chart_object);
    static void                 _CallbackScrollbar(Fl_Widget* widget, void* chart_object);
    static void                 _CallbackToggle(Fl_Widget* widget, void* chart_object);

    struct {
        bool                    horizontal;
        bool                    labels;
        bool                    vertical;
    }                           _view;

    ChartArea*                  _area;
    AreaVector                  _areas;
    PriceVector                 _block_dates;
    int                         _bottom_space;
    int                         _cw;
    Date::FORMAT                _date_format;
    Date::RANGE                 _date_range;
    int                         _date_start;
    PriceVector                 _dates;
    int                         _margin_left;
    int                         _margin_right;
    Fl_Menu_Button*             _menu;
    int                         _old_height;
    int                         _old_width;
    Fl_Scrollbar*               _scroll;
    int                         _tick_width;
    int                         _ticks;
    std::string                 _tooltip;
    int                         _top_space;
    int                         _ver_pos[101];
};

}

// MKALGAM_OFF

#endif
