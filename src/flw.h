// Copyright gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#ifndef FLW_H
#define FLW_H

// MKALGAM_ON

#include <string>
#include <vector>
#include <FL/Fl_Menu_.H>
#include <FL/Fl_Preferences.H>

#ifdef DEBUG
#include <iostream>
#include <iomanip>
#define FLW_LINE                        { printf("\033[31m%6u: \033[34m%s::%s\033[0m\n", __LINE__, __FILE__, __func__); fflush(stdout); }
#define FLW_RED                         { printf("\033[7m\033[31m%6u: %s::%s  \033[0m\n", __LINE__, __FILE__, __func__); fflush(stdout); }
#define FLW_GREEN                       { printf("\033[7m\033[32m%6u: %s::%s  \033[0m\n", __LINE__, __FILE__, __func__); fflush(stdout); }
#define FLW_BLUE                        { printf("\033[7m\033[34m%6u: %s::%s  \033[0m\n", __LINE__, __FILE__, __func__); fflush(stdout); }
#define FLW_PRINT(...)                  FLW_PRINT_MACRO(__VA_ARGS__, FLW_PRINT7, FLW_PRINT6, FLW_PRINT5, FLW_PRINT4, FLW_PRINT3, FLW_PRINT2, FLW_PRINT1)(__VA_ARGS__);
#define FLW_PRINT1(A)                   { std::cout << "\033[31m" << std::setw(6) << __LINE__ << ": \033[34m" << __func__ << "\033[0m: " #A "=" << (A) << std::endl; fflush(stdout); }
#define FLW_PRINT2(A,B)                 { std::cout << "\033[31m" << std::setw(6) << __LINE__ << ": \033[34m" << __func__ << "\033[0m: " #A "=" << (A) << ", " #B "=" << (B) << std::endl; fflush(stdout); }
#define FLW_PRINT3(A,B,C)               { std::cout << "\033[31m" << std::setw(6) << __LINE__ << ": \033[34m" << __func__ << "\033[0m: " #A "=" << (A) << ", " #B "=" << (B) << ", " #C "=" << (C) << std::endl; fflush(stdout); }
#define FLW_PRINT4(A,B,C,D)             { std::cout << "\033[31m" << std::setw(6) << __LINE__ << ": \033[34m" << __func__ << "\033[0m: " #A "=" << (A) << ", " #B "=" << (B) << ", " #C "=" << (C) << ", " #D "=" << (D) << std::endl; fflush(stdout); }
#define FLW_PRINT5(A,B,C,D,E)           { std::cout << "\033[31m" << std::setw(6) << __LINE__ << ": \033[34m" << __func__ << "\033[0m: " #A "=" << (A) << ", " #B "=" << (B) << ", " #C "=" << (C) << ", " #D "=" << (D) << ", " #E "=" << (E) << std::endl; fflush(stdout); }
#define FLW_PRINT6(A,B,C,D,E,F)         { std::cout << "\033[31m" << std::setw(6) << __LINE__ << ": \033[34m" << __func__ << "\033[0m: " #A "=" << (A) << ", " #B "=" << (B) << ", " #C "=" << (C) << ", " #D "=" << (D) << ", " #E "=" << (E) << ", " #F "=" << (F) << std::endl; fflush(stdout); }
#define FLW_PRINT7(A,B,C,D,E,F,G)       { std::cout << "\033[31m" << std::setw(6) << __LINE__ << ": \033[34m" << __func__ << "\033[0m: " #A "=" << (A) << ", " #B "=" << (B) << ", " #C "=" << (C) << ", " #D "=" << (D) << ", " #E "=" << (E) << ", " #F "=" << (F) << ", " #G "=" << (G) << std::endl; fflush(stdout); }
#define FLW_PRINT_MACRO(A,B,C,D,E,F,G,N,...) N
#else
#define FLW_LINE
#define FLW_RED
#define FLW_GREEN
#define FLW_BLUE
#define FLW_PRINT(...)
#endif

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
    typedef std::vector<Fl_Widget*>     WidgetVector;

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
        virtual                         ~Buf()
                                            { free(p); }
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

// MKALGAM_OFF

#endif
