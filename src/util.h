// Copyright 2016 - 2021 gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#ifndef FLW_UTIL_H
#define FLW_UTIL_H

#include <assert.h>
#include <string>
#include <vector>
#include <iostream>

class Fl_Group;
class Fl_Widget;
class Fl_Window;

// MKALGAM_ON

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

// MKALGAM_OFF

#endif
