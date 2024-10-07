// Copyright gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#ifndef FLW_JSON_H
#define FLW_JSON_H

// MKALGAM_ON

#include <assert.h>
#include <string.h>
#include <map>
#include <string>
#include <vector>

namespace flw {

class JS;

typedef std::map<std::string, JS*> JSObject;
typedef std::vector<JS*> JSArray;

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
    friend class JSB;

public:
    static const size_t         MAX_DEPTH = 32;

    enum TYPE {
                                OBJECT,
                                ARRAY,
                                STRING,
                                NUMBER,
                                BOOL,
                                NIL,
    };

    enum class ENCODE_OPTION {
                                NORMAL,
                                REMOVE_LEADING,
                                REMOVE_LEADING_AND_NEWLINES,
    };

                                JS(const JS&) = delete;
                                JS(JS&&) = delete;
    JS&                         operator=(const JS&) = delete;
    JS&                         operator=(JS&&) = delete;

                                JS()
                                    { JS::COUNT++; _type = JS::NIL; _name = strdup(""); _vb = false; _parent = nullptr; _enc_flag = 0; _pos = 0; }
                                ~JS()
                                    { JS::COUNT--; _clear(true); }
    bool                        operator==(JS::TYPE type) const
                                    { return _type == type; }
    bool                        operator!=(JS::TYPE type) const
                                    { return _type != type; }
    const JS*                   operator[](std::string name) const
                                    { return _get_object(name.c_str(), true); }
    const JS*                   operator[](size_t index) const
                                    { return (_type == JS::ARRAY && index < _va->size()) ? (*_va)[index] : nullptr; }
    std::string                 decode(const char* json, size_t len, bool ignore_trailing_comma = false, bool ignore_duplicates = false, bool ignore_utf_check = false);
    std::string                 decode(std::string json, bool ignore_trailing_comma = false, bool ignore_duplicates = false, bool ignore_utf_check = false)
                                    { return decode(json.c_str(), json.length(), ignore_trailing_comma, ignore_duplicates, ignore_utf_check); }
    void                        debug() const;
    std::string                 encode(JS::ENCODE_OPTION option = ENCODE_OPTION::NORMAL) const;
    const JS*                   find(std::string name, bool rec = false) const;
    const JS*                   get(std::string name, bool escape_name = true) const
                                    { return _get_object(name.c_str(), escape_name); }
    const JS*                   get(size_t index) const
                                    { return (*this) [index]; }
    bool                        is_array() const
                                    { return _type == JS::ARRAY; }
    bool                        is_bool() const
                                    { return _type == JS::BOOL; }
    bool                        is_null() const
                                    { return _type == JS::NIL; }
    bool                        is_number() const
                                    { return _type == JS::NUMBER; }
    bool                        is_object() const
                                    { return _type == JS::OBJECT; }
    bool                        is_string() const
                                    { return _type == JS::STRING; }
    std::string                 name() const
                                    { return _name; }
    const char*                 name_c() const
                                    { return _name; }
    std::string                 name_u() const
                                    { return JS::Unescape(_name); }
    JS*                         parent()
                                    { return _parent; }
    unsigned                    pos() const
                                    { return _pos; }
    size_t                      size() const
                                    { return (is_array() == true) ? _va->size() : (is_object() == true) ? _vo->size() : 0; }
    TYPE                        type() const
                                    { return (TYPE) _type; }
    std::string                 type_name() const
                                    { assert(_type >= 0 && _type < (int) 6); return TYPE_NAMES[(unsigned) _type]; }
    const JSArray*              va() const
                                    { return (_type == JS::ARRAY) ? _va : nullptr; }
    bool                        vb() const
                                    { assert(_type == JS::BOOL); return (_type == JS::BOOL) ? _vb : false; }
    double                      vn() const
                                    { assert(_type == JS::NUMBER); return (_type == JS::NUMBER) ? _vn : 0.0; }
    long long int               vn_i() const
                                    { assert(_type == JS::NUMBER); return (_type == JS::NUMBER) ? (long long int) _vn : 0; }
    const JSObject*             vo() const
                                    { return (_type == JS::OBJECT) ? _vo : nullptr; }
    const JSArray               vo_to_va() const;
    std::string                 vs() const
                                    { assert(_type == JS::STRING); return (_type == JS::STRING) ? _vs : ""; }
    const char*                 vs_c() const
                                    { assert(_type == JS::STRING); return (_type == JS::STRING) ? _vs : ""; }
    std::string                 vs_u() const
                                    { assert(_type == JS::STRING); return (_type == JS::STRING) ? JS::Unescape(_vs) : ""; }

    static inline ssize_t       Count()
                                    { return JS::COUNT; }
    static size_t               CountUtf8(const char* p);
    static std::string          Escape(const char* string);
    static std::string          FormatNumber(double f, bool E = false);
    static std::string          Unescape(const char* string);

private:
    explicit                    JS(const char* name, JS* parent = nullptr, unsigned pos = 0)
                                    { JS::COUNT++; _type = NIL; _name = strdup((name != nullptr) ? name : ""); _parent = parent; _enc_flag = 0; _pos = pos; }
    bool                        _add_bool(char** sVal1, bool b, bool ignore_duplicates, unsigned pos);
    bool                        _add_nil(char** sVal1, bool ignore_duplicates, unsigned pos);
    bool                        _add_number(char** sVal1, double& nVal, bool ignore_duplicates, unsigned pos);
    bool                        _add_string(char** sVal1, char** sVal2, bool ignore_duplicates, unsigned pos);
    void                        _clear(bool name);
    std::string                 _encode(bool ignore_name, JS::ENCODE_OPTION option) const;
    const JS*                   _get_object(const char* name, bool escape) const;
    bool                        _set_object(const char* name, JS* js, bool ignore_duplicates);

