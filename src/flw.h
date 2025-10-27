/**
* @file
* @brief Assorted utility stuff.
*
* @author gnuwimp@gmail.com
* @copyright Released under the GNU General Public License v3.0
*/

#ifndef FLW_H
#define FLW_H

// MKALGAM_ON

#include <string>
#include <map>
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
#define FLW_YELLOW                      { ::printf("\033[7m\033[33m%6u: %s::%s  \033[0m\n", __LINE__, __FILE__, __func__); fflush(stdout); }
#define FLW_BLUE                        { ::printf("\033[7m\033[34m%6u: %s::%s  \033[0m\n", __LINE__, __FILE__, __func__); fflush(stdout); }
#define FLW_MAGENTA                     { ::printf("\033[7m\033[35m%6u: %s::%s  \033[0m\n", __LINE__, __FILE__, __func__); fflush(stdout); }
#define FLW_CYAN                        { ::printf("\033[7m\033[36m%6u: %s::%s  \033[0m\n", __LINE__, __FILE__, __func__); fflush(stdout); }
#define FLW_RGB                         { ::printf("\033[7m\033[31m%6u: \033[32m%s::\033[34m%s  \033[0m\n", __LINE__, __FILE__, __func__); fflush(stdout); }

#define FLW_PRINT(...)                  FLW_PRINT_MACRO(__VA_ARGS__, FLW_PRINT7, FLW_PRINT6, FLW_PRINT5, FLW_PRINT4, FLW_PRINT3, FLW_PRINT2, FLW_PRINT1)(__VA_ARGS__);
#define FLW_PRINT1(A)                   { std::cout << "\033[31m" << std::setw(6) << __LINE__ << ": \033[34m" << __func__ << ":\033[0m 1=" << (A) << std::endl; fflush(stdout); }
#define FLW_PRINT2(A,B)                 { std::cout << "\033[31m" << std::setw(6) << __LINE__ << ": \033[34m" << __func__ << ":\033[0m 1=" << (A) << ",  \033[32m2=" << (B) << "\033[0m" << std::endl; fflush(stdout); }
#define FLW_PRINT3(A,B,C)               { std::cout << "\033[31m" << std::setw(6) << __LINE__ << ": \033[34m" << __func__ << ":\033[0m 1=" << (A) << ",  \033[32m2=" << (B) << "\033[0m,  3=" << (C) << "" << std::endl; fflush(stdout); }
#define FLW_PRINT4(A,B,C,D)             { std::cout << "\033[31m" << std::setw(6) << __LINE__ << ": \033[34m" << __func__ << ":\033[0m 1=" << (A) << ",  \033[32m2=" << (B) << "\033[0m,  3=" << (C) << ",  \033[32m4=" << (D) << "\033[0m" << std::endl; fflush(stdout); }
#define FLW_PRINT5(A,B,C,D,E)           { std::cout << "\033[31m" << std::setw(6) << __LINE__ << ": \033[34m" << __func__ << ":\033[0m 1=" << (A) << ",  \033[32m2=" << (B) << "\033[0m,  3=" << (C) << ",  \033[32m4=" << (D) << "\033[0m,  5=" << (E) << std::endl; fflush(stdout); }
#define FLW_PRINT6(A,B,C,D,E,F)         { std::cout << "\033[31m" << std::setw(6) << __LINE__ << ": \033[34m" << __func__ << ":\033[0m 1=" << (A) << ",  \033[32m2=" << (B) << "\033[0m,  3=" << (C) << ",  \033[32m4=" << (D) << "\033[0m,  5=" << (E) << ",  \033[32m6=" << (F) << "\033[0m " << std::endl; fflush(stdout); }
#define FLW_PRINT7(A,B,C,D,E,F,G)       { std::cout << "\033[31m" << std::setw(6) << __LINE__ << ": \033[34m" << __func__ << ":\033[0m 1=" << (A) << ",  \033[32m2=" << (B) << "\033[0m,  3=" << (C) << ",  \033[32m4=" << (D) << "\033[0m,  5=" << (E) << ",  \033[32m6=" << (F) << "\033[0m,  7=" << (G) << std::endl; fflush(stdout); }
#define FLW_PRINT_MACRO(A,B,C,D,E,F,G,N,...) N

