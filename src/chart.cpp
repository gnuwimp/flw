/**
* @file
* @brief Chart widget.
*
* @author gnuwimp@gmail.com
* @copyright Released under the GNU General Public License v3.0
*/

#include "chart.h"
#include "flw.h"
#include "file.h"
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
namespace chart {

/*
 *                 _            _
 *                (_)          | |
 *      _ __  _ __ ___   ____ _| |_ ___
 *     | '_ \| '__| \ \ / / _` | __/ _ \
 *     | |_) | |  | |\ V / (_| | ||  __/
 *     | .__/|_|  |_| \_/ \__,_|\__\___|
 *     | |
 *     |_|
 */

#define _FLW_CHART_CB(X)        [](Fl_Widget*, void* o) { static_cast<Chart*>(o)->X; }, this
//#define _FLW_CHART_DEBUG(X) { X; }
#define _FLW_CHART_DEBUG(X)
#define _FLW_CHART_CLIP(X) { X; }
//#define _FLW_CHART_CLIP(X)

static const char* const _LABEL_ADD_CSV         = "Add line from CSV file...";
static const char* const _LABEL_ADD_LINE        = "Create line...";
static const char* const _LABEL_CLEAR           = "Clear chart";
static const char* const _LABEL_LOAD_JSON       = "Load chart from JSON...";
static const char* const _LABEL_PRINT           = "Print to PostScript file...";
static const char* const _LABEL_SAVE_CSV        = "Save line to CSV...";
static const char* const _LABEL_SAVE_JSON       = "Save chart to JSON...";
static const char* const _LABEL_SAVE_PNG        = "Save to png file...";
static const char* const _LABEL_SETUP_AREA      = "Number of areas...";
static const char* const _LABEL_SETUP_DELETE    = "Delete lines...";
static const char* const _LABEL_SETUP_LABEL     = "Label...";
static const char* const _LABEL_SETUP_LINE      = "Line properties...";
static const char* const _LABEL_SETUP_MAX_CLAMP = "Set max clamp...";
static const char* const _LABEL_SETUP_MIN_CLAMP = "Set min clamp...";
static const char* const _LABEL_SETUP_MOVE      = "Move lines...";
static const char* const _LABEL_SETUP_RANGE     = "Date range...";
static const char* const _LABEL_SETUP_SHOW      = "Show or hide lines...";
static const char* const _LABEL_SHOW_HLINES     = "Show horizontal lines";
static const char* const _LABEL_SHOW_LABELS     = "Show line labels";
static const char* const _LABEL_SHOW_VLINES     = "Show vertical lines";
static const int         _MIN_MARGIN            =  3;
static const int         _MIN_AREA_SIZE         = 10;
static const int         _TICK_SIZE             =  4;
static const std::string _LABEL_SYMBOL          = "@-> ";

#ifdef DEBUG
static const char* const _LABEL_DEBUG           = "Debug chart";
static const char* const _LABEL_DEBUG_LINE      = "Print visible values";
#endif

/*
 *           _      _             _____      _
 *          | |    (_)           / ____|    | |
 *          | |     _ _ __   ___| (___   ___| |_ _   _ _ __
 *          | |    | | '_ \ / _ \\___ \ / _ \ __| | | | '_ \
 *          | |____| | | | |  __/____) |  __/ |_| |_| | |_) |
 *          |______|_|_| |_|\___|_____/ \___|\__|\__,_| .__/
 *      ______                                        | |
 *     |______|                                       |_|
 */

/** @brief Chart line settings dialog.
*
* @private
*/
class _LineSetup : public Fl_Double_Window {
public:
    Line&                       _line;
    Fl_Button*                  _cancel;
    Fl_Button*                  _close;
    Fl_Button*                  _color;
    Fl_Choice*                  _align;
    Fl_Choice*                  _type;
    Fl_Hor_Slider*              _width;
    Fl_Input*                   _label;
    GridGroup*                  _grid;
    bool                        _ret;
    bool                        _run;

public:
    /** @brief Create dialog but don't show it.
    *
    */
    _LineSetup(Fl_Window* parent, Line& line) :
    Fl_Double_Window(0, 0, 10, 10, "Line Properties"),
    _line(line) {
        end();

        _align  = new Fl_Choice(0, 0, 0, 0, "Align");
        _cancel = new Fl_Button(0, 0, 0, 0, "&Cancel");
        _close  = new Fl_Return_Button(0, 0, 0, 0, "&Ok");
        _color  = new Fl_Button(0, 0, 0, 0, "Color");
        _grid   = new GridGroup(0, 0, w(), h());
        _label  = new Fl_Input(0, 0, 0, 0, "Label");
        _type   = new Fl_Choice(0, 0, 0, 0, "Type");
        _width  = new Fl_Hor_Slider(0, 0, 0, 0);
        _line   = line;
        _ret    = false;
        _run    = false;

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
        _cancel->callback(_LineSetup::Callback, this);
        _close->callback(_LineSetup::Callback, this);
        _color->align(FL_ALIGN_LEFT);
        _color->callback(_LineSetup::Callback, this);
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
        _width->callback(_LineSetup::Callback, this);
        _width->precision(0);
        _width->range(1, chart::MAX_LINE_WIDTH);
        _width->value(_line.width());
        _width->tooltip("Max line width.");

        resizable(_grid);
        util::labelfont(this);
        callback(_LineSetup::Callback, this);
        size(30 * flw::PREF_FONTSIZE, 16 * flw::PREF_FONTSIZE);
        size_range(30 * flw::PREF_FONTSIZE, 16 * flw::PREF_FONTSIZE);
        set_modal();
        util::center_window(this, parent);
        _grid->do_layout();
        _LineSetup::Callback(_width, this);
        update_widgets();
    }

    /** @brief Callback for all widgets.
    *
    */
    static void Callback(Fl_Widget* w, void* o) {
        auto self = static_cast<_LineSetup*>(o);

        if (w == self) {
        }
        else if (w == self->_cancel) {
            self->_run = false;
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
            self->_run = false;
            self->update_line();
            self->hide();
        }
    }

    /** @brief Show dialog.
    *
    * @return True if close has been pressed.
    */
    bool run() {
        _run = true;
        show();

        while (_run == true) {
            Fl::wait();
            Fl::flush();
        }

        return _ret;
    }

    /** @brief Update input chart line.
    *
    */
    void update_line() {
        _line.set_label(_label->value());
        _line.set_width(_width->value());
        _line.set_color(_color->color());

        if (_type->value() == 0)      _line.set_type(LineType::LINE);
        else if (_type->value() == 1) _line.set_type(LineType::LINE_DOT);
        else if (_type->value() == 2) _line.set_type(LineType::BAR);
        else if (_type->value() == 3) _line.set_type(LineType::BAR_CLAMP);
        else if (_type->value() == 4) _line.set_type(LineType::BAR_HLC);
        else if (_type->value() == 5) _line.set_type(LineType::HORIZONTAL);
        else if (_type->value() == 6) _line.set_type(LineType::EXPAND_HORIZONTAL_ALL);
        else if (_type->value() == 7) _line.set_type(LineType::EXPAND_HORIZONTAL_FIRST);

        if (_align->value() == 0) _line.set_align(FL_ALIGN_LEFT);
        else                      _line.set_align(FL_ALIGN_RIGHT);
    }

    /** @brief Update widgets with chart line data.
    *
    */
    void update_widgets() {
        _label->value(_line.label().c_str());
        _color->color(_line.color());
        _LineSetup::Callback(_width, this);

        if (_line.type() == LineType::LINE)                         _type->value(0);
        else if (_line.type() == LineType::LINE_DOT)                _type->value(1);
        else if (_line.type() == LineType::BAR)                     _type->value(2);
        else if (_line.type() == LineType::BAR_CLAMP)               _type->value(3);
        else if (_line.type() == LineType::BAR_HLC)                 _type->value(4);
        else if (_line.type() == LineType::HORIZONTAL)              _type->value(5);
        else if (_line.type() == LineType::EXPAND_HORIZONTAL_ALL)   _type->value(6);
        else if (_line.type() == LineType::EXPAND_HORIZONTAL_FIRST) _type->value(7);

        if (_line.align() == FL_ALIGN_LEFT) _align->value(0);
        else                                _align->value(1);
    }
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

/** @brief Add one chart line.
*
* Max Area::MAX_LINES lines.
*
* @param[in] chart_line  Line object.
*
* @return True if added. false if there are to many already.
*/
bool Area::add_line(const Line& chart_line) {
    if (_lines.size() >= Area::MAX_LINES) {
        return false;
    }

    _lines.push_back(chart_line);
    Fl::redraw();

    return true;
}

/** @brief Return max clamp value.
*
* @return Value or nullopt.
*/
std::optional<double> Area::clamp_max() const {
    if (std::isfinite(_clamp_max) == true) {
        return _clamp_max;
    }

    return std::nullopt;
}

/** @brief Return min clamp value.
*
* @return Value or nullopt.
*/
std::optional<double> Area::clamp_min() const {
    if (std::isfinite(_clamp_min) == true) {
        return _clamp_min;
    }

    return std::nullopt;
}

/** @brief Print debug info to terminal.
*/
void Area::debug() const {
#ifdef DEBUG
    printf("\t--------------------------------------\n");
    printf("\tArea: %d\n", static_cast<int>(_area));

    if (_percent >= chart::_MIN_AREA_SIZE) {
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

/** @brief Delete chart line
*
* @param[in] index  Line index.
*/
void Area::delete_line(size_t index) {
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

/** @brief Reset all data and values to default value.
*
*/
void Area::reset() {
    _clamp_max = INFINITY;
    _clamp_min = INFINITY;
    _percent   = 0;
    _rect      = Fl_Rect();
    _selected  = 0;

    _left.reset();
    _right.reset();
    _lines.clear();
}

/** @brief Return current selected line.
*
* @return Line or NULL.
*/
Line* Area::selected_line() {
    if (_selected >= _lines.size()) {
        return nullptr;
    }

    return &_lines[_selected];
}

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

/** @brief Create empty point.
*
*/
Point::Point() {
    high = low = close = 0.0;
}

/** @brief Create point.
*
* @param[in] date   Valid date.
* @param[in] value  Y value for high, low and close.
*/
Point::Point(const std::string& date, double value) {
    auto valid_date = gnu::Date(date.c_str());

    if (std::isfinite(value) == true &&
        fabs(value) < chart::MAX_VALUE &&
        valid_date.is_invalid() == false) {

        this->date = valid_date.format(gnu::Date::Format::ISO_TIME);
        this->high = this->low = this->close = value;
    }
    else {
        high = low = close = 0.0;
    }
}

/** @brief Create point.
*
* Values are moved if they are out of size order.\n
* Invalid date sets all values to 0.0.\n
*
* @param[in] date   Valid date.
* @param[in] high   High Y value.
* @param[in] low    Low Y value.
* @param[in] close  Close Y value.
*/
Point::Point(const std::string& date, double high, double low, double close) {
    auto valid_date = gnu::Date(date.c_str());

    if (std::isfinite(high) == true &&
        std::isfinite(low) == true &&
        std::isfinite(close) == true &&
        fabs(high) < chart::MAX_VALUE &&
        fabs(low) < chart::MAX_VALUE &&
        fabs(close) < chart::MAX_VALUE &&
        valid_date.is_invalid() == false) {

        if (low > high) {
            auto tmp = low;
            low = high;
            high = tmp;
        }

        if (close > high) {
            auto tmp = high;
            high = close;
            close = tmp;
        }

        if (close < low) {
            auto tmp = low;
            low = close;
            close = tmp;
        }

        this->date  = valid_date.format(gnu::Date::Format::ISO_TIME);
        this->high  = high;
        this->low   = low;
        this->close = close;
    }
    else {
        this->high = this->low = this->close = 0.0;
    }
}

/** @brief Convert data serie using ATR algorithm.
*
* @param[in] in    Vector with Point objects.
* @param[in] days  Number of days.
*
* @return Result vector with Point objects.
*/
PointVector Point::ATR(const PointVector& in, size_t days) {
    PointVector res;

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
                    res.push_back(Point(data.date, prev_range));
                }
                else if (f > days) {
                    prev_range = ((prev_range * days) + ra) / (days + 1);
                    res.push_back(Point(data.date, prev_range));
                }
            }

            prev_close = data.close;
            f++;
        }
    }

