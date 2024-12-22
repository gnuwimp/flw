// Copyright gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

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

#define _FLW_PLOT_ERROR(X) { fl_alert("error: illegal plot value at pos %u", (X)->pos()); clear(); return false; }
#define _FLW_PLOT_CB(X)    [](Fl_Widget*, void* o) { static_cast<Plot*>(o)->X; }, this
//#define _FLW_PLOT_DEBUG(X) { X; }
#define _FLW_PLOT_DEBUG(X)
#define _FLW_PLOT_CLIP(X) { X; }
//#define _FLW_PLOT_CLIP(X)

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

/***
 *      _____  _       _   _____        _
 *     |  __ \| |     | | |  __ \      | |
 *     | |__) | | ___ | |_| |  | | __ _| |_ __ _
 *     |  ___/| |/ _ \| __| |  | |/ _` | __/ _` |
 *     | |    | | (_) | |_| |__| | (_| | || (_| |
 *     |_|    |_|\___/ \__|_____/ \__,_|\__\__,_|
 *
 *
 */

//------------------------------------------------------------------------------
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

//------------------------------------------------------------------------------
void PlotData::debug(int i) const {
#ifdef DEBUG
    auto X = gnu::JS::FormatNumber(x);
    auto Y = gnu::JS::FormatNumber(y);

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

//------------------------------------------------------------------------------
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

//------------------------------------------------------------------------------
PlotDataVector PlotData::LoadCSV(std::string filename, std::string sep) {
    auto buf = gnu::file::read(filename);

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

//------------------------------------------------------------------------------
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

//------------------------------------------------------------------------------
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

        default: // MODIFY::ADDITION
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

//------------------------------------------------------------------------------
bool PlotData::SaveCSV(const PlotDataVector& in, std::string filename, std::string sep) {
    std::string csv;

    csv.reserve(in.size() * 20 + 256);

    for (const auto& data : in) {
        char buffer[256];
        snprintf(buffer, 256, "%s%s%s\n", gnu::JS::FormatNumber(data.x).c_str(), sep.c_str(), gnu::JS::FormatNumber(data.y).c_str());
        csv += buffer;
    }

    return gnu::file::write(filename, csv.c_str(), csv.size());
}

//------------------------------------------------------------------------------
PlotDataVector PlotData::Swap(const PlotDataVector& in) {
    PlotDataVector res;

    for (const auto& data : in) {
        res.push_back(PlotData(data.y, data.x));
    }

    return res;
}

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

//------------------------------------------------------------------------------
PlotLine::PlotLine() {
    clear();
}

//------------------------------------------------------------------------------
PlotLine::PlotLine(const PlotDataVector& data, std::string label, PlotLine::TYPE type, Fl_Color color, unsigned width) {
    _color   = color;
    _data    = data;
    _label   = label;
    _type    = type;
    _visible = true;
    _width   = (width <= PlotLine::MAX_WIDTH) ? width : 1;
}

//------------------------------------------------------------------------------
void PlotLine::clear() {
    _color   = FL_BLUE;
    _label   = "";
    _rect    = Fl_Rect();
    _type    = TYPE::LINE;
    _visible = true;
    _width   = 1;

    _data.clear();
}

//------------------------------------------------------------------------------
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

//------------------------------------------------------------------------------
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

//------------------------------------------------------------------------------
std::string PlotLine::type_to_string() const {
    static const char* NAMES[] = { "LINE", "LINE_DASH", "LINE_DOT", "LINE_WITH_SQUARE", "VECTOR", "CIRCLE", "FILLED_CIRCLE", "SQUARE", "", "", };
    return NAMES[static_cast<size_t>(_type)];
}

/***
 *      _____  _       _   _      _             _____      _
 *     |  __ \| |     | | | |    (_)           / ____|    | |
 *     | |__) | | ___ | |_| |     _ _ __   ___| (___   ___| |_ _   _ _ __
 *     |  ___/| |/ _ \| __| |    | | '_ \ / _ \\___ \ / _ \ __| | | | '_ \
 *     | |    | | (_) | |_| |____| | | | |  __/____) |  __/ |_| |_| | |_) |
 *     |_|    |_|\___/ \__|______|_|_| |_|\___|_____/ \___|\__|\__,_| .__/
 *                                                                  | |
 *                                                                  |_|
 */

//------------------------------------------------------------------------------
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
    //--------------------------------------------------------------------------
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

    //--------------------------------------------------------------------------
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

    //--------------------------------------------------------------------------
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

//------------------------------------------------------------------------------
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

//------------------------------------------------------------------------------
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

//------------------------------------------------------------------------------
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

//------------------------------------------------------------------------------
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

//------------------------------------------------------------------------------
bool Plot::add_line(const PlotLine& line) {
    if (_lines.size() >= Plot::MAX_LINES) {
        return false;
    }

    _lines.push_back(line);
    return true;
}

//------------------------------------------------------------------------------
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

//------------------------------------------------------------------------------
bool Plot::_CallbackPrinter(void* data, int pw, int ph, int) {
    auto widget = static_cast<Fl_Widget*>(data);
    auto rect   = Fl_Rect(widget);

    widget->resize(0, 0, pw, ph);
    widget->draw();
    widget->resize(rect.x(), rect.y(), rect.w(), rect.h());

    return false;
}

//------------------------------------------------------------------------------
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

//------------------------------------------------------------------------------
// Create new plot line from current line and add it to current area.
//
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

//------------------------------------------------------------------------------
StringVector Plot::_create_check_labels(bool def) const {
    StringVector res;

    for (const auto& l : _lines) {
        std::string s = (def == true && l.is_visible() == true) ? "1" : "0";
        s += l.label();
        res.push_back(s);
    }

    return res;
}

//------------------------------------------------------------------------------
// Create tooltip with x and y values by using  mouse cursor position.
// Or found data point below mouse cursor.
//
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

//------------------------------------------------------------------------------
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

//------------------------------------------------------------------------------
void Plot::debug_line() const {
#ifdef DEBUG
    if (_selected_line >= _lines.size()) {
        return;
    }

    PlotData::Debug(_lines[_selected_line].data());
#endif
}

//------------------------------------------------------------------------------
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

//------------------------------------------------------------------------------
void Plot::draw() {
#ifdef DEBUG
//     auto t = util::milliseconds();
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
//     printf("Plot::draw: %d mS\n", util::milliseconds() - t);
//     fflush(stdout);
#endif
}

//------------------------------------------------------------------------------
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

//------------------------------------------------------------------------------
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

//------------------------------------------------------------------------------
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
                if (x2 == 10'000) { // Don't draw first x,y values.
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

//------------------------------------------------------------------------------
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

//------------------------------------------------------------------------------
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

            if (static_cast<int>(x1 + 1) == (_area.r() - 1)) { // Adjust so rightmost pixel is aligned.
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

//------------------------------------------------------------------------------
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

//------------------------------------------------------------------------------
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
            
            if (y1 < static_cast<double>(_area.y())) { // Adjust top most pixel.
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

//------------------------------------------------------------------------------
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

//------------------------------------------------------------------------------
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
    else if (event == FL_DRAG) { // FL_PUSH must have been called before mouse drag
        _create_tooltip();

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

    return Fl_Group::handle(event);
}

//------------------------------------------------------------------------------
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

//------------------------------------------------------------------------------
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

//------------------------------------------------------------------------------
bool Plot::load_json() {
    auto filename = util::to_string(fl_file_chooser("Select JSON File", "All Files (*)\tJSON Files (*.json)", ""));

    if (util::is_whitespace_or_empty(filename.c_str()) == true) {
        return false;
    }

    return load_json(filename);
}

//------------------------------------------------------------------------------
bool Plot::load_json(std::string filename) {
    _filename = "";
    clear();
    redraw();

    auto wc  = WaitCursor();
    auto buf = gnu::file::read(filename);

    if (buf.p == nullptr) {
        fl_alert("error: failed to load %s", filename.c_str());
        return false;
    }

    auto js  = gnu::JS();
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
    _filename = filename;
    return true;
}

//------------------------------------------------------------------------------
void Plot::print() {
    dlg::print("Print Plot", Plot::_CallbackPrinter, this, 1, 1, top_window());
    redraw();
}

//------------------------------------------------------------------------------
void Plot::resize(int X, int Y, int W, int H) {
    if (_old.w() == W && _old.h() == H) {
        return;
    }

    Fl_Widget::resize(X, Y, W, H);
    _old = Fl_Rect(this);
    init();
}

//------------------------------------------------------------------------------
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

//------------------------------------------------------------------------------
bool Plot::save_json() {
    auto filename = util::to_string(fl_file_chooser("Save To JSON File", "All Files (*)\tJSON Files (*.json)", ""));

    if (util::is_whitespace_or_empty(filename.c_str()) == true) {
        return false;
    }

    return save_json(filename);
}

//----------------------------------------------------------------------
bool Plot::save_json(std::string filename) {
    auto wc  = WaitCursor();
    auto jsb = gnu::JSB();

    try {
        jsb << gnu::JSB::MakeObject();
            jsb << gnu::JSB::MakeObject("Plot");
                jsb << gnu::JSB::MakeNumber(Plot::VERSION, "version");
                jsb << gnu::JSB::MakeString(_label, "label");
                jsb << gnu::JSB::MakeBool(_labels, "labels");
                jsb << gnu::JSB::MakeBool(_horizontal, "horizontal");
                jsb << gnu::JSB::MakeBool(_vertical, "vertical");
                if (std::isfinite(_min_x) == true) jsb << gnu::JSB::MakeNumber(_min_x, "minx");
                if (std::isfinite(_max_x) == true) jsb << gnu::JSB::MakeNumber(_max_x, "maxx");
                if (std::isfinite(_min_y) == true) jsb << gnu::JSB::MakeNumber(_min_y, "miny");
                if (std::isfinite(_max_y) == true) jsb << gnu::JSB::MakeNumber(_max_y, "maxy");
            jsb.end();
            jsb << gnu::JSB::MakeObject("PlotScale::x");
                jsb << gnu::JSB::MakeString(_x.label().c_str(), "label");
                jsb << gnu::JSB::MakeNumber(_x.color(), "color");
                jsb << gnu::JSB::MakeArrayInline("labels");
                    for (const auto& l : _x.custom_labels()) jsb << gnu::JSB::MakeString(l);
                jsb.end();
            jsb.end();
            jsb << gnu::JSB::MakeObject("PlotScale::y");
                jsb << gnu::JSB::MakeString(_y.label().c_str(), "label");
                jsb << gnu::JSB::MakeNumber(_y.color(), "color");
                jsb << gnu::JSB::MakeArrayInline("labels");
                    for (const auto& l : _y.custom_labels()) jsb << gnu::JSB::MakeString(l);
                jsb.end();
            jsb.end();
            jsb << gnu::JSB::MakeArray("PlotLine");

            for (const auto& line : _lines) {
                if (line.data().size() > 0) {
                    jsb << gnu::JSB::MakeObject();
                        jsb << gnu::JSB::MakeNumber(line.width(), "width");
                        jsb << gnu::JSB::MakeNumber(line.color(), "color");
                        jsb << gnu::JSB::MakeString(line.label(), "label");
                        jsb << gnu::JSB::MakeBool(line.is_visible(), "visible");
                        jsb << gnu::JSB::MakeString(line.type_to_string(), "type");
                        jsb << gnu::JSB::MakeArray("xy");
                        for (const auto& point : line.data()) {
                            if (std::isfinite(point.x) == true && std::isfinite(point.y) == true) {
                                jsb << gnu::JSB::MakeArrayInline();
                                    jsb << gnu::JSB::MakeNumber(point.x);
                                    jsb << gnu::JSB::MakeNumber(point.y);
                                jsb.end();
                            }
                        }
                        jsb.end();
                    jsb.end();
                }
            }

        auto js = jsb.encode();
        return gnu::file::write(filename, js.c_str(), js.length());
    }
    catch(const std::string& e) {
        fl_alert("error: failed to encode json\n%s", e.c_str());
        return false;
    }
}

//------------------------------------------------------------------------------
bool Plot::save_png() {
    return flw::util::png_save("", window(), x(),  y(),  w(),  h());
}

//------------------------------------------------------------------------------
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

//------------------------------------------------------------------------------
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

//------------------------------------------------------------------------------
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

//------------------------------------------------------------------------------
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

//------------------------------------------------------------------------------
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

//------------------------------------------------------------------------------
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

//------------------------------------------------------------------------------
void Plot::setup_view_options() {
    _labels     = menu::item_value(_menu, _PLOT_SETUP_LABELS);
    _horizontal = menu::item_value(_menu, _PLOT_SETUP_HLINES);
    _vertical   = menu::item_value(_menu, _PLOT_SETUP_VLINES);

    redraw();
}

//------------------------------------------------------------------------------
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

//------------------------------------------------------------------------------
void Plot::update_pref() {
    _menu->textfont(flw::PREF_FONT);
    _menu->textsize(flw::PREF_FONTSIZE);
}

}

// MKALGAM_OFF
