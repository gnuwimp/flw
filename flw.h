// Copyright gnuwimp@gmail.com
// Released under the GNU General Public License v3.0
// Optional compile flags:
// -DFLW_USE_PNG
#ifndef FLW_H
#define FLW_H
#include <string>
#include <vector>
#include <cmath>
#include <FL/Fl_Group.H>
#include <FL/Fl_Menu_.H>
#include <FL/Fl_Preferences.H>
#include <FL/Fl_PostScript.H>
#ifdef DEBUG
#include <iostream>
#include <iomanip>
#define FLW_LINE                        { ::printf("\033[31m%6u: \033[34m%s::%s\033[0m\n", __LINE__, __FILE__, __func__); fflush(stdout); }
#define FLW_RED                         { ::printf("\033[7m\033[31m%6u: %s::%s  \033[0m\n", __LINE__, __FILE__, __func__); fflush(stdout); }
#define FLW_GREEN                       { ::printf("\033[7m\033[32m%6u: %s::%s  \033[0m\n", __LINE__, __FILE__, __func__); fflush(stdout); }
#define FLW_BLUE                        { ::printf("\033[7m\033[34m%6u: %s::%s  \033[0m\n", __LINE__, __FILE__, __func__); fflush(stdout); }
#define FLW_PRINT(...)                  FLW_PRINT_MACRO(__VA_ARGS__, FLW_PRINT7, FLW_PRINT6, FLW_PRINT5, FLW_PRINT4, FLW_PRINT3, FLW_PRINT2, FLW_PRINT1)(__VA_ARGS__);
#define FLW_PRINT1(A)                   { std::cout << "\033[31m" << std::setw(6) << __LINE__ << ": \033[34m" << __func__ << "\033[0m: " #A "=" << (A) << std::endl; fflush(stdout); }
#define FLW_PRINT2(A,B)                 { std::cout << "\033[31m" << std::setw(6) << __LINE__ << ": \033[34m" << __func__ << "\033[0m: " #A "=" << (A) << ", " #B "=" << (B) << std::endl; fflush(stdout); }
#define FLW_PRINT3(A,B,C)               { std::cout << "\033[31m" << std::setw(6) << __LINE__ << ": \033[34m" << __func__ << "\033[0m: " #A "=" << (A) << ", " #B "=" << (B) << ", " #C "=" << (C) << std::endl; fflush(stdout); }
#define FLW_PRINT4(A,B,C,D)             { std::cout << "\033[31m" << std::setw(6) << __LINE__ << ": \033[34m" << __func__ << "\033[0m: " #A "=" << (A) << ", " #B "=" << (B) << ", " #C "=" << (C) << ", " #D "=" << (D) << std::endl; fflush(stdout); }
#define FLW_PRINT5(A,B,C,D,E)           { std::cout << "\033[31m" << std::setw(6) << __LINE__ << ": \033[34m" << __func__ << "\033[0m: " #A "=" << (A) << ", " #B "=" << (B) << ", " #C "=" << (C) << ", " #D "=" << (D) << ", " #E "=" << (E) << std::endl; fflush(stdout); }
#define FLW_PRINT6(A,B,C,D,E,F)         { std::cout << "\033[31m" << std::setw(6) << __LINE__ << ": \033[34m" << __func__ << "\033[0m: " #A "=" << (A) << ", " #B "=" << (B) << ", " #C "=" << (C) << ", " #D "=" << (D) << ", " #E "=" << (E) << ", " #F "=" << (F) << std::endl; fflush(stdout); }
#define FLW_PRINT7(A,B,C,D,E,F,G)       { std::cout << "\033[31m" << std::setw(6) << __LINE__ << ": \033[34m" << __func__ << "\033[0m: " #A "=" << (A) << ", " #B "=" << (B) << ", " #C "=" << (C) << ", " #D "=" << (D) << ", " #E "=" << (E) << ", " #F "=" << (F) << ", " #G "=" << (G) << std::endl; fflush(stdout); }
#define FLW_PRINT_MACRO(A,B,C,D,E,F,G,N,...) N
#define FLW_ASSERT(X,Y)                 flw::debug::test(X,Y,__LINE__,__func__);
#define FLW_TRUE(X)                     flw::debug::test(X,__LINE__,__func__);
#define FLW_ASSERTD(X,Y,Z)              flw::debug::test(X,Y,Z,__LINE__,__func__);
#else
#define FLW_LINE
#define FLW_RED
#define FLW_GREEN
#define FLW_BLUE
#define FLW_PRINT(...)
#define FLW_ASSERT(X,Y)
#define FLW_TRUE(X)
#define FLW_ASSERTD(X,Y,Z)
#endif
namespace flw {
extern int                      PREF_FIXED_FONT;
extern std::string              PREF_FIXED_FONTNAME;
extern int                      PREF_FIXED_FONTSIZE;
extern Fl_Font                  PREF_FONT;
extern int                      PREF_FONTSIZE;
extern std::string              PREF_FONTNAME;
extern std::vector<char*>       PREF_FONTNAMES;
extern std::string              PREF_THEME;
extern const char* const        PREF_THEMES[];
typedef std::vector<std::string> StringVector;
typedef std::vector<void*>       VoidVector;
typedef std::vector<Fl_Widget*>  WidgetVector;
typedef bool (*PrintCallback)(void* data, int pw, int ph, int page);
namespace debug {
    void                        print(const Fl_Widget* widget);
    void                        print(const Fl_Widget* widget, std::string& indent);
    bool                        test(bool val, int line, const char* func);
    bool                        test(const char* ref, const char* val, int line, const char* func);
    bool                        test(int64_t ref, int64_t val, int line, const char* func);
    bool                        test(double ref, double val, double diff, int line, const char* func);
}
namespace menu {
    void                        enable_item(Fl_Menu_* menu, const char* text, bool value);
    Fl_Menu_Item*               get_item(Fl_Menu_* menu, const char* text);
    Fl_Menu_Item*               get_item(Fl_Menu_* menu, void* v);
    bool                        item_value(Fl_Menu_* menu, const char* text);
    void                        set_item(Fl_Menu_* menu, const char* text, bool value);
    void                        setonly_item(Fl_Menu_* menu, const char* text);
}
namespace util {
    void                        center_window(Fl_Window* window, Fl_Window* parent = nullptr);
    double                      clock();
    Fl_Widget*                  find_widget(Fl_Group* group, std::string label);
    std::string                 fix_menu_string(std::string in);
    std::string                 format(const char* format, ...);
    std::string                 format_int(int64_t num, char del = ' ');
    bool                        is_whitespace_or_empty(const char* str);
    void                        labelfont(Fl_Widget* widget, Fl_Font fn = flw::PREF_FONT, int fs = flw::PREF_FONTSIZE);
    int32_t                     milliseconds();
    bool                        png_save(std::string opt_name, Fl_Window* window, int X = 0, int Y = 0, int W = 0, int H = 0);
    std::string                 print(std::string ps_filename, Fl_Paged_Device::Page_Format format, Fl_Paged_Device::Page_Layout layout, PrintCallback cb, void* data);
    std::string                 print(std::string ps_filename, Fl_Paged_Device::Page_Format format, Fl_Paged_Device::Page_Layout layout, PrintCallback cb, void* data, int from, int to);
    std::string                 remove_browser_format(const char* text);
    std::string&                replace_string(std::string& string, std::string find, std::string replace);
    void                        sleep(int milli);
    StringVector                split_string(const std::string& string, std::string split);
    double                      to_double(std::string s, double def = INFINITY);
    long long                   to_long(std::string s, long long def = 0);
    static inline std::string   to_string(const char* text)
                                    { return text != nullptr ? text : ""; }
    void*                       zero_memory(char* mem, size_t size);
}
namespace theme {
    bool                        is_dark();
    bool                        load(std::string name);
    int                         load_font(std::string requested_font);
    void                        load_fonts(bool iso8859_only = true);
    void                        load_icon(Fl_Window* win, int win_resource, const char** xpm_resource = nullptr, const char* name = nullptr);
    void                        load_theme_pref(Fl_Preferences& pref);
    void                        load_win_pref(Fl_Preferences& pref, Fl_Window* window, int show_0_1_2 = 1, int defw = 800, int defh = 600, std::string basename = "gui.");
    bool                        parse(int argc, const char** argv);
    void                        save_theme_pref(Fl_Preferences& pref);
    void                        save_win_pref(Fl_Preferences& pref, Fl_Window* window, std::string basename = "gui.");
    enum {
                                THEME_DEFAULT,
                                THEME_GLEAM,
                                THEME_GLEAM_BLUE,
                                THEME_GLEAM_DARK,
                                THEME_GLEAM_TAN,
                                THEME_GTK,
                                THEME_GTK_BLUE,
                                THEME_GTK_DARK,
                                THEME_GTK_TAN,
                                THEME_OXY,
                                THEME_OXY_TAN,
                                THEME_PLASTIC,
                                THEME_PLASTIC_TAN,
                                THEME_NIL,
    };
}
namespace color {
    extern Fl_Color             BEIGE;
    extern Fl_Color             CHOCOLATE;
    extern Fl_Color             CRIMSON;
    extern Fl_Color             DARKOLIVEGREEN;
    extern Fl_Color             DODGERBLUE;
    extern Fl_Color             FORESTGREEN;
    extern Fl_Color             GOLD;
    extern Fl_Color             GRAY;
    extern Fl_Color             INDIGO;
    extern Fl_Color             OLIVE;
    extern Fl_Color             PINK;
    extern Fl_Color             ROYALBLUE;
    extern Fl_Color             SIENNA;
    extern Fl_Color             SILVER;
    extern Fl_Color             SLATEGRAY;
    extern Fl_Color             TEAL;
    extern Fl_Color             TURQUOISE;
    extern Fl_Color             VIOLET;
}
struct Buf {
    char*                       p;
    size_t                      s;
                                Buf();
    explicit                    Buf(size_t S);
                                Buf(const char* P, size_t S, bool grab = false);
                                Buf(const Buf& b);
                                Buf(Buf&& b);
    Buf&                        operator=(const Buf& b);
    Buf&                        operator=(Buf&& b);
    Buf&                        operator+=(const Buf& b);
    bool                        operator==(const Buf& other) const;
    virtual                     ~Buf()
                                    { free(p); }
};
struct File {
    enum class TYPE {
                                NA,
                                DIR,
                                FILE,
                                OTHER,
    };
    int64_t                     size;
    int64_t                     mtime;
    TYPE                        type;
                                File()
                                    { size = mtime = 0; type = TYPE::NA; }
                                File(std::string filename);
    static Buf                  Load(std::string filename, bool alert = true);
    static bool                 Save(std::string filename, const char* data, size_t size, bool alert = true);
    static inline bool          Save(std::string filename, const Buf& buf, bool alert = true)
                                    { return File::Save(filename, buf.p, buf.s, alert); }
};
class PrintText {
public:
                                PrintText(std::string filename,
                                    Fl_Paged_Device::Page_Format format = Fl_Paged_Device::Page_Format::A4,
                                    Fl_Paged_Device::Page_Layout layout = Fl_Paged_Device::Page_Layout::PORTRAIT,
                                    Fl_Font font = FL_COURIER,
                                    Fl_Fontsize fontsize = 14,
                                    Fl_Align align = FL_ALIGN_LEFT,
                                    bool wrap = true,
                                    bool border = false,
                                    int line_num = 0);
                                ~PrintText();
    Fl_Fontsize                 fontsize() const
                                    { return _fontsize; }
    int                         page_count() const
                                    { return _page_count; }
    std::string                 print(const char* text, unsigned replace_tab_with_space = 0);
    std::string                 print(const std::string& text, unsigned replace_tab_with_space = 0);
    std::string                 print(const StringVector& lines, unsigned replace_tab_with_space = 0);
private:
    void                        check_for_new_page();
    void                        measure_lw_lh(const std::string& text);
    void                        print_line(const std::string& line);
    void                        print_wrapped_line(const std::string& line);
    std::string                 start();
    std::string                 stop();
    Fl_Align                    _align;
    Fl_Font                     _font;
    Fl_Fontsize                 _fontsize;
    Fl_PostScript_File_Device*  _printer;
    Fl_Paged_Device::Page_Format _page_format;
    Fl_Paged_Device::Page_Layout _page_layout;
    FILE*                       _file;
    bool                        _border;
    bool                        _wrap;
    int                         _lh;
    int                         _line_count;
    int                         _line_num;
    int                         _lw;
    int                         _nw;
    int                         _page_count;
    int                         _ph;
    int                         _pw;
    int                         _px;
    int                         _py;
    std::string                 _filename;
};
}
#include <cstdint>
#include <string>
namespace flw {
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
    static const int            SECS_PER_HOUR  = 3600;
    static const int            SECS_PER_DAY   = 3600 * 24;
    static const int            SECS_PER_WEEK  = 3600 * 24 * 7;
    explicit                    Date(bool utc = false);
                                Date(const Date& other);
                                Date(Date&&);
                                Date(int year, int month, int day, int hour = 0, int min = 0, int sec = 0);
    Date&                       operator=(const Date& other);
    Date&                       operator=(Date&&);
    bool                        operator<(const Date& other) const
                                    { return compare(other) < 0 ? true : false; }
    bool                        operator<=(const Date& other) const
                                    { return compare(other) <= 0 ? true : false; }
    bool                        operator>(const Date& other) const
                                    { return compare(other) > 0 ? true : false; }
    bool                        operator>=(const Date& other) const
                                    { return compare(other) >= 0 ? true : false; }
    bool                        operator==(const Date& other) const
                                    { return compare(other) == 0 ? true : false; }
    bool                        operator!=(const Date& other) const
                                    { return compare(other) != 0 ? true : false; }
    bool                        add_days(int days);
    bool                        add_months(int months);
    bool                        add_seconds(int64_t seconds);
    int                         compare(const Date& other, Date::COMPARE flag = Date::COMPARE::YYYYMMDDHHMMSS) const;
    int                         day() const
                                    { return _day; }
    Date&                       day(int day);
    Date&                       day_last();
    int                         diff_days(const Date& other) const;
    int                         diff_months(const Date& other) const;
    int                         diff_seconds(const Date& other) const;
    std::string                 format(Date::FORMAT format = Date::FORMAT::ISO) const;
    int                         hour() const
                                    { return _hour; }
    Date&                       hour(int hour);
    bool                        is_invalid() const
                                    { return _year == 0 || _month == 0 || _day == 0; }
    bool                        is_leapyear() const;
    int                         minute() const
                                    { return _min; }
    Date&                       minute(int min);
    int                         month() const
                                    { return _month; }
    Date&                       month(int month);
    int                         month_days() const;
    const char*                 month_name() const;
    const char*                 month_name_short() const;
    void                        print() const;
    int                         second() const
                                    { return _sec; }
    Date&                       second(int sec);
    Date&                       set(const Date& other);
    Date&                       set(int year, int month, int day, int hour = 0, int min = 0, int sec = 0);
    int64_t                     time() const;
    int                         week() const;
    Date::DAY                   weekday() const;
    Date&                       weekday(Date::DAY weekday);
    const char*                 weekday_name() const;
    const char*                 weekday_name_short() const;
    int                         year() const
                                    { return _year; }
    Date&                       year(int year);
    int                         yearday() const;
    static bool                 Compare(const Date& a, const Date& b);
    static Date                 FromString(const char* string, bool us = false);
    static Date                 FromTime(int64_t seconds, bool utc = false);
    static Date                 InvalidDate();
private:
    short                       _year;
    char                        _month;
    char                        _day;
    char                        _hour;
    char                        _min;
    char                        _sec;
};
}
#include <FL/Fl_Group.H>
#include <FL/Fl_Menu_Button.H>
#include <FL/Fl_Rect.H>
#include <FL/Fl_Scrollbar.H>
#include <cmath>
#include <optional>
#include <vector>
namespace flw {
class  ChartArea;
struct ChartData;
class  ChartLine;
typedef std::vector<ChartData>  ChartDataVector;
typedef std::vector<ChartLine>  ChartLineVector;
typedef std::vector<ChartArea>  ChartAreaVector;
struct ChartData {
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
    enum class FORMULAS {
                                ATR,
                                DAY_TO_MONTH,
                                DAY_TO_WEEK,
                                EXP_MOVING_AVERAGE,
                                FIXED,
                                MODIFY,
                                MOMENTUM,
                                MOVING_AVERAGE,
                                RSI,
                                STD_DEV,
                                STOCHASTICS,
                                LAST = STOCHASTICS,
    };
    enum class MODIFY {
                                ADDITION,
                                SUBTRACTION,
                                MULTIPLICATION,
                                DIVISION,
                                LAST = DIVISION,
    };
    std::string                 date;
    double                      close;
    double                      high;
    double                      low;
    explicit                    ChartData();
    explicit                    ChartData(std::string date_value, double value = 0.0);
    explicit                    ChartData(std::string date, double high, double low, double close);
    void                        debug() const;
    bool                        operator<(const ChartData& other) const { return date < other.date; }
    bool                        operator<=(const ChartData& other) const { return date <= other.date; }
    bool                        operator==(const ChartData& other) const { return date == other.date; }
    bool                        operator!=(const ChartData& other) const { return date != other.date; }
    static ChartDataVector      ATR(const ChartDataVector& in, size_t days);
    static size_t               BinarySearch(const ChartDataVector& in, const ChartData& key);
    static ChartDataVector      DateSerie(std::string start_date, std::string stop_date, ChartData::RANGE range, const ChartDataVector& block = ChartDataVector());
    static ChartDataVector      DayToMonth(const ChartDataVector& in, bool sum = false);
    static ChartDataVector      DayToWeek(const ChartDataVector& in, Date::DAY weekday, bool sum = false);
    static void                 Debug(const ChartDataVector& in);
    static ChartDataVector      ExponentialMovingAverage(const ChartDataVector& in, size_t days);
    static ChartDataVector      Fixed(const ChartDataVector& in, double value);
    static ChartDataVector      LoadCSV(std::string filename, std::string sep = ",");
    static ChartDataVector      Modify(const ChartDataVector& in, ChartData::MODIFY modify, double value);
    static ChartDataVector      Momentum(const ChartDataVector& in, size_t days);
    static ChartDataVector      MovingAverage(const ChartDataVector& in, size_t days);
    static ChartDataVector      RSI(const ChartDataVector& in, size_t days);
    static bool                 SaveCSV(const ChartDataVector& in, std::string filename, std::string sep = ",");
    static ChartDataVector      StdDev(const ChartDataVector& in, size_t days);
    static ChartDataVector      Stochastics(const ChartDataVector& in, size_t days);
};
class ChartLine {
public:
    static const int            MAX_WIDTH = 25;
    enum class TYPE {
                                LINE,
                                LINE_DOT,
                                BAR,
                                BAR_CLAMP,
                                BAR_HLC,
                                HORIZONTAL,
                                EXPAND_VERTICAL,
                                EXPAND_HORIZONTAL,
                                LAST = EXPAND_HORIZONTAL,
    };
    explicit                    ChartLine()
                                    { clear(); }
    explicit                    ChartLine(const ChartDataVector& data, std::string label = "", TYPE type = ChartLine::TYPE::LINE);
    Fl_Align                    align() const
                                    { return _align; }
    void                        clear();
    Fl_Color                    color() const
                                    { return _color; }
    const ChartDataVector&      data_vector() const
                                    { return _data; }
    void                        debug(int num, bool prices = false) const;
    bool                        is_visible() const
                                    { return _visible; }
    std::string                 label() const
                                    { return _label; }
    const Fl_Rect&              label_rect() const
                                    { return _rect; }
    ChartLine&                  set_align(Fl_Align val)
                                    { if (val == FL_ALIGN_LEFT || val == FL_ALIGN_RIGHT) _align = val; return *this; }
    ChartLine&                  set_color(Fl_Color val)
                                    { _color = val; return *this; }
    ChartLine&                  set_data(const ChartDataVector& data);
    ChartLine&                  set_label(std::string val)
                                    { _label = val; return *this; }
    ChartLine&                  set_label_rect(int x, int y, int w, int h)
                                    { _rect = Fl_Rect(x, y, w, h); return *this; }
    ChartLine&                  set_type(TYPE val)
                                    { _type = val; return *this; }
    ChartLine&                  set_type_from_string(std::string val);
    ChartLine&                  set_visible(bool val)
                                    { _visible = val; return *this; }
    ChartLine&                  set_width(unsigned width)
                                    { if (width <= ChartLine::MAX_WIDTH) _width = width; return *this; }
    size_t                      size() const
                                    { return _data.size(); }
    TYPE                        type() const
                                    { return _type; }
    bool                        type_has_hlc() const
                                    { return _type == TYPE::BAR || _type == TYPE::BAR_CLAMP || _type == ChartLine::TYPE::BAR_HLC; }
    std::string                 type_to_string() const;
    unsigned                    width() const
                                    { return _width; }
private:
    ChartDataVector             _data;
    Fl_Align                    _align;
    Fl_Color                    _color;
    Fl_Rect                     _rect;
    TYPE                        _type;
    bool                        _visible;
    double                      _max;
    double                      _min;
    std::string                 _label;
    unsigned                    _width;
};
class ChartScale {
public:
                                ChartScale();
    void                        calc(int height);
    void                        clear();
    void                        debug(const char* name) const;
    double                      diff() const
                                    { return _max - _min; }
    void                        fix_height();
    std::optional<double>       max() const;
    std::optional<double>       min() const;
    double                      pixel() const
                                    { return _pixel; }
    void                        set_max(double val)
                                    { _max = val; }
    void                        set_min(double val)
                                    { _min = val; }
    double                      tick() const
                                    { return _tick; }
private:
    double                      _max;
    double                      _min;
    double                      _pixel;
    double                      _tick;
};
class ChartArea {
public:
    static const size_t         MAX_LINES = 10;
    enum class NUM {
                                ONE,
                                TWO,
                                THREE,
                                FOUR,
                                FIVE,
                                LAST = FIVE,
    };
    enum class LABELTYPE {
                                OFF,
                                ON,
                                VISIBLE,
                                LAST = VISIBLE,
    };
    explicit                    ChartArea(ChartArea::NUM num)
                                    { _num = num; clear(); }
    bool                        add_line(const ChartLine& chart_line);
    std::optional<double>       clamp_max() const;
    std::optional<double>       clamp_min() const;
    void                        clear();
    void                        debug() const;
    void                        delete_line(size_t index);
    int                         h() const
                                    { return _h; }
    StringVector                label_array(ChartArea::LABELTYPE labeltype) const;
    ChartScale&                 left_scale()
                                    { return _left; }
    ChartLine*                  line(size_t index)
                                    { return (index < _lines.size()) ? &_lines[index] : nullptr; }
    const ChartLineVector&      lines() const
                                    { return _lines; }
    NUM                         num() const
                                    { return _num; }
    int                         percent() const
                                    { return _percent; }
    ChartScale&                 right_scale()
                                    { return _right; }
    size_t                      selected() const
                                    { return _selected; }
    ChartLine*                  selected_line();
    void                        set_max_clamp(double value = INFINITY)
                                    { _clamp_max = value; }
    void                        set_min_clamp(double value = INFINITY)
                                    { _clamp_min = value; }
    void                        set_h(int h)
                                    { _h = h; }
    void                        set_percent(int val)
                                    { _percent = val; }
    void                        set_selected(size_t val)
                                    { _selected = val; }
    void                        set_visible(size_t line, bool val);
    void                        set_w(int w)
                                    { _w = w; }
    void                        set_x(int x)
                                    { _x = x; }
    void                        set_y(int y)
                                    { _y = y; }
    size_t                      size() const
                                    { return _lines.size(); }
    int                         w() const
                                    { return _w; }
    int                         x() const
                                    { return _x; }
    int                         x2() const
                                    { return _x + _w; }
    int                         y() const
                                    { return _y; }
    int                         y2() const
                                    { return _y + _h; }
private:
    NUM                         _num;
    ChartLineVector             _lines;
    ChartScale                  _left;
    ChartScale                  _right;
    double                      _clamp_max;
    double                      _clamp_min;
    int                         _h;
    int                         _w;
    int                         _x;
    int                         _y;
    int                         _percent;
    size_t                      _selected;
};
class Chart : public Fl_Group {
public:
    static const int            MIN_AREA_SIZE =   10;
    static const int            MIN_MARGIN    =    3;
    static const int            DEF_MARGIN    =    6;
    static const int            MAX_MARGIN    =   20;
    static const int            MIN_TICK      =    3;
    static const int            MAX_TICK      = ChartLine::MAX_WIDTH * 2;
    static const int            VERSION       =    4;
    static const size_t         MAX_VLINES    = 1400;
    explicit                    Chart(int X = 0, int Y = 0, int W = 0, int H = 0, const char* l = nullptr);
    void                        block_dates(const ChartDataVector& block_dates)
                                    { _block_dates = block_dates; }
    ChartArea&                  area(ChartArea::NUM num)
                                    { return _areas[static_cast<size_t>(num)]; }
    void                        clear();
    bool                        create_line(ChartData::FORMULAS formula, bool support = false);
    Date::FORMAT                date_format() const
                                    { return _date_format; }
    void                        debug() const;
    void                        disable_menu(bool value = true)
                                    { _disable_menu = value; }
    void                        do_layout()
                                    { _old = Fl_Rect(); resize(x(), y(), w(), h()); redraw(); }
    void                        draw() override;
    int                         handle(int event) override;
    void                        init(bool calc_dates);
    bool                        load_cvs();
    bool                        load_json();
    bool                        load_json(std::string filename);
    void                        print_to_postscript();
    void                        resize(int X, int Y, int W, int H) override;
    bool                        save_cvs();
    bool                        save_json();
    bool                        save_json(std::string filename, double max_diff_high_low = 0.001) const;
    bool                        save_png();
    bool                        set_area_size(int area1 = 100, int area2 = 0, int area3 = 0, int area4 = 0, int area5 = 0);
    void                        set_date_range(ChartData::RANGE range = ChartData::RANGE::DAY);
    void                        set_label(std::string label)
                                    { _label = label; }
    bool                        set_margin(int def);
    bool                        set_tick_width(int width = Chart::MIN_TICK);
    void                        setup_add_line();
    void                        setup_area();
    void                        setup_clamp(bool min = true);
    void                        setup_date_range();
    void                        setup_delete_lines();
    void                        setup_label();
    void                        setup_line();
    void                        setup_move_lines();
    void                        setup_show_or_hide_lines();
    void                        setup_ywidth();
    void                        update_pref();
    void                        view_options(bool line_labels = true, bool hor_lines = true, bool ver_lines = true)
                                    { _view.labels = line_labels; _view.horizontal = hor_lines; _view.vertical = ver_lines; redraw(); }
private:
    void                        _calc_area_height();
    void                        _calc_area_width();
    void                        _calc_dates();
    void                        _calc_ymin_ymax();
    void                        _calc_yscale();
    void                        _calc_ywidth();
    void                        _create_tooltip(bool ctrl);
    void                        _draw_area(ChartArea& area);
    void                        _draw_label();
    void                        _draw_line(const ChartLine& line, const ChartScale& scale, int X, int Y, int W, int H);
    void                        _draw_line_labels(const ChartArea& area);
    void                        _draw_tooltip();
    void                        _draw_ver_lines(const ChartArea& area);
    void                        _draw_xlabels();
    void                        _draw_ylabels(int X, double Y1, double Y2, const ChartScale& scale, bool left);
    ChartArea*                  _inside_area(int X, int Y);
    bool                        _move_or_delete_line(ChartArea* area, size_t index, bool move, ChartArea::NUM destination = ChartArea::NUM::ONE);
    void                        _show_menu();
    static void                 _CallbackDebugChart(Fl_Widget*, void* widget);
    static void                 _CallbackDebugLine(Fl_Widget*, void* widget);
    static bool                 _CallbackPrinter(void* data, int pw, int ph, int page);
    static void                 _CallbackScrollbar(Fl_Widget*, void* widget);
    static void                 _CallbackToggle(Fl_Widget*, void* widget);
    struct {
        bool                    horizontal;
        bool                    labels;
        bool                    vertical;
    }                           _view;
    ChartArea*                  _area;
    ChartAreaVector             _areas;
    ChartData::RANGE            _date_range;
    ChartDataVector             _block_dates;
    ChartDataVector             _dates;
    Date::FORMAT                _date_format;
    Fl_Menu_Button*             _menu;
    Fl_Rect                     _old;
    Fl_Scrollbar*               _scroll;
    bool                        _disable_menu;
    bool                        _printing;
    int                         _bottom_space;
    int                         _cw;
    int                         _date_start;
    int                         _margin;
    int                         _margin_left;
    int                         _margin_right;
    int                         _tick_width;
    int                         _ticks;
    int                         _top_space;
    int                         _ver_pos[MAX_VLINES];
    std::string                 _label;
    std::string                 _tooltip;
};
}
#include <FL/Fl_Group.H>
namespace flw {
class GridGroup : public Fl_Group {
public:
    explicit                    GridGroup(int X = 0, int Y = 0, int W = 0, int H = 0, const char* l = nullptr);
                                ~GridGroup();
    void                        add(Fl_Widget* widget, int X, int Y, int W, int H);
    void                        adjust(Fl_Widget* widget, int L = 0, int R = 0, int T = 0, int B = 0);
    void                        clear();
    void                        do_layout()
                                    { resize(x(), y(), w(), h()); Fl::redraw(); }
    int                         handle(int event) override;
    Fl_Widget*                  remove(Fl_Widget* widget);
    void                        resize(int X, int Y, int W, int H) override;
    void                        resize(Fl_Widget* widget, int X, int Y, int W, int H);
    int                         size() const
                                    { return _size; }
    void                        size(int size)
                                    { _size = (size >= 4 && size <= 72) ? size : 0; }
private:
    void                        _last_active_widget(Fl_Widget** first, Fl_Widget** last);
    VoidVector                  _widgets;
    int                         _size;
};
}
#include <FL/Fl_Group.H>
namespace flw {
class ToolGroup : public Fl_Group {
public:
    enum class DIRECTION {
                                HORIZONTAL,
                                VERTICAL,
    };
    explicit                    ToolGroup(DIRECTION direction = DIRECTION::HORIZONTAL, int X = 0, int Y = 0, int W = 0, int H = 0, const char* l = nullptr);
                                ~ToolGroup();
    void                        add(Fl_Widget* widget, int SIZE = 0);
    void                        clear();
    DIRECTION                   direction() const
                                    { return _direction; }
    void                        direction(DIRECTION direction)
                                    { _direction = direction; }
    void                        do_layout()
                                    { resize(x(), y(), w(), h()); Fl::redraw(); }
    bool                        expand_last() const
                                    { return _expand; }
    void                        expand_last(bool value)
                                    { _expand = value; }
    Fl_Widget*                  remove(Fl_Widget* widget);
    void                        resize(int X, int Y, int W, int H) override;
    void                        resize(Fl_Widget* widget, int SIZE = 0);
private:
    DIRECTION                   _direction;
    VoidVector                  _widgets;
    bool                        _expand;
};
}
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Group.H>
namespace flw {
class DateChooser : public GridGroup {
public:
    explicit                    DateChooser(int X = 0, int Y = 0, int W = 0, int H = 0, const char* l = nullptr);
    void                        draw() override;
    void                        focus();
    Date                        get() const;
    int                         handle(int event) override;
    void                        set(const Date& date);
private:
    static void                 _Callback(Fl_Widget* w, void* o);
    void                        _set_label();
    Fl_Box*                     _month_label;
    Fl_Button*                  _b1;
    Fl_Button*                  _b2;
    Fl_Button*                  _b3;
    Fl_Button*                  _b4;
    Fl_Button*                  _b5;
    Fl_Button*                  _b6;
    Fl_Button*                  _b7;
    Fl_Widget*                  _canvas;
    ToolGroup*                  _buttons;
};
namespace dlg {
bool                            date(const std::string& title, Date& date, Fl_Window* parent);
}
}
#include <FL/Fl_Hold_Browser.H>
#include <FL/Fl_Menu_Button.H>
namespace flw {
class ScrollBrowser : public Fl_Hold_Browser {
public:
                                ScrollBrowser(const ScrollBrowser&) = delete;
                                ScrollBrowser(ScrollBrowser&&) = delete;
    ScrollBrowser&              operator=(const ScrollBrowser&) = delete;
    ScrollBrowser&              operator=(ScrollBrowser&&) = delete;
    explicit                    ScrollBrowser(int scroll = 9, int X = 0, int Y = 0, int W = 0, int H = 0, const char* l = nullptr);
    bool                        enable_menu() const
                                    { return _flag_menu; }
    void                        enable_menu(bool menu)
                                    { _flag_menu = menu; }
    bool                        enable_pagemove() const
                                    { return _flag_move; }
    void                        enable_pagemove(bool move)
                                    { _flag_move = move; }
    int                         handle(int event) override;
    Fl_Menu_Button*             menu()
                                    { return _menu; }
    std::string                 text2() const
                                    { return util::remove_browser_format(text(value())); }
    std::string                 text2(int line) const
                                    { return util::remove_browser_format(text(line)); }
    void                        update_pref()
                                    { update_pref(flw::PREF_FONT, flw::PREF_FONTSIZE); }
    void                        update_pref(Fl_Font text_font, Fl_Fontsize text_size);
    static void                 Callback(Fl_Widget*, void*);
private:
    Fl_Menu_Button*             _menu;
    bool                        _flag_menu;
    bool                        _flag_move;
    int                         _scroll;
};
}
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Hold_Browser.H>
#include <FL/Fl_Hor_Fill_Slider.H>
#include <FL/Fl_Toggle_Button.H>
namespace flw {
namespace dlg {
extern const char*              PASSWORD_CANCEL;
extern const char*              PASSWORD_OK;
void                            center_message_dialog();
StringVector                    check(std::string title, const StringVector& list, Fl_Window* parent = nullptr);
int                             choice(std::string title, const StringVector& list, int selected = 0, Fl_Window* parent = nullptr);
bool                            font(Fl_Font& font, Fl_Fontsize& fontsize, std::string& fontname, bool limit_to_default = false);
void                            html(std::string title, const std::string& text, Fl_Window* parent = nullptr, int W = 40, int H = 23);
void                            list(std::string title, const StringVector& list, Fl_Window* parent = nullptr, bool fixed_font = false, int W = 40, int H = 23);
void                            list(std::string title, const std::string& list, Fl_Window* parent = nullptr, bool fixed_font = false, int W = 40, int H = 23);
void                            list_file(std::string title, std::string file, Fl_Window* parent = nullptr, bool fixed_font = false, int W = 40, int H = 23);
void                            panic(std::string message);
bool                            password1(std::string title, std::string& password, Fl_Window* parent = nullptr);
bool                            password2(std::string title, std::string& password, Fl_Window* parent = nullptr);
bool                            password3(std::string title, std::string& password, std::string& file, Fl_Window* parent = nullptr);
bool                            password4(std::string title, std::string& password, std::string& file, Fl_Window* parent = nullptr);
void                            print(std::string title, PrintCallback cb, void* data = nullptr, int from = 1, int to = 0, Fl_Window* parent = nullptr);
bool                            print_text(std::string title, const std::string& text, Fl_Window* parent = nullptr);
bool                            print_text(std::string title, const StringVector& text, Fl_Window* parent = nullptr);
int                             select(std::string title, const StringVector& list, int select_row, Fl_Window* parent = nullptr, bool fixed_font = false, int W = 40, int H = 23);
int                             select(std::string title, const StringVector& list, const std::string& select_row, Fl_Window* parent = nullptr, bool fixed_font = false, int W = 40, int H = 23);
bool                            slider(std::string title, double min, double max, double& value, double step = 1.0, Fl_Window* parent = nullptr);
void                            text(std::string title, const std::string& text, Fl_Window* parent = nullptr, int W = 40, int H = 23);
bool                            text_edit(std::string title, std::string& text, Fl_Window* parent = nullptr, int W = 40, int H = 23);
void                            theme(bool enable_font = false, bool enable_fixedfont = false, Fl_Window* parent = nullptr);
class AbortDialog : public Fl_Double_Window {
    using Fl_Double_Window::show;
public:
                                AbortDialog(const AbortDialog&) = delete;
                                AbortDialog(AbortDialog&&) = delete;
    AbortDialog&                operator=(const AbortDialog&) = delete;
    AbortDialog&                operator=(AbortDialog&&) = delete;
    explicit                    AbortDialog(std::string label = "", double min = 0.0, double max = 0.0);
    bool                        check(int milliseconds = 200);
    bool                        check(double value, double min, double max, int milliseconds = 200);
    bool                        aborted()
                                    { return _abort; }
    void                        range(double min, double max);
    void                        resize(int X, int Y, int W, int H) override
                                    { Fl_Double_Window::resize(X, Y, W, H); _grid->resize(0, 0, W, H); }
    void                        show(const std::string& label, Fl_Window* parent = nullptr);
    void                        value(double value);
    static void                 Callback(Fl_Widget* w, void* o);
private:
    Fl_Button*                  _button;
    Fl_Hor_Fill_Slider*         _progress;
    GridGroup*                  _grid;
    bool                        _abort;
    int64_t                     _last;
};
class FontDialog : public Fl_Double_Window {
public:
                                FontDialog(const FontDialog&) = delete;
                                FontDialog(FontDialog&&) = delete;
    FontDialog&                 operator=(const FontDialog&) = delete;
    FontDialog&                 operator=(FontDialog&&) = delete;
                                FontDialog(Fl_Font font, Fl_Fontsize fontsize, const std::string& label, bool limit_to_default = false);
                                FontDialog(std::string font, Fl_Fontsize fontsize, std::string label, bool limit_to_default = false);
    void                        activate_font()
                                    { static_cast<Fl_Widget*>(_fonts)->activate(); }
    void                        deactivate_font()
                                    { static_cast<Fl_Widget*>(_fonts)->deactivate(); }
    void                        deactivate_fontsize()
                                    { static_cast<Fl_Widget*>(_sizes)->deactivate(); }
    int                         font()
                                    { return _font; }
    std::string                 fontname()
                                    { return _fontname; }
    int                         fontsize()
                                    { return _fontsize; }
    void                        resize(int X, int Y, int W, int H) override
                                    { Fl_Double_Window::resize(X, Y, W, H); _grid->resize(0, 0, W, H); }
    bool                        run(Fl_Window* parent = nullptr);
    static void                 Callback(Fl_Widget* w, void* o);
private:
    void                        _activate();
    void                        _create(Fl_Font font, std::string fontname, Fl_Fontsize fontsize, std::string label, bool limit_to_default);
    void                        _select_name(std::string font_name);
    Fl_Box*                     _label;
    Fl_Button*                  _cancel;
    Fl_Button*                  _select;
    GridGroup*                  _grid;
    ScrollBrowser*              _fonts;
    ScrollBrowser*              _sizes;
    bool                        _ret;
    int                         _font;
    int                         _fontsize;
    std::string                 _fontname;
};
class WorkDialog : public Fl_Double_Window {
public:
                                WorkDialog(const char* title, Fl_Window* parent, bool cancel, bool pause, int W = 40, int H = 10);
    void                        resize(int X, int Y, int W, int H) override
                                    { Fl_Double_Window::resize(X, Y, W, H); _grid->resize(0, 0, W, H); }
    bool                        run(double update_time, const StringVector& messages);
    bool                        run(double update_time, const std::string& message);
    static void                 Callback(Fl_Widget* w, void* o);
private:
    Fl_Button*                  _cancel;
    Fl_Hold_Browser*            _label;
    Fl_Toggle_Button*           _pause;
    GridGroup*                  _grid;
    bool                        _ret;
    double                      _last;
    std::string                 _message;
};
}
}
#include <FL/Fl_Group.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Menu_Button.H>
namespace flw {
class _InputMenu;
class InputMenu : public Fl_Group {
public:
                                InputMenu(const InputMenu&) = delete;
                                InputMenu(InputMenu&&) = delete;
    InputMenu&                  operator=(const InputMenu&) = delete;
    InputMenu&                  operator=(InputMenu&&) = delete;
    explicit                    InputMenu(int X = 0, int Y = 0, int W = 0, int H = 0, const char* l = nullptr);
    void                        clear();
    bool                        enable_menu() const
                                    { return _menu->visible() != 0; }
    void                        enable_menu(bool value)
                                    { if (value == true) _menu->show(); else _menu->hide(); resize(x(), y(), w(), h()); }
    StringVector                get_history() const;
    Fl_Input*                   input()
                                    { return reinterpret_cast<Fl_Input*>(_input); }
    void                        insert(std::string string, int max_list_len);
    Fl_Menu_Button*             menu()
                                    { return _menu; }
    void                        resize(int X, int Y, int W, int H) override;
    void                        update_pref(Fl_Font text_font = flw::PREF_FONT, Fl_Fontsize text_size = flw::PREF_FONTSIZE);
    const char*                 value() const;
    void                        value(const char* string);
    void                        values(const StringVector& list, bool copy_first_to_input = true);
    static void                 Callback(Fl_Widget*, void*);
private:
    void                        _add(bool insert, const std::string& string, int max_list_len);
    void                        _add(bool insert, const StringVector& list);
    _InputMenu*                 _input;
    Fl_Menu_Button*             _menu;
};
}
#include <assert.h>
#include <string.h>
#include <map>
#include <string>
#include <vector>
namespace flw {
class JS;
typedef std::map<std::string, JS*> JSObject;
typedef std::vector<JS*> JSArray;
class JS {
    friend class JSB;
public:
    static const size_t         MAX_DEPTH = 32;
    enum TYPE {
                                OBJECT,
                                ARRAY,
                                STRING,
                                NUMBER,
                                BOOL,
                                NIL,
    };
                                JS(const JS&) = delete;
                                JS(JS&&) = delete;
    JS&                         operator=(const JS&) = delete;
    JS&                         operator=(JS&&) = delete;
                                JS()
                                    { JS::COUNT++; _type = NIL; _name = strdup(""); _vb = false; _parent = nullptr; _enc_flag = 0; _pos = 0; }
                                ~JS()
                                    { JS::COUNT--; _clear(true); }
    bool                        operator==(TYPE type) const
                                    { return _type == type; }
    bool                        operator!=(TYPE type) const
                                    { return _type != type; }
    const JS*                   operator[](std::string name) const
                                    { return _get_object(name.c_str(), true); }
    const JS*                   operator[](size_t index) const
                                    { return (_type == ARRAY && index < _va->size()) ? (*_va)[index] : nullptr; }
    const JS*                   find(std::string name, bool rec = false) const;
    std::string                 decode(const char* json, size_t len, bool ignore_trailing_comma = false, bool ignore_duplicates = false, bool ignore_utf_check = false);
    std::string                 decode(std::string json, bool ignore_trailing_comma = false, bool ignore_duplicates = false, bool ignore_utf_check = false)
                                    { return decode(json.c_str(), json.length(), ignore_trailing_comma, ignore_duplicates, ignore_utf_check); }
    void                        debug() const;
    std::string                 encode(int skip = 0) const;
    const JS*                   get(std::string name, bool escape_name = true) const
                                    { return _get_object(name.c_str(), escape_name); }
    const JS*                   get(size_t index) const
                                    { return (*this) [index]; }
    bool                        is_array() const
                                    { return _type == ARRAY; }
    bool                        is_bool() const
                                    { return _type == BOOL; }
    bool                        is_null() const
                                    { return _type == NIL; }
    bool                        is_number() const
                                    { return _type == NUMBER; }
    bool                        is_object() const
                                    { return _type == OBJECT; }
    bool                        is_string() const
                                    { return _type == STRING; }
    std::string                 name() const
                                    { return _name; }
    const char*                 name_c() const
                                    { return _name; }
    std::string                 name_u() const
                                    { return JS::Unescape(_name); }
    JS*                         parent()
                                    { return _parent; }
    unsigned                    pos() const
                                    { return _pos; }
    size_t                      size() const
                                    { return (is_array() == true) ? _va->size() : (is_object() == true) ? _vo->size() : 0; }
    TYPE                        type() const
                                    { return (TYPE) _type; }
    std::string                 type_name() const
                                    { assert(_type >= 0 && _type < (int) 6); return TYPE_NAMES[(unsigned) _type]; }
    const JSArray*              va() const
                                    { return (_type == ARRAY) ? _va : nullptr; }
    bool                        vb() const
                                    { assert(_type == BOOL); return (_type == BOOL) ? _vb : false; }
    double                      vn() const
                                    { assert(_type == NUMBER); return (_type == NUMBER) ? _vn : 0.0; }
    long long int               vn_i() const
                                    { assert(_type == NUMBER); return (_type == NUMBER) ? (long long int) _vn : 0; }
    const JSObject*             vo() const
                                    { return (_type == OBJECT) ? _vo : nullptr; }
    const JSArray               vo_to_va() const;
    std::string                 vs() const
                                    { assert(_type == STRING); return (_type == STRING) ? _vs : ""; }
    const char*                 vs_c() const
                                    { assert(_type == STRING); return (_type == STRING) ? _vs : ""; }
    std::string                 vs_u() const
                                    { assert(_type == STRING); return (_type == STRING) ? JS::Unescape(_vs) : ""; }
    static inline ssize_t       Count()
                                    { return JS::COUNT; }
    static size_t               CountUtf8(const char* p);
    static std::string          Escape(const char* string);
    static std::string          Unescape(const char* string);
private:
    explicit                    JS(const char* name, JS* parent = nullptr, unsigned pos = 0)
                                    { JS::COUNT++; _type = NIL; _name = strdup((name != nullptr) ? name : ""); _parent = parent; _enc_flag = 0; _pos = pos; }
    bool                        _add_bool(char** sVal1, bool b, bool ignore_duplicates, unsigned pos);
    bool                        _add_nil(char** sVal1, bool ignore_duplicates, unsigned pos);
    bool                        _add_number(char** sVal1, double& nVal, bool ignore_duplicates, unsigned pos);
    bool                        _add_string(char** sVal1, char** sVal2, bool ignore_duplicates, unsigned pos);
    void                        _clear(bool name);
    std::string                 _encode(bool ignore_name, int skip) const;
    const JS*                   _get_object(const char* name, bool escape) const;
    bool                        _set_object(const char* name, JS* js, bool ignore_duplicates);
    static void                 _Encode(const JS* js, std::string& j, std::string& t, bool comma, int skip);
    static void                 _EncodeInline(const JS* js, std::string& j, bool comma, int skip);
    static inline JS*           _MakeArray(const char* name, JS* parent, unsigned pos)
                                    { auto r = new JS(name, parent, pos); r->_type = ARRAY; r->_va = new JSArray(); return r; }
    static inline JS*           _MakeBool(const char* name, bool vb, JS* parent, unsigned pos)
                                    { auto r = new JS(name, parent, pos); r->_type = BOOL; r->_vb = vb; return r; }
    static inline JS*           _MakeNil(const char* name, JS* parent, unsigned pos)
                                    { return new JS(name, parent, pos); }
    static inline JS*           _MakeNumber(const char* name, double vn, JS* parent, unsigned pos)
                                    { auto r = new JS(name, parent, pos); r->_type = NUMBER; r->_vn = vn; return r; }
    static inline JS*           _MakeObject(const char* name, JS* parent, unsigned pos)
                                    { auto r = new JS(name, parent, pos); r->_type = OBJECT; r->_vo = new JSObject(); return r; }
    static inline JS*           _MakeString(const char* name, const char* vs, JS* parent, unsigned pos)
                                    { auto r = new JS(name, parent, pos); r->_type = STRING; r->_vs = strdup(vs); return r; }
    static constexpr const char* TYPE_NAMES[7] = { "OBJECT", "ARRAY", "STRING", "NUMBER", "BOOL", "NIL", };
    static ssize_t              COUNT;
    char                        _type;
    char                        _enc_flag;
    unsigned                    _pos;
    JS*                         _parent;
    char*                       _name;
    union {
        JSArray*                _va;
        JSObject*               _vo;
        bool                    _vb;
        double                  _vn;
        char*                   _vs;
    };
};
class JSB {
public:
                                JSB()
                                    { _root = _current = nullptr; }
    virtual                     ~JSB()
                                    { delete _root; }
    JSB&                        operator<<(JS* json)
                                    { return add(json); }
    JSB&                        add(JS* json);
    void                        clear()
                                    { delete _root; _root = _current = nullptr; _name = ""; }
    std::string                 encode() const;
    JSB&                        end();
    const JS*                   root() const
                                    { return _root; }
    static inline JS*           MakeArray(const char* name = "", bool escape = true)
                                    { auto r = new JS((escape == true) ? JS::Escape(name).c_str() : name); r->_type = JS::ARRAY; r->_va = new JSArray(); return r; }
    static inline JS*           MakeArrayInline(const char* name = "", bool escape = true)
                                    { auto r = new JS((escape == true) ? JS::Escape(name).c_str() : name); r->_type = JS::ARRAY; r->_va = new JSArray(); r->_enc_flag = 1; return r; }
    static inline JS*           MakeBool(bool vb, const char* name = "", bool escape = true)
                                    { auto r = new JS((escape == true) ? JS::Escape(name).c_str() : name); r->_type = JS::BOOL; r->_vb = vb; return r; }
    static inline JS*           MakeNull(const char* name = "", bool escape = true)
                                    { auto r = new JS((escape == true) ? JS::Escape(name).c_str() : name); r->_type = JS::NIL; return r; }
    static inline JS*           MakeNumber(double vn, const char* name = "", bool escape = true)
                                    { auto r = new JS((escape == true) ? JS::Escape(name).c_str() : name); r->_type = JS::NUMBER; r->_vn = vn; return r; }
    static inline JS*           MakeObject(const char* name = "", bool escape = true)
                                    { auto r = new JS((escape == true) ? JS::Escape(name).c_str() : name); r->_type = JS::OBJECT; r->_vo = new JSObject(); return r; }
    static inline JS*           MakeObjectInline(const char* name = "", bool escape = true)
                                    { auto r = new JS((escape == true) ? JS::Escape(name).c_str() : name); r->_type = JS::OBJECT; r->_vo = new JSObject(); r->_enc_flag = 1; return r; }
    static inline JS*           MakeString(const char* vs, const char* name = "", bool escape = true)
                                    { auto r = new JS((escape == true) ? JS::Escape(name).c_str() : name); r->_type = JS::STRING; r->_vs = strdup((escape == true) ? JS::Escape(vs).c_str() : vs); return r; }
    static inline JS*           MakeString(std::string vs, const char* name = "", bool escape = true)
                                    { auto r = new JS((escape == true) ? JS::Escape(name).c_str() : name); r->_type = JS::STRING; r->_vs = strdup((escape == true) ? JS::Escape(vs.c_str()).c_str() : vs.c_str()); return r; }
private:
    JS*                         _current;
    JS*                         _root;
    std::string                 _name;
};
}
#include <FL/Fl_Box.H>
namespace flw {
    class LcdNumber : public Fl_Box {
    public:
        explicit                        LcdNumber(int X = 0, int Y = 0, int W = 0, int H = 0, const char* l = nullptr);
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
class LogDisplay : public Fl_Text_Display {
public:
    enum COLOR {
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
    explicit                    LogDisplay(int X = 0, int Y = 0, int W = 0, int H = 0, const char* l = nullptr);
                                ~LogDisplay();
    void                        edit_styles();
    void                        find(bool next, bool force_ask);
    int                         handle(int event) override;
    void                        lock_colors(bool lock)
                                    { _lock_colors = lock; }
    void                        save_file();
    void                        style(std::string json = "");
    void                        update_pref();
    void                        value(const char* text);
protected:
    virtual void                line_cb(size_t row, const std::string& line)
                                    { (void) row; (void) line; }
    virtual void                line_custom_cb(size_t row, const std::string& line, const std::string& word1, const std::string& word2, LogDisplay::COLOR color, bool inclusive, size_t start = 0, size_t stop = 0, size_t count = 0)
                                    { (void) row; (void) line; (void) word1; (void) word2; (void) color;  (void) inclusive;  (void) start;  (void) stop;  (void) count; }
    void                        style_between(const std::string& line, const std::string& word1, const std::string& word2, bool inclusive, LogDisplay::COLOR color);
    void                        style_line(size_t start, size_t stop, LogDisplay::COLOR c);
    void                        style_num(const std::string& line, LogDisplay::COLOR color, size_t count = 0);
    void                        style_range(const std::string& line, const std::string& word1, const std::string& word2, bool inclusive, LogDisplay::COLOR color, size_t count = 0);
    void                        style_rstring(const std::string& line, const std::string& word1, LogDisplay::COLOR color, size_t count = 0);
    void                        style_string(const std::string& line, const std::string& word1, LogDisplay::COLOR color, size_t count = 0);
private:
    const char*                 _check_text(const char* text);
    char                        _style_char(size_t pos) const
                                    { pos += _tmp->pos; return (pos < _tmp->size) ? _tmp->buf[pos] : 0; }
    struct Tmp {
        char*                   buf;
        size_t                  len;
        size_t                  pos;
        size_t                  size;
                                Tmp();
                                ~Tmp();
    };
    Fl_Text_Buffer*             _buffer;
    std::string                 _find;
    bool                        _lock_colors;
    std::string                 _json;
    Fl_Text_Buffer*             _style;
    Tmp*                        _tmp;
};
}
#include <FL/Fl_Group.H>
#include <FL/Fl_Menu_Button.H>
namespace flw {
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
    static void                 _CallbackDebug(Fl_Widget*, void* widget);
    static void                 _CallbackPrint(Fl_Widget*, void* widget);
    static bool                 _CallbackPrinter(void* data, int pw, int ph, int page);
    static void                 _CallbackReset(Fl_Widget*, void* widget);
    static void                 _CallbackSave(Fl_Widget*, void* widget);
    static void                 _CallbackToggle(Fl_Widget*, void* widget);
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
#include <FL/Fl_Menu_.H>
#include <FL/Fl_Preferences.H>
namespace flw {
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
#include <FL/Fl.H>
#include <FL/Fl_Group.H>
namespace flw {
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
        explicit                SplitGroup(int X = 0, int Y = 0, int W = 0, int H = 0, const char* l = nullptr);
        void                    add(Fl_Widget* widget, SplitGroup::CHILD child);
        Fl_Widget*              child(SplitGroup::CHILD child)
                                    { return (child == SplitGroup::CHILD::FIRST) ? _widgets[0] : _widgets[1]; }
        void                    clear();
        DIRECTION               direction() const
                                    { return _direction; }
        void                    direction(SplitGroup::DIRECTION direction);
        void                    do_layout()
                                    { SplitGroup::resize(x(), y(), w(), h()); Fl::redraw(); }
        int                     handle(int event) override;
        int                     min_pos() const
                                    { return _min; }
        void                    min_pos(int value)
                                    { _min = value; }
        void                    resize(int X, int Y, int W, int H) override;
        int                     split_pos() const
                                    { return _split_pos; }
        void                    split_pos(int split_pos)
                                    { _split_pos = split_pos; }
        void                    swap()
                                    { auto tmp = _widgets[0]; _widgets[0] = _widgets[1]; _widgets[1] = tmp; do_layout(); }
        void                    toggle(SplitGroup::CHILD child, SplitGroup::DIRECTION direction, int second_size = -1);
        void                    toggle(SplitGroup::CHILD child, int second_size = -1)
                                    { toggle(child, _direction, second_size); }
    private:
        DIRECTION               _direction;
        Fl_Widget*              _widgets[2];
        bool                    _drag;
        int                     _min;
        int                     _split_pos;
    };
}
#include <map>
#include <FL/Fl_Group.H>
#include <FL/Fl_Scrollbar.H>
namespace flw {
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
    explicit                    TableDisplay(int X = 0, int Y = 0, int W = 0, int H = 0, const char* l = nullptr);
    void                        active_cell(int row = -1, int col = -1, bool show = false);
    virtual Fl_Align            cell_align(int row, int col)
                                        { (void) row; (void) col; return FL_ALIGN_LEFT; }
    virtual Fl_Color            cell_color(int row, int col)
                                        { (void) row; (void) col; return FL_BACKGROUND2_COLOR; }
    virtual Fl_Color            cell_textcolor(int row, int col)
                                    { (void) row; (void) col; return FL_FOREGROUND_COLOR; }
    virtual Fl_Font             cell_textfont(int row, int col)
                                        { (void) row; (void) col; return flw::PREF_FONT; }
    virtual Fl_Fontsize         cell_textsize(int row, int col)
                                        { (void) row; (void) col; return flw::PREF_FONTSIZE; }
    virtual const char*         cell_value(int row, int col)
                                        { (void) row; (void) col; return ""; }
    virtual int                 cell_width(int col)
                                        { (void) col; return flw::PREF_FONTSIZE * 6; }
    virtual void                cell_width(int col, int width)
                                        { (void) col; (void) width; }
    virtual void                clear();
    int                         column() const
                                    { return _curr_col; }
    int                         columns() const
                                    { return _cols; }
    virtual void                draw() override;
    void                        expand_last_column(bool expand = false)
                                    { _expand = expand; redraw(); }
    TableDisplay::EVENT         event() const
                                    { return _event; }
    int                         event_col() const
                                    { return _event_col; }
    int                         event_row() const
                                    { return _event_row; }
    virtual int                 handle(int event) override;
    void                        header(bool row = false, bool col = false);
    int                         height() const
                                    { return _height; }
    void                        height(int height)
                                    { _height = height; }
    void                        lines(bool ver = false, bool hor = false);
    void                        resize_column_width(bool resize = false)
                                    { _resize = resize; }
    int                         row() const
                                    { return _curr_row; }
    int                         rows() const
                                    { return _rows; }
    void                        scrollbar(bool ver = true, bool hor = true)
                                    { _disable_ver = !ver; _disable_hor = !hor; redraw(); }
    void                        select_mode(TableDisplay::SELECT select = TableDisplay::SELECT::NO)
                                    { _select = select; }
    void                        show_cell(int row, int col);
    virtual void                size(int rows, int cols);
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
    int                         _cell_height(int Y = -1);
    int                         _cell_width(int col, int X = -1);
    virtual void                _draw_cell(int row, int col, int X, int Y, int W, int H, bool ver, bool hor, bool current = false);
    void                        _draw_row(int row, int W, int Y, int H);
    void                        _draw_text(const char* string, int X, int Y, int W, int H, Fl_Align align);
    int                         _ev_keyboard_down();
    int                         _ev_mouse_click();
    int                         _ev_mouse_drag();
    int                         _ev_mouse_move();
    void                        _get_cell_below_mouse(int& row, int& col);
    void                        _move_cursor(_TABLEDISPLAY_MOVE move);
    void                        _update_scrollbars();
    void                        _set_event(int row, int col, TableDisplay::EVENT event)
                                    { _event_row = row; _event_col = col; _event = event; }
    static void                 _CallbackHor(Fl_Widget* w, void* v);
    static void                 _CallbackVer(Fl_Widget* w, void* v);
    TableDisplay::EVENT         _event;
    TableDisplay::SELECT        _select;
    Fl_Scrollbar*               _hor;
    Fl_Scrollbar*               _ver;
    Fl_Widget*                  _edit;
    std::string                 _find;
    bool                        _disable_hor;
    bool                        _disable_ver;
    bool                        _drag;
    bool                        _expand;
    bool                        _resize;
    bool                        _show_col_header;
    bool                        _show_hor_lines;
    bool                        _show_row_header;
    bool                        _show_ver_lines;
    int                         _cols;
    int                         _curr_col;
    int                         _curr_row;
    int                         _current_cell[4];
    int                         _event_col;
    int                         _event_row;
    int                         _height;
    int                         _resize_col;
    int                         _rows;
    int                         _start_col;
    int                         _start_row;
};
}
namespace flw {
class TableEditor : public TableDisplay {
    using TableDisplay::cell_value;
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
    static const char*          SELECT_DATE;
    static const char*          SELECT_DIR;
    static const char*          SELECT_FILE;
    static const char*          SELECT_LIST;
    explicit                    TableEditor(int X = 0, int Y = 0, int W = 0, int H = 0, const char* l = nullptr);
    void                        send_changed_event_always(bool value)
                                    { _send_changed_event_always = value; }
    virtual StringVector        cell_choice(int row, int col)
                                    { (void) row; (void) col; return StringVector(); }
    virtual bool                cell_edit(int row, int col)
                                    { (void) row; (void) col; return false; }
    virtual FORMAT              cell_format(int row, int col)
                                    { (void) row; (void) col; return TableEditor::FORMAT::DEFAULT; }
    virtual REND                cell_rend(int row, int col)
                                    { (void) row; (void) col; return TableEditor::REND::TEXT; }
    virtual bool                cell_value(int row, int col, const char* value)
                                    { (void) row; (void) col; (void) value; return false; }
    void                        clear() override;
    virtual int                 handle(int event) override;
    static const char*          FormatSlider(double val, double min, double max, double step);
private:
    bool                        _send_changed_event_always;
    void                        _delete_current_cell();
    void                        _draw_cell(int row, int col, int X, int Y, int W, int H, bool ver, bool hor, bool current = false) override;
    void                        _edit_create();
    void                        _edit_quick(const char* key);
    void                        _edit_show();
    void                        _edit_start(const char* key = "");
    void                        _edit_stop(bool save = true);
    int                         _ev_keyboard_down2();
    int                         _ev_mouse_click2();
    int                         _ev_paste();
    Fl_Widget*                  _edit2;
    Fl_Widget*                  _edit3;
};
}
#include <string>
#include <map>
namespace flw {
    typedef std::map<std::string, std::string> StringMap;
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
#include <FL/Fl_Pack.H>
#include <FL/Fl_Scroll.H>
namespace flw {
class TabsGroup : public Fl_Group {
public:
    static const int            DEFAULT_SPACE = 2;
    enum class TABS {
                                NORTH,
                                SOUTH,
                                WEST,
                                EAST,
    };
    explicit                    TabsGroup(int X = 0, int Y = 0, int W = 0, int H = 0, const char* l = nullptr);
    void                        add(std::string label, Fl_Widget* widget, const Fl_Widget* after =  nullptr);
    void                        border(int n = 0, int s = 0, int w = 0, int e = 0)
                                    { _n = n; _s = s; _w = w; _e = e; do_layout(); }
    Fl_Widget*                  child(int num) const;
    int                         children() const
                                    { return (int) _widgets.size(); }
    void                        clear();
    void                        clear_layout()
                                    { _check = Fl_Rect(); }
    void                        debug() const;
    void                        do_layout()
                                    { clear_layout(); TabsGroup::resize(x(), y(), w(), h()); Fl::redraw(); }
    void                        draw() override;
    int                         find(const Fl_Widget* widget) const;
    int                         handle(int event) override;
    void                        hide_tabs();
    void                        insert(std::string label, Fl_Widget* widget, const Fl_Widget* before = nullptr);
    bool                        is_tabs_visible() const
                                    { return _scroll->visible(); }
    std::string                 label(Fl_Widget* widget);
    void                        label(std::string label, Fl_Widget* widget);
    Fl_Widget*                  remove(int num);
    Fl_Widget*                  remove(Fl_Widget* widget)
                                    { return TabsGroup::remove(find(widget)); }
    void                        resize(int X, int Y, int W, int H) override;
    void                        show_tabs();
    void                        sort(bool ascending = true, bool casecompare = false);
    void                        swap(int from, int to);
    TABS                        tabs() const
                                    { return _tabs; }
    void                        tabs(TABS value, int space_max_20 = TabsGroup::DEFAULT_SPACE);
    void                        update_pref(int pos = 14, Fl_Font font = flw::PREF_FONT, Fl_Fontsize fontsize = flw::PREF_FONTSIZE);
    Fl_Widget*                  value() const;
    void                        value(int num);
    void                        value(Fl_Widget* widget)
                                    { value(find(widget)); }
    static void                 Callback(Fl_Widget* sender, void* object);
    static const char*          Help();
private:
    Fl_Widget*                  _active_button();
    void                        _resize_east_west(int X, int Y, int W, int H);
    void                        _resize_north_south(int X, int Y, int W, int H);
    void                        _resize_widgets();
    Fl_Pack*                    _pack;
    Fl_Rect                     _area;
    Fl_Rect                     _check;
    Fl_Scroll*                  _scroll;
    TABS                        _tabs;
    WidgetVector                _widgets;
    bool                        _drag;
    int                         _active;
    int                         _e;
    int                         _n;
    int                         _pos;
    int                         _s;
    int                         _space;
    int                         _w;
};
}
namespace flw {
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
