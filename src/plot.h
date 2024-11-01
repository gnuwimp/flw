// Copyright gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#ifndef FLW_PLOT_H
#define FLW_PLOT_H

#include "flw.h"

// MKALGAM_ON

#include <FL/Fl_Group.H>
#include <FL/Fl_Menu_Button.H>

namespace flw {

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

struct PlotData;
typedef std::vector<PlotData> PlotDataVector;

//----------------------------------------------------------------------
struct PlotData {
    static constexpr const double MAX_VALUE = 9223372036854775807.0;
    static constexpr const double MIN_VALUE = 0.0000001;

    enum class FORMULAS {
                                MODIFY,
                                SWAP,
                                LAST = SWAP,
    };

    enum class MODIFY {
                                ADDITION,
                                DIVISION,
                                MULTIPLICATION,
                                SUBTRACTION,
                                LAST = SUBTRACTION,
    };

    enum class TARGET {
                                X,
                                Y,
                                X_AND_Y,
    };

    double                      x;
    double                      y;

    explicit                    PlotData(double X = 0.0, double Y = 0.0);
    void                        debug(int i = -1) const;
    bool                        is_valid() const
                                    { return std::isfinite(x) == true && std::isfinite(y) == true && fabs(x) < PlotData::MAX_VALUE && fabs(y) < PlotData::MAX_VALUE; }

    static void                 Debug(const PlotDataVector& in);
    static PlotDataVector       LoadCSV(std::string filename, std::string sep = ",");
    static bool                 MinMax(const PlotDataVector& in, double& min_x, double& max_x, double& min_y, double& max_y);
    static PlotDataVector       Modify(const PlotDataVector& in, PlotData::MODIFY modify, PlotData::TARGET target, double value);
    static bool                 SaveCSV(const PlotDataVector& in, std::string filename, std::string sep = ",");
    static PlotDataVector       Swap(const PlotDataVector& in);
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
class PlotLine {
public:
    static const int            MAX_WIDTH = 30;

    enum class TYPE {
                                LINE,
                                LINE_DASH,
                                LINE_DOT,
                                LINE_WITH_SQUARE,
                                VECTOR,
                                CIRCLE,
                                FILLED_CIRCLE,
                                SQUARE,
                                LAST = SQUARE,
    };

                                PlotLine();
                                PlotLine(const PlotDataVector& data, std::string label, PlotLine::TYPE type = PlotLine::TYPE::LINE, Fl_Color color = FL_BLUE, unsigned width = 1);
    void                        clear();
    Fl_Color                    color() const
                                    { return _color; }
    const PlotDataVector&       data() const
                                    { return _data; }
    void                        debug() const;
    bool                        has_line_type() const
                                    { return _type == TYPE::LINE || _type == TYPE::LINE_DASH || _type == TYPE::LINE_DOT || _type == TYPE::LINE_WITH_SQUARE; }
    bool                        has_radius() const
                                    { return _type == TYPE::CIRCLE || _type == TYPE::FILLED_CIRCLE || _type == TYPE::SQUARE; }
    bool                        is_vector() const
                                    { return _type == TYPE::VECTOR; }
    bool                        is_visible() const
                                    { return _visible; }
    std::string                 label() const
                                    { return _label; }
    const Fl_Rect&              label_rect() const
                                    { return _rect; }
    PlotLine&                   set_color(Fl_Color val)
                                    { _color = val; return *this; }
    PlotLine&                   set_data(const PlotDataVector& val)
                                    { _data = val;  return *this; }
    PlotLine&                   set_label(std::string val)
                                    { _label = val; return *this; }
    PlotLine&                   set_label_rect(int x, int y, int w, int h)
                                    { _rect = Fl_Rect(x, y, w, h); return *this; }
    PlotLine&                   set_type(PlotLine::TYPE val)
                                    { _type = val; return *this; }
    PlotLine&                   set_type_from_string(std::string val);
    PlotLine&                   set_visible(bool val)
                                    { _visible = val; return *this; }
    PlotLine&                   set_width(unsigned val)
                                    { if (val <= PlotLine::MAX_WIDTH) _width = val; return *this; }
    PlotLine::TYPE              type() const
                                    { return _type; }
    std::string                 type_to_string() const;
    unsigned                    width() const
                                    { return _width; }

private:
    Fl_Color                    _color;
    Fl_Rect                     _rect;
    PlotDataVector              _data;
    TYPE                        _type;
    bool                        _visible;
    std::string                 _label;
    unsigned                    _width;
};

typedef std::vector<PlotLine> PlotLineVector;

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
// One scale for each x and y axis.
//
class PlotScale {
public:

                                PlotScale()
                                    { clear(); }
    void                        calc_tick(double height);
    void                        clear();
    void                        clear_min_max()
                                    { _min = _max = INFINITY; }
    Fl_Color                    color() const
                                    { return _color; }
    const StringVector&         custom_labels() const
                                    { return _labels; }
    void                        debug(const char* s) const;
    int                         fr() const
                                    { return _fr; }
    bool                        is_max_finite() const
                                    { return std::isfinite(_max); }
    bool                        is_min_finite() const
                                    { return std::isfinite(_min); }
    std::string                 label() const
                                    { return _label; }
    double                      max() const
                                    { return _max; }
    void                        measure_labels(int cw, bool custom);
    double                      min() const
                                    { return _min; }
    double                      pixel() const
                                    { return _pixel; }
    void                        set_color(int val)
                                    { _color = val; }
    void                        set_custom_labels(StringVector val)
                                    { _labels = val; }
    void                        set_label(std::string val)
                                    { _label = val; }
    void                        set_max(double val)
                                    { _max = val; }
    void                        set_min(double val)
                                    { _min = val; }
    int                         text() const
                                    { return static_cast<int>(_text); }
    double                      tick() const
                                    { return _tick; }

private:
    Fl_Color                    _color;
    int                         _fr;
    std::string                 _label;
    StringVector                _labels;
    double                      _max;
    double                      _min;
    double                      _pixel;
    double                      _tick;
    size_t                      _text;
};

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
// Custom x and y labels are only for PlotLine::TYPE::VECTOR and for the first line.
// Also only a few can be drawn to screen (meant for weekdays or months or similar).
//
class Plot : public Fl_Group {
public:
    static const size_t         MAX_LINES = 10;
    static const int            VERSION   =  3;

    enum class CLAMP {
                                MINX,
                                MAXX,
                                MINY,
                                MAXY,
    };

    enum class LABEL {
                                MAIN,
                                X,
                                Y,
    };

    explicit                    Plot(int X = 0, int Y = 0, int W = 0, int H = 0, const char* l = nullptr);
    bool                        add_line(const PlotLine& line);
    void                        clear();
    bool                        create_line(PlotData::FORMULAS formula);
    void                        debug() const;
    void                        debug_line() const;
    void                        delete_line(size_t index);
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
    void                        init()
                                    { _init(false); }
    void                        init_new_data()
                                    { _init(true); }
    bool                        load_csv();
    bool                        load_json();
    bool                        load_json(std::string filename);
    void                        print();
    void                        resize(int X, int Y, int W, int H) override;
    bool                        save_csv();
    bool                        save_json();
    bool                        save_json(std::string filename);
    bool                        save_png();
    void                        set_hor_lines(bool val = true)
                                    { _horizontal = val; }
    void                        set_label(std::string label = "")
                                    { _label = label; }
    void                        set_line_labels(bool val = true)
                                    { _labels = val; }
    void                        set_max_x(double val = INFINITY)
                                    { _max_x = val; }
    void                        set_max_y(double val = INFINITY)
                                    { _max_y = val; }
    void                        set_min_x(double val = INFINITY)
                                    { _min_x = val; }
    void                        set_min_y(double val = INFINITY)
                                    { _min_y = val; }
    void                        set_ver_lines(bool val = true)
                                    { _vertical = val; }
    void                        setup_add_line();
    void                        setup_clamp(Plot::CLAMP clamp);
    void                        setup_delete_lines();
    void                        setup_label(Plot::LABEL val);
    void                        setup_line();
    void                        setup_show_or_hide_lines();
    void                        setup_view_options();
    void                        update_pref();
    PlotScale&                  xscale()
                                    { return _x; }
    PlotScale&                  yscale()
                                    { return _y; }

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
    static bool                 _CallbackPrinter(void* data, int pw, int ph, int page);
    static void                 _CallbackToggle(Fl_Widget*, void* widget);

    struct {
    }                           _view;

    Fl_Menu_Button*             _menu;
    Fl_Rect                     _area;
    Fl_Rect                     _old;
    PlotLineVector              _lines;
    PlotScale                   _x;
    PlotScale                   _y;
    bool                        _disable_menu;
    bool                        _horizontal;
    bool                        _labels;
    bool                        _vertical;
    const int                   _CH;
    const int                   _CW;
    double                      _max_x;
    double                      _max_y;
    double                      _min_x;
    double                      _min_y;
    size_t                      _selected_line;
    size_t                      _selected_point;
    std::string                 _filename;
    std::string                 _label;
    std::string                 _tooltip;

};

}

// MKALGAM_OFF

#endif
