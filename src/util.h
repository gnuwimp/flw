// Copyright 2016 - 2022 gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#ifndef FLW_UTIL_H
#define FLW_UTIL_H

#include <assert.h>
#include <string>
#include <vector>
#include <iostream>
#include <string.h>

class Fl_Group;
class Fl_Widget;
class Fl_Window;
class Fl_Menu_;
class Fl_Menu_Item;

// MKALGAM_ON

#define _FLW_TOSTRING(X)            #X
#define FLW_TOSTRING(X)             _FLW_TOSTRING(X)
#define FLW_LINE                    { fprintf(stderr, "%5d: %s - %s\n", __LINE__, __func__, __FILE__); fflush(stderr); }
#define FLW_RED                     { fprintf(stderr, "\033[7m\033[31m" "%s: %s: " FLW_TOSTRING(__LINE__) " \033[0m\n", __FILE__, __func__); fflush(stderr); }
#define FLW_GREEN                   { fprintf(stderr, "\033[7m\033[32m" "%s: %s: " FLW_TOSTRING(__LINE__) " \033[0m\n", __FILE__, __func__); fflush(stderr); }
#define FLW_YELLOW                  { fprintf(stderr, "\033[7m\033[33m" "%s: %s: " FLW_TOSTRING(__LINE__) " \033[0m\n", __FILE__, __func__); fflush(stderr); }
#define FLW_BLUE                    { fprintf(stderr, "\033[7m\033[34m" "%s: %s: " FLW_TOSTRING(__LINE__) " \033[0m\n", __FILE__, __func__); fflush(stderr); }
#define FLW_MAGENTA                 { fprintf(stderr, "\033[7m\033[35m" "%s: %s: " FLW_TOSTRING(__LINE__) " \033[0m\n", __FILE__, __func__); fflush(stderr); }
#define FLW_CYAN                    { fprintf(stderr, "\033[7m\033[36m" "%s: %s: " FLW_TOSTRING(__LINE__) " \033[0m\n", __FILE__, __func__); fflush(stderr); }
#define FLW_WHITE                   { fprintf(stderr, "\033[7m\033[37m" "%s: %s: " FLW_TOSTRING(__LINE__) " \033[0m\n", __FILE__, __func__); fflush(stderr); }
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

    //--------------------------------------------------------------------------
    struct Buf {
        char*                           p;
        size_t                          s;

                                        Buf()
                                            { p = nullptr; s = 0; }
                                        Buf(size_t size);
                                        Buf(const char* buffer, size_t size);
                                        Buf(const Buf&);
                                        Buf(Buf&& other);
        virtual                         ~Buf()
                                            { free(p); }
        Buf&                            operator=(const Buf&);
        Buf&                            operator=(Buf&& other);
        bool                            operator==(const Buf& other) const
                                            { return p != nullptr && s == other.s && memcmp(p, other.p, s) == 0; }
        uint8_t&                        operator[](size_t index)
                                            { assert(index < s); return (uint8_t&) *(p + index); }
        const char*                     c_str() const
                                            { return p; }
        void                            clear()
                                            { free(p); p = nullptr; s = 0; }
    };

    namespace util {
        //----------------------------------------------------------------------
        // Collection of utility functions
        //
        char*                           allocate(size_t size, bool terminate = true);
        void                            center_window(Fl_Window* window, Fl_Window* parent = nullptr);
        int                             count_decimals(double number);
        std::string                     fix_menu_string(std::string in);
        std::string                     format(const char* format, ...);
        std::string                     format_double(double number, int decimals, char sep = ' ');
        std::string                     format_int(int64_t number, char sep = ' ');
        Buf                             load_file(std::string filename, bool alert = true);
        void                            menu_item_enable(Fl_Menu_* menu, const char* text, bool value);
        Fl_Menu_Item*                   menu_item_get(Fl_Menu_* menu, const char* text);
        void                            menu_item_set(Fl_Menu_* menu, const char* text, bool value);
        void                            menu_item_set_only(Fl_Menu_* menu, const char* text);
        bool                            menu_item_value(Fl_Menu_* menu, const char* text);
        void                            png_save(std::string opt_name, Fl_Window* window, int X = 0, int Y = 0, int W = 0, int H = 0);
        void                            print(Fl_Widget* widget, bool tab = false);
        void                            print(Fl_Group* group);
        int                             replace(std::string& string, std::string find, std::string replace);
        bool                            save_file(std::string filename, const void* data, size_t size, bool alert = true);
        StringVector                    split(const std::string& string, std::string split);
        bool                            test(bool val, int line, const char* func);
        bool                            test(const char* ref, const char* val, int line, const char* func);
        bool                            test(int64_t ref, int64_t val, int line, const char* func);
        bool                            test(double ref, double val, double diff, int line, const char* func);
        double                          time();
        int64_t                         time_micro();
        int32_t                         time_milli();
        void                            time_sleep(int milli);
        double                          to_double(std::string, double def = 0.0);
        int                             to_doubles(std::string string, double numbers[], size_t size);
        int64_t                         to_int(std::string string, int64_t def = 0);
        int                             to_ints(std::string string, int64_t numbers[], size_t size);
        char*                           win_to_unix(const char* string);
        void*                           zero_memory(char* string);
        void*                           zero_memory(void* Buf, size_t size);
        void*                           zero_memory(std::string& string);
    }

    //--------------------------------------------------------------------------
    // Mode is 0 for missing, 1 for directory, 2 for file and 3 for other
    //
    struct Stat {
        int64_t                         size;
        int64_t                         mtime;
        int                             mode;

                                        Stat()
                                            { size = mtime = 0; mode = 0; }
                                        Stat(std::string filename);
    };
}

// MKALGAM_OFF

#endif
