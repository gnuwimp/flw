// Copyright gnuwimp@gmail.com
// Released under the GNU General Public License v3.0
// Optional compile flags:
// -DFLW_USE_PNG
#ifndef FLW_H
#define FLW_H
#include <cstdint>
#include <string>
namespace gnu {
class Date {
public:
    enum class UTC {
                                ON,
                                OFF,
    };
    enum class US {
                                ON,
                                OFF,
    };
    enum class Compare {
                                YYYYMM,
                                YYYYMMDD,
                                YYYYMMDDHH,
                                YYYYMMDDHHMM,
                                YYYYMMDDHHMMSS,
                                LAST = YYYYMMDDHHMMSS,
    };
    enum class Day {
                                INVALID,
                                MONDAY,
                                TUESDAY,
                                WENDSDAY,
                                THURSDAY,
                                FRIDAY,
                                SATURDAY,
                                SUNDAY,
                                LAST = SUNDAY,
    };
    enum class Format {
                                ISO,
                                ISO_LONG,
                                ISO_TIME,
                                ISO_TIME_LONG,
                                TIME,
                                TIME_LONG,
                                US,
                                WORLD,
                                DAY_MONTH_YEAR,
                                DAY_MONTH_YEAR_SHORT,
                                WEEKDAY_MONTH_YEAR,
                                WEEKDAY_MONTH_YEAR_SHORT,
                                WEEKDAY,
                                WEEKDAY_SHORT,
                                MONTH,
                                MONTH_SHORT,
                                LAST = MONTH_SHORT,
    };
    static const int            SECS_PER_HOUR = 3600;
    static const int            SECS_PER_DAY  = 3600 * 24;
    static const int            SECS_PER_WEEK = 3600 * 24 * 7;
    explicit                    Date(Date::UTC utc = Date::UTC::OFF);
                                Date(int year, int month, int day, int hour = 0, int min = 0, int sec = 0);
    explicit                    Date(int64_t unix_time, Date::UTC utc = Date::UTC::OFF);
    explicit                    Date(const std::string& date, Date::US us = Date::US::OFF);
    bool                        operator<(const Date& other) const
                                    { return compare(other) < 0 ? true : false; }
    bool                        operator<=(const Date& other) const
                                    { return compare(other) <= 0 ? true : false; }
    bool                        operator>(const Date& other) const
                                    { return compare(other) > 0 ? true : false; }
    bool                        operator>=(const Date& other) const
                                    { return compare(other) >= 0 ? true : false; }
    bool                        operator==(const Date& other) const
                                    { return compare(other) == 0 ? true : false; }
    bool                        operator!=(const Date& other) const
                                    { return compare(other) != 0 ? true : false; }
    bool                        add_days(int days);
    bool                        add_months(int months);
    bool                        add_seconds(int64_t seconds);
    bool                        add_years(int years)
                                    { return add_months(years * 12); }
    Date&                       clear()
                                    { _year = _month = _day = _hour = _min = _sec = 0; return *this; }
    Date&                       clear_time()
                                    { _hour = _min = _sec = 0; return *this; }
    int                         compare(const Date& other, Compare flag = Compare::YYYYMMDDHHMMSS) const;
    int                         day() const
                                    { return _day; }
    int                         days_in_month() const;
    int                         days_into_year() const;
    void                        debug() const;
    int                         diff_days(const Date& other) const;
    int                         diff_months(const Date& other) const;
    int64_t                     diff_seconds(const Date& other) const;
    std::string                 format(Format format = Format::ISO) const;
    int                         hour() const
                                    { return _hour; }
    bool                        is_invalid() const
                                    { return _year == 0 || _month == 0 || _day == 0; }
    bool                        is_leapyear() const;
    int                         minute() const
                                    { return _min; }
    int                         month() const
                                    { return _month; }
    const char*                 month_name() const;
    const char*                 month_name_short() const;
    int                         second() const
                                    { return _sec; }
    Date&                       set(const Date& other);
    Date&                       set(const std::string& date, Date::US us = Date::US::OFF);
    Date&                       set(int year, int month, int day, int hour = 0, int min = 0, int sec = 0);
    Date&                       set_day(int day);
    Date&                       set_day_to_last_in_month()
                                    { _day = days_in_month(); return *this; }
    Date&                       set_hour(int hour);
    Date&                       set_minute(int min);
    Date&                       set_month(int month);
    Date&                       set_second(int sec);
    Date&                       set_weekday(Day weekday);
    Date&                       set_year(int year);
    int64_t                     time() const;
    int                         week() const;
    Day                         weekday() const;
    const char*                 weekday_name() const;
    const char*                 weekday_name_short() const;
    int                         year() const
                                    { return _year; }
    static inline bool          CompareDates(const Date& a, const Date& b)
                                    { return a.compare(b) < 0; }
private:
    short                       _year;
    char                        _month;
    char                        _day;
    char                        _hour;
    char                        _min;
    char                        _sec;
};
}
#include <cassert>
#include <cstdint>
#include <cstring>
#include <string>
#include <vector>
#include <array>
namespace gnu {
namespace file {
class File;
class Buf;
typedef bool (*CallbackCopy)(int64_t size, int64_t copied, void* data);
typedef std::vector<File> Files;
enum class Type {
    MISSING,
    DIR,
    FILE,
    OTHER,
};
#ifdef _WIN32
    static const int            DEFAULT_DIR_MODE  = 0x00000080;
    static const int            DEFAULT_FILE_MODE = 0x00000080;
#else
    static const int            DEFAULT_DIR_MODE  = 0755;
    static const int            DEFAULT_FILE_MODE = 0664;
#endif
char*                           allocate(char* resize_or_null, size_t size);
File                            canonical(const std::string& path);
bool                            chdir(const std::string& path);
std::string                     check_filename(const std::string& name);
bool                            chmod(const std::string& path, int mode);
bool                            chtime(const std::string& path, int64_t time);
Buf                             close_stderr();
Buf                             close_stdout();
bool                            copy(const std::string& from, const std::string& to, CallbackCopy callback = nullptr, void* data = nullptr, bool flush_write = true);
uint64_t                        fletcher64(const char* buffer, size_t buffer_size);
void                            flush(FILE* file);
File                            home_dir();
bool                            is_circular(const std::string& path);
File                            linkname(const std::string& path);
bool                            mkdir(const std::string& path);
FILE*                           open(const std::string& path, const std::string& mode);
std::string                     os();
FILE*                           popen(const std::string& cmd, bool write = false);
Buf                             read(const std::string& path);
Buf*                            read2(const std::string& path);
Files                           read_dir(const std::string& path);
Files                           read_dir_rec(const std::string& path);
bool                            redirect_stderr();
bool                            redirect_stdout();
bool                            remove(const std::string& path);
bool                            remove_rec(const std::string& path);
bool                            rename(const std::string& from, const std::string& to);
int                             run(const std::string& cmd, bool background, bool hide_win32_window = false);
File                            tmp_dir();
File                            tmp_file(const std::string& prepend = "");
File                            work_dir();
bool                            write(const std::string& path, const char* buffer, size_t size, bool flush = true);
bool                            write(const std::string& path, const Buf& buf, bool flush = true);
class Buf {
public:
                                Buf()
                                    { _str = nullptr; _size = 0; }
    explicit                    Buf(size_t size);
                                Buf(const char* buffer, size_t size);
                                Buf(const Buf& b);
                                Buf(Buf&& b)
                                    { _str = b._str; _size = b._size; b._str = nullptr; }
                                Buf(const std::string& string)
                                    { _str = nullptr; add(string.c_str(), string.length()); }
    virtual                     ~Buf()
                                    { free(_str); }
    unsigned char&              operator[](size_t index)
                                    { if (index >= _size || _str == nullptr) throw std::string("error: gnu::file::Buf::[]: index is out of range"); return ((unsigned char*) _str)[index]; }
    unsigned char               operator[](size_t index) const
                                    { if (index >= _size || _str == nullptr) throw std::string("error: gnu::file::Buf::[]: index is out of range"); return ((unsigned char*) _str)[index]; }
    Buf&                        operator=(const Buf& b)
                                    { return set(b._str, b._size); }
    Buf&                        operator=(Buf&& b)
                                    { free(_str); _str = b._str; _size = b._size; b._str = nullptr; return *this; }
    Buf&                        operator=(const std::string& string)
                                    { free(_str); _str = nullptr; add(string.c_str(), string.length()); return *this; }
    Buf&                        operator+=(const Buf& b)
                                    { return add(b._str, b._size); }
    bool                        operator==(const Buf& other) const;
    bool                        operator!=(const Buf& other) const
                                    { return (*this == other) == false; }
    Buf&                        add(const char* buffer, size_t size);
    const char*                 c_str() const
                                    { return _str; }
    void                        clear()
                                    { free(_str); _str = nullptr; _size = 0; }
    std::array<size_t, 257>     count() const
                                    { return Buf::Count(_str, _size); }
    void                        debug() const
                                    { printf("gnu::Buf(0x%p, %llu)\n", _str, (long long unsigned) _size); }
    uint64_t                    fletcher64() const
                                    { return file::fletcher64(_str, _size); }
    Buf&                        grab(char* buffer, size_t size)
                                    { free(_str); _str = buffer; _size = size; return *this; }
    Buf                         insert_cr(bool dos = true, bool trailing = false) const
                                    { return Buf::InsertCR(_str, _size, dos, trailing); }
    char*                       release()
                                    { auto res = _str; _str = nullptr; _size = 0; return res; }
    Buf                         remove_cr() const
                                    { return Buf::RemoveCR(_str, _size); }
    Buf&                        set(const char* buffer, size_t size);
    size_t                      size() const
                                    { return _size; }
    void                        size(size_t size)
                                    {  if (size >= _size) throw std::string("error: gnu::file::Buf::size(): size is out of range"); _size = size; }
    char*                       str()
                                    { return _str; }
    bool                        write(const std::string& path, bool flush = true) const;
    static std::array<size_t, 257> Count(const char* buffer, size_t size);
    static inline Buf           Grab(char* string)
                                    { auto res = Buf(); res._str = string; res._size = strlen(string); return res; }
    static inline Buf           Grab(char* buffer, size_t size)
                                    { auto res = Buf(); res._str = buffer; res._size = size; return res; }
    static Buf                  InsertCR(const char* buffer, size_t size, bool dos, bool trailing = false);
    static Buf                  RemoveCR(const char* buffer, size_t size);
private:
    char*                       _str;
    size_t                      _size;
};
class File {
public:
    explicit                    File(const std::string& path = "", bool realpath = false);
    bool                        operator==(const File& other) const
                                    { return _filename == other._filename; }
    bool                        operator<(const File& other) const
                                    { return _filename < other._filename; }
    bool                        operator<=(const File& other) const
                                    { return _filename <= other._filename; }
    const char*                 c_str() const
                                    { return _filename.c_str(); }
    File                        canonical() const
                                    { return file::canonical(_filename); }
    int64_t                     ctime() const
                                    { return _ctime; }
    void                        debug(bool short_version = true) const
                                    { printf("%s\n", to_string(short_version).c_str()); fflush(stdout); }
    bool                        exist() const
                                    { return _type != Type::MISSING; }
    const std::string&          ext() const
                                    { return _ext; }
    const std::string&          filename() const
                                    { return _filename; }
    bool                        is_circular() const
                                    { return file::is_circular(_filename); }
    bool                        is_dir() const
                                    { return _type == Type::DIR; }
    bool                        is_file() const
                                    { return _type == Type::FILE; }
    bool                        is_link() const
                                    { return _link; }
    bool                        is_missing() const
                                    { return _type == Type::MISSING; }
    bool                        is_other() const
                                    { return _type == Type::OTHER; }
    File                        linkname() const
                                    { return file::linkname(_filename); }
    int                         mode() const
                                    { return _mode; }
    int64_t                     mtime() const
                                    { return _mtime; }
    const std::string&          name() const
                                    { return _name; }
    std::string                 name_without_ext() const;
    const std::string&          parent() const
                                    { return _path; }
    const std::string&          path() const
                                    { return _path; }
    int64_t                     size() const
                                    { return _size; }
    std::string                 to_string(bool short_version = true) const;
    Type                        type() const
                                    { return _type; }
    std::string                 type_name() const;
private:
    Type                        _type;
    bool                        _link;
    int                         _mode;
    int64_t                     _ctime;
    int64_t                     _mtime;
    int64_t                     _size;
    std::string                 _ext;
    std::string                 _filename;
    std::string                 _name;
    std::string                 _path;
};
}
}
#include <assert.h>
#include <cstdint>
#include <cstring>
#include <map>
#include <string>
#include <vector>
namespace gnu {
namespace json {
enum class TYPE : uint8_t {
                                OBJECT,
                                ARRAY,
                                STRING,
                                NUMBER,
                                BOOL,
                                NIL,
                                ERR,
};
enum class ENCODE : uint8_t {
                                DEFAULT,
                                TRIM,
                                FLAT,
};
static const size_t             MAX_DEPTH = 32;
class JS;
typedef std::map<std::string, JS*> JSObject;
typedef std::vector<JS*> JSArray;
size_t                          count_utf8(const char* p);
JS                              decode(const char* json, size_t len, bool ignore_trailing_comma = false, bool ignore_duplicates = false, bool ignore_utf_check = false);
JS                              decode(const std::string& json, bool ignore_trailing_comma = false, bool ignore_duplicates = false, bool ignore_utf_check = false);
std::string                     encode(const JS& js, ENCODE option = ENCODE::DEFAULT);
std::string                     escape(const char* string);
std::string                     format_number(double f, bool E = false);
std::string                     unescape(const char* string);
class JS {
    friend class                Builder;
    friend JS                   decode(const char* json, size_t len, bool ignore_trailing_comma, bool ignore_duplicates, bool ignore_utf_check);
    friend std::string          encode(const JS* js, ENCODE option);
public:
                                JS(const JS&) = delete;
    JS&                         operator=(const JS&) = delete;
                                JS();
                                JS(JS&& other);
                                ~JS();
    JS&                         operator=(JS&&);
    bool                        operator==(TYPE type) const
                                    { return _type == type; }
    bool                        operator!=(TYPE type) const
                                    { return _type != type; }
    const JS*                   operator[](const std::string& name) const
                                    { return _get_value(name.c_str(), true); }
    const JS*                   operator[](size_t index) const
                                    { return (_type == TYPE::ARRAY && index < _va->size()) ? (*_va)[index] : nullptr; }
    void                        debug() const;
    std::string                 err() const
                                    { return (_type == TYPE::ERR) ? _vs : ""; }
    const char*                 err_c() const
                                    { return (_type == TYPE::ERR) ? _vs : ""; }
    const JS*                   find(const std::string& name, bool rec = false) const;
    const JS*                   get(const std::string& name, bool escape_name = true) const
                                    { return _get_value(name.c_str(), escape_name); }
    const JS*                   get(size_t index) const
                                    { return (*this) [index]; }
    bool                        has_err() const
                                    { return _type == TYPE::ERR; }
    bool                        has_inline() const
                                    { return _inl; }
    bool                        is_array() const
                                    { return _type == TYPE::ARRAY; }
    bool                        is_bool() const
                                    { return _type == TYPE::BOOL; }
    bool                        is_null() const
                                    { return _type == TYPE::NIL; }
    bool                        is_number() const
                                    { return _type == TYPE::NUMBER; }
    bool                        is_object() const
                                    { return _type == TYPE::OBJECT; }
    bool                        is_string() const
                                    { return _type == TYPE::STRING; }
    std::string                 name() const
                                    { return (_name != nullptr) ? _name : ""; }
    const char*                 name_c() const
                                    { return (_name != nullptr) ? _name : ""; }
    std::string                 name_u() const
                                    { return (_name != nullptr) ? json::unescape(_name) : ""; }
    JS*                         parent() const
                                    { return _parent; }
    unsigned                    pos() const
                                    { return _pos; }
    size_t                      size() const
                                    { return (is_array() == true) ? _va->size() : (is_object() == true) ? _vo->size() : 0; }
    std::string                 to_string() const;
    TYPE                        type() const
                                    { return _type; }
    std::string                 type_name() const
                                    { return TYPE_NAMES[static_cast<unsigned>(_type)]; }
    const JSArray*              va() const
                                    { return (_type == TYPE::ARRAY) ? _va : nullptr; }
    bool                        vb() const
                                    { assert(_type == TYPE::BOOL); return (_type == TYPE::BOOL) ? _vb : false; }
    double                      vn() const
                                    { assert(_type == TYPE::NUMBER); return (_type == TYPE::NUMBER) ? _vn : 0.0; }
    long long int               vn_i() const
                                    { assert(_type == TYPE::NUMBER); return (_type == TYPE::NUMBER) ? (long long int) _vn : 0; }
    const JSObject*             vo() const
                                    { return (_type == TYPE::OBJECT) ? _vo : nullptr; }
    const JSArray               vo_to_va() const;
    std::string                 vs() const
                                    { assert(_type == TYPE::STRING); return (_type == TYPE::STRING) ? _vs : ""; }
    const char*                 vs_c() const
                                    { assert(_type == TYPE::STRING); return (_type == TYPE::STRING) ? _vs : ""; }
    std::string                 vs_u() const
                                    { assert(_type == TYPE::STRING); return (_type == TYPE::STRING) ? json::unescape(_vs) : ""; }
    static inline ssize_t       Count()
                                    { return JS::COUNT; }
    static inline ssize_t       Max()
                                    { return JS::MAX; }
private:
                                JS(const char* name, JS* parent = nullptr, unsigned pos = 0);
    bool                        _add_bool(char** sVal1, bool b, bool ignore_duplicates, unsigned pos);
    bool                        _add_null(char** sVal1, bool ignore_duplicates, unsigned pos);
    bool                        _add_number(char** sVal1, double& nVal, bool ignore_duplicates, unsigned pos);
    bool                        _add_string(char** sVal1, char** sVal2, bool ignore_duplicates, unsigned pos);
    void                        _clear(bool name);
    const JS*                   _get_value(const char* name, bool escape) const;
    void                        _set_err(const std::string& err);
    bool                        _set_value(const char* name, JS* js, bool ignore_duplicates);
    void                        _set_child_parent_to_me();
    static JS*                  _MakeArray(const char* name, JS* parent, unsigned pos);
    static JS*                  _MakeBool(const char* name, bool vb, JS* parent, unsigned pos);
    static JS*                  _MakeNull(const char* name, JS* parent, unsigned pos);
    static JS*                  _MakeNumber(const char* name, double vn, JS* parent, unsigned pos);
    static JS*                  _MakeObject(const char* name, JS* parent, unsigned pos);
    static JS*                  _MakeString(const char* name, const char* vs, JS* parent, unsigned pos);
    static constexpr const char* TYPE_NAMES[10] = { "OBJECT", "ARRAY", "STRING", "NUMBER", "BOOL", "NIL", "ERR", "", ""};
    static ssize_t              COUNT;
    static ssize_t              MAX;
    bool                        _inl;
    TYPE                        _type;
    uint32_t                    _pos;
    JS*                         _parent;
    char*                       _name;
    union {
        JSArray*                _va;
        JSObject*               _vo;
        bool                    _vb;
        double                  _vn;
        char*                   _vs;
    };
};
class Builder {
public:
                                Builder()
                                    { _root = _current = nullptr; }
    virtual                     ~Builder()
                                    { delete _root; }
    Builder&                    operator<<(JS* json)
                                    { return add(json); }
    Builder&                    add(JS* json);
    void                        clear()
                                    { delete _root; _root = _current = nullptr; _name = ""; }
    std::string                 encode(ENCODE option = ENCODE::DEFAULT) const;
    Builder&                    end();
    const JS*                   root() const
                                    { return _root; }
    static JS*                  MakeArray(const char* name = "", bool escape = true);
    static JS*                  MakeArrayInline(const char* name = "", bool escape = true);
    static JS*                  MakeBool(bool vb, const char* name = "", bool escape = true);
    static JS*                  MakeNull(const char* name = "", bool escape = true);
    static JS*                  MakeNumber(double vn, const char* name = "", bool escape = true);
    static JS*                  MakeObject(const char* name = "", bool escape = true);
    static JS*                  MakeObjectInline(const char* name = "", bool escape = true);
    static JS*                  MakeString(const char* vs, const char* name = "", bool escape = true);
    static JS*                  MakeString(const std::string& vs, const char* name = "", bool escape = true);
private:
    JS*                         _current;
    JS*                         _root;
    std::string                 _name;
};
}
}
namespace icons {
static const std::string ALERT = R"(
<?xml version="1.0" encoding="iso-8859-1"?>
<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg height="800px" width="800px" version="1.1" id="Layer_1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" 
	 viewBox="0 0 506.4 506.4" xml:space="preserve">
<circle style="fill:#DF5C4E;" cx="253.2" cy="253.2" r="249.2"/>
<g>
	<path style="fill:#F4EFEF;" d="M253.2,332.4c-10.8,0-20-8.8-20-19.6v-174c0-10.8,9.2-19.6,20-19.6s20,8.8,20,19.6v174
		C273.2,323.6,264,332.4,253.2,332.4z"/>
	<path style="fill:#F4EFEF;" d="M253.2,395.6c-5.2,0-10.4-2-14-5.6s-5.6-8.8-5.6-14s2-10.4,5.6-14s8.8-6,14-6s10.4,2,14,6
		c3.6,3.6,6,8.8,6,14s-2,10.4-6,14C263.6,393.6,258.4,395.6,253.2,395.6z"/>
</g>
<path d="M253.2,506.4C113.6,506.4,0,392.8,0,253.2S113.6,0,253.2,0s253.2,113.6,253.2,253.2S392.8,506.4,253.2,506.4z M253.2,8
	C118,8,8,118,8,253.2s110,245.2,245.2,245.2s245.2-110,245.2-245.2S388.4,8,253.2,8z"/>
<path d="M249.2,336.4c-13.2,0-24-10.8-24-23.6v-174c0-13.2,10.8-23.6,24-23.6s24,10.8,24,23.6v174
	C273.2,325.6,262.4,336.4,249.2,336.4z M249.2,122.8c-8.8,0-16,7.2-16,15.6v174c0,8.8,7.2,15.6,16,15.6s16-7.2,16-15.6v-174
	C265.2,130,258,122.8,249.2,122.8z"/>
<path d="M249.2,399.6c-6.4,0-12.4-2.4-16.8-6.8c-4.4-4.4-6.8-10.4-6.8-16.8s2.4-12.4,6.8-16.8c4.4-4.4,10.8-6.8,16.8-6.8
	c6.4,0,12.4,2.4,16.8,6.8c4.4,4.4,6.8,10.4,6.8,16.8s-2.4,12.4-7.2,16.8C261.6,397.2,255.6,399.6,249.2,399.6z M249.2,360
	c-4,0-8.4,1.6-11.2,4.8c-2.8,2.8-4.4,6.8-4.4,11.2c0,4,1.6,8.4,4.8,11.2c2.8,2.8,7.2,4.8,11.2,4.8s8.4-1.6,11.2-4.8
	c2.8-2.8,4.8-7.2,4.8-11.2s-1.6-8.4-4.8-11.2C257.2,361.6,253.2,360,249.2,360z"/>
</svg>
)";
static const std::string CONFIRM = R"(
<?xml version="1.0" encoding="iso-8859-1"?>
<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg height="800px" width="800px" version="1.1" id="Layer_1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" 
	 viewBox="0 0 506.4 506.4" xml:space="preserve">
