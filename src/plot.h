/**
* @file
* @brief Plot widget.
*
* @author gnuwimp@gmail.com
* @copyright Released under the GNU General Public License v3.0
*/

#ifndef FLW_PLOT_H
#define FLW_PLOT_H

#include "flw.h"

// MKALGAM_ON

#include <cmath>
#include <FL/Fl_Group.H>
#include <FL/Fl_Rect.H>

class Fl_Menu_Button;

namespace flw {

/** @brief Plot widget and related classes.
*
*/
namespace plot {

static constexpr const double MAX_VALUE      = 9223372036854775807.0;   ///< @brief Max number value.
static constexpr const double MIN_VALUE      = 0.0000001;               ///< @brief Min (abs) number value.
static constexpr const size_t MAX_LINES      = 10;                      ///< @brief Max number of plot lines.
static constexpr const int    MAX_LINE_WIDTH = 30;                      ///< @brief Max line width (pixels).
static constexpr const int    VERSION        =  4;                      ///< @brief JSON version.

/** @brief Algorithms that can be used to generate new data series.
*
*/
enum class Algorithm {
    MODIFY,         ///< @brief Modify data serie.
    SWAP,           ///< @brief Swap X and Y.
    LAST = SWAP,    ///< @brief Same as SWAP.
};

/** @brief Set different clamp values.
*
*/
enum class Clamp {
    MINX,           ///< @brief Set min X clamp.
    MAXX,           ///< @brief Set max X clamp.
    MINY,           ///< @brief Set min Y clamp.
    MAXY,           ///< @brief Set max Y clamp.
    LAST = MAXY,    ///< @brief Same as MAXY.
};

/** @brief Different labels.
*
*/
enum class Label {
    MAIN,       ///< @brief Set main label.
    X,          ///< @brief Set X label.
    Y,          ///< @brief Set Y label.
    LAST = Y,   ///< @brief Same as Y.
};

/** @brief Plot line types.
*
*/
enum class LineType {
    LINE,                   ///< @brief Simple line.
    LINE_DASH,              ///< @brief Dashed line.
    LINE_DOT,               ///< @brief Dotted line.
    LINE_WITH_SQUARE,       ///< @brief Line with a square.
    VECTOR,                 ///< @brief A vector.
    CIRCLE,                 ///< @brief A circle.
    FILLED_CIRCLE,          ///< @brief Filled circle.
    SQUARE,                 ///< @brief A square.
    LAST = SQUARE,          ///< @brief Same as SQUARE.
};

/** @brief Modify a data serie.
*
*/
enum class Modifier {
    ADDITION,               ///< @brief Add to values.
    SUBTRACTION,            ///< @brief Subtract values.
    MULTIPLICATION,         ///< @brief Multiply values.
    DIVISION,               ///< @brief Divide values.
    LAST = DIVISION,        ///< @brief Same as DIVISION.
};

/** @brief Which value to modify.
*
*/
enum class Target {
    X,                  ///< @brief Change X.
    Y,                  ///< @brief Change Y.
    X_AND_Y,            ///< @brief Change X and Y.
    LAST = X_AND_Y,     ///< @brief Same as X_AND_Y.
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

struct Point;
typedef std::vector<Point> PointVector;     ///< @brief A vector with data points.

/** @brief Point class represent one data point with X and Y values.
*
*/
struct Point {
    double                      x;
    double                      y;

    explicit                    Point(double X = 0.0, double Y = 0.0);
    void                        debug(int i = -1) const;
    bool                        is_valid() const
                                    { return std::isfinite(x) == true && std::isfinite(y) == true && fabs(x) < plot::MAX_VALUE && fabs(y) < plot::MAX_VALUE; } ///< @brief Return true if point has valid values.

