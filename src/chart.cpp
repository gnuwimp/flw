// Copyright gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#include "chart.h"
#include "flw.h"
#include "json.h"
#include "waitcursor.h"

// MKALGAM_ON

#include <FL/Fl_Menu_Button.H>
#include <FL/Fl_Scrollbar.H>
#include <FL/fl_ask.H>
#include <FL/fl_draw.H>
#include <algorithm>
#include <math.h>

namespace flw {

/***
 *                 _            _
 *                (_)          | |
 *      _ __  _ __ ___   ____ _| |_ ___
 *     | '_ \| '__| \ \ / / _` | __/ _ \
 *     | |_) | |  | |\ V / (_| | ||  __/
 *     | .__/|_|  |_| \_/ \__,_|\__\___|
 *     | |
 *     |_|
 */

static const int                _CHART_MAX_VLINES      = 100;
static const int                _CHART_MAX_LINE_WIDTH  = 100;
static const char* const        _CHART_SHOW_LABELS     = "Show line labels";
static const char* const        _CHART_SHOW_HLINES     = "Show horizontal lines";
static const char* const        _CHART_SHOW_VLINES     = "Show vertical lines";
static const char* const        _CHART_RESET_SELECT    = "Reset line selection and visibility";
static const char* const        _CHART_SAVE_PNG        = "Save png to file...";

//------------------------------------------------------------------------------
static size_t _chart_bsearch(const PriceVector& prices, const Price& key) {
    auto it = std::lower_bound(prices.begin(), prices.end(), key);

    if (it == prices.end() || *it != key) {
        return (size_t) -1;
    }
    else {
        return std::distance(prices.begin(), it);
    }
}

//------------------------------------------------------------------------------
static int _chart_count_decimals(double number) {
    number = fabs(number);

    int    res     = 0;
    int    len     = 0;
    char*  end     = 0;
    double inumber = (int64_t) number;
    double fnumber = number - inumber;
    char   buffer[100];

    if (number > 999999999999999) {
        snprintf(buffer, 100, "%.1f", fnumber);
    }
    else if (number > 9999999999999) {
        snprintf(buffer, 100, "%.2f", fnumber);
    }
    else if (number > 999999999999) {
        snprintf(buffer, 100, "%.3f", fnumber);
    }
    else if (number > 99999999999) {
        snprintf(buffer, 100, "%.4f", fnumber);
    }
    else if (number > 9999999999) {
        snprintf(buffer, 100, "%.5f", fnumber);
    }
    else if (number > 999999999) {
        snprintf(buffer, 100, "%.6f", fnumber);
    }
    else if (number > 99999999) {
        snprintf(buffer, 100, "%.7f", fnumber);
    }
    else if (number > 9999999) {
        snprintf(buffer, 100, "%.8f", fnumber);
    }
    else {
        snprintf(buffer, 100, "%.9f", fnumber);
    }

    len = strlen(buffer);
    end = buffer + len - 1;

    while (*end == '0') {
        *end = 0;
        end--;
    }

    res = strlen(buffer) - 2;
    return res;
}

//------------------------------------------------------------------------------
static PriceVector _chart_create_date_serie(const char* start_date, const char* stop_date, Date::RANGE range, const PriceVector& block, bool long_format) {
    int         month   = -1;
    Date        current = Date::FromString(start_date);
    Date        stop    = Date::FromString(stop_date);
    PriceVector res;

    if (range == Date::RANGE::HOUR && current.year() < 1970) {
        return res;
    }

    if (range == Date::RANGE::FRIDAY) {
        while (current.weekday() != Date::DAY::FRIDAY)
            current.add_days(1);
    }
    else if (range == Date::RANGE::SUNDAY) {
        while (current.weekday() != Date::DAY::SUNDAY) {
            current.add_days(1);
        }
    }

    while (current <= stop) {
        Date date(1, 1, 1);

        if (range == Date::RANGE::DAY) {
            date = Date(current);
            current.add_days(1);
        }
        else if (range == Date::RANGE::WEEKDAY) {
            Date::DAY weekday = current.weekday();

            if (weekday >= Date::DAY::MONDAY && weekday <= Date::DAY::FRIDAY) {
                date = Date(current);
            }

            current.add_days(1);
        }
        else if (range == Date::RANGE::FRIDAY || range == Date::RANGE::SUNDAY) {
            date = Date(current);
            current.add_days(7);
        }
        else if (range == Date::RANGE::MONTH) {
            if (current.month() != month) {
                current.day(current.month_days());
                date = Date(current);
                month = current.month();
            }

            current.add_months(1);
        }
        else if (range == Date::RANGE::HOUR) {
            date = Date(current);
            current.add_seconds(3600);
        }
        else if (range == Date::RANGE::MIN) {
            date = Date(current);
            current.add_seconds(60);
        }
        else if (range == Date::RANGE::SEC) {
            date = Date(current);
            current.add_seconds(1);
        }

        if (date.year() > 1) {
            Price price;

            if (range == Date::RANGE::HOUR || range == Date::RANGE::MIN || range == Date::RANGE::SEC) {
                price.date = date.format((long_format == false) ? Date::FORMAT::ISO_TIME : Date::FORMAT::ISO_TIME_LONG);
            }
            else {
                price.date = date.format((long_format == false) ? Date::FORMAT::ISO : Date::FORMAT::ISO_LONG);
            }

            if (block.size() == 0 || std::binary_search(block.begin(), block.end(), price) == false) {
                res.push_back(price);
            }
        }
    }

    return res;
}

//------------------------------------------------------------------------------
static std::string _chart_format_date(const Price& price, Date::FORMAT format) {
    auto date = Date::FromString(price.date.c_str());
    return date.format(format);
}

//------------------------------------------------------------------------------
static std::string _chart_format_double(double num, int decimals, char del) {
    char res[100];

    *res = 0;

    if (decimals < 0) {
        decimals = _chart_count_decimals(num);
    }

    if (decimals == 0) {
        return util::format_int((int64_t) num, del);
    }

    if (fabs(num) < 9223372036854775807.0) {
        char fr_str[100];
        auto int_num    = (int64_t) fabs(num);
        auto double_num = (double) (fabs(num) - int_num);
        auto int_str    = util::format_int(int_num, del);
        auto len        = snprintf(fr_str, 99, "%.*f", decimals, double_num);

        if (len > 0 && len < 100) {
            if (num < 0.0) {
                res[0] = '-';
                res[1] = 0;
            }

            strncat(res, int_str.c_str(), 99);
            strncat(res, fr_str + 1, 99);
        }
    }

    return res;
}

//------------------------------------------------------------------------------
static bool _chart_has_high_low(Chart::TYPE chart_type) {
    return chart_type == Chart::BAR || chart_type == Chart::VERTICAL || chart_type == Chart::CLAMP_VERTICAL;
}

//------------------------------------------------------------------------------
static bool _chart_has_resizable_width(Chart::TYPE chart_type) {
    return chart_type == Chart::BAR || chart_type == Chart::VERTICAL || chart_type == Chart::CLAMP_VERTICAL;
}

//------------------------------------------------------------------------------
static bool _chart_has_time(Date::RANGE date_range) {
    return date_range == Date::RANGE::HOUR || date_range == Date::RANGE::MIN || date_range == Date::RANGE::SEC;
}

//----------------------------------------------------------------------
static Chart::TYPE _chart_string_to_type(std::string name) {
    if (name == "LINE") return Chart::LINE;
    else if (name == "BAR") return Chart::BAR;
    else if (name == "VERTICAL") return Chart::VERTICAL;
    else if (name == "CLAMP_VERTICAL") return Chart::CLAMP_VERTICAL;
    else if (name == "HORIZONTAL") return Chart::HORIZONTAL;
    else if (name == "EXPAND_VERTICAL") return Chart::EXPAND_VERTICAL;
    else if (name == "EXPAND_HORIZONTAL") return Chart::EXPAND_HORIZONTAL;
    else return Chart::LINE;
}

//----------------------------------------------------------------------
static std::string _chart_type_to_string(Chart::TYPE type) {
    assert(type >= 0 && type <= 6);
    static const char* NAMES[] = { "LINE", "BAR", "VERTICAL", "CLAMP_VERTICAL", "HORIZONTAL", "EXPAND_VERTICAL", "EXPAND_HORIZONTAL", "", "", };
    return NAMES[type];
}

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
// Data for one chart line
//
struct ChartLine {
    Fl_Align                    align;
    Chart::TYPE                 type;
    Fl_Color                    color;
    bool                        visible;
    int                         width;
    double                      clamp_max;
    double                      clamp_min;
    std::string                 label;
    double                      max;
    double                      min;
    PriceVector                 points;

