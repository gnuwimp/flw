// Copyright gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#include "util.h"

// MKALGAM_ON

#include <algorithm>
#include <math.h>
#include <stdarg.h>
#include <time.h>
#include <FL/Fl_Window.H>
#include <FL/Fl_File_Chooser.H>
#include <FL/fl_ask.H>
//
#ifdef FLW_USE_PNG
    #include <FL/Fl_PNG_Image.H>
    #include <FL/fl_draw.H>
#endif

#ifdef _WIN32
    #include <windows.h>
#else
    #include <unistd.h>
#endif

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
    if (format == nullptr || *format == 0) return "";

    int         l   = 128;
    int         n   = 0;
    char*       buf = (char*) calloc(l, 1);
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
    buf = (char*) calloc(l, 1);
    if (buf == nullptr) return res;

    va_start(args, format);
    n = vsnprintf(buf, l, format, args);
    va_end(args);
    res = buf;
    free(buf);
    return res;
}

//------------------------------------------------------------------------------
std::string flw::util::format_double(double num, int decimals, char del) {
    char res[100];

    *res = 0;

    if (decimals < 0) {
        decimals = util::count_decimals(num);
    }

    if (decimals == 0) {
        return util::format_int((int64_t) num, del);
    }

    if (fabs(num) < 9223372036854775807.0) {
        char fr_str[100];
        auto int_num    = (int64_t) fabs(num);
        auto double_num = (double) (fabs(num) - int_num);
        auto int_str    = util::format_int(int_num, del);
        auto len        = snprintf(fr_str, 99, "%.*f", decimals, double_num);

        if (len > 0 && len < 100) {
            if (num < 0.0) {
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
std::string flw::util::format_int(int64_t num, char del) {
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

    auto file = fl_fopen(filename.c_str(), "rb");

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
flw::Buf::Buf() {
    p = nullptr;
    s = 0;
}

//------------------------------------------------------------------------------
flw::Buf::Buf(size_t s_) {
    p = (s_ < SIZE_MAX) ? (char*) calloc(s_ + 1, 1) : nullptr;
    s = 0;

    if (p != nullptr) {
        s = s_;
    }
}

//------------------------------------------------------------------------------
flw::Buf::Buf(char* p_, size_t s_) {
    p = p_;
    s = s_;
}

//------------------------------------------------------------------------------
flw::Buf::Buf(const char* p_, size_t s_) {
    p = (s_ < SIZE_MAX) ? (char*) calloc(s_ + 1, 1) : nullptr;
    s = 0;

    if (p != nullptr) {
        memcpy(p, p_, s_);
        s = s_;
    }
}

//------------------------------------------------------------------------------
flw::Buf::Buf(const Buf& b) {
    p = (b.s < SIZE_MAX) ? (char*) calloc(b.s + 1, 1) : nullptr;
    s = 0;

    if (p != nullptr) {
        memcpy(p, b.p, b.s);
        s = b.s;
    }
}

//------------------------------------------------------------------------------
flw::Buf::Buf(Buf&& b) {
    p = b.p;
    s = b.s;
    b.p = nullptr;
}

//------------------------------------------------------------------------------
flw::Buf& flw::Buf::operator=(const Buf& b) {
    free(p);
    p = (b.s < SIZE_MAX) ? (char*) calloc(b.s + 1, 1) : nullptr;
    s = 0;

    if (p != nullptr) {
        memcpy(p, b.p, b.s);
        s = b.s;
    }

    return *this;
}

//------------------------------------------------------------------------------
flw::Buf& flw::Buf::operator=(Buf&& b) {
    free(p);
    p = b.p;
    s = b.s;
    b.p = nullptr;
    return *this;
}

//------------------------------------------------------------------------------
flw::Buf& flw::Buf::operator+=(const Buf& b) {
    auto t = (b.s < SIZE_MAX) ? (char*) calloc(b.s + 1, 1) : nullptr;

    if (t != nullptr) {
        memcpy(t, p, s);
        memcpy(t + s, b.p, b.s);
        free(p);
        p = t;
        s += b.s;
    }
    else {
        free(p);
        p = nullptr;
        s = 0;
    }

    return *this;
}

//------------------------------------------------------------------------------
bool flw::Buf::operator==(const Buf& other) const {
    return p != nullptr && s == other.s && memcmp(p, other.p, s) == 0;
}

//------------------------------------------------------------------------------
flw::Buf::~Buf() {
    free(p);
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

// MKALGAM_OFF
