/**
* @file
* @brief Assorted utility stuff and some global values.
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
#include <FL/Fl_Menu_.H>

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

/** @brief FLW is an widget library for the FLTK gui library.
*
*/
namespace flw {

typedef std::map<std::string, std::string> StringHash;                  ///< @brief Hash with string keys and string values.
typedef std::vector<std::string> StringVector;                          ///< @brief Vector with strings.
typedef std::vector<void*> VoidVector;                                  ///< @brief Vector with void pointers
typedef std::vector<Fl_Widget*> WidgetVector;                           ///< @brief Vector with widget pointers.

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
extern Fl_Window*               PREF_WINDOW;                            ///< @brief Used for things like the parent to dialogs for centering dialogs on parent.

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
 *      _
 *     (_)
 *      _  ___ ___  _ __  ___
 *     | |/ __/ _ \| '_ \/ __|
 *     | | (_| (_) | | | \__ \
 *     |_|\___\___/|_| |_|___/
 *
 *
 */

/** @brief Some predefined svg icons.
*
* Use it with flw::util::icon() function.\n
* Or flw::SVGButton class.\n
*
* @image html icons.png
*/
namespace icons {
    extern std::string          ALERT;          ///< @brief Alert SVG icon.
    extern std::string          BACK;           ///< @brief Back SVG icon.
    extern std::string          CANCEL;         ///< @brief Cancel SVG icon.
    extern std::string          CONFIRM;        ///< @brief Confirm SVG icon.
    extern std::string          DEL;            ///< @brief Deletge SVG icon.
    extern std::string          DOWN;           ///< @brief Down SVG icon.
    extern std::string          EDIT;           ///< @brief Edit SVG icon.
    extern std::string          ERR;            ///< @brief Error SVG icon.
    extern std::string          FORWARD;        ///< @brief Forward SVG icon.
    extern std::string          INFO;           ///< @brief Info SVG icon.
    extern std::string          LEFT;           ///< @brief Left SVG icon.
    extern std::string          PASSWORD;       ///< @brief Password SVG icon.
    extern std::string          PAUSE;          ///< @brief Pause SVG icon.
    extern std::string          PLAY;           ///< @brief Play SVG icon.
    extern std::string          QUESTION;       ///< @brief Question SVG icon.
    extern std::string          RIGHT;          ///< @brief Right SVG icon.
    extern std::string          STOP;           ///< @brief Stop SVG icon.
    extern std::string          UP;             ///< @brief Up SVG icon.
    extern std::string          WARNING;        ///< @brief Warning SVG icon.
} // icons

/*
 *      _       _          _
 *     | |     | |        | |
 *     | | __ _| |__   ___| |___
 *     | |/ _` | '_ \ / _ \ / __|
 *     | | (_| | |_) |  __/ \__ \
 *     |_|\__,_|_.__/ \___|_|___/
 *
 *
 */

/** @brief Some common label strings.
*/
namespace labels {
    extern std::string          BROWSE;
    extern std::string          CANCEL;
    extern std::string          CLOSE;
    extern std::string          DEL;
    extern std::string          DISCARD;
    extern std::string          MONO;
    extern std::string          NEXT;
    extern std::string          NO;
    extern std::string          OK;
    extern std::string          PREV;
    extern std::string          PRINT;
    extern std::string          REGULAR;
    extern std::string          SAVE;
    extern std::string          SAVE_DOT;
    extern std::string          SELECT;
    extern std::string          UPDATE;
    extern std::string          YES;

} // flw::labels

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
    std::string                 format(const char* format, ...);
    std::string                 format_double(double num, int decimals = 0, char del = ' ');
    std::string                 format_int(int64_t num, char del = ' ');
    bool                        icon(Fl_Widget* widget, const std::string& svg_image, unsigned size);
    bool                        is_empty(const std::string& string);
    void                        labelfont(Fl_Widget* widget, Fl_Font font = flw::PREF_FONT, int size = flw::PREF_FONTSIZE);
    int64_t                     microseconds();
    int32_t                     milliseconds();
    bool                        png_save(Fl_Window* window, const std::string& opt_name = "", int X = 0, int Y = 0, int W = 0, int H = 0);
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
                                    { return text != nullptr ? text : ""; } ///< @brief Return a std::string of c string. @param[in] text  C pointer, can be NULL.
    std::string                 trim(const std::string& string);
    void*                       zero_memory(char* mem, size_t size);
} // flw::util
} // flw

// MKALGAM_OFF

#endif