#define FLW_PRINTV(...)                 FLW_PRINTV_MACRO(__VA_ARGS__, FLW_PRINTV7, FLW_PRINTV6, FLW_PRINTV5, FLW_PRINTV4, FLW_PRINTV3, FLW_PRINTV2, FLW_PRINTV1)(__VA_ARGS__);
#define FLW_PRINTV1(A)                  { std::cout << "\033[31m" << std::setw(6) << __LINE__ << ": \033[34m" << __func__ << ":\033[0m " #A "=" << (A) << "" << std::endl; fflush(stdout); }
#define FLW_PRINTV2(A,B)                { std::cout << "\033[31m" << std::setw(6) << __LINE__ << ": \033[34m" << __func__ << ":\033[0m " #A "=" << (A) << ",  \033[32m" #B "=" << (B) << "\033[0m" << std::endl; fflush(stdout); }
#define FLW_PRINTV3(A,B,C)              { std::cout << "\033[31m" << std::setw(6) << __LINE__ << ": \033[34m" << __func__ << ":\033[0m " #A "=" << (A) << ",  \033[32m" #B "=" << (B) << "\033[0m,  " #C "=" << (C) << "" << std::endl; fflush(stdout); }
#define FLW_PRINTV4(A,B,C,D)            { std::cout << "\033[31m" << std::setw(6) << __LINE__ << ": \033[34m" << __func__ << ":\033[0m " #A "=" << (A) << ",  \033[32m" #B "=" << (B) << "\033[0m,  " #C "=" << (C) << ",  \033[32m" #D "=" << (D) << "\033[0m" << std::endl; fflush(stdout); }
#define FLW_PRINTV5(A,B,C,D,E)          { std::cout << "\033[31m" << std::setw(6) << __LINE__ << ": \033[34m" << __func__ << ":\033[0m " #A "=" << (A) << ",  \033[32m" #B "=" << (B) << "\033[0m,  " #C "=" << (C) << ",  \033[32m" #D "=" << (D) << "\033[0m,  " #E "=" << (E) << "" << std::endl; fflush(stdout); }
#define FLW_PRINTV6(A,B,C,D,E,F)        { std::cout << "\033[31m" << std::setw(6) << __LINE__ << ": \033[34m" << __func__ << ":\033[0m " #A "=" << (A) << ",  \033[32m" #B "=" << (B) << "\033[0m,  " #C "=" << (C) << ",  \033[32m" #D "=" << (D) << "\033[0m,  " #E "=" << (E) << ",  \033[32m" #F "=" << (F) << "\033[0m" << std::endl; fflush(stdout); }
#define FLW_PRINTV7(A,B,C,D,E,F,G)      { std::cout << "\033[31m" << std::setw(6) << __LINE__ << ": \033[34m" << __func__ << ":\033[0m " #A "=" << (A) << ",  \033[32m" #B "=" << (B) << "\033[0m,  " #C "=" << (C) << ",  \033[32m" #D "=" << (D) << "\033[0m,  " #E "=" << (E) << ",  \033[32m" #F "=" << (F) << "\033[0m,  " #G "=" << (G) << "" << std::endl; fflush(stdout); }
#define FLW_PRINTV_MACRO(A,B,C,D,E,F,G,N,...) N

