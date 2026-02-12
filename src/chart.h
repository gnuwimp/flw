/**
* @file
* @brief Chart widget.
*
* @author gnuwimp@gmail.com
* @copyright Released under the GNU General Public License v3.0
*/

#ifndef FLW_CHART_H
#define FLW_CHART_H

#include "flw.h"
#include "date.h"

// MKALGAM_ON

class Fl_Menu_Button;
class Fl_Scrollbar;

#include <cmath>
#include <FL/Fl_Group.H>
#include <FL/Fl_Rect.H>
#include <optional>

namespace flw {

/** @brief Chart widget and related classes.
*
*/
namespace chart {

class  Area;
struct Point;
class  Line;

typedef std::vector<Point> PointVector;
typedef std::vector<Line>  LineVector;
typedef std::vector<Area>  AreaVector;

static constexpr const double MAX_VALUE      = 9223372036854775807.0;   ///< @brief Max number value.
static constexpr const double MIN_VALUE      = 0.0000001;               ///< @brief Min (abs) number value.
static constexpr const int    MAX_LINE_WIDTH = 14;                      ///< @brief Max line width (pixels).
static constexpr const int    VERSION        = 6;                       ///< @brief JSON version.
static constexpr const int    MIN_TICK       = 3;                       ///< @brief Min tick size.
static constexpr const int    MAX_TICK       = MAX_LINE_WIDTH * 5;      ///< @brief Max tick size.

/** @brief Algorithms that can be used to generate new data series.
*
*/
enum class Algorithm {
    ATR,                    ///< @brief ATR.
    DAY_TO_MONTH,           ///< @brief Convert day to month.
    DAY_TO_WEEK,            ///< @brief Convert day to week.
    EXP_MOVING_AVERAGE,     ///< @brief Exponential moving average.
    FIXED,                  ///< @brief Fixed Y value.
    MODIFY,                 ///< @brief Modify data serie.
    MOMENTUM,               ///< @brief Momentum algorithm.
    MOVING_AVERAGE,         ///< @brief Moving average algorithm.
    RSI,                    ///< @brief RSI algorithm.
    STD_DEV,                ///< @brief Standard deviation.
    STOCHASTICS,            ///< @brief Stochastic algorithm.
    LAST = STOCHASTICS,     ///< @brief Same as STOCHASTICS.
};

/** @brief Different date series for the x scale.
*
*/
enum class DateRange {
    DAY,            ///< @brief All days.
    WEEKDAY,        ///< @brief Weekdays (mon - fri).
    FRIDAY,         ///< @brief Only fridays.
    SUNDAY,         ///< @brief Only sundays.
    MONTH,          ///< @brief Last day in month.
    HOUR,           ///< @brief Every hour.
    MIN,            ///< @brief Every minute.
    SEC,            ///< @brief Every second.
    LAST = SEC,     ///< @brief Same as SEC.
};

/** @brief Chart line types.
*
*/
enum class LineType {
    LINE,                               ///< @brief Simple line.
    LINE_DOT,                           ///< @brief Dotted line
    BAR,                                ///< @brief Vertical bar chart using high and low values.
    BAR_CLAMP,                          ///< @brief Vertical bar chart with forced minimum value.
    BAR_HLC,                            ///< @brief Vertical bar chart using high, low and close values.
    HORIZONTAL,                         ///< @brief A horisontal line for close value.
    EXPAND_VERTICAL,                    ///< @brief A vertical line using full chart area height.
    EXPAND_HORIZONTAL_ALL,              ///< @brief Expand all points ro full chart area width.
    EXPAND_HORIZONTAL_FIRST,            ///< @brief Expand first point to full chart area width.
    LAST = EXPAND_HORIZONTAL_FIRST,     ///< @brief Same as EXPAND_HORIZONTAL_FIRST.
};

/** @brief Modify a data serie.
*
*/
enum class Modifier {
    ADDITION,               ///< @brief Add to values.
    SUBTRACTION,            ///< @brief Subtract values.
    MULTIPLICATION,         ///< @brief Multiply values.
    DIVISION,               ///< @brief Divide values.
    LAST = DIVISION,        ///< @brief Same as SUBTRACTION.
};

/** @brief Chart area index.
*
* Up to 5 areas can be used.
*/
enum class AreaNum {
    ONE,             ///< @brief Area 1.
    TWO,             ///< @brief Area 2.
    THREE,           ///< @brief Area 3.
    FOUR,            ///< @brief Area 4.
    FIVE,            ///< @brief Area 5.
    LAST = FIVE,     ///< @brief Same as FIVE.
};

/*
 *      _____      _       _
 *     |  __ \    (_)     | |
 *     | |__) |__  _ _ __ | |_
 *     |  ___/ _ \| | '_ \| __|
 *     | |  | (_) | | | | | |_
 *     |_|   \___/|_|_| |_|\__|
 *
 *
 */

/** @brief Point class represent one data point with date and one or more y values.
*
* Data created with constructor that ends up with empty date has invalid input data.\n
* Use the constructor so the date will be converted to "YYYYMMMMDD HHMMSS" (Date::FORMAT::ISO_TIME).\n
*/
struct Point {
    std::string                 date;       ///< @brief Date value that should be in the format of "YYYYMMDD HH:MM:SS".
    double                      close;      ///< @brief Close value, should be within max/min range.
    double                      high;       ///< @brief Largest value.
    double                      low;        ///< @brief Smallest value.

