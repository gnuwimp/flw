// This source file is an amalgamation of one or more source files
// Copyright 2016 - 2021 gnuwimp@gmail.com
// Released under the GNU General Public License v3.0
#ifndef FLW_H
#define FLW_H
#include <FL/Fl_Box.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Hold_Browser.H>
#include <FL/Fl_Text_Buffer.H>
#include <FL/Fl_Text_Display.H>
#include <assert.h>
#include <iostream>
#include <map>
#include <string>
#include <vector>
class Fl_Button;
class Fl_Toggle_Button;
class Fl_Hor_Fill_Slider;
class Fl_Input;
class Fl_Menu_Button;


#define FLW_LINE                    { fprintf(stderr, "%5d: %s - %s\n", __LINE__, __func__, __FILE__); fflush(stderr); }
#define _FLW_TOSTRING(X)            #X
#define FLW_TOSTRING(X)             _FLW_TOSTRING(X)
#define FLW_PRINT(...)              FLW_PRINT_MACRO(__VA_ARGS__, FLW_PRINT7, FLW_PRINT6, FLW_PRINT5, FLW_PRINT4, FLW_PRINT3, FLW_PRINT2, FLW_PRINT1)(__VA_ARGS__);
#define FLW_PRINT1(A)               { std::cerr << __FILE__ << "::" << __func__ << "::" << __LINE__ << " (" FLW_TOSTRING(A) ")=" << (A) << std::endl; fflush(stderr); }
#define FLW_PRINT2(A,B)             { std::cerr << __FILE__ << "::" << __func__ << "::" << __LINE__ << " (" FLW_TOSTRING(A) ")=" << (A) << ", (" FLW_TOSTRING(B) ")=" << (B) << std::endl; fflush(stderr); }
#define FLW_PRINT3(A,B,C)           { std::cerr << __FILE__ << "::" << __func__ << "::" << __LINE__ << " (" FLW_TOSTRING(A) ")=" << (A) << ", (" FLW_TOSTRING(B) ")=" << (B) << ", (" FLW_TOSTRING(C) ")=" << (C) << std::endl; fflush(stderr); }
#define FLW_PRINT4(A,B,C,D)         { std::cerr << __FILE__ << "::" << __func__ << "::" << __LINE__ << " (" FLW_TOSTRING(A) ")=" << (A) << ", (" FLW_TOSTRING(B) ")=" << (B) << ", (" FLW_TOSTRING(C) ")=" << (C) << ", (" FLW_TOSTRING(D) ")=" << (D) << std::endl; fflush(stderr); }
#define FLW_PRINT5(A,B,C,D,E)       { std::cerr << __FILE__ << "::" << __func__ << "::" << __LINE__ << " (" FLW_TOSTRING(A) ")=" << (A) << ", (" FLW_TOSTRING(B) ")=" << (B) << ", (" FLW_TOSTRING(C) ")=" << (C) << ", (" FLW_TOSTRING(D) ")=" << (D) << ", (" FLW_TOSTRING(E) ")=" << (E) << std::endl; fflush(stderr); }
#define FLW_PRINT6(A,B,C,D,E,F)     { std::cerr << __FILE__ << "::" << __func__ << "::" << __LINE__ << " (" FLW_TOSTRING(A) ")=" << (A) << ", (" FLW_TOSTRING(B) ")=" << (B) << ", (" FLW_TOSTRING(C) ")=" << (C) << ", (" FLW_TOSTRING(D) ")=" << (D) << ", (" FLW_TOSTRING(E) ")=" << (E) << ", (" FLW_TOSTRING(F) ")=" << (F) << std::endl; fflush(stderr); }
#define FLW_PRINT7(A,B,C,D,E,F,G)   { std::cerr << __FILE__ << "::" << __func__ << "::" << __LINE__ << " (" FLW_TOSTRING(A) ")=" << (A) << ", (" FLW_TOSTRING(B) ")=" << (B) << ", (" FLW_TOSTRING(C) ")=" << (C) << ", (" FLW_TOSTRING(D) ")=" << (D) << ", (" FLW_TOSTRING(E) ")=" << (E) << ", (" FLW_TOSTRING(F) ")=" << (F) << ", (" FLW_TOSTRING(G) ")=" << (G) << std::endl; fflush(stderr); }
#define FLW_PRINT_MACRO(A,B,C,D,E,F,G,N,...) N

namespace flw {
    typedef std::vector<std::string>    StringVector;

    extern bool                         PREF_IS_DARK;
    extern int                          PREF_FIXED_FONT;
    extern std::string                  PREF_FIXED_FONTNAME;
    extern int                          PREF_FIXED_FONTSIZE;
    extern int                          PREF_FONT;
    extern int                          PREF_FONTSIZE;
    extern std::string                  PREF_FONTNAME;

    namespace util {
        //----------------------------------------------------------------------
        // Collection of utility functions
        //
        void                            center_window(Fl_Window* window, Fl_Window* parent = nullptr);
        std::string                     fix_menu_string(std::string in);
        std::string                     format(const char* format, ...);
        std::string                     format_int(int64_t number, char sep = ' ');
        void                            labelfont(Fl_Widget* widget);
        void                            print(Fl_Widget* widget, bool tab = false);
        void                            print(Fl_Group* group);
        int                             replace(std::string& string, std::string find, std::string replace);
        StringVector                    split(const std::string& string, std::string split);
        bool                            test(bool val, int line, const char* func);
        bool                            test(const char* ref, const char* val, int line, const char* func);
        bool                            test(int64_t ref, int64_t val, int line, const char* func);
        bool                            test(double ref, double val, double diff, int line, const char* func);
        double                          time();
        int64_t                         time_micro();
        int64_t                         time_milli();
        void                            time_sleep(int milli);
        double                          to_double(const char* string, double def = 0.0);
        long double                     to_double_l(const char* string, long double def = 0.0);
        int                             to_doubles(const char* string, double numbers[], size_t size);
        int64_t                         to_int(const char* string, int64_t def = 0);
        int                             to_ints(const char* string, int64_t numbers[], size_t size);
        void*                           zero_memory(char* string);
        void*                           zero_memory(void* mem, size_t size);
        void*                           zero_memory(std::string& string);
    }
}



