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
    enum class COMPARE {
                                YYYYMM,
                                YYYYMMDD,
                                YYYYMMDDHH,
                                YYYYMMDDHHMM,
                                YYYYMMDDHHMMSS,
                                LAST = YYYYMMDDHHMMSS,
    };
    enum class DAY {
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
    enum class FORMAT {
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
                                LAST = WEEKDAY_MONTH_YEAR_SHORT,
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
    int                         compare(const Date& other, Date::COMPARE flag = Date::COMPARE::YYYYMMDDHHMMSS) const;
    int                         day() const
                                    { return _day; }
    int                         days_in_month() const;
    int                         days_into_year() const;
    void                        debug() const;
    int                         diff_days(const Date& other) const;
    int                         diff_months(const Date& other) const;
    int                         diff_seconds(const Date& other) const;
    std::string                 format(Date::FORMAT format = Date::FORMAT::ISO) const;
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
    Date&                       set_weekday(Date::DAY weekday);
    Date&                       set_year(int year);
    int64_t                     time() const;
    int                         week() const;
    Date::DAY                   weekday() const;
    const char*                 weekday_name() const;
    const char*                 weekday_name_short() const;
    int                         year() const
                                    { return _year; }
    static inline bool          Compare(const Date& a, const Date& b)
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
typedef bool (*CallbackCopy)(int64_t size, int64_t copied, void* data); ///< @brief Callback for file copy.
typedef std::vector<File> Files;
enum class TYPE {
                                MISSING,    ///< @brief File does not exist.
                                DIR,        ///< @brief It is a directory.
                                FILE,       ///< @brief A regular file.
                                OTHER,      ///< @brief Something else (not used in windows),
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
                                    { _str = nullptr; _size = 0; } ///< @brief Create empty buffer with NULL data.
    explicit                    Buf(size_t size);
                                Buf(const char* buffer, size_t size);
                                Buf(const Buf& b);
                                Buf(Buf&& b)
                                    { _str = b._str; _size = b._size; b._str = nullptr; } ///< @brief Move buffer.
                                Buf(const std::string& string)
                                    { _str = nullptr; add(string.c_str(), string.length()); } ///< @brief Copy input string.
    virtual                     ~Buf()
                                    { free(_str); } ///< @brief Free memory.
    unsigned char&              operator[](size_t index)
                                    { if (index >= _size || _str == nullptr) throw std::string("error: gnu::file::Buf::[]: index is out of range"); return ((unsigned char*) _str)[index]; } ///< @brief Return byte. @throws std::string exception on error.
    unsigned char               operator[](size_t index) const
                                    { if (index >= _size || _str == nullptr) throw std::string("error: gnu::file::Buf::[]: index is out of range"); return ((unsigned char*) _str)[index]; } ///< @brief Return byte. @throws std::string exception on error.
    Buf&                        operator=(const Buf& b)
                                    { return set(b._str, b._size); } ///< @brief Copy other buffer.
    Buf&                        operator=(Buf&& b)
                                    { free(_str); _str = b._str; _size = b._size; b._str = nullptr; return *this; } ///< @brief Move data to this object.
    Buf&                        operator=(const std::string& string)
                                    { free(_str); _str = nullptr; add(string.c_str(), string.length()); return *this; } ///< @brief Copy string.
    Buf&                        operator+=(const Buf& b)
                                    { return add(b._str, b._size); } ///< @brief Add buffer. @throws std::string exception on error.
    bool                        operator==(const Buf& other) const;
    bool                        operator!=(const Buf& other) const
                                    { return (*this == other) == false; } ///< @brief Compare buffer objects.
    Buf&                        add(const char* buffer, size_t size);
    const char*                 c_str() const
                                    { return _str; } ///< @brief Return buffer data, can be NULL.
    void                        clear()
                                    { free(_str); _str = nullptr; _size = 0; } ///< @brief Delete memory and set internal buffer to NULL.
    std::array<size_t, 257>     count() const
                                    { return Buf::Count(_str, _size); } ///< @brief Count bytes and longest text line.
    void                        debug() const
                                    { printf("gnu::Buf(0x%p, %llu)\n", _str, (long long unsigned) _size); } ///< @brief Print debug info.
    uint64_t                    fletcher64() const
                                    { return file::fletcher64(_str, _size); } ///< @brief Return checksum for this object.
    Buf&                        grab(char* buffer, size_t size)
                                    { free(_str); _str = buffer; _size = size; return *this; } ///< @brief Delete internal memory and take control of input buffer.
    Buf                         insert_cr(bool dos = true, bool trailing = false) const
                                    { return Buf::InsertCR(_str, _size, dos, trailing); } ///< @brief Insert "\r" and remove trailing whitespace. @throws std::string exception on error.
    char*                       release()
                                    { auto res = _str; _str = nullptr; _size = 0; return res; } ///< @brief Release control of internal buffer and return memory (internal memory will be set to NULL).
    Buf                         remove_cr() const
                                    { return Buf::RemoveCR(_str, _size); } ///< @brief Remove "\r" from buffer. @throws std::string exception on error.
    Buf&                        set(const char* buffer, size_t size);
    size_t                      size() const
                                    { return _size; } ///< @brief Return size in bytes.
    void                        size(size_t size)
                                    {  if (size >= _size) throw std::string("error: gnu::file::Buf::size(): size is out of range"); _size = size; } ///< @brief Set new size, must be less or equal than current size. @throws std::string exception on error.
    char*                       str()
                                    { return _str; } ///< @brief Return buffer data, can be NULL.
    bool                        write(const std::string& path, bool flush = true) const;
    static std::array<size_t, 257> Count(const char* buffer, size_t size);
    static inline Buf           Grab(char* string)
                                    { auto res = Buf(); res._str = string; res._size = strlen(string); return res; } ///< @brief Create new object that takes control of input string.
    static inline Buf           Grab(char* buffer, size_t size)
                                    { auto res = Buf(); res._str = buffer; res._size = size; return res; } ///< @brief Create new object that takes control of input buffer.
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
                                    { return _filename == other._filename; } ///< @brief Compare filenames.
    bool                        operator<(const File& other) const
                                    { return _filename < other._filename; } ///< @brief Compare filenames.
    bool                        operator<=(const File& other) const
                                    { return _filename <= other._filename; } ///< @brief Compare filenames.
    const char*                 c_str() const
                                    { return _filename.c_str(); } ///< @brief Return filename.
    File                        canonical() const
                                    { return file::canonical(_filename); } ///< @brief Return canonical file name.
    int64_t                     ctime() const
                                    { return _ctime; } ///< @brief Return created file time.
    void                        debug(bool short_version = true) const
                                    { printf("%s\n", to_string(short_version).c_str()); fflush(stdout); } ///< @brief Print file info to stdout.
    bool                        exist() const
                                    { return _type != TYPE::MISSING; } ///< @brief Does file exist?
    const std::string&          ext() const
                                    { return _ext; } ///< @brief Return file extension, not for directories.
    const std::string&          filename() const
                                    { return _filename; } ///< @brief Return full filename.
    bool                        is_circular() const
                                    { return file::is_circular(_filename); } ///< @brief Is link to a directory a circular one?
    bool                        is_dir() const
                                    { return _type == TYPE::DIR; } ///< @brief Is file a directory?
    bool                        is_file() const
                                    { return _type == TYPE::FILE; } ///< @brief Is file a plain file?
    bool                        is_link() const
                                    { return _link; } ///< @brief Is file a link?
    bool                        is_missing() const
                                    { return _type == TYPE::MISSING; }  ///< @brief Is file missing?
    bool                        is_other() const
                                    { return _type == TYPE::OTHER; }  ///< @brief Is file something else?
    File                        linkname() const
                                    { return file::linkname(_filename); } ///< @brief Read link name.
    int                         mode() const
                                    { return _mode; } ///< @brief Return file mode.
    int64_t                     mtime() const
                                    { return _mtime; } ///< @brief Return file modified time.
    const std::string&          name() const
                                    { return _name; } ///< @brief Return name without path.
    std::string                 name_without_ext() const;
    const std::string&          parent() const
                                    { return _path; } ///< @brief Return parent path.
    const std::string&          path() const
                                    { return _path; } ///< @brief Same as parent(), return parent path.
    int64_t                     size() const
                                    { return _size; } ///< @brief Return size in bytes.
    std::string                 to_string(bool short_version = true) const;
    TYPE                        type() const
                                    { return _type; } ///< @brief Return file type.
    std::string                 type_name() const;
private:
    TYPE                        _type;
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
                                OBJECT,     ///< @brief Object node.
                                ARRAY,      ///< @brief Array node
                                STRING,     ///< @brief Plain string.
                                NUMBER,     ///< @brief Number.
                                BOOL,       ///< @brief Boolean true or false.
                                NIL,        ///< @brief NULL data.
                                ERR,        ///< @brief Error value.
};
enum class ENCODE : uint8_t {
                                DEFAULT,    ///< @brief Indentation.
                                TRIM,       ///< @brief No indentation.
                                FLAT,       ///< @brief No indentation and no newlines.
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
                                    { return _type == type; } ///< @brief Compare type.
    bool                        operator!=(TYPE type) const
                                    { return _type != type; } ///< @brief Compare type.
    const JS*                   operator[](const std::string& name) const
                                    { return _get_value(name.c_str(), true); } ///< @brief Get named child value from object. @param[in] name  Object name.
    const JS*                   operator[](size_t index) const
                                    { return (_type == TYPE::ARRAY && index < _va->size()) ? (*_va)[index] : nullptr; } ///< @brief Get child value from array. @param[in] index  Array index.
    void                        debug() const;
    std::string                 err() const
                                    { return (_type == TYPE::ERR) ? _vs : ""; } ///< @brief Error string.
    const char*                 err_c() const
                                    { return (_type == TYPE::ERR) ? _vs : ""; } ///< @brief Error string.
    const JS*                   find(const std::string& name, bool rec = false) const;
    const JS*                   get(const std::string& name, bool escape_name = true) const
                                    { return _get_value(name.c_str(), escape_name); } ///< @brief Get named child value from object.
    const JS*                   get(size_t index) const
                                    { return (*this) [index]; } ///< @brief Get child value from array.
    bool                        has_err() const
                                    { return _type == TYPE::ERR; } ///< @brief Has node error?
    bool                        has_inline() const
                                    { return _inl; } ///< @brief Is inline on?
    bool                        is_array() const
                                    { return _type == TYPE::ARRAY; } ///< @brief Is value an array?
    bool                        is_bool() const
                                    { return _type == TYPE::BOOL; } ///< @brief Is value a bool?
    bool                        is_null() const
                                    { return _type == TYPE::NIL; } ///< @brief Is value a null?
    bool                        is_number() const
                                    { return _type == TYPE::NUMBER; } ///< @brief Is value a number?
    bool                        is_object() const
                                    { return _type == TYPE::OBJECT; } ///< @brief Is value an object?
    bool                        is_string() const
                                    { return _type == TYPE::STRING; } ///< @brief Is value a a string?
    std::string                 name() const
                                    { return (_name != nullptr) ? _name : ""; } ///< @brief Name of value.
    const char*                 name_c() const
                                    { return (_name != nullptr) ? _name : ""; } ///< @brief Name of value.
    std::string                 name_u() const
                                    { return (_name != nullptr) ? json::unescape(_name) : ""; } ///< @brief Unescaped name of value.
    JS*                         parent() const
                                    { return _parent; } ///< @brief Parent object.
    unsigned                    pos() const
                                    { return _pos; } ///< @brief Position in json buffer.
    size_t                      size() const
                                    { return (is_array() == true) ? _va->size() : (is_object() == true) ? _vo->size() : 0; } ///< @brief Size of array or object.
    std::string                 to_string() const;
    TYPE                        type() const
                                    { return _type; } ///< @brief Type of value.
    std::string                 type_name() const
                                    { return TYPE_NAMES[static_cast<unsigned>(_type)]; } ///< @brief Type name for this value.
    const JSArray*              va() const
                                    { return (_type == TYPE::ARRAY) ? _va : nullptr; } ///< @brief Array node or NULL.
    bool                        vb() const
                                    { assert(_type == TYPE::BOOL); return (_type == TYPE::BOOL) ? _vb : false; } ///< @brief Bool value or false.
    double                      vn() const
                                    { assert(_type == TYPE::NUMBER); return (_type == TYPE::NUMBER) ? _vn : 0.0; } ///< @brief Number value or 0.0.
    long long int               vn_i() const
                                    { assert(_type == TYPE::NUMBER); return (_type == TYPE::NUMBER) ? (long long int) _vn : 0; } ///< @brief Number value as integer or 0.
    const JSObject*             vo() const
                                    { return (_type == TYPE::OBJECT) ? _vo : nullptr; } ///< @brief Object value or NULL.
    const JSArray               vo_to_va() const;
    std::string                 vs() const
                                    { assert(_type == TYPE::STRING); return (_type == TYPE::STRING) ? _vs : ""; } ///< @brief String value or "".
    const char*                 vs_c() const
                                    { assert(_type == TYPE::STRING); return (_type == TYPE::STRING) ? _vs : ""; } ///< @brief String value or "".
    std::string                 vs_u() const
                                    { assert(_type == TYPE::STRING); return (_type == TYPE::STRING) ? json::unescape(_vs) : ""; } ///< @brief Unescaped string value or "".
    static inline ssize_t       Count()
                                    { return JS::COUNT; } ///< @brief Number of allocated values
    static inline ssize_t       Max()
                                    { return JS::MAX; } ///< @brief Maximum number of allocated values during runtime.
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
                                    { _root = _current = nullptr; } ///< @brief Create new empty builder.
    virtual                     ~Builder()
                                    { delete _root; } ///< @brief Delete all values.
    Builder&                    operator<<(JS* json)
                                    { return add(json); } ///< @brief Add json value to current parent.
    Builder&                    add(JS* json);
    void                        clear()
                                    { delete _root; _root = _current = nullptr; _name = ""; } ///< @brief Delete all values.
    std::string                 encode(ENCODE option = ENCODE::DEFAULT) const;
    Builder&                    end();
    const JS*                   root() const
                                    { return _root; } ///< @brief Get root value.
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
#include <string>
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
#define FLW_BLUE                        { ::printf("\033[7m\033[34m%6u: %s::%s  \033[0m\n", __LINE__, __FILE__, __func__); fflush(stdout); }
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
#define FLW_BLUE
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
extern int                      PREF_FIXED_FONT;                        ///< @brief Fixed font - default FL_COURIER.
extern std::string              PREF_FIXED_FONTNAME;                    ///< @brief Fixed font name - default "FL_COURIER".
extern int                      PREF_FIXED_FONTSIZE;                    ///< @brief Fixed font size - default 14.
extern Fl_Font                  PREF_FONT;                              ///< @brief Default font - default FL_HELVETICA.
extern int                      PREF_FONTSIZE;                          ///< @brief Default font size - default 14.
extern std::string              PREF_FONTNAME;                          ///< @brief Default font name - default "FL_HELVETICA".
extern std::vector<char*>       PREF_FONTNAMES;                         ///< @brief List of font names - used internally - load with flw::theme::load_fonts().
extern double                   PREF_SCALE_VAL;                         ///< @brief Scale value.
extern bool                     PREF_SCALE_ON;                          ///< @brief Scale on or off.
extern std::string              PREF_THEME;                             ///< @brief Name of theme - default "default".
extern const char* const        PREF_THEMES[];                          ///< @brief Name of themes.
typedef std::vector<std::string> StringVector;                          ///< @brief Vector with strings.
typedef std::vector<void*>       VoidVector;                            ///< @brief Vector with void pointers
typedef std::vector<Fl_Widget*>  WidgetVector;                          ///< @brief Vector with widget pointers.
typedef bool (*PrintCallback)(void* data, int pw, int ph, int page);    ///< @brief A drawing callback for printing to postscript.
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
    Fl_Widget*                  find_widget(Fl_Group* group, std::string label);
    std::string                 fix_menu_string(std::string in);
    std::string                 format(const char* format, ...);
    std::string                 format_double(double num, int decimals = 0, char del = ' ');
    std::string                 format_int(int64_t num, char del = ' ');
    bool                        is_whitespace_or_empty(const char* str);
    void                        labelfont(Fl_Widget* widget, Fl_Font fn = flw::PREF_FONT, int fs = flw::PREF_FONTSIZE);
    int64_t                     microseconds();
    int32_t                     milliseconds();
    bool                        png_save(std::string opt_name, Fl_Window* window, int X = 0, int Y = 0, int W = 0, int H = 0);
    std::string                 print(std::string ps_filename, Fl_Paged_Device::Page_Format format, Fl_Paged_Device::Page_Layout layout, PrintCallback cb, void* data);
    std::string                 print(std::string ps_filename, Fl_Paged_Device::Page_Format format, Fl_Paged_Device::Page_Layout layout, PrintCallback cb, void* data, int from, int to);
    std::string                 remove_browser_format(const char* text);
    std::string&                replace_string(std::string& string, std::string find, std::string replace);
    void                        sleep(int milli);
    StringVector                split_string(const std::string& string, std::string split);
    std::string                 substr(std::string in, std::string::size_type pos, std::string::size_type size = std::string::npos);
    void                        swap_rect(Fl_Widget* w1, Fl_Widget* w2);
    double                      to_double(std::string s, double def = INFINITY);
    long long                   to_long(std::string s, long long def = 0);
    static inline std::string   to_string(const char* text)
                                    { return text != nullptr ? text : ""; }
    void*                       zero_memory(char* mem, size_t size);
}
namespace theme {
    bool                        is_dark();
    bool                        load(const std::string& name);
    int                         load_font(const std::string& requested_font);
    void                        load_fonts(bool iso8859_only = true);
    void                        load_icon(Fl_Window* win, int win_resource, const char** xpm_resource = nullptr, const char* name = nullptr);
    void                        load_rect_pref(Fl_Preferences& pref, Fl_Rect& rect, const std::string& basename);
    void                        load_theme_pref(Fl_Preferences& pref);
    double                      load_win_pref(Fl_Preferences& pref, Fl_Window* window, bool show = true, int defw = 800, int defh = 600, const std::string& basename = "gui.");
    bool                        parse(int argc, const char** argv);
    void                        save_rect_pref(Fl_Preferences& pref, const Fl_Rect& rect, const std::string& basename);
    void                        save_theme_pref(Fl_Preferences& pref);
    void                        save_win_pref(Fl_Preferences& pref, Fl_Window* window, const std::string& basename = "gui.");
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
}
#include <FL/Fl_Group.H>
#include <FL/Fl_Menu_Button.H>
#include <FL/Fl_Rect.H>
#include <FL/Fl_Scrollbar.H>
#include <cmath>
#include <optional>
#include <vector>
namespace flw {
class  ChartArea;
struct ChartData;
class  ChartLine;
typedef std::vector<ChartData>  ChartDataVector;
typedef std::vector<ChartLine>  ChartLineVector;
typedef std::vector<ChartArea>  ChartAreaVector;
struct ChartData {
    static constexpr const double MAX_VALUE = 9223372036854775807.0;
    static constexpr const double MIN_VALUE = 0.0000001;
    enum class RANGE {
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
    enum class FORMULAS {
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
    enum class MODIFY {
                                ADDITION,
                                DIVISION,
                                MULTIPLICATION,
                                SUBTRACTION,
                                LAST = SUBTRACTION,
    };
    std::string                 date;
    double                      close;
    double                      high;
    double                      low;
    explicit                    ChartData();
    explicit                    ChartData(std::string date_value, double value = 0.0);
    explicit                    ChartData(std::string date, double high, double low, double close);
    void                        debug() const;
    bool                        operator<(const ChartData& other) const { return date < other.date; }
    bool                        operator<=(const ChartData& other) const { return date <= other.date; }
    bool                        operator==(const ChartData& other) const { return date == other.date; }
    bool                        operator!=(const ChartData& other) const { return date != other.date; }
    static ChartDataVector      ATR(const ChartDataVector& in, size_t days);
    static size_t               BinarySearch(const ChartDataVector& in, const ChartData& key);
    static ChartDataVector      DateSerie(std::string start_date, std::string stop_date, ChartData::RANGE range, const ChartDataVector& block = ChartDataVector());
    static ChartDataVector      DayToMonth(const ChartDataVector& in, bool sum = false);
    static ChartDataVector      DayToWeek(const ChartDataVector& in, gnu::Date::DAY weekday, bool sum = false);
    static void                 Debug(const ChartDataVector& in);
    static ChartDataVector      ExponentialMovingAverage(const ChartDataVector& in, size_t days);
    static ChartDataVector      Fixed(const ChartDataVector& in, double value);
    static ChartDataVector      LoadCSV(std::string filename, std::string sep = ",");
    static ChartDataVector      Modify(const ChartDataVector& in, ChartData::MODIFY modify, double value);
    static ChartDataVector      Momentum(const ChartDataVector& in, size_t days);
    static ChartDataVector      MovingAverage(const ChartDataVector& in, size_t days);
    static std::string          RangeToString(ChartData::RANGE range);
    static ChartDataVector      RSI(const ChartDataVector& in, size_t days);
    static bool                 SaveCSV(const ChartDataVector& in, std::string filename, std::string sep = ",");
    static ChartDataVector      StdDev(const ChartDataVector& in, size_t days);
    static ChartDataVector      Stochastics(const ChartDataVector& in, size_t days);
    static ChartData::RANGE     StringToRange(std::string range);
};
class ChartLine {
public:
    static const int            MAX_WIDTH = 14;
    enum class TYPE {
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
    explicit                    ChartLine()
                                    { reset(); }
    explicit                    ChartLine(const ChartDataVector& data, std::string label = "", TYPE type = ChartLine::TYPE::LINE);
    Fl_Align                    align() const
                                    { return _align; }
    Fl_Color                    color() const
                                    { return _color; }
    const ChartDataVector&      data() const
                                    { return _data; }
    void                        debug(size_t num) const;
    bool                        is_visible() const
                                    { return _visible; }
    std::string                 label() const
                                    { return _label; }
    const Fl_Rect&              label_rect() const
                                    { return _rect; }
    void                        reset();
    ChartLine&                  set_align(Fl_Align val)
                                    { if (val == FL_ALIGN_LEFT || val == FL_ALIGN_RIGHT) _align = val; return *this; }
    ChartLine&                  set_color(Fl_Color val)
                                    { _color = val; return *this; }
    ChartLine&                  set_data(const ChartDataVector& val)
                                    { _data = val; return *this; }
    ChartLine&                  set_label(std::string val)
                                    { _label = val; return *this; }
    ChartLine&                  set_label_rect(int x, int y, int w, int h)
                                    { _rect = Fl_Rect(x, y, w, h); return *this; }
    ChartLine&                  set_type(TYPE val)
                                    { _type = val; return *this; }
    ChartLine&                  set_type_from_string(std::string val);
    ChartLine&                  set_visible(bool val)
                                    { _visible = val; return *this; }
    ChartLine&                  set_width(unsigned val = 1)
                                    { if (val > 0 && val <= ChartLine::MAX_WIDTH) _width = val; return *this; }
    size_t                      size() const
                                    { return _data.size(); }
    TYPE                        type() const
                                    { return _type; }
    bool                        type_has_hlc() const
                                    { return _type == TYPE::BAR || _type == TYPE::BAR_CLAMP || _type == ChartLine::TYPE::BAR_HLC; }
    std::string                 type_to_string() const;
    unsigned                    width() const
                                    { return _width; }
private:
    ChartDataVector             _data;
    Fl_Align                    _align;
    Fl_Color                    _color;
    Fl_Rect                     _rect;
    TYPE                        _type;
    bool                        _visible;
    std::string                 _label;
    unsigned                    _width;
};
class ChartScale {
public:
                                ChartScale();
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
class ChartArea {
public:
    static const size_t         MAX_LINES = 10;
    enum class AREA {
                                ONE,
                                TWO,
                                THREE,
                                FOUR,
                                FIVE,
                                LAST = FIVE,
    };
    explicit                    ChartArea(ChartArea::AREA area)
                                    { _area = area; reset(); }
    bool                        add_line(const ChartLine& chart_line);
    AREA                        area() const
                                    { return _area; }
    std::optional<double>       clamp_max() const;
    std::optional<double>       clamp_min() const;
    void                        debug() const;
    void                        delete_line(size_t index);
    ChartScale&                 left_scale()
                                    { return _left; }
    ChartLine*                  line(size_t index)
                                    { return (index < _lines.size()) ? &_lines[index] : nullptr; }
    const ChartLineVector&      lines() const
                                    { return _lines; }
    int                         percent() const
                                    { return _percent; }
    Fl_Rect&                    rect()
                                    { return _rect; }
    void                        reset();
    ChartScale&                 right_scale()
                                    { return _right; }
    size_t                      selected() const
                                    { return _selected; }
    ChartLine*                  selected_line();
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
    AREA                        _area;
    ChartLineVector             _lines;
    ChartScale                  _left;
    ChartScale                  _right;
    Fl_Rect                     _rect;
    double                      _clamp_max;
    double                      _clamp_min;
    int                         _percent;
    size_t                      _selected;
};
class Chart : public Fl_Group {
    static const size_t         MAX_VLINES = 1400;
public:
    static const int            VERSION  =    5;
    static const int            MIN_TICK =    3;
    static const int            MAX_TICK = ChartLine::MAX_WIDTH * 5;
    explicit                    Chart(int X = 0, int Y = 0, int W = 0, int H = 0, const char* l = nullptr);
    double                      alt_size() const
                                    { return _alt_size; }
    ChartArea&                  area(ChartArea::AREA area)
                                    { return _areas[static_cast<size_t>(area)]; }
    bool                        create_line(ChartData::FORMULAS formula, bool support = false);
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
    bool                        load_csv();
    bool                        load_json();
    bool                        load_json(std::string filename);
    std::string                 main_label() const
                                    { return _label; }
    void                        print_to_postscript();
    void                        reset();
    void                        resize(int X, int Y, int W, int H) override;
    bool                        save_csv();
    bool                        save_json();
    bool                        save_json(std::string filename, double max_diff_high_low = 0.001) const;
    bool                        save_png();
    void                        set_alt_size(double val = 0.8)
                                    { if (val >= 0.6 && val <= 1.2) _alt_size = val; }
    bool                        set_area_size(unsigned area1 = 100, unsigned area2 = 0, unsigned area3 = 0, unsigned area4 = 0, unsigned area5 = 0);
    void                        set_block_dates(const ChartDataVector& block_dates)
                                    { _block_dates = block_dates; }
    void                        set_date_range(ChartData::RANGE range = ChartData::RANGE::DAY)
                                    { _date_range  = range; }
    void                        set_hor_lines(bool val = true)
                                    { _horizontal = val; }
    void                        set_line_labels(bool val = true)
                                    { _labels = val; }
    void                        set_main_label(std::string label = "")
                                    { _label = label; }
    void                        set_tick_width(int val = Chart::MIN_TICK)
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
    enum class LABELTYPE {
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
    void                        _draw_lines(ChartArea& area);
    void                        _draw_line_labels(ChartArea& area);
    void                        _draw_tooltip();
    void                        _draw_ver_lines(ChartArea& area);
    void                        _draw_xlabels();
    void                        _draw_ylabels(ChartArea& area, Fl_Align align);
    ChartArea*                  _get_active_area(int X, int Y);
    void                        _init(bool calc_dates);
    StringVector                _label_array(const ChartArea& area, Chart::LABELTYPE labeltype) const;
    bool                        _move_or_delete_line(ChartArea* area, size_t index, bool move, ChartArea::AREA destination = ChartArea::AREA::ONE);
    void                        _show_menu();
    static bool                 _CallbackPrinter(void* data, int pw, int ph, int page);
    static void                 _CallbackScrollbar(Fl_Widget*, void* widget);
    ChartArea*                  _area;
    ChartAreaVector             _areas;
    ChartData::RANGE            _date_range;
    ChartDataVector             _block_dates;
    ChartDataVector             _dates;
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
#include <FL/Fl_Group.H>
namespace flw {
class GridGroup : public Fl_Group {
public:
    explicit                    GridGroup(int X = 0, int Y = 0, int W = 0, int H = 0, const char* l = nullptr);
                                ~GridGroup();
    void                        add(Fl_Widget* widget, int X, int Y, int W, int H);
    void                        adjust(Fl_Widget* widget, int L = 0, int R = 0, int T = 0, int B = 0);
    void                        clear();
    void                        do_layout()
                                    { resize(x(), y(), w(), h()); Fl::redraw(); } ///< @brief Resize all child widgets.
    int                         handle(int event) override;
    Fl_Widget*                  remove(Fl_Widget* widget);
    void                        resize(int X, int Y, int W, int H) override;
    void                        resize(Fl_Widget* widget, int X, int Y, int W, int H);
    int                         size() const
                                    { return _size; } ///< @brief Get grid size. @return Size in pixels.
    void                        size(int size)
                                    { _size = (size >= 4 && size <= 72) ? size : 0; } ///< @brief Set grid size. @param[in] size  From 4 to 72 pixel.
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
    enum class DIRECTION {
                                HORIZONTAL,
                                VERTICAL,
    };
    explicit                    ToolGroup(DIRECTION direction = DIRECTION::HORIZONTAL, int X = 0, int Y = 0, int W = 0, int H = 0, const char* l = nullptr);
                                ~ToolGroup();
    void                        add(Fl_Widget* widget, int SIZE = 0);
    void                        clear();
    DIRECTION                   direction() const
                                    { return _direction; }
    void                        direction(DIRECTION direction)
                                    { _direction = direction; }
    void                        do_layout()
                                    { resize(x(), y(), w(), h()); Fl::redraw(); }
    bool                        expand_last() const
                                    { return _expand; }
    void                        expand_last(bool value)
                                    { _expand = value; }
    Fl_Widget*                  remove(Fl_Widget* widget);
    void                        resize(int X, int Y, int W, int H) override;
    void                        resize(Fl_Widget* widget, int SIZE = 0);
private:
    DIRECTION                   _direction;
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
    void                        set(gnu::Date date);
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
namespace dlg {
bool                            date(const std::string& title, gnu::Date& date, Fl_Window* parent);
}
}
#include <FL/Fl_Hold_Browser.H>
#include <FL/Fl_Menu_Button.H>
namespace flw {
class ScrollBrowser : public Fl_Hold_Browser {
public:
                                ScrollBrowser(const ScrollBrowser&) = delete;
                                ScrollBrowser(ScrollBrowser&&) = delete;
    ScrollBrowser&              operator=(const ScrollBrowser&) = delete;
    ScrollBrowser&              operator=(ScrollBrowser&&) = delete;
    explicit                    ScrollBrowser(int scroll = 9, int X = 0, int Y = 0, int W = 0, int H = 0, const char* l = nullptr);
    bool                        enable_menu() const
                                    { return _flag_menu; }
    void                        enable_menu(bool menu)
                                    { _flag_menu = menu; }
    bool                        enable_pagemove() const
                                    { return _flag_move; }
    void                        enable_pagemove(bool move)
                                    { _flag_move = move; }
    int                         handle(int event) override;
    Fl_Menu_Button*             menu()
                                    { return _menu; }
    std::string                 text2() const
                                    { return util::remove_browser_format(text(value())); }
    std::string                 text2(int line) const
                                    { return util::remove_browser_format(text(line)); }
    void                        update_pref()
                                    { update_pref(flw::PREF_FONT, flw::PREF_FONTSIZE); }
    void                        update_pref(Fl_Font text_font, Fl_Fontsize text_size);
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
bool                            font(Fl_Font& font, Fl_Fontsize& fontsize, std::string& fontname, bool limit_to_default = false);
void                            html(const std::string& title, const std::string& text, Fl_Window* parent = nullptr, int W = 40, int H = 23);
void                            list(const std::string& title, const StringVector& list, Fl_Window* parent = nullptr, bool fixed_font = false, int W = 40, int H = 23);
void                            list(const std::string& title, const std::string& list, Fl_Window* parent = nullptr, bool fixed_font = false, int W = 40, int H = 23);
void                            list_file(const std::string& title, const std::string& file, Fl_Window* parent = nullptr, bool fixed_font = false, int W = 40, int H = 23);
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
class FontDialog : public Fl_Double_Window {
public:
                                FontDialog(const FontDialog&) = delete;
                                FontDialog(FontDialog&&) = delete;
    FontDialog&                 operator=(const FontDialog&) = delete;
    FontDialog&                 operator=(FontDialog&&) = delete;
                                FontDialog(Fl_Font font, Fl_Fontsize fontsize, const std::string& title = "Select Font", bool limit_to_default = false);
                                FontDialog(const std::string& font, Fl_Fontsize fontsize, const std::string& title = "Select Font", bool limit_to_default = false);
    void                        activate_font()
                                    { static_cast<Fl_Widget*>(_fonts)->activate(); } ///< @brief Turn on font list, active by default.
    void                        activate_font_size()
                                    { static_cast<Fl_Widget*>(_fonts)->activate(); } ///< @brief Turn on font size list, active by default.
    void                        deactivate_font()
                                    { static_cast<Fl_Widget*>(_fonts)->deactivate(); } ///< @brief Turn of font list.
    void                        deactivate_fontsize()
                                    { static_cast<Fl_Widget*>(_sizes)->deactivate(); } ///< @brief Turn of font size list.
    int                         font()
                                    { return _font; } ///< @brief Return selected font.
    std::string                 fontname()
                                    { return _fontname; } ///< @brief Return selected font name.
    int                         fontsize()
                                    { return _fontsize; } ///< @brief Return selected font size.
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
class WorkDialog : public Fl_Double_Window {
public:
                                WorkDialog(const std::string& title, bool cancel = false, bool pause = false, double min = 0.0, double max = 0.0);
    void                        range(double min, double max);
    void                        start(Fl_Window* parent = nullptr);
    bool                        update(const StringVector& messages, unsigned milli = 100);
    bool                        update(double value, const StringVector& messages, unsigned milli = 100);
    bool                        update(const std::string& message, unsigned milli = 100);
    bool                        update(double value, const std::string& message, unsigned milli = 100);
    double                      value() const
                                    { return _progress->value(); } ///< @brief Return progress bar value.
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
    bool                        enable_menu() const
                                    { return _menu->visible() != 0; }
    void                        enable_menu(bool value)
                                    { if (value == true) _menu->show(); else _menu->hide(); resize(x(), y(), w(), h()); }
    StringVector                get_history() const;
    Fl_Input*                   input()
                                    { return reinterpret_cast<Fl_Input*>(_input); }
    void                        insert(std::string string, int max_list_len);
    Fl_Menu_Button*             menu()
                                    { return _menu; }
    void                        resize(int X, int Y, int W, int H) override;
    void                        update_pref(Fl_Font text_font = flw::PREF_FONT, Fl_Fontsize text_size = flw::PREF_FONTSIZE);
    const char*                 value() const;
    void                        value(const char* string);
    void                        values(const StringVector& list, bool copy_first_to_input = true);
    static void                 Callback(Fl_Widget*, void*);
private:
    void                        _add(bool insert, const std::string& string, int max_list_len);
    void                        _add(bool insert, const StringVector& list);
    _InputMenu*                 _input;
    Fl_Menu_Button*             _menu;
};
}
#include <FL/Fl_Box.H>
namespace flw {
    class LcdNumber : public Fl_Box {
    public:
        explicit                        LcdNumber(int X = 0, int Y = 0, int W = 0, int H = 0, const char* l = nullptr);
        Fl_Align                        align() const
                                            { return _align; }
        void                            align(Fl_Align align)
                                            { _align = align; Fl::redraw(); }
        int                             dot_size() const
                                            { return _dot_size; }
        void                            dot_size(int size)
                                            { _dot_size = size; Fl::redraw(); }
        void                            draw() override;
        Fl_Color                        segment_color() const
                                            { return _seg_color; }
        void                            segment_color(Fl_Color color)
                                            { _seg_color = color; Fl::redraw(); }
        int                             segment_gap() const
                                            { return _seg_gap; }
        void                            segment_gap(int gap)
                                            { _seg_gap = gap; Fl::redraw(); }
        int                             thickness() const
                                            { return _thick; }
        void                            thickness(int thickness)
                                            { _thick = thickness; Fl::redraw(); }
        int                             unit_gap()
                                            { return _unit_gap; }
        void                            unit_gap(int gap)
                                            { _unit_gap = gap; Fl::redraw(); }
        int                             unit_h() const
                                            { return _unit_h; }
        void                            unit_h(int height)
                                            { _unit_h = height; Fl::redraw(); }
        int                             unit_w() const
                                            { return _unit_w; }
        void                            unit_w(int width)
                                            { _unit_w = width; Fl::redraw(); }
        const char*                     value() const
                                            { return _value; }
        void                            value(const char* value);
    private:
        void                            _draw_seg(uchar a, int x, int y, int w, int h);
        Fl_Align                        _align;
        Fl_Color                        _seg_color;
        char                            _value[100];
        int                             _dot_size;
        int                             _seg_gap;
        int                             _thick;
        int                             _unit_gap;
        int                             _unit_h;
        int                             _unit_w;
    };
}
#include <FL/Fl_Text_Display.H>
#include <string>
namespace flw {
class LogDisplay : public Fl_Text_Display {
public:
    enum COLOR {
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
                                BG_FOREGROUND       = 'Q',
                                BG_GRAY             = 'R',
                                BG_RED              = 'S',
                                BG_GREEN            = 'T',
                                BG_BLUE             = 'U',
                                BG_MAGENTA          = 'V',
                                BG_YELLOW           = 'W',
                                BG_CYAN             = 'X',
                                BG_BOLD_FOREGROUND  = 'Y',
                                BG_BOLD_GRAY        = 'Z',
                                BG_BOLD_RED         = '[',
                                BG_BOLD_GREEN       = '\\',
                                BG_BOLD_BLUE        = ']',
                                BG_BOLD_MAGENTA     = '^',
                                BG_BOLD_YELLOW      = '_',
                                BG_BOLD_CYAN        = '`',
                                LAST                = '`',
    };
    explicit                    LogDisplay(int X = 0, int Y = 0, int W = 0, int H = 0, const char* l = nullptr);
                                ~LogDisplay();
    void                        edit_styles();
    void                        find(bool next, bool force_ask);
    int                         handle(int event) override;
    void                        lock_colors(bool lock)
                                    { _lock_colors = lock; }
    void                        save_file();
    void                        style(std::string json = "");
    void                        update_pref();
    void                        value(const char* text);
protected:
    virtual void                line_cb(size_t row, const std::string& line)
                                    { (void) row; (void) line; }
    virtual void                line_custom_cb(size_t row, const std::string& line, const std::string& word1, const std::string& word2, LogDisplay::COLOR color, bool inclusive, size_t start = 0, size_t stop = 0, size_t count = 0)
                                    { (void) row; (void) line; (void) word1; (void) word2; (void) color;  (void) inclusive;  (void) start;  (void) stop;  (void) count; }
    void                        style_between(const std::string& line, const std::string& word1, const std::string& word2, bool inclusive, LogDisplay::COLOR color);
    void                        style_line(size_t start, size_t stop, LogDisplay::COLOR c);
    void                        style_num(const std::string& line, LogDisplay::COLOR color, size_t count = 0);
    void                        style_range(const std::string& line, const std::string& word1, const std::string& word2, bool inclusive, LogDisplay::COLOR color, size_t count = 0);
    void                        style_rstring(const std::string& line, const std::string& word1, LogDisplay::COLOR color, size_t count = 0);
    void                        style_string(const std::string& line, const std::string& word1, LogDisplay::COLOR color, size_t count = 0);
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
struct PlotData;
typedef std::vector<PlotData> PlotDataVector;
struct PlotData {
    static constexpr const double MAX_VALUE = 9223372036854775807.0;
    static constexpr const double MIN_VALUE = 0.0000001;
    enum class FORMULAS {
                                MODIFY,
                                SWAP,
                                LAST = SWAP,
    };
    enum class MODIFY {
                                ADDITION,
                                DIVISION,
                                MULTIPLICATION,
                                SUBTRACTION,
                                LAST = SUBTRACTION,
    };
    enum class TARGET {
                                X,
                                Y,
                                X_AND_Y,
    };
    double                      x;
    double                      y;
    explicit                    PlotData(double X = 0.0, double Y = 0.0);
    void                        debug(int i = -1) const;
    bool                        is_valid() const
                                    { return std::isfinite(x) == true && std::isfinite(y) == true && fabs(x) < PlotData::MAX_VALUE && fabs(y) < PlotData::MAX_VALUE; }
    static void                 Debug(const PlotDataVector& in);
    static PlotDataVector       LoadCSV(std::string filename, std::string sep = ",");
    static bool                 MinMax(const PlotDataVector& in, double& min_x, double& max_x, double& min_y, double& max_y);
    static PlotDataVector       Modify(const PlotDataVector& in, PlotData::MODIFY modify, PlotData::TARGET target, double value);
    static bool                 SaveCSV(const PlotDataVector& in, std::string filename, std::string sep = ",");
    static PlotDataVector       Swap(const PlotDataVector& in);
};
class PlotLine {
public:
    static const int            MAX_WIDTH = 30;
    enum class TYPE {
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
                                PlotLine();
                                PlotLine(const PlotDataVector& data, std::string label, PlotLine::TYPE type = PlotLine::TYPE::LINE, Fl_Color color = FL_BLUE, unsigned width = 1);
    Fl_Color                    color() const
                                    { return _color; }
    const PlotDataVector&       data() const
                                    { return _data; }
    void                        debug() const;
    bool                        has_line_type() const
                                    { return _type == TYPE::LINE || _type == TYPE::LINE_DASH || _type == TYPE::LINE_DOT || _type == TYPE::LINE_WITH_SQUARE; }
    bool                        has_radius() const
                                    { return _type == TYPE::CIRCLE || _type == TYPE::FILLED_CIRCLE || _type == TYPE::SQUARE; }
    bool                        is_vector() const
                                    { return _type == TYPE::VECTOR; }
    bool                        is_visible() const
                                    { return _visible; }
    std::string                 label() const
                                    { return _label; }
    const Fl_Rect&              label_rect() const
                                    { return _rect; }
    void                        reset();
    PlotLine&                   set_color(Fl_Color val)
                                    { _color = val; return *this; }
    PlotLine&                   set_data(const PlotDataVector& val)
                                    { _data = val;  return *this; }
    PlotLine&                   set_label(std::string val)
                                    { _label = val; return *this; }
    PlotLine&                   set_label_rect(int x, int y, int w, int h)
                                    { _rect = Fl_Rect(x, y, w, h); return *this; }
    PlotLine&                   set_type(PlotLine::TYPE val)
                                    { _type = val; return *this; }
    PlotLine&                   set_type_from_string(std::string val);
    PlotLine&                   set_visible(bool val)
                                    { _visible = val; return *this; }
    PlotLine&                   set_width(unsigned val)
                                    { if (val <= PlotLine::MAX_WIDTH) _width = val; return *this; }
    PlotLine::TYPE              type() const
                                    { return _type; }
    std::string                 type_to_string() const;
    unsigned                    width() const
                                    { return _width; }
private:
    Fl_Color                    _color;
    Fl_Rect                     _rect;
    PlotDataVector              _data;
    TYPE                        _type;
    bool                        _visible;
    std::string                 _label;
    unsigned                    _width;
};
typedef std::vector<PlotLine> PlotLineVector;
class PlotScale {
public:
                                PlotScale()
                                    { reset(); }
    void                        calc_tick(double height);
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
    void                        set_label(std::string val)
                                    { _label = val; }
    void                        set_max(double val)
                                    { _max = val; }
    void                        set_min(double val)
                                    { _min = val; }
    int                         text() const
                                    { return static_cast<int>(_text); }
    double                      tick() const
                                    { return _tick; }
private:
    Fl_Color                    _color;
    int                         _fr;
    std::string                 _label;
    StringVector                _labels;
    double                      _max;
    double                      _min;
    double                      _pixel;
    double                      _tick;
    size_t                      _text;
};
class Plot : public Fl_Group {
public:
    static const size_t         MAX_LINES = 10;
    static const int            VERSION   =  3;
    enum class CLAMP {
                                MINX,
                                MAXX,
                                MINY,
                                MAXY,
    };
    enum class LABEL {
                                MAIN,
                                X,
                                Y,
    };
    explicit                    Plot(int X = 0, int Y = 0, int W = 0, int H = 0, const char* l = nullptr);
    bool                        add_line(const PlotLine& line);
    bool                        create_line(PlotData::FORMULAS formula);
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
    bool                        load_csv();
    bool                        load_json();
    bool                        load_json(std::string filename);
    void                        print();
    void                        reset();
    void                        resize()
                                    { size(w(), h()); }
    void                        resize(int X, int Y, int W, int H) override;
    bool                        save_csv();
    bool                        save_json();
    bool                        save_json(std::string filename);
    bool                        save_png();
    void                        set_hor_lines(bool val = true)
                                    { _horizontal = val; }
    void                        set_label(std::string label = "")
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
    void                        setup_add_line();
    void                        setup_clamp(Plot::CLAMP clamp);
    void                        setup_delete_lines();
    void                        setup_label(Plot::LABEL val);
    void                        setup_line();
    void                        setup_show_or_hide_lines();
    void                        setup_view_options();
    void                        update_pref();
    PlotScale&                  xscale()
                                    { return _x; }
    PlotScale&                  yscale()
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
    struct {
    }                           _view;
    Fl_Menu_Button*             _menu;
    Fl_Rect                     _area;
    Fl_Rect                     _old;
    PlotLineVector              _lines;
    PlotScale                   _x;
    PlotScale                   _y;
    bool                        _disable_menu;
    bool                        _horizontal;
    bool                        _labels;
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
#include <FL/Fl_Menu_.H>
#include <FL/Fl_Preferences.H>
namespace flw {
class RecentMenu {
public:
                                    RecentMenu(Fl_Menu_* menu, Fl_Callback* callback, void* userdata = nullptr, const std::string& base_label = "&File/Open recent", const std::string& clear_label = "/Clear");
    void                            append(const std::string& item)
                                        { return _add(item, true); } ///< @brief Append item last in menu. @param[in] item  Menu label.
    void                            insert(const std::string& item)
                                        { return _add(item, false); } ///< @brief Insert item first in menu. @param[in] item  Menu label.
    StringVector                    items() const
                                        { return _items; } ///< @brief Number of items.
    size_t                          max_items() const
                                        { return _max; } ///< @brief Get max number of items.
    void                            max_items(size_t max)
                                        { if (max > 0 && max <= 100) _max = max; } ///< @brief Set max number of items. @param[in] max  From 1 to 100.
    Fl_Menu_*                       menu()
                                        { return _menu; } ///< @brief Get menu object.
    void                            load_pref(Fl_Preferences& pref, const std::string& base_name = "files");
    void                            save_pref(Fl_Preferences& pref, const std::string& base_name = "files");
    void*                           user_data()
                                        { return _user; } ///< @brief Get user data.
    void                            user_data(void* data)
                                        { _user = data; } ///< @brief Set user data.
    static void                     CallbackClear(Fl_Widget*, void* o);
private:
    void                            _add(const std::string& item, bool append);
    size_t                          _add_string(StringVector& items, size_t max_size, const std::string& string);
    size_t                          _insert_string(StringVector& items, size_t max_size, const std::string& string);
    std::string                     _base;
    Fl_Callback*                    _callback;
    std::string                     _clear;
    StringVector                    _items;
    size_t                          _max;
    Fl_Menu_*                       _menu;
    void*                           _user;
};
}
#include <FL/Fl.H>
#include <FL/Fl_Group.H>
namespace flw {
    class SplitGroup : public Fl_Group {
    public:
        enum class CHILD {
                                FIRST,
                                SECOND,
        };
        enum class DIRECTION {
                                HORIZONTAL,
                                VERTICAL,
        };
        explicit                SplitGroup(int X = 0, int Y = 0, int W = 0, int H = 0, const char* l = nullptr);
        void                    add(Fl_Widget* widget, SplitGroup::CHILD child);
        Fl_Widget*              child(SplitGroup::CHILD child)
                                    { return (child == SplitGroup::CHILD::FIRST) ? _widgets[0] : _widgets[1]; }
        void                    clear();
        DIRECTION               direction() const
                                    { return _direction; }
        void                    direction(SplitGroup::DIRECTION direction);
        void                    do_layout()
                                    { SplitGroup::resize(x(), y(), w(), h()); Fl::redraw(); }
        int                     handle(int event) override;
        int                     min_pos() const
                                    { return _min; }
        void                    min_pos(int value)
                                    { _min = value; }
        void                    resize(int X, int Y, int W, int H) override;
        int                     split_pos() const
                                    { return _split_pos; }
        void                    split_pos(int split_pos)
                                    { _split_pos = split_pos; }
        void                    swap()
                                    { auto tmp = _widgets[0]; _widgets[0] = _widgets[1]; _widgets[1] = tmp; do_layout(); }
        void                    toggle(SplitGroup::CHILD child, SplitGroup::DIRECTION direction, int second_size = -1);
        void                    toggle(SplitGroup::CHILD child, int second_size = -1)
                                    { toggle(child, _direction, second_size); }
    private:
        DIRECTION               _direction;
        Fl_Widget*              _widgets[2];
        bool                    _drag;
        int                     _min;
        int                     _split_pos;
    };
}
#include <map>
#include <FL/Fl_Group.H>
#include <FL/Fl_Scrollbar.H>
namespace flw {
class TableDisplay : public Fl_Group {
    friend class _TableDisplayFindDialog;
public:
    static constexpr const char* HELP_TEXT = "Press CTRL + 'g' to show go to cell dialog.\nPress CTRL + 'f' to show find text dialog.";
    enum class SELECT {
                                NO,
                                CELL,
                                ROW,
    };
    enum class EVENT {
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
    explicit                    TableDisplay(int X = 0, int Y = 0, int W = 0, int H = 0, const char* l = nullptr);
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
    virtual const char*         cell_value(int row, int col)
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
    void                        expand_last_column(bool expand = false)
                                    { _expand = expand; redraw(); }
    TableDisplay::EVENT         event() const
                                    { return _event; }
    int                         event_col() const
                                    { return _event_col; }
    int                         event_row() const
                                    { return _event_row; }
    virtual int                 handle(int event) override;
    void                        header(bool row = false, bool col = false);
    int                         height() const
                                    { return _height; }
    void                        height(int height)
                                    { _height = height; }
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
    void                        select_mode(TableDisplay::SELECT select = TableDisplay::SELECT::NO)
                                    { _select = select; }
    void                        show_cell(int row, int col);
    virtual void                size(int rows, int cols);
protected:
    enum class _TABLEDISPLAY_MOVE {
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
    void                        _move_cursor(_TABLEDISPLAY_MOVE move);
    void                        _update_scrollbars();
    void                        _set_event(int row, int col, TableDisplay::EVENT event)
                                    { _event_row = row; _event_col = col; _event = event; }
    static void                 _CallbackHor(Fl_Widget* w, void* v);
    static void                 _CallbackVer(Fl_Widget* w, void* v);
    TableDisplay::EVENT         _event;
    TableDisplay::SELECT        _select;
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
namespace flw {
class TableEditor : public TableDisplay {
    using TableDisplay::cell_value;
public:
    enum class FORMAT {
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
    enum class REND {
                                TEXT,
                                INTEGER,
                                NUMBER,
                                BOOLEAN,
                                SECRET,
                                CHOICE,
                                INPUT_CHOICE,
                                SLIDER,
                                VALUE_SLIDER,
                                DLG_COLOR,
                                DLG_FILE,
                                DLG_DIR,
                                DLG_DATE,
                                DLG_LIST,
    };
    static const char*          SELECT_DATE;
    static const char*          SELECT_DIR;
    static const char*          SELECT_FILE;
    static const char*          SELECT_LIST;
    explicit                    TableEditor(int X = 0, int Y = 0, int W = 0, int H = 0, const char* l = nullptr);
    void                        send_changed_event_always(bool value)
                                    { _send_changed_event_always = value; }
    virtual StringVector        cell_choice(int row, int col)
                                    { (void) row; (void) col; return StringVector(); }
    virtual bool                cell_edit(int row, int col)
                                    { (void) row; (void) col; return false; }
    virtual FORMAT              cell_format(int row, int col)
                                    { (void) row; (void) col; return TableEditor::FORMAT::DEFAULT; }
    virtual REND                cell_rend(int row, int col)
                                    { (void) row; (void) col; return TableEditor::REND::TEXT; }
    virtual bool                cell_value(int row, int col, const char* value)
                                    { (void) row; (void) col; (void) value; return false; }
    void                        cmd_add(int count);
    void                        cmd_cut();
    void                        cmd_delete();
    void                        cmd_paste();
    int                         handle(int event) override;
    void                        reset() override;
    static const char*          FormatSlider(double val, double min, double max, double step);
private:
    bool                        _send_changed_event_always;
    void                        _draw_cell(int row, int col, int X, int Y, int W, int H, bool ver, bool hor, bool current = false) override;
    void                        _edit_create();
    void                        _edit_quick(const char* key);
    void                        _edit_show();
    void                        _edit_start(const char* key = "");
    void                        _edit_stop(bool save = true);
    int                         _ev_keyboard_down2();
    int                         _ev_mouse_click2();
    int                         _ev_paste();
    Fl_Widget*                  _edit2;
    Fl_Widget*                  _edit3;
};
}
#include <string>
#include <map>
namespace flw {
    typedef std::map<std::string, std::string> StringMap;
    class Grid : public TableEditor {
        friend class                    _TableChoice;
    public:
                                        Grid(int rows, int cols, int X = 0, int Y = 0, int W = 0, int H = 0, const char* l = nullptr);
        virtual                         ~Grid();
        Fl_Align                        cell_align(int row, int col) override;
        void                            cell_align(int row, int col, Fl_Align align);
        StringVector                    cell_choice(int row, int col) override;
        void                            cell_choice(int row, int col, const char* value);
        Fl_Color                        cell_color(int row, int col) override;
        void                            cell_color(int row, int col, Fl_Color color);
        bool                            cell_edit(int row, int col) override;
        void                            cell_edit(int row, int col, bool value);
        TableEditor::FORMAT             cell_format(int row, int col) override;
        void                            cell_format(int row, int col, TableEditor::FORMAT value);
        TableEditor::REND               cell_rend(int row, int col) override;
        void                            cell_rend(int row, int col, TableEditor::REND rend);
        Fl_Color                        cell_textcolor(int row, int col) override;
        void                            cell_textcolor(int row, int col, Fl_Color color);
        const char*                     cell_value(int row, int col) override;
        bool                            cell_value(int row, int col, const char* value) override;
        void                            cell_value2(int row, int col, const char* format, ...);
        int                             cell_width(int col) override;
        void                            cell_width(int col, int width) override;
        void                            size(int rows, int cols) override;
    private:
        int                             _get_int(StringMap& map, int row, int col, int def = 0);
        const char*                     _get_key(int row, int col);
        const char*                     _get_string(StringMap& map, int row, int col, const char* def = "");
        void                            _set_int(StringMap& map, int row, int col, int value);
        void                            _set_string(StringMap& map, int row, int col, const char* value);
        char*                           _buffer;
        StringMap                       _cell_align;
        StringMap                       _cell_choice;
        StringVector                    _cell_choices;
        StringMap                       _cell_color;
        StringMap                       _cell_edit;
        StringMap                       _cell_format;
        StringMap                       _cell_rend;
        StringMap                       _cell_textcolor;
        StringMap                       _cell_value;
        StringMap                       _cell_width;
        char                            _key[100];
    };
}
#include <FL/Fl_Pack.H>
#include <FL/Fl_Scroll.H>
namespace flw {
class TabsGroup : public Fl_Group {
public:
    static const int            DEFAULT_SPACE_PX = 2;   ///< @brief Default space between buttons.
    static int                  MIN_WIDTH_NORTH_SOUTH;  ///< @brief Minimum width of top/bottom buttons, default 4 (4 * flw::PREF_FONTSIZE).
    static int                  MIN_WIDTH_EAST_WEST;    ///< @brief Minimum width of left/right buttons, default 4 (4 * flw::PREF_FONTSIZE).
    enum class TABS {
                                NORTH,          ///< @brief Top of container.
                                TOP = NORTH,    ///< @brief Top of container.
                                SOUTH,          ///< @brief Bottom of container.
                                BOTTOM = SOUTH, ///< @brief Bottom of container.
                                WEST,           ///< @brief On the left side of container.
                                LEFT = WEST,    ///< @brief On the left side of container.
                                EAST,           ///< @brief On the right side of the container.
                                RIGHT = EAST,   ///< @brief On the right side of the container.
    };
    explicit                    TabsGroup(int X = 0, int Y = 0, int W = 0, int H = 0, const char* l = nullptr);
    void                        activate(Fl_Widget* widget)
                                    { _activate(widget, false); } ///< @brief Activate and show button and child widget.
    void                        add(const std::string& label, Fl_Widget* widget, const Fl_Widget* after =  nullptr);
    void                        border(int n = 0, int s = 0, int w = 0, int e = 0)
                                    { _n = n; _s = s; _w = w; _e = e; do_layout(); } ///< @brief Set border around active child widget.
    Fl_Widget*                  child(int index) const;
    int                         children() const
                                    { return (int) _widgets.size(); } ///< @brief Return number of child widgets (tab buttons are not included).
    void                        clear();
    void                        debug(bool all = true) const;
    void                        disable_keyboard()
                                    { _disable_k = true; } ///< @brief Disable all keyboard shortcuts.
    void                        do_layout()
                                    { TabsGroup::resize(x(), y(), w(), h()); Fl::redraw(); } ///< @brief Resize all widgets.
    void                        draw() override;
    void                        enable_keyboard()
                                    { _disable_k = false; } ///< @brief Enable all keyboard shortcuts.
    int                         find(const Fl_Widget* widget) const;
    int                         handle(int event) override;
    void                        hide_tabs();
    void                        insert(const std::string& label, Fl_Widget* widget, const Fl_Widget* before = nullptr);
    bool                        is_tabs_visible() const
                                    { return _scroll->visible(); } ///< @brief Is tab buttons visible?
    std::string                 label(Fl_Widget* widget);
    void                        label(const std::string& label, Fl_Widget* widget);
    Fl_Widget*                  remove(int index);
    Fl_Widget*                  remove(Fl_Widget* widget)
                                    { return TabsGroup::remove(find(widget)); } ///< @brief Remove widget and return it.
    void                        resize(int X, int Y, int W, int H) override;
    void                        show_tabs();
    void                        sort(bool ascending = true, bool casecompare = false);
    int                         swap(int from, int to);
    TABS                        tabs() const
                                    { return _tabs; } ///< @brief Pos of the tab buttons (TABS::NORTH, TABS::SOUTH, TABS::EAST, TABS::WEST).
    void                        tabs(TABS value, int space_max_20 = TabsGroup::DEFAULT_SPACE_PX);
    void                        tooltip(const std::string& label, Fl_Widget* widget);
    void                        update_pref(unsigned characters = 10, Fl_Font font = flw::PREF_FONT, Fl_Fontsize fontsize = flw::PREF_FONTSIZE);
    Fl_Widget*                  value() const;
    void                        value(int num);
    void                        value(Fl_Widget* widget)
                                    { value(find(widget)); } ///< @brief Set active child widget.
    static void                 Callback(Fl_Widget* sender, void* object);
    static const char*          Help();
private:
    void                        _activate(Fl_Widget* widget, bool kludge);
    Fl_Widget*                  _active_button();
    void                        _resize_active_widget();
    void                        _resize_east_west(int X, int Y, int W, int H);
    void                        _resize_north_south(int X, int Y, int W, int H);
    Fl_Align                    _align;
    Fl_Pack*                    _pack;
    Fl_Rect                     _area;
    Fl_Scroll*                  _scroll;
    TABS                        _tabs;
    WidgetVector                _widgets;
    bool                        _drag;
    bool                        _disable_k;
    int                         _active1;
    int                         _active2;
    int                         _e;
    int                         _n;
    int                         _pos;
    int                         _s;
    int                         _space;
    int                         _w;
};
}
namespace flw {
    class WaitCursor {
        static WaitCursor*              WAITCURSOR;
    public:
                                        WaitCursor(const WaitCursor&) = delete;
                                        WaitCursor(WaitCursor&&) = delete;
                                        WaitCursor& operator=(const WaitCursor&) = delete;
                                        WaitCursor& operator=(WaitCursor&&) = delete;
                                        WaitCursor();
                                        ~WaitCursor();
    };
}
#endif
