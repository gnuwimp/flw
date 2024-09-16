// Copyright gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#include "chart.h"
#include "flw.h"
#include "json.h"
#include "dlg.h"
#include "waitcursor.h"

// MKALGAM_ON

#include <FL/Fl_File_Chooser.H>
#include <FL/Fl_Hor_Slider.H>
#include <FL/Fl_Menu_Button.H>
#include <FL/Fl_Scrollbar.H>
#include <FL/fl_ask.H>
#include <FL/fl_draw.H>
#include <FL/fl_show_colormap.H>
#include <algorithm>

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

#define _FLW_CHART_ERROR(X)     { fl_alert("error: illegal chart value at pos %u", (X)->pos()); clear(); return false; }
#define _FLW_CHART_CB(X)        [](Fl_Widget*, void* o) { static_cast<Chart*>(o)->X; }, this

static const char* const        _CHART_ADD_CSV          = "Add line from CSV file...";
static const char* const        _CHART_ADD_LINE         = "Create line...";
static const char* const        _CHART_DEBUG_CHART      = "Debug chart";
static const char* const        _CHART_DEBUG_LINE       = "Debug line";
static const char* const        _CHART_LOAD_JSON        = "Load from JSON...";
static const char* const        _CHART_PRINT            = "Print to PostScript file...";
static const char* const        _CHART_SAVE_CSV         = "Save to CSV...";
static const char* const        _CHART_SAVE_JSON        = "Save to JSON...";
static const char* const        _CHART_SAVE_PNG         = "Save to png file...";
static const char* const        _CHART_SETUP_AREA       = "Number of areas...";
static const char* const        _CHART_SETUP_MAX_CLAMP  = "Set max clamp...";
static const char* const        _CHART_SETUP_MIN_CLAMP  = "Set min clamp...";
static const char* const        _CHART_SETUP_DELETE     = "Delete lines...";
static const char* const        _CHART_SETUP_LABEL      = "Label...";
static const char* const        _CHART_SETUP_LINE       = "Properties...";
static const char* const        _CHART_SETUP_MOVE       = "Move lines...";
static const char* const        _CHART_SETUP_RANGE      = "Date range...";
static const char* const        _CHART_SETUP_SHOW       = "Show or hide lines...";
static const char* const        _CHART_SETUP_WIDTH      = "Y label width...";
static const char* const        _CHART_SHOW_HLINES      = "Show horizontal lines";
static const char* const        _CHART_SHOW_LABELS      = "Show line labels";
static const char* const        _CHART_SHOW_VLINES      = "Show vertical lines";

//------------------------------------------------------------------------------
static int _chart_count_decimals(double number) {
    number = fabs(number);

    int    res     = 0;
    char*  end     = 0;
    double inumber = static_cast<int64_t>(number);
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

    size_t len = strlen(buffer);
    end = buffer + len - 1;

    while (*end == '0') {
        *end = 0;
        end--;
    }

    res = strlen(buffer) - 2;
    return res;
}