namespace flw {
    //--------------------------------------------------------------------------
    // An date and time class
    // Date range is from 1-1-1 00:00:00 to 9999-12-31 23:59:59
    //
    class Date {
    public:
        enum class COMPARE {
                                        YYYYMM,
                                        YYYYMMDD,
                                        YYYYMMDDHH,
                                        YYYYMMDDHHMM,
                                        YYYYMMDDHHMMSS,
        };

        enum class DAY {
                                        INVALID,
                                        MONDAY,
                                        TUESDAY,
                                        WENDSDAY,
                                        THURSDAY,
                                        FRIDAY,
                                        SATURDAY,
                                        SUNDAY,
        };

        enum class FORMAT {
                                        ISO,
                                        ISO_LONG,
                                        ISO_TIME,
                                        ISO_TIME_LONG,
                                        US,
                                        WORLD,
                                        NAME,
                                        NAME_LONG,
                                        YEAR_MONTH,
                                        YEAR_MONTH_LONG,
        };

        enum class RANGE {
                                        DAY,
                                        WEEKDAY,
                                        FRIDAY,
                                        SUNDAY,
                                        MONTH,
                                        HOUR,
                                        MIN,
                                        SEC,
        };

        static const int                SECS_PER_HOUR;
        static const int                SECS_PER_DAY;
        static const int                SECS_PER_WEEK;

                                        Date(bool utc = false);
                                        Date(const Date& other);
                                        Date(Date&&);
                                        Date(int year, int month, int day, int hour = 0, int min = 0, int sec = 0);
        Date&                           operator=(const Date& other);
        Date&                           operator=(Date&&);
        bool                            operator<(const Date& other) const
                                            { return compare(other) < 0 ? true : false; }
        bool                            operator<=(const Date& other) const
                                            { return compare(other) <= 0 ? true : false; }
        bool                            operator>(const Date& other) const
                                            { return compare(other) > 0 ? true : false; }
        bool                            operator>=(const Date& other) const
                                            { return compare(other) >= 0 ? true : false; }
        bool                            operator==(const Date& other) const
                                            { return compare(other) == 0 ? true : false; }
        bool                            operator!=(const Date& other) const
                                            { return compare(other) != 0 ? true : false; }
        bool                            add_days(int days);
        bool                            add_months(int months);
        bool                            add_seconds(int64_t seconds);
        int                             compare(const Date& other, Date::COMPARE flag = Date::COMPARE::YYYYMMDDHHMMSS) const;
        int                             day() const
                                            { return _day; }
        Date&                           day(int day);
        Date&                           day_last();
        int                             diff_days(const Date& other) const;
        int                             diff_months(const Date& other) const;
        int                             diff_seconds(const Date& other) const;
        std::string                     format(Date::FORMAT format = Date::FORMAT::ISO) const;
        int                             hour() const
                                            { return _hour; }
        Date&                           hour(int hour);
        bool                            is_leapyear() const;
        int                             minute() const
                                            { return _min; }
        Date&                           minute(int min);
        int                             month() const
                                            { return _month; }
        Date&                           month(int month);
        int                             month_days() const;
        const char*                     month_name() const;
        const char*                     month_name_short() const;
        void                            print() const;
        int                             second() const
                                            { return _sec; }
        Date&                           second(int sec);
        Date&                           set(const Date& other);
        Date&                           set(int year, int month, int day, int hour = 0, int min = 0, int sec = 0);
        int64_t                         time() const;
        int                             week() const;
        Date::DAY                       weekday() const;
        Date&                           weekday(Date::DAY weekday);
        const char*                     weekday_name() const;
        const char*                     weekday_name_short() const;
        int                             year() const
                                            { return _year; }
        Date&                           year(int year);
        int                             yearday() const;

        static int                      Compare(const void* a, const void* b);
        static bool                     Compare(const Date& a, const Date& b);
        static void                     Del(void* self);
        static Date                     FromString(const char* string, bool us = false);
        static Date                     FromTime(int64_t seconds, bool utc = false);

    private:
        short                           _year;
        char                            _month;
        char                            _day;
        char                            _hour;
        char                            _min;
        char                            _sec;
    };
}



namespace flw {
    struct Price;

    //------------------------------------------------------------------------------
    // Price object that is used by the Chart widget
    //
    struct Price {
        std::string                     date;
        double                          close;
        double                          high;
        double                          low;
        double                          vol;

                                        Price();
                                        Price(const std::string& date_value, double value = 0.0);
                                        Price(const std::string& date, double high, double low, double close, double vol = 0.0);
                                        Price(const Price& price);
                                        Price(Price&& price);
        Price&                          operator=(const Price& price);
        Price&                          operator=(Price&& price);
        bool                            operator<(const Price& price) const { return date < price.date; }
        bool                            operator<=(const Price& price) const { return date <= price.date; }
        bool                            operator==(const Price& price) const { return date == price.date; }
        bool                            operator!=(const Price& price) const { return date != price.date; }
        std::string                     format(Date::FORMAT format = Date::FORMAT::ISO) const;
        void                            print() const;

