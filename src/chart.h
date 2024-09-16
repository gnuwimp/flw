// Copyright gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#ifndef FLW_CHART_H
#define FLW_CHART_H

#include "flw.h"
#include "date.h"

// MKALGAM_ON

#include <FL/Fl_Group.H>
#include <FL/Fl_Menu_Button.H>
#include <FL/Fl_Rect.H>
#include <FL/Fl_Scrollbar.H>
#include <cmath>
#include <optional>
#include <vector>

namespace flw {

class  ChartArea;
struct ChartData;
class  ChartLine;

typedef std::vector<ChartData>  ChartDataVector;
typedef std::vector<ChartLine>  ChartLineVector;
typedef std::vector<ChartArea>  ChartAreaVector;

/***
 *       _____ _                _   _____        _
 *      / ____| |              | | |  __ \      | |
 *     | |    | |__   __ _ _ __| |_| |  | | __ _| |_ __ _
 *     | |    | '_ \ / _` | '__| __| |  | |/ _` | __/ _` |
 *     | |____| | | | (_| | |  | |_| |__| | (_| | || (_| |
 *      \_____|_| |_|\__,_|_|   \__|_____/ \__,_|\__\__,_|
 *
 *
 */

//------------------------------------------------------------------------------
// Data created with constructor that ends up with empty date has invalid input data.
// Use the constructor so the date will be converted to "YYYYMMMMDD HHMMSS" (Date::FORMAT::ISO_TIME).
//
struct ChartData {
    enum class RANGE {
                                DAY,
                                WEEKDAY,
                                FRIDAY,
                                SUNDAY,
                                MONTH,
                                HOUR,
                                MIN,
                                SEC,
                                LAST = SEC,
    };

    enum class FORMULAS {
                                ATR,
                                DAY_TO_MONTH,
                                DAY_TO_WEEK,
                                EXP_MOVING_AVERAGE,
                                FIXED,
                                MODIFY,
                                MOMENTUM,
                                MOVING_AVERAGE,
                                RSI,
                                STD_DEV,
                                STOCHASTICS,
                                LAST = STOCHASTICS,
    };

    enum class MODIFY {
                                ADDITION,
                                SUBTRACTION,
                                MULTIPLICATION,
                                DIVISION,
                                LAST = DIVISION,
    };

    std::string                 date;
    double                      close;
    double                      high;
    double                      low;

    explicit                    ChartData();
    explicit                    ChartData(std::string date_value, double value = 0.0);
    explicit                    ChartData(std::string date, double high, double low, double close);
    void                        debug() const;
    bool                        operator<(const ChartData& other) const { return date < other.date; }
    bool                        operator<=(const ChartData& other) const { return date <= other.date; }
    bool                        operator==(const ChartData& other) const { return date == other.date; }
    bool                        operator!=(const ChartData& other) const { return date != other.date; }

    static ChartDataVector      ATR(const ChartDataVector& in, size_t days);
    static size_t               BinarySearch(const ChartDataVector& in, const ChartData& key);
    static ChartDataVector      DateSerie(std::string start_date, std::string stop_date, ChartData::RANGE range, const ChartDataVector& block = ChartDataVector());
    static ChartDataVector      DayToMonth(const ChartDataVector& in, bool sum = false);
    static ChartDataVector      DayToWeek(const ChartDataVector& in, Date::DAY weekday, bool sum = false);
    static void                 Debug(const ChartDataVector& in);
    static ChartDataVector      ExponentialMovingAverage(const ChartDataVector& in, size_t days);
    static ChartDataVector      Fixed(const ChartDataVector& in, double value);
    static ChartDataVector      LoadCSV(std::string filename, std::string sep = ",");
    static ChartDataVector      Modify(const ChartDataVector& in, ChartData::MODIFY modify, double value);
    static ChartDataVector      Momentum(const ChartDataVector& in, size_t days);
    static ChartDataVector      MovingAverage(const ChartDataVector& in, size_t days);
    static ChartDataVector      RSI(const ChartDataVector& in, size_t days);
    static bool                 SaveCSV(const ChartDataVector& in, std::string filename, std::string sep = ",");
    static ChartDataVector      StdDev(const ChartDataVector& in, size_t days);
    static ChartDataVector      Stochastics(const ChartDataVector& in, size_t days);
};

/***
 *       _____ _                _   _      _
 *      / ____| |              | | | |    (_)
 *     | |    | |__   __ _ _ __| |_| |     _ _ __   ___
 *     | |    | '_ \ / _` | '__| __| |    | | '_ \ / _ \
 *     | |____| | | | (_| | |  | |_| |____| | | | |  __/
 *      \_____|_| |_|\__,_|_|   \__|______|_|_| |_|\___|
 *
 *
 */

//------------------------------------------------------------------------------
// Data for one chart line.
//
class ChartLine {
public:
    static const int            MAX_WIDTH = 25;

