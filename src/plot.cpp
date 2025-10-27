/**
* @file
* @brief Plot widget.
*
* @author gnuwimp@gmail.com
* @copyright Released under the GNU General Public License v3.0
*/

#include "plot.h"
#include "flw.h"
#include "file.h"
#include "waitcursor.h"
#include "json.h"
#include "dlg.h"

// MKALGAM_ON

#include <assert.h>
#include <FL/fl_ask.H>
#include <FL/fl_draw.H>
#include <FL/fl_show_colormap.H>
#include <FL/Fl_File_Chooser.H>
#include <FL/Fl_Hor_Slider.H>

namespace flw {
namespace plot {

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

#define _FLW_PLOT_CB(X)    [](Fl_Widget*, void* o) { static_cast<Plot*>(o)->X; }, this
//#define _FLW_PLOT_DEBUG(X) { X; }
#define _FLW_PLOT_DEBUG(X)
#define _FLW_PLOT_CLIP(X) { X; }
//#define _FLW_PLOT_CLIP(X)

static const char* const _LABEL_ADD_LINE     = "Create line...";
static const char* const _LABEL_CLEAR        = "Clear plot";
static const char* const _LABEL_LOAD_CVS     = "Add line from cvs...";
static const char* const _LABEL_LOAD_JSON    = "Load plot from JSON...";
static const char* const _LABEL_PRINT        = "Print to PostScript file...";
static const char* const _LABEL_SAVE_CVS     = "Save line to cvs...";
static const char* const _LABEL_SAVE_JSON    = "Save plot to JSON...";
static const char* const _LABEL_SAVE_PNG     = "Save to png file...";
static const char* const _LABEL_SETUP_DELETE = "Delete lines...";
static const char* const _LABEL_SETUP_HLINES = "Show horizontal lines";
static const char* const _LABEL_SETUP_LABEL  = "Label...";
static const char* const _LABEL_SETUP_LABELS = "Show line labels";
static const char* const _LABEL_SETUP_LINE   = "Line properties...";
static const char* const _LABEL_SETUP_MAXX   = "Set max X...";
static const char* const _LABEL_SETUP_MAXY   = "Set max Y...";
static const char* const _LABEL_SETUP_MINX   = "Set min X...";
static const char* const _LABEL_SETUP_MINY   = "Set min Y...";
static const char* const _LABEL_SETUP_SHOW   = "Show or hide lines...";
static const char* const _LABEL_SETUP_VLINES = "Show vertical lines";
static const char* const _LABEL_SETUP_XLABEL = "X Label...";
static const char* const _LABEL_SETUP_YLABEL = "Y Label...";
static const int         _TICK_SIZE     = 4;

#ifdef DEBUG
static const char* const _LABEL_DEBUG        = "Debug";
static const char* const _LABEL_DEBUG_LINE   = "Debug line";
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

        _cancel = new Fl_Button(0, 0, 0, 0, "&Cancel");
        _close  = new Fl_Return_Button(0, 0, 0, 0, label::OK.c_str());
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
        _grid->add(_color,     12,  11,  -1,  4);
        _grid->add(_width,     12,  16,  -1,  4);
        _grid->add(_cancel,   -34,  -5,  16,  4);
        _grid->add(_close,    -17,  -5,  16,  4);
        add(_grid);

        _cancel->callback(_LineSetup::Callback, this);
        _close->callback(_LineSetup::Callback, this);
        _color->align(FL_ALIGN_LEFT);
        _color->callback(_LineSetup::Callback, this);
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
        _width->callback(_LineSetup::Callback, this);
        _width->precision(0);
        _width->range(0, plot::MAX_LINE_WIDTH);
        _width->value(_line.width());
        _width->tooltip("Set line width or circle/square size.");

        resizable(_grid);
        util::labelfont(this);
        callback(_LineSetup::Callback, this);
        size(30 * flw::PREF_FONTSIZE, 14 * flw::PREF_FONTSIZE);
        size_range(30 * flw::PREF_FONTSIZE, 14 * flw::PREF_FONTSIZE);
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
        else if (_type->value() == 1) _line.set_type(LineType::LINE_DASH);
        else if (_type->value() == 2) _line.set_type(LineType::LINE_DOT);
        else if (_type->value() == 3) _line.set_type(LineType::LINE_WITH_SQUARE);
        else if (_type->value() == 4) _line.set_type(LineType::VECTOR);
        else if (_type->value() == 5) _line.set_type(LineType::CIRCLE);
        else if (_type->value() == 6) _line.set_type(LineType::FILLED_CIRCLE);
        else if (_type->value() == 7) _line.set_type(LineType::SQUARE);
    }

