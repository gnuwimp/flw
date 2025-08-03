/**
* @file
* @brief JSON parser and builder.
*
* gnu::json::JS Represents one json value.\n
* gnu::json::Builder Utility class to create JSON data.\n
* gnu::json Has function to decode and encode JSON.\n
*
* @author gnuwimp@gmail.com
* @copyright Released under the GNU General Public License v3.0
*/

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

/** @brief JSON helper functions.
*
* json::decode() decodes buffer to one root JS object.\n
* json::encode() encodes one root JS object to json string.\n
*/
namespace json {

/** @brief JSON data types.
*
*/
enum class TYPE : uint8_t {
                                OBJECT,     ///< @brief Object node.
                                ARRAY,      ///< @brief Array node
                                STRING,     ///< @brief Plain string.
                                NUMBER,     ///< @brief Number.
                                BOOL,       ///< @brief Boolean true or false.
                                NIL,        ///< @brief NULL data.
                                ERR,        ///< @brief Error value.
};

/** @brief Encoding options.
*
*/
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

/** @brief JSON value.
*
* Can only be one type at a time.
*
* @snippet json.cpp gnu::json::JS example
*
*/
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

    bool                        _inl;       // To create values inline without newlines.
    TYPE                        _type;      // JSON type.
    uint32_t                    _pos;       // Position in json buffer.
    JS*                         _parent;    // Parent of value.
    char*                       _name;      // Name of json value.

    union {                                 // Union of all possible json values, only one can exist at the same time, null has no value.
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

/** @brief A JSON builder class.
*
* Throws exception for all errors.\n
*
* @snippet json.cpp gnu::json::Builder example
*/
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

} // json
} // gnu

// MKALGAM_OFF

#endif // GNU_JSON_H
