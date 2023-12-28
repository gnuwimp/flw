// Copyright gnuwimp@gmail.com
// Released under the GNU General Public License v3.0
// Optional compile flags:
// -DFLW_USE_PNG
#ifndef FLW_H
#define FLW_H


#include <string>
#include <vector>
#include <FL/Fl_Menu_.H>
#include <FL/Fl_Preferences.H>

namespace flw {
    extern int                          PREF_FIXED_FONT;        // Fixed font - FL_COURIER
    extern std::string                  PREF_FIXED_FONTNAME;    // Fixed font name - "FL_COURIER"
    extern int                          PREF_FIXED_FONTSIZE;    // Fixed font size - default 14
    extern int                          PREF_FONT;              // Default font - FL_HELVETICA
    extern int                          PREF_FONTSIZE;          // Default font size - 14
    extern std::string                  PREF_FONTNAME;          // Default font name - "FL_HELVETICA"
    extern std::vector<char*>           PREF_FONTNAMES;         // List of font names - used internally - load with theme::load_fonts()
    extern std::string                  PREF_THEME;             // Name of theme - default "default"
    extern const char* const            PREF_THEMES[];          // Name of themes
    extern int                          PREF_SCROLLBAR;         // Extra pixel width for scrollbar depending what theme are active - used internally

    typedef std::vector<std::string>    StringVector;

    //--------------------------------------------------------------------------
    // Buffer container that frees memory automatically
    //
    struct Buf {
        char*                           p;
        size_t                          s;

                                        Buf();
                                        Buf(size_t S);
                                        Buf(char* P, size_t S);
                                        Buf(const char* P, size_t S);
                                        Buf(const Buf& b);
                                        Buf(Buf&& b);
        Buf&                            operator=(const Buf& b);
        Buf&                            operator=(Buf&& b);
        Buf&                            operator+=(const Buf& b);
        bool                            operator==(const Buf& other) const;
        virtual                         ~Buf();
    };

    //--------------------------------------------------------------------------
    // Print widget sizes
    //
    namespace debug {
        void                            print(Fl_Widget* widget, bool tab = false);
        void                            print(Fl_Group* group);
    }

    //--------------------------------------------------------------------------
    // Menu item functions
    //
    namespace menu {
        void                            enable_item(Fl_Menu_* menu, const char* text, bool value);
        Fl_Menu_Item*                   get_item(Fl_Menu_* menu, const char* text);
        bool                            item_value(Fl_Menu_* menu, const char* text);
        void                            set_item(Fl_Menu_* menu, const char* text, bool value);
        void                            setonly_item(Fl_Menu_* menu, const char* text);
    }

    //--------------------------------------------------------------------------
    // Assorted utility functions
    //
    namespace util {
        void                            center_window(Fl_Window* window, Fl_Window* parent = nullptr);
        double                          clock();
        std::string                     fix_menu_string(std::string in);
        std::string                     format(const char* format, ...);
        std::string                     format_int(int64_t num, char del = ' ');
        void                            labelfont(Fl_Widget* widget);
        Buf                             load_file(std::string filename, bool alert = true);
        int32_t                         milliseconds();
        void                            png_save(std::string opt_name, Fl_Window* window, int X = 0, int Y = 0, int W = 0, int H = 0);
        std::string                     remove_browser_format(std::string text);
        std::string&                    replace(std::string& string, std::string find, std::string replace);
        bool                            save_file(std::string filename, const void* data, size_t size, bool alert = true);
        void                            sleep(int milli);
        StringVector                    split(const std::string& string, std::string split);
    }

    //--------------------------------------------------------------------------
    // Load different themes and save/load window preferences
    //
    namespace theme {
        bool                            is_dark();
        bool                            load(std::string name);
        int                             load_font(std::string requested_font);
        void                            load_fonts(bool iso8859_only = true);
        void                            load_icon(Fl_Window* win, int win_resource, const char** xpm_resource = nullptr, const char* name = nullptr);
        void                            load_theme_pref(Fl_Preferences& pref);
        void                            load_win_pref(Fl_Preferences& pref, Fl_Window* window, bool show = true, int defw = 800, int defh = 600, std::string basename = "gui.");
        bool                            parse(int argc, const char** argv);
        void                            save_theme_pref(Fl_Preferences& pref);
        void                            save_win_pref(Fl_Preferences& pref, Fl_Window* window, std::string basename = "gui.");