    enum class TYPE {
                                LINE,
                                LINE_DOT,
                                BAR,
                                BAR_CLAMP,
                                BAR_HLC,
                                HORIZONTAL,
                                EXPAND_VERTICAL,
                                EXPAND_HORIZONTAL,
                                LAST = EXPAND_HORIZONTAL,
    };

    explicit                    ChartLine()
                                    { clear(); }
    explicit                    ChartLine(const ChartDataVector& data, std::string label = "", TYPE type = ChartLine::TYPE::LINE);
    Fl_Align                    align() const
                                    { return _align; }
    void                        clear();
    Fl_Color                    color() const
                                    { return _color; }
    const ChartDataVector&      data_vector() const
                                    { return _data; }
    void                        debug(int num, bool prices = false) const;
    bool                        is_visible() const
                                    { return _visible; }
    std::string                 label() const
                                    { return _label; }
    const Fl_Rect&              label_rect() const
                                    { return _rect; }
    ChartLine&                  set_align(Fl_Align val)
                                    { if (val == FL_ALIGN_LEFT || val == FL_ALIGN_RIGHT) _align = val; return *this; }
    ChartLine&                  set_color(Fl_Color val)
                                    { _color = val; return *this; }
    ChartLine&                  set_data(const ChartDataVector& data);
    ChartLine&                  set_label(std::string val)
                                    { _label = val; return *this; }
    ChartLine&                  set_label_rect(int x, int y, int w, int h)
                                    { _rect = Fl_Rect(x, y, w, h); return *this; }
    ChartLine&                  set_type(TYPE val)
                                    { _type = val; return *this; }
    ChartLine&                  set_type_from_string(std::string val);
    ChartLine&                  set_visible(bool val)
                                    { _visible = val; return *this; }
    ChartLine&                  set_width(unsigned width)
                                    { if (width <= ChartLine::MAX_WIDTH) _width = width; return *this; }
    size_t                      size() const
                                    { return _data.size(); }
    TYPE                        type() const
                                    { return _type; }
    bool                        type_has_hlc() const
                                    { return _type == TYPE::BAR || _type == TYPE::BAR_CLAMP || _type == ChartLine::TYPE::BAR_HLC; }
    std::string                 type_to_string() const;
    unsigned                    width() const
                                    { return _width; }

private:
    ChartDataVector             _data;
    Fl_Align                    _align;
    Fl_Color                    _color;
    Fl_Rect                     _rect;
    TYPE                        _type;
    bool                        _visible;
    double                      _max;
    double                      _min;
    std::string                 _label;
    unsigned                    _width;
};

/***
 *       _____ _                _    _____           _
 *      / ____| |              | |  / ____|         | |
 *     | |    | |__   __ _ _ __| |_| (___   ___ __ _| | ___
 *     | |    | '_ \ / _` | '__| __|\___ \ / __/ _` | |/ _ \
 *     | |____| | | | (_| | |  | |_ ____) | (_| (_| | |  __/
 *      \_____|_| |_|\__,_|_|   \__|_____/ \___\__,_|_|\___|
 *
 *
 */

//------------------------------------------------------------------------------
// Each chart area can have a scale on the left and/or right side.
//
class ChartScale {
public:
                                ChartScale();
    void                        calc(int height);
    void                        clear();
    void                        debug(const char* name) const;
    double                      diff() const
                                    { return _max - _min; }
    void                        fix_height();
    std::optional<double>       max() const;
    std::optional<double>       min() const;
    double                      pixel() const
                                    { return _pixel; }
    void                        set_max(double val)
                                    { _max = val; }
    void                        set_min(double val)
                                    { _min = val; }
    double                      tick() const
                                    { return _tick; }
private:
    double                      _max;
    double                      _min;
    double                      _pixel;
    double                      _tick;
};

/***
 *       _____ _                _
 *      / ____| |              | |     /\
 *     | |    | |__   __ _ _ __| |_   /  \   _ __ ___  __ _
 *     | |    | '_ \ / _` | '__| __| / /\ \ | '__/ _ \/ _` |
 *     | |____| | | | (_| | |  | |_ / ____ \| | |  __/ (_| |
 *      \_____|_| |_|\__,_|_|   \__/_/    \_\_|  \___|\__,_|
 *
 *
 */

//------------------------------------------------------------------------------
// Data for one chart area.
//
class ChartArea {
public:
    static const size_t         MAX_LINES = 10;

