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

#include "json.h"

// MKALGAM_ON

#include <cmath>
#include <cstdint>
#include <errno.h>
#include <climits>

namespace gnu {
namespace priv {

/*
 *                 _            _
 *                (_)          | |
 *      _ __  _ __ ___   ____ _| |_ ___
 *     | '_ \| '__| \ \ / / _` | __/ _ \
 *     | |_) | |  | |\ V / (_| | ||  __/
 *     | .__/|_|  |_| \_/ \__,_|\__\___|
 *     | |
 *     |_|
 */

#define _GNU_JSON_ERROR(X,Y) priv::_json_format_error(__LINE__, static_cast<unsigned>(X), Y)
#define _GNU_JSON_FREE_STRINGS(X,Y) free(X); free(Y); X = Y = nullptr;

static const char* const _JSON_BOM = "\xef\xbb\xbf";

/** @brief Print json node and all child nodes to stdout.
*
* @param[in] js  JSON node.
* @param[in] t   Indendtation string.
*/
static void _json_debug(const json::JS* js, std::string t) {
    if (js->is_array() == true) {
        printf("%sARRAY(%u, %u, %p, %p): \"%s\"\n", t.c_str(), js->pos(), static_cast<unsigned>(js->size()), js, js->parent(), js->name().c_str());
        t += "\t";
        for (const auto js2 : *js->va()) _json_debug(js2, t);
        t.pop_back();
    }
    else if (js->is_object() == true) {
        printf("%sOBJECT(%u, %u, %p, %p): \"%s\"\n", t.c_str(), js->pos(), static_cast<unsigned>(js->size()), js, js->parent(), js->name().c_str());
        t += "\t";
        for (auto js2 : *js->vo()) _json_debug(js2.second, t);
        t.pop_back();
    }
    else if (js->is_null()) printf("%s%s(%u, %p): \"%s\"\n", t.c_str(), js->type_name().c_str(), js->pos(), js->parent(), js->name_c());
    else if (js->is_string()) printf("%s%s(%u, %p): \"%s\": \"%s\"\n", t.c_str(), js->type_name().c_str(), js->pos(), js->parent(), js->name_c(), js->vs_c());
    else if (js->is_number()) printf("%s%s(%u, %p): \"%s\": %f\n", t.c_str(), js->type_name().c_str(), js->pos(), js->parent(), js->name_c(), js->vn());
    else if (js->is_bool()) printf("%s%s(%u, %p): \"%s\": %s\n", t.c_str(), js->type_name().c_str(), js->pos(), js->parent(), js->name_c(), js->vb() ? "true" : "false");
    fflush(stdout);
}

/** @brief Encode one json node.
*
* @param[in] js           JSON value.
* @param[in] ignore_name  True to make it nameless.
* @param[in] option       Whitespace option.
*
* @return Encoded string.
*/
static std::string _json_encode(const json::JS& js, bool ignore_name, json::Encode option) {
    static const std::string QUOTE = "\"";

    std::string res;
    std::string arr    = (option == json::Encode::DEFAULT) ? "\": [" : "\":[";
    std::string obj    = (option == json::Encode::DEFAULT) ? "\": {" : "\":{";
    std::string name1  = (option == json::Encode::DEFAULT) ? "\": " : "\":";
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

/** @brief Encode json value inline.
*
* No new line between values, only comma.
*
* @param[in]     js      JSON value.
* @param[in,out] j       Result string.
* @param[in]     comma   True to add comma.
* @param[in]     option  Whitespace option.
*/
static void _json_encode_inline(const json::JS& js, std::string& j, bool comma, json::Encode option) {
    std::string c = (comma == true) ? "," : "";
    size_t      f = 0;

    if (js.is_array() == true) {
        j += priv::_json_encode(js, false, option);

        for (const auto n : *js.va()) {
            priv::_json_encode_inline(*n, j, f < (js.va()->size() - 1), option);
            f++;
        }

        j += "]" + c;
    }
    else if (js.is_object() == true) {
        j += priv::_json_encode(js, false, option);

        for (const auto& n : *js.vo()) {
            priv::_json_encode_inline(*n.second, j, f < (js.vo()->size() - 1), option);
            f++;
        }

        j += "}" + c;
    }
    else {
        j += priv::_json_encode(js, false, option) + c;
    }
}

/** @brief Encode all json nodes.
*
* @param[in]     js      Root node.
* @param[in,out] j       Result string.
* @param[in,out] t       Indendtation string.
* @param[in]     comma   True to add comma.
* @param[in]     option  Whitespace option.
*/
static void _json_encode_all(const json::JS& js, std::string& j, std::string& t, bool comma, json::Encode option) {
    std::string c = (comma == true) ? "," : "";
    std::string n = (option == json::Encode::FLAT) ? "" : "\n";
    size_t      f = 0;

    if (js.is_array() == true) {
        j += t + priv::_json_encode(js, j == "", option);

        if (js.has_inline() == false && js.size() > 0) {
            j += n;
        }

        for (const auto n2 : *js.va()) {
            if (option == json::Encode::DEFAULT) {
                t += "\t";
            }

            if (js.has_inline() == true) {
                priv::_json_encode_inline(*n2, j, f < (js.va()->size() - 1), option);
            }
            else {
                priv::_json_encode_all(*n2, j, t, f < (js.va()->size() - 1), option);
            }

            if (option == json::Encode::DEFAULT) {
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
        j += t + priv::_json_encode(js, j == "", option);

        if (js.has_inline() == false && js.size() > 0) {
            j += n;
        }

        for (const auto& n2 : *js.vo()) {
            if (option == json::Encode::DEFAULT) {
                t += "\t";
            }

            if (js.has_inline() == true) {
                priv::_json_encode_inline(*n2.second, j, f < (js.vo()->size() - 1), option);
            }
            else {
                priv::_json_encode_all(*n2.second, j, t, f < (js.vo()->size() - 1), option);
            }

            if (option == json::Encode::DEFAULT) {
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
        j += t + priv::_json_encode(js, false, option) + c + n;
    }
}

/** @brief Format error string.
*
* @param[in] source  Source line.
* @param[in] pos     Byte pos in json data.
* @param[in] line    Line number in json data.
*
* @return Error string.
*/
static std::string _json_format_error(unsigned source, unsigned pos, unsigned line) {
    char buf[256];
    snprintf(buf, 256, "Error: invalid json (%u) at pos %u and line %u", source, pos, line);
    return buf;
}

/** @brief Convert string to number.
*
* @param[in]     json   JSON string.
* @param[in]     len    JSON string length.
* @param[in,out] pos    Position in json string.
* @param[in,out] nVal   Result number, contains NAN for any failure.
*
* @return True if ok.
*/
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
            minus = plus = E = 0;

            for (auto c : n2) {
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

/** @brief Parse string.
*
* @param[in]     ignore_utf_check  True to skip basic utf8 check.
* @param[in]     json              JSON string.
* @param[in]     len               JSON string length.
* @param[in,out] pos               Position in json string.
* @param[in,out] sVal1             Result string if it is NULL.
* @param[in,out] sVal2             Result string if it is NULL but sval1 is not NULL.
*
* @return True if any of result strings have been set.
*/
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

} // gnu::json
} // json

/*
 *        _
 *       (_)
 *        _ ___  ___  _ __
 *       | / __|/ _ \| '_ \
 *       | \__ \ (_) | | | |
 *       | |___/\___/|_| |_|
 *      _/ |
 *     |__/
 */

/** @brief Basic utf8 character counting
*
* @param[in] p  String to count.
*
* @return Number of characters or 0 if empty or it has invalid characters.
*/
size_t gnu::json::count_utf8(const char* p) {
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

/** @brief Decode json string into json values.
*
* @param[in] json                   JSON string data.
* @param[in] len                    Length of json string.
* @param[in] ignore_trailing_comma  True to ignore trailing commas.
* @param[in] ignore_duplicates      True to ignore duplicate names.
* @param[in] ignore_utf_check       True to skip basic utf8 check.
*
* @return Root JSON node, type will be set to Type::ERR for any error.
*/
gnu::json::JS gnu::json::decode(const char* json, size_t len, bool ignore_trailing_comma, bool ignore_duplicates, bool ignore_utf_check) {
    auto ret     = JS();                // Return value.
    auto tmp     = JS();                // Temporary working root
    auto colon   = 0;                   // Colon counter.
    auto comma   = 0;                   // Comman counter.
    auto count_a = 0;                   // Array counter, should be 0 when done or error.
    auto count_o = 0;                   // Object counter, should be 0 when done or error.
    auto current = (JS*) nullptr;       // Current active object.
    auto line    = (unsigned) 1;        // Line counter.
    auto n       = (JS*) nullptr;       // Value to be added.
    auto nVal    = (double) NAN;        // JSON number.
    auto pos1    = (size_t) 0;          // Position in input string.
    auto pos2    = (size_t) 0;          // Temporary position.
    auto posn    = (size_t) 0;          // Temporary position.
    auto sVal1   = (char*) nullptr;     // JSON String value.
    auto sVal2   = (char*) nullptr;     // JSON String value.

    try {
        tmp._type = Type::ARRAY;
        tmp._va   = new JSArray();
        current   = &tmp;

        if (strncmp(json, priv::_JSON_BOM, 3) == 0) {
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
            else if (c == '"') { // Parse string.
                pos2 = pos1;

                if (sVal1 == nullptr) {
                    posn = pos1;
                }

                if (priv::_json_parse_string(ignore_utf_check, json, len, pos1, &sVal1, &sVal2) == false) {
                    throw _GNU_JSON_ERROR(start, line);
                }

                auto add_object = (current->is_object() == true && sVal2 != nullptr);
                auto add_array = (current->is_array() == true);

                if (comma > 0 && current->size() == 0) {
                    throw _GNU_JSON_ERROR(start, line);
                }
                else if (comma == 0 && current->size() > 0) {
                    throw _GNU_JSON_ERROR(start, line);
                }
                else if (add_object == true && colon != 1) {
                    throw _GNU_JSON_ERROR(start, line);
                }
                else if (add_object == true || add_array == true) {
                    pos2 = (sVal2 == nullptr) ? pos2 : posn;

                    if (current->_add_string(&sVal1, &sVal2, ignore_duplicates, pos2) == false) {
                        throw _GNU_JSON_ERROR(start, line);
                    }

                    colon = 0;
                    comma = 0;
                }

                pos1++;
            }
            else if ((c >= '0' && c <= '9') || c == '-') { // Parse number.
                pos2 = (sVal1 == nullptr) ? pos1 : posn;

                if (priv::_json_parse_number(json, len, pos1, nVal) == false) {
                    throw _GNU_JSON_ERROR(start, line);
                }
                else if (comma > 0 && current->size() == 0) {
                    throw _GNU_JSON_ERROR(start, line);
                }
                else if (comma == 0 && current->size() > 0) {
                    throw _GNU_JSON_ERROR(start, line);
                }
                else if (current->is_object() == true && colon != 1) {
                    throw _GNU_JSON_ERROR(start, line);
                }
                else if (current->_add_number(&sVal1, nVal, ignore_duplicates, pos2) == false) {
                    throw _GNU_JSON_ERROR(start, line);
                }

                colon = 0;
                comma = 0;
                pos1++;
            }
            else if (c == ',') { // Comma separator, throw if two commas.
                if (comma > 0) {
                    throw _GNU_JSON_ERROR(pos1, line);
                }
                else if (current == &tmp) {
                    throw _GNU_JSON_ERROR(pos1, line);
                }

                comma++;
                pos1++;
            }
            else if (c == ':') {
                if (colon > 0) {
                    throw _GNU_JSON_ERROR(pos1, line);
                }
                else if (current->is_object() == false) {
                    throw _GNU_JSON_ERROR(pos1, line);
                }
                else if (sVal1 == nullptr) {
                    throw _GNU_JSON_ERROR(pos1, line);
                }

                colon++;
                pos1++;
            }
            else if (c == '[') { // Start of array.
                if (current->size() == 0 && comma > 0) {
                    throw _GNU_JSON_ERROR(pos1, line);
                }
                else if (current->size() > 0 && comma != 1) {
                    throw _GNU_JSON_ERROR(pos1, line);
                }
                else if (current->is_array() == true) {
                    if (sVal1 != nullptr) {
                        throw _GNU_JSON_ERROR(pos1, line);
                    }

                    n = JS::_MakeArray("", current, pos1);
                    current->_va->push_back(n);
                }
                else {
                    if (sVal1 == nullptr) {
                        throw _GNU_JSON_ERROR(pos1, line);
                    }
                    else if (colon != 1) {
                        throw _GNU_JSON_ERROR(pos1, line);
                    }

                    n = JS::_MakeArray(sVal1, current, pos2);

                    if (current->_set_value(sVal1, n, ignore_duplicates) == false) {
                        throw _GNU_JSON_ERROR(pos1, line);
                    }

                    _GNU_JSON_FREE_STRINGS(sVal1, sVal2)
                }

                current = n;
                colon = 0;
                comma = 0;
                count_a++;
                pos1++;
            }
            else if (c == ']') { // End of array.
                if (current->_parent == nullptr) {
                    throw _GNU_JSON_ERROR(pos1, line);
                }
                else if (current->is_array() == false) {
                    throw _GNU_JSON_ERROR(pos1, line);
                }
                else if (sVal1 != nullptr || sVal2 != nullptr) {
                    throw _GNU_JSON_ERROR(pos1, line);
                }
                else if (comma > 0 && ignore_trailing_comma == false) {
                    throw _GNU_JSON_ERROR(pos1, line);
                }
                else if (count_a < 0) {
                    throw _GNU_JSON_ERROR(pos1, line);
                }

                current = current->_parent;
                comma = 0;
                count_a--;
                pos1++;
            }
            else if (c == '{') { // Start of object.
                if (current->size() == 0 && comma > 0) {
                    throw _GNU_JSON_ERROR(pos1, line);
                }
                else if (current->size() > 0 && comma != 1) {
                    throw _GNU_JSON_ERROR(pos1, line);
                }
                else if (current->is_array() == true) {
                    if (sVal1 != nullptr) {
                        throw _GNU_JSON_ERROR(pos1, line);
                    }

                    n = JS::_MakeObject("", current, pos1);
                    current->_va->push_back(n);
                }
                else {
                    if (sVal1 == nullptr) {
                        throw _GNU_JSON_ERROR(pos1, line);
                    }
                    else if (colon != 1) {
                        throw _GNU_JSON_ERROR(pos1, line);
                    }

                    n = JS::_MakeObject(sVal1, current, posn);

                    if (current->_set_value(sVal1, n, ignore_duplicates) == false) {
                        throw _GNU_JSON_ERROR(pos1, line);
                    }

                    _GNU_JSON_FREE_STRINGS(sVal1, sVal2)
                }

                current = n;
                colon = 0;
                comma = 0;
                count_o++;
                pos1++;
            }
            else if (c == '}') { // End of object.
                if (current->_parent == nullptr) {
                    throw _GNU_JSON_ERROR(pos1, line);
                }
                else if (current->is_object() == false) {
                    throw _GNU_JSON_ERROR(pos1, line);
                }
                else if (sVal1 != nullptr || sVal2 != nullptr) {
                    throw _GNU_JSON_ERROR(pos1, line);
                }
                else if (comma > 0 && ignore_trailing_comma == false) {
                    throw _GNU_JSON_ERROR(pos1, line);
                }
                else if (count_o < 0) {
                    throw _GNU_JSON_ERROR(pos1, line);
                }

                current = current->_parent;
                comma = 0;
                count_o--;
                pos1++;
            }
            else if (
                    (c == 't' && json[pos1 + 1] == 'r' && json[pos1 + 2] == 'u' && json[pos1 + 3] == 'e') ||
                    (c == 'f' && json[pos1 + 1] == 'a' && json[pos1 + 2] == 'l' && json[pos1 + 3] == 's' && json[pos1 + 4] == 'e')
                ) { // True or false values.
                pos2 = (sVal1 == nullptr) ? pos1 : posn;

                if (current->size() > 0 && comma == 0) {
                    throw _GNU_JSON_ERROR(start, line);
                }
                else if (comma > 0 && current->size() == 0) {
                    throw _GNU_JSON_ERROR(start, line);
                }
                else if (current->is_object() == true && colon != 1) {
                    throw _GNU_JSON_ERROR(start, line);
                }
                else if (current->_add_bool(&sVal1, c == 't', ignore_duplicates, pos2) == false) {
                    throw _GNU_JSON_ERROR(start, line);
                }

                colon = 0;
                comma = 0;
                pos1 += 4;
                pos1 += (c == 'f');
            }
            else if (c == 'n' && json[pos1 + 1] == 'u' && json[pos1 + 2] == 'l' && json[pos1 + 3] == 'l') { // NULL value.
                pos2 = (sVal1 == nullptr) ? pos1 : posn;

                if (current->size() > 0 && comma == 0) {
                    throw _GNU_JSON_ERROR(start, line);
                }
                else if (comma > 0 && current->size() == 0) {
                    throw _GNU_JSON_ERROR(start, line);
                }
                else if (current->is_object() == true && colon != 1) {
                    throw _GNU_JSON_ERROR(start, line);
                }
                else if (current->_add_null(&sVal1, ignore_duplicates, pos1) == false) {
                    throw _GNU_JSON_ERROR(start, line);
                }

                colon = 0;
                comma = 0;
                pos1 += 4;
            }
            else { // Unknown input.
                throw _GNU_JSON_ERROR(pos1, line);
            }

            if (count_a > (int) json::MAX_DEPTH || count_o > (int) json::MAX_DEPTH) { // Break parsing it it has to deep structure.
                throw _GNU_JSON_ERROR(pos1, line);
            }
        }

        if (count_a != 0 || count_o != 0) {
            throw _GNU_JSON_ERROR(len, 1);
        }
        else if (tmp.size() != 1) { // Root value can have only one value.
            throw _GNU_JSON_ERROR(len, 1);
        }
        else if (tmp[0]->_type == Type::ARRAY) { // Child is array so set result value.
            ret._type = Type::ARRAY;
            ret._va = tmp[0]->_va;
            ret._set_child_parent_to_me();
            const_cast<JS*>(tmp[0])->_type = Type::NIL;
        }
        else if (tmp[0]->_type == Type::OBJECT) { // Child is object so set result value.
            ret._type = Type::OBJECT;
            ret._vo = tmp[0]->_vo;
            ret._set_child_parent_to_me();
            const_cast<JS*>(tmp[0])->_type = Type::NIL;
        }
        else if (tmp[0]->_type == Type::BOOL) { // Only one boolean.
            ret._type = Type::BOOL;
            ret._vb   = tmp[0]->_vb;
        }
        else if (tmp[0]->_type == Type::NUMBER) { // Only one number.
            ret._type = Type::NUMBER;
            ret._vn   = tmp[0]->_vn;
        }
        else if (tmp[0]->_type == Type::STRING) { // Only one string.
            ret._type = Type::STRING;
            ret._vs   = tmp[0]->_vs;
            const_cast<JS*>(tmp[0])->_type = Type::NIL;
        }
        else if (tmp[0]->_type == Type::NIL) { // Only one null value.
            ret._type = Type::NIL;
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

/** @brief Decode json string into json values.
*
* @param[in] json                   JSON string data.
* @param[in] ignore_trailing_comma  True to ignore trailing commas.
* @param[in] ignore_duplicates      True to ignore duplicate names.
* @param[in] ignore_utf_check       True to skip basic utf8 check.
*
* @return Root JSON node, type will be set to Type::ERR for any error.
*/
gnu::json::JS gnu::json::decode(const std::string& json, bool ignore_trailing_comma, bool ignore_duplicates, bool ignore_utf_check) {
    return decode(json.c_str(), json.length(), ignore_trailing_comma, ignore_duplicates, ignore_utf_check);
}

/** @brief Encode json node to string.
*
* @param[in] js      JSON object.
* @param[in] option  Whitespace option.
*
* @return JSON string.
*/
std::string gnu::json::encode(const JS& js, Encode option) {
    std::string t;
    std::string j;

    try {
        if (js.is_array() == true || js.is_object() == true) {
            priv::_json_encode_all(js, j, t, false, option);
        }
        else {
            j = priv::_json_encode(js, true, option);
        }
    }
    catch (const std::string& e) {
        j = e;
    }

    return j;
}

/** @brief Escape string.
*
* @param[in] string  String to escape.
*
* @return Escaped string.
*/
std::string gnu::json::escape(const char* string) {
    std::string res;
    res.reserve(strlen(string) + 5);

    while (*string != 0) {
        auto c = *string;

        if (c == 9) {
            res += "\\t";
        }
        else if (c == 10) {
            res += "\\n";
        }
        else if (c == 13) {
            res += "\\r";
        }
        else if (c == 8) {
            res += "\\b";
        }
        else if (c == 14) {
            res += "\\f";
        }
        else if (c == 34) {
            res += "\\\"";
        }
        else if (c == 92) {
            res += "\\\\";
        }
        else {
            res += c;
        }

        string++;
    }

    return res;
}

/** @brief Format a number to string.
*
* @param[in] f  Number.
* @param[in] E  True to create a exponential version.
*
* @return Converted number.
*/
std::string gnu::json::format_number(double f, bool E) {
    double ABS = fabs(f);
    double MIN = (ABS >= static_cast<double>(LLONG_MAX)) ? 0.0 : ABS - static_cast<int64_t>(ABS);
    size_t n   = 0;
    char b[400];

    if (ABS > 999'000'000'000) {
        n = (E == true) ? snprintf(b, 400, "%e", f) : snprintf(b, 400, "%f", f);
    }
    else {
        if (MIN < 0.0000001) {
            n = (E == true) ? snprintf(b, 400, "%.0e", f) : snprintf(b, 400, "%.0f", f);
        }
        else if (MIN < 0.001) {
            n = (E == true) ? snprintf(b, 400, "%.7e", f) : snprintf(b, 400, "%.7f", f);
        }
        else {
            n = (E == true) ? snprintf(b, 400, "%e", f) : snprintf(b, 400, "%f", f);
        }
    }

    if (n < 1 || n >= 400) {
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

/** @brief Unsecape string.
*
* @param[in] string  String to unescape.
*
* @return Unescaped string.
*/
std::string gnu::json::unescape(const char* string) {
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

/*
 *           _  _____
 *          | |/ ____|
 *          | | (___
 *      _   | |\___ \
 *     | |__| |____) |
 *      \____/|_____/
 *
 *
 */

ssize_t gnu::json::JS::COUNT = 0;
ssize_t gnu::json::JS::MAX   = 0;

/** @brief Create new empty object.
*
* The type is set to Type::NIL.
*/
gnu::json::JS::JS() {
    JS::COUNT++;

    if (JS::COUNT > JS::MAX) {
        JS::MAX = JS::COUNT;
    }

    _inl    = false;
    _name   = nullptr;
    _parent = nullptr;
    _pos    = 0;
    _type   = Type::NIL;
    _va     = nullptr;
}

/** @brief Create new object a name and a parent.
*
* @param[in] name    Name of value.
* @param[in] parent  Parent object.
* @param[in] pos     Position in input string.
*/
gnu::json::JS::JS(const char* name, JS* parent, unsigned pos) {
    JS::COUNT++;

    if (JS::COUNT > JS::MAX) {
        JS::MAX = JS::COUNT;
    }

    _inl    = false;
    _name   = (name != nullptr) ? strdup(name) : nullptr;
    _parent = parent;
    _pos    = pos;
    _type   = Type::NIL;
    _va     = nullptr;
}

/** @brief Move constructor.
*
* @param[in] other  Object to move.
*/
gnu::json::JS::JS(JS&& other) {
    JS::COUNT++;
    if (JS::COUNT > JS::MAX) JS::MAX = JS::COUNT;

    _inl    = other._inl;
    _name   = other._name;
    _parent = other._parent;
    _pos    = other._pos;
    _type   = other._type;

    if (other._type == Type::ARRAY) {
        _va = other._va;
    }
    else if (other._type == Type::OBJECT) {
        _vo = other._vo;
    }
    else if (other._type == Type::BOOL) {
        _vb = other._vb;
    }
    else if (other._type == Type::STRING) {
        _vs = other._vs;
    }
    else if (other._type == Type::NUMBER) {
        _vn = other._vn;
    }

    other._type = Type::NIL;
    other._name = nullptr;

    _set_child_parent_to_me();
}

/** @brief Delete memory.
*
*/
gnu::json::JS::~JS() {
    JS::COUNT--;
    _clear(true);
}

/** @brief Move operator.
*
* @param[in] other  Object to move.
*
* @return This object.
*/
gnu::json::JS& gnu::json::JS::operator=(JS&& other) {
    _clear(true);

    _inl    = other._inl;
    _name   = other._name;
    _parent = other._parent;
    _pos    = other._pos;
    _type   = other._type;

    if (other._type == Type::ARRAY) {
        _va = other._va;
    }
    else if (other._type == Type::OBJECT) {
        _vo = other._vo;
    }
    else if (other._type == Type::BOOL) {
        _vb = other._vb;
    }
    else if (other._type == Type::STRING) {
        _vs = other._vs;
    }
    else if (other._type == Type::NUMBER) {
        _vn = other._vn;
    }

    other._type = Type::NIL;
    other._name = nullptr;

    _set_child_parent_to_me();

    return *this;
}

/** @brief Add boolean object to array or object.
*
* @param[in,out] sVal1              Name for object value, its memory will be deleted.
* @param[in]     b                  Bool value.
* @param[in]     ignore_duplicates  True to ignore duplicates.
* @param[in]     pos                Pos in input data.
*
* @return True if ok.
*/
bool gnu::json::JS::_add_bool(char** sVal1, bool b, bool ignore_duplicates, unsigned pos) {
    bool res = false;

    if (is_array() == true) {
        _va->push_back(JS::_MakeBool("", b, this, pos));
        res = true;
    }
    else if (is_object() == true) {
        res = _set_value(*sVal1, json::JS::_MakeBool(*sVal1, b, this, pos), ignore_duplicates);
    }

    free(*sVal1);
    *sVal1 = nullptr;

    return res;
}

/** @brief Add null object to array or object.
*
* @param[in,out] sVal1              Name for object value, its memory will be deleted.
* @param[in]     ignore_duplicates  True to ignore duplicates.
* @param[in]     pos                Pos in input data.
*
* @return True if ok.
*/
bool gnu::json::JS::_add_null(char** sVal1, bool ignore_duplicates, unsigned pos) {
    bool res = false;

    if (is_array() == true) {
        _va->push_back(JS::_MakeNull("", this, pos));
        res = true;
    }
    else if (is_object() == true) {
        res = _set_value(*sVal1, json::JS::_MakeNull(*sVal1, this, pos), ignore_duplicates);
    }

    free(*sVal1);
    *sVal1 = nullptr;

    return res;
}

/** @brief Add number object to array or object.
*
* @param[in,out] sVal1              Name for object value, its memory will be deleted.
* @param[in,out] nVal               Number value, it will be set to NAN.
* @param[in]     ignore_duplicates  True to ignore duplicates.
* @param[in]     pos                Pos in input data.
*
* @return True if ok.
*/
bool gnu::json::JS::_add_number(char** sVal1, double& nVal, bool ignore_duplicates, unsigned pos) {
    bool res = false;

    if (is_array() == true && std::isnan(nVal) == false) {
        _va->push_back(JS::_MakeNumber("", nVal, this, pos));
        res = true;
    }
    else if (is_object() == true && std::isnan(nVal) == false) {
        res = _set_value(*sVal1, json::JS::_MakeNumber(*sVal1, nVal, this, pos), ignore_duplicates);
    }

    free(*sVal1);
    *sVal1 = nullptr;
    nVal = NAN;

    return res;
}

/** @brief Add string object to array or object.
*
* @param[in,out] sVal1              Name for object value, its memory will be deleted.
* @param[in,out] sVal2              String object value, its memory will be deleted.
* @param[in]     ignore_duplicates  True to ignore duplicates.
* @param[in]     pos                Pos in input data.
*
* @return True if ok.
*/
bool gnu::json::JS::_add_string(char** sVal1, char** sVal2, bool ignore_duplicates, unsigned pos) {
    bool res = false;

    if (is_array() == true && *sVal1 != nullptr && *sVal2 == nullptr) {
        _va->push_back(JS::_MakeString("", *sVal1, this, pos));
        res = true;
    }
    else if (is_object() == true && *sVal1 != nullptr && *sVal2 != nullptr) {
        res = _set_value(*sVal1, json::JS::_MakeString(*sVal1, *sVal2, this, pos), ignore_duplicates);
    }

    free(*sVal1);
    free(*sVal2);

    *sVal1 = nullptr;
    *sVal2 = nullptr;

    return res;
}

/** @brief Clear all values.
*
* @param[in] name  True to delete name also.
*/
void gnu::json::JS::_clear(bool name) {
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

    _type   = Type::NIL;
    _vb     = false;
    _parent = nullptr;
}

/** @brief Print debug info.
*
*/
void gnu::json::JS::debug() const {
    std::string t;
    priv::_json_debug(this, t);
}

/** @brief Find named JSON object.
*
* This instance must be an object (Type::OBJECT).
*
* @param[in] name  Name to search for.
* @param[in] rec   True to search recursive.
*
* @return Found node or NULL.
*/
const gnu::json::JS* gnu::json::JS::find(const std::string& name, bool rec) const {
    if (is_object() == false) {
        return nullptr;
    }

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

    return nullptr;
}

/** @brief Get value.
*
* @param[in] name    Name of value.
* @param[in] escape  True to escape name.
*
* @return Found value or NULL.
*/
const gnu::json::JS* gnu::json::JS::_get_value(const char* name, bool escape) const {
    if (is_object() == false) {
        return nullptr;
    }

    auto key   = (escape == true) ? json::escape(name) : name;
    auto find1 = _vo->find(key);

    return (find1 != _vo->end()) ? find1->second : nullptr;
}

/** @brief Make an array.
*
* @param[in] name    Name of value.
* @param[in] parent  Parent of new value.
* @param[in] pos     Index in file.
*
* @return Array value.
*/
gnu::json::JS* gnu::json::JS::_MakeArray(const char* name, JS* parent, unsigned pos) {
    auto r   = new JS(name, parent, pos);

    r->_type = Type::ARRAY;
    r->_va   = new JSArray();

    return r;
}

/** @brief Make an bool.
*
* @param[in] name    Name of value.
* @param[in] vb      Bool value.
* @param[in] parent  Parent of new value.
* @param[in] pos     Index in file.
*
* @return Bool value.
*/
gnu::json::JS* gnu::json::JS::_MakeBool(const char* name, bool vb, JS* parent, unsigned pos) {
    auto r   = new JS(name, parent, pos);

    r->_type = Type::BOOL;
    r->_vb   = vb;

    return r;
}

/** @brief Make an null value.
*
* @param[in] name    Name of value.
* @param[in] parent  Parent of new value.
* @param[in] pos     Index in file.
*
* @return Null value.
*/
gnu::json::JS* gnu::json::JS::_MakeNull(const char* name, JS* parent, unsigned pos) {
    return new JS(name, parent, pos);
}

/** @brief Make an number value.
*
* @param[in] name    Name of value.
* @param[in] vn      Number value.
* @param[in] parent  Parent of new value.
* @param[in] pos     Index in file.
*
* @return Nil value.
*/
gnu::json::JS* gnu::json::JS::_MakeNumber(const char* name, double vn, JS* parent, unsigned pos) {
    auto r   = new JS(name, parent, pos);

    r->_type = Type::NUMBER;
    r->_vn   = vn;

    return r;
}

/** @brief Make a object value.
*
* @param[in] name    Name of value.
* @param[in] parent  Parent of new value.
* @param[in] pos     Index in file.
*
* @return Object value.
*/
gnu::json::JS* gnu::json::JS::_MakeObject(const char* name, JS* parent, unsigned pos) {
    auto r   = new JS(name, parent, pos);
    r->_type = Type::OBJECT;
    r->_vo   = new JSObject();

    return r;
}

/** @brief Make a string.
*
* @param[in] name    Name of value.
* @param[in] vs      String value.
* @param[in] parent  Parent of new value.
* @param[in] pos     Index in file.
*
* @return Nil value.
*/
gnu::json::JS* gnu::json::JS::_MakeString(const char* name, const char* vs, JS* parent, unsigned pos) {
    auto r   = new JS(name, parent, pos);

    r->_type = Type::STRING;
    r->_vs   = strdup(vs);

    return r;
}

/** @brief Set error message
*
* @param[in] err  Message.
*/
void gnu::json::JS::_set_err(const std::string& err) {
    _clear(true);

    _vs   = strdup(err.c_str());
    _type = Type::ERR;
}

/** @brief Set value in object.
*
* Delete existing value before setting new.
*
* @param[in] name               New name.
* @param[in] js                 JS value.
* @param[in] ignore_duplicates  True to ignore duplicates.
*
* @return True if ok.
*/
bool gnu::json::JS::_set_value(const char* name, JS* js, bool ignore_duplicates) {
    if (is_object() == false) {
        return false;
    }

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

    return true;
}

/** @brief Set parent for all children thi this.
*
*/
void gnu::json::JS::_set_child_parent_to_me() {
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

/** @brief Get a string of this value.
*
* @return String description, not a real json value.
*/
std::string gnu::json::JS::to_string() const {
    std::string ret = type_name();
    char b[400];

    if (_type == Type::NUMBER) {
        snprintf(b, 400, ": %f", _vn);
        ret += b;
    }
    else if (_type == Type::STRING) {
        ret += ": ";
        ret += _vs;
    }
    else if (_type == Type::BOOL) {
        ret += ": ";
        ret += _vb ? "true" : "false";
    }
    else if (_type == Type::ARRAY || _type == Type::OBJECT) {
        snprintf(b, 400, ": %d children", static_cast<int>(size()));
        ret += b;
    }

    if (_name != nullptr && *_name != 0) {
        return std::string(_name) + ": " + ret;
    }
    else {
        return ret;
    }
}

/** @brief Create an array with all children for an object.
*
* @return JSON array.
*/
const gnu::json::JSArray gnu::json::JS::vo_to_va() const {
    JSArray res;

    if (_type != Type::OBJECT) {
        return res;
    }

    for (auto& m : *_vo) {
        res.push_back(m.second);
    }

    return res;
}

/*
 *      ____        _ _     _
 *     |  _ \      (_) |   | |
 *     | |_) |_   _ _| | __| | ___ _ __
 *     |  _ <| | | | | |/ _` |/ _ \ '__|
 *     | |_) | |_| | | | (_| |  __/ |
 *     |____/ \__,_|_|_|\__,_|\___|_|
 *
 *
 */

/** @brief Add json value,
*
* @param[in] js  JSON value.
*
* @return Reference to this object.
*
* @throws std::string exception on error.
*/
gnu::json::Builder& gnu::json::Builder::add(JS* js) {
    auto name = js->name();

    if (_current == nullptr) {
        if (name != "") {
            delete js;
            throw std::string("Error: root object must be nameless <" + name + ">.");
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
                throw std::string("Error: values added to array are nameless <" + name + ">.");
            }
            else if (*js == Type::ARRAY || *js == Type::OBJECT) {
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
                throw std::string("Error: duplicate name <" + name + ">.");
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
            throw std::string("Error: missing container.");
        }
    }

    return *this;
}

/** @brief Encode to json.
*
* @param[in] option  Encode option.
*
* @return JSON string.
*
* @throws std::string exception on error.
*/
std::string gnu::json::Builder::encode(Encode option) const {
    if (_root == nullptr) {
        throw std::string("Error: empty json.");
    }

    auto j = json::encode(*_root, option);

    if (j.find("error") == 0) {
        throw j;
    }

    return j;
}

/** @brief End current node.
*
* @return Reference to this object.
*
* @throws std::string exception on error.
*/
gnu::json::Builder& gnu::json::Builder::end() {
    if (_current == nullptr) {
        throw std::string("Error: empty json.");
    }
    else if (_current == _root) {
        throw std::string("Error: already at the top level.");
    }

    _current = _current->parent();

    return *this;
}

/** @brief Make an array.
*
* @param[in] name    Name of value.
* @param[in] escape  True to escape name.
*
* @return New value.
*/
gnu::json::JS* gnu::json::Builder::MakeArray(const char* name, bool escape) {
    auto r   = new JS((escape == true) ? json::escape(name).c_str() : name);
    r->_type = Type::ARRAY;
    r->_va   = new JSArray();

    return r;
}

/** @brief Make an array inline (without newlines).
*
* @param[in] name    Name of value.
* @param[in] escape  True to escape name.
*
* @return New value.
*/
gnu::json::JS* gnu::json::Builder::MakeArrayInline(const char* name, bool escape) {
    auto r   = new JS((escape == true) ? json::escape(name).c_str() : name);
    r->_type = Type::ARRAY;
    r->_va   = new JSArray();
    r->_inl  = true;

    return r;
}

/** @brief Make an bool.
*
* @param[in] vb      Bool value.
* @param[in] name    Name of value.
* @param[in] escape  True to escape name.
*
* @return New value.
*/
gnu::json::JS* gnu::json::Builder::MakeBool(bool vb, const char* name, bool escape) {
    auto r   = new JS((escape == true) ? json::escape(name).c_str() : name);
    r->_type = Type::BOOL;
    r->_vb   = vb;

    return r;
}

/** @brief Make an null value.
*
* @param[in] name    Name of value.
* @param[in] escape  True to escape name.
*
* @return New value.
*/
gnu::json::JS* gnu::json::Builder::MakeNull(const char* name, bool escape) {
    auto r   = new JS((escape == true) ? json::escape(name).c_str() : name);
    r->_type = Type::NIL;

    return r;
}

/** @brief Make an number value.
*
* @param[in] vn      Number value.
* @param[in] name    Name of value.
* @param[in] escape  True to escape name.
*
* @return New value.
*/
gnu::json::JS* gnu::json::Builder::MakeNumber(double vn, const char* name, bool escape) {
    auto r   = new JS((escape == true) ? json::escape(name).c_str() : name);
    r->_type = Type::NUMBER;
    r->_vn   = vn;

    return r;
}

/** @brief Make a object value.
*
* @param[in] name    Name of value.
* @param[in] escape  True to escape name.
*
* @return New value.
*/
gnu::json::JS* gnu::json::Builder::MakeObject(const char* name, bool escape) {
    auto r   = new JS((escape == true) ? json::escape(name).c_str() : name);
    r->_type = Type::OBJECT;
    r->_vo   = new JSObject();

    return r;
}

/** @brief Make a object inline (no newline).
*
* @param[in] name    Name of value.
* @param[in] escape  True to escape name.
*
* @return New value.
*/
gnu::json::JS* gnu::json::Builder::MakeObjectInline(const char* name, bool escape) {
    auto r   = new JS((escape == true) ? json::escape(name).c_str() : name);
    r->_type = Type::OBJECT;
    r->_vo   = new JSObject();
    r->_inl  = true;

    return r;
}

/** @brief Make a string value.
*
* @param[in] vs      String value.
* @param[in] name    Name of value.
* @param[in] escape  True to escape name.
*
* @return New value.
*/
gnu::json::JS* gnu::json::Builder::MakeString(const char* vs, const char* name, bool escape) {
    auto r   = new JS((escape == true) ? json::escape(name).c_str() : name);
    r->_type = Type::STRING;
    r->_vs   = strdup((escape == true) ? json::escape(vs).c_str() : vs);

    return r;
}

/** @brief Make a string value.
*
* @param[in] vs      String value.
* @param[in] name    Name of value.
* @param[in] escape  True to escape name.
*
* @return New value.
*/
gnu::json::JS* gnu::json::Builder::MakeString(const std::string& vs, const char* name, bool escape) {
    return Builder::MakeString(vs.c_str(), name, escape);
}

// MKALGAM_OFF
