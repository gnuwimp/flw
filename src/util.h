// Copyright 2016 - 2021 gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#ifndef FLW_UTIL_H
#define FLW_UTIL_H

#include <string>
#include <vector>

class Fl_Group;
class Fl_Widget;
class Fl_Window;

// MKALGAM_ON

#define FLW_LINE printf("%5d: %s - %s\n", __LINE__, __func__, __FILE__); fflush(stdout);

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
        std::string                     fix_menu_string(const std::string& in);
        std::string                     format(const char* format, ...);
        std::string                     format_int(int64_t number, char sep = ' ');
        void                            labelfont(Fl_Widget* widget);
        void                            print(Fl_Widget* widget, bool tab = false);
        void                            print(Fl_Group* group);
        int                             replace(std::string& string, const std::string& find, const std::string& replace);
        StringVector                    split(const std::string& string, const std::string& split);
        const char*                     str_copy(char* dest, const char* src, int dest_size);
        bool                            test(bool val, int line, const char* func);
        bool                            test(const char* ref, const char* val, int line, const char* func);
        bool                            test(int64_t ref, int64_t val, int line, const char* func);
        bool                            test(double ref, double val, double diff, int line, const char* func);
        double                          time();
        int64_t                         time_micro();
        int64_t                         time_milli();
        void                            time_sleep(int milli);
        double                          to_double(const char* buffer, double def = 0.0);
        long double                     to_double_l(const char* buffer, long double def = 0.0);
        int                             to_doubles(const char* val, double* num1, double* num2 = nullptr, double* num3 = nullptr, double* num4 = nullptr, double* num5 = nullptr);
        int64_t                         to_int(const char* buffer, int64_t def = 0);
        void*                           zero_memory(char* string);
        void*                           zero_memory(void* mem, size_t size);
        void*                           zero_memory(std::string& string);
    }
}

// MKALGAM_OFF

#endif
