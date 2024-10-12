// Copyright gnuwimp@gmail.com
// Released under the GNU General Public License v3.0
#include "flw.h"
#include <FL/Fl_File_Chooser.H>
#include <FL/Fl_Hor_Slider.H>
#include <FL/Fl_Menu_Button.H>
#include <FL/Fl_Scrollbar.H>
#include <FL/fl_ask.H>
#include <FL/fl_draw.H>
#include <FL/fl_show_colormap.H>
#include <algorithm>
namespace flw {
#define _FLW_CHART_ERROR(X)     { fl_alert("error: illegal chart value at pos %u", (X)->pos()); clear(); return false; }
#define _FLW_CHART_CB(X)        [](Fl_Widget*, void* o) { static_cast<Chart*>(o)->X; }, this
#define _FLW_CHART_DEBUG(X)
#define _FLW_CHART_CLIP(X) { X; }
static const char* const _CHART_ADD_CSV         = "Add line from CSV file...";
static const char* const _CHART_ADD_LINE        = "Create line...";
static const char* const _CHART_CLEAR           = "Clear chart";
static const char* const _CHART_DEBUG           = "Debug chart";
static const char* const _CHART_DEBUG_LINE      = "Print visible values";
static const char* const _CHART_LOAD_JSON       = "Load chart from JSON...";
static const char* const _CHART_PRINT           = "Print to PostScript file...";
static const char* const _CHART_SAVE_CSV        = "Save line to CSV...";
static const char* const _CHART_SAVE_JSON       = "Save chart to JSON...";
static const char* const _CHART_SAVE_PNG        = "Save to png file...";
static const char* const _CHART_SETUP_AREA      = "Number of areas...";
static const char* const _CHART_SETUP_DELETE    = "Delete lines...";
static const char* const _CHART_SETUP_LABEL     = "Label...";
static const char* const _CHART_SETUP_LINE      = "Line properties...";
static const char* const _CHART_SETUP_MAX_CLAMP = "Set max clamp...";
static const char* const _CHART_SETUP_MIN_CLAMP = "Set min clamp...";
static const char* const _CHART_SETUP_MOVE      = "Move lines...";
static const char* const _CHART_SETUP_RANGE     = "Date range...";
static const char* const _CHART_SETUP_SHOW      = "Show or hide lines...";
static const char* const _CHART_SHOW_HLINES     = "Show horizontal lines";
static const char* const _CHART_SHOW_LABELS     = "Show line labels";
static const char* const _CHART_SHOW_VLINES     = "Show vertical lines";
static const int         _CHART_MIN_MARGIN      =  3;
static const int         _CHART_MAX_MARGIN      = 20;
static const int         _CHART_MIN_AREA_SIZE   = 10;
static const int         _CHART_TICK_SIZE       =  4;
static const std::string _CHART_LABEL_SYMBOL    = "@-> ";
bool ChartArea::add_line(const ChartLine& chart_line) {
    if (_lines.size() >= ChartArea::MAX_LINES) {
        return false;
    }
    _lines.push_back(chart_line);
    Fl::redraw();
    return true;
}
std::optional<double> ChartArea::clamp_max() const {
    if (std::isfinite(_clamp_max) == true) {
        return _clamp_max;
    }
    return std::nullopt;
}
std::optional<double> ChartArea::clamp_min() const {
    if (std::isfinite(_clamp_min) == true) {
        return _clamp_min;
    }
    return std::nullopt;
}
void ChartArea::clear() {
    _clamp_max = INFINITY;
    _clamp_min = INFINITY;
    _percent   = 0;
    _rect      = Fl_Rect();
    _selected  = 0;
    _left.clear();
    _right.clear();
    _lines.clear();
}
void ChartArea::debug() const {
#ifdef DEBUG
    printf("\t--------------------------------------\n");
    printf("\tChartArea: %d\n", static_cast<int>(_area));
    if (_percent >= _CHART_MIN_AREA_SIZE) {
        printf("\t\tx, y:       %12d, %4d\n", _rect.x(), _rect.y());
        printf("\t\tw, h:       %12d, %4d\n", _rect.w(), _rect.h());
        printf("\t\tclamp_max:  %18.4f\n", _clamp_max);
        printf("\t\tclamp_min:  %18.4f\n", _clamp_min);
        printf("\t\tpercent:    %18d\n", _percent);
        printf("\t\tlines:      %18d\n", static_cast<int>(_lines.size()));
        printf("\t\tselected:   %18d\n", static_cast<int>(_selected));
        _left.debug("left");
        _right.debug("right");
        auto c = 0;
        for (const auto& l : _lines) {
            l.debug(c++);
        }
        fflush(stdout);
    }
#endif
}
void ChartArea::delete_line(size_t index) {
    size_t count = 0;
    for (auto it = _lines.begin(); it != _lines.end(); ++it) {
        if (index == count) {
            _lines.erase(it);
            if (_lines.size() == 0) {
                _selected = 0;
            }
            else if (_selected >= _lines.size()) {
                _selected = _lines.size() - 1;
            }
            break;
        }
        count++;
    }
}
ChartLine* ChartArea::selected_line() {
    if (_selected >= _lines.size()) {
        return nullptr;
    }
    return &_lines[_selected];
}
ChartData::ChartData() {
    high = low = close = 0.0;
}
ChartData::ChartData(std::string DATE, double value) {
    Date valid_date = Date::FromString(DATE.c_str());
    if (std::isfinite(value) == true &&
        fabs(value) < ChartData::MAX_VALUE &&
        valid_date.is_invalid() == false) {
        date = valid_date.format(Date::FORMAT::ISO_TIME);
        high = low = close = value;
    }
    else {
        high = low = close = 0.0;
    }
}
ChartData::ChartData(std::string DATE, double HIGH, double LOW, double CLOSE) {
    Date valid_date = Date::FromString(DATE.c_str());
    if (std::isfinite(HIGH) == true &&
        std::isfinite(LOW) == true &&
        std::isfinite(CLOSE) == true &&
        fabs(HIGH) < ChartData::MAX_VALUE &&
        fabs(LOW) < ChartData::MAX_VALUE &&
        fabs(CLOSE) < ChartData::MAX_VALUE &&
        valid_date.is_invalid() == false) {
        if (LOW > HIGH) {
            auto tmp = LOW;
            LOW = HIGH;
            HIGH = tmp;
        }
        if (CLOSE > HIGH) {
            auto tmp = HIGH;
            HIGH = CLOSE;
            CLOSE = tmp;
        }
        if (CLOSE < LOW) {
            auto tmp = LOW;
            LOW = CLOSE;
            CLOSE = tmp;
        }
        date  = valid_date.format(Date::FORMAT::ISO_TIME);
        high  = HIGH;
        low   = LOW;
        close = CLOSE;
    }
    else {
        high = low = close = 0.0;
    }
}
ChartDataVector ChartData::ATR(const ChartDataVector& in, std::size_t days) {
    ChartDataVector res;
    if (days > 1 && days < in.size()) {
        double tot        = 0.0;
        double prev_close = 0.0;
        double prev_range = 0.0;
        size_t f          = 0;
        days--;
        for (const auto& data : in) {
            if (f == 0) {
                tot += data.high - data.low;
            }
            else {
                double t1 = data.high - data.low;
                double t2 = fabs(data.high - prev_close);
                double t3 = fabs(data.low - prev_close);
                double ra = 0.0;
                if (t1 > t2 && t1 > t3) {
                    ra = t1;
                }
                else if (t2 > t1 && t2 > t3) {
                    ra = t2;
                }
                else {
                    ra = t3;
                }
                tot += ra;
                if (f == days) {
                    prev_range = tot / (days + 1);
                    res.push_back(ChartData(data.date, prev_range));
                }
                else if (f > days) {
                    prev_range = ((prev_range * days) + ra) / (days + 1);
                    res.push_back(ChartData(data.date, prev_range));
                }
            }
            prev_close = data.close;
            f++;
        }
    }
    return res;
}
size_t ChartData::BinarySearch(const ChartDataVector& in, const ChartData& key) {
    auto it = std::lower_bound(in.begin(), in.end(), key);
    if (it == in.end() || *it != key) {
        return (size_t) -1;
    }
    else {
        return std::distance(in.begin(), it);
    }
}
ChartDataVector ChartData::DateSerie(std::string start_date, std::string stop_date, ChartData::RANGE range, const ChartDataVector& block) {
    int             month   = -1;
    Date            current = Date::FromString(start_date.c_str());
    Date            stop    = Date::FromString(stop_date.c_str());
    ChartDataVector res;
    if (range == ChartData::RANGE::FRIDAY) {
        while (current.weekday() != Date::DAY::FRIDAY)
            current.add_days(1);
    }
    else if (range == ChartData::RANGE::SUNDAY) {
        while (current.weekday() != Date::DAY::SUNDAY) {
            current.add_days(1);
        }
    }
    while (current <= stop) {
        Date date(1, 1, 1);
        if (range == ChartData::RANGE::DAY) {
            date = Date(current);
            current.add_days(1);
        }
        else if (range == ChartData::RANGE::WEEKDAY) {
            Date::DAY weekday = current.weekday();
            if (weekday >= Date::DAY::MONDAY && weekday <= Date::DAY::FRIDAY) {
                date = Date(current);
            }
            current.add_days(1);
        }
        else if (range == ChartData::RANGE::FRIDAY || range == ChartData::RANGE::SUNDAY) {
            date = Date(current);
            current.add_days(7);
        }
        else if (range == ChartData::RANGE::MONTH) {
            if (current.month() != month) {
                current.day(current.month_days());
                date = Date(current);
                month = current.month();
            }
            current.add_months(1);
        }
        else if (range == ChartData::RANGE::HOUR) {
            date = Date(current);
            current.add_seconds(3600);
        }
        else if (range == ChartData::RANGE::MIN) {
            date = Date(current);
            current.add_seconds(60);
        }
        else if (range == ChartData::RANGE::SEC) {
            date = Date(current);
            current.add_seconds(1);
        }
        if (date.year() > 1) {
            ChartData price(date.format(Date::FORMAT::ISO_TIME_LONG));
            if (block.size() == 0 || std::binary_search(block.begin(), block.end(), price) == false) {
                res.push_back(price);
            }
        }
    }
    return res;
}
ChartDataVector ChartData::DayToMonth(const ChartDataVector& in, bool sum) {
    size_t          f = 0;
    ChartDataVector res;
    ChartData       current;
    Date            stop;
    Date            pdate;
    for (const auto& data : in) {
        if (f == 0) {
            current = data;
            stop = Date::FromString(current.date.c_str());
            stop.day_last();
        }
        else {
            pdate = Date::FromString(data.date.c_str());
            if (stop < pdate) {
                current.date = stop.format(Date::FORMAT::ISO_TIME);
                res.push_back(current);
                current = data;
                stop = Date::FromString(current.date.c_str());
                stop.day_last();
            }
            else if (sum == true) {
                current.high  += data.high;
                current.low   += data.low;
                current.close += data.close;
            }
            else {
                if (data.high > current.high) {
                    current.high = data.high;
                }
                if (data.low < current.low) {
                    current.low = data.low;
                }
                current.close  = data.close;
            }
        }
        if (f + 1 == in.size()) {
            auto s = stop.format(Date::FORMAT::ISO_TIME);
            stop.day_last();
            current.date = s;
            res.push_back(current);
        }
        f++;
    }
    return res;
}
ChartDataVector ChartData::DayToWeek(const ChartDataVector& in, Date::DAY weekday, bool sum) {
    size_t          f = 0;
    ChartDataVector res;
    ChartData       current;
    Date            stop;
    Date            pdate;
    for (const auto& data : in) {
        if (f == 0) {
            stop = Date::FromString(data.date.c_str());
            if (weekday > stop.weekday()) {
                stop.weekday(weekday);
            }
            else if (weekday < stop.weekday()) {
                stop.weekday(weekday);
                stop.add_days(7);
            }
            current = data;
        }
        else {
            pdate = Date::FromString(data.date.c_str());
            if (stop < pdate) {
                current.date = stop.format(Date::FORMAT::ISO_TIME);
                res.push_back(current);
                current = data;
            }
            else if (sum == true) {
                current.high  += data.high;
                current.low   += data.low;
                current.close += data.close;
            }
            else {
                if (data.high > current.high) {
                    current.high = data.high;
                }
                if (data.low < current.low) {
                    current.low = data.low;
                }
                current.close  = data.close;
            }
            while (stop < pdate) {
                stop.add_days(7);
            }
            current.date = stop.format(Date::FORMAT::ISO_TIME);
        }
        if (f + 1 == in.size()) {
            current.date = stop.format(Date::FORMAT::ISO_TIME);
            res.push_back(current);
        }
        f++;
    }
    return res;
}
void ChartData::debug() const {
#ifdef DEBUG
    printf("%s, %20.8f, %20.8f, %20.8f\n", date.c_str(), high, low, close);
#endif
}
void ChartData::Debug(const ChartDataVector& in) {
#ifdef DEBUG
    printf("\nChartDataVector(%u)\n", static_cast<unsigned>(in.size()));
    for (const auto& data : in) {
        data.debug();
    }
    fflush(stdout);
#else
    (void) in;
#endif
}
ChartDataVector ChartData::ExponentialMovingAverage(const ChartDataVector& in, size_t days) {
    ChartDataVector res;
    if (days > 1 && days < in.size()) {
        double sma   = 0.0;
        double prev  = 0.0;
        double multi = 2.0 / static_cast<double>(days + 1.0);
        size_t f     = 0;
        for (const auto& data : in) {
            if (f < (days - 1)) {
                sma += data.close;
            }
            else if (f == (days - 1)) {
                sma += data.close;
                prev = sma / days;
                res.push_back(ChartData(data.date, prev));
            }
            else {
                prev = ((data.close - prev) * multi) + prev;
                res.push_back(ChartData(data.date, prev));
            }
            f++;
        }
    }
    return res;
}
ChartDataVector ChartData::Fixed(const ChartDataVector& in, double value) {
    ChartDataVector res;
    for (const auto& data : in) {
        res.push_back(ChartData(data.date, value));
    }
    return res;
}
ChartDataVector ChartData::LoadCSV(std::string filename, std::string sep) {
    Buf buf = File::Load(filename);
    if (buf.s < 10) {
        return ChartDataVector();
    }
    std::string     str   = buf.p;
    StringVector    lines = util::split_string(str, "\n");
    ChartDataVector res;
    for (const auto& l : lines) {
        StringVector line = util::split_string(l, sep);
        ChartData    data;
        if (line.size() == 2) {
            data = ChartData(line[0], util::to_double(line[1]));
        }
        else if (line.size() > 3) {
            data = ChartData(line[0], util::to_double(line[1]), util::to_double(line[2]), util::to_double(line[3]));
        }
        if (data.date != "") {
            res.push_back(data);
        }
    }
    return res;
}
ChartDataVector ChartData::Modify(const ChartDataVector& in, ChartData::MODIFY modify, double value) {
    ChartDataVector res;
    if (fabs(value) < ChartData::MIN_VALUE) {
        return res;
    }
    for (const auto& data : in) {
        switch (modify) {
        case MODIFY::SUBTRACTION:
            res.push_back(ChartData(data.date, data.high - value, data.low - value, data.close - value));
            break;
        case MODIFY::MULTIPLICATION:
            res.push_back(ChartData(data.date, data.high * value, data.low * value, data.close * value));
            break;
        case MODIFY::DIVISION:
            res.push_back(ChartData(data.date, data.high / value, data.low / value, data.close / value));
            break;
        default:
            res.push_back(ChartData(data.date, data.high + value, data.low + value, data.close + value));
            break;
        }
    }
    return res;
}
ChartDataVector ChartData::Momentum(const ChartDataVector& in, size_t days) {
    size_t          start = days - 1;
    ChartDataVector res;
    if (days > 1 && days < in.size()) {
        for (auto f = start; f < in.size(); f++) {
            const auto& data1 = in[f];
            const auto& data2 = in[f - start];
            res.push_back(ChartData(data1.date, data1.close - data2.close));
        }
    }
    return res;
}
ChartDataVector ChartData::MovingAverage(const ChartDataVector& in, size_t days) {
    ChartDataVector res;
    if (days > 1 && days < in.size()) {
        size_t count = 0;
        double sum   = 0.0;
        auto   tmp   = new double[in.size() + 1];
        for (const auto& data : in) {
            count++;
            if (count < days) {
                tmp[count - 1] = data.close;
                sum += data.close;
            }
            else if (count == days) {
                tmp[count - 1] = data.close;
                sum += data.close;
                res.push_back(ChartData(data.date, sum / days));
            }
            else {
                tmp[count - 1] = data.close;
                sum -= tmp[count - (days + 1)];
                sum += data.close;
                res.push_back(ChartData(data.date, sum / days));
            }
        }
        delete []tmp;
    }
    return res;
}
std::string ChartData::RangeToString(ChartData::RANGE range) {
    static const char* NAMES[] = { "DAY", "WEEKDAY", "FRIDAY", "SUNDAY", "MONTH", "HOUR", "MIN", "SEC", "", };
    return NAMES[static_cast<unsigned>(range)];
}
ChartDataVector ChartData::RSI(const ChartDataVector& in, size_t days) {
    ChartDataVector res;
    if (days > 1 && days < in.size()) {
        double avg_gain = 0.0;
        double avg_loss = 0.0;
        for (size_t f = 1; f < in.size(); f++) {
            double      diff = 0.0;
            const auto& data = in[f];
            const auto& prev = in[f - 1];
            diff = data.close - prev.close;
            if (f <= days) {
                if (diff > 0) {
                    avg_gain += diff;
                }
                else {
                    avg_loss += fabs(diff);
                }
            }
            if (f == days) {
                avg_gain = avg_gain / days;
                avg_loss = avg_loss / days;
                res.push_back(ChartData(data.date, 100 - (100 / (1 + (avg_gain / avg_loss)))));
            }
            else if (f > days) {
                avg_gain = ((avg_gain * (days - 1)) + ((diff > 0) ? fabs(diff) : 0)) / days;
                avg_loss = ((avg_loss * (days - 1)) + ((diff < 0) ? fabs(diff) : 0)) / days;
                res.push_back(ChartData(data.date, 100 - (100 / (1 + (avg_gain / avg_loss)))));
            }
        }
    }
    return res;
}
bool ChartData::SaveCSV(const ChartDataVector& in, std::string filename, std::string sep) {
    std::string csv;
    csv.reserve(in.size() * 40 + 256);
    for (const auto& data : in) {
        char buffer[256];
        snprintf(buffer, 256, "%s%s%s%s%s%s%s\n", data.date.c_str(), sep.c_str(), JS::FormatNumber(data.high).c_str(), sep.c_str(), JS::FormatNumber(data.low).c_str(), sep.c_str(), JS::FormatNumber(data.close).c_str());
        csv += buffer;
    }
    return File::Save(filename, csv.c_str(), csv.size());
}
ChartDataVector ChartData::StdDev(const ChartDataVector& in, size_t days) {
    ChartDataVector res;
    if (days > 1 && days < in.size()) {
        size_t    count = 0;
        double    sum   = 0.0;
        ChartData data2;
        for (const auto& data : in) {
            count++;
            sum += data.close;
            if (count >= days) {
                double mean = sum / days;
                double dev  = 0.0;
                for (size_t j = count - days; j < count; j++) {
                    data2 = in[j];
                    auto tmp = data2.close - mean;
                    tmp *= tmp;
                    dev  += tmp;
                }
                dev /= days;
                dev = sqrt(dev);
                data2 = in[count - days];
                sum -= data2.close;
                data2 = in[count - 1];
                res.push_back(ChartData(data2.date, dev));
            }
        }
    }
    return res;
}
ChartDataVector ChartData::Stochastics(const ChartDataVector& in, size_t days) {
    double          high = 0.0;
    double          low  = 0.0;
    ChartDataVector res;
    if (days > 1 && days < in.size()) {
        for (size_t f = 0; f < in.size(); f++) {
            const auto& data = in[f];
            if ((f + 1) >= days) {
                high = data.high;
                low  = data.low;
                for (size_t j = (f + 1) - days; j < (f + 1) - 1; j++) {
                    const auto& data2 = in[j];
                    if (data2.high > high) {
                        high = data2.high;
                    }
                    if (data2.low < low) {
                        low = data2.low;
                    }
                }
                double diff1 = data.close - low;
                double diff2 = high - low;
                if (diff2 > ChartData::MIN_VALUE) {
                    double kval  = 100.0 * (diff1 / diff2);
                    res.push_back(ChartData(data.date, kval));
                }
            }
        }
    }
    return res;
}
ChartData::RANGE  ChartData::StringToRange(std::string range) {
    if (range == "WEEKDAY")     return RANGE::WEEKDAY;
    else if (range == "FRIDAY") return RANGE::FRIDAY;
    else if (range == "SUNDAY") return RANGE::SUNDAY;
    else if (range == "MONTH")  return RANGE::MONTH;
    else if (range == "HOUR")   return RANGE::HOUR;
    else if (range == "MIN")    return RANGE::MIN;
    else if (range == "SEC")    return RANGE::SEC;
    else                        return RANGE::DAY;
}
ChartLine::ChartLine(const ChartDataVector& data, std::string label, ChartLine::TYPE type) {
    clear();
    set_data(data);
    set_label(label);
    set_type(type);
}
void ChartLine::clear() {
    _data.clear();
    _align   = FL_ALIGN_LEFT;
    _color   = FL_FOREGROUND_COLOR;
    _label   = "";
    _rect    = Fl_Rect();
    _type    = TYPE::LINE;
    _visible = true;
    _width   = 1;
}
void ChartLine::debug(size_t num) const {
#ifdef DEBUG
    printf("\t\t---------------------------------------------\n");
    printf("\t\tChartLine: %u\n", static_cast<unsigned>(num));
    printf("\t\t\ttype:  %30s\n", type_to_string().c_str());
    printf("\t\t\twidth:      %25u\n", _width);
    printf("\t\t\talign:      %25s\n", (_align == FL_ALIGN_LEFT) ? "LEFT" : "RIGHT");
    printf("\t\t\tlabel: %30s\n", _label.c_str());
    printf("\t\t\trect:          %04d, %04d, %04d, %04d\n", _rect.x(), _rect.y(), _rect.w(), _rect.h());
    printf("\t\t\tvisible:    %25s\n", _visible ? "YES" : "NO");
    printf("\t\t\tprices:     %25d\n", static_cast<int>(size()));
    if (size() > 1) {
        printf("\t\t\tfirst:      %25s\n", _data.front().date.c_str());
        printf("\t\t\tfirst:      %25f\n", _data.front().close);
        printf("\t\t\tlast:       %25s\n", _data.back().date.c_str());
        printf("\t\t\tlast:       %25f\n", _data.back().close);
    }
    fflush(stdout);
#else
    (void) num;
#endif
}
ChartLine&  ChartLine::set_type_from_string(std::string val) {
    if (val == "LINE_DOT")                     _type = TYPE::LINE_DOT;
    else if (val == "BAR")                     _type = TYPE::BAR;
    else if (val == "BAR_CLAMP")               _type = TYPE::BAR_CLAMP;
    else if (val == "BAR_HLC")                 _type = TYPE::BAR_HLC;
    else if (val == "HORIZONTAL")              _type = TYPE::HORIZONTAL;
    else if (val == "EXPAND_VERTICAL")         _type = TYPE::EXPAND_VERTICAL;
    else if (val == "EXPAND_HORIZONTAL_ALL")   _type = TYPE::EXPAND_HORIZONTAL_ALL;
    else if (val == "EXPAND_HORIZONTAL_FIRST") _type = TYPE::EXPAND_HORIZONTAL_FIRST;
    else                                       _type = TYPE::LINE;
    return *this;
}
std::string ChartLine::type_to_string() const {
    static const char* NAMES[] = { "LINE", "LINE_DOT", "BAR", "BAR_CLAMP", "BAR_HLC", "HORIZONTAL", "EXPAND_VERTICAL", "EXPAND_HORIZONTAL_ALL", "EXPAND_HORIZONTAL_FIRST", "", };
    return NAMES[static_cast<unsigned>(_type)];
}
class ChartLineSetup : public Fl_Double_Window {
public:
    ChartLine&                  _line;
    Fl_Button*                  _cancel;
    Fl_Button*                  _close;
    Fl_Button*                  _color;
    Fl_Choice*                  _align;
    Fl_Choice*                  _type;
    Fl_Hor_Slider*              _width;
    Fl_Input*                   _label;
    GridGroup*                  _grid;
    bool                        _ret;
public:
    ChartLineSetup(Fl_Window* parent, ChartLine& line) :
    Fl_Double_Window(0, 0, 10, 10, "Line Properties"),
    _line(line) {
        end();
        _line   = line;
        _ret    = false;
        _align  = new Fl_Choice(0, 0, 0, 0, "Align");
        _cancel = new Fl_Button(0, 0, 0, 0, "&Cancel");
        _close  = new Fl_Return_Button(0, 0, 0, 0, "&Ok");
        _color  = new Fl_Button(0, 0, 0, 0, "Color");
        _grid   = new GridGroup(0, 0, w(), h());
        _label  = new Fl_Input(0, 0, 0, 0, "Label");
        _type   = new Fl_Choice(0, 0, 0, 0, "Type");
        _width  = new Fl_Hor_Slider(0, 0, 0, 0);
        _grid->add(_label,     12,   1,  -1,  4);
        _grid->add(_type,      12,   6,  -1,  4);
        _grid->add(_align,     12,  11,  -1,  4);
        _grid->add(_color,     12,  16,  -1,  4);
        _grid->add(_width,     12,  21,  -1,  4);
        _grid->add(_cancel,   -34,  -5,  16,  4);
        _grid->add(_close,    -17,  -5,  16,  4);
        add(_grid);
        _align->add("Left");
        _align->add("Right");
        _align->textfont(flw::PREF_FONT);
        _align->textsize(flw::PREF_FONTSIZE);
        _cancel->callback(ChartLineSetup::Callback, this);
        _close->callback(ChartLineSetup::Callback, this);
        _color->align(FL_ALIGN_LEFT);
        _color->callback(ChartLineSetup::Callback, this);
        _label->textfont(flw::PREF_FONT);
        _label->textsize(flw::PREF_FONTSIZE);
        _type->add("Line");
        _type->add("Dotted Line");
        _type->add("Bar");
        _type->add("Bar clamp");
        _type->add("Bar hlc");
        _type->add("Horizontal");
        _type->add("Expand all horizontal points");
        _type->add("Expand first horizontal point");
        _type->textfont(flw::PREF_FONT);
        _type->textsize(flw::PREF_FONTSIZE);
        _width->align(FL_ALIGN_LEFT);
        _width->callback(ChartLineSetup::Callback, this);
        _width->precision(0);
        _width->range(1, ChartLine::MAX_WIDTH);
        _width->value(_line.width());
        _width->tooltip("Max line width.");
        resizable(_grid);
        util::labelfont(this);
        callback(ChartLineSetup::Callback, this);
        size(30 * flw::PREF_FONTSIZE, 16 * flw::PREF_FONTSIZE);
        size_range(30 * flw::PREF_FONTSIZE, 16 * flw::PREF_FONTSIZE);
        set_modal();
        util::center_window(this, parent);
        _grid->do_layout();
        ChartLineSetup::Callback(_width, this);
        update_widgets();
    }
    static void Callback(Fl_Widget* w, void* o) {
        auto self = static_cast<ChartLineSetup*>(o);
        if (w == self || w == self->_cancel) {
            self->hide();
        }
        else if (w == self->_color) {
            self->_color->color(fl_show_colormap(self->_line.color()));
        }
        else if (w == self->_width) {
            auto l = util::format("Width %u", static_cast<unsigned>(static_cast<Fl_Hor_Slider*>(w)->value()));
            w->copy_label(l.c_str());
            self->redraw();
        }
        else if (w == self->_close) {
            self->_ret = true;
            self->update_line();
            self->hide();
        }
    }
    bool run() {
        show();
        while (visible() != 0) {
            Fl::wait();
            Fl::flush();
        }
        return _ret;
    }
    void update_line() {
        _line.set_label(_label->value());
        _line.set_width(_width->value());
        _line.set_color(_color->color());
        if (_type->value() == 0)      _line.set_type(ChartLine::TYPE::LINE);
        else if (_type->value() == 1) _line.set_type(ChartLine::TYPE::LINE_DOT);
        else if (_type->value() == 2) _line.set_type(ChartLine::TYPE::BAR);
        else if (_type->value() == 3) _line.set_type(ChartLine::TYPE::BAR_CLAMP);
        else if (_type->value() == 4) _line.set_type(ChartLine::TYPE::BAR_HLC);
        else if (_type->value() == 5) _line.set_type(ChartLine::TYPE::HORIZONTAL);
        else if (_type->value() == 6) _line.set_type(ChartLine::TYPE::EXPAND_HORIZONTAL_ALL);
        else if (_type->value() == 7) _line.set_type(ChartLine::TYPE::EXPAND_HORIZONTAL_FIRST);
        if (_align->value() == 0) _line.set_align(FL_ALIGN_LEFT);
        else                      _line.set_align(FL_ALIGN_RIGHT);
    }
    void update_widgets() {
        _label->value(_line.label().c_str());
        _color->color(_line.color());
        ChartLineSetup::Callback(_width, this);
        if (_line.type() == ChartLine::TYPE::LINE)                         _type->value(0);
        else if (_line.type() == ChartLine::TYPE::LINE_DOT)                _type->value(1);
        else if (_line.type() == ChartLine::TYPE::BAR)                     _type->value(2);
        else if (_line.type() == ChartLine::TYPE::BAR_CLAMP)               _type->value(3);
        else if (_line.type() == ChartLine::TYPE::BAR_HLC)                 _type->value(4);
        else if (_line.type() == ChartLine::TYPE::HORIZONTAL)              _type->value(5);
        else if (_line.type() == ChartLine::TYPE::EXPAND_HORIZONTAL_ALL)   _type->value(6);
        else if (_line.type() == ChartLine::TYPE::EXPAND_HORIZONTAL_FIRST) _type->value(7);
        if (_line.align() == FL_ALIGN_LEFT) _align->value(0);
        else                                _align->value(1);
    }
};
ChartScale::ChartScale() {
    clear();
}
int ChartScale::calc_margin() {
    if (diff() < ChartData::MIN_VALUE) {
        return 0;
    }
    const double fr  = util::count_decimals(tick());
    std::string  min = util::format_double(_min, fr, '\'');
    std::string  max = util::format_double(_max, fr, '\'');
    return (min.length() > max.length()) ? min.length() : max.length();
}
void ChartScale::calc_tick(int height) {
    const double RANGE  = diff();
    int          kludge = 0;
    _tick  = 0.0;
    _pixel = 0.0;
    if (std::isfinite(_min) == true && std::isfinite(_max) == true && _min < _max && RANGE > ChartData::MIN_VALUE) {
        double test_inc = 0.0;
        double test_min = 0.0;
        double test_max = 0.0;
        int    ticker   = 0;
        test_inc = pow(10.0, ceil(log10(RANGE / 10.0)));
        test_max = static_cast<int64_t>(_max / test_inc) * test_inc;
        if (test_max < _max) {
            test_max += test_inc;
        }
        test_min = test_max;
        do {
            ++ticker;
            test_min -= test_inc;
        } while (test_min > _min && kludge++ < 100);
        if (ticker < 10) {
            test_inc = (ticker < 5) ? test_inc / 10.0 : test_inc / 2.0;
            while ((test_min + test_inc) <= _min && kludge++ < 100) {
                test_min += test_inc;
            }
            while ((test_max - test_inc) >= _max && kludge++ < 100) {
                test_max -= test_inc;
            }
        }
        _min  = test_min;
        _max  = test_max;
        _tick = test_inc;
        if (_tick >= ChartData::MIN_VALUE / 10.0 && kludge < 100) {
            _pixel = height / diff();
        }
        else {
            _tick = 0.0;
        }
    }
}
void ChartScale::clear() {
    _min   = INFINITY;
    _max   = INFINITY;
    _tick  = 0.0;
    _pixel = 0.0;
}
void ChartScale::debug(const char* name) const {
#ifdef DEBUG
    printf("\t\t---------------------------------------------\n");
    printf("\t\tChartScale: %s\n", name);
    if (std::isfinite(_min) == true) {
        printf("\t\t\tmin:   %30.8f\n", _min);
        printf("\t\t\tmax:   %30.8f\n", _max);
        printf("\t\t\tDiff:  %30.8f\n", diff());
        printf("\t\t\ttick:  %30.8f\n", _tick);
        printf("\t\t\tpixel: %30.8f\n", _pixel);
    }
    fflush(stdout);
#else
    (void) name;
#endif
}
double ChartScale::diff() const {
    if (std::isfinite(_min) == false || std::isfinite(_max) == false) {
        return 0.0;
    }
    return _max - _min;
}
void ChartScale::fix_height() {
    if (std::isfinite(_min) == true && std::isfinite(_max) == true && _min < _max && fabs(_max - _min) < ChartData::MIN_VALUE) {
        if (_min >= 0.0) {
            _min *= 0.9;
        }
        else {
            _min *= 1.1;
        }
        if (_max >= 0.0) {
            _max *= 1.1;
        }
        else {
            _max *= 0.9;
        }
    }
}
std::optional<double> ChartScale::max() const {
    if (std::isfinite(_max) == true) {
        return _max;
    }
    return std::nullopt;
}
std::optional<double> ChartScale::min() const {
    if (std::isfinite(_min) == true) {
        return _min;
    }
    return std::nullopt;
}
Chart::Chart(int X, int Y, int W, int H, const char* l) :
Fl_Group(X, Y, W, H, l),
_CH(14),
_CW(14) {
    end();
    clip_children(1);
    color(FL_BACKGROUND2_COLOR);
    labelcolor(FL_FOREGROUND_COLOR);
    box(FL_BORDER_BOX);
    tooltip(
        "Press ctrl + scroll wheel to change tick width.\n"
        "Press ctrl + left button to show value for selected line.\n"
        "Press left button to show Y value.\n"
        "Press middle button on label to select that line.\n"
        "Press right button for menu.\n"
    );
    _menu   = new Fl_Menu_Button(0, 0, 0, 0);
    _scroll = new Fl_Scrollbar(0, 0, 0, 0);
    add(_menu);
    add(_scroll);
    _scroll->type(FL_HORIZONTAL);
    _scroll->callback(Chart::_CallbackScrollbar, this);
    _menu->add(_CHART_SHOW_LABELS,      0, _FLW_CHART_CB(setup_view_options()), FL_MENU_TOGGLE);
    _menu->add(_CHART_SHOW_HLINES,      0, _FLW_CHART_CB(setup_view_options()), FL_MENU_TOGGLE);
    _menu->add(_CHART_SHOW_VLINES,      0, _FLW_CHART_CB(setup_view_options()), FL_MENU_TOGGLE | FL_MENU_DIVIDER);
    _menu->add(_CHART_CLEAR,            0, _FLW_CHART_CB(clear()));
    _menu->add(_CHART_SETUP_LABEL,      0, _FLW_CHART_CB(setup_label()));
    _menu->add(_CHART_SETUP_AREA,       0, _FLW_CHART_CB(setup_area()));
    _menu->add(_CHART_SETUP_RANGE,      0, _FLW_CHART_CB(setup_date_range()), FL_MENU_DIVIDER);
    _menu->add(_CHART_SETUP_LINE,       0, _FLW_CHART_CB(setup_line()));
    _menu->add(_CHART_SETUP_MIN_CLAMP,  0, _FLW_CHART_CB(setup_clamp(true)));
    _menu->add(_CHART_SETUP_MAX_CLAMP,  0, _FLW_CHART_CB(setup_clamp(false)));
    _menu->add(_CHART_SETUP_SHOW,       0, _FLW_CHART_CB(setup_show_or_hide_lines()));
    _menu->add(_CHART_SETUP_MOVE,       0, _FLW_CHART_CB(setup_move_lines()), FL_MENU_DIVIDER);
    _menu->add(_CHART_SETUP_DELETE,     0, _FLW_CHART_CB(setup_delete_lines()));
    _menu->add(_CHART_ADD_LINE,         0, _FLW_CHART_CB(setup_create_line()));
    _menu->add(_CHART_SAVE_CSV,         0, _FLW_CHART_CB(save_csv()), FL_MENU_DIVIDER);
    _menu->add(_CHART_ADD_CSV,          0, _FLW_CHART_CB(load_csv()));
    _menu->add(_CHART_LOAD_JSON,        0, _FLW_CHART_CB(load_json()));
    _menu->add(_CHART_SAVE_JSON,        0, _FLW_CHART_CB(save_json()), FL_MENU_DIVIDER);
    _menu->add(_CHART_PRINT,            0, _FLW_CHART_CB(print_to_postscript()));
    _menu->add(_CHART_SAVE_PNG,         0, _FLW_CHART_CB(save_png()));
#ifdef DEBUG
    _menu->add(_CHART_SAVE_PNG,         0, _FLW_CHART_CB(save_png()), FL_MENU_DIVIDER);
    _menu->add(_CHART_DEBUG,            0, _FLW_CHART_CB(debug()));
    _menu->add(_CHART_DEBUG_LINE,       0, _FLW_CHART_CB(debug_line()));
#else
    _menu->add(_CHART_SAVE_PNG,         0, _FLW_CHART_CB(save_png()));
#endif
    _menu->type(Fl_Menu_Button::POPUP3);
    _areas.push_back(ChartArea(ChartArea::AREA::ONE));
    _areas.push_back(ChartArea(ChartArea::AREA::TWO));
    _areas.push_back(ChartArea(ChartArea::AREA::THREE));
    _areas.push_back(ChartArea(ChartArea::AREA::FOUR));
    _areas.push_back(ChartArea(ChartArea::AREA::FIVE));
    _disable_menu = false;
    clear();
    update_pref();
}
void Chart::_calc_area_height() {
    auto last   = 0;
    auto addh   = 0;
    auto height = 0;
    _top_space    = (_label == "") ? _CH : _CH * 3;
    _bottom_space = _CH * 3 + Fl::scrollbar_size();
    height        = h() - (_bottom_space + _top_space);
    for (size_t f = 1; f <= static_cast<size_t>(ChartArea::AREA::LAST); f++) {
        const auto& area = _areas[f];
        if (area.percent() >= _CHART_MIN_AREA_SIZE) {
            height -= PREF_FIXED_FONTSIZE;
        }
    }
    _areas[0].rect().y(_top_space);
    _areas[0].rect().h(static_cast<int>((_areas[0].percent() / 100.0) * height));
    for (size_t f = 1; f <= static_cast<size_t>(ChartArea::AREA::LAST); f++) {
        auto& prev = _areas[f - 1];
        auto& area = _areas[f];
        if (area.percent() >= _CHART_MIN_AREA_SIZE) {
            area.rect().y(prev.rect().y() + prev.rect().h() + _CH);
            area.rect().h(static_cast<int>((_areas[f].percent() / 100.0) * height));
            addh += static_cast<int>(prev.rect().h());
            last  = f;
        }
    }
    if (last > 0) {
        _areas[last].rect().h(static_cast<int>(height - addh));
    }
}
void Chart::_calc_area_width() {
    const double width = w() - (_margin_left * _CH + _margin_right * _CH);
    _ticks = static_cast<int>(width / _tick_width);
    if (static_cast<int>(_dates.size()) > _ticks) {
        const double slider_size = _ticks / (_dates.size() + 1.0);
        _scroll->activate();
        _scroll->slider_size((slider_size > 0.05) ? slider_size : 0.05);
        _scroll->range(0, _dates.size() - _ticks);
        if (_scroll->value() > _scroll->maximum()) {
            static_cast<Fl_Slider*>(_scroll)->value(_scroll->maximum());
            _date_start = _dates.size() - _ticks - 1;
        }
    }
    else {
        static_cast<Fl_Valuator*>(_scroll)->value(0);
        _scroll->range(0, 0);
        _scroll->slider_size(1.0);
        _scroll->deactivate();
        _date_start = 0;
    }
    for (auto& area : _areas) {
        area.rect().x(_margin_left * _CH);
        area.rect().w(width);
    }
}
void Chart::_calc_dates() {
    std::string min;
    std::string max;
    for (auto& area : _areas) {
        for (auto& line : area.lines()) {
            if (line.size() > 0 && line.is_visible() == true) {
                const auto& first = line.data().front();
                const auto& last  = line.data().back();
                if (min == "") {
                    min = "99991231 232359";
                    max = "01010101 000000";
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
        _dates = ChartData::DateSerie(min, max, _date_range, _block_dates);
        redraw();
    }
}
void Chart::_calc_margins() {
    auto left  = 0;
    auto right = 0;
    for (auto& area : _areas) {
        if (area.size() == 0) {
            continue;
        }
        const int l = area.left_scale().calc_margin() + 1;
        const int r = area.right_scale().calc_margin() + 1;
        if (l > left) {
            left = l;
        }
        if (r > right) {
            right = r;
        }
    }
    const double FAC     = static_cast<double>(_CW) / static_cast<double>(_CH);
    bool         changed = false;
    left  = round(left * FAC + 0.5);
    right = round(right * FAC + 0.5);
    if (left + 1 >= _margin_left) {
        _margin_left = left + 1;
        changed = true;
    }
    if (right + 1 > _margin_right) {
        _margin_right = right + 1;
        changed = true;
    }
    if (changed == true) {
        if (_date_range == ChartData::RANGE::HOUR) {
            _margin_right += 1;
            _margin_left  += 1;
        }
        else if (_date_range == ChartData::RANGE::MIN) {
            _margin_right += 2;
            _margin_left  += 2;
        }
        else if (_date_range == ChartData::RANGE::SEC) {
            _margin_right += 3;
            _margin_left  += 3;
        }
    }
}
void Chart::_calc_ymin_ymax() {
    for (auto& area : _areas) {
        auto min_clamp = area.clamp_min();
        auto max_clamp = area.clamp_max();
        area.left_scale().clear();
        area.right_scale().clear();
        for (const auto& line : area.lines()) {
            if (line.size() == 0 || line.is_visible() == false) {
                continue;
            }
            const int stop    = _date_start + _ticks;
            int       current = _date_start;
            double    minimum = INFINITY;
            double    maximum = INFINITY;
            double    max     = 0.0;
            double    min     = 0.0;
            if (min_clamp.has_value() == true) {
                minimum = min_clamp.value();
            }
            if (max_clamp.has_value() == true) {
                maximum = max_clamp.value();
            }
            while (current <= stop && current < static_cast<int>(_dates.size())) {
                const ChartData& date  = _dates[current];
                const size_t     index = ChartData::BinarySearch(line.data(), date);
                if (index != static_cast<size_t>(-1)) {
                    const ChartData& data = line.data()[index];
                    if (line.type_has_hlc() == true) {
                        min = data.low;
                        max = data.high;
                    }
                    else {
                        min = data.close;
                        max = data.close;
                    }
                    if (std::isfinite(minimum) == true) {
                        min = minimum;
                    }
                    if (std::isfinite(maximum) == true) {
                        max = maximum;
                    }
                    if (line.align() == FL_ALIGN_LEFT) {
                        auto scale_min = area.left_scale().min();
                        auto scale_max = area.left_scale().max();
                        if (scale_min.has_value() == false || min < scale_min.value()) {
                            area.left_scale().set_min(min);
                        }
                        if (scale_max.has_value() == false || max > scale_max.value()) {
                            area.left_scale().set_max(max);
                        }
                    }
                    else {
                        auto scale_min = area.right_scale().min();
                        auto scale_max = area.right_scale().max();
                        if (scale_min.has_value() == false || min < scale_min.value()) {
                            area.right_scale().set_min(min);
                        }
                        if (scale_max.has_value() == false || max > scale_max.value()) {
                            area.right_scale().set_max(max);
                        }
                    }
                }
                current++;
            }
        }
        area.left_scale().fix_height();
        area.right_scale().fix_height();
    }
}
void Chart::_calc_yscale() {
    for (auto& area : _areas) {
        area.left_scale().calc_tick(area.rect().h());
        area.right_scale().calc_tick(area.rect().h());
    }
}
bool Chart::_CallbackPrinter(void* data, int pw, int ph, int) {
    auto widget = static_cast<Fl_Widget*>(data);
    auto rect   = Fl_Rect(widget);
    widget->resize(0, 0, pw, ph);
    widget->draw();
    widget->resize(rect.x(), rect.y(), rect.w(), rect.h());
    return false;
}
void Chart::_CallbackScrollbar(Fl_Widget*, void* widget) {
    auto self = static_cast<Chart*>(widget);
    self->_date_start = self->_scroll->value();
    self->init();
}
void Chart::clear() {
    static_cast<Fl_Valuator*>(_scroll)->value(0);
    _block_dates.clear();
    _dates.clear();
    for (auto& area : _areas) {
        area.clear();
    }
    *const_cast<int*>(&_CW) = flw::PREF_FIXED_FONTSIZE;
    *const_cast<int*>(&_CH) = flw::PREF_FIXED_FONTSIZE;
    _area         = nullptr;
    _bottom_space = 0;
    _date_start   = 0;
    _margin_left  = _CHART_MIN_MARGIN;
    _margin_right = _CHART_MIN_MARGIN;
    _old          = Fl_Rect();
    _printing     = false;
    _ticks        = 0;
    _tooltip      = "";
    _top_space    = 0;
    _ver_pos[0]   = -1;
    set_alt_size();
    set_area_size();
    set_date_range();
    set_hor_lines();
    set_main_label();
    set_line_labels();
    set_tick_width();
    set_ver_lines();
    init();
}
bool Chart::create_line(ChartData::FORMULAS formula, bool support) {
    if (_area == nullptr || _area->selected_line() == nullptr) {
        fl_alert("Area or line has not been selected!");
        return false;
    }
    else if (_area->size() >= ChartArea::MAX_LINES) {
        fl_alert("Max line count reached!");
        return false;
    }
    auto line0  = _area->selected_line();
    auto line1  = ChartLine();
    auto line2  = ChartLine();
    auto line3  = ChartLine();
    auto vec1   = ChartDataVector();
    auto vec2   = ChartDataVector();
    auto vec3   = ChartDataVector();
    auto label1 = std::string();
    auto label2 = std::string();
    auto label3 = std::string();
    auto type1  = ChartLine::TYPE::LINE;
    auto type2  = ChartLine::TYPE::LINE;
    auto type3  = ChartLine::TYPE::LINE;
    if (formula == ChartData::FORMULAS::ATR) {
        auto days = util::to_long(fl_input_str(3, "Enter number of days (2 - 365)", "14"));
        if (days < 2 || days > 365) {
            return false;
        }
        vec1   = ChartData::ATR(line0->data(), days);
        label1 = util::format("ATR %d Days", days);
    }
    else if (formula == ChartData::FORMULAS::DAY_TO_WEEK) {
        auto answer = fl_choice("Would you like to use highest/lowest and last close value per week?\nOr sum values per week?", nullptr, "High/Low", "Sum");
        vec1   = ChartData::DayToWeek(line0->data(), Date::DAY::SUNDAY, answer == 2);
        label1 = "Weekly (Sunday)";
        type1  = line0->type();
    }
    else if (formula == ChartData::FORMULAS::DAY_TO_MONTH) {
        auto answer = fl_choice("Would you like to use highest/lowest and last close value per month?\nOr sum values per month?", nullptr, "High/Low", "Sum");
        vec1   = ChartData::DayToMonth(line0->data(), answer == 2);
        label1 = "Monthly";
        type1  = line0->type();
    }
    else if (formula == ChartData::FORMULAS::EXP_MOVING_AVERAGE) {
        auto days = util::to_long(fl_input_str(3, "Enter number of days (2 - 365)", "14"));
        if (days < 2 || days > 365) {
            return false;
        }
        vec1   = ChartData::ExponentialMovingAverage(line0->data(), days);
        label1 = util::format("Exponential Moving Average %d Days", days);
    }
    else if (formula == ChartData::FORMULAS::FIXED) {
        auto value = util::to_double(fl_input_str(16, "Enter value", "0"));
        if (std::isinf(value) == true) {
            return false;
        }
        vec1   = ChartData::Fixed(line0->data(), value);
        label1 = util::format("Horizontal %f", value);
        type1  = ChartLine::TYPE::EXPAND_HORIZONTAL_FIRST;
    }
    else if (formula == ChartData::FORMULAS::MODIFY) {
        auto list = StringVector() = {"Addition", "Subtraction", "Multiplication", "Division"};
        auto ans  = dlg::choice("Select Modification", list, 0, top_window());
        if (ans < 0 || ans > static_cast<int>(ChartData::MODIFY::LAST)) {
            return false;
        }
        auto modify = static_cast<ChartData::MODIFY>(ans);
        auto value  = util::to_double(fl_input_str(16, "Enter value", "0"));
        if (std::isinf(value) == true) {
            return false;
        }
        else if (fabs(value) < ChartData::MIN_VALUE) {
            fl_alert("To small value for division!");
            return false;
        }
        vec1   = ChartData::Modify(line0->data(), modify, value);
        label1 = util::format("Modified %s", line0->label().c_str());
        type1  = line0->type();
    }
    else if (formula == ChartData::FORMULAS::MOMENTUM) {
        auto days = util::to_long(fl_input_str(3, "Enter number of days (2 - 365)", "14"));
        if (days < 2 || days > 365) {
            return false;
        }
        vec1   = ChartData::Momentum(line0->data(), days);
        label1 = util::format("Momentum %d Days", days);
        if (support == true) {
            vec2   = ChartData::Fixed(line0->data(), 0.0);
            label2 = "Momentum Zero";
            type2  = ChartLine::TYPE::EXPAND_HORIZONTAL_FIRST;
        }
    }
    else if (formula == ChartData::FORMULAS::MOVING_AVERAGE) {
        auto days = util::to_long(fl_input_str(3, "Enter number of days (2 - 365)", "14"));
        if (days < 2 || days > 365) {
            return false;
        }
        vec1   = ChartData::MovingAverage(line0->data(), days);
        label1 = util::format("Moving Average %d Days", days);
    }
    else if (formula == ChartData::FORMULAS::RSI) {
        auto days = util::to_long(fl_input_str(3, "Enter number of days (2 - 365)", "14"));
        if (days < 2 || days > 365) {
            return false;
        }
        vec1   = ChartData::RSI(line0->data(), days);
        label1 = util::format("RSI %d Days", days);
        if (support == true) {
            vec2   = ChartData::Fixed(line0->data(), 30.0);
            label2 = "RSI 30";
            type2  = ChartLine::TYPE::EXPAND_HORIZONTAL_FIRST;
            vec3   = ChartData::Fixed(line0->data(), 70.0);
            label3 = "RSI 70";
            type3  = ChartLine::TYPE::EXPAND_HORIZONTAL_FIRST;
        }
    }
    else if (formula == ChartData::FORMULAS::STD_DEV) {
        auto days = util::to_long(fl_input_str(3, "Enter number of days (2 - 365)", "14"));
        if (days < 2 || days > 365) {
            return false;
        }
        vec1   = ChartData::StdDev(line0->data(), days);
        label1 = util::format("Std. dev. %d Days", days);
    }
    else if (formula == ChartData::FORMULAS::STOCHASTICS) {
        auto days = util::to_long(fl_input_str(3, "Enter number of days (2 - 365)", "14"));
        if (days < 2 || days > 365) {
            return false;
        }
        vec1   = ChartData::Stochastics(line0->data(), days);
        label1 = util::format("Stochastics %d Days", days);
        if (support == true) {
            vec2   = ChartData::Fixed(line0->data(), 20.0);
            label2 = "Stochastics 20";
            type2  = ChartLine::TYPE::EXPAND_HORIZONTAL_FIRST;
            vec3   = ChartData::Fixed(line0->data(), 80.0);
            label3 = "Stochastics 80";
            type3  = ChartLine::TYPE::EXPAND_HORIZONTAL_FIRST;
        }
    }
    if (vec1.size() == 0) {
        fl_alert("To few data values!");
        return false;
    }
    line1.set_data(vec1).set_label(label1).set_type(type1).set_align(line0->align()).set_color(FL_BLUE);
    ChartLineSetup(top_window(), line1).run();
    _area->add_line(line1);
    if (vec2.size() > 0) {
        line2.set_data(vec2).set_label(label2).set_type(type2).set_align(line1.align()).set_color(FL_BLACK);
        _area->add_line(line2);
    }
    if (vec3.size() > 0) {
        line3.set_data(vec3).set_label(label3).set_type(type3).set_align(line1.align()).set_color(FL_BLACK);
        _area->add_line(line3);
    }
    init_new_data();
    return true;
}
void Chart::_create_tooltip(bool ctrl) {
    int         X   = Fl::event_x();
    int         Y   = Fl::event_y();
    std::string old = _tooltip;
    _tooltip = "";
    if (_area == nullptr) {
        Fl::redraw();
        return;
    }
    const Date::FORMAT FORMAT    = (_date_range == ChartData::RANGE::HOUR || _date_range == ChartData::RANGE::MIN || _date_range == ChartData::RANGE::SEC) ? Date::FORMAT::NAME_TIME_LONG : Date::FORMAT::NAME_LONG;
    const int          STOP      = _date_start + _ticks;
    int                start     = _date_start;
    int                x1        = x() + _margin_left * _CH;
    int                left_dec  = 0;
    int                right_dec = 0;
    if (_area->left_scale().tick() < 10.0 ) {
        left_dec = util::count_decimals(_area->left_scale().tick()) + 1;
    }
    if (_area->right_scale().tick() < 10.0 ) {
        right_dec = util::count_decimals(_area->right_scale().tick()) + 1;
    }
    while (start <= STOP && start < static_cast<int>(_dates.size())) {
        if (X >= x1 && X <= x1 + _tick_width - 1) {
            const std::string fancy_date = Date::FromString(_dates[start].date.c_str()).format(FORMAT);
            const ChartLine*  line       = _area->selected_line();
            _tooltip = fancy_date + "\n \n \n ";
            if (ctrl == false || line == nullptr || line->size() == 0 || line->is_visible() == false) {
                const double                Y_DIFF = static_cast<double>((y() + _area->rect().b()) - Y);
                const std::optional<double> L_MIN  = _area->left_scale().min();
                const std::optional<double> R_MIN  = _area->right_scale().min();
                std::string                 left;
                std::string                 right;
                if (_area->left_scale().diff() > 0.0) {
                    left = util::format_double(L_MIN.value() + (Y_DIFF / _area->left_scale().pixel()), left_dec, '\'');
                }
                if (_area->right_scale().diff() > 0.0) {
                    right = util::format_double(R_MIN.value() + (Y_DIFF / _area->right_scale().pixel()), right_dec, '\'');
                }
                const int LEN = static_cast<int>(left.length() > right.length() ? left.length() : right.length());
                if (left != "" && right != "") {
                    _tooltip = util::format("%s\nleft:  %*s\nright: %*s\n ", fancy_date.c_str(), LEN, left.c_str(), LEN, right.c_str());
                }
                else if (left != "") {
                    _tooltip = util::format("%s\nleft: %*s\n \n ", fancy_date.c_str(), LEN, left.c_str());
                }
                else if (right != "") {
                    _tooltip = util::format("%s\nright: %*s\n \n ", fancy_date.c_str(), LEN, right.c_str());
                }
            }
            else {
                const size_t index = ChartData::BinarySearch(line->data(), _dates[start]);
                if (index != (size_t) -1) {
                    const int         DEC   = (line->align() == FL_ALIGN_RIGHT) ? right_dec : left_dec;
                    const ChartData&  DATA  = line->data()[index];
                    const std::string HIGH  = util::format_double(DATA.high, DEC, '\'');
                    const std::string LOW   = util::format_double(DATA.low, DEC, '\'');
                    const std::string CLOSE = util::format_double(DATA.close, DEC, '\'');
                    const int         LEN   = static_cast<int>(LOW.length() > HIGH.length() ? LOW.length() : HIGH.length());
                    _tooltip = util::format("%s\nhigh:  %*s\nclose: %*s\nlow:   %*s", fancy_date.c_str(), LEN, HIGH.c_str(), LEN, CLOSE.c_str(), LEN, LOW.c_str());
                }
            }
            break;
        }
        x1    += _tick_width;
        start += 1;
    }
    if (_tooltip != "" || old != "") {
        redraw();
    }
}
void Chart::debug() const {
#ifdef DEBUG
    ChartData first, last, start, end;
    if (_dates.size() > 0) {
        first = _dates.front();
    }
    if (_dates.size() > 1) {
        last = _dates.back();
    }
    if (static_cast<int>(_dates.size()) > _date_start) {
        start = _dates[_date_start];
    }
    if (_ticks >= 0 && static_cast<int>(_dates.size()) > (_date_start + _ticks)) {
        end = _dates[_date_start + _ticks];
    }
    printf("\n");
    printf("--------------------------------------------\n");
    printf("Chart:\n");
    printf("\tx - y:                    %4d, %4d\n", x(), y());
    printf("\tw - h:                    %4d, %4d\n", w(), h());
    printf("\ttop - bottom:             %4d, %4d\n", _top_space, _bottom_space);
    printf("\tmargin_l - margin_r:      %4d, %4d\n", _margin_left, _margin_right);
    printf("\tcw - ch:                  %4d, %4d\n", _CW, _CH);
    printf("\talt_size:        %19.1f\n", _alt_size);
    printf("\tticks:           %19d\n", _ticks);
    printf("\ttick_width:      %19d\n", _tick_width);
    printf("\tblock_dates:     %19d\n", static_cast<int>(_block_dates.size()));
    printf("\tdate_start:      %19d\n", _date_start);
    printf("\tdate_end:        %19d\n", _date_start + _ticks);
    printf("\tlabels:          %19s\n", _labels ? "YES" : "NO");
    printf("\thorizontal:      %19s\n", _horizontal ? "YES" : "NO");
    printf("\tvertical:        %19s\n", _vertical ? "YES" : "NO");
    printf("\tdate_range:      %19s\n", ChartData::RangeToString(_date_range).c_str());
    printf("\tdates:           %19d\n", static_cast<int>(_dates.size()));
    printf("\tfirst date:      %19s\n", first.date.c_str());
    printf("\tlast date:       %19s\n", last.date.c_str());
    printf("\tfirst visible:   %19s\n", start.date.c_str());
    printf("\tlast visible:    %19s\n", end.date.c_str());
    for (const auto& area : _areas) {
        area.debug();
    }
    fflush(stdout);
#endif
}
void Chart::debug_line() const {
#ifdef DEBUG
    if (_area == nullptr || _area->selected_line() == nullptr) {
        return;
    }
    const auto* line = _area->selected_line();
    const auto& data = line->data();
    int         curr = _date_start;
    const int   stop = _date_start + _ticks;
    double      min  = 999'999'999'999'999;
    double      max  = -999'999'999'999'999;
    printf("ChartLine: (%d values) %s\n", static_cast<int>(data.size()), line->label().c_str());
    while (curr <= stop && curr < static_cast<int>(_dates.size())) {
        auto& date  = _dates[curr];
        auto  index = ChartData::BinarySearch(data, date);
        if (index != (size_t) -1) {
            auto& d = line->data()[index];
            printf("%5d: %s, %20.8f, %20.8f, %20.8f\n", curr, date.date.c_str(), d.high, d.low, d.close);
            if (d.high > max) max = d.high;
            if (d.low < min) min = d.low;
        }
        curr++;
    }
    printf("%.8f < %8f\n", min, max);
    fflush(stdout);
#endif
}
void Chart::draw() {
#ifdef DEBUG
#endif
    if (_printing == true) {
        fl_line_style(FL_SOLID, 1);
        fl_rectf(x(), y(), w(), h(), FL_BACKGROUND2_COLOR);
    }
    else {
        Fl_Group::draw();
    }
    if (_areas[0].rect().w() < 40 || h() < _top_space + _bottom_space + _CH) {
        fl_line_style(FL_SOLID, 1);
        fl_rect(x() + 12, y() + 12, w() - 24, h() - 36, FL_FOREGROUND_COLOR);
        return;
    }
    _FLW_CHART_CLIP(fl_push_clip(x(), y(), w(), h() - _scroll->h()))
    _draw_label();
    _draw_xlabels();
    _FLW_CHART_CLIP(fl_pop_clip())
    for (auto& area : _areas) {
        if (area.percent() >= _CHART_MIN_AREA_SIZE) {
            _FLW_CHART_CLIP(fl_push_clip(x(), y(), w(), h() - _scroll->h()))
            _draw_ver_lines(area);
            _draw_ylabels(area, FL_ALIGN_LEFT);
            _draw_ylabels(area, FL_ALIGN_RIGHT);
            _FLW_CHART_CLIP(fl_pop_clip())
            _FLW_CHART_CLIP(fl_push_clip(x() + area.rect().x(), y() + area.rect().y() - 1, area.rect().w() + 1, area.rect().h() + 2))
            _draw_lines(area);
            _draw_line_labels(area);
            fl_line_style(FL_SOLID, 1);
            fl_rect(x() + area.rect().x(), y() + area.rect().y() - 1, area.rect().w() + 1, area.rect().h() + 2, labelcolor());
            _FLW_CHART_CLIP(fl_pop_clip())
        }
    }
    _draw_tooltip();
    fl_line_style(0);
#ifdef DEBUG
#endif
}
void Chart::_draw_label() {
    if (_label == "") {
        return;
    }
    fl_color(FL_FOREGROUND_COLOR);
    fl_font(flw::PREF_FIXED_FONT, _CH * 1.5);
    fl_draw(_label.c_str(), x() + _areas[0].rect().x(), y(), _areas[0].rect().w(), _top_space, FL_ALIGN_CENTER | FL_ALIGN_INSIDE | FL_ALIGN_CLIP);
    _FLW_CHART_DEBUG(fl_rect(x() + _areas[0].rect().x(), y(), _areas[0].rect().w() + 1, _top_space))
}
void Chart::_draw_lines(ChartArea& area) {
    for (const auto& line : area.lines()) {
        const auto& scale = (line.align() == FL_ALIGN_LEFT) ? area.left_scale() : area.right_scale();
        if (line.size() > 0 && line.is_visible() == true && scale.diff() >= ChartData::MIN_VALUE) {
            const double SCALE_MIN   = scale.min().value();
            const double SCALE_PIXEL = scale.pixel();
            int          x1          = area.rect().x();
            const int    Y1          = area.rect().y();
            const double B1          = y() + area.rect().b();
            double       x_last      = -10;
            double       y_last      = -10;
            int          date_c      = _date_start;
            const int    DATE_END    = _date_start + _ticks + 1;
            int          width       = static_cast<int>(line.width());
            int          width2      = 0;
            int          width3      = width;
            auto         style       = FL_SOLID;
            if (line.type() == ChartLine::TYPE::BAR_HLC && width + width3 >= _tick_width) {
                width = width3 = _tick_width / 2;
                if (width + width3 >= _tick_width) {
                    width--;
                }
            }
            else if (width >= _tick_width) {
                width = _tick_width - 1;
            }
            if (line.type() == ChartLine::TYPE::LINE_DOT) {
                style = FL_DOT;
            }
            fl_color(line.color());
            fl_line_style(style, width);
            width2 = (width > 1) ? width / 2 : width2;
            while (date_c <= DATE_END && date_c < static_cast<int>(_dates.size())) {
                const size_t INDEX = ChartData::BinarySearch(line.data(), _dates[date_c]);
                if (INDEX != static_cast<size_t>(-1)) {
                    const auto&  DATA = line.data()[INDEX];
                    const double YH   = (DATA.high - SCALE_MIN) * SCALE_PIXEL;
                    const double YL   = (DATA.low - SCALE_MIN) * SCALE_PIXEL;
                    const double YC   = (DATA.close - SCALE_MIN) * SCALE_PIXEL;
                    const int    YH2  = static_cast<int>(B1 - YH);
                    const int    YL2  = static_cast<int>(B1 - YL);
                    const int    YC2  = static_cast<int>(B1 - YC);
                    if (line.type() == ChartLine::TYPE::LINE || line.type() == ChartLine::TYPE::LINE_DOT) {
                        if (x_last > -10 && y_last > -10) {
                            fl_line(x_last + width2, static_cast<int>(B1 - y_last), x() + x1 + width2, YC2);
                        }
                    }
                    else if (line.type() == ChartLine::TYPE::BAR) {
                        fl_line(x() + x1 + width2, YH2, x() + x1 + width2, YL2);
                    }
                    else if (line.type() == ChartLine::TYPE::BAR_CLAMP) {
                        fl_line(x() + x1 + width2, YH2, x() + x1 + width2, B1);
                    }
                    else if (line.type() == ChartLine::TYPE::BAR_HLC) {
                        fl_line(x() + x1 + width2, YH2, x() + x1 + width2, YL2);
                        fl_line(x() + x1, YC2, x() + x1 + width + width3, YC2);
                    }
                    else if (line.type() == ChartLine::TYPE::HORIZONTAL) {
                        fl_line(x() + x1, YC2, x() + x1 + _tick_width - (width == 1 ? 0 : 1), YC2);
                    }
                    else if (line.type() == ChartLine::TYPE::EXPAND_VERTICAL) {
                        fl_line(x() + x1, B1, x() + x1, y() + Y1);
                    }
                    else if (line.type() == ChartLine::TYPE::EXPAND_HORIZONTAL_FIRST) {
                        fl_line(x() + _margin_left * _CH, YC2, x() + Fl_Widget::w() - _margin_right * _CH, YC2);
                        break;
                    }
                    else if (line.type() == ChartLine::TYPE::EXPAND_HORIZONTAL_ALL) {
                        fl_line(x() + _margin_left * _CH, YC2, x() + Fl_Widget::w() - _margin_right * _CH, YC2);
                    }
                    x_last = x() + x1;
                    y_last = YC;
                }
                x1 += _tick_width;
                date_c++;
            }
        }
    }
}
void Chart::_draw_line_labels(ChartArea& area) {
    if (_labels == false) {
        return;
    }
    fl_font(flw::PREF_FIXED_FONT, _CH);
    fl_line_style(FL_SOLID, 1);
    const int LEFT_X  = x() + area.rect().x() + 6;
    int       left_y  = y() + area.rect().y() + 6;
    int       left_w  = 0;
    int       left_h  = 0;
    const int RIGHT_X = x() + area.rect().r() - 6;
    int       right_y = left_y;
    int       right_w = 0;
    int       right_h = 0;
    for (const auto& line : area.lines()) {
        int  ws    = 0;
        int  hs    = 0;
        auto label = _CHART_LABEL_SYMBOL + line.label();
        fl_measure(label.c_str(), ws, hs);
        if (line.align() == FL_ALIGN_LEFT) {
            left_h++;
            if (ws > left_w) {
                left_w = ws;
            }
        }
        else {
            right_h++;
            if (ws > right_w) {
                right_w = ws;
            }
        }
    }
    left_h  *= fl_height();
    left_h  += 8;
    right_h *= fl_height();
    right_h += 8;
    if (left_w > 0) {
        left_w += _CH;
        fl_color(FL_BACKGROUND2_COLOR);
        fl_rectf(LEFT_X, left_y, left_w, left_h);
        fl_color(FL_FOREGROUND_COLOR);
        fl_rect(LEFT_X, left_y, left_w, left_h);
    }
    if (right_w > 0) {
        right_w += _CH;
        fl_color(FL_BACKGROUND2_COLOR);
        fl_rectf(RIGHT_X - right_w, left_y, right_w, right_h);
        fl_color(FL_FOREGROUND_COLOR);
        fl_rect(RIGHT_X - right_w, left_y, right_w, right_h);
    }
    if (left_w > 0 || right_w > 0) {
        size_t line_c = 0;
        for (auto& line : area.lines()) {
            auto const label = (area.selected() == line_c++) ? _CHART_LABEL_SYMBOL + line.label() : line.label();
            fl_color((line.is_visible() == false) ? FL_GRAY : line.color());
            if (line.align() == FL_ALIGN_LEFT) {
                fl_draw(label.c_str(), LEFT_X + 4, left_y + 4, left_w - 8, fl_height(), FL_ALIGN_LEFT | FL_ALIGN_CLIP);
                _FLW_CHART_DEBUG(fl_rect(LEFT_X + 4, left_y + 4, left_w - 8, fl_height()))
                const_cast<ChartLine*>(&line)->set_label_rect(LEFT_X, left_y + 5, left_w, fl_height() + 1);
                left_y += fl_height();
            }
            else {
                fl_draw(label.c_str(), RIGHT_X - right_w + 4, right_y + 4, right_w - 8, fl_height(), FL_ALIGN_LEFT | FL_ALIGN_CLIP);
                _FLW_CHART_DEBUG(fl_rect(RIGHT_X - right_w + 4, right_y + 4, right_w - 8, fl_height()))
                const_cast<ChartLine*>(&line)->set_label_rect(RIGHT_X - right_w, right_y + 5, right_w, fl_height() + 1);
                right_y += fl_height();
            }
        }
    }
}
void Chart::_draw_tooltip() {
    if (_tooltip == "" || _area == nullptr) {
        return;
    }
    fl_font(flw::PREF_FIXED_FONT, _CH);
    int x1 = Fl::event_x();
    int y1 = Fl::event_y();
    int w1 = 14;
    int h1 = 5;
    if (x1 > _area->rect().r() - _CH * (w1 + 5)) {
        x1 -= _CH * (w1 + 4);
    }
    else {
        x1 += _CH * 2;
    }
    if (y1 > y() + h() - _CH * (h1 + 7)) {
        y1 -= _CH * (h1 + 2);
    }
    else {
        y1 += _CH * 2;
    }
    w1 += 3;
    fl_color(FL_BACKGROUND2_COLOR);
    fl_rectf(x1, y1, _CH * w1, _CH * h1);
    fl_color(FL_FOREGROUND_COLOR);
    fl_rect(x1, y1, _CH * w1, _CH * h1);
    fl_line(Fl::event_x(), y() + _area->rect().y(), Fl::event_x(), y() + _area->rect().y() + _area->rect().h());
    fl_line(x() + _area->rect().x(), Fl::event_y(), x() + _area->rect().x() + _area->rect().w(), Fl::event_y());
    fl_draw(_tooltip.c_str(), x1 + 4, y1, _CH * w1 - 8, _CH * h1, FL_ALIGN_LEFT | FL_ALIGN_INSIDE | FL_ALIGN_CLIP);
}
void Chart::_draw_ver_lines(ChartArea& area) {
    if (_vertical == false) {
        return;
    }
    fl_color(fl_color_average(FL_FOREGROUND_COLOR, FL_BACKGROUND2_COLOR, 0.2));
    fl_line_style(FL_SOLID, 1);
    for (size_t i = 0; i < Chart::MAX_VLINES; i++) {
        if (_ver_pos[i] < 0) {
            break;
        }
        fl_line(_ver_pos[i], y() + area.rect().y(), _ver_pos[i], y() + area.rect().b());
    }
}
void Chart::_draw_xlabels() {
    fl_color(labelcolor());
    fl_line_style(FL_SOLID, 1);
    int        cw2      = 0;
    const int  CH2      = _CH * _alt_size;
    const int  Y1       = y() + (h() - _bottom_space);
    int        date_c   = _date_start;
    const int  DATE_END = _date_start + _ticks;
    int        ver_c    = 0;
    int        last     = -1;
    int        x1       = x() + _margin_left * _CH;
    int        x_last   = 0;
    char       buffer1[100];
    char       buffer2[100];
    _ver_pos[0] = -1;
    fl_font(flw::PREF_FIXED_FONT, CH2);
    cw2 = fl_width("X");
    while (date_c <= DATE_END && date_c < static_cast<int>(_dates.size())) {
        const Date date  = Date::FromString(_dates[date_c].date.c_str());
        bool       addv  = false;
        int        month = 1;
        *buffer1 = 0;
        *buffer2 = 0;
        if (_date_range == ChartData::RANGE::SEC) {
            snprintf(buffer2, 100, "%02d", date.second());
            if (date.minute() != last) {
                addv = true;
                last = date.minute();
                if (x1 >= x_last) {
                    snprintf(buffer1, 100, "%04d-%02d-%02d/%02d:%02d", date.year(), date.month(), date.day(), date.hour(), date.minute());
                }
                else {
                    month = 3;
                }
            }
        }
        else if (_date_range == flw::ChartData::RANGE::MIN) {
            snprintf(buffer2, 100, "%02d", date.minute());
            if (date.hour() != last) {
                addv = true;
                last = date.hour();
                if (x1 >= x_last) {
                    snprintf(buffer1, 100, "%04d-%02d-%02d/%02d", date.year(), date.month(), date.day(), date.hour());
                }
                else {
                    month = 3;
                }
            }
        }
        else if (_date_range == ChartData::RANGE::HOUR) {
            snprintf(buffer2, 100, "%02d", date.hour());
            if (date.day() != last) {
                addv = true;
                last = date.day();
                if (x1 >= x_last) {
                    snprintf(buffer1, 100, "%04d-%02d-%02d", date.year(), date.month(), date.day());
                }
                else {
                    month = 3;
                }
            }
        }
        else if (_date_range == ChartData::RANGE::DAY || _date_range == ChartData::RANGE::WEEKDAY) {
            snprintf(buffer2, 100, "%02d", date.day());
            if (date.month() != last) {
                addv = true;
                last = date.month();
                if (x1 >= x_last) {
                    snprintf(buffer1, 100, "%04d-%02d", date.year(), date.month());
                }
                else {
                    month = 3;
                }
            }
        }
        else if (_date_range == ChartData::RANGE::FRIDAY || _date_range == ChartData::RANGE::SUNDAY) {
            snprintf(buffer2, 100, "%02d", date.week());
            if (date.month() != last) {
                addv = true;
                last = date.month();
                if (x1 >= x_last) {
                    snprintf(buffer1, 100, "%04d-%02d", date.year(), date.month());
                }
                else {
                    month = 3;
                }
            }
        }
        else if (_date_range == ChartData::RANGE::MONTH) {
            snprintf(buffer2, 100, "%02d", date.month());
            if (date.month() != last) {
                addv = true;
                last = date.month();
                if (x1 >= x_last) {
                    snprintf(buffer1, 100, "%04d-%02d", date.year(), date.month());
                }
                else {
                    month = 1;
                }
            }
        }
        if (*buffer1 != 0) {
            const double LEN    = static_cast<double>(strlen(buffer1));
            const double ADJUST = static_cast<double>((LEN / 2.0) * _CW);
            month = 4;
            fl_font(flw::PREF_FIXED_FONT, _CH);
            fl_draw(buffer1, x1 - ADJUST, Y1 + _CH + _CHART_TICK_SIZE * 2, ADJUST * 2, _CH, FL_ALIGN_CENTER | FL_ALIGN_INSIDE);
            _FLW_CHART_DEBUG(fl_rect(x1 - ADJUST, Y1 + _CH + _CHART_TICK_SIZE * 2, ADJUST * 2, _CH))
            x_last = x1 + ADJUST * 2 + _CW;
        }
        if (*buffer2 != 0 && (cw2 * 2 + _CHART_TICK_SIZE) < _tick_width) {
            fl_font(flw::PREF_FIXED_FONT, CH2);
            fl_draw(buffer2, x1 - cw2, Y1 + _CHART_TICK_SIZE * 2, cw2 * 2, CH2, FL_ALIGN_CENTER | FL_ALIGN_INSIDE);
            _FLW_CHART_DEBUG(fl_rect(x1 - cw2, Y1 + _CHART_TICK_SIZE * 2, cw2 * 2, CH2))
            month = 1;
        }
        fl_line(x1, Y1, x1, Y1 + _CHART_TICK_SIZE * month);
        if (addv == true && ver_c < static_cast<int>(Chart::MAX_VLINES - 1)) {
            _ver_pos[ver_c++] = x1;
        }
        x1 += _tick_width;
        date_c++;
    }
    _ver_pos[ver_c] = -1;
}
void Chart::_draw_ylabels(ChartArea& area, Fl_Align align) {
    ChartScale& scale = (align == FL_ALIGN_LEFT) ? area.left_scale() : area.right_scale();
    if (scale.diff() < 0.0 || scale.pixel() * scale.tick() < 1.0) {
        return;
    }
    fl_font(flw::PREF_FIXED_FONT, _CH);
    const int    TICK3  = _CHART_TICK_SIZE * 3;
    const double Y_INC  = (scale.pixel() * scale.tick());
    const double Y_FR   = util::count_decimals(scale.tick());
    const int    CH     = _CH / 2;
    const int    WIDTH  = (align == FL_ALIGN_LEFT ? area.rect().x() - TICK3 : w() - area.rect().r() - TICK3 - 1);
    const int    X1     = (align == FL_ALIGN_LEFT ? x() + _CHART_TICK_SIZE : 0);
    double       y1     = area.rect().b();
    double       y_last = 10'000;
    double       y_val  = scale.min().value();
    while (static_cast<int>(y1 + 0.5) >= area.rect().y()) {
        if (y_last > y1) {
            const auto Y2    = static_cast<int>(y() + y1);
            const auto LABEL = util::format_double(y_val, Y_FR, '\'');
            if (align == FL_ALIGN_LEFT) {
                const int R = x() + area.rect().x();
                fl_color(labelcolor());
                fl_line(R - _CHART_TICK_SIZE, Y2, R, Y2);
                fl_draw(LABEL.c_str(), X1 + 1, Y2 - CH, WIDTH, _CH, FL_ALIGN_RIGHT | FL_ALIGN_INSIDE | FL_ALIGN_CLIP);
                _FLW_CHART_DEBUG(fl_rect(X1 + 1, Y2 - CH, WIDTH, _CH))
            }
            else {
                const int R = x() + area.rect().r();
                fl_color(labelcolor());
                fl_line(R, Y2, R + _CHART_TICK_SIZE, Y2);
                fl_draw(LABEL.c_str(), R + _CHART_TICK_SIZE * 2, Y2 - CH, WIDTH, _CH, FL_ALIGN_RIGHT | FL_ALIGN_INSIDE | FL_ALIGN_CLIP);
                _FLW_CHART_DEBUG(fl_rect(R + _CHART_TICK_SIZE * 2, Y2 - CH, WIDTH, _CH))
            }
            if (_horizontal == true && Y2 > y() + area.rect().y() + _CHART_TICK_SIZE) {
                fl_color(fl_color_average(FL_FOREGROUND_COLOR, FL_BACKGROUND2_COLOR, 0.2));
                fl_line(x() + area.rect().x(), Y2, x() + area.rect().r(), Y2);
            }
            y_last = y1 - _CH - _CHART_TICK_SIZE;
        }
        y1    -= Y_INC;
        y_val += scale.tick();
    }
}
flw::ChartArea* Chart::_get_active_area(int X, int Y) {
    for (auto& area : _areas) {
        if (area.percent() >= 10 &&
            X >= x() + area.rect().x() &&
            Y >= y() + area.rect().y() &&
            X <= x() + area.rect().r() &&
            Y <= y() + area.rect().b() + 1) {
            return &area;
        }
    }
    return nullptr;
}
int Chart::handle(int event) {
    static bool LEFT = false;
    if (event == FL_PUSH) {
        const int x = Fl::event_x();
        const int y = Fl::event_y();
        const int b = Fl::event_button();
        if (b == FL_LEFT_MOUSE) {
            LEFT  = true;
            _area = _get_active_area(x, y);
            _create_tooltip(Fl::event_ctrl() != 0);
            if (_tooltip != "") {
                return 1;
            }
        }
        else if (b == FL_MIDDLE_MOUSE) {
            LEFT  = false;
            _area = _get_active_area(x, y);
            if (_area != nullptr) {
                size_t line_c = 0;
                for (const auto& line : _area->lines()) {
                    auto rect = line.label_rect();
                    if (x > rect.x() && x < rect.r() && y > rect.y() && y < rect.b()) {
                        _area->set_selected(line_c);
                        redraw();
                        break;
                    }
                    line_c++;
                }
            }
            return 1;
        }
        else if (b == FL_RIGHT_MOUSE) {
            LEFT  = false;
            _area = _get_active_area(x, y);
            _show_menu();
            return 1;
        }
    }
    else if (event == FL_DRAG && LEFT == true) {
        _area = _get_active_area(Fl::event_x(), Fl::event_y());
        _create_tooltip(Fl::event_ctrl() != 0);
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
        const double dy   = (Fl::event_dy() * 10.0);
        const double dx   = (Fl::event_dx() * 10.0);
        const double dd   = fabs(dx) > fabs(dy) ? dx : dy;
        const double adj  = _ticks / dd;
        if (fabs(dd) < 1.0) {
            return Fl_Group::handle(event);
        }
        else if (Fl::event_ctrl() > 0) {
            const int width = (adj > 0.0) ? _tick_width + 1 : _tick_width - 1;
            if (width >= Chart::MIN_TICK && width <= Chart::MAX_TICK) {
                set_tick_width(width);
                init();
            }
            return 1;
        }
        else {
            if (adj > 0) {
                if ((pos + adj) > size) {
                    static_cast<Fl_Slider*>(_scroll)->value(size);
                }
                else {
                    static_cast<Fl_Slider*>(_scroll)->value(pos + adj);
                }
            }
            else {
                if ((pos + adj) < 0.0) {
                    static_cast<Fl_Slider*>(_scroll)->value(0.0);
                }
                else {
                    static_cast<Fl_Slider*>(_scroll)->value(pos + adj);
                }
            }
            Chart::_CallbackScrollbar(0, this);
            Fl::event_clicks(0);
            return 1;
        }
    }
    return Fl_Group::handle(event);
}
void Chart::_init(bool calc_dates) {
#ifdef DEBUG
#endif
    unsigned kludge = 0;
    fl_font(flw::PREF_FIXED_FONT, flw::PREF_FIXED_FONTSIZE);
    *const_cast<int*>(&_CW) = fl_width("X");
    *const_cast<int*>(&_CH) = flw::PREF_FIXED_FONTSIZE;
    if (calc_dates == true) {
        WaitCursor wc;
        _calc_dates();
        _margin_left  = _CHART_MIN_MARGIN;
        _margin_right = _CHART_MIN_MARGIN;
        kludge++;
    }
    while (kludge <= 1) {
        _calc_area_width();
        _calc_area_height();
        _calc_ymin_ymax();
        _calc_yscale();
        _calc_margins();
        _calc_area_width();
        kludge--;
    }
    redraw();
#ifdef DEBUG
#endif
}
StringVector Chart::_label_array(const ChartArea& area, Chart::LABELTYPE labeltype) const {
    auto res = StringVector();
    for (const auto& l : area.lines()) {
        std::string s;
        if (labeltype == LABELTYPE::OFF) {
            s = "0";
        }
        else if (labeltype == LABELTYPE::ON) {
            s = "1";
        }
        else if (labeltype == LABELTYPE::VISIBLE) {
            s = (l.is_visible() == true) ? "1" : "0";
        }
        s += l.label();
        res.push_back(s);
    }
    return res;
}
bool Chart::load_csv() {
    if (_area == nullptr || _area->size() >= ChartArea::MAX_LINES) {
        fl_alert("Max line count reached!");
        return false;
    }
    auto filename = util::to_string(fl_file_chooser("Select CSV File", "All Files (*)\tCSV Files (*.csv)", ""));
    if (util::is_whitespace_or_empty(filename.c_str()) == true) {
        return false;
    }
    auto vec = ChartData::LoadCSV(filename);
    if (vec.size() == 0) {
        fl_alert("To few data values!");
        return false;
    }
    auto line = ChartLine(vec, filename);
    line.set_color(FL_BLUE);
    ChartLineSetup(top_window(), line).run();
    _area->add_line(line);
    init_new_data();
    return true;
}
bool Chart::load_json() {
    auto filename = util::to_string(fl_file_chooser("Select JSON File", "All Files (*)\tJSON Files (*.json)", ""));
    if (util::is_whitespace_or_empty(filename.c_str()) == true) {
        return false;
    }
    return load_json(filename);
}
bool Chart::load_json(std::string filename) {
    clear();
    redraw();
    auto wc  = WaitCursor();
    auto buf = File::Load(filename);
    if (buf.p == nullptr) {
        fl_alert("error: failed to load %s", filename.c_str());
        return false;
    }
    auto js  = JS();
    auto err = js.decode(buf.p, buf.s, true);
    if (err != "") {
        fl_alert("error: failed to parse %s (%s)", filename.c_str(), err.c_str());
        return false;
    }
    if (js.is_object() == false) _FLW_CHART_ERROR(&js);
    for (const auto j : js.vo_to_va()) {
        if (j->name() == "flw::chart" && j->is_object() == true) {
            for (const auto j2 : j->vo_to_va()) {
                if (j2->name() == "version" && j2->is_number() == true) {
                    if (j2->vn_i() != Chart::VERSION) { fl_alert("error: wrong chart version!\nI expected version %d but the json file had version %d!", static_cast<int>(j2->vn_i()), Chart::VERSION); clear(); return false; }
                }
                else if (j2->name() == "label" && j2->is_string() == true) {
                    set_main_label(j2->vs_u());
                }
                else if (j2->name() == "date_range" && j2->is_string() == true) set_date_range(ChartData::StringToRange(j2->vs()));
                else if (j2->name() == "tick_width" && j2->is_number() == true) set_tick_width(j2->vn_i());
                else if (j2->name() == "labels" && j2->is_bool() == true)       set_line_labels(j2->vb());
                else if (j2->name() == "horizontal" && j2->is_bool() == true)   set_hor_lines(j2->vb());
                else if (j2->name() == "vertical" && j2->is_bool() == true)     set_ver_lines(j2->vb());
                else _FLW_CHART_ERROR(j2)
            }
        }
        else if (j->name() == "flw::chart_areas" && j->is_object() == true) {
            long long int area[6] = { 0 };
            for (const auto j2 : j->vo_to_va()) {
                if (j2->name() == "area0" && j2->is_number() == true)      area[1]   = j2->vn_i();
                else if (j2->name() == "area1" && j2->is_number() == true) area[2]   = j2->vn_i();
                else if (j2->name() == "area2" && j2->is_number() == true) area[3]   = j2->vn_i();
                else if (j2->name() == "area3" && j2->is_number() == true) area[4]   = j2->vn_i();
                else if (j2->name() == "area4" && j2->is_number() == true) area[5]   = j2->vn_i();
                else if (j2->name() == "min0" && j2->is_number() == true)  _areas[0].set_min_clamp(j2->vn());
                else if (j2->name() == "max0" && j2->is_number() == true)  _areas[0].set_max_clamp(j2->vn());
                else if (j2->name() == "min1" && j2->is_number() == true)  _areas[1].set_min_clamp(j2->vn());
                else if (j2->name() == "max1" && j2->is_number() == true)  _areas[1].set_max_clamp(j2->vn());
                else if (j2->name() == "min2" && j2->is_number() == true)  _areas[2].set_min_clamp(j2->vn());
                else if (j2->name() == "max2" && j2->is_number() == true)  _areas[2].set_max_clamp(j2->vn());
                else if (j2->name() == "min3" && j2->is_number() == true)  _areas[3].set_min_clamp(j2->vn());
                else if (j2->name() == "max3" && j2->is_number() == true)  _areas[3].set_max_clamp(j2->vn());
                else if (j2->name() == "min4" && j2->is_number() == true)  _areas[4].set_min_clamp(j2->vn());
                else if (j2->name() == "max4" && j2->is_number() == true)  _areas[4].set_max_clamp(j2->vn());
                else _FLW_CHART_ERROR(j2)
            }
            area[0] += set_area_size(area[1], area[2], area[3], area[4], area[5]) == false;
            if (area[0] != 0) _FLW_CHART_ERROR(j)
        }
        else if (j->name() == "flw::chart_lines" && j->is_array() == true) {
            for (const auto j2 : *j->va()) {
                int             line[5]  = { 0, 0, 0, 0, 1 };
                std::string     label;
                std::string     type;
                ChartDataVector vec;
                if ( j2->is_object() == true) {
                    for (const auto l : j2->vo_to_va()) {
                        if (l->name() == "area" && l->is_number() == true) {
                            line[1] = l->vn_i();
                            if (line[1] < 0 || line[1] > static_cast<int>(ChartArea::AREA::LAST)) _FLW_CHART_ERROR(l)
                        }
                        else if (l->name() == "align" && l->is_number() == true)      line[0]  = l->vn_i();
                        else if (l->name() == "color" && l->is_number() == true)      line[2]  = l->vn_i();
                        else if (l->name() == "label" && l->is_string() == true)      label    = l->vs_u();
                        else if (l->name() == "type" && l->is_string() == true)       type     = l->vs();
                        else if (l->name() == "visible" && l->is_bool() == true)      line[4]  = l->vb();
                        else if (l->name() == "width" && l->is_number() == true)      line[3]  = l->vn_i();
                        else if (l->name() == "yx" && l->is_array() == true) {
                            for (auto p : *l->va()) {
                                if (p->is_array() == false) _FLW_CHART_ERROR(p)
                                else if (p->size() == 2 && (*p)[0]->is_string() == true && (*p)[1]->is_number() == true) vec.push_back(ChartData((*p)[0]->vs(), (*p)[1]->vn()));
                                else if (p->size() == 4 && (*p)[0]->is_string() == true && (*p)[1]->is_number() == true && (*p)[2]->is_number() == true && (*p)[3]->is_number() == true) vec.push_back(ChartData((*p)[0]->vs(), (*p)[1]->vn(), (*p)[2]->vn(), (*p)[3]->vn()));
                                else _FLW_CHART_ERROR(p)
                            }
                        }
                        else _FLW_CHART_ERROR(l)
                    }
                }
                auto l = ChartLine(vec, label);
                auto a = static_cast<ChartArea::AREA>(line[1]);
                l.set_align(line[0]).set_color(line[2]).set_label(label).set_width(line[3]).set_type_from_string(type).set_visible(line[4]);
                area(a).add_line(l);
            }
        }
        else if (j->name() == "flw::chart_block" && j->is_array() == true) {
            ChartDataVector dates;
            for (const auto d : *j->va()) {
                if (d->is_string() == true) dates.push_back(ChartData(d->vs()));
                else _FLW_CHART_ERROR(d)
            }
            set_block_dates(dates);
        }
        else _FLW_CHART_ERROR(j)
    }
    init_new_data();
    return true;
}
bool Chart::_move_or_delete_line(ChartArea* area, size_t index, bool move, ChartArea::AREA destination) {
    if (area == nullptr || index >= area->size()) {
        return false;
    }
    ChartLine* line1 = area->line(index);
    if (line1 == nullptr) {
        return false;
    }
    if (move == true) {
        if (destination == area->area()) {
            return false;
        }
        auto  line2 = *line1;
        auto& dest  = _areas[static_cast<size_t>(destination)];
        if (dest.add_line(line2) == false) {
            fl_alert("Target area has reached maximum number of lines!");
            return false;
        }
    }
    _area->delete_line(index);
    return true;
}
void Chart::print_to_postscript() {
    _printing = true;
    dlg::print("Print Chart", Chart::_CallbackPrinter, this, 1, 1, top_window());
    _printing = false;
    redraw();
}
void Chart::resize(int X, int Y, int W, int H) {
    if (_old.w() == W && _old.h() == H) {
        return;
    }
    Fl_Widget::resize(X, Y, W, H);
    _scroll->resize(X, Y + H - Fl::scrollbar_size(), W, Fl::scrollbar_size());
    _old = Fl_Rect(this);
    init();
}
bool Chart::save_csv() {
    if (_area == nullptr || _area->selected_line() == nullptr) {
        return false;
    }
    auto filename = util::to_string(fl_file_chooser("Save To CSV File", "All Files (*)\tCSV Files (*.csv)", ""));
    if (util::is_whitespace_or_empty(filename.c_str()) == true) {
        return false;
    }
    const auto* line = _area->selected_line();
    return ChartData::SaveCSV(line->data(), filename);
}
bool Chart::save_json() {
    auto filename = util::to_string(fl_file_chooser("Save To JSON File", "All Files (*)\tJSON Files (*.json)", ""));
    if (util::is_whitespace_or_empty(filename.c_str()) == true) {
        return false;
    }
    return save_json(filename);
}
bool Chart::save_json(std::string filename, double max_diff_high_low) const {
    auto wc  = WaitCursor();
    auto jsb = JSB();
    try {
        jsb << JSB::MakeObject();
            jsb << JSB::MakeObject("flw::chart");
                jsb << JSB::MakeNumber(Chart::VERSION, "version");
                jsb << JSB::MakeString(_label.c_str(), "label");
                jsb << JSB::MakeNumber(_tick_width, "tick_width");
                jsb << JSB::MakeString(ChartData::RangeToString(_date_range), "date_range");
                jsb << JSB::MakeBool(_labels, "labels");
                jsb << JSB::MakeBool(_horizontal, "horizontal");
                jsb << JSB::MakeBool(_vertical, "vertical");
            jsb.end();
            jsb << JSB::MakeObject("flw::chart_areas");
            for (size_t f = 0; f <= static_cast<int>(ChartArea::AREA::LAST); f++) {
                auto  perc_str = util::format("area%u", static_cast<unsigned>(f));
                auto  min_str  = util::format("min%u", static_cast<unsigned>(f));
                auto  max_str  = util::format("max%u", static_cast<unsigned>(f));
                auto& area     = _areas[f];
                auto  min      = area.clamp_min();
                auto  max      = area.clamp_max();
                jsb << JSB::MakeNumber(_areas[f].percent(), perc_str.c_str());
                if (min.has_value() == true) jsb << JSB::MakeNumber(min.value(), min_str.c_str());
                if (max.has_value() == true) jsb << JSB::MakeNumber(max.value(), max_str.c_str());
            }
            jsb.end();
            jsb << JSB::MakeArray("flw::chart_lines");
                for (auto& area : _areas) {
                    for (auto& line : area.lines()) {
                        if (line.size() > 0) {
                            jsb << JSB::MakeObject();
                                jsb << JSB::MakeNumber(static_cast<int>(area.area()), "area");
                                jsb << JSB::MakeString(line.label(), "label");
                                jsb << JSB::MakeString(line.type_to_string(), "type");
                                jsb << JSB::MakeNumber(line.align(), "align");
                                jsb << JSB::MakeNumber(line.color(), "color");
                                jsb << JSB::MakeNumber(line.width(), "width");
                                jsb << JSB::MakeBool(line.is_visible(), "visible");
                                jsb << JSB::MakeArray("yx");
                                for (const auto& data : line.data()) {
                                    jsb << JSB::MakeArrayInline();
                                        jsb << JSB::MakeString(data.date);
                                        if (fabs(data.close - data.low) > max_diff_high_low || fabs(data.close - data.high) > max_diff_high_low) {
                                            jsb << JSB::MakeNumber(data.high);
                                            jsb << JSB::MakeNumber(data.low);
                                        }
                                        jsb << JSB::MakeNumber(data.close);
                                        jsb.end();
                                }
                                jsb.end();
                            jsb.end();
                        }
                    }
                }
            jsb.end();
            jsb << JSB::MakeArray("flw::chart_block");
                for (const auto& data : _block_dates) {
                    jsb << JSB::MakeString(data.date);
                }
            jsb.end();
        auto js = jsb.encode();
        return File::Save(filename, js.c_str(), js.length());
    }
    catch(const std::string& e) {
        fl_alert("error: failed to encode json\n%s", e.c_str());
        return false;
    }
}
bool Chart::save_png() {
    return util::png_save("", top_window(), x() + 1,  y() + 1,  w() - 2,  h() - _scroll->h() - 1);
}
bool Chart::set_area_size(unsigned area1, unsigned area2, unsigned area3, unsigned area4, unsigned area5) {
    _area = nullptr;
    _areas[0].set_percent(100);
    _areas[1].set_percent(0);
    _areas[2].set_percent(0);
    _areas[3].set_percent(0);
    _areas[4].set_percent(0);
    if ((area1 == 0 || (area1 >= 10 && area1 <= 100)) &&
        (area2 == 0 || (area2 >= 10 && area2 <= 100)) &&
        (area3 == 0 || (area3 >= 10 && area3 <= 100)) &&
        (area4 == 0 || (area4 >= 10 && area4 <= 100)) &&
        (area5 == 0 || (area5 >= 10 && area5 <= 100)) &&
        area1 + area2 + area3 + area4 + area5 == 100) {
        if (area1 == 0) return false;
        if (area2 == 0 && (area3 > 0 || area4 > 0 || area5 > 0)) return false;
        if (area3 == 0 && (area4 > 0 || area5 > 0)) return false;
        if (area4 == 0 && area5 > 0) return false;
        _areas[0].set_percent(area1);
        _areas[1].set_percent(area2);
        _areas[2].set_percent(area3);
        _areas[3].set_percent(area4);
        _areas[4].set_percent(area5);
        return true;
    }
    return false;
}
void Chart::setup_area() {
    auto list = StringVector() = {"One", "Two equal", "Two (60%, 40%)", "Three equal", "Three (50%, 25%, 25%)", "Four Equal", "Four (40%, 20%, 20%, 20%)", "Five equal"};
    switch (dlg::choice("Select Number Of Chart Areas", list, 0, top_window())) {
    case 0:
        set_area_size(100);
        break;
    case 1:
        set_area_size(50, 50);
        break;
    case 2:
        set_area_size(60, 40);
        break;
    case 3:
        set_area_size(34, 33, 33);
        break;
    case 4:
        set_area_size(50, 25, 25);
        break;
    case 5:
        set_area_size(25, 25, 25, 25);
        break;
    case 6:
        set_area_size(40, 20, 20, 20);
        break;
    case 7:
        set_area_size(20, 20, 20, 20, 20);
        break;
    default:
        break;
    }
    init();
}
void Chart::setup_clamp(bool min) {
    if (_area == nullptr) {
        return;
    }
    auto area   = static_cast<int>(_area->area()) + 1;
    auto clamp  = (min == true) ? _area->clamp_min() : _area->clamp_max();
    auto input  = (clamp.has_value() == true) ? util::format("%f", clamp.value()) : "inf";
    auto info   = (min == true) ? util::format("Enter min clamp value or inf to disable for area %d", area) : util::format("Enter max clamp value or inf to disable for area %d", area);
    auto output = fl_input_str(16, "%s", input.c_str(), info.c_str());
    if (output == "") {
        return;
    }
    auto value = util::to_double(output);
    if (min == true) {
        _area->set_min_clamp(value);
    }
    else {
        _area->set_max_clamp(value);
    }
    init();
}
void Chart::setup_create_line() {
    if (_area == nullptr || _area->selected_line() == nullptr) {
        fl_alert("Area or line has not been selected!");
        return;
    }
    else if (_area->size() >= ChartArea::MAX_LINES) {
        fl_alert("Max line count reached!");
        return;
    }
    auto list = StringVector() = {
        "Moving average",
        "Exponential moving average",
        "Momentum",
        "Momentum + support line",
        "Standard deviation",
        "Stochastics",
        "Stochastics + support lines",
        "Relative Strength Index (RSI)",
        "Relative Strength Index (RSI) + support lines",
        "Average True Range (ATR)",
        "Day to week",
        "Day to month",
        "Modify",
        "Horizontal fixed line"
    };
    switch (dlg::choice("Select Formula", list, 0, top_window())) {
    case 0:
        create_line(ChartData::FORMULAS::MOVING_AVERAGE);
        break;
    case 1:
        create_line(ChartData::FORMULAS::EXP_MOVING_AVERAGE);
        break;
    case 2:
        create_line(ChartData::FORMULAS::MOMENTUM);
        break;
    case 3:
        create_line(ChartData::FORMULAS::MOMENTUM, true);
        break;
    case 4:
        create_line(ChartData::FORMULAS::STD_DEV);
        break;
    case 5:
        create_line(ChartData::FORMULAS::STOCHASTICS);
        break;
    case 6:
        create_line(ChartData::FORMULAS::STOCHASTICS, true);
        break;
    case 7:
        create_line(ChartData::FORMULAS::RSI);
        break;
    case 8:
        create_line(ChartData::FORMULAS::RSI, true);
        break;
    case 9:
        create_line(ChartData::FORMULAS::ATR);
        break;
    case 10:
        create_line(ChartData::FORMULAS::DAY_TO_WEEK);
        break;
    case 11:
        create_line(ChartData::FORMULAS::DAY_TO_MONTH);
        break;
    case 12:
        create_line(ChartData::FORMULAS::MODIFY);
        break;
    case 13:
        create_line(ChartData::FORMULAS::FIXED);
        break;
    default:
        break;
    }
}
void Chart::setup_date_range() {
    auto list = StringVector() = {"Day", "Weekday", "Friday", "Sunday", "Month", "Hour", "Minute", "Second"};
    auto sel  = dlg::choice("Select Date Range Type", list, static_cast<int>(_date_range), top_window());
    if (sel == -1) {
        return;
    }
    _date_range = static_cast<ChartData::RANGE>(sel);
    init_new_data();
}
void Chart::setup_delete_lines() {
    if (_area == nullptr || _area->size() == 0) {
        return;
    }
    auto list_labels = dlg::check("Delete Lines", _label_array(*_area, Chart::LABELTYPE::OFF), top_window());
    if (list_labels.size() == 0) {
        return;
    }
    for (size_t f = 0, e = 0; f < list_labels.size(); f++, e++) {
        if (list_labels[f][0] == '1') {
            if (_move_or_delete_line(_area, e, false) == true) {
                e--;
            }
        }
    }
    init_new_data();
}
void Chart::setup_label() {
    auto l = fl_input(40, "Enter label", _label.c_str());
    if (l != nullptr) {
        _label = l;
        init();
    }
}
void Chart::setup_line() {
    if (_area == nullptr || _area->selected_line() == nullptr) {
        return;
    }
    auto line = const_cast<ChartLine*>(_area->selected_line());
    auto ok   = ChartLineSetup(top_window(), *line).run();
    if (ok == false) {
        return;
    }
    init_new_data();
}
void Chart::setup_move_lines() {
    if (_area == nullptr || _area->size() == 0) {
        return;
    }
    auto list_labels = dlg::check("Move Lines", _label_array(*_area, Chart::LABELTYPE::OFF), top_window());
    if (list_labels.size() == 0) {
        return;
    }
    auto list_areas = StringVector() = {"Area 1", "Area 2", "Area 3", "Area 4", "Area 5"};
    auto area       = dlg::choice("Select Area", list_areas, 0, top_window());
    if (area < 0 || area > static_cast<int>(ChartArea::AREA::LAST)) {
        return;
    }
    for (size_t f = 0, e = 0; f < list_labels.size(); f++, e++) {
        if (list_labels[f][0] == '1') {
            if (_move_or_delete_line(_area, e, true, static_cast<ChartArea::AREA>(area)) == true) {
                e--;
            }
        }
    }
    init_new_data();
}
void Chart::setup_show_or_hide_lines() {
    if (_area == nullptr || _area->size() == 0) {
        return;
    }
    auto list = _label_array(*_area, Chart::LABELTYPE::VISIBLE);
    list = dlg::check("Show Or Hide Lines", list, top_window());
    if (list.size() == 0) {
        return;
    }
    auto f = 0;
    for (auto& line : _area->lines()) {
        const_cast<ChartLine&>(line).set_visible(list[f++][0] == '1');
    }
    init_new_data();
}
void Chart::setup_view_options() {
    _labels     = menu::item_value(_menu, _CHART_SHOW_LABELS);
    _vertical   = menu::item_value(_menu, _CHART_SHOW_VLINES);
    _horizontal = menu::item_value(_menu, _CHART_SHOW_HLINES);
    redraw();
}
void Chart::_show_menu() {
    if (_disable_menu == true) {
        return;
    }
    menu::enable_item(_menu, _CHART_ADD_CSV, false);
    menu::enable_item(_menu, _CHART_ADD_LINE, false);
    menu::enable_item(_menu, _CHART_SAVE_CSV, false);
    menu::enable_item(_menu, _CHART_SETUP_DELETE, false);
    menu::enable_item(_menu, _CHART_SETUP_LINE, false);
    menu::enable_item(_menu, _CHART_SETUP_MAX_CLAMP, false);
    menu::enable_item(_menu, _CHART_SETUP_MIN_CLAMP, false);
    menu::enable_item(_menu, _CHART_SETUP_MOVE, false);
    menu::enable_item(_menu, _CHART_SETUP_SHOW, false);
#ifdef DEBUG
    menu::enable_item(_menu, _CHART_DEBUG_LINE, false);
#endif
    menu::set_item(_menu, _CHART_SHOW_HLINES, _horizontal);
    menu::set_item(_menu, _CHART_SHOW_LABELS, _labels);
    menu::set_item(_menu, _CHART_SHOW_VLINES, _vertical);
    if (_area != nullptr) {
        menu::enable_item(_menu, _CHART_ADD_CSV, true);
        if (_area->size() > 0) {
            menu::enable_item(_menu, _CHART_SETUP_DELETE, true);
            menu::enable_item(_menu, _CHART_SETUP_MOVE, true);
            menu::enable_item(_menu, _CHART_SETUP_SHOW, true);
        }
        if (_area->selected_line() != nullptr && _area->selected_line()->size() > 0) {
            menu::enable_item(_menu, _CHART_ADD_LINE, true);
            menu::enable_item(_menu, _CHART_SAVE_CSV, true);
            menu::enable_item(_menu, _CHART_SETUP_LINE, true);
            menu::enable_item(_menu, _CHART_SETUP_MAX_CLAMP, true);
            menu::enable_item(_menu, _CHART_SETUP_MIN_CLAMP, true);
#ifdef DEBUG
            menu::enable_item(_menu, _CHART_DEBUG_LINE, true);
#endif
        }
    }
    _menu->popup();
}
void Chart::update_pref() {
    _menu->textfont(flw::PREF_FONT);
    _menu->textsize(flw::PREF_FONTSIZE);
}
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
    time_t    rawtime  = seconds;
    const tm* timeinfo = (utc == true) ? gmtime(&rawtime) : localtime(&rawtime);
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
    const char* space = strstr(string, " ");
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
        return Date::InvalidDate();
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
            return Date::InvalidDate();
        }
    }
    year  = val[0] * 1000 + val[1] * 100 + val[2] * 10 + val[3];
    month = val[5] == -1 ? val[4] : val[4] * 10 + val[5];
    day   = val[7] == -1 ? val[6] : val[6] * 10 + val[7];
    hour  = val[8] * 10 + val[9];
    min   = val[10] * 10 + val[11];
    sec   = val[12] * 10 + val[13];
    if (iso == false && us == true) {
        int tmp = month;
        month   = day;
        day     = tmp;
    }
    return Date(year, month, day, hour, min, sec);
}
static Date::DAY _date_weekday(int year, int month, int day) {
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
                return Date::DAY::INVALID;
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
            return Date::DAY::INVALID;
        }
        else {
            return (Date::DAY) start;
        }
    }
    return Date::DAY::INVALID;
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
Date::Date(bool utc) {
    int y, m, d, ho, mi, se;
    _date_from_time(::time(nullptr), utc, y, m, d, ho, mi, se);
    set(y, m, d, ho, mi, se);
}
Date::Date(const Date& other) {
    set(other);
}
Date::Date(Date&& other) {
    set(other);
}
Date::Date(int year, int month, int day, int hour, int min, int sec) {
    _year = _month = _day = 0;
    _hour = _min = _sec = 0;
    set(year, month, day, hour, min, sec);
}
Date& Date::operator=(const Date& date) {
    set(date);
    return *this;
}
Date& Date::operator=(Date&& date) {
    set(date);
    return *this;
}
bool Date::add_days(const int days) {
    if (days) {
        int daym = _date_days(_year, _month);
        if (daym > 0) {
            int inc = days > 0 ? 1 : -1;
            int y   = _year;
            int m   = _month;
            int d   = _day;
            for (int f = 0; f < abs(days); f++) {
                d += inc;
                if (inc < 0 && d == 0) {
                    m--;
                    if (m == 0) {
                        m = 12;
                        y--;
                        if (y < 1) {
                            return false;
                        }
                    }
                    d = _date_days(y, m);
                    if (d == 0) {
                        return false;
                    }
                }
                else if (inc > 0 && d > daym) {
                    d = 1;
                    m++;
                    if (m == 13) {
                        m = 1;
                        y++;
                        if (y > 9999) {
                            return false;
                        }
                    }
                    daym = _date_days(y, m);
                    if (daym == 0) {
                        return false;
                    }
                }
            }
            _year  = y;
            _month = m;
            _day   = d;
            return true;
        }
    }
    return false;
}
bool Date::add_months(const int months) {
    if (months) {
        int inc = months > 0 ? 1 : -1;
        int m   = _month;
        int y   = _year;
        for (int f = 0; f < abs(months); f++) {
            m += inc;
            if (m == 0) {
                m = 12;
                y--;
                if (y < 1) {
                    return false;
                }
            }
            else if (m > 12) {
                m = 1;
                y++;
                if (y > 9999) {
                    return false;
                }
            }
        }
        const int days = _date_days(y, m);
        if (days > 0) {
            _year  = y;
            _month = m;
            if (_day > days) {
                _day = days;
            }
            return true;
        }
    }
    return false;
}
bool Date::add_seconds(const int64_t seconds) {
    if (seconds) {
        int inc = seconds > 0 ? 1 : -1;
        int h   = _hour;
        int m   = _min;
        int s   = _sec;
        for (int64_t f = 0; f < llabs(seconds); f++) {
            s += inc;
            if (inc < 0 && s == -1) {
                m--;
                if (m == -1) {
                    m = 59;
                    h--;
                    if (h == -1) {
                        h = 23;
                        if (add_days(-1) == false) {
                            return false;
                        }
                    }
                }
                s = 59;
            }
            else if (inc > 0 && s == 60) {
                m++;
                if (m == 60) {
                    m = 0;
                    h++;
                    if (h == 24) {
                        h = 0;
                        if (add_days(1) == false) {
                            return false;
                        }
                    }
                }
                s = 0;
            }
        }
        _hour = h;
        _min  = m;
        _sec  = s;
        return true;
    }
    return false;
}
int Date::compare(const Date& other, Date::COMPARE flag) const {
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
    if (flag >= Date::COMPARE::YYYYMMDD) {
        if (_day < other._day) {
            return -1;
        }
        else if (_day > other._day) {
            return 1;
        }
    }
    if (flag >= Date::COMPARE::YYYYMMDDHH) {
        if (_hour < other._hour) {
            return -1;
        }
        else if (_hour > other._hour) {
            return 1;
        }
    }
    if (flag >= Date::COMPARE::YYYYMMDDHHMM) {
        if (_min < other._min) {
            return -1;
        }
        else if (_min > other._min) {
            return 1;
        }
    }
    if (flag >= Date::COMPARE::YYYYMMDDHHMMSS) {
        if (_sec < other._sec) {
            return -1;
        }
        else if (_sec > other._sec) {
            return 1;
        }
    }
    return 0;
}
bool Date::Compare(const Date& a, const Date& b) {
    return a.compare(b) < 0;
}
Date& Date::day(int day) {
    if (day > 0 && day <= _date_days(_year, _month)) {
        _day = day;
    }
    return *this;
}
Date& Date::day_last() {
    _day = month_days();
    return *this;
}
int Date::diff_days(const Date& date) const {
    Date d(date);
    int  res = 0;
    if (compare(d, Date::COMPARE::YYYYMMDD) < 0) {
        while (compare(d, Date::COMPARE::YYYYMMDD) != 0) {
            d.add_days(-1);
            res++;
        }
    }
    else if (compare(d, Date::COMPARE::YYYYMMDD) > 0) {
        while (compare(d, Date::COMPARE::YYYYMMDD) != 0) {
            d.add_days(1);
            res--;
        }
    }
    return res;
}
int Date::diff_months(const Date& date) const {
    Date d(date);
    int  res = 0;
    if (compare(d, Date::COMPARE::YYYYMM) < 0) {
        while (compare(d, Date::COMPARE::YYYYMM)) {
            d.add_months(-1);
            res++;
        }
    }
    else if (compare(d, Date::COMPARE::YYYYMM) > 0) {
        while (compare(d, Date::COMPARE::YYYYMM)) {
            d.add_months(1);
            res--;
        }
    }
    return res;
}
int Date::diff_seconds(const Date& date) const {
    int64_t unix1 = time();
    int64_t unix2 = date.time();
    if (unix1 >= 0 && unix2 >= 0) {
        return unix2 - unix1;
    }
    return 0;
}
std::string Date::format(Date::FORMAT format) const {
    char tmp[100];
    int  n = 0;
    if (format == Date::FORMAT::ISO_LONG) {
        n = snprintf(tmp, 100, "%04d-%02d-%02d", _year, _month, _day);
    }
    else if (format == Date::FORMAT::US) {
        n = snprintf(tmp, 100, "%d/%d/%04d", _month, _day, _year);
    }
    else if (format == Date::FORMAT::WORLD) {
        n = snprintf(tmp, 100, "%d/%d/%04d", _day, _month, _year);
    }
    else if (format == Date::FORMAT::NAME) {
        n = snprintf(tmp, 100, "%04d %s %d", _year, month_name_short(), _day);
    }
    else if (format == Date::FORMAT::NAME_LONG) {
        n = snprintf(tmp, 100, "%04d %s %d", _year, month_name(), _day);
    }
    else if (format == Date::FORMAT::NAME_TIME) {
        n = snprintf(tmp, 100, "%04d %s %d - %02d%02d%02d", _year, month_name_short(), _day, _hour, _min, _sec);
    }
    else if (format == Date::FORMAT::NAME_TIME_LONG) {
        n = snprintf(tmp, 100, "%04d %s %d - %02d:%02d:%02d", _year, month_name(), _day, _hour, _min, _sec);
    }
    else if (format == Date::FORMAT::YEAR_MONTH) {
        n = snprintf(tmp, 100, "%04d %s", _year, month_name_short());
    }
    else if (format == Date::FORMAT::YEAR_MONTH_LONG) {
        n = snprintf(tmp, 100, "%04d %s", _year, month_name());
    }
    else if (format == Date::FORMAT::ISO_TIME) {
        n = snprintf(tmp, 100, "%04d%02d%02d %02d%02d%02d", _year, _month, _day, _hour, _min, _sec);
    }
    else if (format == Date::FORMAT::ISO_TIME_LONG) {
        n = snprintf(tmp, 100, "%04d-%02d-%02d %02d:%02d:%02d", _year, _month, _day, _hour, _min, _sec);
    }
    else if (format == Date::FORMAT::TIME_LONG) {
        n = snprintf(tmp, 100, "%02d:%02d:%02d", _hour, _min, _sec);
    }
    else if (format == Date::FORMAT::TIME) {
        n = snprintf(tmp, 100, "%02d%02d%02d", _hour, _min, _sec);
    }
    else {
        n = snprintf(tmp, 100, "%04d%02d%02d", _year, _month, _day);
    }
    if (n < 0 || n >= 100) {
        *tmp = 0;
    }
    return tmp;
}
std::string Date::FormatSecToISO(int64_t seconds, bool utc) {
    const time_t rawtime  = (time_t) seconds;
    const tm*    timeinfo = (utc == true) ? gmtime(&rawtime) : localtime(&rawtime);
    char         buffer[100];
    if (timeinfo == nullptr) {
        return "";
    }
    snprintf(buffer, 100, "%04d-%02d-%02d %02d:%02d:%02d", timeinfo->tm_year + 1900, timeinfo->tm_mon + 1, timeinfo->tm_mday, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
    return buffer;
}
Date Date::FromString(const char* buffer, bool us) {
    if (buffer == nullptr) {
        return Date::InvalidDate();
    }
    else {
         return _date_parse(buffer, us);
    }
}
Date Date::FromTime(int64_t seconds, bool utc) {
    int y, m, d, ho, mi, se;
    _date_from_time(seconds, utc, y, m, d, ho, mi, se);
    return Date(y, m, d, ho, mi, se);
}
Date& Date::hour(int hour) {
    if (hour >= 0 && hour <= 23) {
        _hour = hour;
    }
    return *this;
}
Date Date::InvalidDate() {
    Date date;
    date._year = date._month = date._day = date._hour = date._min = date._sec = 0;
    return date;
}
bool Date::is_leapyear() const {
    return _date_is_leapyear(_year);
}
Date& Date::minute(int min) {
    if (min >= 0 && min <= 59) {
        _min = min;
    }
    return *this;
}
Date& Date::month(int month) {
    if (month >= 1 && month <= 12) {
        _month = month;
    }
    return *this;
}
int Date::month_days() const {
    return _date_days(_year, _month);
}
const char* Date::month_name() const {
    return _DATE_MONTHS[(int) _month];
}
const char* Date::month_name_short() const {
    return _DATE_MONTHS_SHORT[(int) _month];
}
void Date::print() const {
    auto string = format(Date::FORMAT::ISO_TIME_LONG);
    printf("Date| %s\n", string.c_str());
    fflush(stdout);
}
Date& Date::second(int sec) {
    if (sec >= 0 && sec <= 59) {
        _sec = sec;
    }
    return *this;
}
Date& Date::set(const Date& date) {
    _year  = date._year;
    _month = date._month;
    _day   = date._day;
    _hour  = date._hour;
    _min   = date._min;
    _sec   = date._sec;
    return *this;
}
Date& Date::set(int year, int month, int day, int hour, int min, int sec) {
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
int64_t Date::time() const {
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
int Date::week() const {
    Date::DAY wday  = _date_weekday(_year, _month, _day);
    Date::DAY wday1 = _date_weekday(_year, 1, 1);
    if (wday != Date::DAY::INVALID && wday1 != Date::DAY::INVALID) {
        auto w     = 0;
        auto y1    = _year - 1;
        auto leap  = _date_is_leapyear(_year);
        auto leap1 = _date_is_leapyear(y1);
        auto yday  = yearday();
        if (yday <= (8 - (int) wday1) && wday1 > Date::DAY::THURSDAY) {
            if (wday1 == Date::DAY::FRIDAY || (wday1 == Date::DAY::SATURDAY && leap1)) {
                w = 53;
            }
            else {
                w = 52;
            }
        }
        else {
            auto days = leap ? 366 : 365;
            if ((days - yday) < (4 - (int) wday)) {
                w = 1;
            }
            else {
                days = yday + (7 - (int) wday) + ((int) wday1 - 1);
                days = days / 7;
                if (wday1 > Date::DAY::THURSDAY) {
                    days--;
                }
                w = days;
            }
        }
        if (w > 0 && w < 54) {
            return w;
        }
    }
    return 0;
}
Date::DAY Date::weekday() const {
    return _date_weekday(_year, _month, _day);
}
Date& Date::weekday(Date::DAY day) {
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
const char* Date::weekday_name() const {
    return _DATE_WEEKDAYS[(int) _date_weekday(_year, _month, _day)];
}
const char* Date::weekday_name_short() const {
    return _DATE_WEEKDAYS_SHORT[(int) _date_weekday(_year, _month, _day)];
}
Date& Date::year(int year) {
    if (year >= 1 && year <= 9999) {
        _year = year;
    }
    return *this;
}
int Date::yearday() const {
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
}
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Repeat_Button.H>
#include <FL/Fl_Return_Button.H>
#include <FL/fl_draw.H>
#include <time.h>
namespace flw {
class _DateChooserCanvas : public Fl_Widget {
    Date                        _date[7][8];
    char                        _text[7][8][30];
    int                         _col;
    int                         _row;
public:
    _DateChooserCanvas() :
    Fl_Widget(0, 0, 0, 0, 0) {
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
                        fg = fl_contrast(FL_FOREGROUND_COLOR, bg);
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
flw::DateChooser::DateChooser(int X, int Y, int W, int H, const char* l) :
GridGroup(X, Y, W, H, l) {
    end();
    _b1          = new Fl_Repeat_Button(0, 0, 0, 0, "@<<");
    _b2          = new Fl_Repeat_Button(0, 0, 0, 0, "@<");
    _b3          = new Fl_Repeat_Button(0, 0, 0, 0, "@>");
    _b4          = new Fl_Repeat_Button(0, 0, 0, 0, "@>>");
    _b5          = new Fl_Button(0, 0, 0, 0, "@refresh");
    _b6          = new Fl_Repeat_Button(0, 0, 0, 0, "@|<");
    _b7          = new Fl_Repeat_Button(0, 0, 0, 0, "@>|");
    _buttons     = new ToolGroup();
    _canvas      = new _DateChooserCanvas();
    _month_label = new Fl_Box(0, 0, 0, 0, "");
    Date date;
    set(date);
    _set_label();
    _buttons->add(_b6);
    _buttons->add(_b1);
    _buttons->add(_b2);
    _buttons->add(_b5, 8);
    _buttons->add(_b3);
    _buttons->add(_b4);
    _buttons->add(_b7);
    _buttons->expand_last(true);
    add(_buttons,     0, 0, 0, 4);
    add(_month_label, 0, 4, 0, 4);
    add(_canvas,      0, 8, 0, 0);
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
    resizable(this);
    do_layout();
}
void flw::DateChooser::_Callback(Fl_Widget* w, void* o) {
    auto dc = static_cast<DateChooser*>(o);
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
    Date&                       _value;
    DateChooser*                _date_chooser;
    Fl_Button*                  _cancel;
    Fl_Button*                  _ok;
    GridGroup*                  _grid;
    bool                        _res;
public:
    _DateChooserDlg(const char* title, Date& date) :
    Fl_Double_Window(0, 0, 0, 0),
    _value(date) {
        end();
        _cancel       = new Fl_Button(0, 0, 0, 0, "&Cancel");
        _date_chooser = new DateChooser(0, 0, 0, 0, "DateCHooser");
        _grid         = new GridGroup();
        _ok           = new Fl_Return_Button(0, 0, 0, 0, "&Ok");
        _res          = false;
        _grid->add(_date_chooser,   0,   0,   0,  -6);
        _grid->add(_cancel,       -34,  -5,  16,   4);
        _grid->add(_ok,           -17,  -5,  16,   4);
        add(_grid);
        _cancel->callback(Callback, this);
        _date_chooser->focus();
        _date_chooser->set(_value);
        _ok->callback(Callback, this);
        util::labelfont(this);
        callback(Callback, this);
        copy_label(title);
        resizable(this);
        size(flw::PREF_FONTSIZE * 33, flw::PREF_FONTSIZE * 21);
        size_range(flw::PREF_FONTSIZE * 22, flw::PREF_FONTSIZE * 14);
        set_modal();
    }
    static void Callback(Fl_Widget* w, void* o) {
        auto self = static_cast<_DateChooserDlg*>(o);
        if (w == self) {
            ;
        }
        else if (w == self->_cancel) {
            self->hide();
        }
        else if (w == self->_ok) {
            self->hide();
            self->_res = true;
        }
    }
    void resize(int X, int Y, int W, int H) override {
        Fl_Double_Window::resize(X, Y, W, H);
        _grid->resize(0, 0, W, H);
    }
    bool run(Fl_Window* parent) {
        util::center_window(this, parent);
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
#include <FL/Fl_Hor_Slider.H>
#include <FL/Fl_Hor_Value_Slider.H>
#include <FL/Fl_Output.H>
#include <FL/Fl_Return_Button.H>
#include <FL/Fl_Scroll.H>
#include <FL/Fl_Secret_Input.H>
#include <FL/Fl_Text_Editor.H>
#include <FL/Fl_Tooltip.H>
namespace flw {
namespace theme {
void _load_default();
void _load_gleam();
void _load_gleam_blue();
void _load_gleam_dark();
void _load_gleam_tan();
void _load_gtk();
void _load_gtk_blue();
void _load_gtk_dark();
void _load_gtk_tan();
void _load_oxy();
void _load_oxy_blue();
void _load_oxy_tan();
void _load_plastic();
void _load_plastic_tan();
void _scrollbar();
}
namespace dlg {
static void _init_printer_formats(Fl_Choice* format, Fl_Choice* layout) {
    format->add("A0 format");
    format->add("A1 format");
    format->add("A2 format");
    format->add("A3 format");
    format->add("A4 format");
    format->add("A5 format");
    format->add("A6 format");
    format->add("A7 format");
    format->add("A8 format");
    format->add("A9 format");
    format->add("B0 format");
    format->add("B1 format");
    format->add("B2 format");
    format->add("B3 format");
    format->add("B4 format");
    format->add("B5 format");
    format->add("B6 format");
    format->add("B7 format");
    format->add("B8 format");
    format->add("B9 format");
    format->add("Executive format");
    format->add("Folio format");
    format->add("Ledger format");
    format->add("Legal format");
    format->add("Letter format");
    format->add("Tabloid format");
    format->tooltip("Select paper format.");
    format->value(4);
    layout->add("Portrait");
    layout->add("Landscape");
    layout->tooltip("Select paper layout.");
    layout->value(0);
}
void center_message_dialog() {
    int X, Y, W, H;
    Fl::screen_xywh(X, Y, W, H);
    fl_message_position(W / 2, H / 2, 1);
}
bool font(Fl_Font& font, Fl_Fontsize& fontsize, std::string& fontname, bool limit_to_default) {
    auto dlg = dlg::FontDialog(font, fontsize, "Select Font", limit_to_default);
    if (dlg.run() == false) {
        return false;
    }
    font     = dlg.font();
    fontsize = dlg.fontsize();
    fontname = dlg.fontname();
    return true;
}
void panic(std::string message) {
    fl_alert("panic! I have to quit\n%s", message.c_str());
    exit(1);
}
class _DlgCheck : public Fl_Double_Window {
    Fl_Button*                  _all;
    Fl_Button*                  _cancel;
    Fl_Button*                  _close;
    Fl_Button*                  _invert;
    Fl_Button*                  _none;
    Fl_Scroll*                  _scroll;
    GridGroup*                  _grid;
    const StringVector&         _labels;
    WidgetVector                _checkbuttons;
    bool                        _ret;
public:
    _DlgCheck(const char* title, Fl_Window* parent, const StringVector& strings) :
    Fl_Double_Window(0, 0, flw::PREF_FONTSIZE * 36, flw::PREF_FONTSIZE * 20),
    _labels(strings) {
        end();
        _ret    = false;
        _all    = new Fl_Button(0, 0, 0, 0, "All on");
        _cancel = new Fl_Button(0, 0, 0, 0, "&Cancel");
        _close  = new Fl_Return_Button(0, 0, 0, 0, "&Ok");
        _grid   = new GridGroup(0, 0, w(), h());
        _invert = new Fl_Button(0, 0, 0, 0, "Invert");
        _none   = new Fl_Button(0, 0, 0, 0, "All off");
        _scroll = new Fl_Scroll(0, 0, 0, 0);
        _grid->add(_scroll,   1,  1, -1, -7);
        _grid->add(_invert, -67, -5, 10,  4);
        _grid->add(_none,   -56, -5, 10,  4);
        _grid->add(_all,    -45, -5, 10,  4);
        _grid->add(_cancel, -34, -5, 16,  4);
        _grid->add(_close,  -17, -5, 16,  4);
        add(_grid);
        for (auto& l : _labels) {
            auto b = new Fl_Check_Button(0, 0, 0, 0, l.c_str() + 1);
            b->value(*l.c_str() == '1');
            _checkbuttons.push_back(b);
            _scroll->add(b);
        }
        _all->callback(_DlgCheck::Callback, this);
        _cancel->callback(_DlgCheck::Callback, this);
        _close->callback(_DlgCheck::Callback, this);
        _invert->callback(_DlgCheck::Callback, this);
        _none->callback(_DlgCheck::Callback, this);
        _scroll->box(FL_BORDER_BOX);
        util::labelfont(this);
        callback(_DlgCheck::Callback, this);
        copy_label(title);
        set_modal();
        resizable(_grid);
        size_range(flw::PREF_FONTSIZE * 36, flw::PREF_FONTSIZE * 12);
        util::center_window(this, parent);
    }
    static void Callback(Fl_Widget* w, void* o) {
        auto self = static_cast<_DlgCheck*>(o);
        if (w == self || w == self->_cancel) {
            self->hide();
        }
        else if (w == self->_close) {
            self->_ret = true;
            self->hide();
        }
        else if (w == self->_all) {
            for (auto b : self->_checkbuttons) {
                static_cast<Fl_Check_Button*>(b)->value(1);
            }
        }
        else if (w == self->_none) {
            for (auto b : self->_checkbuttons) {
                static_cast<Fl_Check_Button*>(b)->value(0);
            }
        }
        else if (w == self->_invert) {
            for (auto b : self->_checkbuttons) {
                static_cast<Fl_Check_Button*>(b)->value(static_cast<Fl_Check_Button*>(b)->value() ? 0 : 1);
            }
        }
    }
    void resize(int X, int Y, int W, int H) override {
        Fl_Double_Window::resize(X, Y, W, H);
        _grid->resize(0, 0, W, H);
        auto y = _scroll->y() + 4;
        for (auto b : _checkbuttons) {
            b->resize(_scroll->x() + 4, y, _scroll->w() - Fl::scrollbar_size() - 8, flw::PREF_FONTSIZE * 2);
            y += flw::PREF_FONTSIZE * 2;
        }
    }
    StringVector run() {
        StringVector res;
        show();
        while (visible() != 0) {
            Fl::wait();
            Fl::flush();
        }
        if (_ret == true) {
            for (auto b : _checkbuttons) {
                res.push_back(std::string((static_cast<Fl_Check_Button*>(b)->value() == 0) ? "0" : "1") + b->label());
            }
        }
        return res;
    }
};
StringVector check(std::string title, const StringVector& list, Fl_Window* parent) {
    _DlgCheck dlg(title.c_str(), parent, list);
    return dlg.run();
}
class _DlgChoice : public Fl_Double_Window {
    Fl_Button*                  _cancel;
    Fl_Button*                  _close;
    Fl_Choice*                  _choice;
    GridGroup*                  _grid;
    int                         _ret;
public:
    _DlgChoice(const char* title, Fl_Window* parent, const StringVector& strings, int selected) :
    Fl_Double_Window(0, 0, flw::PREF_FONTSIZE * 30, flw::PREF_FONTSIZE * 6) {
        end();
        _ret    = -1;
        _cancel = new Fl_Button(0, 0, 0, 0, "&Cancel");
        _choice = new Fl_Choice(0, 0, 0, 0);
        _close  = new Fl_Return_Button(0, 0, 0, 0, "&Ok");
        _grid   = new GridGroup(0, 0, w(), h());
        _grid->add(_choice,   1,  1, -1,  4);
        _grid->add(_cancel, -34, -5, 16,  4);
        _grid->add(_close,  -17, -5, 16,  4);
        add(_grid);
        for (const auto& string : strings) {
            _choice->add(string.c_str());
        }
        _cancel->callback(_DlgChoice::Callback, this);
        _choice->textfont(flw::PREF_FONT);
        _choice->textsize(flw::PREF_FONTSIZE);
        _choice->value(selected);
        _close->callback(_DlgChoice::Callback, this);
        util::labelfont(this);
        callback(_DlgChoice::Callback, this);
        copy_label(title);
        set_modal();
        resizable(_grid);
        size_range(flw::PREF_FONTSIZE * 30, flw::PREF_FONTSIZE * 6);
        util::center_window(this, parent);
        _grid->do_layout();
    }
    static void Callback(Fl_Widget* w, void* o) {
        auto self = static_cast<_DlgChoice*>(o);
        if (w == self || w == self->_cancel) {
            self->hide();
        }
        else if (w == self->_close) {
            self->_ret = self->_choice->value();
            self->hide();
        }
    }
    int run() {
        show();
        while (visible() != 0) {
            Fl::wait();
            Fl::flush();
        }
        return _ret;
    }
};
int choice(std::string title, const StringVector& list, int selected, Fl_Window* parent) {
    _DlgChoice dlg(title.c_str(), parent, list, selected);
    return dlg.run();
}
class _DlgHtml  : public Fl_Double_Window {
    Fl_Help_View*               _html;
    Fl_Return_Button*           _close;
    GridGroup*                  _grid;
public:
    _DlgHtml(const char* title, const char* text, Fl_Window* parent, int W, int H) :
    Fl_Double_Window(0, 0, flw::PREF_FONTSIZE * W,flw::PREF_FONTSIZE * H) {
        end();
        _close = new Fl_Return_Button(0, 0, 0, 0, "&Close");
        _grid  = new GridGroup(0, 0, w(), h());
        _html  = new Fl_Help_View(0, 0, 0, 0);
        _grid->add(_html,    1,  1, -1, -6);
        _grid->add(_close, -17, -5, 16,  4);
        add(_grid);
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
        resizable(_grid);
        util::center_window(this, parent);
        _grid->do_layout();
    }
    static void Callback(Fl_Widget* w, void* o) {
        auto self = static_cast<_DlgHtml*>(o);
        if (w == self || w == self->_close) {
            self->hide();
        }
    }
    void run() {
        show();
        while (visible() != 0) {
            Fl::wait();
            Fl::flush();
        }
    }
};
void html(std::string title, const std::string& text, Fl_Window* parent, int W, int H) {
    _DlgHtml dlg(title.c_str(), text.c_str(), parent, W, H);
    dlg.run();
}
class _DlgList : public Fl_Double_Window {
    Fl_Return_Button*           _close;
    GridGroup*                  _grid;
    ScrollBrowser*              _list;
public:
    _DlgList(const char* title, const StringVector& list, std::string file, Fl_Window* parent = nullptr, bool fixed_font = false, int W = 50, int H = 20) :
    Fl_Double_Window(0, 0, (fixed_font ? flw::PREF_FIXED_FONTSIZE : flw::PREF_FONTSIZE) * W, (fixed_font ? flw::PREF_FIXED_FONTSIZE : flw::PREF_FONTSIZE) * H) {
        end();
        _close = new Fl_Return_Button(0, 0, 0, 0, "&Close");
        _grid  = new GridGroup(0, 0, w(), h());
        _list  = new ScrollBrowser();
        _grid->add(_list,     1,   1,  -1,  -6);
        _grid->add(_close,  -17,  -5,  16,   4);
        add(_grid);
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
        resizable(_grid);
        util::center_window(this, parent);
        _grid->do_layout();
        if (list.size() > 0) {
            for (const auto& s : list) {
                _list->add(s.c_str());
            }
        }
        else if (file != "") {
            _list->load(file.c_str());
        }
    }
    static void Callback(Fl_Widget* w, void* o) {
        auto self = static_cast<_DlgList*>(o);
        if (w == self || w == self->_close) {
            self->hide();
        }
    }
    void run() {
        show();
        while (visible() != 0) {
            Fl::wait();
            Fl::flush();
        }
    }
};
void list(std::string title, const StringVector& list, Fl_Window* parent, bool fixed_font, int W, int H) {
    _DlgList dlg(title.c_str(), list, "", parent, fixed_font, W, H);
    dlg.run();
}
void list(std::string title, const std::string& list, Fl_Window* parent, bool fixed_font, int W, int H) {
    auto list2 = util::split_string( list, "\n");
    _DlgList dlg(title.c_str(), list2, "", parent, fixed_font, W, H);
    dlg.run();
}
void list_file(std::string title, std::string file, Fl_Window* parent, bool fixed_font, int W, int H) {
    _DlgList dlg(title.c_str(), flw::StringVector(), file, parent, fixed_font, W, H);
    dlg.run();
}
const char* PASSWORD_CANCEL = "&Cancel";
const char* PASSWORD_OK     = "&Ok";
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
    GridGroup*                  _grid;
    _DlgPassword::TYPE          _mode;
    bool                        _ret;
public:
    _DlgPassword(const char* title, Fl_Window* parent, _DlgPassword::TYPE mode) :
    Fl_Double_Window(0, 0, 10, 10) {
        end();
        _browse    = new Fl_Button(0, 0, 0, 0, "&Browse");
        _cancel    = new Fl_Button(0, 0, 0, 0, PASSWORD_CANCEL);
        _close     = new Fl_Return_Button(0, 0, 0, 0, PASSWORD_OK);
        _file      = new Fl_Output(0, 0, 0, 0, "Key file");
        _grid      = new GridGroup(0, 0, w(), h());
        _password1 = new Fl_Secret_Input(0, 0, 0, 0, "Password");
        _password2 = new Fl_Secret_Input(0, 0, 0, 0, "Enter password again");
        _mode      = mode;
        _ret       = false;
        _grid->add(_password1,  1,   3,  -1,  4);
        _grid->add(_password2,  1,  10,  -1,  4);
        _grid->add(_file,       1,  17,  -1,  4);
        _grid->add(_browse,   -51,  -5,  16,  4);
        _grid->add(_cancel,   -34,  -5,  16,  4);
        _grid->add(_close,    -17,  -5,  16,  4);
        add(_grid);
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
        auto W = flw::PREF_FONTSIZE * 35;
        auto H = flw::PREF_FONTSIZE * 13.5;
        if (_mode == _DlgPassword::TYPE::ASK_PASSWORD) {
            _password2->hide();
            _browse->hide();
            _file->hide();
            H = flw::PREF_FONTSIZE * 6.5;
        }
        else if (_mode == _DlgPassword::TYPE::CONFIRM_PASSWORD) {
            _browse->hide();
            _file->hide();
            H = flw::PREF_FONTSIZE * 10;
        }
        else if (_mode == _DlgPassword::TYPE::ASK_PASSWORD_AND_KEYFILE) {
            _password2->hide();
            _grid->resize(_file, 1, 10, -1, 4);
            H = flw::PREF_FONTSIZE * 10;
        }
        resizable(_grid);
        util::labelfont(this);
        callback(_DlgPassword::Callback, this);
        label(title);
        size(W, H);
        size_range(W, H);
        set_modal();
        util::center_window(this, parent);
        _grid->do_layout();
    }
    static void Callback(Fl_Widget* w, void* o) {
        auto self = static_cast<_DlgPassword*>(o);
        if (w == self) {
            ;
        }
        else if (w == self->_password1) {
            self->check();
        }
        else if (w == self->_password2) {
            self->check();
        }
        else if (w == self->_browse) {
            auto filename = fl_file_chooser("Select Key File", nullptr, nullptr, 0);
            if (filename) {
                self->_file->value(filename);
            }
            else {
                self->_file->value("");
            }
        }
        else if (w == self->_cancel) {
            self->_ret = false;
            self->hide();
        }
        else if (w == self->_close) {
            self->_ret = true;
            self->hide();
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
        util::zero_memory(const_cast<char*>(_password1->value()), strlen(_password1->value()));
        util::zero_memory(const_cast<char*>(_password2->value()), strlen(_password2->value()));
        util::zero_memory(const_cast<char*>(_file->value()), strlen(_file->value()));
        return _ret;
    }
};
bool password1(std::string title, std::string& password, Fl_Window* parent) {
    std::string file;
    _DlgPassword dlg(title.c_str(), parent, _DlgPassword::TYPE::ASK_PASSWORD);
    return dlg.run(password, file);
}
bool password2(std::string title, std::string& password, Fl_Window* parent) {
    std::string file;
    _DlgPassword dlg(title.c_str(), parent, _DlgPassword::TYPE::CONFIRM_PASSWORD);
    return dlg.run(password, file);
}
bool password3(std::string title, std::string& password, std::string& file, Fl_Window* parent) {
    _DlgPassword dlg(title.c_str(), parent, _DlgPassword::TYPE::ASK_PASSWORD_AND_KEYFILE);
    return dlg.run(password, file);
}
bool password4(std::string title, std::string& password, std::string& file, Fl_Window* parent) {
    _DlgPassword dlg(title.c_str(), parent, _DlgPassword::TYPE::CONFIRM_PASSWORD_AND_KEYFILE);
    return dlg.run(password, file);
}
class _DlgPrint : public Fl_Double_Window {
    Fl_Button*                  _close;
    Fl_Button*                  _file;
    Fl_Button*                  _print;
    Fl_Choice*                  _format;
    Fl_Choice*                  _layout;
    Fl_Hor_Slider*              _from;
    Fl_Hor_Slider*              _to;
    GridGroup*                  _grid;
    PrintCallback               _cb;
    void*                       _data;
public:
    _DlgPrint(std::string title, Fl_Window* parent, PrintCallback cb, void* data, int from, int to) :
    Fl_Double_Window(0, 0, flw::PREF_FONTSIZE * 34, flw::PREF_FONTSIZE * 18) {
        end();
        _close  = new Fl_Button(0, 0, 0, 0, "&Close");
        _file   = new Fl_Button(0, 0, 0, 0, "output.ps");
        _format = new Fl_Choice(0, 0, 0, 0);
        _from   = new Fl_Hor_Slider(0, 0, 0, 0);
        _grid   = new GridGroup(0, 0, w(), h());
        _layout = new Fl_Choice(0, 0, 0, 0);
        _print  = new Fl_Button(0, 0, 0, 0, "&Print");
        _to     = new Fl_Hor_Slider(0, 0, 0, 0);
        _cb     = cb;
        _data   = data;
        _grid->add(_from,     1,   3,  -1,   4);
        _grid->add(_to,       1,  10,  -1,   4);
        _grid->add(_format,   1,  15,  -1,   4);
        _grid->add(_layout,   1,  20,  -1,   4);
        _grid->add(_file,     1,  25,  -1,   4);
        _grid->add(_print,  -34,  -5,  16,   4);
        _grid->add(_close,  -17,  -5,  16,   4);
        add(_grid);
        util::labelfont(this);
        _close->callback(_DlgPrint::Callback, this);
        _file->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
        _file->callback(_DlgPrint::Callback, this);
        _file->tooltip("Select output PostScript file.");
        _from->align(FL_ALIGN_TOP | FL_ALIGN_LEFT);
        _from->callback(_DlgPrint::Callback, this);
        _from->color(FL_BACKGROUND2_COLOR);
        _from->range(from, to);
        _from->precision(0);
        _from->value(from);
        _from->tooltip("Start page number.");
        _print->callback(_DlgPrint::Callback, this);
        _to->align(FL_ALIGN_TOP | FL_ALIGN_LEFT);
        _to->callback(_DlgPrint::Callback, this);
        _to->color(FL_BACKGROUND2_COLOR);
        _to->range(from, to);
        _to->precision(0);
        _to->value(to);
        _to->tooltip("End page number.");
        if (from < 1 || from > to) {
            _from->deactivate();
            _from->range(0, 1);
            _from->value(0);
            _to->deactivate();
            _to->range(0, 1);
            _to->value(0);
        }
        else if (from == to) {
            _from->deactivate();
            _to->deactivate();
        }
        dlg::_init_printer_formats(_format, _layout);
        _DlgPrint::Callback(_from, this);
        _DlgPrint::Callback(_to, this);
        callback(_DlgPrint::Callback, this);
        copy_label(title.c_str());
        size_range(flw::PREF_FONTSIZE * 34, flw::PREF_FONTSIZE * 18);
        set_modal();
        resizable(_grid);
        util::center_window(this, parent);
        _grid->do_layout();
    }
    static void Callback(Fl_Widget* w, void* o) {
        auto self = static_cast<_DlgPrint*>(o);
        if (w == self) {
            self->hide();
        }
        else if (w == self->_close) {
            self->hide();
        }
        else if (w == self->_file) {
            auto filename = fl_file_chooser("Save To PostScript File", "PostScript Files (*.ps)\tAll Files (*)", self->_file->label());
            if (filename != nullptr) {
                self->_file->copy_label(filename);
            }
        }
        else if (w == self->_from || w == self->_to) {
            auto l = util::format("%s page: %d", (w == self->_from) ? "From" : "To", (int) static_cast<Fl_Hor_Slider*>(w)->value());
            w->copy_label(l.c_str());
            self->redraw();
        }
        else if (w == self->_print) {
            self->print();
        }
    }
    void print() {
        auto from   = static_cast<int>(_from->value());
        auto to     = static_cast<int>(_to->value());
        auto format = static_cast<Fl_Paged_Device::Page_Format>(_format->value());
        auto layout = (_layout->value() == 0) ? Fl_Paged_Device::Page_Layout::PORTRAIT : Fl_Paged_Device::Page_Layout::LANDSCAPE;
        auto file   = _file->label();
        auto err    = (from == 0) ? util::print(file, format, layout, _cb, _data) : util::print(file, format, layout, _cb, _data, from, to);
        if (err != "") {
            fl_alert("Printing failed!\n%s", err.c_str());
            return;
        }
        hide();
    }
    void run() {
        show();
        while (visible() != 0) {
            Fl::wait();
            Fl::flush();
        }
    }
};
void print(std::string title, PrintCallback cb, void* data, int from, int to, Fl_Window* parent) {
    _DlgPrint dlg(title, parent, cb, data, from, to);
    dlg.run();
}
class _DlgPrintText : public Fl_Double_Window {
    Fl_Box*                     _label;
    Fl_Button*                  _close;
    Fl_Button*                  _file;
    Fl_Button*                  _fonts;
    Fl_Button*                  _print;
    Fl_Check_Button*            _border;
    Fl_Check_Button*            _wrap;
    Fl_Choice*                  _align;
    Fl_Choice*                  _format;
    Fl_Choice*                  _layout;
    Fl_Font                     _font;
    Fl_Fontsize                 _fontsize;
    Fl_Hor_Slider*              _line;
    Fl_Hor_Slider*              _tab;
    GridGroup*                  _grid;
    bool                        _ret;
    const StringVector&         _text;
    std::string                 _label2;
public:
    _DlgPrintText(std::string title, Fl_Window* parent, const StringVector& text) :
    Fl_Double_Window(0, 0, flw::PREF_FONTSIZE * 34, flw::PREF_FONTSIZE * 35),
    _text(text) {
        end();
        _align    = new Fl_Choice(0, 0, 0, 0);
        _border   = new Fl_Check_Button(0, 0, 0, 0, "Print border");
        _close    = new Fl_Button(0, 0, 0, 0, "&Close");
        _file     = new Fl_Button(0, 0, 0, 0, "output.ps");
        _fonts    = new Fl_Button(0, 0, 0, 0, "Courier - 14");
        _format   = new Fl_Choice(0, 0, 0, 0);
        _grid     = new GridGroup(0, 0, w(), h());
        _label    = new Fl_Box(0, 0, 0, 0);
        _layout   = new Fl_Choice(0, 0, 0, 0);
        _line     = new Fl_Hor_Slider(0, 0, 0, 0);
        _print    = new Fl_Button(0, 0, 0, 0, "&Print");
        _tab      = new Fl_Hor_Slider(0, 0, 0, 0);
        _wrap     = new Fl_Check_Button(0, 0, 0, 0, "Wrap lines");
        _ret      = false;
        _font     = FL_COURIER;
        _fontsize = 14;
        _grid->add(_border,   1,   1,  -1,   4);
        _grid->add(_wrap,     1,   6,  -1,   4);
        _grid->add(_line,     1,  13,  -1,   4);
        _grid->add(_tab,      1,  20,  -1,   4);
        _grid->add(_format,   1,  25,  -1,   4);
        _grid->add(_layout,   1,  30,  -1,   4);
        _grid->add(_align,    1,  35,  -1,   4);
        _grid->add(_fonts,    1,  40,  -1,   4);
        _grid->add(_file,     1,  45,  -1,   4);
        _grid->add(_label,    1,  50,  -1,   13);
        _grid->add(_print,  -34,  -5,  16,   4);
        _grid->add(_close,  -17,  -5,  16,   4);
        add(_grid);
        util::labelfont(this);
        _align->add("Left align");
        _align->add("Center align");
        _align->add("Right align");
        _align->tooltip("Line numbers are only used for left aligned text.");
        _align->value(0);
        _border->tooltip("Print line border around the print area.");
        _close->callback(_DlgPrintText::Callback, this);
        _file->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
        _file->callback(_DlgPrintText::Callback, this);
        _file->tooltip("Select output PostScript file.");
        _fonts->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
        _fonts->callback(_DlgPrintText::Callback, this);
        _fonts->tooltip("Select font to use.");
        _label->align(FL_ALIGN_TOP | FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
        _label->box(FL_BORDER_BOX);
        _label->box(FL_THIN_DOWN_BOX);
        _line->align(FL_ALIGN_TOP | FL_ALIGN_LEFT);
        _line->callback(_DlgPrintText::Callback, this);
        _line->color(FL_BACKGROUND2_COLOR);
        _line->range(0, 6);
        _line->precision(0);
        _line->value(0);
        _line->tooltip("Set minimum line number width.\nSet to 0 to disable.");
        _print->callback(_DlgPrintText::Callback, this);
        _tab->align(FL_ALIGN_TOP | FL_ALIGN_LEFT);
        _tab->callback(_DlgPrintText::Callback, this);
        _tab->color(FL_BACKGROUND2_COLOR);
        _tab->range(0, 16);
        _tab->precision(0);
        _tab->value(0);
        _tab->tooltip("Replace tabs with spaces.\nSet to 0 to disable.");
        _wrap->tooltip("Wrap long lines or they will be clipped.");
        dlg::_init_printer_formats(_format, _layout);
        _DlgPrintText::Callback(_line, this);
        _DlgPrintText::Callback(_tab, this);
        callback(_DlgPrintText::Callback, this);
        copy_label(title.c_str());
        size_range(flw::PREF_FONTSIZE * 34, flw::PREF_FONTSIZE * 35);
        set_modal();
        resizable(_grid);
        util::center_window(this, parent);
        _grid->do_layout();
        set_label();
    }
    static void Callback(Fl_Widget* w, void* o) {
        auto self = static_cast<_DlgPrintText*>(o);
        if (w == self) {
            self->hide();
        }
        else if (w == self->_close) {
            self->hide();
        }
        else if (w == self->_file) {
            auto filename = fl_file_chooser("Save To PostScript File", "PostScript Files (*.ps)\tAll Files (*)", self->_file->label());
            if (filename != nullptr) {
                self->_file->copy_label(filename);
            }
        }
        else if (w == self->_fonts) {
            auto dlg = FontDialog(self->_font, self->_fontsize, "Select Print Font", true);
            if (dlg.run() == true) {
                auto l = util::format("%s - %d", dlg.fontname().c_str(), dlg.fontsize());
                self->_fonts->copy_label(l.c_str());
                self->_font     = dlg.font();
                self->_fontsize = dlg.fontsize();
            }
        }
        else if (w == self->_line) {
            auto l = util::format("Line number: %d", (int) self->_line->value());
            self->_line->copy_label(l.c_str());
            self->redraw();
        }
        else if (w == self->_print) {
            self->print();
        }
        else if (w == self->_tab) {
            auto l = util::format("Tab replacement: %d", (int) self->_tab->value());
            self->_tab->copy_label(l.c_str());
            self->redraw();
        }
    }
    void print() {
        auto border  = _border->value();
        auto wrap    = _wrap->value();
        auto line    = static_cast<int>(_line->value());
        auto tab     = static_cast<int>(_tab->value());
        auto format  = static_cast<Fl_Paged_Device::Page_Format>(_format->value());
        auto layout  = (_layout->value() == 0) ? Fl_Paged_Device::Page_Layout::PORTRAIT : Fl_Paged_Device::Page_Layout::LANDSCAPE;
        auto align   = (_align->value() == 0) ? FL_ALIGN_LEFT : (_align->value() == 1) ? FL_ALIGN_CENTER : FL_ALIGN_RIGHT;
        auto file    = _file->label();
        auto printer = PrintText(file, format, layout, _font, _fontsize, align, wrap, border, line);
        auto err     = printer.print(_text, tab);
        if (err == "") {
            auto s = _label2;
            s += util::format("\n%d page%s was printed.", printer.page_count(), printer.page_count() > 1 ? "s" : "");
            _label->copy_label(s.c_str());
            _ret = true;
        }
        else {
            auto s = _label2;
            s += util::format("\nPrinting failed!\n%s", err.c_str());
            _label->copy_label(s.c_str());
            _ret = false;
        }
        redraw();
    }
    bool run() {
        show();
        while (visible() != 0) {
            Fl::wait();
            Fl::flush();
        }
        return _ret;
    }
    void set_label() {
        auto len = 0;
        for (auto& s : _text) {
            auto l = fl_utf_nb_char((unsigned char*) s.c_str(), (int) s.length());
            if (l > len) {
                len = l;
            }
        }
        _label2 = util::format("Text contains %u lines.\nMax line length are %u characters.", (unsigned) _text.size(), (unsigned) len);
        _label->copy_label(_label2.c_str());
    }
};
bool print_text(std::string title, const std::string& text, Fl_Window* parent) {
    auto lines = util::split_string(text, "\n");
    _DlgPrintText dlg(title, parent, lines);
    return dlg.run();
}
bool print_text(std::string title, const StringVector& text, Fl_Window* parent) {
    _DlgPrintText dlg(title, parent, text);
    return dlg.run();
}
class _DlgSelect : public Fl_Double_Window {
    Fl_Button*                  _cancel;
    Fl_Button*                  _close;
    Fl_Input*                   _filter;
    GridGroup*                  _grid;
    ScrollBrowser*              _list;
    const StringVector&         _strings;
public:
    _DlgSelect(const char* title, Fl_Window* parent, const StringVector& strings, int selected_string_index, std::string selected_string, bool fixed_font, int W, int H) :
    Fl_Double_Window(0, 0, ((fixed_font == true) ? flw::PREF_FIXED_FONTSIZE : flw::PREF_FONTSIZE) * W, ((fixed_font == true) ? flw::PREF_FIXED_FONTSIZE : flw::PREF_FONTSIZE) * H),
    _strings(strings) {
        end();
        _cancel = new Fl_Button(0, 0, 0, 0, "&Cancel");
        _close  = new Fl_Return_Button(0, 0, 0, 0, "&Select");
        _filter = new Fl_Input(0, 0, 0, 0);
        _grid   = new GridGroup(0, 0, w(), h());
        _list   = new ScrollBrowser(0, 0, 0, 0);
        _grid->add(_filter,   1,  1, -1,  4);
        _grid->add(_list,     1,  6, -1, -6);
        _grid->add(_cancel, -34, -5, 16,  4);
        _grid->add(_close,  -17, -5, 16,  4);
        add(_grid);
        _cancel->callback(_DlgSelect::Callback, this);
        _close->callback(_DlgSelect::Callback, this);
        _filter->callback(_DlgSelect::Callback, this);
        _filter->tooltip("Enter text to filter rows that macthes the text.\nPress tab to switch focus between input and list widget.");
        _filter->when(FL_WHEN_CHANGED);
        _list->callback(_DlgSelect::Callback, this);
        _list->tooltip("Use Page Up or Page Down in list to scroll faster,");
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
            for (const auto& string : _strings) {
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
        resizable(_grid);
        util::center_window(this, parent);
        _grid->do_layout();
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
        auto self = static_cast<_DlgSelect*>(o);
        if (w == self || w == self->_cancel) {
            self->_list->deselect();
            self->hide();
        }
        else if (w == self->_filter) {
            self->filter(self->_filter->value());
            self->activate_button();
        }
        else if (w == self->_list) {
            self->activate_button();
            if (Fl::event_clicks() > 0 && self->_close->active()) {
                Fl::event_clicks(0);
                self->hide();
            }
        }
        else if (w == self->_close) {
            self->hide();
        }
    }
    void filter(const char* filter) {
        _list->clear();
        for (const auto& string : _strings) {
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
                const auto& string = _strings[f];
                if (string == selected) {
                    return f + 1;
                }
            }
        }
        return 0;
    }
};
int select(std::string title, const StringVector& list, int selected_row, Fl_Window* parent, bool fixed_font, int W, int H) {
    _DlgSelect dlg(title.c_str(), parent, list, selected_row, "", fixed_font, W, H);
    return dlg.run();
}
int select(std::string title, const StringVector& list, const std::string& selected_row, Fl_Window* parent, bool fixed_font, int W, int H) {
    _DlgSelect dlg(title.c_str(), parent, list, 0, selected_row, fixed_font, W, H);
    return dlg.run();
}
class _DlgSlider : public Fl_Double_Window {
    Fl_Button*                  _cancel;
    Fl_Button*                  _close;
    Fl_Hor_Value_Slider*        _slider;
    GridGroup*                  _grid;
    bool                        _ret;
    double&                     _value;
public:
    _DlgSlider(const char* title, Fl_Window* parent, double min, double max, double& value, double step) :
    Fl_Double_Window(0, 0, flw::PREF_FONTSIZE * 40, flw::PREF_FONTSIZE * 6),
    _value(value) {
        end();
        _ret    = false;
        _cancel = new Fl_Button(0, 0, 0, 0, "&Cancel");
        _slider = new Fl_Hor_Value_Slider(0, 0, 0, 0);
        _close  = new Fl_Return_Button(0, 0, 0, 0, "&Ok");
        _grid   = new GridGroup(0, 0, w(), h());
        _grid->add(_slider,   1,  1, -1,  4);
        _grid->add(_cancel, -34, -5, 16,  4);
        _grid->add(_close,  -17, -5, 16,  4);
        add(_grid);
        _cancel->callback(_DlgSlider::Callback, this);
        _slider->align(FL_ALIGN_LEFT    );
        _slider->callback(_DlgSlider::Callback, this);
        _slider->range(min, max);
        _slider->value(value);
        _slider->step(step);
        _slider->textfont(flw::PREF_FONT);
        _slider->textsize(flw::PREF_FONTSIZE);
        _close->callback(_DlgSlider::Callback, this);
        util::labelfont(this);
        callback(_DlgSlider::Callback, this);
        copy_label(title);
        set_modal();
        resizable(_grid);
        size_range(flw::PREF_FONTSIZE * 40, flw::PREF_FONTSIZE * 6);
        util::center_window(this, parent);
        _grid->do_layout();
        _slider->value_width((max >= 100'000) ? flw::PREF_FONTSIZE * 10 : flw::PREF_FONTSIZE * 6);
    }
    static void Callback(Fl_Widget* w, void* o) {
        auto self = static_cast<_DlgSlider*>(o);
        if (w == self || w == self->_cancel) {
            self->hide();
        }
        else if (w == self->_close) {
            self->_ret = true;
            self->_value = self->_slider->value();
            self->hide();
        }
    }
    int run() {
        show();
        while (visible() != 0) {
            Fl::wait();
            Fl::flush();
        }
        return _ret;
    }
};
bool slider(std::string title, double min, double max, double& value, double step, Fl_Window* parent) {
    _DlgSlider dlg(title.c_str(), parent, min, max, value, step);
    return dlg.run();
}
class _DlgText : public Fl_Double_Window {
    Fl_Button*                  _cancel;
    Fl_Button*                  _close;
    Fl_Button*                  _save;
    Fl_Text_Buffer*             _buffer;
    Fl_Text_Display*            _text;
    GridGroup*                  _grid;
    bool                        _edit;
    char*                       _res;
public:
    _DlgText(const char* title, const char* text, bool edit, Fl_Window* parent, int W, int H) :
    Fl_Double_Window(0, 0, flw::PREF_FONTSIZE * W, flw::PREF_FONTSIZE * H) {
        end();
        _buffer = new Fl_Text_Buffer();
        _cancel = new Fl_Button(0, 0, 0, 0, "C&ancel");
        _close  = (edit == false) ? new Fl_Return_Button(0, 0, 0, 0, "&Close") : new Fl_Button(0, 0, 0, 0, "&Update");
        _grid   = new GridGroup(0, 0, w(), h());
        _save   = new Fl_Button(0, 0, 0, 0, "&Save");
        _text   = (edit == false) ? new Fl_Text_Display(0, 0, 0, 0) : new Fl_Text_Editor(0, 0, 0, 0);
        _edit   = edit;
        _res    = nullptr;
        _grid->add(_text,     1,   1,  -1,  -6);
        _grid->add(_cancel, -51,  -5,  16,   4);
        _grid->add(_save,   -34,  -5,  16,   4);
        _grid->add(_close,  -17,  -5,  16,   4);
        add(_grid);
        _buffer->text(text);
        _cancel->callback(_DlgText::Callback, this);
        _cancel->tooltip("Close and abort all changes.");
        _close->callback(_DlgText::Callback, this);
        _close->tooltip(_edit == true ? "Update text and close window." : "Close window.");
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
        resizable(_grid);
        util::center_window(this, parent);
        _grid->do_layout();
    }
    ~_DlgText() {
       _text->buffer(nullptr);
        delete _buffer;
    }
    static void Callback(Fl_Widget* w, void* o) {
        auto self = static_cast<_DlgText*>(o);
        if (w == self || w == self->_cancel) {
            self->hide();
        }
        else if (w == self->_save) {
            auto filename = fl_file_chooser("Select Destination File", nullptr, nullptr, 0);
            if (filename != nullptr && self->_buffer->savefile(filename) != 0) {
                fl_alert("error: failed to save text to %s", filename);
            }
        }
        else if (w == self->_close) {
            if (self->_edit == true) {
                self->_res = self->_buffer->text();
            }
            self->hide();
        }
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
void text(std::string title, const std::string& text, Fl_Window* parent, int W, int H) {
    _DlgText dlg(title.c_str(), text.c_str(), false, parent, W, H);
    dlg.run();
}
bool text_edit(std::string title, std::string& text, Fl_Window* parent, int W, int H) {
    auto dlg = _DlgText(title.c_str(), text.c_str(), true, parent, W, H);
    auto res = dlg.run();
    if (res == nullptr) {
        return false;
    }
    text = res;
    free(res);
    return true;
}
class _DlgTheme : public Fl_Double_Window {
    Fl_Box*                     _fixed_label;
    Fl_Box*                     _font_label;
    Fl_Browser*                 _theme;
    Fl_Button*                  _close;
    Fl_Button*                  _fixedfont;
    Fl_Button*                  _font;
    GridGroup*                  _grid;
    int                         _theme_row;
public:
    _DlgTheme(bool enable_font, bool enable_fixedfont, Fl_Window* parent) :
    Fl_Double_Window(0, 0, 10, 10, "Set Theme") {
        end();
        _close       = new Fl_Return_Button(0, 0, 0, 0, "&Close");
        _fixedfont   = new Fl_Button(0, 0, 0, 0, "&Mono font");
        _fixed_label = new Fl_Box(0, 0, 0, 0);
        _font        = new Fl_Button(0, 0, 0, 0, "&Regular font");
        _font_label  = new Fl_Box(0, 0, 0, 0);
        _grid        = new GridGroup(0, 0, w(), h());
        _theme       = new Fl_Hold_Browser(0, 0, 0, 0);
        _theme_row   = 0;
        _grid->add(_theme,         1,   1,  -1, -16);
        _grid->add(_font_label,    1, -15,  -1,   4);
        _grid->add(_fixed_label,   1, -10,  -1,   4);
        _grid->add(_font,        -51,  -5,  16,   4);
        _grid->add(_fixedfont,   -34,  -5,  16,   4);
        _grid->add(_close,       -17,  -5,  16,   4);
        add(_grid);
        if (enable_font == false) {
          _font->deactivate();
        }
        if (enable_fixedfont == false) {
          _fixedfont->deactivate();
        }
        _close->callback(Callback, this);
        _fixed_label->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
        _fixed_label->box(FL_BORDER_BOX);
        _fixed_label->color(FL_BACKGROUND2_COLOR);
        _fixedfont->callback(Callback, this);
        _font->callback(Callback, this);
        _font_label->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
        _font_label->box(FL_BORDER_BOX);
        _font_label->color(FL_BACKGROUND2_COLOR);
        _theme->box(FL_BORDER_BOX);
        _theme->callback(Callback, this);
        _theme->textfont(flw::PREF_FONT);
        for (size_t f = 0; f < 100; f++) {
            auto t = flw::PREF_THEMES[f];
            if (t != nullptr) {
                _theme->add(t);
            }
            else {
                break;
            }
        }
        resizable(_grid);
        callback(Callback, this);
        set_modal();
        update_pref();
        util::center_window(this, parent);
    }
    static void Callback(Fl_Widget* w, void* o) {
        auto self = static_cast<_DlgTheme*>(o);
        if (w == self) {
            self->hide();
        }
        else if (w == self->_fixedfont) {
            FontDialog fd(flw::PREF_FIXED_FONT, flw::PREF_FIXED_FONTSIZE, "Select Monospaced Font");
            if (fd.run(Fl::first_window()) == true) {
                flw::PREF_FIXED_FONT     = fd.font();
                flw::PREF_FIXED_FONTSIZE = fd.fontsize();
                flw::PREF_FIXED_FONTNAME = fd.fontname();
                if (self->_font->active() == 0) {
                    flw::PREF_FONTSIZE = flw::PREF_FIXED_FONTSIZE;
                }
                self->update_pref();
            }
        }
        else if (w == self->_font) {
            FontDialog fd(flw::PREF_FONT, flw::PREF_FONTSIZE, "Select Regular Font");
            if (fd.run(Fl::first_window()) == true) {
                flw::PREF_FONT     = fd.font();
                flw::PREF_FONTSIZE = fd.fontsize();
                flw::PREF_FONTNAME = fd.fontname();
                Fl_Tooltip::font(flw::PREF_FONT);
                Fl_Tooltip::size(flw::PREF_FONTSIZE);
                if (self->_fixedfont->active() == 0) {
                    flw::PREF_FIXED_FONTSIZE = flw::PREF_FONTSIZE;
                }
                self->update_pref();
            }
        }
        else if (w == self->_theme) {
            auto row = self->_theme->value() - 1;
            if (row == theme::THEME_GLEAM) {
                theme::_load_gleam();
            }
            else if (row == theme::THEME_GLEAM_BLUE) {
                theme::_load_gleam_blue();
            }
            else if (row == theme::THEME_GLEAM_DARK) {
                theme::_load_gleam_dark();
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
            else if (row == theme::THEME_GTK_DARK) {
                theme::_load_gtk_dark();
            }
            else if (row == theme::THEME_GTK_TAN) {
                theme::_load_gtk_tan();
            }
            else if (row == theme::THEME_OXY) {
                theme::_load_oxy();
            }
            else if (row == theme::THEME_OXY_TAN) {
                theme::_load_oxy_tan();
            }
            else if (row == theme::THEME_PLASTIC) {
                theme::_load_plastic();
            }
            else if (row == theme::THEME_PLASTIC_TAN) {
                theme::_load_plastic_tan();
            }
            else {
                theme::_load_default();
            }
            self->update_pref();
        }
        else if (w == self->_close) {
            self->hide();
        }
    }
    void run() {
        show();
        while (visible()) {
            Fl::wait();
            Fl::flush();
        }
    }
    void update_pref() {
        Fl_Tooltip::font(flw::PREF_FONT);
        Fl_Tooltip::size(flw::PREF_FONTSIZE);
        util::labelfont(this);
        _font_label->copy_label(util::format("%s - %d", flw::PREF_FONTNAME.c_str(), flw::PREF_FONTSIZE).c_str());
        _fixed_label->copy_label(util::format("%s - %d", flw::PREF_FIXED_FONTNAME.c_str(), flw::PREF_FIXED_FONTSIZE).c_str());
        _fixed_label->labelfont(flw::PREF_FIXED_FONT);
        _fixed_label->labelsize(flw::PREF_FIXED_FONTSIZE);
        _theme->textsize(flw::PREF_FONTSIZE);
        size(flw::PREF_FONTSIZE * 30, flw::PREF_FONTSIZE * 28);
        size_range(flw::PREF_FONTSIZE * 20, flw::PREF_FONTSIZE * 14);
        _grid->resize(0, 0, w(), h());
        theme::_scrollbar();
        for (int f = 0; f < theme::THEME_NIL; f++) {
            if (flw::PREF_THEME == flw::PREF_THEMES[f]) {
                _theme->value(f + 1);
                break;
            }
        }
        Fl::redraw();
    }
};
void theme(bool enable_font, bool enable_fixedfont, Fl_Window* parent) {
    auto dlg = _DlgTheme(enable_font, enable_fixedfont, parent);
    dlg.run();
}
AbortDialog::AbortDialog(std::string label, double min, double max) :
Fl_Double_Window(0, 0, 0, 0, "Working...") {
    _button   = new Fl_Button(0, 0, 0, 0, "Press to abort");
    _grid     = new GridGroup();
    _progress = new Fl_Hor_Fill_Slider(0, 0, 0, 0);
    _abort    = false;
    _last     = 0;
    _grid->add(_button,     1,   1,   -1,  -6);
    _grid->add(_progress,   1,  -5,   -1,   4);
    add(_grid);
    auto W = flw::PREF_FONTSIZE * 32;
    auto H = flw::PREF_FONTSIZE * 12;
    if (min < max && fabs(max - min) > 0.001) {
        _progress->range(min, max);
        _progress->value(min);
    }
    else {
        _progress->hide();
        _grid->resize(_button, 1, 1, -1, -1);
    }
    _button->callback(AbortDialog::Callback, this);
    _button->labelfont(flw::PREF_FONT);
    _button->labelsize(flw::PREF_FONTSIZE);
    _progress->color(FL_SELECTION_COLOR);
    if (label != "") {
        copy_label(label.c_str());
    }
    resizable(this);
    size(W, H);
    size_range(W, H);
    callback(AbortDialog::Callback, this);
    set_modal();
}
void AbortDialog::Callback(Fl_Widget* w, void* o) {
    auto self = static_cast<AbortDialog*>(o);
    if (w == self->_button) {
        self->_abort = true;
    }
}
bool AbortDialog::check(int milliseconds) {
    auto now = util::milliseconds();
    if (now - _last > milliseconds) {
        _last = now;
        Fl::check();
    }
    return _abort;
}
bool AbortDialog::check(double value, double min, double max, int milliseconds) {
    auto now = util::milliseconds();
    if (now - _last > milliseconds) {
        _progress->value(value);
        _progress->range(min, max);
        _last = now;
        Fl::check();
    }
    return _abort;
}
void AbortDialog::range(double min, double max) {
    _progress->range(min, max);
}
void AbortDialog::show(const std::string& label, Fl_Window* parent) {
    _abort = false;
    _last  = 0;
    _button->copy_label(label.c_str());
    util::center_window(this, parent);
    Fl_Double_Window::show();
    Fl::flush();
}
void AbortDialog::value(double value) {
    _progress->value(value);
}
static const std::string _FONTDIALOG_LABEL = R"(
ABCDEFGHIJKLMNOPQRSTUVWXYZ /0123456789
abcdefghijklmnopqrstuvwxyz £©µÀÆÖÞßéöÿ
–—‘“”„†•…‰™œŠŸž€ ΑΒΓΔΩαβγδω АБВГДабвгд
∀∂∈ℝ∧∪≡∞ ↑↗↨↻⇣ ┐┼╔╘░►☺♀ ﬁ�⑀₂ἠḂӥẄɐː⍎אԱა

japanese: こんにちは世界
korean: 안녕하세요 세계
greek: Γειά σου Κόσμε
ukrainian: Привіт Світ
thai: สวัสดีชาวโลก
amharic: ሰላም ልዑል
braille: ⡌⠁⠧⠑ ⠼⠁⠒  ⡍⠜⠇⠑⠹⠰⠎ ⡣⠕⠌
math: ∮ E⋅da = Q,  n → ∞, ∑ f(i) 2H₂ + O₂ ⇌ 2H₂O, R = 4.7 kΩ

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
FontDialog::FontDialog(Fl_Font font, Fl_Fontsize fontsize, const std::string& label, bool limit_to_default) :
Fl_Double_Window(0, 0, flw::PREF_FONTSIZE * 64, flw::PREF_FONTSIZE * 36) {
    _create(font, "", fontsize, label, limit_to_default);
}
FontDialog::FontDialog(std::string font, Fl_Fontsize fontsize, std::string label, bool limit_to_default) :
Fl_Double_Window(0, 0, flw::PREF_FONTSIZE * 64, flw::PREF_FONTSIZE * 36) {
    _create(0, font, fontsize, label, limit_to_default);
}
void FontDialog::_activate() {
    if (_fonts->value() == 0 || _sizes->value() == 0 || (_fonts->active() == 0 && _sizes->active() == 0)) {
        _select->deactivate();
    }
    else {
        _select->activate();
    }
}
void FontDialog::Callback(Fl_Widget* w, void* o) {
    auto self = static_cast<FontDialog*>(o);
    if (w == self) {
        self->hide();
    }
    else if (w == self->_cancel) {
        self->hide();
    }
    else if (w == self->_fonts) {
        auto row = self->_fonts->value();
        if (row > 0) {
            static_cast<_FontDialogLabel*>(self->_label)->font = row - 1;
        }
        self->_activate();
        Fl::redraw();
    }
    else if (w == self->_select) {
        auto row1 = self->_fonts->value();
        auto row2 = self->_sizes->value();
        if (row1 > 0 && row2 > 0) {
            row1--;
            self->_fontname = util::remove_browser_format(flw::PREF_FONTNAMES[row1]);
            self->_font     = row1;
            self->_fontsize = row2 + 5;
            self->_ret      = true;
            self->hide();
        }
    }
    else if (w == self->_sizes) {
        auto row = self->_sizes->value();
        if (row > 0) {
            static_cast<_FontDialogLabel*>(self->_label)->size = row + 5;
        }
        self->_activate();
        Fl::redraw();
    }
}
void FontDialog::_create(Fl_Font font, std::string fontname, Fl_Fontsize fontsize, std::string label, bool limit_to_default) {
    end();
    _cancel   = new Fl_Button(0, 0, 0, 0, "&Cancel");
    _fonts    = new ScrollBrowser(12);
    _grid     = new GridGroup();
    _label    = new _FontDialogLabel(0, 0, 0, 0);
    _select   = new Fl_Button(0, 0, 0, 0, "&Select");
    _sizes    = new ScrollBrowser(6);
    _font     = -1;
    _fontsize = -1;
    _ret      = false;
    _grid->add(_fonts,    1,   1,  50,  -6);
    _grid->add(_sizes,   52,   1,  12,  -6);
    _grid->add(_label,   65,   1,  -1,  -6);
    _grid->add(_cancel, -34,  -5,  16,   4);
    _grid->add(_select, -17,  -5,  16,   4);
    add(_grid);
    _cancel->callback(FontDialog::Callback, this);
    _cancel->labelfont(flw::PREF_FONT);
    _cancel->labelsize(flw::PREF_FONTSIZE);
    _fonts->box(FL_BORDER_BOX);
    _fonts->callback(FontDialog::Callback, this);
    _fonts->textsize(flw::PREF_FONTSIZE);
    _fonts->when(FL_WHEN_CHANGED);
    static_cast<_FontDialogLabel*>(_label)->font = font;
    static_cast<_FontDialogLabel*>(_label)->size = fontsize;
    _select->callback(FontDialog::Callback, this);
    _select->labelfont(flw::PREF_FONT);
    _select->labelsize(flw::PREF_FONTSIZE);
    _sizes->box(FL_BORDER_BOX);
    _sizes->callback(FontDialog::Callback, this);
    _sizes->textsize(flw::PREF_FONTSIZE);
    _sizes->when(FL_WHEN_CHANGED);
    theme::load_fonts();
    auto count = 0;
    for (auto name : flw::PREF_FONTNAMES) {
        if (limit_to_default == true && count == 12) {
            break;
        }
        _fonts->add(name);
        count++;
    }
    for (auto f = 6; f <= 72; f++) {
        char buf[50];
        snprintf(buf, 50, "@r%2d  ", f);
        _sizes->add(buf);
    }
    if (fontsize >= 6 && fontsize <= 72) {
        _sizes->value(fontsize - 5);
        _sizes->middleline(fontsize - 5);
        static_cast<_FontDialogLabel*>(_label)->font = fontsize;
    }
    else {
        _sizes->value(14 - 5);
        _sizes->middleline(14 - 5);
        static_cast<_FontDialogLabel*>(_label)->font = 14;
    }
    if (fontname != "") {
        _select_name(fontname);
    }
    else if (font >= 0 && font < _fonts->size()) {
        _fonts->value(font + 1);
        _fonts->middleline(font + 1);
        static_cast<_FontDialogLabel*>(_label)->font = font;
    }
    else {
        _fonts->value(1);
        _fonts->middleline(1);
    }
    resizable(this);
    copy_label(label.c_str());
    callback(FontDialog::Callback, this);
    size_range(flw::PREF_FONTSIZE * 38, flw::PREF_FONTSIZE * 12);
    set_modal();
    _fonts->take_focus();
    _grid->do_layout();
}
bool FontDialog::run(Fl_Window* parent) {
    _ret = false;
    _activate();
    util::center_window(this, parent);
    show();
    while (visible() != 0) {
        Fl::wait();
        Fl::flush();
    }
    return _ret;
}
void FontDialog::_select_name(std::string fontname) {
    auto count = 1;
    for (auto f : flw::PREF_FONTNAMES) {
        auto font_without_style = util::remove_browser_format(f);
        if (fontname == font_without_style) {
            _fonts->value(count);
            _fonts->middleline(count);
            static_cast<_FontDialogLabel*>(_label)->font = count - 1;
            return;
        }
        count++;
    }
    _fonts->value(1);
    static_cast<_FontDialogLabel*>(_label)->font = 0;
}
WorkDialog::WorkDialog(const char* title, Fl_Window* parent, bool cancel, bool pause, int W, int H) :
Fl_Double_Window(0, 0, W * flw::PREF_FONTSIZE, H * flw::PREF_FONTSIZE) {
    end();
    _cancel = new Fl_Button(0, 0, 0, 0, "Cancel");
    _grid   = new GridGroup();
    _label  = new Fl_Hold_Browser(0, 0, 0, 0);
    _pause  = new Fl_Toggle_Button(0, 0, 0, 0, "Pause");
    _ret    = true;
    _last   = 0.0;
    _grid->add(_label,    1,   1,  -1,  -6);
    _grid->add(_pause,  -34,  -5,  16,   4);
    _grid->add(_cancel, -17,  -5,  16,   4);
    add(_grid);
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
    util::center_window(this, parent);
    show();
}
void WorkDialog::Callback(Fl_Widget* w, void* o) {
    auto self = static_cast<WorkDialog*>(o);
    if (w == self) {
    }
    else if (w == self->_cancel) {
        self->_ret = false;
    }
    else if (w == self->_pause) {
        bool cancel = self->_cancel->active();
        self->_cancel->deactivate();
        self->_pause->label("C&ontinue");
        while (self->_pause->value() != 0) {
            util::sleep(10);
            Fl::check();
        }
        self->_pause->label("&Pause");
        if (cancel) {
            self->_cancel->activate();
        }
    }
}
bool WorkDialog::run(double update_time, const StringVector& messages) {
    auto now = util::clock();
    if ((now - _last) > update_time) {
        _label->clear();
        for (const auto& s : messages) {
            _label->add(s.c_str());
        }
        _last = now;
        Fl::check();
        Fl::flush();
    }
    return _ret;
}
bool WorkDialog::run(double update_time, const std::string& message) {
    auto now = util::clock();
    if ((now - _last) > update_time) {
        _label->clear();
        _label->add(message.c_str());
        _last = now;
        Fl::check();
        Fl::flush();
    }
    return _ret;
}
}
}
#include <algorithm>
#include <cstdint>
#include <assert.h>
#include <stdarg.h>
#include <time.h>
#include <FL/Fl_Window.H>
#include <FL/Fl_File_Chooser.H>
#include <FL/Fl_Hold_Browser.H>
#include <FL/Fl_Tooltip.H>
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
std::string                 PREF_THEME              = "default";
const char* const           PREF_THEMES[]           = {
                                "default",
                                "gleam",
                                "blue gleam",
                                "dark gleam",
                                "tan gleam",
                                "gtk",
                                "blue gtk",
                                "dark gtk",
                                "tan gtk",
                                "oxy",
                                "tan oxy",
                                "plastic",
                                "tan plastic",
                                nullptr,
};
const char* const           PREF_THEMES2[]           = {
                                "default",
                                "gleam",
                                "blue_gleam",
                                "dark_gleam",
                                "tan_gleam",
                                "gtk",
                                "blue_gtk",
                                "dark_gtk",
                                "tan_gtk",
                                "oxy",
                                "tan_oxy",
                                "plastic",
                                "tan_plastic",
                                nullptr,
};
static std::string _print(std::string ps_filename, Fl_Paged_Device::Page_Format format, Fl_Paged_Device::Page_Layout layout, PrintCallback cb, void* data, int from, int to) {
    bool                      cont = true;
    FILE*                     file = nullptr;
    Fl_PostScript_File_Device printer;
    int                       ph;
    int                       pw;
    std::string               res;
    if ((file = fl_fopen(ps_filename.c_str(), "wb")) == nullptr) {
        return "error: could not open file!";
    }
    printer.begin_job(file, 0, format, layout);
    while (cont == true) {
        if (printer.begin_page() != 0) {
            res = "error: couldn't create new page!";
            goto ERR;
        }
        if (printer.printable_rect(&pw, &ph) != 0) {
            res = "error: couldn't retrieve page size!";
            goto ERR;
        }
        fl_push_clip(0, 0, pw, ph);
        cont = cb(data, pw, ph, from);
        fl_pop_clip();
        if (printer.end_page() != 0) {
            res = "error: couldn't end page!";
            goto ERR;
        }
        if (from > 0) {
            from++;
            if (from > to) {
                cont = false;
            }
        }
    }
ERR:
    printer.end_job();
    fclose(file);
    return res;
}
void debug::print(const Fl_Widget* widget) {
    std::string indent;
    debug::print(widget, indent);
}
void debug::print(const Fl_Widget* widget, std::string& indent) {
    if (widget == nullptr) {
        puts("flw::debug::print() => null widget");
    }
    else {
        printf("%sx=%4d, y=%4d, w=%4d, h=%4d, %c, \"%s\"\n", indent.c_str(), widget->x(), widget->y(), widget->w(), widget->h(), widget->visible() ? 'V' : 'H', widget->label() ? widget->label() : "NULL");
        auto group = widget->as_group();
        if (group != nullptr) {
            indent += "\t";
            for (int f = 0; f < group->children(); f++) {
                debug::print(group->child(f), indent);
            }
            indent.pop_back();
        }
    }
    fflush(stdout);
}
bool debug::test(bool val, int line, const char* func) {
    if (val == false) {
        fprintf(stderr, "error: test failed at line %d in %s\n", line, func);
        fflush(stderr);
        return false;
    }
    return true;
}
bool debug::test(const char* ref, const char* val, int line, const char* func) {
    if (ref == nullptr && val == nullptr) {
        return true;
    }
    else if (ref == nullptr || val == nullptr || strcmp(ref, val) != 0) {
        fprintf(stderr, "error: test failed '%s' != '%s' at line %d in %s\n", ref ? ref : "NULL", val ? val : "NULL", line, func);
        fflush(stderr);
        return false;
    }
    return true;
}
bool debug::test(int64_t ref, int64_t val, int line, const char* func) {
    if (ref != val) {
        fprintf(stderr, "error: test failed '%lld' != '%lld' at line %d in %s\n", (long long int) ref, (long long int) val, line, func);
        fflush(stderr);
        return false;
    }
    return true;
}
bool debug::test(double ref, double val, double diff, int line, const char* func) {
    if (fabs(ref - val) > diff) {
        fprintf(stderr, "error: test failed '%f' != '%f' at line %d in %s\n", ref, val, line, func);
        fflush(stderr);
        return false;
    }
    return true;
}
namespace menu {
static Fl_Menu_Item* _item(Fl_Menu_* menu, const char* text, void* v = nullptr) {
    const Fl_Menu_Item* item;
    if (v == nullptr) {
        assert(menu && text);
        item = menu->find_item(text);
    }
    else {
        item = menu->find_item_with_user_data(v);
    }
#ifdef DEBUG
    if (item == nullptr) fprintf(stderr, "error: cant find menu item <%s>\n", text);
#endif
    return const_cast<Fl_Menu_Item*>(item);
}
}
void menu::enable_item(Fl_Menu_* menu, const char* text, bool value) {
    auto item = _item(menu, text);
    if (item == nullptr) return;
    if (value == true) item->activate();
    else item->deactivate();
}
Fl_Menu_Item* menu::get_item(Fl_Menu_* menu, const char* text) {
    return _item(menu, text);
}
Fl_Menu_Item* menu::get_item(Fl_Menu_* menu, void* v) {
    return _item(menu, nullptr, v);
}
bool menu::item_value(Fl_Menu_* menu, const char* text) {
    auto item = _item(menu, text);
    if (item == nullptr) return false;
    return item->value();
}
void menu::set_item(Fl_Menu_* menu, const char* text, bool value) {
    auto item = _item(menu, text);
    if (item == nullptr) return;
    if (value == true) item->set();
    else item->clear();
}
void menu::setonly_item(Fl_Menu_* menu, const char* text) {
    auto item = _item(menu, text);
    if (item == nullptr) return;
    menu->setonly(item);
}
void util::center_window(Fl_Window* window, Fl_Window* parent) {
    if (parent != nullptr) {
        int x = parent->x() + parent->w() / 2;
        int y = parent->y() + parent->h() / 2;
        window->resize(x - window->w() / 2, y - window->h() / 2, window->w(), window->h());
    }
    else {
        window->resize((Fl::w() / 2) - (window->w() / 2), (Fl::h() / 2) - (window->h() / 2), window->w(), window->h());
    }
}
double util::clock() {
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
int util::count_decimals(double num) {
    num = fabs(num);
    if (num > 9'223'372'036'854'775'807.0) {
        return 0;
    }
    int    res     = 0;
    char*  end     = 0;
    double inum = static_cast<int64_t>(num);
    double fnum = num - inum;
    char   buffer[100];
    if (num > 999'999'999'999'999) {
        snprintf(buffer, 100, "%.1f", fnum);
    }
    else if (num > 9'999'999'999'999) {
        snprintf(buffer, 100, "%.2f", fnum);
    }
    else if (num > 999'999'999'999) {
        snprintf(buffer, 100, "%.3f", fnum);
    }
    else if (num > 99'999'999'999) {
        snprintf(buffer, 100, "%.4f", fnum);
    }
    else if (num > 9'999'999'999) {
        snprintf(buffer, 100, "%.5f", fnum);
    }
    else if (num > 999'999'999) {
        snprintf(buffer, 100, "%.6f", fnum);
    }
    else if (num > 99'999'999) {
        snprintf(buffer, 100, "%.7f", fnum);
    }
    else if (num > 9'999'999) {
        snprintf(buffer, 100, "%.8f", fnum);
    }
    else {
        snprintf(buffer, 100, "%.9f", fnum);
    }
    size_t len = strlen(buffer);
    end = buffer + len - 1;
    while (*end == '0') {
        *end = 0;
        end--;
    }
    res = strlen(buffer) - 2;
    return res;
}
Fl_Widget* util::find_widget(Fl_Group* group, std::string label) {
    for (int f = 0; f < group->children(); f++) {
        auto w = group->child(f);
        if (w->label() != nullptr && label == w->label()) {
            return w;
        }
    }
    for (int f = 0; f < group->children(); f++) {
        auto w = group->child(f);
        auto g = w->as_group();
        if (g != nullptr) {
            w = util::find_widget(g, label);
            if (w != nullptr) {
                return w;
            }
        }
    }
    return nullptr;
}
std::string util::fix_menu_string(std::string in) {
    std::string res = in;
    util::replace_string(res, "\\", "\\\\");
    util::replace_string(res, "_", "\\_");
    util::replace_string(res, "/", "\\/");
    util::replace_string(res, "&", "&&");
    return res;
}
std::string util::format(const char* format, ...) {
    if (format == nullptr || *format == 0) return "";
    int         l   = 128;
    int         n   = 0;
    char*       buf = static_cast<char*>(calloc(l, 1));
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
    buf = static_cast<char*>(calloc(l, 1));
    if (buf == nullptr) return res;
    va_start(args, format);
    vsnprintf(buf, l, format, args);
    va_end(args);
    res = buf;
    free(buf);
    return res;
}
std::string util::format_double(double num, int decimals, char del) {
    if (num > 9'223'372'036'854'775'807.0) {
        return "err";
    }
    if (decimals < 0) {
        decimals = util::count_decimals(num);
    }
    if (del < 32) {
        del = 32;
    }
    if (decimals == 0 || decimals > 9) {
        return util::format_int(static_cast<int64_t>(num), del);
    }
    char res[100];
    char fr_str[100];
    auto int_num    = static_cast<int64_t>(fabs(num));
    auto double_num = static_cast<double>(fabs(num) - int_num);
    auto int_str    = util::format_int(int_num, del);
    auto len        = snprintf(fr_str, 99, "%.*f", decimals, double_num);
    *res = 0;
    if (len > 0 && len < 100) {
        if (num < 0.0) {
            res[0] = '-';
            res[1] = 0;
        }
        strncat(res, int_str.c_str(), 99);
        strncat(res, fr_str + 1, 99);
    }
    return res;
}
std::string util::format_int(int64_t num, char del) {
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
bool util::is_whitespace_or_empty(const char* str) {
    while (*str != 0) {
        if (*str != 9 && *str != 32) {
            return false;
        }
        str++;
    }
    return true;
}
void util::labelfont(Fl_Widget* widget, Fl_Font fn, int fs) {
    widget->labelfont(fn);
    widget->labelsize(fs);
    auto group = widget->as_group();
    if (group != nullptr) {
        for (auto f = 0; f < group->children(); f++) {
            util::labelfont(group->child(f), fn, fs);
        }
    }
}
int64_t util::microseconds() {
#if defined(_WIN32)
    LARGE_INTEGER StartingTime;
    LARGE_INTEGER Frequency;
    QueryPerformanceFrequency(&Frequency);
    QueryPerformanceCounter(&StartingTime);
    StartingTime.QuadPart *= 1000000;
    StartingTime.QuadPart /= Frequency.QuadPart;
    return StartingTime.QuadPart;
#else
    timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return (t.tv_sec * 1000000 + t.tv_nsec / 1000);
#endif
}
int32_t util::milliseconds() {
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
bool util::png_save(std::string opt_name, Fl_Window* window, int X, int Y, int W, int H) {
    auto res = false;
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
                res = true;
            }
            else if (ret == -1) {
                fl_alert("%s", "error: missing libraries");
            }
            else if (ret == -2) {
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
    return res;
}
std::string util::print(std::string ps_filename, Fl_Paged_Device::Page_Format format, Fl_Paged_Device::Page_Layout layout, PrintCallback cb, void* data) {
    return flw::_print(ps_filename, format, layout, cb, data, 0, 0);
}
std::string util::print(std::string ps_filename, Fl_Paged_Device::Page_Format format, Fl_Paged_Device::Page_Layout layout, PrintCallback cb, void* data, int from, int to) {
    if (from < 1 || from > to) {
        return "error: invalid from/to range";
    }
    return flw::_print(ps_filename, format, layout, cb, data, from, to);
}
std::string util::remove_browser_format(const char* text) {
    auto res = std::string((text != nullptr) ? text : "");
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
std::string& util::replace_string(std::string& string, std::string find, std::string replace) {
    if (find == "") {
        return string;
    }
    try {
        size_t start = 0;
        while ((start = string.find(find, start)) != std::string::npos) {
            string.replace(start, find.length(), replace);
            start += replace.length();
        }
    }
    catch(...) {
        string = "";
    }
    return string;
}
void util::sleep(int milli) {
#ifdef _WIN32
    Sleep(milli);
#else
    usleep(milli * 1000);
#endif
}
flw::StringVector util::split_string(const std::string& string, std::string split) {
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
double util::to_double(std::string num, double def) {
    try {
        return std::stod(num);
    }
    catch(...) {
        return def;
    }
}
long long util::to_long(std::string num, long long def) {
    try {
        return std::stoll(num);
    }
    catch(...) {
        return def;
    }
}
void* util::zero_memory(char* mem, size_t size) {
    if (mem == nullptr || size == 0) return mem;
#ifdef _WIN32
    RtlSecureZeroMemory(mem, size);
#else
    auto p = reinterpret_cast<volatile unsigned char*>(mem);
    while (size--) {
        *p = 0;
        p++;
    }
#endif
    return mem;
}
Fl_Color color::BEIGE            = fl_rgb_color(245, 245, 220);
Fl_Color color::CHOCOLATE        = fl_rgb_color(210, 105,  30);
Fl_Color color::CRIMSON          = fl_rgb_color(220,  20,  60);
Fl_Color color::DARKOLIVEGREEN   = fl_rgb_color( 85, 107,  47);
Fl_Color color::DODGERBLUE       = fl_rgb_color( 30, 144, 255);
Fl_Color color::FORESTGREEN      = fl_rgb_color( 34, 139,  34);
Fl_Color color::GOLD             = fl_rgb_color(255, 215,   0);
Fl_Color color::GRAY             = fl_rgb_color(128, 128, 128);
Fl_Color color::INDIGO           = fl_rgb_color( 75,   0, 130);
Fl_Color color::OLIVE            = fl_rgb_color(128, 128,   0);
Fl_Color color::PINK             = fl_rgb_color(255, 192, 203);
Fl_Color color::ROYALBLUE        = fl_rgb_color( 65, 105, 225);
Fl_Color color::SIENNA           = fl_rgb_color(160,  82,  45);
Fl_Color color::SILVER           = fl_rgb_color(192, 192, 192);
Fl_Color color::SLATEGRAY        = fl_rgb_color(112, 128, 144);
Fl_Color color::TEAL             = fl_rgb_color(  0, 128, 128);
Fl_Color color::TURQUOISE        = fl_rgb_color( 64, 224, 208);
Fl_Color color::VIOLET           = fl_rgb_color(238, 130, 238);
namespace theme {
static unsigned char _OLD_R[256]  = { 0 };
static unsigned char _OLD_G[256]  = { 0 };
static unsigned char _OLD_B[256]  = { 0 };
static bool          _IS_DARK     = false;
static bool          _SAVED_COLOR = false;
static int           _SCROLLSIZE  = Fl::scrollbar_size();
static void _additional_colors(bool dark) {
    color::BEIGE            = fl_rgb_color(245, 245, 220);
    color::CHOCOLATE        = fl_rgb_color(210, 105,  30);
    color::CRIMSON          = fl_rgb_color(220,  20,  60);
    color::DARKOLIVEGREEN   = fl_rgb_color( 85, 107,  47);
    color::DODGERBLUE       = fl_rgb_color( 30, 144, 255);
    color::FORESTGREEN      = fl_rgb_color( 34, 139,  34);
    color::GOLD             = fl_rgb_color(255, 215,   0);
    color::GRAY             = fl_rgb_color(128, 128, 128);
    color::INDIGO           = fl_rgb_color( 75,   0, 130);
    color::OLIVE            = fl_rgb_color(128, 128,   0);
    color::PINK             = fl_rgb_color(255, 192, 203);
    color::ROYALBLUE        = fl_rgb_color( 65, 105, 225);
    color::SIENNA           = fl_rgb_color(160,  82,  45);
    color::SILVER           = fl_rgb_color(192, 192, 192);
    color::SLATEGRAY        = fl_rgb_color(112, 128, 144);
    color::TEAL             = fl_rgb_color(  0, 128, 128);
    color::TURQUOISE        = fl_rgb_color( 64, 224, 208);
    color::VIOLET           = fl_rgb_color(238, 130, 238);
    if (dark == true) {
        color::BEIGE            = fl_darker(color::BEIGE);
        color::CHOCOLATE        = fl_darker(color::CHOCOLATE);
        color::CRIMSON          = fl_darker(color::CRIMSON);
        color::DARKOLIVEGREEN   = fl_darker(color::DARKOLIVEGREEN);
        color::DODGERBLUE       = fl_darker(color::DODGERBLUE);
        color::FORESTGREEN      = fl_darker(color::FORESTGREEN);
        color::GOLD             = fl_darker(color::GOLD);
        color::GRAY             = fl_darker(color::GRAY);
        color::INDIGO           = fl_darker(color::INDIGO);
        color::OLIVE            = fl_darker(color::OLIVE);
        color::PINK             = fl_darker(color::PINK);
        color::ROYALBLUE        = fl_darker(color::ROYALBLUE);
        color::SIENNA           = fl_darker(color::SIENNA);
        color::SILVER           = fl_darker(color::SILVER);
        color::SLATEGRAY        = fl_darker(color::SLATEGRAY);
        color::TEAL             = fl_darker(color::TEAL);
        color::TURQUOISE        = fl_darker(color::TURQUOISE);
        color::VIOLET           = fl_darker(color::VIOLET);
    }
}
static void _blue_colors() {
    Fl::set_color(0,   228, 228, 228);
    Fl::set_color(7,    79,  86,  94);
    Fl::set_color(8,   108, 113, 125);
    Fl::set_color(15,  241, 196,  126);
    Fl::set_color(56,    0,   0,   0);
    Fl::background(48, 56, 65);
    unsigned char r = 0;
    unsigned char g = 0;
    unsigned char b = 0;
    for (int f = 32; f < 49; f++) {
        Fl::set_color(f, r, g, b);
        if (f == 32) {
            r = 0;
            g = 9;
            b = 18;
        }
        else {
            r += 2 + (f < 44);
            g += 2 + (f < 44);
            b += 2 + (f < 44);
        }
    }
}
static void _dark_colors() {
    Fl::set_color(0,   200, 200, 200);
    Fl::set_color(7,    64,  64,  64);
    Fl::set_color(8,   100, 100, 100);
    Fl::set_color(15,  177, 227, 177);
    Fl::set_color(56,    0,   0,   0);
    Fl::set_color(49, 43, 43, 43);
    Fl::background(43, 43, 43);
    unsigned char c = 0;
    for (int f = 32; f < 49; f++) {
        Fl::set_color(f, c, c, c);
        c += 2;
        if (f > 40) c++;
    }
}
static void _make_default_colors_darker() {
    Fl::set_color(FL_GREEN, fl_darker(Fl::get_color(FL_GREEN)));
    Fl::set_color(FL_DARK_GREEN, fl_darker(Fl::get_color(FL_DARK_GREEN)));
    Fl::set_color(FL_RED, fl_darker(Fl::get_color(FL_RED)));
    Fl::set_color(FL_DARK_RED, fl_darker(Fl::get_color(FL_DARK_RED)));
    Fl::set_color(FL_YELLOW, fl_darker(Fl::get_color(FL_YELLOW)));
    Fl::set_color(FL_DARK_YELLOW, fl_darker(Fl::get_color(FL_DARK_YELLOW)));
    Fl::set_color(FL_BLUE, fl_darker(Fl::get_color(FL_BLUE)));
    Fl::set_color(FL_DARK_BLUE, fl_darker(Fl::get_color(FL_DARK_BLUE)));
    Fl::set_color(FL_CYAN, fl_darker(Fl::get_color(FL_CYAN)));
    Fl::set_color(FL_DARK_CYAN, fl_darker(Fl::get_color(FL_DARK_CYAN)));
    Fl::set_color(FL_MAGENTA, fl_darker(Fl::get_color(FL_MAGENTA)));
    Fl::set_color(FL_DARK_MAGENTA, fl_darker(Fl::get_color(FL_DARK_MAGENTA)));
}
static void _restore_colors() {
    if (_SAVED_COLOR == true) {
        for (int f = 0; f < 256; f++) {
            Fl::set_color(f, theme::_OLD_R[f], theme::_OLD_G[f], theme::_OLD_B[f]);
        }
    }
}
static void _save_colors() {
    if (_SAVED_COLOR == false) {
        for (int f = 0; f < 256; f++) {
            unsigned char r1, g1, b1;
            Fl::get_color(f, r1, g1, b1);
            theme::_OLD_R[f] = r1;
            theme::_OLD_G[f] = g1;
            theme::_OLD_B[f] = b1;
        }
        _SAVED_COLOR = true;
    }
}
static void _tan_colors() {
    Fl::set_color(0,     0,   0,   0);
    Fl::set_color(7,   255, 255, 255);
    Fl::set_color(8,    85,  85,  85);
    Fl::set_color(15,  188, 114,  50);
    Fl::background(206, 202, 187);
}
void _load_default() {
    theme::_save_colors();
    theme::_restore_colors();
    theme::_additional_colors(false);
    Fl::scheme("none");
    Fl::redraw();
    flw::PREF_THEME = flw::PREF_THEMES[theme::THEME_DEFAULT];
    _IS_DARK = false;
}
void _load_gleam() {
    theme::_save_colors();
    theme::_restore_colors();
    theme::_additional_colors(false);
    Fl::scheme("gleam");
    Fl::redraw();
    flw::PREF_THEME = flw::PREF_THEMES[theme::THEME_GLEAM];
    _IS_DARK = false;
}
void _load_gleam_blue() {
    theme::_save_colors();
    theme::_restore_colors();
    theme::_make_default_colors_darker();
    theme::_blue_colors();
    theme::_additional_colors(true);
    Fl::set_color(255, 101, 117, 125);
    Fl::scheme("gleam");
    Fl::redraw();
    flw::PREF_THEME = flw::PREF_THEMES[theme::THEME_GLEAM_BLUE];
    _IS_DARK = true;
}
void _load_gleam_dark() {
    theme::_save_colors();
    theme::_restore_colors();
    theme::_make_default_colors_darker();
    theme::_dark_colors();
    theme::_additional_colors(true);
    Fl::set_color(255, 112, 112, 112);
    Fl::scheme("gleam");
    Fl::redraw();
    flw::PREF_THEME = flw::PREF_THEMES[theme::THEME_GLEAM_DARK];
    _IS_DARK = true;
}
void _load_gleam_tan() {
    theme::_save_colors();
    theme::_restore_colors();
    theme::_tan_colors();
    theme::_additional_colors(false);
    Fl::scheme("gleam");
    Fl::redraw();
    flw::PREF_THEME = flw::PREF_THEMES[theme::THEME_GLEAM_TAN];
    _IS_DARK = false;
}
void _load_gtk() {
    theme::_save_colors();
    theme::_restore_colors();
    theme::_additional_colors(false);
    Fl::scheme("gtk+");
    Fl::redraw();
    flw::PREF_THEME = flw::PREF_THEMES[theme::THEME_GTK];
    _IS_DARK = false;
}
void _load_gtk_blue() {
    theme::_save_colors();
    theme::_restore_colors();
    theme::_make_default_colors_darker();
    theme::_blue_colors();
    theme::_additional_colors(true);
    Fl::set_color(255, 101, 117, 125);
    Fl::scheme("gtk+");
    Fl::redraw();
    flw::PREF_THEME = flw::PREF_THEMES[theme::THEME_GTK_BLUE];
    _IS_DARK = true;
}
void _load_gtk_dark() {
    theme::_save_colors();
    theme::_restore_colors();
    theme::_make_default_colors_darker();
    theme::_dark_colors();
    theme::_additional_colors(true);
    Fl::set_color(255, 112, 112, 112);
    Fl::scheme("gtk+");
    Fl::redraw();
    flw::PREF_THEME = flw::PREF_THEMES[theme::THEME_GTK_DARK];
    _IS_DARK = true;
}
void _load_gtk_tan() {
    theme::_save_colors();
    theme::_restore_colors();
    theme::_tan_colors();
    theme::_additional_colors(false);
    Fl::scheme("gtk+");
    Fl::redraw();
    flw::PREF_THEME = flw::PREF_THEMES[theme::THEME_GTK_TAN];
    _IS_DARK = false;
}
void _load_oxy() {
    theme::_save_colors();
    theme::_restore_colors();
    theme::_additional_colors(false);
    Fl::scheme("oxy");
    Fl::redraw();
    flw::PREF_THEME = flw::PREF_THEMES[theme::THEME_OXY];
    _IS_DARK = false;
}
void _load_oxy_tan() {
    theme::_save_colors();
    theme::_restore_colors();
    theme::_tan_colors();
    theme::_additional_colors(false);
    Fl::scheme("oxy");
    Fl::redraw();
    flw::PREF_THEME = flw::PREF_THEMES[theme::THEME_OXY_TAN];
    _IS_DARK = false;
}
void _load_plastic() {
    theme::_save_colors();
    theme::_restore_colors();
    theme::_additional_colors(false);
    Fl::scheme("plastic");
    Fl::redraw();
    flw::PREF_THEME = flw::PREF_THEMES[theme::THEME_PLASTIC];
    _IS_DARK = false;
}
void _load_plastic_tan() {
    theme::_save_colors();
    theme::_restore_colors();
    theme::_tan_colors();
    theme::_additional_colors(false);
    Fl::scheme("plastic");
    Fl::redraw();
    flw::PREF_THEME = flw::PREF_THEMES[theme::THEME_PLASTIC_TAN];
    _IS_DARK = false;
}
void _scrollbar() {
    if (flw::PREF_FONTSIZE < 12 || flw::PREF_FONTSIZE > 16) {
        auto f = (double) flw::PREF_FONTSIZE / 14.0;
        auto s = (int) (f * theme::_SCROLLSIZE);
        Fl::scrollbar_size(s);
    }
    else if (theme::_SCROLLSIZE > 0) {
        Fl::scrollbar_size(theme::_SCROLLSIZE);
    }
}
}
bool theme::is_dark() {
    if (flw::PREF_THEME == flw::PREF_THEMES[theme::THEME_GLEAM_BLUE] ||
        flw::PREF_THEME == flw::PREF_THEMES[theme::THEME_GLEAM_DARK] ||
        flw::PREF_THEME == flw::PREF_THEMES[theme::THEME_GTK_BLUE] ||
        flw::PREF_THEME == flw::PREF_THEMES[theme::THEME_GTK_DARK]) {
        return true;
    }
    else {
        return false;
    }
}
bool theme::load(std::string name) {
    if (theme::_SCROLLSIZE == 0) {
        theme::_SCROLLSIZE = Fl::scrollbar_size();
    }
    if (name == flw::PREF_THEMES[theme::THEME_DEFAULT] || name == flw::PREF_THEMES2[theme::THEME_DEFAULT]) {
        theme::_load_default();
    }
    else if (name == flw::PREF_THEMES[theme::THEME_GLEAM] || name == flw::PREF_THEMES2[theme::THEME_GLEAM]) {
        theme::_load_gleam();
    }
    else if (name == flw::PREF_THEMES[theme::THEME_GLEAM_BLUE] || name == flw::PREF_THEMES2[theme::THEME_GLEAM_BLUE]) {
        theme::_load_gleam_blue();
    }
    else if (name == flw::PREF_THEMES[theme::THEME_GLEAM_DARK] || name == flw::PREF_THEMES2[theme::THEME_GLEAM_DARK]) {
        theme::_load_gleam_dark();
    }
    else if (name == flw::PREF_THEMES[theme::THEME_GLEAM_TAN] || name == flw::PREF_THEMES2[theme::THEME_GLEAM_TAN]) {
        theme::_load_gleam_tan();
    }
    else if (name == flw::PREF_THEMES[theme::THEME_GTK] || name == flw::PREF_THEMES2[theme::THEME_GTK]) {
        theme::_load_gtk();
    }
    else if (name == flw::PREF_THEMES[theme::THEME_GTK_BLUE] || name == flw::PREF_THEMES2[theme::THEME_GTK_BLUE]) {
        theme::_load_gtk_blue();
    }
    else if (name == flw::PREF_THEMES[theme::THEME_GTK_DARK] || name == flw::PREF_THEMES2[theme::THEME_GTK_DARK]) {
        theme::_load_gtk_dark();
    }
    else if (name == flw::PREF_THEMES[theme::THEME_GTK_TAN] || name == flw::PREF_THEMES2[theme::THEME_GTK_TAN]) {
        theme::_load_gtk_tan();
    }
    else if (name == flw::PREF_THEMES[theme::THEME_OXY] || name == flw::PREF_THEMES2[theme::THEME_OXY]) {
        theme::_load_oxy();
    }
    else if (name == flw::PREF_THEMES[theme::THEME_OXY_TAN] || name == flw::PREF_THEMES2[theme::THEME_OXY_TAN]) {
        theme::_load_oxy_tan();
    }
    else if (name == flw::PREF_THEMES[theme::THEME_PLASTIC] || name == flw::PREF_THEMES2[theme::THEME_PLASTIC]) {
        theme::_load_plastic();
    }
    else if (name == flw::PREF_THEMES[theme::THEME_PLASTIC_TAN] || name == flw::PREF_THEMES2[theme::THEME_PLASTIC_TAN]) {
        theme::_load_plastic_tan();
    }
    else {
        return false;
    }
    theme::_scrollbar();
    return true;
}
int theme::load_font(std::string requested_font) {
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
void theme::load_fonts(bool iso8859_only) {
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
void theme::load_icon(Fl_Window* win, int win_resource, const char** xpm_resource, const char* name) {
    assert(win);
    if (win->shown() != 0) {
        fl_alert("%s", "warning: load icon before showing window!");
    }
#if defined(_WIN32)
    win->icon(reinterpret_cast<char*>(LoadIcon(fl_display, MAKEINTRESOURCE(win_resource))));
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
void theme::load_theme_pref(Fl_Preferences& pref) {
    auto val = 0;
    char buffer[4000];
    pref.get("regular_name", buffer, "", 4000);
    if (*buffer != 0 && strcmp("FL_HELVETICA", buffer) != 0) {
        auto font = theme::load_font(buffer);
        if (font != -1) {
            flw::PREF_FONT     = font;
            flw::PREF_FONTNAME = buffer;
        }
    }
    pref.get("regular_size", val, flw::PREF_FONTSIZE);
    if (val >= 6 && val <= 72) {
        flw::PREF_FONTSIZE = val;
    }
    pref.get("mono_name", buffer, "", 1000);
    if (*buffer != 0 && strcmp("FL_COURIER", buffer) != 0) {
        auto font = theme::load_font(buffer);
        if (font != -1) {
            flw::PREF_FIXED_FONT     = font;
            flw::PREF_FIXED_FONTNAME = buffer;
        }
    }
    pref.get("mono_size", val, flw::PREF_FIXED_FONTSIZE);
    if (val >= 6 && val <= 72) {
        flw::PREF_FIXED_FONTSIZE = val;
    }
    pref.get("theme", buffer, "oxy", 4000);
    theme::load(buffer);
    Fl_Tooltip::font(flw::PREF_FONT);
    Fl_Tooltip::size(flw::PREF_FONTSIZE);
    _scrollbar();
}
void theme::load_win_pref(Fl_Preferences& pref, Fl_Window* window, int show_0_1_2, int defw, int defh, std::string basename) {
    assert(window);
    int  x, y, w, h, f, m;
    pref.get((basename + "x").c_str(), x, 80);
    pref.get((basename + "y").c_str(), y, 60);
    pref.get((basename + "w").c_str(), w, defw);
    pref.get((basename + "h").c_str(), h, defh);
    pref.get((basename + "fullscreen").c_str(), f, 0);
    pref.get((basename + "maximized").c_str(), m, 0);
    if (w == 0 || h == 0) {
        w = 800;
        h = 600;
    }
    if (x + w <= 0 || y + h <= 0 || x >= Fl::w() || y >= Fl::h()) {
        x = 80;
        y = 60;
    }
    if (show_0_1_2 > 1 && window->shown() == 0) {
        window->show();
    }
    window->resize(x, y, w, h);
    if (f == 1) {
        window->fullscreen();
    }
    else if (m == 1) {
        window->maximize();
    }
    if (show_0_1_2 == 1 && window->shown() == 0) {
        window->show();
    }
}
bool theme::parse(int argc, const char** argv) {
    auto res = false;
    for (auto f = 1; f < argc; f++) {
        if (res == false) {
            res = theme::load(argv[f]);
        }
        auto fontsize = atoi(argv[f]);
        if (fontsize >= 6 && fontsize <= 72) {
            flw::PREF_FONTSIZE = fontsize;
        }
    }
    flw::PREF_FIXED_FONTSIZE = flw::PREF_FONTSIZE;
    Fl_Tooltip::font(flw::PREF_FONT);
    Fl_Tooltip::size(flw::PREF_FONTSIZE);
    return res;
}
void theme::save_theme_pref(Fl_Preferences& pref) {
    pref.set("theme", flw::PREF_THEME.c_str());
    pref.set("regular_name", flw::PREF_FONTNAME.c_str());
    pref.set("regular_size", flw::PREF_FONTSIZE);
    pref.set("mono_name", flw::PREF_FIXED_FONTNAME.c_str());
    pref.set("mono_size", flw::PREF_FIXED_FONTSIZE);
}
void theme::save_win_pref(Fl_Preferences& pref, Fl_Window* window, std::string basename) {
    assert(window);
    pref.set((basename + "x").c_str(), window->x());
    pref.set((basename + "y").c_str(), window->y());
    pref.set((basename + "w").c_str(), window->w());
    pref.set((basename + "h").c_str(), window->h());
    pref.set((basename + "fullscreen").c_str(), window->fullscreen_active() ? 1 : 0);
    pref.set((basename + "maximized").c_str(), window->maximize_active() ? 1 : 0);
}
Buf::Buf() {
    p = nullptr;
    s = 0;
}
Buf::Buf(size_t S) {
    p = (S < SIZE_MAX) ? static_cast<char*>(calloc(S + 1, 1)) : nullptr;
    s = 0;
    if (p != nullptr) {
        s = S;
    }
}
Buf::Buf(const char* P, size_t S, bool grab) {
    if (grab == true) {
        p = const_cast<char*>(P);
        s = S;
    }
    else if (P == nullptr) {
        p = nullptr;
        s = 0;
    }
    else {
        p = static_cast<char*>(calloc(S + 1, 1));
        s = 0;
        if (p == nullptr) {
            return;
        }
        memcpy(p, P, S);
        s = S;
    }
}
Buf::Buf(const Buf& b) {
    if (b.p == nullptr) {
        p = nullptr;
        s = 0;
    }
    else {
        p = static_cast<char*>(calloc(b.s + 1, 1));
        s = 0;
        if (p == nullptr) {
            return;
        }
        memcpy(p, b.p, b.s);
        s = b.s;
    }
}
Buf::Buf(Buf&& b) {
    p = b.p;
    s = b.s;
    b.p = nullptr;
}
Buf& Buf::operator=(const Buf& b) {
    if (this == &b) {
    }
    if (b.p == nullptr) {
        free(p);
        p = nullptr;
        s = 0;
    }
    else {
        free(p);
        p = static_cast<char*>(calloc(b.s + 1, 1));
        s = 0;
        if (p == nullptr) {
            return *this;
        }
        memcpy(p, b.p, b.s);
        s = b.s;
    }
    return *this;
}
Buf& Buf::operator=(Buf&& b) {
    free(p);
    p = b.p;
    s = b.s;
    b.p = nullptr;
    return *this;
}
Buf& Buf::operator+=(const Buf& b) {
    if (b.p == nullptr) {
    }
    else if (p == nullptr) {
        *this = b;
    }
    else {
        auto t = static_cast<char*>(calloc(s + b.s + 1, 1));
        if (t == nullptr) {
            return *this;
        }
        memcpy(t, p, s);
        memcpy(t + s, b.p, b.s);
        free(p);
        p = t;
        s += b.s;
    }
    return *this;
}
bool Buf::operator==(const Buf& other) const {
    return p != nullptr && s == other.s && memcmp(p, other.p, s) == 0;
}
File::File(std::string filename) {
    size  = 0;
    mtime = 0;
    type  = TYPE::NA;
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
            type = TYPE::DIR;
        }
        else if (S_ISREG(st.st_mode)) {
            type = TYPE::FILE;
        }
        else {
            type = TYPE::OTHER;
        }
    }
#else
    struct stat st;
    if (stat(filename.c_str(), &st) == 0) {
        size  = st.st_size;
        mtime = st.st_mtime;
        if (S_ISDIR(st.st_mode)) {
            type = TYPE::DIR;
        }
        else if (S_ISREG(st.st_mode)) {
            type = TYPE::FILE;
        }
        else {
            type = TYPE::OTHER;
        }
    }
#endif
}
Buf File::Load(std::string filename, bool alert) {
    auto file = File(filename);
    if (file.type != TYPE::FILE) {
        if (alert == true) {
            fl_alert("error: file %s is missing or not an file", filename.c_str());
        }
        return Buf();
    }
    auto handle = fl_fopen(filename.c_str(), "rb");
    if (handle == nullptr) {
        if (alert == true) {
            fl_alert("error: can't open %s", filename.c_str());
        }
        return Buf();
    }
    auto buf  = Buf(file.size);
    auto read = fread(buf.p, 1, (size_t) file.size, handle);
    fclose(handle);
    if (read != (size_t) file.size) {
        if (alert == true) {
            fl_alert("error: failed to read %s", filename.c_str());
        }
        return Buf();
    }
    return buf;
}
bool File::Save(std::string filename, const char* data, size_t size, bool alert) {
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
PrintText::PrintText(std::string filename,
    Fl_Paged_Device::Page_Format page_format,
    Fl_Paged_Device::Page_Layout page_layout,
    Fl_Font font,
    Fl_Fontsize fontsize,
    Fl_Align align,
    bool wrap,
    bool border,
    int line_num) {
    _align       = FL_ALIGN_INSIDE | FL_ALIGN_TOP;
    _align      |= (align == FL_ALIGN_CENTER || align == FL_ALIGN_RIGHT) ? align : FL_ALIGN_LEFT;
    _border      = border;
    _file        = nullptr;
    _filename    = filename;
    _font        = font;
    _fontsize    = fontsize;
    _line_num    = (align == FL_ALIGN_LEFT) ? line_num : 0;
    _page_format = page_format;
    _page_layout = page_layout;
    _printer     = nullptr;
    _wrap        = wrap;
}
PrintText::~PrintText() {
    stop();
}
void PrintText::check_for_new_page() {
    if (_py + _lh > _ph || _page_count == 0) {
        if (_page_count > 0) {
            fl_pop_clip();
            if (_printer->end_page() != 0) {
                throw "error: couldn't end current page";
            }
        }
        if (_printer->begin_page() != 0) {
            throw "error: couldn't create new page!";
        }
        if (_printer->printable_rect(&_pw, &_ph) != 0) {
            throw "error: couldn't retrieve page size!";
        }
        fl_font(_font, _fontsize);
        fl_color(FL_BLACK);
        fl_line_style(FL_SOLID, 1);
        fl_push_clip(0, 0, _pw, _ph);
        if (_border == false) {
            _px = 0;
            _py = 0;
        }
        else {
            fl_rect(0, 0, _pw, _ph);
            measure_lw_lh("M");
            _px  = _lw;
            _py  = _lh;
            _pw -= _lw * 2;
            _ph -= _lh * 2;
        }
        _page_count++;
    }
}
void PrintText::measure_lw_lh(const std::string& text) {
    _lw = _lh = 0;
    fl_measure(text.c_str(), _lw, _lh, 0);
}
std::string PrintText::print(const char* text, unsigned replace_tab_with_space) {
    return print(util::split_string(text, "\n"), replace_tab_with_space);
}
std::string PrintText::print(const std::string& text, unsigned replace_tab_with_space) {
    return print(util::split_string(text.c_str(), "\n"), replace_tab_with_space);
}
std::string PrintText::print(const StringVector& lines, unsigned replace_tab_with_space) {
    std::string res;
    std::string tab;
    while (replace_tab_with_space > 0 && replace_tab_with_space <= 16) {
        tab += " ";
        replace_tab_with_space--;
    }
    try {
        auto wc = WaitCursor();
        res = start();
        if (res == "") {
            for (auto& line : lines) {
                if (tab != "") {
                    auto l = line;
                    util::replace_string(l, "\t", "    ");
                    print_line(l == "" ? " " : l);
                }
                else {
                    print_line(line == "" ? " " : line);
                }
            }
            res = stop();
        }
    }
    catch (const char* ex) {
        res = ex;
    }
    catch (...) {
        res = "error: unknown exception!";
    }
    return res;
}
void PrintText::print_line(const std::string& line) {
    _line_count++;
    check_for_new_page();
    if (_line_num > 0) {
        auto num = util::format("%*d: ", _line_num, _line_count);
        measure_lw_lh(num);
        fl_draw(num.c_str(), _px, _py, _pw, _lh, _align, nullptr, 0);
        _nw = _lw;
    }
    measure_lw_lh(line);
    if (_wrap == true && _lw > _pw - _nw) {
        print_wrapped_line(line);
    }
    else {
        fl_draw(line.c_str(), _px + _nw, _py, _pw - _nw, _lh, _align, nullptr, 0);
        _py += _lh;
    }
}
void PrintText::print_wrapped_line(const std::string& line) {
    auto p1 = line.c_str();
    auto s1 = std::string();
    auto s2 = std::string();
    while (*p1 != 0) {
        auto cl = fl_wcwidth(p1);
        if (cl > 1) {
            for (auto f = 0; f < cl && *p1 != 0; f++) {
                s1 += *p1;
                p1++;
            }
        }
        else {
            s1 += *p1;
            p1++;
        }
        auto c = s1.back();
        if (c == ' ' || c == '\t' || c == ',' || c == ';' || c == '.') {
            s2 = s1;
        }
        measure_lw_lh(s1);
        if (_lw >= _pw - _nw) {
            check_for_new_page();
            if (s2 != "") {
                fl_draw(s2.c_str(), _px + _nw, _py, _pw - _nw, _lh, _align, nullptr, 0);
                s1 = (s2.length() < s1.length()) ? s1.substr(s2.length()) : "";
                s2 = "";
            }
            else {
                std::string s;
                if (s1.length() > 1) {
                    s  = s1.substr(s1.length() - 1);
                    s1 = s1.substr(0, s1.length() - 1);
                }
                fl_draw(s1.c_str(), _px + _nw, _py, _pw - _nw, _lh, _align, nullptr, 0);
                s1 = s;
            }
            _py += _lh;
        }
    }
    if (s1 != "") {
        check_for_new_page();
        fl_draw(s1.c_str(), _px + _nw, _py, _pw - _nw, _lh, _align, nullptr, 0);
        _py += _lh;
    }
}
std::string PrintText::start() {
    if ((_file = fl_fopen(_filename.c_str(), "wb")) == nullptr) {
        return "error: could not open file!";
    }
    _lh         = 0;
    _line_count = 0;
    _lw         = 0;
    _nw         = 0;
    _page_count = 0;
    _ph         = 0;
    _pw         = 0;
    _px         = 0;
    _py         = 0;
    _printer    = new Fl_PostScript_File_Device();
    _printer->begin_job(_file, 0, _page_format, _page_layout);
    return "";
}
std::string PrintText::stop() {
    std::string res = "";
    if (_printer != nullptr) {
        if (_page_count > 0) {
            fl_pop_clip();
            if (_printer->end_page() != 0) {
                res = "error: could not end page!";
            }
        }
        _printer->end_job();
        delete _printer;
        fclose(_file);
        _file    = nullptr;
        _printer = nullptr;
    }
    return res;
}
}
#include <stdarg.h>
namespace flw {
    static int _FLW_GRID_STRING_SIZE = 1000;
}
flw::Grid::Grid(int rows, int cols, int X, int Y, int W, int H, const char* l) : flw::TableEditor(X, Y, W, H, l) {
    rows    = rows < 1 ? 1 : rows;
    cols    = cols < 1 ? 1 : cols;
    _buffer = static_cast<char*>(calloc(_FLW_GRID_STRING_SIZE + 1, 1));
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
    Fl_Widget*                  widget;
    short                       x;
    short                       y;
    short                       w;
    short                       h;
    short                       l;
    short                       r;
    short                       t;
    short                       b;
    _GridGroupChild(Fl_Widget* WIDGET, int X, int Y, int W, int H) {
        set(WIDGET, X, Y, W, H);
        adjust();
    }
    void adjust(int L = 0, int R = 0, int T = 0, int B = 0) {
        l = L;
        r = R;
        t = T;
        b = B;
    }
    void set(Fl_Widget* WIDGET, int X, int Y, int W, int H) {
        widget = WIDGET;
        x      = X;
        y      = Y;
        w      = W;
        h      = H;
    }
};
GridGroup::GridGroup(int X, int Y, int W, int H, const char* l) : Fl_Group(X, Y, W, H, l) {
    end();
    clip_children(1);
    resizable(nullptr);
    _size = 0;
}
GridGroup::~GridGroup() {
    for (auto v : _widgets) {
        delete static_cast<_GridGroupChild*>(v);
    }
}
void GridGroup::add(Fl_Widget* widget, int X, int Y, int W, int H) {
    _widgets.push_back(new _GridGroupChild(widget, X, Y, W, H));
    Fl_Group::add(widget);
}
void GridGroup::adjust(Fl_Widget* widget, int L, int R, int T, int B) {
    for (auto& v : _widgets) {
        auto child = static_cast<_GridGroupChild*>(v);
        if (child->widget == widget) {
            child->adjust(L, R, T, B);
            return;
        }
    }
    #ifdef DEBUG
        fprintf(stderr, "error: flw::GridGroup::adjust() failed to find widget (label=%s)\n", widget->label());
    #endif
}
void GridGroup::clear() {
    _widgets.clear();
    Fl_Group::clear();
}
int GridGroup::handle(int event) {
    if (event == FL_KEYDOWN && Fl::event_key() == FL_Tab) {
        if (children() > 0) {
            Fl_Widget* first   = nullptr;
            Fl_Widget* last    = nullptr;
            Fl_Widget* current = Fl::focus();
            _last_active_widget(&first, &last);
            if (Fl::event_shift() == 0) {
                if (first != nullptr && current != nullptr && current == last) {
                    Fl::focus(first);
                    first->redraw();
                    return 1;
                }
            }
            else {
                if (first != nullptr && current != nullptr && current == first) {
                    Fl::focus(last);
                    last->redraw();
                    return 1;
                }
            }
        }
    }
    return Fl_Group::handle(event);
}
void GridGroup::_last_active_widget(Fl_Widget** first, Fl_Widget** last) {
    for (int f = 0; f < children(); f++) {
        auto c = child(f);
        auto g = c->as_group();
        if (g == nullptr) {
            if (c->active() != 0) {
                *last = c;
            }
            if (*first == nullptr && c->active() != 0) {
                *first = c;
            }
        }
    }
}
Fl_Widget* GridGroup::remove(Fl_Widget* widget) {
    for (auto it = _widgets.begin(); it != _widgets.end(); it++) {
        auto child = static_cast<_GridGroupChild*>(*it);
        if (child->widget == widget) {
            delete child;
            Fl_Group::remove(widget);
            _widgets.erase(it);
            return widget;
        }
    }
    #ifdef DEBUG
        fprintf(stderr, "error: GridGroup::remove can't find widget\n");
    #endif
    return nullptr;
}
void GridGroup::resize(int X, int Y, int W, int H) {
    Fl_Widget::resize(X, Y, W, H);
    if (children() == 0 || W == 0 || H == 0 || visible() == 0) {
        return;
    }
    int size = (_size > 0) ? _size : flw::PREF_FONTSIZE / 2;
    for (const auto& v : _widgets) {
        auto child = static_cast<_GridGroupChild*>(v);
        if (child->widget != nullptr && child->widget->visible() != 0) {
            int widget_x  = 0;
            int widget_x2 = 0;
            int widget_y  = 0;
            int widget_y2 = 0;
            int widget_w  = 0;
            int widget_h  = 0;
            if (child->x >= 0) {
                widget_x = X + child->x * size;
            }
            else {
                widget_x = X + W + child->x * size;
            }
            if (child->y >= 0) {
                widget_y = Y + child->y * size;
            }
            else {
                widget_y = Y + H + child->y * size;
            }
            if (child->w == 0) {
                widget_x2 = X + W;
            }
            else if (child->w > 0) {
                widget_x2 = widget_x + child->w * size;
            }
            else {
                widget_x2 = X + W + child->w * size;
            }
            if (child->h == 0) {
                widget_y2 = Y + H;
            }
            else if (child->h > 0) {
                widget_y2 = widget_y + child->h * size;
            }
            else {
                widget_y2 = Y + H + child->h * size;
            }
            widget_w = widget_x2 - widget_x;
            widget_h = widget_y2 - widget_y;
            if (widget_w >= 0 && widget_h >= 0) {
                child->widget->resize(widget_x + child->l, widget_y + child->t, widget_w + child->r, widget_h + child->b);
            }
            else {
                child->widget->resize(0, 0, 0, 0);
            }
        }
    }
}
void GridGroup::resize(Fl_Widget* widget, int X, int Y, int W, int H) {
    for (auto& v : _widgets) {
        auto child = static_cast<_GridGroupChild*>(v);
        if (child->widget == widget) {
            child->set(widget, X, Y, W, H);
            return;
        }
    }
    #ifdef DEBUG
        fprintf(stderr, "error: flw::GridGroup::resize() failed to find widget (label=%s)\n", widget->label());
    #endif
}
}
#include <algorithm>
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
InputMenu::InputMenu(int X, int Y, int W, int H, const char* l) : Fl_Group(X, Y, W, H, l) {
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
void InputMenu::Callback(Fl_Widget* w, void* o) {
    auto self = static_cast<InputMenu*>(o);
    if (w == self->_input) {
        if (self->_input->show_menu) {
            if (self->_menu->visible()) {
                self->_menu->popup();
            }
        }
        else {
            self->do_callback();
        }
    }
    else if (w == self->_menu) {
        auto index = self->_menu->find_index(self->_menu->text());
        if (index >= 0 && index < (int) self->_input->history.size()) {
            self->_input->value(self->_input->history[index].c_str());
            self->_input->index = index;
        }
        self->_input->take_focus();
    }
}
void InputMenu::clear() {
    _menu->clear();
    _input->history.clear();
    _input->index = -1;
}
flw::StringVector InputMenu::get_history() const {
    return _input->history;
}
void InputMenu::insert(std::string string, int max_list_len) {
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
void InputMenu::resize(int X, int Y, int W, int H) {
    Fl_Widget::resize(X, Y, W, H);
    if (_menu->visible() != 0) {
        auto mw = (int) flw::PREF_FONTSIZE;
        _input->resize(X, Y, W - flw::PREF_FONTSIZE - mw, H);
        _menu->resize(X + W - flw::PREF_FONTSIZE - mw, Y, flw::PREF_FONTSIZE + mw, H);
    }
    else {
        _input->resize(X, Y, W, H);
    }
}
void InputMenu::update_pref(Fl_Font text_font, Fl_Fontsize text_size) {
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
const char* InputMenu::value() const {
    return _input->value();
}
void InputMenu::value(const char* string) {
    _input->value(string ? string : "");
}
void InputMenu::values(const StringVector& list, bool copy_first_to_input) {
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
}
#include <cstring>
#include <cmath>
#include <cstdint>
#include <errno.h>
namespace flw {
#define _FLW_JSON_ERROR(X,Y) _json_format_error(__LINE__, (unsigned) (X), Y)
#define _FLW_JSON_FREE_STRINGS(X,Y) free(X); free(Y); X = Y = nullptr;
static const char* const _JSON_BOM = "\xef\xbb\xbf";
static void _json_debug(const JS* js, std::string t) {
    if (js->is_array() == true) {
        printf("%sARRAY(%u, %u): \"%s\"\n", t.c_str(), js->pos(), (unsigned) js->size(), js->name().c_str());
        t += "\t";
        for (const auto js2 : *js->va()) _json_debug(js2, t);
        t.pop_back();
    }
    else if (js->is_object() == true) {
        printf("%sOBJECT(%u, %u): \"%s\"\n", t.c_str(), js->pos(), (unsigned) js->size(), js->name().c_str());
        t += "\t";
        for (auto js2 : *js->vo()) _json_debug(js2.second, t);
        t.pop_back();
    }
    else if (js->is_null()) printf("%s%s(%u): \"%s\"\n", t.c_str(), js->type_name().c_str(), js->pos(), js->name_c());
    else if (js->is_string()) printf("%s%s(%u): \"%s\": \"%s\"\n", t.c_str(), js->type_name().c_str(), js->pos(), js->name_c(), js->vs_c());
    else if (js->is_number()) printf("%s%s(%u): \"%s\": %f\n", t.c_str(), js->type_name().c_str(), js->pos(), js->name_c(), js->vn());
    else if (js->is_bool()) printf("%s%s(%u): \"%s\": %s\n", t.c_str(), js->type_name().c_str(), js->pos(), js->name_c(), js->vb() ? "true" : "false");
    fflush(stdout);
}
static std::string _json_format_error(unsigned source, unsigned pos, unsigned line) {
    char buf[256];
    snprintf(buf, 256, "error: invalid json (%u) at pos %u and line %u", source, pos, line);
    return buf;
}
static bool _json_parse_number(const char* json, size_t len, size_t& pos, double& nVal) {
    bool        res = false;
    std::string n1;
    std::string n2;
    nVal = NAN;
    while (pos < len) {
        unsigned char c = json[pos];
        if (c == '-' || c == '.' || (c >= '0' && c <= '9')) {
            n1 += c;
            pos++;
        }
        else {
            break;
        }
    }
    while (pos < len) {
        unsigned char c = json[pos];
        if (c == 'e' || c == 'E' || c == '-' || c == '+' || (c >= '0' && c <= '9')) {
            n2 += c;
            pos++;
        }
        else {
            break;
        }
    }
    int dot1  = 0;
    int dot2  = 0;
    int minus = 0;
    int plus  = 0;
    int E     = 0;
    int term  = json[pos];
    for (auto c : n1) {
        dot1  += (c =='.');
        minus += (c =='-');
        plus  += (c =='+');
        E     += (c =='e');
        E     += (c =='E');
    }
    if (dot1 > 1 || minus > 1 || n1 == "-") {
        return false;
    }
    else if (n1.back() == '.') {
        return false;
    }
    else if (n1[0] == '-' && n1[1] == '0' && n1[2] != '.' && n1[2] != 0) {
        return false;
    }
    else if (n1[0] == '-' && n1[1] == '.') {
        return false;
    }
    else if (n1[0] == '0' && n1[1] != 0 && n1[1] != '.') {
        return false;
    }
    else if (minus > 0 && n1[0] != '-') {
        return false;
    }
    if (n2 != "") {
        if (n2.length() == 1) {
            return false;
        }
        else {
            dot2 = minus = plus = E = 0;
            for (auto c : n2) {
                dot2  += (c =='.');
                minus += (c =='-');
                plus  += (c =='+');
                E     += (c =='e');
                E     += (c =='E');
            }
            if (plus + minus > 1 || E > 1) {
                return false;
            }
            else if (plus > 0 && n2.back() == '+') {
                return false;
            }
            else if (plus > 0 && n2[1] != '+') {
                return false;
            }
            else if (minus > 0 && n2.back() == '-') {
                return false;
            }
            else if (minus > 0 && n2[1] != '-') {
                return false;
            }
        }
    }
    if (term > 32 && term != ',' && term != ':' && term != '}' && term != ']' && term != '{' && term != '[') {
        return false;
    }
    errno = 0;
    if (E > 0 || dot1 > 0) {
        nVal = strtod((n1 + n2).c_str(), nullptr);
    }
    else {
        nVal = strtoll((n1 + n2).c_str(), nullptr, 0);
    }
    res = (errno == 0);
    pos--;
    return res;
}
static bool _json_parse_string(bool ignore_utf_check, const char* json, size_t len, size_t& pos, char** sVal1, char** sVal2) {
    std::string str   = "";
    bool        term  = false;
    unsigned    c     = 0;
    unsigned    p     = 0;
    pos++;
    while (pos < len) {
        c = json[pos];
        if (p == '\\' && c == '\\') {
            str += c;
            c = 0;
        }
        else if (p == '\\' && c == '"') {
            str += c;
        }
        else if (c == '"') {
            pos++;
            term = true;
            break;
        }
        else if (c < 32) {
            return false;
        }
        else {
            str += c;
        }
        p = c;
        pos++;
    }
    auto ulen = (ignore_utf_check == false) ? JS::CountUtf8(str.c_str()) : 1;
    if (term == false) {
        return false;
    }
    else if (ulen == 0 && str.length() > 0) {
        return false;
    }
    if (*sVal1 == nullptr) {
        *sVal1 = strdup(str.c_str());
    }
    else if (*sVal2 == nullptr) {
        *sVal2 = strdup(str.c_str());
    }
    else {
        return false;
    }
    pos--;
    return true;
}
ssize_t JS::COUNT = 0;
bool JS::_add_bool(char** sVal1, bool b, bool ignore_duplicates, unsigned pos) {
    bool res = false;
    if (is_array() == true) {
        _va->push_back(JS::_MakeBool("", b, this, pos));
        res = true;
    }
    else if (is_object() == true) {
        res = _set_object(*sVal1, JS::_MakeBool(*sVal1, b, this, pos), ignore_duplicates);
    }
    free(*sVal1);
    *sVal1 = nullptr;
    return res;
}
bool JS::_add_nil(char** sVal1, bool ignore_duplicates, unsigned pos) {
    bool res = false;
    if (is_array() == true) {
        _va->push_back(JS::_MakeNil("", this, pos));
        res = true;
    }
    else if (is_object() == true) {
        res = _set_object(*sVal1, JS::_MakeNil(*sVal1, this, pos), ignore_duplicates);
    }
    free(*sVal1);
    *sVal1 = nullptr;
    return res;
}
bool JS::_add_number(char** sVal1, double& nVal, bool ignore_duplicates, unsigned pos) {
    bool res = false;
    if (is_array() == true && std::isnan(nVal) == false) {
        _va->push_back(JS::_MakeNumber("", nVal, this, pos));
        res = true;
    }
    else if (is_object() == true && std::isnan(nVal) == false) {
        res = _set_object(*sVal1, JS::_MakeNumber(*sVal1, nVal, this, pos), ignore_duplicates);
    }
    free(*sVal1);
    *sVal1 = nullptr;
    nVal = NAN;
    return res;
}
bool JS::_add_string(char** sVal1, char** sVal2, bool ignore_duplicates, unsigned pos) {
    bool res = false;
    if (is_array() == true && *sVal1 != nullptr && *sVal2 == nullptr) {
        _va->push_back(JS::_MakeString("", *sVal1, this, pos));
        res = true;
    }
    else if (is_object() == true && *sVal1 != nullptr && *sVal2 != nullptr) {
        res = _set_object(*sVal1, JS::_MakeString(*sVal1, *sVal2, this, pos), ignore_duplicates);
    }
    free(*sVal1);
    free(*sVal2);
    *sVal1 = nullptr;
    *sVal2 = nullptr;
    return res;
}
void JS::_clear(bool name) {
    if (_type == JS::ARRAY) {
        for (auto js : *_va) {
            delete js;
        }
        delete _va;
        _va = nullptr;
    }
    else if (_type == JS::OBJECT) {
        for (auto js : *_vo) {
            delete js.second;
        }
        delete _vo;
        _vo = nullptr;
    }
    else if (_type == JS::STRING) {
        free(_vs);
        _vs = nullptr;
    }
    if (name == true) {
        free(_name);
        _name   = nullptr;
    }
    _type   = JS::NIL;
    _vb     = false;
    _parent = nullptr;
}
size_t JS::CountUtf8(const char* p) {
    auto count = (size_t) 0;
    auto f     = (size_t) 0;
    auto u     = reinterpret_cast<const unsigned char*>(p);
    auto c     = (unsigned) u[0];
    while (c != 0) {
        if (c >= 128) {
            if (c >= 194 && c <= 223) {
                c = u[++f];
                if (c < 128 || c > 191) return 0;
            }
            else if (c >= 224 && c <= 239) {
                c = u[++f];
                if (c < 128 || c > 191) return 0;
                c = u[++f];
                if (c < 128 || c > 191) return 0;
            }
            else if (c >= 240 && c <= 244) {
                c = u[++f];
                if (c < 128 || c > 191) return 0;
                c = u[++f];
                if (c < 128 || c > 191) return 0;
                c = u[++f];
                if (c < 128 || c > 191) return 0;
            }
            else {
                return 0;
            }
        }
        count++;
        c = u[++f];
    }
    return count;
}
std::string JS::decode(const char* json, size_t len, bool ignore_trailing_comma, bool ignore_duplicates, bool ignore_utf_check) {
    auto colon   = 0;
    auto comma   = 0;
    auto count_a = 0;
    auto count_o = 0;
    auto current = (JS*) nullptr;
    auto line    = (unsigned) 1;
    auto n       = (JS*) nullptr;
    auto nVal    = (double) NAN;
    auto pos1    = (size_t) 0;
    auto pos2    = (size_t) 0;
    auto posn    = (size_t) 0;
    auto sVal1   = (char*) nullptr;
    auto sVal2   = (char*) nullptr;
    auto tmp     = JS();
    try {
        _clear(true);
        _name = strdup("");
        tmp._type = JS::ARRAY;
        tmp._va   = new JSArray();
        current   = &tmp;
        if (strncmp(json, _JSON_BOM, 3) == 0) {
            pos1 += 3;
        }
        while (pos1 < len) {
            auto start = pos1;
            auto c     = (unsigned) json[pos1];
            if (c == '\t' || c == '\r' || c == ' ') {
                pos1++;
            }
            else if (c == '\n') {
                line++;
                pos1++;
            }
            else if (c == '"') {
                pos2 = pos1;
                if (sVal1 == nullptr) posn = pos1;
                if (_json_parse_string(ignore_utf_check, json, len, pos1, &sVal1, &sVal2) == false) throw _FLW_JSON_ERROR(start, line);
                auto add_object = (current->is_object() == true && sVal2 != nullptr);
                auto add_array = (current->is_array() == true);
                if (comma > 0 && current->size() == 0) throw _FLW_JSON_ERROR(start, line);
                else if (comma == 0 && current->size() > 0) throw _FLW_JSON_ERROR(start, line);
                else if (add_object == true && colon != 1) throw _FLW_JSON_ERROR(start, line);
                else if (add_object == true || add_array == true) {
                    pos2 = (sVal2 == nullptr) ? pos2 : posn;
                    if (current->_add_string(&sVal1, &sVal2, ignore_duplicates, pos2) == false) throw _FLW_JSON_ERROR(start, line);
                    colon = 0;
                    comma = 0;
                }
                pos1++;
            }
            else if ((c >= '0' && c <= '9') || c == '-') {
                pos2 = (sVal1 == nullptr) ? pos1 : posn;
                if (_json_parse_number(json, len, pos1, nVal) == false) throw _FLW_JSON_ERROR(start, line);
                else if (comma > 0 && current->size() == 0) throw _FLW_JSON_ERROR(start, line);
                else if (comma == 0 && current->size() > 0) throw _FLW_JSON_ERROR(start, line);
                else if (current->is_object() == true && colon != 1) throw _FLW_JSON_ERROR(start, line);
                else if (current->_add_number(&sVal1, nVal, ignore_duplicates, pos2) == false) throw _FLW_JSON_ERROR(start, line);
                colon = 0;
                comma = 0;
                pos1++;
            }
            else if (c == ',') {
                if (comma > 0) throw _FLW_JSON_ERROR(pos1, line);
                else if (current == &tmp) throw _FLW_JSON_ERROR(pos1, line);
                comma++;
                pos1++;
            }
            else if (c == ':') {
                if (colon > 0) throw _FLW_JSON_ERROR(pos1, line);
                else if (current->is_object() == false) throw _FLW_JSON_ERROR(pos1, line);
                else if (sVal1 == nullptr) throw _FLW_JSON_ERROR(pos1, line);
                colon++;
                pos1++;
            }
            else if (c == '[') {
                if (current->size() == 0 && comma > 0) throw _FLW_JSON_ERROR(pos1, line);
                else if (current->size() > 0 && comma != 1) throw _FLW_JSON_ERROR(pos1, line);
                else if (current->is_array() == true) {
                    if (sVal1 != nullptr) throw _FLW_JSON_ERROR(pos1, line);
                    n = JS::_MakeArray("", current, pos1);
                    current->_va->push_back(n);
                }
                else {
                    if (sVal1 == nullptr) throw _FLW_JSON_ERROR(pos1, line);
                    else if (colon != 1) throw _FLW_JSON_ERROR(pos1, line);
                    n = JS::_MakeArray(sVal1, current, pos2);
                    if (current->_set_object(sVal1, n, ignore_duplicates) == false) throw _FLW_JSON_ERROR(pos1, line);
                    _FLW_JSON_FREE_STRINGS(sVal1, sVal2)
                }
                current = n;
                colon = 0;
                comma = 0;
                count_a++;
                pos1++;
            }
            else if (c == ']') {
                if (current->_parent == nullptr) throw _FLW_JSON_ERROR(pos1, line);
                else if (current->is_array() == false) throw _FLW_JSON_ERROR(pos1, line);
                else if (sVal1 != nullptr || sVal2 != nullptr) throw _FLW_JSON_ERROR(pos1, line);
                else if (comma > 0 && ignore_trailing_comma == false) throw _FLW_JSON_ERROR(pos1, line);
                else if (count_a < 0) throw _FLW_JSON_ERROR(pos1, line);
                current = current->_parent;
                comma = 0;
                count_a--;
                pos1++;
            }
            else if (c == '{') {
                if (current->size() == 0 && comma > 0) throw _FLW_JSON_ERROR(pos1, line);
                else if (current->size() > 0 && comma != 1) throw _FLW_JSON_ERROR(pos1, line);
                else if (current->is_array() == true) {
                    if (sVal1 != nullptr) throw _FLW_JSON_ERROR(pos1, line);
                    n = JS::_MakeObject("", current, pos1);
                    current->_va->push_back(n);
                }
                else {
                    if (sVal1 == nullptr) throw _FLW_JSON_ERROR(pos1, line);
                    else if (colon != 1) throw _FLW_JSON_ERROR(pos1, line);
                    n = JS::_MakeObject(sVal1, current, posn);
                    if (current->_set_object(sVal1, n, ignore_duplicates) == false) throw _FLW_JSON_ERROR(pos1, line);
                    _FLW_JSON_FREE_STRINGS(sVal1, sVal2)
                }
                current = n;
                colon = 0;
                comma = 0;
                count_o++;
                pos1++;
            }
            else if (c == '}') {
                if (current->_parent == nullptr) throw _FLW_JSON_ERROR(pos1, line);
                else if (current->is_object() == false) throw _FLW_JSON_ERROR(pos1, line);
                else if (sVal1 != nullptr || sVal2 != nullptr) throw _FLW_JSON_ERROR(pos1, line);
                else if (comma > 0 && ignore_trailing_comma == false) throw _FLW_JSON_ERROR(pos1, line);
                else if (count_o < 0) throw _FLW_JSON_ERROR(pos1, line);
                current = current->_parent;
                comma = 0;
                count_o--;
                pos1++;
            }
            else if ((c == 't' && json[pos1 + 1] == 'r' && json[pos1 + 2] == 'u' && json[pos1 + 3] == 'e') ||
                    (c == 'f' && json[pos1 + 1] == 'a' && json[pos1 + 2] == 'l' && json[pos1 + 3] == 's' && json[pos1 + 4] == 'e')) {
                pos2 = (sVal1 == nullptr) ? pos1 : posn;
                if (current->size() > 0 && comma == 0) throw _FLW_JSON_ERROR(start, line);
                else if (comma > 0 && current->size() == 0) throw _FLW_JSON_ERROR(start, line);
                else if (current->is_object() == true && colon != 1) throw _FLW_JSON_ERROR(start, line);
                else if (current->_add_bool(&sVal1, c == 't', ignore_duplicates, pos2) == false) throw _FLW_JSON_ERROR(start, line);
                colon = 0;
                comma = 0;
                pos1 += 4;
                pos1 += (c == 'f');
            }
            else if (c == 'n' && json[pos1 + 1] == 'u' && json[pos1 + 2] == 'l' && json[pos1 + 3] == 'l') {
                pos2 = (sVal1 == nullptr) ? pos1 : posn;
                if (current->size() > 0 && comma == 0) throw _FLW_JSON_ERROR(start, line);
                else if (comma > 0 && current->size() == 0) throw _FLW_JSON_ERROR(start, line);
                else if (current->is_object() == true && colon != 1) throw _FLW_JSON_ERROR(start, line);
                else if (current->_add_nil(&sVal1, ignore_duplicates, pos1) == false) throw _FLW_JSON_ERROR(start, line);
                colon = 0;
                comma = 0;
                pos1 += 4;
            }
            else {
                throw _FLW_JSON_ERROR(pos1, line);
            }
            if (count_a > (int) JS::MAX_DEPTH || count_o > (int) JS::MAX_DEPTH) {
                throw _FLW_JSON_ERROR(pos1, line);
            }
        }
        if (count_a != 0 || count_o != 0) {
            throw _FLW_JSON_ERROR(len, 1);
        }
        else if (tmp.size() != 1) {
            throw _FLW_JSON_ERROR(len, 1);
        }
        else if (tmp[0]->_type == JS::ARRAY) {
            _type = JS::ARRAY;
            _va = tmp[0]->_va;
            const_cast<JS*>(tmp[0])->_type = JS::NIL;
            for (auto o : *_va) {
                o->_parent = this;
            }
        }
        else if (tmp[0]->_type == JS::OBJECT) {
            _type = JS::OBJECT;
            _vo = tmp[0]->_vo;
            const_cast<JS*>(tmp[0])->_type = JS::NIL;
            for (const auto& it : *_vo) {
                it.second->_parent = this;
            }
        }
        else if (tmp[0]->_type == JS::BOOL) {
            _type = JS::BOOL;
            _vb   = tmp[0]->_vb;
        }
        else if (tmp[0]->_type == JS::NUMBER) {
            _type = JS::NUMBER;
            _vn   = tmp[0]->_vn;
        }
        else if (tmp[0]->_type == JS::STRING) {
            _type = JS::STRING;
            _vs   = tmp[0]->_vs;
            const_cast<JS*>(tmp[0])->_type = JS::NIL;
        }
        else if (tmp[0]->_type == JS::NIL) {
            _type = JS::NIL;
        }
        else {
            throw _FLW_JSON_ERROR(0, 1);
        }
    }
    catch(const std::string& err) {
        _FLW_JSON_FREE_STRINGS(sVal1, sVal2)
        _clear(false);
        return err;
    }
    return "";
}
void JS::debug() const {
    std::string t;
    _json_debug(this, t);
}
std::string JS::encode(JS::ENCODE_OPTION option) const {
    std::string t;
    std::string j;
    try {
        if (is_array() == true || is_object() == true) {
            JS::_Encode(this, j, t, false, option);
        }
        else {
            return _encode(true, option);
        }
    }
    catch (const std::string& e) {
        j = e;
    }
    return j;
}
std::string JS::_encode(bool ignore_name, JS::ENCODE_OPTION option) const {
    static const std::string QUOTE = "\"";
    std::string res;
    std::string arr   = (option == ENCODE_OPTION::NORMAL) ? "\": [" : "\":[";
    std::string obj   = (option == ENCODE_OPTION::NORMAL) ? "\": {" : "\":{";
    std::string name1 = (option == ENCODE_OPTION::NORMAL) ? "\": " : "\":";
    bool object = (_parent != nullptr && _parent->is_object() == true);
    if (_type == JS::ARRAY) {
        res = (object == false || ignore_name == true) ? res = "[" : (QUOTE + _name + arr);
    }
    else if (_type == JS::OBJECT) {
        res = (object == false || ignore_name == true) ? "{" : (QUOTE + _name + obj);
    }
    else {
        res = (object == false || ignore_name == true) ? "" : (QUOTE + _name + name1);
        if (_type == JS::STRING) {
            res += QUOTE + _vs + QUOTE;
        }
        else if (_type == JS::NIL) {
            res += "null";
        }
        else if (_type == JS::BOOL && _vb == true) {
            res += "true";
        }
        else if (_type == JS::BOOL && _vb == false) {
            res += "false";
        }
        else if (_type == JS::NUMBER) {
            res += JS::FormatNumber(_vn);
        }
    }
    return res;
}
void JS::_Encode(const JS* js, std::string& j, std::string& t, bool comma, JS::ENCODE_OPTION option) {
    std::string c = (comma == true) ? "," : "";
    std::string n = (option == ENCODE_OPTION::REMOVE_LEADING_AND_NEWLINES) ? "" : "\n";
    size_t      f = 0;
    if (js->is_array() == true) {
        j += t + js->_encode(j == "", option) + ((js->_enc_flag == 1) ? "" : n);
        for (const auto n2 : *js->_va) {
            if (option == ENCODE_OPTION::NORMAL) t += "\t";
            if (js->_enc_flag == 1) JS::_EncodeInline(n2, j, f < (js->_va->size() - 1), option);
            else JS::_Encode(n2, j, t, f < (js->_va->size() - 1), option);
            if (option == ENCODE_OPTION::NORMAL) t.pop_back();
            f++;
        }
        j += ((js->_enc_flag == 1) ? ("]" + c + "\n") : (t + "]" + c + n));
    }
    else if (js->is_object() == true) {
        j += t + js->_encode(j == "", option) + ((js->_enc_flag == 1) ? "" : n);
        for (const auto& n2 : *js->_vo) {
            if (option == ENCODE_OPTION::NORMAL) t += "\t";
            if (js->_enc_flag == 1) JS::_EncodeInline(n2.second, j, f < (js->_vo->size() - 1), option);
            else JS::_Encode(n2.second, j, t, f < (js->_vo->size() - 1), option);
            if (option == ENCODE_OPTION::NORMAL) t.pop_back();
            f++;
        }
        j += ((js->_enc_flag == 1) ? ("}" + c + "\n") : (t + "}" + c + n));
    }
    else {
        j += t + js->_encode(false, option) + c + n;
    }
}
void JS::_EncodeInline(const JS* js, std::string& j, bool comma, JS::ENCODE_OPTION option) {
    std::string c = (comma == true) ? "," : "";
    size_t      f = 0;
    if (*js == JS::ARRAY) {
        j += js->_encode(false, option);
        for (const auto n : *js->_va) {
            JS::_EncodeInline(n, j, f < (js->_va->size() - 1), option);
            f++;
        }
        j += "]" + c;
    }
    else if (*js == JS::OBJECT) {
        j += js->_encode(false, option);
        for (const auto& n : *js->_vo) {
            JS::_EncodeInline(n.second, j, f < (js->_vo->size() - 1), option);
            f++;
        }
        j += "}" + c;
    }
    else {
        j += js->_encode(false, option) + c;
    }
}
std::string JS::Escape(const char* string) {
    std::string res;
    res.reserve(strlen(string) + 5);
    while (*string != 0) {
        auto c = *string;
        if (c == 9) res += "\\t";
        else if (c == 10) res += "\\n";
        else if (c == 13) res += "\\r";
        else if (c == 8) res += "\\b";
        else if (c == 14) res += "\\f";
        else if (c == 34) res += "\\\"";
        else if (c == 92) res += "\\\\";
        else res += c;
        string++;
    }
    return res;
}
const JS* JS::find(std::string name, bool rec) const {
    if (is_object() == true) {
        auto find1 = _vo->find(name);
        if (find1 != _vo->end()) {
            return find1->second;
        }
        else if (rec == true) {
            for (auto o : *_vo) {
                if (o.second->is_object() == true) {
                    return o.second->find(name, rec);
                }
            }
        }
    }
    return nullptr;
}
std::string JS::FormatNumber(double f, bool E) {
    double ABS = fabs(f);
    double MIN = ABS - static_cast<int64_t>(ABS);
    size_t n   = 0;
    char b[100];
    if (ABS > 999'000'000'000) {
        n = (E == true) ? snprintf(b, 100, "%e", f) : snprintf(b, 100, "%f", f);
    }
    else {
        if (MIN < 0.0000001) {
            n = (E == true) ? snprintf(b, 100, "%.0e", f) : snprintf(b, 100, "%.0f", f);
        }
        else if (MIN < 0.001) {
            n = (E == true) ? snprintf(b, 100, "%.7e", f) : snprintf(b, 100, "%.7f", f);
        }
        else {
            n = (E == true) ? snprintf(b, 100, "%e", f) : snprintf(b, 100, "%f", f);
        }
    }
    if (n < 1 || n >= 100) {
        return "0";
    }
    std::string s = b;
    if (s.find('.') == std::string::npos) {
        return s;
    }
    while (s.back() == '0') {
        s.pop_back();
    }
    if (s.back() == '.') {
        s.pop_back();
    }
    return s;
}
const JS* JS::_get_object(const char* name, bool escape) const {
    if (is_object() == true) {
        auto key   = (escape == true) ? JS::Escape(name) : name;
        auto find1 = _vo->find(key);
        return (find1 != _vo->end()) ? find1->second : nullptr;
    }
    return nullptr;
}
bool JS::_set_object(const char* name, JS* js, bool ignore_duplicates) {
    if (is_object() == true) {
        auto find1 = _vo->find(name);
        if (find1 != _vo->end()) {
            if (ignore_duplicates == false) {
                delete js;
                return false;
            }
            else {
                delete find1->second;
            }
        }
        (*_vo)[name] = js;
    }
    return true;
}
const JSArray JS::vo_to_va() const {
    JSArray res;
    if (_type == JS::OBJECT) {
        for (auto& m : *_vo) {
            res.push_back(m.second);
        }
    }
    return res;
}
std::string JS::Unescape(const char* string) {
    std::string res;
    res.reserve(strlen(string));
    while (*string != 0) {
        unsigned char c = *string;
        unsigned char n = *(string + 1);
        if (c == '\\') {
            if (n == 't') res += '\t';
            else if (n == 'n') res += '\n';
            else if (n == 'r') res += '\r';
            else if (n == 'b') res += '\b';
            else if (n == 'f') res += '\f';
            else if (n == '\"') res += '"';
            else if (n == '\\') res += '\\';
            else if (n == 0) break;
            string++;
        }
        else {
            res += c;
        }
        string++;
    }
    return res;
}
JSB& JSB::add(JS* js) {
    auto name = js->name();
    if (_current == nullptr) {
        if (name != "") {
            delete js;
            throw std::string("error: root object must be nameless <" + name + ">");
        }
        else {
            _root = _current = js;
        }
    }
    else {
        js->_parent = _current;
        if (_current->is_array() == true) {
            if (name != "") {
                delete js;
                throw std::string("error: values added to array are nameless <" + name + ">");
            }
            else if (*js == JS::ARRAY || *js == JS::OBJECT) {
                _current->_va->push_back(js);
                _current = js;
            }
            else {
                _current->_va->push_back(js);
            }
        }
        else if (_current->is_object() == true) {
            if (_current->find(name) != nullptr) {
                delete js;
                throw std::string("error: duplicate name <" + name + ">");
            }
            else if (js->is_array() == true || js->is_object() == true) {
                (*_current->_vo)[name] = js;
                _current = js;
            }
            else {
                (*_current->_vo)[name] = js;
            }
        }
        else {
            delete js;
            throw std::string("error: missing container");
        }
    }
    return *this;
}
std::string JSB::encode() const {
    if (_root == nullptr) {
        throw std::string("error: empty json");
    }
    else if (_name != "") {
        throw std::string("error: unused name value <" + _name + ">");
    }
    auto j = _root->encode();
    if (j.find("error") == 0) {
        throw j;
    }
    return j;
}
JSB& JSB::end() {
    if (_current == nullptr) {
        throw std::string("error: empty json");
    }
    else if (_current == _root) {
        throw std::string("error: already at the top level");
    }
    _current = _current->parent();
    return *this;
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
Fl_Text_Display::Style_Table_Entry _LOGDISPLAY_STYLE_TABLE[] = {
    { FL_FOREGROUND_COLOR,  FL_COURIER,         14, 0, 0 },
    { FL_GRAY,              FL_COURIER,         14, 0, 0 },
    { FL_RED,               FL_COURIER,         14, 0, 0 },
    { FL_DARK_GREEN,        FL_COURIER,         14, 0, 0 },
    { FL_BLUE,              FL_COURIER,         14, 0, 0 },
    { FL_MAGENTA,           FL_COURIER,         14, 0, 0 },
    { FL_DARK_YELLOW,       FL_COURIER,         14, 0, 0 },
    { FL_CYAN,              FL_COURIER,         14, 0, 0 },
    { FL_FOREGROUND_COLOR,  FL_COURIER_BOLD,    14, 0, 0 },
    { FL_GRAY,              FL_COURIER_BOLD,    14, 0, 0 },
    { FL_RED,               FL_COURIER_BOLD,    14, 0, 0 },
    { FL_DARK_GREEN,        FL_COURIER_BOLD,    14, 0, 0 },
    { FL_BLUE,              FL_COURIER_BOLD,    14, 0, 0 },
    { FL_MAGENTA,           FL_COURIER_BOLD,    14, 0, 0 },
    { FL_DARK_YELLOW,       FL_COURIER_BOLD,    14, 0, 0 },
    { FL_CYAN,              FL_COURIER_BOLD,    14, 0, 0 },
    { FL_FOREGROUND_COLOR,  FL_COURIER,         14, Fl_Text_Display::ATTR_BGCOLOR, 0 },
    { FL_GRAY,              FL_COURIER,         14, Fl_Text_Display::ATTR_BGCOLOR, 0 },
    { FL_RED,               FL_COURIER,         14, Fl_Text_Display::ATTR_BGCOLOR, 0 },
    { FL_DARK_GREEN,        FL_COURIER,         14, Fl_Text_Display::ATTR_BGCOLOR, 0 },
    { FL_BLUE,              FL_COURIER,         14, Fl_Text_Display::ATTR_BGCOLOR, 0 },
    { FL_MAGENTA,           FL_COURIER,         14, Fl_Text_Display::ATTR_BGCOLOR, 0 },
    { FL_DARK_YELLOW,       FL_COURIER,         14, Fl_Text_Display::ATTR_BGCOLOR, 0 },
    { FL_CYAN,              FL_COURIER,         14, Fl_Text_Display::ATTR_BGCOLOR, 0 },
    { FL_FOREGROUND_COLOR,  FL_COURIER_BOLD,    14, Fl_Text_Display::ATTR_BGCOLOR, 0 },
    { FL_GRAY,              FL_COURIER_BOLD,    14, Fl_Text_Display::ATTR_BGCOLOR, 0 },
    { FL_RED,               FL_COURIER_BOLD,    14, Fl_Text_Display::ATTR_BGCOLOR, 0 },
    { FL_DARK_GREEN,        FL_COURIER_BOLD,    14, Fl_Text_Display::ATTR_BGCOLOR, 0 },
    { FL_BLUE,              FL_COURIER_BOLD,    14, Fl_Text_Display::ATTR_BGCOLOR, 0 },
    { FL_MAGENTA,           FL_COURIER_BOLD,    14, Fl_Text_Display::ATTR_BGCOLOR, 0 },
    { FL_DARK_YELLOW,       FL_COURIER_BOLD,    14, Fl_Text_Display::ATTR_BGCOLOR, 0 },
    { FL_CYAN,              FL_COURIER_BOLD,    14, Fl_Text_Display::ATTR_BGCOLOR, 0 },
};
static const std::string _LOGDISPLAY_JSON_EXAMPLE = R"(Example json string
All available options are below.
Use one or all.
[
    {
        "style": "lock",
        "on": true
    },
    {
        "style": "line",
        "start": 0,
        "stop": 10,
        "color": "RED"
    },
    {
        "style": "num",
        "color": "MAGENTA",
        "count": 0
    },
    {
        "style": "string",
        "word1": "find_text_from_left",
        "color": "BLUE",
        "count": 0
    },
    {
        "style": "rstring",
        "word1": "find_text_from_right",
        "color": "BLUE",
        "count": 1
    },
    {
        "style": "range",
        "word1": "from_string",
        "word2": "to_string",
        "inclusive": true,
        "color": "BLUE",
        "count": 0
    },
    {
        "style": "between",
        "word1": "from_first_string",
        "word2": "to_last_string",
        "inclusive": true,
        "color": "BLUE",
        "count": 0
    },
    {
        "style": "custom",
        "word1": "string_1",
        "word2": "string_2",
        "start": 0,
        "stop": 10,
        "inclusive": true,
        "color": "BLUE",
        "count": 0
    }
]
)";
static const std::string _LOGDISPLAY_HELP = R"(@bSet style colors
All options will be called for every line.
Text must be valid JSON wrapped within [].
Count property is how many strings to color, 0 means all.
If inclusive is set to false only characters between found strings will be colored.

If lock is true then never change colors that have been set.
@f{
@f    "style": "lock",
@f    "on": true
@f}

Color characters by using index in line.
@f{
@f    "style": "line",
@f    "start": 0,
@f    "stop": 10,
@f    "color": "RED"
@f}

Color all numbers.
@f{
@f    "style": "num",
@f    "color": "MAGENTA",
@f    "count": 0
@f}

Color strings.
@f{
@f    "style": "string",
@f    "word1": "find_text_from_left",
@f    "color": "BLUE",
@f    "count": 0
@f}

Color string but start from the right.
@f{
@f    "style": "rstring",
@f    "word1": "find_text_from_right",
@f    "color": "BLUE",
@f    "count": 0
@f}

Color text between two strings.
@f{
@f    "style": "range",
@f    "word1": "from_string",
@f    "word2": "to_string",
@f    "inclusive": true,
@f    "color": "BLUE",
@f    "count": 0
@f}

Color text from first found string to the last found string.
@f{
@f    "style": "between",
@f    "word1": "from_first_string",
@f    "word2": "to_last_string",
@f    "inclusive": true,
@f    "color": "BLUE"
@f}

This property will call LogDisplay::line_custom_cb() which does nothing so override it.
@f{
@f    "style": "custom",
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
static const std::string _LOGDISPLAY_TOOLTIP = R"(Ctrl + 'f' for enter search text.
F3 to search for next word.
Shift + F3 to search for previous word.
Ctrl + 'h' to display style string help.
Ctrl + 'e' to edit style string.
)";
struct _LogDisplayStyle {
    enum STYLE {
                                EMPTY,
                                BETWEEN,
                                CUSTOM,
                                LINE,
                                LOCK,
                                NUM,
                                RANGE,
                                RSTRING,
                                STRING,
    };
    LogDisplay::COLOR           color;
    STYLE                       style;
    bool                        inclusive;
    bool                        on;
    size_t                      count;
    size_t                      start;
    size_t                      stop;
    std::string                 word1;
    std::string                 word2;
    _LogDisplayStyle() {
        color     = LogDisplay::FOREGROUND;
        count     = 0;
        inclusive = false;
        on        = false;
        start     = 0;
        stop      = 0;
        style     = _LogDisplayStyle::EMPTY;
    }
};
static LogDisplay::COLOR _logdisplay_convert_color(std::string name) {
    if (name == "GRAY") return LogDisplay::GRAY;
    else if (name == "RED") return LogDisplay::RED;
    else if (name == "GREEN") return LogDisplay::GREEN;
    else if (name == "BLUE") return LogDisplay::BLUE;
    else if (name == "MAGENTA") return LogDisplay::MAGENTA;
    else if (name == "YELLOW") return LogDisplay::YELLOW;
    else if (name == "CYAN") return LogDisplay::CYAN;
    else if (name == "BOLD_FOREGROUND") return LogDisplay::BOLD_FOREGROUND;
    else if (name == "BOLD_GRAY") return LogDisplay::BOLD_GRAY;
    else if (name == "BOLD_RED") return LogDisplay::BOLD_RED;
    else if (name == "BOLD_GREEN") return LogDisplay::BOLD_GREEN;
    else if (name == "BOLD_BLUE") return LogDisplay::BOLD_BLUE;
    else if (name == "BOLD_MAGENTA") return LogDisplay::BOLD_MAGENTA;
    else if (name == "BOLD_YELLOW") return LogDisplay::BOLD_YELLOW;
    else if (name == "BOLD_CYAN") return LogDisplay::BOLD_CYAN;
    else if (name == "BG_FOREGROUND") return LogDisplay::BG_FOREGROUND;
    else if (name == "BG_GRAY") return LogDisplay::BG_GRAY;
    else if (name == "BG_RED") return LogDisplay::BG_RED;
    else if (name == "BG_GREEN") return LogDisplay::BG_GREEN;
    else if (name == "BG_BLUE") return LogDisplay::BG_BLUE;
    else if (name == "BG_MAGENTA") return LogDisplay::BG_MAGENTA;
    else if (name == "BG_YELLOW") return LogDisplay::BG_YELLOW;
    else if (name == "BG_CYAN") return LogDisplay::BG_CYAN;
    else if (name == "BG_BOLD_FOREGROUND") return LogDisplay::BG_BOLD_FOREGROUND;
    else if (name == "BG_BOLD_GRAY") return LogDisplay::BG_BOLD_GRAY;
    else if (name == "BG_BOLD_RED") return LogDisplay::BG_BOLD_RED;
    else if (name == "BG_BOLD_GREEN") return LogDisplay::BG_BOLD_GREEN;
    else if (name == "BG_BOLD_BLUE") return LogDisplay::BG_BOLD_BLUE;
    else if (name == "BG_BOLD_MAGENTA") return LogDisplay::BG_BOLD_MAGENTA;
    else if (name == "BG_BOLD_YELLOW") return LogDisplay::BG_BOLD_YELLOW;
    else if (name == "BG_BOLD_CYAN") return LogDisplay::BG_BOLD_CYAN;
    else return LogDisplay::GRAY;
}
static std::vector<_LogDisplayStyle> _logdisplay_parse_json(std::string json) {
    #define FLW_LOGDISPLAY_ERROR(X) { fl_alert("error: illegal value at pos %u", (X)->pos()); res.clear(); return res; }
    auto res = std::vector<_LogDisplayStyle>();
    auto js  = JS();
    auto err = js.decode(json.c_str(), json.length());
    if (err != "") {
        fl_alert("error: failed to parse json\n%s", err.c_str());
        return res;
    }
    if (js.is_array() == false) FLW_LOGDISPLAY_ERROR(&js)
    for (const auto j : *js.va()) {
        if (j->is_object() == false) FLW_LOGDISPLAY_ERROR(j);
        auto style = _LogDisplayStyle();
        for (const auto j2 : j->vo_to_va()) {
            if (j2->name() == "color" && j2->is_string() == true)           style.color     = _logdisplay_convert_color(j2->vs());
            else if (j2->name() == "count" && j2->is_number() == true)      style.count     = (size_t) j2->vn_i();
            else if (j2->name() == "inclusive" && j2->is_bool() == true)    style.inclusive = j2->vb();
            else if (j2->name() == "on" && j2->is_bool() == true)           style.on        = j2->vb();
            else if (j2->name() == "start" && j2->is_number() == true)      style.start     = (size_t) j2->vn_i();
            else if (j2->name() == "stop" && j2->is_number() == true)       style.stop      = (size_t) j2->vn_i();
            else if (j2->name() == "word1" && j2->is_string() == true)      style.word1     = j2->vs_u();
            else if (j2->name() == "word2" && j2->is_string() == true)      style.word2     = j2->vs_u();
            else if (j2->name() == "style" && j2->is_string() == true) {
                if (j2->vs() == "between")      style.style = _LogDisplayStyle::BETWEEN;
                else if (j2->vs() == "custom")  style.style = _LogDisplayStyle::CUSTOM;
                else if (j2->vs() == "line")    style.style = _LogDisplayStyle::LINE;
                else if (j2->vs() == "lock")    style.style = _LogDisplayStyle::LOCK;
                else if (j2->vs() == "num")     style.style = _LogDisplayStyle::NUM;
                else if (j2->vs() == "range")   style.style = _LogDisplayStyle::RANGE;
                else if (j2->vs() == "rstring") style.style = _LogDisplayStyle::RSTRING;
                else if (j2->vs() == "string")  style.style = _LogDisplayStyle::STRING;
                else FLW_LOGDISPLAY_ERROR(j2)
            }
            else FLW_LOGDISPLAY_ERROR(j2)
        }
        if (style.style != _LogDisplayStyle::EMPTY) {
            res.push_back(style);
        }
    }
    return res;
}
static char* _logdisplay_win_to_unix(const char* string) {
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
    auto res = static_cast<char*>(calloc(len + 1, 1));
    if (res != nullptr) {
        auto pos = 0;
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
LogDisplay::LogDisplay::Tmp::Tmp() {
    buf  = nullptr;
    len  = 0;
    pos  = 0;
    size = 0;
}
LogDisplay::LogDisplay::Tmp::~Tmp() {
    free(buf);
}
LogDisplay::LogDisplay(int x, int y, int w, int h, const char *l) : Fl_Text_Display(x, y, w, h, l) {
    _buffer      = new Fl_Text_Buffer();
    _style       = new Fl_Text_Buffer();
    _lock_colors = false;
    _tmp         = nullptr;
    buffer(_buffer);
    linenumber_align(FL_ALIGN_RIGHT);
    linenumber_format("%5d");
    update_pref();
    tooltip(_LOGDISPLAY_TOOLTIP.c_str());
}
LogDisplay::~LogDisplay() {
    buffer(nullptr);
    delete _buffer;
    delete _style;
    delete _tmp;
}
void LogDisplay::edit_styles() {
    auto json    = (_json == "") ? _LOGDISPLAY_JSON_EXAMPLE : _json;
    auto changed = dlg::text_edit("Edit JSON Style String", json, top_window(), 40, 50);
    if (changed == false) {
        return;
    }
    style(json);
}
void LogDisplay::find(bool next, bool force_ask) {
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
int LogDisplay::handle(int event) {
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
                dlg::list("Style Help", _LOGDISPLAY_HELP, top_window(), false, 40, 30);
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
void LogDisplay::save_file() {
    auto filename = fl_file_chooser("Select Destination File", nullptr, nullptr, 0);
    if (filename != nullptr && _buffer->savefile(filename) != 0) {
        fl_alert("error: failed to save text to %s", filename);
    }
}
void LogDisplay::style(std::string json) {
    auto ds  = (json != "") ? _logdisplay_parse_json(json) : std::vector<_LogDisplayStyle>();
    _json      = json;
    _tmp       = new Tmp();
    _tmp->size = _buffer->length();
    _tmp->buf  = static_cast<char*>(calloc(_tmp->size + 1, 1));
    if (_tmp->buf != nullptr) {
        auto row = 1;
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
                    if (d.style == _LogDisplayStyle::BETWEEN) {
                        style_range(line, d.word1, d.word2, d.inclusive, d.color);
                    }
                    else if (d.style == _LogDisplayStyle::CUSTOM) {
                        line_custom_cb(row, line, d.word1, d.word2, d.color, d.inclusive, d.start, d.stop, d.count);
                    }
                    else if (d.style == _LogDisplayStyle::LINE) {
                        style_line(d.start, d.stop, d.color);
                    }
                    else if (d.style == _LogDisplayStyle::LOCK) {
                        lock_colors(d.on);
                    }
                    else if (d.style == _LogDisplayStyle::NUM) {
                        style_num(line, d.color, d.count);
                    }
                    else if (d.style == _LogDisplayStyle::RANGE) {
                        style_range(line, d.word1, d.word2, d.inclusive, d.color, d.count);
                    }
                    else if (d.style == _LogDisplayStyle::RSTRING) {
                        style_rstring(line, d.word1, d.color, d.count);
                    }
                    else if (d.style == _LogDisplayStyle::STRING) {
                        style_string(line, d.word1, d.color, d.count);
                    }
                }
            }
            _tmp->pos += _tmp->len + 1;
            row += 1;
            free(line);
        }
        _style->text(_tmp->buf);
        highlight_data(_style, _LOGDISPLAY_STYLE_TABLE, sizeof(_LOGDISPLAY_STYLE_TABLE) / sizeof(_LOGDISPLAY_STYLE_TABLE[0]), (char) LogDisplay::FOREGROUND, nullptr, 0);
    }
    delete _tmp;
    _tmp = nullptr;
}
void LogDisplay::style_between(const std::string& line, const std::string& word1, const std::string& word2, bool inclusive, LogDisplay::COLOR color) {
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
void LogDisplay::style_line(size_t start, size_t stop, LogDisplay::COLOR c) {
    assert(_tmp);
    start += _tmp->pos;
    stop  += _tmp->pos;
    while (start <= stop && start < _tmp->size && start < _tmp->pos + _tmp->len) {
        if (_lock_colors == false || _tmp->buf[start] == (char) LogDisplay::LogDisplay::FOREGROUND) {
            _tmp->buf[start] = (char) c;
        }
        start++;
    }
}
void LogDisplay::style_num(const std::string& line, LogDisplay::COLOR color, size_t count) {
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
void LogDisplay::style_range(const std::string& line, const std::string& word1, const std::string& word2, bool inclusive, LogDisplay::COLOR color, size_t count) {
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
            else {
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
void LogDisplay::style_rstring(const std::string& line, const std::string& word1, LogDisplay::COLOR color, size_t count) {
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
void LogDisplay::style_string(const std::string& line, const std::string& word1, LogDisplay::COLOR color, size_t count) {
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
void LogDisplay::update_pref() {
    labelsize(flw::PREF_FIXED_FONTSIZE);
    linenumber_bgcolor(FL_BACKGROUND_COLOR);
    linenumber_fgcolor(FL_FOREGROUND_COLOR);
    linenumber_font(flw::PREF_FIXED_FONT);
    linenumber_size(flw::PREF_FIXED_FONTSIZE);
    linenumber_width(flw::PREF_FIXED_FONTSIZE * 3);
    textfont(flw::PREF_FIXED_FONT);
    textsize(flw::PREF_FIXED_FONTSIZE);
    for (size_t f = 0; f < sizeof(_LOGDISPLAY_STYLE_TABLE) / sizeof(_LOGDISPLAY_STYLE_TABLE[0]); f++) {
        _LOGDISPLAY_STYLE_TABLE[f].size = flw::PREF_FIXED_FONTSIZE;
        if (theme::is_dark() == true) {
            _LOGDISPLAY_STYLE_TABLE[f].bgcolor = FL_WHITE;
        }
        else {
            _LOGDISPLAY_STYLE_TABLE[f].bgcolor = fl_lighter(FL_GRAY);
        }
    }
}
void LogDisplay::value(const char* text) {
    assert(text);
    _buffer->text("");
    _style->text("");
    auto win = _logdisplay_win_to_unix(text);
    if (win != nullptr) {
        _buffer->text(win);
        free(win);
    }
    else {
        _buffer->text(text);
    }
}
}
#include <assert.h>
#include <FL/fl_ask.H>
#include <FL/fl_draw.H>
#include <FL/fl_show_colormap.H>
#include <FL/Fl_File_Chooser.H>
#include <FL/Fl_Hor_Slider.H>
namespace flw {
#define _FLW_PLOT_ERROR(X) { fl_alert("error: illegal plot value at pos %u", (X)->pos()); clear(); return false; }
#define _FLW_PLOT_CB(X)    [](Fl_Widget*, void* o) { static_cast<Plot*>(o)->X; }, this
#define _FLW_PLOT_DEBUG(X)
#define _FLW_PLOT_CLIP(X) { X; }
static const char* const _PLOT_ADD_LINE     = "Create line...";
static const char* const _PLOT_CLEAR        = "Clear plot";
static const char* const _PLOT_DEBUG        = "Debug";
static const char* const _PLOT_DEBUG_LINE   = "Debug line";
static const char* const _PLOT_LOAD_CVS     = "Add line from cvs...";
static const char* const _PLOT_LOAD_JSON    = "Load plot from JSON...";
static const char* const _PLOT_PRINT        = "Print to PostScript file...";
static const char* const _PLOT_SAVE_CVS     = "Save line to cvs...";
static const char* const _PLOT_SAVE_JSON    = "Save plot to JSON...";
static const char* const _PLOT_SAVE_PNG     = "Save to png file...";
static const char* const _PLOT_SETUP_DELETE = "Delete lines...";
static const char* const _PLOT_SETUP_HLINES = "Show horizontal lines";
static const char* const _PLOT_SETUP_LABEL  = "Label...";
static const char* const _PLOT_SETUP_LABELS = "Show line labels";
static const char* const _PLOT_SETUP_LINE   = "Line properties...";
static const char* const _PLOT_SETUP_MAXX   = "Set max X...";
static const char* const _PLOT_SETUP_MAXY   = "Set max Y...";
static const char* const _PLOT_SETUP_MINX   = "Set min X...";
static const char* const _PLOT_SETUP_MINY   = "Set min Y...";
static const char* const _PLOT_SETUP_SHOW   = "Show or hide lines...";
static const char* const _PLOT_SETUP_VLINES = "Show vertical lines";
static const char* const _PLOT_SETUP_XLABEL = "X Label...";
static const char* const _PLOT_SETUP_YLABEL = "Y Label...";
static const int         _PLOT_TICK_SIZE    = 4;
PlotData::PlotData(double X, double Y) {
    x = y = INFINITY;
    if (std::isfinite(X) == true &&
        std::isfinite(Y) == true &&
        fabs(X) < PlotData::MAX_VALUE &&
        fabs(Y) < PlotData::MAX_VALUE) {
        x = X;
        y = Y;
    }
}
void PlotData::debug(int i) const {
#ifdef DEBUG
    auto X = JS::FormatNumber(x);
    auto Y = JS::FormatNumber(y);
    if (i >= 0) {
        printf("[%04d] = %24.8f, %24.8f\n", i, x, y);
    }
    else {
        printf("x, y = %24.8f, %24.8f\n", x, y);
    }
#else
    (void) i;
#endif
}
void PlotData::Debug(const PlotDataVector& in) {
#ifdef DEBUG
    printf("\nPlotDataVector(%u)\n", static_cast<unsigned>(in.size()));
    int c = 0;
    for (const auto& data : in) {
        data.debug(c++);
    }
    fflush(stdout);
#else
    (void) in;
#endif
}
PlotDataVector PlotData::LoadCSV(std::string filename, std::string sep) {
    Buf buf = File::Load(filename);
    if (buf.s < 3) {
        return PlotDataVector();
    }
    std::string    str   = buf.p;
    StringVector   lines = util::split_string(str, "\n");
    PlotDataVector res;
    for (const auto& l : lines) {
        StringVector line = util::split_string(l, sep);
        if (line.size() == 2) {
            auto data = PlotData(util::to_double(line[0]), util::to_double(line[1]));
            if (data.is_valid() == true) {
                res.push_back(data);
            }
        }
    }
    return res;
}
bool PlotData::MinMax(const PlotDataVector& in, double& min_x, double& max_x, double& min_y, double& max_y) {
    bool init = false;
    min_x = max_x = min_y = max_y = INFINITY;
    for (const auto& data : in) {
        if (std::isinf(data.x) == true || std::isinf(data.y) == true) {
        }
        else if (init == false) {
            min_x = max_x = data.x;
            min_y = max_y = data.y;
            init  = true;
        }
        else {
            if (data.x < min_x) {
                min_x = data.x;
            }
            if (data.x > max_x) {
                max_x = data.x;
            }
            if (data.y < min_y) {
                min_y = data.y;
            }
            if (data.y > max_y) {
                max_y = data.y;
            }
        }
    }
    return std::isfinite(min_x) && std::isfinite(max_x) && std::isfinite(min_y) && std::isfinite(max_y);
}
PlotDataVector PlotData::Modify(const PlotDataVector& in, PlotData::MODIFY modify, PlotData::TARGET target, double value) {
    PlotDataVector res;
    if (fabs(value) < PlotData::MIN_VALUE || fabs(value) > PlotData::MAX_VALUE) {
        return res;
    }
    for (const auto& data : in) {
        switch (modify) {
        case MODIFY::SUBTRACTION:
            if (target == PlotData::TARGET::X) {
                res.push_back(PlotData(data.x - value, data.y));
            }
            else if (target == PlotData::TARGET::Y) {
                res.push_back(PlotData(data.x, data.y - value));
            }
            else {
                res.push_back(PlotData(data.x - value, data.y - value));
            }
            break;
        case MODIFY::MULTIPLICATION:
            if (target == PlotData::TARGET::X) {
                res.push_back(PlotData(data.x * value, data.y));
            }
            else if (target == PlotData::TARGET::Y) {
                res.push_back(PlotData(data.x, data.y * value));
            }
            else {
                res.push_back(PlotData(data.x * value, data.y * value));
            }
            break;
        case MODIFY::DIVISION:
            if (target == PlotData::TARGET::X) {
                res.push_back(PlotData(data.x / value, data.y));
            }
            else if (target == PlotData::TARGET::Y) {
                res.push_back(PlotData(data.x, data.y / value));
            }
            else {
                res.push_back(PlotData(data.x / value, data.y / value));
            }
            break;
        default:
            if (target == PlotData::TARGET::X) {
                res.push_back(PlotData(data.x + value, data.y));
            }
            else if (target == PlotData::TARGET::Y) {
                res.push_back(PlotData(data.x, data.y + value));
            }
            else {
                res.push_back(PlotData(data.x + value, data.y + value));
            }
            break;
        }
    }
    return res;
}
bool PlotData::SaveCSV(const PlotDataVector& in, std::string filename, std::string sep) {
    std::string csv;
    csv.reserve(in.size() * 20 + 256);
    for (const auto& data : in) {
        char buffer[256];
        snprintf(buffer, 256, "%s%s%s\n", JS::FormatNumber(data.x).c_str(), sep.c_str(), JS::FormatNumber(data.y).c_str());
        csv += buffer;
    }
    return File::Save(filename, csv.c_str(), csv.size());
}
PlotDataVector PlotData::Swap(const PlotDataVector& in) {
    PlotDataVector res;
    for (const auto& data : in) {
        res.push_back(PlotData(data.y, data.x));
    }
    return res;
}
PlotLine::PlotLine() {
    clear();
}
PlotLine::PlotLine(const PlotDataVector& data, std::string label, PlotLine::TYPE type, Fl_Color color, unsigned width) {
    _color   = color;
    _data    = data;
    _label   = label;
    _type    = type;
    _visible = true;
    _width   = (width <= PlotLine::MAX_WIDTH) ? width : 1;
}
void PlotLine::clear() {
    _color   = FL_BLUE;
    _label   = "";
    _rect    = Fl_Rect();
    _type    = TYPE::LINE;
    _visible = true;
    _width   = 1;
    _data.clear();
}
void PlotLine::debug() const {
#ifdef DEBUG
    double minx, maxx, miny, maxy;
    PlotData::MinMax(_data, minx, maxx, miny, maxy);
    printf("\t----------------------------------------\n");
    printf("\tPlotLine:\n");
    printf("\t\tcolor   = %u\n", _color);
    printf("\t\tlabel   = '%s'\n", _label.c_str());
    printf("\t\tpoints  = %d\n", static_cast<int>(_data.size()));
    printf("\t\ttype    = %d\n", static_cast<int>(_type));
    printf("\t\tvisible = %s\n", _visible ? "YES" : "NO");
    printf("\t\twidth   = %d px\n", static_cast<int>(_width));
    printf("\t\trect    = %04d, %04d, %04d, %04d\n", _rect.x(), _rect.y(), _rect.w(), _rect.h());
    printf("\t\tminX    = %22.8f\n", minx);
    printf("\t\tmaxX    = %22.8f\n", maxx);
    printf("\t\tminY    = %22.8f\n", miny);
    printf("\t\tmaxY    = %22.8f\n", maxy);
    fflush(stdout);
#endif
}
PlotLine& PlotLine::set_type_from_string(std::string val) {
    if (val == "LINE_DASH")             _type = TYPE::LINE_DASH;
    else if (val == "LINE_DOT")         _type = TYPE::LINE_DOT;
    else if (val == "LINE_WITH_SQUARE") _type = TYPE::LINE_WITH_SQUARE;
    else if (val == "VECTOR")           _type = TYPE::VECTOR;
    else if (val == "CIRCLE")           _type = TYPE::CIRCLE;
    else if (val == "FILLED_CIRCLE")    _type = TYPE::FILLED_CIRCLE;
    else if (val == "SQUARE")           _type = TYPE::SQUARE;
    else                                _type = TYPE::LINE;
    return *this;
}
std::string PlotLine::type_to_string() const {
    static const char* NAMES[] = { "LINE", "LINE_DASH", "LINE_DOT", "LINE_WITH_SQUARE", "VECTOR", "CIRCLE", "FILLED_CIRCLE", "SQUARE", "", "", };
    return NAMES[static_cast<size_t>(_type)];
}
class PlotLineSetup : public Fl_Double_Window {
public:
    PlotLine&                   _line;
    Fl_Button*                  _cancel;
    Fl_Button*                  _close;
    Fl_Button*                  _color;
    Fl_Choice*                  _type;
    Fl_Hor_Slider*              _width;
    Fl_Input*                   _label;
    GridGroup*                  _grid;
    bool                        _ret;
public:
    PlotLineSetup(Fl_Window* parent, PlotLine& line) :
    Fl_Double_Window(0, 0, 10, 10, "Line Properties"),
    _line(line) {
        end();
        _line   = line;
        _ret    = false;
        _cancel = new Fl_Button(0, 0, 0, 0, "&Cancel");
        _close  = new Fl_Return_Button(0, 0, 0, 0, "&Ok");
        _color  = new Fl_Button(0, 0, 0, 0, "Color");
        _grid   = new GridGroup(0, 0, w(), h());
        _label  = new Fl_Input(0, 0, 0, 0, "Label");
        _type   = new Fl_Choice(0, 0, 0, 0, "Type");
        _width  = new Fl_Hor_Slider(0, 0, 0, 0);
        _grid->add(_label,     12,   1,  -1,  4);
        _grid->add(_type,      12,   6,  -1,  4);
        _grid->add(_color,     12,  11,  -1,  4);
        _grid->add(_width,     12,  16,  -1,  4);
        _grid->add(_cancel,   -34,  -5,  16,  4);
        _grid->add(_close,    -17,  -5,  16,  4);
        add(_grid);
        _cancel->callback(PlotLineSetup::Callback, this);
        _close->callback(PlotLineSetup::Callback, this);
        _color->align(FL_ALIGN_LEFT);
        _color->callback(PlotLineSetup::Callback, this);
        _label->textfont(flw::PREF_FONT);
        _label->textsize(flw::PREF_FONTSIZE);
        _type->add("Line");
        _type->add("Dashed line");
        _type->add("Dotted line");
        _type->add("Line with square");
        _type->add("Vector");
        _type->add("Circle");
        _type->add("Filled circle");
        _type->add("Square");
        _type->textfont(flw::PREF_FONT);
        _type->textsize(flw::PREF_FONTSIZE);
        _width->align(FL_ALIGN_LEFT);
        _width->callback(PlotLineSetup::Callback, this);
        _width->precision(0);
        _width->range(0, PlotLine::MAX_WIDTH);
        _width->value(_line.width());
        _width->tooltip("Set line width or circle/square size.");
        resizable(_grid);
        util::labelfont(this);
        callback(PlotLineSetup::Callback, this);
        size(30 * flw::PREF_FONTSIZE, 14 * flw::PREF_FONTSIZE);
        size_range(30 * flw::PREF_FONTSIZE, 14 * flw::PREF_FONTSIZE);
        set_modal();
        util::center_window(this, parent);
        _grid->do_layout();
        PlotLineSetup::Callback(_width, this);
        update_widgets();
    }
    static void Callback(Fl_Widget* w, void* o) {
        auto self = static_cast<PlotLineSetup*>(o);
        if (w == self || w == self->_cancel) {
            self->hide();
        }
        else if (w == self->_color) {
            self->_color->color(fl_show_colormap(self->_line.color()));
        }
        else if (w == self->_width) {
            auto l = util::format("Width %u", static_cast<unsigned>(static_cast<Fl_Hor_Slider*>(w)->value()));
            w->copy_label(l.c_str());
            self->redraw();
        }
        else if (w == self->_close) {
            self->_ret = true;
            self->update_line();
            self->hide();
        }
    }
    bool run() {
        show();
        while (visible() != 0) {
            Fl::wait();
            Fl::flush();
        }
        return _ret;
    }
    void update_line() {
        _line.set_label(_label->value());
        _line.set_width(_width->value());
        _line.set_color(_color->color());
        if (_type->value() == 0)      _line.set_type(PlotLine::TYPE::LINE);
        else if (_type->value() == 1) _line.set_type(PlotLine::TYPE::LINE_DASH);
        else if (_type->value() == 2) _line.set_type(PlotLine::TYPE::LINE_DOT);
        else if (_type->value() == 3) _line.set_type(PlotLine::TYPE::LINE_WITH_SQUARE);
        else if (_type->value() == 4) _line.set_type(PlotLine::TYPE::VECTOR);
        else if (_type->value() == 5) _line.set_type(PlotLine::TYPE::CIRCLE);
        else if (_type->value() == 6) _line.set_type(PlotLine::TYPE::FILLED_CIRCLE);
        else if (_type->value() == 7) _line.set_type(PlotLine::TYPE::SQUARE);
    }
    void update_widgets() {
        _label->value(_line.label().c_str());
        _color->color(_line.color());
        PlotLineSetup::Callback(_width, this);
        if (_line.type() == PlotLine::TYPE::LINE)                  _type->value(0);
        else if (_line.type() == PlotLine::TYPE::LINE_DASH)        _type->value(1);
        else if (_line.type() == PlotLine::TYPE::LINE_DOT)         _type->value(2);
        else if (_line.type() == PlotLine::TYPE::LINE_WITH_SQUARE) _type->value(3);
        else if (_line.type() == PlotLine::TYPE::VECTOR)           _type->value(4);
        else if (_line.type() == PlotLine::TYPE::CIRCLE)           _type->value(5);
        else if (_line.type() == PlotLine::TYPE::FILLED_CIRCLE)    _type->value(6);
        else if (_line.type() == PlotLine::TYPE::SQUARE)           _type->value(7);
    }
};
void PlotScale::calc_tick(double height) {
    _tick  = 0.0;
    _pixel = 0.0;
    _fr    = 0;
    if (std::isfinite(_min) == true && std::isfinite(_max) == true && _min < _max) {
        const double RANGE    = _max - _min;
        int          kludge   = 0;
        double       test_inc = 0.0;
        double       test_min = 0.0;
        double       test_max = 0.0;
        int          ticker   = 0;
        if (RANGE < PlotData::MIN_VALUE || RANGE > PlotData::MAX_VALUE) {
            return;
        }
        test_inc = pow(10.0, ceil(log10(RANGE / 10.0)));
        test_max = static_cast<int64_t>(_max / test_inc) * test_inc;
        if (test_max < _max) {
            test_max += test_inc;
        }
        test_min = test_max;
        do {
            ++ticker;
            test_min -= test_inc;
        } while (test_min > _min && kludge++ < 100);
        if (ticker < 10) {
            test_inc = (ticker < 5) ? test_inc / 10.0 : test_inc / 2.0;
            while ((test_min + test_inc) <= _min && kludge++ < 100) {
                test_min += test_inc;
            }
            while ((test_max - test_inc) >= _max && kludge < 100) {
                test_max -= test_inc;
            }
        }
        _min  = test_min;
        _max  = test_max;
        _tick = test_inc;
        if (_tick > PlotData::MIN_VALUE / 10.0 && kludge < 100) {
            _pixel = height / (_max - _min);
            _fr    = util::count_decimals(_tick);
        }
        else {
            _tick = 0;
        }
    }
}
void PlotScale::clear() {
    _color  = FL_FOREGROUND_COLOR;
    _fr     = 0;
    _label  = "";
    _labels = StringVector();
    _pixel  = 0.0;
    _text   = 0;
    _tick   = 0.0;
    clear_min_max();
}
void PlotScale::debug(const char* s) const {
#ifdef DEBUG
    printf("\t-----------------------------------------\n");
    printf("\tPlotScale: %s\n", s);
    printf("\t\tcolor  = %u\n", _color);
    printf("\t\tfr     = %d\n", _fr);
    printf("\t\tlabel  = %s\n", _label.c_str());
    printf("\t\tlabels = %d strings\n", static_cast<int>(_labels.size()));
    printf("\t\ttext   = %d px\n", text());
    printf("\t\tmin    = %24.8f\n", _min);
    printf("\t\tmax    = %24.8f\n", _max);
    printf("\t\tdiff   = %24.8f\n", _max - _min);
    printf("\t\ttick   = %24.8f\n", _tick);
    printf("\t\tpixel  = %24.8f\n", _pixel);
    fflush(stdout);
#else
    (void) s;
#endif
}
void PlotScale::measure_labels(int cw, bool custom) {
    if (_labels.size() > 0 && custom == true) {
        _text = 0;
        for (const auto& l : _labels) {
            if (l.length() > _text) {
                _text = l.length();
            }
        }
        _text *= cw;
    }
    else {
        int         dec = (_fr > 0) ? _fr : -1;
        std::string l1  = util::format_double(_min, dec, ' ');
        std::string l2  = util::format_double(_max, dec, ' ');
        _text = (l1.length() > l2.length()) ? l1.length() * cw : l2.length() * cw;
    }
}
Plot::Plot(int X, int Y, int W, int H, const char* l) :
Fl_Group(X, Y, W, H, l),
_CH(14), _CW(14) {
    end();
    clip_children(1);
    resizable(nullptr);
    color(FL_BACKGROUND2_COLOR);
    labelcolor(FL_FOREGROUND_COLOR);
    box(FL_BORDER_BOX);
    tooltip(
        "Press right button for menu.\n"
        "Press left button to show X and Y values and possible found data point.\n"
    );
    _menu = new Fl_Menu_Button(0, 0, 0, 0);
    add(_menu);
    _menu->add(_PLOT_SETUP_LABELS,  0, _FLW_PLOT_CB(setup_view_options()), FL_MENU_TOGGLE);
    _menu->add(_PLOT_SETUP_HLINES,  0, _FLW_PLOT_CB(setup_view_options()), FL_MENU_TOGGLE);
    _menu->add(_PLOT_SETUP_VLINES,  0, _FLW_PLOT_CB(setup_view_options()), FL_MENU_TOGGLE | FL_MENU_DIVIDER);
    _menu->add(_PLOT_CLEAR,         0, _FLW_PLOT_CB(clear()));
    _menu->add(_PLOT_SETUP_LABEL,   0, _FLW_PLOT_CB(setup_label(LABEL::MAIN)));
    _menu->add(_PLOT_SETUP_XLABEL,  0, _FLW_PLOT_CB(setup_label(LABEL::X)));
    _menu->add(_PLOT_SETUP_YLABEL,  0, _FLW_PLOT_CB(setup_label(LABEL::Y)));
    _menu->add(_PLOT_SETUP_MINX,    0, _FLW_PLOT_CB(setup_clamp(CLAMP::MINX)));
    _menu->add(_PLOT_SETUP_MAXX,    0, _FLW_PLOT_CB(setup_clamp(CLAMP::MAXX)));
    _menu->add(_PLOT_SETUP_MINY,    0, _FLW_PLOT_CB(setup_clamp(CLAMP::MINY)));
    _menu->add(_PLOT_SETUP_MAXY,    0, _FLW_PLOT_CB(setup_clamp(CLAMP::MAXY)), FL_MENU_DIVIDER);
    _menu->add(_PLOT_SETUP_LINE,    0, _FLW_PLOT_CB(setup_line()));
    _menu->add(_PLOT_ADD_LINE,      0, _FLW_PLOT_CB(setup_add_line()));
    _menu->add(_PLOT_LOAD_CVS,      0, _FLW_PLOT_CB(load_csv()), FL_MENU_DIVIDER);
    _menu->add(_PLOT_SETUP_SHOW,    0, _FLW_PLOT_CB(setup_show_or_hide_lines()));
    _menu->add(_PLOT_SETUP_DELETE,  0, _FLW_PLOT_CB(setup_delete_lines()), FL_MENU_DIVIDER);
    _menu->add(_PLOT_SAVE_CVS,      0, _FLW_PLOT_CB(save_csv()));
    _menu->add(_PLOT_SAVE_JSON,     0, _FLW_PLOT_CB(save_json()));
    _menu->add(_PLOT_LOAD_JSON,     0, _FLW_PLOT_CB(load_json()), FL_MENU_DIVIDER);
    _menu->add(_PLOT_PRINT,         0, _FLW_PLOT_CB(print()));
    _menu->add(_PLOT_SAVE_PNG,      0, _FLW_PLOT_CB(save_png()));
#ifdef DEBUG
    _menu->add(_PLOT_SAVE_PNG,      0, _FLW_PLOT_CB(save_png()), FL_MENU_DIVIDER);
    _menu->add(_PLOT_DEBUG,         0, _FLW_PLOT_CB(debug()));
    _menu->add(_PLOT_DEBUG_LINE,    0, _FLW_PLOT_CB(debug_line()));
#else
    _menu->add(_PLOT_SAVE_PNG,      0, _FLW_PLOT_CB(save_png()));
#endif
    _menu->type(Fl_Menu_Button::POPUP3);
    _disable_menu = false;
    clear();
    update_pref();
}
bool Plot::add_line(const PlotLine& line) {
    if (_lines.size() >= Plot::MAX_LINES) {
        return false;
    }
    _lines.push_back(line);
    return true;
}
void Plot::_calc_min_max() {
    _x.clear_min_max();
    _y.clear_min_max();
    for (const auto& line : _lines) {
        if (line.is_visible() == false) {
            continue;
        }
        double minx, maxx, miny, maxy;
        if (PlotData::MinMax(line.data(), minx, maxx, miny, maxy) == true) {
            if (_x.is_min_finite() == false) {
                _x.set_min(minx);
                _x.set_max(maxx);
                _y.set_min(miny);
                _y.set_max(maxy);
            }
            else {
                if (minx < _x.min()) {
                    _x.set_min(minx);
                }
                if (maxx > _x.max()) {
                    _x.set_max(maxx);
                }
                if (miny < _y.min()) {
                    _y.set_min(miny);
                }
                if (maxy > _y.max()) {
                    _y.set_max(maxy);
                }
            }
        }
    }
    if (_x.is_min_finite() == true) {
        if (std::isfinite(_min_x) == true) {
            _x.set_min(_min_x);
        }
        if (std::isfinite(_max_x) == true) {
            _x.set_max(_max_x);
        }
        if (std::isfinite(_min_y) == true) {
            _y.set_min(_min_y);
        }
        if (std::isfinite(_max_y) == true) {
            _y.set_max(_max_y);
        }
    }
}
bool Plot::_CallbackPrinter(void* data, int pw, int ph, int) {
    auto widget = static_cast<Fl_Widget*>(data);
    auto rect   = Fl_Rect(widget);
    widget->resize(0, 0, pw, ph);
    widget->draw();
    widget->resize(rect.x(), rect.y(), rect.w(), rect.h());
    return false;
}
void Plot::clear() {
    *const_cast<int*>(&_CW) = flw::PREF_FIXED_FONTSIZE;
    *const_cast<int*>(&_CH) = flw::PREF_FIXED_FONTSIZE;
    _old            = Fl_Rect();
    _selected_line  = 0;
    _selected_point = -1;
    _x              = PlotScale();
    _y              = PlotScale();
    _lines.clear();
    selection_color(FL_FOREGROUND_COLOR);
    set_hor_lines();
    set_label();
    set_line_labels();
    set_max_x();
    set_max_y();
    set_min_x();
    set_min_y();
    set_ver_lines();
    update_pref();
    init();
}
bool Plot::create_line(PlotData::FORMULAS formula) {
    if (_selected_line >= _lines.size()) {
        fl_alert("Line has not been selected!");
        return false;
    }
    else if (_lines.size() >= Plot::MAX_LINES) {
        fl_alert("Max line count reached!");
        return false;
    }
    PlotLine&      line0 = _lines[_selected_line];
    PlotDataVector vec1;
    std::string    label1;
    if (formula == PlotData::FORMULAS::MODIFY) {
        auto list = StringVector() = {"Addition", "Subtraction", "Multiplication", "Division"};
        auto ans  = dlg::choice("Select Modification", list, 0, top_window());
        if (ans < 0 || ans > static_cast<int>(PlotData::MODIFY::LAST)) {
            return false;
        }
        auto modify = static_cast<PlotData::MODIFY>(ans);
        auto value  = util::to_double(fl_input_str(16, "Enter value", "0"));
        if (std::isinf(value) == true) {
            return false;
        }
        else if (fabs(value) < PlotData::MIN_VALUE) {
            fl_alert("To small value for division!");
            return false;
        }
        list   = StringVector() = {"Only X", "Only Y", "Both X && Y"};
        ans    = dlg::choice("Select Target", list, 0, top_window());
        vec1   = PlotData::Modify(line0.data(), modify, (ans == 0 ? PlotData::TARGET::X : ans == 1 ? PlotData::TARGET::Y : PlotData::TARGET::X_AND_Y), value);
        label1 = util::format("Modified %s", line0.label().c_str());
    }
    else if (formula == PlotData::FORMULAS::SWAP) {
        vec1   = PlotData::Swap(line0.data());
        label1 = util::format("%s swapped X/Y", line0.label().c_str());
    }
    if (vec1.size() == 0) {
        fl_alert("To few data values!");
        return false;
    }
    auto line1 = PlotLine(vec1, label1);
    PlotLineSetup(top_window(), line1).run();
    _lines.push_back(line1);
    init_new_data();
    return true;
}
StringVector Plot::_create_check_labels(bool def) const {
    StringVector res;
    for (const auto& l : _lines) {
        std::string s = (def == true && l.is_visible() == true) ? "1" : "0";
        s += l.label();
        res.push_back(s);
    }
    return res;
}
void Plot::_create_tooltip() {
    const int   X   = Fl::event_x();
    const int   Y   = Fl::event_y();
    std::string old = _tooltip;
    _tooltip        = "";
    _selected_point = -1;
    if (X >= _area.x() && X < _area.r() && Y >= _area.y() && Y < _area.b()) {
        const int X_FR = _x.fr() + (_x.tick() <= 1.001 ? 2 : _x.tick() <= 10.001 ? 1 : 0);
        const int Y_FR = _y.fr() + (_y.tick() <= 1.001 ? 2 : _y.tick() <= 10.001 ? 1 : 0);
        if (_selected_line < _lines.size()) {
            const auto& line = _lines[_selected_line];
            if (line.is_visible() == true) {
                const int    RAD = (line.has_radius() == true) ? line.width() / 2 : 4;
                const double XV1 = (static_cast<double>(X - _area.x() - RAD) / _x.pixel()) + _x.min();
                const double XV2 = (static_cast<double>(X - _area.x() + RAD) / _x.pixel()) + _x.min();
                const double YV1 = (static_cast<double>(_area.b() - Y - RAD) / _y.pixel()) + _y.min();
                const double YV2 = (static_cast<double>(_area.b() - Y + RAD) / _y.pixel()) + _y.min();
                size_t       p   = 0;
                for (const auto& d : line.data()) {
                    if (d.x >= XV1 && d.x <= XV2 && d.y >= YV1 && d.y <= YV2) {
                        std::string xl  = util::format_double(d.x, X_FR, ' ');
                        std::string yl  = util::format_double(d.y, Y_FR, ' ');
                        size_t      len = (xl.length() > yl.length()) ? xl.length() : yl.length();
                        _tooltip = flw::util::format("%s\n[%d].X = %*s\n[%d].Y = %*s", line.label().c_str(), p, len, xl.c_str(), p, len, yl.c_str());
                        _selected_point = p;
                        break;
                    }
                    p++;
                }
            }
        }
        if (_tooltip == "") {
            const double      XV  = (static_cast<double>(X - _area.x()) / _x.pixel()) + _x.min();
            const double      YV  = (static_cast<double>(_area.b() - Y) / _y.pixel()) + _y.min();
            const std::string XL  = util::format_double(XV, X_FR, ' ');
            const std::string YL  = util::format_double(YV, Y_FR, ' ');
            const size_t      LEN = (XL.length() > YL.length()) ? XL.length() : YL.length();
            _tooltip = flw::util::format("X = %*s\nY = %*s\n ", LEN, XL.c_str(), LEN, YL.c_str());
        }
    }
    if (_tooltip != "" || old != "") {
        redraw();
    }
}
void Plot::debug() const {
#ifdef DEBUG
    printf("\n-----------------------------------------------------\n");
    printf("Plot:\n");
    printf("\twidget          = (%4d, %4d) - (%4d, %4d)\n", x(), y(), w(), h());
    printf("\tarea            = (%4d, %4d) - (%4d, %4d)\n", _area.x(), _area.y(), _area.w(), _area.h());
    printf("\tfont            = cw: %d, ch: %d\n", _CW, _CH);
    printf("\tselected_line   = %d\n", static_cast<int>(_selected_line));
    printf("\tselected_point  = %d\n", static_cast<int>(_selected_point));
    printf("\tlines           = %d\n", static_cast<int>(_lines.size()));
    printf("\tlabels          = %s\n", _labels ? "YES" : "NO");
    printf("\thorizontal      = %s\n", _horizontal ? "YES" : "NO");
    printf("\tvertical        = %s\n", _vertical ? "YES" : "NO");
    printf("\tmin_x           = %24.8f\n", _min_x);
    printf("\tmax_x           = %24.8f\n", _max_x);
    printf("\tmin_y           = %24.8f\n", _min_y);
    printf("\tmax_y           = %24.8f\n", _max_y);
    printf("\tlabel           = %s\n", _label.c_str());
    _x.debug("X");
    _y.debug("Y");
    for (const auto& line : _lines) {
        line.debug();
    }
    fflush(stdout);
#endif
}
void Plot::debug_line() const {
#ifdef DEBUG
    if (_selected_line >= _lines.size()) {
        return;
    }
    PlotData::Debug(_lines[_selected_line].data());
#endif
}
void Plot::delete_line(size_t index) {
    size_t count = 0;
    for (auto it = _lines.begin(); it != _lines.end(); ++it) {
        if (index == count) {
            _lines.erase(it);
            if (_lines.size() == 0) {
                _selected_line = 0;
            }
            else if (_selected_line >= _lines.size()) {
                _selected_line = _lines.size() - 1;
            }
            break;
        }
        count++;
    }
}
void Plot::draw() {
#ifdef DEBUG
#endif
    fl_rectf(x(), y(), w(), h(), FL_BACKGROUND2_COLOR);
    if (_area.w() < 75 || _area.h() < 50 || _x.min() >= _x.max() || _x.pixel() * _x.tick() < 1.0 || _y.min() >= _y.max() || _y.pixel() * _y.tick() < 1.0) {
        goto DRAW_RECTANGLE;
    }
    _FLW_PLOT_CLIP(fl_push_clip(x(), y(), w(), h()))
    if (_lines.size() > 0) {
        if (_x.custom_labels().size() > 0 && _lines[0].type() == PlotLine::TYPE::VECTOR) {
            _draw_xlabels_custom();
        }
        else {
            _draw_xlabels();
        }
        if (_y.custom_labels().size() > 0 && _lines[0].type() == PlotLine::TYPE::VECTOR) {
            _draw_ylabels_custom();
        }
        else {
            _draw_ylabels();
        }
    }
    _FLW_PLOT_CLIP(fl_pop_clip())
    if (_lines.size() > 0) {
        _FLW_PLOT_CLIP(fl_push_clip(_area.x(), _area.y(), _area.w(), _area.h()))
        _draw_lines();
        _draw_line_labels();
        _draw_tooltip();
        _FLW_PLOT_CLIP(fl_pop_clip())
    }
DRAW_RECTANGLE:
    _FLW_PLOT_CLIP(fl_push_clip(x(), y(), w(), h()))
    _draw_labels();
    fl_color(FL_FOREGROUND_COLOR);
    fl_line_style(FL_SOLID, 1);
    fl_rect(_area.x(), _area.y(), _area.w(), _area.h(), FL_FOREGROUND_COLOR);
    fl_line_style(0);
    _FLW_PLOT_CLIP(fl_pop_clip())
#ifdef DEBUG
#endif
}
void Plot::_draw_labels() {
    const int CH = _CH / 2;
    if (_x.label() != "") {
        fl_color(_x.color());
        fl_font(flw::PREF_FIXED_FONT, _CH);
        fl_draw (_x.label().c_str(), _area.x(), y() + h() - _CH * 2, _area.w(), _CH * 2, FL_ALIGN_CENTER | FL_ALIGN_INSIDE | FL_ALIGN_CLIP);
        _FLW_PLOT_DEBUG(fl_rect(_area.x(), y() + h() - _CH * 2, _area.w(), _CH * 2))
    }
    if (_y.label() != "") {
        int w1 = 0;
        int h1 = 0;
        fl_measure(_y.label().c_str(), w1, h1);
        const int W2 = w1 / 2;
        const int M2 = _area.h() / 2 - W2;
        fl_color(_y.color());
        fl_font(flw::PREF_FIXED_FONT, _CH);
        fl_draw (90, _y.label().c_str(), x() + _CH + CH, _area.y() + M2 + w1);
        _FLW_PLOT_DEBUG(fl_rect(x(), _area.y() + M2, _CH * 2, w1))
    }
    if (_label != "") {
        fl_font(flw::PREF_FIXED_FONT, _CH + CH);
        fl_color(FL_FOREGROUND_COLOR);
        fl_draw(_label.c_str(), _area.x(), y(), _area.w(), _CH * 3 + 1, FL_ALIGN_CENTER | FL_ALIGN_INSIDE | FL_ALIGN_CLIP);
        _FLW_PLOT_DEBUG(fl_rect(_area.x(), y(),  _area.w(), _CH * 3 + 1))
    }
}
void Plot::_draw_line_labels() {
    static const std::string SYMBOL = "@-> ";
    if (_labels == false) {
        return;
    }
    fl_font(flw::PREF_FIXED_FONT, _CH);
    int w1 = 0;
    for (const auto& line : _lines) {
        int ws = 0;
        int hs = 0;
        fl_measure((SYMBOL + line.label()).c_str(), ws, hs);
        if (ws > w1) {
            w1 = ws;
        }
    }
    if (w1 > 0) {
        const int X      = _area.x() + 6;
        int       y1     = _area.y() + 6;
        int       h1     = static_cast<int>(_lines.size());
        size_t    line_c = 0;
        h1 *= fl_height();
        h1 += 8;
        fl_color(FL_BACKGROUND2_COLOR);
        fl_line_style(FL_SOLID, 1);
        fl_rectf(X, y1, w1 + _CW, h1);
        fl_color(FL_FOREGROUND_COLOR);
        fl_line_style(FL_SOLID, 1);
        fl_rect(X, y1, w1 + _CW, h1);
        for (auto& line : _lines) {
            auto label = (_selected_line == line_c++) ? SYMBOL + line.label() : line.label();
            fl_color((line.is_visible() == false) ? FL_GRAY : line.color());
            fl_draw(label.c_str(), X + 4, y1 + 4, w1, fl_height(), FL_ALIGN_LEFT | FL_ALIGN_INSIDE | FL_ALIGN_CLIP, nullptr, 1);
            _FLW_PLOT_DEBUG(fl_rect(X + 4, y1 + 4, w1, fl_height()))
            line.set_label_rect(X, y1 + 4, w1, fl_height());
            y1 += fl_height();
        }
    }
}
void Plot::_draw_lines() {
    const int X1     = _area.x();
    const int Y1     = _area.b() - 1;
    size_t    line_c = -1;
    for (const auto& line : _lines) {
        const auto  TYPE   = line.type();
        const int   WIDTH1 = static_cast<int>(line.width());
        const int   WIDTH2 = WIDTH1 / 2;
        int         x2     = 10'000;
        int         y2     = 0;
        size_t      data_c = 0;
        line_c++;
        if (line.is_visible() == false) {
            continue;
        }
        for (const auto& data : line.data()) {
            if (std::isinf(data.x) == true || std::isinf(data.y) == true) {
                continue;
            }
            Fl_Color  col = line.color();
            const int X3  = X1 + static_cast<int>((data.x - _x.min()) * _x.pixel());
            const int Y3  = Y1 - static_cast<int>((data.y - _y.min()) * _y.pixel());
            if (TYPE == PlotLine::TYPE::VECTOR && (_selected_point == data_c || _selected_point == data_c - 1) && _selected_line == line_c) {
                col = selection_color();
            }
            else if (TYPE != PlotLine::TYPE::VECTOR && _selected_point == data_c && _selected_line == line_c) {
                col = selection_color();
            }
            fl_color(col);
            if (line.has_line_type() == true) {
                if (x2 == 10'000) {
                    x2 = X3;
                    y2 = Y3;
                }
                else {
                    if (TYPE == PlotLine::TYPE::LINE_DASH) {
                        fl_line_style(FL_DASH, WIDTH1);
                    }
                    else if (TYPE == PlotLine::TYPE::LINE_DOT) {
                        fl_line_style(FL_DOT, WIDTH1);
                    }
                    else {
                        fl_line_style(FL_SOLID, WIDTH1);
                    }
                    fl_line(x2, y2, X3, Y3);
                    x2 = X3;
                    y2 = Y3;
                }
                if (TYPE == PlotLine::TYPE::LINE_WITH_SQUARE) {
                    fl_rectf(X3 - WIDTH1 - 3, Y3 - WIDTH1 - 3, WIDTH1 * 2 + 6, WIDTH1 * 2 + 6);
                }
            }
            else if (TYPE == PlotLine::TYPE::VECTOR) {
                if (x2 == 10'000) {
                    x2 = X3;
                    y2 = Y3;
                }
                else {
                    fl_line_style(FL_SOLID, WIDTH1);
                    fl_line(X3, Y3, x2, y2);
                    x2 = 10'000;
                }
            }
            else if (TYPE == PlotLine::TYPE::CIRCLE) {
                fl_line_style(FL_SOLID, WIDTH1 / 25 + 1);
                fl_begin_loop();
                fl_circle(X3, Y3 - 1, WIDTH2);
                fl_end_loop();
            }
            else if (TYPE == PlotLine::TYPE::FILLED_CIRCLE) {
                fl_draw_circle(X3 - WIDTH2, Y3 - WIDTH2 - 1, WIDTH1, col);
            }
            else if (TYPE == PlotLine::TYPE::SQUARE) {
                fl_rectf(X3 - WIDTH2, Y3 - WIDTH2 - 1, WIDTH1, WIDTH1);
            }
            data_c++;
            fl_line_style(0);
        }
    }
}
void Plot::_draw_tooltip() {
    if (_tooltip == "") {
        return;
    }
    auto x1 = Fl::event_x();
    auto y1 = Fl::event_y();
    if (x1 > _area.r() - _CH * 21) {
        x1 -= _CH * 20;
    }
    else {
        x1 += _CH * 2;
    }
    if (y1 > _area.b() - _CH * 8) {
        y1 -= _CH * 7;
    }
    else {
        y1 += _CH * 2;
    }
    fl_color(FL_BACKGROUND2_COLOR);
    fl_line_style(FL_SOLID, 1);
    fl_rectf(x1, y1, _CH * 18, _CH * 4);
    fl_color(FL_FOREGROUND_COLOR);
    fl_line_style(FL_SOLID, 1);
    fl_rect(x1, y1, _CH * 18, _CH * 4);
    fl_line_style(FL_SOLID, 1);
    fl_line(Fl::event_x(), _area.y(), Fl::event_x(), _area.b() - 1);
    fl_line(_area.x(), Fl::event_y(), _area.r() - 1, Fl::event_y());
    fl_draw(_tooltip.c_str(), x1 + 4, y1, _CH * 18 - 8, _CH * 4, FL_ALIGN_LEFT | FL_ALIGN_CENTER | FL_ALIGN_INSIDE | FL_ALIGN_CLIP);
    fl_line_style(0);
}
void Plot::_draw_xlabels() {
    fl_font(flw::PREF_FIXED_FONT, _CH);
    double       x1     = _area.x();
    const int    Y      = _area.b();
    const int    R      = x() + w();
    const double X_INC  = _x.pixel() * _x.tick();
    double       x_val  = _x.min();
    double       x_last = -10'000;
    const int    TW     = _x.text() / 2;
    while (x1 < _area.r()) {
        if (x1 > (x_last + TW) && (x1 + TW + _CW) < R) {
            auto label = util::format_double(x_val, _x.fr(), '\'');
            if (static_cast<int>(x1 + 1) == (_area.r() - 1)) {
                x1 += 1.0;
            }
            if (_vertical == true) {
                fl_color(fl_color_average(FL_FOREGROUND_COLOR, FL_BACKGROUND2_COLOR, 0.2));
                fl_line(x1, _area.y() + 1, x1, _area.b() - 2);
            }
            _FLW_PLOT_DEBUG(fl_rect(x1 - TW, Y + _PLOT_TICK_SIZE * 2, TW * 2, _CH, FL_FOREGROUND_COLOR))
            fl_color(FL_FOREGROUND_COLOR);
            fl_line(x1, Y, x1, Y + _PLOT_TICK_SIZE);
            fl_draw(label.c_str(), x1 - TW, Y + _PLOT_TICK_SIZE * 2, TW * 2, _CH, FL_ALIGN_CENTER | FL_ALIGN_INSIDE);
            x_last = x1 + TW + _CH;
        }
        x1    += X_INC;
        x_val += _x.tick();
    }
}
void Plot::_draw_xlabels_custom() {
    fl_color(FL_FOREGROUND_COLOR);
    fl_font(flw::PREF_FIXED_FONT, _CH);
    size_t label_c = 0;
    size_t data_c  = 0;
    double x_last  = 0;
    for (const auto& data : _lines[0].data()) {
        if (label_c >= _x.custom_labels().size()) {
            break;
        }
        if (data_c++ % 2 == 1) {
            std::string label = _x.custom_labels()[label_c++];
            const int   X     = _area.x() + static_cast<int>((data.x - _x.min()) * _x.pixel());
            const int   W     = label.length() * _CW / 2;
            fl_line(X, _area.b(), X, _area.b() + _PLOT_TICK_SIZE);
            if (X - W >= x_last && label != "") {
                _FLW_PLOT_DEBUG(fl_rect(X - W, _area.b() + _PLOT_TICK_SIZE * 2, W * 2, _CH))
                fl_draw(label.c_str(), X - W, _area.b() + _PLOT_TICK_SIZE * 2, W * 2, _CH, FL_ALIGN_CENTER | FL_ALIGN_INSIDE | FL_ALIGN_CLIP);
                x_last = X + W + _PLOT_TICK_SIZE;
            }
        }
    }
}
void Plot::_draw_ylabels() {
    fl_font(flw::PREF_FIXED_FONT, _CH);
    const int X      = x() + (_y.label() != "" ? _CH * 2 : 0);
    const int W      = _area.x() - X - _PLOT_TICK_SIZE * 3;
    const int CH     = _CH / 2;
    double    y1     = _area.b() - 1.0;
    double    y_tick = _y.tick();
    double    y_inc  = _y.pixel() * y_tick;
    double    y_val  = _y.min();
    double    y_last = 10'000;
    while (static_cast<int>(y1 + 0.5) >= _area.y()) {
        if (y_last > y1) {
            auto label = util::format_double(y_val, _y.fr(), '\'');
            if (y1 < static_cast<double>(_area.y())) {
                y1 = _area.y();
            }
            if (_horizontal == true) {
                fl_color(fl_color_average(FL_FOREGROUND_COLOR, FL_BACKGROUND2_COLOR, 0.2));
                fl_line(_area.x() + 1, y1, _area.r() - 2, y1);
            }
            fl_color(FL_FOREGROUND_COLOR);
            fl_line(_area.x(), y1, _area.x() - _PLOT_TICK_SIZE, y1);
            fl_draw(label.c_str(), X + _PLOT_TICK_SIZE, y1 - CH, W, _CH, FL_ALIGN_RIGHT | FL_ALIGN_INSIDE | FL_ALIGN_CLIP);
            _FLW_PLOT_DEBUG(fl_rect(X + _PLOT_TICK_SIZE, y1 - CH, W, _CH, FL_FOREGROUND_COLOR))
            y_last = y1 - _CH - _PLOT_TICK_SIZE;
        }
        y1    -= y_inc;
        y_val += y_tick;
    }
}
void Plot::_draw_ylabels_custom() {
    fl_color(FL_FOREGROUND_COLOR);
    fl_font(flw::PREF_FIXED_FONT, _CH);
    const int  X       = x() + (_y.label() != "" ? _CH * 2 : _PLOT_TICK_SIZE);
    const int  W       = _area.x() - X - _PLOT_TICK_SIZE * 3;
    const int  CH      = fl_height() / 2;
    size_t     label_c = 0;
    size_t     data_c  = 0;
    double     y_last  = 10'000;
    for (const auto& data : _lines[0].data()) {
        if (label_c >= _y.custom_labels().size()) {
            break;
        }
        if (data_c++ % 2 == 1) {
            const int   Y     = _area.b() - 1 - static_cast<int>((data.y - _y.min()) * _y.pixel());
            std::string label = _y.custom_labels()[label_c++];
            fl_line(_area.x(), Y, _area.x() - _PLOT_TICK_SIZE, Y);
            if (y_last > Y && label != "") {
                fl_draw(label.c_str(), X + _PLOT_TICK_SIZE, Y - CH, W, fl_height(), FL_ALIGN_LEFT | FL_ALIGN_INSIDE | FL_ALIGN_CLIP);
                _FLW_PLOT_DEBUG(fl_rect(X + _PLOT_TICK_SIZE, Y - CH, W, fl_height(), FL_FOREGROUND_COLOR))
                y_last = Y - fl_height() - _PLOT_TICK_SIZE;
            }
        }
    }
}
int Plot::handle(int event) {
    if (event == FL_PUSH) {
        auto b = Fl::event_button();
        if (b == FL_LEFT_MOUSE) {
            _create_tooltip();
            if (_tooltip != "") {
                return 1;
            }
        }
        else if (b == FL_MIDDLE_MOUSE) {
            const int X      = Fl::event_x();
            const int Y      = Fl::event_y();
            size_t    line_c = 0;
            for (const auto& line : _lines) {
                auto& r = line.label_rect();
                if (X > r.x() && X < r.r() && Y > r.y() && Y < r.b()) {
                    _selected_line  = line_c;
                    _selected_point = -1;
                    redraw();
                    break;
                }
                line_c++;
            }
        }
        else if (b == FL_RIGHT_MOUSE) {
            _show_menu();
            return 1;
        }
    }
    else if (event == FL_DRAG) {
        _create_tooltip();
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
    return Fl_Group::handle(event);
}
void Plot::_init(bool new_data) {
    fl_font(flw::PREF_FIXED_FONT, flw::PREF_FIXED_FONTSIZE);
    *const_cast<int*>(&_CW) = fl_width("X");
    *const_cast<int*>(&_CH) = flw::PREF_FIXED_FONTSIZE;
    if (new_data == true) {
        _calc_min_max();
        _x.calc_tick(w() - _CH * 6);
        _y.calc_tick(h() - _CH * 6);
        _x.measure_labels(_CW, _lines.size() > 0 ? _lines[0].is_vector() : false);
        _y.measure_labels(_CW, _lines.size() > 0 ? _lines[0].is_vector() : false);
    }
    auto move_x   = (_y.label() != "") ? _CH * 2 : 0;
    auto shrink_h = (_x.label() != "") ? _CH * 2 + _PLOT_TICK_SIZE * 2 : _CH;
    _area.x(x() + (_CH * 2) + _y.text() + move_x);
    _area.w(w() - (_area.x() - x()) - _CH * 2);
    if (_label != "") {
        _area.y(y() + _CH * 3);
        _area.h(h() - (_CH * 4) - shrink_h);
    }
    else {
        _area.y(y() + _CH);
        _area.h(h() - (_CH * 2) - shrink_h);
    }
    _x.calc_tick(_area.w() - 1);
    _y.calc_tick(_area.h() - 1);
    redraw();
}
bool Plot::load_csv() {
    if (_lines.size() >= Plot::MAX_LINES) {
        fl_alert("Max line count reached!");
        return false;
    }
    auto filename = util::to_string(fl_file_chooser("Select CSV File", "All Files (*)\tCSV Files (*.csv)", ""));
    if (util::is_whitespace_or_empty(filename.c_str()) == true) {
        return false;
    }
    auto vec = PlotData::LoadCSV(filename);
    if (vec.size() == 0) {
        fl_alert("To few data values!");
        return false;
    }
    auto line = PlotLine(vec, filename);
    PlotLineSetup(top_window(), line).run();
    _lines.push_back(line);
    init_new_data();
    return true;
}
bool Plot::load_json() {
    auto filename = util::to_string(fl_file_chooser("Select JSON File", "All Files (*)\tJSON Files (*.json)", ""));
    if (util::is_whitespace_or_empty(filename.c_str()) == true) {
        return false;
    }
    return load_json(filename);
}
bool Plot::load_json(std::string filename) {
    clear();
    redraw();
    auto wc  = WaitCursor();
    auto buf = File::Load(filename);
    if (buf.p == nullptr) {
        fl_alert("error: failed to load %s", filename.c_str());
        return false;
    }
    auto js  = JS();
    auto err = js.decode(buf.p, buf.s);
    auto x   = PlotScale();
    auto y   = PlotScale();
    double clamp[4] = { INFINITY, INFINITY, INFINITY, INFINITY };
    if (err != "") {
        fl_alert("error: failed to parse %s (%s)", filename.c_str(), err.c_str());
        return false;
    }
    if (js.is_object() == false) _FLW_PLOT_ERROR(&js);
    for (auto j : js.vo_to_va()) {
        if (j->name() == "Plot" && j->is_object() == true) {
            for (const auto j2 : j->vo_to_va()) {
                if (j2->name() == "version" && j2->is_number() == true) {
                    if (j2->vn_i() != Plot::VERSION) _FLW_PLOT_ERROR(j2)
                }
                else if (j2->name() == "label" && j2->is_string() == true)    set_label(j2->vs_u());
                else if (j2->name() == "horizontal" && j2->is_bool() == true) set_hor_lines(j2->vb());
                else if (j2->name() == "labels" && j2->is_bool() == true)     set_line_labels(j2->vb());
                else if (j2->name() == "vertical" && j2->is_bool() == true)   set_ver_lines(j2->vb());
                else if (j2->name() == "minx" && j2->is_number() == true)     clamp[0] = j2->vn();
                else if (j2->name() == "maxx" && j2->is_number() == true)     clamp[1] = j2->vn();
                else if (j2->name() == "miny" && j2->is_number() == true)     clamp[2] = j2->vn();
                else if (j2->name() == "maxy" && j2->is_number() == true)     clamp[3] = j2->vn();
                else _FLW_PLOT_ERROR(j2)
            }
        }
        else if ((j->name() == "PlotScale::x" || j->name() == "PlotScale::y") && j->is_object() == true) {
            auto& scale = (j->name() == "PlotScale::x") ? x : y;
            StringVector labels;
            for (auto s : j->vo_to_va()) {
                if (s->name() == "color")      scale.set_color(s->vn_i());
                else if (s->name() == "label") scale.set_label(s->vs_u());
                else if (s->name() == "labels" && s->is_array() == true) {
                    for (auto s2 : *s->va()) {
                        if (s2->is_string() == true) labels.push_back(s2->vs_u());
                        else _FLW_PLOT_ERROR(s2)
                    }
                }
                else _FLW_PLOT_ERROR(s)
            }
            scale.set_custom_labels(labels);
        }
        else if (j->name() == "PlotLine" && j->is_array() == true) {
            for (const auto j2 : *j->va()) {
                if (j2->is_object() == true) {
                    PlotLine line;
                    PlotDataVector data;
                    for (auto j3 : j2->vo_to_va()) {
                        if (j3->name() == "color" && j3->is_number() == true)      line.set_color((Fl_Color) j3->vn_i());
                        else if (j3->name() == "label" && j3->is_string() == true) line.set_label(j3->vs_u());
                        else if (j3->name() == "type" && j3->is_string() == true)  line.set_type_from_string(j3->vs());
                        else if (j3->name() == "visible" && j3->is_bool() == true) line.set_visible(j3->vb());
                        else if (j3->name() == "width" && j3->is_number() == true) line.set_width((unsigned) j3->vn_i());
                        else if (j3->name() == "xy" && j3->is_array() == true) {
                            for (auto p : *j3->va()) {
                                if (p->is_array() == true && p->size() == 2 && (*p)[0]->is_number() == true && (*p)[1]->is_number() == true) {
                                    data.push_back(PlotData((*p)[0]->vn(), (*p)[1]->vn()));
                                }
                                else _FLW_PLOT_ERROR(p)
                            }
                        }
                        else _FLW_PLOT_ERROR(j3)
                    }
                    line.set_data(data);
                    if (add_line(line) == false) _FLW_PLOT_ERROR(j2)
                }
            }
        }
    }
    xscale().set_label(x.label());
    yscale().set_label(y.label());
    xscale().set_color(x.color());
    yscale().set_color(x.color());
    xscale().set_custom_labels(x.custom_labels());
    yscale().set_custom_labels(y.custom_labels());
    set_min_x(clamp[0]);
    set_max_x(clamp[1]);
    set_min_y(clamp[2]);
    set_max_y(clamp[3]);
    init_new_data();
    return true;
}
void Plot::print() {
    dlg::print("Print Plot", Plot::_CallbackPrinter, this, 1, 1, top_window());
    redraw();
}
void Plot::resize(int X, int Y, int W, int H) {
    if (_old.w() == W && _old.h() == H) {
        return;
    }
    Fl_Widget::resize(X, Y, W, H);
    _old = Fl_Rect(this);
    init();
}
bool Plot::save_csv() {
    if (_lines.size() == 0 || _selected_line >= _lines.size()) {
        return false;
    }
    auto filename = util::to_string(fl_file_chooser("Save To CSV File", "All Files (*)\tCSV Files (*.csv)", ""));
    if (util::is_whitespace_or_empty(filename.c_str()) == true) {
        return false;
    }
    return PlotData::SaveCSV(_lines[_selected_line].data(), filename);
}
bool Plot::save_json() {
    auto filename = util::to_string(fl_file_chooser("Save To JSON File", "All Files (*)\tJSON Files (*.json)", ""));
    if (util::is_whitespace_or_empty(filename.c_str()) == true) {
        return false;
    }
    return save_json(filename);
}
bool Plot::save_json(std::string filename) {
    auto wc  = WaitCursor();
    auto jsb = JSB();
    try {
        jsb << JSB::MakeObject();
            jsb << JSB::MakeObject("Plot");
                jsb << JSB::MakeNumber(Plot::VERSION, "version");
                jsb << JSB::MakeString(_label, "label");
                jsb << JSB::MakeBool(_labels, "labels");
                jsb << JSB::MakeBool(_horizontal, "horizontal");
                jsb << JSB::MakeBool(_vertical, "vertical");
                if (std::isfinite(_min_x) == true) jsb << JSB::MakeNumber(_min_x, "minx");
                if (std::isfinite(_max_x) == true) jsb << JSB::MakeNumber(_max_x, "maxx");
                if (std::isfinite(_min_y) == true) jsb << JSB::MakeNumber(_min_y, "miny");
                if (std::isfinite(_max_y) == true) jsb << JSB::MakeNumber(_max_y, "maxy");
            jsb.end();
            jsb << JSB::MakeObject("PlotScale::x");
                jsb << JSB::MakeString(_x.label().c_str(), "label");
                jsb << JSB::MakeNumber(_x.color(), "color");
                jsb << JSB::MakeArrayInline("labels");
                    for (const auto& l : _x.custom_labels()) jsb << JSB::MakeString(l);
                jsb.end();
            jsb.end();
            jsb << JSB::MakeObject("PlotScale::y");
                jsb << JSB::MakeString(_y.label().c_str(), "label");
                jsb << JSB::MakeNumber(_y.color(), "color");
                jsb << JSB::MakeArrayInline("labels");
                    for (const auto& l : _y.custom_labels()) jsb << JSB::MakeString(l);
                jsb.end();
            jsb.end();
            jsb << JSB::MakeArray("PlotLine");
            for (const auto& line : _lines) {
                if (line.data().size() > 0) {
                    jsb << JSB::MakeObject();
                        jsb << JSB::MakeNumber(line.width(), "width");
                        jsb << JSB::MakeNumber(line.color(), "color");
                        jsb << JSB::MakeString(line.label(), "label");
                        jsb << JSB::MakeBool(line.is_visible(), "visible");
                        jsb << JSB::MakeString(line.type_to_string(), "type");
                        jsb << JSB::MakeArray("xy");
                        for (const auto& point : line.data()) {
                            if (std::isfinite(point.x) == true && std::isfinite(point.y) == true) {
                                jsb << JSB::MakeArrayInline();
                                    jsb << JSB::MakeNumber(point.x);
                                    jsb << JSB::MakeNumber(point.y);
                                jsb.end();
                            }
                        }
                        jsb.end();
                    jsb.end();
                }
            }
        auto js = jsb.encode();
        return File::Save(filename, js.c_str(), js.length());
    }
    catch(const std::string& e) {
        fl_alert("error: failed to encode json\n%s", e.c_str());
        return false;
    }
}
bool Plot::save_png() {
    return flw::util::png_save("", window(), x(),  y(),  w(),  h());
}
void Plot::setup_add_line() {
    if (_selected_line >= _lines.size()) {
        fl_alert("Line has not been selected!");
        return;
    }
    else if (_lines.size() >= Plot::MAX_LINES) {
        fl_alert("Max line count reached!");
        return;
    }
    auto list = StringVector() = {
        "Modify",
        "Swap values",
    };
    switch (dlg::choice("Select Formula", list, 0, top_window())) {
    case 0:
        create_line(PlotData::FORMULAS::MODIFY);
        break;
    case 1:
        create_line(PlotData::FORMULAS::SWAP);
        break;
    default:
        break;
    }
}
void Plot::setup_clamp(Plot::CLAMP clamp) {
    double      val = 0.0;
    std::string info;
    switch (clamp) {
    case CLAMP::MINX:
        val = _min_x;
        info = "Set min x value or inf  to disable";
        break;
    case CLAMP::MAXX:
        val = _max_x;
        info = "Set max x value or inf  to disable";
        break;
    case CLAMP::MINY:
        val = _min_y;
        info = "Set min y value or inf  to disable";
        break;
    case CLAMP::MAXY:
        val = _max_y;
        info = "Set max y value or inf  to disable";
        break;
    }
    auto input  = (std::isfinite(val) == true) ? util::format("%f", val) : "inf";
    auto output = fl_input_str(16, "%s", input.c_str(), info.c_str());
    if (output == "") {
        return;
    }
    val = util::to_double(output);
    switch (clamp) {
    case CLAMP::MINX:
        _min_x = val;
        break;
    case CLAMP::MAXX:
        _max_x = val;
        break;
    case CLAMP::MINY:
        _min_y = val;
        break;
    case CLAMP::MAXY:
        _max_y = val;
        break;
    }
    init_new_data();
}
void Plot::setup_delete_lines() {
    if (_lines.size() == 0) {
        return;
    }
    auto list = _create_check_labels(false);
    list = dlg::check("Delete Lines", list, top_window());
    if (list.size() == 0) {
        return;
    }
    for (size_t f = 0, e = 0; f < list.size(); f++, e++) {
        if (list[f][0] == '1') {
            delete_line(e);
            e--;
        }
    }
    init_new_data();
}
void Plot::setup_label(Plot::LABEL val) {
    const char* l = nullptr;
    switch (val) {
    case Plot::LABEL::MAIN:
        l = fl_input(40, "Enter main label", _label.c_str());
        break;
    case Plot::LABEL::X:
        l = fl_input(40, "Enter x label", _x.label().c_str());
        break;
    case Plot::LABEL::Y:
        l = fl_input(40, "Enter y label", _y.label().c_str());
        break;
    }
    if (l != nullptr) {
        switch (val) {
        case Plot::LABEL::MAIN:
            _label = l;
            break;
        case Plot::LABEL::X:
            _x.set_label(l);
            break;
        case Plot::LABEL::Y:
            _y.set_label(l);
            break;
        }
        init();
    }
}
void Plot::setup_line() {
    if (_lines.size() == 0 || _selected_line >= _lines.size()) {
        return;
    }
    auto& line = _lines[_selected_line];
    auto ok    = PlotLineSetup(top_window(), line).run();
    if (ok == false) {
        return;
    }
    redraw();
}
void Plot::setup_show_or_hide_lines() {
    if (_lines.size() == 0) {
        return;
    }
    auto list = _create_check_labels(true);
    list = dlg::check("Show Or Hide Lines", list, top_window());
    if (list.size() == 0) {
        return;
    }
    for (size_t f = 0; f < list.size(); f++) {
        _lines[f].set_visible(list[f][0] == '1');
    }
    init_new_data();
}
void Plot::setup_view_options() {
    _labels     = menu::item_value(_menu, _PLOT_SETUP_LABELS);
    _horizontal = menu::item_value(_menu, _PLOT_SETUP_HLINES);
    _vertical   = menu::item_value(_menu, _PLOT_SETUP_VLINES);
    redraw();
}
void Plot::_show_menu() {
    if (_disable_menu == true) {
        return;
    }
    menu::set_item(_menu, _PLOT_SETUP_LABELS, _labels);
    menu::set_item(_menu, _PLOT_SETUP_HLINES, _horizontal);
    menu::set_item(_menu, _PLOT_SETUP_VLINES, _vertical);
    if (_lines.size() == 0) {
        menu::enable_item(_menu, _PLOT_ADD_LINE, false);
        menu::enable_item(_menu, _PLOT_SAVE_CVS, false);
        menu::enable_item(_menu, _PLOT_SETUP_DELETE, false);
        menu::enable_item(_menu, _PLOT_SETUP_LINE, false);
        menu::enable_item(_menu, _PLOT_SETUP_MAXX, false);
        menu::enable_item(_menu, _PLOT_SETUP_MAXY, false);
        menu::enable_item(_menu, _PLOT_SETUP_MINX, false);
        menu::enable_item(_menu, _PLOT_SETUP_MINY, false);
        menu::enable_item(_menu, _PLOT_SETUP_SHOW, false);
    }
    else {
        menu::enable_item(_menu, _PLOT_ADD_LINE, true);
        menu::enable_item(_menu, _PLOT_SAVE_CVS, true);
        menu::enable_item(_menu, _PLOT_SETUP_DELETE, true);
        menu::enable_item(_menu, _PLOT_SETUP_LINE, true);
        menu::enable_item(_menu, _PLOT_SETUP_MAXX, true);
        menu::enable_item(_menu, _PLOT_SETUP_MAXY, true);
        menu::enable_item(_menu, _PLOT_SETUP_MINX, true);
        menu::enable_item(_menu, _PLOT_SETUP_MINY, true);
        menu::enable_item(_menu, _PLOT_SETUP_SHOW, true);
    }
    _menu->popup();
}
void Plot::update_pref() {
    _menu->textfont(flw::PREF_FONT);
    _menu->textsize(flw::PREF_FONTSIZE);
}
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
        for (const auto& f : _files) {
            _menu->add((_base + "/" + flw::util::fix_menu_string(f)).c_str(), 0, _callback, _user);
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
        auto self = static_cast<RecentMenu*>(o);
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
        for (const auto& s : _files) {
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
    static_cast<Fl_Group*>(this)->add(_menu);
    _menu->add(_SCROLLBROWSER_MENU_LINE.c_str(), 0, ScrollBrowser::Callback, this);
    _menu->add(_SCROLLBROWSER_MENU_ALL.c_str(), 0, ScrollBrowser::Callback, this);
    _menu->type(Fl_Menu_Button::POPUP3);
    tooltip(_SCROLLBROWSER_TOOLTIP.c_str());
    update_pref();
}
void flw::ScrollBrowser::Callback(Fl_Widget*, void* o) {
    auto self  = static_cast<ScrollBrowser*>(o);
    auto txt   = self->_menu->text();
    auto label = std::string((txt != nullptr) ? txt : "");
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
    resizable(nullptr);
    clear();
}
void flw::SplitGroup::add(Fl_Widget* widget, SplitGroup::CHILD child) {
    auto num = child == SplitGroup::CHILD::FIRST ? 0 : 1;
    if (_widgets[num] != nullptr) {
        remove(_widgets[num]);
        delete _widgets[num];
    }
    _widgets[num] = widget;
    if (widget != nullptr) {
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
                do_layout();
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
    if (children() == 0 || W == 0 || H == 0 || visible() == 0) {
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
#include <FL/Fl_Return_Button.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Int_Input.H>
#include <FL/fl_ask.H>
#include <FL/fl_draw.H>
namespace flw {
class _TableDisplayScrollbar : public Fl_Scrollbar {
public:
    _TableDisplayScrollbar(int X, int Y, int W, int H) : Fl_Scrollbar(X, Y, W, H) {
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
    GridGroup*                      _grid;
    bool                            _ret;
public:
    _TableDisplayCellDialog(int row, int col) :
    Fl_Double_Window(0, 0, 10, 10, "Goto Cell") {
        end();
        _col  = new Fl_Int_Input(0, 0, 0, 0, "Column:");
        _grid = new GridGroup(0, 0, w(), h());
        _row  = new Fl_Int_Input(0, 0, 0, 0, "Row:");
        _ret  = false;
        _grid->add(_row,   1,  3,  -1,   4);
        _grid->add(_col,   1, 10,  -1,   4);
        add(_grid);
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
        resizable(_grid);
        resize(0, 0, flw::PREF_FONTSIZE * 16, flw::PREF_FONTSIZE * 8);
    }
    static void Callback(Fl_Widget* w, void* o) {
        auto dlg = static_cast<_TableDisplayCellDialog*>(o);
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
    Fl_Input*                   _find;
    Fl_Button*                  _next;
    Fl_Button*                  _prev;
    Fl_Button*                  _close;
    GridGroup*                  _grid;
    TableDisplay*               _table;
    bool                        _repeat;
public:
    explicit _TableDisplayFindDialog(TableDisplay* table) :
    Fl_Double_Window(0, 0, 10, 10, "Find Text In Table Cells") {
        end();
        _close  = new Fl_Button(0, 0, 0, 0, "&Close");
        _grid   = new GridGroup(0, 0, w(), h());
        _next   = new Fl_Return_Button(0, 0, 0, 0, "&Next");
        _prev   = new Fl_Button(0, 0, 0, 0, "&Previous");
        _find   = new Fl_Input(0, 0, 0, 0, "Find:");
        _table  = table;
        _repeat = true;
        _grid->add(_find,     8,  1,  -1,   4);
        _grid->add(_prev,   -51, -5,  16,   4);
        _grid->add(_next,   -34, -5,  16,   4);
        _grid->add(_close,  -17, -5,  16,   4);
        add(_grid);
        _close->callback(_TableDisplayFindDialog::Callback, this);
        _close->labelsize(flw::PREF_FONTSIZE);
        _next->callback(_TableDisplayFindDialog::Callback, this);
        _next->labelsize(flw::PREF_FONTSIZE);
        _prev->callback(_TableDisplayFindDialog::Callback, this);
        _prev->labelsize(flw::PREF_FONTSIZE);
        _find->align(FL_ALIGN_LEFT);
        _find->callback(_TableDisplayFindDialog::Callback, this);
        _find->labelsize(flw::PREF_FONTSIZE);
        _find->textfont(flw::PREF_FIXED_FONT);
        _find->textsize(flw::PREF_FONTSIZE);
        _find->value(_table->_find.c_str());
        _find->when(FL_WHEN_ENTER_KEY_ALWAYS);
        callback(_TableDisplayFindDialog::Callback, this);
        set_modal();
        resizable(_grid);
        resize(0, 0, flw::PREF_FONTSIZE * 35, flw::PREF_FONTSIZE * 6);
    }
    static void Callback(Fl_Widget* w, void* o) {
        auto dlg = static_cast<_TableDisplayFindDialog*>(o);
        if (w == dlg) {
            dlg->hide();
        }
        else if (w == dlg->_close) {
            dlg->_table->_find = dlg->_find->value();
            dlg->hide();
        }
        else if (w == dlg->_next) {
            dlg->find(true);
        }
        else if (w == dlg->_prev) {
            dlg->find(false);
        }
        else if (w == dlg->_find) {
            dlg->find(dlg->_repeat);
        }
    }
    void find(bool next) {
        auto find = _find->value();
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
    void run(Fl_Window* parent) {
        flw::util::center_window(this, parent);
        show();
        while (visible() != 0) {
            Fl::wait();
            Fl::flush();
        }
    }
};
TableDisplay::TableDisplay(int X, int Y, int W, int H, const char* l) : Fl_Group(X, Y, W, H, l) {
    end();
    _hor  = new _TableDisplayScrollbar(0, 0, 0, 0);
    _ver  = new _TableDisplayScrollbar(0, 0, 0, 0);
    _edit = nullptr;
    add(_ver);
    add(_hor);
    _hor->callback(&_CallbackVer, this);
    _hor->linesize(10);
    _hor->type(FL_HORIZONTAL);
    _ver->callback(&_CallbackHor, this);
    _ver->linesize(10);
    resizable(nullptr);
    box(FL_BORDER_BOX);
    color(FL_BACKGROUND_COLOR);
    selection_color(FL_SELECTION_COLOR);
    clip_children(1);
    util::labelfont(this);
    TableDisplay::clear();
}
void TableDisplay::active_cell(int row, int col, bool show) {
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
        _set_event(_curr_row, _curr_col, TableDisplay::EVENT::CURSOR);
        do_callback();
    }
    redraw();
    if (show &&
        _curr_row > 0 &&
        _curr_col > 0) {
        show_cell(_curr_row, _curr_col);
    }
}
void TableDisplay::_CallbackVer(Fl_Widget*, void* o) {
    auto table = static_cast<TableDisplay*>(o);
    table->_start_col = table->_hor->value();
    table->redraw();
}
 void TableDisplay::_CallbackHor(Fl_Widget*, void* o) {
    auto table = static_cast<TableDisplay*>(o);
    table->_start_row = table->_ver->value();
    table->redraw();
}
int TableDisplay::_cell_height(int Y) {
    auto y2 = y() + h() - _hor->h();
    if (Y + _height >= y2) {
        return y2 - Y;
    }
    else {
        return _height;
    }
}
int TableDisplay::_cell_width(int col, int X) {
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
void TableDisplay::clear() {
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
    _event           = TableDisplay::EVENT::UNDEFINED;
    _event_col       = -1;
    _event_row       = -1;
    _expand          = false;
    _height          = flw::PREF_FONTSIZE * 2;
    _resize          = false;
    _resize_col      = -1;
    _rows            = 0;
    _select          = TableDisplay::SELECT::NO;
    _show_col_header = false;
    _show_hor_lines  = false;
    _show_row_header = false;
    _show_ver_lines  = false;
    _start_col       = 1;
    _start_row       = 1;
    _find            = "";
    redraw();
}
void TableDisplay::draw() {
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
    if (_edit != nullptr) {
        fl_color(fl_lighter(FL_FOREGROUND_COLOR));
        fl_rect(_current_cell[0], _current_cell[1], _current_cell[2] + 1, _current_cell[3] + 1);
    }
    fl_pop_clip();
}
void TableDisplay::_draw_cell(int row, int col, int X, int Y, int W, int H, bool ver, bool hor, bool current) {
    fl_push_clip(X, Y, W + 1, H);
    auto align    = cell_align(row, col);
    auto textfont = cell_textfont(row, col);
    auto textsize = cell_textsize(row, col);
    auto val      = cell_value(row, col);
    auto space    = (align & FL_ALIGN_RIGHT) ? 6 : 4;
    if (row > 0 && col > 0) {
        auto color     = cell_color(row, col);
        auto textcolor = cell_textcolor(row, col);
        if (current == true) {
            color     = selection_color();
            textcolor = fl_contrast(FL_FOREGROUND_COLOR, color);
        }
        fl_color(color);
        fl_rectf(X + 1, Y, W + 1, H);
        fl_font(textfont, textsize);
        fl_color(textcolor);
        _draw_text(val, X + space, Y + 2, W - space * 2, H - 4, align);
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
        _draw_text(val, X + space, Y + 2, W - space * 2, H - 4, align);
    }
    fl_pop_clip();
}
void TableDisplay::_draw_row(int row, int W, int Y, int H) {
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
                _draw_cell(row, c, x1, Y, cw, H, _show_ver_lines, _show_hor_lines, _select != TableDisplay::SELECT::NO);
            }
        }
        else if (row > 0 && row == _curr_row && _select == TableDisplay::SELECT::ROW) {
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
void TableDisplay::_draw_text(const char* string, int X, int Y, int W, int H, Fl_Align align) {
    if (align == FL_ALIGN_CENTER || align == FL_ALIGN_RIGHT) {
        if (fl_width(string) > W) {
            align = FL_ALIGN_LEFT;
        }
    }
    fl_draw(string, X, Y, W, H, align);
}
int TableDisplay::_ev_keyboard_down() {
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
int TableDisplay::_ev_mouse_click () {
    if (Fl::event_button1() && _drag == true) {
        return 1;
    }
    auto r  = 0;
    auto c  = 0;
    auto cr = _curr_row;
    auto cc = _curr_col;
    _get_cell_below_mouse(r, c);
    if (_edit == nullptr) {
        Fl::focus(this);
    }
    if (r == 0 && c >= 1) {
        _set_event(r, c, (Fl::event_ctrl() != 0) ? TableDisplay::EVENT::COLUMN_CTRL : TableDisplay::EVENT::COLUMN);
        do_callback();
    }
    else if (c == 0 && r >= 1) {
        _set_event(r, c, (Fl::event_ctrl() != 0) ? TableDisplay::EVENT::ROW_CTRL : TableDisplay::EVENT::ROW);
        do_callback();
    }
    else if (r == -1 || c == -1) {
        if (r == -1 && _hor->visible() != 0 && Fl::event_y() >= _hor->y()) {
            ;
        }
        else if (c == -1 && _ver->visible() != 0 && Fl::event_x() >= _ver->x()) {
            ;
        }
        else {
            active_cell(-1, -1);
            return 0;
        }
    }
    else if (r >= 1 && c >= 1 && (r != cr || c != cc) && _select != TableDisplay::SELECT::NO) {
        active_cell(r, c);
    }
    return 2;
}
int TableDisplay::_ev_mouse_drag() {
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
int TableDisplay::_ev_mouse_move() {
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
void TableDisplay::_get_cell_below_mouse(int& row, int& col) {
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
int TableDisplay::handle(int event) {
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
void TableDisplay::header(bool row, bool col) {
    _show_row_header = row;
    _show_col_header = col;
    redraw();
}
void TableDisplay::lines(bool ver, bool hor) {
    _show_ver_lines = ver;
    _show_hor_lines = hor;
    redraw();
}
void TableDisplay::_move_cursor(_TABLEDISPLAY_MOVE move) {
    if (_edit == nullptr && _rows > 0 && _cols > 0 && _select != TableDisplay::SELECT::NO) {
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
void TableDisplay::show_cell(int row, int col) {
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
void TableDisplay::size(int rows, int cols) {
    if (rows > -1 && _cols > -1) {
        _rows      = rows;
        _cols      = cols;
        _curr_row  = 1;
        _curr_col  = 1;
        _start_row = 1;
        _start_col = 1;
        _set_event(_curr_row, _curr_col, TableDisplay::EVENT::SIZE);
        do_callback();
    }
    redraw();
}
void TableDisplay::_update_scrollbars() {
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
            auto r = (h() - Fl::scrollbar_size() - (_show_col_header ? _height : 0)) / _height;
            if (_rows > r) {
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
namespace flw {
int _tableeditor_to_doubles(std::string string, double numbers[], size_t size) {
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
int64_t _tableeditor_to_int(std::string string, int64_t def) {
    try { return std::stoll(string.c_str(), 0, 0); }
    catch (...) { return def; }
}
int _tableeditor_to_ints(std::string string, int64_t numbers[], size_t size) {
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
const char* TableEditor::SELECT_DATE = "Select Date";
const char* TableEditor::SELECT_DIR  = "Select Directory";
const char* TableEditor::SELECT_FILE = "Select File";
const char* TableEditor::SELECT_LIST = "Select String";
TableEditor::TableEditor(int X, int Y, int W, int H, const char* l) : TableDisplay(X, Y, W, H, l) {
    TableEditor::clear();
}
void TableEditor::clear() {
    TableDisplay::clear();
    _send_changed_event_always = false;
    _edit2 = nullptr;
    _edit3 = nullptr;
}
void TableEditor::_delete_current_cell() {
    if (_curr_row > 0 && _curr_col > 0) {
        auto edit = cell_edit(_curr_row, _curr_col);
        if (edit == true) {
            auto rend = cell_rend(_curr_row, _curr_col);
            auto set  = false;
            switch (rend) {
                case TableEditor::REND::TEXT:
                case TableEditor::REND::SECRET:
                case TableEditor::REND::INPUT_CHOICE:
                case TableEditor::REND::DLG_FILE:
                case TableEditor::REND::DLG_DIR:
                case TableEditor::REND::DLG_LIST:
                    set = cell_value(_curr_row, _curr_col, "");
                    break;
                case TableEditor::REND::INTEGER:
                    set = cell_value(_curr_row, _curr_col, "0");
                    break;
                case TableEditor::REND::NUMBER:
                    set = cell_value(_curr_row, _curr_col, "0.0");
                    break;
                case TableEditor::REND::BOOLEAN:
                    set = cell_value(_curr_row, _curr_col, "0");
                    break;
                case TableEditor::REND::DLG_DATE:
                    set = cell_value(_curr_row, _curr_col, "1970-01-01");
                    break;
                case TableEditor::REND::DLG_COLOR:
                    set = cell_value(_curr_row, _curr_col, "56");
                    break;
                default:
                    break;
            }
            if (set == true) {
                _set_event(_curr_row, _curr_col, TableEditor::EVENT::CHANGED);
                do_callback();
                redraw();
            }
        }
    }
}
void TableEditor::_draw_cell(int row, int col, int X, int Y, int W, int H, bool ver, bool hor, bool current) {
    fl_push_clip(X, Y, W + 1, H);
    auto align     = cell_align(row, col);
    auto textcolor = cell_textcolor(row, col);
    auto textfont  = cell_textfont(row, col);
    auto textsize  = cell_textsize(row, col);
    auto val       = static_cast<TableDisplay*>(this)->cell_value(row, col);
    assert(val);
    if (row > 0 && col > 0) {
        auto format = cell_format(row, col);
        auto rend   = cell_rend(row, col);
        auto color  = cell_color(row, col);
        char buffer[100];
        if (current == true) {
            color     = selection_color();
            textcolor = fl_contrast(FL_FOREGROUND_COLOR, color);
        }
        fl_rectf(X + 1, Y, W + 1, H, color);
        if (rend == TableEditor::REND::SECRET) {
            fl_font(textfont, textsize);
            fl_color(textcolor);
            if (format == TableEditor::FORMAT::SECRET_DOT) {
                fl_draw("••••••", X + 4, Y + 2, W - 8, H - 4, align, 0, 1);
            }
            else {
                fl_draw("******", X + 4, Y + 2, W - 8, H - 4, align, 0, 1);
            }
        }
        else if (rend == TableEditor::REND::SLIDER) {
            double nums[4];
            if (_tableeditor_to_doubles(val, nums, 4) == 4) {
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
        else if (rend == TableEditor::REND::DLG_COLOR) {
            fl_draw_box(FL_FLAT_BOX, X + 2, Y + 2, W - 3, H - 3, (Fl_Color) _tableeditor_to_int(val, 0));
        }
        else if (rend == TableEditor::REND::DLG_DATE) {
            auto        date = Date::FromString(val);
            std::string string;
            if (format == TableEditor::FORMAT::DATE_WORLD) {
                string = date.format(Date::FORMAT::WORLD);
            }
            else if (format == TableEditor::FORMAT::DATE_US) {
                string = date.format(Date::FORMAT::US);
            }
            else {
                string = date.format(Date::FORMAT::ISO_LONG);
            }
            fl_font(textfont, textsize);
            fl_color(textcolor);
            _draw_text(string.c_str(), X + 4, Y + 2, W - 8, H - 4, align);
        }
        else if (rend == TableEditor::REND::BOOLEAN) {
            auto bw  = textsize;
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
                Fl_Rect r(x_1, y_1, bw - 1, bw - 1);
                fl_draw_check(r, selection_color());
            }
        }
        else if (rend == TableEditor::REND::INTEGER) {
            auto num = _tableeditor_to_int(val, 0);
            fl_font(textfont, textsize);
            fl_color(textcolor);
            if (format == TableEditor::FORMAT::INT_SEP) {
                auto s = util::format_int(num);
                _draw_text(s.c_str(), X + 4, Y + 2, W - 8, H - 4, align);
            }
            else {
                snprintf(buffer, 100, "%lld", (long long int) num);
                _draw_text(buffer, X + 4, Y + 2, W - 8, H - 4, align);
            }
        }
        else if (rend == TableEditor::REND::NUMBER || rend == TableEditor::REND::VALUE_SLIDER) {
            auto num = util::to_double(val, 0.0);
            if (rend == TableEditor::REND::VALUE_SLIDER) {
                double nums[1];
                if (_tableeditor_to_doubles(val, nums, 1) == 1) {
                    num = nums[0];
                }
            }
            if (format == TableEditor::FORMAT::DEC_0) {
                snprintf(buffer, 100, "%.0f", num);
            }
            else if (format == TableEditor::FORMAT::DEC_1) {
                snprintf(buffer, 100, "%.1f", num);
            }
            else if (format == TableEditor::FORMAT::DEC_2) {
                snprintf(buffer, 100, "%.2f", num);
            }
            else if (format == TableEditor::FORMAT::DEC_3) {
                snprintf(buffer, 100, "%.3f", num);
            }
            else if (format == TableEditor::FORMAT::DEC_4) {
                snprintf(buffer, 100, "%.4f", num);
            }
            else if (format == TableEditor::FORMAT::DEC_5) {
                snprintf(buffer, 100, "%.5f", num);
            }
            else if (format == TableEditor::FORMAT::DEC_6) {
                snprintf(buffer, 100, "%.6f", num);
            }
            else if (format == TableEditor::FORMAT::DEC_7) {
                snprintf(buffer, 100, "%.7f", num);
            }
            else if (format == TableEditor::FORMAT::DEC_8) {
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
void TableEditor::_edit_create() {
    auto rend      = cell_rend(_curr_row, _curr_col);
    auto align     = cell_align(_curr_row, _curr_col);
    auto color     = cell_color(_curr_row, _curr_col);
    auto textcolor = FL_FOREGROUND_COLOR;
    auto textfont  = cell_textfont(_curr_row, _curr_col);
    auto textsize  = cell_textsize(_curr_row, _curr_col);
    auto val       = static_cast<TableDisplay*>(this)->cell_value(_curr_row, _curr_col);
    assert(val);
    if (rend == TableEditor::REND::TEXT ||
        rend == TableEditor::REND::INTEGER ||
        rend == TableEditor::REND::NUMBER ||
        rend == TableEditor::REND::SECRET) {
        auto w = (Fl_Input*) nullptr;
        if (rend == TableEditor::REND::TEXT) {
            w = new Fl_Input(0, 0, 0, 0);
        }
        else if (rend == TableEditor::REND::INTEGER) {
            w = new Fl_Int_Input(0, 0, 0, 0);
        }
        else if (rend == TableEditor::REND::NUMBER) {
            w = new Fl_Float_Input(0, 0, 0, 0);
        }
        else if (rend == TableEditor::REND::SECRET) {
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
    else if (rend == TableEditor::REND::BOOLEAN) {
        auto w = new Fl_Check_Button(0, 0, 0, 0);
        w->box(FL_BORDER_BOX);
        w->color(color);
        w->labelcolor(textcolor);
        w->labelsize(textsize);
        w->value(*val && val[0] == '1' ? 1 : 0);
        _edit = w;
    }
    else if (rend == TableEditor::REND::SLIDER) {
        auto w = (Fl_Slider*) nullptr;
        double nums[4];
        if (_tableeditor_to_doubles(val, nums, 4) == 4) {
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
    else if (rend == TableEditor::REND::VALUE_SLIDER) {
        auto w = (Fl_Slider*) nullptr;
        double nums[4];
        if (_tableeditor_to_doubles(val, nums, 4) == 4) {
            w = new Fl_Value_Slider(0, 0, 0, 0);
            w->color(color);
            w->selection_color(textcolor);
            w->range(nums[1], nums[2]);
            w->step(nums[3]);
            w->value(nums[0]);
            w->type(FL_HOR_FILL_SLIDER);
            w->box(FL_BORDER_BOX);
            static_cast<Fl_Value_Slider*>(w)->textsize(textsize * 0.8);
            _edit = w;
        }
    }
    else if (rend == TableEditor::REND::CHOICE) {
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
                const auto& s = choices[f];
                w->add(s.c_str());
                if (s == val) {
                    select = f;
                }
            }
            w->value(select);
            _edit = w;
        }
    }
    else if (rend == TableEditor::REND::INPUT_CHOICE) {
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
            for (const auto& s : choices) {
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
void TableEditor::_edit_quick(const char* key) {
    auto rend = cell_rend(_curr_row, _curr_col);
    auto val  = static_cast<TableDisplay*>(this)->cell_value(_curr_row, _curr_col);
    char buffer[100];
    assert(val);
    if (rend == TableEditor::REND::INTEGER) {
        auto num = _tableeditor_to_int(val, 0);
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
            _set_event(_curr_row, _curr_col, TableEditor::EVENT::CHANGED);
            do_callback();
        }
    }
    else if (rend == TableEditor::REND::NUMBER) {
        auto num = util::to_double(val, 0.0);
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
            _set_event(_curr_row, _curr_col, TableEditor::EVENT::CHANGED);
            do_callback();
        }
    }
    else if (rend == TableEditor::REND::DLG_DATE) {
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
            _set_event(_curr_row, _curr_col, TableEditor::EVENT::CHANGED);
            do_callback();
        }
    }
    else if (rend == TableEditor::REND::SLIDER || rend == TableEditor::REND::VALUE_SLIDER) {
        double nums[4];
        if (_tableeditor_to_doubles(val, nums, 4) == 4) {
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
                _set_event(_curr_row, _curr_col, TableEditor::EVENT::CHANGED);
                do_callback();
            }
        }
    }
}
void TableEditor::_edit_show() {
    auto rend = cell_rend(_curr_row, _curr_col);
    auto val  = static_cast<TableDisplay*>(this)->cell_value(_curr_row, _curr_col);
    assert(val);
    if (rend == TableEditor::REND::DLG_COLOR) {
        auto color1 = (int) _tableeditor_to_int(val, 0);
        auto color2 = (int) fl_show_colormap((Fl_Color) color1);
        char buffer[100];
        snprintf(buffer, 20, "%d", color2);
        if ((_send_changed_event_always == true || color1 != color2) && cell_value(_curr_row, _curr_col, buffer) == true) {
            _set_event(_curr_row, _curr_col, TableEditor::EVENT::CHANGED);
            do_callback();
        }
    }
    else if (rend == TableEditor::REND::DLG_FILE) {
        auto result = fl_file_chooser(TableEditor::SELECT_FILE, "", val, 0);
        if ((_send_changed_event_always == true || (result != nullptr && strcmp(val, result) != 0)) && cell_value(_curr_row, _curr_col, result) == true) {
            _set_event(_curr_row, _curr_col, TableEditor::EVENT::CHANGED);
            do_callback();
        }
    }
    else if (rend == TableEditor::REND::DLG_DIR) {
        auto result = fl_dir_chooser(TableEditor::SELECT_DIR, val);
        if ((_send_changed_event_always == true || (result != nullptr && strcmp(val, result) != 0)) && cell_value(_curr_row, _curr_col, result) == true) {
            _set_event(_curr_row, _curr_col, TableEditor::EVENT::CHANGED);
            do_callback();
        }
    }
    else if (rend == TableEditor::REND::DLG_DATE) {
        auto date1  = Date::FromString(val);
        auto date2  = Date(date1);
        auto result = flw::dlg::date(TableEditor::SELECT_DATE, date1, top_window());
        auto string = date1.format(Date::FORMAT::ISO_LONG);
        if ((_send_changed_event_always == true || (result == true && date1 != date2)) && cell_value(_curr_row, _curr_col, string.c_str()) == true) {
            _set_event(_curr_row, _curr_col, TableEditor::EVENT::CHANGED);
            do_callback();
        }
    }
    else if (rend == TableEditor::REND::DLG_LIST) {
        auto choices = cell_choice(_curr_row, _curr_col);
        if (choices.size() > 0) {
            auto row = dlg::select(TableEditor::SELECT_LIST, choices, val);
            if (row > 0) {
                const auto& string = choices[row - 1];
                if ((_send_changed_event_always == true || string != val) && cell_value(_curr_row, _curr_col, string.c_str()) == true) {
                    _set_event(_curr_row, _curr_col, TableEditor::EVENT::CHANGED);
                    do_callback();
                }
            }
        }
    }
}
void TableEditor::_edit_start(const char* key) {
    if (_select != TableEditor::SELECT::NO && _edit == nullptr && _curr_row >= 1 && _curr_col >= 1 && cell_edit(_curr_row, _curr_col)) {
        Fl::event_clicks(0);
        Fl::event_is_click(0);
        TableEditor::REND rend = cell_rend(_curr_row, _curr_col);
        if (*key != 0) {
            _edit_quick(key);
        }
        else if (rend == TableEditor::REND::DLG_COLOR ||
                 rend == TableEditor::REND::DLG_FILE ||
                 rend == TableEditor::REND::DLG_DIR ||
                 rend == TableEditor::REND::DLG_DATE ||
                 rend == TableEditor::REND::DLG_LIST) {
            _edit_show();
        }
        else {
            _edit_create();
        }
        redraw();
    }
}
void TableEditor::_edit_stop(bool save) {
    if (_edit) {
        auto rend = cell_rend(_curr_row, _curr_col);
        auto val  = static_cast<TableDisplay*>(this)->cell_value(_curr_row, _curr_col);
        auto stop = true;
        if (save == true) {
            if (rend == TableEditor::REND::TEXT ||
                rend == TableEditor::REND::INTEGER ||
                rend == TableEditor::REND::NUMBER ||
                rend == TableEditor::REND::SECRET) {
                auto input = static_cast<Fl_Input*>(_edit);
                auto val2  = input->value();
                char buffer[100];
                if (rend == TableEditor::REND::INTEGER) {
                    snprintf(buffer, 100, "%d", (int) _tableeditor_to_int(val2, 0));
                    val2 = buffer;
                }
                else if (rend == TableEditor::REND::NUMBER) {
                    auto num = util::to_double(val2, 0.0);
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
            else if (rend == TableEditor::REND::BOOLEAN) {
                auto button = static_cast<Fl_Check_Button*>(_edit);
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
            else if (rend == TableEditor::REND::SLIDER || rend == TableEditor::REND::VALUE_SLIDER) {
                auto slider = static_cast<Fl_Slider*>(_edit);
                auto val2   = FormatSlider(slider->value(), slider->minimum(), slider->maximum(), slider->step());
                if (strcmp(val, val2) == 0) {
                    stop = true;
                    save = false;
                }
                else {
                    stop = cell_value(_curr_row, _curr_col, val2);
                }
            }
            else if (rend == TableEditor::REND::CHOICE) {
                auto choice = static_cast<Fl_Choice*>(_edit);
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
            else if (rend == TableEditor::REND::INPUT_CHOICE) {
                auto input_choice = static_cast<Fl_Input_Choice*>(_edit);
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
                _set_event(_curr_row, _curr_col, TableEditor::EVENT::CHANGED);
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
int TableEditor::_ev_keyboard_down2() {
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
            auto val = static_cast<TableDisplay*>(this)->cell_value(_curr_row, _curr_col);
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
int TableEditor::_ev_mouse_click2() {
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
int TableEditor::_ev_paste() {
    auto text = Fl::event_text();
    if (_curr_row > 0 && _curr_col > 0 && text && *text) {
        auto        rend = cell_rend(_curr_row, _curr_col);
        auto        val  = static_cast<TableDisplay*>(this)->cell_value(_curr_row, _curr_col);
        char        buffer[100];
        std::string string;
        switch(rend) {
            case TableEditor::REND::CHOICE:
            case TableEditor::REND::DLG_LIST:
            case TableEditor::REND::SLIDER:
            case TableEditor::REND::VALUE_SLIDER:
                return 1;
            case TableEditor::REND::DLG_DIR:
            case TableEditor::REND::DLG_FILE:
            case TableEditor::REND::INPUT_CHOICE:
            case TableEditor::REND::SECRET:
            case TableEditor::REND::TEXT:
                break;
            case TableEditor::REND::BOOLEAN:
                if (strcmp("1", text) == 0 || strcmp("true", text) == 0) {
                    text = "1";
                }
                else if (strcmp("0", text) == 0 || strcmp("false", text) == 0) {
                    text = "0";
                }
                else {
                    return 1;
                }
            case TableEditor::REND::DLG_COLOR:
            case TableEditor::REND::INTEGER:
                if (*text < '0' || *text > '9') {
                    return 1;
                }
                else {
                    auto num = _tableeditor_to_int(text, 0);
                    snprintf(buffer, 100, "%lld", (long long int) num);
                    text = buffer;
                    if (rend == TableEditor::REND::DLG_COLOR && (num < 0 || num > 255)) {
                        return 1;
                    }
                    else {
                        break;
                    }
                }
            case TableEditor::REND::NUMBER: {
                if (*text < '0' || *text > '9') {
                    return 1;
                }
                else {
                    auto num = util::to_double(text, 0.0);
                    snprintf(buffer, 100, "%f", num);
                    text = buffer;
                    break;
                }
            }
            case TableEditor::REND::DLG_DATE: {
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
            _set_event(_curr_row, _curr_col, TableEditor::EVENT::CHANGED);
            do_callback();
            redraw();
        }
    }
    return 1;
}
const char* TableEditor::FormatSlider(double val, double min, double max, double step) {
    static char result[200];
    snprintf(result, 200, "%.4f %.4f %.4f %.4f", val, min, max, step);
    return result;
}
int TableEditor::handle(int event) {
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
            ret = _ev_keyboard_down2();
        }
        else if (event == FL_PUSH) {
            ret = _ev_mouse_click2();
        }
    }
    if (ret == 1) {
        return ret;
    }
    else {
        return TableDisplay::handle(event);
    }
}
}
#include <assert.h>
#include <algorithm>
#include <FL/Fl_Toggle_Button.H>
#include <FL/fl_draw.H>
namespace flw {
class _TabsGroupButton : public Fl_Toggle_Button {
public:
    int                         tw;
    Fl_Widget*                  widget;
    explicit _TabsGroupButton(std::string label, Fl_Widget* WIDGET, void* o) : Fl_Toggle_Button(0, 0, 0, 0) {
        tw     = 0;
        widget = WIDGET;
        align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE | FL_ALIGN_CLIP);
        copy_label(label.c_str());
        tooltip("");
        when(FL_WHEN_CHANGED);
        callback(TabsGroup::Callback, o);
        selection_color(FL_SELECTION_COLOR);
        labelfont(flw::PREF_FONT);
        labelsize(flw::PREF_FONTSIZE);
    }
};
TabsGroup::TabsGroup(int X, int Y, int W, int H, const char* l) : Fl_Group(X, Y, W, H, l) {
    end();
    clip_children(1);
    resizable(nullptr);
    tooltip(TabsGroup::Help());
    _scroll = new Fl_Scroll(X, Y, W, H);
    _pack   = new Fl_Pack(X, Y, W, H);
    _active = -1;
    _n      = 0;
    _s      = 0;
    _w      = 0;
    _e      = 0;
    _pack->end();
    _scroll->box(FL_NO_BOX);
    _scroll->add(_pack);
    Fl_Group::add(_scroll);
    tabs(TABS::NORTH);
    update_pref();
}
void TabsGroup::add(std::string label, Fl_Widget* widget, const Fl_Widget* after) {
    assert(widget);
    auto button = new _TabsGroupButton(label, widget, this);
    auto idx    = (after != nullptr) ? find(after) : (int) _widgets.size();
    if (idx < 0 || idx >= (int) _widgets.size() - 1) {
        Fl_Group::add(widget);
        _pack->add(button);
        _widgets.push_back(button);
    }
    else {
        idx++;
        auto b = static_cast<_TabsGroupButton*>(_widgets[idx]);
        Fl_Group::insert(*widget, b->widget);
        _pack->insert(*button, b);
        _widgets.insert(_widgets.begin() + idx, button);
    }
    TabsGroup::Callback(button, this);
    do_layout();
}
Fl_Widget* TabsGroup::_active_button() {
    return (_active >= 0 && _active < (int) _widgets.size()) ? _widgets[_active] : nullptr;
}
void TabsGroup::Callback(Fl_Widget* sender, void* object) {
    auto self   = static_cast<TabsGroup*>(object);
    auto count  = 0;
    self->_active = -1;
    for (auto widget : self->_widgets) {
        auto b = static_cast<_TabsGroupButton*>(widget);
        if (b == sender) {
            self->_active = count;
            b->value(1);
            b->widget->show();
            b->widget->take_focus();
        }
        else {
            b->value(0);
            b->widget->hide();
        }
        count++;
    }
    self->_resize_widgets();
}
Fl_Widget* TabsGroup::child(int num) const {
    return (num >= 0 && num < (int) _widgets.size()) ? static_cast<_TabsGroupButton*>(_widgets[num])->widget : nullptr;
}
void TabsGroup::clear() {
    _scroll->remove(_pack);
    _scroll->clear();
    _pack->clear();
    Fl_Group::remove(_scroll);
    Fl_Group::clear();
    Fl_Group::add(_scroll);
    _widgets.clear();
    _scroll->add(_pack);
    _active = -1;
    update_pref();
    Fl::redraw();
}
void TabsGroup::debug() const {
#ifdef DEBUG
    printf("TabsGroup ==>\n");
    printf("    _active  = %d\n", _active);
    printf("    _drag    = %d\n", _drag);
    printf("    _pos     = %d\n", _pos);
    printf("    _widgets = %d\n", (int) _widgets.size());
    printf("    tabs     = %s\n", _scroll->visible() ? "visible" : "hidden");
    printf("    children = %d\n", children());
    printf("    scroll   = %d\n", _scroll->children());
    printf("\n");
    auto count = 0;
    for (auto b : _widgets) {
        printf("    widget[%02d] = %s\n", count++, b->label());
    }
    printf("\n");
    flw::debug::print(this);
    printf("TabsGroup <==\n");
    fflush(stdout);
#endif
}
void TabsGroup::draw() {
    _scroll->redraw();
    Fl_Group::draw();
}
int TabsGroup::find(const Fl_Widget* widget) const {
    auto num = 0;
    for (const auto W : _widgets) {
        const auto b = static_cast<_TabsGroupButton*>(W);
        if (b->widget == widget) {
            return num;
        }
        else {
            num++;
        }
    }
    return -1;
}
int TabsGroup::handle(int event) {
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
                    do_layout();
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
    if (_widgets.size() > 1) {
        if (event == FL_KEYBOARD) {
            auto key   = Fl::event_key();
            auto alt   = Fl::event_alt() != 0;
            auto alt2  = alt;
            auto shift = Fl::event_shift() != 0;
#ifdef __APPLE__
            alt2 = Fl::event_command() != 0;
#endif
            if (alt2 == true && key >= '0' && key <= '9') {
                auto tab = key - '0';
                tab = (tab == 0) ? 9 : tab - 1;
                if (tab < (int) _widgets.size()) {
                    TabsGroup::Callback(_widgets[tab], this);
                }
                return 1;
            }
            else if (alt == true && shift == true && key == FL_Left) {
                swap(_active, _active - 1);
                TabsGroup::Callback(_active_button(), this);
                return 1;
            }
            else if (alt == true && shift == true && key == FL_Right) {
                swap(_active, _active + 1);
                TabsGroup::Callback(_active_button(), this);
                return 1;
            }
            else if (alt == true && key == FL_Left) {
                _active = _active == 0 ? (int) _widgets.size() - 1 : _active - 1;
                TabsGroup::Callback(_active_button(), this);
                return 1;
            }
            else if (alt == true && key == FL_Right) {
                _active = _active == (int) _widgets.size() - 1 ? 0 : _active + 1;
                TabsGroup::Callback(_active_button(), this);
                return 1;
            }
        }
    }
    if (event == FL_FOCUS) {
        auto widget = value();
        if (widget != nullptr && widget != Fl::focus()) {
            widget->take_focus();
            return 1;
        }
    }
    return Fl_Group::handle(event);
}
const char* TabsGroup::Help() {
    static const char* const HELP =
    "Use alt + left/right to move between tabs.\n"
    "Or alt (command key) + [1 - 9] to select tab.\n"
    "And alt + shift + left/right to move tabs.\n"
    "Tabs on the left/right side can have its width changed by dragging the mouse.";
    return HELP;
}
void TabsGroup::hide_tabs() {
    _scroll->hide();
    do_layout();
}
void TabsGroup::insert(std::string label, Fl_Widget* widget, const Fl_Widget* before) {
    auto button = new _TabsGroupButton(label, widget, this);
    auto idx    = (before != nullptr) ? find(before) : 0;
    if (idx >= (int) _widgets.size()) {
        Fl_Group::add(widget);
        _pack->add(button);
        _widgets.push_back(button);
    }
    else {
        auto b = static_cast<_TabsGroupButton*>(_widgets[idx]);
        Fl_Group::insert(*widget, b->widget);
        _pack->insert(*button, b);
        _widgets.insert(_widgets.begin() + idx, button);
    }
    TabsGroup::Callback(button, this);
    do_layout();
}
std::string TabsGroup::label(Fl_Widget* widget) {
    auto num = find(widget);
    if (num == -1) {
        return "";
    }
    return _widgets[num]->label();
}
void TabsGroup::label(std::string label, Fl_Widget* widget) {
    auto num = find(widget);
    if (num == -1) {
        return;
    }
    _widgets[num]->copy_label(label.c_str());
}
Fl_Widget* TabsGroup::remove(int num) {
    if (num < 0 || num >= (int) _widgets.size()) {
        return nullptr;
    }
    auto W = _widgets[num];
    auto b = static_cast<_TabsGroupButton*>(W);
    auto w = b->widget;
    _widgets.erase(_widgets.begin() + num);
    remove(w);
    _scroll->remove(b);
    delete b;
    if (num < _active) {
        _active--;
    }
    else if (_active == (int) _widgets.size()) {
        _active = (int) _widgets.size() - 1;
    }
    do_layout();
    TabsGroup::Callback(_active_button(), this);
    return w;
}
void TabsGroup::resize(int X, int Y, int W, int H) {
    Fl_Widget::resize(X, Y, W, H);
    if (W == 0 || H == 0) {
        return;
    }
    if (_scroll->visible() == 0) {
        _area = Fl_Rect(X + _w, Y + _n, W - _w - _e, H - _n - _s);
    }
    else {
        fl_font(flw::PREF_FONT, flw::PREF_FONTSIZE);
        if (_tabs == TABS::NORTH || _tabs == TABS::SOUTH) {
            _resize_north_south(X, Y, W, H);
        }
        else {
            _resize_east_west(X, Y, W, H);
        }
    }
    _resize_widgets();
}
void TabsGroup::_resize_east_west(int X, int Y, int W, int H) {
    auto height = flw::PREF_FONTSIZE + 8;
    auto pack_h = (height + _space) * (int) _widgets.size() - _space;
    auto scroll = 0;
    if (_pos < flw::PREF_FONTSIZE * _space) {
        _pos = flw::PREF_FONTSIZE * _space;
    }
    else if (_pos > W - flw::PREF_FONTSIZE * 3) {
        _pos = W - flw::PREF_FONTSIZE * 3;
    }
    if (pack_h > H) {
        scroll = (_scroll->scrollbar_size() == 0) ? Fl::scrollbar_size() : _scroll->scrollbar_size();
    }
    for (auto b : _widgets) {
        b->size(0, height);
    }
    if (_tabs == TABS::WEST) {
        _scroll->resize(X, Y, _pos, H);
        _pack->resize(X, Y, _pos - scroll, pack_h);
        _area = Fl_Rect(X + _pos + _w, Y + _n, W - _pos - _w - _e, H - _n - _s);
    }
    else {
        _scroll->resize(X + W - _pos, Y, _pos, H);
        _pack->resize(X + W - _pos, Y, _pos - scroll, pack_h);
        _area = Fl_Rect(X + _w, Y + _n, W - _pos - _w - _e, H - _n - _s);
    }
}
void TabsGroup::_resize_north_south(int X, int Y, int W, int H) {
    auto height = flw::PREF_FONTSIZE + 8;
    auto scroll = 0;
    auto pack_w = 0;
    for (auto widget : _widgets) {
        auto b  = static_cast<_TabsGroupButton*>(widget);
        auto th = 0;
        b->tw = 0;
        fl_measure(b->label(), b->tw, th);
        b->tw += flw::PREF_FONTSIZE;
        pack_w += b->tw + _space;
    }
    if (pack_w - _space > W) {
        scroll = (_scroll->scrollbar_size() == 0) ? Fl::scrollbar_size() : _scroll->scrollbar_size();
    }
    for (auto widget : _widgets) {
        auto b  = static_cast<_TabsGroupButton*>(widget);
        b->size(b->tw, 0);
    }
    if (_tabs == TABS::NORTH) {
        _scroll->resize(X, Y, W, height + scroll);
        _pack->resize(X, Y, pack_w, height);
        _area = Fl_Rect(X + _w, Y + height + scroll + _n, W - _w - _e, H - height - scroll - _n - _s);
    }
    else {
        _scroll->resize(X, Y + H - height - scroll, W, height + scroll);
        _pack->resize(X, Y + H - height - scroll, pack_w, height);
        _area = Fl_Rect(X + _w, Y + _n, W - _w - _e, H - height - scroll - _n - _s);
    }
}
void TabsGroup::_resize_widgets() {
    for (auto w : _widgets) {
        auto b = static_cast<_TabsGroupButton*>(w);
        if (b->widget->visible() != 0) {
            b->widget->resize(_area.x(), _area.y(), _area.w(), _area.h());
        }
    }
    Fl::redraw();
}
void TabsGroup::show_tabs() {
    _scroll->show();
    do_layout();
}
void TabsGroup::sort(bool ascending, bool casecompare) {
    auto pack = const_cast<Fl_Widget**>(_pack->array());
    auto butt = _active_button();
    if (ascending == true && casecompare == true) {
        std::sort(_widgets.begin(), _widgets.end(), [](const Fl_Widget* a, const Fl_Widget* b) { return strcmp(a->label(), b->label()) < 0; });
    }
    else if (ascending == true && casecompare == false) {
        std::sort(_widgets.begin(), _widgets.end(), [](const Fl_Widget* a, const Fl_Widget* b) { return fl_utf_strcasecmp(a->label(), b->label()) < 0; });
    }
    else if (ascending == false && casecompare == true) {
        std::sort(_widgets.begin(), _widgets.end(), [](const Fl_Widget* a, const Fl_Widget* b) { return strcmp(b->label(), a->label()) < 0; });
    }
    else if (ascending == false && casecompare == false) {
        std::sort(_widgets.begin(), _widgets.end(), [](const Fl_Widget* a, const Fl_Widget* b) { return fl_utf_strcasecmp(b->label(), a->label()) < 0; });
    }
    for (int f = 0; f < _pack->children(); f++) {
        pack[f] = _widgets[f];
        if (_widgets[f] == butt) {
            _active = f;
        }
    }
    do_layout();
}
void TabsGroup::swap(int from, int to) {
    auto last = (int) _widgets.size() - 1;
    if (_widgets.size() < 2 || to < -1 || to > (int) _widgets.size()) {
        return;
    }
    auto active = (_active == from);
    auto pack   = const_cast<Fl_Widget**>(_pack->array());
    if (from == 0 && to == -1) {
        auto widget = _widgets[0];
        for (int f = 1; f <= last; f++) {
            _widgets[f - 1] = _widgets[f];
            pack[f - 1]     = pack[f];
        }
        from           = last;
        pack[from]     = widget;
        _widgets[from] = widget;
    }
    else if (from == last && to == (int) _widgets.size()) {
        auto widget = _widgets[last];
        for (int f = last - 1; f >= 0; f--) {
            _widgets[f + 1] = _widgets[f];
            pack[f + 1]     = pack[f];
        }
        from           = 0;
        pack[from]     = widget;
        _widgets[from] = widget;
    }
    else {
        auto widget = _widgets[from];
        pack[from]     = pack[to];
        pack[to]       = widget;
        _widgets[from] = _widgets[to];
        _widgets[to]   = widget;
        from           = to;
    }
    if (active == true) {
        _active = from;
    }
    do_layout();
}
void TabsGroup::tabs(TABS tabs, int space_max_20) {
    _tabs   = tabs;
    _space  = (space_max_20 >= 0 && space_max_20 <= 20) ? space_max_20 : TabsGroup::DEFAULT_SPACE;
    auto al = FL_ALIGN_LEFT | FL_ALIGN_INSIDE | FL_ALIGN_CLIP;
    if (_tabs == TABS::NORTH || _tabs == TABS::SOUTH) {
        al = FL_ALIGN_CENTER | FL_ALIGN_INSIDE | FL_ALIGN_CLIP;
        _scroll->type(Fl_Scroll::HORIZONTAL);
        _pack->type(Fl_Pack::HORIZONTAL);
    }
    else {
        _pack->type(Fl_Pack::VERTICAL);
        _scroll->type(Fl_Scroll::VERTICAL);
    }
    for (auto widget : _widgets) {
        widget->align(al);
    }
    _pack->spacing(_space);
    do_layout();
    auto w = value();
    if (w != nullptr) {
        w->take_focus();
    }
}
void TabsGroup::update_pref(unsigned characters, Fl_Font font, Fl_Fontsize fontsize) {
    _drag = false;
    _pos  = fontsize * characters;
    for (auto widget : _widgets) {
        widget->labelfont(font);
        widget->labelsize(fontsize);
    }
}
Fl_Widget* TabsGroup::value() const {
    return (_active >= 0 && _active < (int) _widgets.size()) ? static_cast<_TabsGroupButton*>(_widgets[_active])->widget : nullptr;
}
void TabsGroup::value(int num) {
    if (num >= 0 && num < (int) _widgets.size()) {
        TabsGroup::Callback(_widgets[num], this);
    }
}
}
namespace flw {
struct _ToolGroupChild {
    Fl_Widget*                  widget;
    short                       size;
    _ToolGroupChild(Fl_Widget* WIDGET, int SIZE) {
        set(WIDGET, SIZE);
    }
    void set(Fl_Widget* WIDGET, int SIZE) {
        widget = WIDGET;
        size   = SIZE;
    }
};
ToolGroup::ToolGroup(DIRECTION direction, int X, int Y, int W, int H, const char* l) : Fl_Group(X, Y, W, H, l) {
    end();
    clip_children(1);
    resizable(nullptr);
    _direction = direction;
    _expand    = false;
}
ToolGroup::~ToolGroup() {
    for (auto v : _widgets) {
        delete static_cast<_ToolGroupChild*>(v);
    }
}
void ToolGroup::add(Fl_Widget* widget, int SIZE) {
    _widgets.push_back(new _ToolGroupChild(widget, SIZE));
    Fl_Group::add(widget);
}
void ToolGroup::clear() {
    _widgets.clear();
    Fl_Group::clear();
}
Fl_Widget* ToolGroup::remove(Fl_Widget* widget) {
    for (auto it = _widgets.begin(); it != _widgets.end(); it++) {
        auto child = static_cast<_ToolGroupChild*>(*it);
        if (child->widget == widget) {
            Fl_Group::remove(widget);
            _widgets.erase(it);
            delete child;
            return widget;
        }
    }
    #ifdef DEBUG
        fprintf(stderr, "error: ToolGroup::remove can't find widget\n");
    #endif
    return nullptr;
}
void ToolGroup::resize(const int X, const int Y, const int W, const int H) {
    Fl_Widget::resize(X, Y, W, H);
    if (children() == 0 || W == 0 || H == 0 || visible() == 0) {
        return;
    }
    auto leftover = (_direction == DIRECTION::HORIZONTAL) ? W : H;
    auto count    = 0;
    auto last     = static_cast<Fl_Widget*>(nullptr);
    auto avg      = 0;
    auto xpos     = X;
    auto ypos     = Y;
    for (auto v : _widgets) {
        auto child = static_cast<_ToolGroupChild*>(v);
        last = child->widget;
        if (child->size > 0) {
            leftover -= (child->size * flw::PREF_FONTSIZE);
        }
        else {
            count++;
        }
    }
    if (count > 0) {
        avg = leftover / count;
    }
    for (auto v : _widgets) {
        auto child = static_cast<_ToolGroupChild*>(v);
        if (child->widget != nullptr) {
            if (_direction == DIRECTION::HORIZONTAL) {
                if (_expand == true && child->widget == last) {
                    child->widget->resize(xpos, Y, X + W - xpos, H);
                }
                else if (child->size > 0) {
                    child->widget->resize(xpos, Y, child->size * flw::PREF_FONTSIZE, H);
                    xpos += flw::PREF_FONTSIZE * child->size;
                }
                else {
                    child->widget->resize(xpos, Y, avg, H);
                    xpos += avg;
                }
            }
            else {
                if (_expand == true && child->widget == last) {
                    child->widget->resize(X, ypos, W, Y + H - ypos);
                }
                else if (child->size > 0) {
                    child->widget->resize(X, ypos, W, child->size * flw::PREF_FONTSIZE);
                    ypos += flw::PREF_FONTSIZE * child->size;
                }
                else {
                    child->widget->resize(X, ypos, W, avg);
                    ypos += avg;
                }
            }
        }
    }
}
void ToolGroup::resize(Fl_Widget* widget, int SIZE) {
    for (auto v : _widgets) {
        auto child = static_cast<_ToolGroupChild*>(v);
        if (child->widget == widget) {
            child->set(widget, SIZE);
            return;
        }
    }
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