    static void                 _Encode(const JS* js, std::string& j, std::string& t, bool comma, JS::ENCODE_OPTION option);
    static void                 _EncodeInline(const JS* js, std::string& j, bool comma, JS::ENCODE_OPTION option);
    static inline JS*           _MakeArray(const char* name, JS* parent, unsigned pos)
                                    { auto r = new JS(name, parent, pos); r->_type = ARRAY; r->_va = new JSArray(); return r; }
    static inline JS*           _MakeBool(const char* name, bool vb, JS* parent, unsigned pos)
                                    { auto r = new JS(name, parent, pos); r->_type = BOOL; r->_vb = vb; return r; }
    static inline JS*           _MakeNil(const char* name, JS* parent, unsigned pos)
                                    { return new JS(name, parent, pos); }
    static inline JS*           _MakeNumber(const char* name, double vn, JS* parent, unsigned pos)
                                    { auto r = new JS(name, parent, pos); r->_type = NUMBER; r->_vn = vn; return r; }
    static inline JS*           _MakeObject(const char* name, JS* parent, unsigned pos)
                                    { auto r = new JS(name, parent, pos); r->_type = OBJECT; r->_vo = new JSObject(); return r; }
    static inline JS*           _MakeString(const char* name, const char* vs, JS* parent, unsigned pos)
                                    { auto r = new JS(name, parent, pos); r->_type = STRING; r->_vs = strdup(vs); return r; }

    static constexpr const char* TYPE_NAMES[7] = { "OBJECT", "ARRAY", "STRING", "NUMBER", "BOOL", "NIL", };
    static ssize_t              COUNT;

    char                        _type;
    char                        _enc_flag;
    unsigned                    _pos;
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
 *           _  _____ ____
 *          | |/ ____|  _ \
 *          | | (___ | |_) |
 *      _   | |\___ \|  _ <
 *     | |__| |____) | |_) |
 *      \____/|_____/|____/
 *
 *
 */

//------------------------------------------------------------------------------
// Create JSON structure.
// It will throw exception (std::string) for any error.
//
class JSB {
public:
                                JSB()
                                    { _root = _current = nullptr; }
    virtual                     ~JSB()
                                    { delete _root; }
    JSB&                        operator<<(JS* json)
                                    { return add(json); }
    JSB&                        add(JS* json);
    void                        clear()
                                    { delete _root; _root = _current = nullptr; _name = ""; }
    std::string                 encode() const;
    JSB&                        end();
    const JS*                   root() const
                                    { return _root; }

    static inline JS*           MakeArray(const char* name = "", bool escape = true)
                                    { auto r = new JS((escape == true) ? JS::Escape(name).c_str() : name); r->_type = JS::ARRAY; r->_va = new JSArray(); return r; }
    static inline JS*           MakeArrayInline(const char* name = "", bool escape = true)
                                    { auto r = new JS((escape == true) ? JS::Escape(name).c_str() : name); r->_type = JS::ARRAY; r->_va = new JSArray(); r->_enc_flag = 1; return r; }
    static inline JS*           MakeBool(bool vb, const char* name = "", bool escape = true)
                                    { auto r = new JS((escape == true) ? JS::Escape(name).c_str() : name); r->_type = JS::BOOL; r->_vb = vb; return r; }
    static inline JS*           MakeNull(const char* name = "", bool escape = true)
                                    { auto r = new JS((escape == true) ? JS::Escape(name).c_str() : name); r->_type = JS::NIL; return r; }
    static inline JS*           MakeNumber(double vn, const char* name = "", bool escape = true)
                                    { auto r = new JS((escape == true) ? JS::Escape(name).c_str() : name); r->_type = JS::NUMBER; r->_vn = vn; return r; }
    static inline JS*           MakeObject(const char* name = "", bool escape = true)
                                    { auto r = new JS((escape == true) ? JS::Escape(name).c_str() : name); r->_type = JS::OBJECT; r->_vo = new JSObject(); return r; }
    static inline JS*           MakeObjectInline(const char* name = "", bool escape = true)
                                    { auto r = new JS((escape == true) ? JS::Escape(name).c_str() : name); r->_type = JS::OBJECT; r->_vo = new JSObject(); r->_enc_flag = 1; return r; }
    static inline JS*           MakeString(const char* vs, const char* name = "", bool escape = true)
                                    { auto r = new JS((escape == true) ? JS::Escape(name).c_str() : name); r->_type = JS::STRING; r->_vs = strdup((escape == true) ? JS::Escape(vs).c_str() : vs); return r; }
    static inline JS*           MakeString(std::string vs, const char* name = "", bool escape = true)
                                    { auto r = new JS((escape == true) ? JS::Escape(name).c_str() : name); r->_type = JS::STRING; r->_vs = strdup((escape == true) ? JS::Escape(vs.c_str()).c_str() : vs.c_str()); return r; }

private:
    JS*                         _current;
    JS*                         _root;
    std::string                 _name;
};

} // flw

// MKALGAM_OFF

#endif