        //----------------------------------------------------------------------
        // Internal usage only
        //
        enum {
                                        THEME_DEFAULT,
                                        THEME_OXY,
                                        THEME_OXY_BLUE,
                                        THEME_OXY_TAN,
                                        THEME_GLEAM,
                                        THEME_GLEAM_BLUE,
                                        THEME_GLEAM_DARK_BLUE,
                                        THEME_GLEAM_DARK,
                                        THEME_GLEAM_DARKER,
                                        THEME_GLEAM_TAN,
                                        THEME_GTK,
                                        THEME_GTK_BLUE,
                                        THEME_GTK_DARK_BLUE,
                                        THEME_GTK_DARK,
                                        THEME_GTK_DARKER,
                                        THEME_GTK_TAN,
                                        THEME_PLASTIC,
                                        THEME_PLASTIC_BLUE,
                                        THEME_PLASTIC_TAN,
                                        THEME_SYSTEM,
                                        THEME_NIL,
        };
    }

    //--------------------------------------------------------------------------
    // Drawing colors
    //
    namespace color {
        extern Fl_Color                 AZURE;
        extern Fl_Color                 BEIGE;
        extern Fl_Color                 BLUE;
        extern Fl_Color                 BROWN;
        extern Fl_Color                 CYAN;
        extern Fl_Color                 GRAY;
        extern Fl_Color                 GREEN;
        extern Fl_Color                 LIME;
        extern Fl_Color                 MAGENTA;
        extern Fl_Color                 MAROON;
        extern Fl_Color                 NAVY;
        extern Fl_Color                 OLIVE;
        extern Fl_Color                 PINK;
        extern Fl_Color                 PURPLE;
        extern Fl_Color                 RED;
        extern Fl_Color                 SILVER;
        extern Fl_Color                 TEAL;
        extern Fl_Color                 YELLOW;
    }
}



#include <cstdint>
#include <string>

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
                                        LAST = YYYYMMDDHHMMSS,
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
                                        LAST = SUNDAY,
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
                                        LAST = YEAR_MONTH_LONG,
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
                                        LAST = SEC,
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



#include <FL/Fl_Group.H>
#include <FL/Fl_Menu_Button.H>
#include <FL/Fl_Scrollbar.H>
#include <vector>

namespace flw {
    class Chart;

    struct Price {
        std::string                     date;
        double                          close;
        double                          high;
        double                          low;
        double                          vol;

                                        Price();
                                        Price(const std::string& date_value, double value = 0.0);
                                        Price(const std::string& date, double high, double low, double close, double vol = 0.0);
        bool                            operator<(const Price& price) const { return date < price.date; }
        bool                            operator<=(const Price& price) const { return date <= price.date; }
        bool                            operator==(const Price& price) const { return date == price.date; }
        bool                            operator!=(const Price& price) const { return date != price.date; }
    };

    typedef std::vector<Price>          PriceVector;

    namespace chart {
        struct Area;
        struct Line;
        enum class TYPE;

        const int                       MIN_TICK  = 3;
        const int                       MAX_TICK  = 100;
        const size_t                    MAX_AREA  = 3;
        const size_t                    MAX_LINE  = 10;
        const double                    MIN_VAL   = -999'999'999'999'999.0;
        const double                    MAX_VAL   =  999'999'999'999'999.0;

        typedef std::vector<chart::Line> LineVector;
        typedef std::vector<chart::Area> AreaVector;

        size_t                          bsearch(const PriceVector& prices, const Price& key);
        bool                            has_high_low(TYPE chart_type);
        bool                            has_resizable_width(TYPE chart_type);
        bool                            has_time(Date::RANGE date_range);
        bool                            load_data(Chart* chart, std::string filename);
        bool                            save_data(const Chart* chart, std::string filename, double max_diff_high_low = 0.001);

        //----------------------------------------------------------------------
        enum class TYPE {
                                        LINE,
                                        BAR,
                                        VERTICAL,
                                        CLAMP_VERTICAL,
                                        HORIZONTAL,
                                        EXPAND_VERTICAL,
                                        EXPAND_HORIZONTAL,
                                        LAST = EXPAND_HORIZONTAL,
        };

