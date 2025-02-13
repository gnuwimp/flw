// Copyright gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#include "json.h"

// MKALGAM_ON

#include <cmath>
#include <cstdint>
#include <errno.h>

namespace gnu {
namespace json {

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

#define _GNU_JSON_ERROR(X,Y) _format_error(__LINE__, static_cast<unsigned>(X), Y)
#define _GNU_JSON_FREE_STRINGS(X,Y) free(X); free(Y); X = Y = nullptr;

static const char* const _JSON_BOM = "\xef\xbb\xbf";

//------------------------------------------------------------------------------
static void _debug(const JS* js, std::string t) {
    if (js->is_array() == true) {
        printf("%sARRAY(%u, %u, %p, %p): \"%s\"\n", t.c_str(), js->pos(), static_cast<unsigned>(js->size()), js, js->parent(), js->name().c_str());
        t += "\t";
        for (const auto js2 : *js->va()) _debug(js2, t);
        t.pop_back();
    }
    else if (js->is_object() == true) {
        printf("%sOBJECT(%u, %u, %p, %p): \"%s\"\n", t.c_str(), js->pos(), static_cast<unsigned>(js->size()), js, js->parent(), js->name().c_str());
        t += "\t";
        for (auto js2 : *js->vo()) _debug(js2.second, t);
        t.pop_back();
    }
    else if (js->is_null()) printf("%s%s(%u, %p): \"%s\"\n", t.c_str(), js->type_name().c_str(), js->pos(), js->parent(), js->name_c());
    else if (js->is_string()) printf("%s%s(%u, %p): \"%s\": \"%s\"\n", t.c_str(), js->type_name().c_str(), js->pos(), js->parent(), js->name_c(), js->vs_c());
    else if (js->is_number()) printf("%s%s(%u, %p): \"%s\": %f\n", t.c_str(), js->type_name().c_str(), js->pos(), js->parent(), js->name_c(), js->vn());
    else if (js->is_bool()) printf("%s%s(%u, %p): \"%s\": %s\n", t.c_str(), js->type_name().c_str(), js->pos(), js->parent(), js->name_c(), js->vb() ? "true" : "false");
    fflush(stdout);
}

//------------------------------------------------------------------------------
static std::string _encode(const JS& js, bool ignore_name, ENCODE option) {
    static const std::string QUOTE = "\"";

    std::string res;
    std::string arr    = (option == ENCODE::DEFAULT) ? "\": [" : "\":[";
    std::string obj    = (option == ENCODE::DEFAULT) ? "\": {" : "\":{";
    std::string name1  = (option == ENCODE::DEFAULT) ? "\": " : "\":";
    bool        object = (js.parent() != nullptr && js.parent()->is_object() == true);

    if (js.is_array() == true) {
        if (object == false || ignore_name == true) {
            res = "[";
        }
        else {
            res = QUOTE + js.name() + arr;
        }
    }
    else if (js.is_object() == true) {
        if (object == false || ignore_name == true) {
            res = "{";
        }
        else {
            res = QUOTE + js.name() + obj;
        }
    }
    else {
        res = (object == false || ignore_name == true) ? "" : (QUOTE + js.name() + name1);

        if (js.is_string() == true) {
            res += QUOTE + js.vs() + QUOTE;
        }
        else if (js.is_null() == true) {
            res += "null";
        }
        else if (js.is_bool() == true) {
            res += (js.vb() == true) ? "true" : "false";
        }
        else if (js.is_number()) {
            res += json::format_number(js.vn());
        }
    }

    return res;
}

//------------------------------------------------------------------------------
static void _encode_inline(const JS& js, std::string& j, bool comma, ENCODE option) {
    std::string c = (comma == true) ? "," : "";
    size_t      f = 0;

    if (js.is_array() == true) {
        j += json::_encode(js, false, option);

        for (const auto n : *js.va()) {
            json::_encode_inline(*n, j, f < (js.va()->size() - 1), option);
            f++;
        }

        j += "]" + c;
    }
    else if (js.is_object() == true) {
        j += json::_encode(js, false, option);

        for (const auto& n : *js.vo()) {
            json::_encode_inline(*n.second, j, f < (js.vo()->size() - 1), option);
            f++;
        }

        j += "}" + c;
    }
    else {
        j += json::_encode(js, false, option) + c;
    }
}

//------------------------------------------------------------------------------
static void _encode_all(const JS& js, std::string& j, std::string& t, bool comma, ENCODE option) {
    std::string c = (comma == true) ? "," : "";
    std::string n = (option == ENCODE::FLAT) ? "" : "\n";
    size_t      f = 0;

    if (js.is_array() == true) {
        j += t + json::_encode(js, j == "", option);

        if (js.has_inline() == false && js.size() > 0) {
            j += n;
        }

        for (const auto n2 : *js.va()) {
            if (option == ENCODE::DEFAULT) {
                t += "\t";
            }

            if (js.has_inline() == true) {
                json::_encode_inline(*n2, j, f < (js.va()->size() - 1), option);
            }
            else {
                json::_encode_all(*n2, j, t, f < (js.va()->size() - 1), option);
            }

            if (option == ENCODE::DEFAULT) {
                t.pop_back();
            }

            f++;
        }

        if (js.has_inline() == true || js.size() == 0) {
            j += "]" + c + n;
        }
        else {
            j += t + "]" + c + n;
        }
    }
    else if (js.is_object() == true) {
        j += t + json::_encode(js, j == "", option);

        if (js.has_inline() == false && js.size() > 0) {
            j += n;
        }

        for (const auto& n2 : *js.vo()) {
            if (option == ENCODE::DEFAULT) {
                t += "\t";
            }

            if (js.has_inline() == true) {
                json::_encode_inline(*n2.second, j, f < (js.vo()->size() - 1), option);
            }
            else {
                json::_encode_all(*n2.second, j, t, f < (js.vo()->size() - 1), option);
            }

            if (option == ENCODE::DEFAULT) {
                t.pop_back();
            }

            f++;
        }

        if (js.has_inline() == true || js.size() == 0) {
            j += "}" + c + n;
        }
        else {
            j += t + "}" + c + n;
        }
    }
    else {
        j += t + json::_encode(js, false, option) + c + n;
    }
}

//------------------------------------------------------------------------------
static std::string _format_error(unsigned source, unsigned pos, unsigned line) {
    char buf[256];
    snprintf(buf, 256, "error: invalid json (%u) at pos %u and line %u", source, pos, line);
    return buf;
}

//------------------------------------------------------------------------------
static bool _parse_number(const char* json, size_t len, size_t& pos, double& nVal) {
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

    res = (errno == 0);
    pos--;
    return res;
}

//------------------------------------------------------------------------------
static bool _parse_string(bool ignore_utf_check, const char* json, size_t len, size_t& pos, char** sVal1, char** sVal2) {
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

    auto ulen = (ignore_utf_check == false) ? json::count_utf8(str.c_str()) : 1;

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

} // json

/***
 *        _
 *       (_)
 *        _ ___  ___  _ __
 *       | / __|/ _ \| '_ \
 *       | \__ \ (_) | | | |
 *       | |___/\___/|_| |_|
 *      _/ |
 *     |__/
 */

//------------------------------------------------------------------------------
size_t json::count_utf8(const char* p) {
    auto count = (size_t) 0;
    auto f     = (size_t) 0;
    auto u     = reinterpret_cast<const unsigned char*>(p);
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
json::JS json::decode(const char* json, size_t len, bool ignore_trailing_comma, bool ignore_duplicates, bool ignore_utf_check) {
    auto ret     = JS();
    auto tmp     = JS();
    auto colon   = 0;
    auto comma   = 0;
    auto count_a = 0;
    auto count_o = 0;
    auto current = (JS*) nullptr;
    auto line    = (unsigned) 1;
    auto n       = (JS*) nullptr;
    auto nVal    = (double) NAN;
    auto pos1    = (size_t) 0;
    auto pos2    = (size_t) 0;
    auto posn    = (size_t) 0;
    auto sVal1   = (char*) nullptr;
    auto sVal2   = (char*) nullptr;

    try {
        tmp._type = TYPE::ARRAY;
        tmp._va   = new JSArray();
        current   = &tmp;

        if (strncmp(json, _JSON_BOM, 3) == 0) {
            pos1 += 3;
        }

        while (pos1 < len) {
            auto start = pos1;
            auto c     = (unsigned) json[pos1];

            if (c == '\t' || c == '\r' || c == ' ') {
                pos1++;
            }
            else if (c == '\n') {
                line++;
                pos1++;
            }
            else if (c == '"') {
                pos2 = pos1;
                if (sVal1 == nullptr) posn = pos1;
                if (_parse_string(ignore_utf_check, json, len, pos1, &sVal1, &sVal2) == false) throw _GNU_JSON_ERROR(start, line);

                auto add_object = (current->is_object() == true && sVal2 != nullptr);
                auto add_array = (current->is_array() == true);

                if (comma > 0 && current->size() == 0) throw _GNU_JSON_ERROR(start, line);
                else if (comma == 0 && current->size() > 0) throw _GNU_JSON_ERROR(start, line);
                else if (add_object == true && colon != 1) throw _GNU_JSON_ERROR(start, line);
                else if (add_object == true || add_array == true) {
                    pos2 = (sVal2 == nullptr) ? pos2 : posn;
                    if (current->_add_string(&sVal1, &sVal2, ignore_duplicates, pos2) == false) throw _GNU_JSON_ERROR(start, line);
                    colon = 0;
                    comma = 0;
                }

                pos1++;
            }
            else if ((c >= '0' && c <= '9') || c == '-') {
                pos2 = (sVal1 == nullptr) ? pos1 : posn;
                if (_parse_number(json, len, pos1, nVal) == false) throw _GNU_JSON_ERROR(start, line);
                else if (comma > 0 && current->size() == 0) throw _GNU_JSON_ERROR(start, line);
                else if (comma == 0 && current->size() > 0) throw _GNU_JSON_ERROR(start, line);
                else if (current->is_object() == true && colon != 1) throw _GNU_JSON_ERROR(start, line);
                else if (current->_add_number(&sVal1, nVal, ignore_duplicates, pos2) == false) throw _GNU_JSON_ERROR(start, line);

                colon = 0;
                comma = 0;
                pos1++;
            }
            else if (c == ',') {
                if (comma > 0) throw _GNU_JSON_ERROR(pos1, line);
                else if (current == &tmp) throw _GNU_JSON_ERROR(pos1, line);

                comma++;
                pos1++;
            }
            else if (c == ':') {
                if (colon > 0) throw _GNU_JSON_ERROR(pos1, line);
                else if (current->is_object() == false) throw _GNU_JSON_ERROR(pos1, line);
                else if (sVal1 == nullptr) throw _GNU_JSON_ERROR(pos1, line);

                colon++;
                pos1++;
            }
            else if (c == '[') {
                if (current->size() == 0 && comma > 0) throw _GNU_JSON_ERROR(pos1, line);
                else if (current->size() > 0 && comma != 1) throw _GNU_JSON_ERROR(pos1, line);
                else if (current->is_array() == true) {
                    if (sVal1 != nullptr) throw _GNU_JSON_ERROR(pos1, line);

                    n = JS::_MakeArray("", current, pos1);
                    current->_va->push_back(n);
                }
                else {
                    if (sVal1 == nullptr) throw _GNU_JSON_ERROR(pos1, line);
                    else if (colon != 1) throw _GNU_JSON_ERROR(pos1, line);

                    n = JS::_MakeArray(sVal1, current, pos2);
                    if (current->_set_object(sVal1, n, ignore_duplicates) == false) throw _GNU_JSON_ERROR(pos1, line);
                    _GNU_JSON_FREE_STRINGS(sVal1, sVal2)
                }

                current = n;
                colon = 0;
                comma = 0;
                count_a++;
                pos1++;
            }
            else if (c == ']') {
                if (current->_parent == nullptr) throw _GNU_JSON_ERROR(pos1, line);
                else if (current->is_array() == false) throw _GNU_JSON_ERROR(pos1, line);
                else if (sVal1 != nullptr || sVal2 != nullptr) throw _GNU_JSON_ERROR(pos1, line);
                else if (comma > 0 && ignore_trailing_comma == false) throw _GNU_JSON_ERROR(pos1, line);
                else if (count_a < 0) throw _GNU_JSON_ERROR(pos1, line);

                current = current->_parent;
                comma = 0;
                count_a--;
                pos1++;
            }
            else if (c == '{') {
                if (current->size() == 0 && comma > 0) throw _GNU_JSON_ERROR(pos1, line);
                else if (current->size() > 0 && comma != 1) throw _GNU_JSON_ERROR(pos1, line);
                else if (current->is_array() == true) {
                    if (sVal1 != nullptr) throw _GNU_JSON_ERROR(pos1, line);

                    n = JS::_MakeObject("", current, pos1);
                    current->_va->push_back(n);
                }
                else {
                    if (sVal1 == nullptr) throw _GNU_JSON_ERROR(pos1, line);
                    else if (colon != 1) throw _GNU_JSON_ERROR(pos1, line);

                    n = JS::_MakeObject(sVal1, current, posn);
                    if (current->_set_object(sVal1, n, ignore_duplicates) == false) throw _GNU_JSON_ERROR(pos1, line);
                    _GNU_JSON_FREE_STRINGS(sVal1, sVal2)
                }

                current = n;
                colon = 0;
                comma = 0;
                count_o++;
                pos1++;
            }
            else if (c == '}') {
                if (current->_parent == nullptr) throw _GNU_JSON_ERROR(pos1, line);
                else if (current->is_object() == false) throw _GNU_JSON_ERROR(pos1, line);
                else if (sVal1 != nullptr || sVal2 != nullptr) throw _GNU_JSON_ERROR(pos1, line);
                else if (comma > 0 && ignore_trailing_comma == false) throw _GNU_JSON_ERROR(pos1, line);
                else if (count_o < 0) throw _GNU_JSON_ERROR(pos1, line);

                current = current->_parent;
                comma = 0;
                count_o--;
                pos1++;
            }
            else if ((c == 't' && json[pos1 + 1] == 'r' && json[pos1 + 2] == 'u' && json[pos1 + 3] == 'e') ||
                    (c == 'f' && json[pos1 + 1] == 'a' && json[pos1 + 2] == 'l' && json[pos1 + 3] == 's' && json[pos1 + 4] == 'e')) {
                pos2 = (sVal1 == nullptr) ? pos1 : posn;
                if (current->size() > 0 && comma == 0) throw _GNU_JSON_ERROR(start, line);
                else if (comma > 0 && current->size() == 0) throw _GNU_JSON_ERROR(start, line);
                else if (current->is_object() == true && colon != 1) throw _GNU_JSON_ERROR(start, line);
                else if (current->_add_bool(&sVal1, c == 't', ignore_duplicates, pos2) == false) throw _GNU_JSON_ERROR(start, line);

                colon = 0;
                comma = 0;
                pos1 += 4;
                pos1 += (c == 'f');
            }
            else if (c == 'n' && json[pos1 + 1] == 'u' && json[pos1 + 2] == 'l' && json[pos1 + 3] == 'l') {
                pos2 = (sVal1 == nullptr) ? pos1 : posn;
                if (current->size() > 0 && comma == 0) throw _GNU_JSON_ERROR(start, line);
                else if (comma > 0 && current->size() == 0) throw _GNU_JSON_ERROR(start, line);
                else if (current->is_object() == true && colon != 1) throw _GNU_JSON_ERROR(start, line);
                else if (current->_add_nil(&sVal1, ignore_duplicates, pos1) == false) throw _GNU_JSON_ERROR(start, line);

                colon = 0;
                comma = 0;
                pos1 += 4;
            }
            else {
                throw _GNU_JSON_ERROR(pos1, line);
            }

            if (count_a > (int) JS::MAX_DEPTH || count_o > (int) JS::MAX_DEPTH) {
                throw _GNU_JSON_ERROR(pos1, line);
            }
        }

        if (count_a != 0 || count_o != 0) {
            throw _GNU_JSON_ERROR(len, 1);
        }
        else if (tmp.size() != 1) {
            throw _GNU_JSON_ERROR(len, 1);
        }
        else if (tmp[0]->_type == TYPE::ARRAY) {
            ret._type = TYPE::ARRAY;
            ret._va = tmp[0]->_va;
            ret._set_child_parent_to_me();
            const_cast<JS*>(tmp[0])->_type = TYPE::NIL;
        }
        else if (tmp[0]->_type == TYPE::OBJECT) {
            ret._type = TYPE::OBJECT;
            ret._vo = tmp[0]->_vo;
            ret._set_child_parent_to_me();
            const_cast<JS*>(tmp[0])->_type = TYPE::NIL;
        }
        else if (tmp[0]->_type == TYPE::BOOL) {
            ret._type = TYPE::BOOL;
            ret._vb   = tmp[0]->_vb;
        }
        else if (tmp[0]->_type == TYPE::NUMBER) {
            ret._type = TYPE::NUMBER;
            ret._vn   = tmp[0]->_vn;
        }
        else if (tmp[0]->_type == TYPE::STRING) {
            ret._type = TYPE::STRING;
            ret._vs   = tmp[0]->_vs;
            const_cast<JS*>(tmp[0])->_type = TYPE::NIL;
        }
        else if (tmp[0]->_type == TYPE::NIL) {
            ret._type = TYPE::NIL;
        }
        else {
            throw _GNU_JSON_ERROR(0, 1);
        }
    }
    catch(const std::string& err) {
        _GNU_JSON_FREE_STRINGS(sVal1, sVal2)
        ret._set_err(err);
    }

    return ret;
}

//------------------------------------------------------------------------------
json::JS json::decode(const std::string& json, bool ignore_trailing_comma, bool ignore_duplicates, bool ignore_utf_check) {
    return decode(json.c_str(), json.length(), ignore_trailing_comma, ignore_duplicates, ignore_utf_check);
}

//------------------------------------------------------------------------------
std::string json::encode(const JS& js, ENCODE option) {
    std::string t;
    std::string j;

    try {
        if (js.is_array() == true || js.is_object() == true) {
            json::_encode_all(js, j, t, false, option);
        }
        else {
            j = json::_encode(js, true, option);
        }
    }
    catch (const std::string& e) {
        j = e;
    }

    return j;
}

//------------------------------------------------------------------------------
std::string json::escape(const char* string) {
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
std::string json::format_number(double f, bool E) {
    double ABS = fabs(f);
    double MIN = ABS - static_cast<int64_t>(ABS);
    size_t n   = 0;
    char b[100];

    if (ABS > 999'000'000'000) {
        n = (E == true) ? snprintf(b, 100, "%e", f) : snprintf(b, 100, "%f", f);
    }
    else {
        if (MIN < 0.0000001) {
            n = (E == true) ? snprintf(b, 100, "%.0e", f) : snprintf(b, 100, "%.0f", f);
        }
        else if (MIN < 0.001) {
            n = (E == true) ? snprintf(b, 100, "%.7e", f) : snprintf(b, 100, "%.7f", f);
        }
        else {
            n = (E == true) ? snprintf(b, 100, "%e", f) : snprintf(b, 100, "%f", f);
        }
    }

    if (n < 1 || n >= 100) {
        return "0";
    }

    std::string s = b;

    if (s.find('.') == std::string::npos) {
        return s;
    }

    while (s.back() == '0') {
        s.pop_back();
    }

    if (s.back() == '.') {
        s.pop_back();
    }

    return s;
}

//------------------------------------------------------------------------------
std::string json::unescape(const char* string) {
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
 *           _  _____
 *          | |/ ____|
 *          | | (___
 *      _   | |\___ \
 *     | |__| |____) |
 *      \____/|_____/
 *
 *
 */

ssize_t json::JS::COUNT = 0;
ssize_t json::JS::MAX   = 0;

//------------------------------------------------------------------------------
json::JS::JS() {
    JS::COUNT++;
    if (JS::COUNT > JS::MAX) JS::MAX = JS::COUNT;

    _inline = false;
    _name   = nullptr;
    _parent = nullptr;
    _pos    = 0;
    _type   = TYPE::NIL;
    _vb     = false;
}

//------------------------------------------------------------------------------
json::JS::JS(const char* name, JS* parent, unsigned pos) {
    JS::COUNT++;
    if (JS::COUNT > JS::MAX) JS::MAX = JS::COUNT;

    _inline = false;
    _name   = (name != nullptr) ? strdup(name) : nullptr;
    _parent = parent;
    _pos    = pos;
    _type   = TYPE::NIL;
    _vb     = false;
}

//------------------------------------------------------------------------------
json::JS::JS(JS&& other) {
    JS::COUNT++;
    if (JS::COUNT > JS::MAX) JS::MAX = JS::COUNT;

    _inline = other._inline;
    _name   = other._name;
    _parent = other._parent;
    _pos    = other._pos;
    _type   = other._type;

    if (other._type == TYPE::ARRAY) {
        _va = other._va;
    }
    else if (other._type == TYPE::OBJECT) {
        _vo = other._vo;
    }
    else if (other._type == TYPE::BOOL) {
        _vb = other._vb;
    }
    else if (other._type == TYPE::STRING) {
        _vs = other._vs;
    }
    else if (other._type == TYPE::NUMBER) {
        _vn = other._vn;
    }

    other._type = TYPE::NIL;
    other._name = nullptr;

    _set_child_parent_to_me();
}

//------------------------------------------------------------------------------
json::JS::~JS() {
    JS::COUNT--;
    _clear(true);
}

//------------------------------------------------------------------------------
json::JS& json::JS::operator=(JS&& other) {
    _clear(true);

    _inline = other._inline;
    _name   = other._name;
    _parent = other._parent;
    _pos    = other._pos;
    _type   = other._type;

    if (other._type == TYPE::ARRAY) {
        _va = other._va;
    }
    else if (other._type == TYPE::OBJECT) {
        _vo = other._vo;
    }
    else if (other._type == TYPE::BOOL) {
        _vb = other._vb;
    }
    else if (other._type == TYPE::STRING) {
        _vs = other._vs;
    }
    else if (other._type == TYPE::NUMBER) {
        _vn = other._vn;
    }

    other._type = TYPE::NIL;
    other._name = nullptr;

    _set_child_parent_to_me();

    return *this;
}


//------------------------------------------------------------------------------
bool json::JS::_add_bool(char** sVal1, bool b, bool ignore_duplicates, unsigned pos) {
    bool res = false;

    if (is_array() == true) {
        _va->push_back(JS::_MakeBool("", b, this, pos));
        res = true;
    }
    else if (is_object() == true) {
        res = _set_object(*sVal1, json::JS::_MakeBool(*sVal1, b, this, pos), ignore_duplicates);
    }

    free(*sVal1);
    *sVal1 = nullptr;
    return res;
}

//------------------------------------------------------------------------------
bool json::JS::_add_nil(char** sVal1, bool ignore_duplicates, unsigned pos) {
    bool res = false;

    if (is_array() == true) {
        _va->push_back(JS::_MakeNil("", this, pos));
        res = true;
    }
    else if (is_object() == true) {
        res = _set_object(*sVal1, json::JS::_MakeNil(*sVal1, this, pos), ignore_duplicates);
    }

    free(*sVal1);
    *sVal1 = nullptr;
    return res;
}

//------------------------------------------------------------------------------
bool json::JS::_add_number(char** sVal1, double& nVal, bool ignore_duplicates, unsigned pos) {
    bool res = false;

    if (is_array() == true && std::isnan(nVal) == false) {
        _va->push_back(JS::_MakeNumber("", nVal, this, pos));
        res = true;
    }
    else if (is_object() == true && std::isnan(nVal) == false) {
        res = _set_object(*sVal1, json::JS::_MakeNumber(*sVal1, nVal, this, pos), ignore_duplicates);
    }

    free(*sVal1);
    *sVal1 = nullptr;
    nVal = NAN;
    return res;
}

//------------------------------------------------------------------------------
bool json::JS::_add_string(char** sVal1, char** sVal2, bool ignore_duplicates, unsigned pos) {
    bool res = false;

    if (is_array() == true && *sVal1 != nullptr && *sVal2 == nullptr) {
        _va->push_back(JS::_MakeString("", *sVal1, this, pos));
        res = true;
    }
    else if (is_object() == true && *sVal1 != nullptr && *sVal2 != nullptr) {
        res = _set_object(*sVal1, json::JS::_MakeString(*sVal1, *sVal2, this, pos), ignore_duplicates);
    }

    free(*sVal1);
    free(*sVal2);

    *sVal1 = nullptr;
    *sVal2 = nullptr;

    return res;
}

//------------------------------------------------------------------------------
void json::JS::_clear(bool name) {
    if (is_array() == true) {
        for (auto js : *_va) {
            delete js;
        }

        delete _va;
        _va = nullptr;
    }
    else if (is_object() == true) {
        for (auto js : *_vo) {
            delete js.second;
        }

        delete _vo;
        _vo = nullptr;
    }
    else if (is_string() == true || has_err() == true) {
        free(_vs);
        _vs = nullptr;
    }

    if (name == true) {
        free(_name);
        _name = nullptr;
    }

    _type   = TYPE::NIL;
    _vb     = false;
    _parent = nullptr;
}

//------------------------------------------------------------------------------
void json::JS::debug() const {
    std::string t;
    _debug(this, t);
}

//------------------------------------------------------------------------------
const json::JS* json::JS::find(std::string name, bool rec) const {
    if (is_object() == true) {
        auto find1 = _vo->find(name);

        if (find1 != _vo->end()) {
            return find1->second;
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
const json::JS* json::JS::_get_object(const char* name, bool escape) const {
    if (is_object() == true) {
        auto key   = (escape == true) ? json::escape(name) : name;
        auto find1 = _vo->find(key);
        return (find1 != _vo->end()) ? find1->second : nullptr;
    }

    return nullptr;
}

//------------------------------------------------------------------------------
json::JS* json::JS::_MakeArray(const char* name, JS* parent, unsigned pos) {
    auto r   = new JS(name, parent, pos);
    r->_type = TYPE::ARRAY;
    r->_va   = new JSArray();

    return r;
}

//------------------------------------------------------------------------------
json::JS* json::JS::_MakeBool(const char* name, bool vb, JS* parent, unsigned pos) {
    auto r   = new JS(name, parent, pos);
    r->_type = TYPE::BOOL;
    r->_vb   = vb;

    return r;
}

//------------------------------------------------------------------------------
json::JS* json::JS::_MakeNil(const char* name, JS* parent, unsigned pos) {
    return new JS(name, parent, pos);
}

//------------------------------------------------------------------------------
json::JS* json::JS::_MakeNumber(const char* name, double vn, JS* parent, unsigned pos) {
    auto r   = new JS(name, parent, pos);
    r->_type = TYPE::NUMBER;
    r->_vn   = vn;

    return r;
}

//------------------------------------------------------------------------------
json::JS* json::JS::_MakeObject(const char* name, JS* parent, unsigned pos) {
    auto r   = new JS(name, parent, pos);
    r->_type = TYPE::OBJECT;
    r->_vo   = new JSObject();

    return r;
}

//------------------------------------------------------------------------------
json::JS* json::JS::_MakeString(const char* name, const char* vs, JS* parent, unsigned pos) {
    auto r   = new JS(name, parent, pos);
    r->_type = TYPE::STRING;
    r->_vs   = strdup(vs);

    return r;
}

//------------------------------------------------------------------------------
void json::JS::_set_err(const std::string& err) {
    _clear(true);

    _vs   = strdup(err.c_str());
    _type = TYPE::ERR;
}

//------------------------------------------------------------------------------
// Delete existing object before setting new.
//
bool json::JS::_set_object(const char* name, JS* js, bool ignore_duplicates) {
    if (is_object() == true) {
        auto find1 = _vo->find(name);

        if (find1 != _vo->end()) {
            if (ignore_duplicates == false) {
                delete js;
                return false;
            }
            else {
                delete find1->second;
            }
        }

        (*_vo)[name] = js;
    }

    return true;
}

//------------------------------------------------------------------------------
void json::JS::_set_child_parent_to_me() {
    if (is_array() == true) {
        for (auto o : *_va) {
            o->_parent = this;
        }
    }
    else if (is_object() == true) {
        for (const auto& it : *_vo) {
            it.second->_parent = this;
        }
    }
}

//------------------------------------------------------------------------------
std::string json::JS::to_string() const {
    std::string ret = type_name();
    char b[100];

    if (_type == TYPE::NUMBER) {
        snprintf(b, 100, ": %f", _vn);
        ret += b;
    }
    else if (_type == TYPE::STRING) {
        ret += ": ";
        ret += _vs;
    }
    else if (_type == TYPE::BOOL) {
        ret += ": ";
        ret += _vb ? "true" : "false";
    }
    else if (_type == TYPE::ARRAY || _type == TYPE::OBJECT) {
        snprintf(b, 100, ": %d", static_cast<int>(size()));
        ret += b;
    }

    return ret;
}

//------------------------------------------------------------------------------
const json::JSArray json::JS::vo_to_va() const {
    JSArray res;

    if (_type == TYPE::OBJECT) {
        for (auto& m : *_vo) {
            res.push_back(m.second);
        }
    }

    return res;
}

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
json::Builder& json::Builder::add(JS* js) {
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
            else if (*js == TYPE::ARRAY || *js == TYPE::OBJECT) {
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
std::string json::Builder::encode(ENCODE option) const {
    if (_root == nullptr) {
        throw std::string("error: empty json");
    }
    else if (_name != "") {
        throw std::string("error: unused name value <" + _name + ">");
    }

    auto j = json::encode(*_root, option);

    if (j.find("error") == 0) {
        throw j;
    }

    return j;
}

//------------------------------------------------------------------------------
json::Builder& json::Builder::end() {
    if (_current == nullptr) {
        throw std::string("error: empty json");
    }
    else if (_current == _root) {
        throw std::string("error: already at the top level");
    }

    _current = _current->parent();
    return *this;
}

//------------------------------------------------------------------------------
json::JS* json::Builder::MakeArray(const char* name, bool escape) {
    auto r   = new JS((escape == true) ? json::escape(name).c_str() : name);
    r->_type = TYPE::ARRAY;
    r->_va   = new JSArray();

    return r;
}

//------------------------------------------------------------------------------
json::JS* json::Builder::MakeArrayInline(const char* name, bool escape) {
    auto r     = new JS((escape == true) ? json::escape(name).c_str() : name);
    r->_type   = TYPE::ARRAY;
    r->_va     = new JSArray();
    r->_inline = true;

    return r;
}

//------------------------------------------------------------------------------
json::JS* json::Builder::MakeBool(bool vb, const char* name, bool escape) {
    auto r   = new JS((escape == true) ? json::escape(name).c_str() : name);
    r->_type = TYPE::BOOL;
    r->_vb   = vb;

    return r;
}

//------------------------------------------------------------------------------
json::JS* json::Builder::MakeNull(const char* name, bool escape) {
    auto r   = new JS((escape == true) ? json::escape(name).c_str() : name);
    r->_type = TYPE::NIL;

    return r;
}

//------------------------------------------------------------------------------
json::JS* json::Builder::MakeNumber(double vn, const char* name, bool escape) {
    auto r   = new JS((escape == true) ? json::escape(name).c_str() : name);
    r->_type = TYPE::NUMBER;
    r->_vn   = vn;

    return r;
}

//------------------------------------------------------------------------------
json::JS* json::Builder::MakeObject(const char* name, bool escape) {
    auto r   = new JS((escape == true) ? json::escape(name).c_str() : name);
    r->_type = TYPE::OBJECT;
    r->_vo   = new JSObject();

    return r;
}

//------------------------------------------------------------------------------
json::JS* json::Builder::MakeObjectInline(const char* name, bool escape) {
    auto r     = new JS((escape == true) ? json::escape(name).c_str() : name);
    r->_type   = TYPE::OBJECT;
    r->_vo     = new JSObject();
    r->_inline = true;

    return r;
}

//------------------------------------------------------------------------------
json::JS* json::Builder::MakeString(const char* vs, const char* name, bool escape) {
    auto r   = new JS((escape == true) ? json::escape(name).c_str() : name);
    r->_type = TYPE::STRING;
    r->_vs   = strdup((escape == true) ? json::escape(vs).c_str() : vs);

    return r;
}

//------------------------------------------------------------------------------
json::JS* json::Builder::MakeString(std::string vs, const char* name, bool escape) {
    auto r   = new JS((escape == true) ? json::escape(name).c_str() : name);
    r->_type = TYPE::STRING;
    r->_vs   = strdup((escape == true) ? json::escape(vs.c_str()).c_str() : vs.c_str());

    return r;
}

} // flw

// MKALGAM_OFF