    enum class NUM {
                                ONE,
                                TWO,
                                THREE,
                                FOUR,
                                FIVE,
                                LAST = FIVE,
    };

    enum class LABELTYPE {
                                OFF,
                                ON,
                                VISIBLE,
                                LAST = VISIBLE,
    };

    explicit                    ChartArea(ChartArea::NUM num)
                                    { _num = num; clear(); }
    bool                        add_line(const ChartLine& chart_line);
    std::optional<double>       clamp_max() const;
    std::optional<double>       clamp_min() const;
    void                        clear();
    void                        debug() const;
    void                        delete_line(size_t index);
    int                         h() const
                                    { return _h; }
    StringVector                label_array(ChartArea::LABELTYPE labeltype) const;
    ChartScale&                 left_scale()
                                    { return _left; }
    ChartLine*                  line(size_t index)
                                    { return (index < _lines.size()) ? &_lines[index] : nullptr; }
    const ChartLineVector&      lines() const
                                    { return _lines; }
    NUM                         num() const
                                    { return _num; }
    int                         percent() const
                                    { return _percent; }
    ChartScale&                 right_scale()
                                    { return _right; }
    size_t                      selected() const
                                    { return _selected; }
    ChartLine*                  selected_line();
    void                        set_max_clamp(double value = INFINITY)
                                    { _clamp_max = value; }
    void                        set_min_clamp(double value = INFINITY)
                                    { _clamp_min = value; }
    void                        set_h(int h)
                                    { _h = h; }
    void                        set_percent(int val)
                                    { _percent = val; }
    void                        set_selected(size_t val)
                                    { _selected = val; }
    void                        set_visible(size_t line, bool val);
    void                        set_w(int w)
                                    { _w = w; }
    void                        set_x(int x)
                                    { _x = x; }
    void                        set_y(int y)
                                    { _y = y; }
    size_t                      size() const
                                    { return _lines.size(); }
    int                         w() const
                                    { return _w; }
    int                         x() const
                                    { return _x; }
    int                         x2() const
                                    { return _x + _w; }
    int                         y() const
                                    { return _y; }
    int                         y2() const
                                    { return _y + _h; }

private:
    NUM                         _num;
    ChartLineVector             _lines;
    ChartScale                  _left;
    ChartScale                  _right;
    double                      _clamp_max;
    double                      _clamp_min;
    int                         _h;
    int                         _w;
    int                         _x;
    int                         _y;
    int                         _percent;
    size_t                      _selected;

};

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
// Chart widget that is using dates for the x scale.
// It has an popup menu for changing settings and modify chart lines.
//
class Chart : public Fl_Group {
public:
    static const int            MIN_AREA_SIZE =   10;
    static const int            MIN_MARGIN    =    3;
    static const int            DEF_MARGIN    =    6;
    static const int            MAX_MARGIN    =   20;
    static const int            MIN_TICK      =    3;
    static const int            MAX_TICK      = ChartLine::MAX_WIDTH * 2;
    static const int            VERSION       =    4;
    static const size_t         MAX_VLINES    = 1400;