        static std::vector<Price>       Atr(const std::vector<Price>& in, std::size_t days);
        static std::vector<Price>       DateSerie(const char* start_date, const char* stop_date, Date::RANGE range = Date::RANGE::DAY, const std::vector<Price>& block = std::vector<Price>());
        static std::vector<Price>       DayToMonth(const std::vector<Price>& in);
        static std::vector<Price>       DayToWeek(const std::vector<Price>& in, Date::DAY weekday);
        static std::vector<Price>       ExponentialMovingAverage(const std::vector<Price>& in, std::size_t days);
        static std::vector<Price>       Momentum(const std::vector<Price>& in, std::size_t days);
        static std::vector<Price>       MovingAverage(const std::vector<Price>& in, std::size_t days);
        static void                     Print(const std::vector<Price>& in);
        static std::vector<Price>       RSI(const std::vector<Price>& in, std::size_t days);
        static std::vector<Price>       StdDev(const std::vector<Price>& in, std::size_t days);
        static std::vector<Price>       Stochastics(const std::vector<Price>& in, std::size_t days);
    };
}



namespace flw {
    namespace dlg {
        //----------------------------------------------------------------------
        extern const char*              PASSWORD_CANCEL;
        extern const char*              PASSWORD_OK;

        void                            html(const std::string& title, const std::string& text, Fl_Window* parent = nullptr, int W = 40, int H = 23);
        void                            list(const std::string& title, const StringVector& list, Fl_Window* parent = nullptr, bool fixed_font = false, int W = 40, int H = 23);
        void                            list(const std::string& title, const std::string& list, Fl_Window* parent = nullptr, bool fixed_font = false, int W = 40, int H = 23);
        void                            panic(std::string message);
        bool                            password1(const std::string& title, std::string& password, Fl_Window* parent = nullptr);
        bool                            password2(const std::string& title, std::string& password, Fl_Window* parent = nullptr);
        bool                            password3(const std::string& title, std::string& password, std::string& file, Fl_Window* parent = nullptr);
        bool                            password4(const std::string& title, std::string& password, std::string& file, Fl_Window* parent = nullptr);
        int                             select(const std::string& title, const StringVector& list, int select_row, Fl_Window* parent = nullptr, bool fixed_font = false, int W = 40, int H = 23);
        int                             select(const std::string& title, const StringVector& list, const std::string& select_row, Fl_Window* parent = nullptr, bool fixed_font = false, int W = 40, int H = 23);
        void                            text(const std::string& title, const std::string& text, Fl_Window* parent = nullptr, bool fixed_font = false, int W = 40, int H = 23);

        //----------------------------------------------------------------------
        class AbortDialog : public Fl_Double_Window {
        public:
                                        AbortDialog(const AbortDialog&) = delete;
                                        AbortDialog(AbortDialog&&) = delete;
            AbortDialog&                operator=(const AbortDialog&) = delete;
            AbortDialog&                operator=(AbortDialog&&) = delete;

                                        AbortDialog(double min = 0.0, double max = 0.0);
            bool                        check(int milliseconds = 200);
            bool                        check(double value, double min, double max, int milliseconds = 200);
            bool                        aborted()
                                            { return _abort; }
            void                        range(double min, double max);
            void                        resize(int X, int Y, int W, int H) override;
            void                        show(const std::string& label, Fl_Window* parent = nullptr);
            void                        value(double value);


            static void                 Callback(Fl_Widget* w, void* o);

        private:
            Fl_Button*                  _button;
            Fl_Hor_Fill_Slider*         _progress;
            bool                        _abort;
            int64_t                     _last;
        };

        //----------------------------------------------------------------------
        class WorkDialog : public Fl_Double_Window {
        public:
                                        WorkDialog(const char* title, Fl_Window* parent, bool cancel, bool pause, int W = 40, int H = 10);
            void                        resize(int X, int Y, int W, int H) override;
            bool                        run(double update_time, const StringVector& messages);
            bool                        run(double update_time, const std::string& message);

            static void                 Callback(Fl_Widget* w, void* o);

        private:
            Fl_Button*                  _cancel;
            Fl_Hold_Browser*            _label;
            Fl_Toggle_Button*           _pause;
            bool                        _ret;
            double                      _last;
            std::string                 _message;
        };
    }
}



namespace flw {
    //--------------------------------------------------------------------------
    // Show an wait mouse pointer until object is deleted
    //
    class WaitCursor {
        static WaitCursor*              WAITCURSOR;

    public:
                                        WaitCursor(const WaitCursor&) = delete;
                                        WaitCursor(WaitCursor&&) = delete;
                                        WaitCursor& operator=(const WaitCursor&) = delete;
                                        WaitCursor& operator=(WaitCursor&&) = delete;

                                        WaitCursor();
                                        ~WaitCursor();
    };
}



namespace flw {
    struct ChartArea;
    struct ChartLine;
    struct ChartScale;
    struct Price;

    //--------------------------------------------------------------------------
    // Chart widget that is using dates for the x scale
    //
    class Chart : public Fl_Group {
    public:
        enum class AREA {
                                        A1,
                                        A2,
                                        A3,
                                        SIZE,
        };

        enum class LINE {
                                        L1,
                                        L2,
                                        L3,
                                        L4,
                                        L5,
                                        L6,
                                        L7,
                                        L8,
                                        L9,
                                        L10,
                                        SIZE,
        };

        enum class TYPE {
                                        LINE,
                                        BAR,
                                        VERTICAL,
                                        VERTICAL2,
                                        VERTICAL3,
                                        HORIZONTAL,
                                        HORIZONTAL2,
        };

        static const double             MAX;
        static const double             MIN;

