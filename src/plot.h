// Copyright gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#ifndef FLW_PLOT_H
#define FLW_PLOT_H

#include "flw.h"

// MKALGAM_ON

#include <FL/Fl_Group.H>
#include <FL/Fl_Menu_Button.H>

namespace flw {

//----------------------------------------------------------------------
struct Point {
    double                      x;
    double                      y;

    explicit                    Point(double x = 0.0, double y = 0.0) {
                                    this->x = x;
                                    this->y = y;
                                }
};

typedef std::vector<Point>      PointVector;

struct PlotArea;
struct PlotLine;
struct PlotScale;

//--------------------------------------------------------------------------
//
class Plot : public Fl_Group {
public:
    static constexpr double     MAX        =  999'999'999'999'999.0;
    static constexpr double     MIN        = -999'999'999'999'999.0;
    static const size_t         MAX_LINES  = 10;

    enum TYPE {
                                LINE,
                                LINE_DASH,
                                LINE_DOT,
                                LINE_WITH_SQUARE,
                                VECTOR,
                                CIRCLE,
                                FILLED_CIRCLE,
                                SQUARE,
    };

    explicit                    Plot(int X = 0, int Y = 0, int W = 0, int H = 0, const char* l = nullptr);
    virtual                     ~Plot();
    bool                        add_line(const PointVector& points, TYPE type, unsigned width = 1, std::string label = "", Fl_Color color = FL_FOREGROUND_COLOR);
    void                        clear();
    void                        custom_xlabels_for_points0(const flw::StringVector& xlabels = StringVector());
    void                        custom_ylabels_for_points0(const flw::StringVector& ylabels = StringVector());
    void                        debug() const;
    void                        draw() override;
    int                         handle(int event) override;
    void                        labels(std::string x, std::string y);
    void                        label_colors(Fl_Color x, Fl_Color y);
    void                        layout()
                                    { _calc = true; _w = _h = 0; resize(x(), y(), w(), h()); redraw(); }
    void                        resize(int X, int Y, int W, int H) override;
    void                        update_pref();
    void                        view_options(bool line_labels = true, bool hor_lines = true, bool ver_lines = true)
                                    { _view.labels = line_labels; _view.horizontal = hor_lines; _view.vertical = ver_lines; redraw(); }
    int                         x2() const
                                    { return x() + w(); }
    int                         y2() const
                                    { return y() + h(); }

    static bool                 Load(Plot* plot, std::string filename);
    static bool                 Save(const Plot* plot, std::string filename);

private:
    void                        _calc_min_max();
    void                        _create_tooltip(bool ctrl);
    void                        _draw_labels();
    void                        _draw_line_labels();
    void                        _draw_lines();
    void                        _draw_lines_style(Plot::TYPE type, int size);
    void                        _draw_tooltip();
    void                        _draw_xlabels();
    void                        _draw_xlabels2();
    void                        _draw_ylabels();
    void                        _draw_ylabels2();

    static void                 _CallbackDebug(Fl_Widget*, void* plot_object);
    static void                 _CallbackSave(Fl_Widget*, void* plot_object);
    static void                 _CallbackToggle(Fl_Widget*, void* plot_object);

    struct {
        bool                    horizontal;
        bool                    labels;
        bool                    vertical;
    }                           _view;

    Fl_Menu_Button*             _menu;
    PlotArea*                   _area;
    PlotLine*                   _lines[Plot::MAX_LINES];
    PlotScale*                  _x;
    PlotScale*                  _y;
    bool                        _calc;
    int                         _ch;
    int                         _ct;
    int                         _cw;
    int                         _h;
    int                         _w;
    size_t                      _selected_line;
    size_t                      _selected_point;
    size_t                      _size;
    std::string                 _tooltip;

};

}

// MKALGAM_OFF

#endif