    explicit                    Chart(int X = 0, int Y = 0, int W = 0, int H = 0, const char* l = nullptr);
    void                        block_dates(const ChartDataVector& block_dates)
                                    { _block_dates = block_dates; }
    ChartArea&                  area(ChartArea::NUM num)
                                    { return _areas[static_cast<size_t>(num)]; }
    void                        clear();
    bool                        create_line(ChartData::FORMULAS formula, bool support = false);
    Date::FORMAT                date_format() const
                                    { return _date_format; }
    void                        debug() const;
    void                        disable_menu(bool value = true)
                                    { _disable_menu = value; }
    void                        do_layout()
                                    { _old = Fl_Rect(); resize(x(), y(), w(), h()); redraw(); }
    void                        draw() override;
    int                         handle(int event) override;
    void                        init(bool calc_dates);
    bool                        load_cvs();
    bool                        load_json();
    bool                        load_json(std::string filename);
    void                        print_to_postscript();
    void                        resize(int X, int Y, int W, int H) override;
    bool                        save_cvs();
    bool                        save_json();
    bool                        save_json(std::string filename, double max_diff_high_low = 0.001) const;
    bool                        save_png();
    bool                        set_area_size(int area1 = 100, int area2 = 0, int area3 = 0, int area4 = 0, int area5 = 0);
    void                        set_date_range(ChartData::RANGE range = ChartData::RANGE::DAY);
    void                        set_label(std::string label)
                                    { _label = label; }
    bool                        set_margin(int def);
    bool                        set_tick_width(int width = Chart::MIN_TICK);
    void                        setup_add_line();
    void                        setup_area();
    void                        setup_clamp(bool min = true);
    void                        setup_date_range();
    void                        setup_delete_lines();
    void                        setup_label();
    void                        setup_line();
    void                        setup_move_lines();
    void                        setup_show_or_hide_lines();
    void                        setup_ywidth();
    void                        update_pref();
    void                        view_options(bool line_labels = true, bool hor_lines = true, bool ver_lines = true)
                                    { _view.labels = line_labels; _view.horizontal = hor_lines; _view.vertical = ver_lines; redraw(); }

private:
    void                        _calc_area_height();
    void                        _calc_area_width();
    void                        _calc_dates();
    void                        _calc_ymin_ymax();
    void                        _calc_yscale();
    void                        _calc_ywidth();
    void                        _create_tooltip(bool ctrl);
    void                        _draw_area(ChartArea& area);
    void                        _draw_label();
    void                        _draw_line(const ChartLine& line, const ChartScale& scale, int X, int Y, int W, int H);
    void                        _draw_line_labels(const ChartArea& area);
    void                        _draw_tooltip();
    void                        _draw_ver_lines(const ChartArea& area);
    void                        _draw_xlabels();
    void                        _draw_ylabels(int X, double Y1, double Y2, const ChartScale& scale, bool left);
    ChartArea*                  _inside_area(int X, int Y);
    bool                        _move_or_delete_line(ChartArea* area, size_t index, bool move, ChartArea::NUM destination = ChartArea::NUM::ONE);
    void                        _show_menu();

    static void                 _CallbackDebugChart(Fl_Widget*, void* widget);
    static void                 _CallbackDebugLine(Fl_Widget*, void* widget);
    static bool                 _CallbackPrinter(void* data, int pw, int ph, int page);
    static void                 _CallbackScrollbar(Fl_Widget*, void* widget);
    static void                 _CallbackToggle(Fl_Widget*, void* widget);

    struct {
        bool                    horizontal;             // Horizontal support lines.
        bool                    labels;                 // Line labels.
        bool                    vertical;               // Vertical support lines.
    }                           _view;

    ChartArea*                  _area;                  // Active area, set in handle() when area is clicked.
    ChartAreaVector             _areas;                 // Area objects, all are using same ChartData::RANGE.
    ChartData::RANGE            _date_range;            // What kind of date serie that are generated.
    ChartDataVector             _block_dates;           // Vector with dates that are removed from the date list.
    ChartDataVector             _dates;                 // Vector with dates from first to last date according to _date_range.
    Date::FORMAT                _date_format;           // Date format for drawing date string.
    Fl_Menu_Button*             _menu;                  // Popup menu.
    Fl_Rect                     _old;
    Fl_Scrollbar*               _scroll;                // Horizontal scroll bar.
    bool                        _disable_menu;
    bool                        _printing;              // Used when printing.
    int                         _bottom_space;
    int                         _cw;                    // Character width in pixels.
    int                         _date_start;            // Index in _dates vector for first date to display, changed by the scrollbar.
    int                         _margin;                // The number of characters if y values should be displayed.
    int                         _margin_left;           // Number of characters for the y values.
    int                         _margin_right;          // Number of characters for the y values.
    int                         _tick_width;            // Number of pixels for every tick.
    int                         _ticks;                 // Number of ticks in the view.
    int                         _top_space;
    int                         _ver_pos[MAX_VLINES];   // Save x pos for vertical lines.
    std::string                 _label;
    std::string                 _tooltip;
};

} // namespace flw

// MKALGAM_OFF

#endif // FLW_CHART_H