                                        Chart(int X = 0, int Y = 0, int W = 0, int H = 0, const char* l = nullptr);
        virtual                         ~Chart();
        void                            area_size(int area1 = 100, int area2 = 0, int area3 = 0);
        void                            block(const std::vector<flw::Price>& dates);
        void                            clamp(Chart::AREA area, Chart::LINE line, double clamp_min = MIN, double clamp_max = MAX);
        void                            clear();
        void                            debug() const;
        void                            font(Fl_Font font, Fl_Fontsize size)
                                            { _font = font; _fs   = size; }
        void                            draw() override;
        int                             handle(int event) override;
        void                            init(bool calc_dates);
        void                            line(Chart::AREA area, Chart::LINE line, const std::vector<flw::Price>& points, const char* line_label, Chart::TYPE chart_type, Fl_Align line_align, Fl_Color line_color, int line_width);
        void                            margin(int left = 6, int right = 1)
                                            { _margin_left = left; _margin_right = right; redraw(); }
        void                            range(Date::RANGE range = Date::RANGE::DAY);
        void                            resize(int X, int Y, int W, int H) override;
        void                            support_lines(bool hor = false, bool ver = false)
                                            { _hor_lines = hor; _ver_lines = ver; redraw(); }
        void                            tick_width(int tick_width = 3);
        void                            tooltip(bool tooltip = false)
                                            { _tooltip = tooltip; }
        void                            zoom(bool zoom = false)
                                            { _zoom = zoom; }

    private:
        void                            _calc_area_height();
        void                            _calc_area_width();
        void                            _calc_dates();
        void                            _calc_ymin_ymax();
        void                            _calc_yscale();
        void                            _create_tooltip();
        void                            _draw_area(const ChartArea* area);
        void                            _draw_line(const ChartLine* line, const ChartScale* scale, int X, int Y, int W, int H);
        void                            _draw_line_labels(const ChartArea* area);
        void                            _draw_tooltip(const char* label, const ChartArea* area);
        void                            _draw_ver_lines(const ChartArea* area);
        void                            _draw_xlabels();
        void                            _draw_ylabels(int X, double Y1, double Y2, const ChartScale* scale, bool left);
        ChartArea*                      _inside_area(int X, int Y);

        static void                     _CallbackScrollbar(Fl_Widget* widget, void* chart_object);

        Fl_Font                         _font;
        Fl_Fontsize                     _fs;
        Fl_Scrollbar*                   _scroll;
        ChartArea*                      _area;
        ChartArea*                      _areas[(int) Chart::AREA::SIZE];
        bool                            _hor_lines;
        bool                            _tooltip;
        bool                            _ver_lines;
        bool                            _zoom;
        char                            _label[101];
        Date::FORMAT                    _date_format;
        Date::RANGE                     _range;
        std::vector<Price>              _blocks;
        std::vector<Price>              _dates;
        int                             _bottom_space;
        int                             _date_start;
        int                             _margin_left;
        int                             _margin_right;
        int                             _old_height;
        int                             _old_width;
        int                             _tick_width;
        int                             _ticks;
        int                             _top_space;
        int                             _ver_pos[1001];
    };
}



namespace flw {
    //--------------------------------------------------------------------------
    // An date chooser widget
    // User can navigate with mouse or arrow keys within a month
    // Or use buttons to jump 1 month/1 year/10 years
    //
    class DateChooser : public Fl_Group {
    public:
                                        DateChooser(int X = 0, int Y = 0, int W = 0, int H = 0, const char* l = nullptr);
        void                            draw() override;
        void                            focus();
        Date                            get() const;
        int                             handle(int event) override;
        void                            resize(int X, int Y, int W, int H) override;
        void                            set(const Date& date);


    private:
        static void                     _Callback(Fl_Widget* w, void* o);
        void                            _set_label();

        Fl_Box*                         _month_label;
        Fl_Button*                      _b1;
        Fl_Button*                      _b2;
        Fl_Button*                      _b3;
        Fl_Button*                      _b4;
        Fl_Button*                      _b5;
        Fl_Button*                      _b6;
        Fl_Button*                      _b7;
        Fl_Widget*                      _canvas;
        int                             _h;
        int                             _w;
    };

    namespace dlg {
        bool                            date(const std::string& title, Date& date, Fl_Window* parent);
    }
}



namespace flw {
    class ScrollBrowser;

    namespace dlg {
        //----------------------------------------------------------------------
        // Dialog for selecting font and font size
        // FontDialog::LoadFonts() will be called automatically (or do it manually)
        // It is only needed once
        // Call FontDialog::DeleteFonts() before app exit (this is unnecessarily, only for keeping memory sanitizers satisfied)
        //
        class FontDialog : public Fl_Double_Window {
        public:
                                        FontDialog(const FontDialog&) = delete;
                                        FontDialog(FontDialog&&) = delete;
            FontDialog&                 operator=(const FontDialog&) = delete;
            FontDialog&                 operator=(FontDialog&&) = delete;

                                        FontDialog(Fl_Font font, Fl_Fontsize fontsize, const std::string& label);
                                        FontDialog(std::string font, Fl_Fontsize fontsize, std::string label);
            void                        activate_font()
                                            { ((Fl_Widget*) _fonts)->activate(); }
            void                        deactivate_font()
                                            { ((Fl_Widget*) _fonts)->deactivate(); }
            void                        deactivate_fontsize()
                                            { ((Fl_Widget*) _sizes)->deactivate(); }
            int                         font()
                                            { return _font; }
            std::string                 fontname()
                                            { return _fontname; }
            int                         fontsize()
                                            { return _fontsize; }
            void                        resize(int X, int Y, int W, int H) override;
            bool                        run(Fl_Window* parent = nullptr);

            static void                 Callback(Fl_Widget* w, void* o);
            static void                 DeleteFonts();
            static int                  LoadFont(std::string requested_font);
            static void                 LoadFonts(bool iso8859_only = true);

        private:
            void                        _activate();
            void                        _create(Fl_Font font, std::string fontname, Fl_Fontsize fontsize, std::string label);
            void                        _select_name(std::string font_name);

            Fl_Box*                     _label;
            Fl_Button*                  _cancel;
            Fl_Button*                  _select;
            ScrollBrowser*              _fonts;
            ScrollBrowser*              _sizes;
            bool                        _ret;
            int                         _font;
            int                         _fontsize;
            std::string                 _fontname;
        };
    }
}



namespace flw {
    struct _GridGroupChild;