        //----------------------------------------------------------------------
        // Data for one chart line
        //
        struct Line {
            Fl_Align                    align;
            TYPE                        type;
            Fl_Color                    color;
            bool                        visible;
            int                         width;
            double                      clamp_max;
            double                      clamp_min;
            std::string                 label;
            double                      max;
            double                      min;
            PriceVector                 points;

                                        Line();
                                        ~Line()
                                            { clear(); }
            void                        clear();
            void                        debug(int num) const;
            bool                        set(const PriceVector& points, std::string label, TYPE type, Fl_Align align, Fl_Color color, int width, double clamp_min, double clamp_max);
        };

        //----------------------------------------------------------------------
        // Each chart area can have an scale on the left and right side
        //
        struct Scale {
            double                      max;
            double                      min;
            double                      pixel;
            double                      tick;

                                        Scale();
            void                        calc(int height);
            void                        clear();
            void                        debug(const char* name) const;
            double                      diff() const
                                            { return max - min; }
            void                        fix_height();
        };

        //----------------------------------------------------------------------
        // Data for one chart area, currently max 3 areas in one widget can be shown at the same time
        // All are using the same date range
        //
        struct Area {
            size_t                      count;
            double                      h;
            Scale                       left;
            LineVector                  lines;
            int                         percent;
            Scale                       right;
            size_t                      selected;
            double                      w;
            double                      x;
            double                      y;

                                        Area();
            void                        clear(bool clear_data);
            void                        debug(int num) const;
            int                         x2() const
                                            { return (int) (x + w); }
            int                         y2() const
                                            { return (int) (y + h); }
        };
    }

    //--------------------------------------------------------------------------
    // Chart widget that is using dates for the x scale
    // It has an popup menu for a few settings and option to save chart to png image
    // It can also use date + hours/minutes/seconds
    // Use block_dates() to set an date list that removes those dates
    // Line types BAR, VERTICAL and CLAMP_VERTICAL can use line width of -1 which means it will expand to tick width - 1
    //
    class Chart : public Fl_Group {
        friend bool                     chart::save_data(const Chart*, std::string, double);

    public:
                                        Chart(int X = 0, int Y = 0, int W = 0, int H = 0, const char* l = nullptr);
        bool                            add_line(size_t area_0_to_2, const PriceVector& points, std::string line_label, chart::TYPE chart_type = chart::TYPE::LINE, Fl_Align line_align = FL_ALIGN_LEFT, Fl_Color line_color = FL_BLUE, int line_width = 1, double clamp_min = chart::MIN_VAL, double clamp_max = chart::MAX_VAL);
        bool                            area_size(int area1 = 100, int area2 = 0, int area3 = 0);
        void                            block_dates(const PriceVector& block_dates)
                                            { _block_dates = block_dates; }
        void                            clear();
        Date::FORMAT                    date_format() const
                                            { return _date_format; }
        bool                            date_range(Date::RANGE range = Date::RANGE::DAY);
        void                            debug() const;
        void                            draw() override;
        int                             handle(int event) override;
        bool                            has_time() const
                                            { return chart::has_time(_date_range); }
        void                            init(bool calc_dates);
        bool                            margin(int left_1_to_20 = 6, int right_1_to_20 = 1);
        void                            resize()
                                            { _old_width = _old_height = -1; resize(x(), y(), w(), h()); }
        void                            resize(int X, int Y, int W, int H) override;
        bool                            tick_width(int tick_width_from_3_to_100 = chart::MIN_TICK);
        void                            update_pref();
        void                            view_options(bool line_labels = true, bool hor_lines = true, bool ver_lines = true)
                                            { _view.labels = line_labels; _view.horizontal = hor_lines; _view.vertical = ver_lines; redraw(); }

    private:
        void                            _calc_area_height();
        void                            _calc_area_width();
        void                            _calc_dates();
        void                            _calc_ymin_ymax();
        void                            _calc_yscale();
        void                            _create_tooltip(bool ctrl);
        void                            _draw_area(const chart::Area& area);
        void                            _draw_line(const chart::Line& line, const chart::Scale& scale, int X, int Y, int W, int H);
        void                            _draw_line_labels(const chart::Area& area);
        void                            _draw_tooltip();
        void                            _draw_ver_lines(const chart::Area& area);
        void                            _draw_xlabels();
        void                            _draw_ylabels(int X, double Y1, double Y2, const chart::Scale& scale, bool left);
        chart::Area*                    _inside_area(int X, int Y);