#define FLW_PRINTD(...)                 FLW_PRINTD_MACRO(__VA_ARGS__, FLW_PRINTD4, FLW_PRINTD3, FLW_PRINTD2, FLW_PRINTD1)(__VA_ARGS__);
#define FLW_PRINTD1(A)                  { ::printf("\033[31m%6d: \033[34m%s:\033[0m  %s = %.10f\n", __LINE__, __func__, #A, static_cast<double>(A)); fflush(stdout); }
#define FLW_PRINTD2(A,B)                { ::printf("\033[31m%6d: \033[34m%s:\033[0m  %s = %.10f,  \033[32m%s = %.10f\033[0m\n", __LINE__, __func__, #A, static_cast<double>(A), #B, static_cast<double>(B)); fflush(stdout); }
#define FLW_PRINTD3(A,B,C)              { ::printf("\033[31m%6d: \033[34m%s:\033[0m  %s = %.10f,  \033[32m%s = %.10f\033[0m,  %s = %.10f\n", __LINE__, __func__, #A, static_cast<double>(A), #B, static_cast<double>(B), #C, static_cast<double>(C)); fflush(stdout); }
#define FLW_PRINTD4(A,B,C,D)            { ::printf("\033[31m%6d: \033[34m%s:\033[0m  %s = %.10f,  \033[32m%s = %.10f\033[0m,  %s = %.10f, \033[32m %s = %.10f\033[0m\n", __LINE__, __func__, #A, static_cast<double>(A), #B, static_cast<double>(B), #C, static_cast<double>(C), #D, static_cast<double>(D)); fflush(stdout); }
#define FLW_PRINTD_MACRO(A,B,C,D,N,...) N

#define FLW_PRINTDS(...)                FLW_PRINTDS_MACRO(__VA_ARGS__, FLW_PRINTDS4, FLW_PRINTDS3, FLW_PRINTDS2, FLW_PRINTDS1)(__VA_ARGS__);
#define FLW_PRINTDS1(A)                 { ::printf("\033[31m%6d: \033[34m%s:\033[0m  %s = %s\n", __LINE__, __func__, #A, flw::util::format_double(static_cast<double>(A), 0, '\'').c_str()); fflush(stdout); }
#define FLW_PRINTDS2(A,B)               { ::printf("\033[31m%6d: \033[34m%s:\033[0m  %s = %s,  \033[32m%s = %s\033[0m\n", __LINE__, __func__, #A, flw::util::format_double(static_cast<double>(A), 0, '\'').c_str(), #B, flw::util::format_double(static_cast<double>(B), 0, '\'').c_str()); fflush(stdout); }
#define FLW_PRINTDS3(A,B,C)             { ::printf("\033[31m%6d: \033[34m%s:\033[0m  %s = %s,  \033[32m%s = %s\033[0m,  %s = %s\n", __LINE__, __func__, #A, flw::util::format_double(static_cast<double>(A), 0, '\'').c_str(), #B, flw::util::format_double(static_cast<double>(B), 0, '\'').c_str(), #C, flw::util::format_double(static_cast<double>(C), 0, '\'').c_str()); fflush(stdout); }
#define FLW_PRINTDS4(A,B,C,D)           { ::printf("\033[31m%6d: \033[34m%s:\033[0m  %s = %s,  \033[32m%s = %s\033[0m,  %s = %s,  \033[32m%s = %s\033[0m\n", __LINE__, __func__, #A, flw::util::format_double(static_cast<double>(A), 0, '\'').c_str(), #B, flw::util::format_double(static_cast<double>(B), 0, '\'').c_str(), #C, flw::util::format_double(static_cast<double>(C), 0, '\'').c_str(), #D, flw::util::format_double(static_cast<double>(D), 0, '\'').c_str()); fflush(stdout); }
#define FLW_PRINTDS_MACRO(A,B,C,D,N,...) N

#define FLW_NL                          { ::printf("\n"); fflush(stdout); }
#define FLW_ASSERT(X,Y)                 if ((X) == 0) fl_alert("assert in %s on line %d: %s", __func__, __LINE__, Y);
#define FLW_TEST(X,Y)                   flw::debug::test(X,Y,__LINE__,__func__);
#define FLW_TEST_FLOAT(X,Y,Z)           flw::debug::test(X,Y,Z,__LINE__,__func__);
#define FLW_TEST_TRUE(X)                flw::debug::test(X,__LINE__,__func__);
#else
#define FLW_LINE
#define FLW_RED
#define FLW_GREEN
#define FLW_YELLOW
#define FLW_BLUE
#define FLW_MAGENTA
#define FLW_CYAN
#define FLW_RGB
#define FLW_PRINT(...)
#define FLW_PRINTV(...)
#define FLW_PRINTD(...)
#define FLW_PRINTDS(...)
#define FLW_NL
#define FLW_ASSERT(X,Y)
#define FLW_TEST(X,Y)
#define FLW_TEST_FLOAT(X,Y,Z)
#define FLW_TEST_TRUE(X)
#endif

