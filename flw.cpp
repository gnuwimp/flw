// This source file is an amalgamation of one or more source files.
// Copyright 2016 - 2021 gnuwimp@gmail.com
// Released under the GNU General Public License v3.0
#include "flw.h"
#include <algorithm>
#include <math.h>
#include <stdarg.h>
#include <time.h>
#include <sys/timeb.h>
#include <unistd.h>
#include <FL/Fl_File_Chooser.H>
#include <FL/Fl_Float_Input.H>
#include <FL/Fl_Help_View.H>
#include <FL/Fl_Hor_Fill_Slider.H>
#include <FL/Fl_Input_Choice.H>
#include <FL/Fl_Int_Input.H>
#include <FL/Fl_Menu_Button.H>
#include <FL/Fl_Multiline_Output.H>
#include <FL/Fl_Repeat_Button.H>
#include <FL/Fl_Return_Button.H>
#include <FL/Fl_Secret_Input.H>
#include <FL/Fl_Toggle_Button.H>
#include <FL/Fl_Value_Slider.H>
#include <FL/fl_show_colormap.H>
#include <FL/x.H>
#ifdef _WIN32
    #include <FL/x.H>
    #include <windows.h>
#elif defined(__linux__)
    #include <FL/Fl_Pixmap.H>
    #include <FL/Fl_RGB_Image.H>
#endif
#ifdef FLW_USE_PNG
    #include <FL/Fl_PNG_Image.H>
#endif


namespace flw {
    namespace chart {
        const int                       MIN_TICK        = 3;
        const int                       MAX_TICK        = 100;
        const size_t                    MAX_AREA        = 3;
        const size_t                    MAX_LINE        = 10;
        const double                    MIN_VAL         = -999'999'999'999'999.0;
        const double                    MAX_VAL         =  999'999'999'999'999.0;
        static const int                MAX_VLINES      = 100;
        static const int                MAX_LINE_WIDTH  = 100;
        static const char* const        SHOW_LABELS     = "Show line labels";
        static const char* const        SHOW_HLINES     = "Show horizontal lines";
        static const char* const        SHOW_VLINES     = "Show vertical lines";
        static const char* const        RESET_SELECT    = "Reset line selection and visibility";
        static const char* const        SAVE_PNG        = "Save png to file...";
        static const char* const        PRINT_DEBUG     = "Debug";

        //----------------------------------------------------------------------
        size_t bsearch(const flw::PriceVector& prices, const flw::Price& key) {
            auto it = std::lower_bound(prices.begin(), prices.end(), key);

            if (it == prices.end() || *it != key) {
                return (size_t) -1;
            }
            else {
                return std::distance(prices.begin(), it);
            }
        }

        //----------------------------------------------------------------------
        bool has_high_low(flw::chart::TYPE chart_type) {
            return chart_type == flw::chart::TYPE::BAR || chart_type == flw::chart::TYPE::VERTICAL || chart_type == flw::chart::TYPE::CLAMP_VERTICAL;
        }

        //----------------------------------------------------------------------
        bool has_resizable_width(flw::chart::TYPE chart_type) {
            return chart_type == flw::chart::TYPE::BAR || chart_type == flw::chart::TYPE::VERTICAL || chart_type == flw::chart::TYPE::CLAMP_VERTICAL;
        }

        //----------------------------------------------------------------------
        bool has_time(flw::Date::RANGE date_range) {
            return date_range == flw::Date::RANGE::HOUR || date_range == flw::Date::RANGE::MIN || date_range == flw::Date::RANGE::SEC;
        }

        //----------------------------------------------------------------------
        bool load_data(flw::Chart* chart, std::string filename) {
            struct ChartData {
                bool             labels;
                bool             horizontal;
                bool             vertical;
                int              date_range;
                int              margin_left;
                int              margin_right;
                int              size0;
                int              size1;
                int              size2;
                int              tick_width;
                flw::PriceVector block;
            };

            struct LineData {
                int              area;
                std::string      label;
                int              type;
                int              width;
                int              color;
                int              align;
                double           clamp_max;
                double           clamp_min;
                flw::PriceVector prices;
            };

            flw::Buf buf = flw::util::load_file(filename);

            if (buf.p == nullptr) {
                return false;
            }

            const std::string str = buf.p;

            if (str.length() != buf.s) {
                fl_alert("error: file %s is not an text file", filename.c_str());
                return false;
            }

            chart->clear();

            auto lines    = flw::util::split(str, "\n");
            auto state    = 0;
            auto count    = 0;
            auto data     = ChartData { true, true, true, (int) flw::Date::RANGE::DAY, 6, 1, 100, 0, 0, 1, PriceVector(), } ;
            auto linedata = LineData { 0, "", (int) flw::chart::TYPE::LINE, 1, FL_BLUE, FL_ALIGN_LEFT, flw::chart::MIN_VAL, flw::chart::MAX_VAL, PriceVector(), };

            for (const auto& line : lines) {
                count++;

                if (line == "") {
                    continue;
                }

                auto columns = flw::util::split(line, "\t");

                if (columns.size() == 0) {
                    continue;
                }

                auto key = columns[0];

                if (state == 0 && key == "chart") {
                    if (columns.size() < 2 || columns[1] != "1") goto ERR;
                    state = 1;
                }
                else if (state == 0) {
                    goto ERR;
                }
                else if (state == 1) {
                    if (key == "start_area") {
                        state = 2;
                    }
                    if (key == "start_line") {
                         state    = 3;
                         linedata = LineData { 0, "", (int) flw::chart::TYPE::LINE, 1, FL_BLUE, FL_ALIGN_LEFT, flw::chart::MIN_VAL, flw::chart::MAX_VAL, PriceVector(), };
                    }
                    else if (key == "start_block") {
                         state = 4;
                    }
                }
                else if (state == 2) {
                    if (key == "area") {
                        if (columns.size() != 3) goto ERR;
                        auto area = (int) flw::util::to_int(columns[1].c_str());
                        auto size = (int) flw::util::to_int(columns[2].c_str());

                        if (area == 2) data.size2 = size;
                        else if (area == 1) data.size1 = size;
                        else data.size0 = size;
                    }
                    else if (key == "tick_width") {
                        if (columns.size() < 2) goto ERR;
                        data.tick_width = (int) flw::util::to_int(columns[1].c_str());
                    }
                    else if (key == "date_range") {
                        if (columns.size() < 2) goto ERR;
                        data.date_range = (int) flw::util::to_int(columns[1].c_str());
                    }
                    else if (key == "margin_left") {
                        if (columns.size() < 2) goto ERR;
                        data.margin_left = (int) flw::util::to_int(columns[1].c_str());
                    }
                    else if (key == "margin_right") {
                        if (columns.size() < 2) goto ERR;
                        data.margin_right = (int) flw::util::to_int(columns[1].c_str());
                    }
                    else if (key == "labels") {
                        if (columns.size() < 2) goto ERR;
                        data.labels = (int) flw::util::to_int(columns[1].c_str());
                    }
                    else if (key == "horizontal") {
                        if (columns.size() < 2) goto ERR;
                        data.horizontal = (int) flw::util::to_int(columns[1].c_str());
                    }
                    else if (key == "vertical") {
                        if (columns.size() < 2) goto ERR;
                        data.vertical = (int) flw::util::to_int(columns[1].c_str());
                    }
                    else if (key == "end_chart") {
                        if (chart->area_size(data.size0, data.size1, data.size2) == false) goto ERR;
                        if (chart->tick_width(data.tick_width) == false) goto ERR;
                        if (chart->date_range((flw::Date::RANGE) data.date_range) == false) goto ERR;
                        if (chart->margin(data.margin_left, data.margin_right) == false) goto ERR;
                        chart->view_options(data.labels, data.horizontal, data.vertical);
                        state = 1;
                    }
                }
                else if (state == 3) {
                    if (key == "area") {
                        if (columns.size() < 2) goto ERR;
                        linedata.area = (int) flw::util::to_int(columns[1].c_str());
                    }
                    else if (key == "type") {
                        if (columns.size() < 2) goto ERR;
                        linedata.type = (int) flw::util::to_int(columns[1].c_str());
                    }
                    else if (key == "color") {
                        if (columns.size() < 2) goto ERR;
                        linedata.color = (int) flw::util::to_int(columns[1].c_str());
                    }
                    else if (key == "width") {
                        if (columns.size() < 2) goto ERR;
                        linedata.width = (int) flw::util::to_int(columns[1].c_str());
                    }
                    else if (key == "align") {
                        if (columns.size() < 2) goto ERR;
                        linedata.align = (int) flw::util::to_int(columns[1].c_str());
                    }
                    else if (key == "clamp_max") {
                        if (columns.size() < 2) goto ERR;
                        linedata.clamp_max = flw::util::to_double(columns[1].c_str());
                    }
                    else if (key == "clamp_min") {
                        if (columns.size() < 2) goto ERR;
                        linedata.clamp_min = flw::util::to_double(columns[1].c_str());
                    }
                    else if (key == "label") {
                        if (columns.size() >= 2) linedata.label = columns[1];
                        else linedata.label = "";
                    }
                    else if (key == "p") {
                        auto date = flw::Date::FromString((columns.size() > 1) ? columns[1].c_str() : "");

                        if (date.year() == 1 && date.month() == 1 && date.day() == 1) {
                            goto ERR;
                        }
                        else if (columns.size() > 4) {
                            auto h = flw::util::to_double(columns[2].c_str());
                            auto l = flw::util::to_double(columns[3].c_str());
                            auto c = flw::util::to_double(columns[4].c_str());

                            linedata.prices.push_back(Price(date.format(chart->_date_format), h, l, c));
                        }
                        else if (columns.size() > 2) {
                            auto c = flw::util::to_double(columns[2].c_str());

                            linedata.prices.push_back(Price(date.format(chart->_date_format), c));
                        }
                        else {
                            goto ERR;
                        }
                    }
                    else if (key == "end_line") {
                        if (chart->add_line(linedata.area, linedata.prices, linedata.label, (flw::chart::TYPE) linedata.type, linedata.align, linedata.color, linedata.width, linedata.clamp_min, linedata.clamp_max) == false) {
                            goto ERR;
                        }

                        state = 1;
                    }
                }
                else if (state == 4) {
                    if (key == "d") {
                        auto date = flw::Date::FromString((columns.size() > 1) ? columns[1].c_str() : "");

                        if (date.year() == 1 && date.month() == 1 && date.day() == 1) {
                            goto ERR;
                        }
                        else {
                            data.block.push_back(Price(date.format(chart->_date_format), 0.0));
                        }
                    }
                    else if (key == "end_block") {
                        chart->block_dates(data.block);
                        state = 1;
                    }
                }
            }

            if (state != 1) goto ERR;
            chart->init(true);
            chart->redraw();
            return true;

        ERR:
            chart->clear();
            chart->redraw();
            fl_alert("error: can't parse %s\nlast read line was %d", filename.c_str(), count);
            return false;
        }

        //----------------------------------------------------------------------
        bool save_data(const flw::Chart* chart, std::string filename) {
            int area_count = 0;
            std::string res;

            res.resize(1'000'000);

            res  = "chart" "\t" "1" "\n\n";
            res += "start_area" "\n";

            for (auto& area : chart->_areas) {
                res += flw::util::format("area" "\t" "%d" "\t" "%d" "\n", area_count++, area.percent);
            }

            res += flw::util::format("tick_width"   "\t" "%d" "\n", chart->_tick_width);
            res += flw::util::format("date_range"   "\t" "%d" "\n", (int) chart->_date_range);
            res += flw::util::format("margin_left"  "\t" "%d" "\n", (int) chart->_margin_left);
            res += flw::util::format("margin_right" "\t" "%d" "\n", (int) chart->_margin_right);
            res += flw::util::format("labels"       "\t" "%d" "\n", (int) chart->_view.labels);
            res += flw::util::format("horizontal"   "\t" "%d" "\n", (int) chart->_view.horizontal);
            res += flw::util::format("vertical"     "\t" "%d" "\n", (int) chart->_view.vertical);
            res += "end_chart" "\n";
            area_count = 0;

            for (const auto& area : chart->_areas) {
                for (const auto& line : area.lines) {
                    if (line.points.size() > 0) {
                        res += flw::util::format("\nstart_line\n");
                        res += flw::util::format("area"      "\t" "%d\n", area_count);
                        res += flw::util::format("label"     "\t" "%s\n", line.label.c_str());
                        res += flw::util::format("type"      "\t" "%d\n", line.type);
                        res += flw::util::format("align"     "\t" "%d\n", line.align);
                        res += flw::util::format("color"     "\t" "%u\n", line.color);
                        res += flw::util::format("width"     "\t" "%d\n", line.width);
                        res += flw::util::format("clamp_max" "\t" "%f\n", line.clamp_max);
                        res += flw::util::format("clamp_min" "\t" "%f\n", line.clamp_min);

                        for (auto& price : line.points) {
                            res += "p\t" + price.format_price((chart->has_time() == true) ? flw::Date::FORMAT::ISO_TIME : flw::Date::FORMAT::ISO, true, false) + "\n";
                        }

                        res += "end_line\n";
                    }
                }

                area_count++;
            }

            res += "\nstart_block\n";
            for (const auto& date : chart->_block_dates) {
                res += "d\t" + date.format_date((chart->has_time() == true) ? flw::Date::FORMAT::ISO_TIME : flw::Date::FORMAT::ISO) + "\n";
            }
            res += flw::util::format("end_block\n");

            return flw::util::save_file(filename, res.c_str(), res.length());
        }
    }
}

//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
flw::chart::Area::Area() {
    for (size_t f = 0; f < flw::chart::MAX_LINE; f++) {
        lines.push_back(flw::chart::Line());
    }

    clear(false);
}

//------------------------------------------------------------------------------
void flw::chart::Area::clear(bool clear_data) {
    count    = 0;
    x        = 0.0;
    y        = 0.0;
    w        = 0.0;
    h        = 0.0;
    selected = 0;

    left.clear();
    right.clear();

    if (clear_data == true) {
        percent = 0;

        for (size_t f = 0; f < flw::chart::MAX_LINE; f++) {
            lines[f] = flw::chart::Line();
        }
    }
}

//------------------------------------------------------------------------------
void flw::chart::Area::debug(int num) const {
#ifdef DEBUG
    auto count = 1;

    for (const auto& line : lines) {
        if (line.points.size() > 0) {
            count = 0;
        }
    }

    if (count == 0 || num == 0) {
        fprintf(stderr, "\t----------------------\n");
        fprintf(stderr, "\tflw::chart::Area: %d\n", num);
        fprintf(stderr, "\t\tcount:    %d\n", (int) count);
        fprintf(stderr, "\t\tx:        %4.0f\n", x);
        fprintf(stderr, "\t\ty:        %4.0f\n", y);
        fprintf(stderr, "\t\tw:        %4.0f\n", w);
        fprintf(stderr, "\t\th:        %4.0f\n", h);
        fprintf(stderr, "\t\tpercent:  %4d\n", percent);
        fprintf(stderr, "\t\tselected: %4d\n", (int) selected);

        left.debug("left");
        right.debug("right");
        count = 0;

        for (const auto& line : lines) {
            if (line.points.size() > 0) {
                line.debug(count++);
            }
        }
    }
#else
    (void) num;
#endif
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
flw::chart::Line::Line() {
    clear();
}

//------------------------------------------------------------------------------
void flw::chart::Line::clear() {
    points.clear();

    align     = FL_ALIGN_CENTER;
    type      = flw::chart::TYPE::LINE;
    clamp_max = flw::chart::MAX_VAL;
    clamp_min = flw::chart::MIN_VAL;
    color     = FL_FOREGROUND_COLOR;
    label     = "";
    max       = flw::chart::MIN_VAL;
    min       = flw::chart::MAX_VAL;
    visible   = true;
    width     = 1;
}

//------------------------------------------------------------------------------
void flw::chart::Line::debug(int num) const {
#ifdef DEBUG
    fprintf(stderr, "\t\t---------------------------------------------\n");
    fprintf(stderr, "\t\tflw::chart::Line: %d\n", num);
    fprintf(stderr, "\t\t\talign:      %25s\n", (align == FL_ALIGN_LEFT) ? "LEFT" : "RIGHT");
    fprintf(stderr, "\t\t\ttype:       %25d\n", (int) type);
    fprintf(stderr, "\t\t\tclamp_max:  %25.4f\n", clamp_max);
    fprintf(stderr, "\t\t\tclamp_min:  %25.4f\n", clamp_min);
    fprintf(stderr, "\t\t\tlabel: %30s\n", label.c_str());
    fprintf(stderr, "\t\t\tprices:     %25d\n", (int) points.size());
    fprintf(stderr, "\t\t\tvisible:    %25d\n", visible);
    fprintf(stderr, "\t\t\twidth:      %25d\n", width);

    if (points.size() > 1) {
        fprintf(stderr, "\t\t\tfirst:      %25s\n", points.front().date.c_str());
        fprintf(stderr, "\t\t\tfirst:      %25f\n", points.front().close);
        fprintf(stderr, "\t\t\tlast:       %25s\n", points.back().date.c_str());
        fprintf(stderr, "\t\t\tlast:       %25f\n", points.back().close);
    }
#else
    (void) num;
#endif
}

//------------------------------------------------------------------------------
bool flw::chart::Line::set(const flw::PriceVector& points, std::string label, flw::chart::TYPE type, Fl_Align align, Fl_Color color, int width, double clamp_min, double clamp_max) {
    if (width == 0 || width > MAX_LINE_WIDTH || (int) type < 0 || (int) type > (int) flw::chart::TYPE::LAST) {
        assert(false);
        return false;
    }

    if (width < 0 && flw::chart::has_resizable_width(type) == false) {
        assert(false);
        return false;
    }

    if (align != FL_ALIGN_LEFT && align != FL_ALIGN_RIGHT) {
        assert(false);
        return false;
    }

    clear();

    this->points    = points;
    this->label     = label;
    this->type      = type;
    this->align     = align;
    this->color     = color;
    this->width     = width;
    this->clamp_min = clamp_min;
    this->clamp_max = clamp_max;

    for (const auto& price : this->points) {
        if (price.high > max) {
            max = price.high;
        }

        if (price.low < min) {
            min = price.low;
        }
    }

    return true;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
flw::chart::Scale::Scale() {
    clear();
}

//------------------------------------------------------------------------------
void flw::chart::Scale::calc(int height) {
    auto range = diff();

    tick  = 0.0;
    pixel = 0.0;

    if (min < max && range > 0.000000001) {
        double test_inc = 0.0;
        double test_min = 0.0;
        double test_max = 0.0;
        int    ticker   = 0;

        test_inc = pow((double) 10, (double) ceil(log10(range / 10.0)));
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

        min   = test_min;
        max   = test_max;
        tick  = test_inc;

        if (tick > 0.000000001) {
            pixel = height / diff();
        }
    }
}

//------------------------------------------------------------------------------
void flw::chart::Scale::clear() {
    min   = flw::chart::MAX_VAL;
    max   = flw::chart::MIN_VAL;
    tick  = 0.0;
    pixel = 0.0;
}

//------------------------------------------------------------------------------
void flw::chart::Scale::debug(const char* name) const {
#ifdef DEBUG
    fprintf(stderr, "\t\t---------------------------------------------\n");
    fprintf(stderr, "\t\tflw::chart::Scale: %s\n", name);
    fprintf(stderr, "\t\t\tmin:   %30.6f\n", min);
    fprintf(stderr, "\t\t\tmax:   %30.6f\n", max);
    fprintf(stderr, "\t\t\tDiff:  %30.6f\n", diff());
    fprintf(stderr, "\t\t\ttick:  %30.6f\n", tick);
    fprintf(stderr, "\t\t\tpixel: %30.6f\n", pixel);
#else
    (void) name;
#endif
}

//------------------------------------------------------------------------------
void flw::chart::Scale::fix_height() {
    if (fabs(max - min) < 0.000001) {
        if (min >= 0.0) {
            min *= 0.9;
        }
        else {
            min *= 1.1;
        }

        if (max >= 0.0) {
            max *= 1.1;
        }
        else {
            max *= 0.9;
        }
    }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
flw::Chart::Chart(int X, int Y, int W, int H, const char* l) : Fl_Group(X, Y, W, H, l) {
    end();
    clip_children(1);
    color(FL_BACKGROUND2_COLOR);
    labelcolor(FL_FOREGROUND_COLOR);
    box(FL_BORDER_BOX);
    tooltip(
        "Press right button for menu.\n"
        "Press ctrl + scroll wheel to change tick size.\n"
        "Press left button to show Y value.\n"
        "Press ctrl + left button to show actual price data.\n"
        "Press 1 - 9 to select which line to use.\n"
        "Press shift + 1 - 9 to toggle line visibility.\n"
        "Click in target area first."
    );

    _menu   = new Fl_Menu_Button(0, 0, 0, 0);
    _scroll = new Fl_Scrollbar(0, 0, 0, 0);

    add(_menu);
    add(_scroll);

    _scroll->type(FL_HORIZONTAL);
    _scroll->callback(flw::Chart::_CallbackScrollbar, this);
    _menu->add(flw::chart::SHOW_LABELS, 0, flw::Chart::_CallbackToggle, this, FL_MENU_TOGGLE);
    _menu->add(flw::chart::SHOW_HLINES, 0, flw::Chart::_CallbackToggle, this, FL_MENU_TOGGLE);
    _menu->add(flw::chart::SHOW_VLINES, 0, flw::Chart::_CallbackToggle, this, FL_MENU_TOGGLE | FL_MENU_DIVIDER);
    _menu->add(flw::chart::RESET_SELECT, 0, flw::Chart::_CallbackReset, this);
    _menu->add(flw::chart::SAVE_PNG, 0, flw::Chart::_CallbackSavePng, this);
#ifdef DEBUG
    _menu->add(flw::chart::PRINT_DEBUG, 0, flw::Chart::_CallbackDebug, this);
#endif
    _menu->type(Fl_Menu_Button::POPUP3);

    _areas.push_back(flw::chart::Area());
    _areas.push_back(flw::chart::Area());
    _areas.push_back(flw::chart::Area());

    clear();
    update_pref();
}

//------------------------------------------------------------------------------
bool flw::Chart::add_line(size_t area, const flw::PriceVector& points, std::string line_label, flw::chart::TYPE chart_type, Fl_Align line_align, Fl_Color line_color, int line_width, double clamp_min, double clamp_max) {
    _area = nullptr;
    redraw();

    if (area >= flw::chart::MAX_AREA || _areas[area].count >= flw::chart::MAX_LINE) {
        assert(false);
        return false;
    }

    if (_areas[area].lines[_areas[area].count].set(points, line_label, chart_type, line_align, line_color, line_width, clamp_min, clamp_max) == false) {
        _areas[area].lines[_areas[area].count] = flw::chart::Line();
        return false;
    }

    _areas[area].count++;
    return true;
}

//------------------------------------------------------------------------------
bool flw::Chart::area_size(int area1, int area2, int area3) {
    _area = nullptr;

    if (area1 < 0 || area1 > 100 || area2 < 0 || area2 > 100 || area3 < 0 || area3 > 100 || area1 + area2 + area3 != 100) {
        assert(false);
        return false;
    }

    _areas[0].percent = area1;
    _areas[1].percent = area2;
    _areas[2].percent = area3;
    return true;
}

//------------------------------------------------------------------------------
void flw::Chart::_calc_area_height() {
    int last   = 0;
    int addh   = 0;
    int height = 0;

    _top_space    = flw::PREF_FIXED_FONTSIZE;
    _bottom_space = flw::PREF_FIXED_FONTSIZE * 3 + Fl::scrollbar_size();
    height        = h() - (_bottom_space + _top_space);

    for (size_t f = 1; f < flw::chart::MAX_AREA; f++) {
        const auto& area = _areas[f];

        if (area.percent >= 10) {
            height -= flw::PREF_FIXED_FONTSIZE;
        }
    }

    _areas[0].y = _top_space;
    _areas[0].h = (int) ((_areas[0].percent / 100.0) * height);

    for (size_t f = 1; f < flw::chart::MAX_AREA; f++) {
        const auto& prev = _areas[f - 1];
        auto&       area = _areas[f];

        if (area.percent > 10) {
            area.y  = prev.y + prev.h + flw::PREF_FIXED_FONTSIZE;
            area.h  = (int) ((_areas[1].percent / 100.0) * height);
            addh   += (int) (prev.h);
            last    = f;
        }
    }

    if (last > 0) {
        _areas[last].h = (int) (height - addh);
    }
}

//------------------------------------------------------------------------------
void flw::Chart::_calc_area_width() {
    const double width = w() - (_margin_left * flw::PREF_FIXED_FONTSIZE + _margin_right * flw::PREF_FIXED_FONTSIZE);

    _ticks     = (int) (width / _tick_width);
    _top_space = flw::PREF_FIXED_FONTSIZE;

    if ((int) _dates.size() > _ticks) {
        const double slider_size = _ticks / (_dates.size() + 1.0);

        _scroll->activate();
        _scroll->slider_size((slider_size > 0.05) ? slider_size : 0.05);
        _scroll->range(0, _dates.size() - _ticks);

        if (_scroll->value() > _scroll->maximum()) {
            ((Fl_Slider*) _scroll)->value(_scroll->maximum());
            _date_start = _dates.size() - _ticks - 1;
        }
    }
    else {
        ((Fl_Valuator*) _scroll)->value(0);
        _scroll->range(0, 0);
        _scroll->slider_size(1.0);
        _scroll->deactivate();
        _date_start = 0;
    }

    for (auto& area : _areas) {
        area.x = _margin_left * flw::PREF_FIXED_FONTSIZE;
        area.w = width;
    }
}

//------------------------------------------------------------------------------
void flw::Chart::_calc_dates() {
    std::string min = "99991231";
    std::string max = "01010101";

    for (const auto& area : _areas) {
        for (const auto& line : area.lines) {
            if (line.points.size() > 0) {
                auto& first = line.points.front();
                auto& last  = line.points.back();

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

    if (min != "99991231") {
        _dates = Price::DateSerie(min.c_str(), max.c_str(), _date_range, _block_dates);
        redraw();
    }
}

//------------------------------------------------------------------------------
void flw::Chart::_calc_ymin_ymax() {
    for (auto& area : _areas) {
        area.left.clear();
        area.right.clear();

        for (const auto& line : area.lines) {
            if (line.points.size() > 0) {
                const int stop    = _date_start + _ticks;
                int       current = _date_start;
                double    max     = flw::chart::MIN_VAL;
                double    min     = flw::chart::MAX_VAL;

                while (current <= stop && current < (int) _dates.size()) {
                    const flw::Price& date  = _dates[current];
                    const size_t      index = flw::chart::bsearch(line.points, date);

                    if (index != (size_t) -1) {
                        const flw::Price& price = line.points[index];

                        if (flw::chart::has_high_low(line.type) == true) {
                            min = price.low;
                            max = price.high;
                        }
                        else {
                            min = price.close;
                            max = price.close;
                        }

                        if ((int64_t) line.clamp_min > flw::chart::MIN_VAL) {
                            min = line.clamp_min;
                        }

                        if ((int64_t) line.clamp_max < flw::chart::MAX_VAL) {
                            max = line.clamp_max;
                        }

                        if (line.align == FL_ALIGN_LEFT) {
                            if (min < area.left.min) {
                                area.left.min = min;
                            }

                            if (max > area.left.max) {
                                area.left.max = max;
                            }
                        }
                        else {
                            if (min < area.right.min) {
                                area.right.min = min;
                            }

                            if (max > area.right.max) {
                                area.right.max = max;
                            }
                        }
                    }

                    current++;
                }
            }
        }

        area.left.fix_height();
        area.right.fix_height();
    }
}

//------------------------------------------------------------------------------
void flw::Chart::_calc_yscale() {
    for (auto& area : _areas) {
        area.left.calc(area.h);
        area.right.calc(area.h);
    }
}

//------------------------------------------------------------------------------
void flw::Chart::_CallbackDebug(Fl_Widget*, void* chart_object) {
    auto self = (flw::Chart*) chart_object;

    self->debug();
}

//------------------------------------------------------------------------------
void flw::Chart::_CallbackToggle(Fl_Widget*, void* chart_object) {
    auto self = (flw::Chart*) chart_object;

    self->_view.labels     = flw::util::menu_item_value(self->_menu, flw::chart::SHOW_LABELS);
    self->_view.vertical   = flw::util::menu_item_value(self->_menu, flw::chart::SHOW_VLINES);
    self->_view.horizontal = flw::util::menu_item_value(self->_menu, flw::chart::SHOW_HLINES);
    self->redraw();
}

//------------------------------------------------------------------------------
void flw::Chart::_CallbackReset(Fl_Widget*, void* chart_object) {
    auto self = (flw::Chart*) chart_object;

    for (auto& area : self->_areas) {
        area.selected = 0;

        for (auto& line : area.lines) {
            line.visible = true;
        }
    }

    self->redraw();
}

//------------------------------------------------------------------------------
void flw::Chart::_CallbackSavePng(Fl_Widget*, void* chart_object) {
    auto self = (flw::Chart*) chart_object;

    flw::util::png_save("", self->window(), self->x() + 1,  self->y() + 1,  self->w() - 2,  self->h() - self->_scroll->h() - 1);
}

//------------------------------------------------------------------------------
void flw::Chart::_CallbackScrollbar(Fl_Widget*, void* chart_object) {
    auto self = (flw::Chart*) chart_object;

    self->_date_start = self->_scroll->value();
    self->init(false);
}

//------------------------------------------------------------------------------
void flw::Chart::clear() {
    ((Fl_Valuator*) _scroll)->value(0);

    _block_dates.clear();
    _dates.clear();

    _areas[0].clear(true);
    _areas[1].clear(true);
    _areas[2].clear(true);

    _area         = nullptr;
    _bottom_space = 0;
    _cw           = 0;
    _date_start   = 0;
    _old_height   = -1;
    _old_width    = -1;
    _tooltip      = "";
    _top_space    = 0;
    _ver_pos[0]   = -1;

    area_size();
    margin();
    date_range();
    tick_width();
}

//------------------------------------------------------------------------------
// Create tooltip string either by using mouse cursor pos and convert it to real value or use price data
// If ctrl is true then use actual price data
//
void flw::Chart::_create_tooltip(bool ctrl) {
    const int X   = Fl::event_x();
    const int Y   = Fl::event_y();
    auto      old = _tooltip;

    _tooltip = "";

    if (_area != nullptr) { // Set in handle()
        const flw::Date::FORMAT date_format = (_date_format == flw::Date::FORMAT::ISO) ? flw::Date::FORMAT::NAME_LONG : flw::Date::FORMAT::ISO_TIME_LONG;
        const int               stop        = _date_start + _ticks;
        int                     start       = _date_start;
        int                     X1          = x() + _margin_left * flw::PREF_FIXED_FONTSIZE;
        int                     left_dec    = 0;
        int                     right_dec   = 0;

        if (_area->left.tick < 10.0 ) {
            left_dec = flw::util::count_decimals(_area->left.tick) + 1;
        }

        if (_area->right.tick < 10.0 ) {
            right_dec = flw::util::count_decimals(_area->right.tick) + 1;
        }

        while (start <= stop && start < (int) _dates.size()) {
            if (X >= X1 && X <= X1 + _tick_width - 1) { // Is mouse x pos inside current tick?
                const std::string fancy_date = flw::Date::FromString(_dates[start].date.c_str()).format(date_format);

                _tooltip = fancy_date;

                if (ctrl == false || _area->lines[_area->selected].points.size() == 0) { // Convert mouse pos to scale value
                    const double ydiff = (double) ((y() + _area->y2()) - Y);
                    std::string  left;
                    std::string  right;

                    if (_area->left.max > _area->left.min) {
                        left = flw::util::format_double(_area->left.min + (ydiff / _area->left.pixel), left_dec, '\'');
                    }

                    if (_area->right.max > _area->right.min) {
                        right = flw::util::format_double(_area->right.min + (ydiff / _area->right.pixel), right_dec, '\'');
                    }

                    const size_t len = (left.length() > right.length()) ? left.length() : right.length();

                    if (left != "" && right != "") {
                        _tooltip = flw::util::format("%s\nleft:  %*s\nright: %*s", fancy_date.c_str(), (int) len, left.c_str(), (int) len, right.c_str());
                    }
                    else if (left != "") {
                        _tooltip = flw::util::format("%s\nleft: %*s", fancy_date.c_str(), (int) len, left.c_str());
                    }
                    else if (right != "") {
                        _tooltip = flw::util::format("%s\nright: %*s", fancy_date.c_str(), (int) len, right.c_str());
                    }
                }
                else { // Use actual price data
                    const flw::chart::Line& line  = _area->lines[_area->selected];
                    const size_t            index = flw::chart::bsearch(line.points, _dates[start].date);

                    if (index != (size_t) -1) {
                        const int         dec   = (line.align == FL_ALIGN_RIGHT) ? right_dec : left_dec;
                        const flw::Price& price = line.points[index];
                        const std::string high  = flw::util::format_double(price.high, dec, '\'');
                        const std::string low   = flw::util::format_double(price.low, dec, '\'');
                        const std::string close = flw::util::format_double(price.close, dec, '\'');
                        const size_t      len   = (low.length() > high.length()) ? low.length() : high.length();

                        _tooltip = flw::util::format("%s\nhigh:  %*s\nclose: %*s\nlow:   %*s", fancy_date.c_str(), (int) len, high.c_str(), (int) len, close.c_str(), (int) len, low.c_str());
                    }
                }

                break;
            }

            X1    += _tick_width;
            start += 1;
        }
    }

    if (_tooltip != "" || old != "") {
        redraw();
    }
}

//------------------------------------------------------------------------------
bool flw::Chart::date_range(flw::Date::RANGE range) {
    if ((int) range < 0 || (int) range > (int) flw::Date::RANGE::LAST) {
        assert(false);
        return false;
    }

    _date_range  = range;
    _date_format = (_date_range == flw::Date::RANGE::HOUR || _date_range == flw::Date::RANGE::MIN || _date_range == flw::Date::RANGE::SEC) ? flw::Date::FORMAT::ISO_TIME : flw::Date::FORMAT::ISO;

    return true;
}

//------------------------------------------------------------------------------
void flw::Chart::debug() const {
#ifdef DEBUG
    Price first, last, start, end;

    if (_dates.size()) {
        first = _dates.front();
    }

    if (_dates.size() > 1) {
        last = _dates.back();
    }

    if ((int) _dates.size() > _date_start) {
        start = _dates[_date_start];
    }

    if (_ticks >= 0 && (int) _dates.size() > (_date_start + _ticks)) {
        end = _dates[_date_start + _ticks];
    }

    fprintf(stderr, "\n");
    fprintf(stderr, "--------------------------------------------\n");
    fprintf(stderr, "flw::Chart:\n");
    fprintf(stderr, "\tblock_dates:     %19d\n", (int) _block_dates.size());
    fprintf(stderr, "\tbottom_space:    %19d\n", _bottom_space);
    fprintf(stderr, "\tcw:              %19d\n", _cw);
    fprintf(stderr, "\tdate_end:        %19d\n", _date_start + _ticks);
    fprintf(stderr, "\tdate_format:     %19d\n", (int) _date_format);
    fprintf(stderr, "\tdate_start:      %19d\n", _date_start);
    fprintf(stderr, "\tdates:           %19d\n", (int) _dates.size());
    fprintf(stderr, "\th:               %19d\n", h());
    fprintf(stderr, "\thorizontal:      %19d\n", _view.horizontal);
    fprintf(stderr, "\tlabels:          %19d\n", _view.labels);
    fprintf(stderr, "\tmargin_left:     %19d\n", _margin_left * flw::PREF_FIXED_FONTSIZE);
    fprintf(stderr, "\tmargin_right:    %19d\n", _margin_right * flw::PREF_FIXED_FONTSIZE);
    fprintf(stderr, "\trange:           %19d\n", (int) _date_range);
    fprintf(stderr, "\ttick_width:      %19d\n", _tick_width);
    fprintf(stderr, "\tticks:           %19d\n", _ticks);
    fprintf(stderr, "\ttop_space:       %19d\n", _top_space);
    fprintf(stderr, "\tvertical:        %19d\n", _view.vertical);
    fprintf(stderr, "\tw:               %19d\n", w());
    fprintf(stderr, "\tx:               %19d\n", x());
    fprintf(stderr, "\ty:               %19d\n", y());
    fprintf(stderr, "\tfirst date:      %19s\n", first.date.c_str());
    fprintf(stderr, "\tlast date:       %19s\n", last.date.c_str());
    fprintf(stderr, "\tfirst visible:   %19s\n", start.date.c_str());
    fprintf(stderr, "\tlast visible:    %19s\n", end.date.c_str());

    _areas[0].debug(0);
    _areas[1].debug(1);
    _areas[2].debug(2);

    fflush(stderr);
#endif
}

//------------------------------------------------------------------------------
void flw::Chart::draw() {
#ifdef DEBUG
    // auto t = flw::util::time_milli();
#endif

    fl_font(flw::PREF_FIXED_FONT, flw::PREF_FIXED_FONTSIZE);
    Fl_Group::draw();
    _cw = fl_width("X");

    if (w() < 100 || h() < 100) {
        return;
    }

    fl_push_clip(x(), y(), w(), h() - _scroll->h());
    _draw_xlabels();

    for (const auto& area : _areas) {
        if (area.percent >= 10) {
            _draw_area(area);
        }
    }

    _draw_tooltip();
    fl_pop_clip();
    fl_line_style(0);

#ifdef DEBUG
    // fprintf(stderr, "draw: %3d mS\n", flw::util::time_milli() - t);
    // fflush(stdout);
#endif
}

//------------------------------------------------------------------------------
void flw::Chart::_draw_area(const flw::chart::Area& area) {
    _draw_ver_lines(area);
    _draw_ylabels(_margin_left * flw::PREF_FIXED_FONTSIZE, area.y + area.h, area.y, area.left, true);
    _draw_ylabels(w() - _margin_right * flw::PREF_FIXED_FONTSIZE, area.y + area.h, area.y, area.right, false);
    fl_push_matrix();
    fl_push_clip(x() + area.x, y() + area.y - 1, area.w + 1, area.h + 2);

    for (const auto& line : area.lines) {
        _draw_line(line, (line.align == FL_ALIGN_LEFT) ? area.left : area.right, area.x, area.y, area.w, area.h);
    }

    _draw_line_labels(area);
    fl_rect(x() + area.x, y() + area.y - 1, area.w + 1, area.h + 2, labelcolor());
    fl_pop_clip();
    fl_pop_matrix();
}

//------------------------------------------------------------------------------
void flw::Chart::_draw_line(const flw::chart::Line& line, const flw::chart::Scale& scale, int X, const int Y, const int W, const int H) {
    if (line.points.size() > 0 && line.visible == true) {
        const int y2      = y() + Y + H;
        int       lastX   = -1;
        int       lastY   = -1;
        int       current = _date_start;
        const int stop    = _date_start + _ticks + 1;
        int       width   = line.width;

        if (line.width < 0) {
            width  = _tick_width - 1;
            width -= (int) (_tick_width / 10);
        }

        const int lw2 = width / 2;
        const int lw4 = lw2 / 2;

        fl_push_clip(x() + X, y() + Y, W + 1, H + 1);
        fl_color(line.color);

        if (line.type == flw::chart::TYPE::BAR) {
            fl_line_style(FL_SOLID, lw2);
        }
        else {
            fl_line_style(FL_SOLID, width);
        }

        while (current <= stop && current < (int) _dates.size()) {
            const flw::Price& date  = _dates[current];
            const size_t      index = flw::chart::bsearch(line.points, date);

            if (index != (size_t) -1) {
                const Price& pr = line.points[index];
                const int    yh = (int) ((pr.high - scale.min) * scale.pixel);
                const int    yl = (int) ((pr.low - scale.min) * scale.pixel);
                const int    yc = (int) ((pr.close - scale.min) * scale.pixel);

                if (line.type == flw::chart::TYPE::LINE) {
                    if (lastX > -1 && lastY > -1) {
                        fl_line(lastX + lw2, y2 - lastY, x() + X + lw2, y2 - yc);
                    }
                }
                else if (line.type == flw::chart::TYPE::BAR) {
                    fl_line(x() + X + lw4, y2 - yl, x() + X + lw4, y2 - yh);
                    fl_line(x() + X, y2 - yc, x() + X + _tick_width - 1, y2 - yc);
                }
                else if (line.type == flw::chart::TYPE::HORIZONTAL) {
                    fl_line(x() + X, y2 - yc, x() + X + _tick_width, y2 - yc);
                }
                else if (line.type == flw::chart::TYPE::VERTICAL) {
                    auto h = yh - yl;
                    fl_rectf(x() + X, y2 - yh, width, (h < 1) ? 1 : h);
                }
                else if (line.type == flw::chart::TYPE::CLAMP_VERTICAL) {
                    fl_rectf(x() + X, y2 - yh, width, yh);
                }
                else if (line.type == flw::chart::TYPE::EXPAND_VERTICAL) {
                    fl_line(x() + X, y2, x() + X, y() + Y);
                }
                else if (line.type == flw::chart::TYPE::EXPAND_HORIZONTAL) {
                    fl_line(x() + _margin_left * flw::PREF_FIXED_FONTSIZE, y2 - yc, x() + Fl_Widget::w() - _margin_right * flw::PREF_FIXED_FONTSIZE, y2 - yc);
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
void flw::Chart::_draw_line_labels(const flw::chart::Area& area) {
    if (_view.labels == true) {
        int       left_h  = 0;
        int       left_w  = 0;
        const int left_x  = x() + area.x + 6;
        int       left_y  = y() + area.y + 6;
        int       right_h = 0;
        int       right_w = 0;
        const int right_x = x() + area.x + area.w;
        int       right_y = left_y;

        for (const auto& line : area.lines) {
            if (line.label == "") {
                continue;
            }

            if (line.align == FL_ALIGN_RIGHT) {
                right_h++;

                if ((int) line.label.length() * _cw > right_w) {
                    right_w = line.label.length() * _cw;
                }
            }
            else {
                left_h++;

                if ((int) line.label.length() * _cw > left_w) {
                    left_w = line.label.length() * _cw;
                }
            }
        }

        left_h  *= flw::PREF_FIXED_FONTSIZE;
        right_h *= flw::PREF_FIXED_FONTSIZE;

        if (left_w > 0) {
            left_w += _cw * 2 + 4;
            fl_color(FL_BACKGROUND2_COLOR);
            fl_rectf(left_x, left_y, left_w + 8, left_h + 8);

            fl_color(FL_FOREGROUND_COLOR);
            fl_rect(left_x, left_y, left_w + 8, left_h + 8);
        }

        if (right_w > 0) {
            right_w += _cw * 2 + 4;
            fl_color(FL_BACKGROUND2_COLOR);
            fl_rectf(right_x - right_w - 14, left_y, right_w + 6, right_h + 8);

            fl_color(FL_FOREGROUND_COLOR);
            fl_rect(right_x - right_w - 14, left_y, right_w + 6, right_h + 8);
        }

        fl_font(flw::PREF_FIXED_FONT, flw::PREF_FIXED_FONTSIZE);

        if (left_w > 0 || right_w > 0) {
            size_t count   = 0;

            for (const auto& line : area.lines) {
                auto label = line.label;

                if (label != "") {
                    if (area.selected == count) {
                        label = "@-> " + label;
                    }

                    fl_color((line.visible == false) ? FL_GRAY : line.color);

                    if (line.align == FL_ALIGN_RIGHT) {
                        fl_draw(label.c_str(), right_x - right_w - 10, right_y + 4, right_w + 8, flw::PREF_FIXED_FONTSIZE, FL_ALIGN_LEFT);
                        right_y += flw::PREF_FIXED_FONTSIZE;
                    }
                    else {
                        fl_draw(label.c_str(), left_x + 4, left_y + 4, left_w + 8, flw::PREF_FIXED_FONTSIZE, FL_ALIGN_LEFT);
                        left_y += flw::PREF_FIXED_FONTSIZE;
                    }
                }

                count++;
            }
        }
    }
}

//------------------------------------------------------------------------------
void flw::Chart::_draw_tooltip() {
    if (_tooltip != "" && _area != nullptr) {
        auto       X = Fl::event_x();
        auto       Y = Fl::event_y();
        const auto W = 14;
        const auto H = 5;

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

        fl_line(Fl::event_x(), y() + _area->y, Fl::event_x(), y() + _area->y + _area->h);
        fl_line(x() + _area->x, Fl::event_y(), x() + _area->x + _area->w, Fl::event_y());

        fl_font(flw::PREF_FIXED_FONT, flw::PREF_FIXED_FONTSIZE);
        fl_draw(_tooltip.c_str(), X + flw::PREF_FIXED_FONTSIZE, Y, flw::PREF_FIXED_FONTSIZE * W, flw::PREF_FIXED_FONTSIZE * H, FL_ALIGN_LEFT | FL_ALIGN_CENTER | FL_ALIGN_INSIDE);
    }
}

//------------------------------------------------------------------------------
void flw::Chart::_draw_ver_lines(const flw::chart::Area& area) {
    if (_view.vertical == true) {
        fl_color(fl_color_average(FL_FOREGROUND_COLOR, FL_BACKGROUND2_COLOR, 0.2));

        for (auto i = 0; i < flw::chart::MAX_VLINES; i++) {
            if (_ver_pos[i] < 0) {
                break;
            }
            else {
                fl_line(_ver_pos[i], y() + (int) area.y, _ver_pos[i], y() + (int) (area.y + area.h - 1));
            }
        }
    }
}

//------------------------------------------------------------------------------
void flw::Chart::_draw_xlabels() {
    const int  stop  = _date_start + _ticks;
    const int  cw2   = _cw * 2;
    const int  cw4   = _cw * 4;
    const int  fs05  = flw::PREF_FIXED_FONTSIZE * 0.5;
    const int  Y     = y() + (h() - _bottom_space);
    int        start = _date_start;
    int        index = 0;
    int        last  = -1;
    int        X1    = x() + _margin_left * flw::PREF_FIXED_FONTSIZE;
    int        X2    = 0;
    char       buffer1[100];
    char       buffer2[100];

    _ver_pos[0] = -1;

    while (start <= stop && start < (int) _dates.size()) {
        const flw::Price& price = _dates[start];
        const flw::Date   date  = flw::Date::FromString(price.date.c_str());
        auto              addv  = false;

        fl_color(labelcolor());
        fl_line(X1, Y, X1, Y + fs05);

        *buffer1 = 0;
        *buffer2 = 0;

        if (_date_range == flw::Date::RANGE::HOUR) {
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
        else if (_date_range == flw::Date::RANGE::MIN) {
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
        else if (_date_range == flw::Date::RANGE::SEC) {
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
        else if (_date_range == flw::Date::RANGE::DAY || _date_range == flw::Date::RANGE::WEEKDAY) {
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
        else if (_date_range == flw::Date::RANGE::FRIDAY || _date_range == flw::Date::RANGE::SUNDAY) {
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
        else if (_date_range == flw::Date::RANGE::MONTH) {
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
            auto len  = (double) strlen(buffer1);
            auto half = (double) ((len / 2.0) * _cw);

            fl_font(flw::PREF_FIXED_FONT, flw::PREF_FIXED_FONTSIZE);
            fl_draw(buffer1, X1 - half, Y + flw::PREF_FIXED_FONTSIZE + 6, half + half, flw::PREF_FIXED_FONTSIZE, FL_ALIGN_CENTER | FL_ALIGN_INSIDE);
            fl_line(X1, Y, X1, Y + flw::PREF_FIXED_FONTSIZE);
            X2 = X1 + _cw + half + half;
        }

        if (*buffer2 != 0 && cw2 <= _tick_width) { // Draw second x label
            fl_font(flw::PREF_FIXED_FONT, flw::PREF_FIXED_FONTSIZE - 2);
            fl_draw(buffer2, X1 - cw2, Y + fs05, cw4, flw::PREF_FIXED_FONTSIZE, FL_ALIGN_CENTER | FL_ALIGN_INSIDE);
        }

        if (addv == true && index < flw::chart::MAX_VLINES) { // Save x pos for vertical lines
            _ver_pos[index++] = X1;
        }

        X1    += _tick_width;
        start += 1;
    }

    _ver_pos[index] = -1;
}

//------------------------------------------------------------------------------
void flw::Chart::_draw_ylabels(const int X, double Y1, const double Y2, const flw::chart::Scale& scale, const bool left) {
    const double yinc  = (scale.pixel * scale.tick);
    const int    fs05  = flw::PREF_FIXED_FONTSIZE * 0.5;
    const int    fr    = flw::util::count_decimals(scale.tick);
    int          width = w() - (_margin_left * flw::PREF_FIXED_FONTSIZE + _margin_right * flw::PREF_FIXED_FONTSIZE);
    double       ylast = flw::chart::MAX_VAL;
    double       yval  = scale.min;

    fl_font(flw::PREF_FIXED_FONT, flw::PREF_FIXED_FONTSIZE);

    if (scale.min >= scale.max || scale.pixel * scale.tick < 1.0) {
        return;
    }

    while ((int) (Y1 + 0.5) >= (int) Y2) {
        if (ylast > Y1) {
            auto y1     = y() + (int) Y1;
            auto x1     = x() + X;
            auto string = flw::util::format_double(yval, fr, '\'');

            if (left == true) {
                fl_color(labelcolor());
                fl_line(x1 - fs05, y1, x1, y1);
                fl_draw(string.c_str(), x(), y1 - fs05, _margin_left * flw::PREF_FIXED_FONTSIZE - flw::PREF_FIXED_FONTSIZE, flw::PREF_FIXED_FONTSIZE, FL_ALIGN_RIGHT | FL_ALIGN_INSIDE);

                if (_view.horizontal == true && (int) Y1 >= (int) (Y2 + fs05)) {
                    fl_color(fl_color_average(FL_FOREGROUND_COLOR, FL_BACKGROUND2_COLOR, 0.2));
                    fl_line(x1 + 1, y1, x1 + width - 1, y1);
                }
            }
            else {
                fl_color(labelcolor());
                fl_line(x1, y1, x1 + fs05, y1);
                fl_draw(string.c_str(), x1 + flw::PREF_FIXED_FONTSIZE, y1 - fs05, _margin_right * flw::PREF_FIXED_FONTSIZE - (flw::PREF_FIXED_FONTSIZE * 2), flw::PREF_FIXED_FONTSIZE, FL_ALIGN_RIGHT | FL_ALIGN_INSIDE);

                if (_view.horizontal == true && (int) Y1 >= (int) (Y2 + fs05)) {
                    fl_color(fl_color_average(FL_FOREGROUND_COLOR, FL_BACKGROUND2_COLOR, 0.2));
                    fl_line(x1 - width + 1, y1, x1 - 1, y1);
                }
            }

            ylast = Y1 - (flw::PREF_FIXED_FONTSIZE + fs05);
        }

        Y1   -= yinc;
        yval += scale.tick;
    }
}

//------------------------------------------------------------------------------
int flw::Chart::handle(int event) {
    if (event == FL_PUSH) {
        if (Fl::event_button1() != 0) {
            _area = _inside_area(Fl::event_x(), Fl::event_y());
            _create_tooltip(Fl::event_ctrl());

            if (_tooltip != "") {
                return 1;
            }
        }
        else if (Fl::event_button3() != 0) {
            flw::util::menu_item_set(_menu, flw::chart::SHOW_LABELS, _view.labels);
            flw::util::menu_item_set(_menu, flw::chart::SHOW_HLINES, _view.horizontal);
            flw::util::menu_item_set(_menu, flw::chart::SHOW_VLINES, _view.vertical);
            _menu->popup();
            return 1;
        }
    }
    else if (event == FL_DRAG) { // FL_PUSH must have been called before mouse drag
        _area = _inside_area(Fl::event_x(), Fl::event_y());
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
    else if (event == FL_MOUSEWHEEL) {
        const double size = _dates.size() - _ticks;
        const double pos  = _scroll->value();
        const double dy   = (Fl::event_dy() * 10);
        const double dx   = (Fl::event_dx() * 10);
        const double dd   = fabs(dx) > fabs(dy) ? dx : dy;
        const double adj  = _ticks / dd;

        if (fabs(dd) < 1.0) {
            return Fl_Group::handle(event);
        }
        else if (Fl::event_ctrl() > 0) {
            const int width = (adj > 0.0) ? _tick_width + 1 : _tick_width - 1;

            if (width >= flw::chart::MIN_TICK && width <= flw::chart::MAX_TICK) {
                tick_width(width);
                init(false);
            }

            return 1;
        }
        else {
            if (adj > 0) {
                if ((pos + adj) > size) {
                    ((Fl_Slider*) _scroll)->value(size);
                }
                else {
                    ((Fl_Slider*) _scroll)->value(pos + adj);
                }
            }
            else {
                if ((pos + adj) < 0.0) {
                    ((Fl_Slider*) _scroll)->value(0.0);
                }
                else {
                    ((Fl_Slider*) _scroll)->value(pos + adj);
                }
            }

            flw::Chart::_CallbackScrollbar(0, this);
            Fl::event_clicks(0);
            return 1;
        }
    }
    else if (event == FL_KEYDOWN) {
        auto key = Fl::event_key();

        if (key >= '0' && key <= '9') {
            if (_area == nullptr) {
                fl_beep(FL_BEEP_ERROR);
            }
            else {
                auto selected = key - '0';

                if (selected == 0) {
                    selected = 10;
                }

                selected--;

                if ((size_t) selected >= _area->count) {
                    selected = 0;
                }

                if (Fl::event_shift() != 0) {
                    _area->lines[selected].visible = !_area->lines[selected].visible;
                }
                else {
                    _area->selected = selected;
                }

                redraw();
            }
        }
    }

    return Fl_Group::handle(event);
}

//------------------------------------------------------------------------------
void flw::Chart::init(bool calc_dates) {
#ifdef DEBUG
    // auto t = flw::util::time_milli();
#endif

    if (calc_dates == true) {
        _calc_dates();
    }

    _calc_area_height();
    _calc_area_width();
    _calc_ymin_ymax();
    _calc_yscale();
    redraw();

#ifdef DEBUG
    // fprintf(stderr, "%s: %3d mS\n", (calc_dates == true) ? "INIT" : "init", (int) (util::time_milli() - t));
    // fflush(stdout);
#endif
}

//------------------------------------------------------------------------------
flw::chart::Area* flw::Chart::_inside_area(int X, int Y) {
    for (auto& area : _areas) {
        if (area.percent >= 10 &&
            X >= x() + (int) area.x &&
            Y >= y() + (int) area.y &&
            X <= x() + (int) (area.x + area.w) &&
            Y <= y() + (int) (area.y + area.h + 1)) {
            return &area;
        }
    }

    return nullptr;
}

//------------------------------------------------------------------------------
bool flw::Chart::margin(int left, int right) {
    if (left < 1 || left > 20 || right < 1 || right > 20) {
        assert(false);
        return false;
    }

    _margin_left  = left;
    _margin_right = right;
    redraw();
    return true;
}

//------------------------------------------------------------------------------
void flw::Chart::resize(int X, int Y, int W, int H) {
    if (_old_width != W || _old_height != H) {
        Fl_Group::resize(X, Y, W, H);
        _scroll->resize(X, Y + H - Fl::scrollbar_size(), W, Fl::scrollbar_size());
        _old_width = W;
        _old_height = H;
        init(false);
    }
}

//------------------------------------------------------------------------------
bool flw::Chart::tick_width(int tick_width) {
    if (tick_width < 3 || tick_width > 100) {
        assert(false);
        return false;
    }

    _tick_width = tick_width;
    redraw();
    return true;
}

//------------------------------------------------------------------------------
void flw::Chart::update_pref() {
    _menu->textfont(flw::PREF_FONT);
    _menu->textsize(flw::PREF_FONTSIZE);
}



namespace flw {
    static int          _DATE_DAYS_MONTH[]      = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    static int          _DATE_DAYS_MONTH_LEAP[] = {0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    static const char*  _DATE_WEEKDAYS[]        = {"", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday", ""};
    static const char*  _DATE_WEEKDAYS_SHORT[]  = {"", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun", ""};
    static const char*  _DATE_MONTHS[]          = {"", "January", "February", "Mars", "April", "May", "June", "July", "August", "September", "October", "November", "December", ""};
    static const char*  _DATE_MONTHS_SHORT[]    = {"", "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec", ""};

    const int Date::SECS_PER_HOUR = 3600;
    const int Date::SECS_PER_DAY  = 3600 * 24;
    const int Date::SECS_PER_WEEK = 3600 * 24 * 7;

    //------------------------------------------------------------------------------
    static int _date_days(int year, int month) {
        if (year < 1 || year > 9999 || month < 1 || month > 12) {
            return 0;
        }
        else if ((year % 4 == 0 && year % 100 != 0) || year % 400 == 0) {
            return _DATE_DAYS_MONTH_LEAP[month];
        }

        return _DATE_DAYS_MONTH[month];
    }

    //------------------------------------------------------------------------------
    static void _date_from_time(int64_t seconds, bool utc, int& year, int& month, int& day, int& hour, int& min, int& sec) {
        year = month = day = 1;
        hour = min = sec = 0;

        if (seconds < 0) {
            return;
        }

        auto rawtime  = (time_t) seconds;
        auto timeinfo = utc ? gmtime(&rawtime) : localtime(&rawtime);

        if (timeinfo == nullptr) {
            return;
        }

        year  = timeinfo->tm_year + 1900;
        month = timeinfo->tm_mon + 1;
        day   = timeinfo->tm_mday;
        hour  = timeinfo->tm_hour;
        min   = timeinfo->tm_min;
        sec   = timeinfo->tm_sec;
    }

    //------------------------------------------------------------------------------
    static Date _date_parse(const char* string, bool us) {
        auto space = strstr(string, " ");
        int  tot   = strlen(string);
        int  tlen  = space ? (int) (space - string) : 0;
        int  len   = space ? tlen : tot;
        int  year  = 1;
        int  month = 1;
        int  day   = 1;
        int  hour  = 0;
        int  min   = 0;
        int  sec   = 0;
        bool iso   = false;
        int  Y     = 0;
        int  val[15];

        for (int f = 0; f < 15; f++) {
            val[f] = 0;
        }

        if (len == 10 && string[4] == '-' && string[7] == '-') { // "YYYY-MM-DD"
            iso    = true;
            val[4] = string[5] - '0';
            val[5] = string[6] - '0';
            val[6] = string[8] - '0';
            val[7] = string[9] - '0';
        }
        else if (len == 8 && string[1] == '/' && string[3] == '/') { // "D/M/YYYY"
            Y      = 4;
            val[4] = string[2] - '0';
            val[5] = -1;
            val[6] = string[0] - '0';
            val[7] = -1;
        }
        else if (len == 9 && string[1] == '/' && string[4] == '/') { // "D/MM/YYYY"
            Y      = 5;
            val[4] = string[2] - '0';
            val[5] = string[3] - '0';
            val[6] = string[0] - '0';
            val[7] = -1;
        }
        else if (len == 9 && string[2] == '/' && string[4]) { // "DD/M/YYYY"
            Y      = 5;
            val[4] = string[3] - '0';
            val[5] = -1;
            val[6] = string[0] - '0';
            val[7] = string[1] - '0';
        }
        else if (len == 10 && string[2] == '/' && string[5] == '/') { // "DD/MM/YYYY"
            Y      = 6;
            val[4] = string[3] - '0';
            val[5] = string[4] - '0';
            val[6] = string[0] - '0';
            val[7] = string[1] - '0';
        }
        else if (len == 8) { // "YYYYMMDD"
            iso    = true;
            val[4] = string[4] - '0';
            val[5] = string[5] - '0';
            val[6] = string[6] - '0';
            val[7] = string[7] - '0';
        }
        else {
            return Date(1, 1, 1);
        }

        val[0] = string[Y] - '0';
        val[1] = string[Y + 1] - '0';
        val[2] = string[Y + 2] - '0';
        val[3] = string[Y + 3] - '0';

        if (tlen && tot - tlen >= 9) {
            val[8]  = string[tlen + 1] - '0';
            val[9]  = string[tlen + 2] - '0';
            val[10] = string[tlen + 4] - '0';
            val[11] = string[tlen + 5] - '0';
            val[12] = string[tlen + 7] - '0';
            val[13] = string[tlen + 8] - '0';
        }
        else if (tlen && tot - tlen >= 7) {
            val[8]  = string[tlen + 1] - '0';
            val[9]  = string[tlen + 2] - '0';
            val[10] = string[tlen + 3] - '0';
            val[11] = string[tlen + 4] - '0';
            val[12] = string[tlen + 5] - '0';
            val[13] = string[tlen + 6] - '0';
        }

        // printf("(%2d, %2d, %2d, %2d)  (%2d, %2d)  (%2d, %2d)  (%2d, %2d, %2d, %2d, %2d, %2d)  <%s>\n", val[0], val[1], val[2], val[3], val[4], val[5], val[6], val[7], val[8], val[9], val[10], val[11], val[12], val[13], string);
        // fflush(stdout);

        for (int f = 0; f < 15; f++) {
            if ((f == 5 || f == 7) && val[f] == -1) {
                ;
            }
            else if (val[f] < 0 || val[f] > 9) {
                return Date(1, 1, 1);
            }
        }

        year  = val[0] * 1000 + val[1] * 100 + val[2] * 10 + val[3];
        month = val[5] == -1 ? val[4] : val[4] * 10 + val[5];
        day   = val[7] == -1 ? val[6] : val[6] * 10 + val[7];
        hour  = val[8] * 10 + val[9];
        min   = val[10] * 10 + val[11];
        sec   = val[12] * 10 + val[13];

        if (iso == false && us) { // Swap world and us numbers
            int tmp = month;
            month   = day;
            day     = tmp;
        }

        return Date(year, month, day, hour, min, sec);
    }

    //------------------------------------------------------------------------------
    static flw::Date::DAY _date_weekday(int year, int month, int day) {
        if (year > 0 && year < 10000 && month > 0 && month < 13 && day > 0 && day <= _date_days(year, month)) {
            int start = 0;
            int y1    = year - 1;
            int pre   = ((year < 1582) || ((year == 1582) && (month <= 10)));

            if (pre) {
                start = 6 + y1 + (y1 / 4);
            }
            else {
                start = 1 + y1 + (y1 / 4) - (y1 / 100) + (y1 / 400);
            }

            for (int i = 1; i < month; i++) {
                int days = _date_days(year, i);

                if (days) {
                    start += days;
                }
                else {
                    return flw::Date::DAY::INVALID;
                }
            }

            start = start % 7;
            start = start == 0 ? 7 : start;

            for (int i = 2; i <= day; i++) {
                start++;

                if (start > 7) {
                    start = 1;
                }
            }

            if (start < 1 || start > 7) {
                return flw::Date::DAY::INVALID;
            }
            else {
                return (flw::Date::DAY) start;
            }
        }

        return flw::Date::DAY::INVALID;
    }

    //------------------------------------------------------------------------------
    static bool _date_is_leapyear(int year) {
        if (year < 1 || year > 9999) {
            return false;
        }
        else if ((year % 4 == 0 && year % 100 != 0) || year % 400 == 0) {
            return true;
        }
        else {
           return false;
        }
    }
}

//------------------------------------------------------------------------------
flw::Date::Date(bool utc) {
    int year, month, day, hour, min, sec;
    _date_from_time(::time(nullptr), utc, year, month, day, hour, min, sec);
    set(year, month, day, hour, min, sec);
}

//------------------------------------------------------------------------------
flw::Date::Date(const Date& other) {
    set(other);
}

//------------------------------------------------------------------------------
flw::Date::Date(Date&& other) {
    set(other);
}

//------------------------------------------------------------------------------
flw::Date::Date(int year, int month, int day, int hour, int min, int sec) {
    _year = _month = _day = 1;
    _hour = _min = _sec = 0;
    set(year, month, day, hour, min, sec);
}

//------------------------------------------------------------------------------
flw::Date& flw::Date::operator=(const Date& date) {
    set(date);
    return *this;
}

//------------------------------------------------------------------------------
flw::Date& flw::Date::operator=(Date&& date) {
    set(date);
    return *this;
}

//------------------------------------------------------------------------------
bool flw::Date::add_days(const int days) {
    if (days) {
        int daym = _date_days(_year, _month);

        if (daym > 0) {
            const int inc   = days > 0 ? 1 : -1;
            int       year  = _year;
            int       month = _month;
            int       day   = _day;

            for (int f = 0; f < abs(days); f++) {
                day += inc;

                if (inc < 0 && day == 0) {
                    month--;

                    if (month == 0) {
                        month = 12;
                        year--;

                        if (year < 1) {
                            return false;
                        }
                    }

                    day = _date_days(year, month);

                    if (day == 0) {
                        return false;
                    }
                }
                else if (inc > 0 && day > daym) {
                    day = 1;
                    month++;

                    if (month == 13) {
                        month = 1;
                        year++;

                        if (year > 9999) {
                            return false;
                        }
                    }

                    daym = _date_days(year, month);

                    if (daym == 0) {
                        return false;
                    }
                }
            }

            _year  = year;
            _month = month;
            _day   = day;

            return true;
        }
    }

    return false;
}

//------------------------------------------------------------------------------
bool flw::Date::add_months(const int months) {
    if (months) {
        const int inc   = months > 0 ? 1 : -1;
        int       month = _month;
        int       year  = _year;

        for (int f = 0; f < abs(months); f++) {
            month += inc;

            if (month == 0) {
                month = 12;
                year--;

                if (year < 1) {
                    return false;
                }
            }
            else if (month > 12) {
                month = 1;
                year++;

                if (year > 9999) {
                    return false;
                }
            }
        }

        const int days = _date_days(year, month);

        if (days > 0) {
            _year  = year;
            _month = month;

            if (_day > days) {
                _day = days;
            }

            return true;
        }
    }

    return false;
}

//------------------------------------------------------------------------------
bool flw::Date::add_seconds(const int64_t seconds) {
    if (seconds) {
        const int inc  = seconds > 0 ? 1 : -1;
        int       hour = _hour;
        int       min  = _min;
        int       sec  = _sec;

        for (int64_t f = 0; f < llabs(seconds); f++) {
            sec += inc;

            if (inc < 0 && sec == -1) {
                min--;

                if (min == -1) {
                    min = 59;
                    hour--;

                    if (hour == -1) {
                        hour = 23;

                        if (add_days(-1) == false) {
                            return false;
                        }
                    }
                }

                sec = 59;
            }
            else if (inc > 0 && sec == 60) {
                min++;

                if (min == 60) {
                    min = 0;
                    hour++;

                    if (hour == 24) {
                        hour = 0;

                        if (add_days(1) == false) {
                            return false;
                        }
                    }
                }

                sec = 0;
            }
        }

        _hour = hour;
        _min  = min;
        _sec  = sec;

        return true;
    }

    return false;
}

//------------------------------------------------------------------------------
int flw::Date::compare(const Date& other, flw::Date::COMPARE flag) const {
    if (_year < other._year) {
        return -1;
    }
    else if (_year > other._year) {
        return 1;
    }

    if (_month < other._month) {
        return -1;
    }
    else if (_month > other._month) {
        return 1;
    }

    if (flag >= flw::Date::COMPARE::YYYYMMDD) {
        if (_day < other._day) {
            return -1;
        }
        else if (_day > other._day) {
            return 1;
        }
    }

    if (flag >= flw::Date::COMPARE::YYYYMMDDHH) {
        if (_hour < other._hour) {
            return -1;
        }
        else if (_hour > other._hour) {
            return 1;
        }
    }

    if (flag >= flw::Date::COMPARE::YYYYMMDDHHMM) {
        if (_min < other._min) {
            return -1;
        }
        else if (_min > other._min) {
            return 1;
        }
    }

    if (flag >= flw::Date::COMPARE::YYYYMMDDHHMMSS) {
        if (_sec < other._sec) {
            return -1;
        }
        else if (_sec > other._sec) {
            return 1;
        }
    }

    return 0;
}

//------------------------------------------------------------------------------
int flw::Date::Compare(const void* a, const void* b) {
    Date* A = *(Date**) a;
    Date* B = *(Date**) b;
    return A->compare(*B);
}

//------------------------------------------------------------------------------
bool flw::Date::Compare(const Date& a, const Date& b) {
    return a.compare(b) < 0;
}

//------------------------------------------------------------------------------
flw::Date& flw::Date::day(int day) {
    if (day > 0 && day <= _date_days(_year, _month)) {
        _day = day;
    }

    return *this;
}

//------------------------------------------------------------------------------
flw::Date& flw::Date::day_last() {
    _day = month_days();
    return *this;
}

//------------------------------------------------------------------------------
void flw::Date::Del(void* self) {
    if (self) {
        delete (Date*) self;
    }
}

//------------------------------------------------------------------------------
int flw::Date::diff_days(const Date& date) const {
    Date d(date);
    int  res = 0;

    if (compare(d, flw::Date::COMPARE::YYYYMMDD) < 0) {
        while (compare(d, flw::Date::COMPARE::YYYYMMDD) != 0) {
            d.add_days(-1);
            res++;
        }
    }
    else if (compare(d, flw::Date::COMPARE::YYYYMMDD) > 0) {
        while (compare(d, flw::Date::COMPARE::YYYYMMDD) != 0) {
            d.add_days(1);
            res--;
        }
    }

    return res;
}

//------------------------------------------------------------------------------
int flw::Date::diff_months(const Date& date) const {
    Date d(date);
    int  res = 0;

    if (compare(d, flw::Date::COMPARE::YYYYMM) < 0) {
        while (compare(d, flw::Date::COMPARE::YYYYMM)) {
            d.add_months(-1);
            res++;
        }
    }
    else if (compare(d, flw::Date::COMPARE::YYYYMM) > 0) {
        while (compare(d, flw::Date::COMPARE::YYYYMM)) {
            d.add_months(1);
            res--;
        }
    }

    return res;
}

//------------------------------------------------------------------------------
int flw::Date::diff_seconds(const Date& date) const {
    int64_t unix1 = time();
    int64_t unix2 = date.time();

    if (unix1 >= 0 && unix2 >= 0) {
        return unix2 - unix1;
    }

    return 0;
}

//------------------------------------------------------------------------------
std::string flw::Date::format(flw::Date::FORMAT format) const {
    char tmp[100];
    int  n = 0;

    if (format == flw::Date::FORMAT::ISO_LONG) {
        n = snprintf(tmp, 100, "%04d-%02d-%02d", _year, _month, _day);
    }
    else if (format == flw::Date::FORMAT::US) {
        n = snprintf(tmp, 100, "%d/%d/%04d", _month, _day, _year);
    }
    else if (format == flw::Date::FORMAT::WORLD) {
        n = snprintf(tmp, 100, "%d/%d/%04d", _day, _month, _year);
    }
    else if (format == flw::Date::FORMAT::NAME) {
        n = snprintf(tmp, 100, "%04d %s %d", _year, month_name_short(), _day);
    }
    else if (format == flw::Date::FORMAT::NAME_LONG) {
        n = snprintf(tmp, 100, "%04d %s %d", _year, month_name(), _day);
    }
    else if (format == flw::Date::FORMAT::YEAR_MONTH) {
        n = snprintf(tmp, 100, "%04d %s", _year, month_name_short());
    }
    else if (format == flw::Date::FORMAT::YEAR_MONTH_LONG) {
        n = snprintf(tmp, 100, "%04d %s", _year, month_name());
    }
    else if (format == flw::Date::FORMAT::ISO_TIME) {
        n = snprintf(tmp, 100, "%04d%02d%02d %02d%02d%02d", _year, _month, _day, _hour, _min, _sec);
    }
    else if (format == flw::Date::FORMAT::ISO_TIME_LONG) {
        n = snprintf(tmp, 100, "%04d-%02d-%02d %02d:%02d:%02d", _year, _month, _day, _hour, _min, _sec);
    }
    else {
        n = snprintf(tmp, 100, "%04d%02d%02d", _year, _month, _day);
    }

    if (n < 0 || n >= 100) {
        *tmp = 0;
    }

    return tmp;
}

//------------------------------------------------------------------------------
// Create date object by parsing a string buffer
// Valid formats for dates are:
// "19991224"
// "19991224"
// "1999-12-24"
// "24/12/1999" "world formar"
// "12/24/1999" "us format"
// And optional time after date with one space between them:
// "000000"
// "00:00:00"
// Invalid date returns a Date set to (1, 1, 1)
//
flw::Date flw::Date::FromString(const char* buffer, bool us) {
    if (buffer == nullptr) {
        return Date(1, 1, 1);
    }
    else {
         return _date_parse(buffer, us);
    }
}

//------------------------------------------------------------------------------
flw::Date flw::Date::FromTime(int64_t seconds, bool utc) {
    int year, month, day, hour, min, sec;
    _date_from_time(seconds, utc, year, month, day, hour, min, sec);
    return Date(year, month, day, hour, min, sec);
}

//------------------------------------------------------------------------------
flw::Date& flw::Date::hour(int hour) {
    if (hour >= 0 && hour <= 23) {
        _hour = hour;
    }

    return *this;
}

//------------------------------------------------------------------------------
bool flw::Date::is_leapyear() const {
    return _date_is_leapyear(_year);
}

//------------------------------------------------------------------------------
flw::Date& flw::Date::minute(int min) {
    if (min >= 0 && min <= 59) {
        _min = min;
    }

    return *this;
}

//------------------------------------------------------------------------------
flw::Date& flw::Date::month(int month) {
    if (month >= 1 && month <= 12) {
        _month = month;
    }

    return *this;
}

//------------------------------------------------------------------------------
int flw::Date::month_days() const {
    return _date_days(_year, _month);
}

//------------------------------------------------------------------------------
const char* flw::Date::month_name() const {
    return _DATE_MONTHS[(int) _month];
}

//------------------------------------------------------------------------------
const char* flw::Date::month_name_short() const {
    return _DATE_MONTHS_SHORT[(int) _month];
}

//------------------------------------------------------------------------------
void flw::Date::print() const {
    auto string = format(flw::Date::FORMAT::ISO_TIME_LONG);
    printf("Date| %s\n", string.c_str());
    fflush(stdout);
}

//------------------------------------------------------------------------------
flw::Date& flw::Date::second(int sec) {
    if (sec >= 0 && sec <= 59) {
        _sec = sec;
    }

    return *this;
}

//------------------------------------------------------------------------------
flw::Date& flw::Date::set(const Date& date) {
    _year  = date._year;
    _month = date._month;
    _day   = date._day;
    _hour  = date._hour;
    _min   = date._min;
    _sec   = date._sec;

    return *this;
}

//------------------------------------------------------------------------------
flw::Date& flw::Date::set(int year, int month, int day, int hour, int min, int sec) {
    if (year < 1 || year > 9999 ||
        month < 1 || month > 12 ||
        day < 1 || day > _date_days(year, month) ||
        hour < 0 || hour > 23 ||
        min < 0 || min > 59 ||
        sec < 0 || sec > 59) {
        return *this;
    }
    else {
        _year  = year;
        _month = month;
        _day   = day;
        _hour  = hour;
        _min   = min;
        _sec   = sec;

        return *this;
    }
}

//------------------------------------------------------------------------------
int64_t flw::Date::time() const {
    tm t;

    if (_year < 1970) {
        return -1;
    }

    memset(&t, '\0', sizeof(t));
    t.tm_year = _year - 1900;
    t.tm_mon  = _month - 1;
    t.tm_mday = _day;
    t.tm_hour = _hour;
    t.tm_min  = _min;
    t.tm_sec  = _sec;
    return mktime(&t);
}

//------------------------------------------------------------------------------
int flw::Date::week() const {
    flw::Date::DAY wday  = _date_weekday(_year, _month, _day);
    flw::Date::DAY wday1 = _date_weekday(_year, 1, 1);

    if (wday != flw::Date::DAY::INVALID && wday1 != flw::Date::DAY::INVALID) {
        auto week  = 0;
        auto y1    = _year - 1;
        auto leap  = _date_is_leapyear(_year);
        auto leap1 = _date_is_leapyear(y1);
        auto yday  = yearday();

        if (yday <= (8 - (int) wday1) && wday1 > flw::Date::DAY::THURSDAY) {
            if (wday1 == flw::Date::DAY::FRIDAY || (wday1 == flw::Date::DAY::SATURDAY && leap1)) {
                week = 53;
            }
            else {
                week = 52;
            }
        }
        else {
            auto days = leap ? 366 : 365;

            if ((days - yday) < (4 - (int) wday)) {
                week = 1;
            }
            else {
                days = yday + (7 - (int) wday) + ((int) wday1 - 1);
                days = days / 7;

                if (wday1 > flw::Date::DAY::THURSDAY) {
                    days--;
                }

                week = days;
            }
        }

        if (week > 0 && week < 54) {
            return week;
        }
    }

    return 0;
}

//------------------------------------------------------------------------------
flw::Date::DAY flw::Date::weekday() const {
    return _date_weekday(_year, _month, _day);
}

//------------------------------------------------------------------------------
flw::Date& flw::Date::weekday(flw::Date::DAY day) {
    if (weekday() < day) {
        while (weekday() < day) {
            add_days(1);
        }
    }
    else if (weekday() > day) {
        while (weekday() > day) {
            add_days(-1);
        }
    }

    return *this;
}

//------------------------------------------------------------------------------
const char* flw::Date::weekday_name() const {
    return _DATE_WEEKDAYS[(int) _date_weekday(_year, _month, _day)];
}

//------------------------------------------------------------------------------
const char* flw::Date::weekday_name_short() const {
    return _DATE_WEEKDAYS_SHORT[(int) _date_weekday(_year, _month, _day)];
}

//------------------------------------------------------------------------------
flw::Date& flw::Date::year(int year) {
    if (year >= 1 && year <= 9999) {
        _year = year;
    }

    return *this;
}

//------------------------------------------------------------------------------
int flw::Date::yearday() const {
    auto res  = 0;
    auto leap = _date_is_leapyear(_year);

    for (auto m = 1; m < _month && m < 13; m++) {
        if (leap) {
            res += _DATE_DAYS_MONTH_LEAP[m];
        }
        else {
            res += _DATE_DAYS_MONTH[m];
        }
    }

    return res + _day;
}



namespace flw {
    //--------------------------------------------------------------------------
    class _DateChooserCanvas : public Fl_Widget {
        Date                    _date[7][8];
        char                    _text[7][8][30];
        int                     _col;
        int                     _row;

    public:
        //----------------------------------------------------------------------
        _DateChooserCanvas() : Fl_Widget(0, 0, 0, 0, 0) {
            _row   = 1;
            _col   = 1;

            strncpy(_text[0][0], "Week", 20);
            strncpy(_text[0][1], "Mon", 20);
            strncpy(_text[0][2], "Tue", 20);
            strncpy(_text[0][3], "Wed", 20);
            strncpy(_text[0][4], "Thu", 20);
            strncpy(_text[0][5], "Fri", 20);
            strncpy(_text[0][6], "Sat", 20);
            strncpy(_text[0][7], "Sun", 20);
        }

        //----------------------------------------------------------------------
        void draw() override {
            int cw  = w() / 8;
            int ch  = h() / 7;
            int d   = 99;
            int col = 0;

            fl_rectf(x(), y(), w(), h(), FL_BACKGROUND_COLOR);

            for (int r = 0; r < 7; r++) {
                for (int c  = 0; c < 8; c++) {
                    auto t  = _text[r][c];
                    auto x1 = x() + (c * cw) + 1;
                    auto y1 = y() + (r * ch) + 1;
                    auto w1 = (c == 7) ? x() + w() - x1 : cw - 2;
                    auto h1 = (r == 6) ? y() + h() - y1 : ch - 2;
                    auto bg = FL_BACKGROUND_COLOR;
                    auto fg = FL_FOREGROUND_COLOR;

                    if (r == 0 || c == 0) {
                        ;
                    }
                    else {
                        int v = atoi(t);

                        if (v <= d && col < 3) {
                            d = v;
                            col++;
                        }

                        if (r == _row && c == _col) {
                            bg = FL_SELECTION_COLOR;
                        }
                        else if (col == 2) {
                            bg = FL_BACKGROUND2_COLOR;
                        }
                        else {
                            bg = FL_DARK2;
                        }
                    }

                    fl_rectf(x1, y1, w1, h1, bg);
                    fl_color(fg);
                    fl_font(r == 0 || c == 0 ? FL_HELVETICA_BOLD : FL_HELVETICA, flw::PREF_FONTSIZE);
                    fl_draw(t, x1, y1, w1, h1, FL_ALIGN_CENTER | FL_ALIGN_INSIDE);
                    fl_rect(x1, y1, w1, h1, FL_DARK3);
                }
            }
        }

        //----------------------------------------------------------------------
        Date& get() {
            return _date[_row][_col];
        }

        //----------------------------------------------------------------------
        int handle(int event) override {
            if (event == FL_PUSH) {
                take_focus();

                auto cw = (int) (w() / 8);
                auto ch = (int) (h() / 7);

                for (auto r = 1; r < 7; r++) {
                    for (auto c = 1; c < 8; c++) {
                        auto x1 = (int) (x() + (c * cw));
                        auto y1 = (int) (y() + (r * ch));

                        if (Fl::event_x() >= x1 && Fl::event_x() < x1 + cw && Fl::event_y() >= y1 && Fl::event_y() < y1 + ch) {
                            _row = r;
                            _col = c;
                            do_callback();
                            redraw();
                            break;
                        }
                    }
                }

                return 1;
            }
            else if (event == FL_FOCUS) {
                return 1;
            }
            else if (event == FL_KEYDOWN) {
                if (Fl::event_command() == 0) {
                    int num = Fl::event_key();

                    if (num == FL_Up) {
                        if (_row > 1) {
                            _row--;
                            redraw();
                            do_callback();
                        }

                        return 1;
                    }
                    else if (num == FL_Down) {
                        if (_row < 6) {
                            _row++;
                            redraw();
                            do_callback();
                        }

                        return 1;
                    }
                    else if (num == FL_Left) {
                        if (_col > 1) {
                            _col--;
                            redraw();
                            do_callback();
                        }

                        return 1;
                    }
                    else if (num == FL_Right) {
                        if (_col < 7) {
                            _col++;
                            redraw();
                            do_callback();
                        }

                        return 1;
                    }
                }
            }

            return Fl_Widget::handle(event);
        }

        //----------------------------------------------------------------------
        void set_current(int row, int col) {
            if (row != _row || col != _col) {
                _row = row;
                _col = col;
                do_callback();
            }
        }

        //----------------------------------------------------------------------
        void set_date(int row, int col, const Date& date) {
            _date[row][col] = date;
        }

        //----------------------------------------------------------------------
        void set_text(int row, int col, const char* text) {
            strncpy(_text[row][col], text, 30);
        }
    };
}

//------------------------------------------------------------------------------
flw::DateChooser::DateChooser(int X, int Y, int W, int H, const char* l) : Fl_Group(X, Y, W, H, l) {
    end();

    _h           = -1;
    _w           = -1;
    _month_label = new Fl_Box(0, 0, 0, 0, "");
    _canvas      = new _DateChooserCanvas();
    _b6          = new Fl_Repeat_Button(0, 0, 0, 0, "@|<");
    _b1          = new Fl_Repeat_Button(0, 0, 0, 0, "@<<");
    _b2          = new Fl_Repeat_Button(0, 0, 0, 0, "@<");
    _b5          = new Fl_Button(0, 0, 0, 0, "@refresh");
    _b3          = new Fl_Repeat_Button(0, 0, 0, 0, "@>");
    _b4          = new Fl_Repeat_Button(0, 0, 0, 0, "@>>");
    _b7          = new Fl_Repeat_Button(0, 0, 0, 0, "@>|");

    Date date;
    set(date);
    _set_label();

    add(_b1);
    add(_b2);
    add(_b5);
    add(_b3);
    add(_b4);
    add(_b6);
    add(_b7);
    add(_month_label);
    add(_canvas);

    _b1->callback(flw::DateChooser::_Callback, this);
    _b1->tooltip("Previous year");
    _b2->callback(flw::DateChooser::_Callback, this);
    _b2->tooltip("Previous month");
    _b3->callback(flw::DateChooser::_Callback, this);
    _b3->tooltip("Next month");
    _b4->callback(flw::DateChooser::_Callback, this);
    _b4->tooltip("Next year");
    _b5->callback(flw::DateChooser::_Callback, this);
    _b5->tooltip("Today");
    _b6->callback(flw::DateChooser::_Callback, this);
    _b6->tooltip("10 years in the past");
    _b7->callback(flw::DateChooser::_Callback, this);
    _b7->tooltip("10 years in the future");
    _canvas->callback(flw::DateChooser::_Callback, this);
    _month_label->box(FL_UP_BOX);

    flw::theme::labelfont(this);
    tooltip("Use arrow keys to navigate\nUse ctrl+left/right to change month");
    flw::DateChooser::resize(X, Y, W, H);
}

//------------------------------------------------------------------------------
void flw::DateChooser::_Callback(Fl_Widget* w, void* o) {
    auto dc = (DateChooser*) o;
    auto dt = dc->get();

    if (w == dc->_b6) {
        dt.add_months(-120);
        dc->set(dt);
    }
    else if (w == dc->_b1) {
        dt.add_months(-12);
        dc->set(dt);
    }
    else if (w == dc->_b2) {
        dt.add_months(-1);
        dc->set(dt);
    }
    else if (w == dc->_b5) {
        dt = Date::FromTime(::time(0));
        dc->set(dt);
    }
    else if (w == dc->_b3) {
        dt.add_months(1);
        dc->set(dt);
    }
    else if (w == dc->_b4) {
        dt.add_months(12);
        dc->set(dt);
    }
    else if (w == dc->_b7) {
        dt.add_months(120);
        dc->set(dt);
    }
    else if (w == dc->_canvas) {
        dc->_set_label();
        dc->do_callback();
    }

    dc->_canvas->take_focus();
}

//------------------------------------------------------------------------------
void flw::DateChooser::draw() {
    _month_label->labelfont(FL_HELVETICA_BOLD);
    Fl_Group::draw();
}

//------------------------------------------------------------------------------
void flw::DateChooser::focus() {
    _canvas->take_focus();
}

//------------------------------------------------------------------------------
flw::Date flw::DateChooser::get() const {
    auto canvas = (flw::_DateChooserCanvas*) _canvas;
    return canvas->get();
}

//------------------------------------------------------------------------------
int flw::DateChooser::handle(int event) {
    if (event == FL_KEYDOWN) {
        if (Fl::event_command()) {
            Date dt = get();

            if (Fl::event_key() == FL_Left) {
                dt.add_months(-1);
                set(dt);
                return 1;
            }
            else if (Fl::event_key() == FL_Right) {
                dt.add_months(+1);
                set(dt);
                return 1;
            }
        }
    }

    return Fl_Group::handle(event);
}

//------------------------------------------------------------------------------
void flw::DateChooser::resize(int X, int Y, int W, int H) {
    Fl_Widget::resize(X, Y, W, H);

    if (W == _w && H == _h) {
        return;
    }

    int w1 = W / 8;
    int w2 = W - (w1 * 6);
    int fs = flw::PREF_FONTSIZE;

    _b6->resize(X, Y, w1, fs * 2);
    _b1->resize(X + w1, Y, w1, fs * 2);
    _b2->resize(X + w1 * 2, Y, w1, fs * 2);

    _b3->resize(X + w1 * 3 + w2, Y, w1, fs * 2);
    _b4->resize(X + w1 * 4 + w2, Y, w1, fs * 2);
    _b7->resize(X + w1 * 5 + w2, Y, w1, fs * 2);

    _b5->resize(X + w1 * 3, Y, w2, fs * 2);

    _month_label->resize(X, Y + fs * 2, W, fs * 2);
    _canvas->resize(X, Y + fs * 4, W, H - fs * 4);

    _w = W;
    _h = H;
}

//------------------------------------------------------------------------------
void flw::DateChooser::set(const Date& date) {
    auto canvas = (flw::_DateChooserCanvas*) _canvas;
    auto date2  = date;

    if (date2.year() < 2 && date2.month() < 2) {
        date2.month(2);
    }

    auto start_cell   = 0;
    auto first_date   = Date(date2.year(), date2.month(), 1);
    auto current_date = Date();
    char tmp[30];

    // Optimize where to start with the 1:st in every month

    start_cell    = (int) first_date.weekday() - 1;
    start_cell    = start_cell + first_date.month_days() < 32 ? start_cell + 7 : start_cell;
    current_date  = first_date;

    current_date.add_days(-start_cell);

    for (auto r = 1; r <= 6; r++) {
        snprintf(tmp, 30, "%02d", current_date.week());
        canvas->set_text(r, 0, tmp);

        for (auto c = 1; c <= 7; c++) {
            snprintf(tmp, 30, "%02d", current_date.day());
            canvas->set_text(r, c, tmp);
            canvas->set_date(r, c, current_date);

            if (current_date.month() == date2.month() && current_date.day() == date2.day()) {
                canvas->set_current(r, c);
            }

            current_date.add_days(1);
        }
    }

    _set_label();
    redraw();
}

//------------------------------------------------------------------------------
void flw::DateChooser::_set_label() {
    auto canvas = (flw::_DateChooserCanvas*) _canvas;
    auto date   = canvas->get();
    auto string = date.format(Date::FORMAT::YEAR_MONTH_LONG);

    _month_label->copy_label(string.c_str());
}

namespace flw {
    namespace dlg {
        //----------------------------------------------------------------------
        class _DlgDate : public Fl_Double_Window {
            DateChooser*            _date_chooser;
            Date&                   _value;
            Fl_Button*              _cancel;
            Fl_Button*              _ok;
            bool                    _res;

        public:
            //------------------------------------------------------------------
            _DlgDate(const char* title, Date& date) : Fl_Double_Window(0, 0, 0, 0), _value(date) {
                end();

                _date_chooser = new DateChooser();
                _ok           = new Fl_Return_Button(0, 0, 0, 0, "&Ok");
                _cancel       = new Fl_Button(0, 0, 0, 0, "&Cancel");
                _res          = false;

                add(_date_chooser);
                add(_ok);
                add(_cancel);

                _cancel->callback(Callback, this);
                _date_chooser->focus();
                _date_chooser->set(_value);
                _ok->callback(Callback, this);

                flw::theme::labelfont(this);
                callback(Callback, this);
                copy_label(title);
                size(flw::PREF_FONTSIZE * 33, flw::PREF_FONTSIZE * 21);
                size_range(flw::PREF_FONTSIZE * 22, flw::PREF_FONTSIZE * 14);
                set_modal();
                resizable(this);
            }

            //------------------------------------------------------------------
            static void Callback(Fl_Widget* w, void* o) {
                _DlgDate* dlg = (_DlgDate*) o;

                if (w == dlg) {
                    ;
                }
                else if (w == dlg->_cancel) {
                    dlg->hide();
                }
                else if (w == dlg->_ok) {
                    dlg->hide();
                    dlg->_res = true;
                }
            }

            //------------------------------------------------------------------
            void resize(int X, int Y, int W, int H) override {
                auto fs = flw::PREF_FONTSIZE;

                Fl_Double_Window::resize(X, Y, W, H);

                _date_chooser->resize (4,                 4,                  W - 8,    H - fs * 2 - 16);
                _cancel->resize       (W - fs * 16 - 8,   H - fs * 2 - 4,     fs * 8,   fs * 2);
                _ok->resize           (W - fs * 8 - 4,    H - fs * 2 - 4,     fs * 8,   fs * 2);
            }

            //------------------------------------------------------------------
            bool run(Fl_Window* parent) {
                flw::util::center_window(this, parent);
                show();

                while (visible() != 0) {
                    Fl::wait();
                    Fl::flush();
                }

                if (_res == true) {
                    _value = _date_chooser->get();
                }

                return _res;
            }
        };

        //----------------------------------------------------------------------
        bool date(const std::string& title, flw::Date& date, Fl_Window* parent) {
            flw::dlg::_DlgDate dlg(title.c_str(), date);
            return dlg.run(parent);
        }
    }
}




namespace flw {
    namespace dlg {
        //----------------------------------------------------------------------
        class _DlgHtml  : public Fl_Double_Window {
            Fl_Help_View*               _html;
            Fl_Return_Button*           _close;

        public:
            //------------------------------------------------------------------
            _DlgHtml(const char* title, const char* text, Fl_Window* parent, int W, int H) :
            Fl_Double_Window(0, 0, flw::PREF_FONTSIZE * W,flw::PREF_FONTSIZE * H) {
                end();

                _close = new Fl_Return_Button(0, 0, 0, 0, "&Close");
                _html  = new Fl_Help_View(0, 0, 0, 0);

                add(_close);
                add(_html);

                _close->callback(_DlgHtml::Callback, this);
                _close->labelfont(flw::PREF_FONT);
                _close->labelsize(flw::PREF_FONTSIZE);
                _html->textfont(flw::PREF_FONT);
                _html->textsize(flw::PREF_FONTSIZE);
                _html->value(text);

                callback(_DlgHtml::Callback, this);
                copy_label(title);
                size_range(flw::PREF_FONTSIZE * 24, flw::PREF_FONTSIZE * 12);
                set_modal();
                resizable(this);
                flw::util::center_window(this, parent);
            }

            //------------------------------------------------------------------
            static void Callback(Fl_Widget* w, void* o) {
                auto dlg = (_DlgHtml*) o;

                if (w == dlg || w == dlg->_close) {
                    dlg->hide();
                }
            }

            //------------------------------------------------------------------
            void resize(int X, int Y, int W, int H) override {
                Fl_Double_Window::resize(X, Y, W, H);

                _html->resize(4, 4, W - 8, H - flw::PREF_FONTSIZE * 2 - 16);
                _close->resize(W - flw::PREF_FONTSIZE * 8 - 4, H - flw::PREF_FONTSIZE * 2 - 4, flw::PREF_FONTSIZE * 8, flw::PREF_FONTSIZE * 2);
            }

            //------------------------------------------------------------------
            void run() {
                show();

                while (visible() != 0) {
                    Fl::wait();
                    Fl::flush();
                }
            }
        };

        //----------------------------------------------------------------------
        //----------------------------------------------------------------------
        //----------------------------------------------------------------------

        //----------------------------------------------------------------------
        class _DlgList : public Fl_Double_Window {
            flw::ScrollBrowser*         _list;
            Fl_Return_Button*           _close;

        public:
            //------------------------------------------------------------------
            _DlgList(const char* title, const StringVector& list, Fl_Window* parent = nullptr, bool fixed_font = false, int W = 50, int H = 20) :
            Fl_Double_Window(0, 0, (fixed_font ? flw::PREF_FIXED_FONTSIZE : flw::PREF_FONTSIZE) * W, (fixed_font ? flw::PREF_FIXED_FONTSIZE : flw::PREF_FONTSIZE) * H) {
                end();

                _close = new Fl_Return_Button(0, 0, 0, 0, "&Close");
                _list  = new flw::ScrollBrowser();

                add(_close);
                add(_list);

                _close->callback(_DlgList::Callback, this);
                _close->labelfont(flw::PREF_FONT);
                _close->labelsize(flw::PREF_FONTSIZE);

                _list->take_focus();

                for (auto& s : list) {
                    _list->add(s.c_str());
                }

                if (fixed_font == true) {
                    _list->textfont(flw::PREF_FIXED_FONT);
                    _list->textsize(flw::PREF_FIXED_FONTSIZE);
                }
                else {
                    _list->textfont(flw::PREF_FONT);
                    _list->textsize(flw::PREF_FONTSIZE);
                }

                callback(_DlgList::Callback, this);
                copy_label(title);
                size_range(flw::PREF_FONTSIZE * 24, flw::PREF_FONTSIZE * 12);
                set_modal();
                resizable(this);
                flw::util::center_window(this, parent);
            }

            //------------------------------------------------------------------
            static void Callback(Fl_Widget* w, void* o) {
                auto dlg = (_DlgList*) o;

                if (w == dlg || w == dlg->_close) {
                    dlg->hide();
                }
            }

            //------------------------------------------------------------------
            void resize(int X, int Y, int W, int H) override {
                Fl_Double_Window::resize(X, Y, W, H);

                _list->resize(4, 4, W - 8, H - flw::PREF_FONTSIZE * 2 - 16);
                _close->resize(W - flw::PREF_FONTSIZE * 8 - 4, H - flw::PREF_FONTSIZE * 2 - 4, flw::PREF_FONTSIZE * 8, flw::PREF_FONTSIZE * 2);
            }

            //------------------------------------------------------------------
            void run() {
                show();

                while (visible() != 0) {
                    Fl::wait();
                    Fl::flush();
                }
            }
        };

        //----------------------------------------------------------------------
        //----------------------------------------------------------------------
        //----------------------------------------------------------------------

        //----------------------------------------------------------------------
        class _DlgSelect : public Fl_Double_Window {
            Fl_Button*                  _close;
            Fl_Button*                  _cancel;
            ScrollBrowser*              _list;
            Fl_Input*                   _filter;
            const StringVector&         _strings;

        public:
            //------------------------------------------------------------------
            _DlgSelect(const char* title, Fl_Window* parent, const StringVector& strings, int selected_string_index, std::string selected_string, bool fixed_font, int W, int H) :
            Fl_Double_Window(0, 0, ((fixed_font == true) ? flw::PREF_FIXED_FONTSIZE : flw::PREF_FONTSIZE) * W, ((fixed_font == true) ? flw::PREF_FIXED_FONTSIZE : flw::PREF_FONTSIZE) * H),
            _strings(strings) {
                end();

                _filter = new Fl_Input(0, 0, 0, 0);
                _close  = new Fl_Return_Button(0, 0, 0, 0, "&Select");
                _cancel = new Fl_Button(0, 0, 0, 0, "&Cancel");
                _list   = new flw::ScrollBrowser(0, 0, 0, 0);

                add(_filter);
                add(_list);
                add(_cancel);
                add(_close);

                _cancel->callback(_DlgSelect::Callback, this);
                _close->callback(_DlgSelect::Callback, this);
                _filter->callback(_DlgSelect::Callback, this);
                _filter->tooltip("Enter text to filter rows that macthes the text\nPress tab to switch focus between input and list widget.");
                _filter->when(FL_WHEN_CHANGED);
                _list->callback(_DlgSelect::Callback, this);
                _list->tooltip("Use Page Up or Page Down in list to scroll faster");

                if (fixed_font == true) {
                    _filter->textfont(flw::PREF_FIXED_FONT);
                    _filter->textsize(flw::PREF_FIXED_FONTSIZE);
                    _list->textfont(flw::PREF_FIXED_FONT);
                    _list->textsize(flw::PREF_FIXED_FONTSIZE);
                }
                else {
                    _filter->textfont(flw::PREF_FONT);
                    _filter->textsize(flw::PREF_FONTSIZE);
                    _list->textfont(flw::PREF_FONT);
                    _list->textsize(flw::PREF_FONTSIZE);
                }

                {
                    auto r = 0;
                    auto f = 0;

                    for (auto& string : _strings) {
                        _list->add(string.c_str());

                        if (string == selected_string) {
                            r = f + 1;
                        }

                        f++;
                    }

                    if (selected_string_index > 0 && selected_string_index <= (int) _strings.size()) {
                        _list->value(selected_string_index);
                        _list->middleline(selected_string_index);
                    }
                    else if (r > 0) {
                        _list->value(r);
                        _list->middleline(r);
                    }
                    else {
                        _list->value(1);
                    }
                }

                _filter->take_focus();
                flw::theme::labelfont(this);
                callback(_DlgSelect::Callback, this);
                copy_label(title);
                activate_button();
                size_range(flw::PREF_FONTSIZE * 24, flw::PREF_FONTSIZE * 12);
                set_modal();
                resizable(this);
                flw::util::center_window(this, parent);
            }

            //------------------------------------------------------------------
            void activate_button() {
                if (_list->value() == 0) {
                    _close->deactivate();
                }
                else {
                    _close->activate();
                }
            }

            //------------------------------------------------------------------
            static void Callback(Fl_Widget* w, void* o) {
                auto dlg = (_DlgSelect*) o;

                if (w == dlg || w == dlg->_cancel) {
                    dlg->_list->deselect();
                    dlg->hide();
                }
                else if (w == dlg->_filter) {
                    dlg->filter(dlg->_filter->value());
                    dlg->activate_button();
                }
                else if (w == dlg->_list) {
                    dlg->activate_button();

                    if (Fl::event_clicks() > 0 && dlg->_close->active()) {
                        Fl::event_clicks(0);
                        dlg->hide();
                    }
                }
                else if (w == dlg->_close) {
                    dlg->hide();
                }
            }

            //------------------------------------------------------------------
            void filter(const char* filter) {
                _list->clear();

                for (auto& string : _strings) {
                    if (*filter == 0) {
                        _list->add(string.c_str());
                    }
                    else if (strstr(string.c_str(), filter) != nullptr) {
                        _list->add(string.c_str());
                    }
                }

                _list->value(1);
            }

            //------------------------------------------------------------------
            // Tab key changes focus between list and input
            //
            int handle(int event) override {
                if (event == FL_KEYDOWN) {
                    if (Fl::event_key() == FL_Enter) {
                        if (_list->value() > 0) {
                            hide();
                        }

                        return 1;
                    }
                    else if (Fl::event_key() == FL_Tab) {
                        if (Fl::focus() == _list || Fl::focus() == _list->menu()) {
                            _filter->take_focus();
                        }
                        else {
                            _list->take_focus();
                        }

                        return 1;
                    }
                }

                return Fl_Double_Window::handle(event);
            }

            //------------------------------------------------------------------
            void resize(int X, int Y, int W, int H) override {
                Fl_Double_Window::resize(X, Y, W, H);

                _filter->resize(4, 4, W - 8, flw::PREF_FONTSIZE * 2);
                _list->resize(4, flw::PREF_FONTSIZE * 2  + 8, W - 8, H - flw::PREF_FONTSIZE * 4 - 20);
                _cancel->resize(W - flw::PREF_FONTSIZE * 16 - 8, H - flw::PREF_FONTSIZE * 2 - 4, flw::PREF_FONTSIZE * 8, flw::PREF_FONTSIZE * 2);
                _close->resize(W - flw::PREF_FONTSIZE * 8 - 4, H - flw::PREF_FONTSIZE * 2 - 4, flw::PREF_FONTSIZE * 8, flw::PREF_FONTSIZE * 2);
            }

            //------------------------------------------------------------------
            int run() {
                show();

                while (visible() != 0) {
                    Fl::wait();
                    Fl::flush();
                }

                auto row = _list->value();

                if (row > 0) {
                    auto selected = _list->text(row);

                    for (int f = 0; f < (int) _strings.size(); f++) {
                        auto& string = _strings[f];

                        if (string == selected) {
                            return f + 1;
                        }
                    }
                }

                return 0;
            }
        };

        //----------------------------------------------------------------------
        //----------------------------------------------------------------------
        //----------------------------------------------------------------------

        const char* PASSWORD_CANCEL = "Cancel";
        const char* PASSWORD_OK     = "Ok";

        //----------------------------------------------------------------------
        class _DlgPassword : public Fl_Double_Window {
        public:
            enum class TYPE {
                                        ASK_PASSWORD,
                                        ASK_PASSWORD_AND_KEYFILE,
                                        CONFIRM_PASSWORD,
                                        CONFIRM_PASSWORD_AND_KEYFILE,
            };

        private:
            Fl_Button*                  _browse;
            Fl_Button*                  _cancel;
            Fl_Button*                  _close;
            Fl_Input*                   _file;
            Fl_Input*                   _password1;
            Fl_Input*                   _password2;
            _DlgPassword::TYPE          _mode;
            bool                        _ret;

        public:
            //------------------------------------------------------------------
            _DlgPassword(const char* title, Fl_Window* parent, _DlgPassword::TYPE mode) :
            Fl_Double_Window(0, 0, 0, 0) {
                end();

                _password1 = new Fl_Secret_Input(0, 0, 0, 0, "Password");
                _password2 = new Fl_Secret_Input(0, 0, 0, 0, "Enter Password Again");
                _browse    = new Fl_Button(0, 0, 0, 0, "&Browse");
                _cancel    = new Fl_Button(0, 0, 0, 0, flw::dlg::PASSWORD_CANCEL);
                _close     = new Fl_Return_Button(0, 0, 0, 0, flw::dlg::PASSWORD_OK);
                _file      = new Fl_Output(0, 0, 0, 0, "Key File");
                _mode      = mode;
                _ret       = false;

                add(_password1);
                add(_password2);
                add(_file);
                add(_browse);
                add(_cancel);
                add(_close);

                _browse->callback(_DlgPassword::Callback, this);
                _cancel->callback(_DlgPassword::Callback, this);
                _close->callback(_DlgPassword::Callback, this);
                _close->deactivate();
                _file->align(FL_ALIGN_TOP | FL_ALIGN_LEFT);
                _file->textfont(flw::PREF_FIXED_FONT);
                _file->textsize(flw::PREF_FONTSIZE);
                _file->tooltip("Select optional key file");
                _password1->align(FL_ALIGN_TOP | FL_ALIGN_LEFT);
                _password1->callback(_DlgPassword::Callback, this);
                _password1->textfont(flw::PREF_FIXED_FONT);
                _password1->textsize(flw::PREF_FONTSIZE);
                _password1->when(FL_WHEN_CHANGED);
                _password2->align(FL_ALIGN_TOP | FL_ALIGN_LEFT);
                _password2->callback(_DlgPassword::Callback, this);
                _password2->textfont(flw::PREF_FIXED_FONT);
                _password2->textsize(flw::PREF_FONTSIZE);
                _password2->when(FL_WHEN_CHANGED);

                auto W = 40 * flw::PREF_FONTSIZE;
                auto H = 11 * flw::PREF_FONTSIZE + 28;

                if (_mode == _DlgPassword::TYPE::ASK_PASSWORD) {
                    _password2->hide();
                    _browse->hide();
                    _file->hide();
                    W = 30 * flw::PREF_FONTSIZE;
                    H = 5 * flw::PREF_FONTSIZE + 16;
                }
                else if (_mode == _DlgPassword::TYPE::CONFIRM_PASSWORD) {
                    _browse->hide();
                    _file->hide();
                    W = 30 * flw::PREF_FONTSIZE;
                    H = 8 * flw::PREF_FONTSIZE + 24;
                }
                else if (_mode == _DlgPassword::TYPE::ASK_PASSWORD_AND_KEYFILE) {
                    _password2->hide();
                    W = 40 * flw::PREF_FONTSIZE;
                    H = 8 * flw::PREF_FONTSIZE + 24;
                }

                flw::theme::labelfont(this);
                callback(_DlgPassword::Callback, this);
                label(title);
                size(W, H);
                size_range(W, H);
                set_modal();
                resizable(this);
                flw::util::center_window(this, parent);
            }

            //------------------------------------------------------------------
            static void Callback(Fl_Widget* w, void* o) {
                auto dlg = (_DlgPassword*) o;

                if (w == dlg) {
                    ;
                }
                else if (w == dlg->_password1) {
                    dlg->check();
                }
                else if (w == dlg->_password2) {
                    dlg->check();
                }
                else if (w == dlg->_browse) {
                    auto filename = fl_file_chooser("Select Key File", nullptr, nullptr, 0);

                    if (filename) {
                        dlg->_file->value(filename);
                    }
                    else {
                        dlg->_file->value("");
                    }
                }
                else if (w == dlg->_cancel) {
                    dlg->_ret = false;
                    dlg->hide();
                }
                else if (w == dlg->_close) {
                    dlg->_ret = true;
                    dlg->hide();
                }
            }

            //------------------------------------------------------------------
            void check() {
                auto p1 = _password1->value();
                auto p2 = _password2->value();

                if (_mode == _DlgPassword::TYPE::ASK_PASSWORD ||
                    _mode == _DlgPassword::TYPE::ASK_PASSWORD_AND_KEYFILE) {

                    if (strlen(p1)) {
                        _close->activate();
                    }
                    else {
                        _close->deactivate();
                    }
                }
                else if (_mode == _DlgPassword::TYPE::CONFIRM_PASSWORD ||
                         _mode == _DlgPassword::TYPE::CONFIRM_PASSWORD_AND_KEYFILE) {

                    if (strlen(p1) && strcmp(p1, p2) == 0) {
                        _close->activate();
                    }
                    else {
                        _close->deactivate();
                    }
                }
            }

            //------------------------------------------------------------------
            void resize(int X, int Y, int W, int H) override {
                Fl_Double_Window::resize(X, Y, W, H);

                if (_mode == _DlgPassword::TYPE::ASK_PASSWORD) {
                    _password1->resize(4, flw::PREF_FONTSIZE + 4, W - 8, flw::PREF_FONTSIZE * 2);
                }
                else if (_mode == _DlgPassword::TYPE::CONFIRM_PASSWORD) {
                    _password1->resize(4, flw::PREF_FONTSIZE + 4, W - 8, flw::PREF_FONTSIZE * 2);
                    _password2->resize(4, flw::PREF_FONTSIZE * 4 + 12, W - 8, flw::PREF_FONTSIZE * 2);
                }
                else if (_mode == _DlgPassword::TYPE::ASK_PASSWORD_AND_KEYFILE) {
                    _password1->resize(4, flw::PREF_FONTSIZE + 4, W - 8, flw::PREF_FONTSIZE * 2);
                    _file->resize(4, flw::PREF_FONTSIZE * 4 + 12, W - flw::PREF_FONTSIZE * 8 - 12, flw::PREF_FONTSIZE * 2);
                    _browse->resize(W - flw::PREF_FONTSIZE * 8 - 4, flw::PREF_FONTSIZE * 4 + 12, flw::PREF_FONTSIZE * 8, flw::PREF_FONTSIZE * 2);
                }
                else if (_mode == _DlgPassword::TYPE::CONFIRM_PASSWORD_AND_KEYFILE) {
                    _password1->resize(4, flw::PREF_FONTSIZE + 4, W - 8, flw::PREF_FONTSIZE * 2);
                    _password2->resize(4, flw::PREF_FONTSIZE * 4 + 12, W - 8, flw::PREF_FONTSIZE * 2);
                    _file->resize(4, flw::PREF_FONTSIZE * 7 + 16, W - flw::PREF_FONTSIZE * 8 - 12, flw::PREF_FONTSIZE * 2);
                    _browse->resize(W - flw::PREF_FONTSIZE * 8 - 4, flw::PREF_FONTSIZE * 7 + 16, flw::PREF_FONTSIZE * 8, flw::PREF_FONTSIZE * 2);
                }

                _cancel->resize(W - flw::PREF_FONTSIZE * 16 - 8, H - flw::PREF_FONTSIZE * 2 - 4, flw::PREF_FONTSIZE * 8, flw::PREF_FONTSIZE * 2);
                _close->resize(W - flw::PREF_FONTSIZE * 8 - 4, H - flw::PREF_FONTSIZE * 2 - 4, flw::PREF_FONTSIZE * 8, flw::PREF_FONTSIZE * 2);
            }

            //------------------------------------------------------------------
            bool run(std::string& password, std::string& file) {
                show();

                while (visible() != 0) {
                    Fl::wait();
                    Fl::flush();
                }

                if (_ret) {
                    file = _file->value();
                    password = _password1->value();
                }

                flw::util::zero_memory((void*) _password1->value(), strlen(_password1->value()));
                flw::util::zero_memory((void*) _password2->value(), strlen(_password2->value()));
                flw::util::zero_memory((void*) _file->value(), strlen(_file->value()));

                return _ret;
            }
        };

        //----------------------------------------------------------------------
        //----------------------------------------------------------------------
        //----------------------------------------------------------------------

        //----------------------------------------------------------------------
        class _DlgText : public Fl_Double_Window {
            Fl_Button*                  _close;
            Fl_Button*                  _cancel;
            Fl_Text_Buffer*             _buffer;
            Fl_Text_Display*            _text;
            bool                        _edit;
            char*                       _res;

        public:
            //------------------------------------------------------------------
            _DlgText(const char* title, const char* text, bool edit, Fl_Window* parent, int W, int H) :
            Fl_Double_Window(0, 0, flw::PREF_FONTSIZE * W, flw::PREF_FONTSIZE * H) {
                end();

                _cancel = new Fl_Button(0, 0, 0, 0, "C&ancel");
                _close  = (edit == false) ? new Fl_Return_Button(0, 0, 0, 0, "&Close") : new Fl_Button(0, 0, 0, 0, "&Close");
                _text   = (edit == false) ? new Fl_Text_Display(0, 0, 0, 0) : new Fl_Text_Editor(0, 0, 0, 0);
                _buffer = new Fl_Text_Buffer();
                _edit   = edit;
                _res    = nullptr;

                add(_cancel);
                add(_close);
                add(_text);

                _buffer->text(text);
                _cancel->callback(_DlgText::Callback, this);
                _cancel->labelfont(flw::PREF_FONT);
                _cancel->labelsize(flw::PREF_FONTSIZE);
                _close->callback(_DlgText::Callback, this);
                _close->labelfont(flw::PREF_FONT);
                _close->labelsize(flw::PREF_FONTSIZE);
                _text->buffer(_buffer);
                _text->linenumber_align(FL_ALIGN_RIGHT);
                _text->linenumber_bgcolor(FL_BACKGROUND_COLOR);
                _text->linenumber_fgcolor(FL_FOREGROUND_COLOR);
                _text->linenumber_font(flw::PREF_FIXED_FONT);
                _text->linenumber_size(flw::PREF_FIXED_FONTSIZE);
                _text->linenumber_width(flw::PREF_FIXED_FONTSIZE * 3);
                _text->take_focus();
                _text->textfont(flw::PREF_FIXED_FONT);
                _text->textsize(flw::PREF_FIXED_FONTSIZE);

                if (edit == false) {
                    _cancel->hide();
                }

                callback(_DlgText::Callback, this);
                copy_label(title);
                size_range(flw::PREF_FONTSIZE * 24, flw::PREF_FONTSIZE * 12);
                set_modal();
                resizable(this);
                flw::util::center_window(this, parent);
            }

            //------------------------------------------------------------------
            ~_DlgText() {
               _text->buffer(nullptr);
                delete _buffer;
            }

            //------------------------------------------------------------------
            static void Callback(Fl_Widget* w, void* o) {
                auto dlg = (_DlgText*) o;

                if (w == dlg || w == dlg->_cancel) {
                    dlg->hide();
                }
                else if (w == dlg->_close) {
                    if (dlg->_edit == true) {
                        dlg->_res = dlg->_buffer->text();
                    }

                    dlg->hide();
                }
            }

            //------------------------------------------------------------------
            void resize(int X, int Y, int W, int H) override {
                Fl_Double_Window::resize(X, Y, W, H);

                _text->resize(4, 4, W - 8, H - flw::PREF_FONTSIZE * 2 - 16);
                _cancel->resize(W - flw::PREF_FONTSIZE * 16 - 8, H - flw::PREF_FONTSIZE * 2 - 4, flw::PREF_FONTSIZE * 8, flw::PREF_FONTSIZE * 2);
                _close->resize(W - flw::PREF_FONTSIZE * 8 - 4, H - flw::PREF_FONTSIZE * 2 - 4, flw::PREF_FONTSIZE * 8, flw::PREF_FONTSIZE * 2);
            }

            //------------------------------------------------------------------
            char* run() {
                show();

                while (visible() != 0) {
                    Fl::wait();
                    Fl::flush();
                }

                return _res;
            }
        };
    }
}

//------------------------------------------------------------------------------
void flw::dlg::html(std::string title, const std::string& text, Fl_Window* parent, int W, int H) {
    _DlgHtml dlg(title.c_str(), text.c_str(), parent, W, H);
    dlg.run();
}

//------------------------------------------------------------------------------
void flw::dlg::list(std::string title, const StringVector& list, Fl_Window* parent, bool fixed_font, int W, int H) {
    _DlgList dlg(title.c_str(), list, parent, fixed_font, W, H);
    dlg.run();
}

//------------------------------------------------------------------------------
void flw::dlg::list(std::string title, const std::string& list, Fl_Window* parent, bool fixed_font, int W, int H) {
    auto list2 = flw::util::split( list, "\n");
    _DlgList dlg(title.c_str(), list2, parent, fixed_font, W, H);
    dlg.run();
}

//------------------------------------------------------------------------------
void flw::dlg::panic(std::string message) {
    fl_alert("panic! I have to quit\n%s", message.c_str());
    exit(1);
}

//------------------------------------------------------------------------------
bool flw::dlg::password1(std::string title, std::string& password, Fl_Window* parent) {
    std::string file;
    _DlgPassword dlg(title.c_str(), parent, _DlgPassword::TYPE::ASK_PASSWORD);
    return dlg.run(password, file);
}

//------------------------------------------------------------------------------
bool flw::dlg::password2(std::string title, std::string& password, Fl_Window* parent) {
    std::string file;
    _DlgPassword dlg(title.c_str(), parent, _DlgPassword::TYPE::CONFIRM_PASSWORD);
    return dlg.run(password, file);
}

//------------------------------------------------------------------------------
bool flw::dlg::password3(std::string title, std::string& password, std::string& file, Fl_Window* parent) {
    _DlgPassword dlg(title.c_str(), parent, _DlgPassword::TYPE::ASK_PASSWORD_AND_KEYFILE);
    return dlg.run(password, file);
}

//------------------------------------------------------------------------------
bool flw::dlg::password4(std::string title, std::string& password, std::string& file, Fl_Window* parent) {
    _DlgPassword dlg(title.c_str(), parent, _DlgPassword::TYPE::CONFIRM_PASSWORD_AND_KEYFILE);
    return dlg.run(password, file);
}

//------------------------------------------------------------------------------
int flw::dlg::select(std::string title, const StringVector& list, int selected_row, Fl_Window* parent, bool fixed_font, int W, int H) {
    _DlgSelect dlg(title.c_str(), parent, list, selected_row, "", fixed_font, W, H);
    return dlg.run();
}

//------------------------------------------------------------------------------
int flw::dlg::select(std::string title, const StringVector& list, const std::string& selected_row, Fl_Window* parent, bool fixed_font, int W, int H) {
    _DlgSelect dlg(title.c_str(), parent, list, 0, selected_row, fixed_font, W, H);
    return dlg.run();
}

//------------------------------------------------------------------------------
void flw::dlg::text(std::string title, const std::string& text, Fl_Window* parent, int W, int H) {
    _DlgText dlg(title.c_str(), text.c_str(), false, parent, W, H);
    dlg.run();
}

//------------------------------------------------------------------------------
bool flw::dlg::text_edit(std::string title, std::string& text, Fl_Window* parent, int W, int H) {
    auto dlg = _DlgText(title.c_str(), text.c_str(), true, parent, W, H);
    auto res = dlg.run();

    if (res == nullptr) {
        return false;
    }

    text = res;
    free(res);
    return true;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// To disable progress set min and max to 0.0
//
flw::dlg::AbortDialog::AbortDialog(double min, double max) : Fl_Double_Window(0, 0, 0, 0, "Working...") {
    _button   = new Fl_Button(0, 0, 0, 0, "Press To Abort");
    _progress = new Fl_Hor_Fill_Slider(0, 0, 0, 0);
    _abort    = false;
    _last     = 0;

    add(_button);
    add(_progress);

    auto W = flw::PREF_FONTSIZE * 32;
    auto H = 0;

    if (min < max && fabs(max - min) > 0.001) {
        _progress->range(min, max);
        _progress->value(min);
        H = flw::PREF_FONTSIZE * 6 + 12;
    }
    else {
        _progress->hide();
        H = flw::PREF_FONTSIZE * 4 + 8;
    }

    _button->callback(AbortDialog::Callback, this);
    _button->labelfont(flw::PREF_FONT);
    _button->labelsize(flw::PREF_FONTSIZE);
    _progress->color(FL_SELECTION_COLOR);

    size(W, H);
    size_range(W, H);
    FLW_PRINT(W,H, w(), h())
    callback(AbortDialog::Callback, this);
    set_modal();
}

//------------------------------------------------------------------------------
void flw::dlg::AbortDialog::Callback(Fl_Widget* w, void* o) {
    auto dlg = (AbortDialog*) o;

    if (w == dlg->_button) {
        dlg->_abort = true;
    }
}

//------------------------------------------------------------------------------
bool flw::dlg::AbortDialog::check(int milliseconds) {
    auto now = flw::util::time_milli();

    if (now - _last > milliseconds) {
        _last = now;
        Fl::check();
    }

    return _abort;
}

//------------------------------------------------------------------------------
bool flw::dlg::AbortDialog::check(double value, double min, double max, int milliseconds) {
    auto now = flw::util::time_milli();

    if (now - _last > milliseconds) {
        _progress->value(value);
        _progress->range(min, max);
        _last = now;
        Fl::check();
    }

    return _abort;
}

//------------------------------------------------------------------------------
void flw::dlg::AbortDialog::range(double min, double max) {
    _progress->range(min, max);
}

//------------------------------------------------------------------
void flw::dlg::AbortDialog::resize(int X, int Y, int W, int H) {
    Fl_Double_Window::resize(X, Y, W, H);

    if (_progress->visible() != 0) {
        _button->resize(4, 4, W - 8, H - flw::PREF_FONTSIZE * 2 - 12);
        _progress->resize(4, H - flw::PREF_FONTSIZE * 2 - 4, W - 8, flw::PREF_FONTSIZE * 2);
    }
    else {
        _button->resize(4, 4, W - 8, H - 8);
    }
}

//------------------------------------------------------------------------------
void flw::dlg::AbortDialog::show(const std::string& label, Fl_Window* parent) {
    _abort = false;
    _last  = 0;

    _button->copy_label(label.c_str());
    flw::util::center_window(this, parent);
    Fl_Double_Window::show();
    Fl::flush();
}

//------------------------------------------------------------------------------
void flw::dlg::AbortDialog::value(double value) {
    _progress->value(value);
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
flw::dlg::WorkDialog::WorkDialog(const char* title, Fl_Window* parent, bool cancel, bool pause, int W, int H) : Fl_Double_Window(0, 0, W * flw::PREF_FONTSIZE, H * flw::PREF_FONTSIZE) {
    end();

    _cancel = new Fl_Button(0, 0, 0, 0, "Cancel");
    _pause  = new Fl_Toggle_Button(0, 0, 0, 0, "Pause");
    _label  = new Fl_Hold_Browser(0, 0, 0, 0);
    _ret    = true;
    _last   = 0.0;

    add(_label);
    add(_pause);
    add(_cancel);

    _cancel->callback(WorkDialog::Callback, this);
    _label->box(FL_BORDER_BOX);
    _label->textfont(flw::PREF_FONT);
    _label->textsize(flw::PREF_FONTSIZE);
    _pause->callback(WorkDialog::Callback, this);

    if (cancel == false) {
        _cancel->deactivate();
    }

    if (pause == false) {
        _pause->deactivate();
    }

    flw::theme::labelfont(this);
    callback(WorkDialog::Callback, this);
    copy_label(title);
    size_range(flw::PREF_FONTSIZE * 24, flw::PREF_FONTSIZE * 12);
    set_modal();
    resizable(this);
    flw::util::center_window(this, parent);
    show();
}

//------------------------------------------------------------------------------
void flw::dlg::WorkDialog::Callback(Fl_Widget* w, void* o) {
    auto dlg = (flw::dlg::WorkDialog*) o;

    if (w == dlg) {
    }
    else if (w == dlg->_cancel) {
        dlg->_ret = false;
    }
    else if (w == dlg->_pause) {
        bool cancel = dlg->_cancel->active();
        dlg->_cancel->deactivate();
        dlg->_pause->label("C&ontinue");

        while (dlg->_pause->value() != 0) {
            flw::util::time_sleep(10);
            Fl::check();
        }

        dlg->_pause->label("&Pause");

        if (cancel) {
            dlg->_cancel->activate();
        }
    }
}

//------------------------------------------------------------------------------
void flw::dlg::WorkDialog::resize(int X, int Y, int W, int H) {
    Fl_Double_Window::resize(X, Y, W, H);

    _label->resize(4, 4, W - 8, H - flw::PREF_FONTSIZE * 2 - 16);
    _pause->resize(W - flw::PREF_FONTSIZE * 16 - 8, H - flw::PREF_FONTSIZE * 2 - 4, flw::PREF_FONTSIZE * 8, flw::PREF_FONTSIZE * 2);
    _cancel->resize(W - flw::PREF_FONTSIZE * 8 - 4, H - flw::PREF_FONTSIZE * 2 - 4, flw::PREF_FONTSIZE * 8, flw::PREF_FONTSIZE * 2);
}

//------------------------------------------------------------------------------
bool flw::dlg::WorkDialog::run(double update_time, const StringVector& messages) {
    auto now = flw::util::time();

    if (now - _last > update_time) {
        _label->clear();

        for (auto& s : messages) {
            _label->add(s.c_str());
        }

        _last = now;
        Fl::check();
        Fl::flush();
    }

    return _ret;
}

//------------------------------------------------------------------------------
bool flw::dlg::WorkDialog::run(double update_time, const std::string& message) {
    auto now = flw::util::time();

    if (now - _last > update_time) {
        _label->clear();
        _label->add(message.c_str());
        _last = now;
        Fl::check();
        Fl::flush();
    }

    return _ret;
}



namespace flw {
    namespace dlg {
        static std::vector<char*>   _FONTDIALOG_NAMES;
        static const std::string    _FONTDIALOG_LABEL = R"(
ABCDEFGHIJKLMNOPQRSTUVWXYZ /0123456789
abcdefghijklmnopqrstuvwxyz £©µÀÆÖÞßéöÿ
–—‘“”„†•…‰™œŠŸž€ ΑΒΓΔΩαβγδω АБВГДабвгд
∀∂∈ℝ∧∪≡∞ ↑↗↨↻⇣ ┐┼╔╘░►☺♀ ﬁ�⑀₂ἠḂӥẄɐː⍎אԱა

Hello world, Καλημέρα κόσμε, コンニチハ

math: ∮ E⋅da = Q,  n → ∞, ∑ f(i) 2H₂ + O₂ ⇌ 2H₂O, R = 4.7 kΩ
korean: 구두의
greek: Οὐχὶ ταὐτὰ παρίσταταί μοι γιγνώσκειν
russian: Зарегистрируйтесь сейчас
thai: แผ่นดินฮั่นเสื่อมโทรมแสนสังเวช
amharic: ሰማይ አይታረስ ንጉሥ አይከሰስ
braille: ⡌⠁⠧⠑ ⠼⠁⠒  ⡍⠜⠇⠑⠹⠰⠎ ⡣⠕⠌

“There is nothing else than now.
There is neither yesterday, certainly,
nor is there any tomorrow.
How old must you be before you know that?
There is only now, and if now is only two days,
then two days is your life and everything in it will be in proportion.
This is how you live a life in two days.
And if you stop complaining and asking for what you never will get,
you will have a good life.
A good life is not measured by any biblical span.”
― Ernest Hemingway, For Whom the Bell Tolls
)";

        //----------------------------------------------------------------------
        class _FontDialogLabel : public Fl_Box {
        public:
            int font;
            int size;

            //------------------------------------------------------------------
            _FontDialogLabel(int x, int y, int w, int h) : Fl_Box(x, y, w, h, _FONTDIALOG_LABEL.c_str()) {
                font = FL_HELVETICA;
                size = 14;

                align(FL_ALIGN_TOP | FL_ALIGN_LEFT | FL_ALIGN_INSIDE | FL_ALIGN_CLIP);
                box(FL_BORDER_BOX);
                color(FL_BACKGROUND2_COLOR);
            }

            //------------------------------------------------------------------
            void draw() override {
                draw_box();
                fl_font((Fl_Font) font, size);
                fl_color(FL_FOREGROUND_COLOR);
                fl_draw(label(), x() + 3, y() + 3, w() - 6, h() - 6, align());
            }
        };
    }
}

//------------------------------------------------------------------------------
flw::dlg::FontDialog::FontDialog(Fl_Font font, Fl_Fontsize fontsize, const std::string& label) :
Fl_Double_Window(0, 0, flw::PREF_FONTSIZE * 60, flw::PREF_FONTSIZE * 36) {
    _create(font, "", fontsize, label);
}

//------------------------------------------------------------------------------
flw::dlg::FontDialog::FontDialog(std::string font, Fl_Fontsize fontsize, std::string label) :
Fl_Double_Window(0, 0, flw::PREF_FONTSIZE * 60, flw::PREF_FONTSIZE * 36) {
    _create(0, font, fontsize, label);
}

//------------------------------------------------------------------------------
void flw::dlg::FontDialog::_activate() {
    if (_fonts->value() == 0 || _sizes->value() == 0) {
        _select->deactivate();
    }
    else {
        _select->activate();
    }
}

//------------------------------------------------------------------------------
void flw::dlg::FontDialog::Callback(Fl_Widget* w, void* o) {
    FontDialog* dlg = (FontDialog*) o;

    if (w == dlg) {
        dlg->hide();
    }
    else if (w == dlg->_cancel) {
        dlg->hide();
    }
    else if (w == dlg->_fonts) {
        auto row = dlg->_fonts->value();

        if (row > 0) {
            ((_FontDialogLabel*) dlg->_label)->font = row - 1;
        }

        dlg->_activate();
        Fl::redraw();
    }
    else if (w == dlg->_select) {
        auto row1 = dlg->_fonts->value();
        auto row2 = dlg->_sizes->value();

        if (row1 > 0 && row2 > 0) {
            row1--;

            dlg->_fontname = ScrollBrowser::RemoveFormat(_FONTDIALOG_NAMES[row1]);
            dlg->_font     = row1;
            dlg->_fontsize = row2 + 5;
            dlg->_ret      = true;

            dlg->hide();
        }
    }
    else if (w == dlg->_sizes) {
        auto row = dlg->_sizes->value();

        if (row > 0) {
            ((_FontDialogLabel*) dlg->_label)->size = row + 5;
        }

        dlg->_activate();
        Fl::redraw();
    }
}

//------------------------------------------------------------------------------
void flw::dlg::FontDialog::_create(Fl_Font font, std::string fontname, Fl_Fontsize fontsize, std::string label) {
    end();

    _cancel   = new Fl_Button(0, 0, 0, 0, "&Cancel");
    _fonts    = new flw::ScrollBrowser(12);
    _label    = new flw::dlg::_FontDialogLabel(0, 0, 0, 0);
    _select   = new Fl_Button(0, 0, 0, 0, "&Select");
    _sizes    = new flw::ScrollBrowser(6);
    _font     = -1;
    _fontsize = -1;
    _ret      = false;

    add(_sizes);
    add(_fonts);
    add(_select);
    add(_cancel);
    add(_label);

    _cancel->callback(flw::dlg::FontDialog::Callback, this);
    _cancel->labelfont(flw::PREF_FONT);
    _cancel->labelsize(flw::PREF_FONTSIZE);
    _fonts->callback(flw::dlg::FontDialog::Callback, this);
    _fonts->textsize(flw::PREF_FONTSIZE);
    _fonts->when(FL_WHEN_CHANGED);
    ((_FontDialogLabel*) _label)->font = font;
    ((_FontDialogLabel*) _label)->size = fontsize;
    _select->callback(flw::dlg::FontDialog::Callback, this);
    _select->labelfont(flw::PREF_FONT);
    _select->labelsize(flw::PREF_FONTSIZE);
    _sizes->callback(flw::dlg::FontDialog::Callback, this);
    _sizes->textsize(flw::PREF_FONTSIZE);
    _sizes->when(FL_WHEN_CHANGED);

    size(w(), h());
    FontDialog::LoadFonts();

    for (auto name : _FONTDIALOG_NAMES) {
        _fonts->add(name);
    }

    for (auto f = 6; f <= 72; f++) {
        char buf[50];
        snprintf(buf, 50, "@r%2d  ", f);
        _sizes->add(buf);
    }

    if (fontsize >= 6 && fontsize <= 72) {
        _sizes->value(fontsize - 5);
        _sizes->middleline(fontsize - 5);
        ((_FontDialogLabel*) _label)->font = fontsize;
    }
    else {
        _sizes->value(14 - 5);
        _sizes->middleline(14 - 5);
        ((_FontDialogLabel*) _label)->font = 14;
    }

    if (fontname != "") {
        _select_name(fontname);
    }
    else if (font >= 0 && font < _fonts->size()) {
        _fonts->value(font + 1);
        _fonts->middleline(font + 1);
        ((_FontDialogLabel*) _label)->font = font;
    }
    else {
        _fonts->value(1);
        _fonts->middleline(1);
    }

    resizable(this);
    copy_label(label.c_str());
    callback(flw::dlg::FontDialog::Callback, this);
    size_range(flw::PREF_FONTSIZE * 38, flw::PREF_FONTSIZE * 12);
    set_modal();
    _fonts->take_focus();
}

//------------------------------------------------------------------------------
void flw::dlg::FontDialog::DeleteFonts() {
    for (auto f : _FONTDIALOG_NAMES) {
        free(f);
    }

    _FONTDIALOG_NAMES.clear();
}

//------------------------------------------------------------------------------
int flw::dlg::FontDialog::LoadFont(std::string requested_font) {
    FontDialog::LoadFonts();

    auto count = 0;

    for (auto font : _FONTDIALOG_NAMES) {
        auto font2 = ScrollBrowser::RemoveFormat(font);

        if (requested_font == font2) {
            return count;
        }

        count++;
    }

    return -1;
}

//------------------------------------------------------------------------------
// Load fonts only once in the program
//
void flw::dlg::FontDialog::LoadFonts(bool iso8859_only) {
    if (_FONTDIALOG_NAMES.size() == 0) {
        auto fonts = Fl::set_fonts(iso8859_only ? nullptr : "-*");

        for (int f = 0; f < fonts; f++) {
            auto attr  = 0;
            auto name1 = Fl::get_font_name((Fl_Font) f, &attr);
            auto name2 = std::string();

            if (attr & FL_BOLD) {
                name2 = std::string("@b");
            }

            if (attr & FL_ITALIC) {
                name2 += std::string("@i");
            }

            name2 += std::string("@.");
            name2 += name1;
            _FONTDIALOG_NAMES.push_back(strdup(name2.c_str()));
            // printf("%3d - %-40s - %-40s\n", f, name1, name2.c_str()); fflush(stdout);
        }
    }
}

//------------------------------------------------------------------------------
void flw::dlg::FontDialog::resize(int X, int Y, int W, int H) {
    int fs = flw::PREF_FONTSIZE;

    Fl_Double_Window::resize(X, Y, W, H);

    _fonts->resize  (4,                 4,                  fs * 25,            H - fs * 2 - 16);
    _sizes->resize  (fs * 25 + 8,       4,                  fs * 6,             H - fs * 2 - 16);
    _label->resize  (fs * 31 + 12,      4,                  W - fs * 31 - 16,   H - fs * 2 - 16);
    _cancel->resize (W - fs * 16 - 8,   H - fs * 2 - 4,     fs * 8,             fs * 2);
    _select->resize (W - fs * 8 - 4,    H - fs * 2 - 4,     fs * 8,             fs * 2);
}

//------------------------------------------------------------------------------
bool flw::dlg::FontDialog::run(Fl_Window* parent) {
    _ret = false;

    _activate();
    flw::util::center_window(this, parent);
    show();

    while (visible() != 0) {
        Fl::wait();
        Fl::flush();
    }

    return _ret;
}

//------------------------------------------------------------------------------
void flw::dlg::FontDialog::_select_name(std::string fontname) {
    auto count = 1;

    for (auto font : _FONTDIALOG_NAMES) {
        auto font_without_style = ScrollBrowser::RemoveFormat(font);

        if (fontname == font_without_style) {
            _fonts->value(count);
            _fonts->middleline(count);
            ((_FontDialogLabel*) _label)->font = count - 1;
            return;
        }

        count++;
    }

    _fonts->value(1);
    ((_FontDialogLabel*) _label)->font = 0;
}



namespace flw {
    static int _FLW_GRID_STRING_SIZE = 1000;
}

//------------------------------------------------------------------------------
flw::Grid::Grid(int rows, int cols, int X, int Y, int W, int H, const char* l) : flw::TableEditor(X, Y, W, H, l) {
    rows    = rows < 1 ? 1 : rows;
    cols    = cols < 1 ? 1 : cols;
    _buffer = (char*) calloc(_FLW_GRID_STRING_SIZE + 1, 1);

    Grid::size(rows, cols);
    lines(true, true);
    header(true, true);
    select_mode(TableDisplay::SELECT::CELL);
    active_cell(1, 1);
    resize_column_width(true);
    expand_last_column(true);
}

//------------------------------------------------------------------------------
flw::Grid::~Grid() {
    free(_buffer);
}

//------------------------------------------------------------------------------
Fl_Align flw::Grid::cell_align(int row, int col) {
    return (Fl_Align) _get_int(_cell_align, row, col, TableEditor::cell_align(row, col));
}

//------------------------------------------------------------------------------
void flw::Grid::cell_align(int row, int col, Fl_Align align) {
    _set_int(_cell_align, row, col, (int) align);
}

//------------------------------------------------------------------------------
flw::StringVector flw::Grid::cell_choice(int row, int col) {
    _cell_choices.clear();
    auto choices = _get_string(_cell_choice, row, col);

    if (*choices != 0) {
        auto tmp   = strdup(choices);
        auto start = tmp;
        auto iter  = tmp;

        while (*iter) {
            if (*iter == '\t') {
                *iter = 0;
                _cell_choices.push_back(start);
                start = iter + 1;
            }

            iter++;
        }

        free(tmp);
    }

    return _cell_choices;
}

//------------------------------------------------------------------------------
void flw::Grid::cell_choice(int row, int col, const char* value) {
    _set_string(_cell_choice, row, col, value);
}

//------------------------------------------------------------------------------
Fl_Color flw::Grid::cell_color(int row, int col) {
    return (Fl_Color) _get_int(_cell_color, row, col, TableEditor::cell_color(row, col));
}

//------------------------------------------------------------------------------
void flw::Grid::cell_color(int row, int col, Fl_Color color) {
    _set_int(_cell_color, row, col, (int) color);
}

//------------------------------------------------------------------------------
bool flw::Grid::cell_edit(int row, int col) {
    return (bool) _get_int(_cell_edit, row, col, 0);
}

//------------------------------------------------------------------------------
void flw::Grid::cell_edit(int row, int col, bool value) {
    _set_int(_cell_edit, row, col, (int) value);
}

//------------------------------------------------------------------------------
flw::TableEditor::FORMAT flw::Grid::cell_format(int row, int col) {
    return (TableEditor::FORMAT) _get_int(_cell_format, row, col, (int) TableEditor::FORMAT::DEFAULT);
}

//------------------------------------------------------------------------------
void flw::Grid::cell_format(int row, int col, TableEditor::FORMAT value) {
    _set_int(_cell_format, row, col, (int) value);
}

//------------------------------------------------------------------------------
flw::TableEditor::REND flw::Grid::cell_rend(int row, int col) {
    return (TableEditor::REND) _get_int(_cell_rend, row, col, (int) TableEditor::REND::TEXT);
}

//------------------------------------------------------------------------------
void flw::Grid::cell_rend(int row, int col, TableEditor::REND rend) {
    _set_int(_cell_rend, row, col, (int) rend);
}

//------------------------------------------------------------------------------
Fl_Color flw::Grid::cell_textcolor(int row, int col) {
    return (Fl_Color) _get_int(_cell_textcolor, row, col, TableEditor::cell_textcolor(row, col));
}

//------------------------------------------------------------------------------
void flw::Grid::cell_textcolor(int row, int col, Fl_Color color) {
    _set_int(_cell_textcolor, row, col, (int) color);
}

//------------------------------------------------------------------------------
const char* flw::Grid::cell_value(int row, int col) {
    return _get_string(_cell_value, row, col);
}

//------------------------------------------------------------------------------
bool flw::Grid::cell_value(int row, int col, const char* value) {
    _set_string(_cell_value, row, col, value);
    return true;
}

//------------------------------------------------------------------------------
void flw::Grid::cell_value2(int row, int col, const char* format, ...) {
    va_list args;
    int     n = 0;

    va_start(args, format);
    n = vsnprintf(_buffer, _FLW_GRID_STRING_SIZE, format, args);
    va_end(args);

    if (n < 1 || n >= _FLW_GRID_STRING_SIZE) {
        *_buffer = 0;
    }

    cell_value(row, col, _buffer);
}

//------------------------------------------------------------------------------
int flw::Grid::cell_width(int col) {
    int W = _get_int(_cell_width, 0, col, 80);
    return W >= 10 ? W : TableEditor::cell_width(col);
}

//------------------------------------------------------------------------------
void flw::Grid::cell_width(int col, int width) {
    _set_int(_cell_width, 0, col, (int) width);
}

//------------------------------------------------------------------------------
int flw::Grid::_get_int(StringMap& map, int row, int col, int def) {
    auto value = _get_string(map, row, col);
    return *value ? atoi(value) : def;
}

//------------------------------------------------------------------------------
const char* flw::Grid::_get_key(int row, int col) {
    sprintf(_key, "r%dc%d", row, col);
    return _key;
}

//------------------------------------------------------------------------------
const char* flw::Grid::_get_string(StringMap& map, int row, int col, const char* def) {
    auto search = map.find(_get_key(row, col));
    return search != map.end() ? search->second.c_str() : def;
}

//------------------------------------------------------------------------------
void flw::Grid::_set_int(StringMap& map, int row, int col, int value) {
    char value2[100];
    sprintf(value2, "%d", value);
    map[_get_key(row, col)] = value2;
}

//------------------------------------------------------------------------------
void flw::Grid::_set_string(StringMap& map, int row, int col, const char* value) {
    map[_get_key(row, col)] = value ? value : "";
}

//------------------------------------------------------------------------------
void flw::Grid::size(int rows, int cols) {
    if (rows > 0 && cols > 0) {
        TableEditor::size(rows, cols);
    }
}



namespace flw {
    //------------------------------------------------------------------------------
    // Private struct to store layout data and widgets
    //
    struct _GridGroupChild {
        Fl_Widget* widget;
        int x;
        int y;
        int w;
        int h;

        _GridGroupChild() {
            widget = nullptr;
            x = 0;
            y = 0;
            w = 0;
            h = 0;
        }
    };
}

//------------------------------------------------------------------------------
flw::GridGroup::GridGroup(int X, int Y, int W, int H, const char* l) : Fl_Group(X, Y, W, H, l) {
    end();
    clip_children(1);

    _size = labelsize();

    for (int f = 0; f < flw::GridGroup::MAX_WIDGETS; f++) {
        _widgets[f] = new _GridGroupChild();
    }
}

//------------------------------------------------------------------------------
flw::GridGroup::~GridGroup() {
    for (int f = 0; f < flw::GridGroup::MAX_WIDGETS; f++) {
        delete _widgets[f];
    }
}

//------------------------------------------------------------------------------
void flw::GridGroup::add(Fl_Widget* widget, int x, int y, int w, int h) {
    for (int f = 0; f < flw::GridGroup::MAX_WIDGETS; f++) {
        if (_widgets[f]->widget == nullptr) {
            Fl_Group::add(widget);

            _widgets[f]->widget = widget;
            _widgets[f]->x      = x;
            _widgets[f]->y      = y;
            _widgets[f]->w      = w;
            _widgets[f]->h      = h;

            return;
        }
    }

    #ifdef DEBUG
        fprintf(stderr, "error: flw::GridGroup::add too many widgets\n");
    #endif
}

//------------------------------------------------------------------------------
void flw::GridGroup::clear() {
    for (int f = 0; f < flw::GridGroup::MAX_WIDGETS; f++) {
        _widgets[f]->widget = nullptr;
        _widgets[f]->x      = 0;
        _widgets[f]->y      = 0;
        _widgets[f]->w      = 0;
        _widgets[f]->h      = 0;
    }

    Fl_Group::clear();
}

//------------------------------------------------------------------------------
void flw::GridGroup::remove(Fl_Widget* widget) {
    for (int f = 0; f < flw::GridGroup::MAX_WIDGETS; f++) {
        if (_widgets[f]->widget == widget) {
            Fl_Group::remove(widget);

            _widgets[f]->widget = nullptr;
            _widgets[f]->x      = 0;
            _widgets[f]->y      = 0;
            _widgets[f]->w      = 0;
            _widgets[f]->h      = 0;

            return;
        }
    }

    #ifdef DEBUG
        fprintf(stderr, "error: flw::GridGroup::remove can't find widget\n");
    #endif
}

//------------------------------------------------------------------------------
void flw::GridGroup::resize() {
    resize(x(), y(), w(), h());
    redraw();
}


//------------------------------------------------------------------------------
void flw::GridGroup::resize(int X, int Y, int W, int H) {
    Fl_Widget::resize(X, Y, W, H);

    if (W == 0 || H == 0) {
        return;
    }

    #ifdef DEBUG
        // fprintf(stderr, "\n%-20s (x=%4d y=%4d)  (w=%4d h=%4d)  (x2=%4d y2=%4d)\n", "GridGroup: ", X, Y, W, H, X + W, Y + H);
        // fprintf(stderr, "%-20s (%4s, %4s)  (%4s, %4s)  (%4s, %4s) <=> (%3s, %3s, %3s, %3s)\n", "", "x", "y", "w", "h", "x+w", "y+h", "lx", "ly", "lw", "lh");
    #endif

    for (int f = 0; f < flw::GridGroup::MAX_WIDGETS; f++) {
        _GridGroupChild* widget = _widgets[f];

        if (widget->widget) {
            int widget_x  = 0;
            int widget_x2 = 0;
            int widget_y  = 0;
            int widget_y2 = 0;
            int widget_w  = 0;
            int widget_h  = 0;

            if (widget->x >= 0) {
                widget_x = X + widget->x * _size;
            }
            else {
                widget_x = X + W + widget->x * _size;
            }

            if (widget->y >= 0) {
                widget_y = Y + widget->y * _size;
            }
            else {
                widget_y = Y + H + widget->y * _size;
            }

            if (widget->w == 0) {
                widget_x2 = X + W;
            }
            else if (widget->w > 0) {
                widget_x2 = widget_x + widget->w * _size;
            }
            else {
                widget_x2 = X + W + widget->w * _size;
            }

            if (widget->h == 0) {
                widget_y2 = Y + H;
            }
            else if (widget->h > 0) {
                widget_y2 = widget_y + widget->h * _size;
            }
            else {
                widget_y2 = Y + H + widget->h * _size;
            }

            widget_w = widget_x2 - widget_x;
            widget_h = widget_y2 - widget_y;

            // if (widget_w >= _size && widget_h >= _size && widget_x + widget_w >= _size && widget_y + widget_h >= _size) {
            if (widget_w >= 0 && widget_h >= 0) {
                widget->widget->resize(widget_x, widget_y, widget_w, widget_h);
            }
            else {
                widget->widget->resize(widget_x, widget_y, 0, 0);
            }

            #ifdef DEBUG
                // fprintf(stderr, "%-20s (%4d, %4d)  (%4d, %4d)  (%4d, %4d) <=> (%3d, %3d, %3d, %3d)\n", widget->widget->label() ? widget->widget->label() : "", widget_x, widget_y, widget_w, widget_h, widget_x + widget_w, widget_y + widget_h, widget->x, widget->y, widget->w, widget->h);
            #endif
        }
    }
}



namespace flw {
    static const unsigned char _LCDNUMBER_SEGMENTS[20] = {
        0x00,
        0x7E, //0
        0x30, //1
        0x6D, //2
        0x79, //3
        0x33, //4
        0x5B, //5
        0x5F, //6
        0x70, //7
        0x7F, //8
        0x7B, //9
        0x77, //A
        0x1F, //B
        0x4E, //C
        0x3D, //D
        0x4F, //E
        0x47, //F
        0x01, //-
        0xA0, //.
        0x00, // Repeat the first one.
    };
}

//------------------------------------------------------------------------------
flw::LcdNumber::LcdNumber(int x, int y, int w, int h, const char *l) : Fl_Box(x, y, w, h, l) {
    _align     = FL_ALIGN_LEFT;
    _dot_size  = 6;
    _seg_color = FL_FOREGROUND_COLOR;
    _seg_gap   = 1;
    _thick     = 4;
    _unit_gap  = 4;
    _unit_h    = 42;
    _unit_w    = 21;

    *_value = 0;
    value("0");
    box(FL_NO_BOX);
}

//------------------------------------------------------------------------------
void flw::LcdNumber::draw() {
    Fl_Box::draw();

    int sw   = 0;
    int tx   = x();
    int ty   = y();
    int tw   = w();
    int th   = h();

    for (size_t f = 0; f < strlen(_value); f++) {
        if (_value[f] == 0x12) {
            sw += _dot_size + _unit_gap;
        }
        else {
            sw += _unit_w + _unit_gap;
        }
    }

    switch(_align) {
        case FL_ALIGN_RIGHT:
            tx += (tw - sw) - _unit_w;
            break;

        case FL_ALIGN_CENTER:
            tx += (tw / 2 - sw / 2);
            break;

        default:
            tx += _unit_w;
            break;
    }

    ty += ((th - _unit_h) >> 1);
    fl_color(active_r() ? _seg_color : fl_inactive(_seg_color));
    int xx = tx;

    for(int i = 0; _value[i]; i++) {
        if (_value[i] == 0x12) {
            fl_rectf(xx, ty + _unit_h + 1 - _dot_size, _dot_size, _dot_size);
            xx += (_dot_size + _unit_gap);
        }
        else {
            _draw_seg(_LCDNUMBER_SEGMENTS[(int) _value[i]], xx, ty, _unit_w, _unit_h);
            xx += (_unit_w + _unit_gap);
        }
    }
}

//------------------------------------------------------------------------------
void flw::LcdNumber::_draw_seg(uchar a, int x, int y, int w, int h) {
    int x0, y0, x1, y1, x2, y2, x3, y3;
    int h2     = h >> 1;
    int thick2 = _thick >> 1;

    if (a & 0x40) {
        x0 = x + _seg_gap;
        y0 = y;
        x1 = x0 + _thick;
        y1 = y0 + _thick;
        x2 = x + w - _thick - _seg_gap;
        y2 = y1;
        x3 = x2 + _thick;
        y3 = y0;

        fl_polygon(x0, y0, x1, y1, x2, y2, x3, y3);
    }

    if (a & 0x20) {
        x0 = x + w;
        y0 = y + _seg_gap;
        x1 = x0 - _thick;
        y1 = y0 + _thick;
        x2 = x1;
        y2 = y + h2 - thick2 - _seg_gap;
        x3 = x0;
        y3 = y2 + thick2;

        fl_polygon(x0, y0, x1, y1, x2, y2, x3, y3);
    }

    if (a & 0x10) {
        x0 = x + w;
        y0 = y + h2 + _seg_gap;
        x1 = x0 - _thick;
        y1 = y0 + thick2;
        x2 = x1;
        y2 = y + h - _thick - _seg_gap;
        x3 = x0;
        y3 = y2 + _thick;

        fl_polygon(x0, y0, x1, y1, x2, y2, x3, y3);
    }

    if (a & 0x08) {
        x0 = x + _seg_gap;
        y0 = y + h;
        x1 = x0 + _thick;
        y1 = y0 - _thick;
        x2 = x + w - _thick - _seg_gap;
        y2 = y1;
        x3 = x2 + _thick;
        y3 = y0;

        fl_polygon(x0, y0, x1, y1, x2, y2, x3, y3);
    }

    if (a & 0x04) {
        x0 = x;
        y0 = y + h2 + _seg_gap;
        x1 = x0 + _thick;
        y1 = y0 + thick2;
        x2 = x1;
        y2 = y + h - _thick - _seg_gap;
        x3 = x0;
        y3 = y2 + _thick;

        fl_polygon(x0, y0, x1, y1, x2, y2, x3, y3);
    }

    if (a & 0x02) {
        x0 = x;
        y0 = y + _seg_gap;
        x1 = x0 + _thick;
        y1 = y0 + _thick;
        x2 = x1;
        y2 = y + h2 - thick2 - _seg_gap;
        x3 = x0;
        y3 = y2 + thick2;

        fl_polygon(x0, y0, x1, y1, x2, y2, x3, y3);
    }

    if (a & 0x01) {
        x0 = x + _seg_gap;
        y0 = y + h2;
        x1 = x0 + _thick;
        y1 = y0 - thick2;
        x2 = x1;
        y2 = y1 + _thick;

        fl_polygon(x0, y0, x1, y1, x2, y2);

        x0 = x1;
        y0 = y1;
        x1 = x0 + w - _thick * 2 - _seg_gap * 2;
        y1 = y0;
        x2 = x1;
        y2 = y1 + _thick;
        x3 = x0;
        y3 = y2;

        fl_polygon(x0, y0, x1, y1, x2, y2, x3, y3);

        x0 = x + w - _seg_gap;
        y0 = y + h2;
        x1 = x0 - _thick;
        y1 = y0 - thick2;
        x2 = x1;
        y2 = y1 + _thick;

        fl_polygon(x0, y0, x1, y1, x2, y2);
    }
}

//------------------------------------------------------------------------------
void flw::LcdNumber::value(const char *value) {
    size_t l = value ? strlen(value) : 0;

    if (l && l < 100) {
        for (size_t i = 0; i < l; i++) {
            if (value[i] >= 0x30 && value[i] <= 0x39) {
                _value[i] = value[i] - 0x2F;
            }
            else if (value[i] >= 0x41 && value[i] <= 0x46) {
                _value[i] = value[i] - 0x36;
            }
            else if (value[i] >= 0x61 && value[i] <= 0x66) {
                _value[i] = value[i] - 0x56;
            }
            else if (value[i] == '-') {
                _value[i] = 0x11;
            }
            else if (value[i] == ' ') {
                _value[i] = 0x13;
            }
            else if (value[i] == '.') {
                _value[i] = 0x12;
            }
            else if (value[i] == ':') {
                _value[i] = 0x12;
            }
            else {
                _value[i] = 0x13;
            }

            _value[i + 1] = 0;
        }
    }
    else {
        *_value = 0;
    }

    Fl::redraw();
}



#if FL_MINOR_VERSION == 4
    #define _LOGDISPLAY_ATTRIBUTE 0, 0
#else
    #define _LOGDISPLAY_ATTRIBUTE 0
#endif

namespace flw {
    Fl_Text_Display::Style_Table_Entry _LOGDISPLAY_STYLE[] = {
        { FL_FOREGROUND_COLOR,          FL_COURIER,             14, _LOGDISPLAY_ATTRIBUTE }, // FOREGROUND
        { fl_rgb_color(115, 115, 115),  FL_COURIER,             14, _LOGDISPLAY_ATTRIBUTE }, // GRAY
        { fl_rgb_color(255, 64, 64),    FL_COURIER,             14, _LOGDISPLAY_ATTRIBUTE }, // RED
        { fl_rgb_color(0, 230, 0),      FL_COURIER,             14, _LOGDISPLAY_ATTRIBUTE }, // GREEN
        { fl_rgb_color(0, 168, 255),    FL_COURIER,             14, _LOGDISPLAY_ATTRIBUTE }, // BLUE
        { fl_rgb_color(192, 0, 0),      FL_COURIER,             14, _LOGDISPLAY_ATTRIBUTE }, // DARK_RED
        { fl_rgb_color(0, 128, 0),      FL_COURIER,             14, _LOGDISPLAY_ATTRIBUTE }, // DARK_GREEN
        { fl_rgb_color(0, 0, 255),      FL_COURIER,             14, _LOGDISPLAY_ATTRIBUTE }, // DARK_BLUE
        { fl_rgb_color(255, 128, 0),    FL_COURIER,             14, _LOGDISPLAY_ATTRIBUTE }, // ORANGE
        { FL_MAGENTA,                   FL_COURIER,             14, _LOGDISPLAY_ATTRIBUTE }, // MAGENTA
        { FL_YELLOW,                    FL_COURIER,             14, _LOGDISPLAY_ATTRIBUTE }, // YELLOW
        { FL_CYAN,                      FL_COURIER,             14, _LOGDISPLAY_ATTRIBUTE }, // CYAN
        { FL_DARK_MAGENTA,              FL_COURIER,             14, _LOGDISPLAY_ATTRIBUTE }, // DARK_MAGENTA
        { FL_DARK_YELLOW,               FL_COURIER,             14, _LOGDISPLAY_ATTRIBUTE }, // DARK_YELLOW
        { FL_DARK_CYAN,                 FL_COURIER,             14, _LOGDISPLAY_ATTRIBUTE }, // DARK_CYAN
        { FL_BLACK,                     FL_COURIER,             14, _LOGDISPLAY_ATTRIBUTE }, // BLACK
        { FL_WHITE,                     FL_COURIER,             14, _LOGDISPLAY_ATTRIBUTE }, // WHITE

        { FL_FOREGROUND_COLOR,          FL_COURIER_BOLD,        14, _LOGDISPLAY_ATTRIBUTE }, // BOLD_FOREGROUND
        { fl_rgb_color(115, 115, 115),  FL_COURIER_BOLD,        14, _LOGDISPLAY_ATTRIBUTE }, // BOLD_GRAY
        { fl_rgb_color(255, 64, 64),    FL_COURIER_BOLD,        14, _LOGDISPLAY_ATTRIBUTE }, // BOLD_RED
        { fl_rgb_color(0, 230, 0),      FL_COURIER_BOLD,        14, _LOGDISPLAY_ATTRIBUTE }, // BOLD_GREEN
        { fl_rgb_color(0, 168, 255),    FL_COURIER_BOLD,        14, _LOGDISPLAY_ATTRIBUTE }, // BOLD_BLUE
        { fl_rgb_color(192, 0, 0),      FL_COURIER_BOLD,        14, _LOGDISPLAY_ATTRIBUTE }, // BOLD_DARK_RED
        { fl_rgb_color(0, 128, 0),      FL_COURIER_BOLD,        14, _LOGDISPLAY_ATTRIBUTE }, // BOLD_DARK_GREEN
        { fl_rgb_color(0, 0, 255),      FL_COURIER_BOLD,        14, _LOGDISPLAY_ATTRIBUTE }, // BOLD_DARK_BLUE
        { fl_rgb_color(255, 128, 0),    FL_COURIER_BOLD,        14, _LOGDISPLAY_ATTRIBUTE }, // BOLD_ORANGE
        { FL_MAGENTA,                   FL_COURIER_BOLD,        14, _LOGDISPLAY_ATTRIBUTE }, // BOLD_MAGENTA
        { FL_YELLOW,                    FL_COURIER_BOLD,        14, _LOGDISPLAY_ATTRIBUTE }, // BOLD_YELLOW
        { FL_CYAN,                      FL_COURIER_BOLD,        14, _LOGDISPLAY_ATTRIBUTE }, // BOLD_CYAN
        { FL_DARK_MAGENTA,              FL_COURIER_BOLD,        14, _LOGDISPLAY_ATTRIBUTE }, // BOLD_DARK_MAGENTA
        { FL_DARK_YELLOW,               FL_COURIER_BOLD,        14, _LOGDISPLAY_ATTRIBUTE }, // BOLD_DARK_YELLOW
        { FL_DARK_CYAN,                 FL_COURIER_BOLD,        14, _LOGDISPLAY_ATTRIBUTE }, // BOLD_DARK_CYAN
        { FL_BLACK,                     FL_COURIER_BOLD,        14, _LOGDISPLAY_ATTRIBUTE }, // BOLD_BLACK
        { FL_WHITE,                     FL_COURIER_BOLD,        14, _LOGDISPLAY_ATTRIBUTE }, // BOLD_WHITE

        { FL_FOREGROUND_COLOR,          FL_COURIER_ITALIC,      14, _LOGDISPLAY_ATTRIBUTE }, // ITALIC_FOREGROUND
        { fl_rgb_color(255, 64, 64),    FL_COURIER_ITALIC,      14, _LOGDISPLAY_ATTRIBUTE }, // ITALIC_RED
        { fl_rgb_color(0, 230, 0),      FL_COURIER_ITALIC,      14, _LOGDISPLAY_ATTRIBUTE }, // ITALIC_GREEN
        { fl_rgb_color(0, 168, 255),    FL_COURIER_ITALIC,      14, _LOGDISPLAY_ATTRIBUTE }, // ITALIC_BLUE
        { fl_rgb_color(255, 128, 0),    FL_COURIER_ITALIC,      14, _LOGDISPLAY_ATTRIBUTE }, // ITALIC_ORANGE
        { FL_MAGENTA,                   FL_COURIER_ITALIC,      14, _LOGDISPLAY_ATTRIBUTE }, // ITALIC_MAGENTA
        { FL_YELLOW,                    FL_COURIER_ITALIC,      14, _LOGDISPLAY_ATTRIBUTE }, // ITALIC_YELLOW
        { FL_CYAN,                      FL_COURIER_ITALIC,      14, _LOGDISPLAY_ATTRIBUTE }, // ITALIC_CYAN

        { FL_FOREGROUND_COLOR,          FL_COURIER_BOLD_ITALIC, 14, _LOGDISPLAY_ATTRIBUTE }, // BOLD_ITALIC_FOREGROUND
        { fl_rgb_color(255, 64, 64),    FL_COURIER_BOLD_ITALIC, 14, _LOGDISPLAY_ATTRIBUTE }, // BOLD_ITALIC_RED
        { fl_rgb_color(0, 230, 0),      FL_COURIER_BOLD_ITALIC, 14, _LOGDISPLAY_ATTRIBUTE }, // BOLD_ITALIC_GREEN
        { fl_rgb_color(0, 168, 255),    FL_COURIER_BOLD_ITALIC, 14, _LOGDISPLAY_ATTRIBUTE }, // BOLD_ITALIC_BLUE
        { fl_rgb_color(255, 128, 0),    FL_COURIER_BOLD_ITALIC, 14, _LOGDISPLAY_ATTRIBUTE }, // BOLD_ITALIC_ORANGE
        { FL_MAGENTA,                   FL_COURIER_BOLD_ITALIC, 14, _LOGDISPLAY_ATTRIBUTE }, // BOLD_ITALIC_MAGENTA
        { FL_YELLOW,                    FL_COURIER_BOLD_ITALIC, 14, _LOGDISPLAY_ATTRIBUTE }, // BOLD_ITALIC_YELLOW
        { FL_CYAN,                      FL_COURIER_BOLD_ITALIC, 14, _LOGDISPLAY_ATTRIBUTE }, // BOLD_ITALIC_CYAN
    };
}

//------------------------------------------------------------------------------
flw::LogDisplay::LogDisplay(int x, int y, int w, int h, const char *l) : Fl_Text_Display(x, y, w, h, l) {
    _buffer = new Fl_Text_Buffer();
    _style  = new Fl_Text_Buffer();
    _tmp    = nullptr;

    buffer(_buffer);
    labelsize(flw::PREF_FIXED_FONTSIZE);
    linenumber_align(FL_ALIGN_RIGHT);
    linenumber_bgcolor(FL_BACKGROUND_COLOR);
    linenumber_fgcolor(FL_FOREGROUND_COLOR);
    linenumber_font(PREF_FIXED_FONT);
    linenumber_format("%5d");
    linenumber_size(flw::PREF_FIXED_FONTSIZE);
    linenumber_width(flw::PREF_FIXED_FONTSIZE * 3);
    textfont(flw::PREF_FIXED_FONT);
    textsize(flw::PREF_FIXED_FONTSIZE);
    style_size(flw::PREF_FIXED_FONTSIZE);
}

//------------------------------------------------------------------------------
flw::LogDisplay::~LogDisplay() {
    buffer(nullptr);
    delete _buffer;
    delete _style;
}

//------------------------------------------------------------------------------
void flw::LogDisplay::color_word(const char* string, const char* word, flw::LogDisplay::COLOR color) {
    auto pos   = 0;
    auto index = 0;
    auto len   = strlen(word);

    while (str_index(string + pos, word, &index) == 1) {
        style(index + pos, index + pos + len - 1, color);
        pos += index + len;
    }
}

//------------------------------------------------------------------------------
void flw::LogDisplay::find(const char* find, bool select) {
    auto offset1 = 0;
    auto offset2 = strlen(find);
    auto found   = _buffer->search_forward(0, find, &offset1, 1);

    if (found) {
        auto lines = 0;

        for (int f = 0; f < offset1; f++) {
            if (_buffer->byte_at(f) == 10) {
                lines++;
            }
        }

        offset2 += offset1;
        scroll(lines, 0);

        if (select) {
            _buffer->select(offset1, offset2);
        }
    }
}

//------------------------------------------------------------------------------
void flw::LogDisplay::line(int, const char*, int) {
}

//------------------------------------------------------------------------------
int flw::LogDisplay::str_index(const char* string, const char* find1, int* index1, const char* find2, int* index2, const char* find3, int* index3) {
    if (string == nullptr || find1 == nullptr || *find1 == 0 || index1 == nullptr) {
        return 0;
    }
    else {
        auto found = strstr(string, find1);

        *index1 = found ? (int) (found - string) : -1;

        if (*index1 >= 0 && find2 && *find2 && index2) {
            found   = strstr(string + *index1 + 1, find2);
            *index2 = found ? (int) (found - string) : -1;

            if (*index2 >= 0 && find3 && *find3 && index3) {
                found   = strstr(string + *index2 + 1, find3);
                *index3 = found ? (int) (found - string) : -1;

                return *index3 == -1 ? 2 : 3;
            }
            else {
                return *index2 == -1 ? 1 : 2;
            }
        }
        else {
            return *index1 == -1 ? 0 : 1;
        }
    }
}

//------------------------------------------------------------------------------
void flw::LogDisplay::value(const char* text) {
    auto line  = (char*) nullptr;
    auto end   = (const char*) nullptr;
    auto start = (const char*) nullptr;
    auto count = 0;
    auto len   = 0;
    auto alloc = 100;

    if (text == nullptr || *text == 0) {
        goto EXIT;
    }
    else {
        len       = strlen(text);
        start     = text;
        end       = text;
        line      = (char*) calloc(alloc + 1, 1);
        _tmp      = (char*) calloc(len + 1, 1);
        _tmp_size = len;

        if (_tmp == nullptr || line == nullptr) {
            goto EXIT;
        }
        else {
            memset(_tmp, 'A', len);

            while (*end) {
                if (*end == '\n' || *(end + 1) == 0) {
                    auto line_len = (int) (end - start) + (*end == '\n' ? 0 : 1);
                    _tmp_pos = (int) (start - text);

                    if (line_len > alloc) {
                        alloc = line_len;
                        free(line);
                        line = (char*) calloc(alloc + 1, 1);

                        if (line == nullptr) {
                            goto EXIT;
                        }
                    }

                    count++;

                    if (line_len > 0) {
                        memcpy(line, start, line_len);
                        line[line_len] = 0;
                        this->line(count, line, line_len);
                    }

                    start = end + 1;
                }

                end++;
            }

            _buffer->text(text);
            _style->text(_tmp);
            highlight_data(_style, _LOGDISPLAY_STYLE, sizeof(_LOGDISPLAY_STYLE) / sizeof(_LOGDISPLAY_STYLE[0]), (char) COLOR::FOREGROUND, nullptr, 0);
            scroll(0, 0);
            free(line);
            free(_tmp);
            _tmp = nullptr;
            return;
        }
    }

EXIT:
    _buffer->text("");
    _style->text("");
    highlight_data(_style, _LOGDISPLAY_STYLE, sizeof(_LOGDISPLAY_STYLE) / sizeof(_LOGDISPLAY_STYLE[0]), (char) COLOR::FOREGROUND, nullptr, 0);
    scroll(0, 0);
    free(line);
    free(_tmp);
    _tmp = nullptr;
}

//------------------------------------------------------------------------------
void flw::LogDisplay::style(int start, int stop, flw::LogDisplay::COLOR c) {
    start += _tmp_pos;
    stop  += _tmp_pos;

    while (start <= stop && start < _tmp_size) {
        *(_tmp + start++) = (char) c;
    }
}

//------------------------------------------------------------------------------
void flw::LogDisplay::style_size(Fl_Fontsize textsize) {
    for (size_t f = 0; f < sizeof(_LOGDISPLAY_STYLE) / sizeof(_LOGDISPLAY_STYLE[0]); f++) {
        _LOGDISPLAY_STYLE[f].size = textsize;
    }
}



//------------------------------------------------------------------------------
flw::Price::Price() {
    high  = 0.0;
    low   = 0.0;
    close = 0.0;
    vol   = 0.0;
}

//------------------------------------------------------------------------------
flw::Price::Price(const std::string& date, double value) {
    this->date = date;
    high       = value;
    low        = value;
    close      = value;
    vol        = value;
}

//------------------------------------------------------------------------------
flw::Price::Price(const std::string& date, double high, double low, double close, double vol) {
    this->date  = date;
    this->high  = high;
    this->low   = low;
    this->close = close;
    this->vol   = vol;

#ifdef DEBUG
    if (close > high || close < low || high < low) {
        fprintf(stderr, "error: values out of order in Price(%s, %15.5f  >=  %15.5f  <=  %15.5f)\n", date.c_str(), high, low, close);
    }
#endif
}

//------------------------------------------------------------------------------
flw::Price::Price(const Price& price) {
    date  = price.date;
    high  = price.high;
    low   = price.low;
    close = price.close;
    vol   = price.vol;
}

//------------------------------------------------------------------------------
flw::Price::Price(Price&& price) {
    date  = price.date;
    high  = price.high;
    low   = price.low;
    close = price.close;
    vol   = price.vol;
}

//------------------------------------------------------------------------------
flw::Price& flw::Price::operator=(const Price& price) {
    date  = price.date;
    high  = price.high;
    low   = price.low;
    close = price.close;
    vol   = price.vol;

    return *this;
}

//------------------------------------------------------------------------------
flw::Price& flw::Price::operator=(Price&& price) {
    date  = price.date;
    high  = price.high;
    low   = price.low;
    close = price.close;
    vol   = price.vol;

    return *this;
}

//------------------------------------------------------------------------------
flw::PriceVector flw::Price::Atr(const PriceVector& in, std::size_t days) {
    PriceVector res;

    if (days > 1 && in.size() > days) {
        auto        tot        = 0.0;
        auto        prev_close = 0.0;
        auto        prev_range = 0.0;
        std::size_t f        = 0;

        days--;

        for (auto& price : in) {
            if (f == 0) {
                tot += price.high - price.low;
            }
            else {
                auto t1 = price.high - price.low;
                auto t2 = fabs(price.high - prev_close);
                auto t3 = fabs(price.low - prev_close);
                auto ra = 0.0;

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
                    res.push_back(Price(price.date, prev_range));
                }
                else if (f > days) {
                    prev_range = ((prev_range * days) + ra) / (days + 1);
                    res.push_back(Price(price.date, prev_range));
                }
            }

            prev_close = price.close;
            f++;
        }
    }

    return res;
}

//------------------------------------------------------------------------------
flw::PriceVector flw::Price::DateSerie(const char* start_date, const char* stop_date, Date::RANGE range, const PriceVector& block) {
    int         month   = -1;
    Date        current = Date::FromString(start_date);
    Date        stop    = Date::FromString(stop_date);
    PriceVector res;

    if (range == Date::RANGE::HOUR && current.year() < 1970) {
        return res;
    }

    if (range == Date::RANGE::FRIDAY) {
        while (current.weekday() != Date::DAY::FRIDAY)
            current.add_days(1);
    }
    else if (range == Date::RANGE::SUNDAY) {
        while (current.weekday() != Date::DAY::SUNDAY) {
            current.add_days(1);
        }
    }

    while (current <= stop) {
        Date date(1, 1, 1);

        if (range == Date::RANGE::DAY) {
            date = Date(current);
            current.add_days(1);
        }
        else if (range == Date::RANGE::WEEKDAY) {
            Date::DAY weekday = current.weekday();

            if (weekday >= Date::DAY::MONDAY && weekday <= Date::DAY::FRIDAY) {
                date = Date(current);
            }

            current.add_days(1);
        }
        else if (range == Date::RANGE::FRIDAY || range == Date::RANGE::SUNDAY) {
            date = Date(current);
            current.add_days(7);
        }
        else if (range == Date::RANGE::MONTH) {
            if (current.month() != month) {
                current.day(current.month_days());
                date = Date(current);
                month = current.month();
            }

            current.add_months(1);
        }
        else if (range == Date::RANGE::HOUR) {
            date = Date(current);
            current.add_seconds(3600);
        }
        else if (range == Date::RANGE::MIN) {
            date = Date(current);
            current.add_seconds(60);
        }
        else if (range == Date::RANGE::SEC) {
            date = Date(current);
            current.add_seconds(1);
        }

        if (date.year() > 1) {
            Price price;

            if (range == Date::RANGE::HOUR || range == Date::RANGE::MIN || range == Date::RANGE::SEC) {
                price.date = date.format(Date::FORMAT::ISO_TIME);
            }
            else {
                price.date = date.format(Date::FORMAT::ISO);
            }

            if (block.size() == 0 || std::binary_search(block.begin(), block.end(), price) == false) {
                res.push_back(price);
            }
        }
    }

    return res;
}

//------------------------------------------------------------------------------
flw::PriceVector flw::Price::DayToMonth(const PriceVector& in) {
    PriceVector res;
    Price       current;
    Date        stop;
    Date        pdate;
    std::size_t f = 0;

    for (auto& price : in) {
        if (f == 0) {
            current = price;
            stop = Date::FromString(current.date.c_str());
            stop.day_last();
        }
        else {
            pdate = Date::FromString(price.date.c_str());

            if (stop < pdate) {
                current.date = stop.format();
                res.push_back(current);
                current = price;
                stop = Date::FromString(current.date.c_str());
                stop.day_last();
            }
            else {
                if (price.high > current.high) {
                    current.high = price.high;
                }

                if (price.low < current.low) {
                    current.low = price.low;
                }

                current.vol   += price.vol;
                current.close  = price.close;

            }
        }

        if (f + 1 == in.size()) {
            auto s = stop.format();
            stop.day_last();
            current.date = s;
            res.push_back(current);
        }

        f++;
    }

    return res;
}


//------------------------------------------------------------------------------
flw::PriceVector flw::Price::DayToWeek(const PriceVector& in, Date::DAY weekday) {
    Price       current;
    Date        stop;
    Date        pdate;
    PriceVector res;
    std::size_t f = 0;

    for (auto& price : in) {
        if (f == 0) {
            stop = Date::FromString(price.date.c_str());

            if (weekday > stop.weekday()) {
                stop.weekday(weekday);
            }
            else if (weekday < stop.weekday()) {
                stop.weekday(weekday);
                stop.add_days(7);
            }

            current = price;
        }
        else {
            pdate = Date::FromString(price.date.c_str());

            if (stop < pdate) {
                current.date = stop.format();
                res.push_back(current);
                current = price;
            }
            else {
                if (price.high > current.high) {
                    current.high = price.high;
                }

                if (price.low < current.low) {
                    current.low = price.low;
                }

                current.vol   += price.vol;
                current.close  = price.close;
            }

            while (stop < pdate) {
                stop.add_days(7);
            }

            current.date = stop.format();
        }

        if (f + 1 == in.size()) {
            current.date = stop.format();
            res.push_back(current);
        }

        f++;
    }

    return res;
}

//------------------------------------------------------------------------------
flw::PriceVector flw::Price::ExponentialMovingAverage(const PriceVector& in, std::size_t days) {
    PriceVector res;

    if (days > 1 && days < in.size()) {
        auto         sma   = 0.0;
        auto         prev  = 0.0;
        auto         multi = 2.0 / double(days + 1.0);
        std::size_t f      = 0;

        for (auto& price : in) {
            if (f < (days - 1)) {
                sma += price.close;
            }
            else if (f == (days - 1)) {
                sma += price.close;
                prev = sma / days;
                res.push_back(Price(price.date, prev));
            }
            else {
                prev = ((price.close - prev) * multi) + prev;
                res.push_back(Price(price.date, prev));
            }

            f++;
        }
    }

    return res;
}

//------------------------------------------------------------------------------
std::string flw::Price::format_date(Date::FORMAT format) const {
    auto date = flw::Date::FromString(this->date.c_str());
    return date.format(format);
}

//------------------------------------------------------------------------------
std::string flw::Price::format_price(Date::FORMAT format, bool hlc, bool v) const {
    auto d = flw::Date::FromString(this->date.c_str());

    if (v == true) {
        return flw::util::format("%s\t%f\t%f\t%f\t%f", d.format(format).c_str(), high, low, close, vol);
    }
    else if (hlc == false || (fabs(close - low) < 0.000001 && fabs(close - high) < 0.000001)) {
        return flw::util::format("%s\t%f", d.format(format).c_str(), close);
    }
    else {
        return flw::util::format("%s\t%f\t%f\t%f", d.format(format).c_str(), high, low, close);
    }
}

//------------------------------------------------------------------------------
flw::PriceVector flw::Price::Momentum(const PriceVector& in, std::size_t days) {
    std::size_t start = days - 1;
    PriceVector res;

    if (days > 1 && days < in.size()) {
        for (auto f = start; f < in.size(); f++) {
            auto& price1 = in[f];
            auto& price2 = in[f - start];

            res.push_back(Price(price1.date, price1.close - price2.close));
        }

    }

    return res;
}

//------------------------------------------------------------------------------
flw::PriceVector flw::Price::MovingAverage(const PriceVector& in, std::size_t days) {
    PriceVector res;

    if (days > 1 && days <= 500 && days < in.size()) {
        std::size_t count = 0;
        auto        sum   = 0.0;
        auto        tmp   = new double[in.size() + 1];

        for (auto& price : in) {
            count++;

            if (count < days) { //  Add data until the first moving average price can be calculated
                tmp[count - 1] = price.close;
                sum += price.close;
            }
            else if (count == days) { //  This is the first point
                tmp[count - 1] = price.close;
                sum += price.close;
                res.push_back(Price(price.date, sum / days));
            }
            else { //  Remove oldest data in range and add current to sum
                tmp[count - 1] = price.close;
                sum -= tmp[count - (days + 1)];
                sum += price.close;
                res.push_back(Price(price.date, sum / days));
            }
        }

        delete [] tmp;
    }

    return res;
}

//------------------------------------------------------------------------------
void flw::Price::print() const {
    printf("Price| date: %s   high=%12.4f   low=%12.4f   close=%12.4f   vol=%12.0f\n", date.c_str(), high, low, close, vol);
    fflush(stdout);
}

//------------------------------------------------------------------------------
void flw::Price::Print(const PriceVector& in) {
    printf("PriceVector(%d)\n", (int) in.size());

    for (auto& price : in) {
        price.print();
    }
}

//------------------------------------------------------------------------------
flw::PriceVector flw::Price::RSI(const PriceVector& in, std::size_t days) {
    PriceVector res;

    if (days > 1 && days <= in.size()) {
        auto avg_gain = 0.0;
        auto avg_loss = 0.0;

        for (std::size_t f = 1; f < in.size(); f++) {
            auto  diff  = 0.0;
            auto& price = in[f];
            auto& prev  = in[f - 1];

            diff = price.close - prev.close;

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
                res.push_back(Price(price.date, 100 - (100 / (1 + (avg_gain / avg_loss)))));
            }
            else if (f > days) {
                avg_gain = ((avg_gain * (days - 1)) + ((diff > 0) ? fabs(diff) : 0)) / days;
                avg_loss = ((avg_loss * (days - 1)) + ((diff < 0) ? fabs(diff) : 0)) / days;
                res.push_back(Price(price.date, 100 - (100 / (1 + (avg_gain / avg_loss)))));
            }
        }
    }

    return res;
}

//------------------------------------------------------------------------------
flw::PriceVector flw::Price::StdDev(const PriceVector& in, std::size_t days) {
    PriceVector res;

    if (days > 2 && days <= in.size()) {
        std::size_t count  = 0;
        auto        sum    = 0.0;
        Price       price2;

        for (auto& price : in) {
            count++;
            sum += price.close;

            if (count >= days) {
                auto mean = sum / days;
                auto dev  = 0.0;

                for (std::size_t j = count - days; j < count; j++) {
                    price2 = in[j];
                    auto tmp = price2.close - mean;

                    tmp *= tmp;
                    dev  += tmp;
                }

                dev /= days;
                dev = sqrt(dev);

                price2 = in[count - days];
                sum -= price2.close;

                price2 = in[count - 1];
                res.push_back(Price(price2.date, dev));
            }
        }
    }

    return res;
}

//------------------------------------------------------------------------------
flw::PriceVector flw::Price::Stochastics(const PriceVector& in, std::size_t days) {
    auto        high = 0.0;
    auto        low  = 0.0;
    PriceVector res;

    for (std::size_t f = 0; f < in.size(); f++) {
        auto& price = in[f];

        if ((f + 1) >= days) {
            high = price.high;
            low  = price.low;

            for (std::size_t j = (f + 1) - days; j < (f + 1) - 1; j++) { //  Get max/min in for current range.
                auto& price2 = in[j];

                if (price2.high > high) {
                    high = price2.high;
                }

                if (price2.low < low) {
                    low = price2.low;
                }
            }

            auto diff1 = price.close - low;
            auto diff2 = high - low;

            if (diff2 > 0.0001) {
                auto kval  = 100 * (diff1 / diff2);
                res.push_back(Price(price.date, kval));
            }
        }
    }

    return res;
}



//------------------------------------------------------------------------------
flw::SplitGroup::SplitGroup(int X, int Y, int W, int H, const char* l) : Fl_Group(X, Y, W, H, l) {
    end();
    clip_children(1);
    clear();
}

//------------------------------------------------------------------------------
void flw::SplitGroup::add(Fl_Widget* widget, SplitGroup::CHILD child) {
    auto num = child == SplitGroup::CHILD::FIRST ? 0 : 1;

    if (_widgets[num]) {
        remove(_widgets[num]);
        delete _widgets[num];
    }

    _widgets[num] = widget;

    if (widget) {
        Fl_Group::add(widget);
    }
}

//------------------------------------------------------------------------------
void flw::SplitGroup::clear() {
    Fl_Group::clear();

    _drag       = false;
    _min        = 50;
    _split_pos  = -1;
    _direction  = DIRECTION::VERTICAL;
    _widgets[0] = nullptr;
    _widgets[1] = nullptr;
}

//------------------------------------------------------------------------------
// When direction is set the child size will by default be set to equal sizes
// Set new size after if you want to change it
//
void flw::SplitGroup::direction(SplitGroup::DIRECTION direction) {
    _direction = direction;
    _split_pos = -1;

    Fl::redraw();
}

//------------------------------------------------------------------------------
int flw::SplitGroup::handle(int event) {
    if (event == FL_DRAG) {
        if (_drag == true) {
            auto pos = 0;

            if (_direction == SplitGroup::DIRECTION::VERTICAL) {
                pos = Fl::event_x() - x();
            }
            else {
                pos = Fl::event_y() - y();
            }

            if (pos != _split_pos) {
                _split_pos = pos;
                resize();
            }

            return 1;
        }
    }
    else if (event == FL_LEAVE) {
        if (_direction == SplitGroup::DIRECTION::VERTICAL) {
            if (Fl::event_y() < y() || Fl::event_y() > y() + h() || Fl::event_x() < x() || Fl::event_x() > x() + w()) {
                _drag = false;
                fl_cursor(FL_CURSOR_DEFAULT);
            }
        }
        else {
            if (Fl::event_y() < y() || Fl::event_y() > y() + h() || Fl::event_x() < x() || Fl::event_x() > x() + w()) {
                _drag = false;
                fl_cursor(FL_CURSOR_DEFAULT);
            }
        }
    }
    else if (event == FL_MOVE) {
        if (_widgets[0] != nullptr && _widgets[1] != nullptr && _widgets[0]->visible() != 0 && _widgets[1]->visible() != 0) {
            if (_direction == SplitGroup::DIRECTION::VERTICAL) {
                auto mx  = Fl::event_x();
                auto pos = x() + _split_pos;

                if (mx > (pos - 3) && mx <= (pos + 3)) {
                    if (_drag == false) {
                        _drag = true;
                        fl_cursor(FL_CURSOR_WE);
                    }

                    return 1;
                }
            }
            else {
                auto my  = Fl::event_y();
                auto pos = y() + _split_pos;

                if (my > (pos - 3) && my <= (pos + 3)) {
                    if (_drag == false) {
                        _drag = true;
                        fl_cursor(FL_CURSOR_NS);
                    }

                    return 1;
                }
            }
        }

        if (_drag == true) {
            _drag = false;
            fl_cursor(FL_CURSOR_DEFAULT);
        }
    }
    else if (event == FL_PUSH) {
        if (_drag == true) {
            return 1;
        }
    }

    return Fl_Group::handle(event);
}

//------------------------------------------------------------------------------
void flw::SplitGroup::resize(int X, int Y, int W, int H) {
    Fl_Widget::resize(X, Y, W, H);

    if (W == 0 || H == 0) {
        return;
    }

    auto currx = X;
    auto curry = Y;
    auto currh = H;
    auto currw = W;

    if (_direction == SplitGroup::DIRECTION::VERTICAL) {
        if (_widgets[0] != nullptr && _widgets[1] != nullptr && _widgets[0]->visible() != 0 && _widgets[1]->visible() != 0) {
            if (_split_pos == -1) {
                _split_pos = W / 2;
            }
            else if (_split_pos >= W - _min) {
                _split_pos = W - _min;
            }
            else if (_split_pos <= _min) {
                _split_pos = _min;
            }

            auto max = (X + W) - 4;
            auto pos = _split_pos + X;

            if (pos < X) {
                pos = X;
            }
            else if (pos > max) {
                pos = max;
            }

            auto w1 = pos - (X + 2);
            auto w2 = (X + W) - (pos + 2);

            _widgets[0]->resize(currx, curry, w1, currh);
            _widgets[1]->resize(currx + w1 + 4, curry, w2, currh);
        }
        else if (_widgets[0] && _widgets[0]->visible()) {
            _widgets[0]->resize(currx, curry, currw, currh);
        }
        else if (_widgets[1] && _widgets[1]->visible()) {
            _widgets[1]->resize(currx, curry, currw, currh);
        }
    }
    else if (_widgets[0] != nullptr && _widgets[1] != nullptr && _widgets[0]->visible() != 0 && _widgets[1]->visible() != 0) { // SplitGroup::DIRECTION::HORIZONTAL
        if (_split_pos == -1) {
            _split_pos = H / 2;
        }
        else if (_split_pos >= H - _min) {
            _split_pos = H - _min;
        }
        else if (_split_pos <= _min) {
            _split_pos = _min;
        }

        auto max = (Y + H) - 4;
        auto pos = _split_pos + Y;

        if (pos < Y) {
            pos = Y;
        }
        else if (pos > max) {
            pos = max;
        }

        auto h1 = pos - (Y + 2);
        auto h2 = (Y + H) - (pos + 2);

        _widgets[0]->resize(currx, curry, currw, h1);
        _widgets[1]->resize(currx, curry + h1 + 4, currw, h2);
        // fprintf(stderr, "%s %4d - %4d <=> %4d - %4d\n", _widgets[0]->label(), _widgets[0]->x(), _widgets[0]->y(), _widgets[0]->w(), _widgets[0]->h());
        // fprintf(stderr, "%s %4d - %4d <=> %4d - %4d\n", _widgets[1]->label(), _widgets[1]->x(), _widgets[1]->y(), _widgets[1]->w(), _widgets[1]->h());
    }
    else if (_widgets[0] != nullptr && _widgets[0]->visible() != 0) {
        _widgets[0]->resize(currx, curry, currw, currh);
    }
    else if (_widgets[1] != nullptr && _widgets[1]->visible() != 0) {
        _widgets[1]->resize(currx, curry, currw, currh);
    }
}


//------------------------------------------------------------------------------
void flw::SplitGroup::toggle(SplitGroup::CHILD child, SplitGroup::DIRECTION direction, int second_size) {
    if (_widgets[0] == nullptr || _widgets[1] == nullptr) {
        return;
    }

    auto num = (child == SplitGroup::CHILD::FIRST) ? 0 : 1;

    if (_widgets[num]->visible() && _direction == direction) { // Direction is the same so hide widget
        _widgets[num]->hide();
    }
    else {
        _widgets[num]->show();

        if (_direction != direction || split_pos() == -1) { // Direction has changed or has no size so resize widgets
            _direction = direction;

            if (second_size == -1) {
                split_pos(-1);
            }
            else if (_direction == SplitGroup::DIRECTION::VERTICAL) {
                split_pos(w() - second_size);
            }
            else {
                split_pos(h() - second_size);
            }
        }
    }
}



namespace flw {
    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------

    //--------------------------------------------------------------------------
    class _TableDisplay_Scrollbar : public Fl_Scrollbar {
    public:
        //----------------------------------------------------------------------
        _TableDisplay_Scrollbar(int X, int Y, int W, int H) : Fl_Scrollbar(X, Y, W, H) {
        }

        //----------------------------------------------------------------------
        int handle(int event) override {
            if (event == FL_FOCUS ||
                event == FL_KEYUP ||
                event == FL_KEYDOWN ||
                event == FL_SHORTCUT) {

                return 0;
            }

            return Fl_Scrollbar::handle(event);
        }
    };

    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------

    //--------------------------------------------------------------------------
    class _TableDisplayCellDialog : public Fl_Double_Window {
        Fl_Int_Input*                   _row;
        Fl_Int_Input*                   _col;
        bool                            _ret;

    public:
        _TableDisplayCellDialog(int row, int col) : Fl_Double_Window(0, 0, 0, 0, "Goto Cell") {
            end();

            _row = new Fl_Int_Input(0, 0, 0, 0, "Row:");
            _col = new Fl_Int_Input(0, 0, 0, 0, "Column:");
            _ret = false;

            add(_row);
            add(_col);

            _row->align(FL_ALIGN_LEFT | FL_ALIGN_TOP);
            _row->callback(_TableDisplayCellDialog::Callback, this);
            _row->labelsize(flw::PREF_FONTSIZE);
            _row->textfont(flw::PREF_FIXED_FONT);
            _row->textsize(flw::PREF_FONTSIZE);
            _row->when(FL_WHEN_ENTER_KEY_ALWAYS);
            _col->align(FL_ALIGN_LEFT | FL_ALIGN_TOP);
            _col->callback(_TableDisplayCellDialog::Callback, this);
            _col->labelsize(flw::PREF_FONTSIZE);
            _col->textfont(flw::PREF_FIXED_FONT);
            _col->textsize(flw::PREF_FONTSIZE);
            _col->when(FL_WHEN_ENTER_KEY_ALWAYS);

            char b[100];
            sprintf(b, "%d", row > 0 ? row : 1);
            _row->value(b);
            sprintf(b, "%d", col > 0 ? col : 1);
            _col->value(b);

            callback(_TableDisplayCellDialog::Callback, this);
            set_modal();
            resizable(this);
            resize(0, 0, flw::PREF_FONTSIZE * 20, flw::PREF_FONTSIZE * 7);
        }

        //----------------------------------------------------------------------
        static void Callback(Fl_Widget* w, void* o) {
            auto dlg = (_TableDisplayCellDialog*) o;

            if (w == dlg) {
                dlg->hide();
            }
            else if (w == dlg->_row) {
                dlg->_ret = true;
                dlg->hide();
            }
            else if (w == dlg->_col) {
                dlg->_ret = true;
                dlg->hide();
            }
        }

        //----------------------------------------------------------------------
        int col() const {
            return (*_col->value() >= '0' && *_col->value() <= '9') ? atoi(_col->value()) : 0;
        }

        //----------------------------------------------------------------------
        void resize(int X, int Y, int W, int H) {
            Fl_Double_Window::resize(X, Y, W, H);

            _row->resize(4, flw::PREF_FONTSIZE + 4, W - 8, flw::PREF_FONTSIZE * 2);
            _col->resize(4, flw::PREF_FONTSIZE * 4 + 8, W - 8, flw::PREF_FONTSIZE * 2);
        }

        //----------------------------------------------------------------------
        int row() const {
            return (*_row->value() >= '0' && *_row->value() <= '9') ? atoi(_row->value()) : 0;
        }

        //----------------------------------------------------------------------
        bool run(Fl_Window* parent) {
            flw::util::center_window(this, parent);
            show();

            while (visible() != 0) {
                Fl::wait();
                Fl::flush();
            }

            return _ret;
        }
    };

    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------

    //--------------------------------------------------------------------------
    class _TableDisplayFindDialog : public Fl_Double_Window {
        Fl_Input*                   _text;
        Fl_Button*                  _next;
        Fl_Button*                  _prev;
        Fl_Button*                  _close;
        TableDisplay*               _table;
        bool                        _repeat;

    public:
        _TableDisplayFindDialog(TableDisplay* table) : Fl_Double_Window(0, 0, 0, 0, "Goto Cell") {
            end();

            _close  = new Fl_Button(0, 0, 0, 0, "&Close");
            _next   = new Fl_Button(0, 0, 0, 0, "&Find next");
            _prev   = new Fl_Button(0, 0, 0, 0, "Find &previous");
            _text   = new Fl_Input(0, 0, 0, 0, "Search:");
            _table  = table;
            _repeat = true;

            add(_text);
            add(_prev);
            add(_next);
            add(_close);

            _close->callback(_TableDisplayFindDialog::Callback, this);
            _close->labelsize(flw::PREF_FONTSIZE);
            _next->callback(_TableDisplayFindDialog::Callback, this);
            _next->labelsize(flw::PREF_FONTSIZE);
            _prev->callback(_TableDisplayFindDialog::Callback, this);
            _prev->labelsize(flw::PREF_FONTSIZE);
            _text->align(FL_ALIGN_LEFT);
            _text->callback(_TableDisplayFindDialog::Callback, this);
            _text->labelsize(flw::PREF_FONTSIZE);
            _text->textfont(flw::PREF_FIXED_FONT);
            _text->textsize(flw::PREF_FONTSIZE);
            _text->value(_table->_find.c_str());
            _text->when(FL_WHEN_ENTER_KEY_ALWAYS);

            callback(_TableDisplayFindDialog::Callback, this);
            set_modal();
            resizable(this);
            resize(0, 0, flw::PREF_FONTSIZE * 35, flw::PREF_FONTSIZE * 7);
        }

        //----------------------------------------------------------------------
        static void Callback(Fl_Widget* w, void* o) {
            auto dlg = (_TableDisplayFindDialog*) o;

            if (w == dlg) {
                dlg->hide();
            }
            else if (w == dlg->_close) {
                dlg->_table->_find = dlg->_text->value();
                dlg->hide();
            }
            else if (w == dlg->_next) {
                dlg->find(true);
            }
            else if (w == dlg->_prev) {
                dlg->find(false);
            }
            else if (w == dlg->_text) {
                dlg->find(dlg->_repeat);
            }
        }

        //----------------------------------------------------------------------
        void find(bool next) {
            auto find = _text->value();

            _repeat = next;

            if (*find == 0) {
                fl_beep(FL_BEEP_ERROR);
                return;
            }

            auto row = _table->row();
            auto col = _table->column();

            if (next == true) {
                if (row < 1 || col < 1) {
                    row = 1;
                    col = 1;
                }
                else {
                    col++;
                }
            }
            else {
                if (row < 1 || col < 1) {
                    row = _table->rows();
                    col = _table->columns();
                }
                else {
                    col--;
                }
            }

AGAIN:
            if (next == true) {
                for (int r = row; r <= _table->rows(); r++) {
                    for (int c = col; c <= _table->columns(); c++) {
                        auto v = _table->cell_value(r, c);

                        if (v != nullptr && strstr(v, find) != nullptr) {
                            _table->active_cell(r, c, true);
                            _table->_find = find;
                            return;
                        }
                    }

                    col = 1;
                }

                if (fl_choice("Unable to find <%s>!\nWould you like to try again from the beginning?", nullptr, "Yes", "No", find) == 1) {
                    col = row = 1;
                    goto AGAIN;
                }
            }
            else {
                for (int r = row; r >= 1; r--) {
                    for (int c = col; c >= 1; c--) {
                        auto v = _table->cell_value(r, c);

                        if (v != nullptr && strstr(v, find) != nullptr) {
                            _table->active_cell(r, c, true);
                            _table->_find = find;
                            return;
                        }
                    }

                    col = _table->columns();
                }

                if (fl_choice("Unable to find <%s>!\nWould you like to try again from the end?", nullptr, "Yes", "No", find) == 1) {
                    row = _table->rows();
                    col = _table->columns();
                    goto AGAIN;
                }
            }
        }

        //----------------------------------------------------------------------
        void resize(int X, int Y, int W, int H) {
            Fl_Double_Window::resize(X, Y, W, H);

            _text->resize  (flw::PREF_FONTSIZE * 5,             flw::PREF_FONTSIZE,                 W - flw::PREF_FONTSIZE * 5 - 4, flw::PREF_FONTSIZE * 2);
            _prev->resize  (W - flw::PREF_FONTSIZE * 30 - 12,   H - flw::PREF_FONTSIZE * 2 - 4,     flw::PREF_FONTSIZE * 10,        flw::PREF_FONTSIZE * 2);
            _next->resize  (W - flw::PREF_FONTSIZE * 20 - 8,    H - flw::PREF_FONTSIZE * 2 - 4,     flw::PREF_FONTSIZE * 10,        flw::PREF_FONTSIZE * 2);
            _close->resize (W - flw::PREF_FONTSIZE * 10 - 4,    H - flw::PREF_FONTSIZE * 2 - 4,     flw::PREF_FONTSIZE * 10,        flw::PREF_FONTSIZE * 2);
        }

        //----------------------------------------------------------------------
        void run(Fl_Window* parent) {
            flw::util::center_window(this, parent);
            show();

            while (visible() != 0) {
                Fl::wait();
                Fl::flush();
            }
        }
    };
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
flw::TableDisplay::TableDisplay(int X, int Y, int W, int H, const char* l) : Fl_Group(X, Y, W, H, l) {
    end();

    _hor  = new _TableDisplay_Scrollbar(0, 0, 0, 0);
    _ver  = new _TableDisplay_Scrollbar(0, 0, 0, 0);
    _edit = nullptr;

    add(_ver);
    add(_hor);

    _hor->callback(&_CallbackVer, this);
    _hor->linesize(10);
    _hor->type(FL_HORIZONTAL);
    _ver->callback(&_CallbackHor, this);
    _ver->linesize(10);

    box(FL_BORDER_BOX);
    color(FL_BACKGROUND_COLOR);
    selection_color(FL_SELECTION_COLOR);
    clip_children(1);
    flw::theme::labelfont(this);
    TableDisplay::clear();
}

//------------------------------------------------------------------------------
void flw::TableDisplay::active_cell(int row, int col, bool show) {
    auto send = false;

    if (_rows == 0 || row < 1 || row > _rows || _cols == 0 || col < 1 || col > _cols) {
        row = -1;
        col = -1;
    }

    if (_curr_row != row || _curr_col != col) {
        send = true;
    }

    _curr_row = row;
    _curr_col = col;

    if (send) {
        _set_event(_curr_row, _curr_col, flw::TableDisplay::EVENT::CURSOR);
        do_callback();
    }

    redraw();

    if (show &&
        _curr_row > 0 &&
        _curr_col > 0) {

        show_cell(_curr_row, _curr_col);
    }
}

//------------------------------------------------------------------------------
void flw::TableDisplay::_CallbackVer(Fl_Widget*, void* o) {
    auto table = (flw::TableDisplay*) o;

    table->_start_col = table->_hor->value();
    table->redraw();
}

//------------------------------------------------------------------------------
 void flw::TableDisplay::_CallbackHor(Fl_Widget*, void* o) {
    auto table = (TableDisplay*) o;

    table->_start_row = table->_ver->value();
    table->redraw();
}

//------------------------------------------------------------------------------
int flw::TableDisplay::_cell_height(int Y) {
    auto y2 = y() + h() - _hor->h();

    if (Y + _height >= y2) {
        return y2 - Y;
    }
    else {
        return _height;
    }
}

//------------------------------------------------------------------------------
int flw::TableDisplay::_cell_width(int col, int X) {
    auto cw  = cell_width(col);
    auto min = _height;

    if (cw < min) {
        cw = min;
    }
    else if (cw > 3840) {
        cw = 3840;
    }

    if (X >= 0 && _expand && col == _cols) {
        return x() + w() - _ver->w() - X;
    }
    else {
        auto x2 = x() + w() - _ver->w();

        if (X + cw >= x2) {
            return x2 - X;
        }
    }

    return cw;
}

//------------------------------------------------------------------------------
void flw::TableDisplay::clear() {
    _cols            = 0;
    _curr_col        = -1;
    _curr_row        = -1;
    _current_cell[0] = 0;
    _current_cell[1] = 0;
    _current_cell[2] = 0;
    _current_cell[3] = 0;
    _disable_hor     = false;
    _disable_ver     = false;
    _drag            = false;
    _edit            = nullptr;
    _event           = flw::TableDisplay::EVENT::UNDEFINED;
    _event_col       = -1;
    _event_row       = -1;
    _expand          = false;
    _height          = flw::PREF_FONTSIZE * 2;
    _resize          = false;
    _resize_col      = -1;
    _rows            = 0;
    _select          = flw::TableDisplay::SELECT::NO;
    _show_col_header = false;
    _show_hor_lines  = false;
    _show_row_header = false;
    _show_ver_lines  = false;
    _start_col       = 1;
    _start_row       = 1;
    _find            = "";
    redraw();
}

//------------------------------------------------------------------------------
void flw::TableDisplay::draw() {
    if (_edit) { // Have to be resized before parent drawing
        _edit->resize(_current_cell[0], _current_cell[1], _current_cell[2] + 1, _current_cell[3] + 1);
    }

    _update_scrollbars();
    Fl_Group::draw();

    if (_ver->visible() != 0 && _hor->visible() != 0) {
        fl_push_clip(x() + 1, y() + 1, w() - 2 - _ver->w(), h() - 2 - _hor->h());
    }
    else if (_ver->visible() != 0) {
        fl_push_clip(x() + 1, y() + 1, w() - 2 - _ver->w(), h() - 2);
    }
    else if (_hor->visible() != 0) {
        fl_push_clip(x() + 1, y() + 1, w() - 2, h() - 2 - _hor->h());
    }
    else {
        fl_push_clip(x() + 1, y() + 1, w() - 2, h() - 2);
    }

    auto w1 = w() - _ver->w();
    auto y1 = y();
    auto y2 = y() + h() - _hor->h();

    for (auto r = (_show_col_header ? 0 : _start_row); r <= _rows; r++) {
        auto ch = _cell_height(y1);

        _draw_row(r, w1, y1, ch);
        y1 += _height;

        if (y1 >= y2) {
            break;
        }
        else if (r == 0) { // After header start with first used row
            r = _start_row - 1;
        }
    }

    if (_edit) { // Make sure border are consistent around edit widget
        fl_color(fl_lighter(FL_FOREGROUND_COLOR));
        fl_rect(_current_cell[0], _current_cell[1], _current_cell[2] + 1, _current_cell[3] + 1);
    }

    fl_pop_clip();
}

//------------------------------------------------------------------------------
void flw::TableDisplay::_draw_cell(int row, int col, int X, int Y, int W, int H, bool ver, bool hor, bool current) {
    fl_push_clip(X, Y, W + 1, H);

    auto align    = cell_align(row, col);
    auto textfont = cell_textfont(row, col);
    auto textsize = cell_textsize(row, col);
    auto val      = cell_value(row, col);
    auto space    = (align & FL_ALIGN_RIGHT) ? 6 : 4;

    if (row > 0 && col > 0) { // Draw normal cell
        auto color     = cell_color(row, col);
        auto textcolor = cell_textcolor(row, col);

        if (current) {
            color = selection_color();
        }

        fl_color(color);
        fl_rectf(X + 1, Y, W + 1, H);
        fl_font(textfont, textsize);
        fl_color(textcolor);
        _draw_text(val, X + space, Y + 2, W - space * 2, H - 4, align);
        fl_color(FL_DARK3);

        if (ver) {
            fl_line(X, Y, X, Y + H);
            fl_line(X + W, Y, X + W, Y + H);
        }

        if (hor) {
            fl_line(X, Y, X + W, Y);
            fl_line(X, Y + H - (row == _rows ? 1 : 0), X + W, Y + H - (row == _rows ? 1 : 0));
        }
    }
    else { // Draw header cell
        fl_draw_box(FL_THIN_UP_BOX, X, Y, W + 1, H + (row == _rows ? 0 : 1), FL_BACKGROUND_COLOR);
        fl_font(textfont, textsize);
        fl_color(FL_FOREGROUND_COLOR);
        _draw_text(val, X + space, Y + 2, W - space * 2, H - 4, align);
    }

    fl_pop_clip();
}

//------------------------------------------------------------------------------
void flw::TableDisplay::_draw_row(int row, int W, int Y, int H) {
    auto x1 = x();
    auto x2 = x() + W;

    for (auto c = (_show_row_header ? 0 : _start_col); c <= _cols; c++) {
        auto cw = _cell_width(c, x1);

        if (row > 0 && c > 0 && row == _curr_row && c == _curr_col) {
            _current_cell[0] = x1;
            _current_cell[1] = Y;
            _current_cell[2] = cw;
            _current_cell[3] = H;

            if (_edit == nullptr) {
                _draw_cell(row, c, x1, Y, cw, H, _show_ver_lines, _show_hor_lines, _select != flw::TableDisplay::SELECT::NO);
            }
        }
        else if (row > 0 && row == _curr_row && _select == flw::TableDisplay::SELECT::ROW) {
            _draw_cell(row, c, x1, Y, cw, H, _show_ver_lines, _show_hor_lines, true);
        }
        else {
            _draw_cell(row, c, x1, Y, cw, H, _show_ver_lines, _show_hor_lines, false);
        }

        x1 += cw;

        if (x1 >= x2) {
            break;
        }
        else if (c == 0) { // After header start with first used column
            c = _start_col - 1;
        }
    }

}

//------------------------------------------------------------------------------
void flw::TableDisplay::_draw_text(const char* string, int X, int Y, int W, int H, Fl_Align align) {
    if (align == FL_ALIGN_CENTER || align == FL_ALIGN_RIGHT) {
        if (fl_width(string) > W) {
            align = FL_ALIGN_LEFT;
        }
    }

    fl_draw(string, X, Y, W, H, align);
}

//------------------------------------------------------------------------------
int flw::TableDisplay::_ev_keyboard_down() {
    auto key   = Fl::event_key();
    auto cmd   = Fl::event_command() != 0;
    auto shift = Fl::event_shift() != 0;

    // printf("key=%d, alt=%d, cmd=%d, shift=%d\n", key, alt, cmd, shift); fflush(stdout);

    if (cmd == true && key == FL_Up) {
        _move_cursor(_TABLEDISPLAY_MOVE::SCROLL_UP);
        return 1;
    }
    else if (key == FL_Up) {
        _move_cursor(_TABLEDISPLAY_MOVE::UP);
        return 1;
    }
    else if (cmd == true && key == FL_Down) {
        _move_cursor(_TABLEDISPLAY_MOVE::SCROLL_DOWN);
        return 1;
    }
    else if (key == FL_Down) {
        _move_cursor(_TABLEDISPLAY_MOVE::DOWN);
        return 1;
    }
    else if (cmd == true && key == FL_Left) {
        _move_cursor(_TABLEDISPLAY_MOVE::SCROLL_LEFT);
        return 1;
    }
    else if (key == FL_Left || (key == FL_Tab && shift == true)) {
        _move_cursor(_TABLEDISPLAY_MOVE::LEFT);
        return 1;
    }
    else if (cmd == true && key == FL_Right) {
        _move_cursor(_TABLEDISPLAY_MOVE::SCROLL_RIGHT);
        return 1;
    }
    else if (key == FL_Right || key == FL_Tab) {
        _move_cursor(_TABLEDISPLAY_MOVE::RIGHT);
        return 1;
    }
    else if (cmd == true && key == FL_Page_Up) {
        _move_cursor(_TABLEDISPLAY_MOVE::FIRST_ROW);
        return 1;
    }
    else if (key == FL_Page_Up) {
        _move_cursor(_TABLEDISPLAY_MOVE::PAGE_UP);
        return 1;
    }
    else if (cmd == true && key == FL_Page_Down) {
        _move_cursor(_TABLEDISPLAY_MOVE::LAST_ROW);
        return 1;
    }
    else if (key == FL_Page_Down) {
        _move_cursor(_TABLEDISPLAY_MOVE::PAGE_DOWN);
        return 1;
    }
    else if (key == FL_Home) {
        _move_cursor(_TABLEDISPLAY_MOVE::FIRST_COL);
        return 1;
    }
    else if (key == FL_End) {
        _move_cursor(_TABLEDISPLAY_MOVE::LAST_COL);
        return 1;
    }
    else if (cmd == true && key == 'c') {
        auto val = cell_value(_curr_row, _curr_col);

        Fl::copy(val, strlen(val), 1);
        return 1;
    }
    else if (cmd == true && key == 'g') {
        auto dlg = _TableDisplayCellDialog(_curr_row, _curr_col);

        if (dlg.run(window()) == true) {
            active_cell(dlg.row(), dlg.col(), true);
        }

        return 1;
    }
    else if (cmd == true && key == 'f') {
        auto dlg = _TableDisplayFindDialog(this);
        dlg.run(window());

        return 1;
    }

    return 0;
}

//------------------------------------------------------------------------------
int flw::TableDisplay::_ev_mouse_click () {
    if (Fl::event_button1() && _drag == true) {
        return 1;
    }

    auto row         = 0;
    auto col         = 0;
    auto current_row = _curr_row;
    auto current_col = _curr_col;

    _get_cell_below_mouse(row, col);

    if (_edit == nullptr) {
        Fl::focus(this);
    }

    if (row == 0 && col >= 1) { // Mouse click on top header cells
        _set_event(row, col, (Fl::event_ctrl() != 0) ? flw::TableDisplay::EVENT::COLUMN_CTRL : flw::TableDisplay::EVENT::COLUMN);
        do_callback();
    }
    else if (col == 0 && row >= 1) { // Mouse click on left header cells
        _set_event(row, col, (Fl::event_ctrl() != 0) ? flw::TableDisplay::EVENT::ROW_CTRL : flw::TableDisplay::EVENT::ROW);
        do_callback();
    }
    else if (row == -1 || col == -1) { // Mouse click outside cell
        if (row == -1 && _hor->visible() != 0 && Fl::event_y() >= _hor->y()) { // Don't deselect if clicked on scrollbar
            ;
        }
        else if (col == -1 && _ver->visible() != 0 && Fl::event_x() >= _ver->x()) { // Don't deselect if clicked on scrollbar
            ;
        }
        else { // If clicked in whitespace then deselect cell
            active_cell(-1, -1);
            return 0;
        }
    }
    else if (row >= 1 && col >= 1 && (row != current_row || col != current_col) && _select != flw::TableDisplay::SELECT::NO) { // Set new current cell and send event
        active_cell(row, col);
    }

    return 2;
}

//------------------------------------------------------------------------------
int flw::TableDisplay::_ev_mouse_drag() {
    if (_drag == false) {
        return 2;
    }

    auto xpos  = Fl::event_x();
    auto currx = x();

    if (_show_row_header && _resize_col == 0) {
        if ((xpos - currx) >= 10) {
            cell_width(_resize_col, xpos - currx);
            redraw();
        }
    }
    else {
        if (_show_row_header) {
            currx += _cell_width(0);
        }

        for (auto c = _start_col; c < _resize_col; c++) {
            currx += _cell_width(c);
        }

        if ((xpos - currx) >= 10) {
            cell_width(_resize_col, xpos - currx);
            redraw();
        }
    }

    return 2;
}

//------------------------------------------------------------------------------
int flw::TableDisplay::_ev_mouse_move() {
    auto mx = Fl::event_x();
    auto x1 = x() + (_show_row_header ? _cell_width(0) : 0);
    auto x2 = x() + w() - _ver->w();

    if (_resize) {
        if (_show_row_header && mx > (x1 - 3) && mx < (x1 + 4)) {
            _resize_col = 0;
            _drag       = true;

            fl_cursor(FL_CURSOR_WE);
            return 1;
        }
        else {
            for (auto c = _start_col; c <= _cols; c++) {
                x1 += _cell_width(c, x1);

                if (x1 >= x2) {
                    break;
                }
                else if (mx > (x1 - 3) && mx < (x1 + 4)) { // Change cursor to show that it is possible to change column width
                    _resize_col = c;
                    _drag       = true;

                    fl_cursor(FL_CURSOR_WE);
                    return 1;
                }
            }
        }
    }

    if (_drag == true) { // Set cursor to the default cursor only if it was set to another cursor before
        _drag = false;
        fl_cursor(FL_CURSOR_DEFAULT);
    }

    _resize_col = -1;
    return 2;
}

//------------------------------------------------------------------------------
void flw::TableDisplay::_get_cell_below_mouse(int& row, int& col) {
    row = -1;
    col = -1;

    auto my = Fl::event_y();
    auto mx = Fl::event_x();

    if (!((_ver->visible() != 0 && mx >= _ver->x()) || (_hor->visible() != 0 && my >= _hor->y()))) { // Dont click on scrollbars
        if (_show_col_header && (my - y()) < _height) {
            row = 0;
        }
        else {
            auto y1 = y() + (_show_col_header ? _height : 0);
            auto y2 = y() + h();

            for (auto r = _start_row; r <= _rows; r++) {
                if (y1 > y2) {
                    break;
                }
                else if (my > y1 && my < y1 + _height) {
                    row = r;
                    break;
                }

                y1 += _height;
            }
        }

        if (_show_row_header && (mx - x()) < _cell_width(0)) {
            col = 0;
        }
        else {
            auto x1 = x() + (_show_row_header ? _cell_width(0) : 0);
            auto x2 = x() + w();

            for (auto c = _start_col; c <= _cols; c++) {
                auto cw = _cell_width(c);

                if (x1 > x2) {
                    break;
                }
                else if (mx > x1 && (mx < x1 + cw || c == _cols)) {
                    col = c;
                    break;
                }

                x1 += cw;
            }
        }
    }
}

//------------------------------------------------------------------------------
int flw::TableDisplay::handle(int event) {
    auto ret = 2;

    if (_rows > 0 && _cols > 0) {
        if (event == FL_FOCUS) {
            ret = 1;
        }
        else if (event == FL_LEAVE) {
            fl_cursor(FL_CURSOR_DEFAULT);
        }
        else if (event == FL_DRAG) {
            ret = _ev_mouse_drag();
        }
        else if (event == FL_KEYDOWN) {
            ret = _ev_keyboard_down();
        }
        else if (event == FL_MOVE) {
            ret = _ev_mouse_move();
        }
        else if (event == FL_PUSH) {
            ret = _ev_mouse_click();
        }
    }

    if (ret == 0 || ret == 1) {
        return ret;
    }
    else {
        return Fl_Group::handle(event);
    }
}

//------------------------------------------------------------------------------
void flw::TableDisplay::header(bool row, bool col) {
    _show_row_header = row;
    _show_col_header = col;
    redraw();
}

//------------------------------------------------------------------------------
void flw::TableDisplay::lines(bool ver, bool hor) {
    _show_ver_lines = ver;
    _show_hor_lines = hor;
    redraw();
}

//------------------------------------------------------------------------------
void flw::TableDisplay::_move_cursor(_TABLEDISPLAY_MOVE move) {
    if (_edit == nullptr && _rows > 0 && _cols > 0 && _select != flw::TableDisplay::SELECT::NO) {
        auto r     = _curr_row;
        auto c     = _curr_col;
        auto range = (int) ((h() - _hor->h() - (_show_row_header ? _height : 0)) / _height);

        if (r < 1) {
            r = 1;
        }
        else if (r > _rows) {
            r = _rows;
        }
        else {
            if (range > 0) {
                switch (move) {
                    case _TABLEDISPLAY_MOVE::FIRST_ROW:
                        r = 1;
                        break;

                    case _TABLEDISPLAY_MOVE::UP:
                        r = r > 1 ? r - 1 : 1;
                        break;

                    case _TABLEDISPLAY_MOVE::SCROLL_UP:
                        r = r > 8 ? r - 8 : 1;
                        break;

                    case _TABLEDISPLAY_MOVE::PAGE_UP:
                        r = _start_row - range > 0 ? _start_row - range : 1;
                        _start_row = r;
                        break;

                    case _TABLEDISPLAY_MOVE::DOWN:
                        r = r < _rows ? r + 1 : _rows;
                        break;

                    case _TABLEDISPLAY_MOVE::SCROLL_DOWN:
                        r = r < _rows - 8 ? r + 8 : _rows;
                        break;

                    case _TABLEDISPLAY_MOVE::PAGE_DOWN:
                        r = _start_row + range <= _rows ? _start_row + range : _rows;
                        _start_row = r;
                        break;

                    case _TABLEDISPLAY_MOVE::LAST_ROW:
                        r = _rows;
                        break;

                    default:
                        break;
                }
            }
        }

        if (c < 1) {
            c = 1;
        }
        else if (c > _cols) {
            c = _cols;
        }
        else {
            switch (move) {
                case _TABLEDISPLAY_MOVE::LEFT:
                    c = c > 1 ? c - 1 : 1;
                    break;

                case _TABLEDISPLAY_MOVE::SCROLL_LEFT:
                    c = c > 4 ? c - 4 : 1;
                    break;

                case _TABLEDISPLAY_MOVE::RIGHT:
                    c = c < _cols ? c + 1 : _cols;
                    break;

                case _TABLEDISPLAY_MOVE::SCROLL_RIGHT:
                    c = c < _cols - 4 ? c + 4 : _cols;
                    break;

                case _TABLEDISPLAY_MOVE::FIRST_COL:
                    c = 1;
                    break;

                case _TABLEDISPLAY_MOVE::LAST_COL:
                    c = _cols;
                    break;

                default:
                    break;
            }
        }

        if (r > 0 && r <= _rows && c > 0 && c <= _cols) {
            active_cell(r, c, true);
        }
    }
}

//------------------------------------------------------------------------------
void flw::TableDisplay::show_cell(int row, int col) {
    if (_rows > 0 && row > 0 && row <= _rows) {
        auto rc = (h() - Fl::scrollbar_size() - (_show_col_header ? _height : 0)) / _height;

        if (row <= _start_row) {
            _start_row = row;
        }
        else if (row >= _start_row + rc) {
            _start_row = row - rc + 1;
        }
    }

    if (_cols > 0 && col > 0 && col <= _cols) {
        if (col <= _start_col) {
            _start_col = col;
        }
        else {
            LOOP:

            if (_start_col < col) {
                auto x1 = x() + (_show_row_header ? _cell_width(0) : 0);
                auto x2 = x() + w() - Fl::scrollbar_size();

                for (auto c = _start_col; c <= col; c++) {
                    auto cw = _cell_width(c);

                    if (x1 + cw >= x2) {
                        if (col - c < 20) {
                            _start_col++;
                        }
                        else {
                            _start_col = col - 20;
                        }

                        goto LOOP;
                    }

                    x1 += cw;
                }
            }
        }
    }

    redraw();
}

//------------------------------------------------------------------------------
void flw::TableDisplay::size(int rows, int cols) {
    if (rows > -1 && _cols > -1) {
        _rows      = rows;
        _cols      = cols;
        _curr_row  = 1;
        _curr_col  = 1;
        _start_row = 1;
        _start_col = 1;

        _set_event(_curr_row, _curr_col, flw::TableDisplay::EVENT::SIZE);
        do_callback();
    }

    redraw();
}

//------------------------------------------------------------------------------
void flw::TableDisplay::_update_scrollbars() {
    if (_rows > 0 && _cols > 0) {
        if (_disable_hor == true) {
            _hor->hide();
        }
        else {
            auto cols  = 0;
            auto width = _show_row_header ? _cell_width(0) : 0;

            for (auto c = 1; c <= _cols; c++) {
                width += _cell_width(c);

                if (width > (w() - Fl::scrollbar_size())) {
                    break;
                }
                else {
                    cols++;
                }
            }

            if (_cols > cols) {
                _hor->slider_size(0.2);
                _hor->range(1, _cols);
                _hor->show();
            }
            else {
                _hor->hide();

                if (_start_col > 0) {
                    _start_col = 1;
                }
            }
        }


        if (_disable_ver == true) {
            _ver->hide();
        }
        else {
            auto rows = (h() - Fl::scrollbar_size() - (_show_col_header ? _height : 0)) / _height;

            if (_rows > rows) {
                _ver->slider_size(0.2);
                _ver->range(1, _rows);
                _ver->show();
            }
            else {
                _ver->hide();

                if (_start_row > 0) {
                    _start_row = 1;
                }
            }
        }
    }
    else {
        _ver->hide();
        _hor->hide();
    }

    if (_ver->visible() != 0 && _hor->visible() != 0) {
        _ver->resize(x() + w() - Fl::scrollbar_size() - 1, y() + 1, Fl::scrollbar_size(), h() - Fl::scrollbar_size() - 2);
        _hor->resize(x() + 1, y() + h() - Fl::scrollbar_size() - 1, w() - Fl::scrollbar_size() - 2, Fl::scrollbar_size());
    }
    else if (_ver->visible() != 0) {
        _ver->resize(x() + w() - Fl::scrollbar_size() - 1, y() + 1, Fl::scrollbar_size(), h() - 2);
        _hor->resize(0, 0, 0, 0);
    }
    else if (_hor->visible() != 0) {
        _hor->resize(x() + 1, y() + h() - Fl::scrollbar_size() - 1, w() - 2, Fl::scrollbar_size());
        _ver->resize(0, 0, 0, 0);
    }
    else {
        _ver->resize(0, 0, 0, 0);
        _hor->resize(0, 0, 0, 0);
    }

    _ver->Fl_Valuator::value(_start_row);
    _hor->Fl_Valuator::value(_start_col);
}



const char* flw::TableEditor::SELECT_DATE = "Select Date";
const char* flw::TableEditor::SELECT_DIR  = "Select Directory";
const char* flw::TableEditor::SELECT_FILE = "Select File";
const char* flw::TableEditor::SELECT_LIST = "Select String";

//------------------------------------------------------------------------------
flw::TableEditor::TableEditor(int X, int Y, int W, int H, const char* l) : TableDisplay(X, Y, W, H, l) {
    TableEditor::clear();
}

//------------------------------------------------------------------------------
void flw::TableEditor::clear() {
    TableDisplay::clear();

    _send_changed_event_always = false;
    _edit2 = nullptr;
    _edit3 = nullptr;
}

//------------------------------------------------------------------------------
void flw::TableEditor::_delete_current_cell() {
    if (_curr_row > 0 && _curr_col > 0) {
        auto edit = cell_edit(_curr_row, _curr_col);

        if (edit == true) {
            auto rend = cell_rend(_curr_row, _curr_col);
            auto set  = false;

            switch (rend) {
                case flw::TableEditor::REND::TEXT:
                case flw::TableEditor::REND::SECRET:
                case flw::TableEditor::REND::INPUT_CHOICE:
                case flw::TableEditor::REND::DLG_FILE:
                case flw::TableEditor::REND::DLG_DIR:
                case flw::TableEditor::REND::DLG_LIST:
                    set = cell_value(_curr_row, _curr_col, "");
                    break;

                case flw::TableEditor::REND::INTEGER:
                    set = cell_value(_curr_row, _curr_col, "0");
                    break;

                case flw::TableEditor::REND::NUMBER:
                    set = cell_value(_curr_row, _curr_col, "0.0");
                    break;

                case flw::TableEditor::REND::BOOLEAN:
                    set = cell_value(_curr_row, _curr_col, "0");
                    break;

                case flw::TableEditor::REND::DLG_DATE:
                    set = cell_value(_curr_row, _curr_col, "1970-01-01");
                    break;

                case flw::TableEditor::REND::DLG_COLOR:
                    set = cell_value(_curr_row, _curr_col, "56");
                    break;

                default:
                    break;
            }

            if (set == true) {
                _set_event(_curr_row, _curr_col, flw::TableEditor::EVENT::CHANGED);
                do_callback();
                redraw();
            }
        }
    }
}

//------------------------------------------------------------------------------
void flw::TableEditor::_draw_cell(int row, int col, int X, int Y, int W, int H, bool ver, bool hor, bool current) {
    fl_push_clip(X, Y, W + 1, H);

    auto align     = cell_align(row, col);
    auto textcolor = cell_textcolor(row, col);
    auto textfont  = cell_textfont(row, col);
    auto textsize  = cell_textsize(row, col);
    auto val       = ((TableDisplay*) this)->cell_value(row, col);

    assert(val);

    if (row > 0 && col > 0) { // Draw normal cell
        auto format = cell_format(row, col);
        auto rend   = cell_rend(row, col);
        auto color  = cell_color(row, col);
        char buffer[100];

        if (current) {
            color = selection_color();
        }

        fl_rectf(X + 1, Y, W + 1, H, color);

        if (rend == flw::TableEditor::REND::SECRET) {
            fl_font(textfont, textsize);
            fl_color(textcolor);

            if (format == flw::TableEditor::FORMAT::SECRET_DOT) {
                fl_draw("••••••", X + 4, Y + 2, W - 8, H - 4, align, 0, 1);
            }
            else {
                fl_draw("******", X + 4, Y + 2, W - 8, H - 4, align, 0, 1);
            }
        }
        else if (rend == flw::TableEditor::REND::SLIDER) {
            double nums[4];

            if (util::to_doubles(val, nums, 4) == 4) {
                auto range = 0.0;

                if ((nums[2] - nums[1]) > 0.0001) {
                    range = (nums[0] - nums[1]) / (nums[2] - nums[1]);
                }

                auto width = (int) (range * (W - 3));

                if (width > 0) {
                    fl_draw_box(FL_FLAT_BOX, X + 2, Y + 2, width, H - 3, textcolor);
                }
            }
        }
        else if (rend == flw::TableEditor::REND::DLG_COLOR) {
            fl_draw_box(FL_FLAT_BOX, X + 2, Y + 2, W - 3, H - 3, (Fl_Color) util::to_int(val));
        }
        else if (rend == flw::TableEditor::REND::DLG_DATE) {
            auto        date = Date::FromString(val);
            std::string string;

            if (format == flw::TableEditor::FORMAT::DATE_WORLD) {
                string = date.format(Date::FORMAT::WORLD);
            }
            else if (format == flw::TableEditor::FORMAT::DATE_US) {
                string = date.format(Date::FORMAT::US);
            }
            else {
                string = date.format(Date::FORMAT::ISO_LONG);
            }

            fl_font(textfont, textsize);
            fl_color(textcolor);
            _draw_text(string.c_str(), X + 4, Y + 2, W - 8, H - 4, align);
        }
        else if (rend == flw::TableEditor::REND::BOOLEAN) {
            auto bw  = textsize + 4;
            auto y_1 = Y + (H / 2) - (bw / 2);
            auto x_1 = 0;

            if (align == FL_ALIGN_RIGHT) {
                x_1 = X + W - bw - 6;
            }
            else if (align == FL_ALIGN_CENTER) {
                x_1 = X + (W / 2) - (bw / 2);
            }
            else if (align == FL_ALIGN_LEFT) {
                x_1 = X + 6;
            }

            fl_draw_box(FL_DOWN_BOX, x_1, y_1, bw, bw, FL_WHITE);

            if (*val == '1') {
                bw -= 6;
                fl_draw_box(FL_ROUND_UP_BOX, x_1 + 3, y_1 + 3, bw, bw, selection_color());
            }
        }
        else if (rend == flw::TableEditor::REND::INTEGER) {
            auto num = util::to_int(val);

            fl_font(textfont, textsize);
            fl_color(textcolor);

            if (format == flw::TableEditor::FORMAT::INT_SEP) {
                auto s = util::format_int(num);
                _draw_text(s.c_str(), X + 4, Y + 2, W - 8, H - 4, align);
            }
            else {
                snprintf(buffer, 100, "%lld", (long long int) num);
                _draw_text(buffer, X + 4, Y + 2, W - 8, H - 4, align);
            }
        }
        else if (rend == flw::TableEditor::REND::NUMBER || rend == flw::TableEditor::REND::VALUE_SLIDER) {
            auto num = util::to_double(val);

            if (rend == flw::TableEditor::REND::VALUE_SLIDER) {
                double nums[1];

                if (util::to_doubles(val, nums, 1) == 1) {
                    num = nums[0];
                }
            }

            if (format == flw::TableEditor::FORMAT::DEC_0) {
                snprintf(buffer, 100, "%.0f", num);
            }
            else if (format == flw::TableEditor::FORMAT::DEC_1) {
                snprintf(buffer, 100, "%.1f", num);
            }
            else if (format == flw::TableEditor::FORMAT::DEC_2) {
                snprintf(buffer, 100, "%.2f", num);
            }
            else if (format == flw::TableEditor::FORMAT::DEC_3) {
                snprintf(buffer, 100, "%.3f", num);
            }
            else if (format == flw::TableEditor::FORMAT::DEC_4) {
                snprintf(buffer, 100, "%.4f", num);
            }
            else if (format == flw::TableEditor::FORMAT::DEC_5) {
                snprintf(buffer, 100, "%.5f", num);
            }
            else if (format == flw::TableEditor::FORMAT::DEC_6) {
                snprintf(buffer, 100, "%.6f", num);
            }
            else if (format == flw::TableEditor::FORMAT::DEC_7) {
                snprintf(buffer, 100, "%.7f", num);
            }
            else if (format == flw::TableEditor::FORMAT::DEC_8) {
                snprintf(buffer, 100, "%.8f", num);
            }
            else {
                snprintf(buffer, 100, "%f", num);
            }

            fl_font(textfont, textsize);
            fl_color(textcolor);
            _draw_text(buffer, X + 4, Y + 2, W - 8, H - 4, align);
        }
        else { // Draw plain text string
            fl_font(textfont, textsize);
            fl_color(textcolor);
            _draw_text(val, X + 4, Y + 2, W - 8, H - 4, align);
        }

        fl_color(FL_DARK3);

        if (ver == true) {
            fl_line(X, Y, X, Y + H);
            fl_line(X + W, Y, X + W, Y + H);
        }

        if (hor == true) {
            fl_line(X, Y, X + W, Y);
            fl_line(X, Y + H - (row == _rows ? 1 : 0), X + W, Y + H - (row == _rows ? 1 : 0));
        }
    }
    else { // Draw header cell
        fl_draw_box(FL_THIN_UP_BOX, X, Y, W + 1, H + (row == _rows ? 0 : 1), FL_BACKGROUND_COLOR);
        fl_font(textfont, textsize);
        fl_color(FL_FOREGROUND_COLOR);
        _draw_text(val, X + 4, Y + 2, W - 8, H - 4, align);
    }

    fl_pop_clip();
}

//------------------------------------------------------------------------------
void flw::TableEditor::_edit_create() {
    auto rend      = cell_rend(_curr_row, _curr_col);
    auto align     = cell_align(_curr_row, _curr_col);
    auto color     = cell_color(_curr_row, _curr_col);
    auto textcolor = FL_FOREGROUND_COLOR;
    auto textfont  = cell_textfont(_curr_row, _curr_col);
    auto textsize  = cell_textsize(_curr_row, _curr_col);
    auto val       = ((TableDisplay*) this)->cell_value(_curr_row, _curr_col);

    if (rend == flw::TableEditor::REND::TEXT ||
        rend == flw::TableEditor::REND::INTEGER ||
        rend == flw::TableEditor::REND::NUMBER ||
        rend == flw::TableEditor::REND::SECRET) {

        auto w = (Fl_Input*) nullptr;

        if (rend == flw::TableEditor::REND::TEXT) {
            w = new Fl_Input(0, 0, 0, 0);
        }
        else if (rend == flw::TableEditor::REND::INTEGER) {
            w = new Fl_Int_Input(0, 0, 0, 0);
        }
        else if (rend == flw::TableEditor::REND::NUMBER) {
            w = new Fl_Float_Input(0, 0, 0, 0);
        }
        else if (rend == flw::TableEditor::REND::SECRET) {
            w = new Fl_Secret_Input(0, 0, 0, 0);
        }

        w->align(align);
        w->box(FL_BORDER_BOX);
        w->color(color);
        w->labelsize(textsize);
        w->textcolor(textcolor);
        w->textfont(textfont);
        w->textsize(textsize);
        w->value(val);
        w->position(0, strlen(val));

        _edit = w;
    }
    else if (rend == flw::TableEditor::REND::BOOLEAN) {
        auto w = new Fl_Check_Button(0, 0, 0, 0);

        w->box(FL_BORDER_BOX);
        w->color(color);
        w->labelcolor(textcolor);
        w->labelsize(textsize);
        w->value(*val && val[0] == '1' ? 1 : 0);

        _edit = w;
    }
    else if (rend == flw::TableEditor::REND::SLIDER) {
        auto w = (Fl_Slider*) nullptr;
        double nums[4];

        if (util::to_doubles(val, nums, 4) == 4) {
            w = new Fl_Slider(0, 0, 0, 0);
            w->color(color);
            w->selection_color(textcolor);
            w->range(nums[1], nums[2]);
            w->step(nums[3]);
            w->value(nums[0]);
            w->type(FL_HOR_FILL_SLIDER);
            w->box(FL_BORDER_BOX);

            _edit = w;
        }
    }
    else if (rend == flw::TableEditor::REND::VALUE_SLIDER) {
        auto w = (Fl_Slider*) nullptr;
        double nums[4];

        if (util::to_doubles(val, nums, 4) == 4) {
            w = new Fl_Value_Slider(0, 0, 0, 0);
            w->color(color);
            w->selection_color(textcolor);
            w->range(nums[1], nums[2]);
            w->step(nums[3]);
            w->value(nums[0]);
            w->type(FL_HOR_FILL_SLIDER);
            w->box(FL_BORDER_BOX);
            ((Fl_Value_Slider*) w)->textsize(textsize * 0.8);

            _edit = w;
        }
    }
    else if (rend == flw::TableEditor::REND::CHOICE) {
        auto choices = cell_choice(_curr_row, _curr_col);

        if (choices.size() > 0) {
            auto w      = new Fl_Choice(0, 0, 0, 0);
            auto select = (std::size_t) 0;

            w->align(align);
            w->box(FL_BORDER_BOX);
            w->down_box(FL_BORDER_BOX);
            w->labelsize(textsize);
            w->textfont(textfont);
            w->textsize(textsize);

            for (std::size_t f = 0; f < choices.size(); f++) {
                auto& s = choices[f];

                w->add(s.c_str());

                if (s == val) {
                    select = f;
                }
            }

            w->value(select);
            _edit = w;
        }
    }
    else if (rend == flw::TableEditor::REND::INPUT_CHOICE) {
        auto choices = cell_choice(_curr_row, _curr_col);

        if (choices.size() > 0) {
            auto w = new Fl_Input_Choice(0, 0, 0, 0);

            w->box(FL_NO_BOX);
            w->down_box(FL_BORDER_BOX);
            w->input()->box(FL_BORDER_BOX);
            w->input()->textfont(textfont);
            w->input()->textsize(textsize);
            w->labelsize(textsize);
            w->menubutton()->box(FL_BORDER_BOX);
            w->menubutton()->textfont(textfont);
            w->menubutton()->textsize(textsize);
            w->textsize(textsize);

            for (auto& s : choices) {
                w->add(s.c_str());
            }

            if (*val) {
                w->value(val);
                w->input()->position(0, strlen(val));
            }

            _edit  = w;
            _edit2 = w->input();
            _edit3 = w->menubutton();
        }
    }

    if (_edit != nullptr) {
        add(_edit);

        if (_edit2 != nullptr) {
            Fl::focus(_edit2);
        }
        else {
            Fl::focus(_edit);
        }
    }
}

//------------------------------------------------------------------------------
void flw::TableEditor::_edit_quick(const char* key) {
    auto rend = cell_rend(_curr_row, _curr_col);
    auto val  = ((TableDisplay*) this)->cell_value(_curr_row, _curr_col);
    char buffer[100];

    if (rend == flw::TableEditor::REND::INTEGER) {
        auto num = util::to_int(val);

        if (strcmp(key, "+") == 0) {
            num++;
        }
        else if (strcmp(key, "++") == 0) {
            num += 10;
        }
        else if (strcmp(key, "+++") == 0) {
            num += 100;
        }
        else if (strcmp(key, "-") == 0) {
            num--;
        }
        else if (strcmp(key, "--") == 0) {
            num -= 10;
        }
        else if (strcmp(key, "---") == 0) {
            num -= 100;
        }

        snprintf(buffer, 100, "%lld", (long long int) num);

        if ((_send_changed_event_always == true || strcmp(val, buffer) != 0) && cell_value(_curr_row, _curr_col, buffer) == true) {
            _set_event(_curr_row, _curr_col, flw::TableEditor::EVENT::CHANGED);
            do_callback();
        }
    }
    else if (rend == flw::TableEditor::REND::NUMBER) {
        auto num = util::to_double(val);

        if (strcmp(key, "+") == 0) {
            num += 0.1;
        }
        else if (strcmp(key, "++") == 0) {
            num += 1.0;
        }
        else if (strcmp(key, "+++") == 0) {
            num += 10.0;
        }
        else if (strcmp(key, "-") == 0) {
            num -= 0.1;
        }
        else if (strcmp(key, "--") == 0) {
            num -= 1.0;
        }
        else if (strcmp(key, "---") == 0) {
            num -= 10.0;
        }

        snprintf(buffer, 100, "%f", num);

        if ((_send_changed_event_always == true || strcmp(val, buffer) != 0) && cell_value(_curr_row, _curr_col, buffer) == true) {
            _set_event(_curr_row, _curr_col, flw::TableEditor::EVENT::CHANGED);
            do_callback();
        }
    }
    else if (rend == flw::TableEditor::REND::DLG_DATE) {
        auto date = Date::FromString(val);

        if (strcmp(key, "+") == 0) {
            date.add_days(1);
        }
        else if (strcmp(key, "++") == 0) {
            date.add_months(1);
        }
        else if (strcmp(key, "+++") == 0) {
            date.year(date.year() + 1);
        }
        else if (strcmp(key, "-") == 0) {
            date.add_days(-1);
        }
        else if (strcmp(key, "--") == 0) {
            date.add_months(-1);
        }
        else if (strcmp(key, "---") == 0) {
            date.year(date.year() - 1);
        }

        auto string = date.format(Date::FORMAT::ISO_LONG);

        if ((_send_changed_event_always == true || string != val) && cell_value(_curr_row, _curr_col, string.c_str()) == true) {
            _set_event(_curr_row, _curr_col, flw::TableEditor::EVENT::CHANGED);
            do_callback();
        }
    }
    else if (rend == flw::TableEditor::REND::SLIDER || rend == flw::TableEditor::REND::VALUE_SLIDER) {
        double nums[4];

        if (util::to_doubles(val, nums, 4) == 4) {
            if (strcmp(key, "+") == 0) {
                nums[0] += nums[3];
            }
            else if (strcmp(key, "++") == 0) {
                nums[0] += (nums[3] * 10);
            }
            else if (strcmp(key, "+++") == 0) {
                nums[0] += (nums[3] * 100);
            }
            else if (strcmp(key, "-") == 0) {
                nums[0] -= nums[3];
            }
            else if (strcmp(key, "--") == 0) {
                nums[0] -= (nums[3] * 10);
            }
            else if (strcmp(key, "---") == 0) {
                nums[0] -= (nums[3] * 100);
            }

            if (nums[0] < nums[1]) {
                nums[0] = nums[1];
            }
            else if (nums[0] > nums[2]) {
                nums[0] = nums[2];
            }

            auto val2 = FormatSlider(nums[0], nums[1], nums[2], nums[3]);

            if ((_send_changed_event_always == true || strcmp(val, val2) != 0) && cell_value(_curr_row, _curr_col, val2) == true) {
                _set_event(_curr_row, _curr_col, flw::TableEditor::EVENT::CHANGED);
                do_callback();
            }
        }
    }
}

//------------------------------------------------------------------------------
void flw::TableEditor::_edit_show() {
    auto rend = cell_rend(_curr_row, _curr_col);
    auto val  = ((TableDisplay*) this)->cell_value(_curr_row, _curr_col);
    char buffer[100];

    if (rend == flw::TableEditor::REND::DLG_COLOR) {
        auto color1 = (int) util::to_int(val);
        auto color2 = (int) fl_show_colormap((Fl_Color) color1);

        snprintf(buffer, 20, "%d", color2);

        if ((_send_changed_event_always == true || color1 != color2) && cell_value(_curr_row, _curr_col, buffer) == true) {
            _set_event(_curr_row, _curr_col, flw::TableEditor::EVENT::CHANGED);
            do_callback();
        }
    }
    else if (rend == flw::TableEditor::REND::DLG_FILE) {
        auto result = fl_file_chooser(flw::TableEditor::SELECT_FILE, "", val, 0);

        if ((_send_changed_event_always == true || (result != nullptr && strcmp(val, result) != 0)) && cell_value(_curr_row, _curr_col, result) == true) {
            _set_event(_curr_row, _curr_col, flw::TableEditor::EVENT::CHANGED);
            do_callback();
        }
    }
    else if (rend == flw::TableEditor::REND::DLG_DIR) {
        auto result = fl_dir_chooser(flw::TableEditor::SELECT_DIR, val);

        if ((_send_changed_event_always == true || (result != nullptr && strcmp(val, result) != 0)) && cell_value(_curr_row, _curr_col, result) == true) {
            _set_event(_curr_row, _curr_col, flw::TableEditor::EVENT::CHANGED);
            do_callback();
        }
    }
    else if (rend == flw::TableEditor::REND::DLG_DATE) {
        auto date1  = Date::FromString(val);
        auto date2  = Date(date1);
        auto result = flw::dlg::date(flw::TableEditor::SELECT_DATE, date1, top_window());
        auto string = date1.format(Date::FORMAT::ISO_LONG);

        if ((_send_changed_event_always == true || (result == true && date1 != date2)) && cell_value(_curr_row, _curr_col, string.c_str()) == true) {
            _set_event(_curr_row, _curr_col, flw::TableEditor::EVENT::CHANGED);
            do_callback();
        }
    }
    else if (rend == flw::TableEditor::REND::DLG_LIST) {
        auto choices = cell_choice(_curr_row, _curr_col);

        if (choices.size() > 0) {
            auto row = dlg::select(flw::TableEditor::SELECT_LIST, choices, val);

            if (row > 0) {
                auto& string = choices[row - 1];

                if ((_send_changed_event_always == true || string != val) && cell_value(_curr_row, _curr_col, string.c_str()) == true) {
                    _set_event(_curr_row, _curr_col, flw::TableEditor::EVENT::CHANGED);
                    do_callback();
                }
            }
        }
    }
}

//------------------------------------------------------------------------------
void flw::TableEditor::_edit_start(const char* key) {
    if (_select != flw::TableEditor::SELECT::NO && _edit == nullptr && _curr_row >= 1 && _curr_col >= 1 && cell_edit(_curr_row, _curr_col)) {
        Fl::event_clicks(0);
        Fl::event_is_click(0);

        flw::TableEditor::REND rend = cell_rend(_curr_row, _curr_col);

        if (*key != 0) {
            _edit_quick(key);
        }
        else if (rend == flw::TableEditor::REND::DLG_COLOR ||
                 rend == flw::TableEditor::REND::DLG_FILE ||
                 rend == flw::TableEditor::REND::DLG_DIR ||
                 rend == flw::TableEditor::REND::DLG_DATE ||
                 rend == flw::TableEditor::REND::DLG_LIST) {
            _edit_show();
        }
        else {
            _edit_create();
        }

        redraw();
    }
}

//------------------------------------------------------------------------------
void flw::TableEditor::_edit_stop(bool save) {
    if (_edit) {
        auto rend = cell_rend(_curr_row, _curr_col);
        auto val  = ((TableDisplay*) this)->cell_value(_curr_row, _curr_col);
        auto stop = true;

        if (save == true) {
            if (rend == flw::TableEditor::REND::TEXT ||
                rend == flw::TableEditor::REND::INTEGER ||
                rend == flw::TableEditor::REND::NUMBER ||
                rend == flw::TableEditor::REND::SECRET) {

                auto input = (Fl_Input*) _edit;
                auto val2  = input->value();
                char buffer[100];

                if (rend == flw::TableEditor::REND::INTEGER) {
                    snprintf(buffer, 100, "%d", (int) util::to_int(val2));
                    val2 = buffer;
                }
                else if (rend == flw::TableEditor::REND::NUMBER) {
                    auto num = util::to_double(val2);
                    snprintf(buffer, 100, "%f", num);
                    val2 = buffer;
                }

                if (val2 == 0) {
                    stop = true;
                    save = false;
                }
                else if (strcmp(val, val2) == 0) {
                    stop = true;
                    save = false;
                }
                else {
                    stop = cell_value(_curr_row, _curr_col, val2);
                }
            }
            else if (rend == flw::TableEditor::REND::BOOLEAN) {
                auto button = (Fl_Check_Button*) _edit;
                auto val2   = "0";

                if (button->value())
                    val2 = "1";

                if (strcmp(val, val2) == 0) {
                    stop = true;
                    save = false;
                }
                else {
                    stop = cell_value(_curr_row, _curr_col, val2);
                }
            }
            else if (rend == flw::TableEditor::REND::SLIDER || rend == flw::TableEditor::REND::VALUE_SLIDER) {
                auto slider = (Fl_Slider*) _edit;
                auto val2   = FormatSlider(slider->value(), slider->minimum(), slider->maximum(), slider->step());

                if (strcmp(val, val2) == 0) {
                    stop = true;
                    save = false;
                }
                else {
                    stop = cell_value(_curr_row, _curr_col, val2);
                }
            }
            else if (rend == flw::TableEditor::REND::CHOICE) {
                auto choice = (Fl_Choice*) _edit;
                auto val2   = choice->text();

                if (val2 == 0) {
                    stop = true;
                    save = false;
                }
                else if (strcmp(val, val2) == 0) {
                    stop = true;
                    save = false;
                }
                else {
                    stop = cell_value(_curr_row, _curr_col, val2);
                }
            }
            else if (rend == flw::TableEditor::REND::INPUT_CHOICE) {
                auto input_choice = (Fl_Input_Choice*) _edit;
                auto val2         = input_choice->value();

                if (val2 == 0) {
                    stop = true;
                    save = false;
                }
                else if (strcmp(val, val2) == 0) {
                    stop = true;
                    save = false;
                }
                else {
                    stop = cell_value(_curr_row, _curr_col, val2);
                }
            }
        }

        if (stop == true) {
            remove(_edit);
            delete _edit;

            _edit            = nullptr;
            _edit2           = nullptr;
            _edit3           = nullptr;
            _current_cell[0] = 0;
            _current_cell[1] = 0;
            _current_cell[2] = 0;
            _current_cell[3] = 0;

            if (_send_changed_event_always == true || save == true) {
                _set_event(_curr_row, _curr_col, flw::TableEditor::EVENT::CHANGED);
                do_callback();
            }

            Fl::check();
            Fl::focus(this);
            fl_cursor(FL_CURSOR_DEFAULT);
            redraw();
        }
        else {
            fl_beep();
        }
    }
}

//------------------------------------------------------------------------------
// Change value without editing
// Use +/- to change value (+/- 1 for integers)
// Use alt + "+|-" to change value (+/-10 for integers)
// Use alt + shift + "+|-" to change value (+/-100 for integers)
//
int flw::TableEditor::_ev_keyboard_down() {
    auto key   = Fl::event_key();
    auto text  = std::string(Fl::event_text());
    auto alt   = Fl::event_alt() != 0;
    auto cmd   = Fl::event_command() != 0;
    auto shift = Fl::event_shift() != 0;

    // printf("key=%d <%s>, alt=%d, cmd=%d, shift=%d\n", key, text.c_str(), alt, cmd, shift); fflush(stdout);

    if (_edit) {
        if (key == FL_Enter || key == FL_KP_Enter) {
            _edit_stop();
            return 1;
        }
        else if (key == FL_Escape) {
            _edit_stop(false);
            return 1;
        }
    }
    else {
        if (key == FL_Enter || key == FL_KP_Enter) {
            _edit_start();
            return 1;
        }
        else if (cmd == true && key == FL_BackSpace) {
            _delete_current_cell();
            return 1;
        }
        else if (key == FL_Delete) {
            _delete_current_cell();
        }
        else if (cmd == true && key == 'x') {
            auto val = ((TableDisplay*) this)->cell_value(_curr_row, _curr_col);

            Fl::copy(val, strlen(val), 1);
            _delete_current_cell();
            return 1;
        }
        else if (cmd == true && key == 'v') {
            Fl::paste(*this, 1);
            return 1;
        }
        else if (alt == true && shift == true && (key == '+' || text == "+" || key == FL_KP + '+')) { // !!! Problems with shift as it changes key depending on layout
            _edit_start("+++");
            return 1;
        }
        else if (alt == true && (key == '+' || text == "+" || key == FL_KP + '+')) {
            _edit_start("++");
            return 1;
        }
        else if (key == '+' || text == "+" || key == FL_KP + '+') {
            _edit_start("+");
            return 1;
        }
        else if (alt == true && shift == true && (key == '-' || text == "-" || key == FL_KP + '-')) { // !!! Problems with shift as it changes key depending on layout
            _edit_start("---");
            return 1;
        }
        else if (alt == true && (key == '-' || text == "-" || key == FL_KP + '-')) {
            _edit_start("--");
            return 1;
        }
        else if (key == '-' || text == "-" || key == FL_KP + '-') {
            _edit_start("-");
            return 1;
        }
    }

    return 0;
}

//------------------------------------------------------------------------------
int flw::TableEditor::_ev_mouse_click () {
    auto row         = 0;
    auto col         = 0;
    auto current_row = _curr_row;
    auto current_col = _curr_col;

    _get_cell_below_mouse(row, col);

    if (_edit && (row != current_row || col != current_col)) {
        _edit_stop();
        return 1;
    }
    else if (row == current_row && col == current_col && Fl::event_clicks() == 1 && Fl::event_is_click()) {
        _edit_start();
        return 1;
    }

    return 2;
}

//------------------------------------------------------------------------------
int flw::TableEditor::_ev_paste() {
    auto text = Fl::event_text();

    if (_curr_row > 0 && _curr_col > 0 && text && *text) {
        auto        rend = cell_rend(_curr_row, _curr_col);
        auto        val  = ((TableDisplay*) this)->cell_value(_curr_row, _curr_col);
        char        buffer[100];
        std::string string;

        switch(rend) {
            case flw::TableEditor::REND::CHOICE:
            case flw::TableEditor::REND::DLG_LIST:
            case flw::TableEditor::REND::SLIDER:
            case flw::TableEditor::REND::VALUE_SLIDER:
                return 1;

            case flw::TableEditor::REND::DLG_DIR:
            case flw::TableEditor::REND::DLG_FILE:
            case flw::TableEditor::REND::INPUT_CHOICE:
            case flw::TableEditor::REND::SECRET:
            case flw::TableEditor::REND::TEXT:
                break;

            case flw::TableEditor::REND::BOOLEAN:
                if (strcmp("1", text) == 0 || strcmp("true", text) == 0) {
                    text = "1";
                }
                else if (strcmp("0", text) == 0 || strcmp("false", text) == 0) {
                    text = "0";
                }
                else {
                    return 1;
                }

            case flw::TableEditor::REND::DLG_COLOR:
            case flw::TableEditor::REND::INTEGER:
                if (*text < '0' || *text > '9') {
                    return 1;
                }
                else {
                    auto num = util::to_int(text);
                    snprintf(buffer, 100, "%lld", (long long int) num);
                    text = buffer;

                    if (rend == flw::TableEditor::REND::DLG_COLOR && (num < 0 || num > 255)) {
                        return 1;
                    }
                    else {
                        break;
                    }
                }

            case flw::TableEditor::REND::NUMBER: {
                if (*text < '0' || *text > '9') {
                    return 1;
                }
                else {
                    auto num = util::to_double(text);
                    snprintf(buffer, 100, "%f", num);
                    text = buffer;
                    break;
                }
            }

            case flw::TableEditor::REND::DLG_DATE: {
                auto date = Date::FromString(text);

                if (date.year() == 1 && date.month() == 1 && date.day() == 1) {
                    return 1;
                }
                else {
                    string = date.format(Date::FORMAT::ISO_LONG);
                    text = string.c_str();
                    break;
                }
            }
        }

        if ((_send_changed_event_always == true || strcmp(val, text) != 0) && cell_value(_curr_row, _curr_col, text) == true) {
            _set_event(_curr_row, _curr_col, flw::TableEditor::EVENT::CHANGED);
            do_callback();
            redraw();
        }
    }

    return 1;
}

//------------------------------------------------------------------------------
const char* flw::TableEditor::FormatSlider(double val, double min, double max, double step) {
    static char result[200];

    snprintf(result, 200, "%.4f %.4f %.4f %.4f", val, min, max, step);
    return result;
}

//------------------------------------------------------------------------------
int flw::TableEditor::handle(int event) {
    auto ret = 2;

    if (_rows > 0 && _cols > 0) {
        if (event == FL_UNFOCUS) {
            if (_edit && Fl::focus() && Fl::focus()->parent()) {
                if ((_edit2 == nullptr && Fl::focus() != _edit) || (_edit2 && Fl::focus() != _edit2 && Fl::focus() != _edit3)) {
                    _edit_stop();
                }
            }
        }
        else if (event == FL_PASTE) {
            ret = _ev_paste();
        }
        else if (event == FL_KEYDOWN) {
            ret = _ev_keyboard_down();
        }
        else if (event == FL_PUSH) {
            ret = _ev_mouse_click();
        }
    }

    if (ret == 1) {
        return ret;
    }
    else {
        return TableDisplay::handle(event);
    }
}



namespace flw {
    //--------------------------------------------------------------------------
    class _TabsGroupButton : public Fl_Toggle_Button {
    public:
        static int                      _BORDER;
        static Fl_Boxtype               _BOXTYPE;
        static Fl_Color                 _BOXSELCOLOR;
        static Fl_Color                 _BOXCOLOR;

        int _tw;

        //----------------------------------------------------------------------
        _TabsGroupButton(const char* label) : Fl_Toggle_Button(0, 0, 0, 0) {
            _tw = 0;
            copy_label(label);
            copy_tooltip(label);
        }

        //----------------------------------------------------------------------
        void draw() override {
            fl_draw_box(_BOXTYPE, x(), y(), w(), h(), value() ? _TabsGroupButton::_BOXSELCOLOR : _TabsGroupButton::_BOXCOLOR);
            fl_font(flw::PREF_FONT, flw::PREF_FONTSIZE);
            fl_color(FL_FOREGROUND_COLOR);
            fl_draw(label(), x() + 3, y(), w() - 6, h(), FL_ALIGN_LEFT | FL_ALIGN_CLIP);
        }
    };

    int        _TabsGroupButton::_BORDER      = 6;
    Fl_Boxtype _TabsGroupButton::_BOXTYPE     = FL_FLAT_BOX;
    Fl_Color   _TabsGroupButton::_BOXSELCOLOR = FL_SELECTION_COLOR;
    Fl_Color   _TabsGroupButton::_BOXCOLOR    = FL_DARK1;
}

//------------------------------------------------------------------------------
flw::TabsGroup::TabsGroup(int X, int Y, int W, int H, const char* l) : Fl_Group(X, Y, W, H, l) {
    end();
    clip_children(1);

    _active = -1;
    _drag   = false;
    _hide   = false;
    _pos    = flw::PREF_FONTSIZE * 10;
    _tabs   = TABS::NORTH;
}

//------------------------------------------------------------------------------
void flw::TabsGroup::add(const std::string& label, Fl_Widget* widget) {
    auto button = new _TabsGroupButton(label.c_str());

    button->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
    button->box(FL_THIN_UP_BOX);
    button->callback(flw::TabsGroup::Callback, this);
    button->when(FL_WHEN_CHANGED);

    Fl_Group::add(widget);
    Fl_Group::add(button);

    if (_active + 1 >= (int) _widgets.size()) {
        _widgets.push_back(widget);
        _buttons.push_back(button);
    }
    else {
        _widgets.insert(_widgets.begin() + _active + 1, widget);
        _buttons.insert(_buttons.begin() + _active + 1, button);
    }

    flw::TabsGroup::Callback(button, this);
}

//------------------------------------------------------------------------------
void flw::TabsGroup::BoxColor(Fl_Color boxcolor) {
    flw::_TabsGroupButton::_BOXCOLOR = boxcolor;
}

//------------------------------------------------------------------------------
void flw::TabsGroup::BoxSelectionColor(Fl_Color boxcolor) {
    flw::_TabsGroupButton::_BOXSELCOLOR = boxcolor;
}

//------------------------------------------------------------------------------
void flw::TabsGroup::BoxType(Fl_Boxtype boxtype) {
    flw::_TabsGroupButton::_BOXTYPE = boxtype;
}

//------------------------------------------------------------------------------
Fl_Widget* flw::TabsGroup::_button() {
    return (_active >= 0 && _active < (int) _buttons.size()) ? _buttons[_active] : nullptr;
}

//------------------------------------------------------------------------------
void flw::TabsGroup::Callback(Fl_Widget* sender, void* object) {
    if (sender) {
        auto button = (_TabsGroupButton*) sender;
        auto self   = (TabsGroup*) object;
        auto count  = 0;

        self->_active = -1;

        for (auto b : self->_buttons) {
            if (b == button) {
                ((Fl_Button*) b)->value(1);
                self->_active = count;
                self->_widgets[count]->show();
            }
            else {
                ((Fl_Button*) b)->value(0);
                self->_widgets[count]->hide();
            }

            count++;
        }

        if (self->value() != nullptr && Fl::focus() != self->value()) {
            self->value()->take_focus();
        }

        self->resize();
    }
}

//------------------------------------------------------------------------------
Fl_Widget* flw::TabsGroup::child(int num) const {
    return (num >= 0 && num < (int) _widgets.size()) ? _widgets[num] : nullptr;
}

//------------------------------------------------------------------------------
int flw::TabsGroup::find(Fl_Widget* widget) const{
    auto num = 0;

    for (auto w : _widgets) {
        if (w == widget) {
            return num;
        }
        else {
            num++;
        }
    }

    return -1;
}

//------------------------------------------------------------------------------
int flw::TabsGroup::handle(int event) {
    if (_widgets.size() == 0) {
        return Fl_Group::handle(event);
    }

    if (_tabs == TABS::WEST || _tabs == TABS::EAST) {
        if (event == FL_DRAG) {
            if (_drag == true) {
                auto pos = 0;

                if (_tabs == TABS::WEST) {
                    pos = Fl::event_x() - x();
                }
                else {
                    pos = x() + w() - Fl::event_x();
                }

                if (pos != _pos) {
                    _pos = pos;
                    resize();
                }

                return 1;
            }
        }
        else if (event == FL_LEAVE) {
            _drag = false;
            fl_cursor(FL_CURSOR_DEFAULT);
        }
        else if (event == FL_MOVE) {
            auto event_x = Fl::event_x();

            if (_tabs == TABS::WEST) {
                auto pos = x() + _pos;

                if (event_x > (pos - 3) && event_x <= (pos + 3)) {
                    if (_drag == false) {
                        _drag = true;
                        fl_cursor(FL_CURSOR_WE);
                    }

                    return 1;
                }
            }
            else {
                auto pos = x() + w() - _pos;

                if (event_x > (pos - 3) && event_x <= (pos + 3)) {
                    if (_drag == false) {
                        _drag = true;
                        fl_cursor(FL_CURSOR_WE);
                    }

                    return 1;
                }
            }

            if (_drag == true) {
                _drag = false;
                fl_cursor(FL_CURSOR_DEFAULT);
            }
        }
        else if (event == FL_PUSH) {
            if (_drag == true) {
                return 1;
            }
        }
    }

    if (_buttons.size() > 1) {
        if (event == FL_KEYBOARD) {
            auto key   = Fl::event_key();
            auto alt   = Fl::event_alt() != 0;
            auto shift = Fl::event_shift() != 0;

            if (alt == true && key >= '0' && key <= '9') {
                auto tab = key - '0';
                tab = (tab == 0) ? 9 : tab - 1;

                if (tab < (int) _buttons.size()) {
                    flw::TabsGroup::Callback(_buttons[tab], this);
                }
                return 1;
            }
            else if (alt == true && shift == true && key == FL_Left) {
                swap(_active, _active - 1);
                flw::TabsGroup::Callback(_button(), this);
                return 1;
            }
            else if (alt == true && shift == true && key == FL_Right) {
                swap(_active, _active + 1);
                flw::TabsGroup::Callback(_button(), this);
                return 1;
            }
            else if (alt == true && key == FL_Left) {
                _active = _active == 0 ? (int) _widgets.size() - 1 : _active - 1;
                flw::TabsGroup::Callback(_button(), this);
                return 1;
            }
            else if (alt == true && key == FL_Right) {
                _active = _active == (int) _widgets.size() - 1 ? 0 : _active + 1;
                flw::TabsGroup::Callback(_button(), this);
                return 1;
            }
        }
    }

    if (event == FL_FOCUS) {
        if (value() != nullptr && Fl::focus() != value()) {
            value()->take_focus();
            return 1;
        }
    }

    return Fl_Group::handle(event);
}

//------------------------------------------------------------------------------
void flw::TabsGroup::_hide_tab_buttons(bool hide) {
    _hide = hide;

    for (auto b : _buttons) {
        if (_hide == true) {
            b->hide();
        }
        else {
            b->show();
        }
    }

    resize();
}

//------------------------------------------------------------------------------
void flw::TabsGroup::label(const std::string& label, Fl_Widget* widget) {
    auto num = find(widget);

    if (num != -1) {
        _buttons[num]->copy_label(label.c_str());
        _buttons[num]->copy_tooltip(label.c_str());
    }
}

//------------------------------------------------------------------------------
Fl_Widget* flw::TabsGroup::remove(int num) {
    if (num >= 0 && num < (int) _widgets.size()) {
        auto w = _widgets[num];
        auto b = _buttons[num];

        _widgets.erase(_widgets.begin() + num);
        _buttons.erase(_buttons.begin() + num);

        remove(w);
        remove(b);
        delete b;

        if (num < _active) {
            _active--;
        }
        else if (_active == (int) _widgets.size()) {
            _active = (int) _widgets.size() - 1;
        }

        flw::TabsGroup::Callback(_button(), this);
        return w;
    }
    else {
        return nullptr;
    }
}

//------------------------------------------------------------------------------
void flw::TabsGroup::resize(int X, int Y, int W, int H) {
    Fl_Widget::resize(X, Y, W, H);

    if (W == 0 || H == 0) {
        return;
    }

    if (visible() == 0 || _widgets.size() == 0) {
        return;
    }

    auto height = flw::PREF_FONTSIZE + 8;

    if (_hide == true) {
        for (auto w : _widgets) { // Resize widgets
            if (w->visible() == 0) {
            }
            else {
                w->resize(X, Y, W, H);
            }
        }

        return;
    }

    if (_tabs == TABS::NORTH || _tabs == TABS::SOUTH) {
        auto space = 6;
        auto x     = 0;
        auto w     = 0;
        auto th    = 0;

        fl_font(flw::PREF_FONT, flw::PREF_FONTSIZE);

        for (auto b : _buttons) { // Calc total width of buttons
            b->_tw = 0;
            fl_measure(b->label(), b->_tw, th);

            b->_tw += _TabsGroupButton::_BORDER;
            w      += b->_tw + space;
        }

        if (w > W) { // If width is too large then divide equal
            w = (W - (_buttons.size() * 4)) / _buttons.size();
        }
        else {
            w = 0;
        }

        for (auto b : _buttons) { // Resize buttons
            auto bw = (w != 0) ? w : b->_tw;

            if (_tabs == TABS::NORTH) {
                b->resize(X + x, Y, bw, height);
            }
            else {
                b->resize(X + x, Y + H - height, bw, height);
            }

            x += bw;
            x += space;
        }
    }
    else { // TABS::WEST, TABS::EAST
        auto y        = Y;
        auto h        = height;
        auto shrinked = false;
        auto space    = 3;

        if ((h + space) * (int) _buttons.size() > H) { // Shrink button height if any falls outside area
            h = (H - _buttons.size()) / _buttons.size();
            shrinked = true;
        }

        if (_pos < flw::PREF_FONTSIZE * space) { // Set min size for widgets on the left
            _pos = flw::PREF_FONTSIZE * space;
        }
        else if (_pos > W - flw::PREF_FONTSIZE * 8) { // Set min size for widgets on the right
            _pos = W - flw::PREF_FONTSIZE * 8;
        }

        for (auto b : _buttons) {
            if (_tabs == TABS::WEST) {
                b->resize(X, y, _pos, h);
                y += h + (shrinked == false ? space : 1);
            }
            else {
                b->resize(X + W - _pos, y, _pos, h);
                y += h + (shrinked == false ? space : 1);
            }
        }
    }

    for (auto w : _widgets) { // Resize widgets
        if (w->visible() == 0) {
        }
        else if (_tabs == TABS::NORTH) {
            w->resize(X, Y + height, W, H - height);
        }
        else if (_tabs == TABS::SOUTH) {
            w->resize(X, Y, W, H - height);
        }
        else if (_tabs == TABS::WEST) {
            w->resize(X + _pos, Y, W - _pos, H);
        }
        else if (_tabs == TABS::EAST) {
            w->resize(X, Y, W - _pos, H);
        }
    }
}

//------------------------------------------------------------------------------
void flw::TabsGroup::swap(int from, int to) {
    auto last = (int) _widgets.size() - 1;

    if (_widgets.size() < 2 || to < -1 || to > (int) _widgets.size()) {
        return;
    }
    else {
        bool active = (_active == from);

        if (from == 0 && to == -1) {
            auto widget = _widgets[0];
            auto button = _buttons[0];

            for (int f = 1; f <= last; f++) {
                _widgets[f - 1] = _widgets[f];
                _buttons[f - 1] = _buttons[f];
            }

            from           = last;
            _widgets[from] = widget;
            _buttons[from] = button;
        }
        else if (from == last && to == (int) _widgets.size()) {
            auto widget = _widgets[last];
            auto button = _buttons[last];

            for (int f = last - 1; f >= 0; f--) {
                _widgets[f + 1] = _widgets[f];
                _buttons[f + 1] = _buttons[f];
            }

            from           = 0;
            _widgets[from] = widget;
            _buttons[from] = button;
        }
        else {
            auto widget = _widgets[from];
            auto button = _buttons[from];

            _widgets[from] = _widgets[to];
            _buttons[from] = _buttons[to];
            _widgets[to]   = widget;
            _buttons[to]   = button;
            from           = to;
        }

        if (active) {
            _active = from;
        }

        resize();
    }
}

//------------------------------------------------------------------------------
Fl_Widget* flw::TabsGroup::value() const {
    return (_active >= 0 && _active < (int) _widgets.size()) ? _widgets[_active] : nullptr;
}

//------------------------------------------------------------------------------
void flw::TabsGroup::value(int num) {
    if (num >= 0 && num < (int) _buttons.size()) {
        flw::TabsGroup::Callback(_buttons[num], this);
    }
}



namespace flw {
    bool        PREF_IS_DARK        = false;
    int         PREF_FIXED_FONT     = FL_COURIER;
    std::string PREF_FIXED_FONTNAME = "FL_COURIER";
    int         PREF_FIXED_FONTSIZE = 14;
    int         PREF_FONT           = FL_HELVETICA;
    int         PREF_FONTSIZE       = 14;
    std::string PREF_FONTNAME       = "FL_HELVETICA";

    namespace color {
        Fl_Color RED     = fl_rgb_color(255, 0, 0);
        Fl_Color LIME    = fl_rgb_color(0, 255, 0);
        Fl_Color BLUE    = fl_rgb_color(0, 0, 255);
        Fl_Color YELLOW  = fl_rgb_color(255, 255, 0);
        Fl_Color CYAN    = fl_rgb_color(0, 255, 255);
        Fl_Color MAGENTA = fl_rgb_color(255, 0, 255);
        Fl_Color SILVER  = fl_rgb_color(192, 192, 192);
        Fl_Color GRAY    = fl_rgb_color(128, 128, 128);
        Fl_Color MAROON  = fl_rgb_color(128, 0, 0);
        Fl_Color OLIVE   = fl_rgb_color(128, 128, 0);
        Fl_Color GREEN   = fl_rgb_color(0, 128, 0);
        Fl_Color PURPLE  = fl_rgb_color(128, 0, 128);
        Fl_Color TEAL    = fl_rgb_color(0, 128, 128);
        Fl_Color NAVY    = fl_rgb_color(0, 0, 128);
        Fl_Color BROWN   = fl_rgb_color(210, 105, 30);
        Fl_Color PINK    = fl_rgb_color(255, 192, 203);
        Fl_Color BEIGE   = fl_rgb_color(245, 245, 220);
        Fl_Color AZURE   = fl_rgb_color(240, 255, 250);
    }

    namespace theme {
        static const char* _NAMES[] = {
            "default",
            "gleam",
            "blue gleam",
            "dark blue gleam",
            "dark gleam",
            "darker gleam",
            "tan gleam",
            "gtk",
            "blue gtk",
            "dark blue gtk",
            "dark gtk",
            "darker gtk",
            "tan gtk",
            "plastic",
            "blue plastic",
            "tan plastic",
            "system"
        };

        enum {
            _NAME_DEFAULT,
            _NAME_GLEAM,
            _NAME_GLEAM_BLUE,
            _NAME_GLEAM_DARK_BLUE,
            _NAME_GLEAM_DARK,
            _NAME_GLEAM_DARKER,
            _NAME_GLEAM_TAN,
            _NAME_GTK,
            _NAME_GTK_BLUE,
            _NAME_GTK_DARK_BLUE,
            _NAME_GTK_DARK,
            _NAME_GTK_DARKER,
            _NAME_GTK_TAN,
            _NAME_PLASTIC,
            _NAME_PLASTIC_BLUE,
            _NAME_PLASTIC_TAN,
            _NAME_SYSTEM,
        };

        static unsigned char _OLD_R[256]  = { 0 };
        static unsigned char _OLD_G[256]  = { 0 };
        static unsigned char _OLD_B[256]  = { 0 };
        static unsigned char _SYS_R[256]  = { 0 };
        static unsigned char _SYS_G[256]  = { 0 };
        static unsigned char _SYS_B[256]  = { 0 };
        static bool          _SAVED_COLOR = false;
        static bool          _SAVED_SYS   = false;
        static std::string   _NAME        = _NAMES[_NAME_DEFAULT];

        //----------------------------------------------------------------------
        // Colors are reset every time a new theme has been selected
        //!!! Colors are same for dark and light for now
        //
        static void _colors(bool dark) {
            (void) dark;

            color::AZURE   = fl_rgb_color(240, 255, 250);
            color::BEIGE   = fl_rgb_color(245, 245, 220);
            color::BLUE    = fl_rgb_color(0, 0, 255);
            color::BLUE    = fl_rgb_color(0, 0, 255);
            color::BROWN   = fl_rgb_color(210, 105, 30);
            color::CYAN    = fl_rgb_color(0, 255, 255);
            color::GRAY    = fl_rgb_color(128, 128, 128);
            color::GREEN   = fl_rgb_color(0, 128, 0);
            color::LIME    = fl_rgb_color(0, 255, 0);
            color::MAGENTA = fl_rgb_color(255, 0, 255);
            color::MAROON  = fl_rgb_color(128, 0, 0);
            color::NAVY    = fl_rgb_color(0, 0, 128);
            color::OLIVE   = fl_rgb_color(128, 128, 0);
            color::PINK    = fl_rgb_color(255, 192, 203);
            color::PURPLE  = fl_rgb_color(128, 0, 128);
            color::RED     = fl_rgb_color(255, 0, 0);
            color::SILVER  = fl_rgb_color(192, 192, 192);
            color::TEAL    = fl_rgb_color(0, 128, 128);
            color::YELLOW  = fl_rgb_color(255, 255, 0);
        }

        //----------------------------------------------------------------------
        static void _make_default_colors_darker() {
            Fl::set_color(60,    0,  63,   0); // FL_DARK_GREEN
            Fl::set_color(63,    0, 110,   0); // FL_GREEN
            Fl::set_color(72,   55,   0,   0); // FL_DARK_RED
            Fl::set_color(76,   55,  63,   0); // FL_DARK_YELLOW
            Fl::set_color(88,  110,   0,   0); // FL_RED
            Fl::set_color(95,  140, 140, 100); // FL_YELLOW
            Fl::set_color(136,   0,   0,  55); // FL_DARK_BLUE
            Fl::set_color(140,   0,  63,  55); // FL_DARK_CYAN
            Fl::set_color(152,  55,   0,  55); // FL_DARK_MAGENTA
            Fl::set_color(216,   0,   0, 110); // FL_BLUE
            Fl::set_color(223,   0, 110, 110); // FL_CYAN
            Fl::set_color(248, 110,   0, 110); // FL_MAGENTA
        }

        //----------------------------------------------------------------------
        static void _blue_colors() {
            Fl::set_color(0,     0,   0,   0); // FL_FOREGROUND_COLOR
            Fl::set_color(7,   255, 255, 255); // FL_BACKGROUND2_COLOR
            Fl::set_color(8,   130, 149, 166); // FL_INACTIVE_COLOR
            Fl::set_color(15,  255, 160,  63); // FL_SELECTION_COLOR
            Fl::set_color(255, 244, 244, 244); // FL_WHITE
            Fl::background(170, 189, 206);
        }

        //----------------------------------------------------------------------
        static void _dark_blue_colors() {
            Fl::set_color(0,   255, 255, 255); // FL_FOREGROUND_COLOR
            Fl::set_color(7,    31,  47,  55); // FL_BACKGROUND2_COLOR
            Fl::set_color(8,   108, 113, 125); // FL_INACTIVE_COLOR
            Fl::set_color(15,   68, 138, 255); // FL_SELECTION_COLOR
            Fl::set_color(56,    0,   0,   0); // FL_BLACK
            Fl::background(64, 80, 87);
        }

        //----------------------------------------------------------------------
        static void _dark_colors() {
            Fl::set_color(0,   240, 240, 240); // FL_FOREGROUND_COLOR
            Fl::set_color(7,    55,  55,  55); // FL_BACKGROUND2_COLOR
            Fl::set_color(8,   100, 100, 100); // FL_INACTIVE_COLOR
            Fl::set_color(15,  149,  75,  37); // FL_SELECTION_COLOR
            Fl::set_color(56,    0,   0,   0); // FL_BLACK
            Fl::background(85, 85, 85);
            Fl::background(64, 64, 64);
        }

        //----------------------------------------------------------------------
        static void _darker_colors() {
            Fl::set_color(0,   164, 164, 164); // FL_FOREGROUND_COLOR
            Fl::set_color(7,    16,  16,  16); // FL_BACKGROUND2_COLOR
            Fl::set_color(7,    28,  28,  28); // FL_BACKGROUND2_COLOR
            Fl::set_color(8,   100, 100, 100); // FL_INACTIVE_COLOR
            Fl::set_color(15,  133,  59,  21); // FL_SELECTION_COLOR
            Fl::set_color(56,    0,   0,   0); // FL_BLACK
            Fl::background(32, 32, 32);
            Fl::background(38, 38, 38);
        }

        //----------------------------------------------------------------------
        static void _tan_colors() {
            Fl::set_color(0,     0,   0,   0); // FL_FOREGROUND_COLOR
            Fl::set_color(7,   255, 255, 255); // FL_BACKGROUND2_COLOR
            Fl::set_color(8,    85,  85,  85); // FL_INACTIVE_COLOR
            Fl::set_color(15,  255, 160,  63); // FL_SELECTION_COLOR
            Fl::set_color(255, 244, 244, 244); // FL_WHITE
            Fl::background(206, 202, 187);
        }

        //----------------------------------------------------------------------
        static void _restore_colors() {
            if (_SAVED_COLOR == true) {
                for (int f = 0; f < 256; f++) {
                    Fl::set_color(f, flw::theme::_OLD_R[f], flw::theme::_OLD_G[f], flw::theme::_OLD_B[f]);
                }
            }
        }

        //----------------------------------------------------------------------
        static void _restore_sys() {
            if (_SAVED_SYS == true) {
                for (int f = 0; f < 256; f++) {
                    Fl::set_color(f, flw::theme::_SYS_R[f], flw::theme::_SYS_G[f], flw::theme::_SYS_B[f]);
                }
            }
        }

        //----------------------------------------------------------------------
        static void _save_colors() {
            if (_SAVED_COLOR == false) {
                for (int f = 0; f < 256; f++) {
                    unsigned char r1, g1, b1;
                    Fl::get_color(f, r1, g1, b1);
                    flw::theme::_OLD_R[f] = r1;
                    flw::theme::_OLD_G[f] = g1;
                    flw::theme::_OLD_B[f] = b1;
                }

                _SAVED_COLOR = true;
            }
        }

        //----------------------------------------------------------------------
        static void _save_sys() {
            if (_SAVED_SYS == false) {
                for (int f = 0; f < 256; f++) {
                    unsigned char r1, g1, b1;
                    Fl::get_color(f, r1, g1, b1);
                    flw::theme::_SYS_R[f] = r1;
                    flw::theme::_SYS_G[f] = g1;
                    flw::theme::_SYS_B[f] = b1;
                }

                _SAVED_SYS = true;
            }
        }

        //------------------------------------------------------------------------------
        static void _load_default() {
            flw::theme::_save_colors();
            flw::theme::_restore_colors();
            flw::theme::_colors(false);
            Fl::set_color(FL_SELECTION_COLOR, 0, 120, 200);
            Fl::scheme("none");
            Fl::redraw();
            _NAME = _NAMES[_NAME_DEFAULT];
            flw::PREF_IS_DARK = false;
        }

        //------------------------------------------------------------------------------
        static void _load_gleam() {
            flw::theme::_save_colors();
            flw::theme::_restore_colors();
            flw::theme::_colors(false);
            Fl::scheme("gleam");
            Fl::set_color(FL_SELECTION_COLOR, 0, 120, 200);
            Fl::redraw();
            _NAME = _NAMES[_NAME_GLEAM];
            flw::PREF_IS_DARK = false;
        }

        //------------------------------------------------------------------------------
        static void _load_gleam_blue() {
            flw::theme::_save_colors();
            flw::theme::_restore_colors();
            flw::theme::_blue_colors();
            flw::theme::_colors(false);
            Fl::scheme("gleam");
            Fl::redraw();
            _NAME = _NAMES[_NAME_GLEAM_BLUE];
            flw::PREF_IS_DARK = false;
        }

        //------------------------------------------------------------------------------
        static void _load_gleam_dark() {
            flw::theme::_save_colors();
            flw::theme::_restore_colors();
            flw::theme::_dark_colors();
            flw::theme::_make_default_colors_darker();
            flw::theme::_colors(true);
            Fl::set_color(255, 125, 125, 125);
            Fl::scheme("gleam");
            Fl::redraw();
            _NAME = _NAMES[_NAME_GLEAM_DARK];
            flw::PREF_IS_DARK = true;
        }

        //------------------------------------------------------------------------------
        static void _load_gleam_darker() {
            flw::theme::_save_colors();
            flw::theme::_restore_colors();
            flw::theme::_darker_colors();
            flw::theme::_make_default_colors_darker();
            flw::theme::_colors(true);
            Fl::set_color(255, 125, 125, 125);
            Fl::scheme("gleam");
            Fl::redraw();
            _NAME = _NAMES[_NAME_GLEAM_DARKER];
            flw::PREF_IS_DARK = true;
        }

        //------------------------------------------------------------------------------
        static void _load_gleam_dark_blue() {
            flw::theme::_save_colors();
            flw::theme::_restore_colors();
            flw::theme::_dark_blue_colors();
            flw::theme::_make_default_colors_darker();
            flw::theme::_colors(true);
            Fl::set_color(255, 101, 117, 125);
            Fl::scheme("gleam");
            Fl::redraw();
            _NAME = _NAMES[_NAME_GLEAM_DARK_BLUE];
            flw::PREF_IS_DARK = true;
        }

        //------------------------------------------------------------------------------
        static void _load_gleam_tan() {
            flw::theme::_save_colors();
            flw::theme::_restore_colors();
            flw::theme::_tan_colors();
            flw::theme::_colors(false);
            Fl::scheme("gleam");
            Fl::redraw();
            _NAME = _NAMES[_NAME_GLEAM_TAN];
            flw::PREF_IS_DARK = false;
        }

        //------------------------------------------------------------------------------
        static void _load_gtk() {
            flw::theme::_save_colors();
            flw::theme::_restore_colors();
            flw::theme::_colors(false);
            Fl::scheme("gtk+");
            Fl::set_color(FL_SELECTION_COLOR, 0, 120, 200);
            Fl::redraw();
            _NAME = _NAMES[_NAME_GTK];
            flw::PREF_IS_DARK = false;
        }

        //------------------------------------------------------------------------------
        static void _load_gtk_blue() {
            flw::theme::_save_colors();
            flw::theme::_restore_colors();
            flw::theme::_blue_colors();
            flw::theme::_colors(false);
            Fl::scheme("gtk+");
            Fl::redraw();
            _NAME = _NAMES[_NAME_GTK_BLUE];
            flw::PREF_IS_DARK = false;
        }

        //------------------------------------------------------------------------------
        static void _load_gtk_dark() {
            flw::theme::_save_colors();
            flw::theme::_restore_colors();
            flw::theme::_dark_colors();
            flw::theme::_make_default_colors_darker();
            flw::theme::_colors(true);
            Fl::set_color(255, 185, 185, 185);
            Fl::scheme("gtk+");
            Fl::redraw();
            _NAME = _NAMES[_NAME_GTK_DARK];
            flw::PREF_IS_DARK = true;
        }

        //------------------------------------------------------------------------------
        static void _load_gtk_darker() {
            flw::theme::_save_colors();
            flw::theme::_restore_colors();
            flw::theme::_darker_colors();
            flw::theme::_make_default_colors_darker();
            flw::theme::_colors(true);
            Fl::set_color(255, 125, 125, 125);
            Fl::scheme("gtk+");
            Fl::redraw();
            _NAME = _NAMES[_NAME_GTK_DARKER];
            flw::PREF_IS_DARK = true;
        }

        //------------------------------------------------------------------------------
        static void _load_gtk_dark_blue() {
            flw::theme::_save_colors();
            flw::theme::_restore_colors();
            flw::theme::_dark_blue_colors();
            flw::theme::_make_default_colors_darker();
            flw::theme::_colors(true);
            Fl::set_color(255, 161, 177, 185);
            Fl::scheme("gtk+");
            Fl::redraw();
            _NAME = _NAMES[_NAME_GTK_DARK_BLUE];
            flw::PREF_IS_DARK = true;
        }

        //------------------------------------------------------------------------------
        static void _load_gtk_tan() {
            flw::theme::_save_colors();
            flw::theme::_restore_colors();
            flw::theme::_tan_colors();
            flw::theme::_colors(false);
            Fl::scheme("gtk+");
            Fl::redraw();
            _NAME = _NAMES[_NAME_GTK_TAN];
            flw::PREF_IS_DARK = false;
        }

        //------------------------------------------------------------------------------
        static void _load_plastic() {
            flw::theme::_save_colors();
            flw::theme::_restore_colors();
            flw::theme::_colors(false);
            Fl::set_color(FL_SELECTION_COLOR, 0, 120, 200);
            Fl::scheme("plastic");
            Fl::redraw();
            _NAME = _NAMES[_NAME_PLASTIC];
            flw::PREF_IS_DARK = false;
        }

        //------------------------------------------------------------------------------
        static void _load_blue_plastic() {
            flw::theme::_save_colors();
            flw::theme::_restore_colors();
            flw::theme::_blue_colors();
            flw::theme::_colors(false);
            Fl::scheme("plastic");
            Fl::redraw();
            _NAME = _NAMES[_NAME_PLASTIC_BLUE];
            flw::PREF_IS_DARK = false;
        }

        //------------------------------------------------------------------------------
        static void _load_tan_plastic() {
            flw::theme::_save_colors();
            flw::theme::_restore_colors();
            flw::theme::_tan_colors();
            flw::theme::_colors(false);
            Fl::scheme("plastic");
            Fl::redraw();
            _NAME = _NAMES[_NAME_PLASTIC_TAN];
            flw::PREF_IS_DARK = false;
        }

        //------------------------------------------------------------------------------
        static void _load_system() {
            flw::theme::_save_colors();
            flw::theme::_restore_colors();
            flw::theme::_colors(false);
            Fl::scheme("gtk+");

            if (theme::_SAVED_SYS) {
              flw::theme::_restore_sys();
            }
            else {
              Fl::get_system_colors();
              flw::theme::_save_sys();
            }

            Fl::redraw();
            _NAME = _NAMES[_NAME_SYSTEM];
            flw::PREF_IS_DARK = false;
        }
    }

    namespace dlg {
        //----------------------------------------------------------------------
        class _DlgTheme : public Fl_Double_Window {
            Fl_Box*                     _font_label;
            Fl_Box*                     _fixedfont_label;
            Fl_Browser*                 _theme;
            Fl_Button*                  _close;
            Fl_Button*                  _font;
            Fl_Button*                  _fixedfont;
            int                         _theme_row;

        public:
            //------------------------------------------------------------------
            _DlgTheme(bool enable_font, bool enable_fixedfont, Fl_Window* parent) : Fl_Double_Window(0, 0, 0, 0, "Set Theme") {
                end();

                _close           = new Fl_Return_Button(0, 0, 0, 0, "&Close");
                _fixedfont       = new Fl_Button(0, 0, 0, 0, "F&ixed Font");
                _fixedfont_label = new Fl_Box(0, 0, 0, 0);
                _font            = new Fl_Button(0, 0, 0, 0, "&Font");
                _font_label      = new Fl_Box(0, 0, 0, 0);
                _theme           = new Fl_Hold_Browser(0, 0, 0, 0);
                _theme_row       = 0;

                add(_theme);
                add(_fixedfont);
                add(_font);
                add(_close);
                add(_fixedfont_label);
                add(_font_label);

                if (enable_font == false) {
                  _font->deactivate();
                }

                if (enable_fixedfont == false) {
                  _fixedfont->deactivate();
                }

                _close->callback(Callback, this);
                _font_label->color(FL_BACKGROUND2_COLOR);
                _font_label->box(FL_DOWN_BOX);
                _font->callback(Callback, this);
                _font_label->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
                _fixedfont_label->color(FL_BACKGROUND2_COLOR);
                _fixedfont_label->box(FL_DOWN_BOX);
                _fixedfont->callback(Callback, this);
                _fixedfont_label->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
                _theme->callback(Callback, this);
                _theme->textfont(flw::PREF_FONT);

                for (int f = 0; f <= flw::theme::_NAME_SYSTEM; f++) {
                    _theme->add(theme::_NAMES[f]);
                }

                resizable(this);
                callback(Callback, this);
                set_modal();
                update_pref();
                util::center_window(this, parent);
            }

            //------------------------------------------------------------------
            static void Callback(Fl_Widget* w, void* o) {
                auto dlg = (_DlgTheme*) o;

                if (w == dlg) {
                    dlg->hide();
                }
                else if (w == dlg->_fixedfont) {
                    flw::dlg::FontDialog fd(flw::PREF_FIXED_FONT, flw::PREF_FIXED_FONTSIZE, "Select Monospaced Font");

                    if (fd.run(Fl::first_window()) == true) {
                        flw::PREF_FIXED_FONT     = fd.font();
                        flw::PREF_FIXED_FONTSIZE = fd.fontsize();
                        flw::PREF_FIXED_FONTNAME = fd.fontname();
                        dlg->update_pref();
                    }
                }
                else if (w == dlg->_font) {
                    flw::dlg::FontDialog fd(flw::PREF_FONT, flw::PREF_FONTSIZE, "Select Font");

                    if (fd.run(Fl::first_window()) == true) {
                        flw::PREF_FONT     = fd.font();
                        flw::PREF_FONTSIZE = fd.fontsize();
                        flw::PREF_FONTNAME = fd.fontname();
                        dlg->update_pref();
                    }
                }
                else if (w == dlg->_theme) {
                    auto row = dlg->_theme->value() - 1;

                    if (row == flw::theme::_NAME_GLEAM) {
                        flw::theme::_load_gleam();
                    }
                    else if (row == flw::theme::_NAME_GLEAM_BLUE) {
                        flw::theme::_load_gleam_blue();
                    }
                    else if (row == flw::theme::_NAME_GLEAM_DARK_BLUE) {
                        flw::theme::_load_gleam_dark_blue();
                    }
                    else if (row == flw::theme::_NAME_GLEAM_DARK) {
                        flw::theme::_load_gleam_dark();
                    }
                    else if (row == flw::theme::_NAME_GLEAM_DARKER) {
                        flw::theme::_load_gleam_darker();
                    }
                    else if (row == flw::theme::_NAME_GLEAM_TAN) {
                        flw::theme::_load_gleam_tan();
                    }
                    else if (row == flw::theme::_NAME_GTK) {
                        flw::theme::_load_gtk();
                    }
                    else if (row == flw::theme::_NAME_GTK_BLUE) {
                        flw::theme::_load_gtk_blue();
                    }
                    else if (row == flw::theme::_NAME_GTK_DARK_BLUE) {
                        flw::theme::_load_gtk_dark_blue();
                    }
                    else if (row == flw::theme::_NAME_GTK_DARK) {
                        flw::theme::_load_gtk_dark();
                    }
                    else if (row == flw::theme::_NAME_GTK_DARKER) {
                        flw::theme::_load_gtk_darker();
                    }
                    else if (row == flw::theme::_NAME_GTK_TAN) {
                        flw::theme::_load_gtk_tan();
                    }
                    else if (row == flw::theme::_NAME_PLASTIC) {
                        flw::theme::_load_plastic();
                    }
                    else if (row == flw::theme::_NAME_PLASTIC_BLUE) {
                        flw::theme::_load_blue_plastic();
                    }
                    else if (row == flw::theme::_NAME_PLASTIC_TAN) {
                        flw::theme::_load_tan_plastic();
                    }
                    else if (row == flw::theme::_NAME_SYSTEM) {
                        flw::theme::_load_system();
                    }
                    else {
                        flw::theme::_load_default();
                    }

                    Fl::redraw();
                }
                else if (w == dlg->_close) {
                    dlg->hide();
                }
            }

            //------------------------------------------------------------------
            void resize(int X, int Y, int W, int H) override {
                auto fs = flw::PREF_FONTSIZE;

                Fl_Double_Window::resize(X, Y, W, H);

                _theme->resize           (4,                 4,                  W - 8,     H - fs * 6 - 24);
                _font_label->resize      (4,                 H - fs * 6 - 16,    W - 8,     fs * 2);
                _fixedfont_label->resize (4,                 H - fs * 4 - 12,    W - 8,     fs * 2);
                _font->resize            (W - fs * 24 - 12,  H - fs * 2 - 4,     fs * 8,    fs * 2);
                _fixedfont->resize       (W - fs * 16 - 8,   H - fs * 2 - 4,     fs * 8,    fs * 2);
                _close->resize           (W - fs * 8 - 4,    H - fs * 2 - 4,     fs * 8,    fs * 2);
            }

            //------------------------------------------------------------------
            void run() {
                show();

                while (visible()) {
                    Fl::wait();
                    Fl::flush();
                }
            }

            //------------------------------------------------------------------
            void update_pref() {
                flw::theme::labelfont(this);
                _font_label->copy_label(flw::util::format("%s - %d", flw::PREF_FONTNAME.c_str(), flw::PREF_FONTSIZE).c_str());
                _fixedfont_label->copy_label(flw::util::format("%s - %d", flw::PREF_FIXED_FONTNAME.c_str(), flw::PREF_FIXED_FONTSIZE).c_str());
                _fixedfont_label->labelfont(flw::PREF_FIXED_FONT);
                _fixedfont_label->labelsize(flw::PREF_FIXED_FONTSIZE);
                _theme->textsize(flw::PREF_FONTSIZE);
                size(flw::PREF_FONTSIZE * 32, flw::PREF_FONTSIZE * 30);

                for (int f = 0; f <= flw::theme::_NAME_SYSTEM; f++) {
                    if (theme::_NAME == flw::theme::_NAMES[f]) {
                        _theme->value(f + 1);
                        break;
                    }

                }

                Fl::redraw();
            }
        };
    }
}

//------------------------------------------------------------------------------
void flw::dlg::theme(bool enable_font, bool enable_fixedfont, Fl_Window* parent) {
    auto dlg = dlg::_DlgTheme(enable_font, enable_fixedfont, parent);
    dlg.run();
}

//------------------------------------------------------------------------------
bool flw::theme::is_dark() {
    if (theme::_NAME == flw::theme::_NAMES[_NAME_GLEAM_DARK_BLUE] ||
        flw::theme::_NAME == flw::theme::_NAMES[_NAME_GLEAM_DARK] ||
        flw::theme::_NAME == flw::theme::_NAMES[_NAME_GLEAM_DARKER] ||
        flw::theme::_NAME == flw::theme::_NAMES[_NAME_GTK_DARK_BLUE] ||
        flw::theme::_NAME == flw::theme::_NAMES[_NAME_GTK_DARK] ||
        flw::theme::_NAME == flw::theme::_NAMES[_NAME_GTK_DARKER]) {
        return true;
    }
    else {
        return false;
    }
}

//------------------------------------------------------------------------------
// Set label font properties for widget
// If widget is an group widget set also the font for child widgets (recursive)
//
void flw::theme::labelfont(Fl_Widget* widget) {
    assert(widget);

    widget->labelfont(flw::PREF_FONT);
    widget->labelsize(flw::PREF_FONTSIZE);

    auto group = widget->as_group();

    if (group != nullptr) {
        for (auto f = 0; f < group->children(); f++) {
            flw::theme::labelfont(group->child(f));
        }
    }
}

//------------------------------------------------------------------------------
bool flw::theme::load(std::string name) {
    if (name == flw::theme::_NAMES[_NAME_DEFAULT]) {
        flw::theme::_load_default();
    }
    else if (name == flw::theme::_NAMES[_NAME_GLEAM]) {
        flw::theme::_load_gleam();
    }
    else if (name == flw::theme::_NAMES[_NAME_GLEAM_BLUE]) {
        flw::theme::_load_gleam_blue();
    }
    else if (name == flw::theme::_NAMES[_NAME_GLEAM_DARK_BLUE]) {
        flw::theme::_load_gleam_dark_blue();
    }
    else if (name == flw::theme::_NAMES[_NAME_GLEAM_DARK]) {
        flw::theme::_load_gleam_dark();
    }
    else if (name == flw::theme::_NAMES[_NAME_GLEAM_DARKER]) {
        flw::theme::_load_gleam_darker();
    }
    else if (name == flw::theme::_NAMES[_NAME_GLEAM_TAN]) {
        flw::theme::_load_gleam_tan();
    }
    else if (name == flw::theme::_NAMES[_NAME_GTK]) {
        flw::theme::_load_gtk();
    }
    else if (name == flw::theme::_NAMES[_NAME_GTK_BLUE]) {
        flw::theme::_load_gtk_blue();
    }
    else if (name == flw::theme::_NAMES[_NAME_GTK_DARK_BLUE]) {
        flw::theme::_load_gtk_dark_blue();
    }
    else if (name == flw::theme::_NAMES[_NAME_GTK_DARK]) {
        flw::theme::_load_gtk_dark();
    }
    else if (name == flw::theme::_NAMES[_NAME_GTK_DARKER]) {
        flw::theme::_load_gtk_darker();
    }
    else if (name == flw::theme::_NAMES[_NAME_GTK_TAN]) {
        flw::theme::_load_gtk_tan();
    }
    else if (name == flw::theme::_NAMES[_NAME_PLASTIC]) {
        flw::theme::_load_plastic();
    }
    else if (name == flw::theme::_NAMES[_NAME_PLASTIC_BLUE]) {
        flw::theme::_load_blue_plastic();
    }
    else if (name == flw::theme::_NAMES[_NAME_PLASTIC_TAN]) {
        flw::theme::_load_tan_plastic();
    }
    else {
        return false;
    }

    return true;
}


//------------------------------------------------------------------------------
// Load icon before showing window
//
void flw::theme::load_icon(Fl_Window* win, int win_resource, const char** xpm_resource, const char* name) {
    assert(win);

    if (win->shown() != 0) {
        fl_alert("%s", "warning: load icon before showing window!");
    }

#if defined(_WIN32)
    win->icon((char*) LoadIcon(fl_display, MAKEINTRESOURCE(win_resource)));
    (void) name;
    (void) xpm_resource;
    (void) name;
#elif defined(__linux__)
    assert(xpm_resource);

    Fl_Pixmap    pix(xpm_resource);
    Fl_RGB_Image rgb(&pix, Fl_Color(0));

    win->icon(&rgb);
    win->xclass((name != nullptr) ? name : "FLTK");
    (void) win_resource;
#else
    (void) win;
    (void) win_resource;
    (void) xpm_resource;
    (void) name;
#endif
}

//------------------------------------------------------------------------------
// Load theme and font data
//
void flw::theme::load_theme_pref(Fl_Preferences& pref) {
    auto val = 0;
    char buffer[4000];

    pref.get("fontname", buffer, "", 4000);

    if (*buffer != 0 && strcmp("FL_HELVETICA", buffer) != 0) {
        auto font = dlg::FontDialog::LoadFont(buffer);

        if (font != -1) {
            flw::PREF_FONT     = font;
            flw::PREF_FONTNAME = buffer;
        }
    }

    pref.get("fontsize", val, flw::PREF_FONTSIZE);

    if (val >= 6 && val <= 72) {
        flw::PREF_FONTSIZE = val;
    }

    pref.get("fixedfontname", buffer, "", 1000);

    if (*buffer != 0 && strcmp("FL_COURIER", buffer) != 0) {
        auto font = dlg::FontDialog::LoadFont(buffer);

        if (font != -1) {
            flw::PREF_FIXED_FONT     = font;
            flw::PREF_FIXED_FONTNAME = buffer;
        }
    }

    pref.get("fixedfontsize", val, flw::PREF_FIXED_FONTSIZE);

    if (val >= 6 && val <= 72) {
        flw::PREF_FIXED_FONTSIZE = val;
    }

    pref.get("theme", buffer, "gtk", 4000);
    flw::theme::load(buffer);
}

//------------------------------------------------------------------------------
// Load window size
//
void flw::theme::load_win_pref(Fl_Preferences& pref, Fl_Window* window, bool show, int defw, int defh, std::string basename) {
    assert(window);

    int  x, y, w, h, f;

    pref.get((basename + "x").c_str(), x, 80);
    pref.get((basename + "y").c_str(), y, 60);
    pref.get((basename + "w").c_str(), w, defw);
    pref.get((basename + "h").c_str(), h, defh);
    pref.get((basename + "fullscreen").c_str(), f, 0);

    if (w == 0 || h == 0) {
        w = 800;
        h = 600;
    }

    if (x + w <= 0 || y + h <= 0 || x >= Fl::w() || y >= Fl::h()) {
        x = 80;
        y = 60;
    }

    window->resize(x, y, w, h);

    if (show == true) {
        window->show();
        window->resize(x, y, w, h);
    }

    if (f == 1) {
        window->fullscreen();
    }
}

//------------------------------------------------------------------------------
std::string flw::theme::name() {
    return _NAME;
}

//------------------------------------------------------------------------------
bool flw::theme::parse(int argc, const char** argv) {
    auto res = false;

    for (auto f = 1; f < argc; f++) {
        if (res == false) {
            res = flw::theme::load(argv[f]);
        }

        auto fontsize = util::to_int(argv[f]);

        if (fontsize >= 6 && fontsize <= 72) {
            flw::PREF_FONTSIZE = fontsize;
        }
    }

    flw::PREF_FIXED_FONTSIZE = flw::PREF_FONTSIZE;
    return res;
}

//------------------------------------------------------------------------------
// Save theme and font data
//
void flw::theme::save_theme_pref(Fl_Preferences& pref) {
    pref.set("theme", flw::theme::name().c_str());
    pref.set("fontname", flw::PREF_FONTNAME.c_str());
    pref.set("fontsize", flw::PREF_FONTSIZE);
    pref.set("fixedfontname", flw::PREF_FIXED_FONTNAME.c_str());
    pref.set("fixedfontsize", flw::PREF_FIXED_FONTSIZE);
}

//------------------------------------------------------------------------------
// Save window size
//
void flw::theme::save_win_pref(Fl_Preferences& pref, Fl_Window* window, std::string basename) {
    assert(window);

    pref.set((basename + "x").c_str(), window->x());
    pref.set((basename + "y").c_str(), window->y());
    pref.set((basename + "w").c_str(), window->w());
    pref.set((basename + "h").c_str(), window->h());
    pref.set((basename + "fullscreen").c_str(), window->fullscreen_active() ? 1 : 0);
}



namespace flw {
    //--------------------------------------------------------------------------
    static Fl_Menu_Item* _util_menu_item(Fl_Menu_* menu, const char* text) {
        assert(menu && text);

        auto item = menu->find_item(text);

#ifdef DEBUG
        if (item == nullptr) {
            fprintf(stderr, "error: cant find menu item <%s>\n", text);
        }
#endif

        return (Fl_Menu_Item*) item;
    }
}

//------------------------------------------------------------------------------
char* flw::util::allocate(size_t size, bool terminate) {
    auto res = (char*) calloc(size + 1, 1);

    if (res == nullptr && terminate == true) {
        fl_alert("error: failed to allocate memory with size %lld\nI'm going to quit now!", (long long int) size);
        exit(1);
    }

    return res;
}

//------------------------------------------------------------------------------
void flw::util::center_window(Fl_Window* window, Fl_Window* parent) {
    if (parent != nullptr) {
        int x = parent->x() + parent->w() / 2;
        int y = parent->y() + parent->h() / 2;
        window->resize(x - window->w() / 2, y - window->h() / 2, window->w(), window->h());
    }
    else {
        window->resize((Fl::w() / 2) - (window->w() / 2), (Fl::h() / 2) - (window->h() / 2), window->w(), window->h());
    }
}

//------------------------------------------------------------------------------
int flw::util::count_decimals(double number) {
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
std::string flw::util::fix_menu_string(std::string in) {
    std::string res = in;

    util::replace(res, "\\", "\\\\");
    util::replace(res, "_", "\\_");
    util::replace(res, "/", "\\/");
    util::replace(res, "&", "&&");

    return res;
}

//------------------------------------------------------------------------------
std::string flw::util::format(const char* format, ...) {
    assert(format);

    std::string res;
    int         l = 100;
    int         n = 0;
    char*       s = (char*) calloc(100 + 1, 1);

    if (s == nullptr) {
        return res;
    }

    while (true) {
        va_list args;

        va_start(args, format);
        n = vsnprintf(s, l, format, args);
        va_end(args);

        if (n == -1) {
            s[0] = 0;
            break;
        }
        else if (n >= l) {
            l = n + 1;
            char* tmp = (char*) realloc(s, l);

            if (tmp == nullptr) {
                free(s);
                return res;
            }

            s = tmp;
        }
        else {
            break;
        }
    }

    res = s;
    free(s);
    return res;
}

//------------------------------------------------------------------------------
std::string flw::util::format_double(double number, int decimals, char sep) {
    char res[100];

    *res = 0;

    if (decimals < 0) {
        decimals = util::count_decimals(number);
    }

    if (decimals == 0) {
        return util::format_int((int64_t) number, sep);
    }

    if (fabs(number) < 9223372036854775807.0) {
        char fr_str[100];
        auto int_num    = (int64_t) fabs(number);
        auto double_num = (double) (fabs(number) - int_num);
        auto int_str    = util::format_int(int_num, sep);
        auto len        = snprintf(fr_str, 99, "%.*f", decimals, double_num);

        if (len > 0 && len < 100) {
            if (number < 0.0) {
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
std::string flw::util::format_int(int64_t number, char sep) {
    auto pos = 0;
    char tmp1[100];
    char tmp2[100];

    if (sep < 32) {
        sep = 32;
    }

    snprintf(tmp1, 100, "%lld", (long long int) llabs(number));
    auto len = (int) strlen(tmp1);

    for (auto f = len - 1, c = 0; f >= 0 && pos < 100; f--, c++) {
        if ((c % 3) == 0 && c > 0) {
            tmp2[pos] = sep;
            pos++;
        }

        tmp2[pos] = tmp1[f];
        pos++;
    }

    if (number < 0) {
        tmp2[pos] = '-';
        pos++;
    }

    tmp2[pos] = 0;
    len = strlen(tmp2);

    for (auto f = 0; f < len / 2; f++) {
        auto c = tmp2[f];
        tmp2[f] = tmp2[len - f - 1];
        tmp2[len - f - 1] = c;
    }

    return tmp2;
}

//------------------------------------------------------------------------------
flw::Buf flw::util::load_file(std::string filename, bool alert) {
    auto stat = flw::Stat(filename);

    if (stat.mode != 2) {
        if (alert == true) {
            fl_alert("error: file %s is missing or not an file", filename.c_str());
        }

        return Buf();
    }

    auto file = fopen(filename.c_str(), "rb");

    if (file == nullptr) {
        if (alert == true) {
            fl_alert("error: can't open %s", filename.c_str());
        }

        return Buf();
    }

    auto buf  = Buf(stat.size);
    auto read = fread(buf.p, 1, (size_t) stat.size, file);

    fclose(file);

    if (read != (size_t) stat.size) {
        if (alert == true) {
            fl_alert("error: failed to read %s", filename.c_str());
        }

        free(buf.p);
        return Buf();
    }

    return buf;
}

//------------------------------------------------------------------------------
void flw::util::menu_item_enable(Fl_Menu_* menu, const char* text, bool value) {
    assert(menu && text);

    auto item = flw::_util_menu_item(menu, text);

    if (item == nullptr) {
        return;
    }

    if (value == true) {
        item->activate();
    }
    else {
        item->deactivate();
    }
}

//------------------------------------------------------------------------------
Fl_Menu_Item* flw::util::menu_item_get(Fl_Menu_* menu, const char* text) {
    assert(menu && text);

    return flw::_util_menu_item(menu, text);
}

//------------------------------------------------------------------------------
void flw::util::menu_item_set(Fl_Menu_* menu, const char* text, bool value) {
    assert(menu && text);

    auto item = flw::_util_menu_item(menu, text);

    if (item == nullptr) {
        return;
    }

    if (value == true) {
        item->set();
    }
    else {
        item->clear();
    }
}

//------------------------------------------------------------------------------
void flw::util::menu_item_set_only(Fl_Menu_* menu, const char* text) {
    assert(menu && text);

    auto item = flw::_util_menu_item(menu, text);

    if (item == nullptr) {
        return;
    }

    menu->setonly(item);
}

//------------------------------------------------------------------------------
bool flw::util::menu_item_value(Fl_Menu_* menu, const char* text) {
    assert(menu && text);

    auto item = flw::_util_menu_item(menu, text);

    if (item == nullptr) {
        return false;
    }

    return item->value();
}

//------------------------------------------------------------------------------
// Must be compiled with FLW_USE_PNG flag and linked with fltk images (fltk-config --ldflags --use-images)
// If filename is empty you will be asked for the name
//
void flw::util::png_save(std::string opt_name, Fl_Window* window, int X, int Y, int W, int H) {
#ifdef FLW_USE_PNG
#if FL_MINOR_VERSION == 4
    auto filename = (opt_name == "") ? fl_file_chooser("Save To PNG File", "All Files (*)\tPNG Files (*.png)", "") : opt_name.c_str();

    if (filename != nullptr) {
        window->make_current();

        if (X == 0 && Y == 0 && W == 0 && H == 0) {
            X = window->x();
            Y = window->y();
            W = window->w();
            H = window->h();
        }

        auto image = fl_read_image(nullptr, X, Y, W, H);


        if (image != nullptr) {
            auto ret = fl_write_png(filename, image, W, H);

            if (ret == 0) {
            }
            else if (ret == -1) {
                fl_alert("%s", "error: missing libraries");
            }
            else if (ret == -1) {
                fl_alert("error: failed to save image to %s", filename);
            }

            delete []image;
        }
        else {
            fl_alert("%s", "error: failed to grab image");
        }
    }
#else
    (void) opt_name;
    (void) window;
    (void) X;
    (void) Y;
    (void) W;
    (void) H;
    fl_alert("error: does not work with fltk 1.3");
#endif
#else
    (void) opt_name;
    (void) window;
    (void) X;
    (void) Y;
    (void) W;
    (void) H;
    fl_alert("error: flw not compiled with FLW_USE_PNG flag");
#endif
}

//------------------------------------------------------------------------------
void flw::util::print(Fl_Widget* widget, bool tab) {
    assert(widget);

    printf("%s%-*s| x=%4d, y=%4d, w=%4d, h=%4d\n", tab ? "    " : "", tab ? 16 : 20, widget->label() ? widget->label() : "NO_LABEL",  widget->x(),  widget->y(),  widget->w(),  widget->h());
    fflush(stdout);
}

//------------------------------------------------------------------------------
void flw::util::print(Fl_Group* group) {
    assert(group);
    puts("");
    util::print((Fl_Widget*) group);

    for (int f = 0; f < group->children(); f++) {
        util::print(group->child(f), true);
    }
}

//------------------------------------------------------------------------------
// Replace string and return number of replacements or -1 for error
//
int flw::util::replace(std::string& string, std::string find, std::string replace) {
    auto index = (size_t) 0;
    auto count = (size_t) 0;

    try {
        if (string.size() > 0 && find.size() > 0) {
            while (true) {
                index = string.find(find, index);

                if (index == std::string::npos) {
                    break;
                }

                string.replace(index, find.size(), replace);
                count++;
                index += replace.size();
            }

            string.shrink_to_fit();
        }

        return (int) count;
    }
    catch(...) {
        return -1;
    }
}

//------------------------------------------------------------------------------
bool flw::util::save_file(std::string filename, const void* data, size_t size, bool alert) {
    auto file = fl_fopen(filename.c_str(), "wb");

    if (file != nullptr) {
        auto wrote = fwrite(data, 1, size, file);
        fclose(file);

        if (wrote != size) {
            if (alert == true) {
                fl_alert("error: saving data to %s failed", filename.c_str());
            }

            return false;
        }
    }
    else if (alert == true) {
        fl_alert("error: failed to open %s", filename.c_str());
        return false;
    }

    return true;
}

//------------------------------------------------------------------------------
// Split string and return an vector with splitted strings
//
flw::StringVector flw::util::split(const std::string& string, std::string split) {
    auto res = StringVector();

    try {
        if (split != "") {
            auto pos1 = (std::string::size_type) 0;
            auto pos2 = string.find(split);

            while (pos2 != std::string::npos) {
                res.push_back(string.substr(pos1, pos2 - pos1));
                pos1 = pos2 + split.size();
                pos2 = string.find(split, pos1);
            }

            if (pos1 <= string.size()) {
                res.push_back(string.substr(pos1));
            }
        }
    }
    catch(...) {
        res.clear();
    }

    return res;
}

//------------------------------------------------------------------------------
bool flw::util::test(bool val, int line, const char* func) {
    if (val == false) {
        fprintf(stderr, "error: test failed at line %d in %s\n", line, func);
        fflush(stderr);
        return false;
    }

    return true;
}

//------------------------------------------------------------------------------
bool flw::util::test(const char* ref, const char* val, int line, const char* func) {
    if (ref == nullptr && val == nullptr) {
        return true;
    }
    else if (ref == nullptr || val == nullptr || strcmp(ref, val) != 0) {
        fprintf(stderr, "error: test failed '%s' != '%s' at line %d in %s\n", ref ? ref : "NULL", val ? val : "NULL", line, func);
        fflush(stderr);
        return false;
    }

    return true;
}

//------------------------------------------------------------------------------
bool flw::util::test(int64_t ref, int64_t val, int line, const char* func) {
    if (ref != val) {
        fprintf(stderr, "error: test failed '%lld' != '%lld' at line %d in %s\n", (long long int) ref, (long long int) val, line, func);
        fflush(stderr);
        return false;
    }

    return true;
}

//------------------------------------------------------------------------------
bool flw::util::test(double ref, double val, double diff, int line, const char* func) {
    if (fabs(ref - val) > diff) {
        fprintf(stderr, "error: test failed '%f' != '%f' at line %d in %s\n", ref, val, line, func);
        fflush(stderr);
        return false;
    }

    return true;
}

//------------------------------------------------------------------------------
// Return time as seconds since 1970
//
double flw::util::time() {
    auto sec   = 0.0;
    auto milli = 0.0;

#ifdef _WIN32
    struct __timeb64 timeVal;

    _ftime64(&timeVal);
    sec   = (double) timeVal.time;
    milli = (double) timeVal.millitm;
    return sec + (milli / 1000.0);
#else
    struct timeb timeVal;

    ftime(&timeVal);
    sec   = (double) timeVal.time;
    milli = (double) timeVal.millitm;
    return sec + (milli / 1000.0);
#endif
}

//------------------------------------------------------------------------------
// Return time stamp
//
int64_t flw::util::time_micro() {
#if defined(_WIN32)
    LARGE_INTEGER StartingTime;
    LARGE_INTEGER Frequency;

    QueryPerformanceFrequency(&Frequency);
    QueryPerformanceCounter(&StartingTime);

    StartingTime.QuadPart *= 1000000;
    StartingTime.QuadPart /= Frequency.QuadPart;
    return StartingTime.QuadPart;
#else
    timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return t.tv_sec * 1000000 + t.tv_nsec / 1000;
#endif
}

//------------------------------------------------------------------------------
// Return time stamp
//
int32_t flw::util::time_milli() {
    return (int) (util::time_micro() / 1000);
}

//------------------------------------------------------------------------------
void flw::util::time_sleep(int milli) {
#ifdef _WIN32
    Sleep(milli);
#else
    usleep(milli * 1000);
#endif
}

//------------------------------------------------------------------------------
double flw::util::to_double(std::string string, double def) {
    try {
        return std::stod(string.c_str(), 0);
    }
    catch (...) {
        return def;
    }
}

//------------------------------------------------------------------------------
// Convert double numbers in string to actual numbers
// Return number of set doubles
//
int flw::util::to_doubles(std::string string, double numbers[], size_t size) {
    auto end = (char*) nullptr;
    auto in  = string.c_str();
    auto f   = (size_t) 0;

    errno = 0;

    for (; f < size; f++) {
        numbers[f] = strtod(in, &end);

        if (errno != 0 || in == end) {
            return f;
        }

        in = end;
    }

    return f;
}

//------------------------------------------------------------------------------
int64_t flw::util::to_int(std::string string, int64_t def) {
    try {
        return std::stoll(string.c_str(), 0, 0);
    }
    catch (...) {
        return def;
    }
}

//------------------------------------------------------------------------------
// Return number of set integers
//
int flw::util::to_ints(std::string string, int64_t numbers[], size_t size) {
    auto end = (char*) nullptr;
    auto in  = string.c_str();
    auto f   = (size_t) 0;

    errno = 0;

    for (; f < size; f++) {
        numbers[f] = strtoll(in, &end, 10);

        if (errno != 0 || in == end) {
            return f;
        }

        in = end;
    }

    return f;
}

//------------------------------------------------------------------------------
// Returns new converted buffer if it does contain \r
// Otherwise it returns nullptr
//
char* flw::util::win_to_unix(const char* string) {
    auto r = false;
    auto b = string;

    while (*b != 0) {
        if (*b++ == '\r') {
            r = true;
            break;
        }
    }

    if (r == false) {
        return nullptr;
    }

    auto len = strlen(string);
    auto res = util::allocate(len);
    auto pos = 0;

    b = string;

    while (*b != 0) {
        if (*b != '\r') {
            res[pos++] = *b;
        }

        b++;
    }

    return res;
}

//------------------------------------------------------------------------------
void* flw::util::zero_memory(char* string) {
    assert(string);

    if (string == nullptr) {
        return nullptr;
    }

    return util::zero_memory(string, strlen(string));
}

//------------------------------------------------------------------------------
void* flw::util::zero_memory(void* mem, size_t size) {
    if (mem == nullptr || size == 0) {
        return mem;
    }

#ifdef _WIN32
    RtlSecureZeroMemory(mem, size);
#else
    auto p = (volatile unsigned char*) mem;

    while (size--) {
        *p = 0;
        p++;
    }
#endif

    return mem;
}

//------------------------------------------------------------------------------
void* flw::util::zero_memory(std::string& string) {
    return util::zero_memory((char*) string.data());
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
flw::Buf::Buf(size_t size) {
    p = util::allocate(size, 1);
    s = size;
}

//------------------------------------------------------------------------------
flw::Buf::Buf(const char* buffer, size_t size) {
    p = util::allocate(size, 1);
    s = size;

    assert(buffer != p);
    memcpy(p, buffer, s);
}

//------------------------------------------------------------------------------
flw::Buf::Buf(const Buf& other) {
    p = nullptr;
    s = 0;

    if (other.p != nullptr && other.s > 0) {
        s = other.s;
        p = util::allocate(s);

        assert(other.p != p);
        memcpy(p, other.p, s);
    }
}

//------------------------------------------------------------------------------
flw::Buf::Buf(Buf&& other) {
    s       = other.s;
    p       = other.p;
    other.p = nullptr;
}

//------------------------------------------------------------------------------
flw::Buf& flw::Buf::operator=(const Buf& other) {
    assert(other.p != p);

    free(p);
    p = nullptr;
    s = 0;

    if (other.p != nullptr && other.s > 0) {
        s = other.s;
        p = util::allocate(s);

        assert(other.p != p);
        memcpy(p, other.p, s);
    }

    return *this;
}

//------------------------------------------------------------------------------
flw::Buf& flw::Buf::operator=(Buf&& other) {
    free(p);

    s       = other.s;
    p       = other.p;
    other.p = nullptr;

    return *this;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
flw::Stat::Stat(std::string filename) {
    size  = 0;
    mtime = 0;
    mode  = 0;

#ifdef _WIN32
    wchar_t wbuffer[1025];
    struct __stat64 st;

    while (filename.empty() == false && (filename.back() == '\\' || filename.back() == '/')) {
        filename.pop_back();
    }

    fl_utf8towc	(filename.c_str(), filename.length(), wbuffer, 1024);

    if (_wstat64(wbuffer, &st) == 0) {
        size  = st.st_size;
        mtime = st.st_mtime;

        if (S_ISDIR(st.st_mode)) {
            mode = 1;
        }
        else if (S_ISREG(st.st_mode)) {
            mode = 2;
        }
        else {
            mode = 3;
        }
    }
#else
    struct stat st;

    if (stat(filename.c_str(), &st) == 0) {
        size  = st.st_size;
        mtime = st.st_mtime;

        if (S_ISDIR(st.st_mode)) {
            mode = 1;
        }
        else if (S_ISREG(st.st_mode)) {
            mode = 2;
        }
        else {
            mode = 3;
        }
    }
#endif
}



flw::WaitCursor* flw::WaitCursor::WAITCURSOR = nullptr;

//------------------------------------------------------------------------------
flw::WaitCursor::WaitCursor() {
    if (WaitCursor::WAITCURSOR == nullptr) {
        WaitCursor::WAITCURSOR = this;
        fl_cursor(FL_CURSOR_WAIT);
        Fl::redraw();
        Fl::flush();
    }
}

//------------------------------------------------------------------------------
flw::WaitCursor::~WaitCursor() {
    if (WaitCursor::WAITCURSOR == this) {
        WaitCursor::WAITCURSOR = nullptr;
        fl_cursor(FL_CURSOR_DEFAULT);
    }
}



namespace flw {
    static const std::string _INPUTMENU_TOOLTIP       = "Use up/down arrows to switch between previous values\nPress ctrl + space to open menu button (if visible)";
    static const std::string _SCROLLBROWSER_MENU_ALL  = "Copy All Lines";
    static const std::string _SCROLLBROWSER_MENU_LINE = "Copy Current Line";
    static const std::string _SCROLLBROWSER_TOOLTIP   = "Right click to show the menu";

    //--------------------------------------------------------------------------
    class _InputMenu : public Fl_Input {
    public:

        bool            show_menu;
        int             index;
        StringVector    history;

        //----------------------------------------------------------------------
        _InputMenu() : Fl_Input(0, 0, 0, 0) {
            tooltip(_INPUTMENU_TOOLTIP.c_str());

            index     = -1;
            show_menu = false;
        }

        //----------------------------------------------------------------------
        int handle(int event) override {
            if (event == FL_KEYBOARD) {
                auto key = Fl::event_key();

                if (Fl::event_ctrl() != 0 && key == ' ') {
                    if (history.size() > 0) {
                        show_menu = true;
                        do_callback();
                    }

                    return 1;
                }
                else if (key == FL_Up && history.size() > 0) {
                    if (index <= 0) {
                        value("");
                        index = -1;
                    }
                    else {
                        index--;
                        show_menu = false;
                        value(history[index].c_str());
                    }

                    return 1;
                }
                else if (key == FL_Down && history.size() > 0 && index < (int) history.size() - 1) {
                    index++;

                    value(history[index].c_str());
                    show_menu = false;
                    return 1;
                }
                else {
                    show_menu = false;
                }
            }

            return Fl_Input::handle(event);
        }
    };
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
flw::InputMenu::InputMenu(int X, int Y, int W, int H, const char* l) : Fl_Group(X, Y, W, H, l) {
    end();

    _input = new flw::_InputMenu();
    _menu  = new Fl_Menu_Button(0, 0, 0, 0);

    Fl_Group::add(_input);
    Fl_Group::add(_menu);

    _input->callback(InputMenu::Callback, this);
    _input->when(FL_WHEN_ENTER_KEY_ALWAYS);
    _menu->callback(InputMenu::Callback, this);
    update_pref();
}

//------------------------------------------------------------------------------
void flw::InputMenu::Callback(Fl_Widget* sender, void* self) {
    auto w = (InputMenu*) self;

    if (sender == w->_input) {
        if (w->_input->show_menu) {
            if (w->_menu->visible()) {
                w->_menu->popup();
            }
        }
        else {
            w->do_callback();
        }
    }
    else if (sender == w->_menu) {
        auto index = w->_menu->find_index(w->_menu->text());

        if (index >= 0 && index < (int) w->_input->history.size()) {
            w->_input->value(w->_input->history[index].c_str());
            w->_input->index = index;
        }

        w->_input->take_focus();
    }
}

//------------------------------------------------------------------------------
void flw::InputMenu::clear() {
    _menu->clear();
    _input->history.clear();
    _input->index = -1;
}

//------------------------------------------------------------------------------
flw::StringVector flw::InputMenu::get_history() const {
    return _input->history;
}

//------------------------------------------------------------------------------
void flw::InputMenu::insert(const std::string& string, int max_list_len) {
    for (auto it = _input->history.begin(); it != _input->history.end(); ++it) {
        if (*it == string) {
            _input->history.erase(it);
            break;
        }
    }

    _input->history.insert(_input->history.begin(), string);

    while ((int) _input->history.size() > max_list_len) {
        _input->history.pop_back();
    }

    _menu->clear();

    for (auto& s : _input->history) {
        _menu->add(flw::util::fix_menu_string(s.c_str()).c_str());
    }

    _input->index = -1;
    _input->value(string.c_str());
    _input->position(string.length(), 0);
}

//------------------------------------------------------------------------------
void flw::InputMenu::resize(int X, int Y, int W, int H) {
    Fl_Group::resize(X, Y, W, H);

    if (_menu->visible()) {
        _input->resize(X, Y, W - flw::PREF_FONTSIZE * 2, H);
        _menu->resize(X + W - flw::PREF_FONTSIZE * 2, Y, flw::PREF_FONTSIZE * 2, H);
    }
    else {
        _input->resize(X, Y, W, H);
    }
}

//------------------------------------------------------------------------------
void flw::InputMenu::set(const StringVector& list, bool copy_first_to_input) {
    clear();
    _input->history = list;

    for (auto& s : _input->history) {
        _menu->add(flw::util::fix_menu_string(s.c_str()).c_str());
    }

    if (list.size() && copy_first_to_input) {
        auto s = list.front();
        _input->value(s.c_str());
        _input->position(s.length(), 0);
    }
}

//------------------------------------------------------------------------------
void flw::InputMenu::update_pref(Fl_Font text_font, Fl_Fontsize text_size) {
    labelfont(flw::PREF_FONT);
    labelsize(flw::PREF_FONTSIZE);
    _input->textfont(text_font);
    _input->textsize(text_size);
    _menu->labelfont(text_font);
    _menu->labelsize(text_size);
    _menu->textfont(text_font);
    _menu->textsize(text_size);
}

//------------------------------------------------------------------------------
const char* flw::InputMenu::value() const {
    return _input->value();
}

//------------------------------------------------------------------------------
void flw::InputMenu::value(const char* string) {
    _input->value(string ? string : "");
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
flw::ScrollBrowser::ScrollBrowser(int scroll, int X, int Y, int W, int H, const char* l) : Fl_Hold_Browser(X, Y, W, H, l) {
    end();

    _menu      = new Fl_Menu_Button(0, 0, 0, 0);
    _scroll    = (scroll > 0) ? scroll : 9;
    _flag_move = true;
    _flag_menu = true;

    ((Fl_Group*) this)->add(_menu);
    _menu->add(_SCROLLBROWSER_MENU_LINE.c_str(), 0, ScrollBrowser::Callback, this);
    _menu->add(_SCROLLBROWSER_MENU_ALL.c_str(), 0, ScrollBrowser::Callback, this);
    _menu->type(Fl_Menu_Button::POPUP3);
    tooltip(_SCROLLBROWSER_TOOLTIP.c_str());
    update_pref();
}

//------------------------------------------------------------------------------
void flw::ScrollBrowser::Callback(Fl_Widget*, void* o) {
    auto self  = (ScrollBrowser*) o;
    auto menu  = self->_menu->text();
    auto label = std::string((menu != nullptr) ? menu : "");
    auto clip  = std::string();

    clip.reserve(self->size() * 40 + 100);

    if (label == _SCROLLBROWSER_MENU_LINE) {
        if (self->value() != 0) {
            clip = ScrollBrowser::RemoveFormat(self->text(self->value()));
        }
    }
    else if (label == _SCROLLBROWSER_MENU_ALL) {
        for (auto f = 1; f <= self->size(); f++) {
            auto s = ScrollBrowser::RemoveFormat(self->text(f));
            clip += s;
            clip += "\n";
        }
    }

    if (clip != "") {
        Fl::copy(clip.c_str(), clip.length(), 2);
    }
}

//------------------------------------------------------------------------------
int flw::ScrollBrowser::handle(int event) {
    if (event == FL_MOUSEWHEEL) {
        if (Fl::event_dy() > 0) {
            topline(topline() + _scroll);
        }
        else if (Fl::event_dy() < 0) {
            topline(topline() - _scroll);
        }

        return 1;
    }
    else if (event == FL_KEYBOARD) {
        if (_flag_move == true) {
            auto key = Fl::event_key();

            if (key == FL_Page_Up) {
                auto line = value();

                if (line == 1) {
                }
                else if (line - _scroll < 1) {
                    value(1);
                    do_callback();
                }
                else {
                    value(line - _scroll);
                    topline(line - _scroll);
                    do_callback();
                }

                return 1;
            }
            else if (key == FL_Page_Down) {
                auto line = value();

                if (line == size()) {
                }
                else if (line + _scroll > size()) {
                    value(size());
                    do_callback();
                }
                else {
                    value(line + _scroll);
                    bottomline(line + _scroll);
                    do_callback();
                }

                return 1;
            }
        }
    }
    else if (event == FL_PUSH) {
        if (_flag_menu == true && Fl::event_button() == FL_RIGHT_MOUSE) {
            _menu->popup();
            return 1;
        }
    }

    return Fl_Hold_Browser::handle(event);
}

//------------------------------------------------------------------------------
std::string flw::ScrollBrowser::RemoveFormat(const char* text) {
    auto res = std::string(text);
    auto f   = res.find_last_of("@");

    if (f != std::string::npos) {
        auto tmp = res.substr(f + 1);

        if (tmp[0] == '.' || tmp[0] == 'l' || tmp[0] == 'm' || tmp[0] == 's' || tmp[0] == 'b' || tmp[0] == 'i' || tmp[0] == 'f' || tmp[0] == 'c' || tmp[0] == 'r' || tmp[0] == 'u' || tmp[0] == '-') {
            res = tmp.substr(1);
        }
        else if (tmp[0] == 'B' || tmp[0] == 'C' || tmp[0] == 'F' || tmp[0] == 'S') {
            auto s = std::string();
            auto e = false;

            tmp = tmp.substr(f + 1);

            for (auto c : tmp) {
                if (e == false && c >= '0' && c <= '9') {
                }
                else {
                    e = true;
                    s += c;
                }
            }

            res = s;
        }
        else {
            res = res.substr(f);
        }
    }

    return res;
}

//------------------------------------------------------------------------------
void flw::ScrollBrowser::update_pref(Fl_Font text_font, Fl_Fontsize text_size) {
    labelfont(flw::PREF_FONT);
    labelsize(flw::PREF_FONTSIZE);
    textfont(text_font);
    textsize(text_size);
}