        static void                     _CallbackDebug(Fl_Widget* widget, void* chart_object);
        static void                     _CallbackReset(Fl_Widget* widget, void* chart_object);
        static void                     _CallbackSavePng(Fl_Widget* widget, void* chart_object);
        static void                     _CallbackScrollbar(Fl_Widget* widget, void* chart_object);
        static void                     _CallbackToggle(Fl_Widget* widget, void* chart_object);

        struct {
            bool                        horizontal;
            bool                        labels;
            bool                        vertical;
        }                               _view;

        chart::Area*                    _area;
        chart::AreaVector               _areas;
        PriceVector                     _block_dates;
        int                             _bottom_space;
        int                             _cw;
        Date::FORMAT                    _date_format;
        Date::RANGE                     _date_range;
        int                             _date_start;
        PriceVector                     _dates;
        int                             _margin_left;
        int                             _margin_right;
        Fl_Menu_Button*                 _menu;
        int                             _old_height;
        int                             _old_width;
        Fl_Scrollbar*                   _scroll;
        int                             _tick_width;
        int                             _ticks;
        std::string                     _tooltip;
        int                             _top_space;
        int                             _ver_pos[101];
    };
}



#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Group.H>

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



#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Hold_Browser.H>
#include <FL/Fl_Hor_Fill_Slider.H>
#include <FL/Fl_Toggle_Button.H>

namespace flw {
    class ScrollBrowser;

    namespace dlg {
        //----------------------------------------------------------------------
        extern const char*              PASSWORD_CANCEL;
        extern const char*              PASSWORD_OK;

        void                            html(std::string title, const std::string& text, Fl_Window* parent = nullptr, int W = 40, int H = 23);
        void                            list(std::string title, const StringVector& list, Fl_Window* parent = nullptr, bool fixed_font = false, int W = 40, int H = 23);
        void                            list(std::string title, const std::string& list, Fl_Window* parent = nullptr, bool fixed_font = false, int W = 40, int H = 23);
        void                            list_file(std::string title, std::string file, Fl_Window* parent = nullptr, bool fixed_font = false, int W = 40, int H = 23);
        void                            panic(std::string message);
        bool                            password1(std::string title, std::string& password, Fl_Window* parent = nullptr);
        bool                            password2(std::string title, std::string& password, Fl_Window* parent = nullptr);
        bool                            password3(std::string title, std::string& password, std::string& file, Fl_Window* parent = nullptr);
        bool                            password4(std::string title, std::string& password, std::string& file, Fl_Window* parent = nullptr);
        int                             select(std::string title, const StringVector& list, int select_row, Fl_Window* parent = nullptr, bool fixed_font = false, int W = 40, int H = 23);
        int                             select(std::string title, const StringVector& list, const std::string& select_row, Fl_Window* parent = nullptr, bool fixed_font = false, int W = 40, int H = 23);
        void                            text(std::string title, const std::string& text, Fl_Window* parent = nullptr, int W = 40, int H = 23);
        bool                            text_edit(std::string title, std::string& text, Fl_Window* parent = nullptr, int W = 40, int H = 23);
        void                            theme(bool enable_font = false, bool enable_fixedfont = false, Fl_Window* parent = nullptr);

        //----------------------------------------------------------------------
        class AbortDialog : public Fl_Double_Window {
            using Fl_Double_Window::show;

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



#include <FL/Fl_Group.H>

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



#include <FL/Fl_Group.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Menu_Button.H>

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
}



#include <string>
#include <vector>

namespace flw {
namespace json {
    //----------------------------------------------------------------------
    // Very simple json parser
    // Don't use it for arbitrary data download from internet

    // Macros for creating json
    #define FLW_JSON_START(VEC, X)                      { json::NodeVector& V = VEC; int D = 0; char B[50]; X; (void) B; size_t I = 0; for (auto& n : V) n.index = I++; }
    #define FLW_JSON_START_OBJECT(X)                    { V.push_back(json::Node(json::OBJECT, "", "", D++)); X; V.push_back(json::Node(json::END_OBJECT, "", "", --D)); }
    #define FLW_JSON_START_ARRAY(X)                     { V.push_back(json::Node(json::ARRAY, "", "", D++)); X; V.push_back(json::Node(json::END_ARRAY, "", "", --D)); }
    #define FLW_JSON_START_ARRAY_NL(X)                  { V.push_back(json::Node(json::ARRAY_NL, "", "", D++)); X; V.push_back(json::Node(json::END_ARRAY, "", "", --D)); }