    static void                 Debug(const PointVector& in);
    static PointVector          LoadCSV(const std::string& filename, const std::string& sep = ",");
    static bool                 MinMax(const PointVector& in, double& min_x, double& max_x, double& min_y, double& max_y);
    static PointVector          Modify(const PointVector& in, plot::Modifier modify, plot::Target target, double value);
    static bool                 SaveCSV(const PointVector& in, const std::string& filename, const std::string& sep = ",");
    static PointVector          Swap(const PointVector& in);
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

/** @brief Data for one plot line.
*
*/
class Line {
public:
                                Line();
                                Line(const PointVector& data, const std::string& label, LineType type = LineType::LINE, Fl_Color color = FL_BLUE, unsigned width = 1);
    Fl_Color                    color() const
                                    { return _color; } ///< @brief Return line color.
    const PointVector&          data() const
                                    { return _data; } ///< @brief Return vector with points.
    void                        debug() const;
    bool                        has_line_type() const
                                    { return _type == LineType::LINE || _type == LineType::LINE_DASH || _type == LineType::LINE_DOT || _type == LineType::LINE_WITH_SQUARE; } ///< @brief Return true if line is some kind of line.
    bool                        has_radius() const
                                    { return _type == LineType::CIRCLE || _type == LineType::FILLED_CIRCLE || _type == LineType::SQUARE; } ///< @brief Return true if it is an circle or rectangle.
    bool                        is_vector() const
                                    { return _type == LineType::VECTOR; } ///< @brief Return true if line is an vector (it has two points).
    bool                        is_visible() const
                                    { return _visible; } ///< @brief Is it visible?.
    std::string                 label() const
                                    { return _label; } ///< @brief Return line label.
    const Fl_Rect&              label_rect() const
                                    { return _rect; } ///< @brief Return label rectangle.
    void                        reset();
    Line&                       set_color(Fl_Color val)
                                    { _color = val; return *this; } ///< @brief Set line color. @param[in] val  Color value. @return This object.
    Line&                       set_data(const PointVector& val)
                                    { _data = val;  return *this; } ///< @brief Set points. @param[in] val  Vector with points. @return This object.
    Line&                       set_label(const std::string& val)
                                    { _label = val; return *this; } ///< @brief Set line label. @param[in] val  Label value. @return This object.
    Line&                       set_label_rect(int x, int y, int w, int h)
                                    { _rect = Fl_Rect(x, y, w, h); return *this; } ///< @brief Set label rectangle. @return This object.
    Line&                       set_type(LineType val)
                                    { _type = val; return *this; } ///< @brief Set line type. @param[in] val  Type value. @return This object.
    Line&                       set_type_from_string(const std::string& val);
    Line&                       set_visible(bool val)
                                    { _visible = val; return *this; } ///< @brief Show or hide line. @param[in] val  Visible value. @return This object.
    Line&                       set_width(unsigned val)
                                    { if (val > 0 && val <= plot::MAX_LINE_WIDTH) _width = val; return *this; } ///< @brief Set line width. @param[in] val  Line width. @return This object.
    LineType                    type() const
                                    { return _type; } ///< @brief Get line type.
    std::string                 type_to_string() const;
    unsigned                    width() const
                                    { return _width; } ///< @brief Get line width.

private:
    bool                        _visible;   // Show/hide line.
    Fl_Color                    _color;     // Line color.
    Fl_Rect                     _rect;      // Label rectangle.
    LineType                    _type;      // Line type.
    PointVector                 _data;      // Vector with points.
    std::string                 _label;     // Line label.
    unsigned                    _width;     // Line width in pixels.
};

typedef std::vector<Line> LineVector;

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

/** @brief X and Y scale.
*
*/
class Scale {
public:

                                Scale()
                                    { reset(); } ///< @brief Clear all data.
    void                        calc_tick(double pixels);
    Fl_Color                    color() const
                                    { return _color; } ///< @brief Get label color.
    const StringVector&         custom_labels() const
                                    { return _labels; } ///< @brief Get vector with custom labels.
    void                        debug(const char* s) const;
    int                         fr() const
                                    { return _fr; } ///< @brief Get number of decimals.
    bool                        is_max_finite() const
                                    { return std::isfinite(_max); } ///< @brief Is max value not INFINITY?
    bool                        is_min_finite() const
                                    { return std::isfinite(_min); } ///< @brief Is min value not INFINITY?
    std::string                 label() const
                                    { return _label; } ///< @brief Scale label.
    int                         label_width() const
                                    { return static_cast<int>(_lwidth); } ///< @brief label width in pixels.
    double                      max() const
                                    { return _max; } ///< @brief Get max value.
    void                        measure_labels(int cw, bool custom);
    double                      min() const
                                    { return _min; } ///< @brief Get min value.
    double                      pixel() const
                                    { return _pixel; } ///< @brief Get value per pixel.
    void                        reset();
    void                        reset_min_max()
                                    { _min = _max = INFINITY; } ///< @brief Clear min and max values.
    void                        set_color(int val)
                                    { _color = val; } ///< @brief Set label color.
    void                        set_custom_labels(StringVector val)
                                    { _labels = val; } ///< @brief Set custom labels.
    void                        set_label(const std::string& val)
                                    { _label = val; } ///< @brief Set scale label that is below or to the left of scale values.
    void                        set_max(double val)
                                    { _max = val; } ///< @brief Set max value.
    void                        set_min(double val)
                                    { _min = val; } ///< @brief Set min value.
    double                      tick() const
                                    { return _tick; } ///< @brief Get value between every tick.

private:
    Fl_Color                    _color;     ///< @brief Label color.
    StringVector                _labels;    ///< @brief Custom labels.
    double                      _max;       ///< @brief Max scale value.
    double                      _min;       ///< @brief Min scale value.
    double                      _pixel;     ///< @brief Numerical value between every pixel.
    double                      _tick;      ///< @brief Value between every tick.
    int                         _fr;        ///< @brief Number of decimals for the value labels.
    size_t                      _lwidth;    ///< @brief Max label width.
    std::string                 _label;     ///< @brief Long label below or to the left of scale values.
};

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

/** @brief Plot widget for displaying numerical values.
*
* Custom x and y labels are only for LineType::VECTOR and for the first line.\n
* Also only a few can be drawn to screen (meant for weekdays or months or similar).\n
*
* @snippet plot.cpp flw::plot::Plot example
* @image html plot.png
*/
class Plot : public Fl_Group {
public:
    explicit                    Plot(int X = 0, int Y = 0, int W = 0, int H = 0, const char* l = nullptr);
    bool                        add_line(const Line& line);
    bool                        create_line(Algorithm alg);
    void                        debug() const;
    void                        debug_line() const;
    void                        delete_line(size_t index);
    void                        disable_menu()
                                    { _disable_menu = true; } ///< @brief Disable popup menu.
    void                        do_layout()
                                    { _old = Fl_Rect(); resize(x(), y(), w(), h()); redraw(); } ///< @brief Resize widget.
    void                        draw() override;
    void                        enable_menu()
                                    { _disable_menu = false; } ///< @brief Enable popup menu.
    std::string                 filename() const
                                    { return _filename; } ///< @brief Get last used filename.
    int                         handle(int event) override;
    void                        init()
                                    { _init(false); } ///< @brief Initiate data but don't calc min and max.
    void                        init_new_data()
                                    { _init(true); } ///< @brief Initiate data and calc min and max.
    bool                        load_json();
    bool                        load_json(const std::string& filename);
    bool                        load_line_from_csv();
    void                        print_to_postscript();
    void                        reset();
    void                        resize(int X, int Y, int W, int H) override;
    bool                        save_json();
    bool                        save_json(const std::string& filename);
    bool                        save_line_to_csv();
    bool                        save_png();
    void                        set_hor_lines(bool val = true)
                                    { _horizontal = val; } ///< @brief Turn on/off horizontal lines.
    void                        set_label(const std::string& label = "")
                                    { _label = label; } ///< @brief Set main label.
    void                        set_line_labels(bool val = true)
                                    { _labels = val; } ///< @brief Turn on/off line labels.
    void                        set_max_x(double val = INFINITY)
                                    { _max_x = val; } ///< @brief Set max X value.
    void                        set_max_y(double val = INFINITY)
                                    { _max_y = val; } ///< @brief Set max Y value.
    void                        set_min_x(double val = INFINITY)
                                    { _min_x = val; } ///< @brief Set min X value.
    void                        set_min_y(double val = INFINITY)
                                    { _min_y = val; } ///< @brief Set min Y value.
    void                        set_ver_lines(bool val = true)
                                    { _vertical = val; } ///< @brief Turn vertical lines on/off.
    void                        setup_create_line();
    void                        setup_clamp(Clamp clamp);
    void                        setup_delete_lines();
    void                        setup_label(Label val);
    void                        setup_line();
    void                        setup_show_or_hide_lines();
    void                        setup_view_options();
    void                        update_pref();
    Scale&                      xscale()
                                    { return _x; } ///< @brief Get X scale object.
    Scale&                      yscale()
                                    { return _y; } ///< @brief Get Y scale object.

private:
    void                        _calc_min_max();
    StringVector                _create_check_labels(bool def) const;
    void                        _create_tooltip();
    void                        _draw_labels();
    void                        _draw_line_labels();
    void                        _draw_lines();
    void                        _draw_tooltip();
    void                        _draw_xlabels();
    void                        _draw_xlabels_custom();
    void                        _draw_ylabels();
    void                        _draw_ylabels_custom();
    void                        _init(bool new_data);
    void                        _show_menu();

    static void                 _CallbackDebug(Fl_Widget*, void* widget);
    static bool                 _CallbackPrinter(void* data, int pw, int ph, unsigned page);
    static void                 _CallbackToggle(Fl_Widget*, void* widget);

    Fl_Menu_Button*             _menu;                  ///< @brief Menu object.
    Fl_Rect                     _area;                  ///< @brief Plot area rectangle.
    Fl_Rect                     _old;                   ///< @brief For skipping resizes.
    LineVector                  _lines;                 ///< @brief All plotlines.
    Scale                       _x;                     ///< @brief X scale object.
    Scale                       _y;                     ///< @brief Y scale object.
    bool                        _disable_menu;          ///< @brief True to disable popup menu.
    bool                        _horizontal;            ///< @brief True to show horizontal lines
    bool                        _labels;                ///< @brief True to draw line labels.
    bool                        _printing;              ///< @brief Used when printing.
    bool                        _vertical;              ///< @brief True to show vertical lines.
    const int                   _CH;                    ///< @brief Character height in pixels.
    const int                   _CW;                    ///< @brief Character width in pixels.
    double                      _max_x;                 ///< @brief Max X value.
    double                      _max_y;                 ///< @brief Max Y value.
    double                      _min_x;                 ///< @brief Min X value.
    double                      _min_y;                 ///< @brief Min Y value.
    size_t                      _selected_line;         ///< @brief Selected line index in vector.
    size_t                      _selected_point;        ///< @brief Selected point index in selected line.
    std::string                 _filename;              ///< @brief Last used filename.
    std::string                 _label;                 ///< @brief Main label.
    std::string                 _tooltip;               ///< @brief Tooltip text with numerical values.

};

} // plot
} // flw

// MKALGAM_OFF

#endif