<circle style="fill:#54B265;" cx="253.2" cy="253.2" r="249.2"/>
<path style="fill:#F4EFEF;" d="M372.8,200.4l-11.2-11.2c-4.4-4.4-12-4.4-16.4,0L232,302.4l-69.6-69.6c-4.4-4.4-12-4.4-16.4,0
	L134.4,244c-4.4,4.4-4.4,12,0,16.4l89.2,89.2c4.4,4.4,12,4.4,16.4,0l0,0l0,0l10.4-10.4l0.8-0.8l121.6-121.6
	C377.2,212.4,377.2,205.2,372.8,200.4z"/>
<path d="M253.2,506.4C113.6,506.4,0,392.8,0,253.2S113.6,0,253.2,0s253.2,113.6,253.2,253.2S392.8,506.4,253.2,506.4z M253.2,8
	C118,8,8,118,8,253.2s110,245.2,245.2,245.2s245.2-110,245.2-245.2S388.4,8,253.2,8z"/>
<path d="M231.6,357.2c-4,0-8-1.6-11.2-4.4l-89.2-89.2c-6-6-6-16,0-22l11.6-11.6c6-6,16.4-6,22,0l66.8,66.8L342,186.4
	c2.8-2.8,6.8-4.4,11.2-4.4c4,0,8,1.6,11.2,4.4l11.2,11.2l0,0c6,6,6,16,0,22L242.8,352.4C239.6,355.6,235.6,357.2,231.6,357.2z
	 M154,233.6c-2,0-4,0.8-5.6,2.4l-11.6,11.6c-2.8,2.8-2.8,8,0,10.8l89.2,89.2c2.8,2.8,8,2.8,10.8,0l132.8-132.8c2.8-2.8,2.8-8,0-10.8
	l-11.2-11.2c-2.8-2.8-8-2.8-10.8,0L234.4,306c-1.6,1.6-4,1.6-5.6,0l-69.6-69.6C158,234.4,156,233.6,154,233.6z"/>
</svg>
)";
static const std::string DELETE = R"(
<?xml version="1.0" encoding="iso-8859-1"?>
<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg height="800px" width="800px" version="1.1" id="Layer_1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" 
	 viewBox="0 0 506.4 506.4" xml:space="preserve">
<circle style="fill:#DF5C4E;" cx="253.2" cy="253.2" r="249.2"/>
<path style="fill:#F4EFEF;" d="M373.2,244.8c0-6.4-5.2-11.6-11.6-11.6H140.8c-6.4,0-11.6,5.2-11.6,11.6v16.8
	c0,6.4,5.2,11.6,11.6,11.6h220.8c6.4,0,11.6-5.2,11.6-11.6V244.8z"/>
<path d="M253.2,506.4C113.6,506.4,0,392.8,0,253.2S113.6,0,253.2,0s253.2,113.6,253.2,253.2S392.8,506.4,253.2,506.4z M253.2,8
	C118,8,8,118,8,253.2s110,245.2,245.2,245.2s245.2-110,245.2-245.2S388.4,8,253.2,8z"/>
<path d="M357.6,277.2H136.8c-8.8,0-15.6-7.2-15.6-15.6v-16.8c0-8.8,7.2-15.6,15.6-15.6h220.8c8.8,0,15.6,7.2,15.6,15.6v16.8
	C373.2,270,366,277.2,357.6,277.2z M136.8,237.2c-4.4,0-7.6,3.6-7.6,7.6v16.8c0,4.4,3.6,7.6,7.6,7.6h220.8c4.4,0,7.6-3.6,7.6-7.6
	v-16.8c0-4.4-3.6-7.6-7.6-7.6H136.8z"/>
</svg>
)";
static const std::string ERROR = R"(
<?xml version="1.0" encoding="UTF-8"?>
<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg width="800px" height="800px" version="1.1" viewBox="0 0 506.4 506.4" xml:space="preserve" xmlns="http://www.w3.org/2000/svg">
<circle cx="253.2" cy="253.2" r="249.2" fill="#ff2a2a"/>
<path d="m281.6 253.2 90.8-90.8c4.4-4.4 4.4-12 0-16.4l-11.2-11.2c-4.4-4.4-12-4.4-16.4 0l-90.8 90.8-90.8-90.8c-4.4-4.4-12-4.4-16.4 0l-11.2 11.2c-4.4 4.4-4.4 12 0 16.4l90.8 90.8-90.8 90.8c-4.4 4.4-4.4 12 0 16.4l11.2 11.6c4.4 4.4 12 4.4 16.4 0l90.8-90.8 90.8 90.8c4.4 4.4 12 4.4 16.4 0l11.2-11.6c4.4-4.4 4.4-12 0-16.4l-90.8-90.8z" fill="#F4EFEF"/>
<path d="M253.2,506.4C113.6,506.4,0,392.8,0,253.2S113.6,0,253.2,0s253.2,113.6,253.2,253.2S392.8,506.4,253.2,506.4z M253.2,8  C118,8,8,118,8,253.2s110,245.2,245.2,245.2s245.2-110,245.2-245.2S388.4,8,253.2,8z"/>
<path d="m352.8 379.6c-4 0-8-1.6-11.2-4.4l-88-88-88 88c-2.8 2.8-6.8 4.4-11.2 4.4-4 0-8-1.6-11.2-4.4l-11.2-11.2c-2.8-2.8-4.4-6.8-4.4-11.2 0-4 1.6-8 4.4-11.2l88-88-88-88c-2.8-2.8-4.4-6.8-4.4-11.2 0-4 1.6-8 4.4-11.2l11.2-11.2c6-6 16.4-6 22 0l88 88 88-88c2.8-2.8 6.8-4.4 11.2-4.4 4 0 8 1.6 11.2 4.4l11.2 11.2c6 6 6 16 0 22l-88 88 88 88c2.8 2.8 4.4 6.8 4.4 11.2 0 4-1.6 8-4.4 11.2l-11.2 11.2c-2.8 3.2-6.4 4.8-10.8 4.8zm-99.2-102.4c1.2 0 2 0.4 2.8 1.2l90.8 90.8c1.6 1.6 3.2 2.4 5.6 2.4 2 0 4-0.8 5.6-2.4l11.6-11.6c1.6-1.6 2.4-3.2 2.4-5.6 0-2-0.8-4-2.4-5.6l-90.8-90.8c-0.8-0.8-1.2-1.6-1.2-2.8s0.4-2 1.2-2.8l90.8-90.8c2.8-2.8 2.8-8 0-10.8l-11.2-11.2c-1.6-1.6-3.2-2.4-5.6-2.4-2 0-4 0.8-5.6 2.4l-90.8 90.8c-1.6 1.6-4 1.6-5.6 0l-90.8-90.8c-2.8-2.8-8-2.8-10.8 0l-11.6 11.2c-1.6 1.6-2.4 3.2-2.4 5.6s0.8 4 2.4 5.6l90.8 90.8c1.6 1.6 1.6 4 0 5.6l-90.8 90.8c-1.6 1.6-2.4 3.2-2.4 5.6 0 2 0.8 4 2.4 5.6l11.6 11.6c2.8 2.8 8 2.8 10.8 0l90.8-90.8c0.4-1.2 1.2-1.6 2.4-1.6z"/>
</svg>
)";
static const std::string INFO = R"(
<?xml version="1.0" encoding="iso-8859-1"?>
<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg version="1.1" id="Layer_1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" 
	 viewBox="0 0 512 512" xml:space="preserve">