    #define FLW_JSON_ADD_OBJECT(NAME,X)                 { std::string N = json::escape_string(NAME); V.push_back(json::Node(json::OBJECT, N, "", D++)); X; V.push_back(json::Node(json::END_OBJECT, N, "", --D)); }
    #define FLW_JSON_ADD_ARRAY(NAME,X)                  { std::string N = json::escape_string(NAME); V.push_back(json::Node(json::ARRAY, N, "", D++)); X; V.push_back(json::Node(json::END_ARRAY, N, "", --D)); }
    #define FLW_JSON_ADD_ARRAY_NL(NAME,X)               { std::string N = json::escape_string(NAME); V.push_back(json::Node(json::ARRAY_NL, N, "", D++)); X; V.push_back(json::Node(json::END_ARRAY, N, "", --D)); }

    #define FLW_JSON_ADD_STRING(NAME,VALUE)             { V.push_back(json::Node(json::STRING, json::escape_string(NAME), json::escape_string(VALUE), D)); }
    #define FLW_JSON_ADD_NUMBER(NAME,VALUE)             { snprintf(B, 50, "%f", (double) VALUE); V.push_back(json::Node(json::NUMBER, json::escape_string(NAME), B, D)); }
    #define FLW_JSON_ADD_NUMBER2(NAME,VALUE)            { snprintf(B, 50, "%.2f", (double) VALUE); V.push_back(json::Node(json::NUMBER, json::escape_string(NAME), B, D)); }
    #define FLW_JSON_ADD_INT(NAME,VALUE)                { snprintf(B, 50, "%lld", (long long int) VALUE); V.push_back(json::Node(json::NUMBER, json::escape_string(NAME), B, D)); }
    #define FLW_JSON_ADD_UINT(NAME,VALUE)               { snprintf(B, 50, "%llu", (long long unsigned) VALUE); V.push_back(json::Node(json::NUMBER, json::escape_string(NAME), B, D)); }
    #define FLW_JSON_ADD_BOOL(NAME,VALUE)               { V.push_back(json::Node(json::BOOL, json::escape_string(NAME), VALUE == true ? "true" : "false", D)); }
    #define FLW_JSON_ADD_NIL(NAME)                      { V.push_back(json::Node(json::NIL, json::escape_string(NAME), "", D)); }

    #define FLW_JSON_ADD_STRING_TO_ARRAY(VALUE)         { V.push_back(json::Node(json::STRING, "", json::escape_string(VALUE), D)); }
    #define FLW_JSON_ADD_NUMBER_TO_ARRAY(VALUE)         { snprintf(B, 50, "%f", (double) VALUE); V.push_back(json::Node(json::NUMBER, "", B, D)); }
    #define FLW_JSON_ADD_NUMBER2_TO_ARRAY(VALUE)        { snprintf(B, 50, "%.2f", (double) VALUE); V.push_back(json::Node(json::NUMBER, "", B, D)); }
    #define FLW_JSON_ADD_INT_TO_ARRAY(VALUE)            { snprintf(B, 50, "%lld", (long long int) VALUE); V.push_back(json::Node(json::NUMBER, "", B, D)); }
    #define FLW_JSON_ADD_UINT_TO_ARRAY(VALUE)           { snprintf(B, 50, "%llu", (long long unsigned) VALUE); V.push_back(json::Node(json::NUMBER, "", B, D)); }
    #define FLW_JSON_ADD_BOOL_TO_ARRAY(VALUE)           { V.push_back(json::Node(json::BOOL, "", VALUE == true ? "true" : "false", D)); }
    #define FLW_JSON_ADD_NIL_TO_ARRAY()                 { V.push_back(json::Node(json::NIL, "", "", D)); }

    struct Err {
        ssize_t                         pos;
        ssize_t                         line;
                                        Err()
                                            { pos = -1; line = -1; }
                                        Err(ssize_t pos, ssize_t line)
                                            { this->pos = pos; this->line = line; }
    };

    enum TYPE {
                                        NA,
                                        OBJECT,
                                        END_OBJECT,
                                        ARRAY,
                                        END_ARRAY,
                                        STRING,
                                        NUMBER,
                                        BOOL,
                                        NIL,