    explicit                    Point();
    explicit                    Point(const std::string& date, double value = 0.0);
    explicit                    Point(const std::string& date, double high, double low, double close);
    void                        debug() const;
    bool                        operator<(const Point& other) const { return date < other.date; } ///< @brief Compare date.
    bool                        operator<=(const Point& other) const { return date <= other.date; } ///< @brief Compare date.
    bool                        operator==(const Point& other) const { return date == other.date; } ///< @brief Compare date.
    bool                        operator!=(const Point& other) const { return date != other.date; } ///< @brief Compare date.

    static PointVector          ATR(const PointVector& in, size_t days);
    static size_t               BinarySearch(const PointVector& in, const Point& key);
    static PointVector          DateSerie(const std::string& start_date, const std::string& stop_date, DateRange range, const PointVector& block = PointVector());
    static PointVector          DayToMonth(const PointVector& in, bool sum = false);
    static PointVector          DayToWeek(const PointVector& in, gnu::Date::Day weekday, bool sum = false);
    static void                 Debug(const PointVector& in);
    static PointVector          ExponentialMovingAverage(const PointVector& in, size_t days);
    static PointVector          Fixed(const PointVector& in, double value);
    static PointVector          LoadCSV(const std::string& filename, const std::string& sep = ",");
    static PointVector          Modify(const PointVector& in, Modifier modify, double value);
    static PointVector          Momentum(const PointVector& in, size_t days);
    static PointVector          MovingAverage(const PointVector& in, size_t days);
    static std::string          RangeToString(DateRange range);
    static PointVector          RSI(const PointVector& in, size_t days);
    static bool                 SaveCSV(const PointVector& in, const std::string& filename, const std::string& sep = ",");
    static PointVector          StdDev(const PointVector& in, size_t days);
    static PointVector          Stochastics(const PointVector& in, size_t days);
    static DateRange            StringToRange(const std::string& range);
};

/*
 *      _      _
 *     | |    (_)
 *     | |     _ _ __   ___
 *     | |    | | '_ \ / _ \
 *     | |____| | | | |  __/
 *     |______|_|_| |_|\___|
 *
 *
 */

/** @brief Data for one chart line.
*/
class Line {
public:
    explicit                    Line()
                                    { reset(); } ///< @brief Create empty chart line.
    explicit                    Line(const PointVector& data, const std::string& label = "", LineType type = LineType::LINE);
    Fl_Align                    align() const
                                    { return _align; } ///< @brief Return what side its y scale is on.
    Fl_Color                    color() const
                                    { return _color; } ///< @brief Get line color.
    const PointVector&          data() const
                                    { return _data; } ///< @brief Get chart data.
    void                        debug(size_t num) const;
    bool                        is_visible() const
                                    { return _visible; } ///< @brief Is line visible?
    std::string                 label() const
                                    { return _label; } ///< @brief Get line label.
    const Fl_Rect&              label_rect() const
                                    { return _rect; } ///< @brief Get line label rectangle.
    void                        reset();
    Line&                       set_align(Fl_Align val)
                                    { if (val == FL_ALIGN_LEFT || val == FL_ALIGN_RIGHT) _align = val; return *this; } ///< @brief Set scale side (only FL_ALIGN_LEFT or FL_ALIGN_RIGHT).
    Line&                       set_color(Fl_Color val)
                                    { _color = val; return *this; } ///< @brief Set line color.
    Line&                       set_data(const PointVector& val)
                                    { _data = val; return *this; } ///< @brief Set chart data.
    Line&                       set_label(const std::string& val)
                                    { _label = val; return *this; } ///< @brief Set line label.
    Line&                       set_label_rect(int x, int y, int w, int h)
                                    { _rect = Fl_Rect(x, y, w, h); return *this; } ///< @brief Set line label rectangle.
    Line&                       set_type(LineType val)
                                    { _type = val; return *this; }
    Line&                       set_type_from_string(const std::string& val);
    Line&                       set_visible(bool val)
                                    { _visible = val; return *this; } ///< @brief Set visibilty.
    Line&                       set_width(unsigned val = 1)
                                    { if (val > 0 && val <= chart::MAX_LINE_WIDTH) _width = val; return *this; } ///< @brief Set line size.
    size_t                      size() const
                                    { return _data.size(); } ///< @brief Get number of data points.
    LineType                    type() const
                                    { return _type; } ///< @brief Get line type.
    bool                        type_has_high_and_low() const
                                    { return _type == LineType::BAR || _type == LineType::BAR_CLAMP || _type == LineType::BAR_HLC; } ///< @brief Check if line type are using high and low values.
    std::string                 type_to_string() const;
    unsigned                    width() const
                                    { return _width; } ///< @brief Get line width.

private:
    PointVector                 _data;      ///< @brief Vector with data points.
    Fl_Align                    _align;     ///< @brief Side of scale, left or right.
    Fl_Color                    _color;     ///< @brief Line color.
    Fl_Rect                     _rect;      ///< @brief Label rectangle.
    LineType                    _type;      ///< @brief What line type to draw.
    bool                        _visible;   ///< @brief Visible or not.
    std::string                 _label;     ///< @brief Line label.
    unsigned                    _width;     ///< @brief Line size.
};

/*
 *       _____           _
 *      / ____|         | |
 *     | (___   ___ __ _| | ___
 *      \___ \ / __/ _` | |/ _ \
 *      ____) | (_| (_| | |  __/
 *     |_____/ \___\__,_|_|\___|
 *
 *
 */

/** @brief Chart scale object.
*
* Each Area can have a scale on the left and/or right side.\n
* Private class.\n
*/
class Scale {
public:
                                Scale();
    int                         calc_margin();
    void                        calc_tick(int height);
    void                        debug(const char* name) const;
    double                      diff() const;
    void                        fix_height();
    std::optional<double>       max() const;
    std::optional<double>       min() const;
    double                      pixel() const
                                    { return _pixel; } ///< @brief Get y value for one pixel.
    void                        reset();
    void                        set_max(double val)
                                    { _max = val; } ///< @brief Set max y value.
    void                        set_min(double val)
                                    { _min = val; } ///< @brief Set min y value.
    double                      tick() const
                                    { return _tick; } ///< @brief Get y value between every y tick.
private:
    double                      _max;       ///< @brief Max y scale value.
    double                      _min;       ///< @brief Min y scale value.
    double                      _pixel;     ///< @brief Y value for one pixel.
    double                      _tick;      ///< @brief Value between every y tick.
};

/*
 *
 *         /\
 *        /  \   _ __ ___  __ _
 *       / /\ \ | '__/ _ \/ _` |
 *      / ____ \| | |  __/ (_| |
 *     /_/    \_\_|  \___|\__,_|
 *
 *
 */

/** @brief Data for one chart area.
*
* It can have a scale on the left or right or both sides.\n
* The height is in percent, from 10 to 100.\n
* All used charts must have a total of 100 percent.\n
*/
class Area {
public:
    static const size_t         MAX_LINES = 10;

