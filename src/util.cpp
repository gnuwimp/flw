// Copyright 2016 - 2022 gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#include "util.h"
#include <errno.h>
#include <math.h>
#include <stdarg.h>
#include <sys/timeb.h>
#include <time.h>
#include <unistd.h>
#include <FL/Fl_Window.H>
#include <FL/Fl.H>
#include <FL/Fl_File_Chooser.H>
#include <FL/Fl_Menu_.H>
#include <FL/fl_ask.H>

#ifdef FLW_USE_PNG
    #include <FL/Fl_PNG_Image.H>
    #include <FL/fl_draw.H>
#endif

#ifdef _WIN32
    #include <windows.h>
#endif

// MKALGAM_ON

namespace flw {
    int         PREF_FIXED_FONT     = FL_COURIER;
    std::string PREF_FIXED_FONTNAME = "FL_COURIER";
    int         PREF_FIXED_FONTSIZE = 14;
    int         PREF_FONT           = FL_HELVETICA;
    int         PREF_FONTSIZE       = 14;
    std::string PREF_FONTNAME       = "FL_HELVETICA";

    //--------------------------------------------------------------------------
    static Fl_Menu_Item* _util_menu_item(Fl_Menu_* menu, const char* text) {
        assert(menu && text);

        auto item = menu->find_item(text);

#ifdef DEBUG
        if (item == nullptr) {
            fprintf(stderr, "error: cant find menu item <%s>\n", text);
        }
#endif

        return (Fl_Menu_Item*) item;
    }
}

//------------------------------------------------------------------------------
size_t flw::util::add_string(StringVector& in, size_t max_size, std::string string) {
    for (auto it = in.begin(); it != in.end(); ++it) {
        if (*it == string) {
            in.erase(it);
            break;
        }
    }

    in.push_back(string);

    while (in.size() > max_size) {
        in.erase(in.begin());
    }

    return in.size();
}

//------------------------------------------------------------------------------
char* flw::util::allocate(size_t size, bool terminate) {
    auto res = (char*) calloc(size + 1, 1);

    if (res == nullptr && terminate == true) {
        fl_alert("error: failed to allocate memory with size %lld\nI'm going to quit now!", (long long int) size);
        exit(1);
    }

    return res;
}