                                        COLON, // Only for tokenizer
                                        COMMA, // Only for tokenizer
                                        ARRAY_NL, // Only for creating json
    };

    std::string                         escape_string(const std::string& string);
    std::string                         unescape_string(const std::string& string);

    struct Node {
        TYPE                            type;
        int                             depth;
        size_t                          index;
        std::string                     value;
        std::string                     name;
        size_t                          textpos;

                                        Node(TYPE type = TYPE::NA, const std::string& name = "", const std::string& value = "", int depth = 0, size_t textpos = 0)
                                            { this->type = type; this->name = name; this->value = value; this->depth = depth; this->textpos = textpos; }
        bool                            operator==(const Node& other) const
                                            { return (type == other.type || (type == TYPE::ARRAY && other.type == TYPE::ARRAY_NL) || (type == TYPE::ARRAY_NL && other.type == TYPE::ARRAY)) && depth == other.depth && value == other.value && name == other.name; }
        bool                            is_array() const
                                            { return type == json::ARRAY; }
        bool                            is_bool() const
                                            { return type == json::BOOL; }
        bool                            is_data() const
                                            { return type == json::STRING || type == json::NUMBER || type == json::BOOL || type == json::NIL; }
        bool                            is_end() const
                                            { return type == json::END_ARRAY || type == json::END_OBJECT; }
        bool                            is_nil() const
                                            { return type == json::NIL; }
        bool                            is_number() const
                                            { return type == json::NUMBER; }
        bool                            is_object() const
                                            { return type == json::OBJECT; }
        bool                            is_start() const
                                            { return type == json::ARRAY || type == json::ARRAY_NL || type == json::OBJECT; }
        bool                            is_string() const
                                            { return type == json::STRING; }
        void                            print() const;
        bool                            tobool() const
                                            { return value == "true"; }
        long long                       toint() const
                                            { return (type == json::NUMBER) ? strtoll(value.c_str(), nullptr, 0) : 0; }
        double                          tonumber() const
                                            { return (type == json::NUMBER) ? strtod(value.c_str(), nullptr) : 0.0; }
        std::string                     tostring() const
                                            { return unescape_string(value); }
        std::string                     unescape_name() const
                                            { return unescape_string(name); }
    };

    typedef std::vector<Node>           NodeVector;
    typedef std::vector<size_t>         SizeTVector;

    NodeVector                          find_children(const NodeVector& nodes, const Node& start, bool grandchildren = false);
    NodeVector                          find_nodes(const NodeVector& nodes, std::string name, TYPE type = TYPE::NA);
    NodeVector                          find_siblings(const NodeVector& nodes, const Node& start);
    Err                                 parse(const char* json, NodeVector& nodes, bool ignore_trailing_comma = false);
    Err                                 parse(std::string json, NodeVector& nodes, bool ignore_trailing_comma = false);
    void                                print(const NodeVector& nodes);
    std::string                         tostring(const NodeVector& nodes);
    Err                                 validate(const char* json);
    Err                                 validate(std::string json);
} // json
} // flw



#include <FL/Fl_Box.H>

namespace flw {
    //--------------------------------------------------------------------------
    // A 7 segment number label widget
    //
    class LcdNumber : public Fl_Box {
    public:
                                        LcdNumber(int X = 0, int Y = 0, int W = 0, int H = 0, const char* l = nullptr);
        Fl_Align                        align() const
                                            { return _align; }
        void                            align(Fl_Align align)
                                            { _align = align; Fl::redraw(); }
        int                             dot_size() const
                                            { return _dot_size; }
        void                            dot_size(int size)
                                            { _dot_size = size; Fl::redraw(); }
        void                            draw() override;
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
        void                            value(const char* value);

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



#include <FL/Fl_Text_Display.H>
#include <string>

namespace flw {
    namespace logdisplay {
        enum class COLOR {
                                        FOREGROUND          = 'A',
                                        GRAY                = 'B',
                                        RED                 = 'C',
                                        GREEN               = 'D',
                                        BLUE                = 'E',
                                        MAGENTA             = 'F',
                                        YELLOW              = 'G',
                                        CYAN                = 'H',
                                        BOLD_FOREGROUND     = 'I',
                                        BOLD_GRAY           = 'J',
                                        BOLD_RED            = 'K',
                                        BOLD_GREEN          = 'L',
                                        BOLD_BLUE           = 'M',
                                        BOLD_MAGENTA        = 'N',
                                        BOLD_YELLOW         = 'O',
                                        BOLD_CYAN           = 'P',
                                        BG_FOREGROUND       = 'Q',
                                        BG_GRAY             = 'R',
                                        BG_RED              = 'S',
                                        BG_GREEN            = 'T',
                                        BG_BLUE             = 'U',
                                        BG_MAGENTA          = 'V',
                                        BG_YELLOW           = 'W',
                                        BG_CYAN             = 'X',
                                        BG_BOLD_FOREGROUND  = 'Y',
                                        BG_BOLD_GRAY        = 'Z',
                                        BG_BOLD_RED         = '[',
                                        BG_BOLD_GREEN       = '\\',
                                        BG_BOLD_BLUE        = ']',
                                        BG_BOLD_MAGENTA     = '^',
                                        BG_BOLD_YELLOW      = '_',
                                        BG_BOLD_CYAN        = '`',
                                        LAST                = '`',
        };