                                ChartLine();
                                ~ChartLine()
                                    { clear(); }
    void                        clear();
    void                        debug(int num) const;
    bool                        set(const PriceVector& points, std::string label, Chart::TYPE type, Fl_Align align, Fl_Color color, int width, double clamp_min, double clamp_max);
};

//------------------------------------------------------------------------------
ChartLine::ChartLine() {
    clear();
}

//------------------------------------------------------------------------------
void ChartLine::clear() {
    points.clear();

    align     = FL_ALIGN_CENTER;
    type      = Chart::LINE;
    clamp_max = Chart::MAX_VAL;
    clamp_min = Chart::MIN_VAL;
    color     = FL_FOREGROUND_COLOR;
    label     = "";
    max       = Chart::MIN_VAL;
    min       = Chart::MAX_VAL;
    visible   = true;
    width     = 1;
}

//------------------------------------------------------------------------------
void ChartLine::debug(int num) const {
#ifdef DEBUG
    fprintf(stderr, "\t\t---------------------------------------------\n");
    fprintf(stderr, "\t\tChartLine: %d\n", num);
    fprintf(stderr, "\t\t\talign:      %25s\n", (align == FL_ALIGN_LEFT) ? "LEFT" : "RIGHT");
    fprintf(stderr, "\t\t\ttype:       %25d\n", (int) type);
    fprintf(stderr, "\t\t\tclamp_max:  %25.4f\n", clamp_max);
    fprintf(stderr, "\t\t\tclamp_min:  %25.4f\n", clamp_min);
    fprintf(stderr, "\t\t\tlabel: %30s\n", label.c_str());
    fprintf(stderr, "\t\t\tprices:     %25d\n", (int) points.size());
    fprintf(stderr, "\t\t\tvisible:    %25d\n", visible);
    fprintf(stderr, "\t\t\twidth:      %25d\n", width);

    if (points.size() > 1) {
        fprintf(stderr, "\t\t\tfirst:      %25s\n", points.front().date.c_str());
        fprintf(stderr, "\t\t\tfirst:      %25f\n", points.front().close);
        fprintf(stderr, "\t\t\tlast:       %25s\n", points.back().date.c_str());
        fprintf(stderr, "\t\t\tlast:       %25f\n", points.back().close);
    }
#else
    (void) num;
#endif
}

//------------------------------------------------------------------------------
bool ChartLine::set(const PriceVector& points, std::string label, Chart::TYPE type, Fl_Align align, Fl_Color color, int width, double clamp_min, double clamp_max) {
    if (width == 0 || width > _CHART_MAX_LINE_WIDTH || (int) type < 0 || (int) type > (int) Chart::LAST) {
        return false;
    }

    if (width < 0 && _chart_has_resizable_width(type) == false) {
        return false;
    }

    if (align != FL_ALIGN_LEFT && align != FL_ALIGN_RIGHT) {
        return false;
    }

    clear();

    this->points    = points;
    this->label     = label;
    this->type      = type;
    this->align     = align;
    this->color     = color;
    this->width     = width;
    this->clamp_min = clamp_min;
    this->clamp_max = clamp_max;

    for (const auto& price : this->points) {
        if (price.high > max) {
            max = price.high;
        }

        if (price.low < min) {
            min = price.low;
        }
    }

    return true;
}

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
// Each chart area can have an scale on the left and right side
//
struct ChartScale {
    double                      max;
    double                      min;
    double                      pixel;
    double                      tick;

                                ChartScale();
    void                        calc(int height);
    void                        clear();
    void                        debug(const char* name) const;
    double                      diff() const
                                    { return max - min; }
    void                        fix_height();
};

//------------------------------------------------------------------------------
ChartScale::ChartScale() {
    clear();
}

//------------------------------------------------------------------------------
void ChartScale::calc(int height) {
    auto range = diff();

    tick  = 0.0;
    pixel = 0.0;

    if (min < max && range > 0.000000001) {
        double test_inc = 0.0;
        double test_min = 0.0;
        double test_max = 0.0;
        int    ticker   = 0;

        test_inc = pow((double) 10, (double) ceil(log10(range / 10.0)));
        test_max = ((int) (max / test_inc)) * test_inc;

        if (test_max < max) {
            test_max += test_inc;
        }

        test_min = test_max;

        do {
            ++ticker;
            test_min -= test_inc;
        } while (test_min > min);

        if (ticker < 10) {
            test_inc = (ticker < 5) ? test_inc / 10.0 : test_inc / 2.0;

            while ((test_min + test_inc) <= min) {
                test_min += test_inc;
            }

            while ((test_max - test_inc) >= max) {
                test_max -= test_inc;
            }

        }

        min   = test_min;
        max   = test_max;
        tick  = test_inc;

        if (tick > 0.000000001) {
            pixel = height / diff();
        }
    }
}

//------------------------------------------------------------------------------
void ChartScale::clear() {
    min   = Chart::MAX_VAL;
    max   = Chart::MIN_VAL;
    tick  = 0.0;
    pixel = 0.0;
}

//------------------------------------------------------------------------------
void ChartScale::debug(const char* name) const {
#ifdef DEBUG
    fprintf(stderr, "\t\t---------------------------------------------\n");
    fprintf(stderr, "\t\tChartScale: %s\n", name);
    fprintf(stderr, "\t\t\tmin:   %30.6f\n", min);
    fprintf(stderr, "\t\t\tmax:   %30.6f\n", max);
    fprintf(stderr, "\t\t\tDiff:  %30.6f\n", diff());
    fprintf(stderr, "\t\t\ttick:  %30.6f\n", tick);
    fprintf(stderr, "\t\t\tpixel: %30.6f\n", pixel);
#else
    (void) name;
#endif
}

//------------------------------------------------------------------------------
void ChartScale::fix_height() {
    if (fabs(max - min) < 0.000001) {
        if (min >= 0.0) {
            min *= 0.9;
        }
        else {
            min *= 1.1;
        }

        if (max >= 0.0) {
            max *= 1.1;
        }
        else {
            max *= 0.9;
        }
    }
}

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
// Data for one chart area, currently max 3 areas in one widget can be shown at the same time
// All are using the same date range
//
struct ChartArea {
    size_t                      count;
    double                      h;
    ChartScale                  left;
    LineVector                  lines;
    int                         percent;
    ChartScale                  right;
    size_t                      selected;
    double                      w;
    double                      x;
    double                      y;