    //--------------------------------------------------------------------------
    // A simple layout widget that uses a grid for placing widgets
    // One cell is set to labelsize() at start, use set_size() to change it
    //
    class GridGroup : public Fl_Group {
    public:
        static const int                MAX_WIDGETS = 100;

                                        GridGroup(int X = 0, int Y = 0, int W = 0, int H = 0, const char* l = nullptr);
        virtual                         ~GridGroup();
        void                            add(Fl_Widget* widget, int X, int Y, int W, int H);
        void                            clear();
        void                            remove(Fl_Widget* widget);
        void                            resize();
        void                            resize(int X, int Y, int W, int H) override;
        int                             size() const
                                            { return _size; }
        void                            size(int size)
                                            { _size = size; }

    private:
        _GridGroupChild*                _widgets[MAX_WIDGETS];
        int                             _size;
    };
}



namespace flw
                                    {
    //--------------------------------------------------------------------------
    // A 7 segment number label widget
    //
    class LcdNumber : public Fl_Box
                                        {
    public:
                                        LcdNumber(int X = 0, int Y = 0, int W = 0, int H = 0, const char* l = nullptr);
        void                            draw() override;
        void                            value(const char* value);

        Fl_Align                        align() const
                                            { return _align; }
        void                            align(Fl_Align align)
                                            { _align = align; Fl::redraw(); }
        int                             dot_size() const
                                            { return _dot_size; }
        void                            dot_size(int size)
                                            { _dot_size = size; Fl::redraw(); }
        Fl_Color                        segment_color() const
                                            { return _seg_color; }
        void                            segment_color(Fl_Color color)
                                            { _seg_color = color; Fl::redraw(); }
        int                             segment_gap() const
                                            { return _seg_gap; }
        void                            segment_gap(int gap)
                                            { _seg_gap = gap; Fl::redraw(); }
        int                             thickness() const
                                            { return _thick; }
        void                            thickness(int thickness)
                                            { _thick = thickness; Fl::redraw(); }
        int                             unit_gap()
                                            { return _unit_gap; }
        void                            unit_gap(int gap)
                                            { _unit_gap = gap; Fl::redraw(); }
        int                             unit_h() const
                                            { return _unit_h; }
        void                            unit_h(int height)
                                            { _unit_h = height; Fl::redraw(); }
        int                             unit_w() const
                                            { return _unit_w; }
        void                            unit_w(int width)
                                            { _unit_w = width; Fl::redraw(); }
        const char*                     value() const
                                            { return _value; }

    private:
        void                            _draw_seg(uchar a, int x, int y, int w, int h);

        Fl_Align                        _align;
        Fl_Color                        _seg_color;
        char                            _value[100];
        int                             _dot_size;
        int                             _seg_gap;
        int                             _thick;
        int                             _unit_gap;
        int                             _unit_h;
        int                             _unit_w;
    };
}



namespace flw {
    //--------------------------------------------------------------------------
    // A static text display widget with line based text coloring
    // Implement line() method which will be called for every line when you set text with value()
    // LogDisplay will manage text and style buffers
    //
    class LogDisplay : public Fl_Text_Display {
    public:
        enum class COLOR {
                                        FOREGROUND             = 'A',
                                        GRAY                   = 'B',
                                        RED                    = 'C',
                                        GREEN                  = 'D',
                                        BLUE                   = 'E',
                                        DARK_RED               = 'F',
                                        DARK_GREEN             = 'G',
                                        DARK_BLUE              = 'H',
                                        ORANGE                 = 'I',
                                        MAGENTA                = 'J',
                                        YELLOW                 = 'K',
                                        CYAN                   = 'L',
                                        DARK_MAGENTA           = 'M',
                                        DARK_YELLOW            = 'N',
                                        DARK_CYAN              = 'O',
                                        BLACK                  = 'P',
                                        WHITE                  = 'Q',
                                        BOLD_FOREGROUND        = 'R',
                                        BOLD_GRAY              = 'S',
                                        BOLD_RED               = 'T',
                                        BOLD_GREEN             = 'U',
                                        BOLD_BLUE              = 'V',
                                        BOLD_DARK_RED          = 'W',
                                        BOLD_DARK_GREEN        = 'X',
                                        BOLD_DARK_BLUE         = 'Y',
                                        BOLD_ORANGE            = 'Z',
                                        BOLD_MAGENTA           = '[',
                                        BOLD_YELLOW            = '\\',
                                        BOLD_CYAN              = ']',
                                        BOLD_DARK_MAGENTA      = '`',
                                        BOLD_DARK_YELLOW       = '^',
                                        BOLD_DARK_CYAN         = '_',
                                        BOLD_BLACK             = 'a',
                                        BOLD_WHITE             = 'b',
                                        ITALIC_FOREGROUND      = 'c',
                                        ITALIC_RED             = 'd',
                                        ITALIC_GREEN           = 'e',
                                        ITALIC_BLUE            = 'f',
                                        ITALIC_ORANGE          = 'g',
                                        ITALIC_MAGENTA         = 'h',
                                        ITALIC_YELLOW          = 'i',
                                        ITALIC_CYAN            = 'j',
                                        BOLD_ITALIC_FOREGROUND = 'k',
                                        BOLD_ITALIC_RED        = 'l',
                                        BOLD_ITALIC_GREEN      = 'm',
                                        BOLD_ITALIC_BLUE       = 'n',
                                        BOLD_ITALIC_ORANGE     = 'o',
                                        BOLD_ITALIC_MAGENTA    = 'p',
                                        BOLD_ITALIC_YELLOW     = 'q',
                                        BOLD_ITALIC_CYAN       = 'r',
                                        LAST                   = 'r',
        };

                                        LogDisplay(int X = 0, int Y = 0, int W = 0, int H = 0, const char* l = nullptr);
                                        ~LogDisplay();
        void                            find(const char* text, bool select);
        virtual void                    line(int count, const char* line, int len);
        void                            style_size(Fl_Fontsize textsize);
        void                            value(const char* text);