    /** @brief Update widgets with plot line data.
    *
    */
    void update_widgets() {
        _label->value(_line.label().c_str());
        _color->color(_line.color());
        _LineSetup::Callback(_width, this);

        if (_line.type() == LineType::LINE)                  _type->value(0);
        else if (_line.type() == LineType::LINE_DASH)        _type->value(1);
        else if (_line.type() == LineType::LINE_DOT)         _type->value(2);
        else if (_line.type() == LineType::LINE_WITH_SQUARE) _type->value(3);
        else if (_line.type() == LineType::VECTOR)           _type->value(4);
        else if (_line.type() == LineType::CIRCLE)           _type->value(5);
        else if (_line.type() == LineType::FILLED_CIRCLE)    _type->value(6);
        else if (_line.type() == LineType::SQUARE)           _type->value(7);
    }
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

/** @brief Create point.
*
* If any value is invalid both values are set to INFINITY.
*
* @param[in] X  X value.
* @param[in] Y  Y value.
*/
Point::Point(double X, double Y) {
    x = y = INFINITY;

    if (std::isfinite(X) == true &&
        std::isfinite(Y) == true &&
        fabs(X) < plot::MAX_VALUE &&
        fabs(Y) < plot::MAX_VALUE) {

        x = X;
        y = Y;
    }
}

/** @brief Print value.
*
* @param[in] i  Array index (< 0 == not printed).
*/
void Point::debug(int i) const {
#ifdef DEBUG
    auto X = gnu::json::format_number(x);
    auto Y = gnu::json::format_number(y);

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

/** @brief Print values.
*
* @param[in] in  Vector with Point objects.
*/
void Point::Debug(const PointVector& in) {
#ifdef DEBUG
    printf("\nPointVector(%u)\n", static_cast<unsigned>(in.size()));

    int c = 0;

    for (const auto& data : in) {
        data.debug(c++);
    }

    fflush(stdout);
#else
    (void) in;
#endif
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

    if (buf.size() < 3) {
        return PointVector();
    }

    std::string  str   = buf.c_str();
    StringVector lines = util::split_string(str, "\n");
    PointVector  res;

    for (const auto& l : lines) {
        StringVector line = util::split_string(l, sep);

        if (line.size() == 2) {
            auto data = Point(util::to_double(line[0]), util::to_double(line[1]));

            if (data.is_valid() == true) {
                res.push_back(data);
            }
        }
    }

    return res;
}

/** @brief Find min and max values.
*
* @param[in] in     Vector with Point objects.
* @param[in] min_x  Min X value.
* @param[in] max_x  max X value.
* @param[in] min_y  Min Y value.
* @param[in] max_y  Max Y value.
*
* @return True if values not INFINITY.
*/
bool Point::MinMax(const PointVector& in, double& min_x, double& max_x, double& min_y, double& max_y) {
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

/** @brief Modify plot data.
*
* @param[in] in      Vector with Point objects.
* @param[in] modify  Type of modify.
* @param[in] target  X or Y or both.
* @param[in] value   Value to use.
*
* @return Result vector with Point objects.
*/
PointVector Point::Modify(const PointVector& in, plot::Modifier modify, plot::Value target, double value) {
    PointVector res;

    if (fabs(value) < plot::MIN_VALUE || fabs(value) > plot::MAX_VALUE) {
        return res;
    }

    for (const auto& data : in) {
        switch (modify) {
            case plot::Modifier::SUBTRACTION:
                if (target == plot::Value::X) {
                    res.push_back(Point(data.x - value, data.y));
                }
                else if (target == plot::Value::Y) {
                    res.push_back(Point(data.x, data.y - value));
                }
                else {
                    res.push_back(Point(data.x - value, data.y - value));
                }
                break;

            case plot::Modifier::MULTIPLICATION:
                if (target == plot::Value::X) {
                    res.push_back(Point(data.x * value, data.y));
                }
                else if (target == plot::Value::Y) {
                    res.push_back(Point(data.x, data.y * value));
                }
                else {
                    res.push_back(Point(data.x * value, data.y * value));
                }
                break;

            case plot::Modifier::DIVISION:
                if (target == plot::Value::X) {
                    res.push_back(Point(data.x / value, data.y));
                }
                else if (target == plot::Value::Y) {
                    res.push_back(Point(data.x, data.y / value));
                }
                else {
                    res.push_back(Point(data.x / value, data.y / value));
                }
                break;

            default: // plot::Modifier::ADDITION
                if (target == plot::Value::X) {
                    res.push_back(Point(data.x + value, data.y));
                }
                else if (target == plot::Value::Y) {
                    res.push_back(Point(data.x, data.y + value));
                }
                else {
                    res.push_back(Point(data.x + value, data.y + value));
                }
                break;
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

    csv.reserve(in.size() * 20 + 256);

    for (const auto& data : in) {
        char buffer[256];
        snprintf(buffer, 256, "%s%s%s\n", gnu::json::format_number(data.x).c_str(), sep.c_str(), gnu::json::format_number(data.y).c_str());
        csv += buffer;
    }

    return gnu::file::write(filename, csv.c_str(), csv.size());
}

/** @brief Swap X and Y values.
*
* @param[in] in  Vector with Point objects.
*
* @return Result vector with Point objects.
*/
PointVector Point::Swap(const PointVector& in) {
    PointVector res;

    for (const auto& data : in) {
        res.push_back(Point(data.y, data.x));
    }

    return res;
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

/** @brief Create a empty plot line.
*
*/
Line::Line() {
    reset();
}

/** @brief Create a plot line.
*
* @param[in] data    Vector with Point objects.
* @param[in] label   Line label.
* @param[in] type    Line type.
* @param[in] color   Line color.
* @param[in] width   Line width.
*/
Line::Line(const PointVector& data, const std::string& label, LineType type, Fl_Color color, unsigned width) {
    _color   = color;
    _data    = data;
    _label   = label;
    _type    = type;
    _visible = true;
    _width   = (width > 0 && width <= plot::MAX_LINE_WIDTH) ? width : 1;
}

/** @brief Print debug info to stdout.
*
*/
void Line::debug() const {
#ifdef DEBUG
    double minx, maxx, miny, maxy;
    Point::MinMax(_data, minx, maxx, miny, maxy);

    printf("\t------------------------------------------\n");
    printf("\tLine:\n");
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

/** @brief Clear all data.
*
*/
void Line::reset() {
    _color   = FL_BLUE;
    _label   = "";
    _rect    = Fl_Rect();
    _type    = LineType::LINE;
    _visible = true;
    _width   = 1;

    _data.clear();
}

/** @brief Set line type from string.
*
* If input name is invalid it will set type to LineType::LINE.
*
* @param[in] val  Line type name, one of ("LINE", "LINE_DASH", "LINE_DOT", "LINE_WITH_SQUARE", "VECTOR", "CIRCLE", "FILLED_CIRCLE", "SQUARE").
*
* @return Input plot line.
*/
Line& Line::set_type_from_string(const std::string& val) {
    if (val == "LINE_DASH")             _type = LineType::LINE_DASH;
    else if (val == "LINE_DOT")         _type = LineType::LINE_DOT;
    else if (val == "LINE_WITH_SQUARE") _type = LineType::LINE_WITH_SQUARE;
    else if (val == "VECTOR")           _type = LineType::VECTOR;
    else if (val == "CIRCLE")           _type = LineType::CIRCLE;
    else if (val == "FILLED_CIRCLE")    _type = LineType::FILLED_CIRCLE;
    else if (val == "SQUARE")           _type = LineType::SQUARE;
    else                                _type = LineType::LINE;

    return *this;
}

/** @brief Return line type as a string.
*
* @return Line type name, one of ("LINE", "LINE_DASH", "LINE_DOT", "LINE_WITH_SQUARE", "VECTOR", "CIRCLE", "FILLED_CIRCLE", "SQUARE", "", "").
*/
std::string Line::type_to_string() const {
    static const std::string NAMES[] = { "LINE", "LINE_DASH", "LINE_DOT", "LINE_WITH_SQUARE", "VECTOR", "CIRCLE", "FILLED_CIRCLE", "SQUARE", "", "", };
    return NAMES[static_cast<size_t>(_type)];
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

/** @brief Calc tick size.
*
* @param[in] pixels  Pixel count.
*/
void Scale::calc_tick(double pixels) {
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

        if (RANGE < plot::MIN_VALUE || RANGE > plot::MAX_VALUE) {
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

        if (_tick > plot::MIN_VALUE / 10.0 && kludge < 100) {
            _pixel = pixels / (_max - _min);
            _fr    = util::count_decimals(_tick);
        }
        else {
            _tick = 0;
        }
    }
}

/** @brief Print debug info.
*
* @param[in] s  Label.
*/
void Scale::debug(const char* s) const {
#ifdef DEBUG
    printf("\t------------------------------------------\n");
    printf("\tScale: %s\n", s);
    printf("\t\tcolor   = %u\n", _color);
    printf("\t\tfr      = %d\n", _fr);
    printf("\t\tlabel   = %s\n", _label.c_str());
    printf("\t\tlabels  = %d strings\n", static_cast<int>(_labels.size()));
    printf("\t\t_lwidth = %d px\n", (int) _lwidth);
    printf("\t\tmin     = %24.8f\n", _min);
    printf("\t\tmax     = %24.8f\n", _max);
    printf("\t\tdiff    = %24.8f\n", _max - _min);
    printf("\t\ttick    = %24.8f\n", _tick);
    printf("\t\tpixel   = %24.8f\n", _pixel);
    fflush(stdout);
#else
    (void) s;
#endif
}

/** @brief Measure width of all labels to set the largest width.
*
* @param[in] cw      Width of one character.
* @param[in] custom  True to measure custom labels.
*/
void Scale::measure_labels(int cw, bool custom) {
    if (_labels.size() > 0 && custom == true) {
        _lwidth = 0;

        for (const auto& l : _labels) {
            if (l.length() > _lwidth) {
                _lwidth = l.length();
            }
        }

        _lwidth *= cw;
    }
    else {
        int         dec = (_fr > 0) ? _fr : -1;
        std::string l1  = util::format_double(_min, dec, ' ');
        std::string l2  = util::format_double(_max, dec, ' ');

        _lwidth = (l1.length() > l2.length()) ? l1.length() * cw : l2.length() * cw;
    }
}

/** @brief Clera all data.
*
*/
void Scale::reset() {
    _color  = FL_FOREGROUND_COLOR;
    _fr     = 0;
    _label  = "";
    _labels = StringVector();
    _lwidth = 0;
    _pixel  = 0.0;
    _tick   = 0.0;

    reset_min_max();
}

/*
 *      _____  _       _
 *     |  __ \| |     | |
 *     | |__) | | ___ | |_
 *     |  ___/| |/ _ \| __|
 *     | |    | | (_) | |_
 *     |_|    |_|\___/ \__|
 *
 *
 */

/** @brief Create plot widget.
*
* @param[in] X  X pos.
* @param[in] Y  Y pos.
* @param[in] W  Width.
* @param[in] H  Height.
* @param[in] l  Label.
*/
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

    _menu->add(_LABEL_SETUP_LABELS,  0, _FLW_PLOT_CB(setup_view_options()), FL_MENU_TOGGLE);
    _menu->add(_LABEL_SETUP_HLINES,  0, _FLW_PLOT_CB(setup_view_options()), FL_MENU_TOGGLE);
    _menu->add(_LABEL_SETUP_VLINES,  0, _FLW_PLOT_CB(setup_view_options()), FL_MENU_TOGGLE | FL_MENU_DIVIDER);
    _menu->add(_LABEL_CLEAR,         0, _FLW_PLOT_CB(reset()));
    _menu->add(_LABEL_SETUP_LABEL,   0, _FLW_PLOT_CB(setup_label(plot::Label::MAIN)));
    _menu->add(_LABEL_SETUP_XLABEL,  0, _FLW_PLOT_CB(setup_label(plot::Label::X)));
    _menu->add(_LABEL_SETUP_YLABEL,  0, _FLW_PLOT_CB(setup_label(plot::Label::Y)));
    _menu->add(_LABEL_SETUP_MINX,    0, _FLW_PLOT_CB(setup_clamp(plot::Clamp::MINX)));
    _menu->add(_LABEL_SETUP_MAXX,    0, _FLW_PLOT_CB(setup_clamp(plot::Clamp::MAXX)));
    _menu->add(_LABEL_SETUP_MINY,    0, _FLW_PLOT_CB(setup_clamp(plot::Clamp::MINY)));
    _menu->add(_LABEL_SETUP_MAXY,    0, _FLW_PLOT_CB(setup_clamp(plot::Clamp::MAXY)), FL_MENU_DIVIDER);
    _menu->add(_LABEL_SETUP_LINE,    0, _FLW_PLOT_CB(setup_line()));
    _menu->add(_LABEL_ADD_LINE,      0, _FLW_PLOT_CB(setup_create_line()));
    _menu->add(_LABEL_LOAD_CVS,      0, _FLW_PLOT_CB(load_line_from_csv()), FL_MENU_DIVIDER);
    _menu->add(_LABEL_SETUP_SHOW,    0, _FLW_PLOT_CB(setup_show_or_hide_lines()));
    _menu->add(_LABEL_SETUP_DELETE,  0, _FLW_PLOT_CB(setup_delete_lines()), FL_MENU_DIVIDER);
    _menu->add(_LABEL_SAVE_CVS,      0, _FLW_PLOT_CB(save_line_to_csv()));
    _menu->add(_LABEL_SAVE_JSON,     0, _FLW_PLOT_CB(save_json()));
    _menu->add(_LABEL_LOAD_JSON,     0, _FLW_PLOT_CB(load_json()), FL_MENU_DIVIDER);
    _menu->add(_LABEL_PRINT,         0, _FLW_PLOT_CB(print_to_postscript()));
    _menu->add(_LABEL_SAVE_PNG,      0, _FLW_PLOT_CB(save_png()));
#ifdef DEBUG
    _menu->add(_LABEL_SAVE_PNG,      0, _FLW_PLOT_CB(save_png()), FL_MENU_DIVIDER);
    _menu->add(_LABEL_DEBUG,         0, _FLW_PLOT_CB(debug()));
    _menu->add(_LABEL_DEBUG_LINE,    0, _FLW_PLOT_CB(debug_line()));
#else
    _menu->add(_LABEL_SAVE_PNG,      0, _FLW_PLOT_CB(save_png()));
#endif
    _menu->type(Fl_Menu_Button::POPUP3);

    _disable_menu = false;

    reset();
    update_pref();
}

/** @brief Add plot line.
*
* @param[in] line  Line to add.
*
* @return True if added.
*/
bool Plot::add_line(const Line& line) {
    if (_lines.size() >= plot::MAX_LINES) {
        return false;
    }

    _lines.push_back(line);

    return true;
}

/** @brief Calc min and max X/Y values for all lines and all points.
*
*/
void Plot::_calc_min_max() {
    _x.reset_min_max();
    _y.reset_min_max();

    for (const auto& line : _lines) {
        if (line.is_visible() == false) {
            continue;
        }

        double minx, maxx, miny, maxy;

        if (Point::MinMax(line.data(), minx, maxx, miny, maxy) == true) {
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

    if (_x.is_min_finite() == true) { // Both scales have been set by now so check if clamp values exist.
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

/** @brief Callback for printing to postscript.
*
* @param[in] data  Plot widget.
* @param[in] pw    Page wisth.
* @param[in] ph    Page height.
* @param[in]       Page number, not used.
*
* @return Always false, stop printing.
*/
bool Plot::_CallbackPrinter(void* data, int pw, int ph, int) {
    auto widget = static_cast<Fl_Widget*>(data);
    auto rect   = Fl_Rect(widget);

    widget->resize(0, 0, pw, ph);
    widget->draw();
    widget->resize(rect.x(), rect.y(), rect.w(), rect.h());

    return false;
}

/** @brief Create new plot line from current line and add it to current area.
*
* @param[in] alg  Algorithm to use.
*
* @return True if a new line was added.
*/
bool Plot::create_line(Algorithm alg) {
    if (_selected_line >= _lines.size()) {
        dlg::msg_alert("Plot Error", "Line has not been selected!");
        return false;
    }
    else if (_lines.size() >= plot::MAX_LINES) {
        dlg::msg_alert("Plot Error", "Max line count reached!");
        return false;
    }

    Line&      line0 = _lines[_selected_line];
    PointVector vec1;
    std::string label1;

    if (alg == Algorithm::MODIFY) {
        auto list = StringVector() = {"Addition", "Subtraction", "Multiplication", "Division"};
        auto ans  = dlg::select_choice("Select Modification", list, 0);

        if (ans < 0 || ans > static_cast<int>(plot::Modifier::LAST)) {
            return false;
        }

        auto modify = static_cast<plot::Modifier>(ans);
        auto value  = 0.0;
        auto answer = flw::dlg::input_double("Plot", "Enter value.", value);

        if (answer == flw::label::CANCEL || std::isinf(value) == true) {
            return false;
        }
        else if (fabs(value) < plot::MIN_VALUE) {
            dlg::msg_alert("Plot Error", "To small value for division!");
            return false;
        }

        list   = StringVector() = {"Only X", "Only Y", "Both X && Y"};
        ans    = dlg::select_choice("Select Target", list, 0);
        vec1   = Point::Modify(line0.data(), modify, (ans == 0 ? plot::Value::X : ans == 1 ? plot::Value::Y : plot::Value::X_AND_Y), value);
        label1 = util::format("Modified %s", line0.label().c_str());
    }
    else if (alg == Algorithm::SWAP) {
        vec1   = Point::Swap(line0.data());
        label1 = util::format("%s swapped X/Y", line0.label().c_str());
    }

    if (vec1.size() == 0) {
        dlg::msg_alert("Plot Error", "To few data values!");
        return false;
    }

    auto line1 = Line(vec1, label1);

    _LineSetup(top_window(), line1).run();
    _lines.push_back(line1);
    init_new_data();

    return true;
}

/** @brief Create a vector with labels for the checkbox dialog.
*
* @param[in] def  True to use the default value or false to uncheck all.
*
* @return String vector.
*/
StringVector Plot::_create_check_labels(bool def) const {
    StringVector res;

    for (const auto& l : _lines) {
        std::string s = (def == true && l.is_visible() == true) ? "1" : "0";
        s += l.label();
        res.push_back(s);
    }

    return res;
}

/** @brief Create tooltip with x and y values when left mouse button is pressed.
*
* Either by using mouse cursor position.\n
* Or the actual point below mouse cursor.\n
*/
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

/** @brief Print debug info to stdout.
*
*/
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

/** @brief Print debug info for current selected line.
*
*/
void Plot::debug_line() const {
#ifdef DEBUG
    if (_selected_line >= _lines.size()) {
        return;
    }

    Point::Debug(_lines[_selected_line].data());
#endif
}

/** @brief Delete plot line.
*
* @param[in] index  Line vector index.
*/
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

/** @brief Draw plot widget.
*
*/
void Plot::draw() {
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

    if (_area.w() < 75 || _area.h() < 50 || _x.min() >= _x.max() || _x.pixel() * _x.tick() < 1.0 || _y.min() >= _y.max() || _y.pixel() * _y.tick() < 1.0) {
        goto DRAW_RECTANGLE;
    }

    _FLW_PLOT_CLIP(fl_push_clip(x(), y(), w(), h()))

    if (_lines.size() > 0) {
        if (_x.custom_labels().size() > 0 && _lines[0].type() == LineType::VECTOR) {
            _draw_xlabels_custom();
        }
        else {
            _draw_xlabels();
        }

        if (_y.custom_labels().size() > 0 && _lines[0].type() == LineType::VECTOR) {
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
     //printf("Plot::draw: %d mS\n", util::milliseconds() - t);
     //fflush(stdout);
#endif
}

/** @brief Draw all labels.
*
*/
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

/** @brief Draw all line labels.
*
*/
void Plot::_draw_line_labels() {
    static const std::string SYMBOL = "@-> ";

    if (_labels == false) {
        return;
    }

    fl_font(flw::PREF_FIXED_FONT, _CH);

    int w1 = 0;

    for (const auto& line : _lines) { // Calc max width.
        int ws = 0;
        int hs = 0;

        fl_measure((SYMBOL + line.label()).c_str(), ws, hs);

        if (ws > w1) {
            w1 = ws;
        }
    }

    if (w1 > 0) { // Draw labels.
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

/** @brief Draw all plot lines.
*
*/
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

            if (TYPE == LineType::VECTOR && (_selected_point == data_c || _selected_point == data_c - 1) && _selected_line == line_c) {
                col = selection_color();
            }
            else if (TYPE != LineType::VECTOR && _selected_point == data_c && _selected_line == line_c) {
                col = selection_color();
            }

            fl_color(col);

            if (line.has_line_type() == true) {
                if (x2 == 10'000) { // Don't draw first x,y values.
                    x2 = X3;
                    y2 = Y3;
                }
                else {
                    if (TYPE == LineType::LINE_DASH) {
                        fl_line_style(FL_DASH, WIDTH1);
                    }
                    else if (TYPE == LineType::LINE_DOT) {
                        fl_line_style(FL_DOT, WIDTH1);
                    }
                    else {
                        fl_line_style(FL_SOLID, WIDTH1);
                    }

                    fl_line(x2, y2, X3, Y3);
                    x2 = X3;
                    y2 = Y3;
                }

                if (TYPE == LineType::LINE_WITH_SQUARE) {
                    fl_rectf(X3 - WIDTH1 - 3, Y3 - WIDTH1 - 3, WIDTH1 * 2 + 6, WIDTH1 * 2 + 6);
                }
            }
            else if (TYPE == LineType::VECTOR) {
                if (x2 == 10'000) { // Draw every two x,y pairs.
                    x2 = X3;
                    y2 = Y3;
                }
                else {
                    fl_line_style(FL_SOLID, WIDTH1);
                    fl_line(X3, Y3, x2, y2);
                    x2 = 10'000;
                }
            }
            else if (TYPE == LineType::CIRCLE) {
                fl_line_style(FL_SOLID, WIDTH1 / 25 + 1);
                fl_begin_loop();
                fl_circle(X3, Y3 - 1, WIDTH2);
                fl_end_loop();
            }
            else if (TYPE == LineType::FILLED_CIRCLE) {
                fl_draw_circle(X3 - WIDTH2, Y3 - WIDTH2 - 1, WIDTH1, col);
            }
            else if (TYPE == LineType::SQUARE) {
                fl_rectf(X3 - WIDTH2, Y3 - WIDTH2 - 1, WIDTH1, WIDTH1);
            }

            data_c++;
            fl_line_style(0);
        }
    }
}

/** @brief Draw tooltip rectangle with text.
*
*/
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

    const int H = _CH * 4 + _CH * 0.5;
    const int W = _CH * 18;

    fl_color(FL_BACKGROUND2_COLOR);
    fl_line_style(FL_SOLID, 1);
    fl_rectf(x1, y1, W, H);

    fl_color(FL_FOREGROUND_COLOR);
    fl_line_style(FL_SOLID, 1);
    fl_rect(x1, y1, W, H);

    fl_line_style(FL_SOLID, 1);
    fl_line(Fl::event_x(), _area.y(), Fl::event_x(), _area.b() - 1);
    fl_line(_area.x(), Fl::event_y(), _area.r() - 1, Fl::event_y());

    fl_draw(_tooltip.c_str(), x1 + 2, y1, W - 4, H, FL_ALIGN_LEFT | FL_ALIGN_CENTER | FL_ALIGN_INSIDE | FL_ALIGN_CLIP | FL_ALIGN_TOP);

    fl_line_style(0);
}

/** @brief Draw x labels.
*
*/
void Plot::_draw_xlabels() {
    fl_font(flw::PREF_FIXED_FONT, _CH);

    double       x1     = _area.x();
    const int    Y      = _area.b();
    const int    R      = x() + w();
    const double X_INC  = _x.pixel() * _x.tick();
    double       x_val  = _x.min();
    double       x_last = -10'000;
    const int    TW     = _x.label_width() / 2;

    while (x1 < _area.r()) {
        if (x1 > (x_last + TW) && (x1 + TW + _CW) < R) {
            auto label = util::format_double(x_val, _x.fr(), '\'');

            if (static_cast<int>(x1 + 1) == (_area.r() - 1)) { // Adjust so rightmost pixel is aligned.
                x1 += 1.0;
            }

            if (_vertical == true) {
                fl_color(fl_color_average(FL_FOREGROUND_COLOR, FL_BACKGROUND2_COLOR, 0.2));
                fl_line(x1, _area.y() + 1, x1, _area.b() - 2);
            }

            _FLW_PLOT_DEBUG(fl_rect(x1 - TW, Y + _TICK_SIZE * 2, TW * 2, _CH, FL_FOREGROUND_COLOR))
            fl_color(FL_FOREGROUND_COLOR);
            fl_line(x1, Y, x1, Y + _TICK_SIZE);
            fl_draw(label.c_str(), x1 - TW, Y + _TICK_SIZE * 2, TW * 2, _CH, FL_ALIGN_CENTER | FL_ALIGN_INSIDE);
            x_last = x1 + TW + _CH;
        }

        x1    += X_INC;
        x_val += _x.tick();
    }
}

/** @brief Draw custom x labels.
*
*/
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

            fl_line(X, _area.b(), X, _area.b() + _TICK_SIZE);

            if (X - W >= x_last && label != "") {
                _FLW_PLOT_DEBUG(fl_rect(X - W, _area.b() + _TICK_SIZE * 2, W * 2, _CH))
                fl_draw(label.c_str(), X - W, _area.b() + _TICK_SIZE * 2, W * 2, _CH, FL_ALIGN_CENTER | FL_ALIGN_INSIDE | FL_ALIGN_CLIP);
                x_last = X + W + _TICK_SIZE;
            }
        }
    }
}

/** @brief Draw y labels.
*
*/
void Plot::_draw_ylabels() {
    fl_font(flw::PREF_FIXED_FONT, _CH);

    const int X      = x() + (_y.label() != "" ? _CH * 2 : 0);
    const int W      = _area.x() - X - _TICK_SIZE * 3;
    const int CH     = _CH / 2;
    double    y1     = _area.b() - 1.0;
    double    y_tick = _y.tick();
    double    y_inc  = _y.pixel() * y_tick;
    double    y_val  = _y.min();
    double    y_last = 10'000;

    while (static_cast<int>(y1 + 0.5) >= _area.y()) {
        if (y_last > y1) {
            auto label = util::format_double(y_val, _y.fr(), '\'');

            if (y1 < static_cast<double>(_area.y())) { // Adjust top most pixel.
                y1 = _area.y();
            }

            if (_horizontal == true) {
                fl_color(fl_color_average(FL_FOREGROUND_COLOR, FL_BACKGROUND2_COLOR, 0.2));
                fl_line(_area.x() + 1, y1, _area.r() - 2, y1);
            }

            fl_color(FL_FOREGROUND_COLOR);
            fl_line(_area.x(), y1, _area.x() - _TICK_SIZE, y1);
            fl_draw(label.c_str(), X + _TICK_SIZE, y1 - CH, W, _CH, FL_ALIGN_RIGHT | FL_ALIGN_INSIDE | FL_ALIGN_CLIP);
            _FLW_PLOT_DEBUG(fl_rect(X + _TICK_SIZE, y1 - CH, W, _CH, FL_FOREGROUND_COLOR))
            y_last = y1 - _CH - _TICK_SIZE;
        }

        y1    -= y_inc;
        y_val += y_tick;
    }
}

/** @brief Draw custom y labels.
*
*/
void Plot::_draw_ylabels_custom() {
    fl_color(FL_FOREGROUND_COLOR);
    fl_font(flw::PREF_FIXED_FONT, _CH);

    const int  X       = x() + (_y.label() != "" ? _CH * 2 : _TICK_SIZE);
    const int  W       = _area.x() - X - _TICK_SIZE * 3;
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

            fl_line(_area.x(), Y, _area.x() - _TICK_SIZE, Y);

            if (y_last > Y && label != "") {
                fl_draw(label.c_str(), X + _TICK_SIZE, Y - CH, W, fl_height(), FL_ALIGN_LEFT | FL_ALIGN_INSIDE | FL_ALIGN_CLIP);
                _FLW_PLOT_DEBUG(fl_rect(X + _TICK_SIZE, Y - CH, W, fl_height(), FL_FOREGROUND_COLOR))
                y_last = Y - fl_height() - _TICK_SIZE;
            }
        }
    }
}

/** @brief Take care of mouse events.
*
* Show tooltip on left click and drag.\n
* Show menu on right click.\n
* Select current line with middle click on line label.\n
*
* @param[in] event  Event.
*
* @return 1 or 0.
*/
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
    else if (event == FL_DRAG) { // FL_PUSH must have been called before mouse drag.
        _create_tooltip();

        if (_tooltip != "") {
            return 1;
        }
    }
    else if (event == FL_MOVE) { // Remove tooltip if mouse is outside chart area.
        if (_tooltip != "") {
            _tooltip = "";
            redraw();
        }
    }

    return Fl_Group::handle(event);
}

/** @brief Initiate widget.
*
* @param[in] new_data  Set to true if new data has been added.
*/
void Plot::_init(bool new_data) {
    fl_font(flw::PREF_FIXED_FONT, flw::PREF_FIXED_FONTSIZE);
    *const_cast<int*>(&_CW) = fl_width("X");
    *const_cast<int*>(&_CH) = flw::PREF_FIXED_FONTSIZE;

    if (new_data == true) { // Call Scale::calc() twice when new data has changed, first with default width.
        _calc_min_max();
        _x.calc_tick(w() - _CH * 6);
        _y.calc_tick(h() - _CH * 6);
        _x.measure_labels(_CW, _lines.size() > 0 ? _lines[0].is_vector() : false);
        _y.measure_labels(_CW, _lines.size() > 0 ? _lines[0].is_vector() : false);
    }

    auto move_x   = (_y.label() != "") ? _CH * 2 : 0;
    auto shrink_h = (_x.label() != "") ? _CH * 2 + _TICK_SIZE * 2 : _CH;

    _area.x(x() + (_CH * 2) + _y.label_width() + move_x);
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

/** @brief Load a complete plot view from json file.
*
* @return True if ok.
*/
bool Plot::load_json() {
    auto filename = util::to_string(fl_file_chooser("Select JSON File", "All Files (*)\tJSON Files (*.json)", ""));

    if (util::is_empty(filename) == true) {
        return false;
    }

    return load_json(filename);
}

/** @brief Load a complete plot view from json file.
*
* @param[in] filename  JSON file name.
*
* @return True if ok.
*/
bool Plot::load_json(const std::string& filename) {
#define _FLW_PLOT_ERROR(X) { dlg::msg_alert("JSON Error", util::format("Illegal plot value at pos %u", (X)->pos())); reset(); return false; }
    _filename = "";

    reset();
    redraw();

    auto wc  = WaitCursor();
    auto buf = gnu::file::read(filename);

    if (buf.c_str() == nullptr) {
        dlg::msg_alert("JSON Error", util::format("Failed to load %s", filename.c_str()));
        return false;
    }

    auto   js       = gnu::json::decode(buf.c_str(), buf.size());
    auto   x        = Scale();
    auto   y        = Scale();
    double clamp[4] = { INFINITY, INFINITY, INFINITY, INFINITY };

    if (js.has_err() == true) {
        dlg::msg_alert("JSON Error", util::format("Failed to parse %s (%s)", filename.c_str(), js.err_c()));
        return false;
    }

    if (js.is_object() == false) _FLW_PLOT_ERROR(&js);

    for (auto j : js.vo_to_va()) {
        if (j->name() == "flw::plot" && j->is_object() == true) {
            for (const auto j2 : j->vo_to_va()) {
                if (j2->name() == "version" && j2->is_number() == true) {
                    if (j2->vn_i() != plot::VERSION) _FLW_PLOT_ERROR(j2)
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
        else if ((j->name() == "flw::plot::scale::x" || j->name() == "flw::plot::scale::y") && j->is_object() == true) {
            auto& scale = (j->name() == "Scale::x") ? x : y;
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
        else if (j->name() == "flw::plot::line" && j->is_array() == true) {
            for (const auto j2 : *j->va()) {
                if (j2->is_object() == true) {
                    Line line;
                    PointVector data;

                    for (auto j3 : j2->vo_to_va()) {
                        if (j3->name() == "color" && j3->is_number() == true)      line.set_color((Fl_Color) j3->vn_i());
                        else if (j3->name() == "label" && j3->is_string() == true) line.set_label(j3->vs_u());
                        else if (j3->name() == "type" && j3->is_string() == true)  line.set_type_from_string(j3->vs());
                        else if (j3->name() == "visible" && j3->is_bool() == true) line.set_visible(j3->vb());
                        else if (j3->name() == "width" && j3->is_number() == true) line.set_width((unsigned) j3->vn_i());
                        else if (j3->name() == "xy" && j3->is_array() == true) {
                            for (auto p : *j3->va()) {
                                if (p->is_array() == true && p->size() == 2 && (*p)[0]->is_number() == true && (*p)[1]->is_number() == true) {
                                    data.push_back(Point((*p)[0]->vn(), (*p)[1]->vn()));
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
        else _FLW_PLOT_ERROR(j)
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
    _filename = filename;

    return true;
}

/** @brief Load a plot line from csv file.
*
* @return True if ok.
*/
bool Plot::load_line_from_csv() {
    if (_lines.size() >= plot::MAX_LINES) {
        dlg::msg_alert("Plot Error", "Max line count reached!");
        return false;
    }

    auto filename = util::to_string(fl_file_chooser("Select CSV File", "All Files (*)\tCSV Files (*.csv)", ""));

    if (util::is_empty(filename) == true) {
        return false;
    }

    auto vec = Point::LoadCSV(filename);

    if (vec.size() == 0) {
        dlg::msg_alert("Plot Error", "To few data values!");
        return false;
    }

    auto line = Line(vec, filename);
    _LineSetup(top_window(), line).run();
    _lines.push_back(line);
    init_new_data();

    return true;
}

/** @brief Print view to postscript.
*
*/
void Plot::print_to_postscript() {
    _printing = true;
    dlg::print("Print Plot", Plot::_CallbackPrinter, this, 1, 1);
    _printing = false;
    redraw();
}

/** @brief Clear all data.
*
*/
void Plot::reset() {
    *const_cast<int*>(&_CW) = flw::PREF_FIXED_FONTSIZE;
    *const_cast<int*>(&_CH) = flw::PREF_FIXED_FONTSIZE;

    _area           = Fl_Rect();
    _filename       = "";
    _old            = Fl_Rect();
    _selected_line  = 0;
    _selected_point = -1;
    _x              = Scale();
    _y              = Scale();

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

/** @brief Resize widget.
*
* @param[in] X  X pos.
* @param[in] Y  Y pos.
* @param[in] W  Width.
* @param[in] H  Height.
*/
void Plot::resize(int X, int Y, int W, int H) {
    if (_old.w() == W && _old.h() == H) {
        return;
    }

    Fl_Widget::resize(X, Y, W, H);
    _old = Fl_Rect(this);
    init();
}

/** @brief Save complete plot view to json file.
*
* @return True if ok.
*/
bool Plot::save_json() {
    auto filename = util::to_string(fl_file_chooser("Save To JSON File", "All Files (*)\tJSON Files (*.json)", ""));

    if (util::is_empty(filename) == true) {
        return false;
    }

    return save_json(filename);
}

/** @brief Save complete plot view to json file.
*
* @param[in] filename  Destination filename.
*
* @return True if ok.
*/
bool Plot::save_json(const std::string& filename) {
    auto wc  = WaitCursor();
    auto jsb = gnu::json::Builder();

    try {
        jsb << gnu::json::Builder::MakeObject();
            jsb << gnu::json::Builder::MakeObject("flw::plot");
                jsb << gnu::json::Builder::MakeNumber(plot::VERSION, "version");
                jsb << gnu::json::Builder::MakeString(_label, "label");
                jsb << gnu::json::Builder::MakeBool(_labels, "labels");
                jsb << gnu::json::Builder::MakeBool(_horizontal, "horizontal");
                jsb << gnu::json::Builder::MakeBool(_vertical, "vertical");
                if (std::isfinite(_min_x) == true) jsb << gnu::json::Builder::MakeNumber(_min_x, "minx");
                if (std::isfinite(_max_x) == true) jsb << gnu::json::Builder::MakeNumber(_max_x, "maxx");
                if (std::isfinite(_min_y) == true) jsb << gnu::json::Builder::MakeNumber(_min_y, "miny");
                if (std::isfinite(_max_y) == true) jsb << gnu::json::Builder::MakeNumber(_max_y, "maxy");
            jsb.end();

            jsb << gnu::json::Builder::MakeObject("flw::plot::scale::x");
                jsb << gnu::json::Builder::MakeString(_x.label().c_str(), "label");
                jsb << gnu::json::Builder::MakeNumber(_x.color(), "color");
                jsb << gnu::json::Builder::MakeArrayInline("labels");
                    for (const auto& l : _x.custom_labels()) jsb << gnu::json::Builder::MakeString(l);
                jsb.end();
            jsb.end();

            jsb << gnu::json::Builder::MakeObject("flw::plot::scale::y");
                jsb << gnu::json::Builder::MakeString(_y.label().c_str(), "label");
                jsb << gnu::json::Builder::MakeNumber(_y.color(), "color");
                jsb << gnu::json::Builder::MakeArrayInline("labels");
                    for (const auto& l : _y.custom_labels()) jsb << gnu::json::Builder::MakeString(l);
                jsb.end();
            jsb.end();

            jsb << gnu::json::Builder::MakeArray("flw::plot::line");

            for (const auto& line : _lines) {
                if (line.data().size() > 0) {
                    jsb << gnu::json::Builder::MakeObject();
                        jsb << gnu::json::Builder::MakeNumber(line.width(), "width");
                        jsb << gnu::json::Builder::MakeNumber(line.color(), "color");
                        jsb << gnu::json::Builder::MakeString(line.label(), "label");
                        jsb << gnu::json::Builder::MakeBool(line.is_visible(), "visible");
                        jsb << gnu::json::Builder::MakeString(line.type_to_string(), "type");
                        jsb << gnu::json::Builder::MakeArray("xy");
                            for (const auto& point : line.data()) {
                                if (std::isfinite(point.x) == true && std::isfinite(point.y) == true) {
                                    jsb << gnu::json::Builder::MakeArrayInline();
                                        jsb << gnu::json::Builder::MakeNumber(point.x);
                                        jsb << gnu::json::Builder::MakeNumber(point.y);
                                    jsb.end();
                                }
                            }
                        jsb.end();
                    jsb.end();
                }
            }
            jsb.end();

        auto js  = jsb.encode();
        auto res = gnu::file::write(filename, js.c_str(), js.length());

        if (res == true) {
            _filename = filename;
        }

        return res;
    }
    catch(const std::string& e) {
        dlg::msg_alert("JSON Error", util::format("Failed to encode json\n%s", e.c_str()));
        return false;
    }
}

/** @brief Save plot line values to csv file.
*
* @return True if a file was saved.
*/
bool Plot::save_line_to_csv() {
    if (_lines.size() == 0 || _selected_line >= _lines.size()) {
        return false;
    }

    auto filename = util::to_string(fl_file_chooser("Save To CSV File", "All Files (*)\tCSV Files (*.csv)", ""));

    if (util::is_empty(filename) == true) {
        return false;
    }

    return Point::SaveCSV(_lines[_selected_line].data(), filename);
}

/** @brief Save view to an png file.
*
* @return True if ok.
*/
bool Plot::save_png() {
    return flw::util::png_save(window(), "", x(),  y(),  w(),  h());
}

/** @brief Ask for min or max clamp value.
*
* @param[in] clamp  What clamp value to set.
*/
void Plot::setup_clamp(Clamp clamp) {
    double      val = 0.0;
    std::string info;

    switch (clamp) {
        case plot::Clamp::MINX:
            val = _min_x;
            info = "Set min X value or press cancel to remove.";
            break;

        case plot::Clamp::MAXX:
            val = _max_x;
            info = "Set max X value or press cancel to remove.";
            break;

        case plot::Clamp::MINY:
            val = _min_y;
            info = "Set min Y value or press cancel to remove.";
            break;

        case plot::Clamp::MAXY:
            val = _max_y;
            info = "Set max Y value or press cancel to remove.";
            break;
    }

    auto answer = flw::dlg::input_double("Plot", info, val);

    if (answer == flw::label::CANCEL) {
        val = INFINITY;
    }

    switch (clamp) {
        case plot::Clamp::MINX:
            _min_x = val;
            break;

        case plot::Clamp::MAXX:
            _max_x = val;
            break;

        case plot::Clamp::MINY:
            _min_y = val;
            break;

        case plot::Clamp::MAXY:
            _max_y = val;
            break;
    }

    init_new_data();
}

/** @brief Show dialog to add a new line using current selected line.
*
*/
void Plot::setup_create_line() {
    if (_selected_line >= _lines.size()) {
        dlg::msg_alert("Plot Error", "Line has not been selected!");
        return;
    }
    else if (_lines.size() >= plot::MAX_LINES) {
        dlg::msg_alert("Plot Error", "Max line count reached!");
        return;
    }

    auto list = StringVector() = {
        "Modify",
        "Swap values",
    };

    switch (dlg::select_choice("Select Formula", list, 0)) {
        case 0:
            create_line(Algorithm::MODIFY);
            break;
        case 1:
            create_line(Algorithm::SWAP);
            break;
        default:
            break;
    }
}

/** @brief Show an dialog to select lines to delete.
*
*/
void Plot::setup_delete_lines() {
    if (_lines.size() == 0) {
        return;
    }

    auto list = _create_check_labels(false);
    list = dlg::select_checkboxes("Delete Lines", list);

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

/** @brief Ask user for plot labels.
*
* Main label, x label or y label.
*
* @param[in] val  What label to set.
*/
void Plot::setup_label(Label val) {
    std::string l, answer;

    switch (val) {
        case plot::Label::MAIN:
            l = _label;
            answer = flw::dlg::input("Plot", "Enter main label", l);
            break;

        case plot::Label::X:
            l = _x.label();
            answer = flw::dlg::input("Plot", "Enter X label", l);
            break;

        case plot::Label::Y:
            l = _y.label();
            answer = flw::dlg::input("Plot", "Enter Y label", l);
            break;
    }

    if (answer == flw::label::CANCEL) {
        return;
    }

    switch (val) {
        case plot::Label::MAIN:
            _label = l;
            break;

        case plot::Label::X:
            _x.set_label(l);
            break;

        case plot::Label::Y:
            _y.set_label(l);
            break;
    }

    init();
}

/** @brief Show and property dialog for current line.
*
*/
void Plot::setup_line() {
    if (_lines.size() == 0 || _selected_line >= _lines.size()) {
        return;
    }

    auto& line = _lines[_selected_line];
    auto ok    = _LineSetup(top_window(), line).run();

    if (ok == false) {
        return;
    }

    redraw();
}

/** @brief Show an dialog for showing or hiding lines.
*
*/
void Plot::setup_show_or_hide_lines() {
    if (_lines.size() == 0) {
        return;
    }

    auto list = _create_check_labels(true);
    list = dlg::select_checkboxes("Show Or Hide Lines", list);

    if (list.size() == 0) {
        return;
    }

    for (size_t f = 0; f < list.size(); f++) {
        _lines[f].set_visible(list[f][0] == '1');
    }

    init_new_data();
}

/** @brief Configure some view options from menu checkboxes.
*
* Show/hide labels.
* Show/hide vertical lines.
* Show/hide horizontal lines.
*/
void Plot::setup_view_options() {
    _labels     = menu::item_value(_menu, _LABEL_SETUP_LABELS);
    _horizontal = menu::item_value(_menu, _LABEL_SETUP_HLINES);
    _vertical   = menu::item_value(_menu, _LABEL_SETUP_VLINES);

    redraw();
}

/** @brief Show menu
*
*/
void Plot::_show_menu() {
    if (_disable_menu == true) {
        return;
    }

    menu::set_item(_menu, _LABEL_SETUP_LABELS, _labels);
    menu::set_item(_menu, _LABEL_SETUP_HLINES, _horizontal);
    menu::set_item(_menu, _LABEL_SETUP_VLINES, _vertical);

    if (_lines.size() == 0) {
        menu::enable_item(_menu, _LABEL_ADD_LINE, false);
        menu::enable_item(_menu, _LABEL_SAVE_CVS, false);
        menu::enable_item(_menu, _LABEL_SETUP_DELETE, false);
        menu::enable_item(_menu, _LABEL_SETUP_LINE, false);
        menu::enable_item(_menu, _LABEL_SETUP_MAXX, false);
        menu::enable_item(_menu, _LABEL_SETUP_MAXY, false);
        menu::enable_item(_menu, _LABEL_SETUP_MINX, false);
        menu::enable_item(_menu, _LABEL_SETUP_MINY, false);
        menu::enable_item(_menu, _LABEL_SETUP_SHOW, false);
    }
    else {
        menu::enable_item(_menu, _LABEL_ADD_LINE, true);
        menu::enable_item(_menu, _LABEL_SAVE_CVS, true);
        menu::enable_item(_menu, _LABEL_SETUP_DELETE, true);
        menu::enable_item(_menu, _LABEL_SETUP_LINE, true);
        menu::enable_item(_menu, _LABEL_SETUP_MAXX, true);
        menu::enable_item(_menu, _LABEL_SETUP_MAXY, true);
        menu::enable_item(_menu, _LABEL_SETUP_MINX, true);
        menu::enable_item(_menu, _LABEL_SETUP_MINY, true);
        menu::enable_item(_menu, _LABEL_SETUP_SHOW, true);
    }

    _menu->popup();
}

/** @brief Update font properties
*
*/
void Plot::update_pref() {
    _menu->textfont(flw::PREF_FONT);
    _menu->textsize(flw::PREF_FONTSIZE);
}

} // plot
} // flw

// MKALGAM_OFF