    explicit                    Area(AreaNum area)
                                    { _area = area; reset(); } ///< @brief Create chart area. @param[in] area  Area index.
    bool                        add_line(const Line& chart_line);
    AreaNum                     area() const
                                    { return _area; } ///< @brief Get area index.
    std::optional<double>       clamp_max() const;
    std::optional<double>       clamp_min() const;
    void                        debug() const;
    void                        delete_line(size_t index);
    Scale&                      left_scale()
                                    { return _left; } ///< @brief Get left scale.
    Line*                       line(size_t index)
                                    { return (index < _lines.size()) ? &_lines[index] : nullptr; } ///< @brief Get chart line.
    const LineVector&           lines() const
                                    { return _lines; } ///< @brief Get all chart lines.
    int                         percent() const
                                    { return _percent; } ///< @brief Get size as a percent value.
    Fl_Rect&                    rect()
                                    { return _rect; } ///< @brief Get area rectangle.
    void                        reset();
    Scale&                      right_scale()
                                    { return _right; } ///< @brief Get right scale.
    size_t                      selected() const
                                    { return _selected; } ///< @brief Return which line is selected.
    Line*                       selected_line();
    void                        set_max_clamp(double val = INFINITY)
                                    { _clamp_max = val; } ///< @brief Set max clamp value.
    void                        set_min_clamp(double val = INFINITY)
                                    { _clamp_min = val; } ///< @brief Set min clamp value.
    void                        set_percent(int val)
                                    { _percent = val; } ///< @brief Set size as a percent value. @param[in] val  From 10 to 100.
    void                        set_selected(size_t val)
                                    { _selected = val; } ///< @brief Set which line should be selected.
    size_t                      size() const
                                    { return _lines.size(); } ///< @brief Get number of chart lines.

private:

    AreaNum                     _area;          ///< @brief Area index.
    LineVector                  _lines;         ///< @brief Vector with Line objects.
    Scale                       _left;          ///< @brief Left y scale.
    Scale                       _right;         ///< @brief Right y scale.
    Fl_Rect                     _rect;          ///< @brief Area rectangle.
    double                      _clamp_max;     ///< @brief Max clamp value, default to INFINITY.
    double                      _clamp_min;     ///< @brief Min clamp value, default to INFINITY.
    int                         _percent;       ///< @brief Size as a percent value from 10 to 100.
    size_t                      _selected;      ///< @brief Selected chart line index in _lines.

};

/*
 *       _____ _                _
 *      / ____| |              | |
 *     | |    | |__   __ _ _ __| |_
 *     | |    | '_ \ / _` | '__| __|
 *     | |____| | | | (_| | |  | |_
 *      \_____|_| |_|\__,_|_|   \__|
 *
 *
 */

/** @brief Chart widget for displaying time series.
*
* It can have 1 to 5 chart areas, which are aligned vertically.\n
* All areas use the same x date serie.\n
* Lines can be toggled on/off.\n
* It has an popup menu for changing settings and modify chart lines.\n
* Charts can be saved or loaded from/to a json file.\n
*
* @snippet chart.cpp flw::chart::Chart example
* @image html chart.png
*/
class Chart : public Fl_Group {
    static const size_t         MAX_VLINES = 1600;

public:
    explicit                    Chart(int X = 0, int Y = 0, int W = 0, int H = 0, const char* l = nullptr);
    double                      alt_size() const
                                    { return _alt_size; } ///< @brief Get tweaked x label font size (for days and time).
    Area&                       area(AreaNum area)
                                    { return _areas[static_cast<size_t>(area)]; } ///< @brief Get chart area object.
    bool                        create_line(Algorithm formula, bool support = false);
    void                        debug() const;
    void                        debug_line() const;
    void                        disable_menu()
                                    { _disable_menu = true; } ///< @brief Disable popup menu.
    void                        do_layout()
                                    { _old = Fl_Rect(); resize(x(), y(), w(), h()); redraw(); } ///< @brief Resize widget.
    void                        draw() override;
    void                        enable_menu()
                                    { _disable_menu = false; } ///< @brief Enable popup menu.
    std::string                 filename() const
                                    { return _filename; } ///< @brief Return latest  json filename.
    int                         handle(int event) override;
    bool                        hor_lines() const
                                    { return _horizontal; } ///< @brief Are horisontal lines shown?
    void                        init()
                                    { _init(false); } ///< @brief Initiate chart but dont create new date serie.
    void                        init_new_data()
                                    { _init(true); } ///< @brief Initiate chart and create new date serie.
    bool                        line_labels() const
                                    { return _labels; } ///< @brief Are labels shown?.
    bool                        load_json();
    bool                        load_json(const std::string& filename);
    bool                        load_line_from_csv();
    std::string                 main_label() const
                                    { return _label; } ///< @brief Get main label.
    void                        print_to_postscript();
    void                        reset();
    void                        resize(int X, int Y, int W, int H) override;
    bool                        save_json();
    bool                        save_json(const std::string& filename, double max_diff_high_low = 0.001);
    bool                        save_line_to_csv();
    bool                        save_png();
    void                        set_alt_size(double val = 0.8)
                                    { if (val >= 0.6 && val <= 1.2) _alt_size = val; } ///< @brief Tweak fontsize for x labels  (for days and time). @param[in] val  Valid number is 0.6 to 1.2 (default 0.8).
    bool                        set_area_size(unsigned area1 = 100, unsigned area2 = 0, unsigned area3 = 0, unsigned area4 = 0, unsigned area5 = 0);
    void                        set_block_dates(const PointVector& block_dates)
                                    { _block_dates = block_dates; } ///< @brief Set block date list.
    void                        set_date_range(DateRange range = DateRange::DAY)
                                    { _date_range  = range; } ///< @brief Set date range.
    void                        set_hor_lines(bool val = true)
                                    { _horizontal = val; } ///< @brief Show/hide horisontal lines.
    void                        set_line_labels(bool val = true)
                                    { _labels = val; } ///< @brief Show/hide line labels.
    void                        set_main_label(std::string label = "")
                                    { _label = label; } ///< @brief Set main chart label.
    void                        set_tick_width(int val = chart::MIN_TICK)
                                    { if (val >= MIN_TICK && val <= MAX_TICK) _tick_width = val; } ///< @brief Set tich width. @param[in] val  Tick value from MIN_TICK to MAX_TICK.
    void                        set_ver_lines(bool val = true)
                                    { _vertical = val; } ///< @brief Show/hide vertical lines.
    void                        setup_area();
    void                        setup_clamp(bool min = true);
    void                        setup_create_line();
    void                        setup_date_range();
    void                        setup_delete_lines();
    void                        setup_label();
    void                        setup_line();
    void                        setup_move_lines();
    void                        setup_show_or_hide_lines();
    void                        setup_view_options();
    void                        update_pref();
    bool                        ver_lines() const
                                    { return _vertical; } ///< @brief Are vertical lines visible?

private:
    /** @brief Values for selecting lines.
    *
    */
    enum class LabelType {
                                OFF,                ///< @brief All checkboxes are off.
                                ON,                 ///< @brief All checkboxes are on.
                                VISIBLE,            ///< @brief Visible lines are on.
                                LAST = VISIBLE,     ///< @brief Same as VISIBLE.
    };

