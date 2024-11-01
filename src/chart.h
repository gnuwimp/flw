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
    static constexpr const double MAX_VALUE = 9223372036854775807.0;
    static constexpr const double MIN_VALUE = 0.0000001;

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
                                DIVISION,
                                MULTIPLICATION,
                                SUBTRACTION,
                                LAST = SUBTRACTION,
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
    static ChartDataVector      DayToWeek(const ChartDataVector& in, gnu::Date::DAY weekday, bool sum = false);
    static void                 Debug(const ChartDataVector& in);
    static ChartDataVector      ExponentialMovingAverage(const ChartDataVector& in, size_t days);
    static ChartDataVector      Fixed(const ChartDataVector& in, double value);
    static ChartDataVector      LoadCSV(std::string filename, std::string sep = ",");
    static ChartDataVector      Modify(const ChartDataVector& in, ChartData::MODIFY modify, double value);
    static ChartDataVector      Momentum(const ChartDataVector& in, size_t days);
    static ChartDataVector      MovingAverage(const ChartDataVector& in, size_t days);
    static std::string          RangeToString(ChartData::RANGE range);
    static ChartDataVector      RSI(const ChartDataVector& in, size_t days);
    static bool                 SaveCSV(const ChartDataVector& in, std::string filename, std::string sep = ",");
    static ChartDataVector      StdDev(const ChartDataVector& in, size_t days);
    static ChartDataVector      Stochastics(const ChartDataVector& in, size_t days);
    static ChartData::RANGE     StringToRange(std::string range);
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
    static const int            MAX_WIDTH = 14;

    enum class TYPE {
                                LINE,
                                LINE_DOT,
                                BAR,
                                BAR_CLAMP,
                                BAR_HLC,
                                HORIZONTAL,
                                EXPAND_VERTICAL,
                                EXPAND_HORIZONTAL_ALL,
                                EXPAND_HORIZONTAL_FIRST,
                                LAST = EXPAND_HORIZONTAL_FIRST,
    };

    explicit                    ChartLine()
                                    { clear(); }
    explicit                    ChartLine(const ChartDataVector& data, std::string label = "", TYPE type = ChartLine::TYPE::LINE);
    Fl_Align                    align() const
                                    { return _align; }
    void                        clear();
    Fl_Color                    color() const
                                    { return _color; }
    const ChartDataVector&      data() const
                                    { return _data; }
    void                        debug(size_t num) const;
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
    ChartLine&                  set_data(const ChartDataVector& val)
                                    { _data = val; return *this; }
    ChartLine&                  set_label(std::string val)
                                    { _label = val; return *this; }
    ChartLine&                  set_label_rect(int x, int y, int w, int h)
                                    { _rect = Fl_Rect(x, y, w, h); return *this; }
    ChartLine&                  set_type(TYPE val)
                                    { _type = val; return *this; }
    ChartLine&                  set_type_from_string(std::string val);
    ChartLine&                  set_visible(bool val)
                                    { _visible = val; return *this; }
    ChartLine&                  set_width(unsigned val = 1)
                                    { if (val > 0 && val <= ChartLine::MAX_WIDTH) _width = val; return *this; }
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
    int                         calc_margin();
    void                        calc_tick(int height);
    void                        clear();
    void                        debug(const char* name) const;
    double                      diff() const;
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

    enum class AREA {
                                ONE,
                                TWO,
                                THREE,
                                FOUR,
                                FIVE,
                                LAST = FIVE,
    };

    explicit                    ChartArea(ChartArea::AREA area)
                                    { _area = area; clear(); }
    bool                        add_line(const ChartLine& chart_line);
    AREA                        area() const
                                    { return _area; }
    std::optional<double>       clamp_max() const;
    std::optional<double>       clamp_min() const;
    void                        clear();
    void                        debug() const;
    void                        delete_line(size_t index);
    ChartScale&                 left_scale()
                                    { return _left; }
    ChartLine*                  line(size_t index)
                                    { return (index < _lines.size()) ? &_lines[index] : nullptr; }
    const ChartLineVector&      lines() const
                                    { return _lines; }
    int                         percent() const
                                    { return _percent; }
    Fl_Rect&                    rect()
                                    { return _rect; }
    ChartScale&                 right_scale()
                                    { return _right; }
    size_t                      selected() const
                                    { return _selected; }
    ChartLine*                  selected_line();
    void                        set_max_clamp(double val = INFINITY)
                                    { _clamp_max = val; }
    void                        set_min_clamp(double val = INFINITY)
                                    { _clamp_min = val; }
    void                        set_percent(int val)
                                    { _percent = val; }
    void                        set_selected(size_t val)
                                    { _selected = val; }
    size_t                      size() const
                                    { return _lines.size(); }

private:

    AREA                        _area;
    ChartLineVector             _lines;
    ChartScale                  _left;
    ChartScale                  _right;
    Fl_Rect                     _rect;
    double                      _clamp_max;
    double                      _clamp_min;
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
    static const size_t         MAX_VLINES = 1400;

