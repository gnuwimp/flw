// Copyright gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#ifndef FLW_UTIL_H
#define FLW_UTIL_H

// MKALGAM_ON

#include <string>
#include <vector>
#include <FL/Fl_Menu_.H>

namespace flw {
    extern int                          PREF_FIXED_FONT;
    extern std::string                  PREF_FIXED_FONTNAME;
    extern int                          PREF_FIXED_FONTSIZE;
    extern int                          PREF_FONT;
    extern int                          PREF_FONTSIZE;
    extern std::string                  PREF_FONTNAME;

    typedef std::vector<std::string>    StringVector;

    struct Buf {
        char*                           p;
        size_t                          s;

                                        Buf();
                                        Buf(size_t s);
                                        Buf(char* p, size_t s);
                                        Buf(const char* p, size_t s);
                                        Buf(const Buf& b);
                                        Buf(Buf&& b);
        Buf&                            operator=(const Buf& b);
        Buf&                            operator=(Buf&& b);
        Buf&                            operator+=(const Buf& b);
        bool                            operator==(const Buf& other) const;
        virtual                         ~Buf();
    };

    namespace debug {
        void                            print(Fl_Widget* widget, bool tab = false);
        void                            print(Fl_Group* group);
    }

    namespace menu {
        void                            enable_item(Fl_Menu_* menu, const char* text, bool value);
        Fl_Menu_Item*                   get_item(Fl_Menu_* menu, const char* text);
        bool                            item_value(Fl_Menu_* menu, const char* text);
        void                            set_item(Fl_Menu_* menu, const char* text, bool value);
        void                            setonly_item(Fl_Menu_* menu, const char* text);
    }

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
        std::string&                    replace(std::string& string, std::string find, std::string replace);
        bool                            save_file(std::string filename, const void* data, size_t size, bool alert = true);
        void                            sleep(int milli);
        StringVector                    split(const std::string& string, std::string split);
    }
}

// MKALGAM_OFF

#endif