    protected:
        void                            color_word(const char* string, const char* word, LogDisplay::COLOR color);
        int                             str_index(const char* string, const char* find1, int* index1, const char* find2 = nullptr, int* index2 = nullptr, const char* find3 = nullptr, int* index3 = nullptr);
        void                            style(int start, int stop, LogDisplay::COLOR c);

    private:
        char*                           _tmp;
        int                             _tmp_pos;
        int                             _tmp_size;
        Fl_Text_Buffer*                 _buffer;
        Fl_Text_Buffer*                 _style;
    };
}



namespace flw {
    //--------------------------------------------------------------------------
    // A split view widget for two child widgets
    // That can be resized by dragging the border between the widgets
    // By default it places widgets in LAYOUT::VERTICAL direction (child widgets get full height)
    //
    class SplitGroup : public Fl_Group {
    public:
        enum class CHILD {
                                        FIRST,
                                        SECOND,
        };

        enum class DIRECTION {
                                        HORIZONTAL,
                                        VERTICAL,
        };
                                        SplitGroup(int X = 0, int Y = 0, int W = 0, int H = 0, const char* l = nullptr);
        void                            add(Fl_Widget* widget, SplitGroup::CHILD child);
        Fl_Widget*                      child(SplitGroup::CHILD child)
                                            { return (child == SplitGroup::CHILD::FIRST) ? _widgets[0] : _widgets[1]; }
        void                            clear();
        DIRECTION                       direction() const
                                            { return _direction; }
        void                            direction(SplitGroup::DIRECTION direction);
        int                             handle(int event) override;
        int                             min_pos() const
                                            { return _min; }
        void                            min_pos(int value)
                                            { _min = value; }
        void                            resize()
                                            { Fl::redraw(); SplitGroup::resize(x(), y(), w(), h()); }
        void                            resize(int X, int Y, int W, int H) override;
        int                             split_pos() const
                                            { return _split_pos; }
        void                            split_pos(int split_pos)
                                            { _split_pos = split_pos; }
        void                            toggle(SplitGroup::CHILD child, SplitGroup::DIRECTION direction, int second_size = -1);
        void                            toggle(SplitGroup::CHILD child, int second_size = -1)
                                            { toggle(child, _direction, second_size); }

    private:
        Fl_Widget*                      _widgets[2];
        DIRECTION                       _direction;
        bool                            _drag;
        int                             _min;
        int                             _split_pos;
    };
}



namespace flw {
    //--------------------------------------------------------------------------
    // A table display widget
    //
    class TableDisplay : public Fl_Group {
    public:
        enum class SELECT {
                                        NO,
                                        CELL,
                                        ROW,
        };

        enum class EVENT {
                                        UNDEFINED,
                                        CHANGED,
                                        CURSOR,
                                        COLUMN,
                                        COLUMN_CTRL,
                                        ROW,
                                        ROW_CTRL,
                                        SIZE,
        };

                                        TableDisplay(int X = 0, int Y = 0, int W = 0, int H = 0, const char* l = nullptr);
        void                            active_cell(int row = -1, int col = -1, bool show = false);
        virtual Fl_Align                cell_align(int row, int col)
                                            { (void) row; (void) col; return FL_ALIGN_LEFT; }
        virtual Fl_Color                cell_color(int row, int col)
                                            { (void) row; (void) col; return FL_BACKGROUND2_COLOR; }
        virtual Fl_Color                cell_textcolor(int row, int col)
                                            { (void) row; (void) col; return FL_FOREGROUND_COLOR; }
        virtual Fl_Font                 cell_textfont(int row, int col)
                                            { (void) row; (void) col; return flw::PREF_FONT; }
        virtual Fl_Fontsize             cell_textsize(int row, int col)
                                            { (void) row; (void) col; return flw::PREF_FONTSIZE; }
        virtual const char*             cell_value(int row, int col)
                                            { (void) row; (void) col; return ""; }
        virtual int                     cell_width(int col)
                                            { (void) col; return flw::PREF_FONTSIZE * 6; }
        virtual void                    cell_width(int col, int width)
                                            { (void) col; (void) width; }
        virtual void                    clear();
        int                             column() const
                                            { return _curr_col; }
        int                             columns() const
                                            { return _cols; }
        virtual void                    draw() override;
        void                            expand_last_column(bool expand = false)
                                            { _expand = expand; redraw(); }
        TableDisplay::EVENT             event() const
                                            { return _event; }
        int                             event_col() const
                                            { return _event_col; }
        int                             event_row() const
                                            { return _event_row; }
        virtual int                     handle(int event) override;
        void                            header(bool row = false, bool col = false);
        int                             height() const
                                            { return _height; }
        void                            height(int height)
                                            { _height = height; }
        void                            lines(bool ver = false, bool hor = false);
        void                            move_cursor(int pos);
        void                            resize_column_width(bool resize = false)
                                            { _resize = resize; }
        int                             row() const
                                            { return _curr_row; }
        int                             rows() const
                                            { return _rows; }
        void                            scrollbar(bool ver = true, bool hor = true)
                                            { _disable_ver = !ver; _disable_hor = !hor; redraw(); }
        void                            select_mode(TableDisplay::SELECT select = TableDisplay::SELECT::NO)
                                            { _select = select; }
        void                            show_cell(int row, int col);
        virtual void                    size(int rows, int cols);

    protected:
        int                             _cell_height(int Y = -1);
        int                             _cell_width(int col, int X = -1);
        virtual void                    _draw_cell(int row, int col, int X, int Y, int W, int H, bool ver, bool hor, bool current = false);
        void                            _draw_row(int row, int W, int Y, int H);
        void                            _draw_text(const char* string, int X, int Y, int W, int H, Fl_Align align);
        int                             _ev_keyboard_down();
        int                             _ev_mouse_click();
        int                             _ev_mouse_drag();
        int                             _ev_mouse_move();
        void                            _get_cell_below_mouse(int& row, int& col);
        void                            _update_scrollbars();
        void                            _set_event(int row, int col, TableDisplay::EVENT event)
                                            { _event_row = row; _event_col = col; _event = event; }

