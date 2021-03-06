// Copyright 2016 - 2021 gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#include "util.h"
#include <errno.h>
#include <math.h>
#include <stdarg.h>
#include <sys/timeb.h>
#include <time.h>
#include <unistd.h>
#include <FL/Fl_Window.H>

#ifdef _WIN32
    #include <profileapi.h>
    #include <synchapi.h>
#endif

// MALAGMA_ON

namespace flw {
    bool        PREF_IS_DARK        = false;
    int         PREF_FIXED_FONT     = FL_COURIER;
    std::string PREF_FIXED_FONTNAME = "FL_COURIER";
    int         PREF_FIXED_FONTSIZE = 14;
    int         PREF_FONT           = FL_HELVETICA;
    int         PREF_FONTSIZE       = 14;
    std::string PREF_FONTNAME       = "FL_HELVETICA";
}

//------------------------------------------------------------------------------
void flw::util::center_window(Fl_Window* window, Fl_Window* parent) {
    if (parent) {
        int x = parent->x() + parent->w() / 2;
        int y = parent->y() + parent->h() / 2;
        window->position(x - window->w() / 2, y - window->h() / 2);
    }
    else {
        window->position((Fl::w() / 2) - (window->w() / 2), (Fl::h() / 2) - (window->h() / 2));
    }
}

//------------------------------------------------------------------------------
std::string flw::util::format(const char* format, ...) {
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
void flw::util::labelfont(Fl_Widget* widget) {
    widget->labelfont(flw::PREF_FONT);
    widget->labelsize(flw::PREF_FONTSIZE);

    auto group = widget->as_group();

    if (group) {
        for (auto f = 0; f < group->children(); f++) {
            flw::util::labelfont(group->child(f));
        }
    }
}

//------------------------------------------------------------------------------
void flw::util::print(Fl_Widget* widget, bool tab) {
    printf("%s%-*s| x=%4d, y=%4d, w=%4d, h=%4d\n", tab ? "    " : "", tab ? 16 : 20, widget->label() ? widget->label() : "NO_LABEL",  widget->x(),  widget->y(),  widget->w(),  widget->h());
    fflush(stdout);
}

//------------------------------------------------------------------------------
void flw::util::print(Fl_Group* group) {
    puts("");
    flw::util::print((Fl_Widget*) group);
    for (int f = 0; f < group->children(); f++) {
        flw::util::print(group->child(f), true);
    }
}

//------------------------------------------------------------------------------
std::vector<std::string> flw::util::split(const std::string& string, const std::string& split) {
    auto res = std::vector<std::string>();

    try {
        if (split.size()) {
            auto pos1 = (std::string::size_type) 0;
            auto pos2 = string.find(split);

            while (pos2 != std::string::npos) {
                res.push_back(string.substr(pos1, pos2 - pos1));
                pos1 = pos2 + split.size();
                pos2 = string.find(split, pos1);
            }

            auto last = string.substr(pos1);

            if (last != "") {
                res.push_back(last);
            }
        }
    }
    catch(...) {
        res.clear();
    }

    return res;
}

//------------------------------------------------------------------------------
const char* flw::util::str_copy(char* dest, const char* src, int dest_size) {
    if (dest == nullptr || src == nullptr || dest_size < 2) {
        return "";
    }

    int len = strlen(src);
    *dest = 0;

    if (len && len + 1 <= dest_size) {
        memcpy(dest, src, len + 1);
    }

    return dest;
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
int64_t flw::util::time_micro() {
    #if defined(_WIN32)
        LARGE_INTEGER StartingTime;
        LARGE_INTEGER Frequency;

        QueryPerformanceFrequency(&Frequency);
        QueryPerformanceCounter(&StartingTime);

        StartingTime.QuadPart *= 1000000;
        StartingTime.QuadPart /= Frequency.QuadPart;
        return StartingTime.QuadPart;
    #elif defined(FLW_LINUX)
        timespec t;
        clock_gettime(CLOCK_MONOTONIC, &t);
        return t.tv_sec * 1000000 + t.tv_nsec / 1000;
    #else
        struct timeb timeVal;
        ftime(&timeVal);
        return timeVal.time * 1000000 + timeVal.millitm * 1000;
    #endif
}

//------------------------------------------------------------------------------
int64_t flw::util::time_milli() {
    return flw::util::time_micro() / 1000;
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
double flw::util::to_double(const char* buffer, double def) {
    double result = def;

    if (buffer && *buffer && *buffer >= '0' && *buffer <= '9') {
        errno  = 0;
        result = strtod(buffer, nullptr);
        result = (errno == ERANGE) ? def : result;
    }

    return result;
}

//------------------------------------------------------------------------------
long double flw::util::to_double_l(const char* buffer, long double def) {
    long double result = def;

    if (buffer && *buffer && *buffer >= '0' && *buffer <= '9') {
        errno  = 0;
        result = strtold(buffer, nullptr);
        result = (errno == ERANGE) ? def : result;
    }

    return result;
}

//------------------------------------------------------------------------------
int flw::util::to_doubles(const char* val, double* num1, double* num2, double* num3, double* num4, double* num5) {
    int res = 0;

    if (val == nullptr || *val == 0) {
        return 0;
    }

    char* end = nullptr;

    errno = 0;
    *num1 = strtod(val, &end);

    if (errno == 0 && val != end) {
        res++;
        val = end;

        if (num2) {
            *num2 = strtod(val, &end);

            if (errno == 0 && val != end) {
                res++;
                val = end;

                if (num3) {
                    *num3 = strtod(val, &end);

                    if (errno == 0 && val != end) {
                        res++;
                        val = end;

                        if (num4) {
                            *num4 = strtod(val, &end);

                            if (errno == 0 && val != end) {
                                res++;
                                val = end;

                                if (num5) {
                                    *num5 = strtod(val, &end);

                                    if (errno == 0 && val != end) {
                                        res++;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    return res;
}

//------------------------------------------------------------------------------
int64_t flw::util::to_int(const char* buffer, int64_t def) {
    int64_t res = def;

    if (buffer && *buffer && *buffer >= '0' && *buffer <= '9') {
        errno  = 0;
        res = strtoll(buffer, nullptr, 0);
        res = (errno == ERANGE) ? def : res;
    }

    return res;
}

//------------------------------------------------------------------------------
void* flw::util::zero_memory(char* string) {
    if (string == nullptr) {
        return nullptr;
    }

    return flw::util::zero_memory(string, strlen(string));
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
    return flw::util::zero_memory((char*) string.data());
}

// MALAGMA_OFF
