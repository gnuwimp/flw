// Copyright 2022 gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#include "plot.h"
#include "util.h"
#include "theme.h"
#include <assert.h>
#include <math.h>
#include <FL/Fl.H>
#include <FL/Fl_File_Chooser.H>
#include <FL/Fl_Menu_Button.H>
#include <FL/fl_draw.H>

// MKALGAM_ON

namespace flw {
    namespace plot {
        static const char* const        SHOW_LABELS     = "Show line labels";
        static const char* const        SHOW_HLINES     = "Show horizontal lines";
        static const char* const        SHOW_VLINES     = "Show vertical lines";
        static const char* const        SAVE_FILE       = "Save to png file...";
        static const char* const        PRINT_DEBUG     = "Debug";

        //----------------------------------------------------------------------
        bool has_pairs(TYPE type) {
            return type != TYPE::CIRCLE && type != TYPE::FILLED_CIRCLE && type != TYPE::SQUARE;
        }

        //----------------------------------------------------------------------
        bool has_radius(TYPE type) {
            return type == TYPE::CIRCLE || type == TYPE::FILLED_CIRCLE || type == TYPE::SQUARE;
        }

        //----------------------------------------------------------------------
        bool load_data(Plot* plot, std::string filename) {
            auto buf = util::load_file(filename);
            auto nv  = json::NodeVector();
            auto ok  = 0;
            auto x   = Scale();
            auto y   = Scale();
            auto l   = false;
            auto h   = false;
            auto v   = false;
            auto err = (size_t) 0;

            plot->clear();
            plot->redraw();

            if (buf.p == nullptr) {
                fl_alert("error: failed to load %s", filename.c_str());
                return false;
            }
            else if ((err = json::parse(buf.p, nv)) != (size_t) -1) {
                fl_alert("error: failed to parse %s on position %u", filename.c_str(), (unsigned) err);
                return false;
            }

            for (const auto& n : json::find_children(nv, nv[0])) {
                auto children = json::find_children(nv, n);

                if (n.name == "descr" && n.is_object() == true) {
                    for (auto& n2 : children) {
                        if ((n2.name == "type" && n2.value == "flw::plot") || (n2.name == "version" && n2.value == "1")) {
                            ok++;
                        }
                    }
                }
                else if (ok != 2) {
                    fl_alert("error: unknown file format");
                    return false;
                }
                else if ((n.name == "xscale" || n.name == "yscale") && n.is_object() == true) {
                    auto& scale = (n.name == "xscale") ? x : y;

                    for (auto& n2 : children) {
                        if (n2.name == "label") {
                            scale.label = n2.value;
                        }
                        else if (n2.name == "color") {
                            scale.color = (Fl_Color) n2.toint();
                        }
                        else if (n2.name == "labels") {
                            for (auto& n3 : json::find_children(nv, n2)) {
                                if (n3.is_string() == true) {
                                    scale.labels.push_back(n3.value);
                                }
                            }
                        }
                    }
                }
                else if (n.name == "view" && n.is_object() == true) {
                    for (auto& n2 : children) {
                        if (n2.name == "labels") {
                            l = (bool) n2.tobool();
                        }
                        else if (n2.name == "horizontal") {
                            h = (bool) n2.tobool();
                        }
                        else if (n2.name == "vertical") {
                            v = (bool) n2.tobool();
                        }
                    }
                }
                else if (n.name == "line" && n.is_object() == true) {
                    plot::Line line;

                    for (auto& n2 : children) {
                        if (n2.name == "width" && n2.is_number() == true) {
                            line.width = (unsigned) n2.toint();
                        }
                        else if (n2.name == "color" && n2.is_number() == true) {
                            line.color = (Fl_Color) n2.toint();
                        }
                        else if (n2.name == "type" && n2.is_string() == true) {
                            line.type = plot::string_to_type(n2.value);
                        }
                        else if (n2.name == "label" && n2.is_string() == true) {
                            line.label = n2.value;
                        }
                        else if (n2.name == "p" && n2.is_array()) {
                            auto p = json::find_children(nv, n2);

                            if (p.size() == 2) {
                                line.points.push_back(plot::Point(p[0].tonumber(), p[1].tonumber()));
                            }
                        }
                    }

                    if (plot->add_line(line) == false) {
                        fl_alert("error: failed to add line that started on position %u", (unsigned) n.textpos);
                        plot->clear();
                        return false;
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
        void print(const PointVector& points) {
            auto c = 1;

            for (auto& p : points) {
                fprintf(stderr, "%3d| X=%16.6f   Y=%16.6f\n", c++, p.x, p.y);
            }

            fflush(stderr);
        }

        //----------------------------------------------------------------------
        bool save_data(const Plot* plot, std::string filename) {
            json::NodeVector nv;

            FLW_JSON_START(nv,
                FLW_JSON_START_OBJECT(
                    FLW_JSON_ADD_OBJECT("descr",
                        FLW_JSON_ADD_STRING("type", "flw::plot")
                        FLW_JSON_ADD_UINT("version", 1)
                    )
                    FLW_JSON_ADD_OBJECT("view",
                        FLW_JSON_ADD_BOOL("labels", plot->_view.labels)
                        FLW_JSON_ADD_BOOL("horizontal", plot->_view.horizontal)
                        FLW_JSON_ADD_BOOL("vertical", plot->_view.vertical)
                    )
                    FLW_JSON_ADD_OBJECT("xscale",
                        FLW_JSON_ADD_STRING("label", plot->_x.label)
                        FLW_JSON_ADD_UINT("color", plot->_x.color)
                        FLW_JSON_ADD_ARRAY_NL("labels",
                            for (const auto& l : plot->_x.labels)
                            FLW_JSON_ADD_STRING_TO_ARRAY(l)
                        )
                    )
                    FLW_JSON_ADD_OBJECT("yscale",
                        FLW_JSON_ADD_STRING("label", plot->_y.label)
                        FLW_JSON_ADD_UINT("color", plot->_y.color)
                        FLW_JSON_ADD_ARRAY_NL("labels",
                            for (const auto& l : plot->_y.labels)
                            FLW_JSON_ADD_STRING_TO_ARRAY(l)
                        )
                    )
                    for (size_t f = 0; f < plot->_size; f++) {
                    const auto& line = plot->_lines[f];
                    if (line.points.size() > 0) {
                    FLW_JSON_ADD_OBJECT("line",
                        FLW_JSON_ADD_UINT("width", line.width)
                        FLW_JSON_ADD_UINT("color", line.color)
                        FLW_JSON_ADD_STRING("type", plot::type_to_string(line.type))
                        FLW_JSON_ADD_STRING("label", line.label)
                        for (const auto& point : line.points) {
                        FLW_JSON_ADD_ARRAY_NL("p",
                            FLW_JSON_ADD_NUMBER_TO_ARRAY(point.x)
                            FLW_JSON_ADD_NUMBER_TO_ARRAY(point.y)
                        )
                        }
                    )
                    }
                    }
                )
            )

            auto js = json::tostring(nv);
            return util::save_file(filename, js.c_str(), js.length());
        }

        //----------------------------------------------------------------------
        TYPE string_to_type(std::string name) {
            if (name == "TYPE::LINE_DASH") return TYPE::LINE_DASH;
            else if (name == "TYPE::LINE_DOT") return TYPE::LINE_DOT;
            else if (name == "TYPE::LINE_WITH_SQUARE") return TYPE::LINE_WITH_SQUARE;
            else if (name == "TYPE::VECTOR") return TYPE::VECTOR;
            else if (name == "TYPE::CIRCLE") return TYPE::CIRCLE;
            else if (name == "TYPE::FILLED_CIRCLE") return TYPE::FILLED_CIRCLE;
            else if (name == "TYPE::SQUARE") return TYPE::SQUARE;
            else return TYPE::LINE;
        }

        //----------------------------------------------------------------------
        std::string type_to_string(TYPE type) {
            assert((size_t) type <= 8);
            static const char* NAMES[] = { "TYPE::LINE", "TYPE::LINE_DASH", "TYPE::LINE_DOT", "TYPE::LINE_WITH_SQUARE", "TYPE::VECTOR", "TYPE::CIRCLE", "TYPE::FILLED_CIRCLE", "TYPE::SQUARE", "", "", };
            return NAMES[(size_t) type];
        }
    }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
flw::plot::Scale::Scale() {
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
void flw::plot::Scale::calc(double canvas_size) {
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

    fr = util::count_decimals(tick);
}

//------------------------------------------------------------------------------
void flw::plot::Scale::debug(const char* s) const {
#ifdef DEBUG
    fprintf(stderr, "\tflw::plot::Scale: %s\n", s);
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
void flw::plot::Scale::measure_text(int cw) {
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
        const auto l1  = util::format_double(min, dec, ' ');
        const auto l2  = util::format_double(max, dec, ' ');

        text = (l1.length() > l2.length()) ? l1.length() * cw : l2.length() * cw;
    }
}

//------------------------------------------------------------------------------
void flw::plot::Scale::reset_min_max() {
    min = min2 = plot::MAX;
    max = max2 = plot::MIN;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
flw::plot::Line::Line() {
    color   = FL_BLUE;
    type    = plot::TYPE::LINE;
    visible = true;
    width   = 1;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
flw::Plot::Plot(int X, int Y, int W, int H, const char* l) : Fl_Group(X, Y, W, H, l) {
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

    _menu->add(flw::plot::SHOW_LABELS, 0, Plot::_CallbackToggle, this, FL_MENU_TOGGLE);
    _menu->add(flw::plot::SHOW_HLINES, 0, Plot::_CallbackToggle, this, FL_MENU_TOGGLE);
    _menu->add(flw::plot::SHOW_VLINES, 0, Plot::_CallbackToggle, this, FL_MENU_TOGGLE | FL_MENU_DIVIDER);
    _menu->add(flw::plot::SAVE_FILE, 0, Plot::_CallbackSave, this);
#ifdef DEBUG
    _menu->add(flw::plot::PRINT_DEBUG, 0, Plot::_CallbackDebug, this);
#endif
    _menu->type(Fl_Menu_Button::POPUP3);

    clear();
    update_pref();
    view_options();
}

//------------------------------------------------------------------------------
bool flw::Plot::add_line(const flw::plot::PointVector& points, flw::plot::TYPE type, unsigned width, std::string label, Fl_Color color) {
    if (_size >= plot::MAX_LINES || width == 0 || width > 100) {
        return false;
    }

    _lines[_size].points    = points;
    _lines[_size].label     = label;
    _lines[_size].width     = width;
    _lines[_size].color     = color;
    _lines[_size].type      = type;
    _lines[_size++].visible = true;
    _calc                   = true;

    return true;
}

//------------------------------------------------------------------------------
bool flw::Plot::add_line(const plot::Line& line) {
    if (_size >= plot::MAX_LINES || line.width < 1 || line.width > 100) {
        return false;
    }

    _lines[_size++] = line;

    return true;
}

//------------------------------------------------------------------------------
void flw::Plot::_calc_min_max() {
    _x.reset_min_max();
    _y.reset_min_max();

    for (size_t f = 0; f < _size; f++) {
        for (auto& p : _lines[f].points) {
            if (p.x < _x.min) {
                _x.min = p.x;
            }

            if (p.x > _x.max) {
                _x.max = p.x;
            }

            if (p.y < _y.min) {
                _y.min = p.y;
            }

            if (p.y > _y.max) {
                _y.max = p.y;
            }
        }
    }

    _x.min2 = _x.min;
    _x.max2 = _x.max;
    _y.min2 = _y.min;
    _y.max2 = _y.max;
}

//------------------------------------------------------------------------------
void flw::Plot::_CallbackDebug(Fl_Widget*, void* plot_object) {
    auto self = (Plot*) plot_object;
    self->debug();
}

//------------------------------------------------------------------------------
void flw::Plot::_CallbackSave(Fl_Widget*, void* plot_object) {
    auto self = (Plot*) plot_object;
    flw::util::png_save("", self->window(), self->x(),  self->y(),  self->w(),  self->h());
}

//------------------------------------------------------------------------------
void flw::Plot::_CallbackToggle(Fl_Widget*, void* plot_object) {
    auto self = (Plot*) plot_object;

    self->_view.labels     = flw::util::menu_item_value(self->_menu, plot::SHOW_LABELS);
    self->_view.horizontal = flw::util::menu_item_value(self->_menu, plot::SHOW_HLINES);
    self->_view.vertical   = flw::util::menu_item_value(self->_menu, plot::SHOW_VLINES);

    self->redraw();
}

//------------------------------------------------------------------------------
void flw::Plot::clear() {
    _calc           = false;
    _ch             = 0;
    _ct             = 0;
    _cw             = 0;
    _h              = 0;
    _selected_line  = 0;
    _selected_point = -1;
    _size           = 0;
    _w              = 0;
    _x              = flw::plot::Scale();
    _y              = flw::plot::Scale();

    selection_color(FL_FOREGROUND_COLOR);
    clear_points();
}

//------------------------------------------------------------------------------
void flw::Plot::clear_points() {
    for (size_t f = 0; f < _size; f++) {
        _lines[f] = plot::Line();
    }
}

//------------------------------------------------------------------------------
// Create tooltip with x and y values by using  mouse cursor position
// If ctrl is true then the hit area is increased
//
void flw::Plot::_create_tooltip(bool ctrl) {
    const auto X   = Fl::event_x();
    const auto Y   = Fl::event_y();
    const auto old = _tooltip;

    _tooltip        = "";
    _selected_point = -1;

    if (X >= _area.x && X < _area.x2() && Y >= _area.y && Y < _area.y2()) {
        auto fr = (_x.fr > _y.fr) ? _x.fr : _y.fr;

        if (_selected_line < _size) {
            const plot::Line&        line   = _lines[_selected_line];
            const bool               radius = flw::plot::has_radius(line.type);
            const plot::PointVector& points = line.points;
            const int                rad    = (ctrl == true) ? (radius == true) ? line.width : line.width * 2 : (radius == true) ? line.width / 2 : line.width;
            const double             xv1    = ((double) (X - _area.x - rad) / _x.pixel) + _x.min;
            const double             xv2    = ((double) (X - _area.x + rad) / _x.pixel) + _x.min;
            const double             yv1    = ((double) (_area.y2() - Y - rad) / _y.pixel) + _y.min;
            const double             yv2    = ((double) (_area.y2() - Y + rad) / _y.pixel) + _y.min;

            for (size_t i = 0; i < points.size(); i++) {
                const plot::Point& point = points[i];

                if (point.x >= xv1 && point.x <= xv2 && point.y >= yv1 && point.y <= yv2) {
                    auto xl  = util::format_double(point.x, fr, ' ');
                    auto yl  = util::format_double(point.y, fr, ' ');
                    auto len = (xl.length() > yl.length()) ? xl.length() : yl.length();

                    _tooltip = flw::util::format("Line %d, Point %d\nX = %*s\nY = %*s", (int) _selected_line + 1, (int) i + 1, len, xl.c_str(), len, yl.c_str());
                    _selected_point = i;
                    break;
                }
            }
        }

        if (_tooltip == "") {
            auto xv  = ((double) (X - _area.x) / _x.pixel) + _x.min;
            auto yv  = ((double) (_area.y2() - Y) / _y.pixel) + _y.min;
            auto xl  = util::format_double(xv, fr, ' ');
            auto yl  = util::format_double(yv, fr, ' ');
            auto len = (xl.length() > yl.length()) ? xl.length() : yl.length();

            _tooltip = flw::util::format("X = %*s\nY = %*s", len, xl.c_str(), len, yl.c_str());
        }
    }

    if (_tooltip != "" || old != "") {
        redraw();
    }
}

//------------------------------------------------------------------------------
void flw::Plot::debug() const {
#ifdef DEBUG
    fprintf(stderr, "\nflw::Plot: %d\n", (int) _size);
    fprintf(stderr, "\tcanvas          = (%d, %d) - (%d, %d) - (%d, %d)\n", x(), y(), w(), h(), x() + w(), y() + h());
    fprintf(stderr, "\tarea            = (%d, %d) - (%d, %d) - (%d, %d)\n", _area.x, _area.y, _area.w, _area.h, _area.x + _area.w, _area.y + _area.h);
    fprintf(stderr, "\tcw, ch          = (%d, %d)\n", _cw, _ch);
    fprintf(stderr, "\tselected_line   = %d\n", (int) _selected_line);
    fprintf(stderr, "\tselected_point  = %d\n", (int) _selected_point);
    fprintf(stderr, "\tline_count      = %d\n", (int) _size);
    fprintf(stderr, "\tview_labels     = %s\n", _view.labels ? "YES" : "NO");
    fprintf(stderr, "\tview_horizontal = %s\n", _view.horizontal ? "YES" : "NO");
    fprintf(stderr, "\tview_vertical   = %s\n", _view.vertical ? "YES" : "NO");

    _x.debug("X");
    _y.debug("Y");
    for (size_t f = 0; f < _size; f++) {
        fprintf(stderr, "\tflw::Plot::Line: %d\n", (int) f);
        fprintf(stderr, "\t\tcolor   = %u\n", _lines[f].color);
        fprintf(stderr, "\t\tlabel   = '%s'\n", _lines[f].label.c_str());
        fprintf(stderr, "\t\tpoints  = %d\n", (int) _lines[f].points.size());
        fprintf(stderr, "\t\ttype    = %d\n", (int) _lines[f].type);
        fprintf(stderr, "\t\tvisible = %s\n", _lines[f].visible ? "YES" : "NO");
        fprintf(stderr, "\t\twidth   = %d px\n", (int) _lines[f].width);
    }
    // plot::print(_lines[0].points);
    // plot::print(_lines[1].points);
    fflush(stderr);
#endif
}

//------------------------------------------------------------------------------
void flw::Plot::draw() {
#ifdef DEBUG
    // auto t = util::time_milli();
#endif

    fl_font(flw::PREF_FIXED_FONT, flw::PREF_FIXED_FONTSIZE);
    Fl_Group::draw();

    if (_area.w < 50 || _area.h < 50 || _x.min >= _x.max || _x.pixel * _x.tick < 1.0 || _y.min >= _y.max || _y.pixel * _y.tick < 1.0) {
        return;
    }

    fl_push_clip(x(), y(), w(), h());
    _draw_xlabels();
    _draw_xlabels2();
    _draw_ylabels();
    _draw_ylabels2();
    _draw_labels();
    fl_pop_clip();

    fl_push_clip(_area.x, _area.y, _area.w, _area.h);
    _draw_lines();
    _draw_line_labels();
    _draw_tooltip();
    fl_pop_clip();

    fl_color(FL_FOREGROUND_COLOR);
    fl_line_style(FL_SOLID, 1);
    fl_rect(_area.x, _area.y, _area.w, _area.h, FL_FOREGROUND_COLOR);
    fl_line_style(0);

#ifdef DEBUG
    // fprintf(stderr, "Plot::draw: %d mS\n", util::time_milli() - t);
    // fflush(stderr);
#endif
}

//------------------------------------------------------------------------------
void flw::Plot::_draw_labels() {
    if (_x.label != "") {
        fl_color(_x.color);
        fl_draw (_x.label.c_str(), _area.x, y() + h() - _ch, _area.w, flw::PREF_FIXED_FONTSIZE, FL_ALIGN_CENTER | FL_ALIGN_INSIDE);
    }

    if (_y.label != "") {
        auto y = _area.y + (_area.h / 2) + (_y.label.length() * _cw / 2);
        fl_color(_y.color);
        fl_draw (90, _y.label.c_str(), x() + _ch, y);
    }
}

//------------------------------------------------------------------------------
void flw::Plot::_draw_line_labels() {
    if (_view.labels == true) {
        const int X = _area.x + 6;
        int       Y = _area.y + 6;
        int       W = 0;
        int       H = 0;

        for (size_t f = 0; f < _size; f++) {
            const auto& label = _lines[f].label;

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
                const plot::Line& line = _lines[f];

                if (line.label != "") {
                    auto label = line.label;

                    if (f == _selected_line) {
                        label = "@-> " + label;
                    }

                    fl_color((line.visible == false) ? FL_GRAY : _lines[f].color);
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
void flw::Plot::_draw_lines() {
    const int X = _area.x;
    const int Y = _area.y2() - 1;

    for (size_t f = 0; f < _size; f++) {
        const plot::Line& data  = _lines[f];
        const plot::TYPE  type  = data.type;
        const int         size1 = data.width;
        const int         size2 = size1 / 2;
        int               x1    = 10000;
        int               y1    = 0;

        if (data.visible == false) {
            continue;
        }

        for (size_t i = 0; i < data.points.size(); i++) {
            const plot::Point& p = data.points[i];
            const int          x = X + (int) ((p.x - _x.min) * _x.pixel);
            const int          y = Y - (int) ((p.y - _y.min) * _y.pixel);

            if (type == plot::TYPE::LINE || type == plot::TYPE::LINE_DASH || type == plot::TYPE::LINE_DOT || type == plot::TYPE::LINE_WITH_SQUARE) {
                if (x1 == 10000) {
                    x1 = x;
                    y1 = y;
                }
                else {
                    fl_color((_selected_point == i && _selected_line == f) ? selection_color() : data.color);
                    _draw_lines_style(type, size1);
                    fl_line(x1, y1, x, y);
                    x1 = x;
                    y1 = y;
                }

                if (type == plot::TYPE::LINE_WITH_SQUARE) {
                    fl_color((_selected_point == i && _selected_line == f) ? selection_color() : data.color);
                    _draw_lines_style(type, size1);
                    fl_rectf(x - size1 - 3, y - size1 - 3, size1 * 2 + 6, size1 * 2 + 6);
                }
            }
            else if (type == plot::TYPE::VECTOR) {
                if (x1 == 10000) {
                    x1 = x;
                    y1 = y;
                }
                else {
                    fl_color(((_selected_point == i || _selected_point == i - 1) && _selected_line == f) ? selection_color() : data.color);
                    _draw_lines_style(type, size1);
                    fl_line(x, y, x1, y1);
                    x1 = 10000;
                }
            }
            else if (type == plot::TYPE::CIRCLE) {
                fl_color((_selected_point == i && _selected_line == f) ? selection_color() : data.color);
                _draw_lines_style(type, 1);
                fl_circle(x, y - 1, size2);
            }
            else if (type == plot::TYPE::FILLED_CIRCLE) {
                fl_color((_selected_point == i && _selected_line == f) ? selection_color() : data.color);
                _draw_lines_style(type, 1);
                fl_begin_polygon();
                fl_circle(x, y - 1, size2);
                fl_end_polygon();
            }
            else if (type == plot::TYPE::SQUARE) {
                fl_color((_selected_point == i && _selected_line == f) ? selection_color() : data.color);
                _draw_lines_style(type, 1);
                fl_rectf(x - size2, y - size2 - 1, size1, size1);
            }
        }
    }

    fl_line_style(0);
}

//------------------------------------------------------------------------------
void flw::Plot::_draw_lines_style(plot::TYPE type, int size) {
    if (type == plot::TYPE::LINE) {
        fl_line_style(FL_SOLID, size);
    }
    else if (type == plot::TYPE::LINE_DASH) {
        fl_line_style(FL_DASH, size);
    }
    else if (type == plot::TYPE::LINE_DOT) {
        fl_line_style(FL_DOT, size);
    }
    else if (type == plot::TYPE::VECTOR) {
        fl_line_style(FL_SOLID, size);
    }
    else if (type == plot::TYPE::LINE_WITH_SQUARE) {
        fl_line_style(FL_SOLID, size);
    }
    else if (type == plot::TYPE::CIRCLE) {
        fl_line_style(FL_SOLID, size);
    }
    else if (type == plot::TYPE::FILLED_CIRCLE) {
        fl_line_style(FL_SOLID, size);
    }
    else if (type == plot::TYPE::SQUARE) {
        fl_line_style(FL_SOLID, size);
    }
    else {
        fl_line_style(FL_SOLID, 1);
    }
}

//------------------------------------------------------------------------------
void flw::Plot::_draw_tooltip() {
    if (_tooltip == "") {
        return;
    }

    auto X = Fl::event_x();
    auto Y = Fl::event_y();

    if (X > _area.x2() - flw::PREF_FIXED_FONTSIZE * 19) {
        X -= flw::PREF_FIXED_FONTSIZE * 18;
    }
    else {
        X += flw::PREF_FIXED_FONTSIZE * 2;
    }

    if (Y > _area.y2() - flw::PREF_FIXED_FONTSIZE * 8) {
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
    fl_line(Fl::event_x(), _area.y, Fl::event_x(), _area.y2() - 1);
    fl_line(_area.x, Fl::event_y(), _area.x2() - 1, Fl::event_y());
    fl_draw(_tooltip.c_str(), X + flw::PREF_FIXED_FONTSIZE, Y, flw::PREF_FIXED_FONTSIZE * 14, flw::PREF_FIXED_FONTSIZE * 4, FL_ALIGN_LEFT | FL_ALIGN_CENTER | FL_ALIGN_INSIDE);
    fl_line_style(0);
}

//------------------------------------------------------------------------------
void flw::Plot::_draw_xlabels() {
    double       X    = _area.x;
    const int    Y    = _area.y2();
    const int    W    = x2();
    const double inc  = _x.pixel * _x.tick;
    double       val  = _x.min;
    int          last = -10'000;
    const int    tw   = _x.text / 2;

    while (X < _area.x2()) {
        if (_view.vertical == true && X > (_area.x + 4) && X < (_area.x2() - 4)) {
            fl_color(fl_color_average(FL_FOREGROUND_COLOR, FL_BACKGROUND2_COLOR, 0.2));
            fl_line(X, _area.y + 1, X, _area.y2() - 2);
        }

        if (X > (last + tw) && (X + tw) < W) {
            auto label = util::format_double(val, _x.fr, '\'');

            if ((int) (X + 1) == (_area.x2() - 1)) { // Adjust so rightmost pixel is aligned
                X += 1.0;
            }

            fl_color(FL_FOREGROUND_COLOR);
            fl_line(X, Y, X, Y + _ct * 2);

            if (_x.labels.size() == 0) {
                fl_draw(label.c_str(), X - tw, Y + _ct * 2, tw * 2, flw::PREF_FIXED_FONTSIZE, FL_ALIGN_CENTER | FL_ALIGN_INSIDE);
            }

            last = X + tw + _cw;
        }
        else {
            fl_color(FL_FOREGROUND_COLOR);
            fl_line(X, Y, X, Y + _ct);
        }

        X   += inc;
        val += _x.tick;
    }
}

//------------------------------------------------------------------------------
void flw::Plot::_draw_xlabels2() {
    if (_x.labels.size() > 0) {
        auto index = (size_t) 0;
        auto pair  = plot::has_pairs(_lines[0].type);

        for (size_t i = 0; i < _lines[0].points.size(); i++) {
            const plot::Point& point = _lines[0].points[i];

            if (index >= _x.labels.size()) {
                break;
            }

            if ((i % 2 == 1 && pair == true) || pair == false) {
                const int X = _area.x + (int) ((point.x - _x.min) * _x.pixel);
                const int W = _x.labels[index].length() * _cw / 2;

                fl_draw(_x.labels[index].c_str(), X - W, _area.y2() + _ct * 2, W * 2, flw::PREF_FIXED_FONTSIZE, FL_ALIGN_CENTER | FL_ALIGN_INSIDE);
                index++;
            }
        }
    }
}

//------------------------------------------------------------------------------
void flw::Plot::_draw_ylabels() {
    const int    X    = x();
    double       Y    = _area.y2() - 1.0;
    const int    W    = _area.x - X - flw::PREF_FIXED_FONTSIZE;
    const double inc  = _y.pixel * _y.tick;
    double       val  = _y.min;
    int          last = 10'000;

    while ((int) (Y + 0.5) >= _area.y) {
        if (_view.horizontal == true && Y > (_area.y + _ct) && Y < (_area.y2() - _ct)) {
            fl_color(fl_color_average(FL_FOREGROUND_COLOR, FL_BACKGROUND2_COLOR, 0.2));
            fl_line(_area.x + 1, Y, _area.x2() - 2, Y);
        }

        if (Y < last) {
            const auto label = util::format_double(val, _y.fr, '\'');

            if (Y < (double) _area.y) { // Adjust top most pixel
                Y = _area.y;
            }

            fl_color(FL_FOREGROUND_COLOR);
            fl_line(_area.x, Y, _area.x - _ct * 2, Y);

            if (_y.labels.size() == 0) {
                fl_draw(label.c_str(), X, Y - _ct * 2, W, flw::PREF_FIXED_FONTSIZE, FL_ALIGN_RIGHT | FL_ALIGN_INSIDE);
            }

            last = Y - flw::PREF_FIXED_FONTSIZE;
        }
        else {
            fl_color(FL_FOREGROUND_COLOR);
            fl_line(_area.x, Y, _area.x - _ct, Y);
        }

        Y   -= inc;
        val += _y.tick;
    }
}

//------------------------------------------------------------------------------
void flw::Plot::_draw_ylabels2() {
    if (_y.labels.size() > 0) {
        size_t     index = 0;
        const bool pair  = plot::has_pairs(_lines[0].type);

        for (size_t i = 0; i < _lines[0].points.size(); i++) {
            const plot::Point& p = _lines[0].points[i];

            if (index >= _y.labels.size()) {
                break;
            }

            if ((i % 2 == 1 && pair == true) || pair == false) {
                const int Y = _area.y2() - 1 - ((p.y - _y.min) * _y.pixel);

                fl_draw(_y.labels[index].c_str(), x() + _cw * 3, Y - _ct * 2, _y.text, flw::PREF_FIXED_FONTSIZE, FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
                index++;
            }
        }
    }
}

//------------------------------------------------------------------------------
int flw::Plot::handle(int event) {
    if (event == FL_PUSH) {
        if (Fl::event_button1() != 0) {
            _create_tooltip(Fl::event_ctrl());

            if (_tooltip != "") {
                return 1;
            }
        }
        else if (Fl::event_button3() != 0) {
            flw::util::menu_item_set(_menu, plot::SHOW_LABELS, _view.labels);
            flw::util::menu_item_set(_menu, plot::SHOW_HLINES, _view.horizontal);
            flw::util::menu_item_set(_menu, plot::SHOW_VLINES, _view.vertical);
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
                _lines[line].visible = !_lines[line].visible;
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
void flw::Plot::resize(int X, int Y, int W, int H) {
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
        _x.calc(W - flw::PREF_FIXED_FONTSIZE * 6);
        _y.calc(H - flw::PREF_FIXED_FONTSIZE * 6);
        _x.measure_text(_cw);
        _y.measure_text(_cw);
    }

    _area.x = X + (flw::PREF_FIXED_FONTSIZE * 2) + _y.text + ((_y.label != "") ? flw::PREF_FIXED_FONTSIZE : 0);
    _area.y = Y + flw::PREF_FIXED_FONTSIZE;
    _area.w = W - (_area.x - X) - flw::PREF_FIXED_FONTSIZE * 2;
    _area.h = H - (flw::PREF_FIXED_FONTSIZE * 3) - ((_x.label != "") ? flw::PREF_FIXED_FONTSIZE : 0);

    _x.calc(_area.w - 1);
    _y.calc(_area.h - 1);

    _calc = false;
    _w    = W;
    _h    = H;
}

//------------------------------------------------------------------------------
void flw::Plot::update_pref() {
    _menu->textfont(flw::PREF_FONT);
    _menu->textsize(flw::PREF_FONTSIZE);
}

// MKALGAM_OFF