        static void                     _CallbackHor(Fl_Widget* w, void* v);
        static void                     _CallbackVer(Fl_Widget* w, void* v);

        TableDisplay::EVENT             _event;
        TableDisplay::SELECT            _select;
        Fl_Scrollbar*                   _hor;
        Fl_Scrollbar*                   _ver;
        Fl_Widget*                      _edit;
        bool                            _disable_hor;
        bool                            _disable_ver;
        bool                            _drag;
        bool                            _expand;
        bool                            _resize;
        bool                            _show_col_header;
        bool                            _show_hor_lines;
        bool                            _show_row_header;
        bool                            _show_ver_lines;
        int                             _cols;
        int                             _curr_col;
        int                             _curr_row;
        int                             _current_cell[4];
        int                             _event_col;
        int                             _event_row;
        int                             _height;
        int                             _resize_col;
        int                             _rows;
        int                             _start_col;
        int                             _start_row;
    };
}



namespace flw {
    //--------------------------------------------------------------------------
    // A table editing widget
    //
    class TableEditor : public TableDisplay {
    public:
        enum class FORMAT {
                                        DEFAULT,
                                        INT_DEF,
                                        INT_SEP,
                                        DEC_DEF,
                                        DEC_0,
                                        DEC_1,
                                        DEC_2,
                                        DEC_3,
                                        DEC_4,
                                        DEC_5,
                                        DEC_6,
                                        DEC_7,
                                        DEC_8,
                                        DATE_DEF,
                                        DATE_WORLD,
                                        DATE_US,
                                        SECRET_DEF,
                                        SECRET_DOT,
        };

        enum class REND {
                                        TEXT,
                                        INTEGER,
                                        NUMBER,
                                        BOOLEAN,
                                        SECRET,
                                        CHOICE,
                                        INPUT_CHOICE,
                                        SLIDER,
                                        VALUE_SLIDER,
                                        DLG_COLOR,
                                        DLG_FILE,
                                        DLG_DIR,
                                        DLG_DATE,
                                        DLG_LIST,
        };

        static const char*              SELECT_DATE;
        static const char*              SELECT_DIR;
        static const char*              SELECT_FILE;
        static const char*              SELECT_LIST;

                                        TableEditor(int X = 0, int Y = 0, int W = 0, int H = 0, const char* l = nullptr);
        void                            send_changed_event_always(bool value)
                                            { _send_changed_event_always = value; }
        virtual StringVector            cell_choice(int row, int col)
                                            { (void) row; (void) col; return StringVector(); }
        virtual bool                    cell_edit(int row, int col)
                                            { (void) row; (void) col; return false; }
        virtual TableEditor::FORMAT     cell_format(int row, int col)
                                            { (void) row; (void) col; return TableEditor::FORMAT::DEFAULT; }
        virtual TableEditor::REND       cell_rend(int row, int col)
                                            { (void) row; (void) col; return TableEditor::REND::TEXT; }
        virtual bool                    cell_value(int row, int col, const char* value)
                                            { (void) row; (void) col; (void) value; return false; }
        void                            clear() override;
        virtual int                     handle(int event) override;

        static const char*              FormatSlider(double val, double min, double max, double step);

    private:
        bool                            _send_changed_event_always;
        void                            _delete_current_cell();
        void                            _draw_cell(int row, int col, int X, int Y, int W, int H, bool ver, bool hor, bool current = false) override;
        void                            _edit_create();
        void                            _edit_quick(const char* key);
        void                            _edit_show();
        void                            _edit_start(const char* key = "");
        void                            _edit_stop(bool save = true);
        int                             _ev_keyboard_down();
        int                             _ev_mouse_click();
        int                             _ev_paste();
        void                            _update_scrollbars();

        Fl_Widget*                      _edit2;
        Fl_Widget*                      _edit3;
    };
}



namespace flw {
    class _TabsGroupButton;

    //--------------------------------------------------------------------------
    // Group widget that uses tabs for labels
    // You can move between tabs using alt + left/right
    // And move tabs around by using alt + up/down
    //
    class TabsGroup : public Fl_Group {
    public:
        enum class TABS {
                                        NORTH,
                                        SOUTH,
                                        WEST,
                                        EAST,
        };

                                        TabsGroup(int X = 0, int Y = 0, int W = 0, int H = 0, const char* l = nullptr);
        void                            add(const std::string& label, Fl_Widget* widget);
        Fl_Widget*                      child(int num) const;
        int                             children() const
                                            { return (int) _widgets.size(); }
        int                             find(Fl_Widget* widget) const;
        int                             handle(int event) override;
        void                            label(const std::string& label, Fl_Widget* widget);
        Fl_Widget*                      remove(int num);
        Fl_Widget*                      remove(Fl_Widget* widget)
                                            { return remove(find(widget)); }
        void                            resize()
                                            { Fl::redraw(); resize(x(), y(), w(), h()); }
        void                            resize(int X, int Y, int W, int H) override;
        void                            swap(int from, int to);
        TABS                            tabs()
                                            { return _tabs; }
        void                            tabs(TABS value)
                                            { _tabs = value; }
        Fl_Widget*                      value() const;
        void                            value(int num);
        void                            value(Fl_Widget* widget)
                                            { value(find(widget)); }

        static void                     BoxColor(Fl_Color boxcolor);
        static void                     BoxSelectionColor(Fl_Color boxcolor);
        static void                     BoxType(Fl_Boxtype boxtype);
        static void                     Callback(Fl_Widget* sender, void* object);

    private:
        Fl_Widget*                      _button();

