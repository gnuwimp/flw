// Copyright gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#ifndef FLW_H
#define FLW_H

// MKALGAM_ON

#include <string>
#include <vector>
#include <FL/Fl_Group.H>
#include <FL/Fl_Menu_.H>
#include <FL/Fl_Preferences.H>
#include <FL/Fl_PostScript.H>

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

extern int                      PREF_FIXED_FONT;        // Fixed font - FL_COURIER
extern std::string              PREF_FIXED_FONTNAME;    // Fixed font name - "FL_COURIER"
extern int                      PREF_FIXED_FONTSIZE;    // Fixed font size - default 14
extern Fl_Font                  PREF_FONT;              // Default font - FL_HELVETICA
extern int                      PREF_FONTSIZE;          // Default font size - 14
extern std::string              PREF_FONTNAME;          // Default font name - "FL_HELVETICA"
extern std::vector<char*>       PREF_FONTNAMES;         // List of font names - used internally - load with theme::load_fonts()
extern std::string              PREF_THEME;             // Name of theme - default "default"
extern const char* const        PREF_THEMES[];          // Name of themes

typedef std::vector<std::string> StringVector;
typedef std::vector<Fl_Widget*>  WidgetVector;
typedef bool (*PrintCallback)(void* data, int pw, int ph, int page);

//------------------------------------------------------------------------------
namespace debug {
    void                        print(Fl_Widget* widget);
    void                        print(Fl_Widget* widget, std::string& indent);
}

//------------------------------------------------------------------------------
// Menu item functions
//
namespace menu {
    void                        enable_item(Fl_Menu_* menu, const char* text, bool value);
    Fl_Menu_Item*               get_item(Fl_Menu_* menu, const char* text);
    bool                        item_value(Fl_Menu_* menu, const char* text);
    void                        set_item(Fl_Menu_* menu, const char* text, bool value);
    void                        setonly_item(Fl_Menu_* menu, const char* text);
}

//------------------------------------------------------------------------------
// Assorted utility functions
//
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
    void                        png_save(std::string opt_name, Fl_Window* window, int X = 0, int Y = 0, int W = 0, int H = 0);
    std::string                 print(std::string ps_filename, Fl_Paged_Device::Page_Format format, Fl_Paged_Device::Page_Layout layout, PrintCallback cb, void* data);
    std::string                 print(std::string ps_filename, Fl_Paged_Device::Page_Format format, Fl_Paged_Device::Page_Layout layout, PrintCallback cb, void* data, int from, int to);
    std::string                 remove_browser_format(const char* text);
    std::string&                replace_string(std::string& string, std::string find, std::string replace);
    void                        sleep(int milli);
    StringVector                split_string(const std::string& string, std::string split);
    void*                       zero_memory(char* mem, size_t size);
}

//------------------------------------------------------------------------------
// Load different themes and save/load window preferences
//
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

    //--------------------------------------------------------------------------
    // Internal usage only
    //
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

//------------------------------------------------------------------------------
// Drawing colors
//
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

/***
 *      ____         __ 
 *     |  _ \       / _|
 *     | |_) |_   _| |_ 
 *     |  _ <| | | |  _|
 *     | |_) | |_| | |  
 *     |____/ \__,_|_|  
 *                      
 *                      
 */

//------------------------------------------------------------------------------
// Buffer container that frees memory automatically.
// If grab is true then it will take ownership of input buffer and will delete it later.
//
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

/***
 *      ______ _ _      
 *     |  ____(_) |     
 *     | |__   _| | ___ 
 *     |  __| | | |/ _ \
 *     | |    | | |  __/
 *     |_|    |_|_|\___|
 *                      
 *                      
 */

//------------------------------------------------------------------------------
// Retrieve basic file info.
// And some utility functions to read and save to files.
//
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

/***
 *      _____      _       _ _______        _   
 *     |  __ \    (_)     | |__   __|      | |  
 *     | |__) | __ _ _ __ | |_ | | _____  _| |_ 
 *     |  ___/ '__| | '_ \| __|| |/ _ \ \/ / __|
 *     | |   | |  | | | | | |_ | |  __/>  <| |_ 
 *     |_|   |_|  |_|_| |_|\__||_|\___/_/\_\\__|
 *                                              
 *                                              
 */

//------------------------------------------------------------------------------
// Line numbers are only used with FL_ALIGN_LEFT.
//
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

} // flw

// MKALGAM_OFF

#endif