//------------------------------------------------------------------------------
static std::string _chart_format_double(double num, int decimals, char del) {
    char res[100];

    *res = 0;

    if (decimals < 0) {
        decimals = _chart_count_decimals(num);
    }

    if (decimals == 0) {
        return util::format_int(static_cast<int64_t>(num), del);
    }

    if (fabs(num) < 9223372036854775807.0) {
        char fr_str[100];
        auto int_num    = static_cast<int64_t>(fabs(num));
        auto double_num = static_cast<double>(fabs(num) - int_num);
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
bool ChartArea::add_line(const ChartLine& chart_line) {
    if (_lines.size() >= ChartArea::MAX_LINES) {
        return false;
    }

    _lines.push_back(chart_line);
    Fl::redraw();
    return true;
}

//------------------------------------------------------------------------------
std::optional<double> ChartArea::clamp_max() const {
    if (std::isfinite(_clamp_max) == true) {
        return _clamp_max;
    }

    return std::nullopt;
}

//------------------------------------------------------------------------------
std::optional<double> ChartArea::clamp_min() const {
    if (std::isfinite(_clamp_min) == true) {
        return _clamp_min;
    }

    return std::nullopt;
}

//------------------------------------------------------------------------------
void ChartArea::clear() {
    _clamp_max = INFINITY;
    _clamp_min = INFINITY;
    _h         = 0;
    _percent   = 0;
    _selected  = 0;
    _w         = 0;
    _x         = 0;
    _y         = 0;

    _left.clear();
    _right.clear();
    _lines.clear();
}

//------------------------------------------------------------------------------
void ChartArea::debug() const {
#ifdef DEBUG
    auto c = 0;

    for (const auto& l : _lines) {
        if (l.size() > 0) {
            c++;
        }
    }

    if (c > 0 || _num == NUM::ONE) {
        fprintf(stderr, "\t----------------------\n");
        fprintf(stderr, "\tChartArea: %d\n", static_cast<int>(_num));
        fprintf(stderr, "\t\tlines:      %4d\n", c);
        fprintf(stderr, "\t\tx:          %4d\n", _x);
        fprintf(stderr, "\t\ty:          %4d\n", _y);
        fprintf(stderr, "\t\tw:          %4d\n", _w);
        fprintf(stderr, "\t\th:          %4d\n", _h);
        fprintf(stderr, "\t\tclamp_max:  %4.4f\n", _clamp_max);
        fprintf(stderr, "\t\tclamp_min:  %4.4f\n", _clamp_min);
        fprintf(stderr, "\t\tpercent:    %4d\n", _percent);
        fprintf(stderr, "\t\tselected:   %4d\n", static_cast<int>(_selected));

        _left.debug("left");
        _right.debug("right");
        c = 0;

        for (const auto& l : _lines) {
            if (l.size() > 0) {
                l.debug(c++);
            }
        }
    }
#endif
}

//------------------------------------------------------------------------------
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

//------------------------------------------------------------------------------
StringVector ChartArea::label_array(ChartArea::LABELTYPE labeltype) const {
    auto res = StringVector();

    for (const auto& l : _lines) {
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

//------------------------------------------------------------------------------
ChartLine* ChartArea::selected_line() {
    if (_selected >= _lines.size()) {
        return nullptr;
    }

    return &_lines[_selected];
}

//------------------------------------------------------------------------------
void ChartArea::set_visible(size_t line, bool val) {
    if (line >= _lines.size()) {
        return;
    }

    _lines[line].set_visible(val);
}


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
ChartData::ChartData() {
    high = low = close = 0.0;
}

//------------------------------------------------------------------------------
ChartData::ChartData(std::string DATE, double value) {
    Date valid_date = Date::FromString(DATE.c_str());

    if (std::isfinite(value) == true && valid_date.is_invalid() == false) {
        date = valid_date.format(Date::FORMAT::ISO_TIME);
        high = low = close = value;
    }
    else {
        high = low = close = 0.0;
    }
}

//------------------------------------------------------------------------------
ChartData::ChartData(std::string DATE, double HIGH, double LOW, double CLOSE) {
    high = low = close = 0.0;
    Date valid_date = Date::FromString(DATE.c_str());

    if (std::isfinite(HIGH) == true &&
        std::isfinite(LOW) == true &&
        std::isfinite(CLOSE) == true &&
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
}

//------------------------------------------------------------------------------
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

//------------------------------------------------------------------------------
size_t ChartData::BinarySearch(const ChartDataVector& in, const ChartData& key) {
    auto it = std::lower_bound(in.begin(), in.end(), key);

    if (it == in.end() || *it != key) {
        return (size_t) -1;
    }
    else {
        return std::distance(in.begin(), it);
    }
}

//------------------------------------------------------------------------------
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

//------------------------------------------------------------------------------
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

//------------------------------------------------------------------------------
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

//------------------------------------------------------------------------------
void ChartData::debug() const {
#ifdef DEBUG
    printf("%s, %15f, %15f, %15f\n", date.c_str(), high, low, close);
#endif
}

//------------------------------------------------------------------------------
void ChartData::Debug(const ChartDataVector& in) {
#ifdef DEBUG
    printf("\nChartDataVector(%u)\n", (unsigned) in.size());

    for (const auto& data : in) {
        data.debug();
    }

    fflush(stdout);
#else
    (void) in;
#endif
}

//------------------------------------------------------------------------------
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

//------------------------------------------------------------------------------
ChartDataVector ChartData::Fixed(const ChartDataVector& in, double value) {
    ChartDataVector res;

    for (const auto& data : in) {
        res.push_back(ChartData(data.date, value));
    }

    return res;
}

//------------------------------------------------------------------------------
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

//------------------------------------------------------------------------------
ChartDataVector ChartData::Modify(const ChartDataVector& in, ChartData::MODIFY modify, double value) {
    ChartDataVector res;

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
            default: // MODIFY::ADDITION
                res.push_back(ChartData(data.date, data.high + value, data.low + value, data.close + value));
                break;
        }
    }

    return res;
}

//------------------------------------------------------------------------------
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

//------------------------------------------------------------------------------
ChartDataVector ChartData::MovingAverage(const ChartDataVector& in, size_t days) {
    ChartDataVector res;

    if (days > 1 && days < in.size()) {
        size_t count = 0;
        double sum   = 0.0;
        auto   tmp   = new double[in.size() + 1];

        for (const auto& data : in) {
            count++;

            if (count < days) { //  Add data until the first moving average price can be calculated.
                tmp[count - 1] = data.close;
                sum += data.close;
            }
            else if (count == days) { //  This is the first point.
                tmp[count - 1] = data.close;
                sum += data.close;
                res.push_back(ChartData(data.date, sum / days));
            }
            else { //  Remove oldest data in range and add current to sum.
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

//------------------------------------------------------------------------------
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

//------------------------------------------------------------------------------
bool ChartData::SaveCSV(const ChartDataVector& in, std::string filename, std::string sep) {
    std::string csv;

    csv.reserve(in.size() * 40 + 256);

    for (const auto& data : in) {
        char buffer[256];
        snprintf(buffer, 256, "%s%s%f%s%f%s%f\n", data.date.c_str(), sep.c_str(), data.high, sep.c_str(), data.low, sep.c_str(), data.close);
        csv += buffer;
    }

    return File::Save(filename, csv.c_str(), csv.size());
}

//------------------------------------------------------------------------------
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

//------------------------------------------------------------------------------
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

                for (size_t j = (f + 1) - days; j < (f + 1) - 1; j++) { //  Get max/min in for current range.
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

                if (diff2 > 0.0001) {
                    double kval  = 100 * (diff1 / diff2);
                    res.push_back(ChartData(data.date, kval));
                }
            }
        }
    }

    return res;
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
ChartLine::ChartLine(const ChartDataVector& data, std::string label, ChartLine::TYPE type) {
    clear();
    set_data(data);
    set_label(label);
    set_type(type);
}

//------------------------------------------------------------------------------
void ChartLine::clear() {
    _data.clear();

    _align     = FL_ALIGN_LEFT;
    _color     = FL_FOREGROUND_COLOR;
    _label     = "";
    _max       = INFINITY;
    _min       = INFINITY;
    _type      = TYPE::LINE;
    _visible   = true;
    _width     = 1;
}

//------------------------------------------------------------------------------
void ChartLine::debug(int num, bool prices) const {
#ifdef DEBUG
    fprintf(stderr, "\t\t---------------------------------------------\n");
    fprintf(stderr, "\t\tChartLine: %d (%p)\n", num, this);
    fprintf(stderr, "\t\t\talign:      %25s\n", (_align == FL_ALIGN_LEFT) ? "LEFT" : "RIGHT");
    fprintf(stderr, "\t\t\ttype:  %31s\n", type_to_string().c_str());
    fprintf(stderr, "\t\t\tlabel: %30s\n", _label.c_str());
    fprintf(stderr, "\t\t\tlabel_rect:    %04d, %04d, %04d, %04d\n", _rect.x(), _rect.y(), _rect.w(), _rect.h());
    fprintf(stderr, "\t\t\tprices:     %25d\n", (int) size());
    fprintf(stderr, "\t\t\tvisible:    %25d\n", _visible);
    fprintf(stderr, "\t\t\twidth:      %25u\n", _width);

    if (size() > 1) {
        fprintf(stderr, "\t\t\tfirst:      %25s\n", _data.front().date.c_str());
        fprintf(stderr, "\t\t\tfirst:      %25f\n", _data.front().close);
        fprintf(stderr, "\t\t\tlast:       %25s\n", _data.back().date.c_str());
        fprintf(stderr, "\t\t\tlast:       %25f\n", _data.back().close);
    }

    if (prices == true) {
        ChartData::Debug(_data);
    }
#else
    (void) num;
    (void) prices;
#endif
}

//----------------------------------------------------------------------
ChartLine& ChartLine::set_data(const ChartDataVector& in) {
    _max  = INFINITY;
    _min  = INFINITY;
    _data = in;

    for (const auto& data : _data) {
        if (std::isfinite(_min) == false || data.low < _min)  {
            _min = data.low;
        }

        if (std::isfinite(_max) == false || data.high > _max) {
            _max = data.high;
        }
    }

    return *this;
}

//------------------------------------------------------------------------------
ChartLine&  ChartLine::set_type_from_string(std::string val) {
    if (val == "LINE_DOT")               _type = TYPE::LINE_DOT;
    else if (val == "BAR")               _type = TYPE::BAR;
    else if (val == "BAR_CLAMP")         _type = TYPE::BAR_CLAMP;
    else if (val == "BAR_HLC")           _type = TYPE::BAR_HLC;
    else if (val == "HORIZONTAL")        _type = TYPE::HORIZONTAL;
    else if (val == "EXPAND_VERTICAL")   _type = TYPE::EXPAND_VERTICAL;
    else if (val == "EXPAND_HORIZONTAL") _type = TYPE::EXPAND_HORIZONTAL;
    else                                 _type = TYPE::LINE;

    return *this;
}

//----------------------------------------------------------------------
std::string ChartLine::type_to_string() const {
    static const char* NAMES[] = { "LINE", "LINE_DOT", "BAR", "BAR_CLAMP", "BAR_HLC", "HORIZONTAL", "EXPAND_VERTICAL", "EXPAND_HORIZONTAL", "", "", };
    return NAMES[static_cast<unsigned>(_type)];
}

/***
 *       _____ _                _   _      _             _____      _
 *      / ____| |              | | | |    (_)           / ____|    | |
 *     | |    | |__   __ _ _ __| |_| |     _ _ __   ___| (___   ___| |_ _   _ _ __
 *     | |    | '_ \ / _` | '__| __| |    | | '_ \ / _ \\___ \ / _ \ __| | | | '_ \
 *     | |____| | | | (_| | |  | |_| |____| | | | |  __/____) |  __/ |_| |_| | |_) |
 *      \_____|_| |_|\__,_|_|   \__|______|_|_| |_|\___|_____/ \___|\__|\__,_| .__/
 *                                                                           | |
 *                                                                           |_|
 */

//------------------------------------------------------------------------------
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
    //--------------------------------------------------------------------------
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
        _align->textfont(flw::PREF_FIXED_FONT);
        _align->textsize(flw::PREF_FONTSIZE);
        _cancel->callback(ChartLineSetup::Callback, this);
        _close->callback(ChartLineSetup::Callback, this);
        _color->align(FL_ALIGN_LEFT);
        _color->callback(ChartLineSetup::Callback, this);
        _label->textfont(flw::PREF_FIXED_FONT);
        _label->textsize(flw::PREF_FONTSIZE);
        _type->add("Line");
        _type->add("Dotted Line");
        _type->add("Bar");
        _type->add("Bar clamp");
        _type->add("Bar hlc");
        _type->add("Horizontal");
        _type->add("Expand Horizontal");
        _type->textfont(flw::PREF_FIXED_FONT);
        _type->textsize(flw::PREF_FONTSIZE);
        _width->align(FL_ALIGN_LEFT);
        _width->callback(ChartLineSetup::Callback, this);
        _width->precision(0);
        _width->range(0, ChartLine::MAX_WIDTH);
        _width->value(_line.width());
        _width->tooltip("Max line width.\nIf tick width is smaller,\nthen tick width is prioritized.\n0 width is dynamic width.");

        resizable(_grid);
        util::labelfont(this);
        callback(ChartLineSetup::Callback, this);
        size(30 * flw::PREF_FONTSIZE, 16 * flw::PREF_FONTSIZE);
        size_range(30 * flw::PREF_FONTSIZE, 16 * flw::PREF_FONTSIZE);
        set_modal();
        util::center_window(this, parent);
        _grid->do_layout();
        ChartLineSetup::Callback(_width, this);
        set();
    }

    //--------------------------------------------------------------------------
    static void Callback(Fl_Widget* w, void* o) {
        auto self = static_cast<ChartLineSetup*>(o);

        if (w == self) {
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
        else if (w == self->_cancel) {
            self->hide();
        }
        else if (w == self->_close) {
            self->_ret = true;
            self->get();
            self->hide();
        }
    }

    //--------------------------------------------------------------------------
    void get() {
        _line.set_label(_label->value());
        _line.set_width(_width->value());
        _line.set_color(_color->color());

        if (_type->value() == 0)      _line.set_type(ChartLine::TYPE::LINE);
        else if (_type->value() == 1) _line.set_type(ChartLine::TYPE::LINE_DOT);
        else if (_type->value() == 2) _line.set_type(ChartLine::TYPE::BAR);
        else if (_type->value() == 3) _line.set_type(ChartLine::TYPE::BAR_CLAMP);
        else if (_type->value() == 4) _line.set_type(ChartLine::TYPE::BAR_HLC);
        else if (_type->value() == 5) _line.set_type(ChartLine::TYPE::HORIZONTAL);
        else if (_type->value() == 6) _line.set_type(ChartLine::TYPE::EXPAND_HORIZONTAL);

        if (_align->value() == 0) _line.set_align(FL_ALIGN_LEFT);
        else                      _line.set_align(FL_ALIGN_RIGHT);
    }

    //--------------------------------------------------------------------------
    bool run() {
        show();

        while (visible() != 0) {
            Fl::wait();
            Fl::flush();
        }

        return _ret;
    }

    //--------------------------------------------------------------------------
    void set() {
        _label->value(_line.label().c_str());
        _color->color(_line.color());
        ChartLineSetup::Callback(_width, this);

        if (_line.type() == ChartLine::TYPE::LINE)                   _type->value(0);
        else if (_line.type() == ChartLine::TYPE::LINE_DOT)          _type->value(1);
        else if (_line.type() == ChartLine::TYPE::BAR)               _type->value(2);
        else if (_line.type() == ChartLine::TYPE::BAR_CLAMP)         _type->value(3);
        else if (_line.type() == ChartLine::TYPE::BAR_HLC)           _type->value(4);
        else if (_line.type() == ChartLine::TYPE::HORIZONTAL)        _type->value(5);
        else if (_line.type() == ChartLine::TYPE::EXPAND_HORIZONTAL) _type->value(6);

        if (_line.align() == FL_ALIGN_LEFT) _align->value(0);
        else                                _align->value(1);
    }
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
ChartScale::ChartScale() {
    clear();
}

//------------------------------------------------------------------------------
void ChartScale::calc(int height) {
    auto range = diff();

    _tick  = 0.0;
    _pixel = 0.0;

    if (std::isfinite(_min) == true && std::isfinite(_max) == true && _min < _max && range > 0.000000001) {
        double test_inc = 0.0;
        double test_min = 0.0;
        double test_max = 0.0;
        int    ticker   = 0;

        test_inc = pow(10.0, ceil(log10(range / 10.0)));
        test_max = static_cast<int>(_max / test_inc) * test_inc;

        if (test_max < _max) {
            test_max += test_inc;
        }

        test_min = test_max;

        do {
            ++ticker;
            test_min -= test_inc;
        } while (test_min > _min);

        if (ticker < 10) {
            test_inc = (ticker < 5) ? test_inc / 10.0 : test_inc / 2.0;

            while ((test_min + test_inc) <= _min) {
                test_min += test_inc;
            }

            while ((test_max - test_inc) >= _max) {
                test_max -= test_inc;
            }

        }

        _min   = test_min;
        _max   = test_max;
        _tick  = test_inc;

        if (_tick > 0.000000001) {
            _pixel = height / diff();
        }
    }
}

//------------------------------------------------------------------------------
void ChartScale::clear() {
    _min   = INFINITY;
    _max   = INFINITY;
    _tick  = 0.0;
    _pixel = 0.0;
}

//------------------------------------------------------------------------------
void ChartScale::debug(const char* name) const {
#ifdef DEBUG
    fprintf(stderr, "\t\t---------------------------------------------\n");
    fprintf(stderr, "\t\tChartScale: %s\n", name);
    fprintf(stderr, "\t\t\tmin:   %30.6f\n", _min);
    fprintf(stderr, "\t\t\tmax:   %30.6f\n", _max);
    fprintf(stderr, "\t\t\tDiff:  %30.6f\n", diff());
    fprintf(stderr, "\t\t\ttick:  %30.6f\n", _tick);
    fprintf(stderr, "\t\t\tpixel: %30.6f\n", _pixel);
#else
    (void) name;
#endif
}

//------------------------------------------------------------------------------
void ChartScale::fix_height() {
    if (std::isfinite(_min) == true && std::isfinite(_max) == true && _min < _max && fabs(_max - _min) < 0.000001) {
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

//------------------------------------------------------------------------------
std::optional<double> ChartScale::max() const {
    if (std::isfinite(_max) == true) {
        return _max;
    }

    return std::nullopt;
}

//------------------------------------------------------------------------------
std::optional<double> ChartScale::min() const {
    if (std::isfinite(_min) == true) {
        return _min;
    }

    return std::nullopt;
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
        "Press ctrl + scroll wheel to change tick width.\n"
        "Press left button to show Y value.\n"
        "Press ctrl + left button to show value for selected line.\n"
        "Press middle button on label to select that line.\n"
        "Press right button for menu.\n"
    );

    _menu   = new Fl_Menu_Button(0, 0, 0, 0);
    _scroll = new Fl_Scrollbar(0, 0, 0, 0);

    add(_menu);
    add(_scroll);

    _scroll->type(FL_HORIZONTAL);
    _scroll->callback(Chart::_CallbackScrollbar, this);

    _menu->add(_CHART_SHOW_LABELS,      0, Chart::_CallbackToggle, this, FL_MENU_TOGGLE);
    _menu->add(_CHART_SHOW_HLINES,      0, Chart::_CallbackToggle, this, FL_MENU_TOGGLE);
    _menu->add(_CHART_SHOW_VLINES,      0, Chart::_CallbackToggle, this, FL_MENU_TOGGLE | FL_MENU_DIVIDER);
    _menu->add(_CHART_SETUP_LABEL,      0, _FLW_CHART_CB(setup_label()));
    _menu->add(_CHART_SETUP_AREA,       0, _FLW_CHART_CB(setup_area()));
    _menu->add(_CHART_SETUP_RANGE,      0, _FLW_CHART_CB(setup_date_range()));
    _menu->add(_CHART_SETUP_WIDTH,      0, _FLW_CHART_CB(setup_ywidth()), FL_MENU_DIVIDER);
    _menu->add(_CHART_SETUP_MIN_CLAMP,  0, _FLW_CHART_CB(setup_clamp(true)));
    _menu->add(_CHART_SETUP_MAX_CLAMP,  0, _FLW_CHART_CB(setup_clamp(false)));
    _menu->add(_CHART_SETUP_MOVE,       0, _FLW_CHART_CB(setup_move_lines()));
    _menu->add(_CHART_SETUP_DELETE,     0, _FLW_CHART_CB(setup_delete_lines()));
    _menu->add(_CHART_SETUP_SHOW,       0, _FLW_CHART_CB(setup_show_or_hide_lines()));
    _menu->add(_CHART_ADD_CSV,          0, _FLW_CHART_CB(load_cvs()), FL_MENU_DIVIDER);
    _menu->add(_CHART_SETUP_LINE,       0, _FLW_CHART_CB(setup_line()));
    _menu->add(_CHART_ADD_LINE,         0, _FLW_CHART_CB(setup_add_line()));
    _menu->add(_CHART_SAVE_CSV,         0, _FLW_CHART_CB(save_cvs()), FL_MENU_DIVIDER);
    _menu->add(_CHART_SAVE_JSON,        0, _FLW_CHART_CB(save_json()));
    _menu->add(_CHART_LOAD_JSON,        0, _FLW_CHART_CB(load_json()), FL_MENU_DIVIDER);
    _menu->add(_CHART_PRINT,            0, _FLW_CHART_CB(print_to_postscript()));
    _menu->add(_CHART_SAVE_PNG,         0, _FLW_CHART_CB(save_png()));
#ifdef DEBUG
    _menu->add(_CHART_DEBUG_CHART,      0, Chart::_CallbackDebugChart, this);
    _menu->add(_CHART_DEBUG_LINE,       0, Chart::_CallbackDebugLine, this);
#endif
    _menu->type(Fl_Menu_Button::POPUP3);

    _areas.push_back(ChartArea(ChartArea::NUM::ONE));
    _areas.push_back(ChartArea(ChartArea::NUM::TWO));
    _areas.push_back(ChartArea(ChartArea::NUM::THREE));
    _areas.push_back(ChartArea(ChartArea::NUM::FOUR));
    _areas.push_back(ChartArea(ChartArea::NUM::FIVE));

    _disable_menu = false;

    clear();
    update_pref();
    view_options();
}

//------------------------------------------------------------------------------
void Chart::_calc_area_height() {
    auto last   = 0;
    auto addh   = 0;
    auto height = 0;

    _top_space    = (_label == "") ? flw::PREF_FIXED_FONTSIZE : flw::PREF_FIXED_FONTSIZE * 3;
    _bottom_space = flw::PREF_FIXED_FONTSIZE * 3 + Fl::scrollbar_size();
    height        = h() - (_bottom_space + _top_space);

    for (size_t f = 1; f <= static_cast<size_t>(ChartArea::NUM::LAST); f++) {
        const auto& area = _areas[f];

        if (area.percent() >= Chart::MIN_AREA_SIZE) {
            height -= PREF_FIXED_FONTSIZE;
        }
    }

    _areas[0].set_y(_top_space);
    _areas[0].set_h(static_cast<int>((_areas[0].percent() / 100.0) * height));

    for (size_t f = 1; f <= static_cast<size_t>(ChartArea::NUM::LAST); f++) {
        auto& prev = _areas[f - 1];
        auto& area = _areas[f];

        if (area.percent() >= Chart::MIN_AREA_SIZE) {
            area.set_y(prev.y() + prev.h() + flw::PREF_FIXED_FONTSIZE);
            area.set_h(static_cast<int>((_areas[f].percent() / 100.0) * height));
            addh += static_cast<int>(prev.h());
            last  = f;
        }
    }

    if (last > 0) {
        _areas[last].set_h(static_cast<int>(height - addh));
    }
}

//------------------------------------------------------------------------------
void Chart::_calc_area_width() {
    const double width = w() - (_margin_left * flw::PREF_FIXED_FONTSIZE + _margin_right * flw::PREF_FIXED_FONTSIZE);

    _ticks     = static_cast<int>(width / _tick_width);
    _top_space = flw::PREF_FIXED_FONTSIZE;

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
        area.set_x(_margin_left * flw::PREF_FIXED_FONTSIZE);
        area.set_w(width);
    }
}

//------------------------------------------------------------------------------
void Chart::_calc_dates() {
    std::string min;
    std::string max;

    for (const auto& area : _areas) {
        for (const auto& line : area.lines()) {
            if (line.size() > 0 && line.is_visible() == true) {
                auto& first = line.data_vector().front();
                auto& last  = line.data_vector().back();

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

//------------------------------------------------------------------------------
// Calc min and max y values for current screen view.
// These are recalculated when data are scrolled.
// Clamp values does override data set.
//
void Chart::_calc_ymin_ymax() {
    for (auto& area : _areas) {
        auto min_clamp = area.clamp_min();
        auto max_clamp = area.clamp_max();
        
        area.left_scale().clear();
        area.right_scale().clear();

        for (const auto& line : area.lines()) {
            if (line.size() > 0 && line.is_visible() == true) {
                const int stop      = _date_start + _ticks;
                int       current   = _date_start;
                double    minimum   = INFINITY;
                double    maximum   = INFINITY;
                double    max;
                double    min;

                if (min_clamp.has_value() == true && (std::isfinite(minimum) == false || min_clamp.value() < minimum)) {
                    minimum = min_clamp.value();
                }

                if (max_clamp.has_value() == true && (std::isfinite(maximum) == false || max_clamp.value() > maximum)) {
                    maximum = max_clamp.value();
                }

                while (current <= stop && current < static_cast<int>(_dates.size())) {
                    const ChartData& date  = _dates[current];
                    const size_t     index = ChartData::BinarySearch(line.data_vector(), date);

                    if (index != static_cast<size_t>(-1)) {
                        const ChartData& data = line.data_vector()[index];

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
        }

        area.left_scale().fix_height();
        area.right_scale().fix_height();
    }
}

//------------------------------------------------------------------------------
void Chart::_calc_yscale() {
    for (auto& area : _areas) {
        area.left_scale().calc(area.h());
        area.right_scale().calc(area.h());
    }
}

//------------------------------------------------------------------------------
void Chart::_calc_ywidth() {
    auto l = false;
    auto r = false;

    for (const auto& area : _areas) {
        for (const auto& line : area.lines()) {
            if (line.size() == 0) {
            }
            else if (line.align() == FL_ALIGN_LEFT) {
                l = true;
            }
            else {
                r = true;
            }
        }
    }

    if (l == true) {
        _margin_left = _margin;
    }
    else {
        _margin_left = Chart::MIN_MARGIN;
    }

    if (r == true) {
        _margin_right = _margin;
    }
    else {
        _margin_right = Chart::MIN_MARGIN;
    }
}

//------------------------------------------------------------------------------
void Chart::_CallbackDebugChart(Fl_Widget*, void* widget) {
#ifdef DEBUG
    auto self = static_cast<const Chart*>(widget);
    self->debug();
#else
    (void) widget;
#endif
}

//------------------------------------------------------------------------------
void Chart::_CallbackDebugLine(Fl_Widget*, void* widget) {
#ifdef DEBUG
    auto self = static_cast<const Chart*>(widget);

    if (self->_area == nullptr || self->_area->selected_line() == nullptr) {
        return;
    }

    const auto* line = self->_area->selected_line();
    line->debug(-1, true);
#else
    (void) widget;
#endif
}

//------------------------------------------------------------------------------
bool Chart::_CallbackPrinter(void* data, int pw, int ph, int) {
    auto widget = static_cast<Fl_Widget*>(data);
    auto r      = Fl_Rect(widget);

    widget->resize(0, 0, pw, ph);
    widget->draw();
    widget->resize(r.x(), r.y(), r.w(), r.h());
    return false;
}

//------------------------------------------------------------------------------
void Chart::_CallbackScrollbar(Fl_Widget*, void* widget) {
    auto self = static_cast<Chart*>(widget);
    self->_date_start = self->_scroll->value();
    self->init(false);
}

//------------------------------------------------------------------------------
void Chart::_CallbackToggle(Fl_Widget*, void* widget) {
    auto self = static_cast<Chart*>(widget);
    self->_view.labels     = menu::item_value(self->_menu, _CHART_SHOW_LABELS);
    self->_view.vertical   = menu::item_value(self->_menu, _CHART_SHOW_VLINES);
    self->_view.horizontal = menu::item_value(self->_menu, _CHART_SHOW_HLINES);
    self->redraw();
}

//------------------------------------------------------------------------------
void Chart::clear() {
    static_cast<Fl_Valuator*>(_scroll)->value(0);

    _block_dates.clear();
    _dates.clear();

    for (auto& area : _areas) {
        area.clear();
    }

    _area         = nullptr;
    _bottom_space = 0;
    _cw           = 0;
    _date_start   = 0;
    _label        = "";
    _old          = Fl_Rect();
    _printing     = false;
    _tooltip      = "";
    _top_space    = 0;
    _ver_pos[0]   = -1;

    set_area_size();
    set_margin(Chart::DEF_MARGIN);
    set_date_range();
    set_tick_width();
    init(false);
}

//------------------------------------------------------------------------------
// Create new chart line from current line in active area and add it to current area.
//
bool Chart::create_line(ChartData::FORMULAS formula, bool support) {
    if (_area == nullptr || _area->selected_line() == nullptr || _area->size() >= ChartArea::MAX_LINES) {
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
        auto days = util::to_long(fl_input_str(2, "Enter number of days (2 - 99)", "14"));

        if (days < 2 || days > 99) {
            return false;
        }

        vec1   = ChartData::ATR(line0->data_vector(), days);
        label1 = util::format("ATR %d Days", days);
    }
    else if (formula == ChartData::FORMULAS::DAY_TO_WEEK) {
        auto answer = fl_choice("Would you like to use highest/lowest and last close value per week?\nOr sum values per week?", nullptr, "High/Low", "Sum");

        vec1   = ChartData::DayToWeek(line0->data_vector(), Date::DAY::SUNDAY, answer == 2);
        label1 = "Weekly (Sunday)";
        type1  = line0->type();
    }
    else if (formula == ChartData::FORMULAS::DAY_TO_MONTH) {
        auto answer = fl_choice("Would you like to use highest/lowest and last close value per month?\nOr sum values per month?", nullptr, "High/Low", "Sum");

        vec1   = ChartData::DayToMonth(line0->data_vector(), answer == 2);
        label1 = "Monthly";
        type1  = line0->type();
    }
    else if (formula == ChartData::FORMULAS::EXP_MOVING_AVERAGE) {
        auto days = util::to_long(fl_input_str(2, "Enter number of days (2 - 99)", "14"));

        if (days < 2 || days > 99) {
            return false;
        }

        vec1   = ChartData::ExponentialMovingAverage(line0->data_vector(), days);
        label1 = util::format("Exponential Moving Average %d Days", days);
    }
    else if (formula == ChartData::FORMULAS::FIXED) {
        auto value = util::to_double(fl_input_str(16, "Enter value", "0"));

        if (std::isinf(value) == true) {
            return false;
        }

        vec1   = ChartData::Fixed(line0->data_vector(), value);
        label1 = util::format("Horizontal %f", value);
        type1  = ChartLine::TYPE::EXPAND_HORIZONTAL;
    }
    else if (formula == ChartData::FORMULAS::MODIFY) {
        auto list = StringVector() = {"Addition", "Subtraction", "Multiplication", "Division"};
        auto mod  = dlg::choice("Select Modification", list, 0, top_window());

        if (mod < 0 || mod > static_cast<int>(ChartData::MODIFY::LAST)) {
            return false;
        }

        auto modify = static_cast<ChartData::MODIFY>(mod);
        auto value  = util::to_double(fl_input_str(16, "Enter value", "0"));

        if (std::isinf(value) == true) {
            return false;
        }
        else if (modify == ChartData::MODIFY::DIVISION && fabs(value) < 0.0001) {
            fl_alert("To small value for division!");
            return false;
        }

        vec1   = ChartData::Modify(line0->data_vector(), modify, value);
        label1 = util::format("Modified %s", line0->label().c_str());
        type1  = line0->type();
    }
    else if (formula == ChartData::FORMULAS::MOMENTUM) {
        auto days = util::to_long(fl_input_str(2, "Enter number of days (2 - 99)", "14"));

        if (days < 2 || days > 99) {
            return false;
        }

        vec1   = ChartData::Momentum(line0->data_vector(), days);
        label1 = util::format("Momentum %d Days", days);

        if (support == true) {
            vec2   = ChartData::Fixed(line0->data_vector(), 0.0);
            label2 = "Momentum Zero";
            type2  = ChartLine::TYPE::EXPAND_HORIZONTAL;
        }
    }
    else if (formula == ChartData::FORMULAS::MOVING_AVERAGE) {
        auto days = util::to_long(fl_input_str(2, "Enter number of days (2 - 99)", "14"));

        if (days < 2 || days > 99) {
            return false;
        }

        vec1   = ChartData::MovingAverage(line0->data_vector(), days);
        label1 = util::format("Moving Average %d Days", days);
    }
    else if (formula == ChartData::FORMULAS::RSI) {
        auto days = util::to_long(fl_input_str(2, "Enter number of days (2 - 99)", "14"));

        if (days < 2 || days > 99) {
            return false;
        }

        vec1   = ChartData::RSI(line0->data_vector(), days);
        label1 = util::format("RSI %d Days", days);

        if (support == true) {
            vec2   = ChartData::Fixed(line0->data_vector(), 30.0);
            label2 = "RSI 30";
            type2  = ChartLine::TYPE::EXPAND_HORIZONTAL;
            vec3   = ChartData::Fixed(line0->data_vector(), 70.0);
            label3 = "RSI 70";
            type3  = ChartLine::TYPE::EXPAND_HORIZONTAL;
        }
    }
    else if (formula == ChartData::FORMULAS::STD_DEV) {
        auto days = util::to_long(fl_input_str(2, "Enter number of days (2 - 99)", "14"));

        if (days < 2 || days > 99) {
            return false;
        }

        vec1   = ChartData::StdDev(line0->data_vector(), days);
        label1 = util::format("Std. dev. %d Days", days);
    }
    else if (formula == ChartData::FORMULAS::STOCHASTICS) {
        auto days = util::to_long(fl_input_str(2, "Enter number of days (2 - 99)", "14"));

        if (days < 2 || days > 99) {
            return false;
        }

        vec1   = ChartData::Stochastics(line0->data_vector(), days);
        label1 = util::format("Stochastics %d Days", days);

        if (support == true) {
            vec2   = ChartData::Fixed(line0->data_vector(), 20.0);
            label2 = "Stochastics 20";
            type2  = ChartLine::TYPE::EXPAND_HORIZONTAL;
            vec3   = ChartData::Fixed(line0->data_vector(), 80.0);
            label3 = "Stochastics 80";
            type3  = ChartLine::TYPE::EXPAND_HORIZONTAL;
        }
    }

    if (vec1.size() == 0) {
        fl_alert("To few datapoints!");
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

    init(true);
    return true;
}

//------------------------------------------------------------------------------
// Create tooltip string either by using mouse cursor pos and convert it to real value or use chart data.
// If ctrl is pressed then use actual chart data.
//
void Chart::_create_tooltip(bool ctrl) {
    int         X   = Fl::event_x();
    int         Y   = Fl::event_y();
    std::string old = _tooltip;

    _tooltip = "";

    if (_area == nullptr) {
        Fl::redraw();
        return;
    }

    const Date::FORMAT format    = (_date_format == Date::FORMAT::ISO) ? Date::FORMAT::NAME_LONG : Date::FORMAT::ISO_TIME_LONG;
    const int          stop      = _date_start + _ticks;
    int                start     = _date_start;
    int                X1        = x() + _margin_left * flw::PREF_FIXED_FONTSIZE;
    int                left_dec  = 0;
    int                right_dec = 0;

    if (_area->left_scale().tick() < 10.0 ) {
        left_dec = flw::_chart_count_decimals(_area->left_scale().tick()) + 1;
    }

    if (_area->right_scale().tick() < 10.0 ) {
        right_dec = flw::_chart_count_decimals(_area->right_scale().tick()) + 1;
    }

    while (start <= stop && start < static_cast<int>(_dates.size())) {
        if (X >= X1 && X <= X1 + _tick_width - 1) { // Is mouse x pos inside current tick?
            const std::string fancy_date = Date::FromString(_dates[start].date.c_str()).format(format);
            const ChartLine*  line       = _area->selected_line();

            _tooltip = fancy_date + "\n \n \n ";

            if (ctrl == false || line == nullptr || line->size() == 0 || line->is_visible() == false) { // Convert mouse pos to scale value.
                const double ydiff = static_cast<double>((y() + _area->y2()) - Y);
                auto         l_min = _area->left_scale().min();
                auto         r_min = _area->right_scale().min();
                std::string  left;
                std::string  right;

                if (_area->left_scale().diff() > 0.0) {
                    left = flw::_chart_format_double(l_min.value() + (ydiff / _area->left_scale().pixel()), left_dec, '\'');
                }

                if (_area->right_scale().diff() > 0.0) {
                    right = flw::_chart_format_double(r_min.value() + (ydiff / _area->right_scale().pixel()), right_dec, '\'');
                }

                const size_t len = (left.length() > right.length()) ? left.length() : right.length();

                if (left != "" && right != "") {
                    _tooltip = util::format("%s\nleft:  %*s\nright: %*s\n \n ", fancy_date.c_str(), static_cast<int>(len), left.c_str(), static_cast<int>(len), right.c_str());
                }
                else if (left != "") {
                    _tooltip = util::format("%s\nleft: %*s\n \n ", fancy_date.c_str(), static_cast<int>(len), left.c_str());
                }
                else if (right != "") {
                    _tooltip = util::format("%s\nright: %*s\n \n ", fancy_date.c_str(), static_cast<int>(len), right.c_str());
                }
            }
            else { // Use actual chart data.
                const size_t index = ChartData::BinarySearch(line->data_vector(), _dates[start]);

                if (index != (size_t) -1) {
                    const int         dec   = (line->align() == FL_ALIGN_RIGHT) ? right_dec : left_dec;
                    const ChartData&  data  = line->data_vector()[index];
                    const std::string high  = flw::_chart_format_double(data.high, dec, '\'');
                    const std::string low   = flw::_chart_format_double(data.low, dec, '\'');
                    const std::string close = flw::_chart_format_double(data.close, dec, '\'');
                    const int         len   = static_cast<int>((low.length() > high.length()) ? low.length() : high.length());

                    _tooltip = util::format("%s\nhigh:  %*s\nclose: %*s\nlow:   %*s", fancy_date.c_str(), len, high.c_str(), len, close.c_str(), len, low.c_str());
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
void Chart::debug() const {
#ifdef DEBUG
    ChartData first, last, start, end;

    if (_dates.size()) {
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
    fprintf(stderr, "\tmargin:          %19d\n", _margin);
    fprintf(stderr, "\tpixels left:     %19d\n", _margin_left * flw::PREF_FIXED_FONTSIZE);
    fprintf(stderr, "\tpixels right:    %19d\n", _margin_right * flw::PREF_FIXED_FONTSIZE);
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

    for (const auto& area : _areas) {
        area.debug();
    }

    fflush(stderr);
#endif
}

//------------------------------------------------------------------------------
void Chart::draw() {
#ifdef DEBUG
//     auto t = util::milliseconds();
#endif

    if (_printing == true) {
        fl_rectf(x(), y(), w(), h(), FL_BACKGROUND2_COLOR);
    }
    else {
        Fl_Group::draw();
    }

    fl_font(flw::PREF_FIXED_FONT, flw::PREF_FIXED_FONTSIZE);
    _cw = fl_width("X");

    if (w() < 50 || h() < 50) {
        return;
    }

    fl_push_clip(x(), y(), w(), h() - _scroll->h());
    _draw_label();
    _draw_xlabels();

    for (auto& area : _areas) {
        if (area.percent() >= Chart::MIN_AREA_SIZE) {
            _draw_area(area);
        }
    }

    _draw_tooltip();
    fl_pop_clip();
    fl_line_style(0);

#ifdef DEBUG
//     fprintf(stderr, "draw: %3d mS\n", util::milliseconds() - t);
//     fflush(stdout);
#endif
}

//------------------------------------------------------------------------------
void Chart::_draw_area(ChartArea& area) {
    _draw_ver_lines(area);
    _draw_ylabels(_margin_left * flw::PREF_FIXED_FONTSIZE, area.y2(), area.y(), area.left_scale(), true);
    _draw_ylabels(w() - _margin_right * flw::PREF_FIXED_FONTSIZE, area.y2(), area.y(), area.right_scale(), false);
    fl_push_matrix();
    fl_push_clip(x() + area.x(), y() + area.y() - 1, area.w() + 1, area.h() + 2);

    for (const auto& line : area.lines()) {
        _draw_line(line, (line.align() == FL_ALIGN_LEFT) ? area.left_scale() : area.right_scale(), area.x(), area.y(), area.w(), area.h());
    }

    _draw_line_labels(area);
    fl_rect(x() + area.x(), y() + area.y() - 1, area.w() + 1, area.h() + 2, labelcolor());
    fl_pop_clip();
    fl_pop_matrix();
}

//------------------------------------------------------------------------------
void Chart::_draw_label() {
    if (_label == "") {
        return;
    }

    fl_color(FL_FOREGROUND_COLOR);
    fl_font(flw::PREF_FIXED_FONT, flw::PREF_FIXED_FONTSIZE * 1.5);
    fl_draw(_label.c_str(), x(), y() + flw::PREF_FIXED_FONTSIZE, w(), _top_space, FL_ALIGN_CENTER);
}

//------------------------------------------------------------------------------
void Chart::_draw_line(const ChartLine& line, const ChartScale& scale, int X, const int Y, const int W, const int H) {
    if (line.size() > 0 && line.is_visible() == true && scale.diff() > 0.0) {
        const double y2      = y() + Y + H;
        double       lastX   = -10;
        double       lastY   = -10;
        int          current = _date_start;
        const int    stop    = _date_start + _ticks + 1;
        int          width   = static_cast<int>(line.width());
        auto         style   = FL_SOLID;

        if (width == 0) {
            width  = _tick_width - 1;
            width -= static_cast<int>(_tick_width / 10.0);
        }

        if (width >= _tick_width) {
            width = _tick_width - 1;
        }

        const int lw2 = static_cast<int>(width / 2.0);
        const int lw4 = static_cast<int>(lw2 / 2.0);

        fl_push_clip(x() + X, y() + Y, W + 1, H + 1);
        fl_color(line.color());

        if (line.type() == ChartLine::TYPE::LINE_DOT) {
            style = FL_DOT;
        }

        if (line.type() == ChartLine::TYPE::BAR_HLC && line.width() == 0) {
            fl_line_style(style, lw2);
        }
        else if (line.type() == ChartLine::TYPE::HORIZONTAL && line.width() == 0) {
            fl_line_style(style, width > 6 ? 6 : width);
        }
        else if ((line.type() == ChartLine::TYPE::LINE || line.type() == ChartLine::TYPE::LINE_DOT) && line.width() == 0) {
            fl_line_style(style, width > 10 ? 10 : width);
        }
        else {
            fl_line_style(style, width);
        }

        while (current <= stop && current < (int) _dates.size()) {
            const ChartData& date  = _dates[current];
            const size_t     index = ChartData::BinarySearch(line.data_vector(), date);

            if (index != (size_t) -1) {
                const ChartData& pr  = line.data_vector()[index];
                const double     yh  = (pr.high - scale.min().value()) * scale.pixel();
                const double     yl  = (pr.low - scale.min().value()) * scale.pixel();
                const double     yc  = (pr.close - scale.min().value()) * scale.pixel();
                const int        y2h = static_cast<int>(y2 - yh);
                const int        y2l = static_cast<int>(y2 - yl);
                const int        y2c = static_cast<int>(y2 - yc);

                if (line.type() == ChartLine::TYPE::LINE || line.type() == ChartLine::TYPE::LINE_DOT) {
                    if (lastX > -10 && lastY > -10) {
                        fl_line(lastX + lw2, static_cast<int>(y2 - lastY), x() + X + lw2, y2c);
                    }
                }
                else if (line.type() == ChartLine::TYPE::BAR_HLC) {
                    fl_line(x() + X + lw4, y2l, x() + X + lw4, y2h);
                    fl_line(x() + X, y2c, x() + X + _tick_width - 1, y2c);
                }
                else if (line.type() == ChartLine::TYPE::HORIZONTAL) {
                    fl_line(x() + X, y2c, x() + X + _tick_width, y2c);
                }
                else if (line.type() == ChartLine::TYPE::BAR) {
                    auto h = yh - yl;
                    fl_rectf(x() + X, y2h, width, (h < 1) ? 1 : h);
                }
                else if (line.type() == ChartLine::TYPE::BAR_CLAMP) {
                    fl_rectf(x() + X, y2h, width, yh);
                }
                else if (line.type() == ChartLine::TYPE::EXPAND_VERTICAL) {
                    fl_line(x() + X, y2, x() + X, y() + Y);
                }
                else if (line.type() == ChartLine::TYPE::EXPAND_HORIZONTAL) {
                    fl_line(x() + _margin_left * flw::PREF_FIXED_FONTSIZE, y2c, x() + Fl_Widget::w() - _margin_right * flw::PREF_FIXED_FONTSIZE, y2c);
                    break;
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
        const int left_x  = x() + area.x() + 6;
        int       left_y  = y() + area.y() + 6;
        int       right_h = 0;
        int       right_w = 0;
        const int right_x = x() + area.x2();
        int       right_y = left_y;

        for (const auto& line : area.lines()) { // Calc width and height of labels.
            if (line.size() > 0) {
                int len = static_cast<int>(line.label().length());

                if (line.align() == FL_ALIGN_RIGHT) {
                    right_h++;

                    if (len * _cw > right_w) {
                        right_w = len * _cw;
                    }
                }
                else {
                    left_h++;

                    if (len * _cw > left_w) {
                        left_w = len * _cw;
                    }
                }
            }
        }

        left_h  *= flw::PREF_FIXED_FONTSIZE;
        right_h *= flw::PREF_FIXED_FONTSIZE;

        if (left_w > 0) { // Draw left box.
            left_w += _cw * 2 + 4;
            fl_color(FL_BACKGROUND2_COLOR);
            fl_rectf(left_x, left_y, left_w + 8, left_h + 8);

            fl_color(FL_FOREGROUND_COLOR);
            fl_rect(left_x, left_y, left_w + 8, left_h + 8);
        }

        if (right_w > 0) { // Draw right box.
            right_w += _cw * 2 + 4;
            fl_color(FL_BACKGROUND2_COLOR);
            fl_rectf(right_x - right_w - 14, left_y, right_w + 6, right_h + 8);

            fl_color(FL_FOREGROUND_COLOR);
            fl_rect(right_x - right_w - 14, left_y, right_w + 6, right_h + 8);
        }

        fl_font(flw::PREF_FIXED_FONT, flw::PREF_FIXED_FONTSIZE);

        if (left_w > 0 || right_w > 0) { // Draw labels.
            size_t count = 0;

            for (auto& line : area.lines()) {
                if (line.size() > 0) {
                    auto label = line.label();

                    if (area.selected() == count++) {
                        label = "@-> " + label;
                    }

                    fl_color((line.is_visible() == false) ? FL_GRAY : line.color());

                    if (line.align() == FL_ALIGN_RIGHT) {
                        fl_draw(label.c_str(), right_x - right_w - 10, right_y + 4, right_w + 8, flw::PREF_FIXED_FONTSIZE, FL_ALIGN_LEFT);
                        const_cast<ChartLine*>(&line)->set_label_rect(right_x - right_w - 10, right_y + 4, right_w + 8, flw::PREF_FIXED_FONTSIZE);
                        right_y += flw::PREF_FIXED_FONTSIZE;
                    }
                    else {
                        fl_draw(label.c_str(), left_x + 4, left_y + 4, left_w + 8, flw::PREF_FIXED_FONTSIZE, FL_ALIGN_LEFT);
                        const_cast<ChartLine*>(&line)->set_label_rect(left_x + 4, left_y + 4, left_w + 8, flw::PREF_FIXED_FONTSIZE);
                        left_y += flw::PREF_FIXED_FONTSIZE;
                    }
                }
            }
        }
    }
}

//------------------------------------------------------------------------------
void Chart::_draw_tooltip() {
    if (_tooltip == "" || _area == nullptr) {
        return;
    }

    int       X = Fl::event_x();
    int       Y = Fl::event_y();
    const int W = 14;
    const int H = 5;

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

    fl_line(Fl::event_x(), y() + _area->y(), Fl::event_x(), y() + _area->y() + _area->h());
    fl_line(x() + _area->x(), Fl::event_y(), x() + _area->x() + _area->w(), Fl::event_y());

    fl_font(flw::PREF_FIXED_FONT, flw::PREF_FIXED_FONTSIZE);
    fl_draw(_tooltip.c_str(), X + flw::PREF_FIXED_FONTSIZE, Y, flw::PREF_FIXED_FONTSIZE * W, flw::PREF_FIXED_FONTSIZE * H, FL_ALIGN_LEFT | FL_ALIGN_CENTER | FL_ALIGN_INSIDE);
}

//------------------------------------------------------------------------------
void Chart::_draw_ver_lines(const ChartArea& area) {
    if (_view.vertical == true) {
        fl_color(fl_color_average(FL_FOREGROUND_COLOR, FL_BACKGROUND2_COLOR, 0.2));

        for (size_t i = 0; i < Chart::MAX_VLINES; i++) {
            if (_ver_pos[i] < 0) {
                break;
            }
            else {
                fl_line(_ver_pos[i], y() + area.y(), _ver_pos[i], y() + area.y2() - 1);
            }
        }
    }
}

//------------------------------------------------------------------------------
// Draw date x labels.
// And also save x pos for vertical lines.
//
void Chart::_draw_xlabels() {
    const int  stop    = _date_start + _ticks;
    const int  cw2     = _cw * 2;
    const int  cw4     = _cw * 4;
    const int  fs05    = flw::PREF_FIXED_FONTSIZE * 0.5;
    const int  Y       = y() + (h() - _bottom_space);
    int        start   = _date_start;
    int        count_v = 0;
    int        last    = -1;
    int        X1      = x() + _margin_left * flw::PREF_FIXED_FONTSIZE;
    int        X2      = 0;
    char       buffer1[100];
    char       buffer2[100];

    _ver_pos[0] = -1;

    while (start <= stop && start < (int) _dates.size()) {
        const ChartData& data = _dates[start];
        const Date       date = Date::FromString(data.date.c_str());
        bool             addv = false;

        fl_color(labelcolor());
        fl_line(X1, Y, X1, Y + fs05);

        *buffer1 = 0;
        *buffer2 = 0;

        if (_date_range == ChartData::RANGE::HOUR) {
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
        else if (_date_range == flw::ChartData::RANGE::MIN) {
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
        else if (_date_range == ChartData::RANGE::SEC) {
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
        else if (_date_range == ChartData::RANGE::DAY || _date_range == ChartData::RANGE::WEEKDAY) {
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
        else if (_date_range == ChartData::RANGE::FRIDAY || _date_range == ChartData::RANGE::SUNDAY) {
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
        else if (_date_range == ChartData::RANGE::MONTH) {
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
            auto len  = static_cast<double>(strlen(buffer1));
            auto half = static_cast<double>((len / 2.0) * _cw);

            fl_font(flw::PREF_FIXED_FONT, flw::PREF_FIXED_FONTSIZE);
            fl_draw(buffer1, X1 - half, Y + flw::PREF_FIXED_FONTSIZE + 6, half + half, flw::PREF_FIXED_FONTSIZE, FL_ALIGN_CENTER | FL_ALIGN_INSIDE);
            fl_line(X1, Y, X1, Y + flw::PREF_FIXED_FONTSIZE);
            X2 = X1 + _cw + half + half;
        }

        if (*buffer2 != 0 && cw2 <= _tick_width) { // Draw second x label
            fl_font(flw::PREF_FIXED_FONT, flw::PREF_FIXED_FONTSIZE - 2);
            fl_draw(buffer2, X1 - cw2, Y + fs05, cw4, flw::PREF_FIXED_FONTSIZE, FL_ALIGN_CENTER | FL_ALIGN_INSIDE);
        }

        if (addv == true && count_v < static_cast<int>(Chart::MAX_VLINES - 1)) { // Save x pos for vertical lines.
            _ver_pos[count_v++] = X1;
        }

        X1    += _tick_width;
        start += 1;
    }

    _ver_pos[count_v] = -1;
}

//------------------------------------------------------------------------------
void Chart::_draw_ylabels(const int X, double Y1, const double Y2, const ChartScale& scale, const bool left) {
    if (scale.diff() < 0.0 || scale.pixel() * scale.tick() < 1.0) {
        return;
    }

    const double yinc  = (scale.pixel() * scale.tick());
    const double fs05  = flw::PREF_FIXED_FONTSIZE * 0.5;
    const double fr    = flw::_chart_count_decimals(scale.tick());
    int          width = w() - (_margin_left * flw::PREF_FIXED_FONTSIZE + _margin_right * flw::PREF_FIXED_FONTSIZE);
    double       ylast = INFINITY;
    double       yval  = scale.min().value();

    fl_font(flw::PREF_FIXED_FONT, flw::PREF_FIXED_FONTSIZE);

    while (static_cast<int>(Y1 + 0.5) >= static_cast<int>(Y2)) {
        if (std::isfinite(ylast) == false || ylast > Y1) {
            double  y1  = y() + Y1;
            double  x1  = x() + X;
            auto string = flw::_chart_format_double(yval, fr, '\'');

            if (left == true) {
                fl_color(labelcolor());
                fl_line(static_cast<int>(x1 - fs05), static_cast<int>(y1), static_cast<int>(x1), static_cast<int>(y1));
                fl_draw(string.c_str(), x(), static_cast<int>(y1 - fs05), _margin_left * flw::PREF_FIXED_FONTSIZE - flw::PREF_FIXED_FONTSIZE, flw::PREF_FIXED_FONTSIZE, FL_ALIGN_RIGHT | FL_ALIGN_INSIDE);

                if (_view.horizontal == true && static_cast<int>(Y1) >= static_cast<int>(Y2 + fs05)) {
                    fl_color(fl_color_average(FL_FOREGROUND_COLOR, FL_BACKGROUND2_COLOR, 0.2));
                    fl_line(static_cast<int>(x1 + 1.0), static_cast<int>(y1), static_cast<int>(x1 + width - 1.0), static_cast<int>(y1));
                }
            }
            else {
                fl_color(labelcolor());
                fl_line(static_cast<int>(x1), static_cast<int>(y1), static_cast<int>(x1 + fs05), static_cast<int>(y1));
                fl_draw(string.c_str(), static_cast<int>(x1 + flw::PREF_FIXED_FONTSIZE), static_cast<int>(y1 - fs05), _margin_right * flw::PREF_FIXED_FONTSIZE - (flw::PREF_FIXED_FONTSIZE * 2), flw::PREF_FIXED_FONTSIZE, FL_ALIGN_LEFT | FL_ALIGN_INSIDE);

                if (_view.horizontal == true && static_cast<int>(Y1) >= static_cast<int>(Y2 + fs05)) {
                    fl_color(fl_color_average(FL_FOREGROUND_COLOR, FL_BACKGROUND2_COLOR, 0.2));
                    fl_line(static_cast<int>(x1 - width + 1.0), static_cast<int>(y1), static_cast<int>(x1 - 1.0), static_cast<int>(y1));
                }
            }

            ylast = Y1 - (flw::PREF_FIXED_FONTSIZE + fs05);
        }

        Y1   -= yinc;
        yval += scale.tick();
    }
}

//------------------------------------------------------------------------------
int Chart::handle(int event) {
    static bool LEFT = false;

    if (event == FL_PUSH) {
        auto x = Fl::event_x();
        auto y = Fl::event_y();
        auto b = Fl::event_button();

        if (b == FL_LEFT_MOUSE) { // Create and draw tooltip.
            LEFT = true;
            _area = _inside_area(x, y);
            _create_tooltip(Fl::event_ctrl());

            if (_tooltip != "") {
                return 1;
            }
        }
        else if (b == FL_MIDDLE_MOUSE) { // Select line if pressed on line label.
            LEFT = false;
            _area = _inside_area(x, y);

            if (_area != nullptr) {
                auto c = 0;

                for (const auto& line : _area->lines()) {
                    auto& r = line.label_rect();

                    if (x > r.x() && x < r.r() && y > r.y() && y < r.b()) {
                        _area->set_selected(c);
                        redraw();
                        break;
                    }

                    c++;
                }
            }
            return 1;
        }
        else if (b == FL_RIGHT_MOUSE) { // Show menu.
            LEFT  = false;
            _area = _inside_area(x, y);

            _show_menu();
            return 1;
        }
    }
    else if (event == FL_DRAG && LEFT == true) { // Update tooltip and draw it.
        _area = _inside_area(Fl::event_x(), Fl::event_y());
        _create_tooltip(Fl::event_ctrl() != 0);

        if (_tooltip != "") {
            return 1;
        }
    }
    else if (event == FL_MOVE) { // Erase tooltip.
        if (_tooltip != "") {
            _tooltip = "";
            redraw();
        }
    }
    else if (event == FL_MOUSEWHEEL) { // Change tick value.
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
                init(false);
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

//------------------------------------------------------------------------------
void Chart::init(bool calc_dates) {
#ifdef DEBUG
//     auto t = util::milliseconds();
#endif
    fl_font(flw::PREF_FIXED_FONT, flw::PREF_FIXED_FONTSIZE);

    if (calc_dates == true) {
        WaitCursor wc;
        _calc_dates();
    }

    _calc_ywidth();
    _calc_area_height();
    _calc_area_width();
    _calc_ymin_ymax();
    _calc_yscale();
    redraw();

#ifdef DEBUG
//     fprintf(stderr, "%s: %3d mS\n", (calc_dates == true) ? "INIT" : "init", (int) (util::milliseconds() - t));
//     fflush(stdout);
#endif
}

//------------------------------------------------------------------------------
flw::ChartArea* Chart::_inside_area(int X, int Y) {
    for (auto& area : _areas) {
        if (area.percent() >= 10 &&
            X >= x() + area.x() &&
            Y >= y() + area.y() &&
            X <= x() + area.x2() &&
            Y <= y() + area.y2() + 1) {
            return &area;
        }
    }

    return nullptr;
}

//------------------------------------------------------------------------------
//
bool Chart::load_cvs() {
    if (_area == nullptr || _area->size() >= ChartArea::MAX_LINES) {
        return false;
    }

    auto filename = util::to_string(fl_file_chooser("Select CSV File", "All Files (*)\tCSV Files (*.csv)", ""));

    if (util::is_whitespace_or_empty(filename.c_str()) == true) {
        return false;
    }

    auto vec = ChartData::LoadCSV(filename);

    if (vec.size() == 0) {
        fl_alert("To few datapoints!");
        return false;
    }

    auto line = ChartLine(vec, filename);
    ChartLineSetup(top_window(), line).run();
    _area->add_line(line);
    init(true);

    return true;
}

//------------------------------------------------------------------------------
bool Chart::load_json() {
    auto filename = util::to_string(fl_file_chooser("Select JSON File", "All Files (*)\tJSON Files (*.json)", ""));

    if (util::is_whitespace_or_empty(filename.c_str()) == true) {
        return false;
    }

    return load_json(filename);
}

//------------------------------------------------------------------------------
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
            long long int chart[7] = { 0 };

            for (const auto j2 : j->vo_to_va()) {
                if (j2->name() == "version" && j2->is_number() == true) {
                    if (j2->vn_i() != Chart::VERSION) { fl_alert("error: wrong chart version!\nI expected version %d but the json file had version %d!", static_cast<int>(j2->vn_i()), Chart::VERSION); clear(); return false; }
                }
                else if (j2->name() == "label" && j2->is_string() == true) {
                    set_label(j2->vs_u());
                }
                else if (j2->name() == "date_range" && j2->is_number() == true) chart[1] = j2->vn_i();
                else if (j2->name() == "horizontal" && j2->is_bool() == true)   chart[2] = j2->vb();
                else if (j2->name() == "labels" && j2->is_bool() == true)       chart[3] = j2->vb();
                else if (j2->name() == "margin" && j2->is_number() == true)     chart[4] = j2->vn_i();
                else if (j2->name() == "tick_width" && j2->is_number() == true) chart[5] = j2->vn_i();
                else if (j2->name() == "vertical" && j2->is_bool() == true)     chart[6] = j2->vb();
                else _FLW_CHART_ERROR(j2)
            }
            if (chart[1] >= 0 && chart[1] <= static_cast<int>(ChartData::RANGE::SEC)) set_date_range(static_cast<ChartData::RANGE>(chart[1]));
            else chart[0]++;
            view_options(chart[3], chart[2], chart[6]);
            chart[0] += set_margin(chart[4]) == false;
            chart[0] += set_tick_width(chart[5]) == false;
            if (chart[0] != 0) _FLW_CHART_ERROR(j)
        }
        else if (j->name() == "flw::chart_areas" && j->is_object() == true) {
            long long int area[6] = { 0 };
            
            for (const auto j2 : j->vo_to_va()) {
                if (j2->name() == "area0" && j2->is_number() == true)             area[1]   = j2->vn_i();
                else if (j2->name() == "area1" && j2->is_number() == true)        area[2]   = j2->vn_i();
                else if (j2->name() == "area2" && j2->is_number() == true)        area[3]   = j2->vn_i();
                else if (j2->name() == "area3" && j2->is_number() == true)        area[4]   = j2->vn_i();
                else if (j2->name() == "area4" && j2->is_number() == true)        area[5]   = j2->vn_i();
                else if (j2->name() == "min0" && j2->is_number() == true)         _areas[0].set_min_clamp(j2->vn());
                else if (j2->name() == "max0" && j2->is_number() == true)         _areas[0].set_max_clamp(j2->vn());
                else if (j2->name() == "min1" && j2->is_number() == true)         _areas[1].set_min_clamp(j2->vn());
                else if (j2->name() == "max1" && j2->is_number() == true)         _areas[1].set_max_clamp(j2->vn());
                else if (j2->name() == "min2" && j2->is_number() == true)         _areas[2].set_min_clamp(j2->vn());
                else if (j2->name() == "max2" && j2->is_number() == true)         _areas[2].set_max_clamp(j2->vn());
                else if (j2->name() == "min3" && j2->is_number() == true)         _areas[3].set_min_clamp(j2->vn());
                else if (j2->name() == "max3" && j2->is_number() == true)         _areas[3].set_max_clamp(j2->vn());
                else if (j2->name() == "min4" && j2->is_number() == true)         _areas[4].set_min_clamp(j2->vn());
                else if (j2->name() == "max4" && j2->is_number() == true)         _areas[4].set_max_clamp(j2->vn());
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
                            if (line[1] < 0 || line[1] > static_cast<int>(ChartArea::NUM::LAST)) _FLW_CHART_ERROR(l)
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
                auto a = static_cast<ChartArea::NUM>(line[1]);
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

            block_dates(dates);
        }
        else _FLW_CHART_ERROR(j)
    }

    init(true);
    return true;
}

//------------------------------------------------------------------------------
bool Chart::_move_or_delete_line(ChartArea* area, size_t index, bool move, ChartArea::NUM destination) {
    if (area == nullptr || index >= area->size()) {
        return false;
    }

    ChartLine* line1 = area->line(index);

    if (line1 == nullptr) {
        return false;
    }

    if (move == true) {
        if (destination == area->num()) {
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

//------------------------------------------------------------------------------
void Chart::print_to_postscript() {
    _printing = true;
    dlg::print("Print Chart", Chart::_CallbackPrinter, this, 1, 1, top_window());
    _printing = false;
    redraw();
}

//------------------------------------------------------------------------------
void Chart::resize(int X, int Y, int W, int H) {
    if (_old.w() == W && _old.h() == H) {
        return;
    }

    Fl_Widget::resize(X, Y, W, H);
    _scroll->resize(X, Y + H - Fl::scrollbar_size(), W, Fl::scrollbar_size());
    _old = Fl_Rect(this);
    init(false);
}

//------------------------------------------------------------------------------
bool Chart::save_cvs() {
    if (_area == nullptr || _area->selected_line() == nullptr) {
        return false;
    }

    auto filename = util::to_string(fl_file_chooser("Save To CSV File", "All Files (*)\tCSV Files (*.csv)", ""));

    if (util::is_whitespace_or_empty(filename.c_str()) == true) {
        return false;
    }

    const auto* line = _area->selected_line();
    return ChartData::SaveCSV(line->data_vector(), filename);
}

//------------------------------------------------------------------------------
bool Chart::save_json() {
    auto filename = util::to_string(fl_file_chooser("Save To JSON File", "All Files (*)\tJSON Files (*.json)", ""));

    if (util::is_whitespace_or_empty(filename.c_str()) == true) {
        return false;
    }

    return save_json(filename);
}

//----------------------------------------------------------------------
bool Chart::save_json(std::string filename, double max_diff_high_low) const {
    auto wc  = WaitCursor();
    auto jsb = JSB();

    try {
        jsb << JSB::MakeObject();
            jsb << JSB::MakeObject("flw::chart");
                jsb << JSB::MakeNumber(Chart::VERSION, "version");
                jsb << JSB::MakeString(_label.c_str(), "label");
                jsb << JSB::MakeNumber(_tick_width, "tick_width");
                jsb << JSB::MakeNumber((double) _date_range, "date_range");
                jsb << JSB::MakeNumber(_margin, "margin");
                jsb << JSB::MakeBool(_view.labels, "labels");
                jsb << JSB::MakeBool(_view.horizontal, "horizontal");
                jsb << JSB::MakeBool(_view.vertical, "vertical");
            jsb.end();
            jsb << JSB::MakeObject("flw::chart_areas");
            for (size_t f = 0; f <= static_cast<int>(ChartArea::NUM::LAST); f++) {
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
                for (const auto& area : _areas) {
                    for (const auto& line : area.lines()) {
                        if (line.size() > 0) {
                            jsb << JSB::MakeObject();
                                jsb << JSB::MakeNumber(static_cast<int>(area.num()), "area");
                                jsb << JSB::MakeString(line.label(), "label");
                                jsb << JSB::MakeString(line.type_to_string(), "type");
                                jsb << JSB::MakeNumber(line.align(), "align");
                                jsb << JSB::MakeNumber(line.color(), "color");
                                jsb << JSB::MakeNumber(line.width(), "width");
                                jsb << JSB::MakeBool(line.is_visible(), "visible");
                                jsb << JSB::MakeArray("yx");
                                for (const auto& data : line.data_vector()) {
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

//------------------------------------------------------------------------------
bool Chart::save_png() {
    return util::png_save("", top_window(), x() + 1,  y() + 1,  w() - 2,  h() - _scroll->h() - 1);
}

//------------------------------------------------------------------------------
// Minimum area size is 0 if it is hidden or 10 - 100.
// All sizes (in percent) added together must be 100.
//
bool Chart::set_area_size(int area1, int area2, int area3, int area4, int area5) {
    _area = nullptr;

    if ((area1 == 0 || (area1 >= 10 && area1 <= 100)) &&
        (area1 == 0 || (area1 >= 10 && area1 <= 100)) &&
        (area1 == 0 || (area1 >= 10 && area1 <= 100)) &&
        (area1 == 0 || (area1 >= 10 && area1 <= 100)) &&
        (area1 == 0 || (area1 >= 10 && area1 <= 100)) &&
        area1 + area2 + area3 + area4 + area5 == 100) {
    
        _areas[0].set_percent(area1);
        _areas[1].set_percent(area2);
        _areas[2].set_percent(area3);
        _areas[3].set_percent(area4);
        _areas[4].set_percent(area5);
        return true;
    }

    return false;
}

//------------------------------------------------------------------------------
void Chart::set_date_range(ChartData::RANGE range) {
    _date_range  = range;
    _date_format = (_date_range == ChartData::RANGE::HOUR || _date_range == ChartData::RANGE::MIN || _date_range == ChartData::RANGE::SEC) ? Date::FORMAT::ISO_TIME : Date::FORMAT::ISO;
}

//------------------------------------------------------------------------------
bool Chart::set_margin(int def) {
    if (def < Chart::MIN_MARGIN || def > Chart::MAX_MARGIN) {
        return false;
    }

    _margin = def;
    return true;
}

//------------------------------------------------------------------------------
bool Chart::set_tick_width(int width) {
    if (width < Chart::MIN_TICK || width > Chart::MAX_TICK) {
        return false;
    }

    _tick_width = width;
    redraw();
    return true;
}

//------------------------------------------------------------------------------
void Chart::setup_add_line() {
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

//------------------------------------------------------------------------------
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

    init(false);
}

//------------------------------------------------------------------------------
void Chart::setup_clamp(bool min) {
    if (_area == nullptr) {
        return;
    }
    
    auto num    = static_cast<int>(_area->num()) + 1;
    auto clamp  = (min == true) ? _area->clamp_min() : _area->clamp_max();
    auto input  = (clamp.has_value() == true) ? util::format("%f", clamp.value()) : "inf";
    auto info   = (min == true) ? util::format("Enter min clamp value or inf to disable for area %d", num) : util::format("Enter max clamp value or inf to disable for area %d", num);
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

    init(false);
}

//------------------------------------------------------------------------------
void Chart::setup_date_range() {
    auto list = StringVector() = {"Day", "Weekday", "Friday", "Sunday", "Month", "Hour", "Minute", "Second"};
    auto sel  = dlg::choice("Select Date Range Type", list, static_cast<int>(_date_range), top_window());

    if (sel == -1) {
        return;
    }

    _date_range = static_cast<ChartData::RANGE>(sel);
    init(true);
}

//------------------------------------------------------------------------------
void Chart::setup_delete_lines() {
    if (_area == nullptr || _area->size() == 0) {
        return;
    }

    auto list_labels = dlg::check("Delete Lines", _area->label_array(ChartArea::LABELTYPE::OFF), top_window());

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

    init(true);
}

//------------------------------------------------------------------------------
void Chart::setup_label() {
    auto l = fl_input(40, "Enter label", _label.c_str());

    if (l != nullptr) {
        _label = l;
        init(false);
    }
}

//------------------------------------------------------------------------------
void Chart::setup_line() {
    if (_area == nullptr || _area->selected_line() == nullptr) {
        return;
    }

    auto line = const_cast<ChartLine*>(_area->selected_line());
    auto ok   = ChartLineSetup(top_window(), *line).run();

    if (ok == false) {
        return;
    }

    init(false);
}

//------------------------------------------------------------------------------
void Chart::setup_move_lines() {
    if (_area == nullptr || _area->size() == 0) {
        return;
    }

    auto list_labels = dlg::check("Move Lines", _area->label_array(ChartArea::LABELTYPE::OFF), top_window());

    if (list_labels.size() == 0) {
        return;
    }

    auto list_areas = StringVector() = {"Area 1", "Area 2", "Area 3", "Area 4", "Area 5"};
    auto area       = dlg::choice("Select Area", list_areas, 0, top_window());

    if (area < 0 || area > static_cast<int>(ChartArea::NUM::LAST)) {
        return;
    }

    for (size_t f = 0, e = 0; f < list_labels.size(); f++, e++) {
        if (list_labels[f][0] == '1') {
            if (_move_or_delete_line(_area, e, true, static_cast<ChartArea::NUM>(area)) == true) {
                e--;
            }
        }
    }

    init(true);
}

//------------------------------------------------------------------------------
void Chart::setup_show_or_hide_lines() {
    if (_area == nullptr || _area->size() == 0) {
        return;
    }

    auto list = _area->label_array(ChartArea::LABELTYPE::VISIBLE);
    list = dlg::check("Show Or Hide Lines", list, top_window());

    if (list.size() == 0) {
        return;
    }

    for (size_t f = 0; f < list.size(); f++) {
        _area->set_visible(f, list[f][0] == '1');
    }

    init(true);
}

//------------------------------------------------------------------------------
void Chart::setup_ywidth() {
    auto width = static_cast<double>(_margin);

    if (dlg::slider("Set Y Label Width", Chart::MIN_MARGIN, Chart::MAX_MARGIN, width, 1, top_window()) == false) {
        return;
    }

    set_margin(width);
    init(false);
}

//------------------------------------------------------------------------------
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

    menu::set_item(_menu, _CHART_SHOW_HLINES, _view.horizontal);
    menu::set_item(_menu, _CHART_SHOW_LABELS, _view.labels);
    menu::set_item(_menu, _CHART_SHOW_VLINES, _view.vertical);

    if (_area != nullptr) {
        menu::enable_item(_menu, _CHART_ADD_CSV, true);
        menu::enable_item(_menu, _CHART_SETUP_MAX_CLAMP, true);
        menu::enable_item(_menu, _CHART_SETUP_MIN_CLAMP, true);

        if (_area->size() > 0) {
            menu::enable_item(_menu, _CHART_SETUP_DELETE, true);
            menu::enable_item(_menu, _CHART_SETUP_MOVE, true);
            menu::enable_item(_menu, _CHART_SETUP_SHOW, true);
        }

        if (_area->selected_line() != nullptr && _area->selected_line()->size() > 0) {
            menu::enable_item(_menu, _CHART_SETUP_LINE, true);
            menu::enable_item(_menu, _CHART_ADD_LINE, true);
            menu::enable_item(_menu, _CHART_SAVE_CSV, true);
#ifdef DEBUG
            menu::enable_item(_menu, _CHART_DEBUG_LINE, true);
#endif
        }
    }

    _menu->popup();
}

//------------------------------------------------------------------------------
void Chart::update_pref() {
    _menu->textfont(flw::PREF_FONT);
    _menu->textsize(flw::PREF_FONTSIZE);
}

} // flw

// MKALGAM_OFF