namespace flw {

/*
 *       __ _
 *      / _| |
 *     | |_| |_      __
 *     |  _| \ \ /\ / /
 *     | | | |\ V  V /
 *     |_| |_| \_/\_/
 *
 *
 */

typedef std::map<std::string, std::string> StringHash;                  ///< @brief Hash with string keys and string values.
typedef std::vector<std::string> StringVector;                          ///< @brief Vector with strings.
typedef std::vector<void*> VoidVector;                                  ///< @brief Vector with void pointers
typedef std::vector<Fl_Widget*> WidgetVector;                           ///< @brief Vector with widget pointers.
typedef bool (*PrintCallback)(void* data, int pw, int ph, int page);    ///< @brief A drawing callback for printing to postscript.

extern int                      PREF_FIXED_FONT;                        ///< @brief Fixed font - default FL_COURIER.
extern std::string              PREF_FIXED_FONTNAME;                    ///< @brief Fixed font name - default "FL_COURIER".
extern int                      PREF_FIXED_FONTSIZE;                    ///< @brief Fixed font size - default 14.
extern Fl_Font                  PREF_FONT;                              ///< @brief Default font - default FL_HELVETICA.
extern int                      PREF_FONTSIZE;                          ///< @brief Default font size - default 14.
extern std::string              PREF_FONTNAME;                          ///< @brief Default font name - default "FL_HELVETICA".
extern std::vector<char*>       PREF_FONTNAMES;                         ///< @brief List of font names - used internally - load with flw::theme::load_fonts().
extern double                   PREF_SCALE_VAL;                         ///< @brief Scale value.
extern bool                     PREF_SCALE_ON;                          ///< @brief Scale on or off.
extern std::string              PREF_THEME;                             ///< @brief Name of theme - default "default".
extern const StringVector       PREF_THEMES;                            ///< @brief Name of themes.

/*
 *                _
 *               | |
 *       ___ ___ | | ___  _ __
 *      / __/ _ \| |/ _ \| '__|
 *     | (_| (_) | | (_) | |
 *      \___\___/|_|\___/|_|
 *
 *
 */

/** @brief Color values.
    Fl_Color BEIGE\n
    Fl_Color CHOCOLATE\n
    Fl_Color CRIMSON\n
    Fl_Color DARKOLIVEGREEN\n
    Fl_Color DODGERBLUE\n
    Fl_Color FORESTGREEN\n
    Fl_Color GOLD\n
    Fl_Color GRAY\n
    Fl_Color INDIGO\n
    Fl_Color OLIVE\n
    Fl_Color PINK\n
    Fl_Color ROYALBLUE\n
    Fl_Color SIENNA\n
    Fl_Color SILVER\n
    Fl_Color SLATEGRAY\n
    Fl_Color TEAL\n
    Fl_Color TURQUOISE\n
    Fl_Color VIOLET\n
*/
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
} // flw::color

/*
 *          _      _
 *         | |    | |
 *       __| | ___| |__  _   _  __ _
 *      / _` |/ _ \ '_ \| | | |/ _` |
 *     | (_| |  __/ |_) | |_| | (_| |
 *      \__,_|\___|_.__/ \__,_|\__, |
 *                              __/ |
 *                             |___/
 */

/** @brief Debug functions.
*/
namespace debug {
    void                        print(const Fl_Widget* widget, bool recursive = true);
    void                        print(const Fl_Widget* widget, std::string& indent, bool recursive = true);
    bool                        test(bool val, int line, const char* func);
    bool                        test(const char* ref, const char* val, int line, const char* func);
    bool                        test(int64_t ref, int64_t val, int line, const char* func);
    bool                        test(double ref, double val, double diff, int line, const char* func);
} // flw::debug