    void                        _calc_area_height();
    void                        _calc_area_width();
    void                        _calc_dates();
    void                        _calc_margins();
    void                        _calc_ymin_ymax();
    void                        _calc_yscale();
    void                        _create_tooltip(bool ctrl);
    void                        _draw_label();
    void                        _draw_lines(Area& area);
    void                        _draw_line_labels(Area& area);
    void                        _draw_tooltip();
    void                        _draw_ver_lines(Area& area);
    void                        _draw_xlabels();
    void                        _draw_ylabels(Area& area, Fl_Align align);
    Area*                       _get_active_area(int X, int Y);
    void                        _init(bool calc_dates);
    StringVector                _label_array(const Area& area, LabelType labeltype) const;
    bool                        _move_or_delete_line(Area* area, size_t index, bool move, AreaNum destination = AreaNum::ONE);
    void                        _show_menu();

    static bool                 _CallbackPrinter(void* data, int pw, int ph, unsigned page);
    static void                 _CallbackScrollbar(Fl_Widget*, void* widget);

    Area*                       _area;                  ///< @brief Active area, set in handle() when area is clicked.
    AreaVector                  _areas;                 ///< @brief Area objects, all are using same DateRange.
    DateRange                   _date_range;            ///< @brief What kind of date serie that are generated.
    PointVector                 _block_dates;           ///< @brief Vector with dates that are removed from the date list.
    PointVector                 _dates;                 ///< @brief Vector with dates from first to last date according to _date_range.
    Fl_Menu_Button*             _menu;                  ///< @brief Popup menu.
    Fl_Rect                     _old;                   ///< @brief Block resizes.
    Fl_Scrollbar*               _scroll;                ///< @brief Horizontal scroll bar.
    bool                        _disable_menu;          ///< @brief Flag to disable popup menu.
    bool                        _horizontal;            ///< @brief Horizontal support lines.
    bool                        _labels;                ///< @brief Line labels.
    bool                        _printing;              ///< @brief Used when printing.
    bool                        _vertical;              ///< @brief Vertical support lines.
    const int                   _CH;                    ///< @brief Font height.
    const int                   _CW;                    ///< @brief Font width.
    double                      _alt_size;              ///< @brief Alternative size for day labels.
    int                         _bottom_space;          ///< @brief Bottom space from last area to h() in pixels.
    int                         _date_start;            ///< @brief Index in _dates vector for first date to display, changed by the scrollbar.
    int                         _margin_left;           ///< @brief Width of left y scale in characters.
    int                         _margin_right;          ///< @brief Width of right y scale in characters.
    int                         _tick_width;            ///< @brief Number of pixels for every tick.
    int                         _ticks;                 ///< @brief Number of ticks in the view.
    int                         _top_space;             ///< @brief Top space from y() to first area in pixels.
    int                         _ver_pos[MAX_VLINES];   ///< @brief Save x pos for vertical lines.
    std::string                 _filename;              ///< @brief Last used filename.
    std::string                 _label;                 ///< @brief Top label.
    std::string                 _tooltip;               ///< @brief For the popup tooltip when pressing lb inside area.
};

} // namespace chart
} // namespace flw

// MKALGAM_OFF

#endif // FLW_CHART_H