    return res;
}

/** @brief Do a binary search for a date/time.
*
* @param[in] in   Vector with Point objects.
* @param[in] key  Date to search for.
*
* @return Result vector with Point objects.
*/
size_t Point::BinarySearch(const PointVector& in, const Point& key) {
    auto it = std::lower_bound(in.begin(), in.end(), key);

    if (it == in.end() || *it != key) {
        return static_cast<size_t>(-1);
    }
    else {
        return std::distance(in.begin(), it);
    }
}

/** @brief Create date serie.
*
* @param[in] start_date  Start date.
* @param[in] stop_date   Stop date.
* @param[in] range       Date range.
* @param[in] block       Optional list of dates to remove from reference date serie.
*
* @return Result vector with Point objects.
*/
PointVector Point::DateSerie(const std::string& start_date, const std::string& stop_date, DateRange range, const PointVector& block) {
    auto       month   = -1;
    auto       current = gnu::Date(start_date.c_str());
    auto const stop    = gnu::Date(stop_date.c_str());
    auto       res     = PointVector();

    if (range == DateRange::FRIDAY) {
        while (current.weekday() != gnu::Date::Day::FRIDAY)
            current.add_days(1);
    }
    else if (range == DateRange::SUNDAY) {
        while (current.weekday() != gnu::Date::Day::SUNDAY) {
            current.add_days(1);
        }
    }

    while (current <= stop) {
        gnu::Date date(1, 1, 1);

        if (range == DateRange::DAY) {
            date = gnu::Date(current);
            current.add_days(1);
        }
        else if (range == DateRange::WEEKDAY) {
            gnu::Date::Day weekday = current.weekday();

            if (weekday >= gnu::Date::Day::MONDAY && weekday <= gnu::Date::Day::FRIDAY) {
                date = gnu::Date(current);
            }

            current.add_days(1);
        }
        else if (range == DateRange::FRIDAY || range == DateRange::SUNDAY) {
            date = gnu::Date(current);
            current.add_days(7);
        }
        else if (range == DateRange::MONTH) {
            if (current.month() != month) {
                current.set_day_to_last_in_month();
                date = gnu::Date(current);
                month = current.month();
            }

            current.add_months(1);
        }
        else if (range == DateRange::HOUR) {
            date = gnu::Date(current);
            current.add_seconds(3600);
        }
        else if (range == DateRange::MIN) {
            date = gnu::Date(current);
            current.add_seconds(60);
        }
        else if (range == DateRange::SEC) {
            date = gnu::Date(current);
            current.add_seconds(1);
        }

        if (date.year() > 1) {
            Point price(date.format(gnu::Date::Format::ISO_TIME_LONG));

            if (block.size() == 0 || std::binary_search(block.begin(), block.end(), price) == false) {
                res.push_back(price);
            }
        }
    }

    return res;
}

/** @brief Convert data serie to monthly data.
*
* @param[in] in   Vector with Point objects.
* @param[in] sum  True to add values for a month, false to use highest/lowest and last close data.
*
* @return Result vector with Point objects.
*/
PointVector Point::DayToMonth(const PointVector& in, bool sum) {
    size_t          f = 0;
    PointVector res;
    Point       current;
    gnu::Date       stop;
    gnu::Date       pdate;

    for (const auto& data : in) {
        if (f == 0) {
            current = data;
            stop = gnu::Date(current.date.c_str());
            stop.set_day_to_last_in_month();
        }
        else {
            pdate = gnu::Date(data.date.c_str());

            if (stop < pdate) {
                current.date = stop.format(gnu::Date::Format::ISO_TIME);
                res.push_back(current);
                current = data;
                stop = gnu::Date(current.date.c_str());
                stop.set_day_to_last_in_month();
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
            auto s = stop.format(gnu::Date::Format::ISO_TIME);
            stop.set_day_to_last_in_month();
            current.date = s;
            res.push_back(current);
        }

        f++;
    }

    return res;
}

/** @brief Convert data serie to weekly data.
*
* @param[in] in       Vector with Point objects.
* @param[in] weekday  What day to use in week.
* @param[in] sum      True to add values for a week, false to use highest/lowest and last close data.
*
* @return Result vector with Point objects.
*/
PointVector Point::DayToWeek(const PointVector& in, gnu::Date::Day weekday, bool sum) {
    size_t          f = 0;
    PointVector res;
    Point       current;
    gnu::Date       stop;
    gnu::Date       pdate;

    for (const auto& data : in) {
        if (f == 0) {
            stop = gnu::Date(data.date.c_str());

            if (weekday > stop.weekday()) {
                stop.set_weekday(weekday);
            }
            else if (weekday < stop.weekday()) {
                stop.set_weekday(weekday);
                stop.add_days(7);
            }

            current = data;
        }
        else {
            pdate = gnu::Date(data.date.c_str());

            if (stop < pdate) {
                current.date = stop.format(gnu::Date::Format::ISO_TIME);
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

            current.date = stop.format(gnu::Date::Format::ISO_TIME);
        }

        if (f + 1 == in.size()) {
            current.date = stop.format(gnu::Date::Format::ISO_TIME);
            res.push_back(current);
        }

        f++;
    }

    return res;
}

/** @brief Print value.
*
*/
void Point::debug() const {
#ifdef DEBUG
    printf("%s, %20.8f, %20.8f, %20.8f\n", date.c_str(), high, low, close);
#endif
}

/** @brief Print values.
*
* @param[in] in  Vector with Point objects.
*/
void Point::Debug(const PointVector& in) {
#ifdef DEBUG
    printf("\nPointVector(%u)\n", static_cast<unsigned>(in.size()));

    for (const auto& data : in) {
        data.debug();
    }

    fflush(stdout);
#else
    (void) in;
#endif
}

/** @brief Convert data serie using exponential moving average algorithm.
*
* @param[in] in    Vector with Point objects.
* @param[in] days  Number of days.
*
* @return Result vector with Point objects.
*/
PointVector Point::ExponentialMovingAverage(const PointVector& in, size_t days) {
    PointVector res;

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
                res.push_back(Point(data.date, prev));
            }
            else {
                prev = ((data.close - prev) * multi) + prev;
                res.push_back(Point(data.date, prev));
            }

            f++;
        }
    }

    return res;
}

/** @brief Create a data serie with fixed Y value.
*
* @param[in] in     Vector with Point objects (only date is used).
* @param[in] value  Y values.
*
* @return Result vector with Point objects.
*/
PointVector Point::Fixed(const PointVector& in, double value) {
    PointVector res;

    for (const auto& data : in) {
        res.push_back(Point(data.date, value));
    }

    return res;
}

/** @brief Load data from csv file.
*
* @param[in] filename  Valid text file name.
* @param[in] sep       Column separator.
*
* @return Result vector with Point objects.
*/
PointVector Point::LoadCSV(const std::string& filename, const std::string& sep) {
    auto buf = gnu::file::read(filename);

    if (buf.size() < 10) {
        return PointVector();
    }

    std::string  str   = buf.c_str();
    StringVector lines = util::split_string(str, "\n");
    PointVector  res;

    for (const auto& l : lines) {
        StringVector line = util::split_string(l, sep);
        Point    data;

        if (line.size() == 2) {
            data = Point(line[0], util::to_double(line[1]));
        }
        else if (line.size() > 3) {
            data = Point(line[0], util::to_double(line[1]), util::to_double(line[2]), util::to_double(line[3]));
        }

        if (data.date != "") {
            res.push_back(data);
        }
    }

    return res;
}

/** @brief Modify chart data.
*
* @param[in] in      Vector with Point objects.
* @param[in] modify  Type of modify.
* @param[in] value   Value to use.
*
* @return Result vector with Point objects.
*/
PointVector Point::Modify(const PointVector& in, Modifier modify, double value) {
    PointVector res;

    if (fabs(value) < chart::MIN_VALUE) {
        return res;
    }

    for (const auto& data : in) {
        switch (modify) {
            case Modifier::ADDITION:
                res.push_back(Point(data.date, data.high + value, data.low + value, data.close + value));
                break;
            case Modifier::DIVISION:
                res.push_back(Point(data.date, data.high / value, data.low / value, data.close / value));
                break;
            case Modifier::MULTIPLICATION:
                res.push_back(Point(data.date, data.high * value, data.low * value, data.close * value));
                break;
            case Modifier::SUBTRACTION:
                res.push_back(Point(data.date, data.high - value, data.low - value, data.close - value));
                break;
        }
    }

    return res;
}

/** @brief Convert data serie using momentum algorithm.
*
* @param[in] in    Vector with Point objects.
* @param[in] days  Number of days.
*
* @return Result vector with Point objects.
*/
PointVector Point::Momentum(const PointVector& in, size_t days) {
    size_t          start = days - 1;
    PointVector res;

    if (days > 1 && days < in.size()) {
        for (auto f = start; f < in.size(); f++) {
            const auto& data1 = in[f];
            const auto& data2 = in[f - start];

            res.push_back(Point(data1.date, data1.close - data2.close));
        }

    }

    return res;
}

/** @brief Convert data serie using moving average algorithm.
*
* @param[in] in    Vector with Point objects.
* @param[in] days  Number of days.
*
* @return Result vector with Point objects.
*/
PointVector Point::MovingAverage(const PointVector& in, size_t days) {
    PointVector res;

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
                res.push_back(Point(data.date, sum / days));
            }
            else { //  Remove oldest data in range and add current to sum.
                tmp[count - 1] = data.close;
                sum -= tmp[count - (days + 1)];
                sum += data.close;
                res.push_back(Point(data.date, sum / days));
            }
        }

        delete []tmp;
    }

    return res;
}