public:
    static const int            VERSION  =    5;
    static const int            MIN_TICK =    3;
    static const int            MAX_TICK = ChartLine::MAX_WIDTH * 5;

    explicit                    Chart(int X = 0, int Y = 0, int W = 0, int H = 0, const char* l = nullptr);
    double                      alt_size() const
                                    { return _alt_size; }
    ChartArea&                  area(ChartArea::AREA area)
                                    { return _areas[static_cast<size_t>(area)]; }
    void                        clear();
    bool                        create_line(ChartData::FORMULAS formula, bool support = false);
    void                        debug() const;
    void                        debug_line() const;
    void                        disable_menu()
                                    { _disable_menu = true; }
    void                        do_layout()
                                    { _old = Fl_Rect(); resize(x(), y(), w(), h()); redraw(); }
    void                        draw() override;
    void                        enable_menu()
                                    { _disable_menu = false; }
    std::string                 filename() const
                                    { return _filename; }
    int                         handle(int event) override;
    bool                        hor_lines() const
                                    { return _horizontal; }
    void                        init()
                                    { _init(false); }
    void                        init_new_data()
                                    { _init(true); }
    bool                        line_labels() const
                                    { return _labels; }
    bool                        load_csv();
    bool                        load_json();
    bool                        load_json(std::string filename);
    std::string                 main_label() const
                                    { return _label; }
    void                        print_to_postscript();
    void                        resize(int X, int Y, int W, int H) override;
    bool                        save_csv();
    bool                        save_json();
    bool                        save_json(std::string filename, double max_diff_high_low = 0.001) const;
    bool                        save_png();
    void                        set_alt_size(double val = 0.8)
                                    { if (val >= 0.6 && val <= 1.2) _alt_size = val; }
    bool                        set_area_size(unsigned area1 = 100, unsigned area2 = 0, unsigned area3 = 0, unsigned area4 = 0, unsigned area5 = 0);
    void                        set_block_dates(const ChartDataVector& block_dates)
                                    { _block_dates = block_dates; }
    void                        set_date_range(ChartData::RANGE range = ChartData::RANGE::DAY)
                                    { _date_range  = range; }
    void                        set_hor_lines(bool val = true)
                                    { _horizontal = val; }
    void                        set_line_labels(bool val = true)
                                    { _labels = val; }
    void                        set_main_label(std::string label = "")
                                    { _label = label; }
    void                        set_tick_width(int val = Chart::MIN_TICK)
                                    { if (val >= MIN_TICK && val <= MAX_TICK) _tick_width = val; }
    void                        set_ver_lines(bool val = true)
                                    { _vertical = val; }
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
                                    { return _vertical; }

private:
    enum class LABELTYPE {
                                OFF,
                                ON,
                                VISIBLE,
                                LAST = VISIBLE,
    };

    void                        _calc_area_height();
    void                        _calc_area_width();
    void                        _calc_dates();
    void                        _calc_margins();
    void                        _calc_ymin_ymax();
    void                        _calc_yscale();
    void                        _create_tooltip(bool ctrl);
    void                        _draw_label();
    void                        _draw_lines(ChartArea& area);
    void                        _draw_line_labels(ChartArea& area);
    void                        _draw_tooltip();
    void                        _draw_ver_lines(ChartArea& area);
    void                        _draw_xlabels();
    void                        _draw_ylabels(ChartArea& area, Fl_Align align);
    ChartArea*                  _get_active_area(int X, int Y);
    void                        _init(bool calc_dates);
    StringVector                _label_array(const ChartArea& area, Chart::LABELTYPE labeltype) const;
    bool                        _move_or_delete_line(ChartArea* area, size_t index, bool move, ChartArea::AREA destination = ChartArea::AREA::ONE);
    void                        _show_menu();

    static bool                 _CallbackPrinter(void* data, int pw, int ph, int page);
    static void                 _CallbackScrollbar(Fl_Widget*, void* widget);

    ChartArea*                  _area;                  // Active area, set in handle() when area is clicked.
    ChartAreaVector             _areas;                 // Area objects, all are using same ChartData::RANGE.
    ChartData::RANGE            _date_range;            // What kind of date serie that are generated.
    ChartDataVector             _block_dates;           // Vector with dates that are removed from the date list.
    ChartDataVector             _dates;                 // Vector with dates from first to last date according to _date_range.
    Fl_Menu_Button*             _menu;                  // Popup menu.
    Fl_Rect                     _old;                   // Block resizes.
    Fl_Scrollbar*               _scroll;                // Horizontal scroll bar.
    bool                        _disable_menu;
    bool                        _horizontal;            // Horizontal support lines.
    bool                        _labels;                // Line labels.
    bool                        _printing;              // Used when printing.
    bool                        _vertical;              // Vertical support lines.
    const int                   _CH;                    // Font height.
    const int                   _CW;                    // Font width.
    double                      _alt_size;              // Alternative size for day labels.
    int                         _bottom_space;          // Bottom space from last area to h() in pixels.
    int                         _date_start;            // Index in _dates vector for first date to display, changed by the scrollbar.
    int                         _margin_left;           // Width of left y scale in characters.
    int                         _margin_right;          // Width of right y scale in characters.
    int                         _tick_width;            // Number of pixels for every tick.
    int                         _ticks;                 // Number of ticks in the view.
    int                         _top_space;             // Top space from y() to first area in pixels.
    int                         _ver_pos[MAX_VLINES];   // Save x pos for vertical lines.
    std::string                 _filename;              // Last used filename.
    std::string                 _label;                 // Top label.
    std::string                 _tooltip;               // For the popup tooltip when pressing lb inside area.
};

} // namespace flw

// MKALGAM_OFF

#endif // FLW_CHART_H
