// Copyright gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#include "json.h"

// MKALGAM_ON

#include <errno.h>
#include <cstring>
#include <cmath>

namespace flw {

/***
 *                 _            _
 *                (_)          | |
 *      _ __  _ __ ___   ____ _| |_ ___
 *     | '_ \| '__| \ \ / / _` | __/ _ \
 *     | |_) | |  | |\ V / (_| | ||  __/
 *     | .__/|_|  |_| \_/ \__,_|\__\___|
 *     | |
 *     |_|
 */

#define JSON_ERROR(X,Y) _json_format_error(__LINE__, (unsigned) (X), Y)
#define JSON_FREE_STRINGS(X,Y) free(X); free(Y); X = Y = nullptr;

static const char* const _JSON_BOM = "\xef\xbb\xbf";

//------------------------------------------------------------------------------
static void _json_debug(const JS* js, std::string t) {
    if (js->is_array() == true) {
        printf("%sARRAY(%u, %u): \"%s\"\n", t.c_str(), js->pos(), (int) js->size(), js->name().c_str());
        t += "\t";
        for (auto js2 : *js->va()) _json_debug(js2, t);
        t.pop_back();
    }
    else if (js->is_object() == true) {
        printf("%sOBJECT(%u, %u): \"%s\"\n", t.c_str(), js->pos(), (int) js->size(), js->name().c_str());
        t += "\t";
        for (auto js2 : *js->vo()) _json_debug(js2.second, t);
        t.pop_back();
    }
    else if (js->is_null()) printf("%s%s(%u): \"%s\"\n", t.c_str(), js->type_name().c_str(), js->pos(), js->name_c());
    else if (js->is_string()) printf("%s%s(%u): \"%s\": \"%s\"\n", t.c_str(), js->type_name().c_str(), js->pos(), js->name_c(), js->vs_c());
    else if (js->is_number()) printf("%s%s(%u): \"%s\": %f\n", t.c_str(), js->type_name().c_str(), js->pos(), js->name_c(), js->vn());
    else if (js->is_bool()) printf("%s%s(%u): \"%s\": %s\n", t.c_str(), js->type_name().c_str(), js->pos(), js->name_c(), js->vb() ? "true" : "false");
    fflush(stdout);
}

//------------------------------------------------------------------------------
static std::string _json_format_error(unsigned source, unsigned pos, unsigned line) {
    char buf[256];
    snprintf(buf, 256, "error: invalid json (%u) at pos %u and line %u", source, pos, line);
    return buf;
}

//------------------------------------------------------------------------------
static bool _json_parse_number(const char* json, size_t len, size_t& pos, double& nVal) {
    bool        res = false;
    std::string n1;
    std::string n2;

    nVal = NAN;

    while (pos < len) {
        unsigned char c = json[pos];

        if (c == '-' || c == '.' || (c >= '0' && c <= '9')) {
            n1 += c;
            pos++;
        }
        else {
            break;
        }
    }

    while (pos < len) {
        unsigned char c = json[pos];

        if (c == 'e' || c == 'E' || c == '-' || c == '+' || (c >= '0' && c <= '9')) {
            n2 += c;
            pos++;
        }
        else {
            break;
        }
    }

    int dot1  = 0;
    int dot2  = 0;
    int minus = 0;
    int plus  = 0;
    int E     = 0;
    int term  = json[pos];

    for (auto c : n1) {
        dot1  += (c =='.');
        minus += (c =='-');
        plus  += (c =='+');
        E     += (c =='e');
        E     += (c =='E');
    }

    if (dot1 > 1 || minus > 1 || n1 == "-") {
        return false;
    }
    else if (n1.back() == '.') {
        return false;
    }
    else if (n1[0] == '-' && n1[1] == '0' && n1[2] != '.' && n1[2] != 0) {
        return false;
    }
    else if (n1[0] == '-' && n1[1] == '.') {
        return false;
    }
    else if (n1[0] == '0' && n1[1] != 0 && n1[1] != '.') {
        return false;
    }
    else if (minus > 0 && n1[0] != '-') {
        return false;
    }

    if (n2 != "") {
        if (n2.length() == 1) {
            return false;
        }
        else {
            dot2 = minus = plus = E = 0;
            for (auto c : n2) {
                dot2  += (c =='.');
                minus += (c =='-');
                plus  += (c =='+');
                E     += (c =='e');
                E     += (c =='E');
            }

            if (plus + minus > 1 || E > 1) {
                return false;
            }
            else if (plus > 0 && n2.back() == '+') {
                return false;
            }
            else if (plus > 0 && n2[1] != '+') {
                return false;
            }
            else if (minus > 0 && n2.back() == '-') {
                return false;
            }
            else if (minus > 0 && n2[1] != '-') {
                return false;
            }
        }
    }

    if (term > 32 && term != ',' && term != ':' && term != '}' && term != ']' && term != '{' && term != '[') {
        return false;
    }

    errno = 0;

    if (E > 0 || dot1 > 0) {
        nVal = strtod((n1 + n2).c_str(), nullptr);
    }
    else {
        nVal = strtoll((n1 + n2).c_str(), nullptr, 0);
    }

    res   = (errno == 0);
    pos--;
    return res;
}

//------------------------------------------------------------------------------
static bool _json_parse_string(bool ignore_utf_check, const char* json, size_t len, size_t& pos, char** sVal1, char** sVal2) {
    std::string str   = "";
    bool        term  = false;
    unsigned    c     = 0;
    unsigned    p     = 0;

    pos++;

    while (pos < len) {
        c = json[pos];

        if (p == '\\' && c == '\\') {
            str += c;
            c = 0;
        }
        else if (p == '\\' && c == '"') {
            str += c;
        }
        else if (c == '"') {
            pos++;
            term = true;
            break;
        }
        else if (c < 32) {
            return false;
        }
        else {
            str += c;
        }

        p = c;
        pos++;
    }

    auto ulen = (ignore_utf_check == false) ? JS::CountUtf8(str.c_str()) : 1;

    if (term == false) {
        return false;
    }
    else if (ulen == 0 && str.length() > 0) {
        return false;
    }

    if (*sVal1 == nullptr) {
        *sVal1 = strdup(str.c_str());
    }
    else if (*sVal2 == nullptr) {
        *sVal2 = strdup(str.c_str());
    }
    else {
        return false;
    }

    pos--;
    return true;
}

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

ssize_t JS::COUNT = 0;

//------------------------------------------------------------------------------
bool JS::_add_bool(char** sVal1, bool b, bool ignore_duplicates, unsigned pos) {
    bool res = false;

    if (is_array() == true) {
        _va->push_back(JS::_MakeBool("", b, this, pos));
        res = true;
    }
    else if (is_object() == true) {
        res = _set_object(*sVal1, JS::_MakeBool(*sVal1, b, this, pos), ignore_duplicates);
    }

    free(*sVal1);
    *sVal1 = nullptr;
    return res;
}

//------------------------------------------------------------------------------
bool JS::_add_nil(char** sVal1, bool ignore_duplicates, unsigned pos) {
    bool res = false;

    if (is_array() == true) {
        _va->push_back(JS::_MakeNil("", this, pos));
        res = true;
    }
    else if (is_object() == true) {
        res = _set_object(*sVal1, JS::_MakeNil(*sVal1, this, pos), ignore_duplicates);
    }

    free(*sVal1);
    *sVal1 = nullptr;
    return res;
}

//------------------------------------------------------------------------------
bool JS::_add_number(char** sVal1, double& nVal, bool ignore_duplicates, unsigned pos) {
    bool res = false;

    if (is_array() == true && std::isnan(nVal) == false) {
        _va->push_back(JS::_MakeNumber("", nVal, this, pos));
        res = true;
    }
    else if (is_object() == true && std::isnan(nVal) == false) {
        res = _set_object(*sVal1, JS::_MakeNumber(*sVal1, nVal, this, pos), ignore_duplicates);
    }

    free(*sVal1);
    *sVal1 = nullptr;
    nVal = NAN;
    return res;
}

//------------------------------------------------------------------------------
bool JS::_add_string(char** sVal1, char** sVal2, bool ignore_duplicates, unsigned pos) {
    bool res = false;

    if (is_array() == true && *sVal1 != nullptr && *sVal2 == nullptr) {
        _va->push_back(JS::_MakeString("", *sVal1, this, pos));
        res = true;
    }
    else if (is_object() == true && *sVal1 != nullptr && *sVal2 != nullptr) {
        res = _set_object(*sVal1, JS::_MakeString(*sVal1, *sVal2, this, pos), ignore_duplicates);
    }

    free(*sVal1);
    free(*sVal2);

    *sVal1 = nullptr;
    *sVal2 = nullptr;

    return res;
}

//------------------------------------------------------------------------------
void JS::_clear(bool name) {
    if (_type == ARRAY) {
        for (auto js : *_va) {
            delete js;
        }

        delete _va;
        _va = nullptr;
    }
    else if (_type == OBJECT) {
        for (auto js : *_vo) {
            delete js.second;
        }

        delete _vo;
        _vo = nullptr;
    }
    else if (_type == STRING) {
        free(_vs);
        _vs = nullptr;
    }

    if (name == true) {
        free(_name);
        _name   = nullptr;
    }

    _type   = NIL;
    _vb     = false;
    _parent = nullptr;
}


//------------------------------------------------------------------------------
std::string JS::decode(const char* json, size_t len, bool ignore_trailing_comma, bool ignore_duplicates, bool ignore_utf_check) {
    auto colon   = 0;
    auto comma   = 0;
    auto count_a = 0;
    auto count_o = 0;
    auto current = (JS*) nullptr;
    auto line    = (unsigned) 1;
    auto n       = (JS*) nullptr;
    auto nVal    = (double) NAN;
    auto pos     = (size_t) 0;
    auto pos2    = (size_t) 0;
    auto posn    = (size_t) 0;
    auto sVal1   = (char*) nullptr;
    auto sVal2   = (char*) nullptr;
    auto tmp     = JS();

    try {
        _clear(true);
        _name = strdup("");

        tmp._type = JS::ARRAY;
        tmp._va   = new JSArray();
        current   = &tmp;

        if (strncmp(json, _JSON_BOM, 3) == 0) {
            pos += 3;
        }

        while (pos < len) {
            auto start = pos;
            auto c     = (unsigned) json[pos];

            if (c == '\t' || c == '\r' || c == ' ') {
                pos++;
            }
            else if (c == '\n') {
                line++;
                pos++;
            }
            else if (c == '"') {
                pos2 = pos;
                if (sVal1 == nullptr) posn = pos;
                if (_json_parse_string(ignore_utf_check, json, len, pos, &sVal1, &sVal2) == false) throw JSON_ERROR(start, line);

                auto add_object = (current->is_object() == true && sVal2 != nullptr);
                auto add_array = (current->is_array() == true);

                if (comma > 0 && current->size() == 0) throw JSON_ERROR(start, line);
                else if (comma == 0 && current->size() > 0) throw JSON_ERROR(start, line);
                else if (add_object == true && colon != 1) throw JSON_ERROR(start, line);
                else if (add_object == true || add_array == true) {
                    pos2 = (sVal2 == nullptr) ? pos2 : posn;
                    if (current->_add_string(&sVal1, &sVal2, ignore_duplicates, pos2) == false) throw JSON_ERROR(start, line);
                    colon = 0;
                    comma = 0;
                }

                pos++;
            }
            else if ((c >= '0' && c <= '9') || c == '-') {
                pos2 = (sVal1 == nullptr) ? pos : posn;
                if (_json_parse_number(json, len, pos, nVal) == false) throw JSON_ERROR(start, line);
                else if (comma > 0 && current->size() == 0) throw JSON_ERROR(start, line);
                else if (comma == 0 && current->size() > 0) throw JSON_ERROR(start, line);
                else if (current->is_object() == true && colon != 1) throw JSON_ERROR(start, line);
                else if (current->_add_number(&sVal1, nVal, ignore_duplicates, pos2) == false) throw JSON_ERROR(start, line);

                colon = 0;
                comma = 0;
                pos++;
            }
            else if (c == ',') {
                if (comma > 0) throw JSON_ERROR(pos, line);
                else if (current == &tmp) throw JSON_ERROR(pos, line);

                comma++;
                pos++;
            }
            else if (c == ':') {
                if (colon > 0) throw JSON_ERROR(pos, line);
                else if (current->is_object() == false) throw JSON_ERROR(pos, line);
                else if (sVal1 == nullptr) throw JSON_ERROR(pos, line);

                colon++;
                pos++;
            }
            else if (c == '[') {
                if (current->size() == 0 && comma > 0) throw JSON_ERROR(pos, line);
                else if (current->size() > 0 && comma != 1) throw JSON_ERROR(pos, line);
                else if (current->is_array() == true) {
                    if (sVal1 != nullptr) throw JSON_ERROR(pos, line);

                    n = JS::_MakeArray("", current, pos);
                    current->_va->push_back(n);
                }
                else {
                    if (sVal1 == nullptr) throw JSON_ERROR(pos, line);
                    else if (colon != 1) throw JSON_ERROR(pos, line);

                    n = JS::_MakeArray(sVal1, current, pos2);
                    if (current->_set_object(sVal1, n, ignore_duplicates) == false) throw JSON_ERROR(pos, line);
                    JSON_FREE_STRINGS(sVal1, sVal2)
                }

                current = n;
                colon = 0;
                comma = 0;
                count_a++;
                pos++;
            }
            else if (c == ']') {
                if (current->_parent == nullptr) throw JSON_ERROR(pos, line);
                else if (current->is_array() == false) throw JSON_ERROR(pos, line);
                else if (sVal1 != nullptr || sVal2 != nullptr) throw JSON_ERROR(pos, line);
                else if (comma > 0 && ignore_trailing_comma == false) throw JSON_ERROR(pos, line);
                else if (count_a < 0) throw JSON_ERROR(pos, line);

                current = current->_parent;
                comma = 0;
                count_a--;
                pos++;
            }
            else if (c == '{') {
                if (current->size() == 0 && comma > 0) throw JSON_ERROR(pos, line);
                else if (current->size() > 0 && comma != 1) throw JSON_ERROR(pos, line);
                else if (current->is_array() == true) {
                    if (sVal1 != nullptr) throw JSON_ERROR(pos, line);

                    n = JS::_MakeObject("", current, pos);
                    current->_va->push_back(n);
                }
                else {
                    if (sVal1 == nullptr) throw JSON_ERROR(pos, line);
                    else if (colon != 1) throw JSON_ERROR(pos, line);

                    n = JS::_MakeObject(sVal1, current, posn);
                    if (current->_set_object(sVal1, n, ignore_duplicates) == false) throw JSON_ERROR(pos, line);
                    JSON_FREE_STRINGS(sVal1, sVal2)
                }

                current = n;
                colon = 0;
                comma = 0;
                count_o++;
                pos++;
            }
            else if (c == '}') {
                if (current->_parent == nullptr) throw JSON_ERROR(pos, line);
                else if (current->is_object() == false) throw JSON_ERROR(pos, line);
                else if (sVal1 != nullptr || sVal2 != nullptr) throw JSON_ERROR(pos, line);
                else if (comma > 0 && ignore_trailing_comma == false) throw JSON_ERROR(pos, line);
                else if (count_o < 0) throw JSON_ERROR(pos, line);

                current = current->_parent;
                comma = 0;
                count_o--;
                pos++;
            }
            else if ((c == 't' && json[pos + 1] == 'r' && json[pos + 2] == 'u' && json[pos + 3] == 'e') ||
                    (c == 'f' && json[pos + 1] == 'a' && json[pos + 2] == 'l' && json[pos + 3] == 's' && json[pos + 4] == 'e')) {
                pos2 = (sVal1 == nullptr) ? pos : posn;
                if (current->size() > 0 && comma == 0) throw JSON_ERROR(start, line);
                else if (comma > 0 && current->size() == 0) throw JSON_ERROR(start, line);
                else if (comma == 0 && current->size() > 0) throw JSON_ERROR(start, line);
                else if (current->is_object() == true && colon != 1) throw JSON_ERROR(start, line);
                else if (current->_add_bool(&sVal1, c == 't', ignore_duplicates, pos2) == false) throw JSON_ERROR(start, line);

                colon = 0;
                comma = 0;
                pos += 4;
                pos += (c == 'f');
            }
            else if (c == 'n' && json[pos + 1] == 'u' && json[pos + 2] == 'l' && json[pos + 3] == 'l') {
                pos2 = (sVal1 == nullptr) ? pos : posn;
                if (current->size() > 0 && comma == 0) throw JSON_ERROR(start, line);
                else if (comma > 0 && current->size() == 0) throw JSON_ERROR(start, line);
                else if (comma == 0 && current->size() > 0) throw JSON_ERROR(start, line);
                else if (current->is_object() == true && colon != 1) throw JSON_ERROR(start, line);
                else if (current->_add_nil(&sVal1, ignore_duplicates, pos) == false) throw JSON_ERROR(start, line);

                colon = 0;
                comma = 0;
                pos += 4;
            }
            else {
                throw JSON_ERROR(pos, line);
            }

            if (count_a > (int) JS::MAX_DEPTH || count_o > (int) JS::MAX_DEPTH) {
                throw JSON_ERROR(pos, line);
            }
        }

        if (count_a != 0 || count_o != 0) {
            throw JSON_ERROR(len, 1);
        }
        else if (tmp.size() != 1) {
            throw JSON_ERROR(len, 1);
        }
        else if (tmp[0]->_type == JS::ARRAY) {
            _type = JS::ARRAY;
            _va = tmp[0]->_va;
            ((JS*) tmp[0])->_type = JS::NIL;

            for (auto o : *_va) {
                o->_parent = this;
            }
        }
        else if (tmp[0]->_type == JS::OBJECT) {
            _type = JS::OBJECT;
            _vo = tmp[0]->_vo;
            ((JS*) tmp[0])->_type = JS::NIL;

            for (auto& it : *_vo) {
                it.second->_parent = this;
            }
        }
        else if (tmp[0]->_type == JS::BOOL) {
            _type = JS::BOOL;
            _vb   = tmp[0]->_vb;
        }
        else if (tmp[0]->_type == JS::NUMBER) {
            _type = JS::NUMBER;
            _vn   = tmp[0]->_vn;
        }
        else if (tmp[0]->_type == JS::STRING) {
            _type = JS::STRING;
            _vs   = tmp[0]->_vs;
            ((JS*) tmp[0])->_type = JS::NIL;
        }
        else if (tmp[0]->_type == JS::NIL) {
            _type = JS::NIL;
        }
        else {
            throw JSON_ERROR(0, 1);
        }
    }
    catch(std::string err) {
        JSON_FREE_STRINGS(sVal1, sVal2)
        _clear(false);
        return err;
    }

    return "";
}

//------------------------------------------------------------------------------
void JS::debug() const {
    std::string t;
    _json_debug(this, t);
}

//------------------------------------------------------------------------------
std::string JS::encode(int skip) const {
    std::string t;
    std::string j;

    try {
        if (is_array() == true || is_object() == true) {
            JS::_Encode(this, j, t, false, skip);
        }
        else {
            return _encode(true, skip);
        }
    }
    catch (std::string e) {
        j = e;
    }

    return j;
}

//------------------------------------------------------------------------------
std::string JS::_encode(bool ignore_name, int skip) const {
    static const std::string QUOTE = "\"";

    std::string res;
    std::string arr = (skip == 0) ? "\": [" : "\":[";
    std::string obj = (skip == 0) ? "\": {" : "\":{";
    std::string name = (skip == 0) ? "\": " : "\":";
    bool object = (_parent != nullptr && _parent->is_object() == true);

    if (_type == ARRAY) {
        res = (object == false || ignore_name == true) ? res = "[" : (QUOTE + _name + arr);
    }
    else if (_type == OBJECT) {
        res = (object == false || ignore_name == true) ? "{" : (QUOTE + _name + obj);
    }
    else {
        res = (object == false || ignore_name == true) ? "" : (QUOTE + _name + name);

        if (_type == STRING) {
            res += QUOTE + _vs + QUOTE;
        }
        else if (_type == NIL) {
            res += "null";
        }
        else if (_type == BOOL && _vb == true) {
            res += "true";
        }
        else if (_type == BOOL && _vb == false) {
            res += "false";
        }
        else if (_type == NUMBER) {
            char b[500];

            auto n   = snprintf(b, 500, "%f", _vn);
            auto dot = strstr(b, ".");

            if (n < 1 || n >= 500) {
                strcpy(b, "0.0");
            }
            else if (dot != nullptr) {
                auto zero = b + strlen(b) - 1;

                while (zero >= dot) { // Remove trailing decimal zeros
                    if (*zero == '0' || *zero == '.') {
                        *zero = 0;
                    }
                    else {
                        break;
                    }

                    zero--;
                }
            }

            res += b;
        }
    }

    return res;
}

//------------------------------------------------------------------------------
const JS* JS::find(std::string name, bool rec) const {
    if (is_object() == true) {
        auto find = _vo->find(name);

        if (find != _vo->end()) {
            return find->second;
        }
        else if (rec == true) {
            for (auto o : *_vo) {
                if (o.second->is_object() == true) {
                    return o.second->find(name, rec);
                }
            }
        }
    }

    return nullptr;
}

//------------------------------------------------------------------------------
const JS* JS::_get_object(const char* name, bool escape) const {
    if (is_object() == true) {
        auto key  = (escape == true) ? JS::Escape(name) : name;
        auto find = _vo->find(key);
        return (find != _vo->end()) ? find->second : nullptr;
    }

    return nullptr;
}

//------------------------------------------------------------------------------
// Delete existing object before setting new.
//
bool JS::_set_object(const char* name, JS* js, bool ignore_duplicates) {
    if (is_object() == true) {
        auto find = _vo->find(name);

        if (find != _vo->end()) {
            if (ignore_duplicates == false) {
                delete js;
                return false;
            }
            else {
                delete find->second;
            }
        }

        (*_vo)[name] = js;
    }

    return true;
}

//------------------------------------------------------------------------------
const JSArray JS::vo_to_va() const {
    JSArray res;

    if (_type == OBJECT) {
        for (auto& m : *_vo) {
            res.push_back(m.second);
        }
    }

    return res;
}

/***
 *           _  _____     _____ _        _   _
 *          | |/ ____|   / ____| |      | | (_)
 *          | | (___    | (___ | |_ __ _| |_ _  ___
 *      _   | |\___ \    \___ \| __/ _` | __| |/ __|
 *     | |__| |____) |   ____) | || (_| | |_| | (__
 *      \____/|_____/   |_____/ \__\__,_|\__|_|\___|
 *
 *
 */

//------------------------------------------------------------------------------
size_t JS::CountUtf8(const char* p) {
    auto count = (size_t) 0;
    auto f     = (size_t) 0;
    auto u     = (const unsigned char*) p;
    auto c     = (unsigned) u[0];

    while (c != 0) {
        if (c >= 128) {
            if (c >= 194 && c <= 223) {
                c = u[++f];
                if (c < 128 || c > 191) return 0;
            }
            else if (c >= 224 && c <= 239) {
                c = u[++f];
                if (c < 128 || c > 191) return 0;
                c = u[++f];
                if (c < 128 || c > 191) return 0;
            }
            else if (c >= 240 && c <= 244) {
                c = u[++f];
                if (c < 128 || c > 191) return 0;
                c = u[++f];
                if (c < 128 || c > 191) return 0;
                c = u[++f];
                if (c < 128 || c > 191) return 0;
            }
            else {
                return 0;
            }
        }

        count++;
        c = u[++f];
    }

    return count;
}

//------------------------------------------------------------------------------
void JS::_Encode(const JS* js, std::string& j, std::string& t, bool comma, int skip) {
    std::string c = (comma == true) ? "," : "";
    std::string n = (skip > 1) ? "" : "\n";
    size_t      f = 0;

    if (js->is_array() == true) {
        j += t + js->_encode(j == "", skip) + ((js->_enc_flag == 1) ? "" : n);

        for (auto n : *js->_va) {
            if (skip == 0) t += "\t";
            if (js->_enc_flag == 1) JS::_EncodeInline(n, j, f < (js->_va->size() - 1), skip);
            else JS::_Encode(n, j, t, f < (js->_va->size() - 1), skip);
            if (skip == 0) t.pop_back();
            f++;
        }

        j += ((js->_enc_flag == 1) ? ("]" + c + "\n") : (t + "]" + c + n));
    }
    else if (js->is_object() == true) {
        j += t + js->_encode(j == "", skip) + ((js->_enc_flag == 1) ? "" : n);

        for (auto& n : *js->_vo) {
            if (skip == 0) t += "\t";
            if (js->_enc_flag == 1) JS::_EncodeInline(n.second, j, f < (js->_vo->size() - 1), skip);
            else JS::_Encode(n.second, j, t, f < (js->_vo->size() - 1), skip);
            if (skip == 0) t.pop_back();
            f++;
        }

        j += ((js->_enc_flag == 1) ? ("}" + c + "\n") : (t + "}" + c + n));
    }
    else {
        j += t + js->_encode(false, skip) + c + n;
    }
}

//------------------------------------------------------------------------------
void JS::_EncodeInline(const JS* js, std::string& j, bool comma, int skip) {
    std::string c = (comma == true) ? "," : "";
    size_t      f = 0;

    if (*js == ARRAY) {
        j += js->_encode(false, skip);

        for (auto n : *js->_va) {
            JS::_EncodeInline(n, j, f < (js->_va->size() - 1), skip);
            f++;
        }

        j += "]" + c;
    }
    else if (*js == OBJECT) {
        j += js->_encode(false, skip);

        for (auto& n : *js->_vo) {
            JS::_EncodeInline(n.second, j, f < (js->_vo->size() - 1), skip);
            f++;
        }

        j += "}" + c;
    }
    else {
        j += js->_encode(false, skip) + c;
    }
}

//------------------------------------------------------------------------------
std::string JS::Escape(const char* string) {
    std::string res;
    res.reserve(strlen(string) + 5);

    while (*string != 0) {
        auto c = *string;

        if (c == 9) res += "\\t";
        else if (c == 10) res += "\\n";
        else if (c == 13) res += "\\r";
        else if (c == 8) res += "\\b";
        else if (c == 14) res += "\\f";
        else if (c == 34) res += "\\\"";
        else if (c == 92) res += "\\\\";
        else res += c;

        string++;
    }

    return res;
}

//------------------------------------------------------------------------------
std::string JS::Unescape(const char* string) {
    std::string res;
    res.reserve(strlen(string));

    while (*string != 0) {
        unsigned char c = *string;
        unsigned char n = *(string + 1);

        if (c == '\\') {
            if (n == 't') res += '\t';
            else if (n == 'n') res += '\n';
            else if (n == 'r') res += '\r';
            else if (n == 'b') res += '\b';
            else if (n == 'f') res += '\f';
            else if (n == '\"') res += '"';
            else if (n == '\\') res += '\\';
            else if (n == 0) break;
            string++;
        }
        else {
            res += c;
        }

        string++;
    }

    return res;
}

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
JSB& JSB::add(JS* js) {
    auto name = js->name();

    if (_current == nullptr) {
        if (name != "") {
            delete js;
            throw std::string("error: root object must be nameless <" + name + ">");
        }
        else {
            _root = _current = js;
        }
    }
    else {
        js->_parent = _current;

        if (_current->is_array() == true) {
            if (name != "") {
                delete js;
                throw std::string("error: values added to array are nameless <" + name + ">");
            }
            else if (*js == JS::ARRAY || *js == JS::OBJECT) {
                _current->_va->push_back(js);
                _current = js;
            }
            else {
                _current->_va->push_back(js);
            }
        }
        else if (_current->is_object() == true) {
            if (_current->find(name) != nullptr) {
                delete js;
                throw std::string("error: duplicate name <" + name + ">");
            }
            else if (js->is_array() == true || js->is_object() == true) {
                (*_current->_vo)[name] = js;
                _current = js;
            }
            else {
                (*_current->_vo)[name] = js;
            }
        }
        else {
            delete js;
            throw std::string("error: missing container");
        }
    }

    return *this;
}

//------------------------------------------------------------------------------
std::string JSB::encode() const {
    if (_root == nullptr) {
        throw std::string("error: empty json");
    }
    else if (_name != "") {
        throw std::string("error: unused name value <" + _name + ">");
    }

    auto j = _root->encode();

    if (j.find("error") == 0) {
        throw j;
    }

    return j;
}

//------------------------------------------------------------------------------
JSB& JSB::end() {
    if (_current == nullptr) {
        throw std::string("error: empty json");
    }
    else if (_current == _root) {
        throw std::string("error: already at the top level");
    }

    _current = _current->parent();
    return *this;
}

} // flw

// MKALGAM_OFF