/** @brief Conmvert range value to a string.
*
* @param[in] range  Range value.
*
* @return String value, one of ("DAY", "WEEKDAY", "FRIDAY", "SUNDAY", "MONTH", "HOUR", "MIN", "SEC").
*/
std::string Point::RangeToString(DateRange range) {
    static const std::string NAMES[] = { "DAY", "WEEKDAY", "FRIDAY", "SUNDAY", "MONTH", "HOUR", "MIN", "SEC", "", };
    return NAMES[static_cast<unsigned>(range)];
}

/** @brief Convert data serie using rsi algorithm.
*
* @param[in] in    Vector with Point objects.
* @param[in] days  Number of days.
*
* @return Result vector with Point objects.
*/
PointVector Point::RSI(const PointVector& in, size_t days) {
    PointVector res;

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
                res.push_back(Point(data.date, 100 - (100 / (1 + (avg_gain / avg_loss)))));
            }
            else if (f > days) {
                avg_gain = ((avg_gain * (days - 1)) + ((diff > 0) ? fabs(diff) : 0)) / days;
                avg_loss = ((avg_loss * (days - 1)) + ((diff < 0) ? fabs(diff) : 0)) / days;
                res.push_back(Point(data.date, 100 - (100 / (1 + (avg_gain / avg_loss)))));
            }
        }
    }

    return res;
}

/** @brief Save points to an csv file.
*
* @param[in] in        Vector with Point objects.
* @param[in] filename  Result file name.
* @param[in] sep       Column separator.
*
* @return True if ok.
*/
bool Point::SaveCSV(const PointVector& in, const std::string& filename, const std::string& sep) {
    std::string csv;

    csv.reserve(in.size() * 40 + 256);

    for (const auto& data : in) {
        char buffer[256];
        snprintf(buffer, 256, "%s%s%s%s%s%s%s\n", data.date.c_str(), sep.c_str(), gnu::json::format_number(data.high).c_str(), sep.c_str(), gnu::json::format_number(data.low).c_str(), sep.c_str(), gnu::json::format_number(data.close).c_str());
        csv += buffer;
    }

    return gnu::file::write(filename, csv.c_str(), csv.size());
}

/** @brief Convert data serie using standard deviation.
*
* @param[in] in    Vector with Point objects.
* @param[in] days  Number of days.
*
* @return Result vector with Point objects.
*/
PointVector Point::StdDev(const PointVector& in, size_t days) {
    PointVector res;

    if (days > 1 && days < in.size()) {
        size_t    count = 0;
        double    sum   = 0.0;
        Point data2;

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
                res.push_back(Point(data2.date, dev));
            }
        }
    }

    return res;
}

/** @brief Convert data serie using stochastics algorithm.
*
* @param[in] in    Vector with Point objects.
* @param[in] days  Number of days.
*
* @return Result vector with Point objects.
*/
PointVector Point::Stochastics(const PointVector& in, size_t days) {
    double          high = 0.0;
    double          low  = 0.0;
    PointVector res;

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

                if (diff2 > chart::MIN_VALUE) {
                    double kval  = 100.0 * (diff1 / diff2);
                    res.push_back(Point(data.date, kval));
                }
            }
        }
    }

    return res;
}

/** @brief Convert string to range.
*
* @param[in] range  Range string.
*
* @return Range value or DateRange::DAY for invalid input string.
*/
DateRange  Point::StringToRange(const std::string& range) {
    if (range == "WEEKDAY")     return DateRange::WEEKDAY;
    else if (range == "FRIDAY") return DateRange::FRIDAY;
    else if (range == "SUNDAY") return DateRange::SUNDAY;
    else if (range == "MONTH")  return DateRange::MONTH;
    else if (range == "HOUR")   return DateRange::HOUR;
    else if (range == "MIN")    return DateRange::MIN;
    else if (range == "SEC")    return DateRange::SEC;
    else                        return DateRange::DAY;
}

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

/** @brief Create a chart line.
*
* @param[in] data   Vector with Point objects.
* @param[in] label  Line label.
* @param[in] type   Line type.
*/
Line::Line(const PointVector& data, const std::string& label, LineType type) {
    reset();
    set_data(data);
    set_label(label);
    set_type(type);
}