//------------------------------------------------------------------------------
void flw::util::center_window(Fl_Window* window, Fl_Window* parent) {
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
int flw::util::count_decimals(double number) {
    number = fabs(number);

    int    res     = 0;
    int    len     = 0;
    char*  end     = 0;
    double inumber = (int64_t) number;
    double fnumber = number - inumber;
    char   buffer[100];

    if (number > 999999999999999) {
        snprintf(buffer, 100, "%.1f", fnumber);
    }
    else if (number > 9999999999999) {
        snprintf(buffer, 100, "%.2f", fnumber);
    }
    else if (number > 999999999999) {
        snprintf(buffer, 100, "%.3f", fnumber);
    }
    else if (number > 99999999999) {
        snprintf(buffer, 100, "%.4f", fnumber);
    }
    else if (number > 9999999999) {
        snprintf(buffer, 100, "%.5f", fnumber);
    }
    else if (number > 999999999) {
        snprintf(buffer, 100, "%.6f", fnumber);
    }
    else if (number > 99999999) {
        snprintf(buffer, 100, "%.7f", fnumber);
    }
    else if (number > 9999999) {
        snprintf(buffer, 100, "%.8f", fnumber);
    }
    else {
        snprintf(buffer, 100, "%.9f", fnumber);
    }

    len = strlen(buffer);
    end = buffer + len - 1;

    while (*end == '0') {
        *end = 0;
        end--;
    }

    res = strlen(buffer) - 2;
    return res;
}

//------------------------------------------------------------------------------
// Set label font properties for widget
// If widget is an group widget set also the font for child widgets (recursive)
//
void flw::util::labelfont(Fl_Widget* widget) {
    assert(widget);

    widget->labelfont(flw::PREF_FONT);
    widget->labelsize(flw::PREF_FONTSIZE);

    auto group = widget->as_group();

    if (group != nullptr) {
        for (auto f = 0; f < group->children(); f++) {
            util::labelfont(group->child(f));
        }
    }
}

//------------------------------------------------------------------------------
std::string flw::util::fix_menu_string(std::string in) {
    std::string res = in;

    util::replace(res, "\\", "\\\\");
    util::replace(res, "_", "\\_");
    util::replace(res, "/", "\\/");
    util::replace(res, "&", "&&");

    return res;
}

//------------------------------------------------------------------------------
std::string flw::util::format(const char* format, ...) {
    assert(format);

    std::string res;
    int         l = 100;
    int         n = 0;
    char*       s = (char*) calloc(100 + 1, 1);

    if (s == nullptr) {
        return res;
    }

    while (true) {
        va_list args;

        va_start(args, format);
        n = vsnprintf(s, l, format, args);
        va_end(args);

        if (n == -1) {
            s[0] = 0;
            break;
        }
        else if (n >= l) {
            l = n + 1;
            char* tmp = (char*) realloc(s, l);

            if (tmp == nullptr) {
                free(s);
                return res;
            }

            s = tmp;
        }
        else {
            break;
        }
    }

    res = s;
    free(s);
    return res;
}

//------------------------------------------------------------------------------
std::string flw::util::format_double(double number, int decimals, char sep) {
    char res[100];

    *res = 0;

    if (decimals < 0) {
        decimals = util::count_decimals(number);
    }

    if (decimals == 0) {
        return util::format_int((int64_t) number, sep);
    }

    if (fabs(number) < 9223372036854775807.0) {
        char fr_str[100];
        auto int_num    = (int64_t) fabs(number);
        auto double_num = (double) (fabs(number) - int_num);
        auto int_str    = util::format_int(int_num, sep);
        auto len        = snprintf(fr_str, 99, "%.*f", decimals, double_num);

        if (len > 0 && len < 100) {
            if (number < 0.0) {
                res[0] = '-';
                res[1] = 0;
            }

            strncat(res, int_str.c_str(), 99);
            strncat(res, fr_str + 1, 99);
        }
    }

    return res;
}

//------------------------------------------------------------------------------
std::string flw::util::format_int(int64_t number, char sep) {
    auto pos = 0;
    char tmp1[100];
    char tmp2[100];

    if (sep < 32) {
        sep = 32;
    }

    snprintf(tmp1, 100, "%lld", (long long int) llabs(number));
    auto len = (int) strlen(tmp1);

    for (auto f = len - 1, c = 0; f >= 0 && pos < 100; f--, c++) {
        if ((c % 3) == 0 && c > 0) {
            tmp2[pos] = sep;
            pos++;
        }

        tmp2[pos] = tmp1[f];
        pos++;
    }

    if (number < 0) {
        tmp2[pos] = '-';
        pos++;
    }

    tmp2[pos] = 0;
    len = strlen(tmp2);

    for (auto f = 0; f < len / 2; f++) {
        auto c = tmp2[f];
        tmp2[f] = tmp2[len - f - 1];
        tmp2[len - f - 1] = c;
    }

    return tmp2;
}

//------------------------------------------------------------------------------
size_t flw::util::insert_string(StringVector& in, size_t max_size, std::string string) {
    for (auto it = in.begin(); it != in.end(); ++it) {
        if (*it == string) {
            in.erase(it);
            break;
        }
    }

    in.insert(in.begin(), string);

    while (in.size() > max_size) {
        in.pop_back();
    }

    return (int) in.size();
}

//------------------------------------------------------------------------------
flw::Buf flw::util::load_file(std::string filename, bool alert) {
    auto stat = flw::Stat(filename);

    if (stat.mode != 2) {
        if (alert == true) {
            fl_alert("error: file %s is missing or not an file", filename.c_str());
        }

        return Buf();
    }

    auto file = fopen(filename.c_str(), "rb");

    if (file == nullptr) {
        if (alert == true) {
            fl_alert("error: can't open %s", filename.c_str());
        }

        return Buf();
    }

    auto buf  = Buf(stat.size);
    auto read = fread(buf.p, 1, (size_t) stat.size, file);

    fclose(file);

    if (read != (size_t) stat.size) {
        if (alert == true) {
            fl_alert("error: failed to read %s", filename.c_str());
        }

        free(buf.p);
        return Buf();
    }

    return buf;
}

//------------------------------------------------------------------------------
void flw::util::menu_item_enable(Fl_Menu_* menu, const char* text, bool value) {
    assert(menu && text);

    auto item = flw::_util_menu_item(menu, text);

    if (item == nullptr) {
        return;
    }

    if (value == true) {
        item->activate();
    }
    else {
        item->deactivate();
    }
}

//------------------------------------------------------------------------------
Fl_Menu_Item* flw::util::menu_item_get(Fl_Menu_* menu, const char* text) {
    assert(menu && text);

    return flw::_util_menu_item(menu, text);
}

//------------------------------------------------------------------------------
void flw::util::menu_item_set(Fl_Menu_* menu, const char* text, bool value) {
    assert(menu && text);

    auto item = flw::_util_menu_item(menu, text);

    if (item == nullptr) {
        return;
    }

    if (value == true) {
        item->set();
    }
    else {
        item->clear();
    }
}

//------------------------------------------------------------------------------
void flw::util::menu_item_set_only(Fl_Menu_* menu, const char* text) {
    assert(menu && text);

    auto item = flw::_util_menu_item(menu, text);

    if (item == nullptr) {
        return;
    }

    menu->setonly(item);
}

//------------------------------------------------------------------------------
bool flw::util::menu_item_value(Fl_Menu_* menu, const char* text) {
    assert(menu && text);

    auto item = flw::_util_menu_item(menu, text);

    if (item == nullptr) {
        return false;
    }

    return item->value();
}

//------------------------------------------------------------------------------
// Must be compiled with FLW_USE_PNG flag and linked with fltk images (fltk-config --ldflags --use-images)
// If filename is empty you will be asked for the name
//
void flw::util::png_save(std::string opt_name, Fl_Window* window, int X, int Y, int W, int H) {
#ifdef FLW_USE_PNG
#if FL_MINOR_VERSION == 4
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
            else if (ret == -1) {
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
    fl_alert("error: does not work with fltk 1.3");
#endif
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
void flw::util::print(Fl_Widget* widget, bool tab) {
    assert(widget);

    printf("%s%-*s| x=%4d, y=%4d, w=%4d, h=%4d\n", tab ? "    " : "", tab ? 16 : 20, widget->label() ? widget->label() : "NO_LABEL",  widget->x(),  widget->y(),  widget->w(),  widget->h());
    fflush(stdout);
}

//------------------------------------------------------------------------------
void flw::util::print(Fl_Group* group) {
    assert(group);
    puts("");
    util::print((Fl_Widget*) group);

    for (int f = 0; f < group->children(); f++) {
        util::print(group->child(f), true);
    }
}

//------------------------------------------------------------------------------
// Replace string and return number of replacements or -1 for error
//
int flw::util::replace(std::string& string, std::string find, std::string replace) {
    auto index = (size_t) 0;
    auto count = (size_t) 0;

    try {
        if (string.size() > 0 && find.size() > 0) {
            while (true) {
                index = string.find(find, index);

                if (index == std::string::npos) {
                    break;
                }

                string.replace(index, find.size(), replace);
                count++;
                index += replace.size();
            }

            string.shrink_to_fit();
        }

        return (int) count;
    }
    catch(...) {
        return -1;
    }
}

//------------------------------------------------------------------------------
bool flw::util::save_file(std::string filename, const void* data, size_t size, bool alert) {
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

//------------------------------------------------------------------------------
// Split string and return an vector with splitted strings
//
flw::StringVector flw::util::split(const std::string& string, std::string split) {
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
bool flw::util::test(bool val, int line, const char* func) {
    if (val == false) {
        fprintf(stderr, "error: test failed at line %d in %s\n", line, func);
        fflush(stderr);
        return false;
    }

    return true;
}

//------------------------------------------------------------------------------
bool flw::util::test(const char* ref, const char* val, int line, const char* func) {
    if (ref == nullptr && val == nullptr) {
        return true;
    }
    else if (ref == nullptr || val == nullptr || strcmp(ref, val) != 0) {
        fprintf(stderr, "error: test failed '%s' != '%s' at line %d in %s\n", ref ? ref : "NULL", val ? val : "NULL", line, func);
        fflush(stderr);
        return false;
    }

    return true;
}

//------------------------------------------------------------------------------
bool flw::util::test(int64_t ref, int64_t val, int line, const char* func) {
    if (ref != val) {
        fprintf(stderr, "error: test failed '%lld' != '%lld' at line %d in %s\n", (long long int) ref, (long long int) val, line, func);
        fflush(stderr);
        return false;
    }

    return true;
}

//------------------------------------------------------------------------------
bool flw::util::test(double ref, double val, double diff, int line, const char* func) {
    if (fabs(ref - val) > diff) {
        fprintf(stderr, "error: test failed '%f' != '%f' at line %d in %s\n", ref, val, line, func);
        fflush(stderr);
        return false;
    }

    return true;
}

//------------------------------------------------------------------------------
// Return time as seconds since 1970
//
double flw::util::time() {
    auto sec   = 0.0;
    auto milli = 0.0;

#ifdef _WIN32
    struct __timeb64 timeVal;

    _ftime64(&timeVal);
    sec   = (double) timeVal.time;
    milli = (double) timeVal.millitm;
    return sec + (milli / 1000.0);
#else
    struct timeb timeVal;

    ftime(&timeVal);
    sec   = (double) timeVal.time;
    milli = (double) timeVal.millitm;
    return sec + (milli / 1000.0);
#endif
}

//------------------------------------------------------------------------------
// Return time stamp
//
int64_t flw::util::time_micro() {
#if defined(_WIN32)
    LARGE_INTEGER StartingTime;
    LARGE_INTEGER Frequency;

    QueryPerformanceFrequency(&Frequency);
    QueryPerformanceCounter(&StartingTime);

    StartingTime.QuadPart *= 1000000;
    StartingTime.QuadPart /= Frequency.QuadPart;
    return StartingTime.QuadPart;
#else
    timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return t.tv_sec * 1000000 + t.tv_nsec / 1000;
#endif
}

//------------------------------------------------------------------------------
// Return time stamp
//
int32_t flw::util::time_milli() {
    return (int) (util::time_micro() / 1000);
}

//------------------------------------------------------------------------------
void flw::util::time_sleep(int milli) {
#ifdef _WIN32
    Sleep(milli);
#else
    usleep(milli * 1000);
#endif
}

//------------------------------------------------------------------------------
double flw::util::to_double(std::string string, double def) {
    try {
        return std::stod(string.c_str(), 0);
    }
    catch (...) {
        return def;
    }
}

//------------------------------------------------------------------------------
// Convert double numbers in string to actual numbers
// Return number of set doubles
//
int flw::util::to_doubles(std::string string, double numbers[], size_t size) {
    auto end = (char*) nullptr;
    auto in  = string.c_str();
    auto f   = (size_t) 0;

    errno = 0;

    for (; f < size; f++) {
        numbers[f] = strtod(in, &end);

        if (errno != 0 || in == end) {
            return f;
        }

        in = end;
    }

    return f;
}

//------------------------------------------------------------------------------
int64_t flw::util::to_int(std::string string, int64_t def) {
    try {
        return std::stoll(string.c_str(), 0, 0);
    }
    catch (...) {
        return def;
    }
}

//------------------------------------------------------------------------------
// Return number of set integers
//
int flw::util::to_ints(std::string string, int64_t numbers[], size_t size) {
    auto end = (char*) nullptr;
    auto in  = string.c_str();
    auto f   = (size_t) 0;

    errno = 0;

    for (; f < size; f++) {
        numbers[f] = strtoll(in, &end, 10);

        if (errno != 0 || in == end) {
            return f;
        }

        in = end;
    }

    return f;
}

//------------------------------------------------------------------------------
// Returns new converted buffer if it does contain \r
// Otherwise it returns nullptr
//
char* flw::util::win_to_unix(const char* string) {
    auto r = false;
    auto b = string;

    while (*b != 0) {
        if (*b++ == '\r') {
            r = true;
            break;
        }
    }

    if (r == false) {
        return nullptr;
    }

    auto len = strlen(string);
    auto res = util::allocate(len);
    auto pos = 0;

    b = string;

    while (*b != 0) {
        if (*b != '\r') {
            res[pos++] = *b;
        }

        b++;
    }

    return res;
}

//------------------------------------------------------------------------------
void* flw::util::zero_memory(char* string) {
    assert(string);

    if (string == nullptr) {
        return nullptr;
    }

    return util::zero_memory(string, strlen(string));
}

//------------------------------------------------------------------------------
void* flw::util::zero_memory(void* mem, size_t size) {
    if (mem == nullptr || size == 0) {
        return mem;
    }

#ifdef _WIN32
    RtlSecureZeroMemory(mem, size);
#else
    auto p = (volatile unsigned char*) mem;

    while (size--) {
        *p = 0;
        p++;
    }
#endif

    return mem;
}

//------------------------------------------------------------------------------
void* flw::util::zero_memory(std::string& string) {
    return util::zero_memory((char*) string.data());
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
flw::Buf::Buf(size_t size) {
    p = util::allocate(size, 1);
    s = size;
}

//------------------------------------------------------------------------------
flw::Buf::Buf(const char* buffer, size_t size) {
    p = util::allocate(size, 1);
    s = size;

    assert(buffer != p);
    memcpy(p, buffer, s);
}

//------------------------------------------------------------------------------
flw::Buf::Buf(const Buf& other) {
    p = nullptr;
    s = 0;

    if (other.p != nullptr && other.s > 0) {
        s = other.s;
        p = util::allocate(s);

        assert(other.p != p);
        memcpy(p, other.p, s);
    }
}

//------------------------------------------------------------------------------
flw::Buf::Buf(Buf&& other) {
    s       = other.s;
    p       = other.p;
    other.p = nullptr;
}

//------------------------------------------------------------------------------
flw::Buf& flw::Buf::operator=(const Buf& other) {
    assert(other.p != p);

    free(p);
    p = nullptr;
    s = 0;

    if (other.p != nullptr && other.s > 0) {
        s = other.s;
        p = util::allocate(s);

        assert(other.p != p);
        memcpy(p, other.p, s);
    }

    return *this;
}

//------------------------------------------------------------------------------
flw::Buf& flw::Buf::operator=(Buf&& other) {
    free(p);

    s       = other.s;
    p       = other.p;
    other.p = nullptr;

    return *this;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
flw::Stat::Stat(std::string filename) {
    size  = 0;
    mtime = 0;
    mode  = 0;

#ifdef _WIN32
    wchar_t wbuffer[1025];
    struct __stat64 st;

    while (filename.empty() == false && (filename.back() == '\\' || filename.back() == '/')) {
        filename.pop_back();
    }

    fl_utf8towc	(filename.c_str(), filename.length(), wbuffer, 1024);

    if (_wstat64(wbuffer, &st) == 0) {
        size  = st.st_size;
        mtime = st.st_mtime;

        if (S_ISDIR(st.st_mode)) {
            mode = 1;
        }
        else if (S_ISREG(st.st_mode)) {
            mode = 2;
        }
        else {
            mode = 3;
        }
    }
#else
    struct stat st;

    if (stat(filename.c_str(), &st) == 0) {
        size  = st.st_size;
        mtime = st.st_mtime;

        if (S_ISDIR(st.st_mode)) {
            mode = 1;
        }
        else if (S_ISREG(st.st_mode)) {
            mode = 2;
        }
        else {
            mode = 3;
        }
    }
#endif
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

namespace flw {
    namespace json {
        #define _JSON_RETURN(X,Y)           return (size_t) X - (size_t) Y;
        #define _JSON_RETURN_POS(X)         return X.pos;
        #define _JSON_CHECK_SEPARATOR(X)    if (X > 32 && X != ',' && X != ':' && X != '}' && X != ']' && X != '{' && X != '[') return (size_t) text - (size_t) json;

        static const char* _TYPE_NAMES[] = { "NA", "OBJECT", "END_OBJECT", "ARRAY","END_ARRAY", "STRING", "NUMBER", "BOOL", "NIL", "COLON", "COMMA", "ARRAY_NL" };

        //----------------------------------------------------------------------
        struct Token {
            TYPE                        type;
            int                         depth;
            std::string                 value;
            size_t                      pos;
                                        Token()
                                            { type = TYPE::NA; pos = 0; depth = 0; }
                                        Token(TYPE type, const std::string& value = "", size_t pos = 0, size_t depth = 0)
                                            { this->type = type; this->value = value; this->pos = pos; this->depth = depth; }
            bool                        is_end() const
                                            { return type == TYPE::END_ARRAY || type == TYPE::END_OBJECT; }
            bool                        is_start() const
                                            { return type == TYPE::ARRAY || type == TYPE::OBJECT; }
        };

        typedef std::vector<Token>      TokenVector;
        void                            print(const TokenVector& tokens);
        size_t                          tokenize(std::string text, TokenVector& tokens);
        size_t                          tokenize(const char* json, size_t len, TokenVector& tokens);

        //----------------------------------------------------------------------
        static bool _convert_num(const char* s, double& d) {
            errno = 0;
            d = strtod(s, nullptr);
            return errno != ERANGE;
        }

        //----------------------------------------------------------------------
        static void _count_dot_minus_plus_e(const std::string& s, int& dot, int& minus, int& plus, int& E) {
            dot = 0; minus = 0; plus = 0; E = 0;
            for (auto c : s) {
                dot   += (c =='.');
                minus += (c =='-');
                plus  += (c =='+');
                E     += (c =='e');
                E     += (c =='E');
            }
        }

        //----------------------------------------------------------------------
        static std::string _remove_last(StringVector& v) {
            if (v.size() > 0) {
                auto res = v.back();
                v.pop_back();
                return res;
            }

            return "";
        }

        // ----------------------------------------------------------------------
        // ----------------------------------------------------------------------
        // ----------------------------------------------------------------------

        // ----------------------------------------------------------------------
        void Node::print() const {
            printf("%5u| %5u| %2d| %*s %-10s <%s>", (unsigned) index, (unsigned) textpos, depth, depth * 4, "", _TYPE_NAMES[(int) type], name.c_str());
            if (type == TYPE::STRING || type == TYPE::NUMBER || type == TYPE::BOOL) printf(" => <%s>\n", value.c_str());
            else printf("\n");
            fflush(stdout);
        }

        //----------------------------------------------------------------------
        //----------------------------------------------------------------------
        //----------------------------------------------------------------------

        //----------------------------------------------------------------------
        std::string escape_string(const std::string& string) {
            std::string res;

            for (unsigned char c : string) {
                if (c == 9) res += "\\t";
                else if (c == 10) res += "\\n";
                else if (c == 13) res += "\\r";
                else if (c == 8) res += "\\b";
                else if (c == 14) res += "\\f";
                else if (c == 34) res += "\\\"";
                else if (c == 92) res += "\\\\";
                else res += c;
            }

            return res;
        }

        //----------------------------------------------------------------------
        NodeVector find_children(const NodeVector& nodes, const Node& start, bool grandchildren) {
            auto res = NodeVector();
            if (start.index + 1 >= nodes.size()) return res;

            for (size_t f = start.index + 1; f < nodes.size(); f++) {
                auto& node = nodes[f];

                if (node.depth <= start.depth) break;
                else if (grandchildren == true) res.push_back(node);
                else if (node.depth == start.depth + 1 && node.is_end() == false) res.push_back(node);
            }
            return res;
        }

        //----------------------------------------------------------------------
        NodeVector find_nodes(const NodeVector& nodes, std::string name, TYPE type) {
            auto res = NodeVector();

            for (size_t f = 0; f < nodes.size(); f++) {
                auto& node = nodes[f];

                if (name == "") {
                    if (type == TYPE::NA) res.push_back(node);
                    else if (node.type == type) res.push_back(node);
                }
                else if (node.name == name) {
                    if (type == TYPE::NA) res.push_back(node);
                    else if (node.type == type) res.push_back(node);
                }
            }
            return res;
        }

        //----------------------------------------------------------------------
        NodeVector find_siblings(const NodeVector& nodes, const Node& start) {
            auto res = NodeVector();
            if (start.index >= nodes.size()) return res;

            for (size_t f = start.index; f < nodes.size(); f++) {
                auto& node = nodes[f];

                if (node.depth < start.depth) break;
                else if (node.depth == start.depth && node.is_end() == false) res.push_back(node);
            }
            return res;
        }

        //----------------------------------------------------------------------
        size_t parse(const char* json, NodeVector& nodes, bool ignore_trailing_comma) {
            auto tokens      = json::TokenVector();
            auto len         = strlen(json);
            auto err         = json::tokenize(json, len, tokens);
            auto size        = tokens.size();
            auto obj         = 0;
            auto arr         = 0;
            auto num         = 0.0;
            auto arr_name    = std::vector<std::string>();
            auto obj_name    = std::vector<std::string>();
            auto containers  = std::vector<std::string>();
            auto const DUMMY = json::Token();

            nodes.clear();
            // print(tokens);

            if (err != (size_t) -1) return err;
            else if (size < 2) return size;

            for (size_t f = 0; f < size; f++) {
                auto& t  = tokens[f];
                auto& T0 = (f > 0) ? tokens[f - 1] : DUMMY;
                auto& T1 = (f < size - 1) ? tokens[f + 1] : DUMMY;
                auto& T2 = (f < size - 2) ? tokens[f + 2] : DUMMY;

                if ((T0.type == TYPE::STRING || T0.type == TYPE::NUMBER || T0.type == TYPE::NIL || T0.type == TYPE::BOOL) &&
                    (t.type != TYPE::COMMA && t.type != TYPE::END_ARRAY && t.type != TYPE::END_OBJECT)) {
                    _JSON_RETURN_POS(t)
                }
                else if (obj == 0 && arr == 0 && nodes.size() > 0) {
                    _JSON_RETURN_POS(t)
                }
                else if (obj == 0 && arr == 0 && t.type != TYPE::OBJECT && t.type != TYPE::ARRAY) {
                    _JSON_RETURN_POS(t)
                }
                else if (t.type == TYPE::COMMA && T0.type == TYPE::COMMA) {
                    _JSON_RETURN_POS(t)
                }
                else if (t.type == TYPE::COMMA && T0.type == TYPE::ARRAY) {
                    _JSON_RETURN_POS(t)
                }
                else if (t.type == TYPE::COLON && T0.type != TYPE::STRING) {
                    _JSON_RETURN_POS(t)
                }
                else if (t.type == TYPE::END_OBJECT) {
                    if (ignore_trailing_comma == false && T0.type == TYPE::COMMA) _JSON_RETURN_POS(t)
                    containers.pop_back();
                    obj--;
                    if (obj < 0) _JSON_RETURN_POS(t)
                   nodes.push_back(Node(TYPE::END_OBJECT, _remove_last(obj_name), "", t.depth, t.pos));
                }
                else if (t.type == TYPE::OBJECT) {
                    if (containers.size() && containers.back() == "O") _JSON_RETURN_POS(t)
                    if (T0.is_end() == true && T0.depth == t.depth) _JSON_RETURN_POS(t)
                    containers.push_back("O");
                    obj++;
                    nodes.push_back(Node(TYPE::OBJECT, "", "", t.depth, t.pos));
                    obj_name.push_back("");
                }
                else if (t.type == TYPE::STRING && T1.type == TYPE::COLON && T2.type == TYPE::OBJECT) {
                    if (T0.is_end() == true && T0.depth == T2.depth) _JSON_RETURN_POS(t)
                    if (obj == 0) _JSON_RETURN_POS(t)
                    containers.push_back("O");
                    obj++;
                    nodes.push_back(Node(TYPE::OBJECT, t.value, "", T2.depth, t.pos));
                    obj_name.push_back(t.value);
                    f += 2;
                }
                else if (t.type == TYPE::END_ARRAY) {
                    if (ignore_trailing_comma == false && T0.type == TYPE::COMMA) _JSON_RETURN_POS(t)
                    containers.pop_back();
                    arr--;
                    if (arr < 0) _JSON_RETURN_POS(t)
                    nodes.push_back(Node(TYPE::END_ARRAY, _remove_last(arr_name), "", t.depth, t.pos));
                }
                else if (t.type == TYPE::ARRAY) {
                    if (containers.size() && containers.back() == "O") _JSON_RETURN_POS(t)
                    if (T0.is_end() == true && T0.depth == t.depth) _JSON_RETURN_POS(t)
                    containers.push_back("A");
                    arr++;
                    nodes.push_back(Node(TYPE::ARRAY, "", "", t.depth, t.pos));
                    arr_name.push_back("");
                }
                else if (t.type == TYPE::STRING && T1.type == TYPE::COLON && T2.type == TYPE::ARRAY) {
                    if (T0.is_end() == true && T0.depth == T2.depth) _JSON_RETURN_POS(t)
                    if (obj == 0) _JSON_RETURN_POS(t)
                    containers.push_back("A");
                    arr++;
                    nodes.push_back(Node(TYPE::ARRAY, t.value, "", T2.depth, t.pos));
                    arr_name.push_back(t.value);
                    f += 2;
                }
                else if (t.type == TYPE::BOOL) {
                    nodes.push_back(Node(TYPE::BOOL, "", t.value, t.depth, t.pos));
                    if (containers.size() && containers.back() == "O") _JSON_RETURN_POS(t)
                }
                else if (t.type == TYPE::STRING && T1.type == TYPE::COLON && T2.type == TYPE::BOOL) {
                    nodes.push_back(Node(TYPE::BOOL, t.value, T2.value, T2.depth, t.pos));
                    f += 2;
                    if (containers.size() && containers.back() == "A") _JSON_RETURN_POS(t)
                }
                else if (t.type == TYPE::NIL) {
                    nodes.push_back(Node(TYPE::NIL, "", "", t.depth, t.pos));
                    if (containers.size() && containers.back() == "O") _JSON_RETURN_POS(t)
                }
                else if (t.type == TYPE::STRING && T1.type == TYPE::COLON && T2.type == TYPE::NIL) {
                    nodes.push_back(Node(TYPE::NIL, t.value, "", T2.depth, t.pos));
                    f += 2;
                    if (containers.size() && containers.back() == "A") _JSON_RETURN_POS(t)
                }
                else if (t.type == TYPE::NUMBER) {
                    if (_convert_num(t.value.c_str(), num) == false) _JSON_RETURN_POS(t)
                    nodes.push_back(Node(TYPE::NUMBER, "", t.value, t.depth, t.pos));
                    if (containers.size() && containers.back() == "O") _JSON_RETURN_POS(t)
                }
                else if (t.type == TYPE::STRING && T1.type == TYPE::COLON && T2.type == TYPE::NUMBER) {
                    if (_convert_num(T2.value.c_str(), num) == false) _JSON_RETURN_POS(t)
                    nodes.push_back(Node(TYPE::NUMBER, t.value, T2.value, t.depth, t.pos));
                    f += 2;
                    if (containers.size() && containers.back() == "A") _JSON_RETURN_POS(t)
                }
                else if (t.type == TYPE::STRING && T1.type == TYPE::COLON && T2.type == TYPE::STRING) {
                    nodes.push_back(Node(TYPE::STRING, t.value, T2.value, t.depth, t.pos));
                    f += 2;
                    if (containers.size() && containers.back() == "A") _JSON_RETURN_POS(t)
                }
                else if (t.type == TYPE::STRING) {
                    nodes.push_back(Node(TYPE::STRING, "", t.value, t.depth, t.pos));
                    if (containers.size() && containers.back() == "O") _JSON_RETURN_POS(t)
                }

                if (nodes.size() > 0) nodes.back().index   = nodes.size() - 1;
                if (arr < 0 || obj < 0) _JSON_RETURN_POS(t)
            }

            if (arr != 0 || obj != 0) return len;
            return -1;
        }

        //----------------------------------------------------------------------
        size_t parse(std::string json, NodeVector& nodes, bool ignore_trailing_commas) {
            return json::parse(json.c_str(), nodes, ignore_trailing_commas);
        }

        // ----------------------------------------------------------------------
        void print(const TokenVector& tokens) {
            auto c = 0;
            printf("\n%d json::Token objects\n", (int) tokens.size());
            printf("%-5s| %7s| %5s| VALUES\n", "TOKEN", "POS", "DEPTH");
            printf("-------------------------------------------------------\n");
            for (auto& t : tokens) {
                if (t.type == TYPE::NA) ;
                else if (t.type == TYPE::STRING || t.type == TYPE::NUMBER || t.type == TYPE::BOOL) printf("%5d| %7d| %5d|%*s %-10s <%s>\n", c, (int) t.pos, t.depth, t.depth * 4, "", _TYPE_NAMES[(int) t.type], t.value.c_str());
                else printf("%5d| %7d| %5d|%*s %-10s\n", c, (int) t.pos, (int) t.depth, t.depth * 4, "", _TYPE_NAMES[(int) t.type]);
                c++;
            }
            printf("\n");
            fflush(stdout);
        }

        // ----------------------------------------------------------------------
        void print(const NodeVector& nodes) {
            auto c = 0;

            printf("\n%d json::Node objects\n", (int) nodes.size());
            printf("%6s| %6s| %6s| %6s| %s\n", "COUNT", "INDEX", "POS", "DEPTH", "VALUES");
            printf("-------------------------------------------------------\n");
            for (auto& node : nodes) {
                printf("%6d| %6u| %6u| %6d|%*s  %-10s <%s>", c, (unsigned) node.index, (unsigned) node.textpos, node.depth, node.depth * 4, "", _TYPE_NAMES[(int) node.type], node.name.c_str());
                if (node.type == TYPE::STRING || node.type == TYPE::NUMBER || node.type == TYPE::BOOL) printf(" => <%s>", node.value.c_str());
                printf("\n");
                c++;
            }
            fflush(stdout);
        }

       //----------------------------------------------------------------------
        size_t tokenize(const char* json, size_t len, TokenVector& tokens) {
            tokens.clear();

            auto text  = json;
            auto end   = text + len;
            auto value = std::string();
            auto num   = std::string();
            auto depth = 0;
            auto DUMMY = Token();

            num.reserve(50);
            value.reserve(200);

            while (text < end) {
                auto  c  = (unsigned char) *text;
                // auto& T0 = (tokens.size() > 0) ? tokens.back() : DUMMY;

                if (c < 33) {
                    text++;
                }
                else if (c == ',') {
                    tokens.push_back(json::Token(TYPE::COMMA, "", (size_t) text - (size_t) json, depth));
                    text++;
                }
                else if (c == ':') {
                    tokens.push_back(json::Token(TYPE::COLON, "", (size_t) text - (size_t) json, depth));
                    text++;
                }
                else if (c == '{') {
                    tokens.push_back(json::Token(TYPE::OBJECT, "", (size_t) text - (size_t) json, depth));
                    depth++;
                    text++;
                }
                else if (c == '}') {
                    depth--;
                    tokens.push_back(json::Token(TYPE::END_OBJECT, "", (size_t) text - (size_t) json, depth));
                    text++;
                }
                else if (c == '[') {
                    tokens.push_back(json::Token(TYPE::ARRAY, "", (size_t) text - (size_t) json, depth));
                    depth++;
                    text++;
                }
                else if (c == ']') {
                    depth--;
                    tokens.push_back(json::Token(TYPE::END_ARRAY, "", (size_t) text - (size_t) json, depth));
                    text++;
                }
                else if (c == 't' && strncmp(text, "true", 4) == 0) {
                    tokens.push_back(json::Token(TYPE::BOOL, "true", (size_t) text - (size_t) json, depth));
                    text += 4;
                    c = *text;
                    _JSON_CHECK_SEPARATOR(c)
                }
                else if (c == 'f' && strncmp(text, "false", 5) == 0) {
                    tokens.push_back(json::Token(TYPE::BOOL, "false", (size_t) text - (size_t) json, depth));
                    text += 5;
                    c = *text;
                    _JSON_CHECK_SEPARATOR(c)
                }
                else if (c == 'n' && strncmp(text, "null", 4) == 0) {
                    tokens.push_back(json::Token(TYPE::NIL, "", (size_t) text - (size_t) json, depth));
                    text += 4;
                    c = *text;
                    _JSON_CHECK_SEPARATOR(c)
                }
                else if (c == '"') {
                    auto p = (unsigned char) *text;
                    auto start = text;
                    text++;
                    auto c = (unsigned char) *text;
                    value = "";

                    while (text < end) {
                        if (c < 32) _JSON_RETURN(text, json)
                        else if (p == '\\' && c == '"') {
                        }
                        else if (c == '"') {
                            tokens.push_back(json::Token(TYPE::STRING, value, (size_t) start - (size_t) json, depth));
                            text++;
                            break;
                        }
                        value += c;
                        p = c;
                        text++;
                        c = *text;
                    }
                    c = *text;
                    if (text == end) _JSON_RETURN(text, json)
                    _JSON_CHECK_SEPARATOR(c)
                }
                else if (c == '-' || c == '.' || (c >= '0' && c <= '9')) {
                    auto start = text;
                    std::string n1, n2;
                    int dot, minus, plus, E;

                    while (c == '-' || c == '.' || (c >= '0' && c <= '9')) {
                        n1 += c; text++; c = *text;
                    }
                    _count_dot_minus_plus_e(n1, dot, minus, plus, E);
                    if (dot > 1 || minus > 1) _JSON_RETURN(start, json)
                    if (n1[0] == '.' || n1.back() == '.') _JSON_RETURN(start, json)
                    if (n1[0] == '-' && n1[1] == '.') _JSON_RETURN(start, json)
                    if (n1[0] == '-' && n1[1] == '0' && n1.length() > 2 && n1[2] != '.') _JSON_RETURN(start, json)
                    if (n1 == "-") _JSON_RETURN(start, json)
                    if (minus > 0 && n1[0] != '-') _JSON_RETURN(start, json)

                    while (c == 'e' || c == 'E' || c == '-' || c == '+' || (c >= '0' && c <= '9')) {
                        n2 += c; text++; c = *text;
                    }
                    if (n2.empty() == false) {
                        if (n2.length() == 1) _JSON_RETURN(start, json)
                        _count_dot_minus_plus_e(n2, dot, minus, plus, E);
                        if (plus + minus > 1 || E > 1) _JSON_RETURN(start, json)
                        if (plus > 0 && n2.back() == '+') _JSON_RETURN(start, json)
                        if (plus > 0 && n2[1] != '+') _JSON_RETURN(start, json)
                        if (minus > 0 && n2.back() == '-') _JSON_RETURN(start, json)
                        if (minus > 0 && n2[1] != '-') _JSON_RETURN(start, json)
                    }
                    else if (n1[0] == '0' && n1.length() > 1 && n1[1] != '.') _JSON_RETURN(start, json)

                    tokens.push_back(json::Token(TYPE::NUMBER, n1 + n2, (size_t) start - (size_t) json, depth));
                    _JSON_CHECK_SEPARATOR(c)
                }
                else {
                    _JSON_RETURN(text, json);
                }
            }

            return -1;
        }

        //----------------------------------------------------------------------
        size_t tokenize(std::string text, TokenVector& tokens) {
            return json::tokenize(text.c_str(), text.length(), tokens);
        }

        //----------------------------------------------------------------------
        std::string tostring(const NodeVector& nodes) {
            if (nodes.size() < 2) return "";

            auto res    = std::string();
            auto DUMMY  = Node();
            auto last   = nodes.size() - 1;
            auto indent = std::string();
            auto arr    = TYPE::NA;

            res.reserve(20 * nodes.size());

            for (size_t f = 0; f <= last; f++) {
                auto& node = nodes[f];
                auto& next = (f < last) ? nodes[f + 1] : DUMMY;
                auto  nl   = std::string("\n");
                auto  nl2  = std::string("");

                if (node.is_data() && next.is_end() == false) {
                    nl  = ",\n";
                    nl2 = ",";
                }
                else if (node.is_end() && (next.is_start() == true || next.is_data() == true)) {
                    nl  = ",\n";
                    nl2 = ",";
                }

                if (node.type == TYPE::END_OBJECT) {
                    indent.pop_back();
                    res += indent;
                    res += "}";
                }
                else if (node.type == TYPE::END_ARRAY) {
                    indent.pop_back();
                    if (arr != TYPE::ARRAY) res += indent;
                    res += "]";
                    arr = TYPE::NA;
                }
                else {
                    if (arr != TYPE::ARRAY) res += indent;

                    if (node.type == TYPE::OBJECT) {
                        indent += "\t";
                        if (node.name == "") res += "{";
                        else res += "\"" + node.name + "\": {";
                    }
                    else if (node.type == TYPE::ARRAY || node.type == TYPE::ARRAY_NL) {
                        indent += "\t";
                        if (node.name == "") res += "[";
                        else res += "\"" + node.name + "\": [";
                        arr = (node.type == TYPE::ARRAY_NL) ? TYPE::ARRAY : TYPE::NA;
                    }
                    else if (node.type == TYPE::STRING) {
                        if (node.name == "") res += "\"" + node.value + "\"";
                        else res += "\"" + node.name + "\": \"" + node.value + "\"";
                    }
                    else if (node.type == TYPE::NUMBER) {
                        if (node.name == "") res += node.value;
                        else res += "\"" + node.name + "\": " + node.value;
                    }
                    else if (node.type == TYPE::BOOL) {
                        if (node.name == "") res += node.value;
                        else res += "\"" + node.name + "\": " + node.value;
                    }
                    else if (node.type == TYPE::NIL) {
                        if (node.name == "") res += "null";
                        else res += "\"" + node.name + "\": null";
                    }
                }

                if (arr != TYPE::ARRAY) res += nl;
                else res += nl2;
            }

            return res;
        }

        //----------------------------------------------------------------------
        std::string unescape_string(const std::string& string) {
            std::string res;

            for (size_t f = 0; f < string.length(); f++) {
                unsigned char c = string[f];
                unsigned char n = string[f + 1];

                if (c == '\\') {
                    if (n == 't') res += '\t';
                    else if (n == 'n') res += '\n';
                    else if (n == 'r') res += '\r';
                    else if (n == 'b') res += '\b';
                    else if (n == 'f') res += '\f';
                    else if (n == '\"') res += '"';
                    else if (n == '\\') res += '\\';
                    f++;
                }
                else res += c;
            }

            return res;
        }

        //----------------------------------------------------------------------
         size_t validate(const char* json) {
            auto nodes = json::NodeVector();
            return json::parse(json, nodes);
        }

        //----------------------------------------------------------------------
         size_t validate(std::string json) {
            auto nodes = json::NodeVector();
            return json::parse(json.c_str(), nodes);
        }
    }
}

// MKALGAM_OFF
