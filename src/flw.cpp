// Copyright gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#include "flw.h"
#include "waitcursor.h"

// MKALGAM_ON

#include <algorithm>
#include <cstdint>
#include <assert.h>
#include <math.h>
#include <stdarg.h>
#include <time.h>
#include <FL/Fl_Window.H>
#include <FL/Fl_File_Chooser.H>
#include <FL/Fl_Hold_Browser.H>
#include <FL/Fl_Tooltip.H>
#include <FL/fl_ask.H>
#include <FL/fl_draw.H>

#ifdef _WIN32
    #include <FL/x.H>
    #include <windows.h>
#else
    #include <unistd.h>
#endif

#ifdef FLW_USE_PNG
    #include <FL/Fl_PNG_Image.H>
    #include <FL/fl_draw.H>
#endif

/***
 *       __ _
 *      / _| |
 *     | |_| |_      __
 *     |  _| \ \ /\ / /
 *     | | | |\ V  V /
 *     |_| |_| \_/\_/
 *
 *
 */

namespace flw {

std::vector<char*>          PREF_FONTNAMES;
int                         PREF_FIXED_FONT         = FL_COURIER;
std::string                 PREF_FIXED_FONTNAME     = "FL_COURIER";
int                         PREF_FIXED_FONTSIZE     = 14;
int                         PREF_FONT               = FL_HELVETICA;
int                         PREF_FONTSIZE           = 14;
std::string                 PREF_FONTNAME           = "FL_HELVETICA";
std::string                 PREF_THEME              = "default";
const char* const           PREF_THEMES[]           = {
                                "default",
                                "gleam",
                                "blue gleam",
                                "dark gleam",
                                "tan gleam",
                                "gtk",
                                "blue gtk",
                                "dark gtk",
                                "tan gtk",
                                "oxy",
                                "tan oxy",
                                "plastic",
                                "tan plastic",
                                nullptr,
};

const char* const           PREF_THEMES2[]           = {
                                "default",
                                "gleam",
                                "blue_gleam",
                                "dark_gleam",
                                "tan_gleam",
                                "gtk",
                                "blue_gtk",
                                "dark_gtk",
                                "tan_gtk",
                                "oxy",
                                "tan_oxy",
                                "plastic",
                                "tan_plastic",
                                nullptr,
};

//------------------------------------------------------------------------------
bool Buf::operator==(const Buf& other) const {
    return p != nullptr && s == other.s && memcmp(p, other.p, s) == 0;
}

/***
 *          _      _
 *         | |    | |
 *       __| | ___| |__  _   _  __ _
 *      / _` |/ _ \ '_ \| | | |/ _` |
 *     | (_| |  __/ |_) | |_| | (_| |
 *      \__,_|\___|_.__/ \__,_|\__, |
 *                              __/ |
 *                             |___/
 */

//------------------------------------------------------------------------------
void debug::print(Fl_Widget* widget) {
    std::string indent;
    debug::print(widget, indent);
}

//------------------------------------------------------------------------------
void debug::print(Fl_Widget* widget, std::string& indent) {
    if (widget == nullptr) {
        puts("debug::print() null widget");
    }
    else {
        printf("%sx=%4d, y=%4d, w=%4d, h=%4d \"%s\"\n", indent.c_str(), widget->x(), widget->y(), widget->w(), widget->h(), widget->label() ? widget->label() : "NO_LABEL");
        auto group = widget->as_group();

        if (group) {
            indent += "\t";
            for (int f = 0; f < group->children(); f++) {
                debug::print(group->child(f), indent);
            }
            indent.pop_back();
        }
    }
    fflush(stdout);
}

/***
 *
 *
 *      _ __ ___   ___ _ __  _   _
 *     | '_ ` _ \ / _ \ '_ \| | | |
 *     | | | | | |  __/ | | | |_| |
 *     |_| |_| |_|\___|_| |_|\__,_|
 *
 *
 */

namespace menu {

//----------------------------------------------------------------------
static Fl_Menu_Item* _item(Fl_Menu_* menu, const char* text) {
    assert(menu && text);
    auto item = menu->find_item(text);
#ifdef DEBUG
    if (item == nullptr) fprintf(stderr, "error: cant find menu item <%s>\n", text);
#endif
    return const_cast<Fl_Menu_Item*>(item);
}

} // menu

//----------------------------------------------------------------------
void menu::enable_item(Fl_Menu_* menu, const char* text, bool value) {
    auto item = _item(menu, text);
    if (item == nullptr) return;
    if (value == true) item->activate();
    else item->deactivate();
}

//----------------------------------------------------------------------
Fl_Menu_Item* menu::get_item(Fl_Menu_* menu, const char* text) {
    return _item(menu, text);
}

//----------------------------------------------------------------------
bool menu::item_value(Fl_Menu_* menu, const char* text) {
    auto item = _item(menu, text);
    if (item == nullptr) return false;
    return item->value();
}

//----------------------------------------------------------------------
void menu::set_item(Fl_Menu_* menu, const char* text, bool value) {
    auto item = _item(menu, text);
    if (item == nullptr) return;
    if (value == true) item->set();
    else item->clear();
}

//----------------------------------------------------------------------
void menu::setonly_item(Fl_Menu_* menu, const char* text) {
    auto item = _item(menu, text);
    if (item == nullptr) return;
    menu->setonly(item);
}

/***
 *            _   _ _
 *           | | (_) |
 *      _   _| |_ _| |
 *     | | | | __| | |
 *     | |_| | |_| | |
 *      \__,_|\__|_|_|
 *
 *
 */

//------------------------------------------------------------------------------
void util::center_window(Fl_Window* window, Fl_Window* parent) {
    if (parent != nullptr) {
        int x = parent->x() + parent->w() / 2;
        int y = parent->y() + parent->h() / 2;
        window->resize(x - window->w() / 2, y - window->h() / 2, window->w(), window->h());
    }
    else {
        window->resize((Fl::w() / 2) - (window->w() / 2), (Fl::h() / 2) - (window->h() / 2), window->w(), window->h());
    }
}

//------------------------------------------------------------------------------
// Return time as seconds since 1970
//
double util::clock() {
#ifdef _WIN32
    struct timeb timeVal;
    ftime(&timeVal);
    return (double) timeVal.time + (double) (timeVal.millitm / 1000.0);
#else
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return (double) (ts.tv_sec) + (ts.tv_nsec / 1000000000.0);
#endif
}

//------------------------------------------------------------------------------
Fl_Widget* util::find_widget(Fl_Group* group, std::string label) {
    for (int f = 0; f < group->children(); f++) {
        auto w = group->child(f);

        if (w->label() != nullptr && label == w->label()) {
            return w;
        }
    }

    for (int f = 0; f < group->children(); f++) {
        auto w = group->child(f);
        auto g = w->as_group();

        if (g != nullptr) {
            w = util::find_widget(g, label);

            if (w != nullptr) {
                return w;
            }
        }
    }

    return nullptr;
}

//------------------------------------------------------------------------------
std::string util::fix_menu_string(std::string in) {
    std::string res = in;

    util::replace_string(res, "\\", "\\\\");
    util::replace_string(res, "_", "\\_");
    util::replace_string(res, "/", "\\/");
    util::replace_string(res, "&", "&&");

    return res;
}

//------------------------------------------------------------------------------
std::string util::format(const char* format, ...) {
    if (format == nullptr || *format == 0) return "";

    int         l   = 128;
    int         n   = 0;
    char*       buf = static_cast<char*>(calloc(l, 1));
    std::string res;
    va_list     args;

    va_start(args, format);
    n = vsnprintf(buf, l, format, args);
    va_end(args);

    if (n < 0) {
        free(buf);
        return res;
    }

    if (n < l) {
        res = buf;
        free(buf);
        return res;
    }

    free(buf);
    l = n + 1;
    buf = static_cast<char*>(calloc(l, 1));
    if (buf == nullptr) return res;

    va_start(args, format);
    vsnprintf(buf, l, format, args);
    va_end(args);
    res = buf;
    free(buf);
    return res;
}

//------------------------------------------------------------------------------
std::string util::format_int(int64_t num, char del) {
    auto pos = 0;
    char tmp1[32];
    char tmp2[32];

    if (del < 1) {
        del = 32;
    }
    memset(tmp2, 0, 32);
    snprintf(tmp1, 32, "%lld", (long long int) num);
    auto len = strlen(tmp1);

    for (int f = len - 1, i = 0; f >= 0 && pos < 32; f--, i++) {
        char c = tmp1[f];

        if ((i % 3) == 0 && i > 0 && c != '-') {
            tmp2[pos++] = del;
        }

        tmp2[pos++] = c;
    }

    std::string r = tmp2;
    std::reverse(r.begin(), r.end());
    return r;
}

//------------------------------------------------------------------------------
bool util::is_whitespace_or_empty(const char* str) {
    while (*str != 0) {
        if (*str != 9 && *str != 32) {
            return false;
        }

        str++;
    }

    return true;
}

//------------------------------------------------------------------------------
// Set label font properties for widget
// If widget is an group widget set also the font for child widgets (recursive)
//
void util::labelfont(Fl_Widget* widget, Fl_Font fn, int fs) {
    assert(widget);

    widget->labelfont(fn);
    widget->labelsize(fs);

    auto group = widget->as_group();

    if (group != nullptr) {
        for (auto f = 0; f < group->children(); f++) {
            util::labelfont(group->child(f), fn, fs);
        }
    }
}

//------------------------------------------------------------------------------
// Return time stamp
//
int32_t util::milliseconds() {
#if defined(_WIN32)
    LARGE_INTEGER StartingTime;
    LARGE_INTEGER Frequency;

    QueryPerformanceFrequency(&Frequency);
    QueryPerformanceCounter(&StartingTime);

    StartingTime.QuadPart *= 1000000;
    StartingTime.QuadPart /= Frequency.QuadPart;
    return StartingTime.QuadPart / 1000;
#else
    timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return (t.tv_sec * 1000000 + t.tv_nsec / 1000) / 1000;
#endif
}

//------------------------------------------------------------------------------
// Must be compiled with FLW_USE_PNG flag and linked with fltk images (fltk-config --ldflags --use-images)
// If filename is empty you will be asked for the name
//
void util::png_save(std::string opt_name, Fl_Window* window, int X, int Y, int W, int H) {
#ifdef FLW_USE_PNG
    auto filename = (opt_name == "") ? fl_file_chooser("Save To PNG File", "All Files (*)\tPNG Files (*.png)", "") : opt_name.c_str();

    if (filename != nullptr) {
        window->make_current();

        if (X == 0 && Y == 0 && W == 0 && H == 0) {
            X = window->x();
            Y = window->y();
            W = window->w();
            H = window->h();
        }

        auto image = fl_read_image(nullptr, X, Y, W, H);


        if (image != nullptr) {
            auto ret = fl_write_png(filename, image, W, H);

            if (ret == 0) {
            }
            else if (ret == -1) {
                fl_alert("%s", "error: missing libraries");
            }
            else if (ret == -2) {
                fl_alert("error: failed to save image to %s", filename);
            }

            delete []image;
        }
        else {
            fl_alert("%s", "error: failed to grab image");
        }
    }
#else
    (void) opt_name;
    (void) window;
    (void) X;
    (void) Y;
    (void) W;
    (void) H;
    fl_alert("error: flw not compiled with FLW_USE_PNG flag");
#endif
}

//------------------------------------------------------------------------------
std::string util::print(std::string ps_filename, Fl_Paged_Device::Page_Format format, Fl_Paged_Device::Page_Layout layout, PrintCallback cb, Fl_Widget* widget, int from, int to) {
    bool                      cont = true;
    FILE*                     file = nullptr;
    Fl_PostScript_File_Device printer;
    int                       ph;
    int                       pw;
    std::string               res;
    
    if ((file = fl_fopen(ps_filename.c_str(), "wb")) == nullptr) {
        return "error: could not open file!";
    }

    printer.begin_job(file, 0, format, layout);
   
    while (cont == true) {
        if (printer.begin_page() != 0) {
            res = "error: couldn't create new page!";
            goto ERR;
        }

        if (printer.printable_rect(&pw, &ph) != 0) {
            res = "error: couldn't retrieve page size!";
            goto ERR;
        }

        fl_push_clip(0, 0, pw, ph);
        cont = cb(widget, pw, ph, from, to);
        fl_pop_clip();
        
        if (printer.end_page() != 0) {
            res = "error: couldn't end page!";
            goto ERR;
        }
    }
    
ERR:
    printer.end_job();
    fclose(file);
    return res;
}

//------------------------------------------------------------------------------
std::string util::remove_browser_format(const char* text) {
    auto res = std::string((text != nullptr) ? text : "");
    auto f   = res.find_last_of("@");

    if (f != std::string::npos) {
        auto tmp = res.substr(f + 1);

        if (tmp[0] == '.' || tmp[0] == 'l' || tmp[0] == 'm' || tmp[0] == 's' || tmp[0] == 'b' || tmp[0] == 'i' || tmp[0] == 'f' || tmp[0] == 'c' || tmp[0] == 'r' || tmp[0] == 'u' || tmp[0] == '-') {
            res = tmp.substr(1);
        }
        else if (tmp[0] == 'B' || tmp[0] == 'C' || tmp[0] == 'F' || tmp[0] == 'S') {
            auto s = std::string();
            auto e = false;

            tmp = tmp.substr(f + 1);

            for (auto c : tmp) {
                if (e == false && c >= '0' && c <= '9') {
                }
                else {
                    e = true;
                    s += c;
                }
            }

            res = s;
        }
        else {
            res = res.substr(f);
        }
    }

    return res;
}

//------------------------------------------------------------------------------
// Replace string and return number of replacements or -1 for error
//
std::string& util::replace_string(std::string& string, std::string find, std::string replace) {
    if (find == "") {
        return string;
    }
    
    try {
        size_t start = 0;

        while ((start = string.find(find, start)) != std::string::npos) {
            string.replace(start, find.length(), replace);
            start += replace.length();
        }

    }
    catch(...) {
        string = "";
    }

    return string;
}

//------------------------------------------------------------------------------
void util::sleep(int milli) {
#ifdef _WIN32
    Sleep(milli);
#else
    usleep(milli * 1000);
#endif
}


//------------------------------------------------------------------------------
// Split string and return an vector with splitted strings
//
flw::StringVector util::split_string(const std::string& string, std::string split) {
    auto res = StringVector();

    try {
        if (split != "") {
            auto pos1 = (std::string::size_type) 0;
            auto pos2 = string.find(split);

            while (pos2 != std::string::npos) {
                res.push_back(string.substr(pos1, pos2 - pos1));
                pos1 = pos2 + split.size();
                pos2 = string.find(split, pos1);
            }

            if (pos1 <= string.size()) {
                res.push_back(string.substr(pos1));
            }
        }
    }
    catch(...) {
        res.clear();
    }

    return res;
}

//------------------------------------------------------------------------------
void* util::zero_memory(char* mem, size_t size) {
    if (mem == nullptr || size == 0) return mem;
#ifdef _WIN32
    RtlSecureZeroMemory(mem, size);
#else
    auto p = reinterpret_cast<volatile unsigned char*>(mem);

    while (size--) {
        *p = 0;
        p++;
    }
#endif

    return mem;
}

/***
 *                _
 *               | |
 *       ___ ___ | | ___  _ __
 *      / __/ _ \| |/ _ \| '__|
 *     | (_| (_) | | (_) | |
 *      \___\___/|_|\___/|_|
 *
 *
 */

Fl_Color color::BEIGE            = fl_rgb_color(245, 245, 220);
Fl_Color color::CHOCOLATE        = fl_rgb_color(210, 105,  30);
Fl_Color color::CRIMSON          = fl_rgb_color(220,  20,  60);
Fl_Color color::DARKOLIVEGREEN   = fl_rgb_color( 85, 107,  47);
Fl_Color color::DODGERBLUE       = fl_rgb_color( 30, 144, 255);
Fl_Color color::FORESTGREEN      = fl_rgb_color( 34, 139,  34);
Fl_Color color::GOLD             = fl_rgb_color(255, 215,   0);
Fl_Color color::GRAY             = fl_rgb_color(128, 128, 128);
Fl_Color color::INDIGO           = fl_rgb_color( 75,   0, 130);
Fl_Color color::OLIVE            = fl_rgb_color(128, 128,   0);
Fl_Color color::PINK             = fl_rgb_color(255, 192, 203);
Fl_Color color::ROYALBLUE        = fl_rgb_color( 65, 105, 225);
Fl_Color color::SIENNA           = fl_rgb_color(160,  82,  45);
Fl_Color color::SILVER           = fl_rgb_color(192, 192, 192);
Fl_Color color::SLATEGRAY        = fl_rgb_color(112, 128, 144);
Fl_Color color::TEAL             = fl_rgb_color(  0, 128, 128);
Fl_Color color::TURQUOISE        = fl_rgb_color( 64, 224, 208);
Fl_Color color::VIOLET           = fl_rgb_color(238, 130, 238);

/***
 *      _   _
 *     | | | |
 *     | |_| |__   ___ _ __ ___   ___
 *     | __| '_ \ / _ \ '_ ` _ \ / _ \
 *     | |_| | | |  __/ | | | | |  __/
 *      \__|_| |_|\___|_| |_| |_|\___|
 *
 *
 */

namespace theme {

static unsigned char _OLD_R[256]  = { 0 };
static unsigned char _OLD_G[256]  = { 0 };
static unsigned char _OLD_B[256]  = { 0 };
static bool          _IS_DARK     = false;
static bool          _SAVED_COLOR = false;
static int           _SCROLLSIZE  = Fl::scrollbar_size();

//----------------------------------------------------------------------
// Colors are reset every time a new theme has been selected
//
static void _additional_colors(bool dark) {
    color::BEIGE            = fl_rgb_color(245, 245, 220);
    color::CHOCOLATE        = fl_rgb_color(210, 105,  30);
    color::CRIMSON          = fl_rgb_color(220,  20,  60);
    color::DARKOLIVEGREEN   = fl_rgb_color( 85, 107,  47);
    color::DODGERBLUE       = fl_rgb_color( 30, 144, 255);
    color::FORESTGREEN      = fl_rgb_color( 34, 139,  34);
    color::GOLD             = fl_rgb_color(255, 215,   0);
    color::GRAY             = fl_rgb_color(128, 128, 128);
    color::INDIGO           = fl_rgb_color( 75,   0, 130);
    color::OLIVE            = fl_rgb_color(128, 128,   0);
    color::PINK             = fl_rgb_color(255, 192, 203);
    color::ROYALBLUE        = fl_rgb_color( 65, 105, 225);
    color::SIENNA           = fl_rgb_color(160,  82,  45);
    color::SILVER           = fl_rgb_color(192, 192, 192);
    color::SLATEGRAY        = fl_rgb_color(112, 128, 144);
    color::TEAL             = fl_rgb_color(  0, 128, 128);
    color::TURQUOISE        = fl_rgb_color( 64, 224, 208);
    color::VIOLET           = fl_rgb_color(238, 130, 238);

    if (dark == true) {
        color::BEIGE            = fl_darker(color::BEIGE);
        color::CHOCOLATE        = fl_darker(color::CHOCOLATE);
        color::CRIMSON          = fl_darker(color::CRIMSON);
        color::DARKOLIVEGREEN   = fl_darker(color::DARKOLIVEGREEN);
        color::DODGERBLUE       = fl_darker(color::DODGERBLUE);
        color::FORESTGREEN      = fl_darker(color::FORESTGREEN);
        color::GOLD             = fl_darker(color::GOLD);
        color::GRAY             = fl_darker(color::GRAY);
        color::INDIGO           = fl_darker(color::INDIGO);
        color::OLIVE            = fl_darker(color::OLIVE);
        color::PINK             = fl_darker(color::PINK);
        color::ROYALBLUE        = fl_darker(color::ROYALBLUE);
        color::SIENNA           = fl_darker(color::SIENNA);
        color::SILVER           = fl_darker(color::SILVER);
        color::SLATEGRAY        = fl_darker(color::SLATEGRAY);
        color::TEAL             = fl_darker(color::TEAL);
        color::TURQUOISE        = fl_darker(color::TURQUOISE);
        color::VIOLET           = fl_darker(color::VIOLET);
    }
}

//----------------------------------------------------------------------
static void _blue_colors() {
    Fl::set_color(0,   228, 228, 228); // FL_FOREGROUND_COLOR
    Fl::set_color(7,    79,  86,  94); // FL_BACKGROUND2_COLOR
    Fl::set_color(8,   108, 113, 125); // FL_INACTIVE_COLOR
    Fl::set_color(15,  241, 196,  126); // FL_SELECTION_COLOR
    Fl::set_color(56,    0,   0,   0); // FL_BLACK
    Fl::background(48, 56, 65);

    unsigned char r = 0;
    unsigned char g = 0;
    unsigned char b = 0;

    for (int f = 32; f < 49; f++) {
        Fl::set_color(f, r, g, b);

        if (f == 32) {
            r = 0;
            g = 9;
            b = 18;
        }
        else {
            r += 2 + (f < 44);
            g += 2 + (f < 44);
            b += 2 + (f < 44);
        }
//        if (f > 40) c++;
    }
}

//----------------------------------------------------------------------
static void _dark_colors() {
    Fl::set_color(0,   200, 200, 200); // FL_FOREGROUND_COLOR
    Fl::set_color(7,    64,  64,  64); // FL_BACKGROUND2_COLOR
    Fl::set_color(8,   100, 100, 100); // FL_INACTIVE_COLOR
    Fl::set_color(15,  177, 227, 177); // FL_SELECTION_COLOR
    Fl::set_color(56,    0,   0,   0); // FL_BLACK
    Fl::set_color(49, 43, 43, 43);
    Fl::background(43, 43, 43);

    unsigned char c = 0;

    for (int f = 32; f < 49; f++) {
        Fl::set_color(f, c, c, c);
        c += 2;
        if (f > 40) c++;
    }
}

//----------------------------------------------------------------------
static void _make_default_colors_darker() {
    Fl::set_color(FL_GREEN, fl_darker(Fl::get_color(FL_GREEN)));
    Fl::set_color(FL_DARK_GREEN, fl_darker(Fl::get_color(FL_DARK_GREEN)));
    Fl::set_color(FL_RED, fl_darker(Fl::get_color(FL_RED)));
    Fl::set_color(FL_DARK_RED, fl_darker(Fl::get_color(FL_DARK_RED)));
    Fl::set_color(FL_YELLOW, fl_darker(Fl::get_color(FL_YELLOW)));
    Fl::set_color(FL_DARK_YELLOW, fl_darker(Fl::get_color(FL_DARK_YELLOW)));
    Fl::set_color(FL_BLUE, fl_darker(Fl::get_color(FL_BLUE)));
    Fl::set_color(FL_DARK_BLUE, fl_darker(Fl::get_color(FL_DARK_BLUE)));
    Fl::set_color(FL_CYAN, fl_darker(Fl::get_color(FL_CYAN)));
    Fl::set_color(FL_DARK_CYAN, fl_darker(Fl::get_color(FL_DARK_CYAN)));
    Fl::set_color(FL_MAGENTA, fl_darker(Fl::get_color(FL_MAGENTA)));
    Fl::set_color(FL_DARK_MAGENTA, fl_darker(Fl::get_color(FL_DARK_MAGENTA)));
}

//----------------------------------------------------------------------
static void _restore_colors() {
    if (_SAVED_COLOR == true) {
        for (int f = 0; f < 256; f++) {
            Fl::set_color(f, theme::_OLD_R[f], theme::_OLD_G[f], theme::_OLD_B[f]);
        }
    }
}

//----------------------------------------------------------------------
static void _save_colors() {
    if (_SAVED_COLOR == false) {
        for (int f = 0; f < 256; f++) {
            unsigned char r1, g1, b1;
            Fl::get_color(f, r1, g1, b1);
            theme::_OLD_R[f] = r1;
            theme::_OLD_G[f] = g1;
            theme::_OLD_B[f] = b1;
        }

        _SAVED_COLOR = true;
    }
}

//----------------------------------------------------------------------
static void _tan_colors() {
    Fl::set_color(0,     0,   0,   0); // FL_FOREGROUND_COLOR
    Fl::set_color(7,   255, 255, 255); // FL_BACKGROUND2_COLOR
    Fl::set_color(8,    85,  85,  85); // FL_INACTIVE_COLOR
    Fl::set_color(15,  188, 114,  50); // FL_SELECTION_COLOR
    Fl::background(206, 202, 187);
}

//------------------------------------------------------------------------------
void _load_default() {
    theme::_save_colors();
    theme::_restore_colors();
    theme::_additional_colors(false);
    Fl::scheme("none");
    Fl::redraw();
    flw::PREF_THEME = flw::PREF_THEMES[theme::THEME_DEFAULT];
    _IS_DARK = false;
}

//------------------------------------------------------------------------------
void _load_gleam() {
    theme::_save_colors();
    theme::_restore_colors();
    theme::_additional_colors(false);
    Fl::scheme("gleam");
    Fl::redraw();
    flw::PREF_THEME = flw::PREF_THEMES[theme::THEME_GLEAM];
    _IS_DARK = false;
}

//------------------------------------------------------------------------------
void _load_gleam_blue() {
    theme::_save_colors();
    theme::_restore_colors();
    theme::_make_default_colors_darker();
    theme::_blue_colors();
    theme::_additional_colors(true);
    Fl::set_color(255, 101, 117, 125);
    Fl::scheme("gleam");
    Fl::redraw();
    flw::PREF_THEME = flw::PREF_THEMES[theme::THEME_GLEAM_BLUE];
    _IS_DARK = true;
}

//------------------------------------------------------------------------------
void _load_gleam_dark() {
    theme::_save_colors();
    theme::_restore_colors();
    theme::_make_default_colors_darker();
    theme::_dark_colors();
    theme::_additional_colors(true);
    Fl::set_color(255, 112, 112, 112);
    Fl::scheme("gleam");
    Fl::redraw();
    flw::PREF_THEME = flw::PREF_THEMES[theme::THEME_GLEAM_DARK];
    _IS_DARK = true;
}

//------------------------------------------------------------------------------
void _load_gleam_tan() {
    theme::_save_colors();
    theme::_restore_colors();
    theme::_tan_colors();
    theme::_additional_colors(false);
    Fl::scheme("gleam");
    Fl::redraw();
    flw::PREF_THEME = flw::PREF_THEMES[theme::THEME_GLEAM_TAN];
    _IS_DARK = false;
}

//------------------------------------------------------------------------------
void _load_gtk() {
    theme::_save_colors();
    theme::_restore_colors();
    theme::_additional_colors(false);
    Fl::scheme("gtk+");
    Fl::redraw();
    flw::PREF_THEME = flw::PREF_THEMES[theme::THEME_GTK];
    _IS_DARK = false;
}

//------------------------------------------------------------------------------
void _load_gtk_blue() {
    theme::_save_colors();
    theme::_restore_colors();
    theme::_make_default_colors_darker();
    theme::_blue_colors();
    theme::_additional_colors(true);
    Fl::set_color(255, 101, 117, 125);
    Fl::scheme("gtk+");
    Fl::redraw();
    flw::PREF_THEME = flw::PREF_THEMES[theme::THEME_GTK_BLUE];
    _IS_DARK = true;
}

//------------------------------------------------------------------------------
void _load_gtk_dark() {
    theme::_save_colors();
    theme::_restore_colors();
    theme::_make_default_colors_darker();
    theme::_dark_colors();
    theme::_additional_colors(true);
    Fl::set_color(255, 112, 112, 112);
    Fl::scheme("gtk+");
    Fl::redraw();
    flw::PREF_THEME = flw::PREF_THEMES[theme::THEME_GTK_DARK];
    _IS_DARK = true;
}

//------------------------------------------------------------------------------
void _load_gtk_tan() {
    theme::_save_colors();
    theme::_restore_colors();
    theme::_tan_colors();
    theme::_additional_colors(false);
    Fl::scheme("gtk+");
    Fl::redraw();
    flw::PREF_THEME = flw::PREF_THEMES[theme::THEME_GTK_TAN];
    _IS_DARK = false;
}

//------------------------------------------------------------------------------
void _load_oxy() {
    theme::_save_colors();
    theme::_restore_colors();
    theme::_additional_colors(false);
    Fl::scheme("oxy");
    Fl::redraw();
    flw::PREF_THEME = flw::PREF_THEMES[theme::THEME_OXY];
    _IS_DARK = false;
}

//------------------------------------------------------------------------------
void _load_oxy_tan() {
    theme::_save_colors();
    theme::_restore_colors();
    theme::_tan_colors();
    theme::_additional_colors(false);
    Fl::scheme("oxy");
    Fl::redraw();
    flw::PREF_THEME = flw::PREF_THEMES[theme::THEME_OXY_TAN];
    _IS_DARK = false;
}

//------------------------------------------------------------------------------
void _load_plastic() {
    theme::_save_colors();
    theme::_restore_colors();
    theme::_additional_colors(false);
    Fl::scheme("plastic");
    Fl::redraw();
    flw::PREF_THEME = flw::PREF_THEMES[theme::THEME_PLASTIC];
    _IS_DARK = false;
}

//------------------------------------------------------------------------------
void _load_plastic_tan() {
    theme::_save_colors();
    theme::_restore_colors();
    theme::_tan_colors();
    theme::_additional_colors(false);
    Fl::scheme("plastic");
    Fl::redraw();
    flw::PREF_THEME = flw::PREF_THEMES[theme::THEME_PLASTIC_TAN];
    _IS_DARK = false;
}

//------------------------------------------------------------------------------
void _scrollbar() {
    if (flw::PREF_FONTSIZE < 12 || flw::PREF_FONTSIZE > 16) {
        auto f = (double) flw::PREF_FONTSIZE / 14.0;
        auto s = (int) (f * theme::_SCROLLSIZE);
        Fl::scrollbar_size(s);
    }
    else if (theme::_SCROLLSIZE > 0) {
        Fl::scrollbar_size(theme::_SCROLLSIZE);
    }
}

} // theme

//------------------------------------------------------------------------------
bool theme::is_dark() {
    if (flw::PREF_THEME == flw::PREF_THEMES[theme::THEME_GLEAM_BLUE] ||
        flw::PREF_THEME == flw::PREF_THEMES[theme::THEME_GLEAM_DARK] ||
        flw::PREF_THEME == flw::PREF_THEMES[theme::THEME_GTK_BLUE] ||
        flw::PREF_THEME == flw::PREF_THEMES[theme::THEME_GTK_DARK]) {
        return true;
    }
    else {
        return false;
    }
}

//------------------------------------------------------------------------------
bool theme::load(std::string name) {
    if (theme::_SCROLLSIZE == 0) {
        theme::_SCROLLSIZE = Fl::scrollbar_size();
    }

    if (name == flw::PREF_THEMES[theme::THEME_DEFAULT] || name == flw::PREF_THEMES2[theme::THEME_DEFAULT]) {
        theme::_load_default();
    }
    else if (name == flw::PREF_THEMES[theme::THEME_GLEAM] || name == flw::PREF_THEMES2[theme::THEME_GLEAM]) {
        theme::_load_gleam();
    }
    else if (name == flw::PREF_THEMES[theme::THEME_GLEAM_BLUE] || name == flw::PREF_THEMES2[theme::THEME_GLEAM_BLUE]) {
        theme::_load_gleam_blue();
    }
    else if (name == flw::PREF_THEMES[theme::THEME_GLEAM_DARK] || name == flw::PREF_THEMES2[theme::THEME_GLEAM_DARK]) {
        theme::_load_gleam_dark();
    }
    else if (name == flw::PREF_THEMES[theme::THEME_GLEAM_TAN] || name == flw::PREF_THEMES2[theme::THEME_GLEAM_TAN]) {
        theme::_load_gleam_tan();
    }
    else if (name == flw::PREF_THEMES[theme::THEME_GTK] || name == flw::PREF_THEMES2[theme::THEME_GTK]) {
        theme::_load_gtk();
    }
    else if (name == flw::PREF_THEMES[theme::THEME_GTK_BLUE] || name == flw::PREF_THEMES2[theme::THEME_GTK_BLUE]) {
        theme::_load_gtk_blue();
    }
    else if (name == flw::PREF_THEMES[theme::THEME_GTK_DARK] || name == flw::PREF_THEMES2[theme::THEME_GTK_DARK]) {
        theme::_load_gtk_dark();
    }
    else if (name == flw::PREF_THEMES[theme::THEME_GTK_TAN] || name == flw::PREF_THEMES2[theme::THEME_GTK_TAN]) {
        theme::_load_gtk_tan();
    }
    else if (name == flw::PREF_THEMES[theme::THEME_OXY] || name == flw::PREF_THEMES2[theme::THEME_OXY]) {
        theme::_load_oxy();
    }
    else if (name == flw::PREF_THEMES[theme::THEME_OXY_TAN] || name == flw::PREF_THEMES2[theme::THEME_OXY_TAN]) {
        theme::_load_oxy_tan();
    }
    else if (name == flw::PREF_THEMES[theme::THEME_PLASTIC] || name == flw::PREF_THEMES2[theme::THEME_PLASTIC]) {
        theme::_load_plastic();
    }
    else if (name == flw::PREF_THEMES[theme::THEME_PLASTIC_TAN] || name == flw::PREF_THEMES2[theme::THEME_PLASTIC_TAN]) {
        theme::_load_plastic_tan();
    }
    else {
        return false;
    }

    theme::_scrollbar();
    return true;
}

//------------------------------------------------------------------------------
int theme::load_font(std::string requested_font) {
    theme::load_fonts();

    auto count = 0;

    for (auto font : flw::PREF_FONTNAMES) {
        auto font2 = util::remove_browser_format(font);

        if (requested_font == font2) {
            return count;
        }

        count++;
    }

    return -1;
}

//------------------------------------------------------------------------------
// Load fonts only once in the program
//
void theme::load_fonts(bool iso8859_only) {
    if (flw::PREF_FONTNAMES.size() == 0) {
        auto fonts = Fl::set_fonts((iso8859_only == true) ? nullptr : "-*");

        for (int f = 0; f < fonts; f++) {
            auto attr  = 0;
            auto name1 = Fl::get_font_name((Fl_Font) f, &attr);
            auto name2 = std::string();

            if (attr & FL_BOLD) {
                name2 = std::string("@b");
            }

            if (attr & FL_ITALIC) {
                name2 += std::string("@i");
            }

            name2 += std::string("@.");
            name2 += name1;
            flw::PREF_FONTNAMES.push_back(strdup(name2.c_str()));
//             printf("%3d - %-40s - %-40s\n", f, name1, name2.c_str()); fflush(stdout);
        }
    }
}

//------------------------------------------------------------------------------
// Load icon before showing window
//
void theme::load_icon(Fl_Window* win, int win_resource, const char** xpm_resource, const char* name) {
    assert(win);

    if (win->shown() != 0) {
        fl_alert("%s", "warning: load icon before showing window!");
    }

#if defined(_WIN32)
    win->icon(reinterpret_cast<char*>(LoadIcon(fl_display, MAKEINTRESOURCE(win_resource))));
    (void) name;
    (void) xpm_resource;
    (void) name;
#elif defined(__linux__)
    assert(xpm_resource);

    Fl_Pixmap    pix(xpm_resource);
    Fl_RGB_Image rgb(&pix, Fl_Color(0));

    win->icon(&rgb);
    win->xclass((name != nullptr) ? name : "FLTK");
    (void) win_resource;
#else
    (void) win;
    (void) win_resource;
    (void) xpm_resource;
    (void) name;
#endif
}

//------------------------------------------------------------------------------
// Load theme and font data
//
void theme::load_theme_pref(Fl_Preferences& pref) {
    auto val = 0;
    char buffer[4000];

    pref.get("regular_name", buffer, "", 4000);

    if (*buffer != 0 && strcmp("FL_HELVETICA", buffer) != 0) {
        auto font = theme::load_font(buffer);

        if (font != -1) {
            flw::PREF_FONT     = font;
            flw::PREF_FONTNAME = buffer;
        }
    }

    pref.get("regular_size", val, flw::PREF_FONTSIZE);

    if (val >= 6 && val <= 72) {
        flw::PREF_FONTSIZE = val;
    }

    pref.get("mono_name", buffer, "", 1000);

    if (*buffer != 0 && strcmp("FL_COURIER", buffer) != 0) {
        auto font = theme::load_font(buffer);

        if (font != -1) {
            flw::PREF_FIXED_FONT     = font;
            flw::PREF_FIXED_FONTNAME = buffer;
        }
    }

    pref.get("mono_size", val, flw::PREF_FIXED_FONTSIZE);

    if (val >= 6 && val <= 72) {
        flw::PREF_FIXED_FONTSIZE = val;
    }

    pref.get("theme", buffer, "oxy", 4000);
    theme::load(buffer);
    Fl_Tooltip::font(flw::PREF_FONT);
    Fl_Tooltip::size(flw::PREF_FONTSIZE);
    _scrollbar();
}

//------------------------------------------------------------------------------
// Load window size
//
void theme::load_win_pref(Fl_Preferences& pref, Fl_Window* window, int show_0_1_2, int defw, int defh, std::string basename) {
    assert(window);

    int  x, y, w, h, f, m;

    pref.get((basename + "x").c_str(), x, 80);
    pref.get((basename + "y").c_str(), y, 60);
    pref.get((basename + "w").c_str(), w, defw);
    pref.get((basename + "h").c_str(), h, defh);
    pref.get((basename + "fullscreen").c_str(), f, 0);
    pref.get((basename + "maximized").c_str(), m, 0);

    if (w == 0 || h == 0) {
        w = 800;
        h = 600;
    }

    if (x + w <= 0 || y + h <= 0 || x >= Fl::w() || y >= Fl::h()) {
        x = 80;
        y = 60;
    }

    if (show_0_1_2 > 1 && window->shown() == 0) {
        window->show();
    }

    window->resize(x, y, w, h);

    if (f == 1) {
        window->fullscreen();
    }
    else if (m == 1) {
        window->maximize();
    }

    if (show_0_1_2 == 1 && window->shown() == 0) {
        window->show();
    }
}

//------------------------------------------------------------------------------
bool theme::parse(int argc, const char** argv) {
    auto res = false;

    for (auto f = 1; f < argc; f++) {
        if (res == false) {
            res = theme::load(argv[f]);
        }

        auto fontsize = atoi(argv[f]);

        if (fontsize >= 6 && fontsize <= 72) {
            flw::PREF_FONTSIZE = fontsize;
        }
    }

    flw::PREF_FIXED_FONTSIZE = flw::PREF_FONTSIZE;
    return res;
}

//------------------------------------------------------------------------------
// Save theme and font data
//
void theme::save_theme_pref(Fl_Preferences& pref) {
    pref.set("theme", flw::PREF_THEME.c_str());
    pref.set("regular_name", flw::PREF_FONTNAME.c_str());
    pref.set("regular_size", flw::PREF_FONTSIZE);
    pref.set("mono_name", flw::PREF_FIXED_FONTNAME.c_str());
    pref.set("mono_size", flw::PREF_FIXED_FONTSIZE);
}

//------------------------------------------------------------------------------
// Save window size
//
void theme::save_win_pref(Fl_Preferences& pref, Fl_Window* window, std::string basename) {
    assert(window);

    pref.set((basename + "x").c_str(), window->x());
    pref.set((basename + "y").c_str(), window->y());
    pref.set((basename + "w").c_str(), window->w());
    pref.set((basename + "h").c_str(), window->h());
    pref.set((basename + "fullscreen").c_str(), window->fullscreen_active() ? 1 : 0);
    pref.set((basename + "maximized").c_str(), window->maximize_active() ? 1 : 0);
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
Buf::Buf() {
    p = nullptr;
    s = 0;
}

//------------------------------------------------------------------------------
Buf::Buf(size_t S) {
    p = (S < SIZE_MAX) ? static_cast<char*>(calloc(S + 1, 1)) : nullptr;
    s = 0;

    if (p != nullptr) {
        s = S;
    }
}

//------------------------------------------------------------------------------
Buf::Buf(const char* P, size_t S, bool grab) {
    if (grab == true) {
        p = const_cast<char*>(P);
        s = S;
    }
    else if (P == nullptr) {
        p = nullptr;
        s = 0;
    }
    else {
        p = static_cast<char*>(calloc(S + 1, 1));
        s = 0;

        if (p == nullptr) {
            return;
        }

        memcpy(p, P, S);
        s = S;
    }
}

//------------------------------------------------------------------------------
Buf::Buf(const Buf& b) {
    if (b.p == nullptr) {
        p = nullptr;
        s = 0;
    }
    else {
        p = static_cast<char*>(calloc(b.s + 1, 1));
        s = 0;

        if (p == nullptr) {
            return;
        }

        memcpy(p, b.p, b.s);
        s = b.s;
    }
}

//------------------------------------------------------------------------------
Buf::Buf(Buf&& b) {
    p = b.p;
    s = b.s;
    b.p = nullptr;
}

//------------------------------------------------------------------------------
Buf& Buf::operator=(const Buf& b) {
    if (this == &b) {
    }
    if (b.p == nullptr) {
        free(p);
        p = nullptr;
        s = 0;
    }
    else {
        free(p);
        p = static_cast<char*>(calloc(b.s + 1, 1));
        s = 0;

        if (p == nullptr) {
            return *this;
        }

        memcpy(p, b.p, b.s);
        s = b.s;
    }

    return *this;
}

//------------------------------------------------------------------------------
Buf& Buf::operator=(Buf&& b) {
    free(p);
    p = b.p;
    s = b.s;
    b.p = nullptr;
    return *this;
}

//------------------------------------------------------------------------------
Buf& Buf::operator+=(const Buf& b) {
    if (b.p == nullptr) {
    }
    else if (p == nullptr) {
        *this = b;
    }
    else {
        auto t = static_cast<char*>(calloc(s + b.s + 1, 1));

        if (t == nullptr) {
            return *this;
        }

        memcpy(t, p, s);
        memcpy(t + s, b.p, b.s);
        free(p);
        p = t;
        s += b.s;
    }

    return *this;
}

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
File::File(std::string filename) {
    size  = 0;
    mtime = 0;
    type  = TYPE::NA;

#ifdef _WIN32
    wchar_t wbuffer[1025];
    struct __stat64 st;

    while (filename.empty() == false && (filename.back() == '\\' || filename.back() == '/')) {
        filename.pop_back();
    }

    fl_utf8towc(filename.c_str(), filename.length(), wbuffer, 1024);

    if (_wstat64(wbuffer, &st) == 0) {
        size  = st.st_size;
        mtime = st.st_mtime;

        if (S_ISDIR(st.st_mode)) {
            type = TYPE::DIR;
        }
        else if (S_ISREG(st.st_mode)) {
            type = TYPE::FILE;
        }
        else {
            type = TYPE::OTHER;
        }
    }
#else
    struct stat st;

    if (stat(filename.c_str(), &st) == 0) {
        size  = st.st_size;
        mtime = st.st_mtime;

        if (S_ISDIR(st.st_mode)) {
            type = TYPE::DIR;
        }
        else if (S_ISREG(st.st_mode)) {
            type = TYPE::FILE;
        }
        else {
            type = TYPE::OTHER;
        }
    }
#endif
}

//------------------------------------------------------------------------------
Buf File::Load(std::string filename, bool alert) {
    auto file = File(filename);

    if (file.type != TYPE::FILE) {
        if (alert == true) {
            fl_alert("error: file %s is missing or not an file", filename.c_str());
        }

        return Buf();
    }

    auto handle = fl_fopen(filename.c_str(), "rb");

    if (handle == nullptr) {
        if (alert == true) {
            fl_alert("error: can't open %s", filename.c_str());
        }

        return Buf();
    }

    auto buf  = Buf(file.size);
    auto read = fread(buf.p, 1, (size_t) file.size, handle);

    fclose(handle);

    if (read != (size_t) file.size) {
        if (alert == true) {
            fl_alert("error: failed to read %s", filename.c_str());
        }

        return Buf();
    }

    return buf;
}

//------------------------------------------------------------------------------
bool File::Save(std::string filename, const char* data, size_t size, bool alert) {
    auto file = fl_fopen(filename.c_str(), "wb");

    if (file != nullptr) {
        auto wrote = fwrite(data, 1, size, file);
        fclose(file);

        if (wrote != size) {
            if (alert == true) {
                fl_alert("error: saving data to %s failed", filename.c_str());
            }

            return false;
        }
    }
    else if (alert == true) {
        fl_alert("error: failed to open %s", filename.c_str());
        return false;
    }

    return true;
}

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
PrintText::PrintText(std::string filename, 
    Fl_Paged_Device::Page_Format page_format, 
    Fl_Paged_Device::Page_Layout page_layout, 
    Fl_Font font, 
    Fl_Fontsize fontsize, 
    Fl_Align align, 
    bool wrap, 
    bool border, 
    int line_num) {

    _align       = FL_ALIGN_INSIDE | FL_ALIGN_TOP;
    _align      |= (align == FL_ALIGN_CENTER || align == FL_ALIGN_RIGHT) ? align : FL_ALIGN_LEFT;
    _border      = border;
    _file        = nullptr;
    _filename    = filename;
    _font        = font;
    _fontsize    = fontsize;
    _line_num    = (align == FL_ALIGN_LEFT) ? line_num : 0;
    _page_format = page_format;
    _page_layout = page_layout;
    _printer     = nullptr;
    _wrap        = wrap;
}

//------------------------------------------------------------------------------
PrintText::~PrintText() {
    stop();
}

//------------------------------------------------------------------------------
void PrintText::check_for_new_page() {
    if (_py + _lh > _ph || _page_count == 0) {
        if (_page_count > 0) {
            fl_pop_clip();

            if (_printer->end_page() != 0) {
                throw "error: couldn't end current page";
            }
        }

        if (_printer->begin_page() != 0) {
            throw "error: couldn't create new page!";
        }

        if (_printer->printable_rect(&_pw, &_ph) != 0) {
            throw "error: couldn't retrieve page size!";
        }

        fl_font(_font, _fontsize);
        fl_color(FL_BLACK);
        fl_line_style(FL_SOLID, 1);
        fl_push_clip(0, 0, _pw, _ph);

        if (_border == false) {
            _px = 0;
            _py = 0;
        }
        else {
            fl_rect(0, 0, _pw, _ph);
            measure_lw_lh("M");
            
            _px  = _lw;
            _py  = _lh;
            _pw -= _lw * 2;
            _ph -= _lh * 2;
        }

        _page_count++;
    }
}

//------------------------------------------------------------------------------
void PrintText::measure_lw_lh(const std::string& text) {
    _lw = _lh = 0;
    fl_measure(text.c_str(), _lw, _lh, 0);
}

//------------------------------------------------------------------------------
std::string PrintText::print(const char* text, unsigned replace_tab_with_space) {
    return print(util::split_string(text, "\n"), replace_tab_with_space);
}

//------------------------------------------------------------------------------
std::string PrintText::print(const std::string& text, unsigned replace_tab_with_space) {
    return print(util::split_string(text.c_str(), "\n"), replace_tab_with_space);
}

//------------------------------------------------------------------------------
std::string PrintText::print(const StringVector& lines, unsigned replace_tab_with_space) {
    std::string res;
    std::string tab;

    while (replace_tab_with_space > 0 && replace_tab_with_space <= 16) {
        tab += " ";
        replace_tab_with_space--;
    }
    
    try {
        auto wc = WaitCursor();

        res = start();

        if (res == "") {
            for (auto& line : lines) {
                if (tab != "") {
                    auto l = line;
                    util::replace_string(l, "\t", "    ");
                    print_line(l == "" ? " " : l);
                }
                else {
                    print_line(line == "" ? " " : line);
                }
            }

            res = stop();
        }
    }
    catch (const char* ex) {
        res = ex;
    }
    catch (...) {
        res = "error: unknown exception!";
    }

    return res;
}

//------------------------------------------------------------------------------
void PrintText::print_line(const std::string& line) {
    _line_count++;
    check_for_new_page();

    if (_line_num > 0) {
        auto num = util::format("%*d: ", _line_num, _line_count);
        measure_lw_lh(num);
        fl_draw(num.c_str(), _px, _py, _pw, _lh, _align, nullptr, 0);
        _nw = _lw;
    }

    measure_lw_lh(line);

    if (_wrap == true && _lw > _pw - _nw) {
        print_wrapped_line(line);
    }
    else {
        fl_draw(line.c_str(), _px + _nw, _py, _pw - _nw, _lh, _align, nullptr, 0);
        _py += _lh;
    }
}

//------------------------------------------------------------------------------
void PrintText::print_wrapped_line(const std::string& line) {
    auto p1 = line.c_str();
    auto s1 = std::string();
    auto s2 = std::string();

    while (*p1 != 0) {
        auto cl = fl_wcwidth(p1);

        if (cl > 1) { // Add utf8 character.
            for (auto f = 0; f < cl && *p1 != 0; f++) {
                s1 += *p1;
                p1++;
            }
        }
        else {
            s1 += *p1;
            p1++;
        }

        auto c = s1.back();

        if (c == ' ' || c == '\t' || c == ',' || c == ';' || c == '.') { // Save word break.
            s2 = s1;
        }

        measure_lw_lh(s1);

        if (_lw >= _pw - _nw) {
            check_for_new_page();

            if (s2 != "") { // Last word delimiter string, s1 will contain the leftover.
                fl_draw(s2.c_str(), _px + _nw, _py, _pw - _nw, _lh, _align, nullptr, 0);
                s1 = (s2.length() < s1.length()) ? s1.substr(s2.length()) : "";
                s2 = "";
            }
            else {
                std::string s;

                if (s1.length() > 1) { // Remove last char to be sure that it will be inside area.
                    s  = s1.substr(s1.length() - 1);
                    s1 = s1.substr(0, s1.length() - 1);
                }

                fl_draw(s1.c_str(), _px + _nw, _py, _pw - _nw, _lh, _align, nullptr, 0);
                s1 = s;
            }

            _py += _lh;
        }
    }

    if (s1 != "") { // Leftover string.
        check_for_new_page();
        fl_draw(s1.c_str(), _px + _nw, _py, _pw - _nw, _lh, _align, nullptr, 0);
        _py += _lh;
    }
}

//------------------------------------------------------------------------------
// Create printer object.
//
std::string PrintText::start() {
    if ((_file = fl_fopen(_filename.c_str(), "wb")) == nullptr) {
        return "error: could not open file!";
    }

    _lh         = 0;
    _line_count = 0;
    _lw         = 0;
    _nw         = 0;
    _page_count = 0;
    _ph         = 0;
    _pw         = 0;
    _px         = 0;
    _py         = 0;
    _printer    = new Fl_PostScript_File_Device();

    _printer->begin_job(_file, 0, _page_format, _page_layout);
    return "";
}

//------------------------------------------------------------------------------
// End job and delete printer.
//
std::string PrintText::stop() {
    std::string res = "";

    if (_printer != nullptr) {
        if (_page_count > 0) {
            fl_pop_clip();
            
            if (_printer->end_page() != 0) {
                res = "error: could not end page!";
            }
        }

        _printer->end_job();
        delete _printer;
        fclose(_file);

        _file    = nullptr;
        _printer = nullptr;
    }

    return res;
}

} // flw

// MKALGAM_OFF