        static const size_t             ALL_STRINGS = 0;
        extern Fl_Color                 BG_COLOR;
        extern Fl_Color                 BG_DARK_COLOR;
        extern size_t                   MAX_LINE_LEN;
        extern size_t                   TURN_WRAP_ON;
    }

    //--------------------------------------------------------------------------
    // A static text display widget with line based text coloring
    // LogDisplay will manage text and style buffers
    // Search text with ctrl + 'f', F3, shift + F3
    //
    // Implement line_cb() method which will be called for every line
    // If you are doing more then one color call per line,
    //   you can disable changing colors that has already been set
    //   by calling lock_color(true)
    //
    // If json string is used then line_cb() doesn't have to be overidden
    // But it is less flexible
    // Edit json string in the widget with ctrl + 'e'
    //
    // BG colors only work in fltk 1.4
    //
    class LogDisplay : public Fl_Text_Display {
    public:
                                        LogDisplay(int X = 0, int Y = 0, int W = 0, int H = 0, const char* l = nullptr);
                                        ~LogDisplay();
        void                            edit_styles();
        void                            find(bool next, bool force_ask);
        int                             handle(int event) override;
        void                            lock_colors(bool lock)
                                            { _lock_colors = lock; }
        void                            save_file();
        void                            style(std::string json = "");
        void                            update_pref();
        void                            value(const char* text);

    protected:
        virtual void                    line_cb(size_t row, const std::string& line)
                                            { (void) row; (void) line; }
        virtual void                    line_custom_cb(size_t row, const std::string& line, const std::string& word1, const std::string& word2, logdisplay::COLOR color, bool inclusive, size_t start = 0, size_t stop = 0, size_t count = 0)
                                            { (void) row; (void) line; (void) word1; (void) word2; (void) color;  (void) inclusive;  (void) start;  (void) stop;  (void) count; }
        void                            style_between(const std::string& line, const std::string& word1, const std::string& word2, bool inclusive, logdisplay::COLOR color);
        void                            style_line(size_t start, size_t stop, logdisplay::COLOR c);
        void                            style_num(const std::string& line, logdisplay::COLOR color, size_t count = logdisplay::ALL_STRINGS);
        void                            style_range(const std::string& line, const std::string& word1, const std::string& word2, bool inclusive, logdisplay::COLOR color, size_t count = logdisplay::ALL_STRINGS);
        void                            style_rstring(const std::string& line, const std::string& word1, logdisplay::COLOR color, size_t count = logdisplay::ALL_STRINGS);
        void                            style_string(const std::string& line, const std::string& word1, logdisplay::COLOR color, size_t count = logdisplay::ALL_STRINGS);

    private:
        const char*                     _check_text(const char* text);
        char                            _style_char(size_t pos) const
                                            { pos += _tmp->pos; return (pos < _tmp->size) ? _tmp->buf[pos] : 0; }

        struct Tmp {
            char*                       buf;
            size_t                      len;
            size_t                      pos;
            size_t                      size;

                                        Tmp();
                                        ~Tmp();
        };

        Fl_Text_Buffer*                 _buffer;
        std::string                     _find;
        bool                            _lock_colors;
        std::string                     _json;
        Fl_Text_Buffer*                 _style;
        Tmp*                            _tmp;
    };
}



#include <FL/Fl_Group.H>
#include <FL/Fl_Menu_Button.H>

namespace flw {
    class Plot;

