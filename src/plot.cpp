// Copyright gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#include "plot.h"
#include "flw.h"
#include "waitcursor.h"
#include "json.h"

// MKALGAM_ON

#include <assert.h>
#include <math.h>
#include <FL/fl_ask.H>
#include <FL/fl_draw.H>

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

static const char* const        _PLOT_SHOW_LABELS = "Show line labels";
static const char* const        _PLOT_SHOW_HLINES = "Show horizontal lines";
static const char* const        _PLOT_SHOW_VLINES = "Show vertical lines";
static const char* const        _PLOT_SAVE_FILE   = "Save to png file...";

//------------------------------------------------------------------------------
static int _plot_count_decimals(double number) {
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
static std::string _plot_format_double(double num, int decimals, char del) {
    char res[100];

    *res = 0;

    if (decimals < 0) {
        decimals = _plot_count_decimals(num);
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
static bool _plot_has_pairs(Plot::TYPE type) {
    return type != Plot::CIRCLE && type != Plot::FILLED_CIRCLE && type != Plot::SQUARE;
}

//----------------------------------------------------------------------
static bool _plot_has_radius(Plot::TYPE type) {
    return type == Plot::CIRCLE || type == Plot::FILLED_CIRCLE || type == Plot::SQUARE;
}

//------------------------------------------------------------------------------
//static void _plot_print(const PointVector& points) {
//    auto c = 1;
//
//    for (auto& p : points) {
//        fprintf(stderr, "%3d| X=%16.6f   Y=%16.6f\n", c++, p.x, p.y);
//    }
//
//    fflush(stderr);
//}

//------------------------------------------------------------------------------
static Plot::TYPE _plot_string_to_type(std::string name) {
    if (name == "LINE_DASH") return Plot::LINE_DASH;
    else if (name == "LINE_DOT") return Plot::LINE_DOT;
    else if (name == "LINE_WITH_SQUARE") return Plot::LINE_WITH_SQUARE;
    else if (name == "VECTOR") return Plot::VECTOR;
    else if (name == "CIRCLE") return Plot::CIRCLE;
    else if (name == "FILLED_CIRCLE") return Plot::FILLED_CIRCLE;
    else if (name == "SQUARE") return Plot::SQUARE;
    else return Plot::LINE;
}

//------------------------------------------------------------------------------
static std::string _plot_type_to_string(Plot::TYPE type) {
    assert((size_t) type <= 8);
    static const char* NAMES[] = { "LINE", "LINE_DASH", "LINE_DOT", "LINE_WITH_SQUARE", "VECTOR", "CIRCLE", "FILLED_CIRCLE", "SQUARE", "", "", };
    return NAMES[(size_t) type];
}

/***
 *      _____  _       _
 *     |  __ \| |     | |     /\
 *     | |__) | | ___ | |_   /  \   _ __ ___  __ _
 *     |  ___/| |/ _ \| __| / /\ \ | '__/ _ \/ _` |
 *     | |    | | (_) | |_ / ____ \| | |  __/ (_| |
 *     |_|    |_|\___/ \__/_/    \_\_|  \___|\__,_|
 *
 *
 */

//----------------------------------------------------------------------
struct PlotArea {
    int                         x;
    int                         y;
    int                         w;
    int                         h;

    explicit                    PlotArea(int x = 0, int y = 0, int w = 0, int h = 0) {
                                    this->x = x;
                                    this->y = y;
                                    this->w = w;
                                    this->h = h;
                                }
    int                         x2() const
                                    { return x + w; }
    int                         y2() const
                                    { return y + h; }
};

/***
 *      _____  _       _   _      _
 *     |  __ \| |     | | | |    (_)
 *     | |__) | | ___ | |_| |     _ _ __   ___
 *     |  ___/| |/ _ \| __| |    | | '_ \ / _ \
 *     | |    | | (_) | |_| |____| | | | |  __/
 *     |_|    |_|\___/ \__|______|_|_| |_|\___|
 *
 *
 */

//----------------------------------------------------------------------
// Data for one line.
//
struct PlotLine {
    Fl_Color                    color;
    unsigned                    width;
    bool                        visible;
    std::string                 label;
    PointVector                 points;
    Plot::TYPE                  type;
                                PlotLine();
    void                        clear();
};

//------------------------------------------------------------------------------
PlotLine::PlotLine() {
    clear();
}

//------------------------------------------------------------------------------
void PlotLine::clear() {
    color   = FL_BLUE;
    type    = Plot::LINE;
    visible = true;
    width   = 1;

    points.clear();
}

/***
 *      _____  _       _    _____           _
 *     |  __ \| |     | |  / ____|         | |
 *     | |__) | | ___ | |_| (___   ___ __ _| | ___
 *     |  ___/| |/ _ \| __|\___ \ / __/ _` | |/ _ \
 *     | |    | | (_) | |_ ____) | (_| (_| | |  __/
 *     |_|    |_|\___/ \__|_____/ \___\__,_|_|\___|
 *
 *
 */

//----------------------------------------------------------------------
// One scale for each axis.
//
struct PlotScale {
    Fl_Color                    color;
    int                         fr;
    std::string                 label;
    flw::StringVector           labels;
    double                      max;
    double                      max2;
    double                      min;
    double                      min2;
    double                      pixel;
    int                         text;
    double                      tick;

                                PlotScale();
    void                        calc(double canvas_size);
    void                        debug(const char* s) const;
    void                        measure_text(int cw);
    void                        reset_min_max();
};

//------------------------------------------------------------------------------
PlotScale::PlotScale() {
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

//------------------------------------------------------------------------------
void PlotScale::calc(double canvas_size) {
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

    fr = _plot_count_decimals(tick);
}

//------------------------------------------------------------------------------
void PlotScale::debug(const char* s) const {
#ifdef DEBUG
    fprintf(stderr, "\tPlotScale: %s\n", s);
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

//------------------------------------------------------------------------------
void PlotScale::measure_text(int cw) {
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
        const auto l1  = _plot_format_double(min, dec, ' ');
        const auto l2  = _plot_format_double(max, dec, ' ');

        text = (l1.length() > l2.length()) ? l1.length() * cw : l2.length() * cw;
    }
}

//------------------------------------------------------------------------------
void PlotScale::reset_min_max() {
    min = min2 = Plot::MAX;
    max = max2 = Plot::MIN;
}

/***
 *      _____  _       _
 *     |  __ \| |     | |
 *     | |__) | | ___ | |_
 *     |  ___/| |/ _ \| __|
 *     | |    | | (_) | |_
 *     |_|    |_|\___/ \__|
 *
 *
 */

//------------------------------------------------------------------------------
Plot::Plot(int X, int Y, int W, int H, const char* l) : Fl_Group(X, Y, W, H, l) {
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

    _menu->add(_PLOT_SHOW_LABELS, 0, Plot::_CallbackToggle, this, FL_MENU_TOGGLE);
    _menu->add(_PLOT_SHOW_HLINES, 0, Plot::_CallbackToggle, this, FL_MENU_TOGGLE);
    _menu->add(_PLOT_SHOW_VLINES, 0, Plot::_CallbackToggle, this, FL_MENU_TOGGLE | FL_MENU_DIVIDER);
    _menu->add(_PLOT_SAVE_FILE, 0, Plot::_CallbackSave, this);
#ifdef DEBUG
    _menu->add("Debug", 0, Plot::_CallbackDebug, this);
#endif
    _menu->type(Fl_Menu_Button::POPUP3);

    for (size_t f = 0; f < Plot::MAX_LINES; f++) {
        _lines[f] = new PlotLine();
    }

    _x    = nullptr;
    _y    = nullptr;
    _area = new PlotArea();

    clear();
    update_pref();
    view_options();
}

//------------------------------------------------------------------------------
Plot::~Plot() {
    delete _area;
    delete _x;
    delete _y;

    for (size_t f = 0; f < Plot::MAX_LINES; f++) {
        delete _lines[f];
    }
}

//------------------------------------------------------------------------------
bool Plot::add_line(const PointVector& points, TYPE type, unsigned width, std::string label, Fl_Color color) {
    if (_size >= Plot::MAX_LINES || width == 0 || width > 100) {
        return false;
    }

    _lines[_size]->points    = points;
    _lines[_size]->label     = label;
    _lines[_size]->width     = width;
    _lines[_size]->color     = color;
    _lines[_size]->type      = type;
    _lines[_size++]->visible = true;
    _calc                    = true;

    return true;
}

//------------------------------------------------------------------------------
void Plot::_calc_min_max() {
    _x->reset_min_max();
    _y->reset_min_max();

    for (size_t f = 0; f < _size; f++) {
        for (const auto& p : _lines[f]->points) {
            if (p.x < _x->min) {
                _x->min = p.x;
            }

            if (p.x > _x->max) {
                _x->max = p.x;
            }

            if (p.y < _y->min) {
                _y->min = p.y;
            }

            if (p.y > _y->max) {
                _y->max = p.y;
            }
        }
    }

    _x->min2 = _x->min;
    _x->max2 = _x->max;
    _y->min2 = _y->min;
    _y->max2 = _y->max;
}

//------------------------------------------------------------------------------
void Plot::clear() {
    delete _x;
    delete _y;


    _calc           = false;
    _ch             = 0;
    _ct             = 0;
    _cw             = 0;
    _h              = 0;
    _selected_line  = 0;
    _selected_point = -1;
    _size           = 0;
    _w              = 0;
    _x              = new PlotScale();
    _y              = new PlotScale();

    for (size_t f = 0; f < Plot::MAX_LINES; f++) {
        _lines[f]->clear();
    }

    selection_color(FL_FOREGROUND_COLOR);
}

//------------------------------------------------------------------------------
// Create tooltip with x and y values by using  mouse cursor position
// If ctrl is true then the hit area is increased
//
void Plot::_create_tooltip(bool ctrl) {
    int         X   = Fl::event_x();
    int         Y   = Fl::event_y();
    std::string old = _tooltip;

    _tooltip        = "";
    _selected_point = -1;

    if (X >= _area->x && X < _area->x2() && Y >= _area->y && Y < _area->y2()) {
        auto fr = (_x->fr > _y->fr) ? _x->fr : _y->fr;

        if (_selected_line < _size) {
            const auto&  line   = *_lines[_selected_line];
            const auto&  points = line.points;
            const bool   radius = _plot_has_radius(line.type);
            const int    rad    = (ctrl == true) ? (radius == true) ? line.width : line.width * 2 : (radius == true) ? line.width / 2 : line.width;
            const double xv1    = ((double) (X - _area->x - rad) / _x->pixel) + _x->min;
            const double xv2    = ((double) (X - _area->x + rad) / _x->pixel) + _x->min;
            const double yv1    = ((double) (_area->y2() - Y - rad) / _y->pixel) + _y->min;
            const double yv2    = ((double) (_area->y2() - Y + rad) / _y->pixel) + _y->min;

            for (size_t i = 0; i < points.size(); i++) {
                const auto& point = points[i];

                if (point.x >= xv1 && point.x <= xv2 && point.y >= yv1 && point.y <= yv2) {
                    std::string xl  = _plot_format_double(point.x, fr, ' ');
                    std::string yl  = _plot_format_double(point.y, fr, ' ');
                    size_t      len = (xl.length() > yl.length()) ? xl.length() : yl.length();

                    _tooltip = flw::util::format("Line %d, Point %d\nX = %*s\nY = %*s", (int) _selected_line + 1, (int) i + 1, len, xl.c_str(), len, yl.c_str());
                    _selected_point = i;
                    break;
                }
            }
        }

        if (_tooltip == "") {
            double      xv  = ((double) (X - _area->x) / _x->pixel) + _x->min;
            double      yv  = ((double) (_area->y2() - Y) / _y->pixel) + _y->min;
            std::string xl  = _plot_format_double(xv, fr, ' ');
            std::string yl  = _plot_format_double(yv, fr, ' ');
            size_t      len = (xl.length() > yl.length()) ? xl.length() : yl.length();

            _tooltip = flw::util::format("X = %*s\nY = %*s", len, xl.c_str(), len, yl.c_str());
        }
    }

    if (_tooltip != "" || old != "") {
        redraw();
    }
}

//------------------------------------------------------------------------------
void Plot::custom_xlabels_for_points0(const flw::StringVector& xlabels) {
    _x->labels = xlabels;
}

//------------------------------------------------------------------------------
void Plot::custom_ylabels_for_points0(const flw::StringVector& ylabels) {
     _y->labels = ylabels;
}

//------------------------------------------------------------------------------
void Plot::debug() const {
#ifdef DEBUG
    fprintf(stderr, "\nPlot: %d\n", (int) _size);
    fprintf(stderr, "\tcanvas          = (%d, %d) - (%d, %d) - (%d, %d)\n", x(), y(), w(), h(), x() + w(), y() + h());
    fprintf(stderr, "\tarea            = (%d, %d) - (%d, %d) - (%d, %d)\n", _area->x, _area->y, _area->w, _area->h, _area->x + _area->w, _area->y + _area->h);
    fprintf(stderr, "\tcw, ch          = (%d, %d)\n", _cw, _ch);
    fprintf(stderr, "\tselected_line   = %d\n", (int) _selected_line);
    fprintf(stderr, "\tselected_point  = %d\n", (int) _selected_point);
    fprintf(stderr, "\tline_count      = %d\n", (int) _size);
    fprintf(stderr, "\tview_labels     = %s\n", _view.labels ? "YES" : "NO");
    fprintf(stderr, "\tview_horizontal = %s\n", _view.horizontal ? "YES" : "NO");
    fprintf(stderr, "\tview_vertical   = %s\n", _view.vertical ? "YES" : "NO");

    _x->debug("X");
    _y->debug("Y");

    for (size_t f = 0; f < _size; f++) {
        fprintf(stderr, "\tPlot::Line: %d\n", (int) f);
        fprintf(stderr, "\t\tcolor   = %u\n", _lines[f]->color);
        fprintf(stderr, "\t\tlabel   = '%s'\n", _lines[f]->label.c_str());
        fprintf(stderr, "\t\tpoints  = %d\n", (int) _lines[f]->points.size());
        fprintf(stderr, "\t\ttype    = %d\n", (int) _lines[f]->type);
        fprintf(stderr, "\t\tvisible = %s\n", _lines[f]->visible ? "YES" : "NO");
        fprintf(stderr, "\t\twidth   = %d px\n", (int) _lines[f]->width);
    }
    // _plot_print(_lines[0]->points);
    // _plot_print(_lines[1].points);
    fflush(stderr);
#endif
}

//------------------------------------------------------------------------------
void Plot::draw() {
#ifdef DEBUG
    // auto t = util::time_milli();
#endif

    fl_font(flw::PREF_FIXED_FONT, flw::PREF_FIXED_FONTSIZE);
    Fl_Group::draw();

    if (_area->w < 50 || _area->h < 50 || _x->min >= _x->max || _x->pixel * _x->tick < 1.0 || _y->min >= _y->max || _y->pixel * _y->tick < 1.0) {
        return;
    }

    fl_push_clip(x(), y(), w(), h());
    _draw_xlabels();
    _draw_xlabels2();
    _draw_ylabels();
    _draw_ylabels2();
    _draw_labels();
    fl_pop_clip();

    fl_push_clip(_area->x, _area->y, _area->w, _area->h);
    _draw_lines();
    _draw_line_labels();
    _draw_tooltip();
    fl_pop_clip();

    fl_color(FL_FOREGROUND_COLOR);
    fl_line_style(FL_SOLID, 1);
    fl_rect(_area->x, _area->y, _area->w, _area->h, FL_FOREGROUND_COLOR);
    fl_line_style(0);

#ifdef DEBUG
    // fprintf(stderr, "Plot::draw: %d mS\n", util::time_milli() - t);
    // fflush(stderr);
#endif
}

//------------------------------------------------------------------------------
void Plot::_draw_labels() {
    if (_x->label != "") {
        fl_color(_x->color);
        fl_draw (_x->label.c_str(), _area->x, y() + h() - _ch, _area->w, flw::PREF_FIXED_FONTSIZE, FL_ALIGN_CENTER | FL_ALIGN_INSIDE);
    }

    if (_y->label != "") {
        auto y = _area->y + (_area->h / 2) + (_y->label.length() * _cw / 2);
        fl_color(_y->color);
        fl_draw (90, _y->label.c_str(), x() + _ch, y);
    }
}

//------------------------------------------------------------------------------
void Plot::_draw_line_labels() {
    if (_view.labels == true) {
        int X = _area->x + 6;
        int Y = _area->y + 6;
        int W = 0;
        int H = 0;

        for (size_t f = 0; f < _size; f++) {
            const auto& label = _lines[f]->label;

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
                auto& line = *_lines[f];

                if (line.label != "") {
                    auto label = line.label;

                    if (f == _selected_line) {
                        label = "@-> " + label;
                    }

                    fl_color((line.visible == false) ? FL_GRAY : _lines[f]->color);
                    fl_line_style(FL_SOLID, 1);
                    fl_draw(label.c_str(), X + 4, Y + 4, W, flw::PREF_FIXED_FONTSIZE, FL_ALIGN_LEFT | FL_ALIGN_INSIDE, nullptr, 1);
                    Y += flw::PREF_FIXED_FONTSIZE;
                }
            }

            fl_line_style(0);
        }
    }
}

//------------------------------------------------------------------------------
void Plot::_draw_lines() {
    const int X = _area->x;
    const int Y = _area->y2() - 1;

    for (size_t f = 0; f < _size; f++) {
        const auto& line  = *_lines[f];
        const TYPE  type  = line.type;
        const int   size1 = line.width;
        const int   size2 = size1 / 2;
        int         x1    = 10000;
        int         y1    = 0;

        if (line.visible == false) {
            continue;
        }

        for (size_t i = 0; i < line.points.size(); i++) {
            const Point& p = line.points[i];
            const int          x = X + (int) ((p.x - _x->min) * _x->pixel);
            const int          y = Y - (int) ((p.y - _y->min) * _y->pixel);

            if (type == Plot::LINE || type == Plot::LINE_DASH || type == Plot::LINE_DOT || type == Plot::LINE_WITH_SQUARE) {
                if (x1 == 10000) {
                    x1 = x;
                    y1 = y;
                }
                else {
                    fl_color((_selected_point == i && _selected_line == f) ? selection_color() : line.color);
                    _draw_lines_style(type, size1);
                    fl_line(x1, y1, x, y);
                    x1 = x;
                    y1 = y;
                }

                if (type == Plot::LINE_WITH_SQUARE) {
                    fl_color((_selected_point == i && _selected_line == f) ? selection_color() : line.color);
                    _draw_lines_style(type, size1);
                    fl_rectf(x - size1 - 3, y - size1 - 3, size1 * 2 + 6, size1 * 2 + 6);
                }
            }
            else if (type == Plot::VECTOR) {
                if (x1 == 10000) {
                    x1 = x;
                    y1 = y;
                }
                else {
                    fl_color(((_selected_point == i || _selected_point == i - 1) && _selected_line == f) ? selection_color() : line.color);
                    _draw_lines_style(type, size1);
                    fl_line(x, y, x1, y1);
                    x1 = 10000;
                }
            }
            else if (type == Plot::CIRCLE) {
                fl_color((_selected_point == i && _selected_line == f) ? selection_color() : line.color);
                _draw_lines_style(type, 1);
                fl_circle(x, y - 1, size2);
            }
            else if (type == Plot::FILLED_CIRCLE) {
                fl_color((_selected_point == i && _selected_line == f) ? selection_color() : line.color);
                _draw_lines_style(type, 1);
                fl_begin_polygon();
                fl_circle(x, y - 1, size2);
                fl_end_polygon();
            }
            else if (type == Plot::SQUARE) {
                fl_color((_selected_point == i && _selected_line == f) ? selection_color() : line.color);
                _draw_lines_style(type, 1);
                fl_rectf(x - size2, y - size2 - 1, size1, size1);
            }
        }
    }

    fl_line_style(0);
}

//------------------------------------------------------------------------------
void Plot::_draw_lines_style(TYPE type, int size) {
    if (type == Plot::LINE) {
        fl_line_style(FL_SOLID, size);
    }
    else if (type == Plot::LINE_DASH) {
        fl_line_style(FL_DASH, size);
    }
    else if (type == Plot::LINE_DOT) {
        fl_line_style(FL_DOT, size);
    }
    else if (type == Plot::VECTOR) {
        fl_line_style(FL_SOLID, size);
    }
    else if (type == Plot::LINE_WITH_SQUARE) {
        fl_line_style(FL_SOLID, size);
    }
    else if (type == Plot::CIRCLE) {
        fl_line_style(FL_SOLID, size);
    }
    else if (type == Plot::FILLED_CIRCLE) {
        fl_line_style(FL_SOLID, size);
    }
    else if (type == Plot::SQUARE) {
        fl_line_style(FL_SOLID, size);
    }
    else {
        fl_line_style(FL_SOLID, 1);
    }
}

//------------------------------------------------------------------------------
void Plot::_draw_tooltip() {
    if (_tooltip == "") {
        return;
    }

    auto X = Fl::event_x();
    auto Y = Fl::event_y();

    if (X > _area->x2() - flw::PREF_FIXED_FONTSIZE * 19) {
        X -= flw::PREF_FIXED_FONTSIZE * 18;
    }
    else {
        X += flw::PREF_FIXED_FONTSIZE * 2;
    }

    if (Y > _area->y2() - flw::PREF_FIXED_FONTSIZE * 8) {
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
    fl_line(Fl::event_x(), _area->y, Fl::event_x(), _area->y2() - 1);
    fl_line(_area->x, Fl::event_y(), _area->x2() - 1, Fl::event_y());
    fl_draw(_tooltip.c_str(), X + flw::PREF_FIXED_FONTSIZE, Y, flw::PREF_FIXED_FONTSIZE * 14, flw::PREF_FIXED_FONTSIZE * 4, FL_ALIGN_LEFT | FL_ALIGN_CENTER | FL_ALIGN_INSIDE);
    fl_line_style(0);
}

//------------------------------------------------------------------------------
void Plot::_draw_xlabels() {
    double       X    = _area->x;
    const int    Y    = _area->y2();
    const int    W    = x2();
    const double inc  = _x->pixel * _x->tick;
    double       val  = _x->min;
    int          last = -10'000;
    const int    tw   = _x->text / 2;

    while (X < _area->x2()) {
        if (_view.vertical == true && X > (_area->x + 4) && X < (_area->x2() - 4)) {
            fl_color(fl_color_average(FL_FOREGROUND_COLOR, FL_BACKGROUND2_COLOR, 0.2));
            fl_line(X, _area->y + 1, X, _area->y2() - 2);
        }

        if (X > (last + tw) && (X + tw) < W) {
            auto label = _plot_format_double(val, _x->fr, '\'');

            if ((int) (X + 1) == (_area->x2() - 1)) { // Adjust so rightmost pixel is aligned
                X += 1.0;
            }

            fl_color(FL_FOREGROUND_COLOR);
            fl_line(X, Y, X, Y + _ct * 2);

            if (_x->labels.size() == 0) {
                fl_draw(label.c_str(), X - tw, Y + _ct * 2, tw * 2, flw::PREF_FIXED_FONTSIZE, FL_ALIGN_CENTER | FL_ALIGN_INSIDE);
            }

            last = X + tw + _cw;
        }
        else {
            fl_color(FL_FOREGROUND_COLOR);
            fl_line(X, Y, X, Y + _ct);
        }

        X   += inc;
        val += _x->tick;
    }
}

//------------------------------------------------------------------------------
void Plot::_draw_xlabels2() {
    if (_x->labels.size() > 0) {
        size_t index = 0;
        bool   pair  = _plot_has_pairs(_lines[0]->type);

        for (size_t i = 0; i < _lines[0]->points.size(); i++) {
            const Point& point = _lines[0]->points[i];

            if (index >= _x->labels.size()) {
                break;
            }

            if ((i % 2 == 1 && pair == true) || pair == false) {
                const int X = _area->x + (int) ((point.x - _x->min) * _x->pixel);
                const int W = _x->labels[index].length() * _cw / 2;

                fl_draw(_x->labels[index].c_str(), X - W, _area->y2() + _ct * 2, W * 2, flw::PREF_FIXED_FONTSIZE, FL_ALIGN_CENTER | FL_ALIGN_INSIDE);
                index++;
            }
        }
    }
}

//------------------------------------------------------------------------------
void Plot::_draw_ylabels() {
    const int    X    = x();
    double       Y    = _area->y2() - 1.0;
    const int    W    = _area->x - X - flw::PREF_FIXED_FONTSIZE;
    const double inc  = _y->pixel * _y->tick;
    double       val  = _y->min;
    int          last = 10'000;

    while ((int) (Y + 0.5) >= _area->y) {
        if (_view.horizontal == true && Y > (_area->y + _ct) && Y < (_area->y2() - _ct)) {
            fl_color(fl_color_average(FL_FOREGROUND_COLOR, FL_BACKGROUND2_COLOR, 0.2));
            fl_line(_area->x + 1, Y, _area->x2() - 2, Y);
        }

        if (Y < last) {
            auto label = _plot_format_double(val, _y->fr, '\'');

            if (Y < (double) _area->y) { // Adjust top most pixel
                Y = _area->y;
            }

            fl_color(FL_FOREGROUND_COLOR);
            fl_line(_area->x, Y, _area->x - _ct * 2, Y);

            if (_y->labels.size() == 0) {
                fl_draw(label.c_str(), X, Y - _ct * 2, W, flw::PREF_FIXED_FONTSIZE, FL_ALIGN_RIGHT | FL_ALIGN_INSIDE);
            }

            last = Y - flw::PREF_FIXED_FONTSIZE;
        }
        else {
            fl_color(FL_FOREGROUND_COLOR);
            fl_line(_area->x, Y, _area->x - _ct, Y);
        }

        Y   -= inc;
        val += _y->tick;
    }
}

//------------------------------------------------------------------------------
void Plot::_draw_ylabels2() {
    if (_y->labels.size() > 0) {
        size_t index = 0;
        bool   pair  = _plot_has_pairs(_lines[0]->type);

        for (size_t i = 0; i < _lines[0]->points.size(); i++) {
            const Point& p = _lines[0]->points[i];

            if (index >= _y->labels.size()) {
                break;
            }

            if ((i % 2 == 1 && pair == true) || pair == false) {
                const int Y = _area->y2() - 1 - ((p.y - _y->min) * _y->pixel);

                fl_draw(_y->labels[index].c_str(), x() + _cw * 3, Y - _ct * 2, _y->text, flw::PREF_FIXED_FONTSIZE, FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
                index++;
            }
        }
    }
}

//------------------------------------------------------------------------------
int Plot::handle(int event) {
    if (event == FL_PUSH) {
        if (Fl::event_button1() != 0) {
            _create_tooltip(Fl::event_ctrl());

            if (_tooltip != "") {
                return 1;
            }
        }
        else if (Fl::event_button3() != 0) {
            flw::menu::set_item(_menu, _PLOT_SHOW_LABELS, _view.labels);
            flw::menu::set_item(_menu, _PLOT_SHOW_HLINES, _view.horizontal);
            flw::menu::set_item(_menu, _PLOT_SHOW_VLINES, _view.vertical);
            _menu->popup();
            return 1;
        }
    }
    else if (event == FL_DRAG) { // FL_PUSH must have been called before mouse drag
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
    else if (event == FL_KEYDOWN) { // Select chart line or toggle visibility
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
                _lines[line]->visible = !_lines[line]->visible;
            }
            else {
                _selected_line = line;
            }

            redraw();
        }
    }

    return Fl_Group::handle(event);
}

//------------------------------------------------------------------------------
void Plot::labels(std::string x, std::string y) {
    _x->label = x;
    _y->label = y;
}

//------------------------------------------------------------------------------
void Plot::label_colors(Fl_Color x, Fl_Color y) {
    _x->color = x;
    _y->color = y;
}

//------------------------------------------------------------------------------
void Plot::resize(int X, int Y, int W, int H) {
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

    if (_calc == true) { // Call Scale::calc() twice when new data has changed, first with default width
        _calc_min_max();
        _x->calc(W - flw::PREF_FIXED_FONTSIZE * 6);
        _y->calc(H - flw::PREF_FIXED_FONTSIZE * 6);
        _x->measure_text(_cw);
        _y->measure_text(_cw);
    }

    _area->x = X + (flw::PREF_FIXED_FONTSIZE * 2) + _y->text + ((_y->label != "") ? flw::PREF_FIXED_FONTSIZE : 0);
    _area->y = Y + flw::PREF_FIXED_FONTSIZE;
    _area->w = W - (_area->x - X) - flw::PREF_FIXED_FONTSIZE * 2;
    _area->h = H - (flw::PREF_FIXED_FONTSIZE * 3) - ((_x->label != "") ? flw::PREF_FIXED_FONTSIZE : 0);

    _x->calc(_area->w - 1);
    _y->calc(_area->h - 1);

    _calc = false;
    _w    = W;
    _h    = H;
}

//------------------------------------------------------------------------------
void Plot::update_pref() {
    _menu->textfont(flw::PREF_FONT);
    _menu->textsize(flw::PREF_FONTSIZE);
}

/***
 *      _____  _       _       _____ _        _   _
 *     |  __ \| |     | |     / ____| |      | | (_)
 *     | |__) | | ___ | |_   | (___ | |_ __ _| |_ _  ___
 *     |  ___/| |/ _ \| __|   \___ \| __/ _` | __| |/ __|
 *     | |    | | (_) | |_    ____) | || (_| | |_| | (__
 *     |_|    |_|\___/ \__|  |_____/ \__\__,_|\__|_|\___|
 *
 *
 */

#define FLW_PLOT_ERROR(X) { fl_alert("error: illegal plot value at pos %u", (X)->pos()); plot->clear(); return false; }

//------------------------------------------------------------------------------
void Plot::_CallbackDebug(Fl_Widget*, void* plot_object) {
    auto self = static_cast<const Plot*>(plot_object);
    self->debug();
}

//------------------------------------------------------------------------------
void Plot::_CallbackSave(Fl_Widget*, void* plot_object) {
    auto self = static_cast<Plot*>(plot_object);
    flw::util::png_save("", self->window(), self->x(),  self->y(),  self->w(),  self->h());
}

//------------------------------------------------------------------------------
void Plot::_CallbackToggle(Fl_Widget*, void* plot_object) {
    auto self = static_cast<Plot*>(plot_object);

    self->_view.labels     = flw::menu::item_value(self->_menu, _PLOT_SHOW_LABELS);
    self->_view.horizontal = flw::menu::item_value(self->_menu, _PLOT_SHOW_HLINES);
    self->_view.vertical   = flw::menu::item_value(self->_menu, _PLOT_SHOW_VLINES);

    self->redraw();
}

//------------------------------------------------------------------------------
bool Plot::Load(Plot* plot, std::string filename) {
    plot->clear();
    plot->redraw();

    auto wc  = WaitCursor();
    auto buf = util::load_file(filename);

    if (buf.p == nullptr) {
        fl_alert("error: failed to load %s", filename.c_str());
        return false;
    }

    auto js  = JS();
    auto err = js.decode(buf.p, buf.s);
    auto h   = false;
    auto l   = false;
    auto v   = false;
    auto x   = PlotScale();
    auto y   = PlotScale();

    if (err != "") {
        fl_alert("error: failed to parse %s (%s)", filename.c_str(), err.c_str());
        return false;
    }

    if (js.is_object() == false) FLW_PLOT_ERROR(&js);

    for (auto j : js.vo_to_va()) {
        if (j->name() == "descr" && j->is_object() == true) {
            for (const auto j2 : j->vo_to_va()) {
                if (j2->name() == "type" && j2->is_string() == true) {
                    if (j2->vs() != "flw::plot") FLW_PLOT_ERROR(j2)
                }
                else if (j2->name() == "version" && j2->is_number() == true) {
                    if (j2->vn_i() != 2) FLW_PLOT_ERROR(j2)
                }
                else FLW_PLOT_ERROR(j2)
            }
        }
        else if (j->name() == "view" && j->is_object() == true) {
            for (const auto j2 : j->vo_to_va()) {
                if (j2->name() == "horizontal" && j2->is_bool() == true)    h = j2->vb();
                else if (j2->name() == "labels" && j2->is_bool() == true)   l = j2->vb();
                else if (j2->name() == "vertical" && j2->is_bool() == true) v = j2->vb();
                else FLW_PLOT_ERROR(j2)
            }
        }
        else if ((j->name() == "xscale" || j->name() == "yscale") && j->is_object() == true) {
            auto& scale = (j->name() == "xscale") ? x : y;

            for (auto s : j->vo_to_va()) {
                if (s->name() == "color")      scale.color = s->vn_i();
                else if (s->name() == "label") scale.label = s->vs_u();
                else if (s->name() == "labels" && s->is_array() == true) {
                    for (auto s2 : *s->va()) {
                        if (s2->is_string() == true) scale.labels.push_back(s2->vs_u());
                        else FLW_PLOT_ERROR(s2)
                    }
                }
                else FLW_PLOT_ERROR(s)
            }
        }
        else if (j->name() == "yxlines" && j->is_array() == true) {
            for (const auto j2 : *j->va()) {
                if (j2->is_object() == true) {
                    PlotLine line;

                    for (auto j3 : j2->vo_to_va()) {
                        if (j3->name() == "color" && j3->is_number() == true)      line.color = (Fl_Color) j3->vn_i();
                        else if (j3->name() == "label" && j3->is_string() == true) line.label = j3->vs_u();
                        else if (j3->name() == "type" && j3->is_string() == true)  line.type  = _plot_string_to_type(j3->vs());
                        else if (j3->name() == "width" && j3->is_number() == true) line.width = (unsigned) j3->vn_i();
                        else if (j3->name() == "yx" && j3->is_array() == true) {
                            for (auto p : *j3->va()) {
                                if (p->is_array() == true && p->size() == 2 && (*p)[0]->is_number() == true && (*p)[1]->is_number() == true) {
                                    line.points.push_back(Point((*p)[0]->vn(), (*p)[1]->vn()));
                                }
                                else FLW_PLOT_ERROR(p)
                            }
                        }
                        else FLW_PLOT_ERROR(j3)
                    }

                    if (plot->add_line(line.points, line.type, line.width, line.label, line.color) == false) FLW_PLOT_ERROR(j2)
                }
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

//----------------------------------------------------------------------
bool Plot::Save(const Plot* plot, std::string filename) {
    auto wc  = WaitCursor();
    auto jsb = JSB();

    try {
        jsb << JSB::MakeObject();
            jsb << JSB::MakeObject("descr");
                jsb << JSB::MakeString("flw::plot", "type");
                jsb << JSB::MakeNumber(2, "version");
            jsb.end();
            jsb << JSB::MakeObject("view");
                jsb << JSB::MakeBool(plot->_view.labels, "labels");
                jsb << JSB::MakeBool(plot->_view.horizontal, "horizontal");
                jsb << JSB::MakeBool(plot->_view.vertical, "vertical");
            jsb.end();
            jsb << JSB::MakeObject("xscale");
                jsb << JSB::MakeString(plot->_x->label.c_str(), "label");
                jsb << JSB::MakeNumber(plot->_x->color, "color");
                jsb << JSB::MakeArrayInline("labels");
                    for (const auto& l : plot->_x->labels) jsb << JSB::MakeString(l);
                jsb.end();
            jsb.end();
            jsb << JSB::MakeObject("yscale");
                jsb << JSB::MakeString(plot->_y->label.c_str(), "label");
                jsb << JSB::MakeNumber(plot->_y->color, "color");
                jsb << JSB::MakeArrayInline("labels");
                    for (const auto& l : plot->_y->labels) jsb << JSB::MakeString(l);
                jsb.end();
            jsb.end();
            jsb << JSB::MakeArray("yxlines");

            for (size_t f = 0; f < plot->_size; f++) {
                const auto& line = *plot->_lines[f];
                if (line.points.size() > 0) {
                    jsb << JSB::MakeObject();
                        jsb << JSB::MakeNumber(line.width, "width");
                        jsb << JSB::MakeNumber(line.color, "color");
                        jsb << JSB::MakeString(line.label, "label");
                        jsb << JSB::MakeString(_plot_type_to_string(line.type), "type");
                        jsb << JSB::MakeArray("yx");
                        for (const auto& point : line.points) {
                            jsb << JSB::MakeArrayInline();
                                jsb << JSB::MakeNumber(point.x);
                                jsb << JSB::MakeNumber(point.y);
                            jsb.end();
                        }
                        jsb.end();
                    jsb.end();
                }
            }

        auto js = jsb.encode();
        return util::save_file(filename, js.c_str(), js.length());
    }
    catch(const std::string& e) {
        fl_alert("error: failed to encode json\n%s", e.c_str());
        return false;
    }
}

}

// MKALGAM_OFF