<circle style="fill:#FFD782;" cx="256.006" cy="256.005" r="247.389"/>
<path style="opacity:0.1;enable-background:new    ;" d="M77.547,255.999c0-124.928,92.609-228.209,212.918-244.982
	C279.199,9.447,267.697,8.616,256,8.616C119.374,8.616,8.617,119.374,8.617,255.999c0,136.627,110.758,247.383,247.383,247.383
	c11.697,0,23.199-0.831,34.465-2.401C170.156,484.209,77.547,380.928,77.547,255.999z"/>
<g>
	<rect x="222.632" y="197.679" style="fill:#515262;" width="66.736" height="225.802"/>
	<circle style="fill:#515262;" cx="256.006" cy="126.635" r="39.313"/>
</g>
<path d="M439.868,430.105c-2.055,0-4.116-0.731-5.764-2.214c-3.536-3.185-3.821-8.632-0.638-12.168
	c39.53-43.893,61.299-100.617,61.299-159.722C494.767,124.342,387.657,17.232,256,17.232c-55.312,0-109.23,19.347-151.821,54.477
	c-3.669,3.027-9.101,2.508-12.13-1.164c-3.028-3.672-2.507-9.102,1.165-12.129C138.885,20.747,196.696,0,256,0
	c68.38,0,132.667,26.629,181.019,74.981s74.981,112.639,74.981,181.019c0,63.372-23.342,124.192-65.727,171.254
	C444.572,429.143,442.224,430.105,439.868,430.105z"/>
<path d="M256,512c-68.379,0-132.667-26.629-181.019-74.981S0.001,324.38,0.001,255.999c0-64.259,23.919-125.711,67.35-173.036
	c3.216-3.505,8.667-3.741,12.174-0.523c3.506,3.218,3.739,8.668,0.522,12.174C39.54,138.752,17.233,196.067,17.233,255.999
	c0,131.657,107.11,238.767,238.767,238.767c56.109,0,110.637-19.862,153.538-55.927c3.643-3.063,9.077-2.591,12.14,1.051
	c3.062,3.643,2.592,9.078-1.051,12.14C374.625,490.703,316.16,512,256,512z"/>
<path d="M289.368,432.092h-66.736c-4.76,0-8.616-3.858-8.616-8.616V197.677c0-4.758,3.857-8.616,8.616-8.616h66.736
	c4.76,0,8.616,3.858,8.616,8.616v225.798C297.984,428.234,294.127,432.092,289.368,432.092z M231.25,414.86h49.503V206.293H231.25
	V414.86z"/>
<path d="M256,174.561c-26.429,0-47.93-21.501-47.93-47.93s21.501-47.93,47.93-47.93s47.93,21.501,47.93,47.93
	C303.93,153.06,282.429,174.561,256,174.561z M256,95.933c-16.926,0-30.698,13.771-30.698,30.698s13.771,30.698,30.698,30.698
	s30.698-13.771,30.698-30.698S272.927,95.933,256,95.933z"/>
</svg>
)";
static const std::string STOP = R"(
<?xml version="1.0" encoding="UTF-8"?>
<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg width="800px" height="800px" version="1.1" viewBox="0 0 506.4 506.4" xml:space="preserve" xmlns="http://www.w3.org/2000/svg">
<circle cx="253.2" cy="253.2" r="249.2" fill="#80b3ff"/>
<path d="m349.2 331.6c0 12-9.6 21.6-21.6 21.6h-152.8c-12 0-21.6-9.6-21.6-21.6v-152.8c0-12 9.6-21.6 21.6-21.6h152.8c12 0 21.6 9.6 21.6 21.6v152.8z" fill="#F4EFEF"/>
<path d="M253.2,506.4C113.6,506.4,0,392.8,0,253.2S113.6,0,253.2,0s253.2,113.6,253.2,253.2S392.8,506.4,253.2,506.4z M253.2,8  C118,8,8,118,8,253.2s110,245.2,245.2,245.2s245.2-110,245.2-245.2S388.4,8,253.2,8z"/>
<path d="m323.6 357.2h-152.8c-14 0-25.6-11.6-25.6-25.6v-152.8c0-14 11.6-25.6 25.6-25.6h152.8c14 0 25.6 11.6 25.6 25.6v152.8c0 14-11.6 25.6-25.6 25.6zm-152.8-196c-9.6 0-17.6 8-17.6 17.6v152.8c0 9.6 8 17.6 17.6 17.6h152.8c9.6 0 17.6-8 17.6-17.6v-152.8c0-9.6-8-17.6-17.6-17.6h-152.8z"/>
</svg>
)";
static const std::string QUESTION = R"(
<?xml version="1.0" encoding="iso-8859-1"?>
<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg height="800px" width="800px" version="1.1" id="Layer_1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" 
	 viewBox="0 0 506.4 506.4" xml:space="preserve">
<circle style="fill:#ACD9EA;" cx="253.2" cy="253.2" r="249.2"/>
<g>
	<path style="fill:#F4EFEF;" d="M253.2,332.4c-10.8,0-20-8.8-20-19.6v-36c0-10.8,8.8-19.6,20-19.6c28,0,51.2-22.8,51.2-50.8
		s-22.8-50.8-50.8-50.8s-50.8,22.8-50.8,50.8c0,10.8-8.8,19.6-19.6,19.6s-19.6-8.8-19.6-19.6c0-50,40.8-90.4,90.4-90.4
		s90.8,40.8,90.8,90.4c0,43.2-26.8,79.2-70.8,88.4v18.4C273.2,323.6,264,332.4,253.2,332.4z"/>
	<path style="fill:#F4EFEF;" d="M252.4,395.6c-5.2,0-10.4-2-14-6c-3.6-3.6-5.6-8.8-5.6-14s2-10.4,5.6-14s8.8-5.6,14-5.6
		s10.4,2,14,5.6s6,8.8,6,14s-2,10.4-6,14C262.4,393.6,257.6,395.6,252.4,395.6z"/>
</g>
<path d="M253.2,506.4C113.6,506.4,0,392.8,0,253.2S113.6,0,253.2,0s253.2,113.6,253.2,253.2S392.8,506.4,253.2,506.4z M253.2,8
	C118,8,8,118,8,253.2s110,245.2,245.2,245.2s245.2-110,245.2-245.2S388.4,8,253.2,8z"/>
<path d="M249.2,336.4c-13.2,0-24-10.8-24-23.6v-36c0-12.8,12-23.6,26-23.6c26,0,48-21.6,48-46.8c0-26-20.8-46.8-46.4-46.8
	c-25.6,0-46.8,21.2-46.8,46.8c0,13.2-10.8,23.6-23.6,23.6c-13.2,0-23.6-10.8-23.6-23.6c0-52,42.4-94.4,94.4-94.4
	s92.8,41.6,92.8,94.4c0,44.8-29.2,81.2-72.8,91.6v15.2C273.2,325.6,262.4,336.4,249.2,336.4z M252.8,150.8c30,0,54.4,24.8,54.4,54.8
	c0,29.6-25.6,54.8-56,54.8c-8.4,0-18,6.8-18,15.6v36c0,8.8,7.2,15.6,16,15.6s16-7.2,16-15.6v-18.4c0-2,1.2-3.6,3.2-4
	c41.6-8.4,69.6-42.4,69.6-84.4c0-48.4-37.2-86.4-84.8-86.4s-86.4,38.8-86.4,86.4c0,8.8,7.2,15.6,15.6,15.6c8.8,0,15.6-7.2,15.6-15.6
	C198,175.2,222.4,150.8,252.8,150.8z"/>
<path d="M252.4,399.6c-6.4,0-12.4-2.4-16.8-7.2c-4.4-4.4-6.8-10.4-6.8-16.8s2.4-12.4,6.8-16.8c4.4-4.4,10.4-6.8,16.8-6.8
	c6.4,0,12.4,2.4,16.8,6.8c4.4,4.4,6.8,10.8,6.8,16.8c0,6.4-2.4,12.4-6.8,16.8C264.8,397.2,258.4,399.6,252.4,399.6z M252.4,360
	c-4,0-8.4,1.6-11.2,4.8c-2.8,2.8-4.4,6.8-4.4,11.2s1.6,8.4,4.4,11.2c2.8,2.8,7.2,4.8,11.2,4.8s8.4-1.6,11.2-4.8
	c2.8-2.8,4.8-7.2,4.8-11.2s-1.6-8-4.8-11.2C260.4,361.6,256.4,360,252.4,360z"/>
</svg>
)";
static const std::string WARNING = R"(
<?xml version="1.0" encoding="iso-8859-1"?>
<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg height="800px" width="800px" version="1.1" id="Layer_1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" 
	 viewBox="0 0 507.425 507.425" xml:space="preserve">
<path style="fill:#FFC52F;" d="M329.312,129.112l13.6,22l150.8,242.4c22.4,36,6,65.2-36.8,65.2h-406.4c-42.4,0-59.2-29.6-36.8-65.6
	l198.8-320.8c22.4-36,58.8-36,81.2,0L329.312,129.112z"/>
<g>
	<path style="fill:#F4EFEF;" d="M253.712,343.512c-10.8,0-20-8.8-20-20v-143.2c0-10.8,9.2-20,20-20s20,8.8,20,20v143.2
		C273.712,334.312,264.512,343.512,253.712,343.512z"/>
	<path style="fill:#F4EFEF;" d="M253.312,407.112c-5.2,0-10.4-2-14-6c-3.6-3.6-6-8.8-6-14s2-10.4,6-14c3.6-3.6,8.8-6,14-6
		s10.4,2,14,6c3.6,3.6,6,8.8,6,14s-2,10.4-6,14C263.712,404.712,258.512,407.112,253.312,407.112z"/>
</g>
<path d="M456.912,465.512h-406.4c-22,0-38.4-7.6-46-21.6s-5.6-32.8,6-51.2l198.8-321.6c11.6-18.8,27.2-29.2,44.4-29.2l0,0
	c16.8,0,32.4,10,43.6,28.4l35.2,56.4l0,0l13.6,22l150.8,243.6c11.6,18.4,13.6,37.2,6,51.2
	C495.312,457.912,478.912,465.512,456.912,465.512z M253.312,49.912L253.312,49.912c-14,0-27.2,8.8-37.6,25.2l-198.8,321.6
	c-10,16-12,31.6-5.6,43.2s20.4,17.6,39.2,17.6h406.4c18.8,0,32.8-6.4,39.2-17.6c6.4-11.2,4.4-27.2-5.6-43.2l-150.8-243.6l-13.6-22
	l-35.2-56.4C280.512,58.712,267.312,49.912,253.312,49.912z"/>
<path d="M249.712,347.512c-13.2,0-24-10.8-24-24v-143.2c0-13.2,10.8-24,24-24s24,10.8,24,24v143.2
	C273.712,336.712,262.912,347.512,249.712,347.512z M249.712,164.312c-8.8,0-16,7.2-16,16v143.2c0,8.8,7.2,16,16,16s16-7.2,16-16
	v-143.2C265.712,171.512,258.512,164.312,249.712,164.312z"/>
<path d="M249.712,411.112L249.712,411.112c-6.4,0-12.4-2.4-16.8-6.8c-4.4-4.4-6.8-10.8-6.8-16.8c0-6.4,2.4-12.4,6.8-16.8
	c4.4-4.4,10.8-7.2,16.8-7.2c6.4,0,12.4,2.4,16.8,7.2c4.4,4.4,7.2,10.4,7.2,16.8s-2.4,12.4-7.2,16.8
	C262.112,408.312,256.112,411.112,249.712,411.112z M249.712,371.112c-4,0-8.4,1.6-11.2,4.8c-2.8,2.8-4.8,7.2-4.8,11.2
	c0,4.4,1.6,8.4,4.8,11.2c2.8,2.8,7.2,4.8,11.2,4.8s8.4-1.6,11.2-4.8c2.8-2.8,4.8-7.2,4.8-11.2s-1.6-8.4-4.8-11.2
	C258.112,372.712,253.712,371.112,249.712,371.112z"/>