                                ChartArea();
    void                        clear(bool clear_data);
    void                        debug(int num) const;
    int                         x2() const
                                    { return (int) (x + w); }
    int                         y2() const
                                    { return (int) (y + h); }
};

//------------------------------------------------------------------------------
ChartArea::ChartArea() {
    for (size_t f = 0; f < Chart::MAX_LINE; f++) {
        lines.push_back(ChartLine());
    }

    clear(false);
}

//------------------------------------------------------------------------------
void ChartArea::clear(bool clear_data) {
    count    = 0;
    x        = 0.0;
    y        = 0.0;
    w        = 0.0;
    h        = 0.0;
    selected = 0;

    left.clear();
    right.clear();

    if (clear_data == true) {
        percent = 0;

        for (size_t f = 0; f < Chart::MAX_LINE; f++) {
            lines[f] = ChartLine();
        }
    }
}

//------------------------------------------------------------------------------
void ChartArea::debug(int num) const {
#ifdef DEBUG
    auto count = 0;

    for (const auto& line : lines) {
        if (line.points.size() > 0) {
            count++;
        }
    }

    if (count > 0 || num == 0) {
        fprintf(stderr, "\t----------------------\n");
        fprintf(stderr, "\tChartArea: %d\n", num);
        fprintf(stderr, "\t\tlines:    %4d\n", count);
        fprintf(stderr, "\t\tx:        %4.0f\n", x);
        fprintf(stderr, "\t\ty:        %4.0f\n", y);
        fprintf(stderr, "\t\tw:        %4.0f\n", w);
        fprintf(stderr, "\t\th:        %4.0f\n", h);
        fprintf(stderr, "\t\tpercent:  %4d\n", percent);
        fprintf(stderr, "\t\tselected: %4d\n", (int) selected);

        left.debug("left");
        right.debug("right");
        count = 0;

        for (const auto& line : lines) {
            if (line.points.size() > 0) {
                line.debug(count++);
            }
        }
    }
#else
    (void) num;
#endif
}

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
Price::Price() {
    high  = 0.0;
    low   = 0.0;
    close = 0.0;
    vol   = 0.0;
}

//------------------------------------------------------------------------------
Price::Price(const std::string& date, double value) {
    this->date = date;
    high       = value;
    low        = value;
    close      = value;
    vol        = value;
}

//------------------------------------------------------------------------------
Price::Price(const std::string& date, double high, double low, double close, double vol) {
    this->date  = date;
    this->high  = high;
    this->low   = low;
    this->close = close;
    this->vol   = vol;

#ifdef DEBUG
    if (close > high || close < low || high < low) {
        fprintf(stderr, "error: values out of order in Price(%s, %15.5f  >=  %15.5f  <=  %15.5f)\n", date.c_str(), high, low, close);
    }
#endif
}

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
Chart::Chart(int X, int Y, int W, int H, const char* l) : Fl_Group(X, Y, W, H, l) {
    end();
    clip_children(1);
    color(FL_BACKGROUND2_COLOR);
    labelcolor(FL_FOREGROUND_COLOR);
    box(FL_BORDER_BOX);
    tooltip(
        "Press right button for menu.\n"
        "Press ctrl + scroll wheel to change tick size.\n"
        "Press left button to show Y value.\n"
        "Press ctrl + left button to show actual price data.\n"
        "Press 1 - 9 to select which line to use.\n"
        "Press shift + 1 - 9 to toggle line visibility.\n"
        "Click in target area first."
    );

    _menu   = new Fl_Menu_Button(0, 0, 0, 0);
    _scroll = new Fl_Scrollbar(0, 0, 0, 0);

    add(_menu);
    add(_scroll);

    _scroll->type(FL_HORIZONTAL);
    _scroll->callback(Chart::_CallbackScrollbar, this);
    _menu->add(_CHART_SHOW_LABELS, 0, Chart::_CallbackToggle, this, FL_MENU_TOGGLE);
    _menu->add(_CHART_SHOW_HLINES, 0, Chart::_CallbackToggle, this, FL_MENU_TOGGLE);
    _menu->add(_CHART_SHOW_VLINES, 0, Chart::_CallbackToggle, this, FL_MENU_TOGGLE | FL_MENU_DIVIDER);
    _menu->add(_CHART_RESET_SELECT, 0, Chart::_CallbackReset, this);
    _menu->add(_CHART_SAVE_PNG, 0, Chart::_CallbackSavePng, this);
#ifdef DEBUG
    _menu->add("Debug", 0, Chart::_CallbackDebug, this);
#endif
    _menu->type(Fl_Menu_Button::POPUP3);

    _areas.push_back(ChartArea());
    _areas.push_back(ChartArea());
    _areas.push_back(ChartArea());

    clear();
    update_pref();
    view_options();
}

//------------------------------------------------------------------------------
bool Chart::add_line(size_t area, const PriceVector& points, std::string line_label, Chart::TYPE chart_type, Fl_Align line_align, Fl_Color line_color, int line_width, double clamp_min, double clamp_max) {
    _area = nullptr;
    redraw();

    if (area >= Chart::MAX_AREA || _areas[area].count >= Chart::MAX_LINE) {
        return false;
    }

    if (chart_type < Chart::LINE || chart_type > Chart::LAST) {
        return false;
    }

    if (_areas[area].lines[_areas[area].count].set(points, line_label, chart_type, line_align, line_color, line_width, clamp_min, clamp_max) == false) {
        _areas[area].lines[_areas[area].count] = ChartLine();
        return false;
    }

    _areas[area].count++;
    return true;
}

//------------------------------------------------------------------------------
bool Chart::area_size(int area1, int area2, int area3) {
    _area = nullptr;

    if (area1 < 0 || area1 > 100 || area2 < 0 || area2 > 100 || area3 < 0 || area3 > 100 || area1 + area2 + area3 != 100) {
        return false;
    }

    _areas[0].percent = area1;
    _areas[1].percent = area2;
    _areas[2].percent = area3;
    return true;
}

//------------------------------------------------------------------------------
void Chart::_calc_area_height() {
    int last   = 0;
    int addh   = 0;
    int height = 0;

    _top_space    = flw::PREF_FIXED_FONTSIZE;
    _bottom_space = flw::PREF_FIXED_FONTSIZE * 3 + Fl::scrollbar_size();
    height        = h() - (_bottom_space + _top_space);

    for (size_t f = 1; f < Chart::MAX_AREA; f++) {
        const auto& area = _areas[f];

        if (area.percent >= 10) {
            height -= PREF_FIXED_FONTSIZE;
        }
    }

    _areas[0].y = _top_space;
    _areas[0].h = (int) ((_areas[0].percent / 100.0) * height);

    for (size_t f = 1; f < Chart::MAX_AREA; f++) {
        const auto& prev = _areas[f - 1];
        auto&       area = _areas[f];

        if (area.percent > 10) {
            area.y  = prev.y + prev.h + flw::PREF_FIXED_FONTSIZE;
            area.h  = (int) ((_areas[1].percent / 100.0) * height);
            addh   += (int) (prev.h);
            last    = f;
        }
    }

    if (last > 0) {
        _areas[last].h = (int) (height - addh);
    }
}

//------------------------------------------------------------------------------
void Chart::_calc_area_width() {
    const double width = w() - (_margin_left * flw::PREF_FIXED_FONTSIZE + _margin_right * flw::PREF_FIXED_FONTSIZE);

    _ticks     = (int) (width / _tick_width);
    _top_space = flw::PREF_FIXED_FONTSIZE;

    if ((int) _dates.size() > _ticks) {
        const double slider_size = _ticks / (_dates.size() + 1.0);

        _scroll->activate();
        _scroll->slider_size((slider_size > 0.05) ? slider_size : 0.05);
        _scroll->range(0, _dates.size() - _ticks);

        if (_scroll->value() > _scroll->maximum()) {
            ((Fl_Slider*) _scroll)->value(_scroll->maximum());
            _date_start = _dates.size() - _ticks - 1;
        }
    }
    else {
        ((Fl_Valuator*) _scroll)->value(0);
        _scroll->range(0, 0);
        _scroll->slider_size(1.0);
        _scroll->deactivate();
        _date_start = 0;
    }

    for (auto& area : _areas) {
        area.x = _margin_left * flw::PREF_FIXED_FONTSIZE;
        area.w = width;
    }
}

//------------------------------------------------------------------------------
void Chart::_calc_dates() {
    std::string min       = "";
    std::string max       = "";
    bool        long_date = false;

    for (const auto& area : _areas) {
        for (const auto& line : area.lines) {
            if (line.points.size() > 0) {
                auto& first = line.points.front();
                auto& last  = line.points.back();

                if (min.length() == 0) {
                    if (first.date.length() == 15) {
                        min = "99991231 232359";
                        max = "01010101 000000";
                    }
                    else if (first.date.length() == 19) {
                        min = "9999-12-31 23:23:59";
                        max = "0101-01-01 00:00:00";
                        long_date = true;
                    }
                    else if (first.date.length() == 10) {
                        min = "9999-12-31";
                        max = "0101-01-01";
                        long_date = true;
                    }
                    else {
                        min = "99991231";
                        max = "01010101";
                    }
                }

                if (first.date < min) {
                    min = first.date;
                }

                if (last.date > max) {
                    max = last.date;
                }
            }
        }
    }

    _dates.clear();

    if (min != "") {
        _dates = _chart_create_date_serie(min.c_str(), max.c_str(), _date_range, _block_dates, long_date);
        redraw();
    }
}

//------------------------------------------------------------------------------
void Chart::_calc_ymin_ymax() {
    for (auto& area : _areas) {
        area.left.clear();
        area.right.clear();

        for (const auto& line : area.lines) {
            if (line.points.size() > 0) {
                const int stop    = _date_start + _ticks;
                int       current = _date_start;
                double    max     = Chart::MIN_VAL;
                double    min     = Chart::MAX_VAL;

                while (current <= stop && current < (int) _dates.size()) {
                    const Price& date  = _dates[current];
                    const size_t index = _chart_bsearch(line.points, date);

                    if (index != (size_t) -1) {
                        const Price& price = line.points[index];

                        if (_chart_has_high_low(line.type) == true) {
                            min = price.low;
                            max = price.high;
                        }
                        else {
                            min = price.close;
                            max = price.close;
                        }

                        if ((int64_t) line.clamp_min > Chart::MIN_VAL) {
                            min = line.clamp_min;
                        }

                        if ((int64_t) line.clamp_max < Chart::MAX_VAL) {
                            max = line.clamp_max;
                        }

                        if (line.align == FL_ALIGN_LEFT) {
                            if (min < area.left.min) {
                                area.left.min = min;
                            }

                            if (max > area.left.max) {
                                area.left.max = max;
                            }
                        }
                        else {
                            if (min < area.right.min) {
                                area.right.min = min;
                            }

                            if (max > area.right.max) {
                                area.right.max = max;
                            }
                        }
                    }

                    current++;
                }
            }
        }

        area.left.fix_height();
        area.right.fix_height();
    }
}

//------------------------------------------------------------------------------
void Chart::_calc_yscale() {
    for (auto& area : _areas) {
        area.left.calc(area.h);
        area.right.calc(area.h);
    }
}

//------------------------------------------------------------------------------
void Chart::clear() {
    ((Fl_Valuator*) _scroll)->value(0);

    _block_dates.clear();
    _dates.clear();

    _areas[0].clear(true);
    _areas[1].clear(true);
    _areas[2].clear(true);

    _area         = nullptr;
    _bottom_space = 0;
    _cw           = 0;
    _date_start   = 0;
    _old_height   = -1;
    _old_width    = -1;
    _tooltip      = "";
    _top_space    = 0;
    _ver_pos[0]   = -1;

    area_size();
    margin();
    date_range();
    tick_width();
    init(false);
}

//------------------------------------------------------------------------------
// Create tooltip string either by using mouse cursor pos and convert it to real value or use price data
// If ctrl is true then use actual price data
//
void Chart::_create_tooltip(bool ctrl) {
    const int X   = Fl::event_x();
    const int Y   = Fl::event_y();
    auto      old = _tooltip;

    _tooltip = "";

    if (_area == nullptr) { // Set in handle()
        Fl::redraw();
        return;
    }

    const auto date_format = (_date_format == flw::Date::FORMAT::ISO) ? flw::Date::FORMAT::NAME_LONG : flw::Date::FORMAT::ISO_TIME_LONG;
    const int  stop        = _date_start + _ticks;
    int        start       = _date_start;
    int        X1          = x() + _margin_left * flw::PREF_FIXED_FONTSIZE;
    int        left_dec    = 0;
    int        right_dec   = 0;

    if (_area->left.tick < 10.0 ) {
        left_dec = _chart_count_decimals(_area->left.tick) + 1;
    }

    if (_area->right.tick < 10.0 ) {
        right_dec = _chart_count_decimals(_area->right.tick) + 1;
    }

    while (start <= stop && start < (int) _dates.size()) {
        if (X >= X1 && X <= X1 + _tick_width - 1) { // Is mouse x pos inside current tick?
            const std::string fancy_date = Date::FromString(_dates[start].date.c_str()).format(date_format);

            _tooltip = fancy_date;

            if (ctrl == false || _area->lines[_area->selected].points.size() == 0) { // Convert mouse pos to scale value
                const double ydiff = (double) ((y() + _area->y2()) - Y);
                std::string  left;
                std::string  right;

                if (_area->left.max > _area->left.min) {
                    left = _chart_format_double(_area->left.min + (ydiff / _area->left.pixel), left_dec, '\'');
                }

                if (_area->right.max > _area->right.min) {
                    right = _chart_format_double(_area->right.min + (ydiff / _area->right.pixel), right_dec, '\'');
                }

                const size_t len = (left.length() > right.length()) ? left.length() : right.length();

                if (left != "" && right != "") {
                    _tooltip = util::format("%s\nleft:  %*s\nright: %*s", fancy_date.c_str(), (int) len, left.c_str(), (int) len, right.c_str());
                }
                else if (left != "") {
                    _tooltip = util::format("%s\nleft: %*s", fancy_date.c_str(), (int) len, left.c_str());
                }
                else if (right != "") {
                    _tooltip = util::format("%s\nright: %*s", fancy_date.c_str(), (int) len, right.c_str());
                }
            }
            else { // Use actual price data
                const auto&  line  = _area->lines[_area->selected];
                const size_t index = _chart_bsearch(line.points, _dates[start].date);

                if (index != (size_t) -1) {
                    const auto dec   = (line.align == FL_ALIGN_RIGHT) ? right_dec : left_dec;
                    const auto price = line.points[index];
                    auto       high  = _chart_format_double(price.high, dec, '\'');
                    auto       low   = _chart_format_double(price.low, dec, '\'');
                    auto       close = _chart_format_double(price.close, dec, '\'');
                    const auto len   = (low.length() > high.length()) ? low.length() : high.length();

                    _tooltip = util::format("%s\nhigh:  %*s\nclose: %*s\nlow:   %*s", fancy_date.c_str(), (int) len, high.c_str(), (int) len, close.c_str(), (int) len, low.c_str());
                }
            }

            break;
        }

        X1    += _tick_width;
        start += 1;
    }

    if (_tooltip != "" || old != "") {
        redraw();
    }
}

//------------------------------------------------------------------------------
bool Chart::date_range(Date::RANGE range) {
    if ((int) range < 0 || (int) range > (int) Date::RANGE::LAST) {
        return false;
    }

    _date_range  = range;
    _date_format = (_date_range == Date::RANGE::HOUR || _date_range == Date::RANGE::MIN || _date_range == Date::RANGE::SEC) ? Date::FORMAT::ISO_TIME : Date::FORMAT::ISO;

    return true;
}

//------------------------------------------------------------------------------
void Chart::debug() const {
#ifdef DEBUG
    Price first, last, start, end;

    if (_dates.size()) {
        first = _dates.front();
    }

    if (_dates.size() > 1) {
        last = _dates.back();
    }

    if ((int) _dates.size() > _date_start) {
        start = _dates[_date_start];
    }

    if (_ticks >= 0 && (int) _dates.size() > (_date_start + _ticks)) {
        end = _dates[_date_start + _ticks];
    }

    fprintf(stderr, "\n");
    fprintf(stderr, "--------------------------------------------\n");
    fprintf(stderr, "Chart:\n");
    fprintf(stderr, "\tblock_dates:     %19d\n", (int) _block_dates.size());
    fprintf(stderr, "\tbottom_space:    %19d\n", _bottom_space);
    fprintf(stderr, "\tcw:              %19d\n", _cw);
    fprintf(stderr, "\tdate_end:        %19d\n", _date_start + _ticks);
    fprintf(stderr, "\tdate_format:     %19d\n", (int) _date_format);
    fprintf(stderr, "\tdate_range:      %19d\n", (int) _date_range);
    fprintf(stderr, "\tdate_start:      %19d\n", _date_start);
    fprintf(stderr, "\tdates:           %19d\n", (int) _dates.size());
    fprintf(stderr, "\th:               %19d\n", h());
    fprintf(stderr, "\tmargin_left:     %19d\n", _margin_left * flw::PREF_FIXED_FONTSIZE);
    fprintf(stderr, "\tmargin_right:    %19d\n", _margin_right * flw::PREF_FIXED_FONTSIZE);
    fprintf(stderr, "\ttick_width:      %19d\n", _tick_width);
    fprintf(stderr, "\tticks:           %19d\n", _ticks);
    fprintf(stderr, "\ttop_space:       %19d\n", _top_space);
    fprintf(stderr, "\tw:               %19d\n", w());
    fprintf(stderr, "\tx:               %19d\n", x());
    fprintf(stderr, "\ty:               %19d\n", y());
    fprintf(stderr, "\tview_labels:     %19d\n", _view.labels);
    fprintf(stderr, "\tview_horizontal: %19d\n", _view.horizontal);
    fprintf(stderr, "\tview_vertical:   %19d\n", _view.vertical);
    fprintf(stderr, "\tfirst date:      %19s\n", first.date.c_str());
    fprintf(stderr, "\tlast date:       %19s\n", last.date.c_str());
    fprintf(stderr, "\tfirst visible:   %19s\n", start.date.c_str());
    fprintf(stderr, "\tlast visible:    %19s\n", end.date.c_str());

    _areas[0].debug(0);
    _areas[1].debug(1);
    _areas[2].debug(2);

    fflush(stderr);
#endif
}

//------------------------------------------------------------------------------
void Chart::draw() {
#ifdef DEBUG
    // auto t = flw::util::time_milli();
#endif

    fl_font(flw::PREF_FIXED_FONT, flw::PREF_FIXED_FONTSIZE);
    Fl_Group::draw();
    _cw = fl_width("X");

    if (w() < 100 || h() < 100) {
        return;
    }

    fl_push_clip(x(), y(), w(), h() - _scroll->h());
    _draw_xlabels();

    for (const auto& area : _areas) {
        if (area.percent >= 10) {
            _draw_area(area);
        }
    }

    _draw_tooltip();
    fl_pop_clip();
    fl_line_style(0);

#ifdef DEBUG
    // fprintf(stderr, "draw: %3d mS\n", flw::util::time_milli() - t);
    // fflush(stdout);
#endif
}

//------------------------------------------------------------------------------
void Chart::_draw_area(const ChartArea& area) {
    _draw_ver_lines(area);
    _draw_ylabels(_margin_left * flw::PREF_FIXED_FONTSIZE, area.y + area.h, area.y, area.left, true);
    _draw_ylabels(w() - _margin_right * flw::PREF_FIXED_FONTSIZE, area.y + area.h, area.y, area.right, false);
    fl_push_matrix();
    fl_push_clip(x() + area.x, y() + area.y - 1, area.w + 1, area.h + 2);

    for (const auto& line : area.lines) {
        _draw_line(line, (line.align == FL_ALIGN_LEFT) ? area.left : area.right, area.x, area.y, area.w, area.h);
    }

    _draw_line_labels(area);
    fl_rect(x() + area.x, y() + area.y - 1, area.w + 1, area.h + 2, labelcolor());
    fl_pop_clip();
    fl_pop_matrix();
}

//------------------------------------------------------------------------------
void Chart::_draw_line(const ChartLine& line, const ChartScale& scale, int X, const int Y, const int W, const int H) {
    if (line.points.size() > 0 && line.visible == true) {
        const int y2      = y() + Y + H;
        int       lastX   = -1;
        int       lastY   = -1;
        int       current = _date_start;
        const int stop    = _date_start + _ticks + 1;
        int       width   = line.width;

        if (line.width < 0) {
            width  = _tick_width - 1;
            width -= (int) (_tick_width / 10);
        }

        const int lw2 = width / 2;
        const int lw4 = lw2 / 2;

        fl_push_clip(x() + X, y() + Y, W + 1, H + 1);
        fl_color(line.color);

        if (line.type == Chart::BAR) {
            fl_line_style(FL_SOLID, lw2);
        }
        else {
            fl_line_style(FL_SOLID, width);
        }

        while (current <= stop && current < (int) _dates.size()) {
            const auto& date  = _dates[current];
            const auto  index = _chart_bsearch(line.points, date);

            if (index != (size_t) -1) {
                const auto& pr = line.points[index];
                const auto  yh = (int) ((pr.high - scale.min) * scale.pixel);
                const auto  yl = (int) ((pr.low - scale.min) * scale.pixel);
                const auto  yc = (int) ((pr.close - scale.min) * scale.pixel);

                if (line.type == Chart::LINE) {
                    if (lastX > -1 && lastY > -1) {
                        fl_line(lastX + lw2, y2 - lastY, x() + X + lw2, y2 - yc);
                    }
                }
                else if (line.type == Chart::BAR) {
                    fl_line(x() + X + lw4, y2 - yl, x() + X + lw4, y2 - yh);
                    fl_line(x() + X, y2 - yc, x() + X + _tick_width - 1, y2 - yc);
                }
                else if (line.type == Chart::HORIZONTAL) {
                    fl_line(x() + X, y2 - yc, x() + X + _tick_width, y2 - yc);
                }
                else if (line.type == Chart::VERTICAL) {
                    auto h = yh - yl;
                    fl_rectf(x() + X, y2 - yh, width, (h < 1) ? 1 : h);
                }
                else if (line.type == Chart::CLAMP_VERTICAL) {
                    fl_rectf(x() + X, y2 - yh, width, yh);
                }
                else if (line.type == Chart::EXPAND_VERTICAL) {
                    fl_line(x() + X, y2, x() + X, y() + Y);
                }
                else if (line.type == Chart::EXPAND_HORIZONTAL) {
                    fl_line(x() + _margin_left * flw::PREF_FIXED_FONTSIZE, y2 - yc, x() + Fl_Widget::w() - _margin_right * flw::PREF_FIXED_FONTSIZE, y2 - yc);
                }

                lastX = x() + X;
                lastY = yc;
            }

            X += _tick_width;
            current++;
        }

        fl_pop_clip();
        fl_line_style(0);
    }
}

//------------------------------------------------------------------------------
void Chart::_draw_line_labels(const ChartArea& area) {
    if (_view.labels == true) {
        int       left_h  = 0;
        int       left_w  = 0;
        const int left_x  = x() + area.x + 6;
        int       left_y  = y() + area.y + 6;
        int       right_h = 0;
        int       right_w = 0;
        const int right_x = x() + area.x + area.w;
        int       right_y = left_y;

        for (const auto& line : area.lines) {
            if (line.label == "") {
                continue;
            }

            if (line.align == FL_ALIGN_RIGHT) {
                right_h++;

                if ((int) line.label.length() * _cw > right_w) {
                    right_w = line.label.length() * _cw;
                }
            }
            else {
                left_h++;

                if ((int) line.label.length() * _cw > left_w) {
                    left_w = line.label.length() * _cw;
                }
            }
        }

        left_h  *= flw::PREF_FIXED_FONTSIZE;
        right_h *= flw::PREF_FIXED_FONTSIZE;

        if (left_w > 0) {
            left_w += _cw * 2 + 4;
            fl_color(FL_BACKGROUND2_COLOR);
            fl_rectf(left_x, left_y, left_w + 8, left_h + 8);

            fl_color(FL_FOREGROUND_COLOR);
            fl_rect(left_x, left_y, left_w + 8, left_h + 8);
        }

        if (right_w > 0) {
            right_w += _cw * 2 + 4;
            fl_color(FL_BACKGROUND2_COLOR);
            fl_rectf(right_x - right_w - 14, left_y, right_w + 6, right_h + 8);

            fl_color(FL_FOREGROUND_COLOR);
            fl_rect(right_x - right_w - 14, left_y, right_w + 6, right_h + 8);
        }

        fl_font(flw::PREF_FIXED_FONT, flw::PREF_FIXED_FONTSIZE);

        if (left_w > 0 || right_w > 0) {
            size_t count   = 0;

            for (const auto& line : area.lines) {
                auto label = line.label;

                if (label != "") {
                    if (area.selected == count) {
                        label = "@-> " + label;
                    }

                    fl_color((line.visible == false) ? FL_GRAY : line.color);

                    if (line.align == FL_ALIGN_RIGHT) {
                        fl_draw(label.c_str(), right_x - right_w - 10, right_y + 4, right_w + 8, flw::PREF_FIXED_FONTSIZE, FL_ALIGN_LEFT);
                        right_y += flw::PREF_FIXED_FONTSIZE;
                    }
                    else {
                        fl_draw(label.c_str(), left_x + 4, left_y + 4, left_w + 8, flw::PREF_FIXED_FONTSIZE, FL_ALIGN_LEFT);
                        left_y += flw::PREF_FIXED_FONTSIZE;
                    }
                }

                count++;
            }
        }
    }
}

//------------------------------------------------------------------------------
void Chart::_draw_tooltip() {
    if (_tooltip != "" && _area != nullptr) {
        auto       X = Fl::event_x();
        auto       Y = Fl::event_y();
        const auto W = 14;
        const auto H = 5;

        if (X > _area->x2() - flw::PREF_FIXED_FONTSIZE * (W + 5)) {
            X -= flw::PREF_FIXED_FONTSIZE * (W + 4);
        }
        else {
            X += flw::PREF_FIXED_FONTSIZE * 2;
        }

        if (Y > y() + h() - flw::PREF_FIXED_FONTSIZE * (H + 7)) {
            Y -= flw::PREF_FIXED_FONTSIZE * (H + 2);
        }
        else {
            Y += flw::PREF_FIXED_FONTSIZE * 2;
        }

        fl_color(FL_BACKGROUND2_COLOR);
        fl_rectf(X, Y, flw::PREF_FIXED_FONTSIZE * (W + 2), flw::PREF_FIXED_FONTSIZE * H);

        fl_color(FL_FOREGROUND_COLOR);
        fl_rect(X, Y, flw::PREF_FIXED_FONTSIZE * (W + 2), flw::PREF_FIXED_FONTSIZE * H);

        fl_line(Fl::event_x(), y() + _area->y, Fl::event_x(), y() + _area->y + _area->h);
        fl_line(x() + _area->x, Fl::event_y(), x() + _area->x + _area->w, Fl::event_y());

        fl_font(flw::PREF_FIXED_FONT, flw::PREF_FIXED_FONTSIZE);
        fl_draw(_tooltip.c_str(), X + flw::PREF_FIXED_FONTSIZE, Y, flw::PREF_FIXED_FONTSIZE * W, flw::PREF_FIXED_FONTSIZE * H, FL_ALIGN_LEFT | FL_ALIGN_CENTER | FL_ALIGN_INSIDE);
    }
}

//------------------------------------------------------------------------------
void Chart::_draw_ver_lines(const ChartArea& area) {
    if (_view.vertical == true) {
        fl_color(fl_color_average(FL_FOREGROUND_COLOR, FL_BACKGROUND2_COLOR, 0.2));

        for (auto i = 0; i < _CHART_MAX_VLINES; i++) {
            if (_ver_pos[i] < 0) {
                break;
            }
            else {
                fl_line(_ver_pos[i], y() + (int) area.y, _ver_pos[i], y() + (int) (area.y + area.h - 1));
            }
        }
    }
}

//------------------------------------------------------------------------------
void Chart::_draw_xlabels() {
    const int  stop  = _date_start + _ticks;
    const int  cw2   = _cw * 2;
    const int  cw4   = _cw * 4;
    const int  fs05  = flw::PREF_FIXED_FONTSIZE * 0.5;
    const int  Y     = y() + (h() - _bottom_space);
    int        start = _date_start;
    int        index = 0;
    int        last  = -1;
    int        X1    = x() + _margin_left * flw::PREF_FIXED_FONTSIZE;
    int        X2    = 0;
    char       buffer1[100];
    char       buffer2[100];

    _ver_pos[0] = -1;

    while (start <= stop && start < (int) _dates.size()) {
        const auto& price = _dates[start];
        const auto  date  = Date::FromString(price.date.c_str());
        auto        addv  = false;

        fl_color(labelcolor());
        fl_line(X1, Y, X1, Y + fs05);

        *buffer1 = 0;
        *buffer2 = 0;

        if (_date_range == Date::RANGE::HOUR) {
            if (date.day() != last) {
                addv = true;
                last = date.day();

                if (X1 >= X2) {
                    snprintf(buffer1, 100, "%04d-%02d-%02d", date.year(), date.month(), date.day());
                }
                else {
                    fl_line(X1, Y, X1, Y + flw::PREF_FIXED_FONTSIZE);
                }
            }
            else {
                snprintf(buffer2, 100, "%02d", date.hour());
            }
        }
        else if (_date_range == flw::Date::RANGE::MIN) {
            if (date.hour() != last) {
                addv = true;
                last = date.hour();

                if (X1 >= X2) {
                    snprintf(buffer1, 100, "%04d-%02d-%02d/%02d", date.year(), date.month(), date.day(), date.hour());
                }
                else {
                    fl_line(X1, Y, X1, Y + flw::PREF_FIXED_FONTSIZE);
                }
            }
            else {
                snprintf(buffer2, 100, "%02d", date.minute());
            }
        }
        else if (_date_range == Date::RANGE::SEC) {
            if (date.minute() != last) {
                addv = true;
                last = date.minute();

                if (X1 >= X2) {
                    snprintf(buffer1, 100, "%04d-%02d-%02d/%02d:%02d", date.year(), date.month(), date.day(), date.hour(), date.minute());
                }
                else {
                    fl_line(X1, Y, X1, Y + flw::PREF_FIXED_FONTSIZE);
                }
            }
            else {
                snprintf(buffer2, 100, "%02d", date.second());
            }
        }
        else if (_date_range == Date::RANGE::DAY || _date_range == Date::RANGE::WEEKDAY) {
            if (date.month() != last) {
                addv = true;
                last = date.month();

                if (X1 >= X2) {
                    snprintf(buffer1, 100, "%04d-%02d", date.year(), date.month());
                }
                else {
                    fl_line(X1, Y, X1, Y + flw::PREF_FIXED_FONTSIZE);
                }
            }
            else {
                snprintf(buffer2, 100, "%02d", date.day());
            }
        }
        else if (_date_range == Date::RANGE::FRIDAY || _date_range == Date::RANGE::SUNDAY) {
            if (date.month() != last) {
                addv = true;
                last = date.month();

                if (X1 >= X2) {
                    snprintf(buffer1, 100, "%04d-%02d/%02d", date.year(), date.month(), date.week());
                }
                else {
                    fl_line(X1, Y, X1, Y + flw::PREF_FIXED_FONTSIZE);
                }
            }
            else
                snprintf(buffer2, 100, "%02d", date.week());
        }
        else if (_date_range == Date::RANGE::MONTH) {
            if (date.month() != last) {
                addv = true;
                last = date.month();

                if (X1 >= X2) {
                    snprintf(buffer1, 100, "%04d-%02d", date.year(), date.month());
                }
                else {
                    snprintf(buffer2, 100, "%02d", date.month());
                }
            }
        }

        if (*buffer1 != 0) {
            auto len  = (double) strlen(buffer1);
            auto half = (double) ((len / 2.0) * _cw);

            fl_font(flw::PREF_FIXED_FONT, flw::PREF_FIXED_FONTSIZE);
            fl_draw(buffer1, X1 - half, Y + flw::PREF_FIXED_FONTSIZE + 6, half + half, flw::PREF_FIXED_FONTSIZE, FL_ALIGN_CENTER | FL_ALIGN_INSIDE);
            fl_line(X1, Y, X1, Y + flw::PREF_FIXED_FONTSIZE);
            X2 = X1 + _cw + half + half;
        }

        if (*buffer2 != 0 && cw2 <= _tick_width) { // Draw second x label
            fl_font(flw::PREF_FIXED_FONT, flw::PREF_FIXED_FONTSIZE - 2);
            fl_draw(buffer2, X1 - cw2, Y + fs05, cw4, flw::PREF_FIXED_FONTSIZE, FL_ALIGN_CENTER | FL_ALIGN_INSIDE);
        }

        if (addv == true && index < _CHART_MAX_VLINES) { // Save x pos for vertical lines
            _ver_pos[index++] = X1;
        }

        X1    += _tick_width;
        start += 1;
    }

    _ver_pos[index] = -1;
}

//------------------------------------------------------------------------------
void Chart::_draw_ylabels(const int X, double Y1, const double Y2, const ChartScale& scale, const bool left) {
    const double yinc  = (scale.pixel * scale.tick);
    const int    fs05  = flw::PREF_FIXED_FONTSIZE * 0.5;
    const int    fr    = _chart_count_decimals(scale.tick);
    int          width = w() - (_margin_left * flw::PREF_FIXED_FONTSIZE + _margin_right * flw::PREF_FIXED_FONTSIZE);
    double       ylast = Chart::MAX_VAL;
    double       yval  = scale.min;

    fl_font(flw::PREF_FIXED_FONT, flw::PREF_FIXED_FONTSIZE);

    if (scale.min >= scale.max || scale.pixel * scale.tick < 1.0) {
        return;
    }

    while ((int) (Y1 + 0.5) >= (int) Y2) {
        if (ylast > Y1) {
            auto y1     = y() + (int) Y1;
            auto x1     = x() + X;
            auto string = _chart_format_double(yval, fr, '\'');

            if (left == true) {
                fl_color(labelcolor());
                fl_line(x1 - fs05, y1, x1, y1);
                fl_draw(string.c_str(), x(), y1 - fs05, _margin_left * flw::PREF_FIXED_FONTSIZE - flw::PREF_FIXED_FONTSIZE, flw::PREF_FIXED_FONTSIZE, FL_ALIGN_RIGHT | FL_ALIGN_INSIDE);

                if (_view.horizontal == true && (int) Y1 >= (int) (Y2 + fs05)) {
                    fl_color(fl_color_average(FL_FOREGROUND_COLOR, FL_BACKGROUND2_COLOR, 0.2));
                    fl_line(x1 + 1, y1, x1 + width - 1, y1);
                }
            }
            else {
                fl_color(labelcolor());
                fl_line(x1, y1, x1 + fs05, y1);
                fl_draw(string.c_str(), x1 + flw::PREF_FIXED_FONTSIZE, y1 - fs05, _margin_right * flw::PREF_FIXED_FONTSIZE - (flw::PREF_FIXED_FONTSIZE * 2), flw::PREF_FIXED_FONTSIZE, FL_ALIGN_RIGHT | FL_ALIGN_INSIDE);

                if (_view.horizontal == true && (int) Y1 >= (int) (Y2 + fs05)) {
                    fl_color(fl_color_average(FL_FOREGROUND_COLOR, FL_BACKGROUND2_COLOR, 0.2));
                    fl_line(x1 - width + 1, y1, x1 - 1, y1);
                }
            }

            ylast = Y1 - (flw::PREF_FIXED_FONTSIZE + fs05);
        }

        Y1   -= yinc;
        yval += scale.tick;
    }
}

//------------------------------------------------------------------------------
int Chart::handle(int event) {
    if (event == FL_PUSH) {
        if (Fl::event_button1() != 0) {
            _area = _inside_area(Fl::event_x(), Fl::event_y());
            _create_tooltip(Fl::event_ctrl());

            if (_tooltip != "") {
                return 1;
            }
        }
        else if (Fl::event_button3() != 0) {
            menu::set_item(_menu, _CHART_SHOW_LABELS, _view.labels);
            menu::set_item(_menu, _CHART_SHOW_HLINES, _view.horizontal);
            menu::set_item(_menu, _CHART_SHOW_VLINES, _view.vertical);
            _menu->popup();
            return 1;
        }
    }
    else if (event == FL_DRAG) { // FL_PUSH must have been called before mouse drag
        _area = _inside_area(Fl::event_x(), Fl::event_y());
        _create_tooltip(Fl::event_ctrl());

        if (_tooltip != "") {
            return 1;
        }
    }
    else if (event == FL_MOVE) { // Remove tooltip if mouse is outside chart area
        if (_tooltip != "") {
            _tooltip = "";
            redraw();
        }
    }
    else if (event == FL_MOUSEWHEEL) {
        const double size = _dates.size() - _ticks;
        const double pos  = _scroll->value();
        const double dy   = (Fl::event_dy() * 10);
        const double dx   = (Fl::event_dx() * 10);
        const double dd   = fabs(dx) > fabs(dy) ? dx : dy;
        const double adj  = _ticks / dd;

        if (fabs(dd) < 1.0) {
            return Fl_Group::handle(event);
        }
        else if (Fl::event_ctrl() > 0) {
            const int width = (adj > 0.0) ? _tick_width + 1 : _tick_width - 1;

            if (width >= Chart::MIN_TICK && width <= Chart::MAX_TICK) {
                tick_width(width);
                init(false);
            }

            return 1;
        }
        else {
            if (adj > 0) {
                if ((pos + adj) > size) {
                    ((Fl_Slider*) _scroll)->value(size);
                }
                else {
                    ((Fl_Slider*) _scroll)->value(pos + adj);
                }
            }
            else {
                if ((pos + adj) < 0.0) {
                    ((Fl_Slider*) _scroll)->value(0.0);
                }
                else {
                    ((Fl_Slider*) _scroll)->value(pos + adj);
                }
            }

            Chart::_CallbackScrollbar(0, this);
            Fl::event_clicks(0);
            return 1;
        }
    }
    else if (event == FL_KEYDOWN) {
        auto key = Fl::event_key();

        if (key >= '0' && key <= '9') {
            if (_area == nullptr) {
                fl_beep(FL_BEEP_ERROR);
            }
            else {
                auto selected = key - '0';

                if (selected == 0) {
                    selected = 10;
                }

                selected--;

                if ((size_t) selected >= _area->count) {
                    selected = 0;
                }

                if (Fl::event_shift() != 0) {
                    _area->lines[selected].visible = !_area->lines[selected].visible;
                }
                else {
                    _area->selected = selected;
                }

                redraw();
            }
        }
    }

    return Fl_Group::handle(event);
}

//------------------------------------------------------------------------------
bool Chart::has_time() const {
    return _chart_has_time(_date_range);
}

//------------------------------------------------------------------------------
void Chart::init(bool calc_dates) {
#ifdef DEBUG
    // auto t = util::time_milli();
#endif

    if (calc_dates == true) {
        _calc_dates();
    }

    _calc_area_height();
    _calc_area_width();
    _calc_ymin_ymax();
    _calc_yscale();
    redraw();

#ifdef DEBUG
    // fprintf(stderr, "%s: %3d mS\n", (calc_dates == true) ? "INIT" : "init", (int) (util::time_milli() - t));
    // fflush(stdout);
#endif
}

//------------------------------------------------------------------------------
flw::ChartArea* Chart::_inside_area(int X, int Y) {
    for (auto& area : _areas) {
        if (area.percent >= 10 &&
            X >= x() + (int) area.x &&
            Y >= y() + (int) area.y &&
            X <= x() + (int) (area.x + area.w) &&
            Y <= y() + (int) (area.y + area.h + 1)) {
            return &area;
        }
    }

    return nullptr;
}

//------------------------------------------------------------------------------
bool Chart::margin(int left, int right) {
    if (left < 1 || left > 20 || right < 1 || right > 20) {
        return false;
    }

    _margin_left  = left;
    _margin_right = right;
    redraw();
    return true;
}

//------------------------------------------------------------------------------
void Chart::resize(int X, int Y, int W, int H) {
    if (_old_width != W || _old_height != H) {
        Fl_Group::resize(X, Y, W, H);
        _scroll->resize(X, Y + H - Fl::scrollbar_size(), W, Fl::scrollbar_size());
        _old_width = W;
        _old_height = H;
        init(false);
    }
}

//------------------------------------------------------------------------------
bool Chart::tick_width(int tick_width) {
    if (tick_width < 3 || tick_width > 100) {
        return false;
    }

    _tick_width = tick_width;
    redraw();
    return true;
}

//------------------------------------------------------------------------------
void Chart::update_pref() {
    _menu->textfont(flw::PREF_FONT);
    _menu->textsize(flw::PREF_FONTSIZE);
}

/***
 *       _____ _                _       _____ _        _   _
 *      / ____| |              | |     / ____| |      | | (_)
 *     | |    | |__   __ _ _ __| |_   | (___ | |_ __ _| |_ _  ___
 *     | |    | '_ \ / _` | '__| __|   \___ \| __/ _` | __| |/ __|
 *     | |____| | | | (_| | |  | |_    ____) | || (_| | |_| | (__
 *      \_____|_| |_|\__,_|_|   \__|  |_____/ \__\__,_|\__|_|\___|
 *
 *
 */

//------------------------------------------------------------------------------
void Chart::_CallbackDebug(Fl_Widget*, void* chart_object) {
    auto self = (Chart*) chart_object;
    self->debug();
}

//------------------------------------------------------------------------------
void Chart::_CallbackToggle(Fl_Widget*, void* chart_object) {
    auto self = (Chart*) chart_object;

    self->_view.labels     = menu::item_value(self->_menu, _CHART_SHOW_LABELS);
    self->_view.vertical   = menu::item_value(self->_menu, _CHART_SHOW_VLINES);
    self->_view.horizontal = menu::item_value(self->_menu, _CHART_SHOW_HLINES);
    self->redraw();
}

//------------------------------------------------------------------------------
void Chart::_CallbackReset(Fl_Widget*, void* chart_object) {
    auto self = (Chart*) chart_object;

    for (auto& area : self->_areas) {
        area.selected = 0;

        for (auto& line : area.lines) {
            line.visible = true;
        }
    }

    self->redraw();
}

//------------------------------------------------------------------------------
void Chart::_CallbackSavePng(Fl_Widget*, void* chart_object) {
    auto self = (Chart*) chart_object;
    util::png_save("", self->window(), self->x() + 1,  self->y() + 1,  self->w() - 2,  self->h() - self->_scroll->h() - 1);
}

//------------------------------------------------------------------------------
void Chart::_CallbackScrollbar(Fl_Widget*, void* chart_object) {
    auto self = (Chart*) chart_object;
    self->_date_start = self->_scroll->value();
    self->init(false);
}

#define FLW_CHART_ERROR(X) { fl_alert("error: illegal chart value at pos %u", (X)->pos()); chart->clear(); return false; }

//------------------------------------------------------------------------------
bool Chart::Load(Chart* chart, std::string filename) {
    chart->clear();
    chart->redraw();

    auto wc  = WaitCursor();
    auto buf = util::load_file(filename);

    if (buf.p == nullptr) {
        fl_alert("error: failed to load %s", filename.c_str());
        return false;
    }

    auto js  = JS();
    auto err = js.decode(buf.p, buf.s);

    if (err != "") {
        fl_alert("error: failed to parse %s (%s)", filename.c_str(), err.c_str());
        return false;
    }

    if (js.is_object() == false) FLW_CHART_ERROR(&js);

    for (auto j : js.vo_to_va()) {
        if (j->name() == "descr" && j->is_object() == true) {
            for (auto j2 : j->vo_to_va()) {
                if (j2->name() == "type" && j2->is_string() == true) {
                    if (j2->vs() != "flw::chart") FLW_CHART_ERROR(j2)
                }
                else if (j2->name() == "version" && j2->is_number() == true) {
                    if (j2->vn_i() != 2) FLW_CHART_ERROR(j2)
                }
                else FLW_CHART_ERROR(j2)
            }
        }
        else if (j->name() == "descr_area" && j->is_object() == true) {
            long long int area[11] = { 0 };

            for (auto j2 : j->vo_to_va()) {
                if (j2->name() == "area0" && j2->is_number() == true)             area[0] = j2->vn_i();
                else if (j2->name() == "area1" && j2->is_number() == true)        area[1] = j2->vn_i();
                else if (j2->name() == "area2" && j2->is_number() == true)        area[2] = j2->vn_i();
                else if (j2->name() == "date_range" && j2->is_number() == true)   area[3] = j2->vn_i();
                else if (j2->name() == "horizontal" && j2->is_bool() == true)     area[4] = j2->vb();
                else if (j2->name() == "labels" && j2->is_bool() == true)         area[5] = j2->vb();
                else if (j2->name() == "margin_left" && j2->is_number() == true)  area[6] = j2->vn_i();
                else if (j2->name() == "margin_right" && j2->is_number() == true) area[7] = j2->vn_i();
                else if (j2->name() == "tick_width" && j2->is_number() == true)   area[8] = j2->vn_i();
                else if (j2->name() == "vertical" && j2->is_bool() == true)       area[9] = j2->vb();
                else FLW_CHART_ERROR(j2)
            }

            area[10] += chart->area_size(area[0], area[1], area[2]) == false;
            area[10] += chart->tick_width(area[8]) == false;
            area[10] += chart->date_range((Date::RANGE) area[3]) == false;
            area[10] += chart->margin(area[6], area[7]) == false;

            if (area[10] != 0) FLW_CHART_ERROR(j)
            chart->view_options(area[5], area[4], area[9]);
        }
        else if (j->name() == "lines" && j->is_array() == true) {
            for (auto j2 : *j->va()) {
                int         line[5]  = { 0 };
                double      clamp[2] = { 0.0 };
                std::string label;
                PriceVector prices;

                if ( j2->is_object() == true) {
                    for (auto l : j2->vo_to_va()) {
                        if (l->name() == "align" && l->is_number() == true)           line[0]  = l->vn_i();
                        else if (l->name() == "area" && l->is_number() == true)       line[1]  = l->vn_i();
                        else if (l->name() == "clamp_max" && l->is_number() == true)  clamp[0] = l->vn();
                        else if (l->name() == "clamp_min" && l->is_number() == true)  clamp[1] = l->vn();
                        else if (l->name() == "color" && l->is_number() == true)      line[2]  = l->vn_i();
                        else if (l->name() == "label" && l->is_string() == true)      label    = l->vs_u();
                        else if (l->name() == "type" && l->is_string() == true)       line[3]  = _chart_string_to_type(l->vs());
                        else if (l->name() == "width" && l->is_number() == true)      line[4]  = l->vn_i();
                        else if (l->name() == "yx" && l->is_array() == true) {
                            for (auto p : *l->va()) {
                                if (p->is_array() == false) FLW_CHART_ERROR(p)
                                else if (p->size() == 2 && (*p)[0]->is_string() == true && (*p)[1]->is_number() == true) prices.push_back(Price((*p)[0]->vs(), (*p)[1]->vn()));
                                else if (p->size() == 4 && (*p)[0]->is_string() == true && (*p)[1]->is_number() == true && (*p)[2]->is_number() == true && (*p)[3]->is_number() == true) prices.push_back(Price((*p)[0]->vs(), (*p)[1]->vn(), (*p)[2]->vn(), (*p)[3]->vn()));
                                else FLW_CHART_ERROR(p)
                            }
                        }
                        else FLW_CHART_ERROR(l)
                    }
                }

                if (chart->add_line(line[1], prices, label, (Chart::TYPE) line[3], line[0], line[2], line[4], clamp[1], clamp[0]) == false) FLW_CHART_ERROR(j2)
            }
        }
        else if (j->name() == "lines_block" && j->is_array() == true) {
            PriceVector dates;

            for (auto d : *j->va()) {
                if (d->is_string() == true) dates.push_back(Price(d->vs()));
                else FLW_CHART_ERROR(d)
            }

            chart->block_dates(dates);
        }
        else FLW_CHART_ERROR(j)
    }

    chart->init(true);
    return true;
}

//----------------------------------------------------------------------
bool Chart::Save(const Chart* chart, std::string filename, double max_diff_high_low) {
    auto ac  = 0;
    auto wc  = WaitCursor();
    auto jsb = JSB();

    try {
        jsb << JSB::MakeObject();
            jsb << JSB::MakeObject("descr");
                jsb << JSB::MakeString("flw::chart", "type");
                jsb << JSB::MakeNumber(2, "version");
            jsb.end();
            jsb << JSB::MakeObject("descr_area");
                jsb << JSB::MakeNumber(chart->_areas[0].percent, "area0");
                jsb << JSB::MakeNumber(chart->_areas[1].percent, "area1");
                jsb << JSB::MakeNumber(chart->_areas[2].percent, "area2");
                jsb << JSB::MakeNumber(chart->_tick_width, "tick_width");
                jsb << JSB::MakeNumber((double) chart->_date_range, "date_range");
                jsb << JSB::MakeNumber(chart->_margin_left, "margin_left");
                jsb << JSB::MakeNumber(chart->_margin_right, "margin_right");
                jsb << JSB::MakeBool(chart->_view.labels, "labels");
                jsb << JSB::MakeBool(chart->_view.horizontal, "horizontal");
                jsb << JSB::MakeBool(chart->_view.vertical, "vertical");
            jsb.end();
            jsb << JSB::MakeArray("lines");
                for (const auto& area : chart->_areas) {
                    for (const auto& line : area.lines) {
                        if (line.points.size() > 0) {
                            jsb << JSB::MakeObject();
                                jsb << JSB::MakeNumber(ac, "area");
                                jsb << JSB::MakeString(line.label, "label");
                                jsb << JSB::MakeString(_chart_type_to_string(line.type), "type");
                                jsb << JSB::MakeNumber(line.align, "align");
                                jsb << JSB::MakeNumber(line.color, "color");
                                jsb << JSB::MakeNumber(line.width, "width");
                                jsb << JSB::MakeNumber(line.clamp_min, "clamp_min");
                                jsb << JSB::MakeNumber(line.clamp_max, "clamp_max");
                                jsb << JSB::MakeArray("yx");
                                for (auto& price : line.points) {
                                    jsb << JSB::MakeArrayInline();
                                        jsb << JSB::MakeString(_chart_format_date(price, (chart->has_time() == true) ? Date::FORMAT::ISO_TIME : Date::FORMAT::ISO));
                                        if (fabs(price.close - price.low) > max_diff_high_low || fabs(price.close - price.high) > max_diff_high_low) {
                                            jsb << JSB::MakeNumber(price.high);
                                            jsb << JSB::MakeNumber(price.low);
                                        }
                                        jsb << JSB::MakeNumber(price.close);
                                        jsb.end();
                                }
                                jsb.end();
                            jsb.end();
                        }
                    }
                    ac++;
                }
            jsb.end();
            jsb << JSB::MakeArray("lines_block");
                for (auto& price : chart->_block_dates) {
                    jsb << JSB::MakeString(price.date);
                }
            jsb.end();

        auto js = jsb.encode();
        return util::save_file(filename, js.c_str(), js.length());
    }
    catch(std::string e) {
        fl_alert("error: failed to encode json\n%s", e.c_str());
        return false;
    }
}

}

// MKALGAM_OFF
