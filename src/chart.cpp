// Copyright 2016 - 2021 gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#include "chart.h"
#include "price.h"
#include "util.h"
#include <FL/Fl_Scrollbar.H>
#include <FL/fl_draw.H>
#include <FL/Fl.H>
#include <algorithm>
#include <math.h>
#include <time.h>
#include <string.h>
#include <unistd.h>

// MKALGAM_ON

#define _PUSH_CLIP(X,Y,W,H) fl_push_clip(X,Y,W,H)
#define _POP_CLIP()         fl_pop_clip()
// #define _PUSH_CLIP(X,Y,W,H)
// #define _POP_CLIP()

namespace flw {
    static const int  _CHART_VLINES     =               1000;
    static const int  _CHART_LABEL_SIZE =                100;
    const double      flw::Chart::MAX   =  999999999999999.0;
    const double      flw::Chart::MIN   = -999999999999999.0;

    //------------------------------------------------------------------------------
    static int _chart_bsearch(const std::vector<flw::Price>& prices, const Price& key) {
        auto it = std::lower_bound(prices.begin(), prices.end(), key);

        if (it == prices.end() || *it != key) {
            return -1;
        }
        else {
            std::size_t index = std::distance(prices.begin(), it);
            return (int) index;
        }
    }

    //--------------------------------------------------------------------------
    static int _chart_count_decimals(double number) {
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

    //--------------------------------------------------------------------------
    static std::string _chart_format_double(double number, int decimals, char sep) {
        char res[100];

        *res = 0;

        if (sep < 32) {
            sep = 32;
        }

        if (decimals < 0) {
            decimals = flw::_chart_count_decimals(number);
        }

        if (decimals == 0) {
            return flw::util::format_int((int64_t) number, sep);
        }
        else if (fabs(number) < 9223372036854775807.0) {
            char fr_str[100];
            auto int_num    = (int64_t) fabs(number);
            auto double_num = (double) (fabs(number) - int_num);
            auto int_str    = flw::util::format_int(int_num, sep);
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
    struct ChartScale {
        double max;
        double min;
        double pixel;
        double tick;

        //--------------------------------------------------------------------------
        ChartScale() {
            clear();
        }

        //--------------------------------------------------------------------------
        void calc(int height) {
            auto range    = diff();

            tick = 0.0;

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

                if (ticker < 8) {
                    if (ticker < 5) {
                        test_inc /= 5;
                    }
                    else {
                        test_inc /= 2;
                    }

                    while ((test_min + test_inc) <= min) {
                        test_min += test_inc;
                    }

                    while ((test_max - test_inc) >= max) {
                        test_max -= test_inc;
                    }

                }

                pixel = 0.0;
                min   = test_min;
                max   = test_max;
                tick  = test_inc;

                if (tick > 0.000000001) {
                    pixel = height / diff();
                }
            }
        }

        //--------------------------------------------------------------------------
        void clear() {
            min   = flw::Chart::MAX;
            max   = flw::Chart::MIN;
            tick  = 0.0;
            pixel = 0.0;
        }

        //--------------------------------------------------------------------------
        void debug(const char* name) const {
            #ifdef DEBUG
                printf("\t\t------------------------------------\n");
                printf("\t\tChartScale: (%s)\n", name);
                printf("\t\t\tmin: %4s%25.6f\n", "", min);
                printf("\t\t\tmax: %4s%25.6f\n", "", max);
                printf("\t\t\tDiff: %3s%25.6f\n", "", diff());
                printf("\t\t\ttick: %3s%25.6f\n", "", tick);
                printf("\t\t\tpixel: %2s%25.6f\n", "", pixel);
            #else
                (void) name;
            #endif
        }

        //--------------------------------------------------------------------------
        double diff() const {
            return max - min;
        }

        //--------------------------------------------------------------------------
        void fix_height() {
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
    };

    //------------------------------------------------------------------------------
    struct ChartLine {
        flw::Chart::TYPE             chart_type;
        Fl_Align                align;
        Fl_Color                color;
        std::vector<flw::Price> points;
        char*                   label;
        double                  clamp_max;
        double                  clamp_min;
        double                  max;
        double                  min;
        int                     width;

        //--------------------------------------------------------------------------
        ChartLine() {
            label = nullptr;
            clear();
        }

        //--------------------------------------------------------------------------
        ~ChartLine() {
            clear();
        }

        //--------------------------------------------------------------------------
        void clear() {
            points.clear();
            free(label);

            align      = FL_ALIGN_CENTER;
            chart_type = flw::Chart::TYPE::LINE;
            clamp_max  = flw::Chart::MAX;
            clamp_min  = flw::Chart::MIN;
            color      = FL_BLACK;
            label      = nullptr;
            max        = flw::Chart::MIN;
            min        = flw::Chart::MAX;
            width      = 1;
        }

        //--------------------------------------------------------------------------
        void debug(int num) const {
            #ifdef DEBUG
                printf("\t\t------------------------------------------\n");
                printf("\t\tChartLine: (num=%d)\n", num);
                printf("\t\t\tpoints: %7s%25d\n", "", (int) points.size());
                printf("\t\t\tpoints_size: %2s%25d\n", "", (int) (sizeof(Price) * points.size()));
                printf("\t\t\tchart_type: %3s%25d\n", "", (int) chart_type);
                printf("\t\t\talign: %8s%25s\n", "", align == FL_ALIGN_LEFT ? "LEFT" : "RIGHT");
                printf("\t\t\twidth: %8s%25d\n", "", width);
                printf("\t\t\tlabel: %8s%25s\n", "", label);
                printf("\t\t\tclamp_min: %4s%25.4f\n", "", clamp_min);
                printf("\t\t\tclamp_max: %4s%25.4f\n", "", clamp_max);

                if (points.size() > 1) {
                    printf("\t\t\tfirst: %8s%25s\n", "", points.front().date.c_str());
                    printf("\t\t\tfirst: %8s%25f\n", "", points.front().close);
                    printf("\t\t\tlast: %9s%25s\n", "", points.back().date.c_str());
                    printf("\t\t\tlast: %9s%25f\n", "", points.back().close);
                }
            #else
                (void) num;
            #endif
        }

        //--------------------------------------------------------------------------
        void set(const std::vector<flw::Price>& points, const char* label, flw::Chart::TYPE chart_type, Fl_Align align, Fl_Color color, int width) {
            clear();

            this->points     = points;
            this->label      = strdup(label);
            this->chart_type = chart_type;
            this->align      = align;
            this->color      = color;
            this->width      = width > 0 && width <= 100 ? width : 1;

            for (auto& price : this->points) {
                if (price.high > max) {
                    max = price.high;
                }

                if (price.low < min) {
                    min = price.low;
                }
            }
        }
    };

    //------------------------------------------------------------------------------
    struct ChartArea {
        ChartLine*  lines[(int) flw::Chart::LINE::SIZE];
        ChartScale* left;
        ChartScale* right;
        double      h;
        double      w;
        double      x;
        double      y;
        int         percent;

        //--------------------------------------------------------------------------
        ChartArea() {
            for (int f = 0; f < (int) flw::Chart::LINE::SIZE; f++) {
                lines[f] = new ChartLine();
            }

            left  = new ChartScale();
            right = new ChartScale();

            clear(false);
        }

        //--------------------------------------------------------------------------
        ~ChartArea() {
            clear(true);

            for (auto f = 0; f < (int) flw::Chart::LINE::SIZE; f++) {
                delete lines[f];
            }

            delete left;
            delete right;
        }

        //--------------------------------------------------------------------------
        void clear(bool clear_data) {
            x = y = w = h = 0.0;
            left->clear();
            right->clear();

            if (clear_data) {
                percent = 0;

                for (auto f = 0; f < (int) flw::Chart::LINE::SIZE; f++) {
                    lines[f]->clear();
                }
            }
        }

        //--------------------------------------------------------------------------
        void debug(int num) const {
            #ifdef DEBUG
                bool empty = true;

                for (auto f = 0; f < (int) flw::Chart::LINE::SIZE; f++) {
                    if (lines[f]->points.size() > 0) {
                        empty = false;
                    }
                }

                if (empty == false || num == 0) {
                    printf("\t-----------------\n");
                    printf("\tChartArea: (num=%d)\n", num);
                    printf("\t\tobject: %3s%4d\n", "", (int) sizeof(ChartArea));
                    printf("\t\tx: %8s%4.0f\n", "", x);
                    printf("\t\ty: %8s%4.0f\n", "", y);
                    printf("\t\tw: %8s%4.0f\n", "", w);
                    printf("\t\th: %8s%4.0f\n", "", h);
                    printf("\t\tpercent: %2s%4d\n", "", percent);

                    left->debug("left");
                    right->debug("right");

                    for (auto f = 0; f < (int) flw::Chart::LINE::SIZE; f++) {
                        if (lines[f]->points.size() > 0) {
                            lines[f]->debug(f);
                        }
                    }
                }
            #else
                (void) num;
            #endif
        }
    };
}

//------------------------------------------------------------------------------
flw::Chart::Chart(int X, int Y, int W, int H, const char* l) : Fl_Group(X, Y, W, H, l) {
    end();
    color(FL_BACKGROUND2_COLOR);
    labelcolor(FL_FOREGROUND_COLOR);
    box(FL_BORDER_BOX);

    _scroll = new Fl_Scrollbar(0, 0, 0, 0);
    _scroll->type(FL_HORIZONTAL);
    _scroll->callback(flw::Chart::_CallbackScrollbar, this);
    add(_scroll);

    _areas[0] = new ChartArea;
    _areas[1] = new ChartArea;
    _areas[2] = new ChartArea;

    clear();
}

//------------------------------------------------------------------------------
flw::Chart::~Chart() {
    clear();
    delete _areas[0];
    delete _areas[1];
    delete _areas[2];
}

//------------------------------------------------------------------------------
void flw::Chart::area_size(int area1, int area2, int area3) {
    if (area1 >= 0 &&
        area1 <= 100 &&
        area2 >= 0 &&
        area2 <= 100 &&
        area3 >= 0 &&
        area3 <= 100 &&
        area1 + area2 + area3 == 100) {

        _areas[0]->percent = area1;
        _areas[1]->percent = area2;
        _areas[2]->percent = area3;
    }
}

//------------------------------------------------------------------------------
void flw::Chart::block(const std::vector<flw::Price>& dates) {
    _blocks = dates;
}

//------------------------------------------------------------------------------
void flw::Chart::_calc_area_height() {
    auto last   = 0;
    auto addh   = 0;
    auto height = 0;

    _top_space    = _fs;
    _bottom_space = _fs * 3 + Fl::scrollbar_size();

    height = h() - (_bottom_space + _top_space);

    for (auto f = 1; f < (int) flw::Chart::AREA::SIZE; f++) {
        auto area = _areas[f];

        if (area->percent >= 10) {
            height -= _fs;
        }
    }

    _areas[0]->y = _top_space;
    _areas[0]->h = (int) ((_areas[0]->percent / 100.0) * height);

    for (auto f = 1; f < (int) flw::Chart::AREA::SIZE; f++) {
        auto prev = _areas[f - 1];
        auto area = _areas[f];

        if (area->percent > 10) {
            area->y = prev->y + prev->h + _fs;
            area->h = (int) ((_areas[1]->percent / 100.0) * height);
            addh += (int) (prev->h);
            last = f;
        }
    }

    if (last > 0) {
        _areas[last]->h = (int) (height - addh);
    }
}

//------------------------------------------------------------------------------
void flw::Chart::_calc_area_width() {
    const double width = w() - (_margin_left * _fs + _margin_right * _fs);

    _ticks     = (int) (width / _tick_width);
    _top_space = _fs;

    if ((int) _dates.size() > _ticks) {
        double slider_size = _ticks / (_dates.size() + 1.0);

        _scroll->activate();
        _scroll->slider_size(slider_size > 0.05 ? slider_size : 0.05);
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

    for (int f = 0; f < (int) flw::Chart::AREA::SIZE; f++) {
        auto area = _areas[f];

        area->x = _margin_left * _fs;
        area->w = width;
    }
}

//------------------------------------------------------------------------------
void flw::Chart::_calc_dates() {
    std::string min = "99991231";
    std::string max = "01010101";

    for (auto f = 0; f < (int) flw::Chart::AREA::SIZE; f++) {
        ChartArea* area = _areas[f];

        for (auto g = 0; g < (int) flw::Chart::LINE::SIZE; g++) {
            auto line = area->lines[g];

            if (line->points.size() > 0) {
                auto& first = line->points.front();
                auto& last  = line->points.back();

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
        _dates = Price::DateSerie(min.c_str(), max.c_str(), _range, _blocks);
        redraw();
    }
}

//------------------------------------------------------------------------------
void flw::Chart::_calc_ymin_ymax() {
    for (auto f = 0; f < (int) flw::Chart::AREA::SIZE; f++) {
        auto area = _areas[f];

        area->left->clear();
        area->right->clear();

        for (auto g = 0; g < (int) flw::Chart::LINE::SIZE; g++) {
            auto line = area->lines[g];

            if (line->points.size() > 0) {
                int     stop    = _date_start + _ticks;
                int     current = _date_start;
                double  max     = flw::Chart::MIN;
                double  min     = flw::Chart::MAX;
                int     c       = 0;
                int64_t t       = util::time_micro();

                while (current <= stop && current < (int) _dates.size()) {
                    Price& date  = _dates[current];
                    int    index = flw::_chart_bsearch(line->points, date);

                    if (index != -1) {
                        c++;
                        Price& price = line->points[index];

                        if (line->chart_type == flw::Chart::TYPE::BAR ||
                            line->chart_type == flw::Chart::TYPE::VERTICAL ||
                            line->chart_type == flw::Chart::TYPE::VERTICAL2) {

                            min = price.low;
                            max = price.high;
                        }
                        else {
                            min = price.close;
                            max = price.close;
                        }

                        if ((int64_t) line->clamp_min > MIN) {
                            min = line->clamp_min;
                        }

                        if ((int64_t) line->clamp_max < MAX) {
                            max = line->clamp_max;
                        }

                        if (line->align == FL_ALIGN_LEFT) {
                            if (min < area->left->min) {
                                area->left->min = min;
                            }

                            if (max > area->left->max) {
                                area->left->max = max;
                            }
                        }
                        else {
                            if (min < area->right->min) {
                                area->right->min = min;
                            }

                            if (max > area->right->max) {
                                area->right->max = max;
                            }
                        }
                    }

                    current++;
                }

                #ifdef DEBUG
                    printf("found %d in %d us\n", c, (int) (util::time_micro() - t)); fflush(stdout);
                #endif
            }
        }

        area->left->fix_height();
        area->right->fix_height();
    }
}

//------------------------------------------------------------------------------
void flw::Chart::_calc_yscale() {
    for (auto f = 0; f < (int) flw::Chart::AREA::SIZE; f++) {
        auto area = _areas[f];

        area->left->calc(area->h);
        area->right->calc(area->h);
    }
}

//------------------------------------------------------------------------------
void flw::Chart::_CallbackScrollbar(Fl_Widget* widget, void* chart_object) {
    (void) widget;

    auto chart = (Chart*) chart_object;
    chart->_date_start = chart->_scroll->value();
    chart->init(false);
}

//------------------------------------------------------------------------------
void flw::Chart::clamp(flw::Chart::AREA area, flw::Chart::LINE line, double clamp_min, double clamp_max) {
    auto l = _areas[(int) area]->lines[(int) line];

    l->clamp_min = clamp_min;
    l->clamp_max = clamp_max;
    redraw();
}

//------------------------------------------------------------------------------
void flw::Chart::clear() {
    ((Fl_Valuator*) _scroll)->value(0);

    _blocks.clear();
    _dates.clear();

    _areas[0]->clear(true);
    _areas[1]->clear(true);
    _areas[2]->clear(true);

    _area         = nullptr;
    _label[0]     = 0;
    _bottom_space = 0;
    _date_start   = 0;
    _old_height   = -1;
    _old_width    = -1;
    _top_space    = 0;
    _ver_pos[0]   = -1;

    area_size();
    font(flw::PREF_FIXED_FONT, flw::PREF_FIXED_FONTSIZE);
    margin();
    range();
    support_lines();
    tick_width();
    tooltip();
    zoom();
}

//------------------------------------------------------------------------------
void flw::Chart::_create_tooltip() {
    const int X    = Fl::event_x();
    const int Y    = Fl::event_y();
    auto      l    = *_label ? true : false;
    auto      area = _inside_area(X, Y);

    *_label = 0;
    _area   = nullptr;

    if (area) {
        const int stop  = _date_start + _ticks;
        int  X1          = x() + _margin_left * _fs;
        int  current     = _date_start;
        auto date_format = _date_format == Date::FORMAT::ISO ? Date::FORMAT::NAME_LONG : Date::FORMAT::ISO_TIME_LONG;

        while (current <= stop && current < (int) _dates.size()) {
            if (X >= X1 && X < (X1 + _tick_width)) { // Is mouse x pos inside current tick?
                auto        ydiff  = (double) ((y() + area->y + area->h) - Y);
                auto&       price  = _dates[current];
                auto        date   = Date::FromString(price.date.c_str());
                auto        string = date.format(date_format);
                std::string left;
                std::string right;

                if (area->left->max > area->left->min) {
                    left = flw::_chart_format_double(area->left->min + (ydiff / area->left->pixel), flw::_chart_count_decimals(area->left->tick) + 1, ' ');
                }

                if (area->right->max > area->right->min) {
                    right = flw::_chart_format_double(area->right->min + (ydiff / area->right->pixel), flw::_chart_count_decimals(area->right->tick) + 1, ' ');
                }

                if (left.size() && right.size()) {
                    snprintf(_label, _CHART_LABEL_SIZE, "date:  %s\nleft:  %s\nright: %s", string.c_str(), left.c_str(), right.c_str());
                }
                else if (left.size()) {
                    snprintf(_label, _CHART_LABEL_SIZE, "date:  %s\nleft:  %s", string.c_str(), left.c_str());
                }
                else if (right.size()) {
                    snprintf(_label, _CHART_LABEL_SIZE, "date:  %s\nright: %s", string.c_str(), right.c_str());
                }
                else {
                    snprintf(_label, _CHART_LABEL_SIZE, "date:  %s", string.c_str());
                }

                _area = area;
                break;
            }

            X1 += _tick_width;
            current++;
        }
    }

    if (*_label || l) {
        redraw();
    }
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

        printf("\n");
        printf("------------------------------\n");
        printf("Chart:\n");
        printf("\tobject: %9s%19d\n", "", (int) sizeof(Chart));
        printf("\t_bottom_space: %2s%19d\n", "", _bottom_space);
        printf("\t_top_space: %5s%19d\n", "", _top_space);
        printf("\t_dates: %9s%19d\n", "", (int) _dates.size());
        printf("\t_blocks: %8s%19d\n", "", (int) _blocks.size());
        printf("\t_date_format: %3s%19d\n", "", (int) _date_format);
        printf("\t_date_start: %4s%19d\n", "", _date_start);
        printf("\t_date_end: %6s%19d\n", "", _date_start + _ticks);
        printf("\t_margin_left: %3s%19d\n", "", _margin_left * _fs);
        printf("\t_margin_right: %2s%19d\n", "", _margin_right * _fs);
        printf("\t_range: %9s%19d\n", "", (int) _range);
        printf("\t_ticks: %9s%19d\n", "", _ticks);
        printf("\t_tick_width: %4s%19d\n", "", _tick_width);
        printf("\t_tooltip: %7s%19s\n", "", _tooltip ? "true" : "false");
        printf("\tx: %15s%19d\n", "", x());
        printf("\ty: %15s%19d\n", "", y());
        printf("\th: %15s%19d\n", "", h());
        printf("\tw: %15s%19d\n", "", w());
        printf("\tFirst Date: %6s%s\n", "", first.date.c_str());
        printf("\tLast Date: %7s%s\n", "", last.date.c_str());
        printf("\tStart Date: %6s%s\n", "", start.date.c_str());
        printf("\tEnd Date: %8s%s\n", "", end.date.c_str());

        _areas[0]->debug(0);
        _areas[1]->debug(1);
        _areas[2]->debug(2);
        fflush(stdout);
    #endif
}

//------------------------------------------------------------------------------
void flw::Chart::draw() {
    #ifdef DEBUG
        auto t = util::time_micro();
    #endif

        Fl_Group::draw();

        if (w() < (_fs * 10) ||
            h() < (_fs * 8)) {

            return;
        }

        _PUSH_CLIP(x(), y(), w(), h());
        _draw_xlabels();

        for (auto f = 0; f < (int) flw::Chart::AREA::SIZE; f++) {
            auto area = _areas[f];

            if (area->percent >= 10) {
                _draw_area(area);
            }
        }

        if (*_label && _area) {
            _draw_tooltip(_label, _area);
        }

        _POP_CLIP();

    #ifdef DEBUG
        printf("draw: %6lld us\n", (long long int) (util::time_micro() - t));
        fflush(stdout);
    #endif
}

//------------------------------------------------------------------------------
void flw::Chart::_draw_area(const ChartArea* area) {
    if (_ver_lines) {
        _draw_ver_lines(area);
    }

    _draw_ylabels(_margin_left * _fs, area->y + area->h, area->y, area->left, true);
    _draw_ylabels(w() - _margin_right * _fs, area->y + area->h, area->y, area->right, false);

    for (auto f = 0; f < (int) flw::Chart::LINE::SIZE; f++) {
        auto line = area->lines[f];

        if (line->points.size() > 0) {
            _draw_line(line, line->align == FL_ALIGN_LEFT ? area->left : area->right, area->x, area->y, area->w, area->h);
        }
    }

    _draw_line_labels(area);
    fl_rect(x() + area->x, y() + area->y - 1, area->w + 1, area->h + 2, labelcolor());
}

//------------------------------------------------------------------------------
void flw::Chart::_draw_line(const ChartLine* line, const ChartScale* scale, int X, const int Y, const int W, const int H) {
    int y2      = y() + Y + H;
    int lastX1  = -1;
    int lastY   = -1;
    int current = _date_start;
    int stop    = _date_start + _ticks;

    _PUSH_CLIP(x() + X, y() + Y, W + 1, H + 1);
    fl_color(line->color);
    fl_line_style(FL_SOLID, line->width);

    while (current <= stop && current < (int) _dates.size()) {
        auto& date  = _dates[current];
        auto  index = flw::_chart_bsearch(line->points, date);

        // auto string = date->format(_date_format);
        // auto price  = (const Price*) line->points->find(string(), _chart_compare_date_and_price);

        if (index != -1) {
            auto& price = line->points[index];
            auto  yh    = (int) ((price.high - scale->min) * scale->pixel);
            auto  yl    = (int) ((price.low - scale->min) * scale->pixel);
            auto  yc    = (int) ((price.close - scale->min) * scale->pixel);

            if (line->chart_type == flw::Chart::TYPE::LINE) {
                if (lastX1 > -1 && lastY > -1) {
                    fl_line(x() + lastX1, y2 - lastY, x() + X, y2 - yc);
                }
            }
            else if (line->chart_type == flw::Chart::TYPE::BAR) {
                fl_line(x() + X, y2 - yl, x() + X, y2 - yh);
                fl_line(x() + X, y2 - yc, x() + X + _tick_width - 1, y2 - yc);
            }
            else if (line->chart_type == flw::Chart::TYPE::VERTICAL) {
                fl_rectf(x() + X, y2 - yh, line->width, (yh - yl) < 1 ? 1 : yh - yl);
            }
            else if (line->chart_type == flw::Chart::TYPE::VERTICAL2) {
                fl_rectf(x() + X, y2 - yh, line->width, yh);
            }
            else if (line->chart_type == flw::Chart::TYPE::VERTICAL3) {
                fl_line(x() + X, y2, x() + X, y() + Y);
            }
            else if (line->chart_type == flw::Chart::TYPE::HORIZONTAL) {
                fl_line(x() + X, y2 - yc, x() + X + _tick_width, y2 - yc);
            }
            else if (line->chart_type == flw::Chart::TYPE::HORIZONTAL2) {
                fl_line(x() + _margin_left * _fs, y2 - yc, x() + Fl_Widget::w() - _margin_right * _fs, y2 - yc);
            }

            lastX1 = X;
            lastY  = yc;
        }

        X += _tick_width;
        current++;
    }

    fl_line_style(FL_SOLID, 0);
    _POP_CLIP();
}

//------------------------------------------------------------------------------
void flw::Chart::_draw_line_labels(const ChartArea* area) {
    int y_left  = y() + area->y + (_fs * 0.5);
    int y_right = y_left;

    fl_font(_font, _fs);

    for (auto f = 0; f < (int) flw::Chart::LINE::SIZE; f++) {
        auto line = area->lines[f];

        if (line->points.size() > 0 && *line->label) {
            fl_color(line->color);

            if (line->align == FL_ALIGN_LEFT) {
                fl_draw(line->label, x() + area->x + _fs, y_left, area->w, _fs, FL_ALIGN_LEFT);
                y_left += _fs;
            }
            else {
                fl_draw(line->label, x() + area->x, y_right, area->w - _fs, _fs, FL_ALIGN_RIGHT);
                y_right += _fs;
            }
        }
    }
}

//------------------------------------------------------------------------------
void flw::Chart::_draw_tooltip(const char* label, const ChartArea* area) {
    int X = Fl::event_x();
    int Y = Fl::event_y();

    if (X > x() + w() - _fs * 23) {
        X = X - _fs * 23;
    }
    else {
        X += _fs * 3;
    }

    if (Y < y() + _fs * 5) {
        Y += _fs * 5;
    }
    else {
        Y -= _fs * 5;
    }

    fl_font(flw::PREF_FIXED_FONT, _fs);
    fl_color(color());
    fl_rectf(X, Y, _fs * 20, _fs * 4);
    fl_color(labelcolor());
    fl_rect(X, Y, _fs * 20, _fs * 4);
    fl_line(Fl::event_x(), y() + area->y,Fl::event_x(), y() + area->y + area->h);
    fl_line(x() + area->x, Fl::event_y(), x() + area->x + area->w,Fl::event_y());
    fl_draw(label, X + _fs, Y, _fs * 18, _fs * 4, FL_ALIGN_LEFT | FL_ALIGN_CENTER | FL_ALIGN_INSIDE);
}

//------------------------------------------------------------------------------
void flw::Chart::_draw_ver_lines(const ChartArea* area) {
    fl_color(fl_color_average(FL_FOREGROUND_COLOR, FL_BACKGROUND2_COLOR, 0.2));

    for (auto i = 0; i < _CHART_VLINES; i++) {
        if (_ver_pos[i] < 0) {
            break;
        }
        else {
            fl_line(_ver_pos[i], y() + (int) area->y, _ver_pos[i], y() + (int) (area->y + area->h - 1));
        }
    }
}

//------------------------------------------------------------------------------
void flw::Chart::_draw_xlabels() {
    _ver_pos[0] = -1;

    fl_font(_font, _fs);

    const int  stop    = _date_start + _ticks;
    const int  cw      = fl_width("0");
    const int  cw2     = cw * 2;
    const int  cw4     = cw * 4;
    const int  fs05    = _fs * 0.5;
    const int  fs08    = _fs * 0.8;
    const int  Y       = y() + (h() - _bottom_space);
    int        current = _date_start;
    int        index   = 0;
    int        last    = -1;
    int        X1      = x() + _margin_left * _fs;
    int        X2      = 0;
    char       buffer1[100];
    char       buffer2[100];

    while (current <= stop && current < (int) _dates.size()) {
        auto& price = _dates[current];
        auto  date  = Date::FromString(price.date.c_str());
        auto  addv  = false;

        fl_color(labelcolor());
        fl_line(X1, Y, X1, Y + fs05);
        *buffer1 = 0;
        *buffer2 = 0;

        if (_range == Date::RANGE::HOUR) {
            if (date.day() != last) {
                addv = true;
                last = date.day();

                if (X1 >= X2) {
                    snprintf(buffer1, 100, "%04d-%02d-%02d", date.year(), date.month(), date.day());
                }
                else {
                    fl_line(X1, Y, X1, Y + _fs);
                }
            }
            else {
                snprintf(buffer2, 100, "%02d", date.hour());
            }
        }
        else if (_range == Date::RANGE::MIN) {
            if (date.hour() != last) {
                addv = true;
                last = date.hour();

                if (X1 >= X2) {
                    snprintf(buffer1, 100, "%04d-%02d-%02d/%02d", date.year(), date.month(), date.day(), date.hour());
                }
                else {
                    fl_line(X1, Y, X1, Y + _fs);
                }
            }
            else {
                snprintf(buffer2, 100, "%02d", date.minute());
            }
        }
        else if (_range == Date::RANGE::SEC) {
            if (date.minute() != last) {
                addv = true;
                last = date.minute();

                if (X1 >= X2) {
                    snprintf(buffer1, 100, "%04d-%02d-%02d/%02d:%02d", date.year(), date.month(), date.day(), date.hour(), date.minute());
                }
                else {
                    fl_line(X1, Y, X1, Y + _fs);
                }
            }
            else {
                snprintf(buffer2, 100, "%02d", date.second());
            }
        }
        else if (_range == Date::RANGE::DAY || _range == Date::RANGE::WEEKDAY) {
            if (date.month() != last) {
                addv = true;
                last = date.month();

                if (X1 >= X2) {
                    snprintf(buffer1, 100, "%04d-%02d", date.year(), date.month());
                }
                else {
                    fl_line(X1, Y, X1, Y + _fs);
                }
            }
            else {
                snprintf(buffer2, 100, "%02d", date.day());
            }
        }
        else if (_range == Date::RANGE::FRIDAY || _range == Date::RANGE::SUNDAY) {
            if (date.month() != last) {
                addv = true;
                last = date.month();

                if (X1 >= X2) {
                    snprintf(buffer1, 100, "%04d-%02d/%02d", date.year(), date.month(), date.week());
                }
                else {
                    fl_line(X1, Y, X1, Y + _fs);
                }
            }
            else
                snprintf(buffer2, 100, "%02d", date.week());
        }
        else if (_range == Date::RANGE::MONTH) {
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

        if (*buffer1) {
            auto len  = (double) strlen(buffer1);
            auto half = (double) (len / 2.0 * cw);

            fl_font(_font, _fs);
            fl_draw(buffer1, X1 - half, Y + _fs + 6, half + half, _fs, FL_ALIGN_CENTER | FL_ALIGN_INSIDE);
            fl_line(X1, Y, X1, Y + _fs);
            X2 = X1 + cw + half + half;
        }

        if (*buffer2 && cw2 <= _tick_width) { // Draw x label
            fl_font(_font, fs08);
            fl_draw(buffer2, X1 - cw2, Y + fs05, cw4, fs08, FL_ALIGN_CENTER | FL_ALIGN_INSIDE);
        }

        if (addv && index < _CHART_VLINES) { // Save x pos for vertical lines
            _ver_pos[index++] = X1;
        }

        X1 += _tick_width;
        current++;
    }

    _ver_pos[index] = -1;
}

//------------------------------------------------------------------------------
void flw::Chart::_draw_ylabels(const int X, double Y1, const double Y2, const ChartScale* scale, const bool left) {
    const double yinc  = (scale->pixel * scale->tick);
    const int    fs05  = _fs * 0.5;
    const int    fr    = flw::_chart_count_decimals(scale->tick);
    int          width = w() - (_margin_left * _fs + _margin_right * _fs);
    double       ylast = flw::Chart::MAX;
    double       yval  = scale->min;

    if (scale->min >= scale->max) {
        return;
    }

    fl_font(_font, _fs);

    while ((int) (Y1 + 0.5) >= (int) Y2 && (scale->pixel * scale->tick) > 0.1) {
        if (ylast > Y1) {
            int  y1     = y() + (int) Y1;
            int  x1     = x() + X;
            auto string = flw::_chart_format_double(yval, fr, ' ');

            if (left) {
                fl_color(labelcolor());
                fl_line(x1 - fs05, y1, x1, y1);
                fl_draw(string.c_str(), x(), y1 - fs05 - 2, _margin_left * _fs - _fs, _fs, FL_ALIGN_RIGHT | FL_ALIGN_INSIDE);

                if (_hor_lines && (int) Y1 >= (int) (Y2 + fs05)) {
                    fl_color(fl_color_average(FL_FOREGROUND_COLOR, FL_BACKGROUND2_COLOR, 0.2));
                    fl_line(x1 + 1, y1, x1 + width - 1, y1);
                }
            }
            else {
                fl_color(labelcolor());
                fl_line(x1, y1, x1 + fs05, y1);
                fl_draw(string.c_str(), x1 + _fs, y1 - fs05 - 2, _margin_right *_fs - _fs, _fs, FL_ALIGN_LEFT | FL_ALIGN_INSIDE);

                if (_hor_lines && (int) Y1 >= (int) (Y2 + fs05)) {
                    fl_color(fl_color_average(FL_FOREGROUND_COLOR, FL_BACKGROUND2_COLOR, 0.2));
                    fl_line(x1 - width + 1, y1, x1 - 1, y1);
                }
            }

            ylast = Y1 - _fs - fs05;
        }

        Y1 -= yinc;
        yval += scale->tick;
    }
}

//------------------------------------------------------------------------------
flw::ChartArea* flw::Chart::_inside_area(int X, int Y) {
    for (int f = 0; f < (int) flw::Chart::AREA::SIZE; f++) {
        auto area = _areas[f];

        if (area->percent >= 10 &&
            X >= x() + (int) area->x &&
            Y >= y() + (int) area->y &&
            X <= x() + (int) (area->x + area->w) &&
            Y <= y() + (int) (area->y + area->h + 1)) {

            return area;
        }
    }

    return nullptr;
}

//------------------------------------------------------------------------------
int flw::Chart::handle(int event) {
    if (event == FL_PUSH && Fl::event_button1() && _tooltip) {
        _create_tooltip();

        if (*_label) {
            return 1;
        }
    }
    else if (event == FL_DRAG) { // FL_PUSH must have been called before mouse drag
        _create_tooltip();

        if (*_label) {
            return 1;
        }
    }
    else if (event == FL_MOVE) { // Remove tooltip if mouse is outside chart area
        if (*_label) {
            redraw();
        }

        *_label = 0;
    }
    else if (event == FL_MOUSEWHEEL) {
        double size = _dates.size() - _ticks;
        double pos  = _scroll->value();
        double dy   = (Fl::event_dy() * 10);
        double dx   = (Fl::event_dx() * 10);
        double dd   = fabs(dx) > fabs(dy) ? dx : dy;
        double adj  = _ticks / dd;

        if (fabs(dd) < 1.0) {
            return Fl_Group::handle(event);
        }
        else if (Fl::event_ctrl() > 0) {
            if (_zoom) {
                int t = adj > 0.0 ? _tick_width + 1 : _tick_width - 1;

                tick_width(t);
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

    return Fl_Group::handle(event);
}

//------------------------------------------------------------------------------
void flw::Chart::init(bool calc_dates) {
    #ifdef DEBUG
        auto t = util::time_micro();
    #endif

    if (calc_dates) {
        _calc_dates();
    }

    _calc_area_height();
    _calc_area_width();
    _calc_ymin_ymax();
    _calc_yscale();

    #ifdef DEBUG
        printf("%s: %6lld us\n", calc_dates ? "INIT" : "init", (long long int) (util::time_micro() - t));
        fflush(stdout);
    #endif

    redraw();
}

//------------------------------------------------------------------------------
void flw::Chart::line(flw::Chart::AREA area, flw::Chart::LINE line, const std::vector<flw::Price>& points, const char* line_label, flw::Chart::TYPE chart_type, Fl_Align line_align, Fl_Color line_color, int line_width) {
    auto l = _areas[(int) area]->lines[(int) line];

    l->set(points, line_label, chart_type, line_align, line_color, line_width);
    redraw();
}

//------------------------------------------------------------------------------
void flw::Chart::range(Date::RANGE range) {
    _range = range;

    if (_range == Date::RANGE::HOUR ||
        _range == Date::RANGE::MIN ||
        _range == Date::RANGE::SEC) {

        _date_format = Date::FORMAT::ISO_TIME;
    }
    else {
        _date_format = Date::FORMAT::ISO;
    }
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
void flw::Chart::tick_width(int tick_width) {
    if (tick_width > 2 && tick_width <= 100) {
        _tick_width = tick_width;
    }

    redraw();
}

// MKALGAM_OFF