</svg>
)";
}
#include <string>
#include <map>
#include <vector>
#include <cmath>
#include <FL/Fl_Group.H>
#include <FL/Fl_Menu_.H>
#include <FL/Fl_Preferences.H>
#include <FL/Fl_PostScript.H>
#ifdef DEBUG
#include <iostream>
#include <iomanip>
#define FLW_LINE                        { ::printf("\033[31m%6u: \033[34m%s::%s\033[0m\n", __LINE__, __FILE__, __func__); fflush(stdout); }
#define FLW_RED                         { ::printf("\033[7m\033[31m%6u: %s::%s  \033[0m\n", __LINE__, __FILE__, __func__); fflush(stdout); }
#define FLW_GREEN                       { ::printf("\033[7m\033[32m%6u: %s::%s  \033[0m\n", __LINE__, __FILE__, __func__); fflush(stdout); }
#define FLW_YELLOW                      { ::printf("\033[7m\033[33m%6u: %s::%s  \033[0m\n", __LINE__, __FILE__, __func__); fflush(stdout); }
#define FLW_BLUE                        { ::printf("\033[7m\033[34m%6u: %s::%s  \033[0m\n", __LINE__, __FILE__, __func__); fflush(stdout); }
#define FLW_MAGENTA                     { ::printf("\033[7m\033[35m%6u: %s::%s  \033[0m\n", __LINE__, __FILE__, __func__); fflush(stdout); }
#define FLW_CYAN                        { ::printf("\033[7m\033[36m%6u: %s::%s  \033[0m\n", __LINE__, __FILE__, __func__); fflush(stdout); }
#define FLW_RGB                         { ::printf("\033[7m\033[31m%6u: \033[32m%s::\033[34m%s  \033[0m\n", __LINE__, __FILE__, __func__); fflush(stdout); }
#define FLW_PRINT(...)                  FLW_PRINT_MACRO(__VA_ARGS__, FLW_PRINT7, FLW_PRINT6, FLW_PRINT5, FLW_PRINT4, FLW_PRINT3, FLW_PRINT2, FLW_PRINT1)(__VA_ARGS__);
#define FLW_PRINT1(A)                   { std::cout << "\033[31m" << std::setw(6) << __LINE__ << ": \033[34m" << __func__ << ":\033[0m 1=" << (A) << std::endl; fflush(stdout); }
#define FLW_PRINT2(A,B)                 { std::cout << "\033[31m" << std::setw(6) << __LINE__ << ": \033[34m" << __func__ << ":\033[0m 1=" << (A) << ",  \033[32m2=" << (B) << "\033[0m" << std::endl; fflush(stdout); }
#define FLW_PRINT3(A,B,C)               { std::cout << "\033[31m" << std::setw(6) << __LINE__ << ": \033[34m" << __func__ << ":\033[0m 1=" << (A) << ",  \033[32m2=" << (B) << "\033[0m,  3=" << (C) << "" << std::endl; fflush(stdout); }
#define FLW_PRINT4(A,B,C,D)             { std::cout << "\033[31m" << std::setw(6) << __LINE__ << ": \033[34m" << __func__ << ":\033[0m 1=" << (A) << ",  \033[32m2=" << (B) << "\033[0m,  3=" << (C) << ",  \033[32m4=" << (D) << "\033[0m" << std::endl; fflush(stdout); }
#define FLW_PRINT5(A,B,C,D,E)           { std::cout << "\033[31m" << std::setw(6) << __LINE__ << ": \033[34m" << __func__ << ":\033[0m 1=" << (A) << ",  \033[32m2=" << (B) << "\033[0m,  3=" << (C) << ",  \033[32m4=" << (D) << "\033[0m,  5=" << (E) << std::endl; fflush(stdout); }
#define FLW_PRINT6(A,B,C,D,E,F)         { std::cout << "\033[31m" << std::setw(6) << __LINE__ << ": \033[34m" << __func__ << ":\033[0m 1=" << (A) << ",  \033[32m2=" << (B) << "\033[0m,  3=" << (C) << ",  \033[32m4=" << (D) << "\033[0m,  5=" << (E) << ",  \033[32m6=" << (F) << "\033[0m " << std::endl; fflush(stdout); }
#define FLW_PRINT7(A,B,C,D,E,F,G)       { std::cout << "\033[31m" << std::setw(6) << __LINE__ << ": \033[34m" << __func__ << ":\033[0m 1=" << (A) << ",  \033[32m2=" << (B) << "\033[0m,  3=" << (C) << ",  \033[32m4=" << (D) << "\033[0m,  5=" << (E) << ",  \033[32m6=" << (F) << "\033[0m,  7=" << (G) << std::endl; fflush(stdout); }
#define FLW_PRINT_MACRO(A,B,C,D,E,F,G,N,...) N
#define FLW_PRINTV(...)                 FLW_PRINTV_MACRO(__VA_ARGS__, FLW_PRINTV7, FLW_PRINTV6, FLW_PRINTV5, FLW_PRINTV4, FLW_PRINTV3, FLW_PRINTV2, FLW_PRINTV1)(__VA_ARGS__);
#define FLW_PRINTV1(A)                  { std::cout << "\033[31m" << std::setw(6) << __LINE__ << ": \033[34m" << __func__ << ":\033[0m " #A "=" << (A) << "" << std::endl; fflush(stdout); }
#define FLW_PRINTV2(A,B)                { std::cout << "\033[31m" << std::setw(6) << __LINE__ << ": \033[34m" << __func__ << ":\033[0m " #A "=" << (A) << ",  \033[32m" #B "=" << (B) << "\033[0m" << std::endl; fflush(stdout); }
#define FLW_PRINTV3(A,B,C)              { std::cout << "\033[31m" << std::setw(6) << __LINE__ << ": \033[34m" << __func__ << ":\033[0m " #A "=" << (A) << ",  \033[32m" #B "=" << (B) << "\033[0m,  " #C "=" << (C) << "" << std::endl; fflush(stdout); }
#define FLW_PRINTV4(A,B,C,D)            { std::cout << "\033[31m" << std::setw(6) << __LINE__ << ": \033[34m" << __func__ << ":\033[0m " #A "=" << (A) << ",  \033[32m" #B "=" << (B) << "\033[0m,  " #C "=" << (C) << ",  \033[32m" #D "=" << (D) << "\033[0m" << std::endl; fflush(stdout); }
#define FLW_PRINTV5(A,B,C,D,E)          { std::cout << "\033[31m" << std::setw(6) << __LINE__ << ": \033[34m" << __func__ << ":\033[0m " #A "=" << (A) << ",  \033[32m" #B "=" << (B) << "\033[0m,  " #C "=" << (C) << ",  \033[32m" #D "=" << (D) << "\033[0m,  " #E "=" << (E) << "" << std::endl; fflush(stdout); }
#define FLW_PRINTV6(A,B,C,D,E,F)        { std::cout << "\033[31m" << std::setw(6) << __LINE__ << ": \033[34m" << __func__ << ":\033[0m " #A "=" << (A) << ",  \033[32m" #B "=" << (B) << "\033[0m,  " #C "=" << (C) << ",  \033[32m" #D "=" << (D) << "\033[0m,  " #E "=" << (E) << ",  \033[32m" #F "=" << (F) << "\033[0m" << std::endl; fflush(stdout); }
#define FLW_PRINTV7(A,B,C,D,E,F,G)      { std::cout << "\033[31m" << std::setw(6) << __LINE__ << ": \033[34m" << __func__ << ":\033[0m " #A "=" << (A) << ",  \033[32m" #B "=" << (B) << "\033[0m,  " #C "=" << (C) << ",  \033[32m" #D "=" << (D) << "\033[0m,  " #E "=" << (E) << ",  \033[32m" #F "=" << (F) << "\033[0m,  " #G "=" << (G) << "" << std::endl; fflush(stdout); }
#define FLW_PRINTV_MACRO(A,B,C,D,E,F,G,N,...) N
#define FLW_PRINTD(...)                 FLW_PRINTD_MACRO(__VA_ARGS__, FLW_PRINTD4, FLW_PRINTD3, FLW_PRINTD2, FLW_PRINTD1)(__VA_ARGS__);
#define FLW_PRINTD1(A)                  { ::printf("\033[31m%6d: \033[34m%s:\033[0m  %s = %.10f\n", __LINE__, __func__, #A, static_cast<double>(A)); fflush(stdout); }
#define FLW_PRINTD2(A,B)                { ::printf("\033[31m%6d: \033[34m%s:\033[0m  %s = %.10f,  \033[32m%s = %.10f\033[0m\n", __LINE__, __func__, #A, static_cast<double>(A), #B, static_cast<double>(B)); fflush(stdout); }
#define FLW_PRINTD3(A,B,C)              { ::printf("\033[31m%6d: \033[34m%s:\033[0m  %s = %.10f,  \033[32m%s = %.10f\033[0m,  %s = %.10f\n", __LINE__, __func__, #A, static_cast<double>(A), #B, static_cast<double>(B), #C, static_cast<double>(C)); fflush(stdout); }
#define FLW_PRINTD4(A,B,C,D)            { ::printf("\033[31m%6d: \033[34m%s:\033[0m  %s = %.10f,  \033[32m%s = %.10f\033[0m,  %s = %.10f, \033[32m %s = %.10f\033[0m\n", __LINE__, __func__, #A, static_cast<double>(A), #B, static_cast<double>(B), #C, static_cast<double>(C), #D, static_cast<double>(D)); fflush(stdout); }
#define FLW_PRINTD_MACRO(A,B,C,D,N,...) N
#define FLW_PRINTDS(...)                FLW_PRINTDS_MACRO(__VA_ARGS__, FLW_PRINTDS4, FLW_PRINTDS3, FLW_PRINTDS2, FLW_PRINTDS1)(__VA_ARGS__);
#define FLW_PRINTDS1(A)                 { ::printf("\033[31m%6d: \033[34m%s:\033[0m  %s = %s\n", __LINE__, __func__, #A, flw::util::format_double(static_cast<double>(A), 0, '\'').c_str()); fflush(stdout); }
#define FLW_PRINTDS2(A,B)               { ::printf("\033[31m%6d: \033[34m%s:\033[0m  %s = %s,  \033[32m%s = %s\033[0m\n", __LINE__, __func__, #A, flw::util::format_double(static_cast<double>(A), 0, '\'').c_str(), #B, flw::util::format_double(static_cast<double>(B), 0, '\'').c_str()); fflush(stdout); }
#define FLW_PRINTDS3(A,B,C)             { ::printf("\033[31m%6d: \033[34m%s:\033[0m  %s = %s,  \033[32m%s = %s\033[0m,  %s = %s\n", __LINE__, __func__, #A, flw::util::format_double(static_cast<double>(A), 0, '\'').c_str(), #B, flw::util::format_double(static_cast<double>(B), 0, '\'').c_str(), #C, flw::util::format_double(static_cast<double>(C), 0, '\'').c_str()); fflush(stdout); }
#define FLW_PRINTDS4(A,B,C,D)           { ::printf("\033[31m%6d: \033[34m%s:\033[0m  %s = %s,  \033[32m%s = %s\033[0m,  %s = %s,  \033[32m%s = %s\033[0m\n", __LINE__, __func__, #A, flw::util::format_double(static_cast<double>(A), 0, '\'').c_str(), #B, flw::util::format_double(static_cast<double>(B), 0, '\'').c_str(), #C, flw::util::format_double(static_cast<double>(C), 0, '\'').c_str(), #D, flw::util::format_double(static_cast<double>(D), 0, '\'').c_str()); fflush(stdout); }
#define FLW_PRINTDS_MACRO(A,B,C,D,N,...) N
#define FLW_NL                          { ::printf("\n"); fflush(stdout); }
#define FLW_ASSERT(X,Y)                 if ((X) == 0) fl_alert("assert in %s on line %d: %s", __func__, __LINE__, Y);
#define FLW_TEST(X,Y)                   flw::debug::test(X,Y,__LINE__,__func__);
#define FLW_TEST_FLOAT(X,Y,Z)           flw::debug::test(X,Y,Z,__LINE__,__func__);
#define FLW_TEST_TRUE(X)                flw::debug::test(X,__LINE__,__func__);
#else
#define FLW_LINE
#define FLW_RED
#define FLW_GREEN
#define FLW_YELLOW
#define FLW_BLUE
#define FLW_MAGENTA
#define FLW_CYAN
#define FLW_RGB
#define FLW_PRINT(...)
#define FLW_PRINTV(...)
#define FLW_PRINTD(...)
#define FLW_PRINTDS(...)
#define FLW_NL
#define FLW_ASSERT(X,Y)
#define FLW_TEST(X,Y)
#define FLW_TEST_FLOAT(X,Y,Z)
#define FLW_TEST_TRUE(X)
#endif
namespace flw {
typedef std::map<std::string, std::string> StringHash;
typedef std::vector<std::string> StringVector;
typedef std::vector<void*> VoidVector;
typedef std::vector<Fl_Widget*> WidgetVector;
typedef bool (*PrintCallback)(void* data, int pw, int ph, int page);
extern int                      PREF_FIXED_FONT;
extern std::string              PREF_FIXED_FONTNAME;
extern int                      PREF_FIXED_FONTSIZE;
extern Fl_Font                  PREF_FONT;
extern int                      PREF_FONTSIZE;
extern std::string              PREF_FONTNAME;
extern std::vector<char*>       PREF_FONTNAMES;
extern double                   PREF_SCALE_VAL;
extern bool                     PREF_SCALE_ON;
extern std::string              PREF_THEME;
extern const StringVector       PREF_THEMES;
namespace debug {
    void                        print(const Fl_Widget* widget, bool recursive = true);
    void                        print(const Fl_Widget* widget, std::string& indent, bool recursive = true);
    bool                        test(bool val, int line, const char* func);
    bool                        test(const char* ref, const char* val, int line, const char* func);
    bool                        test(int64_t ref, int64_t val, int line, const char* func);
    bool                        test(double ref, double val, double diff, int line, const char* func);
}
namespace menu {
    void                        enable_item(Fl_Menu_* menu, const char* text, bool value);
    Fl_Menu_Item*               get_item(Fl_Menu_* menu, const char* text);
    Fl_Menu_Item*               get_item(Fl_Menu_* menu, void* v);
    bool                        item_value(Fl_Menu_* menu, const char* text);
    void                        set_item(Fl_Menu_* menu, const char* text, bool value);
    void                        setonly_item(Fl_Menu_* menu, const char* text);
}
namespace util {
    void                        center_window(Fl_Window* window, Fl_Window* parent = nullptr);
    double                      clock();
    int                         count_decimals(double number);
    Fl_Widget*                  find_widget(Fl_Group* group, const std::string& label);
    std::string                 fix_menu_string(const std::string& label);
    std::string                 format(const std::string& format, ...);
    std::string                 format_double(double num, int decimals = 0, char del = ' ');
    std::string                 format_int(int64_t num, char del = ' ');
    bool                        icon(Fl_Widget* widget, const std::string& svg_image, unsigned max_size);
    bool                        is_empty(const std::string& string);
    void                        labelfont(Fl_Widget* widget, Fl_Font font = flw::PREF_FONT, int size = flw::PREF_FONTSIZE);
    int64_t                     microseconds();
    int32_t                     milliseconds();
    bool                        png_save(const std::string& opt_name, Fl_Window* window, int X = 0, int Y = 0, int W = 0, int H = 0);
    std::string                 print(const std::string& ps_filename, Fl_Paged_Device::Page_Format format, Fl_Paged_Device::Page_Layout layout, PrintCallback cb, void* data);
    std::string                 print(const std::string& ps_filename, Fl_Paged_Device::Page_Format format, Fl_Paged_Device::Page_Layout layout, PrintCallback cb, void* data, int from, int to);
    std::string                 remove_browser_format(const std::string& text);
    std::string&                replace_string(std::string& string, const std::string& find, const std::string& replace);
    void                        sleep(unsigned milli);
    StringVector                split_string(const std::string& string, const std::string& split);
    std::string                 substr(const std::string& string, std::string::size_type pos, std::string::size_type size = std::string::npos);
    void                        swap_rect(Fl_Widget* w1, Fl_Widget* w2);
    double                      to_double(const std::string& string, double def = INFINITY);
    size_t                      to_doubles(const std::string& string, double numbers[], size_t size);
    int64_t                     to_int(const std::string& string, int64_t def = 0);
    long long                   to_long(const std::string& string, long long def = 0);
    static inline std::string   to_string(const char* text)
                                    { return text != nullptr ? text : ""; }
    void*                       zero_memory(char* mem, size_t size);
class PrintText {
public:
                                PrintText(const std::string& filename,
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
    void                        _check_for_new_page();
    void                        _measure_lw_lh(const std::string& text);
    void                        _print_line(const std::string& line);
    void                        _print_wrapped_line(const std::string& line);
    std::string                 _start();
    std::string                 _stop();
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
}
namespace theme {
    bool                        is_dark();
    bool                        load(const std::string& name);
    Fl_Font                     load_font(const std::string& requested_font);
    void                        load_fonts(bool iso8859_only = true);
    void                        load_icon(Fl_Window* win, int win_resource, const char** xpm_resource = nullptr, const char* name = nullptr);
    Fl_Rect                     load_rect_from_pref(Fl_Preferences& pref, const std::string& basename);
    void                        load_theme_from_pref(Fl_Preferences& pref);
    double                      load_win_from_pref(Fl_Preferences& pref, const std::string& basename, Fl_Window* window, bool show = true, int defw = 800, int defh = 600);
    bool                        parse(int argc, const char** argv);
    void                        save_rect_to_pref(Fl_Preferences& pref, const std::string& basename, const Fl_Rect& rect);
    void                        save_theme_to_pref(Fl_Preferences& pref);
    void                        save_win_to_pref(Fl_Preferences& pref, const std::string& basename, Fl_Window* window);
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
}
#include <FL/Fl_Group.H>
#include <FL/Fl_Menu_Button.H>
#include <FL/Fl_Rect.H>
#include <FL/Fl_Scrollbar.H>
#include <cmath>
#include <optional>
#include <vector>
namespace flw {
namespace chart {
class  Area;
struct Point;
class  Line;
typedef std::vector<Point> PointVector;
typedef std::vector<Line>  LineVector;
typedef std::vector<Area>  AreaVector;
static constexpr const double MAX_VALUE      = 9223372036854775807.0;
static constexpr const double MIN_VALUE      = 0.0000001;
static constexpr const int    MAX_LINE_WIDTH = 14;
static constexpr const int    VERSION        = 6;
static constexpr const int    MIN_TICK       = 3;
static constexpr const int    MAX_TICK       = MAX_LINE_WIDTH * 5;
enum class Algorithm {
    ATR,
    DAY_TO_MONTH,
    DAY_TO_WEEK,
    EXP_MOVING_AVERAGE,
    FIXED,
    MODIFY,
    MOMENTUM,
    MOVING_AVERAGE,
    RSI,
    STD_DEV,
    STOCHASTICS,
    LAST = STOCHASTICS,
};
enum class DateRange {
    DAY,
    WEEKDAY,
    FRIDAY,
    SUNDAY,
    MONTH,
    HOUR,
    MIN,
    SEC,
    LAST = SEC,
};
enum class LineType {
    LINE,
    LINE_DOT,
    BAR,
    BAR_CLAMP,
    BAR_HLC,
    HORIZONTAL,
    EXPAND_VERTICAL,
    EXPAND_HORIZONTAL_ALL,
    EXPAND_HORIZONTAL_FIRST,
    LAST = EXPAND_HORIZONTAL_FIRST,
};
enum class Modifier {
    ADDITION,
    DIVISION,
    MULTIPLICATION,
    SUBTRACTION,
    LAST = SUBTRACTION,
};
enum class AreaNum {
    ONE,
    TWO,
    THREE,
    FOUR,
    FIVE,
    LAST = FIVE,
};
struct Point {
    std::string                 date;
    double                      close;
    double                      high;
    double                      low;
    explicit                    Point();
    explicit                    Point(const std::string& date, double value = 0.0);
    explicit                    Point(const std::string& date, double high, double low, double close);
    void                        debug() const;
    bool                        operator<(const Point& other) const { return date < other.date; }
    bool                        operator<=(const Point& other) const { return date <= other.date; }
    bool                        operator==(const Point& other) const { return date == other.date; }
    bool                        operator!=(const Point& other) const { return date != other.date; }
    static PointVector          ATR(const PointVector& in, size_t days);
    static size_t               BinarySearch(const PointVector& in, const Point& key);
    static PointVector          DateSerie(const std::string& start_date, const std::string& stop_date, DateRange range, const PointVector& block = PointVector());
    static PointVector          DayToMonth(const PointVector& in, bool sum = false);
    static PointVector          DayToWeek(const PointVector& in, gnu::Date::Day weekday, bool sum = false);
    static void                 Debug(const PointVector& in);
    static PointVector          ExponentialMovingAverage(const PointVector& in, size_t days);
    static PointVector          Fixed(const PointVector& in, double value);
    static PointVector          LoadCSV(const std::string& filename, const std::string& sep = ",");
    static PointVector          Modify(const PointVector& in, Modifier modify, double value);
    static PointVector          Momentum(const PointVector& in, size_t days);
    static PointVector          MovingAverage(const PointVector& in, size_t days);
    static std::string          RangeToString(DateRange range);
    static PointVector          RSI(const PointVector& in, size_t days);
    static bool                 SaveCSV(const PointVector& in, const std::string& filename, const std::string& sep = ",");
    static PointVector          StdDev(const PointVector& in, size_t days);
    static PointVector          Stochastics(const PointVector& in, size_t days);
    static DateRange            StringToRange(const std::string& range);
};
class Line {
public:
    explicit                    Line()
                                    { reset(); }
    explicit                    Line(const PointVector& data, const std::string& label = "", LineType type = LineType::LINE);
    Fl_Align                    align() const
                                    { return _align; }
    Fl_Color                    color() const
                                    { return _color; }
    const PointVector&          data() const
                                    { return _data; }
    void                        debug(size_t num) const;
    bool                        is_visible() const
                                    { return _visible; }
    std::string                 label() const
                                    { return _label; }
    const Fl_Rect&              label_rect() const
                                    { return _rect; }
    void                        reset();
    Line&                       set_align(Fl_Align val)
                                    { if (val == FL_ALIGN_LEFT || val == FL_ALIGN_RIGHT) _align = val; return *this; }
    Line&                       set_color(Fl_Color val)
                                    { _color = val; return *this; }
    Line&                       set_data(const PointVector& val)
                                    { _data = val; return *this; }
    Line&                       set_label(const std::string& val)
                                    { _label = val; return *this; }
    Line&                       set_label_rect(int x, int y, int w, int h)
                                    { _rect = Fl_Rect(x, y, w, h); return *this; }
    Line&                       set_type(LineType val)
                                    { _type = val; return *this; }
    Line&                       set_type_from_string(const std::string& val);
    Line&                       set_visible(bool val)
                                    { _visible = val; return *this; }
    Line&                       set_width(unsigned val = 1)
                                    { if (val > 0 && val <= chart::MAX_LINE_WIDTH) _width = val; return *this; }
    size_t                      size() const
                                    { return _data.size(); }
    LineType                    type() const
                                    { return _type; }
    bool                        type_has_high_and_low() const
                                    { return _type == LineType::BAR || _type == LineType::BAR_CLAMP || _type == LineType::BAR_HLC; }
    std::string                 type_to_string() const;
    unsigned                    width() const
                                    { return _width; }
private:
    PointVector                 _data;
    Fl_Align                    _align;
    Fl_Color                    _color;
    Fl_Rect                     _rect;
    LineType                    _type;
    bool                        _visible;
    std::string                 _label;
    unsigned                    _width;
};
class Scale {
public:
                                Scale();
    int                         calc_margin();
    void                        calc_tick(int height);
    void                        debug(const char* name) const;
    double                      diff() const;
    void                        fix_height();
    std::optional<double>       max() const;
    std::optional<double>       min() const;
    double                      pixel() const
                                    { return _pixel; }
    void                        reset();
    void                        set_max(double val)
                                    { _max = val; }
    void                        set_min(double val)
                                    { _min = val; }
    double                      tick() const
                                    { return _tick; }
private:
    double                      _max;
    double                      _min;
    double                      _pixel;
    double                      _tick;
};
class Area {
public:
    static const size_t         MAX_LINES = 10;
    explicit                    Area(AreaNum area)
                                    { _area = area; reset(); }
    bool                        add_line(const Line& chart_line);
    AreaNum                     area() const
                                    { return _area; }
    std::optional<double>       clamp_max() const;
    std::optional<double>       clamp_min() const;
    void                        debug() const;
    void                        delete_line(size_t index);
    Scale&                      left_scale()
                                    { return _left; }
    Line*                       line(size_t index)
                                    { return (index < _lines.size()) ? &_lines[index] : nullptr; }
    const LineVector&           lines() const
                                    { return _lines; }
    int                         percent() const
                                    { return _percent; }
    Fl_Rect&                    rect()
                                    { return _rect; }
    void                        reset();
    Scale&                      right_scale()
                                    { return _right; }
    size_t                      selected() const
                                    { return _selected; }
    Line*                       selected_line();
    void                        set_max_clamp(double val = INFINITY)
                                    { _clamp_max = val; }
    void                        set_min_clamp(double val = INFINITY)
                                    { _clamp_min = val; }
    void                        set_percent(int val)
                                    { _percent = val; }
    void                        set_selected(size_t val)
                                    { _selected = val; }
    size_t                      size() const
                                    { return _lines.size(); }
private:
    AreaNum                     _area;
    LineVector                  _lines;
    Scale                       _left;
    Scale                       _right;
    Fl_Rect                     _rect;
    double                      _clamp_max;
    double                      _clamp_min;
    int                         _percent;
    size_t                      _selected;
};
class Chart : public Fl_Group {
    static const size_t         MAX_VLINES = 1600;
public:
    explicit                    Chart(int X = 0, int Y = 0, int W = 0, int H = 0, const char* l = nullptr);
    double                      alt_size() const
                                    { return _alt_size; }
    Area&                       area(AreaNum area)
                                    { return _areas[static_cast<size_t>(area)]; }
    bool                        create_line(Algorithm formula, bool support = false);
    void                        debug() const;
    void                        debug_line() const;
    void                        disable_menu()
                                    { _disable_menu = true; }
    void                        do_layout()
                                    { _old = Fl_Rect(); resize(x(), y(), w(), h()); redraw(); }
    void                        draw() override;
    void                        enable_menu()
                                    { _disable_menu = false; }
    std::string                 filename() const
                                    { return _filename; }
    int                         handle(int event) override;
    bool                        hor_lines() const
                                    { return _horizontal; }
    void                        init()
                                    { _init(false); }
    void                        init_new_data()
                                    { _init(true); }
    bool                        line_labels() const
                                    { return _labels; }
    bool                        load_json();
    bool                        load_json(const std::string& filename);
    bool                        load_line_from_csv();
    std::string                 main_label() const
                                    { return _label; }
    void                        print_to_postscript();
    void                        reset();
    void                        resize(int X, int Y, int W, int H) override;
    bool                        save_json();
    bool                        save_json(const std::string& filename, double max_diff_high_low = 0.001);
    bool                        save_line_to_csv();
    bool                        save_png();
    void                        set_alt_size(double val = 0.8)
                                    { if (val >= 0.6 && val <= 1.2) _alt_size = val; }
    bool                        set_area_size(unsigned area1 = 100, unsigned area2 = 0, unsigned area3 = 0, unsigned area4 = 0, unsigned area5 = 0);
    void                        set_block_dates(const PointVector& block_dates)
                                    { _block_dates = block_dates; }
    void                        set_date_range(DateRange range = DateRange::DAY)
                                    { _date_range  = range; }
    void                        set_hor_lines(bool val = true)
                                    { _horizontal = val; }
    void                        set_line_labels(bool val = true)
                                    { _labels = val; }
    void                        set_main_label(std::string label = "")
                                    { _label = label; }
    void                        set_tick_width(int val = chart::MIN_TICK)
                                    { if (val >= MIN_TICK && val <= MAX_TICK) _tick_width = val; }
    void                        set_ver_lines(bool val = true)
                                    { _vertical = val; }
    void                        setup_area();
    void                        setup_clamp(bool min = true);
    void                        setup_create_line();
    void                        setup_date_range();
    void                        setup_delete_lines();
    void                        setup_label();
    void                        setup_line();
    void                        setup_move_lines();
    void                        setup_show_or_hide_lines();
    void                        setup_view_options();
    void                        update_pref();
    bool                        ver_lines() const
                                    { return _vertical; }
private:
    enum class LabelType {
                                OFF,
                                ON,
                                VISIBLE,
                                LAST = VISIBLE,
    };
    void                        _calc_area_height();
    void                        _calc_area_width();
    void                        _calc_dates();
    void                        _calc_margins();
    void                        _calc_ymin_ymax();
    void                        _calc_yscale();
    void                        _create_tooltip(bool ctrl);
    void                        _draw_label();
    void                        _draw_lines(Area& area);
    void                        _draw_line_labels(Area& area);
    void                        _draw_tooltip();
    void                        _draw_ver_lines(Area& area);
    void                        _draw_xlabels();
    void                        _draw_ylabels(Area& area, Fl_Align align);
    Area*                       _get_active_area(int X, int Y);
    void                        _init(bool calc_dates);
    StringVector                _label_array(const Area& area, LabelType labeltype) const;
    bool                        _move_or_delete_line(Area* area, size_t index, bool move, AreaNum destination = AreaNum::ONE);
    void                        _show_menu();
    static bool                 _CallbackPrinter(void* data, int pw, int ph, int page);
    static void                 _CallbackScrollbar(Fl_Widget*, void* widget);
    Area*                       _area;
    AreaVector                  _areas;
    DateRange                   _date_range;
    PointVector                 _block_dates;
    PointVector                 _dates;
    Fl_Menu_Button*             _menu;
    Fl_Rect                     _old;
    Fl_Scrollbar*               _scroll;
    bool                        _disable_menu;
    bool                        _horizontal;
    bool                        _labels;
    bool                        _printing;
    bool                        _vertical;
    const int                   _CH;
    const int                   _CW;
    double                      _alt_size;
    int                         _bottom_space;
    int                         _date_start;
    int                         _margin_left;
    int                         _margin_right;
    int                         _tick_width;
    int                         _ticks;
    int                         _top_space;
    int                         _ver_pos[MAX_VLINES];
    std::string                 _filename;
    std::string                 _label;
    std::string                 _tooltip;
};
}
}
#include <FL/Fl_Group.H>
namespace flw {
class GridGroup : public Fl_Group {
public:
    explicit                    GridGroup(int X = 0, int Y = 0, int W = 0, int H = 0, const char* l = nullptr);
                                ~GridGroup();
    void                        add(Fl_Widget* widget, int X, int Y, int W, int H, Fl_Widget* F = nullptr);
    void                        adjust(Fl_Widget* widget, int L = 0, int R = 0, int T = 0, int B = 0);
    void                        clear();
    void                        do_layout()
                                    { resize(x(), y(), w(), h()); Fl::redraw(); }
    int                         handle(int event) override;
    Fl_Widget*                  remove(Fl_Widget* widget);
    void                        resize(int X, int Y, int W, int H) override;
    void                        resize(Fl_Widget* widget, int X, int Y, int W, int H);
    int                         size() const
                                    { return _size; }
    void                        size(int size)
                                    { _size = (size >= 4 && size <= 72) ? size : 0; }
private:
    void                        _last_active_widget(Fl_Widget** first, Fl_Widget** last);
    VoidVector                  _widgets;
    int                         _size;
};
}
#include <FL/Fl_Group.H>
namespace flw {
class ToolGroup : public Fl_Group {
public:
    enum class Pos {
                                HORIZONTAL,
                                VERTICAL,
    };
                                ToolGroup(int X = 0, int Y = 0, int W = 0, int H = 0, const char* l = nullptr);
                                ~ToolGroup();
    Fl_Widget*                  add(Fl_Widget* widget, unsigned size = 0);
    void                        clear();
    void                        do_layout()
                                    { resize(x(), y(), w(), h()); Fl::redraw(); }
    bool                        expand_last() const
                                    { return _expand; }
    void                        expand_last(bool value)
                                    { _expand = value; do_layout(); }
    Pos                         pos() const
                                    { return _pos; }
    void                        pos(Pos pos)
                                    { _pos = pos; do_layout(); }
    Fl_Widget*                  remove(Fl_Widget* widget);
    Fl_Widget*                  replace(Fl_Widget* old_widget, Fl_Widget* new_widget);
    void                        resize(int X, int Y, int W, int H) override;
    void                        size(Fl_Widget* widget, unsigned size);
    void                        size(unsigned size);
private:
    Pos                         _pos;
    VoidVector                  _widgets;
    bool                        _expand;
};
}
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Group.H>
namespace flw {
class DateChooser : public GridGroup {
public:
    explicit                    DateChooser(int X = 0, int Y = 0, int W = 0, int H = 0, const char* l = nullptr);
    void                        draw() override;
    void                        focus();
    gnu::Date                   get() const;
    int                         handle(int event) override;
    void                        set(const gnu::Date& date);
private:
    static void                 _Callback(Fl_Widget* w, void* o);
    void                        _set_label();
    Fl_Box*                     _month_label;
    Fl_Button*                  _b1;
    Fl_Button*                  _b2;
    Fl_Button*                  _b3;
    Fl_Button*                  _b4;
    Fl_Button*                  _b5;
    Fl_Button*                  _b6;
    Fl_Button*                  _b7;
    Fl_Widget*                  _canvas;
    ToolGroup*                  _buttons;
};
}
#include <FL/Fl_Hold_Browser.H>
#include <FL/Fl_Menu_Button.H>
namespace flw {
class ScrollBrowser : public Fl_Hold_Browser {
public:
                                ScrollBrowser(int lines = 9, int X = 0, int Y = 0, int W = 0, int H = 0, const char* l = nullptr);
    void                        disable_menu()
                                    { _flag_menu = false; }
    void                        disable_scrollmode()
                                    { _flag_move = false; }
    void                        enable_menu()
                                    { _flag_menu = true; }
    void                        enable_scrollmode()
                                    { _flag_move = true; }
    bool                        is_menu_enabled() const
                                    { return _flag_menu; }
    bool                        is_scrollmode_enabled() const
                                    { return _flag_move; }
    int                         handle(int event) override;
    Fl_Menu_Button*             menu()
                                    { return _menu; }
    int                         scroll_lines() const
                                    { return _scroll; }
    void                        scroll_lines(int lines)
                                    { if (lines > 0) _scroll = lines; }
    std::string                 text2() const
                                    { return util::remove_browser_format(util::to_string(text(value()))); }
    std::string                 text2(int line) const
                                    { return util::remove_browser_format(util::to_string(text(line))); }
    void                        update_pref(Fl_Font text_font = flw::PREF_FONT, Fl_Fontsize text_size = flw::PREF_FONTSIZE);
    static void                 Callback(Fl_Widget*, void*);
private:
    Fl_Menu_Button*             _menu;
    bool                        _flag_menu;
    bool                        _flag_move;
    int                         _scroll;
};
}
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Hold_Browser.H>
#include <FL/Fl_Hor_Fill_Slider.H>
#include <FL/Fl_Toggle_Button.H>
namespace flw {
namespace dlg {
extern const char*              PASSWORD_CANCEL;
extern const char*              PASSWORD_OK;
void                            center_message_dialog();
bool                            date(const std::string& title, gnu::Date& date, Fl_Window* parent = nullptr, int W = 33, int H = 21);
bool                            font(Fl_Font& font, Fl_Fontsize& fontsize, std::string& fontname, bool limit_to_default = false);
void                            html(const std::string& title, const std::string& text, Fl_Window* parent = nullptr, int W = 40, int H = 23);
void                            list(const std::string& title, const StringVector& list, bool fixed_font = false, Fl_Window* parent = nullptr, int W = 40, int H = 23);
void                            list(const std::string& title, const std::string& list, bool fixed_font = false, Fl_Window* parent = nullptr, int W = 40, int H = 23);
void                            list_file(const std::string& title, const std::string& file, bool fixed_font = false, Fl_Window* parent = nullptr, int W = 40, int H = 23);
void                            panic(const std::string& message);
bool                            password(const std::string& title, std::string& password, Fl_Window* parent = nullptr);
bool                            password_confirm(const std::string& title, std::string& password, Fl_Window* parent = nullptr);
bool                            password_confirm_and_file(const std::string& title, std::string& password, std::string& file, Fl_Window* parent = nullptr);
bool                            password_and_file(const std::string& title, std::string& password, std::string& file, Fl_Window* parent = nullptr);
void                            print(const std::string& title, PrintCallback cb, void* data = nullptr, int from = 1, int to = 0, Fl_Window* parent = nullptr);
bool                            print_text(const std::string& title, const std::string& text, Fl_Window* parent = nullptr);
bool                            print_text(const std::string& title, const StringVector& text, Fl_Window* parent = nullptr);
StringVector                    select_checkboxes(const std::string& title, const StringVector& list, Fl_Window* parent = nullptr);
int                             select_choice(const std::string& title, const StringVector& list, int selected = 0, Fl_Window* parent = nullptr);
int                             select_string(const std::string& title, const StringVector& list, int select_row, bool fixed_font = false, Fl_Window* parent = nullptr, int W = 40, int H = 23);
int                             select_string(const std::string& title, const StringVector& list, const std::string& select_row, bool fixed_font = false, Fl_Window* parent = nullptr, int W = 40, int H = 23);
bool                            slider(const std::string& title, double min, double max, double& value, double step = 1.0, Fl_Window* parent = nullptr);
void                            text(const std::string& title, const std::string& text, Fl_Window* parent = nullptr, int W = 40, int H = 23);
bool                            text_edit(const std::string& title, std::string& text, Fl_Window* parent = nullptr, int W = 40, int H = 23);
void                            theme(bool enable_font = false, bool enable_fixedfont = false, Fl_Window* parent = nullptr);
class Font : public Fl_Double_Window {
public:
                                Font(Fl_Font font, Fl_Fontsize fontsize, const std::string& title = "Select Font", bool limit_to_default = false);
                                Font(const std::string& font, Fl_Fontsize fontsize, const std::string& title = "Select Font", bool limit_to_default = false);
    void                        activate_font()
                                    { static_cast<Fl_Widget*>(_fonts)->activate(); }
    void                        activate_font_size()
                                    { static_cast<Fl_Widget*>(_fonts)->activate(); }
    void                        deactivate_font()
                                    { static_cast<Fl_Widget*>(_fonts)->deactivate(); }
    void                        deactivate_fontsize()
                                    { static_cast<Fl_Widget*>(_sizes)->deactivate(); }
    int                         font()
                                    { return _font; }
    std::string                 fontname()
                                    { return _fontname; }
    int                         fontsize()
                                    { return _fontsize; }
    bool                        run(Fl_Window* parent = nullptr);
private:
    void                        _activate();
    void                        _create(Fl_Font font, const std::string& fontname, Fl_Fontsize fontsize, const std::string& label, bool limit_to_default);
    void                        _select_name(const std::string& font_name);
    static void                 Callback(Fl_Widget* w, void* o);
    Fl_Box*                     _label;
    Fl_Button*                  _cancel;
    Fl_Button*                  _select;
    GridGroup*                  _grid;
    ScrollBrowser*              _fonts;
    ScrollBrowser*              _sizes;
    bool                        _ret;
    bool                        _run;
    int                         _font;
    int                         _fontsize;
    std::string                 _fontname;
};
class Progress : public Fl_Double_Window {
public:
                                Progress(const std::string& title, bool cancel = false, bool pause = false, double min = 0.0, double max = 0.0);
    void                        range(double min, double max);
    void                        start(Fl_Window* parent = nullptr);
    bool                        update(const StringVector& messages, unsigned milli = 100);
    bool                        update(double value, const StringVector& messages, unsigned milli = 100);
    bool                        update(const std::string& message, unsigned milli = 100);
    bool                        update(double value, const std::string& message, unsigned milli = 100);
    double                      value() const
                                    { return _progress->value(); }
    void                        value(double value);
private:
    static void                 Callback(Fl_Widget* w, void* o);
    Fl_Button*                  _cancel;
    Fl_Hold_Browser*            _label;
    Fl_Hor_Fill_Slider*         _progress;
    Fl_Toggle_Button*           _pause;
    GridGroup*                  _grid;
    bool                        _ret;
    unsigned                    _last;
};
}
}
#include <FL/Fl_Group.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Menu_Button.H>
namespace flw {
class _InputMenu;
class InputMenu : public Fl_Group {
public:
                                InputMenu(const InputMenu&) = delete;
                                InputMenu(InputMenu&&) = delete;
    InputMenu&                  operator=(const InputMenu&) = delete;
    InputMenu&                  operator=(InputMenu&&) = delete;
    explicit                    InputMenu(int X = 0, int Y = 0, int W = 0, int H = 0, const char* l = nullptr);
    void                        clear();
    Fl_Input*                   input()
                                    { return reinterpret_cast<Fl_Input*>(_input); }
    void                        insert(const std::string& string, unsigned max_list_len);
    Fl_Menu_Button*             menu()
                                    { return _menu; }
    bool                        menu_visible() const
                                    { return _menu->visible() != 0; }
    void                        menu_visible(bool value)
                                    { if (value == true) _menu->show(); else _menu->hide(); resize(x(), y(), w(), h()); }
    StringVector                get_history() const;
    void                        resize(int X, int Y, int W, int H) override;
    void                        update_pref(Fl_Font text_font = flw::PREF_FONT, Fl_Fontsize text_size = flw::PREF_FONTSIZE);
    std::string                 value() const;
    void                        value(const std::string& string);
    void                        values(const StringVector& list, const std::string& input_value)
                                    { _values(list, input_value); }
    void                        values(const StringVector& list, size_t list_index = -1)
                                    { _values(list, list.size() > list_index ? list[list_index] : ""); }
private:
    void                        _add(bool insert, const std::string& string, int max_list_len);
    void                        _add(bool insert, const StringVector& list);
    void                        _values(const StringVector& list, const std::string& value);
    static void                 _CallbackInput(Fl_Widget*, void*);
    static void                 _CallbackMenu(Fl_Widget*, void*);
    _InputMenu*                 _input;
    Fl_Menu_Button*             _menu;
};
}
#include <FL/Fl_Box.H>
namespace flw {
class LCDDisplay : public Fl_Box {
public:
                                LCDDisplay(int X = 0, int Y = 0, int W = 0, int H = 0, const char* l = nullptr);
    Fl_Align                    align() const
                                    { return _align; }
    void                        align(Fl_Align align)
                                    { _align = align; Fl::redraw(); }
    int                         dot_size() const
                                    { return _dot_size; }
    void                        dot_size(int size)
                                    { _dot_size = size; Fl::redraw(); }
    void                        draw() override;
    Fl_Color                    segment_color() const
                                    { return _seg_color; }
    void                        segment_color(Fl_Color color)
                                    { _seg_color = color; Fl::redraw(); }
    int                         segment_gap() const
                                    { return _seg_gap; }
    void                        segment_gap(int gap)
                                    { _seg_gap = gap; Fl::redraw(); }
    int                         thickness() const
                                    { return _thick; }
    void                        thickness(int thickness)
                                    { _thick = thickness; Fl::redraw(); }
    int                         unit_gap()
                                    { return _unit_gap; }
    void                        unit_gap(int gap)
                                    { _unit_gap = gap; Fl::redraw(); }
    int                         unit_h() const
                                    { return _unit_h; }
    void                        unit_h(int height)
                                    { _unit_h = height; Fl::redraw(); }
    int                         unit_w() const
                                    { return _unit_w; }
    void                        unit_w(int width)
                                    { _unit_w = width; Fl::redraw(); }
    const std::string           value() const
                                    { return _value; }
    void                        value(const std::string& value);
private:
    void                        _draw(unsigned a, int x, int y, int w, int h);
    Fl_Align                    _align;
    Fl_Color                    _seg_color;
    std::string                 _value;
    int                         _dot_size;
    int                         _seg_gap;
    int                         _thick;
    int                         _unit_gap;
    int                         _unit_h;
    int                         _unit_w;
};
}
#include <FL/Fl_Text_Display.H>
#include <string>
namespace flw {
class LogDisplay : public Fl_Text_Display {
public:
    enum class Color {
                                FOREGROUND          = 'A',
                                GRAY                = 'B',
                                RED                 = 'C',
                                GREEN               = 'D',
                                BLUE                = 'E',
                                MAGENTA             = 'F',
                                YELLOW              = 'G',
                                CYAN                = 'H',
                                BOLD_FOREGROUND     = 'I',
                                BOLD_GRAY           = 'J',
                                BOLD_RED            = 'K',
                                BOLD_GREEN          = 'L',
                                BOLD_BLUE           = 'M',
                                BOLD_MAGENTA        = 'N',
                                BOLD_YELLOW         = 'O',
                                BOLD_CYAN           = 'P',
                                U_FOREGROUND        = 'Q',
                                U_GRAY              = 'R',
                                U_RED               = 'S',
                                U_GREEN             = 'T',
                                U_BLUE              = 'U',
                                U_MAGENTA           = 'V',
                                U_YELLOW            = 'W',
                                U_CYAN              = 'X',
                                U_BOLD_FOREGROUND   = 'Y',
                                U_BOLD_GRAY         = 'Z',
                                U_BOLD_RED          = '[',
                                U_BOLD_GREEN        = '\\',
                                U_BOLD_BLUE         = ']',
                                U_BOLD_MAGENTA      = '^',
                                U_BOLD_YELLOW       = '_',
                                U_BOLD_CYAN         = '`',
                                LAST                = U_BOLD_CYAN,
    };
    explicit                    LogDisplay(int X = 0, int Y = 0, int W = 0, int H = 0, const char* l = nullptr);
                                ~LogDisplay();
    void                        edit_styles();
    void                        find(bool next, bool force_ask);
    int                         handle(int event) override;
    void                        lock_colors()
                                    { _lock_colors = true; }
    void                        save_file();
    void                        style(const std::string& json = "");
    void                        unlock_colors()
                                    { _lock_colors = false; }
    void                        update_pref();
    void                        value(const char* text);
    void                        value(const std::string& text)
                                    { value(text.c_str()); }
protected:
    virtual void                line_cb(size_t row, const std::string& line);
    virtual void                line_custom_cb(size_t row, const std::string& line, const std::string& word1, const std::string& word2, LogDisplay::Color color, bool inclusive, size_t start = 0, size_t stop = 0, size_t count = 0);
    void                        style_between(const std::string& line, const std::string& word1, const std::string& word2, bool inclusive, LogDisplay::Color color);
    void                        style_line(size_t start, size_t stop, LogDisplay::Color color);
    void                        style_num(const std::string& line, LogDisplay::Color color, size_t count = 0);
    void                        style_range(const std::string& line, const std::string& word1, const std::string& word2, bool inclusive, LogDisplay::Color color, size_t count = 0);
    void                        style_rstring(const std::string& line, const std::string& word1, LogDisplay::Color color, size_t count = 0);
    void                        style_string(const std::string& line, const std::string& word1, LogDisplay::Color color, size_t count = 0);
private:
    const char*                 _check_text(const char* text);
    char                        _style_char(size_t pos) const
                                    { pos += _tmp->pos; return (pos < _tmp->size) ? _tmp->buf[pos] : 0; }
    struct Tmp {
        char*                   buf;
        size_t                  len;
        size_t                  pos;
        size_t                  size;
                                Tmp();
                                ~Tmp();
    };
    Fl_Text_Buffer*             _buffer;
    std::string                 _find;
    bool                        _lock_colors;
    std::string                 _json;
    Fl_Text_Buffer*             _style;
    Tmp*                        _tmp;
};
}
#include <FL/Fl_Group.H>
#include <FL/Fl_Menu_Button.H>
namespace flw {
namespace plot {
static constexpr const double MAX_VALUE      = 9223372036854775807.0;
static constexpr const double MIN_VALUE      = 0.0000001;
static constexpr const size_t MAX_LINES      = 10;
static constexpr const int    MAX_LINE_WIDTH = 30;
static constexpr const int    VERSION        =  4;
enum class Algorithm {
    MODIFY,
    SWAP,
    LAST = SWAP,
};
enum class Clamp {
    MINX,
    MAXX,
    MINY,
    MAXY,
    LAST = MAXY,
};
enum class Label {
    MAIN,
    X,
    Y,
    LAST = Y,
};
enum class LineType {
    LINE,
    LINE_DASH,
    LINE_DOT,
    LINE_WITH_SQUARE,
    VECTOR,
    CIRCLE,
    FILLED_CIRCLE,
    SQUARE,
    LAST = SQUARE,
};
enum class Modifier {
    ADDITION,
    DIVISION,
    MULTIPLICATION,
    SUBTRACTION,
    LAST = SUBTRACTION,
};
enum class Value {
    X,
    Y,
    X_AND_Y,
    LAST = X_AND_Y,
};
struct Point;
typedef std::vector<Point> PointVector;
struct Point {
    double                      x;
    double                      y;
    explicit                    Point(double X = 0.0, double Y = 0.0);
    void                        debug(int i = -1) const;
    bool                        is_valid() const
                                    { return std::isfinite(x) == true && std::isfinite(y) == true && fabs(x) < plot::MAX_VALUE && fabs(y) < plot::MAX_VALUE; }
    static void                 Debug(const PointVector& in);
    static PointVector          LoadCSV(const std::string& filename, const std::string& sep = ",");
    static bool                 MinMax(const PointVector& in, double& min_x, double& max_x, double& min_y, double& max_y);
    static PointVector          Modify(const PointVector& in, plot::Modifier modify, plot::Value target, double value);
    static bool                 SaveCSV(const PointVector& in, const std::string& filename, const std::string& sep = ",");
    static PointVector          Swap(const PointVector& in);
};
class Line {
public:
                                Line();
                                Line(const PointVector& data, const std::string& label, LineType type = LineType::LINE, Fl_Color color = FL_BLUE, unsigned width = 1);
    Fl_Color                    color() const
                                    { return _color; }
    const PointVector&          data() const
                                    { return _data; }
    void                        debug() const;
    bool                        has_line_type() const
                                    { return _type == LineType::LINE || _type == LineType::LINE_DASH || _type == LineType::LINE_DOT || _type == LineType::LINE_WITH_SQUARE; }
    bool                        has_radius() const
                                    { return _type == LineType::CIRCLE || _type == LineType::FILLED_CIRCLE || _type == LineType::SQUARE; }
    bool                        is_vector() const
                                    { return _type == LineType::VECTOR; }
    bool                        is_visible() const
                                    { return _visible; }
    std::string                 label() const
                                    { return _label; }
    const Fl_Rect&              label_rect() const
                                    { return _rect; }
    void                        reset();
    Line&                       set_color(Fl_Color val)
                                    { _color = val; return *this; }
    Line&                       set_data(const PointVector& val)
                                    { _data = val;  return *this; }
    Line&                       set_label(const std::string& val)
                                    { _label = val; return *this; }
    Line&                       set_label_rect(int x, int y, int w, int h)
                                    { _rect = Fl_Rect(x, y, w, h); return *this; }
    Line&                       set_type(LineType val)
                                    { _type = val; return *this; }
    Line&                       set_type_from_string(const std::string& val);
    Line&                       set_visible(bool val)
                                    { _visible = val; return *this; }
    Line&                       set_width(unsigned val)
                                    { if (val > 0 && val <= plot::MAX_LINE_WIDTH) _width = val; return *this; }
    LineType                    type() const
                                    { return _type; }
    std::string                 type_to_string() const;
    unsigned                    width() const
                                    { return _width; }
private:
    bool                        _visible;
    Fl_Color                    _color;
    Fl_Rect                     _rect;
    LineType                    _type;
    PointVector                 _data;
    std::string                 _label;
    unsigned                    _width;
};
typedef std::vector<Line> LineVector;
class Scale {
public:
                                Scale()
                                    { reset(); }
    void                        calc_tick(double pixels);
    Fl_Color                    color() const
                                    { return _color; }
    const StringVector&         custom_labels() const
                                    { return _labels; }
    void                        debug(const char* s) const;
    int                         fr() const
                                    { return _fr; }
    bool                        is_max_finite() const
                                    { return std::isfinite(_max); }
    bool                        is_min_finite() const
                                    { return std::isfinite(_min); }
    std::string                 label() const
                                    { return _label; }
    int                         label_width() const
                                    { return static_cast<int>(_lwidth); }
    double                      max() const
                                    { return _max; }
    void                        measure_labels(int cw, bool custom);
    double                      min() const
                                    { return _min; }
    double                      pixel() const
                                    { return _pixel; }
    void                        reset();
    void                        reset_min_max()
                                    { _min = _max = INFINITY; }
    void                        set_color(int val)
                                    { _color = val; }
    void                        set_custom_labels(StringVector val)
                                    { _labels = val; }
    void                        set_label(const std::string& val)
                                    { _label = val; }
    void                        set_max(double val)
                                    { _max = val; }
    void                        set_min(double val)
                                    { _min = val; }
    double                      tick() const
                                    { return _tick; }
private:
    Fl_Color                    _color;
    StringVector                _labels;
    double                      _max;
    double                      _min;
    double                      _pixel;
    double                      _tick;
    int                         _fr;
    size_t                      _lwidth;
    std::string                 _label;
};
class Plot : public Fl_Group {
public:
    explicit                    Plot(int X = 0, int Y = 0, int W = 0, int H = 0, const char* l = nullptr);
    bool                        add_line(const Line& line);
    bool                        create_line(Algorithm alg);
    void                        debug() const;
    void                        debug_line() const;
    void                        delete_line(size_t index);
    void                        disable_menu()
                                    { _disable_menu = true; }
    void                        do_layout()
                                    { _old = Fl_Rect(); resize(x(), y(), w(), h()); redraw(); }
    void                        draw() override;
    void                        enable_menu()
                                    { _disable_menu = false; }
    std::string                 filename() const
                                    { return _filename; }
    int                         handle(int event) override;
    void                        init()
                                    { _init(false); }
    void                        init_new_data()
                                    { _init(true); }
    bool                        load_json();
    bool                        load_json(const std::string& filename);
    bool                        load_line_from_csv();
    void                        print_to_postscript();
    void                        reset();
    void                        resize(int X, int Y, int W, int H) override;
    bool                        save_json();
    bool                        save_json(const std::string& filename);
    bool                        save_line_to_csv();
    bool                        save_png();
    void                        set_hor_lines(bool val = true)
                                    { _horizontal = val; }
    void                        set_label(const std::string& label = "")
                                    { _label = label; }
    void                        set_line_labels(bool val = true)
                                    { _labels = val; }
    void                        set_max_x(double val = INFINITY)
                                    { _max_x = val; }
    void                        set_max_y(double val = INFINITY)
                                    { _max_y = val; }
    void                        set_min_x(double val = INFINITY)
                                    { _min_x = val; }
    void                        set_min_y(double val = INFINITY)
                                    { _min_y = val; }
    void                        set_ver_lines(bool val = true)
                                    { _vertical = val; }
    void                        setup_create_line();
    void                        setup_clamp(Clamp clamp);
    void                        setup_delete_lines();
    void                        setup_label(Label val);
    void                        setup_line();
    void                        setup_show_or_hide_lines();
    void                        setup_view_options();
    void                        update_pref();
    Scale&                      xscale()
                                    { return _x; }
    Scale&                      yscale()
                                    { return _y; }
private:
    void                        _calc_min_max();
    StringVector                _create_check_labels(bool def) const;
    void                        _create_tooltip();
    void                        _draw_labels();
    void                        _draw_line_labels();
    void                        _draw_lines();
    void                        _draw_tooltip();
    void                        _draw_xlabels();
    void                        _draw_xlabels_custom();
    void                        _draw_ylabels();
    void                        _draw_ylabels_custom();
    void                        _init(bool new_data);
    void                        _show_menu();
    static void                 _CallbackDebug(Fl_Widget*, void* widget);
    static bool                 _CallbackPrinter(void* data, int pw, int ph, int page);
    static void                 _CallbackToggle(Fl_Widget*, void* widget);
    Fl_Menu_Button*             _menu;
    Fl_Rect                     _area;
    Fl_Rect                     _old;
    LineVector                  _lines;
    Scale                       _x;
    Scale                       _y;
    bool                        _disable_menu;
    bool                        _horizontal;
    bool                        _labels;
    bool                        _printing;
    bool                        _vertical;
    const int                   _CH;
    const int                   _CW;
    double                      _max_x;
    double                      _max_y;
    double                      _min_x;
    double                      _min_y;
    size_t                      _selected_line;
    size_t                      _selected_point;
    std::string                 _filename;
    std::string                 _label;
    std::string                 _tooltip;
};
}
}
#include <FL/Fl_Menu_.H>
#include <FL/Fl_Preferences.H>
namespace flw {
namespace util {
class RecentMenu {
public:
                                RecentMenu(Fl_Menu_* menu, Fl_Callback* callback, void* userdata = nullptr, const std::string& base_label = "&File/Open recent", const std::string& clear_label = "/Clear");
    void                        append(const std::string& item)
                                    { return _add(item, true); }
    void                        insert(const std::string& item)
                                    { return _add(item, false); }
    StringVector                items() const
                                    { return _items; }
    size_t                      max_items() const
                                    { return _max; }
    void                        max_items(size_t max)
                                    { if (max > 0 && max <= 100) _max = max; }
    Fl_Menu_*                   menu()
                                    { return _menu; }
    void                        load_pref(Fl_Preferences& pref, const std::string& base_name = "files");
    void                        save_pref(Fl_Preferences& pref, const std::string& base_name = "files");
    void*                       user_data()
                                    { return _user; }
    void                        user_data(void* data)
                                    { _user = data; }
    static void                 CallbackClear(Fl_Widget*, void* o);
private:
    void                        _add(const std::string& item, bool append);
    size_t                      _add_string(StringVector& items, size_t max_size, const std::string& string);
    size_t                      _insert_string(StringVector& items, size_t max_size, const std::string& string);
    std::string                 _base;
    Fl_Callback*                _callback;
    std::string                 _clear;
    StringVector                _items;
    size_t                      _max;
    Fl_Menu_*                   _menu;
    void*                       _user;
};
}
}
#include <FL/Fl.H>
#include <FL/Fl_Group.H>
namespace flw {
class SplitGroup : public Fl_Group {
public:
    enum class Pos {
                                HORIZONTAL,
                                VERTICAL,
    };
    explicit                    SplitGroup(int X = 0, int Y = 0, int W = 0, int H = 0, const char* l = nullptr);
    void                        add(Fl_Widget* widget, bool first);
    Fl_Widget*                  child(bool first)
                                    { return _widgets[first]; }
    void                        clear();
    void                        do_layout()
                                    { SplitGroup::resize(x(), y(), w(), h()); Fl::redraw(); }
    int                         handle(int event) override;
    void                        hide_child(bool first);
    int                         min_split_pos() const
                                    { return _min_split_pos; }
    void                        min_split_pos(unsigned min_size)
                                    { _min_split_pos = min_size; }
    Pos                         pos() const
                                    { return _pos; }
    void                        pos(Pos pos);
    void                        resize(int X, int Y, int W, int H) override;
    int                         split_pos() const
                                    { return _split_pos; }
    void                        split_pos(int split_pos)
                                    { if (split_pos > 0 || split_pos == -1) _split_pos = split_pos; }
    void                        show_child(bool first);
    void                        swap()
                                    { auto tmp = _widgets[0]; _widgets[0] = _widgets[1]; _widgets[1] = tmp; do_layout(); }
private:
    Pos                         _pos;
    Fl_Widget*                  _widgets[2];
    bool                        _drag;
    int                         _min_split_pos;
    int                         _split_pos;
};
}
#include <map>
#include <FL/Fl_Group.H>
#include <FL/Fl_Scrollbar.H>
namespace flw {
namespace table {
enum class Select {
    NO,
    CELL,
    ROW,
};
enum class Event {
    UNDEFINED,
    CHANGED,
    CURSOR,
    COLUMN,
    COLUMN_CTRL,
    ROW,
    ROW_CTRL,
    SIZE,
    APPEND_ROW,
    APPEND_COLUMN,
    INSERT_ROW,
    INSERT_COLUMN,
    DELETE_ROW,
    DELETE_COLUMN,
};
class Display : public Fl_Group {
    friend class _FindDialog;
public:
    explicit                    Display(int X = 0, int Y = 0, int W = 0, int H = 0, const char* l = nullptr);
    void                        active_cell(int row = -1, int col = -1, bool show = false);
    virtual Fl_Align            cell_align(int row, int col)
                                        { (void) row; (void) col; return FL_ALIGN_LEFT; }
    virtual Fl_Color            cell_color(int row, int col)
                                        { (void) row; (void) col; return FL_BACKGROUND2_COLOR; }
    virtual Fl_Color            cell_textcolor(int row, int col)
                                    { (void) row; (void) col; return FL_FOREGROUND_COLOR; }
    virtual Fl_Font             cell_textfont(int row, int col)
                                        { (void) row; (void) col; return flw::PREF_FONT; }
    virtual Fl_Fontsize         cell_textsize(int row, int col)
                                        { (void) row; (void) col; return flw::PREF_FONTSIZE; }
    virtual std::string         cell_value(int row, int col)
                                        { (void) row; (void) col; return ""; }
    virtual int                 cell_width(int col)
                                        { (void) col; return flw::PREF_FONTSIZE * 6; }
    virtual void                cell_width(int col, int width)
                                        { (void) col; (void) width; }
    int                         column() const
                                    { return _curr_col; }
    int                         columns() const
                                    { return _cols; }
    void                        cmd_copy();
    void                        cmd_find();
    void                        cmd_goto();
    void                        debug() const;
    virtual void                draw() override;
    void                        enable_append_keys(bool val = false)
                                    { _enable_keys = val; }
    Event                       event() const
                                    { return _event; }
    int                         event_col() const
                                    { return _event_col; }
    int                         event_row() const
                                    { return _event_row; }
    void                        expand_last_column(bool expand = false)
                                    { _expand = expand; redraw(); }
    virtual int                 handle(int event) override;
    void                        header(bool row = false, bool col = false);
    int                         height() const
                                    { return _height; }
    void                        height(int height)
                                    { if (height >= 6 && height <= 72) _height = height; }
    void                        lines(bool ver = false, bool hor = false);
    virtual void                reset();
    void                        resize_column_width(bool resize = false)
                                    { _resize = resize; }
    int                         row() const
                                    { return _curr_row; }
    int                         rows() const
                                    { return _rows; }
    void                        scrollbar(bool ver = true, bool hor = true)
                                    { _disable_ver = !ver; _disable_hor = !hor; redraw(); }
    void                        select_mode(Select select = Select::NO)
                                    { _select = select; }
    void                        show_cell(int row, int col);
    virtual void                size(int rows, int cols);
protected:
    enum class Move {
                                DOWN,
                                FIRST_COL,
                                FIRST_ROW,
                                LAST_COL,
                                LAST_ROW,
                                LEFT,
                                PAGE_DOWN,
                                PAGE_UP,
                                RIGHT,
                                SCROLL_DOWN,
                                SCROLL_LEFT,
                                SCROLL_RIGHT,
                                SCROLL_UP,
                                UP,
    };
    int                         _cell_height(int Y = -1);
    int                         _cell_width(int col, int X = -1);
    virtual void                _draw_cell(int row, int col, int X, int Y, int W, int H, bool ver, bool hor, bool current = false);
    void                        _draw_row(int row, int W, int Y, int H);
    void                        _draw_text(const char* string, int X, int Y, int W, int H, Fl_Align align);
    int                         _ev_keyboard_down(bool only_append_insert = false);
    int                         _ev_mouse_click();
    int                         _ev_mouse_drag();
    int                         _ev_mouse_move();
    void                        _get_cell_below_mouse(int& row, int& col);
    virtual std::string         _get_find_value(int row, int col)
                                    { return cell_value(row, col); }
    void                        _move_cursor(Move move);
    void                        _update_scrollbars();
    void                        _set_event(int row, int col, Event event)
                                    { _event_row = row; _event_col = col; _event = event; }
    static void                 _CallbackHor(Fl_Widget* w, void* v);
    static void                 _CallbackVer(Fl_Widget* w, void* v);
    Event                       _event;
    Select                      _select;
    Fl_Scrollbar*               _hor;
    Fl_Scrollbar*               _ver;
    Fl_Widget*                  _edit;
    std::string                 _find;
    bool                        _disable_hor;
    bool                        _disable_ver;
    bool                        _drag;
    bool                        _enable_keys;
    bool                        _expand;
    bool                        _resize;
    bool                        _show_col_header;
    bool                        _show_hor_lines;
    bool                        _show_row_header;
    bool                        _show_ver_lines;
    int                         _cols;
    int                         _curr_col;
    int                         _curr_row;
    int                         _current_cell[4];
    int                         _event_col;
    int                         _event_row;
    int                         _height;
    int                         _resize_col;
    int                         _rows;
    int                         _start_col;
    int                         _start_row;
};
}
}
namespace flw {
namespace table {
enum class Format {
    DEFAULT,
    INT_DEF,
    INT_SEP,
    DEC_DEF,
    DEC_0,
    DEC_1,
    DEC_2,
    DEC_3,
    DEC_4,
    DEC_5,
    DEC_6,
    DEC_7,
    DEC_8,
    DATE_DEF,
    DATE_WORLD,
    DATE_US,
    SECRET_DEF,
    SECRET_DOT,
};
enum class Type {
    TEXT,
    INTEGER,
    NUMBER,
    BOOLEAN,
    SECRET,
    CHOICE,
    ICHOICE,
    SLIDER,
    VSLIDER,
    COLOR,
    FILE,
    DIR,
    DATE,
    LIST,
    MTEXT,
};
extern std::string EditColorLabel;
extern std::string EditDateLabel;
extern std::string EditDirLabel;
extern std::string EditFileLabel;
extern std::string EditListLabel;
extern std::string EditTextLabel;
std::string format_slider(double val, double min, double max, double step);
class Editor : public Display {
    using Display::cell_value;
public:
    explicit                    Editor(int X = 0, int Y = 0, int W = 0, int H = 0, const char* l = nullptr);
    void                        send_changed_event_always(bool value)
                                    { _force_events = value; }
    virtual StringVector        cell_choice(int row, int col)
                                    { (void) row; (void) col; return StringVector(); }
    virtual bool                cell_edit(int row, int col)
                                    { (void) row; (void) col; return false; }
    virtual Format              cell_format(int row, int col)
                                    { (void) row; (void) col; return Format::DEFAULT; }
    virtual Type                cell_type(int row, int col)
                                    { (void) row; (void) col; return Type::TEXT; }
    virtual bool                cell_value(int row, int col, const std::string& value)
                                    { (void) row; (void) col; (void) value; return false; }
    void                        cmd_cut();
    void                        cmd_delete();
    void                        cmd_paste();
    int                         handle(int event) override;
    void                        reset() override;
protected:
    void                        _draw_cell(int row, int col, int X, int Y, int W, int H, bool ver, bool hor, bool current = false) override;
    void                        _edit_create();
    void                        _edit_quick(const std::string& key);
    void                        _edit_show_dlg();
    void                        _edit_start(const std::string& key = "");
    void                        _edit_stop(bool save = true);
    int                         _ev_keyboard_down2();
    int                         _ev_mouse_click2();
    int                         _ev_paste();
    std::string                 _get_find_value(int row, int col) override;
    Fl_Widget*                  _edit2;
    Fl_Widget*                  _edit3;
    bool                        _force_events;
};
}
}
#include <string>
namespace flw {
namespace table {
class Table : public Editor {
    friend class                _TableChoice;
public:
                                Table(int rows, int cols, int X = 0, int Y = 0, int W = 0, int H = 0, const char* l = nullptr);
    Fl_Align                    cell_align(int row, int col) override
                                    { return static_cast<Fl_Align>(_get_int(_cell_align, row, col, Editor::cell_align(row, col))); }
    void                        cell_align(int row, int col, Fl_Align align)
                                    { _set_int(_cell_align, row, col, (int) align); }
    StringVector                cell_choice(int row, int col) override;
    void                        cell_choice(int row, int col, const std::string& value)
                                    { _set_string(_cell_choice, row, col, value); }
    Fl_Color                    cell_color(int row, int col) override
                                    { return (Fl_Color) _get_int(_cell_color, row, col, Editor::cell_color(row, col)); }
    void                        cell_color(int row, int col, Fl_Color color)
                                    { _set_int(_cell_color, row, col, (int) color); }
    bool                        cell_edit(int row, int col) override
                                    { return static_cast<bool>(_get_int(_cell_edit, row, col, 0)); }
    void                        cell_edit(int row, int col, bool value)
                                    { _set_int(_cell_edit, row, col, static_cast<int>(value)); }
    Format                      cell_format(int row, int col) override
                                    { return static_cast<Format>(_get_int(_cell_format, row, col, static_cast<int>(Format::DEFAULT))); }
    void                        cell_format(int row, int col, Format value)
                                    { _set_int(_cell_format, row, col, (int) value); }
    Type                        cell_type(int row, int col) override
                                    { return static_cast<Type>(_get_int(_cell_type, row, col, static_cast<int>(Type::TEXT))); }
    void                        cell_type(int row, int col, Type type)
                                    { _set_int(_cell_type, row, col, static_cast<int>(type)); }
    Fl_Color                    cell_textcolor(int row, int col) override
                                    { return static_cast<Fl_Color>(_get_int(_cell_textcolor, row, col, Editor::cell_textcolor(row, col))); }
    void                        cell_textcolor(int row, int col, Fl_Color color)
                                    { _set_int(_cell_textcolor, row, col, static_cast<int>(color)); }
    std::string                 cell_value(int row, int col) override
                                    { return _get_string(_cell_value, row, col); }
    bool                        cell_value(int row, int col, const std::string& value) override
                                    { _set_string(_cell_value, row, col, value); return true; }
    int                         cell_width(int col) override
                                    { return _get_int(_cell_width, 0, col, flw::PREF_FONTSIZE * 8); }
    void                        cell_width(int col, int width) override
                                    { _set_int(_cell_width, 0, col, width); }
    void                        reset();
    virtual void                size(int rows, int cols) override;
protected:
    virtual int                 _get_int(StringHash& hash, int row, int col, int def = 0);
    virtual std::string         _get_key(int row, int col);
    virtual std::string         _get_string(StringHash& hash, int row, int col, const std::string& def = "");
    virtual void                _set_int(StringHash& hash, int row, int col, int value);
    virtual void                _set_string(StringHash& hash, int row, int col, const std::string& value)
                                    { hash[_get_key(row, col)] = value; }
    StringHash                  _cell_align;
    StringHash                  _cell_choice;
    StringHash                  _cell_color;
    StringHash                  _cell_edit;
    StringHash                  _cell_format;
    StringHash                  _cell_textcolor;
    StringHash                  _cell_type;
    StringHash                  _cell_value;
    StringHash                  _cell_width;
    StringVector                _cell_choices;
};
}
}
#include <FL/Fl_Flex.H>
#include <FL/Fl_Scroll.H>
namespace flw {
class TabsGroup : public Fl_Group {
public:
    static const int            DEFAULT_SPACE             =  2;
    static const int            DEFAULT_MAX_HOR_TAB_WIDTH = 12;
    static const int            DEFAULT_VER_TAB_WIDTH     = 10;
    static const int            HEIGHT                    =  8;
    static const int            MAX_SPACE                 = 20;
    static const int            MIN_WIDTH                 =  4;
    enum class Pos {
                                TOP,
                                BOTTOM,
                                LEFT,
                                RIGHT,
                                TOP2,
                                BOTTOM2,
                                LEFT2,
                                RIGHT2,
                                LAST = RIGHT2,
    };
    explicit                    TabsGroup(int X = 0, int Y = 0, int W = 0, int H = 0, const char* l = nullptr);
    void                        add(const std::string& label, Fl_Widget* widget, const Fl_Widget* after =  nullptr, const std::string& tooltip = "");
    void                        border(int n = 0, int s = 0, int w = 0, int e = 0)
                                    { _n = n; _s = s; _w = w; _e = e; do_layout(); }
    Fl_Widget*                  child(int index) const;
    int                         children() const
                                    { return static_cast<int>(_widgets.size()); }
    void                        clear();
    void                        debug(bool all = true) const;
    void                        disable_keyboard()
                                    { _keyboard = false; }
    void                        do_layout()
                                    { TabsGroup::resize(x(), y(), w(), h()); Fl::redraw(); }
    void                        enable_keyboard()
                                    { _keyboard = true; }
    int                         find(const Fl_Widget* widget) const;
    int                         handle(int event) override;
    void                        hide_tabs();
    void                        insert(const std::string& label, Fl_Widget* widget, const Fl_Widget* before = nullptr, const std::string& tooltip = "");
    bool                        is_tabs_bottom() const
                                    { return _tab_pos == Pos::BOTTOM || _tab_pos == Pos::BOTTOM2; }
    bool                        is_tabs_left() const
                                    { return _tab_pos == Pos::LEFT || _tab_pos == Pos::LEFT2; }
    bool                        is_tabs_horizontal() const
                                    { return _tab_pos == Pos::TOP || _tab_pos == Pos::TOP2 || _tab_pos == Pos::BOTTOM || _tab_pos == Pos::BOTTOM2; }
    bool                        is_tabs_right() const
                                    { return _tab_pos == Pos::RIGHT || _tab_pos == Pos::RIGHT2; }
    bool                        is_tabs_top() const
                                    { return _tab_pos == Pos::TOP || _tab_pos == Pos::TOP2; }
    bool                        is_tabs_visible() const
                                    { return _tabs->visible(); }
    bool                        is_tabs_vertical() const
                                    { return _tab_pos == Pos::LEFT || _tab_pos == Pos::LEFT2 || _tab_pos == Pos::RIGHT || _tab_pos == Pos::RIGHT2; }
    void                        max_top_width(unsigned characters = TabsGroup::DEFAULT_MAX_HOR_TAB_WIDTH)
                                    { if (characters >= TabsGroup::MIN_WIDTH && characters <= 100) _width2 = characters; }
    Fl_Widget*                  remove(int index);
    Fl_Widget*                  remove(Fl_Widget* widget)
                                    { return TabsGroup::remove(find(widget)); }
    void                        resize(int X, int Y, int W, int H) override;
    void                        show_tabs();
    void                        sort(bool ascending = true, bool casecompare = false);
    void                        tab_box(Fl_Boxtype up_box = FL_MAX_BOXTYPE, Fl_Boxtype down_box = FL_MAX_BOXTYPE);
    void                        tab_color(Fl_Color color = FL_SELECTION_COLOR);
    std::string                 tab_label(const Fl_Widget* widget);
    void                        tab_label(const std::string& label, Fl_Widget* widget);
    Pos                         tab_pos() const
                                    { return _tab_pos; }
    void                        tab_pos(Pos pos, int space = TabsGroup::DEFAULT_SPACE);
    int                         swap(int from, int to);
    std::string                 tooltip(Fl_Widget* widget) const;
    void                        tooltip(const std::string& label, Fl_Widget* widget);
    void                        update_pref(Fl_Font font = flw::PREF_FONT, Fl_Fontsize fontsize = flw::PREF_FONTSIZE);
    Fl_Widget*                  value() const;
    void                        value(int num);
    void                        value(Fl_Widget* widget)
                                    { _activate(widget); }
    static void                 CallbackButton(Fl_Widget* sender, void* object);
    static void                 CallbackScrollbar(Fl_Widget* sender, void* object);
    static const char*          Help();
private:
    void                        _activate(Fl_Widget* widget);
    Fl_Widget*                  _active_button();
    void                        _resize_active_widget();
    void                        _resize_left_right(int X, int Y, int W, int H);
    void                        _resize_top_bottom(int X, int Y, int W, int H);
    Fl_Boxtype                  _down_box;
    Fl_Boxtype                  _up_box;
    Fl_Color                    _color;
    Fl_Group*                   _tabs;
    Fl_Rect                     _area;
    Fl_Scrollbar*               _scroll;
    Pos                         _tab_pos;
    WidgetVector                _widgets;
    bool                        _keyboard;
    bool                        _drag;
    int                         _active1;
    int                         _active2;
    int                         _e;
    int                         _n;
    int                         _s;
    int                         _space;
    int                         _visible;
    int                         _w;
    int                         _width1;
    int                         _width2;
};
}
namespace flw {
class WaitCursor {
    static WaitCursor*          WAITCURSOR;
public:
                                WaitCursor();
                                ~WaitCursor();
};
}
#endif