/*
 *      _       _          _
 *     | |     | |        | |
 *     | | __ _| |__   ___| |
 *     | |/ _` | '_ \ / _ \ |
 *     | | (_| | |_) |  __/ |
 *     |_|\__,_|_.__/ \___|_|
 *
 *
 */

/** @brief Some common label strings.
*/
namespace label {
    extern std::string          BROWSE;
    extern std::string          CANCEL;
    extern std::string          CLOSE;
    extern std::string          DEL;
    extern std::string          DISCARD;
    extern std::string          MONO;
    extern std::string          NO;
    extern std::string          OK;
    extern std::string          PRINT;
    extern std::string          REGULAR;
    extern std::string          SAVE;
    extern std::string          SAVE_DOT;
    extern std::string          SELECT;
    extern std::string          UPDATE;
    extern std::string          YES;

} // flw::label

/*
 *
 *
 *      _ __ ___   ___ _ __  _   _
 *     | '_ ` _ \ / _ \ '_ \| | | |
 *     | | | | | |  __/ | | | |_| |
 *     |_| |_| |_|\___|_| |_|\__,_|
 *
 *
 */

/** @brief Menu item functions.
*/
namespace menu {
    void                        enable_item(Fl_Menu_* menu, const char* text, bool value);
    Fl_Menu_Item*               get_item(Fl_Menu_* menu, const char* text);
    Fl_Menu_Item*               get_item(Fl_Menu_* menu, void* v);
    bool                        item_value(Fl_Menu_* menu, const char* text);
    void                        set_item(Fl_Menu_* menu, const char* text, bool value);
    void                        setonly_item(Fl_Menu_* menu, const char* text);
} // flw::debug

/*
 *            _   _ _
 *           | | (_) |
 *      _   _| |_ _| |
 *     | | | | __| | |
 *     | |_| | |_| | |
 *      \__,_|\__|_|_|
 *
 *
 */

