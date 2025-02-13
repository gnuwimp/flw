// Copyright gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#ifndef GNU_JSON_H
#define GNU_JSON_H

// MKALGAM_ON

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
                                DEFAULT,    // Indentation.
                                TRIM,       // No indentation.
                                FLAT,       // No indentation and no newlines.
};

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

/***
 *           _  _____
 *          | |/ ____|
 *          | | (___
 *      _   | |\___ \
 *     | |__| |____) |
 *      \____/|_____/
 *
 *
 */

//------------------------------------------------------------------------------
// JSON object.
//
class JS {
    friend class                Builder;
    friend JS                   decode(const char* json, size_t len, bool ignore_trailing_comma, bool ignore_duplicates, bool ignore_utf_check);
    friend std::string          encode(const JS* js, ENCODE option);

public:
    static const size_t         MAX_DEPTH = 32;

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
    const JS*                   operator[](std::string name) const
                                    { return _get_object(name.c_str(), true); }
    const JS*                   operator[](size_t index) const
                                    { return (_type == TYPE::ARRAY && index < _va->size()) ? (*_va)[index] : nullptr; }
    void                        debug() const;
    std::string                 err() const
                                    { return (_type == TYPE::ERR) ? _vs : ""; }
    const char*                 err_c() const
                                    { return (_type == TYPE::ERR) ? _vs : ""; }
    const JS*                   find(std::string name, bool rec = false) const;
    const JS*                   get(std::string name, bool escape_name = true) const
                                    { return _get_object(name.c_str(), escape_name); }
    const JS*                   get(size_t index) const
                                    { return (*this) [index]; }
    bool                        has_err() const
                                    { return _type == TYPE::ERR; }
    bool                        has_inline() const
                                    { return _inline; }
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
    bool                        _add_nil(char** sVal1, bool ignore_duplicates, unsigned pos);
    bool                        _add_number(char** sVal1, double& nVal, bool ignore_duplicates, unsigned pos);
    bool                        _add_string(char** sVal1, char** sVal2, bool ignore_duplicates, unsigned pos);
    void                        _clear(bool name);
    const JS*                   _get_object(const char* name, bool escape) const;
    void                        _set_err(const std::string& err);
    bool                        _set_object(const char* name, JS* js, bool ignore_duplicates);
    void                        _set_child_parent_to_me();

    static JS*                  _MakeArray(const char* name, JS* parent, unsigned pos);
    static JS*                  _MakeBool(const char* name, bool vb, JS* parent, unsigned pos);
    static JS*                  _MakeNil(const char* name, JS* parent, unsigned pos);
    static JS*                  _MakeNumber(const char* name, double vn, JS* parent, unsigned pos);
    static JS*                  _MakeObject(const char* name, JS* parent, unsigned pos);
    static JS*                  _MakeString(const char* name, const char* vs, JS* parent, unsigned pos);

    static constexpr const char* TYPE_NAMES[10] = { "OBJECT", "ARRAY", "STRING", "NUMBER", "BOOL", "NIL", "ERR", "", ""};
    static ssize_t              COUNT;
    static ssize_t              MAX;

    bool                        _inline;
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

/***
 *      ____        _ _     _
 *     |  _ \      (_) |   | |
 *     | |_) |_   _ _| | __| | ___ _ __
 *     |  _ <| | | | | |/ _` |/ _ \ '__|
 *     | |_) | |_| | | | (_| |  __/ |
 *     |____/ \__,_|_|_|\__,_|\___|_|
 *
 *
 */

//------------------------------------------------------------------------------
// Create JSON structure.
// It will throw exception (std::string) for any error.
//
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
    static JS*                  MakeString(std::string vs, const char* name = "", bool escape = true);

private:
    JS*                         _current;
    JS*                         _root;
    std::string                 _name;
};

} // json
} // gnu

// MKALGAM_OFF

#endif // GNU_JSON_H