    namespace plot {
        struct Point;
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



#include <FL/Fl_Menu_.H>
#include <FL/Fl_Preferences.H>

namespace flw {
    //--------------------------------------------------------------------------
    class RecentMenu {
    public:
                                        RecentMenu(Fl_Menu_* menu, Fl_Callback* file_callback, void* userdata, std::string base_label = "&File/Open recent", std::string clear_label = "/Clear");
        void                            append(std::string file)
                                            { return _add(file, true); }
        static void                     CallbackClear(Fl_Widget*, void* o);
        void                            insert(std::string file)
                                            { return _add(file, false); }
        StringVector                    items() const
                                            { return _files; }
        void                            max_items(size_t max)
                                            { if (max > 0 && max <= 100) _max = max; }
        Fl_Menu_*                       menu()
                                            { return _menu; }
        void                            load_pref(Fl_Preferences& pref, std::string base_name = "files");
        void                            save_pref(Fl_Preferences& pref, std::string base_name = "files");

    private:
        void                            _add(std::string file, bool append);
        size_t                          _add_string(StringVector& in, size_t max_size, std::string string);
        size_t                          _insert_string(StringVector& in, size_t max_size, std::string string);

        std::string                     _base;
        Fl_Callback*                    _callback;
        std::string                     _clear;
        StringVector                    _files;
        size_t                          _max;
        Fl_Menu_*                       _menu;
        void*                           _user;
    };
}



#include <FL/Fl_Hold_Browser.H>
#include <FL/Fl_Menu_Button.H>

namespace flw {
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

    private:
        Fl_Menu_Button*                 _menu;
        bool                            _flag_menu;
        bool                            _flag_move;
        int                             _scroll;
    };
}



#include <FL/Fl_Group.H>
#include <FL/Fl.H>

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



#include <map>
#include <FL/Fl_Group.H>
#include <FL/Fl_Scrollbar.H>

namespace flw {
    //--------------------------------------------------------------------------
    // A table display widget
    //
    class TableDisplay : public Fl_Group {
        friend class _TableDisplayFindDialog;

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
        enum class _TABLEDISPLAY_MOVE {
                                        DOWN,
                                        FIRST_COL,
                                        FIRST_ROW,
                                        LAST_COL,
                                        LAST_ROW,
                                        LEFT,
                                        PAGE_DOWN,
                                        PAGE_UP,
                                        RIGHT,
                                        SCROLL_DOWN,
                                        SCROLL_LEFT,
                                        SCROLL_RIGHT,
                                        SCROLL_UP,
                                        UP,
        };

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
        void                            _move_cursor(_TABLEDISPLAY_MOVE move);
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
        std::string                     _find;
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



#include <string>
#include <map>

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

        char*                           _buffer;
        StringMap                       _cell_align;
        StringMap                       _cell_choice;
        StringVector                    _cell_choices;
        StringMap                       _cell_color;
        StringMap                       _cell_edit;
        StringMap                       _cell_format;
        StringMap                       _cell_rend;
        StringMap                       _cell_textcolor;
        StringMap                       _cell_value;
        StringMap                       _cell_width;
        char                            _key[100];
    };
}



#include <string>
#include <vector>
#include <FL/Fl_Group.H>
#include <FL/Fl.H>

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
        void                            hide_tabs()
                                            { _hide_tab_buttons(true); }
        void                            label(const std::string& label, Fl_Widget* widget);
        Fl_Widget*                      remove(int num);
        Fl_Widget*                      remove(Fl_Widget* widget)
                                            { return remove(find(widget)); }
        void                            resize()
                                            { Fl::redraw(); resize(x(), y(), w(), h()); }
        void                            resize(int X, int Y, int W, int H) override;
        void                            show_tabs()
                                            { _hide_tab_buttons(false); }
        void                            swap(int from, int to);
        TABS                            tabs()
                                            { return _tabs; }
        void                            tabs(TABS value)
                                            { _tabs = value; }
        bool                            tabs_visible() const
                                            { return _hide == false; }
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
        void                            _hide_tab_buttons(bool hide);

        int                             _active;
        std::vector<_TabsGroupButton*>  _buttons;
        bool                            _drag;
        bool                            _hide;
        int                             _pos;
        TABS                            _tabs;
        std::vector<Fl_Widget*>         _widgets;
    };
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

#endif