/** @brief Utility functions.
*/
namespace util {
    void                        center_window(Fl_Window* window, Fl_Window* parent = nullptr);
    double                      clock();
    int                         count_decimals(double number);
    Fl_Widget*                  find_widget(Fl_Group* group, const std::string& label);
    std::string                 fix_menu_string(const std::string& label);
    std::string                 format(const std::string& format, ...);
    std::string                 format_double(double num, int decimals = 0, char del = ' ');
    std::string                 format_int(int64_t num, char del = ' ');
    bool                        icon(Fl_Widget* widget, const std::string& svg_image, unsigned max_size);
    bool                        is_empty(const std::string& string);
    void                        labelfont(Fl_Widget* widget, Fl_Font font = flw::PREF_FONT, int size = flw::PREF_FONTSIZE);
    int64_t                     microseconds();
    int32_t                     milliseconds();
    bool                        png_save(Fl_Window* window, const std::string& opt_name = "", int X = 0, int Y = 0, int W = 0, int H = 0);
    std::string                 print(const std::string& ps_filename, Fl_Paged_Device::Page_Format format, Fl_Paged_Device::Page_Layout layout, PrintCallback cb, void* data);
    std::string                 print(const std::string& ps_filename, Fl_Paged_Device::Page_Format format, Fl_Paged_Device::Page_Layout layout, PrintCallback cb, void* data, int from, int to);
    std::string                 remove_browser_format(const std::string& text);
    std::string&                replace_string(std::string& string, const std::string& find, const std::string& replace);
    void                        sleep(unsigned milli);
    StringVector                split_string(const std::string& string, const std::string& split);
    std::string                 substr(const std::string& string, std::string::size_type pos, std::string::size_type size = std::string::npos);
    void                        swap_rect(Fl_Widget* w1, Fl_Widget* w2);
    double                      to_double(const std::string& string);
    size_t                      to_doubles(const std::string& string, double numbers[], size_t size);
    int64_t                     to_int(const std::string& string, int64_t def = 0);
    long long                   to_long(const std::string& string, long long def = 0);
    static inline std::string   to_string(const char* text)
                                    { return text != nullptr ? text : ""; }
    std::string                 trim(const std::string& string);
    void*                       zero_memory(char* mem, size_t size);

/*
 *      _____      _       _ _______        _
 *     |  __ \    (_)     | |__   __|      | |
 *     | |__) | __ _ _ __ | |_ | | _____  _| |_
 *     |  ___/ '__| | '_ \| __|| |/ _ \ \/ / __|
 *     | |   | |  | | | | | |_ | |  __/>  <| |_
 *     |_|   |_|  |_|_| |_|\__||_|\___/_/\_\\__|
 *
 *
 */

/** @brief Printing text to postscript file.
*
* @snippet dialog.cpp flw::PrintText example
* @image html print_text.png
*/
class PrintText {
public:
                                PrintText(const std::string& filename,
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
                                    { return _fontsize; } ///< @brief Return font size
    int                         page_count() const
                                    { return _page_count; } ///< @brief Return page count.
    std::string                 print(const char* text, unsigned replace_tab_with_space = 0);
    std::string                 print(const std::string& text, unsigned replace_tab_with_space = 0);
    std::string                 print(const StringVector& lines, unsigned replace_tab_with_space = 0);

private:
    void                        _check_for_new_page();
    void                        _measure_lw_lh(const std::string& text);
    void                        _print_line(const std::string& line);
    void                        _print_wrapped_line(const std::string& line);
    std::string                 _start();
    std::string                 _stop();

    Fl_Align                    _align;             // Text align.
    Fl_Font                     _font;              // Text font.
    Fl_Fontsize                 _fontsize;          // Text size.
    Fl_PostScript_File_Device*  _printer;           // Printer device.
    Fl_Paged_Device::Page_Format _page_format;      // Page format.
    Fl_Paged_Device::Page_Layout _page_layout;      // Page layout.
    FILE*                       _file;              // Postscript file handle.
    bool                        _border;            // Border property.
    bool                        _wrap;              // Wrap property.
    int                         _lh;                // Current line height.
    int                         _line_count;        // Line counter.
    int                         _line_num;          // Line number width.
    int                         _lw;                // Current line width.
    int                         _nw;                // Line width if line number is used.
    int                         _page_count;        // Page counter.
    int                         _ph;                // Page height.
    int                         _pw;                // Page width.
    int                         _px;                // Page x pos.
    int                         _py;                // Page y pos.
    std::string                 _filename;          // Postscript filename.
};

} // flw::util

/*
 *      _   _
 *     | | | |
 *     | |_| |__   ___ _ __ ___   ___
 *     | __| '_ \ / _ \ '_ ` _ \ / _ \
 *     | |_| | | |  __/ | | | | |  __/
 *      \__|_| |_|\___|_| |_| |_|\___|
 *
 *
 */

/** @brief Load different themes and save/load window preferences.
*
* Load theme before loading windows preferences.\n
* Scaling settings are not optimal and works different depending on which desktop are being used.\n
*/
namespace theme {
    bool                        is_dark();
    bool                        load(const std::string& name);
    Fl_Font                     load_font(const std::string& requested_font);
    void                        load_fonts(bool iso8859_only = true);
    void                        load_icon(Fl_Window* win, int win_resource, const char** xpm_resource = nullptr, const char* name = nullptr);
    Fl_Rect                     load_rect_from_pref(Fl_Preferences& pref, const std::string& basename);
    double                      load_theme_from_pref(Fl_Preferences& pref, unsigned screen_num = 0);
    void                        load_win_from_pref(Fl_Preferences& pref, const std::string& basename, Fl_Window* window, bool show = true, int defw = 800, int defh = 600);
    bool                        parse(int argc, const char** argv);
    void                        save_rect_to_pref(Fl_Preferences& pref, const std::string& basename, const Fl_Rect& rect);
    void                        save_theme_to_pref(Fl_Preferences& pref);
    void                        save_win_to_pref(Fl_Preferences& pref, const std::string& basename, Fl_Window* window);
} // flw::theme

namespace priv {

/* @brief Theme values, internal usage only.
* @private
*/
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

} // flw::priv

} // flw

// MKALGAM_OFF

#endif
