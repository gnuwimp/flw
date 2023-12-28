// Copyright 2022 gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#ifndef FLW_PLOT_H
#define FLW_PLOT_H

#include "util.h"
#include <FL/Fl_Group.H>

class Fl_Menu_Button;

// MKALGAM_ON

namespace flw {
    class Plot;

    namespace plot {
        class Point;
        enum class TYPE;

        const double                    MAX        =  999'999'999'999'999.0;
        const double                    MIN        = -999'999'999'999'999.0;
        const size_t                    MAX_LINES  = 10;

        typedef std::vector<plot::Point> PointVector;

        bool                            has_pairs(flw::plot::TYPE type);
        bool                            has_radius(flw::plot::TYPE type);
        bool                            load_data(flw::Plot* plot, std::string filename);
        void                            print(const flw::plot::PointVector& points);
        bool                            save_data(const flw::Plot* plot, std::string filename);
        TYPE                            string_to_type(std::string name);
        std::string                     type_to_string(TYPE type);

        //----------------------------------------------------------------------
        enum class TYPE {
                                        LINE,
                                        LINE_DASH,
                                        LINE_DOT,
                                        LINE_WITH_SQUARE,
                                        VECTOR,
                                        CIRCLE,
                                        FILLED_CIRCLE,
                                        SQUARE,
        };

        //----------------------------------------------------------------------
        struct Point {
            double                      x;
            double                      y;

                                        Point(double x = 0.0, double y = 0.0) {
                                            this->x = x;
                                            this->y = y;
                                        }
        };

        //----------------------------------------------------------------------
        struct Area {
            int                         x;
            int                         y;
            int                         w;
            int                         h;

                                        Area(int x = 0, int y = 0, int w = 0, int h = 0) {
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

        //----------------------------------------------------------------------
        struct Scale {
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

                                        Scale();
            void                        calc(double canvas_size);
            void                        debug(const char* s) const;
            void                        measure_text(int cw);
            void                        reset_min_max();
        };

        //----------------------------------------------------------------------
        struct Line {
            Fl_Color                    color;
            unsigned                    width;
            bool                        visible;
            std::string                 label;
            PointVector                 points;
            TYPE                        type;
                                        Line();
        };
    }

    //--------------------------------------------------------------------------
    //
    class Plot : public Fl_Group {
        friend bool                     plot::save_data(const Plot*, std::string);

    public:
                                        Plot(int X = 0, int Y = 0, int W = 0, int H = 0, const char* l = nullptr);
        bool                            add_line(const plot::PointVector& points, plot::TYPE type, unsigned width = 1, std::string label = "", Fl_Color color = FL_FOREGROUND_COLOR);
        bool                            add_line(const plot::Line& line);
        void                            clear();
        void                            clear_points();
        void                            custom_xlabels_for_points0(const flw::StringVector& xlabels = StringVector())
                                            { _x.labels = xlabels; }
        void                            custom_ylabels_for_points0(const flw::StringVector& ylabels = StringVector())
                                            { _y.labels = ylabels; }
        void                            debug() const;
        void                            draw() override;
        int                             handle(int event) override;
        void                            label_colors(Fl_Color x, Fl_Color y)
                                            { _x.color = x; _y.color = y; }
        void                            labels(std::string x, std::string y)
                                            { _x.label = x; _y.label = y; }
        void                            resize()
                                            { _calc = true; _w = _h = 0; resize(x(), y(), w(), h()); redraw(); }
        void                            resize(int X, int Y, int W, int H) override;
        void                            update_pref();
        void                            view_options(bool line_labels = true, bool hor_lines = true, bool ver_lines = true)
                                            { _view.labels = line_labels; _view.horizontal = hor_lines; _view.vertical = ver_lines; redraw(); }
        int                             x2() const
                                            { return x() + w(); }
        int                             y2() const
                                            { return y() + h(); }

    private:
        void                            _calc_min_max();
        void                            _create_tooltip(bool ctrl);
        void                            _draw_labels();
        void                            _draw_line_labels();
        void                            _draw_lines();
        void                            _draw_lines_style(flw::plot::TYPE type, int size);
        void                            _draw_tooltip();
        void                            _draw_xlabels();
        void                            _draw_xlabels2();
        void                            _draw_ylabels();
        void                            _draw_ylabels2();

        static void                     _CallbackDebug(Fl_Widget*, void* plot_object);
        static void                     _CallbackSave(Fl_Widget*, void* plot_object);
        static void                     _CallbackToggle(Fl_Widget*, void* plot_object);

        struct {
            bool                        horizontal;
            bool                        labels;
            bool                        vertical;
        }                               _view;

        plot::Area                      _area;
        bool                            _calc;
        int                             _ch;
        int                             _ct;
        int                             _cw;
        int                             _h;
        plot::Line                      _lines[10];
        Fl_Menu_Button*                 _menu;
        size_t                          _selected_point;
        size_t                          _selected_line;
        size_t                          _size;
        std::string                     _tooltip;
        int                             _w;
        plot::Scale                     _x;
        plot::Scale                     _y;

    };
}

// MKALGAM_OFF

#endif
