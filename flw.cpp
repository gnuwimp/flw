// Copyright gnuwimp@gmail.com
// Released under the GNU General Public License v3.0
#include "flw.h"
#include <FL/Fl_Menu_Button.H>
#include <FL/Fl_Scrollbar.H>
#include <FL/fl_ask.H>
#include <FL/fl_draw.H>
#include <algorithm>
#include <math.h>
namespace flw {
    namespace chart {
        static const int                MAX_VLINES      = 100;
        static const int                MAX_LINE_WIDTH  = 100;
        static const char* const        SHOW_LABELS     = "Show line labels";
        static const char* const        SHOW_HLINES     = "Show horizontal lines";
        static const char* const        SHOW_VLINES     = "Show vertical lines";
        static const char* const        RESET_SELECT    = "Reset line selection and visibility";
        static const char* const        SAVE_PNG        = "Save png to file...";
        size_t bsearch(const PriceVector& prices, const Price& key) {
            auto it = std::lower_bound(prices.begin(), prices.end(), key);
            if (it == prices.end() || *it != key) {
                return (size_t) -1;
            }
            else {
                return std::distance(prices.begin(), it);
            }
        }
        int count_decimals(double number) {
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
        flw::PriceVector create_date_serie(const char* start_date, const char* stop_date, Date::RANGE range, const PriceVector& block, bool long_format) {
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
        std::string format_date(const Price& price, Date::FORMAT format) {
            auto date = flw::Date::FromString(price.date.c_str());
            return date.format(format);
        }
        std::string format_double(double num, int decimals, char del) {
            char res[100];
            *res = 0;
            if (decimals < 0) {
                decimals = chart::count_decimals(num);
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
        bool has_high_low(chart::TYPE chart_type) {
            return chart_type == chart::TYPE::BAR || chart_type == chart::TYPE::VERTICAL || chart_type == chart::TYPE::CLAMP_VERTICAL;
        }
        bool has_resizable_width(chart::TYPE chart_type) {
            return chart_type == chart::TYPE::BAR || chart_type == chart::TYPE::VERTICAL || chart_type == chart::TYPE::CLAMP_VERTICAL;
        }
        bool has_time(Date::RANGE date_range) {
            return date_range == Date::RANGE::HOUR || date_range == Date::RANGE::MIN || date_range == Date::RANGE::SEC;
        }
        bool load_data(Chart* chart, std::string filename) {
            auto wc  = WaitCursor();
            auto buf = util::load_file(filename);
            auto nv  = json::NodeVector();
            auto ok  = 0;
            chart->clear();
            chart->redraw();
            if (buf.p == nullptr) {
                fl_alert("error: failed to load %s", filename.c_str());
                return false;
            }
            auto err = json::parse(buf.p, nv, true);
            if (err.pos >= 0) {
                fl_alert("error: failed to parse %s (line %d and byte %d)", filename.c_str(), (int) err.line, (int) err.pos);
                return false;
            }
            for (const auto& n : json::find_children(nv, nv[0])) {
                auto children = json::find_children(nv, n);
                if (n.name == "descr" && n.is_object() == true) {
                    for (auto& n2 : children) {
                        if ((n2.name == "type" && n2.value == "flw::chart") || (n2.name == "version" && n2.value == "1")) {
                            ok++;
                        }
                    }
                }
                else if (ok != 2) {
                    fl_alert("error: unknown file format");
                    return false;
                }
                else if (n.name == "area" && n.is_object() == true) {
                    int data[12] = { 0 };
                    for (auto& n2 : children) {
                        if (n2.name == "area0")             data[0] = (int) n2.toint();
                        else if (n2.name == "area1")        data[1] = (int) n2.toint();
                        else if (n2.name == "area2")        data[2] = (int) n2.toint();
                        else if (n2.name == "tick_width")   data[3] = (int) n2.toint();
                        else if (n2.name == "date_range")   data[4] = (int) n2.toint();
                        else if (n2.name == "margin_left")  data[5] = (int) n2.toint();
                        else if (n2.name == "margin_right") data[6] = (int) n2.toint();
                        else if (n2.name == "labels")       data[7] = n2.tobool();
                        else if (n2.name == "horizontal")   data[8] = n2.tobool();
                        else if (n2.name == "vertical")     data[9] = n2.tobool();
                    }
                    data[10] += chart->area_size(data[0], data[1], data[2]) == false;
                    data[10] += chart->tick_width(data[3]) == false;
                    data[10] += chart->date_range((Date::RANGE) data[4]) == false;
                    data[10] += chart->margin(data[5], data[6]) == false;
                    chart->view_options(data[7], data[8], data[9]);
                    if (data[10] != 0) {
                        fl_alert("error: failed to set area data started on position %u", (unsigned) n.textpos);
                        chart->clear();
                        return false;
                    }
                }
                else if (n.name == "line" && n.is_object() == true) {
                    int         area[6]  = { 0 };
                    double      clamp[3] = { 0.0 };
                    std::string label;
                    PriceVector prices;
                    for (auto& n2 : children) {
                        if (n2.name == "label" && n2.is_string() == true)          label = n2.value;
                        else if (n2.name == "area" && n2.is_number() == true)      area[0] = (int) n2.toint();
                        else if (n2.name == "type" && n2.is_number() == true)      area[1] = (int) n2.toint();
                        else if (n2.name == "align" && n2.is_number() == true)     area[2] = (int) n2.toint();
                        else if (n2.name == "color" && n2.is_number() == true)     area[3] = (int) n2.toint();
                        else if (n2.name == "width" && n2.is_number() == true)     area[4] = (int) n2.toint();
                        else if (n2.name == "clamp_min" && n2.is_number() == true) clamp[0] = n2.tonumber();
                        else if (n2.name == "clamp_max" && n2.is_number() == true) clamp[1] = n2.tonumber();
                        else if (n2.name == "p" && n2.is_array() == true) {
                            auto pv   = json::find_children(nv, n2);
                            auto date = Date::FromString(pv[0].value.c_str()).year() > 1;
                            if (pv.size() == 2 && date == true && pv[1].is_number() == true) {
                                prices.push_back(Price(pv[0].value, pv[1].tonumber()));
                            }
                            else if (pv.size() == 4 && date == true && pv[1].is_number() == true && pv[2].is_number() == true && pv[3].is_number() == true) {
                                prices.push_back(Price(pv[0].value, pv[1].tonumber(), pv[2].tonumber(), pv[3].tonumber()));
                            }
                            else {
                                fl_alert("error: failed to add price data that started on position %u", (unsigned) pv[0].textpos);
                                chart->clear();
                                return false;
                            }
                        }
                    }
                    if (chart->add_line(area[0], prices, label, (chart::TYPE) area[1], area[2], area[3], area[4], clamp[0], clamp[1]) == false) {
                        fl_alert("error: failed to add line that started on position %u", (unsigned) n.textpos);
                        chart->clear();
                        return false;
                    }
                }
                else if (n.name == "blockdates" && n.is_array() == true) {
                    PriceVector dates;
                    for (auto& n2 : children) {
                        if (n2.name == "" && n2.is_string() == true) {
                            auto d = Date::FromString(n2.value.c_str());
                            if (d.year() > 1) {
                                dates.push_back(Price(n2.value, 0.0));
                            }
                            else {
                                fl_alert("error: failed to add block date that started on position %u", (unsigned) n2.textpos);
                                chart->clear();
                                return false;
                            }
                        }
                    }
                    chart->block_dates(dates);
                }
            }
            chart->init(true);
            return true;
        }
        bool save_data(const Chart* chart, std::string filename, double max_diff_high_low) {
            auto nv = json::NodeVector();
            auto ac = 0;
            FLW_JSON_START(nv,
                FLW_JSON_START_OBJECT(
                    FLW_JSON_ADD_OBJECT("descr",
                        FLW_JSON_ADD_STRING("type", "flw::chart")
                        FLW_JSON_ADD_INT("version", 1)
                    )
                    FLW_JSON_ADD_OBJECT("area",
                        for (auto& area : chart->_areas) {
                        FLW_JSON_ADD_INT(util::format("area%d", ac++), area.percent)
                        }
                        FLW_JSON_ADD_UINT("tick_width", chart->_tick_width)
                        FLW_JSON_ADD_UINT("date_range", chart->_date_range)
                        FLW_JSON_ADD_UINT("margin_left", chart->_margin_left)
                        FLW_JSON_ADD_UINT("margin_right", chart->_margin_right)
                        FLW_JSON_ADD_BOOL("labels", chart->_view.labels)
                        FLW_JSON_ADD_BOOL("horizontal", chart->_view.horizontal)
                        FLW_JSON_ADD_BOOL("vertical", chart->_view.vertical)
                    )
                    ac = 0;
                    for (const auto& area : chart->_areas) {
                    for (const auto& line : area.lines) {
                    if (line.points.size() > 0) {
                    FLW_JSON_ADD_OBJECT("line",
                        FLW_JSON_ADD_STRING("label", line.label)
                        FLW_JSON_ADD_UINT("area", ac)
                        FLW_JSON_ADD_UINT("type", line.type)
                        FLW_JSON_ADD_UINT("align", line.align)
                        FLW_JSON_ADD_UINT("color", line.color)
                        FLW_JSON_ADD_INT("width", line.width)
                        FLW_JSON_ADD_NUMBER("clamp_min", line.clamp_min)
                        FLW_JSON_ADD_NUMBER("clamp_max", line.clamp_max)
                        for (auto& price : line.points) {
                        FLW_JSON_ADD_ARRAY_NL("p",
                            FLW_JSON_ADD_STRING_TO_ARRAY(chart::format_date(price, (chart->has_time() == true) ? Date::FORMAT::ISO_TIME : Date::FORMAT::ISO))
                            if (fabs(price.close - price.low) > max_diff_high_low || fabs(price.close - price.high) > max_diff_high_low) {
                            FLW_JSON_ADD_NUMBER_TO_ARRAY(price.high)
                            FLW_JSON_ADD_NUMBER_TO_ARRAY(price.low)
                            }
                            FLW_JSON_ADD_NUMBER_TO_ARRAY(price.close)
                        )
                        }
                    )
                    }
                    }
                    ac++;
                    }
                    if (chart->_block_dates.size() > 0) {
                    FLW_JSON_ADD_ARRAY("blockdates",
                        for (auto& price : chart->_block_dates) {
                        FLW_JSON_ADD_STRING_TO_ARRAY(price.date)
                        }
                    )
                    }
                )
            )
            auto js = json::tostring(nv);
            return util::save_file(filename, js.c_str(), js.length());
        }
    }
}
flw::Price::Price() {
    high  = 0.0;
    low   = 0.0;
    close = 0.0;
    vol   = 0.0;
}
flw::Price::Price(const std::string& date, double value) {
    this->date = date;
    high       = value;
    low        = value;
    close      = value;
    vol        = value;
}
flw::Price::Price(const std::string& date, double high, double low, double close, double vol) {
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
flw::chart::Area::Area() {
    for (size_t f = 0; f < chart::MAX_LINE; f++) {
        lines.push_back(chart::Line());
    }
    clear(false);
}
void flw::chart::Area::clear(bool clear_data) {
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
        for (size_t f = 0; f < chart::MAX_LINE; f++) {
            lines[f] = chart::Line();
        }
    }
}
void flw::chart::Area::debug(int num) const {
#ifdef DEBUG
    auto count = 1;
    for (const auto& line : lines) {
        if (line.points.size() > 0) {
            count = 0;
        }
    }
    if (count == 0 || num == 0) {
        fprintf(stderr, "\t----------------------\n");
        fprintf(stderr, "\tflw::chart::Area: %d\n", num);
        fprintf(stderr, "\t\tcount:    %d\n", (int) count);
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
flw::chart::Line::Line() {
    clear();
}
void flw::chart::Line::clear() {
    points.clear();
    align     = FL_ALIGN_CENTER;
    type      = chart::TYPE::LINE;
    clamp_max = chart::MAX_VAL;
    clamp_min = chart::MIN_VAL;
    color     = FL_FOREGROUND_COLOR;
    label     = "";
    max       = chart::MIN_VAL;
    min       = chart::MAX_VAL;
    visible   = true;
    width     = 1;
}
void flw::chart::Line::debug(int num) const {
#ifdef DEBUG
    fprintf(stderr, "\t\t---------------------------------------------\n");
    fprintf(stderr, "\t\tflw::chart::Line: %d\n", num);
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
bool flw::chart::Line::set(const PriceVector& points, std::string label, chart::TYPE type, Fl_Align align, Fl_Color color, int width, double clamp_min, double clamp_max) {
    if (width == 0 || width > MAX_LINE_WIDTH || (int) type < 0 || (int) type > (int) chart::TYPE::LAST) {
        return false;
    }
    if (width < 0 && chart::has_resizable_width(type) == false) {
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
flw::chart::Scale::Scale() {
    clear();
}
void flw::chart::Scale::calc(int height) {
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
void flw::chart::Scale::clear() {
    min   = chart::MAX_VAL;
    max   = chart::MIN_VAL;
    tick  = 0.0;
    pixel = 0.0;
}
void flw::chart::Scale::debug(const char* name) const {
#ifdef DEBUG
    fprintf(stderr, "\t\t---------------------------------------------\n");
    fprintf(stderr, "\t\tflw::chart::Scale: %s\n", name);
    fprintf(stderr, "\t\t\tmin:   %30.6f\n", min);
    fprintf(stderr, "\t\t\tmax:   %30.6f\n", max);
    fprintf(stderr, "\t\t\tDiff:  %30.6f\n", diff());
    fprintf(stderr, "\t\t\ttick:  %30.6f\n", tick);
    fprintf(stderr, "\t\t\tpixel: %30.6f\n", pixel);
#else
    (void) name;
#endif
}
void flw::chart::Scale::fix_height() {
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
flw::Chart::Chart(int X, int Y, int W, int H, const char* l) : Fl_Group(X, Y, W, H, l) {
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
    _menu->add(chart::SHOW_LABELS, 0, Chart::_CallbackToggle, this, FL_MENU_TOGGLE);
    _menu->add(chart::SHOW_HLINES, 0, Chart::_CallbackToggle, this, FL_MENU_TOGGLE);
    _menu->add(chart::SHOW_VLINES, 0, Chart::_CallbackToggle, this, FL_MENU_TOGGLE | FL_MENU_DIVIDER);
    _menu->add(chart::RESET_SELECT, 0, Chart::_CallbackReset, this);
    _menu->add(chart::SAVE_PNG, 0, Chart::_CallbackSavePng, this);
#ifdef DEBUG
    _menu->add("Debug", 0, Chart::_CallbackDebug, this);
#endif
    _menu->type(Fl_Menu_Button::POPUP3);
    _areas.push_back(chart::Area());
    _areas.push_back(chart::Area());
    _areas.push_back(chart::Area());
    clear();
    update_pref();
    view_options();
}
bool flw::Chart::add_line(size_t area, const PriceVector& points, std::string line_label, chart::TYPE chart_type, Fl_Align line_align, Fl_Color line_color, int line_width, double clamp_min, double clamp_max) {
    _area = nullptr;
    redraw();
    if (area >= chart::MAX_AREA || _areas[area].count >= chart::MAX_LINE) {
        return false;
    }
    if (_areas[area].lines[_areas[area].count].set(points, line_label, chart_type, line_align, line_color, line_width, clamp_min, clamp_max) == false) {
        _areas[area].lines[_areas[area].count] = chart::Line();
        return false;
    }
    _areas[area].count++;
    return true;
}
bool flw::Chart::area_size(int area1, int area2, int area3) {
    _area = nullptr;
    if (area1 < 0 || area1 > 100 || area2 < 0 || area2 > 100 || area3 < 0 || area3 > 100 || area1 + area2 + area3 != 100) {
        return false;
    }
    _areas[0].percent = area1;
    _areas[1].percent = area2;
    _areas[2].percent = area3;
    return true;
}
void flw::Chart::_calc_area_height() {
    int last   = 0;
    int addh   = 0;
    int height = 0;
    _top_space    = flw::PREF_FIXED_FONTSIZE;
    _bottom_space = flw::PREF_FIXED_FONTSIZE * 3 + Fl::scrollbar_size();
    height        = h() - (_bottom_space + _top_space);
    for (size_t f = 1; f < chart::MAX_AREA; f++) {
        const auto& area = _areas[f];
        if (area.percent >= 10) {
            height -= PREF_FIXED_FONTSIZE;
        }
    }
    _areas[0].y = _top_space;
    _areas[0].h = (int) ((_areas[0].percent / 100.0) * height);
    for (size_t f = 1; f < chart::MAX_AREA; f++) {
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
void flw::Chart::_calc_area_width() {
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
void flw::Chart::_calc_dates() {
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
        _dates = chart::create_date_serie(min.c_str(), max.c_str(), _date_range, _block_dates, long_date);
        redraw();
    }
}
void flw::Chart::_calc_ymin_ymax() {
    for (auto& area : _areas) {
        area.left.clear();
        area.right.clear();
        for (const auto& line : area.lines) {
            if (line.points.size() > 0) {
                const int stop    = _date_start + _ticks;
                int       current = _date_start;
                double    max     = chart::MIN_VAL;
                double    min     = chart::MAX_VAL;
                while (current <= stop && current < (int) _dates.size()) {
                    const Price& date  = _dates[current];
                    const size_t index = chart::bsearch(line.points, date);
                    if (index != (size_t) -1) {
                        const Price& price = line.points[index];
                        if (chart::has_high_low(line.type) == true) {
                            min = price.low;
                            max = price.high;
                        }
                        else {
                            min = price.close;
                            max = price.close;
                        }
                        if ((int64_t) line.clamp_min > chart::MIN_VAL) {
                            min = line.clamp_min;
                        }
                        if ((int64_t) line.clamp_max < chart::MAX_VAL) {
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
void flw::Chart::_calc_yscale() {
    for (auto& area : _areas) {
        area.left.calc(area.h);
        area.right.calc(area.h);
    }
}
void flw::Chart::_CallbackDebug(Fl_Widget*, void* chart_object) {
    auto self = (Chart*) chart_object;
    self->debug();
}
void flw::Chart::_CallbackToggle(Fl_Widget*, void* chart_object) {
    auto self = (Chart*) chart_object;
    self->_view.labels     = menu::item_value(self->_menu, chart::SHOW_LABELS);
    self->_view.vertical   = menu::item_value(self->_menu, chart::SHOW_VLINES);
    self->_view.horizontal = menu::item_value(self->_menu, chart::SHOW_HLINES);
    self->redraw();
}
void flw::Chart::_CallbackReset(Fl_Widget*, void* chart_object) {
    auto self = (Chart*) chart_object;
    for (auto& area : self->_areas) {
        area.selected = 0;
        for (auto& line : area.lines) {
            line.visible = true;
        }
    }
    self->redraw();
}
void flw::Chart::_CallbackSavePng(Fl_Widget*, void* chart_object) {
    auto self = (Chart*) chart_object;
    util::png_save("", self->window(), self->x() + 1,  self->y() + 1,  self->w() - 2,  self->h() - self->_scroll->h() - 1);
}
void flw::Chart::_CallbackScrollbar(Fl_Widget*, void* chart_object) {
    auto self = (Chart*) chart_object;
    self->_date_start = self->_scroll->value();
    self->init(false);
}
void flw::Chart::clear() {
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
void flw::Chart::_create_tooltip(bool ctrl) {
    const int X   = Fl::event_x();
    const int Y   = Fl::event_y();
    auto      old = _tooltip;
    _tooltip = "";
    if (_area != nullptr) {
        const auto date_format = (_date_format == flw::Date::FORMAT::ISO) ? flw::Date::FORMAT::NAME_LONG : flw::Date::FORMAT::ISO_TIME_LONG;
        const int  stop        = _date_start + _ticks;
        int        start       = _date_start;
        int        X1          = x() + _margin_left * flw::PREF_FIXED_FONTSIZE;
        int        left_dec    = 0;
        int        right_dec   = 0;
        if (_area->left.tick < 10.0 ) {
            left_dec = chart::count_decimals(_area->left.tick) + 1;
        }
        if (_area->right.tick < 10.0 ) {
            right_dec = chart::count_decimals(_area->right.tick) + 1;
        }
        while (start <= stop && start < (int) _dates.size()) {
            if (X >= X1 && X <= X1 + _tick_width - 1) {
                const std::string fancy_date = Date::FromString(_dates[start].date.c_str()).format(date_format);
                _tooltip = fancy_date;
                if (ctrl == false || _area->lines[_area->selected].points.size() == 0) {
                    const double ydiff = (double) ((y() + _area->y2()) - Y);
                    std::string  left;
                    std::string  right;
                    if (_area->left.max > _area->left.min) {
                        left = chart::format_double(_area->left.min + (ydiff / _area->left.pixel), left_dec, '\'');
                    }
                    if (_area->right.max > _area->right.min) {
                        right = chart::format_double(_area->right.min + (ydiff / _area->right.pixel), right_dec, '\'');
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
                else {
                    const auto&  line  = _area->lines[_area->selected];
                    const size_t index = chart::bsearch(line.points, _dates[start].date);
                    if (index != (size_t) -1) {
                        const auto dec   = (line.align == FL_ALIGN_RIGHT) ? right_dec : left_dec;
                        const auto price = line.points[index];
                        auto       high  = chart::format_double(price.high, dec, '\'');
                        auto       low   = chart::format_double(price.low, dec, '\'');
                        auto       close = chart::format_double(price.close, dec, '\'');
                        const auto len   = (low.length() > high.length()) ? low.length() : high.length();
                        _tooltip = util::format("%s\nhigh:  %*s\nclose: %*s\nlow:   %*s", fancy_date.c_str(), (int) len, high.c_str(), (int) len, close.c_str(), (int) len, low.c_str());
                    }
                }
                break;
            }
            X1    += _tick_width;
            start += 1;
        }
    }
    if (_tooltip != "" || old != "") {
        redraw();
    }
}
bool flw::Chart::date_range(Date::RANGE range) {
    if ((int) range < 0 || (int) range > (int) Date::RANGE::LAST) {
        return false;
    }
    _date_range  = range;
    _date_format = (_date_range == Date::RANGE::HOUR || _date_range == Date::RANGE::MIN || _date_range == Date::RANGE::SEC) ? Date::FORMAT::ISO_TIME : Date::FORMAT::ISO;
    return true;
}
void flw::Chart::debug() const {
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
    fprintf(stderr, "flw::Chart:\n");
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
void flw::Chart::draw() {
#ifdef DEBUG
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
#endif
}
void flw::Chart::_draw_area(const chart::Area& area) {
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
void flw::Chart::_draw_line(const chart::Line& line, const chart::Scale& scale, int X, const int Y, const int W, const int H) {
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
        if (line.type == chart::TYPE::BAR) {
            fl_line_style(FL_SOLID, lw2);
        }
        else {
            fl_line_style(FL_SOLID, width);
        }
        while (current <= stop && current < (int) _dates.size()) {
            const auto& date  = _dates[current];
            const auto  index = chart::bsearch(line.points, date);
            if (index != (size_t) -1) {
                const auto& pr = line.points[index];
                const auto  yh = (int) ((pr.high - scale.min) * scale.pixel);
                const auto  yl = (int) ((pr.low - scale.min) * scale.pixel);
                const auto  yc = (int) ((pr.close - scale.min) * scale.pixel);
                if (line.type == chart::TYPE::LINE) {
                    if (lastX > -1 && lastY > -1) {
                        fl_line(lastX + lw2, y2 - lastY, x() + X + lw2, y2 - yc);
                    }
                }
                else if (line.type == chart::TYPE::BAR) {
                    fl_line(x() + X + lw4, y2 - yl, x() + X + lw4, y2 - yh);
                    fl_line(x() + X, y2 - yc, x() + X + _tick_width - 1, y2 - yc);
                }
                else if (line.type == chart::TYPE::HORIZONTAL) {
                    fl_line(x() + X, y2 - yc, x() + X + _tick_width, y2 - yc);
                }
                else if (line.type == chart::TYPE::VERTICAL) {
                    auto h = yh - yl;
                    fl_rectf(x() + X, y2 - yh, width, (h < 1) ? 1 : h);
                }
                else if (line.type == chart::TYPE::CLAMP_VERTICAL) {
                    fl_rectf(x() + X, y2 - yh, width, yh);
                }
                else if (line.type == chart::TYPE::EXPAND_VERTICAL) {
                    fl_line(x() + X, y2, x() + X, y() + Y);
                }
                else if (line.type == chart::TYPE::EXPAND_HORIZONTAL) {
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
void flw::Chart::_draw_line_labels(const chart::Area& area) {
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
void flw::Chart::_draw_tooltip() {
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
void flw::Chart::_draw_ver_lines(const chart::Area& area) {
    if (_view.vertical == true) {
        fl_color(fl_color_average(FL_FOREGROUND_COLOR, FL_BACKGROUND2_COLOR, 0.2));
        for (auto i = 0; i < chart::MAX_VLINES; i++) {
            if (_ver_pos[i] < 0) {
                break;
            }
            else {
                fl_line(_ver_pos[i], y() + (int) area.y, _ver_pos[i], y() + (int) (area.y + area.h - 1));
            }
        }
    }
}
void flw::Chart::_draw_xlabels() {
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
        if (*buffer2 != 0 && cw2 <= _tick_width) {
            fl_font(flw::PREF_FIXED_FONT, flw::PREF_FIXED_FONTSIZE - 2);
            fl_draw(buffer2, X1 - cw2, Y + fs05, cw4, flw::PREF_FIXED_FONTSIZE, FL_ALIGN_CENTER | FL_ALIGN_INSIDE);
        }
        if (addv == true && index < chart::MAX_VLINES) {
            _ver_pos[index++] = X1;
        }
        X1    += _tick_width;
        start += 1;
    }
    _ver_pos[index] = -1;
}
void flw::Chart::_draw_ylabels(const int X, double Y1, const double Y2, const chart::Scale& scale, const bool left) {
    const double yinc  = (scale.pixel * scale.tick);
    const int    fs05  = flw::PREF_FIXED_FONTSIZE * 0.5;
    const int    fr    = chart::count_decimals(scale.tick);
    int          width = w() - (_margin_left * flw::PREF_FIXED_FONTSIZE + _margin_right * flw::PREF_FIXED_FONTSIZE);
    double       ylast = chart::MAX_VAL;
    double       yval  = scale.min;
    fl_font(flw::PREF_FIXED_FONT, flw::PREF_FIXED_FONTSIZE);
    if (scale.min >= scale.max || scale.pixel * scale.tick < 1.0) {
        return;
    }
    while ((int) (Y1 + 0.5) >= (int) Y2) {
        if (ylast > Y1) {
            auto y1     = y() + (int) Y1;
            auto x1     = x() + X;
            auto string = chart::format_double(yval, fr, '\'');
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
int flw::Chart::handle(int event) {
    if (event == FL_PUSH) {
        if (Fl::event_button1() != 0) {
            _area = _inside_area(Fl::event_x(), Fl::event_y());
            _create_tooltip(Fl::event_ctrl());
            if (_tooltip != "") {
                return 1;
            }
        }
        else if (Fl::event_button3() != 0) {
            menu::set_item(_menu, chart::SHOW_LABELS, _view.labels);
            menu::set_item(_menu, chart::SHOW_HLINES, _view.horizontal);
            menu::set_item(_menu, chart::SHOW_VLINES, _view.vertical);
            _menu->popup();
            return 1;
        }
    }
    else if (event == FL_DRAG) {
        _area = _inside_area(Fl::event_x(), Fl::event_y());
        _create_tooltip(Fl::event_ctrl());
        if (_tooltip != "") {
            return 1;
        }
    }
    else if (event == FL_MOVE) {
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
            if (width >= chart::MIN_TICK && width <= chart::MAX_TICK) {
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
void flw::Chart::init(bool calc_dates) {
#ifdef DEBUG
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
#endif
}
flw::chart::Area* flw::Chart::_inside_area(int X, int Y) {
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
bool flw::Chart::margin(int left, int right) {
    if (left < 1 || left > 20 || right < 1 || right > 20) {
        return false;
    }
    _margin_left  = left;
    _margin_right = right;
    redraw();
    return true;
}
void flw::Chart::resize(int X, int Y, int W, int H) {
    if (_old_width != W || _old_height != H) {
        Fl_Group::resize(X, Y, W, H);
        _scroll->resize(X, Y + H - Fl::scrollbar_size(), W, Fl::scrollbar_size());
        _old_width = W;
        _old_height = H;
        init(false);
    }
}
bool flw::Chart::tick_width(int tick_width) {
    if (tick_width < 3 || tick_width > 100) {
        return false;
    }
    _tick_width = tick_width;
    redraw();
    return true;
}
void flw::Chart::update_pref() {
    _menu->textfont(flw::PREF_FONT);
    _menu->textsize(flw::PREF_FONTSIZE);
}
#include <string.h>
#include <time.h>
namespace flw {
    static int          _DATE_DAYS_MONTH[]      = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    static int          _DATE_DAYS_MONTH_LEAP[] = {0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    static const char*  _DATE_WEEKDAYS[]        = {"", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday", ""};
    static const char*  _DATE_WEEKDAYS_SHORT[]  = {"", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun", ""};
    static const char*  _DATE_MONTHS[]          = {"", "January", "February", "Mars", "April", "May", "June", "July", "August", "September", "October", "November", "December", ""};
    static const char*  _DATE_MONTHS_SHORT[]    = {"", "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec", ""};
    const int Date::SECS_PER_HOUR = 3600;
    const int Date::SECS_PER_DAY  = 3600 * 24;
    const int Date::SECS_PER_WEEK = 3600 * 24 * 7;
    static int _date_days(int year, int month) {
        if (year < 1 || year > 9999 || month < 1 || month > 12) {
            return 0;
        }
        else if ((year % 4 == 0 && year % 100 != 0) || year % 400 == 0) {
            return _DATE_DAYS_MONTH_LEAP[month];
        }
        return _DATE_DAYS_MONTH[month];
    }
    static void _date_from_time(int64_t seconds, bool utc, int& year, int& month, int& day, int& hour, int& min, int& sec) {
        year = month = day = 1;
        hour = min = sec = 0;
        if (seconds < 0) {
            return;
        }
        auto rawtime  = (time_t) seconds;
        auto timeinfo = utc ? gmtime(&rawtime) : localtime(&rawtime);
        if (timeinfo == nullptr) {
            return;
        }
        year  = timeinfo->tm_year + 1900;
        month = timeinfo->tm_mon + 1;
        day   = timeinfo->tm_mday;
        hour  = timeinfo->tm_hour;
        min   = timeinfo->tm_min;
        sec   = timeinfo->tm_sec;
    }
    static Date _date_parse(const char* string, bool us) {
        auto space = strstr(string, " ");
        int  tot   = strlen(string);
        int  tlen  = space ? (int) (space - string) : 0;
        int  len   = space ? tlen : tot;
        int  year  = 1;
        int  month = 1;
        int  day   = 1;
        int  hour  = 0;
        int  min   = 0;
        int  sec   = 0;
        bool iso   = false;
        int  Y     = 0;
        int  val[15];
        for (int f = 0; f < 15; f++) {
            val[f] = 0;
        }
        if (len == 10 && string[4] == '-' && string[7] == '-') {
            iso    = true;
            val[4] = string[5] - '0';
            val[5] = string[6] - '0';
            val[6] = string[8] - '0';
            val[7] = string[9] - '0';
        }
        else if (len == 8 && string[1] == '/' && string[3] == '/') {
            Y      = 4;
            val[4] = string[2] - '0';
            val[5] = -1;
            val[6] = string[0] - '0';
            val[7] = -1;
        }
        else if (len == 9 && string[1] == '/' && string[4] == '/') {
            Y      = 5;
            val[4] = string[2] - '0';
            val[5] = string[3] - '0';
            val[6] = string[0] - '0';
            val[7] = -1;
        }
        else if (len == 9 && string[2] == '/' && string[4]) {
            Y      = 5;
            val[4] = string[3] - '0';
            val[5] = -1;
            val[6] = string[0] - '0';
            val[7] = string[1] - '0';
        }
        else if (len == 10 && string[2] == '/' && string[5] == '/') {
            Y      = 6;
            val[4] = string[3] - '0';
            val[5] = string[4] - '0';
            val[6] = string[0] - '0';
            val[7] = string[1] - '0';
        }
        else if (len == 8) {
            iso    = true;
            val[4] = string[4] - '0';
            val[5] = string[5] - '0';
            val[6] = string[6] - '0';
            val[7] = string[7] - '0';
        }
        else {
            return Date(1, 1, 1);
        }
        val[0] = string[Y] - '0';
        val[1] = string[Y + 1] - '0';
        val[2] = string[Y + 2] - '0';
        val[3] = string[Y + 3] - '0';
        if (tlen && tot - tlen >= 9) {
            val[8]  = string[tlen + 1] - '0';
            val[9]  = string[tlen + 2] - '0';
            val[10] = string[tlen + 4] - '0';
            val[11] = string[tlen + 5] - '0';
            val[12] = string[tlen + 7] - '0';
            val[13] = string[tlen + 8] - '0';
        }
        else if (tlen && tot - tlen >= 7) {
            val[8]  = string[tlen + 1] - '0';
            val[9]  = string[tlen + 2] - '0';
            val[10] = string[tlen + 3] - '0';
            val[11] = string[tlen + 4] - '0';
            val[12] = string[tlen + 5] - '0';
            val[13] = string[tlen + 6] - '0';
        }
        for (int f = 0; f < 15; f++) {
            if ((f == 5 || f == 7) && val[f] == -1) {
                ;
            }
            else if (val[f] < 0 || val[f] > 9) {
                return Date(1, 1, 1);
            }
        }
        year  = val[0] * 1000 + val[1] * 100 + val[2] * 10 + val[3];
        month = val[5] == -1 ? val[4] : val[4] * 10 + val[5];
        day   = val[7] == -1 ? val[6] : val[6] * 10 + val[7];
        hour  = val[8] * 10 + val[9];
        min   = val[10] * 10 + val[11];
        sec   = val[12] * 10 + val[13];
        if (iso == false && us) {
            int tmp = month;
            month   = day;
            day     = tmp;
        }
        return Date(year, month, day, hour, min, sec);
    }
    static flw::Date::DAY _date_weekday(int year, int month, int day) {
        if (year > 0 && year < 10000 && month > 0 && month < 13 && day > 0 && day <= _date_days(year, month)) {
            int start = 0;
            int y1    = year - 1;
            int pre   = ((year < 1582) || ((year == 1582) && (month <= 10)));
            if (pre) {
                start = 6 + y1 + (y1 / 4);
            }
            else {
                start = 1 + y1 + (y1 / 4) - (y1 / 100) + (y1 / 400);
            }
            for (int i = 1; i < month; i++) {
                int days = _date_days(year, i);
                if (days) {
                    start += days;
                }
                else {
                    return flw::Date::DAY::INVALID;
                }
            }
            start = start % 7;
            start = start == 0 ? 7 : start;
            for (int i = 2; i <= day; i++) {
                start++;
                if (start > 7) {
                    start = 1;
                }
            }
            if (start < 1 || start > 7) {
                return flw::Date::DAY::INVALID;
            }
            else {
                return (flw::Date::DAY) start;
            }
        }
        return flw::Date::DAY::INVALID;
    }
    static bool _date_is_leapyear(int year) {
        if (year < 1 || year > 9999) {
            return false;
        }
        else if ((year % 4 == 0 && year % 100 != 0) || year % 400 == 0) {
            return true;
        }
        else {
           return false;
        }
    }
}
flw::Date::Date(bool utc) {
    int year, month, day, hour, min, sec;
    _date_from_time(::time(nullptr), utc, year, month, day, hour, min, sec);
    set(year, month, day, hour, min, sec);
}
flw::Date::Date(const Date& other) {
    set(other);
}
flw::Date::Date(Date&& other) {
    set(other);
}
flw::Date::Date(int year, int month, int day, int hour, int min, int sec) {
    _year = _month = _day = 1;
    _hour = _min = _sec = 0;
    set(year, month, day, hour, min, sec);
}
flw::Date& flw::Date::operator=(const Date& date) {
    set(date);
    return *this;
}
flw::Date& flw::Date::operator=(Date&& date) {
    set(date);
    return *this;
}
bool flw::Date::add_days(const int days) {
    if (days) {
        int daym = _date_days(_year, _month);
        if (daym > 0) {
            const int inc   = days > 0 ? 1 : -1;
            int       year  = _year;
            int       month = _month;
            int       day   = _day;
            for (int f = 0; f < abs(days); f++) {
                day += inc;
                if (inc < 0 && day == 0) {
                    month--;
                    if (month == 0) {
                        month = 12;
                        year--;
                        if (year < 1) {
                            return false;
                        }
                    }
                    day = _date_days(year, month);
                    if (day == 0) {
                        return false;
                    }
                }
                else if (inc > 0 && day > daym) {
                    day = 1;
                    month++;
                    if (month == 13) {
                        month = 1;
                        year++;
                        if (year > 9999) {
                            return false;
                        }
                    }
                    daym = _date_days(year, month);
                    if (daym == 0) {
                        return false;
                    }
                }
            }
            _year  = year;
            _month = month;
            _day   = day;
            return true;
        }
    }
    return false;
}
bool flw::Date::add_months(const int months) {
    if (months) {
        const int inc   = months > 0 ? 1 : -1;
        int       month = _month;
        int       year  = _year;
        for (int f = 0; f < abs(months); f++) {
            month += inc;
            if (month == 0) {
                month = 12;
                year--;
                if (year < 1) {
                    return false;
                }
            }
            else if (month > 12) {
                month = 1;
                year++;
                if (year > 9999) {
                    return false;
                }
            }
        }
        const int days = _date_days(year, month);
        if (days > 0) {
            _year  = year;
            _month = month;
            if (_day > days) {
                _day = days;
            }
            return true;
        }
    }
    return false;
}
bool flw::Date::add_seconds(const int64_t seconds) {
    if (seconds) {
        const int inc  = seconds > 0 ? 1 : -1;
        int       hour = _hour;
        int       min  = _min;
        int       sec  = _sec;
        for (int64_t f = 0; f < llabs(seconds); f++) {
            sec += inc;
            if (inc < 0 && sec == -1) {
                min--;
                if (min == -1) {
                    min = 59;
                    hour--;
                    if (hour == -1) {
                        hour = 23;
                        if (add_days(-1) == false) {
                            return false;
                        }
                    }
                }
                sec = 59;
            }
            else if (inc > 0 && sec == 60) {
                min++;
                if (min == 60) {
                    min = 0;
                    hour++;
                    if (hour == 24) {
                        hour = 0;
                        if (add_days(1) == false) {
                            return false;
                        }
                    }
                }
                sec = 0;
            }
        }
        _hour = hour;
        _min  = min;
        _sec  = sec;
        return true;
    }
    return false;
}
int flw::Date::compare(const Date& other, flw::Date::COMPARE flag) const {
    if (_year < other._year) {
        return -1;
    }
    else if (_year > other._year) {
        return 1;
    }
    if (_month < other._month) {
        return -1;
    }
    else if (_month > other._month) {
        return 1;
    }
    if (flag >= flw::Date::COMPARE::YYYYMMDD) {
        if (_day < other._day) {
            return -1;
        }
        else if (_day > other._day) {
            return 1;
        }
    }
    if (flag >= flw::Date::COMPARE::YYYYMMDDHH) {
        if (_hour < other._hour) {
            return -1;
        }
        else if (_hour > other._hour) {
            return 1;
        }
    }
    if (flag >= flw::Date::COMPARE::YYYYMMDDHHMM) {
        if (_min < other._min) {
            return -1;
        }
        else if (_min > other._min) {
            return 1;
        }
    }
    if (flag >= flw::Date::COMPARE::YYYYMMDDHHMMSS) {
        if (_sec < other._sec) {
            return -1;
        }
        else if (_sec > other._sec) {
            return 1;
        }
    }
    return 0;
}
int flw::Date::Compare(const void* a, const void* b) {
    Date* A = *(Date**) a;
    Date* B = *(Date**) b;
    return A->compare(*B);
}
bool flw::Date::Compare(const Date& a, const Date& b) {
    return a.compare(b) < 0;
}
flw::Date& flw::Date::day(int day) {
    if (day > 0 && day <= _date_days(_year, _month)) {
        _day = day;
    }
    return *this;
}
flw::Date& flw::Date::day_last() {
    _day = month_days();
    return *this;
}
void flw::Date::Del(void* self) {
    if (self) {
        delete (Date*) self;
    }
}
int flw::Date::diff_days(const Date& date) const {
    Date d(date);
    int  res = 0;
    if (compare(d, flw::Date::COMPARE::YYYYMMDD) < 0) {
        while (compare(d, flw::Date::COMPARE::YYYYMMDD) != 0) {
            d.add_days(-1);
            res++;
        }
    }
    else if (compare(d, flw::Date::COMPARE::YYYYMMDD) > 0) {
        while (compare(d, flw::Date::COMPARE::YYYYMMDD) != 0) {
            d.add_days(1);
            res--;
        }
    }
    return res;
}
int flw::Date::diff_months(const Date& date) const {
    Date d(date);
    int  res = 0;
    if (compare(d, flw::Date::COMPARE::YYYYMM) < 0) {
        while (compare(d, flw::Date::COMPARE::YYYYMM)) {
            d.add_months(-1);
            res++;
        }
    }
    else if (compare(d, flw::Date::COMPARE::YYYYMM) > 0) {
        while (compare(d, flw::Date::COMPARE::YYYYMM)) {
            d.add_months(1);
            res--;
        }
    }
    return res;
}
int flw::Date::diff_seconds(const Date& date) const {
    int64_t unix1 = time();
    int64_t unix2 = date.time();
    if (unix1 >= 0 && unix2 >= 0) {
        return unix2 - unix1;
    }
    return 0;
}
std::string flw::Date::format(flw::Date::FORMAT format) const {
    char tmp[100];
    int  n = 0;
    if (format == flw::Date::FORMAT::ISO_LONG) {
        n = snprintf(tmp, 100, "%04d-%02d-%02d", _year, _month, _day);
    }
    else if (format == flw::Date::FORMAT::US) {
        n = snprintf(tmp, 100, "%d/%d/%04d", _month, _day, _year);
    }
    else if (format == flw::Date::FORMAT::WORLD) {
        n = snprintf(tmp, 100, "%d/%d/%04d", _day, _month, _year);
    }
    else if (format == flw::Date::FORMAT::NAME) {
        n = snprintf(tmp, 100, "%04d %s %d", _year, month_name_short(), _day);
    }
    else if (format == flw::Date::FORMAT::NAME_LONG) {
        n = snprintf(tmp, 100, "%04d %s %d", _year, month_name(), _day);
    }
    else if (format == flw::Date::FORMAT::YEAR_MONTH) {
        n = snprintf(tmp, 100, "%04d %s", _year, month_name_short());
    }
    else if (format == flw::Date::FORMAT::YEAR_MONTH_LONG) {
        n = snprintf(tmp, 100, "%04d %s", _year, month_name());
    }
    else if (format == flw::Date::FORMAT::ISO_TIME) {
        n = snprintf(tmp, 100, "%04d%02d%02d %02d%02d%02d", _year, _month, _day, _hour, _min, _sec);
    }
    else if (format == flw::Date::FORMAT::ISO_TIME_LONG) {
        n = snprintf(tmp, 100, "%04d-%02d-%02d %02d:%02d:%02d", _year, _month, _day, _hour, _min, _sec);
    }
    else {
        n = snprintf(tmp, 100, "%04d%02d%02d", _year, _month, _day);
    }
    if (n < 0 || n >= 100) {
        *tmp = 0;
    }
    return tmp;
}
flw::Date flw::Date::FromString(const char* buffer, bool us) {
    if (buffer == nullptr) {
        return Date(1, 1, 1);
    }
    else {
         return _date_parse(buffer, us);
    }
}
flw::Date flw::Date::FromTime(int64_t seconds, bool utc) {
    int year, month, day, hour, min, sec;
    _date_from_time(seconds, utc, year, month, day, hour, min, sec);
    return Date(year, month, day, hour, min, sec);
}
flw::Date& flw::Date::hour(int hour) {
    if (hour >= 0 && hour <= 23) {
        _hour = hour;
    }
    return *this;
}
bool flw::Date::is_leapyear() const {
    return _date_is_leapyear(_year);
}
flw::Date& flw::Date::minute(int min) {
    if (min >= 0 && min <= 59) {
        _min = min;
    }
    return *this;
}
flw::Date& flw::Date::month(int month) {
    if (month >= 1 && month <= 12) {
        _month = month;
    }
    return *this;
}
int flw::Date::month_days() const {
    return _date_days(_year, _month);
}
const char* flw::Date::month_name() const {
    return _DATE_MONTHS[(int) _month];
}
const char* flw::Date::month_name_short() const {
    return _DATE_MONTHS_SHORT[(int) _month];
}
void flw::Date::print() const {
    auto string = format(flw::Date::FORMAT::ISO_TIME_LONG);
    printf("Date| %s\n", string.c_str());
    fflush(stdout);
}
flw::Date& flw::Date::second(int sec) {
    if (sec >= 0 && sec <= 59) {
        _sec = sec;
    }
    return *this;
}
flw::Date& flw::Date::set(const Date& date) {
    _year  = date._year;
    _month = date._month;
    _day   = date._day;
    _hour  = date._hour;
    _min   = date._min;
    _sec   = date._sec;
    return *this;
}
flw::Date& flw::Date::set(int year, int month, int day, int hour, int min, int sec) {
    if (year < 1 || year > 9999 ||
        month < 1 || month > 12 ||
        day < 1 || day > _date_days(year, month) ||
        hour < 0 || hour > 23 ||
        min < 0 || min > 59 ||
        sec < 0 || sec > 59) {
        return *this;
    }
    else {
        _year  = year;
        _month = month;
        _day   = day;
        _hour  = hour;
        _min   = min;
        _sec   = sec;
        return *this;
    }
}
int64_t flw::Date::time() const {
    tm t;
    if (_year < 1970) {
        return -1;
    }
    memset(&t, '\0', sizeof(t));
    t.tm_year = _year - 1900;
    t.tm_mon  = _month - 1;
    t.tm_mday = _day;
    t.tm_hour = _hour;
    t.tm_min  = _min;
    t.tm_sec  = _sec;
    return mktime(&t);
}
int flw::Date::week() const {
    flw::Date::DAY wday  = _date_weekday(_year, _month, _day);
    flw::Date::DAY wday1 = _date_weekday(_year, 1, 1);
    if (wday != flw::Date::DAY::INVALID && wday1 != flw::Date::DAY::INVALID) {
        auto week  = 0;
        auto y1    = _year - 1;
        auto leap  = _date_is_leapyear(_year);
        auto leap1 = _date_is_leapyear(y1);
        auto yday  = yearday();
        if (yday <= (8 - (int) wday1) && wday1 > flw::Date::DAY::THURSDAY) {
            if (wday1 == flw::Date::DAY::FRIDAY || (wday1 == flw::Date::DAY::SATURDAY && leap1)) {
                week = 53;
            }
            else {
                week = 52;
            }
        }
        else {
            auto days = leap ? 366 : 365;
            if ((days - yday) < (4 - (int) wday)) {
                week = 1;
            }
            else {
                days = yday + (7 - (int) wday) + ((int) wday1 - 1);
                days = days / 7;
                if (wday1 > flw::Date::DAY::THURSDAY) {
                    days--;
                }
                week = days;
            }
        }
        if (week > 0 && week < 54) {
            return week;
        }
    }
    return 0;
}
flw::Date::DAY flw::Date::weekday() const {
    return _date_weekday(_year, _month, _day);
}
flw::Date& flw::Date::weekday(flw::Date::DAY day) {
    if (weekday() < day) {
        while (weekday() < day) {
            add_days(1);
        }
    }
    else if (weekday() > day) {
        while (weekday() > day) {
            add_days(-1);
        }
    }
    return *this;
}
const char* flw::Date::weekday_name() const {
    return _DATE_WEEKDAYS[(int) _date_weekday(_year, _month, _day)];
}
const char* flw::Date::weekday_name_short() const {
    return _DATE_WEEKDAYS_SHORT[(int) _date_weekday(_year, _month, _day)];
}
flw::Date& flw::Date::year(int year) {
    if (year >= 1 && year <= 9999) {
        _year = year;
    }
    return *this;
}
int flw::Date::yearday() const {
    auto res  = 0;
    auto leap = _date_is_leapyear(_year);
    for (auto m = 1; m < _month && m < 13; m++) {
        if (leap) {
            res += _DATE_DAYS_MONTH_LEAP[m];
        }
        else {
            res += _DATE_DAYS_MONTH[m];
        }
    }
    return res + _day;
}
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Repeat_Button.H>
#include <FL/Fl_Return_Button.H>
#include <FL/fl_draw.H>
#include <time.h>
namespace flw {
class _DateChooserCanvas : public Fl_Widget {
    Date                    _date[7][8];
    char                    _text[7][8][30];
    int                     _col;
    int                     _row;
public:
    _DateChooserCanvas() : Fl_Widget(0, 0, 0, 0, 0) {
        _row   = 1;
        _col   = 1;
        strncpy(_text[0][0], "Week", 20);
        strncpy(_text[0][1], "Mon", 20);
        strncpy(_text[0][2], "Tue", 20);
        strncpy(_text[0][3], "Wed", 20);
        strncpy(_text[0][4], "Thu", 20);
        strncpy(_text[0][5], "Fri", 20);
        strncpy(_text[0][6], "Sat", 20);
        strncpy(_text[0][7], "Sun", 20);
    }
    void draw() override {
        int cw  = w() / 8;
        int ch  = h() / 7;
        int d   = 99;
        int col = 0;
        fl_rectf(x(), y(), w(), h(), FL_BACKGROUND_COLOR);
        for (int r = 0; r < 7; r++) {
            for (int c  = 0; c < 8; c++) {
                auto t  = _text[r][c];
                auto x1 = x() + (c * cw) + 1;
                auto y1 = y() + (r * ch) + 1;
                auto w1 = (c == 7) ? x() + w() - x1 : cw - 2;
                auto h1 = (r == 6) ? y() + h() - y1 : ch - 2;
                auto bg = FL_BACKGROUND_COLOR;
                auto fg = FL_FOREGROUND_COLOR;
                if (r == 0 || c == 0) {
                    ;
                }
                else {
                    int v = atoi(t);
                    if (v <= d && col < 3) {
                        d = v;
                        col++;
                    }
                    if (r == _row && c == _col) {
                        bg = FL_SELECTION_COLOR;
                    }
                    else if (col == 2) {
                        bg = FL_BACKGROUND2_COLOR;
                    }
                    else {
                        bg = FL_DARK2;
                    }
                }
                fl_rectf(x1, y1, w1, h1, bg);
                fl_color(fg);
                fl_font(r == 0 || c == 0 ? FL_HELVETICA_BOLD : FL_HELVETICA, flw::PREF_FONTSIZE);
                fl_draw(t, x1, y1, w1, h1, FL_ALIGN_CENTER | FL_ALIGN_INSIDE);
                fl_rect(x1, y1, w1, h1, FL_DARK3);
            }
        }
    }
    Date& get() {
        return _date[_row][_col];
    }
    int handle(int event) override {
        if (event == FL_PUSH) {
            take_focus();
            auto cw = (int) (w() / 8);
            auto ch = (int) (h() / 7);
            for (auto r = 1; r < 7; r++) {
                for (auto c = 1; c < 8; c++) {
                    auto x1 = (int) (x() + (c * cw));
                    auto y1 = (int) (y() + (r * ch));
                    if (Fl::event_x() >= x1 && Fl::event_x() < x1 + cw && Fl::event_y() >= y1 && Fl::event_y() < y1 + ch) {
                        _row = r;
                        _col = c;
                        do_callback();
                        redraw();
                        break;
                    }
                }
            }
            return 1;
        }
        else if (event == FL_FOCUS) {
            return 1;
        }
        else if (event == FL_KEYDOWN) {
            if (Fl::event_command() == 0) {
                int num = Fl::event_key();
                if (num == FL_Up) {
                    if (_row > 1) {
                        _row--;
                        redraw();
                        do_callback();
                    }
                    return 1;
                }
                else if (num == FL_Down) {
                    if (_row < 6) {
                        _row++;
                        redraw();
                        do_callback();
                    }
                    return 1;
                }
                else if (num == FL_Left) {
                    if (_col > 1) {
                        _col--;
                        redraw();
                        do_callback();
                    }
                    return 1;
                }
                else if (num == FL_Right) {
                    if (_col < 7) {
                        _col++;
                        redraw();
                        do_callback();
                    }
                    return 1;
                }
            }
        }
        return Fl_Widget::handle(event);
    }
    void set_current(int row, int col) {
        if (row != _row || col != _col) {
            _row = row;
            _col = col;
            do_callback();
        }
    }
    void set_date(int row, int col, const Date& date) {
        _date[row][col] = date;
    }
    void set_text(int row, int col, const char* text) {
        strncpy(_text[row][col], text, 30);
    }
};
flw::DateChooser::DateChooser(int X, int Y, int W, int H, const char* l) : Fl_Group(X, Y, W, H, l) {
    end();
    _h           = -1;
    _w           = -1;
    _month_label = new Fl_Box(0, 0, 0, 0, "");
    _canvas      = new _DateChooserCanvas();
    _b6          = new Fl_Repeat_Button(0, 0, 0, 0, "@|<");
    _b1          = new Fl_Repeat_Button(0, 0, 0, 0, "@<<");
    _b2          = new Fl_Repeat_Button(0, 0, 0, 0, "@<");
    _b5          = new Fl_Button(0, 0, 0, 0, "@refresh");
    _b3          = new Fl_Repeat_Button(0, 0, 0, 0, "@>");
    _b4          = new Fl_Repeat_Button(0, 0, 0, 0, "@>>");
    _b7          = new Fl_Repeat_Button(0, 0, 0, 0, "@>|");
    Date date;
    set(date);
    _set_label();
    add(_b1);
    add(_b2);
    add(_b5);
    add(_b3);
    add(_b4);
    add(_b6);
    add(_b7);
    add(_month_label);
    add(_canvas);
    _b1->callback(flw::DateChooser::_Callback, this);
    _b1->tooltip("Previous year");
    _b2->callback(flw::DateChooser::_Callback, this);
    _b2->tooltip("Previous month");
    _b3->callback(flw::DateChooser::_Callback, this);
    _b3->tooltip("Next month");
    _b4->callback(flw::DateChooser::_Callback, this);
    _b4->tooltip("Next year");
    _b5->callback(flw::DateChooser::_Callback, this);
    _b5->tooltip("Today");
    _b6->callback(flw::DateChooser::_Callback, this);
    _b6->tooltip("10 years in the past");
    _b7->callback(flw::DateChooser::_Callback, this);
    _b7->tooltip("10 years in the future");
    _canvas->callback(flw::DateChooser::_Callback, this);
    _month_label->box(FL_UP_BOX);
    util::labelfont(this);
    tooltip("Use arrow keys to navigate\nUse ctrl+left/right to change month");
    flw::DateChooser::resize(X, Y, W, H);
}
void flw::DateChooser::_Callback(Fl_Widget* w, void* o) {
    auto dc = (DateChooser*) o;
    auto dt = dc->get();
    if (w == dc->_b6) {
        dt.add_months(-120);
        dc->set(dt);
    }
    else if (w == dc->_b1) {
        dt.add_months(-12);
        dc->set(dt);
    }
    else if (w == dc->_b2) {
        dt.add_months(-1);
        dc->set(dt);
    }
    else if (w == dc->_b5) {
        dt = Date::FromTime(::time(0));
        dc->set(dt);
    }
    else if (w == dc->_b3) {
        dt.add_months(1);
        dc->set(dt);
    }
    else if (w == dc->_b4) {
        dt.add_months(12);
        dc->set(dt);
    }
    else if (w == dc->_b7) {
        dt.add_months(120);
        dc->set(dt);
    }
    else if (w == dc->_canvas) {
        dc->_set_label();
        dc->do_callback();
    }
    dc->_canvas->take_focus();
}
void flw::DateChooser::draw() {
    _month_label->labelfont(FL_HELVETICA_BOLD);
    Fl_Group::draw();
}
void flw::DateChooser::focus() {
    _canvas->take_focus();
}
flw::Date flw::DateChooser::get() const {
    auto canvas = (flw::_DateChooserCanvas*) _canvas;
    return canvas->get();
}
int flw::DateChooser::handle(int event) {
    if (event == FL_KEYDOWN) {
        if (Fl::event_command()) {
            Date dt = get();
            if (Fl::event_key() == FL_Left) {
                dt.add_months(-1);
                set(dt);
                return 1;
            }
            else if (Fl::event_key() == FL_Right) {
                dt.add_months(+1);
                set(dt);
                return 1;
            }
        }
    }
    return Fl_Group::handle(event);
}
void flw::DateChooser::resize(int X, int Y, int W, int H) {
    Fl_Widget::resize(X, Y, W, H);
    if (W == _w && H == _h) {
        return;
    }
    int w1 = W / 8;
    int w2 = W - (w1 * 6);
    int fs = flw::PREF_FONTSIZE;
    _b6->resize(X, Y, w1, fs * 2);
    _b1->resize(X + w1, Y, w1, fs * 2);
    _b2->resize(X + w1 * 2, Y, w1, fs * 2);
    _b3->resize(X + w1 * 3 + w2, Y, w1, fs * 2);
    _b4->resize(X + w1 * 4 + w2, Y, w1, fs * 2);
    _b7->resize(X + w1 * 5 + w2, Y, w1, fs * 2);
    _b5->resize(X + w1 * 3, Y, w2, fs * 2);
    _month_label->resize(X, Y + fs * 2, W, fs * 2);
    _canvas->resize(X, Y + fs * 4, W, H - fs * 4);
    _w = W;
    _h = H;
}
void flw::DateChooser::set(const Date& date) {
    auto canvas = (flw::_DateChooserCanvas*) _canvas;
    auto date2  = date;
    if (date2.year() < 2 && date2.month() < 2) {
        date2.month(2);
    }
    auto start_cell   = 0;
    auto first_date   = Date(date2.year(), date2.month(), 1);
    auto current_date = Date();
    char tmp[30];
    start_cell    = (int) first_date.weekday() - 1;
    start_cell    = start_cell + first_date.month_days() < 32 ? start_cell + 7 : start_cell;
    current_date  = first_date;
    current_date.add_days(-start_cell);
    for (auto r = 1; r <= 6; r++) {
        snprintf(tmp, 30, "%02d", current_date.week());
        canvas->set_text(r, 0, tmp);
        for (auto c = 1; c <= 7; c++) {
            snprintf(tmp, 30, "%02d", current_date.day());
            canvas->set_text(r, c, tmp);
            canvas->set_date(r, c, current_date);
            if (current_date.month() == date2.month() && current_date.day() == date2.day()) {
                canvas->set_current(r, c);
            }
            current_date.add_days(1);
        }
    }
    _set_label();
    redraw();
}
void flw::DateChooser::_set_label() {
    auto canvas = (flw::_DateChooserCanvas*) _canvas;
    auto date   = canvas->get();
    auto string = date.format(Date::FORMAT::YEAR_MONTH_LONG);
    _month_label->copy_label(string.c_str());
}
class _DateChooserDlg : public Fl_Double_Window {
    DateChooser*            _date_chooser;
    Date&                   _value;
    Fl_Button*              _cancel;
    Fl_Button*              _ok;
    bool                    _res;
public:
    _DateChooserDlg(const char* title, Date& date) : Fl_Double_Window(0, 0, 0, 0), _value(date) {
        end();
        _date_chooser = new DateChooser();
        _ok           = new Fl_Return_Button(0, 0, 0, 0, "&Ok");
        _cancel       = new Fl_Button(0, 0, 0, 0, "&Cancel");
        _res          = false;
        add(_date_chooser);
        add(_ok);
        add(_cancel);
        _cancel->callback(Callback, this);
        _date_chooser->focus();
        _date_chooser->set(_value);
        _ok->callback(Callback, this);
        util::labelfont(this);
        callback(Callback, this);
        copy_label(title);
        size(flw::PREF_FONTSIZE * 33, flw::PREF_FONTSIZE * 21);
        size_range(flw::PREF_FONTSIZE * 22, flw::PREF_FONTSIZE * 14);
        set_modal();
        resizable(this);
    }
    static void Callback(Fl_Widget* w, void* o) {
        _DateChooserDlg* dlg = (_DateChooserDlg*) o;
        if (w == dlg) {
            ;
        }
        else if (w == dlg->_cancel) {
            dlg->hide();
        }
        else if (w == dlg->_ok) {
            dlg->hide();
            dlg->_res = true;
        }
    }
    void resize(int X, int Y, int W, int H) override {
        auto fs = flw::PREF_FONTSIZE;
        Fl_Double_Window::resize(X, Y, W, H);
        _date_chooser->resize (4,                 4,                  W - 8,    H - fs * 2 - 16);
        _cancel->resize       (W - fs * 16 - 8,   H - fs * 2 - 4,     fs * 8,   fs * 2);
        _ok->resize           (W - fs * 8 - 4,    H - fs * 2 - 4,     fs * 8,   fs * 2);
    }
    bool run(Fl_Window* parent) {
        flw::util::center_window(this, parent);
        show();
        while (visible() != 0) {
            Fl::wait();
            Fl::flush();
        }
        if (_res == true) {
            _value = _date_chooser->get();
        }
        return _res;
    }
};
bool dlg::date(const std::string& title, flw::Date& date, Fl_Window* parent) {
    flw::_DateChooserDlg dlg(title.c_str(), date);
    return dlg.run(parent);
}
}
#ifdef _WIN32
    #include <windows.h>
#else
#endif
#include <math.h>
#include <FL/Fl_File_Chooser.H>
#include <FL/Fl_Help_View.H>
#include <FL/Fl_Secret_Input.H>
#include <FL/Fl_Output.H>
#include <FL/Fl_Return_Button.H>
#include <FL/Fl_Text_Editor.H>
namespace flw {
    namespace theme {
        void _load_default();
        void _load_oxy();
        void _load_oxy_blue();
        void _load_oxy_tan();
        void _load_gleam();
        void _load_gleam_blue();
        void _load_gleam_dark();
        void _load_gleam_darker();
        void _load_gleam_dark_blue();
        void _load_gleam_tan();
        void _load_gtk();
        void _load_gtk_blue();
        void _load_gtk_dark();
        void _load_gtk_darker();
        void _load_gtk_dark_blue();
        void _load_gtk_tan();
        void _load_plastic();
        void _load_blue_plastic();
        void _load_tan_plastic();
        void _load_system();
        void _scrollbar();
    }
    namespace dlg {
        void* zero_memory(void* mem, size_t size) {
            if (mem == nullptr || size == 0) return mem;
        #ifdef _WIN32
            RtlSecureZeroMemory(mem, size);
        #else
            auto p = (volatile unsigned char*) mem;
            while (size--) {
                *p = 0;
                p++;
            }
        #endif
            return mem;
        }
        class _DlgHtml  : public Fl_Double_Window {
            Fl_Help_View*               _html;
            Fl_Return_Button*           _close;
        public:
            _DlgHtml(const char* title, const char* text, Fl_Window* parent, int W, int H) :
            Fl_Double_Window(0, 0, flw::PREF_FONTSIZE * W,flw::PREF_FONTSIZE * H) {
                end();
                _close = new Fl_Return_Button(0, 0, 0, 0, "&Close");
                _html  = new Fl_Help_View(0, 0, 0, 0);
                add(_close);
                add(_html);
                _close->callback(_DlgHtml::Callback, this);
                _close->labelfont(flw::PREF_FONT);
                _close->labelsize(flw::PREF_FONTSIZE);
                _html->textfont(flw::PREF_FONT);
                _html->textsize(flw::PREF_FONTSIZE);
                _html->value(text);
                callback(_DlgHtml::Callback, this);
                copy_label(title);
                size_range(flw::PREF_FONTSIZE * 24, flw::PREF_FONTSIZE * 12);
                set_modal();
                resizable(this);
                flw::util::center_window(this, parent);
            }
            static void Callback(Fl_Widget* w, void* o) {
                auto dlg = (_DlgHtml*) o;
                if (w == dlg || w == dlg->_close) {
                    dlg->hide();
                }
            }
            void resize(int X, int Y, int W, int H) override {
                Fl_Double_Window::resize(X, Y, W, H);
                _html->resize(4, 4, W - 8, H - flw::PREF_FONTSIZE * 2 - 16);
                _close->resize(W - flw::PREF_FONTSIZE * 8 - 4, H - flw::PREF_FONTSIZE * 2 - 4, flw::PREF_FONTSIZE * 8, flw::PREF_FONTSIZE * 2);
            }
            void run() {
                show();
                while (visible() != 0) {
                    Fl::wait();
                    Fl::flush();
                }
            }
        };
        class _DlgList : public Fl_Double_Window {
            flw::ScrollBrowser*         _list;
            Fl_Return_Button*           _close;
        public:
            _DlgList(const char* title, const StringVector& list, std::string file, Fl_Window* parent = nullptr, bool fixed_font = false, int W = 50, int H = 20) :
            Fl_Double_Window(0, 0, (fixed_font ? flw::PREF_FIXED_FONTSIZE : flw::PREF_FONTSIZE) * W, (fixed_font ? flw::PREF_FIXED_FONTSIZE : flw::PREF_FONTSIZE) * H) {
                end();
                _close = new Fl_Return_Button(0, 0, 0, 0, "&Close");
                _list  = new flw::ScrollBrowser();
                add(_close);
                add(_list);
                _close->callback(_DlgList::Callback, this);
                _close->labelfont(flw::PREF_FONT);
                _close->labelsize(flw::PREF_FONTSIZE);
                _list->take_focus();
                if (fixed_font == true) {
                    _list->textfont(flw::PREF_FIXED_FONT);
                    _list->textsize(flw::PREF_FIXED_FONTSIZE);
                }
                else {
                    _list->textfont(flw::PREF_FONT);
                    _list->textsize(flw::PREF_FONTSIZE);
                }
                callback(_DlgList::Callback, this);
                copy_label(title);
                size_range(flw::PREF_FONTSIZE * 24, flw::PREF_FONTSIZE * 12);
                set_modal();
                resizable(this);
                flw::util::center_window(this, parent);
                if (list.size() > 0) {
                    for (auto& s : list) {
                        _list->add(s.c_str());
                    }
                }
                else if (file != "") {
                    _list->load(file.c_str());
                }
            }
            static void Callback(Fl_Widget* w, void* o) {
                auto dlg = (_DlgList*) o;
                if (w == dlg || w == dlg->_close) {
                    dlg->hide();
                }
            }
            void resize(int X, int Y, int W, int H) override {
                Fl_Double_Window::resize(X, Y, W, H);
                _list->resize(4, 4, W - 8, H - flw::PREF_FONTSIZE * 2 - 16);
                _close->resize(W - flw::PREF_FONTSIZE * 8 - 4, H - flw::PREF_FONTSIZE * 2 - 4, flw::PREF_FONTSIZE * 8, flw::PREF_FONTSIZE * 2);
            }
            void run() {
                show();
                while (visible() != 0) {
                    Fl::wait();
                    Fl::flush();
                }
            }
        };
        class _DlgSelect : public Fl_Double_Window {
            Fl_Button*                  _close;
            Fl_Button*                  _cancel;
            ScrollBrowser*              _list;
            Fl_Input*                   _filter;
            const StringVector&         _strings;
        public:
            _DlgSelect(const char* title, Fl_Window* parent, const StringVector& strings, int selected_string_index, std::string selected_string, bool fixed_font, int W, int H) :
            Fl_Double_Window(0, 0, ((fixed_font == true) ? flw::PREF_FIXED_FONTSIZE : flw::PREF_FONTSIZE) * W, ((fixed_font == true) ? flw::PREF_FIXED_FONTSIZE : flw::PREF_FONTSIZE) * H),
            _strings(strings) {
                end();
                _filter = new Fl_Input(0, 0, 0, 0);
                _close  = new Fl_Return_Button(0, 0, 0, 0, "&Select");
                _cancel = new Fl_Button(0, 0, 0, 0, "&Cancel");
                _list   = new flw::ScrollBrowser(0, 0, 0, 0);
                add(_filter);
                add(_list);
                add(_cancel);
                add(_close);
                _cancel->callback(_DlgSelect::Callback, this);
                _close->callback(_DlgSelect::Callback, this);
                _filter->callback(_DlgSelect::Callback, this);
                _filter->tooltip("Enter text to filter rows that macthes the text\nPress tab to switch focus between input and list widget.");
                _filter->when(FL_WHEN_CHANGED);
                _list->callback(_DlgSelect::Callback, this);
                _list->tooltip("Use Page Up or Page Down in list to scroll faster");
                if (fixed_font == true) {
                    _filter->textfont(flw::PREF_FIXED_FONT);
                    _filter->textsize(flw::PREF_FIXED_FONTSIZE);
                    _list->textfont(flw::PREF_FIXED_FONT);
                    _list->textsize(flw::PREF_FIXED_FONTSIZE);
                }
                else {
                    _filter->textfont(flw::PREF_FONT);
                    _filter->textsize(flw::PREF_FONTSIZE);
                    _list->textfont(flw::PREF_FONT);
                    _list->textsize(flw::PREF_FONTSIZE);
                }
                {
                    auto r = 0;
                    auto f = 0;
                    for (auto& string : _strings) {
                        _list->add(string.c_str());
                        if (string == selected_string) {
                            r = f + 1;
                        }
                        f++;
                    }
                    if (selected_string_index > 0 && selected_string_index <= (int) _strings.size()) {
                        _list->value(selected_string_index);
                        _list->middleline(selected_string_index);
                    }
                    else if (r > 0) {
                        _list->value(r);
                        _list->middleline(r);
                    }
                    else {
                        _list->value(1);
                    }
                }
                _filter->take_focus();
                util::labelfont(this);
                callback(_DlgSelect::Callback, this);
                copy_label(title);
                activate_button();
                size_range(flw::PREF_FONTSIZE * 24, flw::PREF_FONTSIZE * 12);
                set_modal();
                resizable(this);
                flw::util::center_window(this, parent);
            }
            void activate_button() {
                if (_list->value() == 0) {
                    _close->deactivate();
                }
                else {
                    _close->activate();
                }
            }
            static void Callback(Fl_Widget* w, void* o) {
                auto dlg = (_DlgSelect*) o;
                if (w == dlg || w == dlg->_cancel) {
                    dlg->_list->deselect();
                    dlg->hide();
                }
                else if (w == dlg->_filter) {
                    dlg->filter(dlg->_filter->value());
                    dlg->activate_button();
                }
                else if (w == dlg->_list) {
                    dlg->activate_button();
                    if (Fl::event_clicks() > 0 && dlg->_close->active()) {
                        Fl::event_clicks(0);
                        dlg->hide();
                    }
                }
                else if (w == dlg->_close) {
                    dlg->hide();
                }
            }
            void filter(const char* filter) {
                _list->clear();
                for (auto& string : _strings) {
                    if (*filter == 0) {
                        _list->add(string.c_str());
                    }
                    else if (strstr(string.c_str(), filter) != nullptr) {
                        _list->add(string.c_str());
                    }
                }
                _list->value(1);
            }
            int handle(int event) override {
                if (event == FL_KEYDOWN) {
                    if (Fl::event_key() == FL_Enter) {
                        if (_list->value() > 0) {
                            hide();
                        }
                        return 1;
                    }
                    else if (Fl::event_key() == FL_Tab) {
                        if (Fl::focus() == _list || Fl::focus() == _list->menu()) {
                            _filter->take_focus();
                        }
                        else {
                            _list->take_focus();
                        }
                        return 1;
                    }
                }
                return Fl_Double_Window::handle(event);
            }
            void resize(int X, int Y, int W, int H) override {
                Fl_Double_Window::resize(X, Y, W, H);
                _filter->resize(4, 4, W - 8, flw::PREF_FONTSIZE * 2);
                _list->resize(4, flw::PREF_FONTSIZE * 2  + 8, W - 8, H - flw::PREF_FONTSIZE * 4 - 20);
                _cancel->resize(W - flw::PREF_FONTSIZE * 16 - 8, H - flw::PREF_FONTSIZE * 2 - 4, flw::PREF_FONTSIZE * 8, flw::PREF_FONTSIZE * 2);
                _close->resize(W - flw::PREF_FONTSIZE * 8 - 4, H - flw::PREF_FONTSIZE * 2 - 4, flw::PREF_FONTSIZE * 8, flw::PREF_FONTSIZE * 2);
            }
            int run() {
                show();
                while (visible() != 0) {
                    Fl::wait();
                    Fl::flush();
                }
                auto row = _list->value();
                if (row > 0) {
                    auto selected = _list->text(row);
                    for (int f = 0; f < (int) _strings.size(); f++) {
                        auto& string = _strings[f];
                        if (string == selected) {
                            return f + 1;
                        }
                    }
                }
                return 0;
            }
        };
        const char* PASSWORD_CANCEL = "Cancel";
        const char* PASSWORD_OK     = "Ok";
        class _DlgPassword : public Fl_Double_Window {
        public:
            enum class TYPE {
                                        ASK_PASSWORD,
                                        ASK_PASSWORD_AND_KEYFILE,
                                        CONFIRM_PASSWORD,
                                        CONFIRM_PASSWORD_AND_KEYFILE,
            };
        private:
            Fl_Button*                  _browse;
            Fl_Button*                  _cancel;
            Fl_Button*                  _close;
            Fl_Input*                   _file;
            Fl_Input*                   _password1;
            Fl_Input*                   _password2;
            _DlgPassword::TYPE          _mode;
            bool                        _ret;
        public:
            _DlgPassword(const char* title, Fl_Window* parent, _DlgPassword::TYPE mode) :
            Fl_Double_Window(0, 0, 0, 0) {
                end();
                _password1 = new Fl_Secret_Input(0, 0, 0, 0, "Password");
                _password2 = new Fl_Secret_Input(0, 0, 0, 0, "Enter Password Again");
                _browse    = new Fl_Button(0, 0, 0, 0, "&Browse");
                _cancel    = new Fl_Button(0, 0, 0, 0, flw::dlg::PASSWORD_CANCEL);
                _close     = new Fl_Return_Button(0, 0, 0, 0, flw::dlg::PASSWORD_OK);
                _file      = new Fl_Output(0, 0, 0, 0, "Key File");
                _mode      = mode;
                _ret       = false;
                add(_password1);
                add(_password2);
                add(_file);
                add(_browse);
                add(_cancel);
                add(_close);
                _browse->callback(_DlgPassword::Callback, this);
                _cancel->callback(_DlgPassword::Callback, this);
                _close->callback(_DlgPassword::Callback, this);
                _close->deactivate();
                _file->align(FL_ALIGN_TOP | FL_ALIGN_LEFT);
                _file->textfont(flw::PREF_FIXED_FONT);
                _file->textsize(flw::PREF_FONTSIZE);
                _file->tooltip("Select optional key file");
                _password1->align(FL_ALIGN_TOP | FL_ALIGN_LEFT);
                _password1->callback(_DlgPassword::Callback, this);
                _password1->textfont(flw::PREF_FIXED_FONT);
                _password1->textsize(flw::PREF_FONTSIZE);
                _password1->when(FL_WHEN_CHANGED);
                _password2->align(FL_ALIGN_TOP | FL_ALIGN_LEFT);
                _password2->callback(_DlgPassword::Callback, this);
                _password2->textfont(flw::PREF_FIXED_FONT);
                _password2->textsize(flw::PREF_FONTSIZE);
                _password2->when(FL_WHEN_CHANGED);
                auto W = 40 * flw::PREF_FONTSIZE;
                auto H = 11 * flw::PREF_FONTSIZE + 28;
                if (_mode == _DlgPassword::TYPE::ASK_PASSWORD) {
                    _password2->hide();
                    _browse->hide();
                    _file->hide();
                    W = 30 * flw::PREF_FONTSIZE;
                    H = 5 * flw::PREF_FONTSIZE + 16;
                }
                else if (_mode == _DlgPassword::TYPE::CONFIRM_PASSWORD) {
                    _browse->hide();
                    _file->hide();
                    W = 30 * flw::PREF_FONTSIZE;
                    H = 8 * flw::PREF_FONTSIZE + 24;
                }
                else if (_mode == _DlgPassword::TYPE::ASK_PASSWORD_AND_KEYFILE) {
                    _password2->hide();
                    W = 40 * flw::PREF_FONTSIZE;
                    H = 8 * flw::PREF_FONTSIZE + 24;
                }
                util::labelfont(this);
                callback(_DlgPassword::Callback, this);
                label(title);
                size(W, H);
                size_range(W, H);
                set_modal();
                resizable(this);
                flw::util::center_window(this, parent);
            }
            static void Callback(Fl_Widget* w, void* o) {
                auto dlg = (_DlgPassword*) o;
                if (w == dlg) {
                    ;
                }
                else if (w == dlg->_password1) {
                    dlg->check();
                }
                else if (w == dlg->_password2) {
                    dlg->check();
                }
                else if (w == dlg->_browse) {
                    auto filename = fl_file_chooser("Select Key File", nullptr, nullptr, 0);
                    if (filename) {
                        dlg->_file->value(filename);
                    }
                    else {
                        dlg->_file->value("");
                    }
                }
                else if (w == dlg->_cancel) {
                    dlg->_ret = false;
                    dlg->hide();
                }
                else if (w == dlg->_close) {
                    dlg->_ret = true;
                    dlg->hide();
                }
            }
            void check() {
                auto p1 = _password1->value();
                auto p2 = _password2->value();
                if (_mode == _DlgPassword::TYPE::ASK_PASSWORD ||
                    _mode == _DlgPassword::TYPE::ASK_PASSWORD_AND_KEYFILE) {
                    if (strlen(p1)) {
                        _close->activate();
                    }
                    else {
                        _close->deactivate();
                    }
                }
                else if (_mode == _DlgPassword::TYPE::CONFIRM_PASSWORD ||
                         _mode == _DlgPassword::TYPE::CONFIRM_PASSWORD_AND_KEYFILE) {
                    if (strlen(p1) && strcmp(p1, p2) == 0) {
                        _close->activate();
                    }
                    else {
                        _close->deactivate();
                    }
                }
            }
            void resize(int X, int Y, int W, int H) override {
                Fl_Double_Window::resize(X, Y, W, H);
                if (_mode == _DlgPassword::TYPE::ASK_PASSWORD) {
                    _password1->resize(4, flw::PREF_FONTSIZE + 4, W - 8, flw::PREF_FONTSIZE * 2);
                }
                else if (_mode == _DlgPassword::TYPE::CONFIRM_PASSWORD) {
                    _password1->resize(4, flw::PREF_FONTSIZE + 4, W - 8, flw::PREF_FONTSIZE * 2);
                    _password2->resize(4, flw::PREF_FONTSIZE * 4 + 12, W - 8, flw::PREF_FONTSIZE * 2);
                }
                else if (_mode == _DlgPassword::TYPE::ASK_PASSWORD_AND_KEYFILE) {
                    _password1->resize(4, flw::PREF_FONTSIZE + 4, W - 8, flw::PREF_FONTSIZE * 2);
                    _file->resize(4, flw::PREF_FONTSIZE * 4 + 12, W - flw::PREF_FONTSIZE * 8 - 12, flw::PREF_FONTSIZE * 2);
                    _browse->resize(W - flw::PREF_FONTSIZE * 8 - 4, flw::PREF_FONTSIZE * 4 + 12, flw::PREF_FONTSIZE * 8, flw::PREF_FONTSIZE * 2);
                }
                else if (_mode == _DlgPassword::TYPE::CONFIRM_PASSWORD_AND_KEYFILE) {
                    _password1->resize(4, flw::PREF_FONTSIZE + 4, W - 8, flw::PREF_FONTSIZE * 2);
                    _password2->resize(4, flw::PREF_FONTSIZE * 4 + 12, W - 8, flw::PREF_FONTSIZE * 2);
                    _file->resize(4, flw::PREF_FONTSIZE * 7 + 16, W - flw::PREF_FONTSIZE * 8 - 12, flw::PREF_FONTSIZE * 2);
                    _browse->resize(W - flw::PREF_FONTSIZE * 8 - 4, flw::PREF_FONTSIZE * 7 + 16, flw::PREF_FONTSIZE * 8, flw::PREF_FONTSIZE * 2);
                }
                _cancel->resize(W - flw::PREF_FONTSIZE * 16 - 8, H - flw::PREF_FONTSIZE * 2 - 4, flw::PREF_FONTSIZE * 8, flw::PREF_FONTSIZE * 2);
                _close->resize(W - flw::PREF_FONTSIZE * 8 - 4, H - flw::PREF_FONTSIZE * 2 - 4, flw::PREF_FONTSIZE * 8, flw::PREF_FONTSIZE * 2);
            }
            bool run(std::string& password, std::string& file) {
                show();
                while (visible() != 0) {
                    Fl::wait();
                    Fl::flush();
                }
                if (_ret) {
                    file = _file->value();
                    password = _password1->value();
                }
                dlg::zero_memory((void*) _password1->value(), strlen(_password1->value()));
                dlg::zero_memory((void*) _password2->value(), strlen(_password2->value()));
                dlg::zero_memory((void*) _file->value(), strlen(_file->value()));
                return _ret;
            }
        };
        class _DlgText : public Fl_Double_Window {
            Fl_Button*                  _cancel;
            Fl_Button*                  _close;
            Fl_Button*                  _save;
            Fl_Text_Buffer*             _buffer;
            Fl_Text_Display*            _text;
            bool                        _edit;
            char*                       _res;
        public:
            _DlgText(const char* title, const char* text, bool edit, Fl_Window* parent, int W, int H) :
            Fl_Double_Window(0, 0, flw::PREF_FONTSIZE * W, flw::PREF_FONTSIZE * H) {
                end();
                _cancel = new Fl_Button(0, 0, 0, 0, "C&ancel");
                _close  = (edit == false) ? new Fl_Return_Button(0, 0, 0, 0, "&Close") : new Fl_Button(0, 0, 0, 0, "&Close");
                _save   = new Fl_Button(0, 0, 0, 0, "&Save");
                _text   = (edit == false) ? new Fl_Text_Display(0, 0, 0, 0) : new Fl_Text_Editor(0, 0, 0, 0);
                _buffer = new Fl_Text_Buffer();
                _edit   = edit;
                _res    = nullptr;
                add(_save);
                add(_cancel);
                add(_close);
                add(_text);
                _buffer->text(text);
                _cancel->callback(_DlgText::Callback, this);
                _cancel->tooltip("Close and abort all changes.");
                _close->callback(_DlgText::Callback, this);
                _close->tooltip("Close and update text.");
                _save->callback(_DlgText::Callback, this);
                _save->tooltip("Save text to file.");
                _text->buffer(_buffer);
                _text->linenumber_align(FL_ALIGN_RIGHT);
                _text->linenumber_bgcolor(FL_BACKGROUND_COLOR);
                _text->linenumber_fgcolor(FL_FOREGROUND_COLOR);
                _text->linenumber_font(flw::PREF_FIXED_FONT);
                _text->linenumber_size(flw::PREF_FIXED_FONTSIZE);
                _text->linenumber_width(flw::PREF_FIXED_FONTSIZE * 3);
                _text->take_focus();
                _text->textfont(flw::PREF_FIXED_FONT);
                _text->textsize(flw::PREF_FIXED_FONTSIZE);
                util::labelfont(this);
                if (edit == false) {
                    _cancel->hide();
                }
                callback(_DlgText::Callback, this);
                copy_label(title);
                size_range(flw::PREF_FONTSIZE * 24, flw::PREF_FONTSIZE * 12);
                set_modal();
                resizable(this);
                flw::util::center_window(this, parent);
            }
            ~_DlgText() {
               _text->buffer(nullptr);
                delete _buffer;
            }
            static void Callback(Fl_Widget* w, void* o) {
                auto dlg = (_DlgText*) o;
                if (w == dlg || w == dlg->_cancel) {
                    dlg->hide();
                }
                else if (w == dlg->_save) {
                    auto filename = fl_file_chooser("Select Destination File", nullptr, nullptr, 0);
                    if (filename != nullptr && dlg->_buffer->savefile(filename) != 0) {
                        fl_alert("error: failed to save text to %s", filename);
                    }
                }
                else if (w == dlg->_close) {
                    if (dlg->_edit == true) {
                        dlg->_res = dlg->_buffer->text();
                    }
                    dlg->hide();
                }
            }
            void resize(int X, int Y, int W, int H) override {
                Fl_Double_Window::resize(X, Y, W, H);
                _text->resize(4, 4, W - 8, H - flw::PREF_FONTSIZE * 2 - 16);
                if (_cancel->visible() != 0) {
                    _save->resize(W - flw::PREF_FONTSIZE * 24 - 12, H - flw::PREF_FONTSIZE * 2 - 4, flw::PREF_FONTSIZE * 8, flw::PREF_FONTSIZE * 2);
                    _cancel->resize(W - flw::PREF_FONTSIZE * 16 - 8, H - flw::PREF_FONTSIZE * 2 - 4, flw::PREF_FONTSIZE * 8, flw::PREF_FONTSIZE * 2);
                }
                else {
                    _save->resize(W - flw::PREF_FONTSIZE * 16 - 8, H - flw::PREF_FONTSIZE * 2 - 4, flw::PREF_FONTSIZE * 8, flw::PREF_FONTSIZE * 2);
                }
                _close->resize(W - flw::PREF_FONTSIZE * 8 - 4, H - flw::PREF_FONTSIZE * 2 - 4, flw::PREF_FONTSIZE * 8, flw::PREF_FONTSIZE * 2);
            }
            char* run() {
                show();
                while (visible() != 0) {
                    Fl::wait();
                    Fl::flush();
                }
                return _res;
            }
        };
        class _DlgTheme : public Fl_Double_Window {
            Fl_Box*                     _font_label;
            Fl_Box*                     _fixedfont_label;
            Fl_Browser*                 _theme;
            Fl_Button*                  _close;
            Fl_Button*                  _font;
            Fl_Button*                  _fixedfont;
            int                         _theme_row;
        public:
            _DlgTheme(bool enable_font, bool enable_fixedfont, Fl_Window* parent) : Fl_Double_Window(0, 0, 0, 0, "Set Theme") {
                end();
                _close           = new Fl_Return_Button(0, 0, 0, 0, "&Close");
                _fixedfont       = new Fl_Button(0, 0, 0, 0, "F&ixed Font");
                _fixedfont_label = new Fl_Box(0, 0, 0, 0);
                _font            = new Fl_Button(0, 0, 0, 0, "&Font");
                _font_label      = new Fl_Box(0, 0, 0, 0);
                _theme           = new Fl_Hold_Browser(0, 0, 0, 0);
                _theme_row       = 0;
                add(_theme);
                add(_fixedfont);
                add(_font);
                add(_close);
                add(_fixedfont_label);
                add(_font_label);
                if (enable_font == false) {
                  _font->deactivate();
                }
                if (enable_fixedfont == false) {
                  _fixedfont->deactivate();
                }
                _close->callback(Callback, this);
                _font_label->color(FL_BACKGROUND2_COLOR);
                _font_label->box(FL_DOWN_BOX);
                _font->callback(Callback, this);
                _font_label->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
                _fixedfont_label->color(FL_BACKGROUND2_COLOR);
                _fixedfont_label->box(FL_DOWN_BOX);
                _fixedfont->callback(Callback, this);
                _fixedfont_label->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
                _theme->callback(Callback, this);
                _theme->textfont(flw::PREF_FONT);
                for (size_t f = 0; f < 100; f++) {
                    auto theme = flw::PREF_THEMES[f];
                    if (theme != nullptr) _theme->add(theme);
                    else break;
                }
                resizable(this);
                callback(Callback, this);
                set_modal();
                update_pref();
                util::center_window(this, parent);
            }
            static void Callback(Fl_Widget* w, void* o) {
                auto dlg = (_DlgTheme*) o;
                if (w == dlg) {
                    dlg->hide();
                }
                else if (w == dlg->_fixedfont) {
                    flw::dlg::FontDialog fd(flw::PREF_FIXED_FONT, flw::PREF_FIXED_FONTSIZE, "Select Monospaced Font");
                    if (fd.run(Fl::first_window()) == true) {
                        flw::PREF_FIXED_FONT     = fd.font();
                        flw::PREF_FIXED_FONTSIZE = fd.fontsize();
                        flw::PREF_FIXED_FONTNAME = fd.fontname();
                        if (dlg->_font->active() == 0) {
                            flw::PREF_FONTSIZE = flw::PREF_FIXED_FONTSIZE;
                        }
                        dlg->update_pref();
                    }
                }
                else if (w == dlg->_font) {
                    flw::dlg::FontDialog fd(flw::PREF_FONT, flw::PREF_FONTSIZE, "Select Font");
                    if (fd.run(Fl::first_window()) == true) {
                        flw::PREF_FONT     = fd.font();
                        flw::PREF_FONTSIZE = fd.fontsize();
                        flw::PREF_FONTNAME = fd.fontname();
                        if (dlg->_fixedfont->active() == 0) {
                            flw::PREF_FIXED_FONTSIZE = flw::PREF_FONTSIZE;
                        }
                        dlg->update_pref();
                    }
                }
                else if (w == dlg->_theme) {
                    auto row = dlg->_theme->value() - 1;
                    if (row == theme::THEME_OXY) {
                        theme::_load_oxy();
                    }
                    else if (row == theme::THEME_OXY_BLUE) {
                        theme::_load_oxy_blue();
                    }
                    else if (row == theme::THEME_OXY_TAN) {
                        theme::_load_oxy_tan();
                    }
                    else if (row == theme::THEME_GLEAM) {
                        theme::_load_gleam();
                    }
                    else if (row == theme::THEME_GLEAM_BLUE) {
                        theme::_load_gleam_blue();
                    }
                    else if (row == theme::THEME_GLEAM_DARK_BLUE) {
                        theme::_load_gleam_dark_blue();
                    }
                    else if (row == theme::THEME_GLEAM_DARK) {
                        theme::_load_gleam_dark();
                    }
                    else if (row == theme::THEME_GLEAM_DARKER) {
                        theme::_load_gleam_darker();
                    }
                    else if (row == theme::THEME_GLEAM_TAN) {
                        theme::_load_gleam_tan();
                    }
                    else if (row == theme::THEME_GTK) {
                        theme::_load_gtk();
                    }
                    else if (row == theme::THEME_GTK_BLUE) {
                        theme::_load_gtk_blue();
                    }
                    else if (row == theme::THEME_GTK_DARK_BLUE) {
                        theme::_load_gtk_dark_blue();
                    }
                    else if (row == theme::THEME_GTK_DARK) {
                        theme::_load_gtk_dark();
                    }
                    else if (row == theme::THEME_GTK_DARKER) {
                        theme::_load_gtk_darker();
                    }
                    else if (row == theme::THEME_GTK_TAN) {
                        theme::_load_gtk_tan();
                    }
                    else if (row == theme::THEME_PLASTIC) {
                        theme::_load_plastic();
                    }
                    else if (row == theme::THEME_PLASTIC_BLUE) {
                        theme::_load_blue_plastic();
                    }
                    else if (row == theme::THEME_PLASTIC_TAN) {
                        theme::_load_tan_plastic();
                    }
                    else if (row == theme::THEME_SYSTEM) {
                        theme::_load_system();
                    }
                    else {
                        theme::_load_default();
                    }
                    dlg->update_pref();
                }
                else if (w == dlg->_close) {
                    dlg->hide();
                }
            }
            void resize(int X, int Y, int W, int H) override {
                auto fs = flw::PREF_FONTSIZE;
                Fl_Double_Window::resize(X, Y, W, H);
                _theme->resize           (4,                 4,                  W - 8,     H - fs * 6 - 24);
                _font_label->resize      (4,                 H - fs * 6 - 16,    W - 8,     fs * 2);
                _fixedfont_label->resize (4,                 H - fs * 4 - 12,    W - 8,     fs * 2);
                _font->resize            (W - fs * 24 - 12,  H - fs * 2 - 4,     fs * 8,    fs * 2);
                _fixedfont->resize       (W - fs * 16 - 8,   H - fs * 2 - 4,     fs * 8,    fs * 2);
                _close->resize           (W - fs * 8 - 4,    H - fs * 2 - 4,     fs * 8,    fs * 2);
            }
            void run() {
                show();
                while (visible()) {
                    Fl::wait();
                    Fl::flush();
                }
            }
            void update_pref() {
                util::labelfont(this);
                _font_label->copy_label(flw::util::format("%s - %d", flw::PREF_FONTNAME.c_str(), flw::PREF_FONTSIZE).c_str());
                _fixedfont_label->copy_label(flw::util::format("%s - %d", flw::PREF_FIXED_FONTNAME.c_str(), flw::PREF_FIXED_FONTSIZE).c_str());
                _fixedfont_label->labelfont(flw::PREF_FIXED_FONT);
                _fixedfont_label->labelsize(flw::PREF_FIXED_FONTSIZE);
                _theme->textsize(flw::PREF_FONTSIZE);
                size(flw::PREF_FONTSIZE * 32, flw::PREF_FONTSIZE * 30);
                theme::_scrollbar();
                for (int f = 0; f <= theme::THEME_SYSTEM; f++) {
                    if (flw::PREF_THEME == flw::PREF_THEMES[f]) {
                        _theme->value(f + 1);
                        break;
                    }
                }
                Fl::redraw();
            }
        };
    }
}
void flw::dlg::html(std::string title, const std::string& text, Fl_Window* parent, int W, int H) {
    _DlgHtml dlg(title.c_str(), text.c_str(), parent, W, H);
    dlg.run();
}
void flw::dlg::list(std::string title, const StringVector& list, Fl_Window* parent, bool fixed_font, int W, int H) {
    _DlgList dlg(title.c_str(), list, "", parent, fixed_font, W, H);
    dlg.run();
}
void flw::dlg::list(std::string title, const std::string& list, Fl_Window* parent, bool fixed_font, int W, int H) {
    auto list2 = flw::util::split( list, "\n");
    _DlgList dlg(title.c_str(), list2, "", parent, fixed_font, W, H);
    dlg.run();
}
void flw::dlg::list_file(std::string title, std::string file, Fl_Window* parent, bool fixed_font, int W, int H) {
    _DlgList dlg(title.c_str(), flw::StringVector(), file, parent, fixed_font, W, H);
    dlg.run();
}
void flw::dlg::panic(std::string message) {
    fl_alert("panic! I have to quit\n%s", message.c_str());
    exit(1);
}
bool flw::dlg::password1(std::string title, std::string& password, Fl_Window* parent) {
    std::string file;
    _DlgPassword dlg(title.c_str(), parent, _DlgPassword::TYPE::ASK_PASSWORD);
    return dlg.run(password, file);
}
bool flw::dlg::password2(std::string title, std::string& password, Fl_Window* parent) {
    std::string file;
    _DlgPassword dlg(title.c_str(), parent, _DlgPassword::TYPE::CONFIRM_PASSWORD);
    return dlg.run(password, file);
}
bool flw::dlg::password3(std::string title, std::string& password, std::string& file, Fl_Window* parent) {
    _DlgPassword dlg(title.c_str(), parent, _DlgPassword::TYPE::ASK_PASSWORD_AND_KEYFILE);
    return dlg.run(password, file);
}
bool flw::dlg::password4(std::string title, std::string& password, std::string& file, Fl_Window* parent) {
    _DlgPassword dlg(title.c_str(), parent, _DlgPassword::TYPE::CONFIRM_PASSWORD_AND_KEYFILE);
    return dlg.run(password, file);
}
int flw::dlg::select(std::string title, const StringVector& list, int selected_row, Fl_Window* parent, bool fixed_font, int W, int H) {
    _DlgSelect dlg(title.c_str(), parent, list, selected_row, "", fixed_font, W, H);
    return dlg.run();
}
int flw::dlg::select(std::string title, const StringVector& list, const std::string& selected_row, Fl_Window* parent, bool fixed_font, int W, int H) {
    _DlgSelect dlg(title.c_str(), parent, list, 0, selected_row, fixed_font, W, H);
    return dlg.run();
}
void flw::dlg::text(std::string title, const std::string& text, Fl_Window* parent, int W, int H) {
    _DlgText dlg(title.c_str(), text.c_str(), false, parent, W, H);
    dlg.run();
}
bool flw::dlg::text_edit(std::string title, std::string& text, Fl_Window* parent, int W, int H) {
    auto dlg = _DlgText(title.c_str(), text.c_str(), true, parent, W, H);
    auto res = dlg.run();
    if (res == nullptr) {
        return false;
    }
    text = res;
    free(res);
    return true;
}
void flw::dlg::theme(bool enable_font, bool enable_fixedfont, Fl_Window* parent) {
    auto dlg = dlg::_DlgTheme(enable_font, enable_fixedfont, parent);
    dlg.run();
}
flw::dlg::AbortDialog::AbortDialog(double min, double max) : Fl_Double_Window(0, 0, 0, 0, "Working...") {
    _button   = new Fl_Button(0, 0, 0, 0, "Press To Abort");
    _progress = new Fl_Hor_Fill_Slider(0, 0, 0, 0);
    _abort    = false;
    _last     = 0;
    add(_button);
    add(_progress);
    auto W = flw::PREF_FONTSIZE * 32;
    auto H = 0;
    if (min < max && fabs(max - min) > 0.001) {
        _progress->range(min, max);
        _progress->value(min);
        H = flw::PREF_FONTSIZE * 6 + 12;
    }
    else {
        _progress->hide();
        H = flw::PREF_FONTSIZE * 4 + 8;
    }
    _button->callback(AbortDialog::Callback, this);
    _button->labelfont(flw::PREF_FONT);
    _button->labelsize(flw::PREF_FONTSIZE);
    _progress->color(FL_SELECTION_COLOR);
    size(W, H);
    size_range(W, H);
    callback(AbortDialog::Callback, this);
    set_modal();
}
void flw::dlg::AbortDialog::Callback(Fl_Widget* w, void* o) {
    auto dlg = (AbortDialog*) o;
    if (w == dlg->_button) {
        dlg->_abort = true;
    }
}
bool flw::dlg::AbortDialog::check(int milliseconds) {
    auto now = flw::util::milliseconds();
    if (now - _last > milliseconds) {
        _last = now;
        Fl::check();
    }
    return _abort;
}
bool flw::dlg::AbortDialog::check(double value, double min, double max, int milliseconds) {
    auto now = flw::util::milliseconds();
    if (now - _last > milliseconds) {
        _progress->value(value);
        _progress->range(min, max);
        _last = now;
        Fl::check();
    }
    return _abort;
}
void flw::dlg::AbortDialog::range(double min, double max) {
    _progress->range(min, max);
}
void flw::dlg::AbortDialog::resize(int X, int Y, int W, int H) {
    Fl_Double_Window::resize(X, Y, W, H);
    if (_progress->visible() != 0) {
        _button->resize(4, 4, W - 8, H - flw::PREF_FONTSIZE * 2 - 12);
        _progress->resize(4, H - flw::PREF_FONTSIZE * 2 - 4, W - 8, flw::PREF_FONTSIZE * 2);
    }
    else {
        _button->resize(4, 4, W - 8, H - 8);
    }
}
void flw::dlg::AbortDialog::show(const std::string& label, Fl_Window* parent) {
    _abort = false;
    _last  = 0;
    _button->copy_label(label.c_str());
    flw::util::center_window(this, parent);
    Fl_Double_Window::show();
    Fl::flush();
}
void flw::dlg::AbortDialog::value(double value) {
    _progress->value(value);
}
namespace flw {
    namespace dlg {
        static const std::string    _FONTDIALOG_LABEL = R"(
ABCDEFGHIJKLMNOPQRSTUVWXYZ /0123456789
abcdefghijklmnopqrstuvwxyz £©µÀÆÖÞßéöÿ
–—‘“”„†•…‰™œŠŸž€ ΑΒΓΔΩαβγδω АБВГДабвгд
∀∂∈ℝ∧∪≡∞ ↑↗↨↻⇣ ┐┼╔╘░►☺♀ ﬁ�⑀₂ἠḂӥẄɐː⍎אԱა

Hello world, Καλημέρα κόσμε, コンニチハ

math: ∮ E⋅da = Q,  n → ∞, ∑ f(i) 2H₂ + O₂ ⇌ 2H₂O, R = 4.7 kΩ
korean: 구두의
greek: Οὐχὶ ταὐτὰ παρίσταταί μοι γιγνώσκειν
russian: Зарегистрируйтесь сейчас
thai: แผ่นดินฮั่นเสื่อมโทรมแสนสังเวช
amharic: ሰማይ አይታረስ ንጉሥ አይከሰስ
braille: ⡌⠁⠧⠑ ⠼⠁⠒  ⡍⠜⠇⠑⠹⠰⠎ ⡣⠕⠌

“There is nothing else than now.
There is neither yesterday, certainly,
nor is there any tomorrow.
How old must you be before you know that?
There is only now, and if now is only two days,
then two days is your life and everything in it will be in proportion.
This is how you live a life in two days.
And if you stop complaining and asking for what you never will get,
you will have a good life.
A good life is not measured by any biblical span.”
― Ernest Hemingway, For Whom the Bell Tolls
)";
        class _FontDialogLabel : public Fl_Box {
        public:
            int font;
            int size;
            _FontDialogLabel(int x, int y, int w, int h) : Fl_Box(x, y, w, h, _FONTDIALOG_LABEL.c_str()) {
                font = FL_HELVETICA;
                size = 14;
                align(FL_ALIGN_TOP | FL_ALIGN_LEFT | FL_ALIGN_INSIDE | FL_ALIGN_CLIP);
                box(FL_BORDER_BOX);
                color(FL_BACKGROUND2_COLOR);
            }
            void draw() override {
                draw_box();
                fl_font((Fl_Font) font, size);
                fl_color(FL_FOREGROUND_COLOR);
                fl_draw(label(), x() + 3, y() + 3, w() - 6, h() - 6, align());
            }
        };
    }
}
flw::dlg::FontDialog::FontDialog(Fl_Font font, Fl_Fontsize fontsize, const std::string& label) :
Fl_Double_Window(0, 0, flw::PREF_FONTSIZE * 60, flw::PREF_FONTSIZE * 36) {
    _create(font, "", fontsize, label);
}
flw::dlg::FontDialog::FontDialog(std::string font, Fl_Fontsize fontsize, std::string label) :
Fl_Double_Window(0, 0, flw::PREF_FONTSIZE * 60, flw::PREF_FONTSIZE * 36) {
    _create(0, font, fontsize, label);
}
void flw::dlg::FontDialog::_activate() {
    if (_fonts->value() == 0 || _sizes->value() == 0) {
        _select->deactivate();
    }
    else {
        _select->activate();
    }
}
void flw::dlg::FontDialog::Callback(Fl_Widget* w, void* o) {
    FontDialog* dlg = (FontDialog*) o;
    if (w == dlg) {
        dlg->hide();
    }
    else if (w == dlg->_cancel) {
        dlg->hide();
    }
    else if (w == dlg->_fonts) {
        auto row = dlg->_fonts->value();
        if (row > 0) {
            ((_FontDialogLabel*) dlg->_label)->font = row - 1;
        }
        dlg->_activate();
        Fl::redraw();
    }
    else if (w == dlg->_select) {
        auto row1 = dlg->_fonts->value();
        auto row2 = dlg->_sizes->value();
        if (row1 > 0 && row2 > 0) {
            row1--;
            dlg->_fontname = util::remove_browser_format(flw::PREF_FONTNAMES[row1]);
            dlg->_font     = row1;
            dlg->_fontsize = row2 + 5;
            dlg->_ret      = true;
            dlg->hide();
        }
    }
    else if (w == dlg->_sizes) {
        auto row = dlg->_sizes->value();
        if (row > 0) {
            ((_FontDialogLabel*) dlg->_label)->size = row + 5;
        }
        dlg->_activate();
        Fl::redraw();
    }
}
void flw::dlg::FontDialog::_create(Fl_Font font, std::string fontname, Fl_Fontsize fontsize, std::string label) {
    end();
    _cancel   = new Fl_Button(0, 0, 0, 0, "&Cancel");
    _fonts    = new flw::ScrollBrowser(12);
    _label    = new flw::dlg::_FontDialogLabel(0, 0, 0, 0);
    _select   = new Fl_Button(0, 0, 0, 0, "&Select");
    _sizes    = new flw::ScrollBrowser(6);
    _font     = -1;
    _fontsize = -1;
    _ret      = false;
    add(_sizes);
    add(_fonts);
    add(_select);
    add(_cancel);
    add(_label);
    _cancel->callback(flw::dlg::FontDialog::Callback, this);
    _cancel->labelfont(flw::PREF_FONT);
    _cancel->labelsize(flw::PREF_FONTSIZE);
    _fonts->callback(flw::dlg::FontDialog::Callback, this);
    _fonts->textsize(flw::PREF_FONTSIZE);
    _fonts->when(FL_WHEN_CHANGED);
    ((_FontDialogLabel*) _label)->font = font;
    ((_FontDialogLabel*) _label)->size = fontsize;
    _select->callback(flw::dlg::FontDialog::Callback, this);
    _select->labelfont(flw::PREF_FONT);
    _select->labelsize(flw::PREF_FONTSIZE);
    _sizes->callback(flw::dlg::FontDialog::Callback, this);
    _sizes->textsize(flw::PREF_FONTSIZE);
    _sizes->when(FL_WHEN_CHANGED);
    size(w(), h());
    theme::load_fonts();
    for (auto name : flw::PREF_FONTNAMES) {
        _fonts->add(name);
    }
    for (auto f = 6; f <= 72; f++) {
        char buf[50];
        snprintf(buf, 50, "@r%2d  ", f);
        _sizes->add(buf);
    }
    if (fontsize >= 6 && fontsize <= 72) {
        _sizes->value(fontsize - 5);
        _sizes->middleline(fontsize - 5);
        ((_FontDialogLabel*) _label)->font = fontsize;
    }
    else {
        _sizes->value(14 - 5);
        _sizes->middleline(14 - 5);
        ((_FontDialogLabel*) _label)->font = 14;
    }
    if (fontname != "") {
        _select_name(fontname);
    }
    else if (font >= 0 && font < _fonts->size()) {
        _fonts->value(font + 1);
        _fonts->middleline(font + 1);
        ((_FontDialogLabel*) _label)->font = font;
    }
    else {
        _fonts->value(1);
        _fonts->middleline(1);
    }
    resizable(this);
    copy_label(label.c_str());
    callback(flw::dlg::FontDialog::Callback, this);
    size_range(flw::PREF_FONTSIZE * 38, flw::PREF_FONTSIZE * 12);
    set_modal();
    _fonts->take_focus();
}
void flw::dlg::FontDialog::resize(int X, int Y, int W, int H) {
    int fs = flw::PREF_FONTSIZE;
    Fl_Double_Window::resize(X, Y, W, H);
    _fonts->resize  (4,                 4,                  fs * 25,            H - fs * 2 - 16);
    _sizes->resize  (fs * 25 + 8,       4,                  fs * 6,             H - fs * 2 - 16);
    _label->resize  (fs * 31 + 12,      4,                  W - fs * 31 - 16,   H - fs * 2 - 16);
    _cancel->resize (W - fs * 16 - 8,   H - fs * 2 - 4,     fs * 8,             fs * 2);
    _select->resize (W - fs * 8 - 4,    H - fs * 2 - 4,     fs * 8,             fs * 2);
}
bool flw::dlg::FontDialog::run(Fl_Window* parent) {
    _ret = false;
    _activate();
    flw::util::center_window(this, parent);
    show();
    while (visible() != 0) {
        Fl::wait();
        Fl::flush();
    }
    return _ret;
}
void flw::dlg::FontDialog::_select_name(std::string fontname) {
    auto count = 1;
    for (auto font : flw::PREF_FONTNAMES) {
        auto font_without_style = util::remove_browser_format(font);
        if (fontname == font_without_style) {
            _fonts->value(count);
            _fonts->middleline(count);
            ((_FontDialogLabel*) _label)->font = count - 1;
            return;
        }
        count++;
    }
    _fonts->value(1);
    ((_FontDialogLabel*) _label)->font = 0;
}
flw::dlg::WorkDialog::WorkDialog(const char* title, Fl_Window* parent, bool cancel, bool pause, int W, int H) : Fl_Double_Window(0, 0, W * flw::PREF_FONTSIZE, H * flw::PREF_FONTSIZE) {
    end();
    _cancel = new Fl_Button(0, 0, 0, 0, "Cancel");
    _pause  = new Fl_Toggle_Button(0, 0, 0, 0, "Pause");
    _label  = new Fl_Hold_Browser(0, 0, 0, 0);
    _ret    = true;
    _last   = 0.0;
    add(_label);
    add(_pause);
    add(_cancel);
    _cancel->callback(WorkDialog::Callback, this);
    _label->box(FL_BORDER_BOX);
    _label->textfont(flw::PREF_FONT);
    _label->textsize(flw::PREF_FONTSIZE);
    _pause->callback(WorkDialog::Callback, this);
    if (cancel == false) {
        _cancel->deactivate();
    }
    if (pause == false) {
        _pause->deactivate();
    }
    util::labelfont(this);
    callback(WorkDialog::Callback, this);
    copy_label(title);
    size_range(flw::PREF_FONTSIZE * 24, flw::PREF_FONTSIZE * 12);
    set_modal();
    resizable(this);
    flw::util::center_window(this, parent);
    show();
}
void flw::dlg::WorkDialog::Callback(Fl_Widget* w, void* o) {
    auto dlg = (flw::dlg::WorkDialog*) o;
    if (w == dlg) {
    }
    else if (w == dlg->_cancel) {
        dlg->_ret = false;
    }
    else if (w == dlg->_pause) {
        bool cancel = dlg->_cancel->active();
        dlg->_cancel->deactivate();
        dlg->_pause->label("C&ontinue");
        while (dlg->_pause->value() != 0) {
            flw::util::sleep(10);
            Fl::check();
        }
        dlg->_pause->label("&Pause");
        if (cancel) {
            dlg->_cancel->activate();
        }
    }
}
void flw::dlg::WorkDialog::resize(int X, int Y, int W, int H) {
    Fl_Double_Window::resize(X, Y, W, H);
    _label->resize(4, 4, W - 8, H - flw::PREF_FONTSIZE * 2 - 16);
    _pause->resize(W - flw::PREF_FONTSIZE * 16 - 8, H - flw::PREF_FONTSIZE * 2 - 4, flw::PREF_FONTSIZE * 8, flw::PREF_FONTSIZE * 2);
    _cancel->resize(W - flw::PREF_FONTSIZE * 8 - 4, H - flw::PREF_FONTSIZE * 2 - 4, flw::PREF_FONTSIZE * 8, flw::PREF_FONTSIZE * 2);
}
bool flw::dlg::WorkDialog::run(double update_time, const StringVector& messages) {
    auto now = flw::util::clock();
    if (now - _last > update_time) {
        _label->clear();
        for (auto& s : messages) {
            _label->add(s.c_str());
        }
        _last = now;
        Fl::check();
        Fl::flush();
    }
    return _ret;
}
bool flw::dlg::WorkDialog::run(double update_time, const std::string& message) {
    auto now = flw::util::clock();
    if (now - _last > update_time) {
        _label->clear();
        _label->add(message.c_str());
        _last = now;
        Fl::check();
        Fl::flush();
    }
    return _ret;
}
#include <algorithm>
#include <assert.h>
#include <math.h>
#include <stdarg.h>
#include <time.h>
#include <FL/Fl_Window.H>
#include <FL/Fl_File_Chooser.H>
#include <FL/Fl_Hold_Browser.H>
#include <FL/fl_ask.H>
#include <FL/fl_draw.H>
#ifdef _WIN32
    #include <FL/x.H>
    #include <windows.h>
#else
    #include <unistd.h>
#endif
#ifdef FLW_USE_PNG
    #include <FL/Fl_PNG_Image.H>
    #include <FL/fl_draw.H>
#endif
namespace flw {
    std::vector<char*>          PREF_FONTNAMES;
    int                         PREF_FIXED_FONT         = FL_COURIER;
    std::string                 PREF_FIXED_FONTNAME     = "FL_COURIER";
    int                         PREF_FIXED_FONTSIZE     = 14;
    int                         PREF_FONT               = FL_HELVETICA;
    int                         PREF_FONTSIZE           = 14;
    std::string                 PREF_FONTNAME           = "FL_HELVETICA";
    int                         PREF_SCROLLBAR          = 2;
    std::string                 PREF_THEME              = "default";
    const char* const           PREF_THEMES[]           = {
                                    "default",
                                    "oxy",
                                    "blue oxy",
                                    "tan oxy",
                                    "gleam",
                                    "blue gleam",
                                    "dark blue gleam",
                                    "dark gleam",
                                    "darker gleam",
                                    "tan gleam",
                                    "gtk",
                                    "blue gtk",
                                    "dark blue gtk",
                                    "dark gtk",
                                    "darker gtk",
                                    "tan gtk",
                                    "plastic",
                                    "blue plastic",
                                    "tan plastic",
                                    "system",
                                    nullptr,
    };
    struct Stat {
        int64_t                         size;
        int64_t                         mtime;
        int                             mode;
         Stat()
            { size = mtime = 0; mode = 0; }
         Stat(std::string filename) {
            size  = 0;
            mtime = 0;
            mode  = 0;
        #ifdef _WIN32
            wchar_t wbuffer[1025];
            struct __stat64 st;
            while (filename.empty() == false && (filename.back() == '\\' || filename.back() == '/')) {
                filename.pop_back();
            }
            fl_utf8towc(filename.c_str(), filename.length(), wbuffer, 1024);
            if (_wstat64(wbuffer, &st) == 0) {
                size  = st.st_size;
                mtime = st.st_mtime;
                if (S_ISDIR(st.st_mode)) {
                    mode = 1;
                }
                else if (S_ISREG(st.st_mode)) {
                    mode = 2;
                }
                else {
                    mode = 3;
                }
            }
        #else
            struct stat st;
            if (stat(filename.c_str(), &st) == 0) {
                size  = st.st_size;
                mtime = st.st_mtime;
                if (S_ISDIR(st.st_mode)) {
                    mode = 1;
                }
                else if (S_ISREG(st.st_mode)) {
                    mode = 2;
                }
                else {
                    mode = 3;
                }
            }
        #endif
        }
   };
}
flw::Buf::Buf() {
    p = nullptr;
    s = 0;
}
flw::Buf::Buf(size_t S) {
    p = (S < SIZE_MAX) ? (char*) calloc(S + 1, 1) : nullptr;
    s = 0;
    if (p != nullptr) {
        s = S;
    }
}
flw::Buf::Buf(char* P, size_t S) {
    p = P;
    s = S;
}
flw::Buf::Buf(const char* P, size_t S) {
    if (P == nullptr) {
        p = nullptr;
        s = 0;
    }
    else {
        p = (S < SIZE_MAX) ? (char*) calloc(S + 1, 1) : nullptr;
        s = 0;
        if (p == nullptr) {
            throw "error: memory allocation failed";
        }
        else {
            memcpy(p, P, S);
            s = S;
        }
    }
}
flw::Buf::Buf(const Buf& b) {
    if (b.p == nullptr) {
        p = nullptr;
        s = 0;
    }
    else {
        p = (b.s < SIZE_MAX) ? (char*) calloc(b.s + 1, 1) : nullptr;
        s = 0;
        if (p == nullptr) {
            throw "error: memory allocation failed";
        }
        else {
            memcpy(p, b.p, b.s);
            s = b.s;
        }
    }
}
flw::Buf::Buf(Buf&& b) {
    p = b.p;
    s = b.s;
    b.p = nullptr;
}
flw::Buf& flw::Buf::operator=(const Buf& b) {
    if (b.p == nullptr) {
        free(p);
        p = nullptr;
        s = 0;
    }
    else {
        free(p);
        p = (b.s < SIZE_MAX) ? (char*) calloc(b.s + 1, 1) : nullptr;
        s = 0;
        if (p == nullptr) {
            throw "error: memory allocation failed";
        }
        else {
            memcpy(p, b.p, b.s);
            s = b.s;
        }
    }
    return *this;
}
flw::Buf& flw::Buf::operator=(Buf&& b) {
    free(p);
    p = b.p;
    s = b.s;
    b.p = nullptr;
    return *this;
}
flw::Buf& flw::Buf::operator+=(const Buf& b) {
    if (b.p == nullptr) {
    }
    else if (p == nullptr) {
        *this = b;
    }
    else {
        auto t = (b.s < SIZE_MAX) ? (char*) calloc(s + b.s + 1, 1) : nullptr;
        if (t == nullptr) {
            throw "error: memory allocation failed";
        }
        else {
            memcpy(t, p, s);
            memcpy(t + s, b.p, b.s);
            free(p);
            p = t;
            s += b.s;
        }
    }
    return *this;
}
bool flw::Buf::operator==(const Buf& other) const {
    return p != nullptr && s == other.s && memcmp(p, other.p, s) == 0;
}
flw::Buf::~Buf() {
    free(p);
}
void flw::debug::print(Fl_Widget* widget, bool tab) {
    assert(widget);
    printf("%s%-*s| x=%4d, y=%4d, w=%4d, h=%4d\n", tab ? "    " : "", tab ? 16 : 20, widget->label() ? widget->label() : "NO_LABEL",  widget->x(),  widget->y(),  widget->w(),  widget->h());
    fflush(stdout);
}
void flw::debug::print(Fl_Group* group) {
    assert(group);
    puts("");
    debug::print((Fl_Widget*) group);
    for (int f = 0; f < group->children(); f++) {
        debug::print(group->child(f), true);
    }
}
namespace flw {
    namespace menu {
        static Fl_Menu_Item* _item(Fl_Menu_* menu, const char* text) {
            assert(menu && text);
            auto item = menu->find_item(text);
        #ifdef DEBUG
            if (item == nullptr) fprintf(stderr, "error: cant find menu item <%s>\n", text);
        #endif
            return (Fl_Menu_Item*) item;
        }
        void enable_item(Fl_Menu_* menu, const char* text, bool value) {
            auto item = _item(menu, text);
            if (item == nullptr) return;
            if (value == true) item->activate();
            else item->deactivate();
        }
        Fl_Menu_Item* get_item(Fl_Menu_* menu, const char* text) {
            return _item(menu, text);
        }
        bool item_value(Fl_Menu_* menu, const char* text) {
            auto item = _item(menu, text);
            if (item == nullptr) return false;
            return item->value();
        }
        void set_item(Fl_Menu_* menu, const char* text, bool value) {
            auto item = _item(menu, text);
            if (item == nullptr) return;
            if (value == true) item->set();
            else item->clear();
        }
        void setonly_item(Fl_Menu_* menu, const char* text) {
            auto item = _item(menu, text);
            if (item == nullptr) return;
            menu->setonly(item);
        }
    }
}
void flw::util::center_window(Fl_Window* window, Fl_Window* parent) {
    if (parent != nullptr) {
        int x = parent->x() + parent->w() / 2;
        int y = parent->y() + parent->h() / 2;
        window->resize(x - window->w() / 2, y - window->h() / 2, window->w(), window->h());
    }
    else {
        window->resize((Fl::w() / 2) - (window->w() / 2), (Fl::h() / 2) - (window->h() / 2), window->w(), window->h());
    }
}
double flw::util::clock() {
#ifdef _WIN32
    struct timeb timeVal;
    ftime(&timeVal);
    return (double) timeVal.time + (double) (timeVal.millitm / 1000.0);
#else
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return (double) (ts.tv_sec) + (ts.tv_nsec / 1000000000.0);
#endif
}
void flw::util::labelfont(Fl_Widget* widget) {
    assert(widget);
    widget->labelfont(flw::PREF_FONT);
    widget->labelsize(flw::PREF_FONTSIZE);
    auto group = widget->as_group();
    if (group != nullptr) {
        for (auto f = 0; f < group->children(); f++) {
            util::labelfont(group->child(f));
        }
    }
}
std::string flw::util::fix_menu_string(std::string in) {
    std::string res = in;
    util::replace(res, "\\", "\\\\");
    util::replace(res, "_", "\\_");
    util::replace(res, "/", "\\/");
    util::replace(res, "&", "&&");
    return res;
}
std::string flw::util::format(const char* format, ...) {
    if (format == nullptr || *format == 0) return "";
    int         l   = 128;
    int         n   = 0;
    char*       buf = (char*) calloc(l, 1);
    std::string res;
    va_list     args;
    va_start(args, format);
    n = vsnprintf(buf, l, format, args);
    va_end(args);
    if (n < 0) {
        free(buf);
        return res;
    }
    if (n < l) {
        res = buf;
        free(buf);
        return res;
    }
    free(buf);
    l = n + 1;
    buf = (char*) calloc(l, 1);
    if (buf == nullptr) return res;
    va_start(args, format);
    n = vsnprintf(buf, l, format, args);
    va_end(args);
    res = buf;
    free(buf);
    return res;
}
std::string flw::util::format_int(int64_t num, char del) {
    auto pos = 0;
    char tmp1[32];
    char tmp2[32];
    if (del < 1) {
        del = 32;
    }
    memset(tmp2, 0, 32);
    snprintf(tmp1, 32, "%lld", (long long int) num);
    auto len = strlen(tmp1);
    for (int f = len - 1, i = 0; f >= 0 && pos < 32; f--, i++) {
        char c = tmp1[f];
        if ((i % 3) == 0 && i > 0 && c != '-') {
            tmp2[pos++] = del;
        }
        tmp2[pos++] = c;
    }
    std::string r = tmp2;
    std::reverse(r.begin(), r.end());
    return r;
}
flw::Buf flw::util::load_file(std::string filename, bool alert) {
    auto stat = flw::Stat(filename);
    if (stat.mode != 2) {
        if (alert == true) {
            fl_alert("error: file %s is missing or not an file", filename.c_str());
        }
        return Buf();
    }
    auto file = fl_fopen(filename.c_str(), "rb");
    if (file == nullptr) {
        if (alert == true) {
            fl_alert("error: can't open %s", filename.c_str());
        }
        return Buf();
    }
    auto buf  = Buf(stat.size);
    auto read = fread(buf.p, 1, (size_t) stat.size, file);
    fclose(file);
    if (read != (size_t) stat.size) {
        if (alert == true) {
            fl_alert("error: failed to read %s", filename.c_str());
        }
        return Buf();
    }
    return buf;
}
int32_t flw::util::milliseconds() {
#if defined(_WIN32)
    LARGE_INTEGER StartingTime;
    LARGE_INTEGER Frequency;
    QueryPerformanceFrequency(&Frequency);
    QueryPerformanceCounter(&StartingTime);
    StartingTime.QuadPart *= 1000000;
    StartingTime.QuadPart /= Frequency.QuadPart;
    return StartingTime.QuadPart / 1000;
#else
    timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return (t.tv_sec * 1000000 + t.tv_nsec / 1000) / 1000;
#endif
}
void flw::util::png_save(std::string opt_name, Fl_Window* window, int X, int Y, int W, int H) {
#ifdef FLW_USE_PNG
    auto filename = (opt_name == "") ? fl_file_chooser("Save To PNG File", "All Files (*)\tPNG Files (*.png)", "") : opt_name.c_str();
    if (filename != nullptr) {
        window->make_current();
        if (X == 0 && Y == 0 && W == 0 && H == 0) {
            X = window->x();
            Y = window->y();
            W = window->w();
            H = window->h();
        }
        auto image = fl_read_image(nullptr, X, Y, W, H);
        if (image != nullptr) {
            auto ret = fl_write_png(filename, image, W, H);
            if (ret == 0) {
            }
            else if (ret == -1) {
                fl_alert("%s", "error: missing libraries");
            }
            else if (ret == -1) {
                fl_alert("error: failed to save image to %s", filename);
            }
            delete []image;
        }
        else {
            fl_alert("%s", "error: failed to grab image");
        }
    }
#else
    (void) opt_name;
    (void) window;
    (void) X;
    (void) Y;
    (void) W;
    (void) H;
    fl_alert("error: flw not compiled with FLW_USE_PNG flag");
#endif
}
std::string flw::util::remove_browser_format(std::string text) {
    auto res = text;
    auto f   = res.find_last_of("@");
    if (f != std::string::npos) {
        auto tmp = res.substr(f + 1);
        if (tmp[0] == '.' || tmp[0] == 'l' || tmp[0] == 'm' || tmp[0] == 's' || tmp[0] == 'b' || tmp[0] == 'i' || tmp[0] == 'f' || tmp[0] == 'c' || tmp[0] == 'r' || tmp[0] == 'u' || tmp[0] == '-') {
            res = tmp.substr(1);
        }
        else if (tmp[0] == 'B' || tmp[0] == 'C' || tmp[0] == 'F' || tmp[0] == 'S') {
            auto s = std::string();
            auto e = false;
            tmp = tmp.substr(f + 1);
            for (auto c : tmp) {
                if (e == false && c >= '0' && c <= '9') {
                }
                else {
                    e = true;
                    s += c;
                }
            }
            res = s;
        }
        else {
            res = res.substr(f);
        }
    }
    return res;
}
std::string& flw::util::replace(std::string& string, std::string find, std::string replace) {
    if (find == "") return string;
    size_t start = 0;
    while ((start = string.find(find, start)) != std::string::npos) {
        string.replace(start, find.length(), replace);
        start += replace.length();
    }
    return string;
}
bool flw::util::save_file(std::string filename, const void* data, size_t size, bool alert) {
    auto file = fl_fopen(filename.c_str(), "wb");
    if (file != nullptr) {
        auto wrote = fwrite(data, 1, size, file);
        fclose(file);
        if (wrote != size) {
            if (alert == true) {
                fl_alert("error: saving data to %s failed", filename.c_str());
            }
            return false;
        }
    }
    else if (alert == true) {
        fl_alert("error: failed to open %s", filename.c_str());
        return false;
    }
    return true;
}
void flw::util::sleep(int milli) {
#ifdef _WIN32
    Sleep(milli);
#else
    usleep(milli * 1000);
#endif
}
flw::StringVector flw::util::split(const std::string& string, std::string split) {
    auto res = StringVector();
    try {
        if (split != "") {
            auto pos1 = (std::string::size_type) 0;
            auto pos2 = string.find(split);
            while (pos2 != std::string::npos) {
                res.push_back(string.substr(pos1, pos2 - pos1));
                pos1 = pos2 + split.size();
                pos2 = string.find(split, pos1);
            }
            if (pos1 <= string.size()) {
                res.push_back(string.substr(pos1));
            }
        }
    }
    catch(...) {
        res.clear();
    }
    return res;
}
namespace flw {
    namespace color {
        Fl_Color RED     = fl_rgb_color(255, 0, 0);
        Fl_Color LIME    = fl_rgb_color(0, 255, 0);
        Fl_Color BLUE    = fl_rgb_color(0, 0, 255);
        Fl_Color YELLOW  = fl_rgb_color(255, 255, 0);
        Fl_Color CYAN    = fl_rgb_color(0, 255, 255);
        Fl_Color MAGENTA = fl_rgb_color(255, 0, 255);
        Fl_Color SILVER  = fl_rgb_color(192, 192, 192);
        Fl_Color GRAY    = fl_rgb_color(128, 128, 128);
        Fl_Color MAROON  = fl_rgb_color(128, 0, 0);
        Fl_Color OLIVE   = fl_rgb_color(128, 128, 0);
        Fl_Color GREEN   = fl_rgb_color(0, 128, 0);
        Fl_Color PURPLE  = fl_rgb_color(128, 0, 128);
        Fl_Color TEAL    = fl_rgb_color(0, 128, 128);
        Fl_Color NAVY    = fl_rgb_color(0, 0, 128);
        Fl_Color BROWN   = fl_rgb_color(210, 105, 30);
        Fl_Color PINK    = fl_rgb_color(255, 192, 203);
        Fl_Color BEIGE   = fl_rgb_color(245, 245, 220);
        Fl_Color AZURE   = fl_rgb_color(240, 255, 250);
    }
    namespace theme {
        static unsigned char _OLD_R[256]  = { 0 };
        static unsigned char _OLD_G[256]  = { 0 };
        static unsigned char _OLD_B[256]  = { 0 };
        static unsigned char _SYS_R[256]  = { 0 };
        static unsigned char _SYS_G[256]  = { 0 };
        static unsigned char _SYS_B[256]  = { 0 };
        static bool          _IS_DARK     = false;
        static bool          _SAVED_COLOR = false;
        static bool          _SAVED_SYS   = false;
        static void _colors(bool dark) {
            (void) dark;
            color::AZURE   = fl_rgb_color(240, 255, 250);
            color::BEIGE   = fl_rgb_color(245, 245, 220);
            color::BLUE    = fl_rgb_color(0, 0, 255);
            color::BLUE    = fl_rgb_color(0, 0, 255);
            color::BROWN   = fl_rgb_color(210, 105, 30);
            color::CYAN    = fl_rgb_color(0, 255, 255);
            color::GRAY    = fl_rgb_color(128, 128, 128);
            color::GREEN   = fl_rgb_color(0, 128, 0);
            color::LIME    = fl_rgb_color(0, 255, 0);
            color::MAGENTA = fl_rgb_color(255, 0, 255);
            color::MAROON  = fl_rgb_color(128, 0, 0);
            color::NAVY    = fl_rgb_color(0, 0, 128);
            color::OLIVE   = fl_rgb_color(128, 128, 0);
            color::PINK    = fl_rgb_color(255, 192, 203);
            color::PURPLE  = fl_rgb_color(128, 0, 128);
            color::RED     = fl_rgb_color(255, 0, 0);
            color::SILVER  = fl_rgb_color(192, 192, 192);
            color::TEAL    = fl_rgb_color(0, 128, 128);
            color::YELLOW  = fl_rgb_color(255, 255, 0);
        }
        static void _make_default_colors_darker() {
            Fl::set_color(60,    0,  63,   0);
            Fl::set_color(63,    0, 110,   0);
            Fl::set_color(72,   55,   0,   0);
            Fl::set_color(76,   55,  63,   0);
            Fl::set_color(88,  110,   0,   0);
            Fl::set_color(95,  140, 140, 100);
            Fl::set_color(136,   0,   0,  55);
            Fl::set_color(140,   0,  63,  55);
            Fl::set_color(152,  55,   0,  55);
            Fl::set_color(216,   0,   0, 110);
            Fl::set_color(223,   0, 110, 110);
            Fl::set_color(248, 110,   0, 110);
        }
        static void _blue_colors() {
            Fl::set_color(0,     0,   0,   0);
            Fl::set_color(7,   255, 255, 255);
            Fl::set_color(8,   130, 149, 166);
            Fl::set_color(15,  255, 160,  63);
            Fl::set_color(255, 244, 244, 244);
            Fl::background(170, 189, 206);
        }
        static void _dark_blue_colors() {
            Fl::set_color(0,   255, 255, 255);
            Fl::set_color(7,    31,  47,  55);
            Fl::set_color(8,   108, 113, 125);
            Fl::set_color(15,   68, 138, 255);
            Fl::set_color(56,    0,   0,   0);
            Fl::background(64, 80, 87);
        }
        static void _dark_colors() {
            Fl::set_color(0,   240, 240, 240);
            Fl::set_color(7,    55,  55,  55);
            Fl::set_color(8,   100, 100, 100);
            Fl::set_color(15,  149,  75,  37);
            Fl::set_color(56,    0,   0,   0);
            Fl::background(85, 85, 85);
            Fl::background(64, 64, 64);
        }
        static void _darker_colors() {
            Fl::set_color(0,   164, 164, 164);
            Fl::set_color(7,    16,  16,  16);
            Fl::set_color(7,    28,  28,  28);
            Fl::set_color(8,   100, 100, 100);
            Fl::set_color(15,  133,  59,  21);
            Fl::set_color(56,    0,   0,   0);
            Fl::background(32, 32, 32);
            Fl::background(38, 38, 38);
        }
        static void _tan_colors() {
            Fl::set_color(0,     0,   0,   0);
            Fl::set_color(7,   255, 255, 255);
            Fl::set_color(8,    85,  85,  85);
            Fl::set_color(15,  255, 160,  63);
            Fl::set_color(255, 244, 244, 244);
            Fl::background(206, 202, 187);
        }
        static void _restore_colors() {
            if (_SAVED_COLOR == true) {
                for (int f = 0; f < 256; f++) {
                    Fl::set_color(f, flw::theme::_OLD_R[f], flw::theme::_OLD_G[f], flw::theme::_OLD_B[f]);
                }
            }
        }
        static void _restore_sys() {
            if (_SAVED_SYS == true) {
                for (int f = 0; f < 256; f++) {
                    Fl::set_color(f, flw::theme::_SYS_R[f], flw::theme::_SYS_G[f], flw::theme::_SYS_B[f]);
                }
            }
        }
        static void _save_colors() {
            if (_SAVED_COLOR == false) {
                for (int f = 0; f < 256; f++) {
                    unsigned char r1, g1, b1;
                    Fl::get_color(f, r1, g1, b1);
                    flw::theme::_OLD_R[f] = r1;
                    flw::theme::_OLD_G[f] = g1;
                    flw::theme::_OLD_B[f] = b1;
                }
                _SAVED_COLOR = true;
            }
        }
        static void _save_sys() {
            if (_SAVED_SYS == false) {
                for (int f = 0; f < 256; f++) {
                    unsigned char r1, g1, b1;
                    Fl::get_color(f, r1, g1, b1);
                    flw::theme::_SYS_R[f] = r1;
                    flw::theme::_SYS_G[f] = g1;
                    flw::theme::_SYS_B[f] = b1;
                }
                _SAVED_SYS = true;
            }
        }
        void _load_default() {
            flw::theme::_save_colors();
            flw::theme::_restore_colors();
            flw::theme::_colors(false);
            Fl::set_color(FL_SELECTION_COLOR, 0, 120, 200);
            Fl::scheme("none");
            Fl::redraw();
            flw::PREF_THEME = flw::PREF_THEMES[theme::THEME_DEFAULT];
            _IS_DARK = false;
        }
        void _load_oxy() {
            flw::theme::_save_colors();
            flw::theme::_restore_colors();
            flw::theme::_colors(false);
            Fl::scheme("oxy");
            Fl::set_color(FL_SELECTION_COLOR, 0, 120, 200);
            Fl::redraw();
            flw::PREF_THEME = flw::PREF_THEMES[theme::THEME_OXY];
            _IS_DARK = false;
            flw::PREF_SCROLLBAR = 1;
        }
        void _load_oxy_blue() {
            flw::theme::_save_colors();
            flw::theme::_restore_colors();
            flw::theme::_blue_colors();
            flw::theme::_colors(false);
            Fl::scheme("oxy");
            Fl::redraw();
            flw::PREF_THEME = flw::PREF_THEMES[theme::THEME_OXY_BLUE];
            _IS_DARK = false;
            flw::PREF_SCROLLBAR = 1;
        }
        void _load_oxy_tan() {
            flw::theme::_save_colors();
            flw::theme::_restore_colors();
            flw::theme::_tan_colors();
            flw::theme::_colors(false);
            Fl::scheme("oxy");
            Fl::redraw();
            flw::PREF_THEME = flw::PREF_THEMES[theme::THEME_OXY_TAN];
            _IS_DARK = false;
            flw::PREF_SCROLLBAR = 1;
        }
        void _load_gleam() {
            flw::theme::_save_colors();
            flw::theme::_restore_colors();
            flw::theme::_colors(false);
            Fl::scheme("gleam");
            Fl::set_color(FL_SELECTION_COLOR, 0, 120, 200);
            Fl::redraw();
            flw::PREF_THEME = flw::PREF_THEMES[theme::THEME_GLEAM];
            _IS_DARK = false;
            flw::PREF_SCROLLBAR = 1;
        }
        void _load_gleam_blue() {
            flw::theme::_save_colors();
            flw::theme::_restore_colors();
            flw::theme::_blue_colors();
            flw::theme::_colors(false);
            Fl::scheme("gleam");
            Fl::redraw();
            flw::PREF_THEME = flw::PREF_THEMES[theme::THEME_GLEAM_BLUE];
            _IS_DARK = false;
            flw::PREF_SCROLLBAR = 1;
        }
        void _load_gleam_dark() {
            flw::theme::_save_colors();
            flw::theme::_restore_colors();
            flw::theme::_dark_colors();
            flw::theme::_make_default_colors_darker();
            flw::theme::_colors(true);
            Fl::set_color(255, 125, 125, 125);
            Fl::scheme("gleam");
            Fl::redraw();
            flw::PREF_THEME = flw::PREF_THEMES[theme::THEME_GLEAM_DARK];
            _IS_DARK = true;
            flw::PREF_SCROLLBAR = 1;
        }
        void _load_gleam_darker() {
            flw::theme::_save_colors();
            flw::theme::_restore_colors();
            flw::theme::_darker_colors();
            flw::theme::_make_default_colors_darker();
            flw::theme::_colors(true);
            Fl::set_color(255, 125, 125, 125);
            Fl::scheme("gleam");
            Fl::redraw();
            flw::PREF_THEME = flw::PREF_THEMES[theme::THEME_GLEAM_DARKER];
            _IS_DARK = true;
            flw::PREF_SCROLLBAR = 1;
        }
        void _load_gleam_dark_blue() {
            flw::theme::_save_colors();
            flw::theme::_restore_colors();
            flw::theme::_dark_blue_colors();
            flw::theme::_make_default_colors_darker();
            flw::theme::_colors(true);
            Fl::set_color(255, 101, 117, 125);
            Fl::scheme("gleam");
            Fl::redraw();
            flw::PREF_THEME = flw::PREF_THEMES[theme::THEME_GLEAM_DARK_BLUE];
            _IS_DARK = true;
            flw::PREF_SCROLLBAR = 1;
        }
        void _load_gleam_tan() {
            flw::theme::_save_colors();
            flw::theme::_restore_colors();
            flw::theme::_tan_colors();
            flw::theme::_colors(false);
            Fl::scheme("gleam");
            Fl::redraw();
            flw::PREF_THEME = flw::PREF_THEMES[theme::THEME_GLEAM_TAN];
            _IS_DARK = false;
            flw::PREF_SCROLLBAR = 1;
        }
        void _load_gtk() {
            flw::theme::_save_colors();
            flw::theme::_restore_colors();
            flw::theme::_colors(false);
            Fl::scheme("gtk+");
            Fl::set_color(FL_SELECTION_COLOR, 0, 120, 200);
            Fl::redraw();
            flw::PREF_THEME = flw::PREF_THEMES[theme::THEME_GTK];
            _IS_DARK = false;
            flw::PREF_SCROLLBAR = 1;
        }
        void _load_gtk_blue() {
            flw::theme::_save_colors();
            flw::theme::_restore_colors();
            flw::theme::_blue_colors();
            flw::theme::_colors(false);
            Fl::scheme("gtk+");
            Fl::redraw();
            flw::PREF_THEME = flw::PREF_THEMES[theme::THEME_GTK_BLUE];
            _IS_DARK = false;
            flw::PREF_SCROLLBAR = 1;
        }
        void _load_gtk_dark() {
            flw::theme::_save_colors();
            flw::theme::_restore_colors();
            flw::theme::_dark_colors();
            flw::theme::_make_default_colors_darker();
            flw::theme::_colors(true);
            Fl::set_color(255, 185, 185, 185);
            Fl::scheme("gtk+");
            Fl::redraw();
            flw::PREF_THEME = flw::PREF_THEMES[theme::THEME_GTK_DARK];
            _IS_DARK = true;
            flw::PREF_SCROLLBAR = 1;
        }
        void _load_gtk_darker() {
            flw::theme::_save_colors();
            flw::theme::_restore_colors();
            flw::theme::_darker_colors();
            flw::theme::_make_default_colors_darker();
            flw::theme::_colors(true);
            Fl::set_color(255, 125, 125, 125);
            Fl::scheme("gtk+");
            Fl::redraw();
            flw::PREF_THEME = flw::PREF_THEMES[theme::THEME_GTK_DARKER];
            _IS_DARK = true;
        }
        void _load_gtk_dark_blue() {
            flw::theme::_save_colors();
            flw::theme::_restore_colors();
            flw::theme::_dark_blue_colors();
            flw::theme::_make_default_colors_darker();
            flw::theme::_colors(true);
            Fl::set_color(255, 161, 177, 185);
            Fl::scheme("gtk+");
            Fl::redraw();
            flw::PREF_THEME = flw::PREF_THEMES[theme::THEME_GTK_DARK_BLUE];
            _IS_DARK = true;
            flw::PREF_SCROLLBAR = 1;
        }
        void _load_gtk_tan() {
            flw::theme::_save_colors();
            flw::theme::_restore_colors();
            flw::theme::_tan_colors();
            flw::theme::_colors(false);
            Fl::scheme("gtk+");
            Fl::redraw();
            flw::PREF_THEME = flw::PREF_THEMES[theme::THEME_GTK_TAN];
            _IS_DARK = false;
            flw::PREF_SCROLLBAR = 1;
        }
        void _load_plastic() {
            flw::theme::_save_colors();
            flw::theme::_restore_colors();
            flw::theme::_colors(false);
            Fl::set_color(FL_SELECTION_COLOR, 0, 120, 200);
            Fl::scheme("plastic");
            Fl::redraw();
            flw::PREF_THEME = flw::PREF_THEMES[theme::THEME_PLASTIC];
            _IS_DARK = false;
            flw::PREF_SCROLLBAR = 2;
        }
        void _load_blue_plastic() {
            flw::theme::_save_colors();
            flw::theme::_restore_colors();
            flw::theme::_blue_colors();
            flw::theme::_colors(false);
            Fl::scheme("plastic");
            Fl::redraw();
            flw::PREF_THEME = flw::PREF_THEMES[theme::THEME_PLASTIC_BLUE];
            _IS_DARK = false;
            flw::PREF_SCROLLBAR = 2;
        }
        void _load_tan_plastic() {
            flw::theme::_save_colors();
            flw::theme::_restore_colors();
            flw::theme::_tan_colors();
            flw::theme::_colors(false);
            Fl::scheme("plastic");
            Fl::redraw();
            flw::PREF_THEME = flw::PREF_THEMES[theme::THEME_PLASTIC_TAN];
            _IS_DARK = false;
            flw::PREF_SCROLLBAR = 2;
        }
        void _load_system() {
            flw::theme::_save_colors();
            flw::theme::_restore_colors();
            flw::theme::_colors(false);
            Fl::scheme("gtk+");
            if (theme::_SAVED_SYS) {
              flw::theme::_restore_sys();
            }
            else {
              Fl::get_system_colors();
              flw::theme::_save_sys();
            }
            Fl::redraw();
            flw::PREF_THEME = flw::PREF_THEMES[theme::THEME_SYSTEM];
            _IS_DARK = false;
            flw::PREF_SCROLLBAR = 1;
        }
        void _scrollbar() {
            auto s = (flw::PREF_FONTSIZE > flw::PREF_FIXED_FONTSIZE) ? flw::PREF_FONTSIZE : flw::PREF_FIXED_FONTSIZE;
            Fl::scrollbar_size(s + flw::PREF_SCROLLBAR);
        }
    }
}
bool flw::theme::is_dark() {
    if (flw::PREF_THEME == flw::PREF_THEMES[theme::THEME_GLEAM_DARK_BLUE] ||
        flw::PREF_THEME == flw::PREF_THEMES[theme::THEME_GLEAM_DARK] ||
        flw::PREF_THEME == flw::PREF_THEMES[theme::THEME_GLEAM_DARKER] ||
        flw::PREF_THEME == flw::PREF_THEMES[theme::THEME_GTK_DARK_BLUE] ||
        flw::PREF_THEME == flw::PREF_THEMES[theme::THEME_GTK_DARK] ||
        flw::PREF_THEME == flw::PREF_THEMES[theme::THEME_GTK_DARKER]) {
        return true;
    }
    else {
        return false;
    }
}
bool flw::theme::load(std::string name) {
    if (name == flw::PREF_THEMES[theme::THEME_DEFAULT]) {
        flw::theme::_load_default();
    }
    else if (name == flw::PREF_THEMES[theme::THEME_OXY]) {
        flw::theme::_load_oxy();
    }
    else if (name == flw::PREF_THEMES[theme::THEME_OXY_BLUE]) {
        flw::theme::_load_oxy_blue();
    }
    else if (name == flw::PREF_THEMES[theme::THEME_OXY_TAN]) {
        flw::theme::_load_oxy_tan();
    }
    else if (name == flw::PREF_THEMES[theme::THEME_GLEAM]) {
        flw::theme::_load_gleam();
    }
    else if (name == flw::PREF_THEMES[theme::THEME_GLEAM_BLUE]) {
        flw::theme::_load_gleam_blue();
    }
    else if (name == flw::PREF_THEMES[theme::THEME_GLEAM_DARK_BLUE]) {
        flw::theme::_load_gleam_dark_blue();
    }
    else if (name == flw::PREF_THEMES[theme::THEME_GLEAM_DARK]) {
        flw::theme::_load_gleam_dark();
    }
    else if (name == flw::PREF_THEMES[theme::THEME_GLEAM_DARKER]) {
        flw::theme::_load_gleam_darker();
    }
    else if (name == flw::PREF_THEMES[theme::THEME_GLEAM_TAN]) {
        flw::theme::_load_gleam_tan();
    }
    else if (name == flw::PREF_THEMES[theme::THEME_GTK]) {
        flw::theme::_load_gtk();
    }
    else if (name == flw::PREF_THEMES[theme::THEME_GTK_BLUE]) {
        flw::theme::_load_gtk_blue();
    }
    else if (name == flw::PREF_THEMES[theme::THEME_GTK_DARK_BLUE]) {
        flw::theme::_load_gtk_dark_blue();
    }
    else if (name == flw::PREF_THEMES[theme::THEME_GTK_DARK]) {
        flw::theme::_load_gtk_dark();
    }
    else if (name == flw::PREF_THEMES[theme::THEME_GTK_DARKER]) {
        flw::theme::_load_gtk_darker();
    }
    else if (name == flw::PREF_THEMES[theme::THEME_GTK_TAN]) {
        flw::theme::_load_gtk_tan();
    }
    else if (name == flw::PREF_THEMES[theme::THEME_PLASTIC]) {
        flw::theme::_load_plastic();
    }
    else if (name == flw::PREF_THEMES[theme::THEME_PLASTIC_BLUE]) {
        flw::theme::_load_blue_plastic();
    }
    else if (name == flw::PREF_THEMES[theme::THEME_PLASTIC_TAN]) {
        flw::theme::_load_tan_plastic();
    }
    else if (name == flw::PREF_THEMES[theme::THEME_SYSTEM]) {
        flw::theme::_load_system();
    }
    else {
        return false;
    }
    theme::_scrollbar();
    return true;
}
int flw::theme::load_font(std::string requested_font) {
    theme::load_fonts();
    auto count = 0;
    for (auto font : flw::PREF_FONTNAMES) {
        auto font2 = util::remove_browser_format(font);
        if (requested_font == font2) {
            return count;
        }
        count++;
    }
    return -1;
}
void flw::theme::load_fonts(bool iso8859_only) {
    if (flw::PREF_FONTNAMES.size() == 0) {
        auto fonts = Fl::set_fonts((iso8859_only == true) ? nullptr : "-*");
        for (int f = 0; f < fonts; f++) {
            auto attr  = 0;
            auto name1 = Fl::get_font_name((Fl_Font) f, &attr);
            auto name2 = std::string();
            if (attr & FL_BOLD) {
                name2 = std::string("@b");
            }
            if (attr & FL_ITALIC) {
                name2 += std::string("@i");
            }
            name2 += std::string("@.");
            name2 += name1;
            flw::PREF_FONTNAMES.push_back(strdup(name2.c_str()));
        }
    }
}
void flw::theme::load_icon(Fl_Window* win, int win_resource, const char** xpm_resource, const char* name) {
    assert(win);
    if (win->shown() != 0) {
        fl_alert("%s", "warning: load icon before showing window!");
    }
#if defined(_WIN32)
    win->icon((char*) LoadIcon(fl_display, MAKEINTRESOURCE(win_resource)));
    (void) name;
    (void) xpm_resource;
    (void) name;
#elif defined(__linux__)
    assert(xpm_resource);
    Fl_Pixmap    pix(xpm_resource);
    Fl_RGB_Image rgb(&pix, Fl_Color(0));
    win->icon(&rgb);
    win->xclass((name != nullptr) ? name : "FLTK");
    (void) win_resource;
#else
    (void) win;
    (void) win_resource;
    (void) xpm_resource;
    (void) name;
#endif
}
void flw::theme::load_theme_pref(Fl_Preferences& pref) {
    auto val = 0;
    char buffer[4000];
    pref.get("fontname", buffer, "", 4000);
    if (*buffer != 0 && strcmp("FL_HELVETICA", buffer) != 0) {
        auto font = theme::load_font(buffer);
        if (font != -1) {
            flw::PREF_FONT     = font;
            flw::PREF_FONTNAME = buffer;
        }
    }
    pref.get("fontsize", val, flw::PREF_FONTSIZE);
    if (val >= 6 && val <= 72) {
        flw::PREF_FONTSIZE = val;
    }
    pref.get("fixedfontname", buffer, "", 1000);
    if (*buffer != 0 && strcmp("FL_COURIER", buffer) != 0) {
        auto font = theme::load_font(buffer);
        if (font != -1) {
            flw::PREF_FIXED_FONT     = font;
            flw::PREF_FIXED_FONTNAME = buffer;
        }
    }
    pref.get("fixedfontsize", val, flw::PREF_FIXED_FONTSIZE);
    if (val >= 6 && val <= 72) {
        flw::PREF_FIXED_FONTSIZE = val;
    }
    pref.get("theme", buffer, "oxy", 4000);
    flw::theme::load(buffer);
}
void flw::theme::load_win_pref(Fl_Preferences& pref, Fl_Window* window, bool show, int defw, int defh, std::string basename) {
    assert(window);
    int  x, y, w, h, f;
    pref.get((basename + "x").c_str(), x, 80);
    pref.get((basename + "y").c_str(), y, 60);
    pref.get((basename + "w").c_str(), w, defw);
    pref.get((basename + "h").c_str(), h, defh);
    pref.get((basename + "fullscreen").c_str(), f, 0);
    if (w == 0 || h == 0) {
        w = 800;
        h = 600;
    }
    if (x + w <= 0 || y + h <= 0 || x >= Fl::w() || y >= Fl::h()) {
        x = 80;
        y = 60;
    }
    if (show == true && window->shown() == 0) {
        window->resize(x, y, w, h);
        window->show();
    }
    else {
        window->resize(x, y, w, h);
    }
    if (f == 1) {
        window->fullscreen();
    }
}
bool flw::theme::parse(int argc, const char** argv) {
    auto res = false;
    for (auto f = 1; f < argc; f++) {
        if (res == false) {
            res = flw::theme::load(argv[f]);
        }
        auto fontsize = atoi(argv[f]);
        if (fontsize >= 6 && fontsize <= 72) {
            flw::PREF_FONTSIZE = fontsize;
        }
    }
    flw::PREF_FIXED_FONTSIZE = flw::PREF_FONTSIZE;
    return res;
}
void flw::theme::save_theme_pref(Fl_Preferences& pref) {
    pref.set("theme", flw::PREF_THEME.c_str());
    pref.set("fontname", flw::PREF_FONTNAME.c_str());
    pref.set("fontsize", flw::PREF_FONTSIZE);
    pref.set("fixedfontname", flw::PREF_FIXED_FONTNAME.c_str());
    pref.set("fixedfontsize", flw::PREF_FIXED_FONTSIZE);
}
void flw::theme::save_win_pref(Fl_Preferences& pref, Fl_Window* window, std::string basename) {
    assert(window);
    pref.set((basename + "x").c_str(), window->x());
    pref.set((basename + "y").c_str(), window->y());
    pref.set((basename + "w").c_str(), window->w());
    pref.set((basename + "h").c_str(), window->h());
    pref.set((basename + "fullscreen").c_str(), window->fullscreen_active() ? 1 : 0);
}
#include <stdarg.h>
namespace flw {
    static int _FLW_GRID_STRING_SIZE = 1000;
}
flw::Grid::Grid(int rows, int cols, int X, int Y, int W, int H, const char* l) : flw::TableEditor(X, Y, W, H, l) {
    rows    = rows < 1 ? 1 : rows;
    cols    = cols < 1 ? 1 : cols;
    _buffer = (char*) calloc(_FLW_GRID_STRING_SIZE + 1, 1);
    Grid::size(rows, cols);
    lines(true, true);
    header(true, true);
    select_mode(TableDisplay::SELECT::CELL);
    active_cell(1, 1);
    resize_column_width(true);
    expand_last_column(true);
}
flw::Grid::~Grid() {
    free(_buffer);
}
Fl_Align flw::Grid::cell_align(int row, int col) {
    return (Fl_Align) _get_int(_cell_align, row, col, TableEditor::cell_align(row, col));
}
void flw::Grid::cell_align(int row, int col, Fl_Align align) {
    _set_int(_cell_align, row, col, (int) align);
}
flw::StringVector flw::Grid::cell_choice(int row, int col) {
    _cell_choices.clear();
    auto choices = _get_string(_cell_choice, row, col);
    if (*choices != 0) {
        auto tmp   = strdup(choices);
        auto start = tmp;
        auto iter  = tmp;
        while (*iter) {
            if (*iter == '\t') {
                *iter = 0;
                _cell_choices.push_back(start);
                start = iter + 1;
            }
            iter++;
        }
        free(tmp);
    }
    return _cell_choices;
}
void flw::Grid::cell_choice(int row, int col, const char* value) {
    _set_string(_cell_choice, row, col, value);
}
Fl_Color flw::Grid::cell_color(int row, int col) {
    return (Fl_Color) _get_int(_cell_color, row, col, TableEditor::cell_color(row, col));
}
void flw::Grid::cell_color(int row, int col, Fl_Color color) {
    _set_int(_cell_color, row, col, (int) color);
}
bool flw::Grid::cell_edit(int row, int col) {
    return (bool) _get_int(_cell_edit, row, col, 0);
}
void flw::Grid::cell_edit(int row, int col, bool value) {
    _set_int(_cell_edit, row, col, (int) value);
}
flw::TableEditor::FORMAT flw::Grid::cell_format(int row, int col) {
    return (TableEditor::FORMAT) _get_int(_cell_format, row, col, (int) TableEditor::FORMAT::DEFAULT);
}
void flw::Grid::cell_format(int row, int col, TableEditor::FORMAT value) {
    _set_int(_cell_format, row, col, (int) value);
}
flw::TableEditor::REND flw::Grid::cell_rend(int row, int col) {
    return (TableEditor::REND) _get_int(_cell_rend, row, col, (int) TableEditor::REND::TEXT);
}
void flw::Grid::cell_rend(int row, int col, TableEditor::REND rend) {
    _set_int(_cell_rend, row, col, (int) rend);
}
Fl_Color flw::Grid::cell_textcolor(int row, int col) {
    return (Fl_Color) _get_int(_cell_textcolor, row, col, TableEditor::cell_textcolor(row, col));
}
void flw::Grid::cell_textcolor(int row, int col, Fl_Color color) {
    _set_int(_cell_textcolor, row, col, (int) color);
}
const char* flw::Grid::cell_value(int row, int col) {
    return _get_string(_cell_value, row, col);
}
bool flw::Grid::cell_value(int row, int col, const char* value) {
    _set_string(_cell_value, row, col, value);
    return true;
}
void flw::Grid::cell_value2(int row, int col, const char* format, ...) {
    va_list args;
    int     n = 0;
    va_start(args, format);
    n = vsnprintf(_buffer, _FLW_GRID_STRING_SIZE, format, args);
    va_end(args);
    if (n < 1 || n >= _FLW_GRID_STRING_SIZE) {
        *_buffer = 0;
    }
    cell_value(row, col, _buffer);
}
int flw::Grid::cell_width(int col) {
    int W = _get_int(_cell_width, 0, col, 80);
    return W >= 10 ? W : TableEditor::cell_width(col);
}
void flw::Grid::cell_width(int col, int width) {
    _set_int(_cell_width, 0, col, (int) width);
}
int flw::Grid::_get_int(StringMap& map, int row, int col, int def) {
    auto value = _get_string(map, row, col);
    return *value ? atoi(value) : def;
}
const char* flw::Grid::_get_key(int row, int col) {
    sprintf(_key, "r%dc%d", row, col);
    return _key;
}
const char* flw::Grid::_get_string(StringMap& map, int row, int col, const char* def) {
    auto search = map.find(_get_key(row, col));
    return search != map.end() ? search->second.c_str() : def;
}
void flw::Grid::_set_int(StringMap& map, int row, int col, int value) {
    char value2[100];
    sprintf(value2, "%d", value);
    map[_get_key(row, col)] = value2;
}
void flw::Grid::_set_string(StringMap& map, int row, int col, const char* value) {
    map[_get_key(row, col)] = value ? value : "";
}
void flw::Grid::size(int rows, int cols) {
    if (rows > 0 && cols > 0) {
        TableEditor::size(rows, cols);
    }
}
namespace flw {
    struct _GridGroupChild {
        Fl_Widget* widget;
        int x;
        int y;
        int w;
        int h;
        _GridGroupChild() {
            widget = nullptr;
            x = 0;
            y = 0;
            w = 0;
            h = 0;
        }
    };
}
flw::GridGroup::GridGroup(int X, int Y, int W, int H, const char* l) : Fl_Group(X, Y, W, H, l) {
    end();
    clip_children(1);
    _size = labelsize();
    for (int f = 0; f < flw::GridGroup::MAX_WIDGETS; f++) {
        _widgets[f] = new _GridGroupChild();
    }
}
flw::GridGroup::~GridGroup() {
    for (int f = 0; f < flw::GridGroup::MAX_WIDGETS; f++) {
        delete _widgets[f];
    }
}
void flw::GridGroup::add(Fl_Widget* widget, int x, int y, int w, int h) {
    for (int f = 0; f < flw::GridGroup::MAX_WIDGETS; f++) {
        if (_widgets[f]->widget == nullptr) {
            Fl_Group::add(widget);
            _widgets[f]->widget = widget;
            _widgets[f]->x      = x;
            _widgets[f]->y      = y;
            _widgets[f]->w      = w;
            _widgets[f]->h      = h;
            return;
        }
    }
    #ifdef DEBUG
        fprintf(stderr, "error: flw::GridGroup::add too many widgets\n");
    #endif
}
void flw::GridGroup::clear() {
    for (int f = 0; f < flw::GridGroup::MAX_WIDGETS; f++) {
        _widgets[f]->widget = nullptr;
        _widgets[f]->x      = 0;
        _widgets[f]->y      = 0;
        _widgets[f]->w      = 0;
        _widgets[f]->h      = 0;
    }
    Fl_Group::clear();
}
void flw::GridGroup::remove(Fl_Widget* widget) {
    for (int f = 0; f < flw::GridGroup::MAX_WIDGETS; f++) {
        if (_widgets[f]->widget == widget) {
            Fl_Group::remove(widget);
            _widgets[f]->widget = nullptr;
            _widgets[f]->x      = 0;
            _widgets[f]->y      = 0;
            _widgets[f]->w      = 0;
            _widgets[f]->h      = 0;
            return;
        }
    }
    #ifdef DEBUG
        fprintf(stderr, "error: flw::GridGroup::remove can't find widget\n");
    #endif
}
void flw::GridGroup::resize() {
    resize(x(), y(), w(), h());
    redraw();
}
void flw::GridGroup::resize(int X, int Y, int W, int H) {
    Fl_Widget::resize(X, Y, W, H);
    if (W == 0 || H == 0) {
        return;
    }
    #ifdef DEBUG
    #endif
    for (int f = 0; f < flw::GridGroup::MAX_WIDGETS; f++) {
        _GridGroupChild* widget = _widgets[f];
        if (widget->widget) {
            int widget_x  = 0;
            int widget_x2 = 0;
            int widget_y  = 0;
            int widget_y2 = 0;
            int widget_w  = 0;
            int widget_h  = 0;
            if (widget->x >= 0) {
                widget_x = X + widget->x * _size;
            }
            else {
                widget_x = X + W + widget->x * _size;
            }
            if (widget->y >= 0) {
                widget_y = Y + widget->y * _size;
            }
            else {
                widget_y = Y + H + widget->y * _size;
            }
            if (widget->w == 0) {
                widget_x2 = X + W;
            }
            else if (widget->w > 0) {
                widget_x2 = widget_x + widget->w * _size;
            }
            else {
                widget_x2 = X + W + widget->w * _size;
            }
            if (widget->h == 0) {
                widget_y2 = Y + H;
            }
            else if (widget->h > 0) {
                widget_y2 = widget_y + widget->h * _size;
            }
            else {
                widget_y2 = Y + H + widget->h * _size;
            }
            widget_w = widget_x2 - widget_x;
            widget_h = widget_y2 - widget_y;
            if (widget_w >= 0 && widget_h >= 0) {
                widget->widget->resize(widget_x, widget_y, widget_w, widget_h);
            }
            else {
                widget->widget->resize(widget_x, widget_y, 0, 0);
            }
            #ifdef DEBUG
            #endif
        }
    }
}
namespace flw {
    static const std::string _INPUTMENU_TOOLTIP = "Use up/down arrows to switch between previous values\nPress ctrl + space to open menu button (if visible)";
    class _InputMenu : public Fl_Input {
    public:
        bool            show_menu;
        int             index;
        StringVector    history;
        _InputMenu() : Fl_Input(0, 0, 0, 0) {
            tooltip(_INPUTMENU_TOOLTIP.c_str());
            index     = -1;
            show_menu = false;
        }
        int handle(int event) override {
            if (event == FL_KEYBOARD) {
                auto key = Fl::event_key();
                if (Fl::event_ctrl() != 0 && key == ' ') {
                    if (history.size() > 0) {
                        show_menu = true;
                        do_callback();
                    }
                    return 1;
                }
                else if (key == FL_Up && history.size() > 0) {
                    if (index <= 0) {
                        value("");
                        index = -1;
                    }
                    else {
                        index--;
                        show_menu = false;
                        value(history[index].c_str());
                    }
                    return 1;
                }
                else if (key == FL_Down && history.size() > 0 && index < (int) history.size() - 1) {
                    index++;
                    value(history[index].c_str());
                    show_menu = false;
                    return 1;
                }
                else {
                    show_menu = false;
                }
            }
            return Fl_Input::handle(event);
        }
    };
}
flw::InputMenu::InputMenu(int X, int Y, int W, int H, const char* l) : Fl_Group(X, Y, W, H, l) {
    end();
    _input = new flw::_InputMenu();
    _menu  = new Fl_Menu_Button(0, 0, 0, 0);
    Fl_Group::add(_input);
    Fl_Group::add(_menu);
    _input->callback(InputMenu::Callback, this);
    _input->when(FL_WHEN_ENTER_KEY_ALWAYS);
    _menu->callback(InputMenu::Callback, this);
    update_pref();
}
void flw::InputMenu::Callback(Fl_Widget* sender, void* self) {
    auto w = (InputMenu*) self;
    if (sender == w->_input) {
        if (w->_input->show_menu) {
            if (w->_menu->visible()) {
                w->_menu->popup();
            }
        }
        else {
            w->do_callback();
        }
    }
    else if (sender == w->_menu) {
        auto index = w->_menu->find_index(w->_menu->text());
        if (index >= 0 && index < (int) w->_input->history.size()) {
            w->_input->value(w->_input->history[index].c_str());
            w->_input->index = index;
        }
        w->_input->take_focus();
    }
}
void flw::InputMenu::clear() {
    _menu->clear();
    _input->history.clear();
    _input->index = -1;
}
flw::StringVector flw::InputMenu::get_history() const {
    return _input->history;
}
void flw::InputMenu::insert(const std::string& string, int max_list_len) {
    for (auto it = _input->history.begin(); it != _input->history.end(); ++it) {
        if (*it == string) {
            _input->history.erase(it);
            break;
        }
    }
    _input->history.insert(_input->history.begin(), string);
    while ((int) _input->history.size() > max_list_len) {
        _input->history.pop_back();
    }
    _menu->clear();
    for (const auto& s : _input->history) {
        _menu->add(flw::util::fix_menu_string(s.c_str()).c_str());
    }
    _input->index = -1;
    _input->value(string.c_str());
    _input->insert_position(string.length(), 0);
}
void flw::InputMenu::resize(int X, int Y, int W, int H) {
    Fl_Group::resize(X, Y, W, H);
    if (_menu->visible() != 0) {
        auto mw = (int) flw::PREF_FONTSIZE / 2;
        _input->resize(X, Y, W - flw::PREF_FONTSIZE - mw, H);
        _menu->resize(X + W - flw::PREF_FONTSIZE - mw, Y + 2, flw::PREF_FONTSIZE + mw, H - 4);
    }
    else {
        _input->resize(X, Y, W, H);
    }
}
void flw::InputMenu::set(const StringVector& list, bool copy_first_to_input) {
    clear();
    _input->history = list;
    for (const auto& s : _input->history) {
        _menu->add(flw::util::fix_menu_string(s.c_str()).c_str());
    }
    if (list.size() > 0 && copy_first_to_input == true) {
        auto s = list.front();
        _input->value(s.c_str());
        _input->insert_position(s.length(), 0);
    }
}
void flw::InputMenu::update_pref(Fl_Font text_font, Fl_Fontsize text_size) {
    labelfont(flw::PREF_FONT);
    labelsize(flw::PREF_FONTSIZE);
    _input->labelfont(text_font);
    _input->labelsize(text_font);
    _input->textfont(text_font);
    _input->textsize(text_size);
    _menu->labelfont(text_font);
    _menu->labelsize(text_size);
    _menu->textfont(text_font);
    _menu->textsize(text_size);
}
const char* flw::InputMenu::value() const {
    return _input->value();
}
void flw::InputMenu::value(const char* string) {
    _input->value(string ? string : "");
}
#include <string.h>
#include <errno.h>
namespace flw {
namespace json {
    #define _FLW_JSON_RETURN(X,Y)           return Err((ssize_t) (X - Y), line);
    #define _FLW_JSON_RETURN_POS(X)         return Err(X.pos, X.line);
    #define _FLW_JSON_CHECK_SEPARATOR(X)    if (X > 32 && X != ',' && X != ':' && X != '}' && X != ']' && X != '{' && X != '[') return Err((ssize_t) (text - json), line);
    static const char* _TYPE_NAMES[] = { "NA", "OBJECT", "END_OBJECT", "ARRAY","END_ARRAY", "STRING", "NUMBER", "BOOL", "NIL", "COLON", "COMMA", "ARRAY_NL" };
    static bool _convert_num(const char* s, double& d) {
        errno = 0;
        d = strtod(s, nullptr);
        return errno != ERANGE;
    }
    static std::string _remove_last(std::vector<std::string>& v) {
        if (v.size() > 0) {
            auto res = v.back();
            v.pop_back();
            return res;
        }
        return "";
    }
    struct _Token {
        TYPE                            type;
        int                             depth;
        std::string                     value;
        size_t                          pos;
        size_t                          line;
                                        _Token()
                                            { type = json::NA; pos = 0; depth = 0; line = 0; }
                                        _Token(TYPE type, const std::string& value = "", size_t pos = 0, size_t depth = 0, size_t line = 0)
                                            { this->type = type; this->value = value; this->pos = pos; this->depth = depth; this->line = line; }
        bool                            is_end() const
                                            { return type == json::END_ARRAY || type == json::END_OBJECT; }
        bool                            is_start() const
                                            { return type == json::ARRAY || type == json::OBJECT; }
    };
    typedef std::vector<_Token>         _TokenVector;
    static void _tokenize_count_dot_minus_plus_e(const std::string& s, int& dot, int& minus, int& plus, int& E) {
        dot = 0; minus = 0; plus = 0; E = 0;
        for (auto c : s) {
            dot   += (c =='.');
            minus += (c =='-');
            plus  += (c =='+');
            E     += (c =='e');
            E     += (c =='E');
        }
    }
    static Err _tokenize(const char* json, size_t len, _TokenVector& tokens) {
        tokens.clear();
        auto text  = json;
        auto end   = text + len;
        auto value = std::string();
        auto num   = std::string();
        auto depth = 0;
        auto line  = 1;
        auto DUMMY = _Token();
        num.reserve(50);
        value.reserve(200);
        while (text < end) {
            auto  c  = (unsigned char) *text;
            if (c == 10) {
                line++;
            }
            if (c < 33) {
                text++;
            }
            else if (c == ',') {
                tokens.push_back(_Token(json::COMMA, "", (size_t) text - (size_t) json, depth, line));
                text++;
            }
            else if (c == ':') {
                tokens.push_back(_Token(json::COLON, "", (size_t) text - (size_t) json, depth, line));
                text++;
            }
            else if (c == '{') {
                tokens.push_back(_Token(json::OBJECT, "", (size_t) text - (size_t) json, depth, line));
                depth++;
                text++;
            }
            else if (c == '}') {
                depth--;
                tokens.push_back(_Token(json::END_OBJECT, "", (size_t) text - (size_t) json, depth, line));
                text++;
            }
            else if (c == '[') {
                tokens.push_back(_Token(json::ARRAY, "", (size_t) text - (size_t) json, depth, line));
                depth++;
                text++;
            }
            else if (c == ']') {
                depth--;
                tokens.push_back(_Token(json::END_ARRAY, "", (size_t) text - (size_t) json, depth, line));
                text++;
            }
            else if (c == 't' && strncmp(text, "true", 4) == 0) {
                tokens.push_back(_Token(json::BOOL, "true", (size_t) text - (size_t) json, depth, line));
                text += 4;
                c = *text;
                _FLW_JSON_CHECK_SEPARATOR(c)
            }
            else if (c == 'f' && strncmp(text, "false", 5) == 0) {
                tokens.push_back(_Token(json::BOOL, "false", (size_t) text - (size_t) json, depth, line));
                text += 5;
                c = *text;
                _FLW_JSON_CHECK_SEPARATOR(c)
            }
            else if (c == 'n' && strncmp(text, "null", 4) == 0) {
                tokens.push_back(_Token(json::NIL, "", (size_t) text - (size_t) json, depth, line));
                text += 4;
                c = *text;
                _FLW_JSON_CHECK_SEPARATOR(c)
            }
            else if (c == '"') {
                auto p = (unsigned char) *text;
                auto start = text;
                text++;
                auto c = (unsigned char) *text;
                value = "";
                while (text < end) {
                    if (c < 32) _FLW_JSON_RETURN(text, json)
                    else if (p == '\\' && c == '"') {
                    }
                    else if (c == '"') {
                        tokens.push_back(_Token(json::STRING, value, (size_t) start - (size_t) json, depth, line));
                        text++;
                        break;
                    }
                    value += c;
                    p = c;
                    text++;
                    c = *text;
                }
                c = *text;
                if (text == end) _FLW_JSON_RETURN(text, json)
                _FLW_JSON_CHECK_SEPARATOR(c)
            }
            else if (c == '-' || c == '.' || (c >= '0' && c <= '9')) {
                auto start = text;
                std::string n1, n2;
                int dot, minus, plus, E;
                while (c == '-' || c == '.' || (c >= '0' && c <= '9')) {
                    n1 += c; text++; c = *text;
                }
                _tokenize_count_dot_minus_plus_e(n1, dot, minus, plus, E);
                if (dot > 1 || minus > 1) _FLW_JSON_RETURN(start, json)
                if (n1[0] == '.' || n1.back() == '.') _FLW_JSON_RETURN(start, json)
                if (n1[0] == '-' && n1[1] == '.') _FLW_JSON_RETURN(start, json)
                if (n1[0] == '-' && n1[1] == '0' && n1.length() > 2 && n1[2] != '.') _FLW_JSON_RETURN(start, json)
                if (n1 == "-") _FLW_JSON_RETURN(start, json)
                if (minus > 0 && n1[0] != '-') _FLW_JSON_RETURN(start, json)
                while (c == 'e' || c == 'E' || c == '-' || c == '+' || (c >= '0' && c <= '9')) {
                    n2 += c; text++; c = *text;
                }
                if (n2.empty() == false) {
                    if (n2.length() == 1) _FLW_JSON_RETURN(start, json)
                    _tokenize_count_dot_minus_plus_e(n2, dot, minus, plus, E);
                    if (plus + minus > 1 || E > 1) _FLW_JSON_RETURN(start, json)
                    if (plus > 0 && n2.back() == '+') _FLW_JSON_RETURN(start, json)
                    if (plus > 0 && n2[1] != '+') _FLW_JSON_RETURN(start, json)
                    if (minus > 0 && n2.back() == '-') _FLW_JSON_RETURN(start, json)
                    if (minus > 0 && n2[1] != '-') _FLW_JSON_RETURN(start, json)
                }
                else if (n1[0] == '0' && n1.length() > 1 && n1[1] != '.') _FLW_JSON_RETURN(start, json)
                tokens.push_back(_Token(json::NUMBER, n1 + n2, (size_t) start - (size_t) json, depth, line));
                _FLW_JSON_CHECK_SEPARATOR(c)
            }
            else {
                _FLW_JSON_RETURN(text, json);
            }
        }
        return Err();
    }
    void Node::print() const {
        printf("%5u| %5u| %2d| %*s %-10s <%s>", (unsigned) index, (unsigned) textpos, depth, depth * 4, "", _TYPE_NAMES[(int) type], name.c_str());
        if (type == json::STRING || type == json::NUMBER || type == json::BOOL) printf(" => <%s>\n", value.c_str());
        else printf("\n");
        fflush(stdout);
    }
    std::string escape_string(const std::string& string) {
        std::string res;
        for (unsigned char c : string) {
            if (c == 9) res += "\\t";
            else if (c == 10) res += "\\n";
            else if (c == 13) res += "\\r";
            else if (c == 8) res += "\\b";
            else if (c == 14) res += "\\f";
            else if (c == 34) res += "\\\"";
            else if (c == 92) res += "\\\\";
            else res += c;
        }
        return res;
    }
    NodeVector find_children(const NodeVector& nodes, const Node& start, bool grandchildren) {
        auto res = NodeVector();
        if (start.index + 1 >= nodes.size()) return res;
        for (size_t f = start.index + 1; f < nodes.size(); f++) {
            auto& node = nodes[f];
            if (node.depth <= start.depth) break;
            else if (grandchildren == true) res.push_back(node);
            else if (node.depth == start.depth + 1 && node.is_end() == false) res.push_back(node);
        }
        return res;
    }
    NodeVector find_nodes(const NodeVector& nodes, std::string name, TYPE type) {
        auto res = NodeVector();
        for (size_t f = 0; f < nodes.size(); f++) {
            auto& node = nodes[f];
            if (name == "") {
                if (type == json::NA) res.push_back(node);
                else if (node.type == type) res.push_back(node);
            }
            else if (node.name == name) {
                if (type == json::NA) res.push_back(node);
                else if (node.type == type) res.push_back(node);
            }
        }
        return res;
    }
    NodeVector find_siblings(const NodeVector& nodes, const Node& start) {
        auto res = NodeVector();
        if (start.index >= nodes.size()) return res;
        for (size_t f = start.index; f < nodes.size(); f++) {
            auto& node = nodes[f];
            if (node.depth < start.depth) break;
            else if (node.depth == start.depth && node.is_end() == false) res.push_back(node);
        }
        return res;
    }
    Err parse(const char* json, NodeVector& nodes, bool ignore_trailing_comma) {
        auto tokens      = _TokenVector();
        auto len         = strlen(json);
        auto err         = _tokenize(json, len, tokens);
        auto size        = tokens.size();
        auto obj         = 0;
        auto arr         = 0;
        auto num         = 0.0;
        auto arr_name    = std::vector<std::string>();
        auto obj_name    = std::vector<std::string>();
        auto containers  = std::vector<std::string>();
        auto const DUMMY = _Token();
        nodes.clear();
        if (err.pos >= 0) return err;
        else if (size < 2) return Err(size, 1);
        for (size_t f = 0; f < size; f++) {
            auto& t  = tokens[f];
            auto& T0 = (f > 0) ? tokens[f - 1] : DUMMY;
            auto& T1 = (f < size - 1) ? tokens[f + 1] : DUMMY;
            auto& T2 = (f < size - 2) ? tokens[f + 2] : DUMMY;
            if ((T0.type == json::STRING || T0.type == json::NUMBER || T0.type == json::NIL || T0.type == json::BOOL) &&
                (t.type != json::COMMA && t.type != json::END_ARRAY && t.type != json::END_OBJECT)) {
                _FLW_JSON_RETURN_POS(t)
            }
            else if (obj == 0 && arr == 0 && nodes.size() > 0) {
                _FLW_JSON_RETURN_POS(t)
            }
            else if (obj == 0 && arr == 0 && t.type != json::OBJECT && t.type != json::ARRAY) {
                _FLW_JSON_RETURN_POS(t)
            }
            else if (t.type == json::COMMA && T0.type == json::COMMA) {
                _FLW_JSON_RETURN_POS(t)
            }
            else if (t.type == json::COMMA && T0.type == json::ARRAY) {
                _FLW_JSON_RETURN_POS(t)
            }
            else if (t.type == json::COLON && T0.type != json::STRING) {
                _FLW_JSON_RETURN_POS(t)
            }
            else if (t.type == json::END_OBJECT) {
                if (ignore_trailing_comma == false && T0.type == json::COMMA) _FLW_JSON_RETURN_POS(t)
                containers.pop_back();
                obj--;
                if (obj < 0) _FLW_JSON_RETURN_POS(t)
               nodes.push_back(Node(json::END_OBJECT, _remove_last(obj_name), "", t.depth, t.pos));
            }
            else if (t.type == json::OBJECT) {
                if (containers.size() && containers.back() == "O") _FLW_JSON_RETURN_POS(t)
                if (T0.is_end() == true && T0.depth == t.depth) _FLW_JSON_RETURN_POS(t)
                containers.push_back("O");
                obj++;
                nodes.push_back(Node(json::OBJECT, "", "", t.depth, t.pos));
                obj_name.push_back("");
            }
            else if (t.type == json::STRING && T1.type == json::COLON && T2.type == json::OBJECT) {
                if (T0.is_end() == true && T0.depth == T2.depth) _FLW_JSON_RETURN_POS(t)
                if (obj == 0) _FLW_JSON_RETURN_POS(t)
                containers.push_back("O");
                obj++;
                nodes.push_back(Node(json::OBJECT, t.value, "", T2.depth, t.pos));
                obj_name.push_back(t.value);
                f += 2;
            }
            else if (t.type == json::END_ARRAY) {
                if (ignore_trailing_comma == false && T0.type == json::COMMA) _FLW_JSON_RETURN_POS(t)
                containers.pop_back();
                arr--;
                if (arr < 0) _FLW_JSON_RETURN_POS(t)
                nodes.push_back(Node(json::END_ARRAY, _remove_last(arr_name), "", t.depth, t.pos));
            }
            else if (t.type == json::ARRAY) {
                if (containers.size() && containers.back() == "O") _FLW_JSON_RETURN_POS(t)
                if (T0.is_end() == true && T0.depth == t.depth) _FLW_JSON_RETURN_POS(t)
                containers.push_back("A");
                arr++;
                nodes.push_back(Node(json::ARRAY, "", "", t.depth, t.pos));
                arr_name.push_back("");
            }
            else if (t.type == json::STRING && T1.type == json::COLON && T2.type == json::ARRAY) {
                if (T0.is_end() == true && T0.depth == T2.depth) _FLW_JSON_RETURN_POS(t)
                if (obj == 0) _FLW_JSON_RETURN_POS(t)
                containers.push_back("A");
                arr++;
                nodes.push_back(Node(json::ARRAY, t.value, "", T2.depth, t.pos));
                arr_name.push_back(t.value);
                f += 2;
            }
            else if (t.type == json::BOOL) {
                nodes.push_back(Node(json::BOOL, "", t.value, t.depth, t.pos));
                if (containers.size() && containers.back() == "O") _FLW_JSON_RETURN_POS(t)
            }
            else if (t.type == json::STRING && T1.type == json::COLON && T2.type == json::BOOL) {
                nodes.push_back(Node(json::BOOL, t.value, T2.value, T2.depth, t.pos));
                f += 2;
                if (containers.size() && containers.back() == "A") _FLW_JSON_RETURN_POS(t)
            }
            else if (t.type == json::NIL) {
                nodes.push_back(Node(json::NIL, "", "", t.depth, t.pos));
                if (containers.size() && containers.back() == "O") _FLW_JSON_RETURN_POS(t)
            }
            else if (t.type == json::STRING && T1.type == json::COLON && T2.type == json::NIL) {
                nodes.push_back(Node(json::NIL, t.value, "", T2.depth, t.pos));
                f += 2;
                if (containers.size() && containers.back() == "A") _FLW_JSON_RETURN_POS(t)
            }
            else if (t.type == json::NUMBER) {
                if (_convert_num(t.value.c_str(), num) == false) _FLW_JSON_RETURN_POS(t)
                nodes.push_back(Node(json::NUMBER, "", t.value, t.depth, t.pos));
                if (containers.size() && containers.back() == "O") _FLW_JSON_RETURN_POS(t)
            }
            else if (t.type == json::STRING && T1.type == json::COLON && T2.type == json::NUMBER) {
                if (_convert_num(T2.value.c_str(), num) == false) _FLW_JSON_RETURN_POS(t)
                nodes.push_back(Node(json::NUMBER, t.value, T2.value, t.depth, t.pos));
                f += 2;
                if (containers.size() && containers.back() == "A") _FLW_JSON_RETURN_POS(t)
            }
            else if (t.type == json::STRING && T1.type == json::COLON && T2.type == json::STRING) {
                nodes.push_back(Node(json::STRING, t.value, T2.value, t.depth, t.pos));
                f += 2;
                if (containers.size() && containers.back() == "A") _FLW_JSON_RETURN_POS(t)
            }
            else if (t.type == json::STRING) {
                nodes.push_back(Node(json::STRING, "", t.value, t.depth, t.pos));
                if (containers.size() && containers.back() == "O") _FLW_JSON_RETURN_POS(t)
            }
            if (nodes.size() > 0) nodes.back().index   = nodes.size() - 1;
            if (arr < 0 || obj < 0) _FLW_JSON_RETURN_POS(t)
        }
        if (arr != 0 || obj != 0) return Err(len, -1);
        return Err();
    }
    Err parse(std::string json, NodeVector& nodes, bool ignore_trailing_commas) {
        return parse(json.c_str(), nodes, ignore_trailing_commas);
    }
    void print(const NodeVector& nodes) {
        auto c = 0;
        printf("\n%d Node objects\n", (int) nodes.size());
        printf("%6s| %6s| %6s| %6s| %s\n", "COUNT", "INDEX", "POS", "DEPTH", "VALUES");
        printf("-------------------------------------------------------\n");
        for (auto& node : nodes) {
            printf("%6d| %6u| %6u| %6d|%*s  %-10s <%s>", c, (unsigned) node.index, (unsigned) node.textpos, node.depth, node.depth * 4, "", _TYPE_NAMES[(int) node.type], node.name.c_str());
            if (node.type == json::STRING || node.type == json::NUMBER || node.type == json::BOOL) printf(" => <%s>", node.value.c_str());
            printf("\n");
            c++;
        }
        fflush(stdout);
    }
    std::string tostring(const NodeVector& nodes) {
        if (nodes.size() < 2) return "";
        auto res    = std::string();
        auto DUMMY  = Node();
        auto last   = nodes.size() - 1;
        auto indent = std::string();
        auto arr    = json::NA;
        res.reserve(20 * nodes.size());
        for (size_t f = 0; f <= last; f++) {
            auto& node = nodes[f];
            auto& next = (f < last) ? nodes[f + 1] : DUMMY;
            auto  nl   = std::string("\n");
            auto  nl2  = std::string("");
            if (node.is_data() && next.is_end() == false) {
                nl  = ",\n";
                nl2 = ",";
            }
            else if (node.is_end() && (next.is_start() == true || next.is_data() == true)) {
                nl  = ",\n";
                nl2 = ",";
            }
            if (node.type == json::END_OBJECT) {
                indent.pop_back();
                res += indent;
                res += "}";
            }
            else if (node.type == json::END_ARRAY) {
                indent.pop_back();
                if (arr != json::ARRAY) res += indent;
                res += "]";
                arr = json::NA;
            }
            else {
                if (arr != json::ARRAY) res += indent;
                if (node.type == json::OBJECT) {
                    indent += "\t";
                    if (node.name == "") res += "{";
                    else res += "\"" + node.name + "\": {";
                }
                else if (node.type == json::ARRAY || node.type == json::ARRAY_NL) {
                    indent += "\t";
                    if (node.name == "") res += "[";
                    else res += "\"" + node.name + "\": [";
                    arr = (node.type == json::ARRAY_NL) ? json::ARRAY : json::NA;
                }
                else if (node.type == json::STRING) {
                    if (node.name == "") res += "\"" + node.value + "\"";
                    else res += "\"" + node.name + "\": \"" + node.value + "\"";
                }
                else if (node.type == json::NUMBER) {
                    if (node.name == "") res += node.value;
                    else res += "\"" + node.name + "\": " + node.value;
                }
                else if (node.type == json::BOOL) {
                    if (node.name == "") res += node.value;
                    else res += "\"" + node.name + "\": " + node.value;
                }
                else if (node.type == json::NIL) {
                    if (node.name == "") res += "null";
                    else res += "\"" + node.name + "\": null";
                }
            }
            if (arr != json::ARRAY) res += nl;
            else res += nl2;
        }
        return res;
    }
    std::string unescape_string(const std::string& string) {
        std::string res;
        for (size_t f = 0; f < string.length(); f++) {
            unsigned char c = string[f];
            unsigned char n = string[f + 1];
            if (c == '\\') {
                if (n == 't') res += '\t';
                else if (n == 'n') res += '\n';
                else if (n == 'r') res += '\r';
                else if (n == 'b') res += '\b';
                else if (n == 'f') res += '\f';
                else if (n == '\"') res += '"';
                else if (n == '\\') res += '\\';
                f++;
            }
            else res += c;
        }
        return res;
    }
     Err validate(const char* json) {
        auto nodes = NodeVector();
        return parse(json, nodes);
    }
     Err validate(std::string json) {
        auto nodes = NodeVector();
        return parse(json.c_str(), nodes);
    }
}
}
#include <FL/fl_draw.H>
namespace flw {
    static const unsigned char _LCDNUMBER_SEGMENTS[20] = {
        0x00,
        0x7E,
        0x30,
        0x6D,
        0x79,
        0x33,
        0x5B,
        0x5F,
        0x70,
        0x7F,
        0x7B,
        0x77,
        0x1F,
        0x4E,
        0x3D,
        0x4F,
        0x47,
        0x01,
        0xA0,
        0x00,
    };
}
flw::LcdNumber::LcdNumber(int x, int y, int w, int h, const char *l) : Fl_Box(x, y, w, h, l) {
    _align     = FL_ALIGN_LEFT;
    _dot_size  = 6;
    _seg_color = FL_FOREGROUND_COLOR;
    _seg_gap   = 1;
    _thick     = 4;
    _unit_gap  = 4;
    _unit_h    = 42;
    _unit_w    = 21;
    *_value = 0;
    value("0");
    box(FL_NO_BOX);
}
void flw::LcdNumber::draw() {
    Fl_Box::draw();
    int sw   = 0;
    int tx   = x();
    int ty   = y();
    int tw   = w();
    int th   = h();
    for (size_t f = 0; f < strlen(_value); f++) {
        if (_value[f] == 0x12) {
            sw += _dot_size + _unit_gap;
        }
        else {
            sw += _unit_w + _unit_gap;
        }
    }
    switch(_align) {
        case FL_ALIGN_RIGHT:
            tx += (tw - sw) - _unit_w;
            break;
        case FL_ALIGN_CENTER:
            tx += (tw / 2 - sw / 2);
            break;
        default:
            tx += _unit_w;
            break;
    }
    ty += ((th - _unit_h) >> 1);
    fl_color(active_r() ? _seg_color : fl_inactive(_seg_color));
    int xx = tx;
    for(int i = 0; _value[i]; i++) {
        if (_value[i] == 0x12) {
            fl_rectf(xx, ty + _unit_h + 1 - _dot_size, _dot_size, _dot_size);
            xx += (_dot_size + _unit_gap);
        }
        else {
            _draw_seg(_LCDNUMBER_SEGMENTS[(int) _value[i]], xx, ty, _unit_w, _unit_h);
            xx += (_unit_w + _unit_gap);
        }
    }
}
void flw::LcdNumber::_draw_seg(uchar a, int x, int y, int w, int h) {
    int x0, y0, x1, y1, x2, y2, x3, y3;
    int h2     = h >> 1;
    int thick2 = _thick >> 1;
    if (a & 0x40) {
        x0 = x + _seg_gap;
        y0 = y;
        x1 = x0 + _thick;
        y1 = y0 + _thick;
        x2 = x + w - _thick - _seg_gap;
        y2 = y1;
        x3 = x2 + _thick;
        y3 = y0;
        fl_polygon(x0, y0, x1, y1, x2, y2, x3, y3);
    }
    if (a & 0x20) {
        x0 = x + w;
        y0 = y + _seg_gap;
        x1 = x0 - _thick;
        y1 = y0 + _thick;
        x2 = x1;
        y2 = y + h2 - thick2 - _seg_gap;
        x3 = x0;
        y3 = y2 + thick2;
        fl_polygon(x0, y0, x1, y1, x2, y2, x3, y3);
    }
    if (a & 0x10) {
        x0 = x + w;
        y0 = y + h2 + _seg_gap;
        x1 = x0 - _thick;
        y1 = y0 + thick2;
        x2 = x1;
        y2 = y + h - _thick - _seg_gap;
        x3 = x0;
        y3 = y2 + _thick;
        fl_polygon(x0, y0, x1, y1, x2, y2, x3, y3);
    }
    if (a & 0x08) {
        x0 = x + _seg_gap;
        y0 = y + h;
        x1 = x0 + _thick;
        y1 = y0 - _thick;
        x2 = x + w - _thick - _seg_gap;
        y2 = y1;
        x3 = x2 + _thick;
        y3 = y0;
        fl_polygon(x0, y0, x1, y1, x2, y2, x3, y3);
    }
    if (a & 0x04) {
        x0 = x;
        y0 = y + h2 + _seg_gap;
        x1 = x0 + _thick;
        y1 = y0 + thick2;
        x2 = x1;
        y2 = y + h - _thick - _seg_gap;
        x3 = x0;
        y3 = y2 + _thick;
        fl_polygon(x0, y0, x1, y1, x2, y2, x3, y3);
    }
    if (a & 0x02) {
        x0 = x;
        y0 = y + _seg_gap;
        x1 = x0 + _thick;
        y1 = y0 + _thick;
        x2 = x1;
        y2 = y + h2 - thick2 - _seg_gap;
        x3 = x0;
        y3 = y2 + thick2;
        fl_polygon(x0, y0, x1, y1, x2, y2, x3, y3);
    }
    if (a & 0x01) {
        x0 = x + _seg_gap;
        y0 = y + h2;
        x1 = x0 + _thick;
        y1 = y0 - thick2;
        x2 = x1;
        y2 = y1 + _thick;
        fl_polygon(x0, y0, x1, y1, x2, y2);
        x0 = x1;
        y0 = y1;
        x1 = x0 + w - _thick * 2 - _seg_gap * 2;
        y1 = y0;
        x2 = x1;
        y2 = y1 + _thick;
        x3 = x0;
        y3 = y2;
        fl_polygon(x0, y0, x1, y1, x2, y2, x3, y3);
        x0 = x + w - _seg_gap;
        y0 = y + h2;
        x1 = x0 - _thick;
        y1 = y0 - thick2;
        x2 = x1;
        y2 = y1 + _thick;
        fl_polygon(x0, y0, x1, y1, x2, y2);
    }
}
void flw::LcdNumber::value(const char *value) {
    size_t l = value ? strlen(value) : 0;
    if (l && l < 100) {
        for (size_t i = 0; i < l; i++) {
            if (value[i] >= 0x30 && value[i] <= 0x39) {
                _value[i] = value[i] - 0x2F;
            }
            else if (value[i] >= 0x41 && value[i] <= 0x46) {
                _value[i] = value[i] - 0x36;
            }
            else if (value[i] >= 0x61 && value[i] <= 0x66) {
                _value[i] = value[i] - 0x56;
            }
            else if (value[i] == '-') {
                _value[i] = 0x11;
            }
            else if (value[i] == ' ') {
                _value[i] = 0x13;
            }
            else if (value[i] == '.') {
                _value[i] = 0x12;
            }
            else if (value[i] == ':') {
                _value[i] = 0x12;
            }
            else {
                _value[i] = 0x13;
            }
            _value[i + 1] = 0;
        }
    }
    else {
        *_value = 0;
    }
    Fl::redraw();
}
#include <assert.h>
#include <FL/fl_ask.H>
#include <FL/Fl_File_Chooser.H>
namespace flw {
    namespace logdisplay {
        #define STYLE_ATTRIBUTE 0, 0
        #define STYLE_ATTRIBUTE2 Fl_Text_Display::ATTR_BGCOLOR, 0
        Fl_Color BG_COLOR      = fl_lighter(FL_GRAY);
        Fl_Color BG_DARK_COLOR = FL_WHITE;
        size_t   MAX_LINE_LEN  = 1000;
        size_t   TURN_WRAP_ON  =  300;
        Fl_Text_Display::Style_Table_Entry STYLE_TABLE[] = {
            { FL_FOREGROUND_COLOR,  FL_COURIER,         14, STYLE_ATTRIBUTE },
            { FL_GRAY,              FL_COURIER,         14, STYLE_ATTRIBUTE },
            { FL_RED,               FL_COURIER,         14, STYLE_ATTRIBUTE },
            { FL_DARK_GREEN,        FL_COURIER,         14, STYLE_ATTRIBUTE },
            { FL_BLUE,              FL_COURIER,         14, STYLE_ATTRIBUTE },
            { FL_MAGENTA,           FL_COURIER,         14, STYLE_ATTRIBUTE },
            { FL_DARK_YELLOW,       FL_COURIER,         14, STYLE_ATTRIBUTE },
            { FL_CYAN,              FL_COURIER,         14, STYLE_ATTRIBUTE },
            { FL_FOREGROUND_COLOR,  FL_COURIER_BOLD,    14, STYLE_ATTRIBUTE },
            { FL_GRAY,              FL_COURIER_BOLD,    14, STYLE_ATTRIBUTE },
            { FL_RED,               FL_COURIER_BOLD,    14, STYLE_ATTRIBUTE },
            { FL_DARK_GREEN,        FL_COURIER_BOLD,    14, STYLE_ATTRIBUTE },
            { FL_BLUE,              FL_COURIER_BOLD,    14, STYLE_ATTRIBUTE },
            { FL_MAGENTA,           FL_COURIER_BOLD,    14, STYLE_ATTRIBUTE },
            { FL_DARK_YELLOW,       FL_COURIER_BOLD,    14, STYLE_ATTRIBUTE },
            { FL_CYAN,              FL_COURIER_BOLD,    14, STYLE_ATTRIBUTE },
            { FL_FOREGROUND_COLOR,  FL_COURIER,         14, STYLE_ATTRIBUTE2 },
            { FL_GRAY,              FL_COURIER,         14, STYLE_ATTRIBUTE2 },
            { FL_RED,               FL_COURIER,         14, STYLE_ATTRIBUTE2 },
            { FL_DARK_GREEN,        FL_COURIER,         14, STYLE_ATTRIBUTE2 },
            { FL_BLUE,              FL_COURIER,         14, STYLE_ATTRIBUTE2 },
            { FL_MAGENTA,           FL_COURIER,         14, STYLE_ATTRIBUTE2 },
            { FL_DARK_YELLOW,       FL_COURIER,         14, STYLE_ATTRIBUTE2 },
            { FL_CYAN,              FL_COURIER,         14, STYLE_ATTRIBUTE2 },
            { FL_FOREGROUND_COLOR,  FL_COURIER_BOLD,    14, STYLE_ATTRIBUTE2 },
            { FL_GRAY,              FL_COURIER_BOLD,    14, STYLE_ATTRIBUTE2 },
            { FL_RED,               FL_COURIER_BOLD,    14, STYLE_ATTRIBUTE2 },
            { FL_DARK_GREEN,        FL_COURIER_BOLD,    14, STYLE_ATTRIBUTE2 },
            { FL_BLUE,              FL_COURIER_BOLD,    14, STYLE_ATTRIBUTE2 },
            { FL_MAGENTA,           FL_COURIER_BOLD,    14, STYLE_ATTRIBUTE2 },
            { FL_DARK_YELLOW,       FL_COURIER_BOLD,    14, STYLE_ATTRIBUTE2 },
            { FL_CYAN,              FL_COURIER_BOLD,    14, STYLE_ATTRIBUTE2 },
        };
        static const std::string JSON_EXAMPLE = R"(Example json string
All available options are below.
Use one or all.
{
    "lock": {
        "on": true
    },
    "line": {
        "start": 0,
        "stop": 10,
        "color": "RED"
    },
    "num": {
        "color": "MAGENTA",
        "count": 0
    },
    "string": {
        "word1": "find_text_from_left",
        "color": "BLUE",
        "count": 0
    },
    "rstring": {
        "word1": "find_text_from_right",
        "color": "BLUE",
        "count": 1
    },
    "range": {
        "word1": "from_string",
        "word2": "to_string",
        "inclusive": true,
        "color": "BLUE",
        "count": 0
    },
    "between": {
        "word1": "from_first_string",
        "word2": "to_last_string",
        "inclusive": true,
        "color": "BLUE",
        "count": 0
    },
    "custom": {
        "word1": "string_1",
        "word2": "string_2",
        "start": 0,
        "stop": 10,
        "inclusive": true,
        "color": "BLUE",
        "count": 0
    }
}
    )";
        static const std::string HELP = R"(@bSet style colors
All options will be called for every line.
Text must be valid JSON wrapped within [].
Count property is how many strings to color, 0 means all.
If inclusive is set to false only characters between found strings will be colored.

If lock is true then never change colors that have been set.
@f"lock": {
@f    "on": true
@f}

Color characters by using index in line.
@f"line": {
@f    "start": 0,
@f    "stop": 10,
@f    "color": "RED"
@f}

Color all numbers.
@f"num": {
@f    "color": "MAGENTA",
@f    "count": 0
@f}

Color strings.
@f"string": {
@f    "word1": "find_text_from_left",
@f    "color": "BLUE",
@f    "count": 0
@f}

Color string but start from the right.
@f"rstring": {
@f    "word1": "find_text_from_right",
@f    "color": "BLUE",
@f    "count": 0
@f}

Color text between two strings.
@f"range": {
@f    "word1": "from_string",
@f    "word2": "to_string",
@f    "inclusive": true,
@f    "color": "BLUE",
@f    "count": 0
@f}

Color text from first found string to the last found string.
@f"between": {
@f    "word1": "from_first_string",
@f    "word2": "to_last_string",
@f    "inclusive": true,
@f    "color": "BLUE"
@f}

This property will call LogDisplay::line_custom_cb() which does nothing so override it.
@f"custom": {
@f    "word1": "string_1",
@f    "word2": "string_2",
@f    "start": 0,
@f    "stop": 10,
@f    "inclusive": true,
@f    "color": "BLUE"
@f}

@bValid colors
FOREGROUND
GRAY
RED
GREEN
BLUE
MAGENTA
YELLOW
CYAN
BOLD_FOREGROUND
BOLD_GRAY
BOLD_RED
BOLD_GREEN
BOLD_BLUE
BOLD_MAGENTA
BOLD_YELLOW
BOLD_CYAN
BG_FOREGROUND
BG_GRAY
BG_RED
BG_GREEN
BG_BLUE
BG_MAGENTA
BG_YELLOW
BG_CYAN
BG_BOLD_FOREGROUND
BG_BOLD_GRAY
BG_BOLD_RED
BG_BOLD_GREEN
BG_BOLD_BLUE
BG_BOLD_MAGENTA
BG_BOLD_YELLOW
BG_BOLD_CYAN

    )";
        static const std::string TOOLTIP = R"(Ctrl + 'f' for enter search text.
F3 to search for next word.
Shift + F3 to search for previous word.
Ctrl + 'h' to display style string help.
Ctrl + 'e' to edit style string.
    )";
        enum class STYLE {
            EMPTY,
            LOCK,
            LINE,
            STRING,
            RSTRING,
            RANGE,
            NUM,
            BETWEEN,
            CUSTOM,
        };
        struct Style {
            COLOR               color;
            size_t              count;
            bool                inclusive;
            bool                on;
            size_t              start;
            size_t              stop;
            STYLE               style;
            std::string         word1;
            std::string         word2;
            Style() {
                color     = COLOR::FOREGROUND;
                count     = ALL_STRINGS;
                inclusive = false;
                on        = false;
                start     = 0;
                stop      = 0;
                style     = STYLE::EMPTY;
            }
        };
        static COLOR convert_color_string(std::string name) {
            if (name == "GRAY") return COLOR::GRAY;
            else if (name == "RED") return COLOR::RED;
            else if (name == "GREEN") return COLOR::GREEN;
            else if (name == "BLUE") return COLOR::BLUE;
            else if (name == "MAGENTA") return COLOR::MAGENTA;
            else if (name == "YELLOW") return COLOR::YELLOW;
            else if (name == "CYAN") return COLOR::CYAN;
            else if (name == "BOLD_FOREGROUND") return COLOR::BOLD_FOREGROUND;
            else if (name == "BOLD_GRAY") return COLOR::BOLD_GRAY;
            else if (name == "BOLD_RED") return COLOR::BOLD_RED;
            else if (name == "BOLD_GREEN") return COLOR::BOLD_GREEN;
            else if (name == "BOLD_BLUE") return COLOR::BOLD_BLUE;
            else if (name == "BOLD_MAGENTA") return COLOR::BOLD_MAGENTA;
            else if (name == "BOLD_YELLOW") return COLOR::BOLD_YELLOW;
            else if (name == "BOLD_CYAN") return COLOR::BOLD_CYAN;
            else if (name == "BG_FOREGROUND") return COLOR::BG_FOREGROUND;
            else if (name == "BG_GRAY") return COLOR::BG_GRAY;
            else if (name == "BG_RED") return COLOR::BG_RED;
            else if (name == "BG_GREEN") return COLOR::BG_GREEN;
            else if (name == "BG_BLUE") return COLOR::BG_BLUE;
            else if (name == "BG_MAGENTA") return COLOR::BG_MAGENTA;
            else if (name == "BG_YELLOW") return COLOR::BG_YELLOW;
            else if (name == "BG_CYAN") return COLOR::BG_CYAN;
            else if (name == "BG_BOLD_FOREGROUND") return COLOR::BG_BOLD_FOREGROUND;
            else if (name == "BG_BOLD_GRAY") return COLOR::BG_BOLD_GRAY;
            else if (name == "BG_BOLD_RED") return COLOR::BG_BOLD_RED;
            else if (name == "BG_BOLD_GREEN") return COLOR::BG_BOLD_GREEN;
            else if (name == "BG_BOLD_BLUE") return COLOR::BG_BOLD_BLUE;
            else if (name == "BG_BOLD_MAGENTA") return COLOR::BG_BOLD_MAGENTA;
            else if (name == "BG_BOLD_YELLOW") return COLOR::BG_BOLD_YELLOW;
            else if (name == "BG_BOLD_CYAN") return COLOR::BG_BOLD_CYAN;
            else if (name == "BG_BOLD_CYAN") return COLOR::BG_BOLD_CYAN;
            else return COLOR::GRAY;
        }
        static std::vector<Style> parse_json(std::string json) {
            auto nv  = json::NodeVector();
            auto err = json::parse(json, nv);
            auto res = std::vector<Style>();
            if (err.pos >= 0) {
                fl_alert("error: failed to parse json string at line %d and byte %d", (int) err.line, (int) err.pos);
                return res;
            }
            else if (nv.size() < 3) {
                fl_alert("error: nothing to parse");
                return res;
            }
            for (const auto& n : json::find_children(nv, nv[0])) {
                if (n.is_object() == true) {
                    auto style = Style();
                    if (n.name == "lock") {
                        style.style = STYLE::LOCK;
                    }
                    else if (n.name == "line") {
                        style.style = STYLE::LINE;
                    }
                    else if (n.name == "num") {
                        style.style = STYLE::NUM;
                    }
                    else if (n.name == "string") {
                        style.style = STYLE::STRING;
                    }
                    else if (n.name == "rstring") {
                        style.style = STYLE::RSTRING;
                    }
                    else if (n.name == "between") {
                        style.style = STYLE::BETWEEN;
                    }
                    else if (n.name == "range") {
                        style.style = STYLE::RANGE;
                    }
                    else if (n.name == "custom") {
                        style.style = STYLE::CUSTOM;
                    }
                    auto children = json::find_children(nv, n);
                    for (const auto& n2 : children) {
                        if (n2.name == "on" && n2.is_bool() == true) {
                            style.on = n2.tobool();
                        }
                        else if (n2.name == "inclusive" && n2.is_bool() == true) {
                            style.inclusive = n2.tobool();
                        }
                        else if (n2.name == "start" && n2.is_number() == true) {
                            style.start = (size_t) n2.toint();
                        }
                        else if (n2.name == "stop" && n2.is_number() == true) {
                            style.stop = (size_t) n2.toint();
                        }
                        else if (n2.name == "count" && n2.is_number() == true) {
                            style.count = (size_t) n2.toint();
                        }
                        else if (n2.name == "color" && n2.is_string() == true) {
                            style.color = convert_color_string(n2.value);
                        }
                        else if (n2.name == "word1" && n2.is_string() == true) {
                            style.word1 = n2.tostring();
                        }
                        else if (n2.name == "word2" && n2.is_string() == true) {
                            style.word2 = n2.tostring();
                        }
                    }
                    if (style.style != STYLE::EMPTY) {
                        res.push_back(style);
                    }
                }
            }
            return res;
        }
        char* win_to_unix(const char* string) {
            auto r = false;
            auto b = string;
            while (*b != 0) {
                if (*b++ == '\r') {
                    r = true;
                    break;
                }
            }
            if (r == false) {
                return nullptr;
            }
            auto len = strlen(string);
            auto res = (char*) calloc(len + 1, 1);
            auto pos = 0;
            if (res != nullptr) {
                b = string;
                while (*b != 0) {
                    if (*b != '\r') {
                        res[pos++] = *b;
                    }
                    b++;
                }
            }
            return res;
        }
    }
}
flw::LogDisplay::LogDisplay::Tmp::Tmp() {
    buf  = nullptr;
    len  = 0;
    pos  = 0;
    size = 0;
}
flw::LogDisplay::LogDisplay::Tmp::~Tmp() {
    free(buf);
}
flw::LogDisplay::LogDisplay(int x, int y, int w, int h, const char *l) : Fl_Text_Display(x, y, w, h, l) {
    _buffer      = new Fl_Text_Buffer();
    _style       = new Fl_Text_Buffer();
    _lock_colors = false;
    _tmp         = nullptr;
    buffer(_buffer);
    linenumber_align(FL_ALIGN_RIGHT);
    linenumber_format("%5d");
    update_pref();
    tooltip(logdisplay::TOOLTIP.c_str());
}
flw::LogDisplay::~LogDisplay() {
    buffer(nullptr);
    delete _buffer;
    delete _style;
    delete _tmp;
}
void flw::LogDisplay::edit_styles() {
    auto json    = (_json == "") ? logdisplay::JSON_EXAMPLE : _json;
    auto changed = dlg::text_edit("Edit JSON Style String", json, top_window(), 40, 50);
    if (changed == false) {
        return;
    }
    style(json);
}
void flw::LogDisplay::find(bool next, bool force_ask) {
    if (_find == "" || force_ask) {
        auto s = fl_input("Enter search string", _find.c_str());
        if (s == nullptr || *s == 0) {
            return;
        }
        _find = s;
    }
    auto found = false;
    auto pos   = 0;
    auto start = 0;
    auto end   = 0;
    auto sel   = _buffer->selection_position(&start, &end) != 0;
    if (sel == true) {
        pos = (next == true) ? end : start - 1;
    }
    else {
        pos = 0;
    }
    if (next == true) {
        found = _buffer->search_forward(pos, _find.c_str(), &pos, 1);
    }
    else {
        found = _buffer->search_backward(pos, _find.c_str(), &pos, 1);
    }
    if (found == false && sel == true) {
        if (next == true) {
            found = _buffer->search_forward(0, _find.c_str(), &pos, 1);
        }
        else {
            found = _buffer->search_backward(_buffer->length(), _find.c_str(), &pos, 1);
        }
    }
    if (found == false) {
        fl_beep(FL_BEEP_ERROR);
        return;
    }
    _buffer->select(pos, pos + _find.length());
    insert_position(pos);
    show_insert_position();
}
int flw::LogDisplay::handle(int event) {
    if (event == FL_KEYBOARD) {
        auto key = Fl::event_key();
        if (Fl::event_ctrl() != 0) {
            if (key == 'f') {
                _buffer->unselect();
                find(true, true);
                return 1;
            }
            else if (key == 'e') {
                edit_styles();
                return 1;
            }
            else if (key == 's') {
                save_file();
                return 1;
            }
            else if (key == 'h') {
                dlg::list("Style Help", logdisplay::HELP, top_window(), false, 40, 30);
                return 1;
            }
        }
        else if (Fl::event_shift() != 0 && key == FL_F + 3) {
            find(false, false);
        }
        else if (key == FL_F + 3) {
            find(true, false);
        }
    }
    return Fl_Text_Display::handle(event);
}
void flw::LogDisplay::save_file() {
    auto filename = fl_file_chooser("Select Destination File", nullptr, nullptr, 0);
    if (filename != nullptr && _buffer->savefile(filename) != 0) {
        fl_alert("error: failed to save text to %s", filename);
    }
}
void flw::LogDisplay::style(std::string json) {
    auto row = 1;
    auto ds  = (json != "") ? logdisplay::parse_json(json) : std::vector<logdisplay::Style>();
    _json      = json;
    _tmp       = new Tmp();
    _tmp->size = _buffer->length();
    _tmp->buf  = (char*) calloc(_tmp->size + 1, 1);
    if (_tmp->buf != nullptr) {
        memset(_tmp->buf, 'A', _tmp->size);
        while (_tmp->pos < (size_t) _buffer->length()) {
            auto line = _buffer->line_text(_tmp->pos);
            _tmp->len = strlen(line);
            if (_json == "") {
                line_cb(row, line);
            }
            else {
                lock_colors(false);
                for (const auto& d : ds) {
                    if (d.style == logdisplay::STYLE::LOCK) {
                        lock_colors(d.on);
                    }
                    else if (d.style == logdisplay::STYLE::LINE) {
                        style_line(d.start, d.stop, d.color);
                    }
                    else if (d.style == logdisplay::STYLE::STRING) {
                        style_string(line, d.word1, d.color, d.count);
                    }
                    else if (d.style == logdisplay::STYLE::RSTRING) {
                        style_rstring(line, d.word1, d.color, d.count);
                    }
                    else if (d.style == logdisplay::STYLE::RANGE) {
                        style_range(line, d.word1, d.word2, d.inclusive, d.color, d.count);
                    }
                    else if (d.style == logdisplay::STYLE::NUM) {
                        style_num(line, d.color, d.count);
                    }
                    else if (d.style == logdisplay::STYLE::BETWEEN) {
                        style_range(line, d.word1, d.word2, d.inclusive, d.color);
                    }
                    else if (d.style == logdisplay::STYLE::CUSTOM) {
                        line_custom_cb(row, line, d.word1, d.word2, d.color, d.inclusive, d.start, d.stop, d.count);
                    }
                }
            }
            _tmp->pos += _tmp->len + 1;
            row += 1;
            free(line);
        }
        _style->text(_tmp->buf);
        highlight_data(_style, logdisplay::STYLE_TABLE, sizeof(logdisplay::STYLE_TABLE) / sizeof(logdisplay::STYLE_TABLE[0]), (char) logdisplay::COLOR::FOREGROUND, nullptr, 0);
    }
    delete _tmp;
    _tmp = nullptr;
}
void flw::LogDisplay::style_between(const std::string& line, const std::string& word1, const std::string& word2, bool inclusive, logdisplay::COLOR color) {
    if (word1 == "" || word2 == "") {
        return;
    }
    auto pos_left  = line.find(word1);
    auto pos_right = line.rfind(word2);
    if (pos_left != std::string::npos && pos_right != std::string::npos && pos_left < pos_right) {
        if (inclusive == false && pos_right - pos_left > 1) {
            style_line(pos_left + 1, pos_right - 1, color);
        }
        else if (inclusive == true) {
            style_line(pos_left, pos_right, color);
        }
    }
}
void flw::LogDisplay::style_line(size_t start, size_t stop, logdisplay::COLOR c) {
    assert(_tmp);
    start += _tmp->pos;
    stop  += _tmp->pos;
    while (start <= stop && start < _tmp->size && start < _tmp->pos + _tmp->len) {
        if (_lock_colors == false || _tmp->buf[start] == (char) logdisplay::COLOR::FOREGROUND) {
            _tmp->buf[start] = (char) c;
        }
        start++;
    }
}
void flw::LogDisplay::style_num(const std::string& line, logdisplay::COLOR color, size_t count) {
    if (count == 0) {
        count = 999;
    }
    for (size_t f = 0; f < line.length() && count > 0; f++) {
        auto c = line[f];
        if (c >= '0' && c <= '9') {
            style_line(f, f, color);
            count--;
        }
    }
}
void flw::LogDisplay::style_range(const std::string& line, const std::string& word1, const std::string& word2, bool inclusive, logdisplay::COLOR color, size_t count) {
    if (word1 == "" || word2 == "") {
        return;
    }
    auto pos_from = line.find(word1);
    if (pos_from != std::string::npos) {
        auto pos_to = line.find(word2, pos_from + word1.length());
        if (count == 0) {
            count = 999;
        }
        while (pos_from != std::string::npos && pos_to != std::string::npos && count > 0) {
            if (inclusive == false) {
                style_line(pos_from + word1.length(), pos_to - 1, color);
            }
            else if (inclusive == true) {
                style_line(pos_from, pos_to + word2.length() - 1, color);
            }
            pos_from = line.find(word1, pos_to + word2.length());
            if (pos_from != std::string::npos) {
                pos_to = line.find(word2, pos_from + word1.length());
            }
            else {
                break;
            }
            count--;
        }
    }
}
void flw::LogDisplay::style_rstring(const std::string& line, const std::string& word1, logdisplay::COLOR color, size_t count) {
    if (word1 == "") {
        return;
    }
    auto pos = line.rfind(word1);
    if (count == 0) {
        count = 999;
    }
    while (pos != std::string::npos && count > 0) {
        style_line(pos, pos + word1.length() - 1, color);
        if (pos == 0) {
            break;
        }
        pos = line.rfind(word1, pos - 1);
        count--;
    }
}
void flw::LogDisplay::style_string(const std::string& line, const std::string& word1, logdisplay::COLOR color, size_t count) {
    if (word1 == "") {
        return;
    }
    auto pos = line.find(word1);
    if (count == 0) {
        count = 999;
    }
    while (pos != std::string::npos && count > 0) {
        style_line(pos, pos + word1.length() - 1, color);
        pos = line.find(word1, pos + word1.length());
        count--;
    }
}
void flw::LogDisplay::update_pref() {
    labelsize(flw::PREF_FIXED_FONTSIZE);
    linenumber_bgcolor(FL_BACKGROUND_COLOR);
    linenumber_fgcolor(FL_FOREGROUND_COLOR);
    linenumber_font(flw::PREF_FIXED_FONT);
    linenumber_size(flw::PREF_FIXED_FONTSIZE);
    linenumber_width(flw::PREF_FIXED_FONTSIZE * 3);
    textfont(flw::PREF_FIXED_FONT);
    textsize(flw::PREF_FIXED_FONTSIZE);
    for (size_t f = 0; f < sizeof(logdisplay::STYLE_TABLE) / sizeof(logdisplay::STYLE_TABLE[0]); f++) {
        logdisplay::STYLE_TABLE[f].size = flw::PREF_FIXED_FONTSIZE;
#if FL_MINOR_VERSION == 4
        if (theme::is_dark() == true) {
            logdisplay::STYLE_TABLE[f].bgcolor = logdisplay::BG_DARK_COLOR;
        }
        else {
            logdisplay::STYLE_TABLE[f].bgcolor = logdisplay::BG_COLOR;
        }
#endif
    }
}
void flw::LogDisplay::value(const char* text) {
    assert(text);
    _buffer->text("");
    _style->text("");
    auto win = logdisplay::win_to_unix(text);
    if (win != nullptr) {
        _buffer->text(win);
        free(win);
    }
    else {
        _buffer->text(text);
    }
}
#include <assert.h>
#include <math.h>
#include <FL/fl_ask.H>
#include <FL/fl_draw.H>
namespace flw {
    namespace plot {
        static const char* const        SHOW_LABELS     = "Show line labels";
        static const char* const        SHOW_HLINES     = "Show horizontal lines";
        static const char* const        SHOW_VLINES     = "Show vertical lines";
        static const char* const        SAVE_FILE       = "Save to png file...";
        int count_decimals(double number) {
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
        std::string format_double(double num, int decimals, char del) {
            char res[100];
            *res = 0;
            if (decimals < 0) {
                decimals = plot::count_decimals(num);
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
        bool has_pairs(TYPE type) {
            return type != TYPE::CIRCLE && type != TYPE::FILLED_CIRCLE && type != TYPE::SQUARE;
        }
        bool has_radius(TYPE type) {
            return type == TYPE::CIRCLE || type == TYPE::FILLED_CIRCLE || type == TYPE::SQUARE;
        }
        bool load_data(Plot* plot, std::string filename) {
            auto buf = util::load_file(filename);
            auto nv  = json::NodeVector();
            auto ok  = 0;
            auto x   = Scale();
            auto y   = Scale();
            auto l   = false;
            auto h   = false;
            auto v   = false;
            plot->clear();
            plot->redraw();
            if (buf.p == nullptr) {
                fl_alert("error: failed to load %s", filename.c_str());
                return false;
            }
            auto err = json::parse(buf.p, nv, true);
            if (err.pos >= 0) {
                fl_alert("error: failed to parse %s on line %d and byte %d", filename.c_str(), (int) err.line, (int) err.pos);
                return false;
            }
            for (const auto& n : json::find_children(nv, nv[0])) {
                auto children = json::find_children(nv, n);
                if (n.name == "descr" && n.is_object() == true) {
                    for (auto& n2 : children) {
                        if ((n2.name == "type" && n2.value == "flw::plot") || (n2.name == "version" && n2.value == "1")) {
                            ok++;
                        }
                    }
                }
                else if (ok != 2) {
                    fl_alert("error: unknown file format");
                    return false;
                }
                else if ((n.name == "xscale" || n.name == "yscale") && n.is_object() == true) {
                    auto& scale = (n.name == "xscale") ? x : y;
                    for (auto& n2 : children) {
                        if (n2.name == "label") {
                            scale.label = n2.value;
                        }
                        else if (n2.name == "color") {
                            scale.color = (Fl_Color) n2.toint();
                        }
                        else if (n2.name == "labels") {
                            for (auto& n3 : json::find_children(nv, n2)) {
                                if (n3.is_string() == true) {
                                    scale.labels.push_back(n3.value);
                                }
                            }
                        }
                    }
                }
                else if (n.name == "view" && n.is_object() == true) {
                    for (auto& n2 : children) {
                        if (n2.name == "labels") {
                            l = (bool) n2.tobool();
                        }
                        else if (n2.name == "horizontal") {
                            h = (bool) n2.tobool();
                        }
                        else if (n2.name == "vertical") {
                            v = (bool) n2.tobool();
                        }
                    }
                }
                else if (n.name == "line" && n.is_object() == true) {
                    plot::Line line;
                    for (auto& n2 : children) {
                        if (n2.name == "width" && n2.is_number() == true) {
                            line.width = (unsigned) n2.toint();
                        }
                        else if (n2.name == "color" && n2.is_number() == true) {
                            line.color = (Fl_Color) n2.toint();
                        }
                        else if (n2.name == "type" && n2.is_string() == true) {
                            line.type = plot::string_to_type(n2.value);
                        }
                        else if (n2.name == "label" && n2.is_string() == true) {
                            line.label = n2.value;
                        }
                        else if (n2.name == "p" && n2.is_array()) {
                            auto p = json::find_children(nv, n2);
                            if (p.size() == 2) {
                                line.points.push_back(plot::Point(p[0].tonumber(), p[1].tonumber()));
                            }
                        }
                    }
                    if (plot->add_line(line) == false) {
                        fl_alert("error: failed to add line that started on position %u", (unsigned) n.textpos);
                        plot->clear();
                        return false;
                    }
                }
            }
            plot->labels(x.label, y.label);
            plot->label_colors(x.color, y.color);
            plot->view_options(l, h, v);
            plot->custom_xlabels_for_points0(x.labels);
            plot->custom_ylabels_for_points0(y.labels);
            plot->resize();
            return true;
        }
        void print(const PointVector& points) {
            auto c = 1;
            for (auto& p : points) {
                fprintf(stderr, "%3d| X=%16.6f   Y=%16.6f\n", c++, p.x, p.y);
            }
            fflush(stderr);
        }
        bool save_data(const Plot* plot, std::string filename) {
            json::NodeVector nv;
            FLW_JSON_START(nv,
                FLW_JSON_START_OBJECT(
                    FLW_JSON_ADD_OBJECT("descr",
                        FLW_JSON_ADD_STRING("type", "flw::plot")
                        FLW_JSON_ADD_UINT("version", 1)
                    )
                    FLW_JSON_ADD_OBJECT("view",
                        FLW_JSON_ADD_BOOL("labels", plot->_view.labels)
                        FLW_JSON_ADD_BOOL("horizontal", plot->_view.horizontal)
                        FLW_JSON_ADD_BOOL("vertical", plot->_view.vertical)
                    )
                    FLW_JSON_ADD_OBJECT("xscale",
                        FLW_JSON_ADD_STRING("label", plot->_x.label)
                        FLW_JSON_ADD_UINT("color", plot->_x.color)
                        FLW_JSON_ADD_ARRAY_NL("labels",
                            for (const auto& l : plot->_x.labels)
                            FLW_JSON_ADD_STRING_TO_ARRAY(l)
                        )
                    )
                    FLW_JSON_ADD_OBJECT("yscale",
                        FLW_JSON_ADD_STRING("label", plot->_y.label)
                        FLW_JSON_ADD_UINT("color", plot->_y.color)
                        FLW_JSON_ADD_ARRAY_NL("labels",
                            for (const auto& l : plot->_y.labels)
                            FLW_JSON_ADD_STRING_TO_ARRAY(l)
                        )
                    )
                    for (size_t f = 0; f < plot->_size; f++) {
                    const auto& line = plot->_lines[f];
                    if (line.points.size() > 0) {
                    FLW_JSON_ADD_OBJECT("line",
                        FLW_JSON_ADD_UINT("width", line.width)
                        FLW_JSON_ADD_UINT("color", line.color)
                        FLW_JSON_ADD_STRING("type", plot::type_to_string(line.type))
                        FLW_JSON_ADD_STRING("label", line.label)
                        for (const auto& point : line.points) {
                        FLW_JSON_ADD_ARRAY_NL("p",
                            FLW_JSON_ADD_NUMBER_TO_ARRAY(point.x)
                            FLW_JSON_ADD_NUMBER_TO_ARRAY(point.y)
                        )
                        }
                    )
                    }
                    }
                )
            )
            auto js = json::tostring(nv);
            return util::save_file(filename, js.c_str(), js.length());
        }
        TYPE string_to_type(std::string name) {
            if (name == "TYPE::LINE_DASH") return TYPE::LINE_DASH;
            else if (name == "TYPE::LINE_DOT") return TYPE::LINE_DOT;
            else if (name == "TYPE::LINE_WITH_SQUARE") return TYPE::LINE_WITH_SQUARE;
            else if (name == "TYPE::VECTOR") return TYPE::VECTOR;
            else if (name == "TYPE::CIRCLE") return TYPE::CIRCLE;
            else if (name == "TYPE::FILLED_CIRCLE") return TYPE::FILLED_CIRCLE;
            else if (name == "TYPE::SQUARE") return TYPE::SQUARE;
            else return TYPE::LINE;
        }
        std::string type_to_string(TYPE type) {
            assert((size_t) type <= 8);
            static const char* NAMES[] = { "TYPE::LINE", "TYPE::LINE_DASH", "TYPE::LINE_DOT", "TYPE::LINE_WITH_SQUARE", "TYPE::VECTOR", "TYPE::CIRCLE", "TYPE::FILLED_CIRCLE", "TYPE::SQUARE", "", "", };
            return NAMES[(size_t) type];
        }
    }
}
flw::plot::Scale::Scale() {
    color  = FL_FOREGROUND_COLOR;
    fr     = 0;
    label  = "";
    labels = StringVector();
    max    = 0.0;
    max2   = 0.0;
    min    = 0.0;
    min2   = 0.0;
    pixel  = 0.0;
    text   = 0;
    tick   = 0.0;
}
void flw::plot::Scale::calc(double canvas_size) {
    double range = max - min;
    tick  = 0.0;
    pixel = 0.0;
    if (range > 0.000000001) {
        double test_inc = 0.0;
        double test_min = 0.0;
        double test_max = 0.0;
        int    ticker   = 0;
        test_inc = pow(10.0, ceil(log10(range / 10.0)));
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
        min  = test_min;
        max  = test_max;
        tick = test_inc;
        if (tick > 0.000000001) {
            pixel = canvas_size / (max - min);
        }
    }
    fr = plot::count_decimals(tick);
}
void flw::plot::Scale::debug(const char* s) const {
#ifdef DEBUG
    fprintf(stderr, "\tflw::plot::Scale: %s\n", s);
    fprintf(stderr, "\t\tcolor  = %u\n", color);
    fprintf(stderr, "\t\tfr     = %d\n", fr);
    fprintf(stderr, "\t\tlabel  = %s\n", label.c_str());
    fprintf(stderr, "\t\tlabels = %d strings\n", (int) labels.size());
    fprintf(stderr, "\t\tmax    = %16.8f\n", max);
    fprintf(stderr, "\t\tmax2   = %16.8f\n", max2);
    fprintf(stderr, "\t\tmin    = %16.8f\n", min);
    fprintf(stderr, "\t\tmin2   = %16.8f\n", min2);
    fprintf(stderr, "\t\tpixel  = %16.8f\n", pixel);
    fprintf(stderr, "\t\ttext   = %d px\n", text);
    fprintf(stderr, "\t\ttick   = %16.8f\n", tick);
    fflush(stderr);
#else
    (void) s;
#endif
}
void flw::plot::Scale::measure_text(int cw) {
    if (labels.size() > 0) {
        text = 0;
        for (auto& l : labels) {
            if ((int) l.length() > text) {
                text = l.length();
            }
        }
        text *= cw;
    }
    else {
        const auto dec = (fr > 0) ? fr : -1;
        const auto l1  = plot::format_double(min, dec, ' ');
        const auto l2  = plot::format_double(max, dec, ' ');
        text = (l1.length() > l2.length()) ? l1.length() * cw : l2.length() * cw;
    }
}
void flw::plot::Scale::reset_min_max() {
    min = min2 = plot::MAX;
    max = max2 = plot::MIN;
}
flw::plot::Line::Line() {
    color   = FL_BLUE;
    type    = plot::TYPE::LINE;
    visible = true;
    width   = 1;
}
flw::Plot::Plot(int X, int Y, int W, int H, const char* l) : Fl_Group(X, Y, W, H, l) {
    end();
    clip_children(1);
    color(FL_BACKGROUND2_COLOR);
    labelcolor(FL_FOREGROUND_COLOR);
    box(FL_BORDER_BOX);
    tooltip(
        "Press right button for menu.\n"
        "Press left button to show X, Y values and possible found point.\n"
        "Press ctrl + left button to increase match radius.\n"
        "Press 1 - 9 to select which line to use.\n"
        "Press shift + 1 - 9 to toggle line visibility."
    );
    _menu = new Fl_Menu_Button(0, 0, 0, 0);
    add(_menu);
    _menu->add(flw::plot::SHOW_LABELS, 0, Plot::_CallbackToggle, this, FL_MENU_TOGGLE);
    _menu->add(flw::plot::SHOW_HLINES, 0, Plot::_CallbackToggle, this, FL_MENU_TOGGLE);
    _menu->add(flw::plot::SHOW_VLINES, 0, Plot::_CallbackToggle, this, FL_MENU_TOGGLE | FL_MENU_DIVIDER);
    _menu->add(flw::plot::SAVE_FILE, 0, Plot::_CallbackSave, this);
#ifdef DEBUG
    _menu->add("Debug", 0, Plot::_CallbackDebug, this);
#endif
    _menu->type(Fl_Menu_Button::POPUP3);
    clear();
    update_pref();
    view_options();
}
bool flw::Plot::add_line(const flw::plot::PointVector& points, flw::plot::TYPE type, unsigned width, std::string label, Fl_Color color) {
    if (_size >= plot::MAX_LINES || width == 0 || width > 100) {
        return false;
    }
    _lines[_size].points    = points;
    _lines[_size].label     = label;
    _lines[_size].width     = width;
    _lines[_size].color     = color;
    _lines[_size].type      = type;
    _lines[_size++].visible = true;
    _calc                   = true;
    return true;
}
bool flw::Plot::add_line(const plot::Line& line) {
    if (_size >= plot::MAX_LINES || line.width < 1 || line.width > 100) {
        return false;
    }
    _lines[_size++] = line;
    return true;
}
void flw::Plot::_calc_min_max() {
    _x.reset_min_max();
    _y.reset_min_max();
    for (size_t f = 0; f < _size; f++) {
        for (auto& p : _lines[f].points) {
            if (p.x < _x.min) {
                _x.min = p.x;
            }
            if (p.x > _x.max) {
                _x.max = p.x;
            }
            if (p.y < _y.min) {
                _y.min = p.y;
            }
            if (p.y > _y.max) {
                _y.max = p.y;
            }
        }
    }
    _x.min2 = _x.min;
    _x.max2 = _x.max;
    _y.min2 = _y.min;
    _y.max2 = _y.max;
}
void flw::Plot::_CallbackDebug(Fl_Widget*, void* plot_object) {
    auto self = (Plot*) plot_object;
    self->debug();
}
void flw::Plot::_CallbackSave(Fl_Widget*, void* plot_object) {
    auto self = (Plot*) plot_object;
    flw::util::png_save("", self->window(), self->x(),  self->y(),  self->w(),  self->h());
}
void flw::Plot::_CallbackToggle(Fl_Widget*, void* plot_object) {
    auto self = (Plot*) plot_object;
    self->_view.labels     = flw::menu::item_value(self->_menu, plot::SHOW_LABELS);
    self->_view.horizontal = flw::menu::item_value(self->_menu, plot::SHOW_HLINES);
    self->_view.vertical   = flw::menu::item_value(self->_menu, plot::SHOW_VLINES);
    self->redraw();
}
void flw::Plot::clear() {
    _calc           = false;
    _ch             = 0;
    _ct             = 0;
    _cw             = 0;
    _h              = 0;
    _selected_line  = 0;
    _selected_point = -1;
    _size           = 0;
    _w              = 0;
    _x              = flw::plot::Scale();
    _y              = flw::plot::Scale();
    selection_color(FL_FOREGROUND_COLOR);
    clear_points();
}
void flw::Plot::clear_points() {
    for (size_t f = 0; f < _size; f++) {
        _lines[f] = plot::Line();
    }
}
void flw::Plot::_create_tooltip(bool ctrl) {
    const auto X   = Fl::event_x();
    const auto Y   = Fl::event_y();
    const auto old = _tooltip;
    _tooltip        = "";
    _selected_point = -1;
    if (X >= _area.x && X < _area.x2() && Y >= _area.y && Y < _area.y2()) {
        auto fr = (_x.fr > _y.fr) ? _x.fr : _y.fr;
        if (_selected_line < _size) {
            const plot::Line&        line   = _lines[_selected_line];
            const bool               radius = flw::plot::has_radius(line.type);
            const plot::PointVector& points = line.points;
            const int                rad    = (ctrl == true) ? (radius == true) ? line.width : line.width * 2 : (radius == true) ? line.width / 2 : line.width;
            const double             xv1    = ((double) (X - _area.x - rad) / _x.pixel) + _x.min;
            const double             xv2    = ((double) (X - _area.x + rad) / _x.pixel) + _x.min;
            const double             yv1    = ((double) (_area.y2() - Y - rad) / _y.pixel) + _y.min;
            const double             yv2    = ((double) (_area.y2() - Y + rad) / _y.pixel) + _y.min;
            for (size_t i = 0; i < points.size(); i++) {
                const plot::Point& point = points[i];
                if (point.x >= xv1 && point.x <= xv2 && point.y >= yv1 && point.y <= yv2) {
                    auto xl  = plot::format_double(point.x, fr, ' ');
                    auto yl  = plot::format_double(point.y, fr, ' ');
                    auto len = (xl.length() > yl.length()) ? xl.length() : yl.length();
                    _tooltip = flw::util::format("Line %d, Point %d\nX = %*s\nY = %*s", (int) _selected_line + 1, (int) i + 1, len, xl.c_str(), len, yl.c_str());
                    _selected_point = i;
                    break;
                }
            }
        }
        if (_tooltip == "") {
            auto xv  = ((double) (X - _area.x) / _x.pixel) + _x.min;
            auto yv  = ((double) (_area.y2() - Y) / _y.pixel) + _y.min;
            auto xl  = plot::format_double(xv, fr, ' ');
            auto yl  = plot::format_double(yv, fr, ' ');
            auto len = (xl.length() > yl.length()) ? xl.length() : yl.length();
            _tooltip = flw::util::format("X = %*s\nY = %*s", len, xl.c_str(), len, yl.c_str());
        }
    }
    if (_tooltip != "" || old != "") {
        redraw();
    }
}
void flw::Plot::debug() const {
#ifdef DEBUG
    fprintf(stderr, "\nflw::Plot: %d\n", (int) _size);
    fprintf(stderr, "\tcanvas          = (%d, %d) - (%d, %d) - (%d, %d)\n", x(), y(), w(), h(), x() + w(), y() + h());
    fprintf(stderr, "\tarea            = (%d, %d) - (%d, %d) - (%d, %d)\n", _area.x, _area.y, _area.w, _area.h, _area.x + _area.w, _area.y + _area.h);
    fprintf(stderr, "\tcw, ch          = (%d, %d)\n", _cw, _ch);
    fprintf(stderr, "\tselected_line   = %d\n", (int) _selected_line);
    fprintf(stderr, "\tselected_point  = %d\n", (int) _selected_point);
    fprintf(stderr, "\tline_count      = %d\n", (int) _size);
    fprintf(stderr, "\tview_labels     = %s\n", _view.labels ? "YES" : "NO");
    fprintf(stderr, "\tview_horizontal = %s\n", _view.horizontal ? "YES" : "NO");
    fprintf(stderr, "\tview_vertical   = %s\n", _view.vertical ? "YES" : "NO");
    _x.debug("X");
    _y.debug("Y");
    for (size_t f = 0; f < _size; f++) {
        fprintf(stderr, "\tflw::Plot::Line: %d\n", (int) f);
        fprintf(stderr, "\t\tcolor   = %u\n", _lines[f].color);
        fprintf(stderr, "\t\tlabel   = '%s'\n", _lines[f].label.c_str());
        fprintf(stderr, "\t\tpoints  = %d\n", (int) _lines[f].points.size());
        fprintf(stderr, "\t\ttype    = %d\n", (int) _lines[f].type);
        fprintf(stderr, "\t\tvisible = %s\n", _lines[f].visible ? "YES" : "NO");
        fprintf(stderr, "\t\twidth   = %d px\n", (int) _lines[f].width);
    }
    fflush(stderr);
#endif
}
void flw::Plot::draw() {
#ifdef DEBUG
#endif
    fl_font(flw::PREF_FIXED_FONT, flw::PREF_FIXED_FONTSIZE);
    Fl_Group::draw();
    if (_area.w < 50 || _area.h < 50 || _x.min >= _x.max || _x.pixel * _x.tick < 1.0 || _y.min >= _y.max || _y.pixel * _y.tick < 1.0) {
        return;
    }
    fl_push_clip(x(), y(), w(), h());
    _draw_xlabels();
    _draw_xlabels2();
    _draw_ylabels();
    _draw_ylabels2();
    _draw_labels();
    fl_pop_clip();
    fl_push_clip(_area.x, _area.y, _area.w, _area.h);
    _draw_lines();
    _draw_line_labels();
    _draw_tooltip();
    fl_pop_clip();
    fl_color(FL_FOREGROUND_COLOR);
    fl_line_style(FL_SOLID, 1);
    fl_rect(_area.x, _area.y, _area.w, _area.h, FL_FOREGROUND_COLOR);
    fl_line_style(0);
#ifdef DEBUG
#endif
}
void flw::Plot::_draw_labels() {
    if (_x.label != "") {
        fl_color(_x.color);
        fl_draw (_x.label.c_str(), _area.x, y() + h() - _ch, _area.w, flw::PREF_FIXED_FONTSIZE, FL_ALIGN_CENTER | FL_ALIGN_INSIDE);
    }
    if (_y.label != "") {
        auto y = _area.y + (_area.h / 2) + (_y.label.length() * _cw / 2);
        fl_color(_y.color);
        fl_draw (90, _y.label.c_str(), x() + _ch, y);
    }
}
void flw::Plot::_draw_line_labels() {
    if (_view.labels == true) {
        const int X = _area.x + 6;
        int       Y = _area.y + 6;
        int       W = 0;
        int       H = 0;
        for (size_t f = 0; f < _size; f++) {
            const auto& label = _lines[f].label;
            if (label != "") {
                H++;
                if ((int) label.length() * _cw > W) {
                    W = label.length() * _cw;
                }
            }
        }
        if (W > 0) {
            H *= flw::PREF_FIXED_FONTSIZE;
            W += _cw * 2;
            fl_color(FL_BACKGROUND2_COLOR);
            fl_line_style(FL_SOLID, 1);
            fl_rectf(X, Y, W + 8, H + 8);
            fl_color(FL_FOREGROUND_COLOR);
            fl_line_style(FL_SOLID, 1);
            fl_rect(X, Y, W + 8, H + 8);
            for (size_t f = 0; f < _size; f++) {
                const plot::Line& line = _lines[f];
                if (line.label != "") {
                    auto label = line.label;
                    if (f == _selected_line) {
                        label = "@-> " + label;
                    }
                    fl_color((line.visible == false) ? FL_GRAY : _lines[f].color);
                    fl_line_style(FL_SOLID, 1);
                    fl_draw(label.c_str(), X + 4, Y + 4, W, flw::PREF_FIXED_FONTSIZE, FL_ALIGN_LEFT | FL_ALIGN_INSIDE, nullptr, 1);
                    Y += flw::PREF_FIXED_FONTSIZE;
                }
            }
            fl_line_style(0);
        }
    }
}
void flw::Plot::_draw_lines() {
    const int X = _area.x;
    const int Y = _area.y2() - 1;
    for (size_t f = 0; f < _size; f++) {
        const plot::Line& data  = _lines[f];
        const plot::TYPE  type  = data.type;
        const int         size1 = data.width;
        const int         size2 = size1 / 2;
        int               x1    = 10000;
        int               y1    = 0;
        if (data.visible == false) {
            continue;
        }
        for (size_t i = 0; i < data.points.size(); i++) {
            const plot::Point& p = data.points[i];
            const int          x = X + (int) ((p.x - _x.min) * _x.pixel);
            const int          y = Y - (int) ((p.y - _y.min) * _y.pixel);
            if (type == plot::TYPE::LINE || type == plot::TYPE::LINE_DASH || type == plot::TYPE::LINE_DOT || type == plot::TYPE::LINE_WITH_SQUARE) {
                if (x1 == 10000) {
                    x1 = x;
                    y1 = y;
                }
                else {
                    fl_color((_selected_point == i && _selected_line == f) ? selection_color() : data.color);
                    _draw_lines_style(type, size1);
                    fl_line(x1, y1, x, y);
                    x1 = x;
                    y1 = y;
                }
                if (type == plot::TYPE::LINE_WITH_SQUARE) {
                    fl_color((_selected_point == i && _selected_line == f) ? selection_color() : data.color);
                    _draw_lines_style(type, size1);
                    fl_rectf(x - size1 - 3, y - size1 - 3, size1 * 2 + 6, size1 * 2 + 6);
                }
            }
            else if (type == plot::TYPE::VECTOR) {
                if (x1 == 10000) {
                    x1 = x;
                    y1 = y;
                }
                else {
                    fl_color(((_selected_point == i || _selected_point == i - 1) && _selected_line == f) ? selection_color() : data.color);
                    _draw_lines_style(type, size1);
                    fl_line(x, y, x1, y1);
                    x1 = 10000;
                }
            }
            else if (type == plot::TYPE::CIRCLE) {
                fl_color((_selected_point == i && _selected_line == f) ? selection_color() : data.color);
                _draw_lines_style(type, 1);
                fl_circle(x, y - 1, size2);
            }
            else if (type == plot::TYPE::FILLED_CIRCLE) {
                fl_color((_selected_point == i && _selected_line == f) ? selection_color() : data.color);
                _draw_lines_style(type, 1);
                fl_begin_polygon();
                fl_circle(x, y - 1, size2);
                fl_end_polygon();
            }
            else if (type == plot::TYPE::SQUARE) {
                fl_color((_selected_point == i && _selected_line == f) ? selection_color() : data.color);
                _draw_lines_style(type, 1);
                fl_rectf(x - size2, y - size2 - 1, size1, size1);
            }
        }
    }
    fl_line_style(0);
}
void flw::Plot::_draw_lines_style(plot::TYPE type, int size) {
    if (type == plot::TYPE::LINE) {
        fl_line_style(FL_SOLID, size);
    }
    else if (type == plot::TYPE::LINE_DASH) {
        fl_line_style(FL_DASH, size);
    }
    else if (type == plot::TYPE::LINE_DOT) {
        fl_line_style(FL_DOT, size);
    }
    else if (type == plot::TYPE::VECTOR) {
        fl_line_style(FL_SOLID, size);
    }
    else if (type == plot::TYPE::LINE_WITH_SQUARE) {
        fl_line_style(FL_SOLID, size);
    }
    else if (type == plot::TYPE::CIRCLE) {
        fl_line_style(FL_SOLID, size);
    }
    else if (type == plot::TYPE::FILLED_CIRCLE) {
        fl_line_style(FL_SOLID, size);
    }
    else if (type == plot::TYPE::SQUARE) {
        fl_line_style(FL_SOLID, size);
    }
    else {
        fl_line_style(FL_SOLID, 1);
    }
}
void flw::Plot::_draw_tooltip() {
    if (_tooltip == "") {
        return;
    }
    auto X = Fl::event_x();
    auto Y = Fl::event_y();
    if (X > _area.x2() - flw::PREF_FIXED_FONTSIZE * 19) {
        X -= flw::PREF_FIXED_FONTSIZE * 18;
    }
    else {
        X += flw::PREF_FIXED_FONTSIZE * 2;
    }
    if (Y > _area.y2() - flw::PREF_FIXED_FONTSIZE * 8) {
        Y -= flw::PREF_FIXED_FONTSIZE * 6;
    }
    else {
        Y += flw::PREF_FIXED_FONTSIZE * 2;
    }
    fl_color(FL_BACKGROUND2_COLOR);
    fl_line_style(FL_SOLID, 1);
    fl_rectf(X, Y, flw::PREF_FIXED_FONTSIZE * 16, flw::PREF_FIXED_FONTSIZE * 4);
    fl_color(FL_FOREGROUND_COLOR);
    fl_line_style(FL_SOLID, 1);
    fl_rect(X, Y, flw::PREF_FIXED_FONTSIZE * 16, flw::PREF_FIXED_FONTSIZE * 4);
    fl_line_style(FL_SOLID, 1);
    fl_line(Fl::event_x(), _area.y, Fl::event_x(), _area.y2() - 1);
    fl_line(_area.x, Fl::event_y(), _area.x2() - 1, Fl::event_y());
    fl_draw(_tooltip.c_str(), X + flw::PREF_FIXED_FONTSIZE, Y, flw::PREF_FIXED_FONTSIZE * 14, flw::PREF_FIXED_FONTSIZE * 4, FL_ALIGN_LEFT | FL_ALIGN_CENTER | FL_ALIGN_INSIDE);
    fl_line_style(0);
}
void flw::Plot::_draw_xlabels() {
    double       X    = _area.x;
    const int    Y    = _area.y2();
    const int    W    = x2();
    const double inc  = _x.pixel * _x.tick;
    double       val  = _x.min;
    int          last = -10'000;
    const int    tw   = _x.text / 2;
    while (X < _area.x2()) {
        if (_view.vertical == true && X > (_area.x + 4) && X < (_area.x2() - 4)) {
            fl_color(fl_color_average(FL_FOREGROUND_COLOR, FL_BACKGROUND2_COLOR, 0.2));
            fl_line(X, _area.y + 1, X, _area.y2() - 2);
        }
        if (X > (last + tw) && (X + tw) < W) {
            auto label = plot::format_double(val, _x.fr, '\'');
            if ((int) (X + 1) == (_area.x2() - 1)) {
                X += 1.0;
            }
            fl_color(FL_FOREGROUND_COLOR);
            fl_line(X, Y, X, Y + _ct * 2);
            if (_x.labels.size() == 0) {
                fl_draw(label.c_str(), X - tw, Y + _ct * 2, tw * 2, flw::PREF_FIXED_FONTSIZE, FL_ALIGN_CENTER | FL_ALIGN_INSIDE);
            }
            last = X + tw + _cw;
        }
        else {
            fl_color(FL_FOREGROUND_COLOR);
            fl_line(X, Y, X, Y + _ct);
        }
        X   += inc;
        val += _x.tick;
    }
}
void flw::Plot::_draw_xlabels2() {
    if (_x.labels.size() > 0) {
        auto index = (size_t) 0;
        auto pair  = plot::has_pairs(_lines[0].type);
        for (size_t i = 0; i < _lines[0].points.size(); i++) {
            const plot::Point& point = _lines[0].points[i];
            if (index >= _x.labels.size()) {
                break;
            }
            if ((i % 2 == 1 && pair == true) || pair == false) {
                const int X = _area.x + (int) ((point.x - _x.min) * _x.pixel);
                const int W = _x.labels[index].length() * _cw / 2;
                fl_draw(_x.labels[index].c_str(), X - W, _area.y2() + _ct * 2, W * 2, flw::PREF_FIXED_FONTSIZE, FL_ALIGN_CENTER | FL_ALIGN_INSIDE);
                index++;
            }
        }
    }
}
void flw::Plot::_draw_ylabels() {
    const int    X    = x();
    double       Y    = _area.y2() - 1.0;
    const int    W    = _area.x - X - flw::PREF_FIXED_FONTSIZE;
    const double inc  = _y.pixel * _y.tick;
    double       val  = _y.min;
    int          last = 10'000;
    while ((int) (Y + 0.5) >= _area.y) {
        if (_view.horizontal == true && Y > (_area.y + _ct) && Y < (_area.y2() - _ct)) {
            fl_color(fl_color_average(FL_FOREGROUND_COLOR, FL_BACKGROUND2_COLOR, 0.2));
            fl_line(_area.x + 1, Y, _area.x2() - 2, Y);
        }
        if (Y < last) {
            const auto label = plot::format_double(val, _y.fr, '\'');
            if (Y < (double) _area.y) {
                Y = _area.y;
            }
            fl_color(FL_FOREGROUND_COLOR);
            fl_line(_area.x, Y, _area.x - _ct * 2, Y);
            if (_y.labels.size() == 0) {
                fl_draw(label.c_str(), X, Y - _ct * 2, W, flw::PREF_FIXED_FONTSIZE, FL_ALIGN_RIGHT | FL_ALIGN_INSIDE);
            }
            last = Y - flw::PREF_FIXED_FONTSIZE;
        }
        else {
            fl_color(FL_FOREGROUND_COLOR);
            fl_line(_area.x, Y, _area.x - _ct, Y);
        }
        Y   -= inc;
        val += _y.tick;
    }
}
void flw::Plot::_draw_ylabels2() {
    if (_y.labels.size() > 0) {
        size_t     index = 0;
        const bool pair  = plot::has_pairs(_lines[0].type);
        for (size_t i = 0; i < _lines[0].points.size(); i++) {
            const plot::Point& p = _lines[0].points[i];
            if (index >= _y.labels.size()) {
                break;
            }
            if ((i % 2 == 1 && pair == true) || pair == false) {
                const int Y = _area.y2() - 1 - ((p.y - _y.min) * _y.pixel);
                fl_draw(_y.labels[index].c_str(), x() + _cw * 3, Y - _ct * 2, _y.text, flw::PREF_FIXED_FONTSIZE, FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
                index++;
            }
        }
    }
}
int flw::Plot::handle(int event) {
    if (event == FL_PUSH) {
        if (Fl::event_button1() != 0) {
            _create_tooltip(Fl::event_ctrl());
            if (_tooltip != "") {
                return 1;
            }
        }
        else if (Fl::event_button3() != 0) {
            flw::menu::set_item(_menu, plot::SHOW_LABELS, _view.labels);
            flw::menu::set_item(_menu, plot::SHOW_HLINES, _view.horizontal);
            flw::menu::set_item(_menu, plot::SHOW_VLINES, _view.vertical);
            _menu->popup();
            return 1;
        }
    }
    else if (event == FL_DRAG) {
        _create_tooltip(Fl::event_ctrl());
        if (_tooltip != "") {
            return 1;
        }
    }
    else if (event == FL_MOVE) {
        if (_tooltip != "") {
            _tooltip = "";
            redraw();
        }
    }
    else if (event == FL_KEYDOWN) {
        auto key = Fl::event_key();
        if (key >= '0' && key <= '9') {
            auto line = key - '0';
            if (line == 0) {
                line = 10;
            }
            line--;
            if ((size_t) line >= _size) {
                line = 0;
            }
            if (Fl::event_shift() != 0) {
                _lines[line].visible = !_lines[line].visible;
            }
            else {
                _selected_line = line;
            }
            redraw();
        }
    }
    return Fl_Group::handle(event);
}
void flw::Plot::resize(int X, int Y, int W, int H) {
    Fl_Widget::resize(X, Y, W, H);
    if (_calc == false && _w == W && _h == H) {
        return;
    }
    if (_size == 0) {
        return;
    }
    fl_font(flw::PREF_FIXED_FONT, flw::PREF_FIXED_FONTSIZE);
    _ct = flw::PREF_FIXED_FONTSIZE * 0.3;
    _cw = 0;
    _ch = 0;
    fl_measure("X", _cw, _ch, 0);
    if (_calc == true) {
        _calc_min_max();
        _x.calc(W - flw::PREF_FIXED_FONTSIZE * 6);
        _y.calc(H - flw::PREF_FIXED_FONTSIZE * 6);
        _x.measure_text(_cw);
        _y.measure_text(_cw);
    }
    _area.x = X + (flw::PREF_FIXED_FONTSIZE * 2) + _y.text + ((_y.label != "") ? flw::PREF_FIXED_FONTSIZE : 0);
    _area.y = Y + flw::PREF_FIXED_FONTSIZE;
    _area.w = W - (_area.x - X) - flw::PREF_FIXED_FONTSIZE * 2;
    _area.h = H - (flw::PREF_FIXED_FONTSIZE * 3) - ((_x.label != "") ? flw::PREF_FIXED_FONTSIZE : 0);
    _x.calc(_area.w - 1);
    _y.calc(_area.h - 1);
    _calc = false;
    _w    = W;
    _h    = H;
}
void flw::Plot::update_pref() {
    _menu->textfont(flw::PREF_FONT);
    _menu->textsize(flw::PREF_FONTSIZE);
}
namespace flw {
    RecentMenu::RecentMenu(Fl_Menu_* menu, Fl_Callback* file_callback, void* userdata, std::string base_label, std::string clear_label) {
        _menu     = menu;
        _callback = file_callback;
        _base     = base_label;
        _clear    = clear_label;
        _user     = userdata;
        _max      = 10;
        _menu->add((_base + _clear).c_str(), 0, RecentMenu::CallbackClear, this, FL_MENU_DIVIDER);
    }
    void RecentMenu::_add(std::string file, bool append) {
        if (file == "") {
            _files.clear();
        }
        else if (append == true) {
            _add_string(_files, _max, file);
        }
        else {
            _insert_string(_files, _max, file);
        }
        auto index = _menu->find_index(_base.c_str());
        if (index >= 0) {
            _menu->clear_submenu(index);
            _menu->add((_base + _clear).c_str(), 0, RecentMenu::CallbackClear, this, FL_MENU_DIVIDER);
        }
        for (const auto& file : _files) {
            _menu->add((_base + "/" + flw::util::fix_menu_string(file)).c_str(), 0, _callback, _user);
        }
    }
    size_t RecentMenu::_add_string(StringVector& in, size_t max_size, std::string string) {
        for (auto it = in.begin(); it != in.end(); ++it) {
            if (*it == string) {
                in.erase(it);
                break;
            }
        }
        in.push_back(string);
        while (in.size() > max_size) in.erase(in.begin());
        return in.size();
    }
    void RecentMenu::CallbackClear(Fl_Widget*, void* o) {
        auto self = (RecentMenu*) o;
        self->_add("", false);
    }
    size_t RecentMenu::_insert_string(StringVector& in, size_t max_size, std::string string) {
        for (auto it = in.begin(); it != in.end(); ++it) {
            if (*it == string) {
                in.erase(it);
                break;
            }
        }
        in.insert(in.begin(), string);
        while (in.size() > max_size) in.pop_back();
        return (int) in.size();
    }
    void RecentMenu::load_pref(Fl_Preferences& pref, std::string base_name) {
        auto index = 1;
        char buffer[1000];
        while (index <= 100) {
            auto key = flw::util::format("%s%d", base_name.c_str(), index++);
            pref.get(key.c_str(), buffer, "", 1000);
            if (*buffer == 0) {
                break;
            }
            _add(buffer, true);
        }
    }
    void RecentMenu::save_pref(Fl_Preferences& pref, std::string base_name) {
        auto index = 1;
        for (auto& s : _files) {
            pref.set(flw::util::format("%s%d", base_name.c_str(), index++).c_str(), s.c_str());
        }
        pref.set(flw::util::format("%s%d", base_name.c_str(), index++).c_str(), "");
    }
}
namespace flw {
    static const std::string _SCROLLBROWSER_MENU_ALL  = "Copy All Lines";
    static const std::string _SCROLLBROWSER_MENU_LINE = "Copy Current Line";
    static const std::string _SCROLLBROWSER_TOOLTIP   = "Right click to show the menu";
}
flw::ScrollBrowser::ScrollBrowser(int scroll, int X, int Y, int W, int H, const char* l) : Fl_Hold_Browser(X, Y, W, H, l) {
    end();
    _menu      = new Fl_Menu_Button(0, 0, 0, 0);
    _scroll    = (scroll > 0) ? scroll : 9;
    _flag_move = true;
    _flag_menu = true;
    ((Fl_Group*) this)->add(_menu);
    _menu->add(_SCROLLBROWSER_MENU_LINE.c_str(), 0, ScrollBrowser::Callback, this);
    _menu->add(_SCROLLBROWSER_MENU_ALL.c_str(), 0, ScrollBrowser::Callback, this);
    _menu->type(Fl_Menu_Button::POPUP3);
    tooltip(_SCROLLBROWSER_TOOLTIP.c_str());
    update_pref();
}
void flw::ScrollBrowser::Callback(Fl_Widget*, void* o) {
    auto self  = (ScrollBrowser*) o;
    auto menu  = self->_menu->text();
    auto label = std::string((menu != nullptr) ? menu : "");
    auto clip  = std::string();
    clip.reserve(self->size() * 40 + 100);
    if (label == _SCROLLBROWSER_MENU_LINE) {
        if (self->value() != 0) {
            clip = util::remove_browser_format(self->text(self->value()));
        }
    }
    else if (label == _SCROLLBROWSER_MENU_ALL) {
        for (auto f = 1; f <= self->size(); f++) {
            auto s = util::remove_browser_format(self->text(f));
            clip += s;
            clip += "\n";
        }
    }
    if (clip != "") {
        Fl::copy(clip.c_str(), clip.length(), 2);
    }
}
int flw::ScrollBrowser::handle(int event) {
    if (event == FL_MOUSEWHEEL) {
        if (Fl::event_dy() > 0) {
            topline(topline() + _scroll);
        }
        else if (Fl::event_dy() < 0) {
            topline(topline() - _scroll);
        }
        return 1;
    }
    else if (event == FL_KEYBOARD) {
        if (_flag_move == true) {
            auto key = Fl::event_key();
            if (key == FL_Page_Up) {
                auto line = value();
                if (line == 1) {
                }
                else if (line - _scroll < 1) {
                    value(1);
                    do_callback();
                }
                else {
                    value(line - _scroll);
                    topline(line - _scroll);
                    do_callback();
                }
                return 1;
            }
            else if (key == FL_Page_Down) {
                auto line = value();
                if (line == size()) {
                }
                else if (line + _scroll > size()) {
                    value(size());
                    do_callback();
                }
                else {
                    value(line + _scroll);
                    bottomline(line + _scroll);
                    do_callback();
                }
                return 1;
            }
        }
    }
    else if (event == FL_PUSH) {
        if (_flag_menu == true && Fl::event_button() == FL_RIGHT_MOUSE) {
            _menu->popup();
            return 1;
        }
    }
    return Fl_Hold_Browser::handle(event);
}
void flw::ScrollBrowser::update_pref(Fl_Font text_font, Fl_Fontsize text_size) {
    labelfont(flw::PREF_FONT);
    labelsize(flw::PREF_FONTSIZE);
    textfont(text_font);
    textsize(text_size);
}
#include <FL/fl_draw.H>
flw::SplitGroup::SplitGroup(int X, int Y, int W, int H, const char* l) : Fl_Group(X, Y, W, H, l) {
    end();
    clip_children(1);
    clear();
}
void flw::SplitGroup::add(Fl_Widget* widget, SplitGroup::CHILD child) {
    auto num = child == SplitGroup::CHILD::FIRST ? 0 : 1;
    if (_widgets[num]) {
        remove(_widgets[num]);
        delete _widgets[num];
    }
    _widgets[num] = widget;
    if (widget) {
        Fl_Group::add(widget);
    }
}
void flw::SplitGroup::clear() {
    Fl_Group::clear();
    _drag       = false;
    _min        = 50;
    _split_pos  = -1;
    _direction  = DIRECTION::VERTICAL;
    _widgets[0] = nullptr;
    _widgets[1] = nullptr;
}
void flw::SplitGroup::direction(SplitGroup::DIRECTION direction) {
    _direction = direction;
    _split_pos = -1;
    Fl::redraw();
}
int flw::SplitGroup::handle(int event) {
    if (event == FL_DRAG) {
        if (_drag == true) {
            auto pos = 0;
            if (_direction == SplitGroup::DIRECTION::VERTICAL) {
                pos = Fl::event_x() - x();
            }
            else {
                pos = Fl::event_y() - y();
            }
            if (pos != _split_pos) {
                _split_pos = pos;
                resize();
            }
            return 1;
        }
    }
    else if (event == FL_LEAVE) {
        if (_direction == SplitGroup::DIRECTION::VERTICAL) {
            if (Fl::event_y() < y() || Fl::event_y() > y() + h() || Fl::event_x() < x() || Fl::event_x() > x() + w()) {
                _drag = false;
                fl_cursor(FL_CURSOR_DEFAULT);
            }
        }
        else {
            if (Fl::event_y() < y() || Fl::event_y() > y() + h() || Fl::event_x() < x() || Fl::event_x() > x() + w()) {
                _drag = false;
                fl_cursor(FL_CURSOR_DEFAULT);
            }
        }
    }
    else if (event == FL_MOVE) {
        if (_widgets[0] != nullptr && _widgets[1] != nullptr && _widgets[0]->visible() != 0 && _widgets[1]->visible() != 0) {
            if (_direction == SplitGroup::DIRECTION::VERTICAL) {
                auto mx  = Fl::event_x();
                auto pos = x() + _split_pos;
                if (mx > (pos - 3) && mx <= (pos + 3)) {
                    if (_drag == false) {
                        _drag = true;
                        fl_cursor(FL_CURSOR_WE);
                    }
                    return 1;
                }
            }
            else {
                auto my  = Fl::event_y();
                auto pos = y() + _split_pos;
                if (my > (pos - 3) && my <= (pos + 3)) {
                    if (_drag == false) {
                        _drag = true;
                        fl_cursor(FL_CURSOR_NS);
                    }
                    return 1;
                }
            }
        }
        if (_drag == true) {
            _drag = false;
            fl_cursor(FL_CURSOR_DEFAULT);
        }
    }
    else if (event == FL_PUSH) {
        if (_drag == true) {
            return 1;
        }
    }
    return Fl_Group::handle(event);
}
void flw::SplitGroup::resize(int X, int Y, int W, int H) {
    Fl_Widget::resize(X, Y, W, H);
    if (W == 0 || H == 0) {
        return;
    }
    auto currx = X;
    auto curry = Y;
    auto currh = H;
    auto currw = W;
    if (_direction == SplitGroup::DIRECTION::VERTICAL) {
        if (_widgets[0] != nullptr && _widgets[1] != nullptr && _widgets[0]->visible() != 0 && _widgets[1]->visible() != 0) {
            if (_split_pos == -1) {
                _split_pos = W / 2;
            }
            else if (_split_pos >= W - _min) {
                _split_pos = W - _min;
            }
            else if (_split_pos <= _min) {
                _split_pos = _min;
            }
            auto max = (X + W) - 4;
            auto pos = _split_pos + X;
            if (pos < X) {
                pos = X;
            }
            else if (pos > max) {
                pos = max;
            }
            auto w1 = pos - (X + 2);
            auto w2 = (X + W) - (pos + 2);
            _widgets[0]->resize(currx, curry, w1, currh);
            _widgets[1]->resize(currx + w1 + 4, curry, w2, currh);
        }
        else if (_widgets[0] && _widgets[0]->visible()) {
            _widgets[0]->resize(currx, curry, currw, currh);
        }
        else if (_widgets[1] && _widgets[1]->visible()) {
            _widgets[1]->resize(currx, curry, currw, currh);
        }
    }
    else if (_widgets[0] != nullptr && _widgets[1] != nullptr && _widgets[0]->visible() != 0 && _widgets[1]->visible() != 0) {
        if (_split_pos == -1) {
            _split_pos = H / 2;
        }
        else if (_split_pos >= H - _min) {
            _split_pos = H - _min;
        }
        else if (_split_pos <= _min) {
            _split_pos = _min;
        }
        auto max = (Y + H) - 4;
        auto pos = _split_pos + Y;
        if (pos < Y) {
            pos = Y;
        }
        else if (pos > max) {
            pos = max;
        }
        auto h1 = pos - (Y + 2);
        auto h2 = (Y + H) - (pos + 2);
        _widgets[0]->resize(currx, curry, currw, h1);
        _widgets[1]->resize(currx, curry + h1 + 4, currw, h2);
    }
    else if (_widgets[0] != nullptr && _widgets[0]->visible() != 0) {
        _widgets[0]->resize(currx, curry, currw, currh);
    }
    else if (_widgets[1] != nullptr && _widgets[1]->visible() != 0) {
        _widgets[1]->resize(currx, curry, currw, currh);
    }
}
void flw::SplitGroup::toggle(SplitGroup::CHILD child, SplitGroup::DIRECTION direction, int second_size) {
    if (_widgets[0] == nullptr || _widgets[1] == nullptr) {
        return;
    }
    auto num = (child == SplitGroup::CHILD::FIRST) ? 0 : 1;
    if (_widgets[num]->visible() && _direction == direction) {
        _widgets[num]->hide();
    }
    else {
        _widgets[num]->show();
        if (_direction != direction || split_pos() == -1) {
            _direction = direction;
            if (second_size == -1) {
                split_pos(-1);
            }
            else if (_direction == SplitGroup::DIRECTION::VERTICAL) {
                split_pos(w() - second_size);
            }
            else {
                split_pos(h() - second_size);
            }
        }
    }
}
#include <FL/Fl.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Int_Input.H>
#include <FL/fl_ask.H>
#include <FL/fl_draw.H>
namespace flw {
    class _TableDisplay_Scrollbar : public Fl_Scrollbar {
    public:
        _TableDisplay_Scrollbar(int X, int Y, int W, int H) : Fl_Scrollbar(X, Y, W, H) {
        }
        int handle(int event) override {
            if (event == FL_FOCUS ||
                event == FL_KEYUP ||
                event == FL_KEYDOWN ||
                event == FL_SHORTCUT) {
                return 0;
            }
            return Fl_Scrollbar::handle(event);
        }
    };
    class _TableDisplayCellDialog : public Fl_Double_Window {
        Fl_Int_Input*                   _row;
        Fl_Int_Input*                   _col;
        bool                            _ret;
    public:
        _TableDisplayCellDialog(int row, int col) : Fl_Double_Window(0, 0, 0, 0, "Goto Cell") {
            end();
            _row = new Fl_Int_Input(0, 0, 0, 0, "Row:");
            _col = new Fl_Int_Input(0, 0, 0, 0, "Column:");
            _ret = false;
            add(_row);
            add(_col);
            _row->align(FL_ALIGN_LEFT | FL_ALIGN_TOP);
            _row->callback(_TableDisplayCellDialog::Callback, this);
            _row->labelsize(flw::PREF_FONTSIZE);
            _row->textfont(flw::PREF_FIXED_FONT);
            _row->textsize(flw::PREF_FONTSIZE);
            _row->when(FL_WHEN_ENTER_KEY_ALWAYS);
            _col->align(FL_ALIGN_LEFT | FL_ALIGN_TOP);
            _col->callback(_TableDisplayCellDialog::Callback, this);
            _col->labelsize(flw::PREF_FONTSIZE);
            _col->textfont(flw::PREF_FIXED_FONT);
            _col->textsize(flw::PREF_FONTSIZE);
            _col->when(FL_WHEN_ENTER_KEY_ALWAYS);
            char b[100];
            sprintf(b, "%d", row > 0 ? row : 1);
            _row->value(b);
            sprintf(b, "%d", col > 0 ? col : 1);
            _col->value(b);
            callback(_TableDisplayCellDialog::Callback, this);
            set_modal();
            resizable(this);
            resize(0, 0, flw::PREF_FONTSIZE * 20, flw::PREF_FONTSIZE * 7);
        }
        static void Callback(Fl_Widget* w, void* o) {
            auto dlg = (_TableDisplayCellDialog*) o;
            if (w == dlg) {
                dlg->hide();
            }
            else if (w == dlg->_row) {
                dlg->_ret = true;
                dlg->hide();
            }
            else if (w == dlg->_col) {
                dlg->_ret = true;
                dlg->hide();
            }
        }
        int col() const {
            return (*_col->value() >= '0' && *_col->value() <= '9') ? atoi(_col->value()) : 0;
        }
        void resize(int X, int Y, int W, int H) {
            Fl_Double_Window::resize(X, Y, W, H);
            _row->resize(4, flw::PREF_FONTSIZE + 4, W - 8, flw::PREF_FONTSIZE * 2);
            _col->resize(4, flw::PREF_FONTSIZE * 4 + 8, W - 8, flw::PREF_FONTSIZE * 2);
        }
        int row() const {
            return (*_row->value() >= '0' && *_row->value() <= '9') ? atoi(_row->value()) : 0;
        }
        bool run(Fl_Window* parent) {
            flw::util::center_window(this, parent);
            show();
            while (visible() != 0) {
                Fl::wait();
                Fl::flush();
            }
            return _ret;
        }
    };
    class _TableDisplayFindDialog : public Fl_Double_Window {
        Fl_Input*                   _text;
        Fl_Button*                  _next;
        Fl_Button*                  _prev;
        Fl_Button*                  _close;
        TableDisplay*               _table;
        bool                        _repeat;
    public:
        _TableDisplayFindDialog(TableDisplay* table) : Fl_Double_Window(0, 0, 0, 0, "Goto Cell") {
            end();
            _close  = new Fl_Button(0, 0, 0, 0, "&Close");
            _next   = new Fl_Button(0, 0, 0, 0, "&Find next");
            _prev   = new Fl_Button(0, 0, 0, 0, "Find &previous");
            _text   = new Fl_Input(0, 0, 0, 0, "Search:");
            _table  = table;
            _repeat = true;
            add(_text);
            add(_prev);
            add(_next);
            add(_close);
            _close->callback(_TableDisplayFindDialog::Callback, this);
            _close->labelsize(flw::PREF_FONTSIZE);
            _next->callback(_TableDisplayFindDialog::Callback, this);
            _next->labelsize(flw::PREF_FONTSIZE);
            _prev->callback(_TableDisplayFindDialog::Callback, this);
            _prev->labelsize(flw::PREF_FONTSIZE);
            _text->align(FL_ALIGN_LEFT);
            _text->callback(_TableDisplayFindDialog::Callback, this);
            _text->labelsize(flw::PREF_FONTSIZE);
            _text->textfont(flw::PREF_FIXED_FONT);
            _text->textsize(flw::PREF_FONTSIZE);
            _text->value(_table->_find.c_str());
            _text->when(FL_WHEN_ENTER_KEY_ALWAYS);
            callback(_TableDisplayFindDialog::Callback, this);
            set_modal();
            resizable(this);
            resize(0, 0, flw::PREF_FONTSIZE * 35, flw::PREF_FONTSIZE * 7);
        }
        static void Callback(Fl_Widget* w, void* o) {
            auto dlg = (_TableDisplayFindDialog*) o;
            if (w == dlg) {
                dlg->hide();
            }
            else if (w == dlg->_close) {
                dlg->_table->_find = dlg->_text->value();
                dlg->hide();
            }
            else if (w == dlg->_next) {
                dlg->find(true);
            }
            else if (w == dlg->_prev) {
                dlg->find(false);
            }
            else if (w == dlg->_text) {
                dlg->find(dlg->_repeat);
            }
        }
        void find(bool next) {
            auto find = _text->value();
            _repeat = next;
            if (*find == 0) {
                fl_beep(FL_BEEP_ERROR);
                return;
            }
            auto row = _table->row();
            auto col = _table->column();
            if (next == true) {
                if (row < 1 || col < 1) {
                    row = 1;
                    col = 1;
                }
                else {
                    col++;
                }
            }
            else {
                if (row < 1 || col < 1) {
                    row = _table->rows();
                    col = _table->columns();
                }
                else {
                    col--;
                }
            }
        AGAIN:
            if (next == true) {
                for (int r = row; r <= _table->rows(); r++) {
                    for (int c = col; c <= _table->columns(); c++) {
                        auto v = _table->cell_value(r, c);
                        if (v != nullptr && strstr(v, find) != nullptr) {
                            _table->active_cell(r, c, true);
                            _table->_find = find;
                            return;
                        }
                    }
                    col = 1;
                }
                if (fl_choice("Unable to find <%s>!\nWould you like to try again from the beginning?", nullptr, "Yes", "No", find) == 1) {
                    col = row = 1;
                    goto AGAIN;
                }
            }
            else {
                for (int r = row; r >= 1; r--) {
                    for (int c = col; c >= 1; c--) {
                        auto v = _table->cell_value(r, c);
                        if (v != nullptr && strstr(v, find) != nullptr) {
                            _table->active_cell(r, c, true);
                            _table->_find = find;
                            return;
                        }
                    }
                    col = _table->columns();
                }
                if (fl_choice("Unable to find <%s>!\nWould you like to try again from the end?", nullptr, "Yes", "No", find) == 1) {
                    row = _table->rows();
                    col = _table->columns();
                    goto AGAIN;
                }
            }
        }
        void resize(int X, int Y, int W, int H) {
            Fl_Double_Window::resize(X, Y, W, H);
            _text->resize  (flw::PREF_FONTSIZE * 5,             flw::PREF_FONTSIZE,                 W - flw::PREF_FONTSIZE * 5 - 4, flw::PREF_FONTSIZE * 2);
            _prev->resize  (W - flw::PREF_FONTSIZE * 30 - 12,   H - flw::PREF_FONTSIZE * 2 - 4,     flw::PREF_FONTSIZE * 10,        flw::PREF_FONTSIZE * 2);
            _next->resize  (W - flw::PREF_FONTSIZE * 20 - 8,    H - flw::PREF_FONTSIZE * 2 - 4,     flw::PREF_FONTSIZE * 10,        flw::PREF_FONTSIZE * 2);
            _close->resize (W - flw::PREF_FONTSIZE * 10 - 4,    H - flw::PREF_FONTSIZE * 2 - 4,     flw::PREF_FONTSIZE * 10,        flw::PREF_FONTSIZE * 2);
        }
        void run(Fl_Window* parent) {
            flw::util::center_window(this, parent);
            show();
            while (visible() != 0) {
                Fl::wait();
                Fl::flush();
            }
        }
    };
}
flw::TableDisplay::TableDisplay(int X, int Y, int W, int H, const char* l) : Fl_Group(X, Y, W, H, l) {
    end();
    _hor  = new _TableDisplay_Scrollbar(0, 0, 0, 0);
    _ver  = new _TableDisplay_Scrollbar(0, 0, 0, 0);
    _edit = nullptr;
    add(_ver);
    add(_hor);
    _hor->callback(&_CallbackVer, this);
    _hor->linesize(10);
    _hor->type(FL_HORIZONTAL);
    _ver->callback(&_CallbackHor, this);
    _ver->linesize(10);
    box(FL_BORDER_BOX);
    color(FL_BACKGROUND_COLOR);
    selection_color(FL_SELECTION_COLOR);
    clip_children(1);
    util::labelfont(this);
    TableDisplay::clear();
}
void flw::TableDisplay::active_cell(int row, int col, bool show) {
    auto send = false;
    if (_rows == 0 || row < 1 || row > _rows || _cols == 0 || col < 1 || col > _cols) {
        row = -1;
        col = -1;
    }
    if (_curr_row != row || _curr_col != col) {
        send = true;
    }
    _curr_row = row;
    _curr_col = col;
    if (send) {
        _set_event(_curr_row, _curr_col, flw::TableDisplay::EVENT::CURSOR);
        do_callback();
    }
    redraw();
    if (show &&
        _curr_row > 0 &&
        _curr_col > 0) {
        show_cell(_curr_row, _curr_col);
    }
}
void flw::TableDisplay::_CallbackVer(Fl_Widget*, void* o) {
    auto table = (flw::TableDisplay*) o;
    table->_start_col = table->_hor->value();
    table->redraw();
}
 void flw::TableDisplay::_CallbackHor(Fl_Widget*, void* o) {
    auto table = (TableDisplay*) o;
    table->_start_row = table->_ver->value();
    table->redraw();
}
int flw::TableDisplay::_cell_height(int Y) {
    auto y2 = y() + h() - _hor->h();
    if (Y + _height >= y2) {
        return y2 - Y;
    }
    else {
        return _height;
    }
}
int flw::TableDisplay::_cell_width(int col, int X) {
    auto cw  = cell_width(col);
    auto min = _height;
    if (cw < min) {
        cw = min;
    }
    else if (cw > 3840) {
        cw = 3840;
    }
    if (X >= 0 && _expand && col == _cols) {
        return x() + w() - _ver->w() - X;
    }
    else {
        auto x2 = x() + w() - _ver->w();
        if (X + cw >= x2) {
            return x2 - X;
        }
    }
    return cw;
}
void flw::TableDisplay::clear() {
    _cols            = 0;
    _curr_col        = -1;
    _curr_row        = -1;
    _current_cell[0] = 0;
    _current_cell[1] = 0;
    _current_cell[2] = 0;
    _current_cell[3] = 0;
    _disable_hor     = false;
    _disable_ver     = false;
    _drag            = false;
    _edit            = nullptr;
    _event           = flw::TableDisplay::EVENT::UNDEFINED;
    _event_col       = -1;
    _event_row       = -1;
    _expand          = false;
    _height          = flw::PREF_FONTSIZE * 2;
    _resize          = false;
    _resize_col      = -1;
    _rows            = 0;
    _select          = flw::TableDisplay::SELECT::NO;
    _show_col_header = false;
    _show_hor_lines  = false;
    _show_row_header = false;
    _show_ver_lines  = false;
    _start_col       = 1;
    _start_row       = 1;
    _find            = "";
    redraw();
}
void flw::TableDisplay::draw() {
    if (_edit) {
        _edit->resize(_current_cell[0], _current_cell[1], _current_cell[2] + 1, _current_cell[3] + 1);
    }
    _update_scrollbars();
    Fl_Group::draw();
    if (_ver->visible() != 0 && _hor->visible() != 0) {
        fl_push_clip(x() + 1, y() + 1, w() - 2 - _ver->w(), h() - 2 - _hor->h());
    }
    else if (_ver->visible() != 0) {
        fl_push_clip(x() + 1, y() + 1, w() - 2 - _ver->w(), h() - 2);
    }
    else if (_hor->visible() != 0) {
        fl_push_clip(x() + 1, y() + 1, w() - 2, h() - 2 - _hor->h());
    }
    else {
        fl_push_clip(x() + 1, y() + 1, w() - 2, h() - 2);
    }
    auto w1 = w() - _ver->w();
    auto y1 = y();
    auto y2 = y() + h() - _hor->h();
    for (auto r = (_show_col_header ? 0 : _start_row); r <= _rows; r++) {
        auto ch = _cell_height(y1);
        _draw_row(r, w1, y1, ch);
        y1 += _height;
        if (y1 >= y2) {
            break;
        }
        else if (r == 0) {
            r = _start_row - 1;
        }
    }
    if (_edit) {
        fl_color(fl_lighter(FL_FOREGROUND_COLOR));
        fl_rect(_current_cell[0], _current_cell[1], _current_cell[2] + 1, _current_cell[3] + 1);
    }
    fl_pop_clip();
}
void flw::TableDisplay::_draw_cell(int row, int col, int X, int Y, int W, int H, bool ver, bool hor, bool current) {
    fl_push_clip(X, Y, W + 1, H);
    auto align    = cell_align(row, col);
    auto textfont = cell_textfont(row, col);
    auto textsize = cell_textsize(row, col);
    auto val      = cell_value(row, col);
    auto space    = (align & FL_ALIGN_RIGHT) ? 6 : 4;
    if (row > 0 && col > 0) {
        auto color     = cell_color(row, col);
        auto textcolor = cell_textcolor(row, col);
        if (current) {
            color = selection_color();
        }
        fl_color(color);
        fl_rectf(X + 1, Y, W + 1, H);
        fl_font(textfont, textsize);
        fl_color(textcolor);
        _draw_text(val, X + space, Y + 2, W - space * 2, H - 4, align);
        fl_color(FL_DARK3);
        if (ver) {
            fl_line(X, Y, X, Y + H);
            fl_line(X + W, Y, X + W, Y + H);
        }
        if (hor) {
            fl_line(X, Y, X + W, Y);
            fl_line(X, Y + H - (row == _rows ? 1 : 0), X + W, Y + H - (row == _rows ? 1 : 0));
        }
    }
    else {
        fl_draw_box(FL_THIN_UP_BOX, X, Y, W + 1, H + (row == _rows ? 0 : 1), FL_BACKGROUND_COLOR);
        fl_font(textfont, textsize);
        fl_color(FL_FOREGROUND_COLOR);
        _draw_text(val, X + space, Y + 2, W - space * 2, H - 4, align);
    }
    fl_pop_clip();
}
void flw::TableDisplay::_draw_row(int row, int W, int Y, int H) {
    auto x1 = x();
    auto x2 = x() + W;
    for (auto c = (_show_row_header ? 0 : _start_col); c <= _cols; c++) {
        auto cw = _cell_width(c, x1);
        if (row > 0 && c > 0 && row == _curr_row && c == _curr_col) {
            _current_cell[0] = x1;
            _current_cell[1] = Y;
            _current_cell[2] = cw;
            _current_cell[3] = H;
            if (_edit == nullptr) {
                _draw_cell(row, c, x1, Y, cw, H, _show_ver_lines, _show_hor_lines, _select != flw::TableDisplay::SELECT::NO);
            }
        }
        else if (row > 0 && row == _curr_row && _select == flw::TableDisplay::SELECT::ROW) {
            _draw_cell(row, c, x1, Y, cw, H, _show_ver_lines, _show_hor_lines, true);
        }
        else {
            _draw_cell(row, c, x1, Y, cw, H, _show_ver_lines, _show_hor_lines, false);
        }
        x1 += cw;
        if (x1 >= x2) {
            break;
        }
        else if (c == 0) {
            c = _start_col - 1;
        }
    }
}
void flw::TableDisplay::_draw_text(const char* string, int X, int Y, int W, int H, Fl_Align align) {
    if (align == FL_ALIGN_CENTER || align == FL_ALIGN_RIGHT) {
        if (fl_width(string) > W) {
            align = FL_ALIGN_LEFT;
        }
    }
    fl_draw(string, X, Y, W, H, align);
}
int flw::TableDisplay::_ev_keyboard_down() {
    auto key   = Fl::event_key();
    auto cmd   = Fl::event_command() != 0;
    auto shift = Fl::event_shift() != 0;
    if (cmd == true && key == FL_Up) {
        _move_cursor(_TABLEDISPLAY_MOVE::SCROLL_UP);
        return 1;
    }
    else if (key == FL_Up) {
        _move_cursor(_TABLEDISPLAY_MOVE::UP);
        return 1;
    }
    else if (cmd == true && key == FL_Down) {
        _move_cursor(_TABLEDISPLAY_MOVE::SCROLL_DOWN);
        return 1;
    }
    else if (key == FL_Down) {
        _move_cursor(_TABLEDISPLAY_MOVE::DOWN);
        return 1;
    }
    else if (cmd == true && key == FL_Left) {
        _move_cursor(_TABLEDISPLAY_MOVE::SCROLL_LEFT);
        return 1;
    }
    else if (key == FL_Left || (key == FL_Tab && shift == true)) {
        _move_cursor(_TABLEDISPLAY_MOVE::LEFT);
        return 1;
    }
    else if (cmd == true && key == FL_Right) {
        _move_cursor(_TABLEDISPLAY_MOVE::SCROLL_RIGHT);
        return 1;
    }
    else if (key == FL_Right || key == FL_Tab) {
        _move_cursor(_TABLEDISPLAY_MOVE::RIGHT);
        return 1;
    }
    else if (cmd == true && key == FL_Page_Up) {
        _move_cursor(_TABLEDISPLAY_MOVE::FIRST_ROW);
        return 1;
    }
    else if (key == FL_Page_Up) {
        _move_cursor(_TABLEDISPLAY_MOVE::PAGE_UP);
        return 1;
    }
    else if (cmd == true && key == FL_Page_Down) {
        _move_cursor(_TABLEDISPLAY_MOVE::LAST_ROW);
        return 1;
    }
    else if (key == FL_Page_Down) {
        _move_cursor(_TABLEDISPLAY_MOVE::PAGE_DOWN);
        return 1;
    }
    else if (key == FL_Home) {
        _move_cursor(_TABLEDISPLAY_MOVE::FIRST_COL);
        return 1;
    }
    else if (key == FL_End) {
        _move_cursor(_TABLEDISPLAY_MOVE::LAST_COL);
        return 1;
    }
    else if (cmd == true && key == 'c') {
        auto val = cell_value(_curr_row, _curr_col);
        Fl::copy(val, strlen(val), 1);
        return 1;
    }
    else if (cmd == true && key == 'g') {
        auto dlg = _TableDisplayCellDialog(_curr_row, _curr_col);
        if (dlg.run(window()) == true) {
            active_cell(dlg.row(), dlg.col(), true);
        }
        return 1;
    }
    else if (cmd == true && key == 'f') {
        auto dlg = _TableDisplayFindDialog(this);
        dlg.run(window());
        return 1;
    }
    return 0;
}
int flw::TableDisplay::_ev_mouse_click () {
    if (Fl::event_button1() && _drag == true) {
        return 1;
    }
    auto row         = 0;
    auto col         = 0;
    auto current_row = _curr_row;
    auto current_col = _curr_col;
    _get_cell_below_mouse(row, col);
    if (_edit == nullptr) {
        Fl::focus(this);
    }
    if (row == 0 && col >= 1) {
        _set_event(row, col, (Fl::event_ctrl() != 0) ? flw::TableDisplay::EVENT::COLUMN_CTRL : flw::TableDisplay::EVENT::COLUMN);
        do_callback();
    }
    else if (col == 0 && row >= 1) {
        _set_event(row, col, (Fl::event_ctrl() != 0) ? flw::TableDisplay::EVENT::ROW_CTRL : flw::TableDisplay::EVENT::ROW);
        do_callback();
    }
    else if (row == -1 || col == -1) {
        if (row == -1 && _hor->visible() != 0 && Fl::event_y() >= _hor->y()) {
            ;
        }
        else if (col == -1 && _ver->visible() != 0 && Fl::event_x() >= _ver->x()) {
            ;
        }
        else {
            active_cell(-1, -1);
            return 0;
        }
    }
    else if (row >= 1 && col >= 1 && (row != current_row || col != current_col) && _select != flw::TableDisplay::SELECT::NO) {
        active_cell(row, col);
    }
    return 2;
}
int flw::TableDisplay::_ev_mouse_drag() {
    if (_drag == false) {
        return 2;
    }
    auto xpos  = Fl::event_x();
    auto currx = x();
    if (_show_row_header && _resize_col == 0) {
        if ((xpos - currx) >= 10) {
            cell_width(_resize_col, xpos - currx);
            redraw();
        }
    }
    else {
        if (_show_row_header) {
            currx += _cell_width(0);
        }
        for (auto c = _start_col; c < _resize_col; c++) {
            currx += _cell_width(c);
        }
        if ((xpos - currx) >= 10) {
            cell_width(_resize_col, xpos - currx);
            redraw();
        }
    }
    return 2;
}
int flw::TableDisplay::_ev_mouse_move() {
    auto mx = Fl::event_x();
    auto x1 = x() + (_show_row_header ? _cell_width(0) : 0);
    auto x2 = x() + w() - _ver->w();
    if (_resize) {
        if (_show_row_header && mx > (x1 - 3) && mx < (x1 + 4)) {
            _resize_col = 0;
            _drag       = true;
            fl_cursor(FL_CURSOR_WE);
            return 1;
        }
        else {
            for (auto c = _start_col; c <= _cols; c++) {
                x1 += _cell_width(c, x1);
                if (x1 >= x2) {
                    break;
                }
                else if (mx > (x1 - 3) && mx < (x1 + 4)) {
                    _resize_col = c;
                    _drag       = true;
                    fl_cursor(FL_CURSOR_WE);
                    return 1;
                }
            }
        }
    }
    if (_drag == true) {
        _drag = false;
        fl_cursor(FL_CURSOR_DEFAULT);
    }
    _resize_col = -1;
    return 2;
}
void flw::TableDisplay::_get_cell_below_mouse(int& row, int& col) {
    row = -1;
    col = -1;
    auto my = Fl::event_y();
    auto mx = Fl::event_x();
    if (!((_ver->visible() != 0 && mx >= _ver->x()) || (_hor->visible() != 0 && my >= _hor->y()))) {
        if (_show_col_header && (my - y()) < _height) {
            row = 0;
        }
        else {
            auto y1 = y() + (_show_col_header ? _height : 0);
            auto y2 = y() + h();
            for (auto r = _start_row; r <= _rows; r++) {
                if (y1 > y2) {
                    break;
                }
                else if (my > y1 && my < y1 + _height) {
                    row = r;
                    break;
                }
                y1 += _height;
            }
        }
        if (_show_row_header && (mx - x()) < _cell_width(0)) {
            col = 0;
        }
        else {
            auto x1 = x() + (_show_row_header ? _cell_width(0) : 0);
            auto x2 = x() + w();
            for (auto c = _start_col; c <= _cols; c++) {
                auto cw = _cell_width(c);
                if (x1 > x2) {
                    break;
                }
                else if (mx > x1 && (mx < x1 + cw || c == _cols)) {
                    col = c;
                    break;
                }
                x1 += cw;
            }
        }
    }
}
int flw::TableDisplay::handle(int event) {
    auto ret = 2;
    if (_rows > 0 && _cols > 0) {
        if (event == FL_FOCUS) {
            ret = 1;
        }
        else if (event == FL_LEAVE) {
            fl_cursor(FL_CURSOR_DEFAULT);
        }
        else if (event == FL_DRAG) {
            ret = _ev_mouse_drag();
        }
        else if (event == FL_KEYDOWN) {
            ret = _ev_keyboard_down();
        }
        else if (event == FL_MOVE) {
            ret = _ev_mouse_move();
        }
        else if (event == FL_PUSH) {
            ret = _ev_mouse_click();
        }
    }
    if (ret == 0 || ret == 1) {
        return ret;
    }
    else {
        return Fl_Group::handle(event);
    }
}
void flw::TableDisplay::header(bool row, bool col) {
    _show_row_header = row;
    _show_col_header = col;
    redraw();
}
void flw::TableDisplay::lines(bool ver, bool hor) {
    _show_ver_lines = ver;
    _show_hor_lines = hor;
    redraw();
}
void flw::TableDisplay::_move_cursor(_TABLEDISPLAY_MOVE move) {
    if (_edit == nullptr && _rows > 0 && _cols > 0 && _select != flw::TableDisplay::SELECT::NO) {
        auto r     = _curr_row;
        auto c     = _curr_col;
        auto range = (int) ((h() - _hor->h() - (_show_row_header ? _height : 0)) / _height);
        if (r < 1) {
            r = 1;
        }
        else if (r > _rows) {
            r = _rows;
        }
        else {
            if (range > 0) {
                switch (move) {
                    case _TABLEDISPLAY_MOVE::FIRST_ROW:
                        r = 1;
                        break;
                    case _TABLEDISPLAY_MOVE::UP:
                        r = r > 1 ? r - 1 : 1;
                        break;
                    case _TABLEDISPLAY_MOVE::SCROLL_UP:
                        r = r > 8 ? r - 8 : 1;
                        break;
                    case _TABLEDISPLAY_MOVE::PAGE_UP:
                        r = _start_row - range > 0 ? _start_row - range : 1;
                        _start_row = r;
                        break;
                    case _TABLEDISPLAY_MOVE::DOWN:
                        r = r < _rows ? r + 1 : _rows;
                        break;
                    case _TABLEDISPLAY_MOVE::SCROLL_DOWN:
                        r = r < _rows - 8 ? r + 8 : _rows;
                        break;
                    case _TABLEDISPLAY_MOVE::PAGE_DOWN:
                        r = _start_row + range <= _rows ? _start_row + range : _rows;
                        _start_row = r;
                        break;
                    case _TABLEDISPLAY_MOVE::LAST_ROW:
                        r = _rows;
                        break;
                    default:
                        break;
                }
            }
        }
        if (c < 1) {
            c = 1;
        }
        else if (c > _cols) {
            c = _cols;
        }
        else {
            switch (move) {
                case _TABLEDISPLAY_MOVE::LEFT:
                    c = c > 1 ? c - 1 : 1;
                    break;
                case _TABLEDISPLAY_MOVE::SCROLL_LEFT:
                    c = c > 4 ? c - 4 : 1;
                    break;
                case _TABLEDISPLAY_MOVE::RIGHT:
                    c = c < _cols ? c + 1 : _cols;
                    break;
                case _TABLEDISPLAY_MOVE::SCROLL_RIGHT:
                    c = c < _cols - 4 ? c + 4 : _cols;
                    break;
                case _TABLEDISPLAY_MOVE::FIRST_COL:
                    c = 1;
                    break;
                case _TABLEDISPLAY_MOVE::LAST_COL:
                    c = _cols;
                    break;
                default:
                    break;
            }
        }
        if (r > 0 && r <= _rows && c > 0 && c <= _cols) {
            active_cell(r, c, true);
        }
    }
}
void flw::TableDisplay::show_cell(int row, int col) {
    if (_rows > 0 && row > 0 && row <= _rows) {
        auto rc = (h() - Fl::scrollbar_size() - (_show_col_header ? _height : 0)) / _height;
        if (row <= _start_row) {
            _start_row = row;
        }
        else if (row >= _start_row + rc) {
            _start_row = row - rc + 1;
        }
    }
    if (_cols > 0 && col > 0 && col <= _cols) {
        if (col <= _start_col) {
            _start_col = col;
        }
        else {
            LOOP:
            if (_start_col < col) {
                auto x1 = x() + (_show_row_header ? _cell_width(0) : 0);
                auto x2 = x() + w() - Fl::scrollbar_size();
                for (auto c = _start_col; c <= col; c++) {
                    auto cw = _cell_width(c);
                    if (x1 + cw >= x2) {
                        if (col - c < 20) {
                            _start_col++;
                        }
                        else {
                            _start_col = col - 20;
                        }
                        goto LOOP;
                    }
                    x1 += cw;
                }
            }
        }
    }
    redraw();
}
void flw::TableDisplay::size(int rows, int cols) {
    if (rows > -1 && _cols > -1) {
        _rows      = rows;
        _cols      = cols;
        _curr_row  = 1;
        _curr_col  = 1;
        _start_row = 1;
        _start_col = 1;
        _set_event(_curr_row, _curr_col, flw::TableDisplay::EVENT::SIZE);
        do_callback();
    }
    redraw();
}
void flw::TableDisplay::_update_scrollbars() {
    if (_rows > 0 && _cols > 0) {
        if (_disable_hor == true) {
            _hor->hide();
        }
        else {
            auto cols  = 0;
            auto width = _show_row_header ? _cell_width(0) : 0;
            for (auto c = 1; c <= _cols; c++) {
                width += _cell_width(c);
                if (width > (w() - Fl::scrollbar_size())) {
                    break;
                }
                else {
                    cols++;
                }
            }
            if (_cols > cols) {
                _hor->slider_size(0.2);
                _hor->range(1, _cols);
                _hor->show();
            }
            else {
                _hor->hide();
                if (_start_col > 0) {
                    _start_col = 1;
                }
            }
        }
        if (_disable_ver == true) {
            _ver->hide();
        }
        else {
            auto rows = (h() - Fl::scrollbar_size() - (_show_col_header ? _height : 0)) / _height;
            if (_rows > rows) {
                _ver->slider_size(0.2);
                _ver->range(1, _rows);
                _ver->show();
            }
            else {
                _ver->hide();
                if (_start_row > 0) {
                    _start_row = 1;
                }
            }
        }
    }
    else {
        _ver->hide();
        _hor->hide();
    }
    if (_ver->visible() != 0 && _hor->visible() != 0) {
        _ver->resize(x() + w() - Fl::scrollbar_size() - 1, y() + 1, Fl::scrollbar_size(), h() - Fl::scrollbar_size() - 2);
        _hor->resize(x() + 1, y() + h() - Fl::scrollbar_size() - 1, w() - Fl::scrollbar_size() - 2, Fl::scrollbar_size());
    }
    else if (_ver->visible() != 0) {
        _ver->resize(x() + w() - Fl::scrollbar_size() - 1, y() + 1, Fl::scrollbar_size(), h() - 2);
        _hor->resize(0, 0, 0, 0);
    }
    else if (_hor->visible() != 0) {
        _hor->resize(x() + 1, y() + h() - Fl::scrollbar_size() - 1, w() - 2, Fl::scrollbar_size());
        _ver->resize(0, 0, 0, 0);
    }
    else {
        _ver->resize(0, 0, 0, 0);
        _hor->resize(0, 0, 0, 0);
    }
    _ver->Fl_Valuator::value(_start_row);
    _hor->Fl_Valuator::value(_start_col);
}
#include <assert.h>
#include <errno.h>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_File_Chooser.H>
#include <FL/Fl_Float_Input.H>
#include <FL/Fl_Input_Choice.H>
#include <FL/Fl_Int_Input.H>
#include <FL/Fl_Secret_Input.H>
#include <FL/Fl_Value_Slider.H>
#include <FL/fl_draw.H>
#include <FL/fl_show_colormap.H>
const char* flw::TableEditor::SELECT_DATE = "Select Date";
const char* flw::TableEditor::SELECT_DIR  = "Select Directory";
const char* flw::TableEditor::SELECT_FILE = "Select File";
const char* flw::TableEditor::SELECT_LIST = "Select String";
namespace flw {
    namespace tableeditor {
        double to_double(std::string string, double def) {
            try { return std::stod(string.c_str(), 0); }
            catch (...) { return def; }
        }
        int to_doubles(std::string string, double numbers[], size_t size) {
            auto end = (char*) nullptr;
            auto in  = string.c_str();
            auto f   = (size_t) 0;
            errno = 0;
            for (; f < size; f++) {
                numbers[f] = strtod(in, &end);
                if (errno != 0 || in == end) return f;
                in = end;
            }
            return f;
        }
        int64_t to_int(std::string string, int64_t def) {
            try { return std::stoll(string.c_str(), 0, 0); }
            catch (...) { return def; }
        }
        int to_ints(std::string string, int64_t numbers[], size_t size) {
            auto end = (char*) nullptr;
            auto in  = string.c_str();
            auto f   = (size_t) 0;
            errno = 0;
            for (; f < size; f++) {
                numbers[f] = strtoll(in, &end, 10);
                if (errno != 0 || in == end) return f;
                in = end;
            }
            return f;
        }
    }
}
flw::TableEditor::TableEditor(int X, int Y, int W, int H, const char* l) : TableDisplay(X, Y, W, H, l) {
    TableEditor::clear();
}
void flw::TableEditor::clear() {
    TableDisplay::clear();
    _send_changed_event_always = false;
    _edit2 = nullptr;
    _edit3 = nullptr;
}
void flw::TableEditor::_delete_current_cell() {
    if (_curr_row > 0 && _curr_col > 0) {
        auto edit = cell_edit(_curr_row, _curr_col);
        if (edit == true) {
            auto rend = cell_rend(_curr_row, _curr_col);
            auto set  = false;
            switch (rend) {
                case flw::TableEditor::REND::TEXT:
                case flw::TableEditor::REND::SECRET:
                case flw::TableEditor::REND::INPUT_CHOICE:
                case flw::TableEditor::REND::DLG_FILE:
                case flw::TableEditor::REND::DLG_DIR:
                case flw::TableEditor::REND::DLG_LIST:
                    set = cell_value(_curr_row, _curr_col, "");
                    break;
                case flw::TableEditor::REND::INTEGER:
                    set = cell_value(_curr_row, _curr_col, "0");
                    break;
                case flw::TableEditor::REND::NUMBER:
                    set = cell_value(_curr_row, _curr_col, "0.0");
                    break;
                case flw::TableEditor::REND::BOOLEAN:
                    set = cell_value(_curr_row, _curr_col, "0");
                    break;
                case flw::TableEditor::REND::DLG_DATE:
                    set = cell_value(_curr_row, _curr_col, "1970-01-01");
                    break;
                case flw::TableEditor::REND::DLG_COLOR:
                    set = cell_value(_curr_row, _curr_col, "56");
                    break;
                default:
                    break;
            }
            if (set == true) {
                _set_event(_curr_row, _curr_col, flw::TableEditor::EVENT::CHANGED);
                do_callback();
                redraw();
            }
        }
    }
}
void flw::TableEditor::_draw_cell(int row, int col, int X, int Y, int W, int H, bool ver, bool hor, bool current) {
    fl_push_clip(X, Y, W + 1, H);
    auto align     = cell_align(row, col);
    auto textcolor = cell_textcolor(row, col);
    auto textfont  = cell_textfont(row, col);
    auto textsize  = cell_textsize(row, col);
    auto val       = ((TableDisplay*) this)->cell_value(row, col);
    assert(val);
    if (row > 0 && col > 0) {
        auto format = cell_format(row, col);
        auto rend   = cell_rend(row, col);
        auto color  = cell_color(row, col);
        char buffer[100];
        if (current) {
            color = selection_color();
        }
        fl_rectf(X + 1, Y, W + 1, H, color);
        if (rend == flw::TableEditor::REND::SECRET) {
            fl_font(textfont, textsize);
            fl_color(textcolor);
            if (format == flw::TableEditor::FORMAT::SECRET_DOT) {
                fl_draw("••••••", X + 4, Y + 2, W - 8, H - 4, align, 0, 1);
            }
            else {
                fl_draw("******", X + 4, Y + 2, W - 8, H - 4, align, 0, 1);
            }
        }
        else if (rend == flw::TableEditor::REND::SLIDER) {
            double nums[4];
            if (tableeditor::to_doubles(val, nums, 4) == 4) {
                auto range = 0.0;
                if ((nums[2] - nums[1]) > 0.0001) {
                    range = (nums[0] - nums[1]) / (nums[2] - nums[1]);
                }
                auto width = (int) (range * (W - 3));
                if (width > 0) {
                    fl_draw_box(FL_FLAT_BOX, X + 2, Y + 2, width, H - 3, textcolor);
                }
            }
        }
        else if (rend == flw::TableEditor::REND::DLG_COLOR) {
            fl_draw_box(FL_FLAT_BOX, X + 2, Y + 2, W - 3, H - 3, (Fl_Color) tableeditor::to_int(val, 0));
        }
        else if (rend == flw::TableEditor::REND::DLG_DATE) {
            auto        date = Date::FromString(val);
            std::string string;
            if (format == flw::TableEditor::FORMAT::DATE_WORLD) {
                string = date.format(Date::FORMAT::WORLD);
            }
            else if (format == flw::TableEditor::FORMAT::DATE_US) {
                string = date.format(Date::FORMAT::US);
            }
            else {
                string = date.format(Date::FORMAT::ISO_LONG);
            }
            fl_font(textfont, textsize);
            fl_color(textcolor);
            _draw_text(string.c_str(), X + 4, Y + 2, W - 8, H - 4, align);
        }
        else if (rend == flw::TableEditor::REND::BOOLEAN) {
            auto bw  = textsize + 4;
            auto y_1 = Y + (H / 2) - (bw / 2);
            auto x_1 = 0;
            if (align == FL_ALIGN_RIGHT) {
                x_1 = X + W - bw - 6;
            }
            else if (align == FL_ALIGN_CENTER) {
                x_1 = X + (W / 2) - (bw / 2);
            }
            else if (align == FL_ALIGN_LEFT) {
                x_1 = X + 6;
            }
            fl_draw_box(FL_DOWN_BOX, x_1, y_1, bw, bw, FL_WHITE);
            if (*val == '1') {
                bw -= 6;
                fl_draw_box(FL_ROUND_UP_BOX, x_1 + 3, y_1 + 3, bw, bw, selection_color());
            }
        }
        else if (rend == flw::TableEditor::REND::INTEGER) {
            auto num = tableeditor::to_int(val, 0);
            fl_font(textfont, textsize);
            fl_color(textcolor);
            if (format == flw::TableEditor::FORMAT::INT_SEP) {
                auto s = util::format_int(num);
                _draw_text(s.c_str(), X + 4, Y + 2, W - 8, H - 4, align);
            }
            else {
                snprintf(buffer, 100, "%lld", (long long int) num);
                _draw_text(buffer, X + 4, Y + 2, W - 8, H - 4, align);
            }
        }
        else if (rend == flw::TableEditor::REND::NUMBER || rend == flw::TableEditor::REND::VALUE_SLIDER) {
            auto num = tableeditor::to_double(val, 0.0);
            if (rend == flw::TableEditor::REND::VALUE_SLIDER) {
                double nums[1];
                if (tableeditor::to_doubles(val, nums, 1) == 1) {
                    num = nums[0];
                }
            }
            if (format == flw::TableEditor::FORMAT::DEC_0) {
                snprintf(buffer, 100, "%.0f", num);
            }
            else if (format == flw::TableEditor::FORMAT::DEC_1) {
                snprintf(buffer, 100, "%.1f", num);
            }
            else if (format == flw::TableEditor::FORMAT::DEC_2) {
                snprintf(buffer, 100, "%.2f", num);
            }
            else if (format == flw::TableEditor::FORMAT::DEC_3) {
                snprintf(buffer, 100, "%.3f", num);
            }
            else if (format == flw::TableEditor::FORMAT::DEC_4) {
                snprintf(buffer, 100, "%.4f", num);
            }
            else if (format == flw::TableEditor::FORMAT::DEC_5) {
                snprintf(buffer, 100, "%.5f", num);
            }
            else if (format == flw::TableEditor::FORMAT::DEC_6) {
                snprintf(buffer, 100, "%.6f", num);
            }
            else if (format == flw::TableEditor::FORMAT::DEC_7) {
                snprintf(buffer, 100, "%.7f", num);
            }
            else if (format == flw::TableEditor::FORMAT::DEC_8) {
                snprintf(buffer, 100, "%.8f", num);
            }
            else {
                snprintf(buffer, 100, "%f", num);
            }
            fl_font(textfont, textsize);
            fl_color(textcolor);
            _draw_text(buffer, X + 4, Y + 2, W - 8, H - 4, align);
        }
        else {
            fl_font(textfont, textsize);
            fl_color(textcolor);
            _draw_text(val, X + 4, Y + 2, W - 8, H - 4, align);
        }
        fl_color(FL_DARK3);
        if (ver == true) {
            fl_line(X, Y, X, Y + H);
            fl_line(X + W, Y, X + W, Y + H);
        }
        if (hor == true) {
            fl_line(X, Y, X + W, Y);
            fl_line(X, Y + H - (row == _rows ? 1 : 0), X + W, Y + H - (row == _rows ? 1 : 0));
        }
    }
    else {
        fl_draw_box(FL_THIN_UP_BOX, X, Y, W + 1, H + (row == _rows ? 0 : 1), FL_BACKGROUND_COLOR);
        fl_font(textfont, textsize);
        fl_color(FL_FOREGROUND_COLOR);
        _draw_text(val, X + 4, Y + 2, W - 8, H - 4, align);
    }
    fl_pop_clip();
}
void flw::TableEditor::_edit_create() {
    auto rend      = cell_rend(_curr_row, _curr_col);
    auto align     = cell_align(_curr_row, _curr_col);
    auto color     = cell_color(_curr_row, _curr_col);
    auto textcolor = FL_FOREGROUND_COLOR;
    auto textfont  = cell_textfont(_curr_row, _curr_col);
    auto textsize  = cell_textsize(_curr_row, _curr_col);
    auto val       = ((TableDisplay*) this)->cell_value(_curr_row, _curr_col);
    assert(val);
    if (rend == flw::TableEditor::REND::TEXT ||
        rend == flw::TableEditor::REND::INTEGER ||
        rend == flw::TableEditor::REND::NUMBER ||
        rend == flw::TableEditor::REND::SECRET) {
        auto w = (Fl_Input*) nullptr;
        if (rend == flw::TableEditor::REND::TEXT) {
            w = new Fl_Input(0, 0, 0, 0);
        }
        else if (rend == flw::TableEditor::REND::INTEGER) {
            w = new Fl_Int_Input(0, 0, 0, 0);
        }
        else if (rend == flw::TableEditor::REND::NUMBER) {
            w = new Fl_Float_Input(0, 0, 0, 0);
        }
        else if (rend == flw::TableEditor::REND::SECRET) {
            w = new Fl_Secret_Input(0, 0, 0, 0);
        }
        w->align(align);
        w->box(FL_BORDER_BOX);
        w->color(color);
        w->labelsize(textsize);
        w->textcolor(textcolor);
        w->textfont(textfont);
        w->textsize(textsize);
        w->value(val);
        w->insert_position(0, strlen(val));
        _edit = w;
    }
    else if (rend == flw::TableEditor::REND::BOOLEAN) {
        auto w = new Fl_Check_Button(0, 0, 0, 0);
        w->box(FL_BORDER_BOX);
        w->color(color);
        w->labelcolor(textcolor);
        w->labelsize(textsize);
        w->value(*val && val[0] == '1' ? 1 : 0);
        _edit = w;
    }
    else if (rend == flw::TableEditor::REND::SLIDER) {
        auto w = (Fl_Slider*) nullptr;
        double nums[4];
        if (tableeditor::to_doubles(val, nums, 4) == 4) {
            w = new Fl_Slider(0, 0, 0, 0);
            w->color(color);
            w->selection_color(textcolor);
            w->range(nums[1], nums[2]);
            w->step(nums[3]);
            w->value(nums[0]);
            w->type(FL_HOR_FILL_SLIDER);
            w->box(FL_BORDER_BOX);
            _edit = w;
        }
    }
    else if (rend == flw::TableEditor::REND::VALUE_SLIDER) {
        auto w = (Fl_Slider*) nullptr;
        double nums[4];
        if (tableeditor::to_doubles(val, nums, 4) == 4) {
            w = new Fl_Value_Slider(0, 0, 0, 0);
            w->color(color);
            w->selection_color(textcolor);
            w->range(nums[1], nums[2]);
            w->step(nums[3]);
            w->value(nums[0]);
            w->type(FL_HOR_FILL_SLIDER);
            w->box(FL_BORDER_BOX);
            ((Fl_Value_Slider*) w)->textsize(textsize * 0.8);
            _edit = w;
        }
    }
    else if (rend == flw::TableEditor::REND::CHOICE) {
        auto choices = cell_choice(_curr_row, _curr_col);
        if (choices.size() > 0) {
            auto w      = new Fl_Choice(0, 0, 0, 0);
            auto select = (std::size_t) 0;
            w->align(align);
            w->box(FL_BORDER_BOX);
            w->down_box(FL_BORDER_BOX);
            w->labelsize(textsize);
            w->textfont(textfont);
            w->textsize(textsize);
            for (std::size_t f = 0; f < choices.size(); f++) {
                auto& s = choices[f];
                w->add(s.c_str());
                if (s == val) {
                    select = f;
                }
            }
            w->value(select);
            _edit = w;
        }
    }
    else if (rend == flw::TableEditor::REND::INPUT_CHOICE) {
        auto choices = cell_choice(_curr_row, _curr_col);
        if (choices.size() > 0) {
            auto w = new Fl_Input_Choice(0, 0, 0, 0);
            w->box(FL_NO_BOX);
            w->down_box(FL_BORDER_BOX);
            w->input()->box(FL_BORDER_BOX);
            w->input()->textfont(textfont);
            w->input()->textsize(textsize);
            w->labelsize(textsize);
            w->menubutton()->box(FL_BORDER_BOX);
            w->menubutton()->textfont(textfont);
            w->menubutton()->textsize(textsize);
            w->textsize(textsize);
            for (auto& s : choices) {
                w->add(s.c_str());
            }
            if (*val) {
                w->value(val);
                w->input()->insert_position(0, strlen(val));
            }
            _edit  = w;
            _edit2 = w->input();
            _edit3 = w->menubutton();
        }
    }
    if (_edit != nullptr) {
        add(_edit);
        if (_edit2 != nullptr) {
            Fl::focus(_edit2);
        }
        else {
            Fl::focus(_edit);
        }
    }
}
void flw::TableEditor::_edit_quick(const char* key) {
    auto rend = cell_rend(_curr_row, _curr_col);
    auto val  = ((TableDisplay*) this)->cell_value(_curr_row, _curr_col);
    char buffer[100];
    assert(val);
    if (rend == flw::TableEditor::REND::INTEGER) {
        auto num = tableeditor::to_int(val, 0);
        if (strcmp(key, "+") == 0) {
            num++;
        }
        else if (strcmp(key, "++") == 0) {
            num += 10;
        }
        else if (strcmp(key, "+++") == 0) {
            num += 100;
        }
        else if (strcmp(key, "-") == 0) {
            num--;
        }
        else if (strcmp(key, "--") == 0) {
            num -= 10;
        }
        else if (strcmp(key, "---") == 0) {
            num -= 100;
        }
        snprintf(buffer, 100, "%lld", (long long int) num);
        if ((_send_changed_event_always == true || strcmp(val, buffer) != 0) && cell_value(_curr_row, _curr_col, buffer) == true) {
            _set_event(_curr_row, _curr_col, flw::TableEditor::EVENT::CHANGED);
            do_callback();
        }
    }
    else if (rend == flw::TableEditor::REND::NUMBER) {
        auto num = tableeditor::to_double(val, 0.0);
        if (strcmp(key, "+") == 0) {
            num += 0.1;
        }
        else if (strcmp(key, "++") == 0) {
            num += 1.0;
        }
        else if (strcmp(key, "+++") == 0) {
            num += 10.0;
        }
        else if (strcmp(key, "-") == 0) {
            num -= 0.1;
        }
        else if (strcmp(key, "--") == 0) {
            num -= 1.0;
        }
        else if (strcmp(key, "---") == 0) {
            num -= 10.0;
        }
        snprintf(buffer, 100, "%f", num);
        if ((_send_changed_event_always == true || strcmp(val, buffer) != 0) && cell_value(_curr_row, _curr_col, buffer) == true) {
            _set_event(_curr_row, _curr_col, flw::TableEditor::EVENT::CHANGED);
            do_callback();
        }
    }
    else if (rend == flw::TableEditor::REND::DLG_DATE) {
        auto date = Date::FromString(val);
        if (strcmp(key, "+") == 0) {
            date.add_days(1);
        }
        else if (strcmp(key, "++") == 0) {
            date.add_months(1);
        }
        else if (strcmp(key, "+++") == 0) {
            date.year(date.year() + 1);
        }
        else if (strcmp(key, "-") == 0) {
            date.add_days(-1);
        }
        else if (strcmp(key, "--") == 0) {
            date.add_months(-1);
        }
        else if (strcmp(key, "---") == 0) {
            date.year(date.year() - 1);
        }
        auto string = date.format(Date::FORMAT::ISO_LONG);
        if ((_send_changed_event_always == true || string != val) && cell_value(_curr_row, _curr_col, string.c_str()) == true) {
            _set_event(_curr_row, _curr_col, flw::TableEditor::EVENT::CHANGED);
            do_callback();
        }
    }
    else if (rend == flw::TableEditor::REND::SLIDER || rend == flw::TableEditor::REND::VALUE_SLIDER) {
        double nums[4];
        if (tableeditor::to_doubles(val, nums, 4) == 4) {
            if (strcmp(key, "+") == 0) {
                nums[0] += nums[3];
            }
            else if (strcmp(key, "++") == 0) {
                nums[0] += (nums[3] * 10);
            }
            else if (strcmp(key, "+++") == 0) {
                nums[0] += (nums[3] * 100);
            }
            else if (strcmp(key, "-") == 0) {
                nums[0] -= nums[3];
            }
            else if (strcmp(key, "--") == 0) {
                nums[0] -= (nums[3] * 10);
            }
            else if (strcmp(key, "---") == 0) {
                nums[0] -= (nums[3] * 100);
            }
            if (nums[0] < nums[1]) {
                nums[0] = nums[1];
            }
            else if (nums[0] > nums[2]) {
                nums[0] = nums[2];
            }
            auto val2 = FormatSlider(nums[0], nums[1], nums[2], nums[3]);
            if ((_send_changed_event_always == true || strcmp(val, val2) != 0) && cell_value(_curr_row, _curr_col, val2) == true) {
                _set_event(_curr_row, _curr_col, flw::TableEditor::EVENT::CHANGED);
                do_callback();
            }
        }
    }
}
void flw::TableEditor::_edit_show() {
    auto rend = cell_rend(_curr_row, _curr_col);
    auto val  = ((TableDisplay*) this)->cell_value(_curr_row, _curr_col);
    char buffer[100];
    assert(val);
    if (rend == flw::TableEditor::REND::DLG_COLOR) {
        auto color1 = (int) tableeditor::to_int(val, 0);
        auto color2 = (int) fl_show_colormap((Fl_Color) color1);
        snprintf(buffer, 20, "%d", color2);
        if ((_send_changed_event_always == true || color1 != color2) && cell_value(_curr_row, _curr_col, buffer) == true) {
            _set_event(_curr_row, _curr_col, flw::TableEditor::EVENT::CHANGED);
            do_callback();
        }
    }
    else if (rend == flw::TableEditor::REND::DLG_FILE) {
        auto result = fl_file_chooser(flw::TableEditor::SELECT_FILE, "", val, 0);
        if ((_send_changed_event_always == true || (result != nullptr && strcmp(val, result) != 0)) && cell_value(_curr_row, _curr_col, result) == true) {
            _set_event(_curr_row, _curr_col, flw::TableEditor::EVENT::CHANGED);
            do_callback();
        }
    }
    else if (rend == flw::TableEditor::REND::DLG_DIR) {
        auto result = fl_dir_chooser(flw::TableEditor::SELECT_DIR, val);
        if ((_send_changed_event_always == true || (result != nullptr && strcmp(val, result) != 0)) && cell_value(_curr_row, _curr_col, result) == true) {
            _set_event(_curr_row, _curr_col, flw::TableEditor::EVENT::CHANGED);
            do_callback();
        }
    }
    else if (rend == flw::TableEditor::REND::DLG_DATE) {
        auto date1  = Date::FromString(val);
        auto date2  = Date(date1);
        auto result = flw::dlg::date(flw::TableEditor::SELECT_DATE, date1, top_window());
        auto string = date1.format(Date::FORMAT::ISO_LONG);
        if ((_send_changed_event_always == true || (result == true && date1 != date2)) && cell_value(_curr_row, _curr_col, string.c_str()) == true) {
            _set_event(_curr_row, _curr_col, flw::TableEditor::EVENT::CHANGED);
            do_callback();
        }
    }
    else if (rend == flw::TableEditor::REND::DLG_LIST) {
        auto choices = cell_choice(_curr_row, _curr_col);
        if (choices.size() > 0) {
            auto row = dlg::select(flw::TableEditor::SELECT_LIST, choices, val);
            if (row > 0) {
                auto& string = choices[row - 1];
                if ((_send_changed_event_always == true || string != val) && cell_value(_curr_row, _curr_col, string.c_str()) == true) {
                    _set_event(_curr_row, _curr_col, flw::TableEditor::EVENT::CHANGED);
                    do_callback();
                }
            }
        }
    }
}
void flw::TableEditor::_edit_start(const char* key) {
    if (_select != flw::TableEditor::SELECT::NO && _edit == nullptr && _curr_row >= 1 && _curr_col >= 1 && cell_edit(_curr_row, _curr_col)) {
        Fl::event_clicks(0);
        Fl::event_is_click(0);
        flw::TableEditor::REND rend = cell_rend(_curr_row, _curr_col);
        if (*key != 0) {
            _edit_quick(key);
        }
        else if (rend == flw::TableEditor::REND::DLG_COLOR ||
                 rend == flw::TableEditor::REND::DLG_FILE ||
                 rend == flw::TableEditor::REND::DLG_DIR ||
                 rend == flw::TableEditor::REND::DLG_DATE ||
                 rend == flw::TableEditor::REND::DLG_LIST) {
            _edit_show();
        }
        else {
            _edit_create();
        }
        redraw();
    }
}
void flw::TableEditor::_edit_stop(bool save) {
    if (_edit) {
        auto rend = cell_rend(_curr_row, _curr_col);
        auto val  = ((TableDisplay*) this)->cell_value(_curr_row, _curr_col);
        auto stop = true;
        if (save == true) {
            if (rend == flw::TableEditor::REND::TEXT ||
                rend == flw::TableEditor::REND::INTEGER ||
                rend == flw::TableEditor::REND::NUMBER ||
                rend == flw::TableEditor::REND::SECRET) {
                auto input = (Fl_Input*) _edit;
                auto val2  = input->value();
                char buffer[100];
                if (rend == flw::TableEditor::REND::INTEGER) {
                    snprintf(buffer, 100, "%d", (int) tableeditor::to_int(val2, 0));
                    val2 = buffer;
                }
                else if (rend == flw::TableEditor::REND::NUMBER) {
                    auto num = tableeditor::to_double(val2, 0.0);
                    snprintf(buffer, 100, "%f", num);
                    val2 = buffer;
                }
                if (val2 == 0) {
                    stop = true;
                    save = false;
                }
                else if (strcmp(val, val2) == 0) {
                    stop = true;
                    save = false;
                }
                else {
                    stop = cell_value(_curr_row, _curr_col, val2);
                }
            }
            else if (rend == flw::TableEditor::REND::BOOLEAN) {
                auto button = (Fl_Check_Button*) _edit;
                auto val2   = "0";
                if (button->value())
                    val2 = "1";
                if (strcmp(val, val2) == 0) {
                    stop = true;
                    save = false;
                }
                else {
                    stop = cell_value(_curr_row, _curr_col, val2);
                }
            }
            else if (rend == flw::TableEditor::REND::SLIDER || rend == flw::TableEditor::REND::VALUE_SLIDER) {
                auto slider = (Fl_Slider*) _edit;
                auto val2   = FormatSlider(slider->value(), slider->minimum(), slider->maximum(), slider->step());
                if (strcmp(val, val2) == 0) {
                    stop = true;
                    save = false;
                }
                else {
                    stop = cell_value(_curr_row, _curr_col, val2);
                }
            }
            else if (rend == flw::TableEditor::REND::CHOICE) {
                auto choice = (Fl_Choice*) _edit;
                auto val2   = choice->text();
                if (val2 == 0) {
                    stop = true;
                    save = false;
                }
                else if (strcmp(val, val2) == 0) {
                    stop = true;
                    save = false;
                }
                else {
                    stop = cell_value(_curr_row, _curr_col, val2);
                }
            }
            else if (rend == flw::TableEditor::REND::INPUT_CHOICE) {
                auto input_choice = (Fl_Input_Choice*) _edit;
                auto val2         = input_choice->value();
                if (val2 == 0) {
                    stop = true;
                    save = false;
                }
                else if (strcmp(val, val2) == 0) {
                    stop = true;
                    save = false;
                }
                else {
                    stop = cell_value(_curr_row, _curr_col, val2);
                }
            }
        }
        if (stop == true) {
            remove(_edit);
            delete _edit;
            _edit            = nullptr;
            _edit2           = nullptr;
            _edit3           = nullptr;
            _current_cell[0] = 0;
            _current_cell[1] = 0;
            _current_cell[2] = 0;
            _current_cell[3] = 0;
            if (_send_changed_event_always == true || save == true) {
                _set_event(_curr_row, _curr_col, flw::TableEditor::EVENT::CHANGED);
                do_callback();
            }
            Fl::check();
            Fl::focus(this);
            fl_cursor(FL_CURSOR_DEFAULT);
            redraw();
        }
        else {
            fl_beep();
        }
    }
}
int flw::TableEditor::_ev_keyboard_down() {
    auto key   = Fl::event_key();
    auto text  = std::string(Fl::event_text());
    auto alt   = Fl::event_alt() != 0;
    auto cmd   = Fl::event_command() != 0;
    auto shift = Fl::event_shift() != 0;
    if (_edit) {
        if (key == FL_Enter || key == FL_KP_Enter) {
            _edit_stop();
            return 1;
        }
        else if (key == FL_Escape) {
            _edit_stop(false);
            return 1;
        }
    }
    else {
        if (key == FL_Enter || key == FL_KP_Enter) {
            _edit_start();
            return 1;
        }
        else if (cmd == true && key == FL_BackSpace) {
            _delete_current_cell();
            return 1;
        }
        else if (key == FL_Delete) {
            _delete_current_cell();
        }
        else if (cmd == true && key == 'x') {
            auto val = ((TableDisplay*) this)->cell_value(_curr_row, _curr_col);
            Fl::copy(val, strlen(val), 1);
            _delete_current_cell();
            return 1;
        }
        else if (cmd == true && key == 'v') {
            Fl::paste(*this, 1);
            return 1;
        }
        else if (alt == true && shift == true && (key == '+' || text == "+" || key == FL_KP + '+')) {
            _edit_start("+++");
            return 1;
        }
        else if (alt == true && (key == '+' || text == "+" || key == FL_KP + '+')) {
            _edit_start("++");
            return 1;
        }
        else if (key == '+' || text == "+" || key == FL_KP + '+') {
            _edit_start("+");
            return 1;
        }
        else if (alt == true && shift == true && (key == '-' || text == "-" || key == FL_KP + '-')) {
            _edit_start("---");
            return 1;
        }
        else if (alt == true && (key == '-' || text == "-" || key == FL_KP + '-')) {
            _edit_start("--");
            return 1;
        }
        else if (key == '-' || text == "-" || key == FL_KP + '-') {
            _edit_start("-");
            return 1;
        }
    }
    return 0;
}
int flw::TableEditor::_ev_mouse_click () {
    auto row         = 0;
    auto col         = 0;
    auto current_row = _curr_row;
    auto current_col = _curr_col;
    _get_cell_below_mouse(row, col);
    if (_edit && (row != current_row || col != current_col)) {
        _edit_stop();
        return 1;
    }
    else if (row == current_row && col == current_col && Fl::event_clicks() == 1 && Fl::event_is_click()) {
        _edit_start();
        return 1;
    }
    return 2;
}
int flw::TableEditor::_ev_paste() {
    auto text = Fl::event_text();
    if (_curr_row > 0 && _curr_col > 0 && text && *text) {
        auto        rend = cell_rend(_curr_row, _curr_col);
        auto        val  = ((TableDisplay*) this)->cell_value(_curr_row, _curr_col);
        char        buffer[100];
        std::string string;
        switch(rend) {
            case flw::TableEditor::REND::CHOICE:
            case flw::TableEditor::REND::DLG_LIST:
            case flw::TableEditor::REND::SLIDER:
            case flw::TableEditor::REND::VALUE_SLIDER:
                return 1;
            case flw::TableEditor::REND::DLG_DIR:
            case flw::TableEditor::REND::DLG_FILE:
            case flw::TableEditor::REND::INPUT_CHOICE:
            case flw::TableEditor::REND::SECRET:
            case flw::TableEditor::REND::TEXT:
                break;
            case flw::TableEditor::REND::BOOLEAN:
                if (strcmp("1", text) == 0 || strcmp("true", text) == 0) {
                    text = "1";
                }
                else if (strcmp("0", text) == 0 || strcmp("false", text) == 0) {
                    text = "0";
                }
                else {
                    return 1;
                }
            case flw::TableEditor::REND::DLG_COLOR:
            case flw::TableEditor::REND::INTEGER:
                if (*text < '0' || *text > '9') {
                    return 1;
                }
                else {
                    auto num = tableeditor::to_int(text, 0);
                    snprintf(buffer, 100, "%lld", (long long int) num);
                    text = buffer;
                    if (rend == flw::TableEditor::REND::DLG_COLOR && (num < 0 || num > 255)) {
                        return 1;
                    }
                    else {
                        break;
                    }
                }
            case flw::TableEditor::REND::NUMBER: {
                if (*text < '0' || *text > '9') {
                    return 1;
                }
                else {
                    auto num = tableeditor::to_double(text, 0.0);
                    snprintf(buffer, 100, "%f", num);
                    text = buffer;
                    break;
                }
            }
            case flw::TableEditor::REND::DLG_DATE: {
                auto date = Date::FromString(text);
                if (date.year() == 1 && date.month() == 1 && date.day() == 1) {
                    return 1;
                }
                else {
                    string = date.format(Date::FORMAT::ISO_LONG);
                    text = string.c_str();
                    break;
                }
            }
        }
        if ((_send_changed_event_always == true || strcmp(val, text) != 0) && cell_value(_curr_row, _curr_col, text) == true) {
            _set_event(_curr_row, _curr_col, flw::TableEditor::EVENT::CHANGED);
            do_callback();
            redraw();
        }
    }
    return 1;
}
const char* flw::TableEditor::FormatSlider(double val, double min, double max, double step) {
    static char result[200];
    snprintf(result, 200, "%.4f %.4f %.4f %.4f", val, min, max, step);
    return result;
}
int flw::TableEditor::handle(int event) {
    auto ret = 2;
    if (_rows > 0 && _cols > 0) {
        if (event == FL_UNFOCUS) {
            if (_edit && Fl::focus() && Fl::focus()->parent()) {
                if ((_edit2 == nullptr && Fl::focus() != _edit) || (_edit2 && Fl::focus() != _edit2 && Fl::focus() != _edit3)) {
                    _edit_stop();
                }
            }
        }
        else if (event == FL_PASTE) {
            ret = _ev_paste();
        }
        else if (event == FL_KEYDOWN) {
            ret = _ev_keyboard_down();
        }
        else if (event == FL_PUSH) {
            ret = _ev_mouse_click();
        }
    }
    if (ret == 1) {
        return ret;
    }
    else {
        return TableDisplay::handle(event);
    }
}
#include <FL/Fl_Toggle_Button.H>
#include <FL/fl_draw.H>
namespace flw {
    class _TabsGroupButton : public Fl_Toggle_Button {
    public:
        static int                      _BORDER;
        static Fl_Boxtype               _BOXTYPE;
        static Fl_Color                 _BOXSELCOLOR;
        static Fl_Color                 _BOXCOLOR;
        int _tw;
        _TabsGroupButton(const char* label) : Fl_Toggle_Button(0, 0, 0, 0) {
            _tw = 0;
            copy_label(label);
            copy_tooltip(label);
        }
        void draw() override {
            fl_draw_box(_BOXTYPE, x(), y(), w(), h(), value() ? _TabsGroupButton::_BOXSELCOLOR : _TabsGroupButton::_BOXCOLOR);
            fl_font(flw::PREF_FONT, flw::PREF_FONTSIZE);
            fl_color(FL_FOREGROUND_COLOR);
            fl_draw(label(), x() + 3, y(), w() - 6, h(), FL_ALIGN_LEFT | FL_ALIGN_CLIP);
        }
    };
    int        _TabsGroupButton::_BORDER      = 6;
    Fl_Boxtype _TabsGroupButton::_BOXTYPE     = FL_FLAT_BOX;
    Fl_Color   _TabsGroupButton::_BOXSELCOLOR = FL_SELECTION_COLOR;
    Fl_Color   _TabsGroupButton::_BOXCOLOR    = FL_DARK1;
}
flw::TabsGroup::TabsGroup(int X, int Y, int W, int H, const char* l) : Fl_Group(X, Y, W, H, l) {
    end();
    clip_children(1);
    _active = -1;
    _drag   = false;
    _hide   = false;
    _pos    = flw::PREF_FONTSIZE * 10;
    _tabs   = TABS::NORTH;
}
void flw::TabsGroup::add(const std::string& label, Fl_Widget* widget) {
    auto button = new _TabsGroupButton(label.c_str());
    button->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
    button->box(FL_THIN_UP_BOX);
    button->callback(flw::TabsGroup::Callback, this);
    button->when(FL_WHEN_CHANGED);
    Fl_Group::add(widget);
    Fl_Group::add(button);
    if (_active + 1 >= (int) _widgets.size()) {
        _widgets.push_back(widget);
        _buttons.push_back(button);
    }
    else {
        _widgets.insert(_widgets.begin() + _active + 1, widget);
        _buttons.insert(_buttons.begin() + _active + 1, button);
    }
    flw::TabsGroup::Callback(button, this);
}
void flw::TabsGroup::BoxColor(Fl_Color boxcolor) {
    flw::_TabsGroupButton::_BOXCOLOR = boxcolor;
}
void flw::TabsGroup::BoxSelectionColor(Fl_Color boxcolor) {
    flw::_TabsGroupButton::_BOXSELCOLOR = boxcolor;
}
void flw::TabsGroup::BoxType(Fl_Boxtype boxtype) {
    flw::_TabsGroupButton::_BOXTYPE = boxtype;
}
Fl_Widget* flw::TabsGroup::_button() {
    return (_active >= 0 && _active < (int) _buttons.size()) ? _buttons[_active] : nullptr;
}
void flw::TabsGroup::Callback(Fl_Widget* sender, void* object) {
    if (sender) {
        auto button = (_TabsGroupButton*) sender;
        auto self   = (TabsGroup*) object;
        auto count  = 0;
        self->_active = -1;
        for (auto b : self->_buttons) {
            if (b == button) {
                ((Fl_Button*) b)->value(1);
                self->_active = count;
                self->_widgets[count]->show();
            }
            else {
                ((Fl_Button*) b)->value(0);
                self->_widgets[count]->hide();
            }
            count++;
        }
        if (self->value() != nullptr && Fl::focus() != self->value()) {
            self->value()->take_focus();
        }
        self->resize();
    }
}
Fl_Widget* flw::TabsGroup::child(int num) const {
    return (num >= 0 && num < (int) _widgets.size()) ? _widgets[num] : nullptr;
}
int flw::TabsGroup::find(Fl_Widget* widget) const{
    auto num = 0;
    for (auto w : _widgets) {
        if (w == widget) {
            return num;
        }
        else {
            num++;
        }
    }
    return -1;
}
int flw::TabsGroup::handle(int event) {
    if (_widgets.size() == 0) {
        return Fl_Group::handle(event);
    }
    if (_tabs == TABS::WEST || _tabs == TABS::EAST) {
        if (event == FL_DRAG) {
            if (_drag == true) {
                auto pos = 0;
                if (_tabs == TABS::WEST) {
                    pos = Fl::event_x() - x();
                }
                else {
                    pos = x() + w() - Fl::event_x();
                }
                if (pos != _pos) {
                    _pos = pos;
                    resize();
                }
                return 1;
            }
        }
        else if (event == FL_LEAVE) {
            _drag = false;
            fl_cursor(FL_CURSOR_DEFAULT);
        }
        else if (event == FL_MOVE) {
            auto event_x = Fl::event_x();
            if (_tabs == TABS::WEST) {
                auto pos = x() + _pos;
                if (event_x > (pos - 3) && event_x <= (pos + 3)) {
                    if (_drag == false) {
                        _drag = true;
                        fl_cursor(FL_CURSOR_WE);
                    }
                    return 1;
                }
            }
            else {
                auto pos = x() + w() - _pos;
                if (event_x > (pos - 3) && event_x <= (pos + 3)) {
                    if (_drag == false) {
                        _drag = true;
                        fl_cursor(FL_CURSOR_WE);
                    }
                    return 1;
                }
            }
            if (_drag == true) {
                _drag = false;
                fl_cursor(FL_CURSOR_DEFAULT);
            }
        }
        else if (event == FL_PUSH) {
            if (_drag == true) {
                return 1;
            }
        }
    }
    if (_buttons.size() > 1) {
        if (event == FL_KEYBOARD) {
            auto key   = Fl::event_key();
            auto alt   = Fl::event_alt() != 0;
            auto shift = Fl::event_shift() != 0;
            if (alt == true && key >= '0' && key <= '9') {
                auto tab = key - '0';
                tab = (tab == 0) ? 9 : tab - 1;
                if (tab < (int) _buttons.size()) {
                    flw::TabsGroup::Callback(_buttons[tab], this);
                }
                return 1;
            }
            else if (alt == true && shift == true && key == FL_Left) {
                swap(_active, _active - 1);
                flw::TabsGroup::Callback(_button(), this);
                return 1;
            }
            else if (alt == true && shift == true && key == FL_Right) {
                swap(_active, _active + 1);
                flw::TabsGroup::Callback(_button(), this);
                return 1;
            }
            else if (alt == true && key == FL_Left) {
                _active = _active == 0 ? (int) _widgets.size() - 1 : _active - 1;
                flw::TabsGroup::Callback(_button(), this);
                return 1;
            }
            else if (alt == true && key == FL_Right) {
                _active = _active == (int) _widgets.size() - 1 ? 0 : _active + 1;
                flw::TabsGroup::Callback(_button(), this);
                return 1;
            }
        }
    }
    if (event == FL_FOCUS) {
        if (value() != nullptr && Fl::focus() != value()) {
            value()->take_focus();
            return 1;
        }
    }
    return Fl_Group::handle(event);
}
void flw::TabsGroup::_hide_tab_buttons(bool hide) {
    _hide = hide;
    for (auto b : _buttons) {
        if (_hide == true) {
            b->hide();
        }
        else {
            b->show();
        }
    }
    resize();
}
void flw::TabsGroup::label(const std::string& label, Fl_Widget* widget) {
    auto num = find(widget);
    if (num != -1) {
        _buttons[num]->copy_label(label.c_str());
        _buttons[num]->copy_tooltip(label.c_str());
    }
}
Fl_Widget* flw::TabsGroup::remove(int num) {
    if (num >= 0 && num < (int) _widgets.size()) {
        auto w = _widgets[num];
        auto b = _buttons[num];
        _widgets.erase(_widgets.begin() + num);
        _buttons.erase(_buttons.begin() + num);
        remove(w);
        remove(b);
        delete b;
        if (num < _active) {
            _active--;
        }
        else if (_active == (int) _widgets.size()) {
            _active = (int) _widgets.size() - 1;
        }
        flw::TabsGroup::Callback(_button(), this);
        return w;
    }
    else {
        return nullptr;
    }
}
void flw::TabsGroup::resize(int X, int Y, int W, int H) {
    Fl_Widget::resize(X, Y, W, H);
    if (W == 0 || H == 0) {
        return;
    }
    if (visible() == 0 || _widgets.size() == 0) {
        return;
    }
    auto height = flw::PREF_FONTSIZE + 8;
    if (_hide == true) {
        for (auto w : _widgets) {
            if (w->visible() == 0) {
            }
            else {
                w->resize(X, Y, W, H);
            }
        }
        return;
    }
    if (_tabs == TABS::NORTH || _tabs == TABS::SOUTH) {
        auto space = 6;
        auto x     = 0;
        auto w     = 0;
        auto th    = 0;
        fl_font(flw::PREF_FONT, flw::PREF_FONTSIZE);
        for (auto b : _buttons) {
            b->_tw = 0;
            fl_measure(b->label(), b->_tw, th);
            b->_tw += _TabsGroupButton::_BORDER;
            w      += b->_tw + space;
        }
        if (w > W) {
            w = (W - (_buttons.size() * 4)) / _buttons.size();
        }
        else {
            w = 0;
        }
        for (auto b : _buttons) {
            auto bw = (w != 0) ? w : b->_tw;
            if (_tabs == TABS::NORTH) {
                b->resize(X + x, Y, bw, height);
            }
            else {
                b->resize(X + x, Y + H - height, bw, height);
            }
            x += bw;
            x += space;
        }
    }
    else {
        auto y        = Y;
        auto h        = height;
        auto shrinked = false;
        auto space    = 3;
        if ((h + space) * (int) _buttons.size() > H) {
            h = (H - _buttons.size()) / _buttons.size();
            shrinked = true;
        }
        if (_pos < flw::PREF_FONTSIZE * space) {
            _pos = flw::PREF_FONTSIZE * space;
        }
        else if (_pos > W - flw::PREF_FONTSIZE * 8) {
            _pos = W - flw::PREF_FONTSIZE * 8;
        }
        for (auto b : _buttons) {
            if (_tabs == TABS::WEST) {
                b->resize(X, y, _pos, h);
                y += h + (shrinked == false ? space : 1);
            }
            else {
                b->resize(X + W - _pos, y, _pos, h);
                y += h + (shrinked == false ? space : 1);
            }
        }
    }
    for (auto w : _widgets) {
        if (w->visible() == 0) {
        }
        else if (_tabs == TABS::NORTH) {
            w->resize(X, Y + height, W, H - height);
        }
        else if (_tabs == TABS::SOUTH) {
            w->resize(X, Y, W, H - height);
        }
        else if (_tabs == TABS::WEST) {
            w->resize(X + _pos, Y, W - _pos, H);
        }
        else if (_tabs == TABS::EAST) {
            w->resize(X, Y, W - _pos, H);
        }
    }
}
void flw::TabsGroup::swap(int from, int to) {
    auto last = (int) _widgets.size() - 1;
    if (_widgets.size() < 2 || to < -1 || to > (int) _widgets.size()) {
        return;
    }
    else {
        bool active = (_active == from);
        if (from == 0 && to == -1) {
            auto widget = _widgets[0];
            auto button = _buttons[0];
            for (int f = 1; f <= last; f++) {
                _widgets[f - 1] = _widgets[f];
                _buttons[f - 1] = _buttons[f];
            }
            from           = last;
            _widgets[from] = widget;
            _buttons[from] = button;
        }
        else if (from == last && to == (int) _widgets.size()) {
            auto widget = _widgets[last];
            auto button = _buttons[last];
            for (int f = last - 1; f >= 0; f--) {
                _widgets[f + 1] = _widgets[f];
                _buttons[f + 1] = _buttons[f];
            }
            from           = 0;
            _widgets[from] = widget;
            _buttons[from] = button;
        }
        else {
            auto widget = _widgets[from];
            auto button = _buttons[from];
            _widgets[from] = _widgets[to];
            _buttons[from] = _buttons[to];
            _widgets[to]   = widget;
            _buttons[to]   = button;
            from           = to;
        }
        if (active) {
            _active = from;
        }
        resize();
    }
}
Fl_Widget* flw::TabsGroup::value() const {
    return (_active >= 0 && _active < (int) _widgets.size()) ? _widgets[_active] : nullptr;
}
void flw::TabsGroup::value(int num) {
    if (num >= 0 && num < (int) _buttons.size()) {
        flw::TabsGroup::Callback(_buttons[num], this);
    }
}
#include <FL/fl_draw.H>
#include <FL/Fl.H>
flw::WaitCursor* flw::WaitCursor::WAITCURSOR = nullptr;
flw::WaitCursor::WaitCursor() {
    if (WaitCursor::WAITCURSOR == nullptr) {
        WaitCursor::WAITCURSOR = this;
        fl_cursor(FL_CURSOR_WAIT);
        Fl::redraw();
        Fl::flush();
    }
}
flw::WaitCursor::~WaitCursor() {
    if (WaitCursor::WAITCURSOR == this) {
        WaitCursor::WAITCURSOR = nullptr;
        fl_cursor(FL_CURSOR_DEFAULT);
    }
}