        TABS                            _tabs;
        int                             _active;
        int                             _drag;
        int                             _pos;
        std::vector<_TabsGroupButton*>  _buttons;
        std::vector<Fl_Widget*>         _widgets;
    };
}



namespace flw {
    typedef std::map<std::string, std::string> StringMap;

    //--------------------------------------------------------------------------
    // A Table editing widget with built in data storage
    //
    class Grid : public TableEditor {
        friend class                    _TableChoice;

    public:
                                        Grid(int rows, int cols, int X = 0, int Y = 0, int W = 0, int H = 0, const char* l = nullptr);
        virtual                         ~Grid();
        Fl_Align                        cell_align(int row, int col) override;
        void                            cell_align(int row, int col, Fl_Align align);
        StringVector                    cell_choice(int row, int col) override;
        void                            cell_choice(int row, int col, const char* value);
        Fl_Color                        cell_color(int row, int col) override;
        void                            cell_color(int row, int col, Fl_Color color);
        bool                            cell_edit(int row, int col) override;
        void                            cell_edit(int row, int col, bool value);
        TableEditor::FORMAT             cell_format(int row, int col) override;
        void                            cell_format(int row, int col, TableEditor::FORMAT value);
        TableEditor::REND               cell_rend(int row, int col) override;
        void                            cell_rend(int row, int col, TableEditor::REND rend);
        Fl_Color                        cell_textcolor(int row, int col) override;
        void                            cell_textcolor(int row, int col, Fl_Color color);
        const char*                     cell_value(int row, int col) override;
        bool                            cell_value(int row, int col, const char* value) override;
        void                            cell_value2(int row, int col, const char* format, ...);
        int                             cell_width(int col) override;
        void                            cell_width(int col, int width) override;
        void                            size(int rows, int cols) override;

    private:
        int                             _get_int(StringMap& map, int row, int col, int def = 0);
        const char*                     _get_key(int row, int col);
        const char*                     _get_string(StringMap& map, int row, int col, const char* def = "");
        void                            _set_int(StringMap& map, int row, int col, int value);
        void                            _set_string(StringMap& map, int row, int col, const char* value);

        char                            _key[100];
        char*                           _buffer;
        StringMap                       _align;
        StringMap                       _cell;
        StringMap                       _choice;
        StringMap                       _color;
        StringMap                       _edit;
        StringMap                       _format;
        StringMap                       _rend;
        StringMap                       _textcolor;
        StringMap                       _width;
        StringVector                    _choices;
    };
}



class Fl_Preferences;
class Fl_Window;

namespace flw {
    namespace theme {
        bool                            is_dark();
        bool                            load(const std::string& name);
        void                            load_icon(Fl_Window* win, int win_resource, const char** xpm_resource = nullptr, const char* name = nullptr);
        std::string                     name();
        bool                            parse(int argc, const char** argv);
    }

    namespace dlg {
        void                            theme(bool enable_font = false, bool enable_fixedfont = false, Fl_Window* parent = nullptr);
    }

    namespace util {
        void                            pref_load(Fl_Preferences& pref, Fl_Window* window = nullptr);
        void                            pref_save(Fl_Preferences& pref, Fl_Window* window = nullptr);
    }
}



namespace flw {
    class _InputMenu;

    //--------------------------------------------------------------------------
    // First string in a vector is the latest pushed in
    //
    class InputMenu : public Fl_Group {
    public:
                                        InputMenu(const InputMenu&) = delete;
                                        InputMenu(InputMenu&&) = delete;
        InputMenu&                      operator=(const InputMenu&) = delete;
        InputMenu&                      operator=(InputMenu&&) = delete;

                                        InputMenu(int X = 0, int Y = 0, int W = 0, int H = 0, const char* l = nullptr);
        void                            clear();
        StringVector                    get_history() const;
        Fl_Input*                       input()
                                            { return (Fl_Input*) _input; }
        void                            insert(const std::string& string, int max_list_len);
        Fl_Menu_Button*                 menu()
                                            { return _menu; }
        void                            resize(int X, int Y, int W, int H) override;
        void                            set(const StringVector& list, bool copy_first_to_input = true);
        void                            update_pref(Fl_Font text_font = flw::PREF_FIXED_FONT, Fl_Fontsize text_size = flw::PREF_FIXED_FONTSIZE);
        const char*                     value() const;
        void                            value(const char* string);

        static void                     Callback(Fl_Widget*, void*);

    private:
        void                            _add(bool insert, const std::string& string, int max_list_len);
        void                            _add(bool insert, const StringVector& list);

        _InputMenu*                     _input;
        Fl_Menu_Button*                 _menu;
    };

    //--------------------------------------------------------------------------
    class ScrollBrowser : public Fl_Hold_Browser {
    public:
                                        ScrollBrowser(const ScrollBrowser&) = delete;
                                        ScrollBrowser(ScrollBrowser&&) = delete;
        ScrollBrowser&                  operator=(const ScrollBrowser&) = delete;
        ScrollBrowser&                  operator=(ScrollBrowser&&) = delete;

                                        ScrollBrowser(int scroll = 9, int X = 0, int Y = 0, int W = 0, int H = 0, const char* l = nullptr);
        void                            enable_menu(bool menu)
                                            { _flag_menu = menu; }
        void                            enable_pagemove(bool move)
                                            { _flag_move = move; }
        int                             handle(int event) override;
        Fl_Menu_Button*                 menu()
                                            { return _menu; }
        void                            update_pref(Fl_Font text_font = flw::PREF_FONT, Fl_Fontsize text_size = flw::PREF_FONTSIZE);

        static void                     Callback(Fl_Widget*, void*);
        static std::string              RemoveFormat(const char* text);

    private:
        Fl_Menu_Button*                 _menu;
        bool                            _flag_menu;
        bool                            _flag_move;
        int                             _scroll;
    };
}

#endif