/** @brief Print debug info to stdout.
*
* @param[in] num  Line index.
*/
void Line::debug(size_t num) const {
#ifdef DEBUG
    printf("\t\t---------------------------------------------\n");
    printf("\t\tLine: %u\n", static_cast<unsigned>(num));
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

/** @brief Clear all data.
*
*/
void Line::reset() {
    _data.clear();

    _align   = FL_ALIGN_LEFT;
    _color   = FL_FOREGROUND_COLOR;
    _label   = "";
    _rect    = Fl_Rect();
    _type    = LineType::LINE;
    _visible = true;
    _width   = 1;
}

/** @brief Set line type from string.
*
* If input name is invalid it will set type to LineType::LINE.
*
* @param[in] val  Line type name, one of ("LINE", "LINE_DOT", "BAR", "BAR_CLAMP", "BAR_HLC", "HORIZONTAL", "EXPAND_VERTICAL", "EXPAND_HORIZONTAL_ALL", "EXPAND_HORIZONTAL_FIRST").
*
* @return Input chart line.
*/
Line&  Line::set_type_from_string(const std::string&  val) {
    if (val == "LINE_DOT")                     _type = LineType::LINE_DOT;
    else if (val == "BAR")                     _type = LineType::BAR;
    else if (val == "BAR_CLAMP")               _type = LineType::BAR_CLAMP;
    else if (val == "BAR_HLC")                 _type = LineType::BAR_HLC;
    else if (val == "HORIZONTAL")              _type = LineType::HORIZONTAL;
    else if (val == "EXPAND_VERTICAL")         _type = LineType::EXPAND_VERTICAL;
    else if (val == "EXPAND_HORIZONTAL_ALL")   _type = LineType::EXPAND_HORIZONTAL_ALL;
    else if (val == "EXPAND_HORIZONTAL_FIRST") _type = LineType::EXPAND_HORIZONTAL_FIRST;
    else                                       _type = LineType::LINE;

    return *this;
}

/** @brief Return line type as a string.
*
* @return Line type name, one of ("LINE", "LINE_DOT", "BAR", "BAR_CLAMP", "BAR_HLC", "HORIZONTAL", "EXPAND_VERTICAL", "EXPAND_HORIZONTAL_ALL", "EXPAND_HORIZONTAL_FIRST").
*/
std::string Line::type_to_string() const {
    static const std::string NAMES[] = { "LINE", "LINE_DOT", "BAR", "BAR_CLAMP", "BAR_HLC", "HORIZONTAL", "EXPAND_VERTICAL", "EXPAND_HORIZONTAL_ALL", "EXPAND_HORIZONTAL_FIRST", "", };
    return NAMES[static_cast<unsigned>(_type)];
}

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

/** @brief Create a chart scale.
*
*/
Scale::Scale() {
    reset();
}

/** @brief Calculate margins for the scale.
*
* It tries to use size of min and max values.
*
* @return Width in pixels.
*/
int Scale::calc_margin() {
    if (diff() < chart::MIN_VALUE) {
        return 0;
    }

    const double fr  = util::count_decimals(tick());
    std::string  min = util::format_double(_min, fr, '\'');
    std::string  max = util::format_double(_max, fr, '\'');

    return (min.length() > max.length()) ? min.length() : max.length();
}

/** @brief Calculate y tick size.
*
* It tries to create an even number.\n
* Sets tick size (value between every y tick).\n
* And y value for one pixel.\n
*
* @param[in] height  Chart area height in pixels.
*/
void Scale::calc_tick(int height) {
    const double RANGE  = diff();
    int          kludge = 0;

    _tick  = 0.0;
    _pixel = 0.0;

    if (std::isfinite(_min) == true && std::isfinite(_max) == true && _min < _max && RANGE > chart::MIN_VALUE) {
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

        if (_tick >= chart::MIN_VALUE / 10.0 && kludge < 100) {
            _pixel = height / diff();
        }
        else {
            _tick = 0.0;
        }
    }
}

/** @brief Print debug info to stdout.
*
* @param[in] name  Scale name.
*/
void Scale::debug(const char* name) const {
#ifdef DEBUG
    printf("\t\t---------------------------------------------\n");
    printf("\t\tScale: %s\n", name);
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

/** @brief Calculate difference between min and max.
*
* @return Diff value or 0.0 for invalid values.
*/
double Scale::diff() const {
    if (std::isfinite(_min) == false || std::isfinite(_max) == false) {
        return 0.0;
    }

    return _max - _min;
}

/** @brief Increase and decrease min and max scale values.
*
*/
void Scale::fix_height() {
    if (std::isfinite(_min) == true && std::isfinite(_max) == true && _min < _max && fabs(_max - _min) < chart::MIN_VALUE) {
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

/** @brief Get max value.
*
* @return Value or std::nullopt if value is isfinite.
*/
std::optional<double> Scale::max() const {
    if (std::isfinite(_max) == true) {
        return _max;
    }

    return std::nullopt;
}

/** @brief Get min value.
*
* @return Value or std::nullopt if value is isfinite.
*/
std::optional<double> Scale::min() const {
    if (std::isfinite(_min) == true) {
        return _min;
    }

    return std::nullopt;
}

/** @brief Clear all values.
*
*/
void Scale::reset() {
    _min   = INFINITY;
    _max   = INFINITY;
    _tick  = 0.0;
    _pixel = 0.0;
}

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

/** @brief Create chart widget.
*
* @param[in] X  X pos.
* @param[in] Y  Y pos.
* @param[in] W  Width.
* @param[in] H  Height.
* @param[in] l  Label.
*/
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

    _menu->add(_LABEL_SHOW_LABELS,      0, _FLW_CHART_CB(setup_view_options()), FL_MENU_TOGGLE);
    _menu->add(_LABEL_SHOW_HLINES,      0, _FLW_CHART_CB(setup_view_options()), FL_MENU_TOGGLE);
    _menu->add(_LABEL_SHOW_VLINES,      0, _FLW_CHART_CB(setup_view_options()), FL_MENU_TOGGLE | FL_MENU_DIVIDER);
    _menu->add(_LABEL_CLEAR,            0, _FLW_CHART_CB(reset()));
    _menu->add(_LABEL_SETUP_LABEL,      0, _FLW_CHART_CB(setup_label()));
    _menu->add(_LABEL_SETUP_AREA,       0, _FLW_CHART_CB(setup_area()));
    _menu->add(_LABEL_SETUP_RANGE,      0, _FLW_CHART_CB(setup_date_range()), FL_MENU_DIVIDER);
    _menu->add(_LABEL_SETUP_LINE,       0, _FLW_CHART_CB(setup_line()));
    _menu->add(_LABEL_SETUP_MIN_CLAMP,  0, _FLW_CHART_CB(setup_clamp(true)));
    _menu->add(_LABEL_SETUP_MAX_CLAMP,  0, _FLW_CHART_CB(setup_clamp(false)));
    _menu->add(_LABEL_SETUP_SHOW,       0, _FLW_CHART_CB(setup_show_or_hide_lines()));
    _menu->add(_LABEL_SETUP_MOVE,       0, _FLW_CHART_CB(setup_move_lines()), FL_MENU_DIVIDER);
    _menu->add(_LABEL_SETUP_DELETE,     0, _FLW_CHART_CB(setup_delete_lines()));
    _menu->add(_LABEL_ADD_LINE,         0, _FLW_CHART_CB(setup_create_line()));
    _menu->add(_LABEL_SAVE_CSV,         0, _FLW_CHART_CB(save_line_to_csv()), FL_MENU_DIVIDER);
    _menu->add(_LABEL_ADD_CSV,          0, _FLW_CHART_CB(load_line_from_csv()));
    _menu->add(_LABEL_LOAD_JSON,        0, _FLW_CHART_CB(load_json()));
    _menu->add(_LABEL_SAVE_JSON,        0, _FLW_CHART_CB(save_json()), FL_MENU_DIVIDER);
    _menu->add(_LABEL_PRINT,            0, _FLW_CHART_CB(print_to_postscript()));
    _menu->add(_LABEL_SAVE_PNG,         0, _FLW_CHART_CB(save_png()));
#ifdef DEBUG
    _menu->add(_LABEL_SAVE_PNG,         0, _FLW_CHART_CB(save_png()), FL_MENU_DIVIDER);
    _menu->add(_LABEL_DEBUG,            0, _FLW_CHART_CB(debug()));
    _menu->add(_LABEL_DEBUG_LINE,       0, _FLW_CHART_CB(debug_line()));
#else
    _menu->add(_LABEL_SAVE_PNG,         0, _FLW_CHART_CB(save_png()));
#endif

    _menu->type(Fl_Menu_Button::POPUP3);

    _areas.push_back(Area(AreaNum::ONE));
    _areas.push_back(Area(AreaNum::TWO));
    _areas.push_back(Area(AreaNum::THREE));
    _areas.push_back(Area(AreaNum::FOUR));
    _areas.push_back(Area(AreaNum::FIVE));

    _disable_menu = false;

    reset();
    update_pref();
}

/** @brief Calculate area height for all areas.
*
*/
void Chart::_calc_area_height() {
    auto last   = 0;
    auto addh   = 0;
    auto height = 0;

    _top_space    = (_label == "") ? _CH : _CH * 3;
    _bottom_space = _CH * 3 + Fl::scrollbar_size();
    height        = h() - (_bottom_space + _top_space);

    for (size_t f = 1; f <= static_cast<size_t>(AreaNum::LAST); f++) {
        const auto& area = _areas[f];

        if (area.percent() >= chart::_MIN_AREA_SIZE) {
            height -= flw::PREF_FIXED_FONTSIZE;
        }
    }

    _areas[0].rect().y(_top_space);
    _areas[0].rect().h(static_cast<int>((_areas[0].percent() / 100.0) * height));

    for (size_t f = 1; f <= static_cast<size_t>(AreaNum::LAST); f++) {
        auto& prev = _areas[f - 1];
        auto& area = _areas[f];

        if (area.percent() >= chart::_MIN_AREA_SIZE) {
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

/** @brief Calculate area width, same for all areas.
*
*/
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

/** @brief Create date serie which are used to search for data points.
*
* It takes min and max date from all areas and all lines.
*/
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
        _dates = Point::DateSerie(min, max, _date_range, _block_dates);
        redraw();
    }
}

/** @brief Calculate margins on left and right side.
*
*/
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
        if (_date_range == DateRange::HOUR) {
            _margin_right += 1;
            _margin_left  += 1;
        }
        else if (_date_range == DateRange::MIN) {
            _margin_right += 2;
            _margin_left  += 2;
        }
        else if (_date_range == DateRange::SEC) {
            _margin_right += 3;
            _margin_left  += 3;
        }
    }
}

/** @brief Calc min and max y values for all chart areas.
*
* These are recalculated when data are scrolled.\n
* Clamp values does override dataset.\n
*/
void Chart::_calc_ymin_ymax() {
    for (auto& area : _areas) {
        auto min_clamp = area.clamp_min();
        auto max_clamp = area.clamp_max();

        area.left_scale().reset();
        area.right_scale().reset();

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
                const Point& date  = _dates[current];
                const size_t     index = Point::BinarySearch(line.data(), date);

                if (index != static_cast<size_t>(-1)) {
                    const Point& data = line.data()[index];

                    if (line.type_has_high_and_low() == true) {
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

/** @brief Calculate y scales for left and right sides and for all chjart areas.
*
*/
void Chart::_calc_yscale() {
    for (auto& area : _areas) {
        area.left_scale().calc_tick(area.rect().h());
        area.right_scale().calc_tick(area.rect().h());
    }
}

/** @brief Callback for printing to postscript.
*
* @param[in] data  Chart widget.
* @param[in] pw    Page wisth.
* @param[in] ph    Page height.
* @param[in]       Page number, not used.
*
* @return Always false, stop printing.
*/
bool Chart::_CallbackPrinter(void* data, int pw, int ph, int) {
    auto widget = static_cast<Fl_Widget*>(data);
    auto rect   = Fl_Rect(widget);

    widget->resize(0, 0, pw, ph);
    widget->draw();
    widget->resize(rect.x(), rect.y(), rect.w(), rect.h());
    return false;
}

/** @brief Callback for horizontal scrollbar that scrolls the reference date vector.
*
* @param[in]         Sender, not used.
* @param[in] widget  Chart widget.
*/
void Chart::_CallbackScrollbar(Fl_Widget*, void* widget) {
    auto self = static_cast<Chart*>(widget);
    self->_date_start = self->_scroll->value();
    self->init();
}

/** @brief Create new chart line from selected line in active area and add it to current area.
*
* User might be asked for additional options.
*
* @param[in] formula  What algorithm to use to create the new line..
* @param[in] support  True to add horizontal support lines, not for all algorithms.
*
* @return True if a new chart line was added.
*/
bool Chart::create_line(Algorithm formula, bool support) {
    if (_area == nullptr || _area->selected_line() == nullptr) {
        fl_alert("Error: area or line has not been selected!");
        return false;
    }
    else if (_area->size() >= Area::MAX_LINES) {
        fl_alert("Error: max line count reached!");
        return false;
    }

    auto line0  = _area->selected_line();
    auto line1  = Line();
    auto line2  = Line();
    auto line3  = Line();
    auto vec1   = PointVector();
    auto vec2   = PointVector();
    auto vec3   = PointVector();
    auto label1 = std::string();
    auto label2 = std::string();
    auto label3 = std::string();
    auto type1  = LineType::LINE;
    auto type2  = LineType::LINE;
    auto type3  = LineType::LINE;

    if (formula == Algorithm::ATR) {
        auto days = util::to_long(fl_input_str(3, "Enter number of days (2 - 365)", "14"));

        if (days < 2 || days > 365) {
            return false;
        }

        vec1   = Point::ATR(line0->data(), days);
        label1 = util::format("ATR %d Days", days);
    }
    else if (formula == Algorithm::DAY_TO_WEEK) {
        auto answer = fl_choice("Would you like to use highest/lowest and last close value per week?\nOr sum values per week?", nullptr, "High/Low", "Sum");

        vec1   = Point::DayToWeek(line0->data(), gnu::Date::Day::SUNDAY, answer == 2);
        label1 = "Weekly (Sunday)";
        type1  = line0->type();
    }
    else if (formula == Algorithm::DAY_TO_MONTH) {
        auto answer = fl_choice("Would you like to use highest/lowest and last close value per month?\nOr sum values per month?", nullptr, "High/Low", "Sum");

        vec1   = Point::DayToMonth(line0->data(), answer == 2);
        label1 = "Monthly";
        type1  = line0->type();
    }
    else if (formula == Algorithm::EXP_MOVING_AVERAGE) {
        auto days = util::to_long(fl_input_str(3, "Enter number of days (2 - 365)", "14"));

        if (days < 2 || days > 365) {
            return false;
        }

        vec1   = Point::ExponentialMovingAverage(line0->data(), days);
        label1 = util::format("Exponential Moving Average %d Days", days);
    }
    else if (formula == Algorithm::FIXED) {
        auto value = util::to_double(fl_input_str(16, "Enter value", "0"));

        if (std::isinf(value) == true) {
            return false;
        }

        vec1   = Point::Fixed(line0->data(), value);
        label1 = util::format("Horizontal %f", value);
        type1  = LineType::EXPAND_HORIZONTAL_FIRST;
    }
    else if (formula == Algorithm::MODIFY) {
        auto list = StringVector() = {"Addition", "Subtraction", "Multiplication", "Division"};
        auto ans  = dlg::select_choice("Select Modification", list, 0, top_window());

        if (ans < 0 || ans > static_cast<int>(Modifier::LAST)) {
            return false;
        }

        auto modify = static_cast<Modifier>(ans);
        auto value  = util::to_double(fl_input_str(16, "Enter value", "0"));

        if (std::isinf(value) == true) {
            return false;
        }
        else if (fabs(value) < chart::MIN_VALUE) {
            fl_alert("Error: to small value for division!");
            return false;
        }

        vec1   = Point::Modify(line0->data(), modify, value);
        label1 = util::format("Modified %s", line0->label().c_str());
        type1  = line0->type();
    }
    else if (formula == Algorithm::MOMENTUM) {
        auto days = util::to_long(fl_input_str(3, "Enter number of days (2 - 365)", "14"));

        if (days < 2 || days > 365) {
            return false;
        }

        vec1   = Point::Momentum(line0->data(), days);
        label1 = util::format("Momentum %d Days", days);

        if (support == true) {
            vec2   = Point::Fixed(line0->data(), 0.0);
            label2 = "Momentum Zero";
            type2  = LineType::EXPAND_HORIZONTAL_FIRST;
        }
    }
    else if (formula == Algorithm::MOVING_AVERAGE) {
        auto days = util::to_long(fl_input_str(3, "Enter number of days (2 - 365)", "14"));

        if (days < 2 || days > 365) {
            return false;
        }

        vec1   = Point::MovingAverage(line0->data(), days);
        label1 = util::format("Moving Average %d Days", days);
    }
    else if (formula == Algorithm::RSI) {
        auto days = util::to_long(fl_input_str(3, "Enter number of days (2 - 365)", "14"));

        if (days < 2 || days > 365) {
            return false;
        }

        vec1   = Point::RSI(line0->data(), days);
        label1 = util::format("RSI %d Days", days);

        if (support == true) {
            vec2   = Point::Fixed(line0->data(), 30.0);
            label2 = "RSI 30";
            type2  = LineType::EXPAND_HORIZONTAL_FIRST;
            vec3   = Point::Fixed(line0->data(), 70.0);
            label3 = "RSI 70";
            type3  = LineType::EXPAND_HORIZONTAL_FIRST;
        }
    }
    else if (formula == Algorithm::STD_DEV) {
        auto days = util::to_long(fl_input_str(3, "Enter number of days (2 - 365)", "14"));

        if (days < 2 || days > 365) {
            return false;
        }

        vec1   = Point::StdDev(line0->data(), days);
        label1 = util::format("Std. dev. %d Days", days);
    }
    else if (formula == Algorithm::STOCHASTICS) {
        auto days = util::to_long(fl_input_str(3, "Enter number of days (2 - 365)", "14"));

        if (days < 2 || days > 365) {
            return false;
        }

        vec1   = Point::Stochastics(line0->data(), days);
        label1 = util::format("Stochastics %d Days", days);

        if (support == true) {
            vec2   = Point::Fixed(line0->data(), 20.0);
            label2 = "Stochastics 20";
            type2  = LineType::EXPAND_HORIZONTAL_FIRST;
            vec3   = Point::Fixed(line0->data(), 80.0);
            label3 = "Stochastics 80";
            type3  = LineType::EXPAND_HORIZONTAL_FIRST;
        }
    }

    if (vec1.size() == 0) {
        fl_alert("Error: no data!");
        return false;
    }

    line1.set_data(vec1).set_label(label1).set_type(type1).set_align(line0->align()).set_color(FL_BLUE);
    _LineSetup(top_window(), line1).run();
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

/** @brief Create tooltip string with current chart data.
*
* Either by using mouse cursor pos and convert it to real value.
* Or if ctrl is pressed then use actual chart point.
*
* @param[in] ctrl  Is ctrl key pressed?.
*/
void Chart::_create_tooltip(bool ctrl) {
    int         X   = Fl::event_x();
    int         Y   = Fl::event_y();
    std::string old = _tooltip;

    _tooltip = "";

    if (_area == nullptr) {
        Fl::redraw();
        return;
    }

    const int  STOP      = _date_start + _ticks;
    int        start     = _date_start;
    int        x1        = x() + _margin_left * _CH;
    int        left_dec  = 0;
    int        right_dec = 0;

    if (_area->left_scale().tick() < 10.0 ) {
        left_dec = util::count_decimals(_area->left_scale().tick()) + 1;
    }

    if (_area->right_scale().tick() < 10.0 ) {
        right_dec = util::count_decimals(_area->right_scale().tick()) + 1;
    }

    while (start <= STOP && start < static_cast<int>(_dates.size())) {
        if (X >= x1 && X <= x1 + _tick_width - 1) { // Is mouse x pos inside current tick?
            const Line* LINE = _area->selected_line();
            const auto       DATE = gnu::Date(_dates[start].date);
            std::string      date = DATE.format(gnu::Date::Format::DAY_MONTH_YEAR);

            if (_date_range == DateRange::HOUR || _date_range == DateRange::MIN || _date_range == DateRange::SEC) {
                 date += " - " + DATE.format(gnu::Date::Format::TIME_LONG);
            }

            _tooltip = date + "\n \n \n ";

            if (ctrl == false || LINE == nullptr || LINE->size() == 0 || LINE->is_visible() == false) { // Convert mouse pos to scale value.
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
                    _tooltip = util::format("%s\nleft:  %*s\nright: %*s\n ", date.c_str(), LEN, left.c_str(), LEN, right.c_str());
                }
                else if (left != "") {
                    _tooltip = util::format("%s\nleft: %*s\n \n ", date.c_str(), LEN, left.c_str());
                }
                else if (right != "") {
                    _tooltip = util::format("%s\nright: %*s\n \n ", date.c_str(), LEN, right.c_str());
                }
            }
            else { // Use actual chart data.
                const size_t index = Point::BinarySearch(LINE->data(), _dates[start]);

                if (index != static_cast<size_t>(-1)) {
                    const int         DEC   = (LINE->align() == FL_ALIGN_RIGHT) ? right_dec : left_dec;
                    const Point&  DATA  = LINE->data()[index];
                    const std::string HIGH  = util::format_double(DATA.high, DEC, '\'');
                    const std::string LOW   = util::format_double(DATA.low, DEC, '\'');
                    const std::string CLOSE = util::format_double(DATA.close, DEC, '\'');
                    const int         LEN   = static_cast<int>(LOW.length() > HIGH.length() ? LOW.length() : HIGH.length());

                    _tooltip = util::format("%s\nhigh:  %*s\nclose: %*s\nlow:   %*s", date.c_str(), LEN, HIGH.c_str(), LEN, CLOSE.c_str(), LEN, LOW.c_str());
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

/** @brief Print debug info to stdout.
*
* It prints also for all Areas and chart lines.
*/
void Chart::debug() const {
#ifdef DEBUG
    Point first, last, start, end;

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
    printf("\tdate_range:      %19s\n", Point::RangeToString(_date_range).c_str());
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

/** @brief Print chart data for current line and only for current view.
*
*/
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

    puts("");
    printf("%5s: %15s %23s %23s %23s\n", "COUNT", "DATE", "HIGH", "LOW", "CLOSE");

    while (curr <= stop && curr < static_cast<int>(_dates.size())) {
        auto& date  = _dates[curr];
        auto  index = Point::BinarySearch(data, date);

        if (index != static_cast<size_t>(-1)) {
            auto& d = line->data()[index];
            printf("%5d: %15s %23.8f %23.8f %23.8f\n", curr, date.date.c_str(), d.high, d.low, d.close);

            if (d.high > max) {
                max = d.high;
            }

            if (d.low < min) {
                min = d.low;
            }
        }

        curr++;
    }

    puts("");
    printf("Size:  %d\n", static_cast<int>(data.size()));
    printf("Label: %s\n", line->label().c_str());
    printf("Min:   %.8f\n", min);
    printf("Max:   %.8f\n", max);
    fflush(stdout);
#endif
}

/** @brief Draw chart.
*
*/
void Chart::draw() {
#ifdef DEBUG
     //auto t = util::milliseconds();
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
        if (area.percent() >= chart::_MIN_AREA_SIZE) {
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
     //printf("Chart::draw: %3d mS\n", util::milliseconds() - t);
     //fflush(stdout);
#endif
}

/** @brief Draw main label at top of the widget.
*
*/
void Chart::_draw_label() {
    if (_label == "") {
        return;
    }

    fl_color(FL_FOREGROUND_COLOR);
    fl_font(flw::PREF_FIXED_FONT, _CH * 1.5);
    fl_draw(_label.c_str(), x() + _areas[0].rect().x(), y(), _areas[0].rect().w(), _top_space, FL_ALIGN_CENTER | FL_ALIGN_INSIDE | FL_ALIGN_CLIP);
    _FLW_CHART_DEBUG(fl_rect(x() + _areas[0].rect().x(), y(), _areas[0].rect().w() + 1, _top_space))
}

/** @brief Draw all chart lines for input area.
*
* @param[in] area  Area with lines.
*/
void Chart::_draw_lines(Area& area) {
    for (const auto& line : area.lines()) {
        const auto& scale = (line.align() == FL_ALIGN_LEFT) ? area.left_scale() : area.right_scale();

        if (line.size() > 0 && line.is_visible() == true && scale.diff() >= chart::MIN_VALUE) {
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

            if (line.type() == LineType::BAR_HLC && width + width3 >= _tick_width) {
                // Shrink width for BAR_HLC styles so horizontal line can be drawn.
                width = width3 = _tick_width / 2;

                if (width + width3 >= _tick_width) {
                    width--;
                }
            }
            else if (width >= _tick_width) {
                width = _tick_width - 1;
            }

            if (line.type() == LineType::LINE_DOT) {
                style = FL_DOT;
            }

            fl_color(line.color());
            fl_line_style(style, width);
            width2 = (width > 1) ? width / 2 : width2;

            while (date_c <= DATE_END && date_c < static_cast<int>(_dates.size())) {
                const size_t INDEX = Point::BinarySearch(line.data(), _dates[date_c]);

                if (INDEX != static_cast<size_t>(-1)) {
                    const auto&  DATA = line.data()[INDEX];
                    const double YH   = (DATA.high - SCALE_MIN) * SCALE_PIXEL;
                    const double YL   = (DATA.low - SCALE_MIN) * SCALE_PIXEL;
                    const double YC   = (DATA.close - SCALE_MIN) * SCALE_PIXEL;
                    const int    YH2  = static_cast<int>(B1 - YH);
                    const int    YL2  = static_cast<int>(B1 - YL);
                    const int    YC2  = static_cast<int>(B1 - YC);

                    if (line.type() == LineType::LINE || line.type() == LineType::LINE_DOT) {
                        if (x_last > -10 && y_last > -10) {
                            fl_line(x_last + width2, static_cast<int>(B1 - y_last), x() + x1 + width2, YC2);
                        }
                    }
                    else if (line.type() == LineType::BAR) {
                        fl_line(x() + x1 + width2, YH2, x() + x1 + width2, YL2);
                    }
                    else if (line.type() == LineType::BAR_CLAMP) {
                        fl_line(x() + x1 + width2, YH2, x() + x1 + width2, B1);
                    }
                    else if (line.type() == LineType::BAR_HLC) {
                        fl_line(x() + x1 + width2, YH2, x() + x1 + width2, YL2);
                        fl_line(x() + x1, YC2, x() + x1 + width + width3, YC2);
                    }
                    else if (line.type() == LineType::HORIZONTAL) {
                        fl_line(x() + x1, YC2, x() + x1 + _tick_width - (width == 1 ? 0 : 1), YC2);
                    }
                    else if (line.type() == LineType::EXPAND_VERTICAL) {
                        fl_line(x() + x1, B1, x() + x1, y() + Y1);
                    }
                    else if (line.type() == LineType::EXPAND_HORIZONTAL_FIRST) {
                        fl_line(x() + _margin_left * _CH, YC2, x() + Fl_Widget::w() - _margin_right * _CH, YC2);
                        break;
                    }
                    else if (line.type() == LineType::EXPAND_HORIZONTAL_ALL) {
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

/** @brief Draw chart labels for one area.
*
* When labels are drawn each labels rectangle is updated.\n
* Rectangles are used for to select line by clicking inside the rectangle.\n
*
* @param[in] area  Area with line labels.
*/
void Chart::_draw_line_labels(Area& area) {
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

    for (const auto& line : area.lines()) { // Calc max width of line labels.
        int  ws    = 0;
        int  hs    = 0;
        auto label = _LABEL_SYMBOL + line.label();

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

    if (left_w > 0) { // Draw left box.
        left_w += _CH;
        fl_color(FL_BACKGROUND2_COLOR);
        fl_rectf(LEFT_X, left_y, left_w, left_h);

        fl_color(FL_FOREGROUND_COLOR);
        fl_rect(LEFT_X, left_y, left_w, left_h);
    }

    if (right_w > 0) { // Draw right box.
        right_w += _CH;
        fl_color(FL_BACKGROUND2_COLOR);
        fl_rectf(RIGHT_X - right_w, left_y, right_w, right_h);

        fl_color(FL_FOREGROUND_COLOR);
        fl_rect(RIGHT_X - right_w, left_y, right_w, right_h);
    }

    if (left_w > 0 || right_w > 0) { // Draw labels.
        size_t line_c = 0;

        for (auto& line : area.lines()) {
            auto const label = (area.selected() == line_c++) ? _LABEL_SYMBOL + line.label() : line.label();

            fl_color((line.is_visible() == false) ? FL_GRAY : line.color());

            if (line.align() == FL_ALIGN_LEFT) {
                fl_draw(label.c_str(), LEFT_X + 4, left_y + 4, left_w - 8, fl_height(), FL_ALIGN_LEFT | FL_ALIGN_CLIP);
                _FLW_CHART_DEBUG(fl_rect(LEFT_X + 4, left_y + 4, left_w - 8, fl_height()))
                const_cast<Line*>(&line)->set_label_rect(LEFT_X, left_y + 5, left_w, fl_height() + 1);
                left_y += fl_height();
            }
            else {
                fl_draw(label.c_str(), RIGHT_X - right_w + 4, right_y + 4, right_w - 8, fl_height(), FL_ALIGN_LEFT | FL_ALIGN_CLIP);
                _FLW_CHART_DEBUG(fl_rect(RIGHT_X - right_w + 4, right_y + 4, right_w - 8, fl_height()))
                const_cast<Line*>(&line)->set_label_rect(RIGHT_X - right_w, right_y + 5, right_w, fl_height() + 1);
                right_y += fl_height();
            }
        }
    }
}

/** @brief Draw tooltip with date and y value.
*
*/
void Chart::_draw_tooltip() {
    if (_tooltip == "" || _area == nullptr) {
        return;
    }

    fl_font(flw::PREF_FIXED_FONT, _CH);

    int ch = fl_height();
    int x1 = Fl::event_x();
    int y1 = Fl::event_y();
    int w1 = 12;
    int h1 = 5;

    if (x1 > _area->rect().r() - ch * (w1 + 5)) {
        x1 -= ch * (w1 + 4);
    }
    else {
        x1 += ch * 2;
    }

    if (y1 > y() + h() - ch * (h1 + 7)) {
        y1 -= ch * (h1 + 2);
    }
    else {
        y1 += ch * 2;
    }

    w1 += 3;

    fl_color(FL_BACKGROUND2_COLOR);
    fl_rectf(x1, y1, ch * w1, ch * h1);

    fl_color(FL_FOREGROUND_COLOR);
    fl_rect(x1, y1, ch * w1, ch * h1);

    fl_line(Fl::event_x(), y() + _area->rect().y(), Fl::event_x(), y() + _area->rect().y() + _area->rect().h());
    fl_line(x() + _area->rect().x(), Fl::event_y(), x() + _area->rect().x() + _area->rect().w(), Fl::event_y());
    fl_draw(_tooltip.c_str(), x1 + 4, y1 + 4, ch * w1 - 8, ch * h1, FL_ALIGN_LEFT | FL_ALIGN_INSIDE | FL_ALIGN_CLIP | FL_ALIGN_TOP);
    _FLW_CHART_DEBUG(fl_rect(x1 + 4, y1 + 4, ch * w1 - 8, ch * h1))
}

/** @brief Draw all vertical lines.
*
* X value are set in _draw_xlabels.
*
* @param[in] area  Current area.
*/
void Chart::_draw_ver_lines(Area& area) {
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

/** @brief Draw date x labels.
*
* And also save x pos for vertical lines.
*/
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
        const auto date  = gnu::Date(_dates[date_c].date.c_str());
        bool       addv  = false;
        int        month = 1;

        *buffer1 = 0;
        *buffer2 = 0;

        if (_date_range == DateRange::SEC) {
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
        else if (_date_range == DateRange::MIN) {
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
        else if (_date_range == DateRange::HOUR) {
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
        else if (_date_range == DateRange::DAY || _date_range == DateRange::WEEKDAY) {
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
        else if (_date_range == DateRange::FRIDAY || _date_range == DateRange::SUNDAY) {
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
        else if (_date_range == DateRange::MONTH) {
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
            fl_draw(buffer1, x1 - ADJUST, Y1 + _CH + chart::_TICK_SIZE * 2, ADJUST * 2, _CH, FL_ALIGN_CENTER | FL_ALIGN_INSIDE);
            _FLW_CHART_DEBUG(fl_rect(x1 - ADJUST, Y1 + _CH + chart::_TICK_SIZE * 2, ADJUST * 2, _CH))
            x_last = x1 + ADJUST * 2 + _CW;
        }

        if (*buffer2 != 0 && (cw2 * 2 + chart::_TICK_SIZE) < _tick_width) { // Draw second x label (day/week/hour/minute/second).
            fl_font(flw::PREF_FIXED_FONT, CH2);
            fl_draw(buffer2, x1 - cw2, Y1 + chart::_TICK_SIZE * 2, cw2 * 2, CH2, FL_ALIGN_CENTER | FL_ALIGN_INSIDE);
            _FLW_CHART_DEBUG(fl_rect(x1 - cw2, Y1 + chart::_TICK_SIZE * 2, cw2 * 2, CH2))
            month = 1;
        }

        fl_line(x1, Y1, x1, Y1 + chart::_TICK_SIZE * month);

        if (addv == true && ver_c < static_cast<int>(Chart::MAX_VLINES - 1)) { // Save x pos for vertical lines.
            _ver_pos[ver_c++] = x1;
        }

        x1 += _tick_width;
        date_c++;
    }

    _ver_pos[ver_c] = -1;
}

/** @brief Draw all ylabels on one side.
*
* @param[in] area   Current area.
* @param[in] align  Which side to draw on, FL_ALIGN_LEFT or FL_ALIGN_RIGHT.
*/
void Chart::_draw_ylabels(Area& area, Fl_Align align) {
    Scale& scale = (align == FL_ALIGN_LEFT) ? area.left_scale() : area.right_scale();

    if (scale.diff() < 0.0 || scale.pixel() * scale.tick() < 1.0) {
        return;
    }

    fl_font(flw::PREF_FIXED_FONT, _CH);

    const int    TICK3  = chart::_TICK_SIZE * 3;
    const double Y_INC  = (scale.pixel() * scale.tick());
    const double Y_FR   = util::count_decimals(scale.tick());
    const int    CH     = _CH / 2;
    const int    WIDTH  = (align == FL_ALIGN_LEFT ? area.rect().x() - TICK3 : w() - area.rect().r() - TICK3 - 1);
    const int    X1     = (align == FL_ALIGN_LEFT ? x() + chart::_TICK_SIZE : 0);
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
                fl_line(R - chart::_TICK_SIZE, Y2, R, Y2);
                fl_draw(LABEL.c_str(), X1 + 1, Y2 - CH, WIDTH, _CH, FL_ALIGN_RIGHT | FL_ALIGN_INSIDE | FL_ALIGN_CLIP);
                _FLW_CHART_DEBUG(fl_rect(X1 + 1, Y2 - CH, WIDTH, _CH))
            }
            else {
                const int R = x() + area.rect().r();
                fl_color(labelcolor());
                fl_line(R, Y2, R + chart::_TICK_SIZE, Y2);
                fl_draw(LABEL.c_str(), R + chart::_TICK_SIZE * 2, Y2 - CH, WIDTH, _CH, FL_ALIGN_RIGHT | FL_ALIGN_INSIDE | FL_ALIGN_CLIP);
                _FLW_CHART_DEBUG(fl_rect(R + chart::_TICK_SIZE * 2, Y2 - CH, WIDTH, _CH))
            }

            if (_horizontal == true && Y2 > y() + area.rect().y() + chart::_TICK_SIZE) {
                fl_color(fl_color_average(FL_FOREGROUND_COLOR, FL_BACKGROUND2_COLOR, 0.2));
                fl_line(x() + area.rect().x(), Y2, x() + area.rect().r(), Y2);
            }

            y_last = y1 - _CH - chart::_TICK_SIZE;
        }

        y1    -= Y_INC;
        y_val += scale.tick();
    }
}

/** @brief Find current area below mouse cursor.
*
* @param[in] x  X pos.
* @param[in] y Y pos.
*
* @return Area or nullptr.
*/
Area* Chart::_get_active_area(int X, int Y) {
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

/** @brief Event handler.
*
* Left mouse press inside an area shows an tooltip.\n
* Middle click in a lebl rectangle selected that line.\n
* Mouse wheel scrolls throuh the data set.\n
* Ctrl + mouse wheel changes tick width.\n
*
* @param[in] event  Event value.
*
* @return 1 or 0.
*/
int Chart::handle(int event) {
    static bool LEFT = false; // Disable dragging with middle mouse button.

    if (event == FL_PUSH) {
        const int x = Fl::event_x();
        const int y = Fl::event_y();
        const int b = Fl::event_button();

        if (b == FL_LEFT_MOUSE) { // Create and draw tooltip.
            LEFT  = true;
            _area = _get_active_area(x, y);

            _create_tooltip(Fl::event_ctrl() != 0);

            if (_tooltip != "") {
                return 1;
            }
        }
        else if (b == FL_MIDDLE_MOUSE) { // Select line if pressed on line label.
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
        else if (b == FL_RIGHT_MOUSE) { // Show menu.
            LEFT  = false;
            _area = _get_active_area(x, y);

            _show_menu();
            return 1;
        }
    }
    else if (event == FL_DRAG && LEFT == true) { // Update tooltip and draw it.
        _area = _get_active_area(Fl::event_x(), Fl::event_y());
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

            if (width >= chart::MIN_TICK && width <= chart::MAX_TICK) {
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

/** @brief Initiate the chart.
*
* @param[in] calc_dates  True to generatee new rerence date serie.
*/
void Chart::_init(bool calc_dates) {
#ifdef DEBUG
//     auto t = util::microseconds();
#endif
    unsigned kludge = 0;

    fl_font(flw::PREF_FIXED_FONT, flw::PREF_FIXED_FONTSIZE);
    *const_cast<int*>(&_CW) = fl_width("X");
    *const_cast<int*>(&_CH) = flw::PREF_FIXED_FONTSIZE;

    if (calc_dates == true) {
        WaitCursor wc;

        _calc_dates();
        _margin_left  = chart::_MIN_MARGIN;
        _margin_right = chart::_MIN_MARGIN;
        kludge++;
    }

    while (kludge <= 1) { // To improve margins calculations.
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
//     printf("%s: %3d uS\n", (calc_dates == true) ? "INIT" : "init", static_cast<int>(util::microseconds() - t));
//     fflush(stdout);
#endif
}

/** @brief Create an vector with labels and 0/1 prepended to the label.
*
* For dlg::select_checkboxes() dialogs.
*
* @param[in] area       Current area.
* @param[in] labeltype  Value to set.
*
* @return Vector with strings like "1Label".
*/
//
StringVector Chart::_label_array(const Area& area, LabelType labeltype) const {
    auto res = StringVector();

    for (const auto& l : area.lines()) {
        std::string s;

        if (labeltype == LabelType::OFF) {
            s = "0";
        }
        else if (labeltype == LabelType::ON) {
            s = "1";
        }
        else if (labeltype == LabelType::VISIBLE) {
            s = (l.is_visible() == true) ? "1" : "0";
        }

        s += l.label();
        res.push_back(s);
    }

    return res;
}

/** @brief Show file dialog and select a json file.
*
* @return False for no file or return value from Chart::load_json(std::string).
*/
bool Chart::load_json() {
    auto filename = util::to_string(fl_file_chooser("Select JSON File", "All Files (*)\tJSON Files (*.json)", ""));

    if (util::is_empty(filename) == true) {
        return false;
    }

    return load_json(filename);
}

/** @brief Parse json file and create a complete char with data from it.
*
* @param[in] filename  File with json data.
*
* @return True if ok.
*/
bool Chart::load_json(const std::string& filename) {
    #define _FLW_CHART_ERROR(X) { fl_alert("Error: illegal chart value at pos %u", (X)->pos()); reset(); return false; }

    _filename = "";

    reset();
    redraw();

    auto wc  = WaitCursor();
    auto buf = gnu::file::read(filename);

    if (buf.c_str() == nullptr) {
        fl_alert("Error: failed to load %s", filename.c_str());
        return false;
    }

    auto js = gnu::json::decode(buf.c_str(), buf.size(), true);

    if (js.has_err() == true) {
        fl_alert("Error: failed to parse %s (%s)", filename.c_str(), js.err_c());
        return false;
    }

    if (js.is_object() == false) _FLW_CHART_ERROR(&js);

    for (const auto j : js.vo_to_va()) {
        if (j->name() == "flw::chart" && j->is_object() == true) {
            for (const auto j2 : j->vo_to_va()) {
                if (j2->name() == "version" && j2->is_number() == true) {
                    if (j2->vn_i() != chart::VERSION) { fl_alert("Error: wrong chart version!\nI expected version %d but the json file had version %d!", static_cast<int>(j2->vn_i()), chart::VERSION); reset(); return false; }
                }
                else if (j2->name() == "label" && j2->is_string() == true) {
                    set_main_label(j2->vs_u());
                }
                else if (j2->name() == "date_range" && j2->is_string() == true) set_date_range(Point::StringToRange(j2->vs()));
                else if (j2->name() == "tick_width" && j2->is_number() == true) set_tick_width(j2->vn_i());
                else if (j2->name() == "labels" && j2->is_bool() == true)       set_line_labels(j2->vb());
                else if (j2->name() == "horizontal" && j2->is_bool() == true)   set_hor_lines(j2->vb());
                else if (j2->name() == "vertical" && j2->is_bool() == true)     set_ver_lines(j2->vb());
                else _FLW_CHART_ERROR(j2)
            }
        }
        else if (j->name() == "flw::chart::areas" && j->is_object() == true) {
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
        else if (j->name() == "flw::chart::lines" && j->is_array() == true) {
            for (const auto j2 : *j->va()) {
                int             line[5]  = { 0, 0, 0, 0, 1 };
                std::string     label;
                std::string     type;
                PointVector vec;

                if ( j2->is_object() == true) {
                    for (const auto l : j2->vo_to_va()) {
                        if (l->name() == "area" && l->is_number() == true) {
                            line[1] = l->vn_i();
                            if (line[1] < 0 || line[1] > static_cast<int>(AreaNum::LAST)) _FLW_CHART_ERROR(l)
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
                                else if (p->size() == 2 && (*p)[0]->is_string() == true && (*p)[1]->is_number() == true) vec.push_back(Point((*p)[0]->vs(), (*p)[1]->vn()));
                                else if (p->size() == 4 && (*p)[0]->is_string() == true && (*p)[1]->is_number() == true && (*p)[2]->is_number() == true && (*p)[3]->is_number() == true) vec.push_back(Point((*p)[0]->vs(), (*p)[1]->vn(), (*p)[2]->vn(), (*p)[3]->vn()));
                                else _FLW_CHART_ERROR(p)
                            }
                        }
                        else _FLW_CHART_ERROR(l)
                    }
                }

                auto l = Line(vec, label);
                auto a = static_cast<AreaNum>(line[1]);
                l.set_align(line[0]).set_color(line[2]).set_label(label).set_width(line[3]).set_type_from_string(type).set_visible(line[4]);
                area(a).add_line(l);
            }
        }
        else if (j->name() == "flw::chart::block" && j->is_array() == true) {
            PointVector dates;

            for (const auto d : *j->va()) {
                if (d->is_string() == true) dates.push_back(Point(d->vs()));
                else _FLW_CHART_ERROR(d)
            }

            set_block_dates(dates);
        }
        else _FLW_CHART_ERROR(j)
    }

    init_new_data();
    _filename = filename;

    return true;
}

/** @brief Load csv data from file and add a new chart line.
*
* @return True if a line has been added to current area.
*/
bool Chart::load_line_from_csv() {
    if (_area == nullptr || _area->size() >= Area::MAX_LINES) {
        fl_alert("Error: max line count reached!");
        return false;
    }

    auto filename = util::to_string(fl_file_chooser("Select CSV File", "All Files (*)\tCSV Files (*.csv)", ""));

    if (util::is_empty(filename) == true) {
        return false;
    }

    auto vec = Point::LoadCSV(filename);

    if (vec.size() == 0) {
        fl_alert("Error: no data!");
        return false;
    }

    auto line = Line(vec, filename);

    line.set_color(FL_BLUE);
    _LineSetup(top_window(), line).run();
    _area->add_line(line);
    init_new_data();

    return true;
}

/** @brief Move line between two chart areas or delete line.
*
* @param[in] area          Current area.
* @param[in] index         Line index.
* @param[in] move          True to move, false to delete.
* @param[in] destination   Destination area (only for moving).
*
* @return True if ok.
*/
bool Chart::_move_or_delete_line(Area* area, size_t index, bool move, AreaNum destination) {
    if (area == nullptr || index >= area->size()) {
        return false;
    }

    Line* line1 = area->line(index);

    if (line1 == nullptr) {
        return false;
    }

    if (move == true) {
        if (destination == area->area()) { // Same area.
            return false;
        }

        auto  line2 = *line1;
        auto& dest  = _areas[static_cast<size_t>(destination)];

        if (dest.add_line(line2) == false) {
            fl_alert("Error: target area has reached maximum number of lines!");
            return false;
        }
    }

    _area->delete_line(index);
    return true;
}

/** @brief Print view to postscript.
*
*/
void Chart::print_to_postscript() {
    _printing = true;
    dlg::print("Print Chart", Chart::_CallbackPrinter, this, 1, 1, top_window());
    _printing = false;
    redraw();
}

/** @brief Clear all data.
*
*/
void Chart::reset() {
    static_cast<Fl_Valuator*>(_scroll)->value(0);

    _block_dates.clear();
    _dates.clear();

    for (auto& area : _areas) {
        area.reset();
    }

    *const_cast<int*>(&_CW) = flw::PREF_FIXED_FONTSIZE;
    *const_cast<int*>(&_CH) = flw::PREF_FIXED_FONTSIZE;

    _area         = nullptr;
    _bottom_space = 0;
    _date_start   = 0;
    _filename     = "";
    _margin_left  = chart::_MIN_MARGIN;
    _margin_right = chart::_MIN_MARGIN;
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

/** @brief Resize widget.
*
* @param[in] X  X pos.
* @param[in] Y  Y pos.
* @param[in] W  Width.
* @param[in] H  Height.
*/
void Chart::resize(int X, int Y, int W, int H) {
    if (_old.w() == W && _old.h() == H) {
        return;
    }

    Fl_Widget::resize(X, Y, W, H);
    _scroll->resize(X, Y + H - Fl::scrollbar_size(), W, Fl::scrollbar_size());
    _old = Fl_Rect(this);
    init();
}

/** @brief Ask for filename and save complete chart to json file.
*
* @return True if ok.
*/
bool Chart::save_json() {
    auto filename = util::to_string(fl_file_chooser("Save To JSON File", "All Files (*)\tJSON Files (*.json)", ""));

    if (util::is_empty(filename) == true) {
        return false;
    }

    return save_json(filename);
}

/** @brief Save complete chart view to json file.
*
* @param[in] filename           Destination filename.
* @param[in] max_diff_high_low  If difference bwteen high, low and close are less than this value only close value are saved.
*
* @return True if ok.
*/
bool Chart::save_json(const std::string& filename, double max_diff_high_low) {
    auto wc  = WaitCursor();
    auto jsb = gnu::json::Builder();

    try {
        jsb << gnu::json::Builder::MakeObject();
            jsb << gnu::json::Builder::MakeObject("flw::chart");
                jsb << gnu::json::Builder::MakeNumber(chart::VERSION, "version");
                jsb << gnu::json::Builder::MakeString(_label.c_str(), "label");
                jsb << gnu::json::Builder::MakeNumber(_tick_width, "tick_width");
                jsb << gnu::json::Builder::MakeString(Point::RangeToString(_date_range), "date_range");
                jsb << gnu::json::Builder::MakeBool(_labels, "labels");
                jsb << gnu::json::Builder::MakeBool(_horizontal, "horizontal");
                jsb << gnu::json::Builder::MakeBool(_vertical, "vertical");
            jsb.end();

            jsb << gnu::json::Builder::MakeObject("flw::chart::areas");
            for (size_t f = 0; f <= static_cast<int>(AreaNum::LAST); f++) {
                auto  perc_str = util::format("area%u", static_cast<unsigned>(f));
                auto  min_str  = util::format("min%u", static_cast<unsigned>(f));
                auto  max_str  = util::format("max%u", static_cast<unsigned>(f));
                auto& area     = _areas[f];
                auto  min      = area.clamp_min();
                auto  max      = area.clamp_max();

                jsb << gnu::json::Builder::MakeNumber(_areas[f].percent(), perc_str.c_str());
                if (min.has_value() == true) jsb << gnu::json::Builder::MakeNumber(min.value(), min_str.c_str());
                if (max.has_value() == true) jsb << gnu::json::Builder::MakeNumber(max.value(), max_str.c_str());
            }
            jsb.end();

            jsb << gnu::json::Builder::MakeArray("flw::chart::lines");
                for (auto& area : _areas) {
                    for (auto& line : area.lines()) {
                        if (line.size() > 0) {
                            jsb << gnu::json::Builder::MakeObject();
                                jsb << gnu::json::Builder::MakeNumber(static_cast<int>(area.area()), "area");
                                jsb << gnu::json::Builder::MakeString(line.label(), "label");
                                jsb << gnu::json::Builder::MakeString(line.type_to_string(), "type");
                                jsb << gnu::json::Builder::MakeNumber(line.align(), "align");
                                jsb << gnu::json::Builder::MakeNumber(line.color(), "color");
                                jsb << gnu::json::Builder::MakeNumber(line.width(), "width");
                                jsb << gnu::json::Builder::MakeBool(line.is_visible(), "visible");
                                jsb << gnu::json::Builder::MakeArray("yx");
                                for (const auto& data : line.data()) {
                                    jsb << gnu::json::Builder::MakeArrayInline();
                                        jsb << gnu::json::Builder::MakeString(data.date);
                                        if (fabs(data.close - data.low) > max_diff_high_low || fabs(data.close - data.high) > max_diff_high_low) {
                                            jsb << gnu::json::Builder::MakeNumber(data.high);
                                            jsb << gnu::json::Builder::MakeNumber(data.low);
                                        }
                                        jsb << gnu::json::Builder::MakeNumber(data.close);
                                    jsb.end();
                                }
                                jsb.end();
                            jsb.end();
                        }
                    }
                }
            jsb.end();

            jsb << gnu::json::Builder::MakeArray("flw::chart::block");
                for (const auto& data : _block_dates) {
                    jsb << gnu::json::Builder::MakeString(data.date);
                }
            jsb.end();

        auto json = jsb.encode();
        auto res  = gnu::file::write(filename, json.c_str(), json.length());

        if (res == true) {
            _filename = filename;
        }

        return res;
    }
    catch(const std::string& e) {
        fl_alert("Error: failed to encode json\n%s", e.c_str());
        return false;
    }
}

/** @brief Save chart line values to csv file.
*
* @return True if a file was saved.
*/
bool Chart::save_line_to_csv() {
    if (_area == nullptr || _area->selected_line() == nullptr) {
        return false;
    }

    auto filename = util::to_string(fl_file_chooser("Save To CSV File", "All Files (*)\tCSV Files (*.csv)", ""));

    if (util::is_empty(filename) == true) {
        return false;
    }

    const auto* line   = _area->selected_line();
    const auto  answer = fl_choice("Save all data or only those in view?", nullptr, "All", "View");
    auto        data   = PointVector();
    const auto& ldata  = line->data();

    if (answer == 2) {
        auto curr = _date_start;
        auto stop = _date_start + _ticks;

        while (curr <= stop && curr < static_cast<int>(_dates.size())) {
            auto& date  = _dates[curr];
            auto  index = Point::BinarySearch(ldata, date);

            if (index != static_cast<size_t>(-1)) {
                data.push_back(ldata[index]);
            }

            curr++;
        }
    }
    else {
        data = ldata;
    }

    if (data.size() == 0) {
        fl_alert("Error: no data!");
        return false;
    }
    else {
        return Point::SaveCSV(data, filename);
    }
}

/** @brief Save view to an png file.
*
* @return True if ok.
*/
bool Chart::save_png() {
    return util::png_save("", top_window(), x() + 1,  y() + 1,  w() - 2,  h() - _scroll->h() - 1);
}

/** @brief Set size for all chart areas
*
* Total area size must be 100.\n
*
* @param[in] area1  Size of area 1 (0 or 10 - 100).
* @param[in] area2  Size of area 2 (0 or 10 - 100).
* @param[in] area3  Size of area 3 (0 or 10 - 100).
* @param[in] area4  Size of area 4 (0 or 10 - 100).
* @param[in] area5  Size of area 5 (0 or 10 - 100).
*
* @return True if ok.
*/
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

        if (area1 == 0) {
            return false;
        }

        if (area2 == 0 && (area3 > 0 || area4 > 0 || area5 > 0)) {
            return false;
        }

        if (area3 == 0 && (area4 > 0 || area5 > 0)) {
            return false;
        }

        if (area4 == 0 && area5 > 0) {
            return false;
        }

        _areas[0].set_percent(area1);
        _areas[1].set_percent(area2);
        _areas[2].set_percent(area3);
        _areas[3].set_percent(area4);
        _areas[4].set_percent(area5);

        return true;
    }

    return false;
}

/** @brief Show and dialog and select an area layout.
*
*/
void Chart::setup_area() {
    auto list = StringVector() = {"One", "Two equal", "Two (60%, 40%)", "Three equal", "Three (50%, 25%, 25%)", "Four Equal", "Four (40%, 20%, 20%, 20%)", "Five equal"};

    switch (dlg::select_choice("Select Number Of Chart Areas", list, 0, top_window())) {
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

/** @brief Ask for min or max clamp value.
*
* @param[in] min  True to set min clamp value or false for max value.
*/
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

/** @brief Create a line from an existing line.
*
*/
void Chart::setup_create_line() {
    if (_area == nullptr || _area->selected_line() == nullptr) {
        fl_alert("Error: area or line has not been selected!");
        return;
    }
    else if (_area->size() >= Area::MAX_LINES) {
        fl_alert("Error: max line count reached!");
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

    switch (dlg::select_choice("Select Formula", list, 0, top_window())) {
        case 0:
            create_line(Algorithm::MOVING_AVERAGE);
            break;
        case 1:
            create_line(Algorithm::EXP_MOVING_AVERAGE);
            break;
        case 2:
            create_line(Algorithm::MOMENTUM);
            break;
        case 3:
            create_line(Algorithm::MOMENTUM, true);
            break;
        case 4:
            create_line(Algorithm::STD_DEV);
            break;
        case 5:
            create_line(Algorithm::STOCHASTICS);
            break;
        case 6:
            create_line(Algorithm::STOCHASTICS, true);
            break;
        case 7:
            create_line(Algorithm::RSI);
            break;
        case 8:
            create_line(Algorithm::RSI, true);
            break;
        case 9:
            create_line(Algorithm::ATR);
            break;
        case 10:
            create_line(Algorithm::DAY_TO_WEEK);
            break;
        case 11:
            create_line(Algorithm::DAY_TO_MONTH);
            break;
        case 12:
            create_line(Algorithm::MODIFY);
            break;
        case 13:
            create_line(Algorithm::FIXED);
            break;
        default:
            break;
    }
}

/** @brief Show an dialog for creating new date serie with different date range.
*
*/
void Chart::setup_date_range() {
    auto list = StringVector() = {"Day", "Weekday", "Friday", "Sunday", "Month", "Hour", "Minute", "Second"};
    auto sel  = dlg::select_choice("Select Date Range Type", list, static_cast<int>(_date_range), top_window());

    if (sel == -1) {
        return;
    }

    _date_range = static_cast<DateRange>(sel);
    init_new_data();
}

/** @brief Show an dialog to select lines to delete.
*
*/
void Chart::setup_delete_lines() {
    if (_area == nullptr || _area->size() == 0) {
        return;
    }

    auto list_labels = dlg::select_checkboxes("Delete Lines", _label_array(*_area, LabelType::OFF), top_window());

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

/** @brief Ask user for main chart label.
*
*/
void Chart::setup_label() {
    auto l = fl_input(40, "Enter label", _label.c_str());

    if (l != nullptr) {
        _label = l;
        init();
    }
}

/** @brief Show property dialog for current line in current area.
*
*/
void Chart::setup_line() {
    if (_area == nullptr || _area->selected_line() == nullptr) {
        return;
    }

    auto line = const_cast<Line*>(_area->selected_line());
    auto ok   = _LineSetup(top_window(), *line).run();

    if (ok == false) {
        return;
    }

    init_new_data();
}

/** @brief Show an dialog to move lines between areas.
*
*/
void Chart::setup_move_lines() {
    if (_area == nullptr || _area->size() == 0) {
        return;
    }

    auto list_labels = dlg::select_checkboxes("Move Lines", _label_array(*_area, LabelType::OFF), top_window());

    if (list_labels.size() == 0) {
        return;
    }

    auto list_areas = StringVector() = {"Area 1", "Area 2", "Area 3", "Area 4", "Area 5"};
    auto area       = dlg::select_choice("Select Area", list_areas, 0, top_window());

    if (area < 0 || area > static_cast<int>(AreaNum::LAST)) {
        return;
    }

    for (size_t f = 0, e = 0; f < list_labels.size(); f++, e++) {
        if (list_labels[f][0] == '1') {
            if (_move_or_delete_line(_area, e, true, static_cast<AreaNum>(area)) == true) {
                e--;
            }
        }
    }

    init_new_data();
}

/** @brief Show an dialog for showing or hiding lines.
*
*/
void Chart::setup_show_or_hide_lines() {
    if (_area == nullptr || _area->size() == 0) {
        return;
    }

    auto list = _label_array(*_area, LabelType::VISIBLE);
    list = dlg::select_checkboxes("Show Or Hide Lines", list, top_window());

    if (list.size() == 0) {
        return;
    }

    auto f = 0;

    for (auto& line : _area->lines()) {
        const_cast<Line&>(line).set_visible(list[f++][0] == '1');
    }

    init_new_data();
}

/** @brief Configure some view options from menu checkboxes.
*
* Show/hide labels.
* Show/hide vertical lines.
* Show/hide horizontal lines.
*/
void Chart::setup_view_options() {
    _labels     = menu::item_value(_menu, _LABEL_SHOW_LABELS);
    _vertical   = menu::item_value(_menu, _LABEL_SHOW_VLINES);
    _horizontal = menu::item_value(_menu, _LABEL_SHOW_HLINES);

    redraw();
}

/** @brief Show menu
*
*/
void Chart::_show_menu() {
    if (_disable_menu == true) {
        return;
    }

    menu::enable_item(_menu, _LABEL_ADD_CSV, false);
    menu::enable_item(_menu, _LABEL_ADD_LINE, false);
    menu::enable_item(_menu, _LABEL_SAVE_CSV, false);
    menu::enable_item(_menu, _LABEL_SETUP_DELETE, false);
    menu::enable_item(_menu, _LABEL_SETUP_LINE, false);
    menu::enable_item(_menu, _LABEL_SETUP_MAX_CLAMP, false);
    menu::enable_item(_menu, _LABEL_SETUP_MIN_CLAMP, false);
    menu::enable_item(_menu, _LABEL_SETUP_MOVE, false);
    menu::enable_item(_menu, _LABEL_SETUP_SHOW, false);
#ifdef DEBUG
    menu::enable_item(_menu, _LABEL_DEBUG_LINE, false);
#endif

    menu::set_item(_menu, _LABEL_SHOW_HLINES, _horizontal);
    menu::set_item(_menu, _LABEL_SHOW_LABELS, _labels);
    menu::set_item(_menu, _LABEL_SHOW_VLINES, _vertical);

    if (_area != nullptr) {
        menu::enable_item(_menu, _LABEL_ADD_CSV, true);

        if (_area->size() > 0) {
            menu::enable_item(_menu, _LABEL_SETUP_DELETE, true);
            menu::enable_item(_menu, _LABEL_SETUP_MOVE, true);
            menu::enable_item(_menu, _LABEL_SETUP_SHOW, true);
        }

        if (_area->selected_line() != nullptr && _area->selected_line()->size() > 0) {
            menu::enable_item(_menu, _LABEL_ADD_LINE, true);
            menu::enable_item(_menu, _LABEL_SAVE_CSV, true);
            menu::enable_item(_menu, _LABEL_SETUP_LINE, true);
            menu::enable_item(_menu, _LABEL_SETUP_MAX_CLAMP, true);
            menu::enable_item(_menu, _LABEL_SETUP_MIN_CLAMP, true);
#ifdef DEBUG
            menu::enable_item(_menu, _LABEL_DEBUG_LINE, true);
#endif
        }
    }

    _menu->popup();
}

/** @brief Update font properties
*
*/
void Chart::update_pref() {
    _menu->textfont(flw::PREF_FONT);
    _menu->textsize(flw::PREF_FONTSIZE);
}

} // chart
} // flw

// MKALGAM_OFF
