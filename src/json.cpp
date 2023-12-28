// Copyright gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#include "json.h"

// MKALGAM_ON

#include <string.h>

namespace flw {
namespace json {
    #define _FLW_JSON_RETURN(X,Y)           return Err((ssize_t) (X - Y), line);
    #define _FLW_JSON_RETURN_POS(X)         return Err(X.pos, X.line);
    #define _FLW_JSON_CHECK_SEPARATOR(X)    if (X > 32 && X != ',' && X != ':' && X != '}' && X != ']' && X != '{' && X != '[') return Err((ssize_t) (text - json), line);

    static const char* _TYPE_NAMES[] = { "NA", "OBJECT", "END_OBJECT", "ARRAY","END_ARRAY", "STRING", "NUMBER", "BOOL", "NIL", "COLON", "COMMA", "ARRAY_NL" };

    //--------------------------------------------------------------------------
    static bool _convert_num(const char* s, double& d) {
        errno = 0;
        d = strtod(s, nullptr);
        return errno != ERANGE;
    }

    //--------------------------------------------------------------------------
    static std::string _remove_last(std::vector<std::string>& v) {
        if (v.size() > 0) {
            auto res = v.back();
            v.pop_back();
            return res;
        }

        return "";
    }

    // ----------------------------------------------------------------------
    // ----------------------------------------------------------------------
    // ----------------------------------------------------------------------

    struct _Token {
        TYPE                            type;
        int                             depth;
        std::string                     value;
        size_t                          pos;
        size_t                          line;
                                        _Token()
                                            { type = json::NA; pos = 0; depth = 0; line = 0; }
                                        _Token(TYPE type, const std::string& value = "", size_t pos = 0, size_t depth = 0, size_t line = 0)
                                            { this->type = type; this->value = value; this->pos = pos; this->depth = depth; this->line = line; }
        bool                            is_end() const
                                            { return type == json::END_ARRAY || type == json::END_OBJECT; }
        bool                            is_start() const
                                            { return type == json::ARRAY || type == json::OBJECT; }
    };

    typedef std::vector<_Token>         _TokenVector;

    //--------------------------------------------------------------------------
    static void _tokenize_count_dot_minus_plus_e(const std::string& s, int& dot, int& minus, int& plus, int& E) {
        dot = 0; minus = 0; plus = 0; E = 0;
        for (auto c : s) {
            dot   += (c =='.');
            minus += (c =='-');
            plus  += (c =='+');
            E     += (c =='e');
            E     += (c =='E');
        }
    }

    //----------------------------------------------------------------------
    static Err _tokenize(const char* json, size_t len, _TokenVector& tokens) {
        tokens.clear();

        auto text  = json;
        auto end   = text + len;
        auto value = std::string();
        auto num   = std::string();
        auto depth = 0;
        auto line  = 1;
        auto DUMMY = _Token();

        num.reserve(50);
        value.reserve(200);

        while (text < end) {
            auto  c  = (unsigned char) *text;
            // auto& T0 = (tokens.size() > 0) ? tokens.back() : DUMMY;

            if (c == 10) {
                line++;
            }

            if (c < 33) {
                text++;
            }
            else if (c == ',') {
                tokens.push_back(_Token(json::COMMA, "", (size_t) text - (size_t) json, depth, line));
                text++;
            }
            else if (c == ':') {
                tokens.push_back(_Token(json::COLON, "", (size_t) text - (size_t) json, depth, line));
                text++;
            }
            else if (c == '{') {
                tokens.push_back(_Token(json::OBJECT, "", (size_t) text - (size_t) json, depth, line));
                depth++;
                text++;
            }
            else if (c == '}') {
                depth--;
                tokens.push_back(_Token(json::END_OBJECT, "", (size_t) text - (size_t) json, depth, line));
                text++;
            }
            else if (c == '[') {
                tokens.push_back(_Token(json::ARRAY, "", (size_t) text - (size_t) json, depth, line));
                depth++;
                text++;
            }
            else if (c == ']') {
                depth--;
                tokens.push_back(_Token(json::END_ARRAY, "", (size_t) text - (size_t) json, depth, line));
                text++;
            }
            else if (c == 't' && strncmp(text, "true", 4) == 0) {
                tokens.push_back(_Token(json::BOOL, "true", (size_t) text - (size_t) json, depth, line));
                text += 4;
                c = *text;
                _FLW_JSON_CHECK_SEPARATOR(c)
            }
            else if (c == 'f' && strncmp(text, "false", 5) == 0) {
                tokens.push_back(_Token(json::BOOL, "false", (size_t) text - (size_t) json, depth, line));
                text += 5;
                c = *text;
                _FLW_JSON_CHECK_SEPARATOR(c)
            }
            else if (c == 'n' && strncmp(text, "null", 4) == 0) {
                tokens.push_back(_Token(json::NIL, "", (size_t) text - (size_t) json, depth, line));
                text += 4;
                c = *text;
                _FLW_JSON_CHECK_SEPARATOR(c)
            }
            else if (c == '"') {
                auto p = (unsigned char) *text;
                auto start = text;
                text++;
                auto c = (unsigned char) *text;
                value = "";

                while (text < end) {
                    if (c < 32) _FLW_JSON_RETURN(text, json)
                    else if (p == '\\' && c == '"') {
                    }
                    else if (c == '"') {
                        tokens.push_back(_Token(json::STRING, value, (size_t) start - (size_t) json, depth, line));
                        text++;
                        break;
                    }
                    value += c;
                    p = c;
                    text++;
                    c = *text;
                }
                c = *text;
                if (text == end) _FLW_JSON_RETURN(text, json)
                _FLW_JSON_CHECK_SEPARATOR(c)
            }
            else if (c == '-' || c == '.' || (c >= '0' && c <= '9')) {
                auto start = text;
                std::string n1, n2;
                int dot, minus, plus, E;

                while (c == '-' || c == '.' || (c >= '0' && c <= '9')) {
                    n1 += c; text++; c = *text;
                }
                _tokenize_count_dot_minus_plus_e(n1, dot, minus, plus, E);
                if (dot > 1 || minus > 1) _FLW_JSON_RETURN(start, json)
                if (n1[0] == '.' || n1.back() == '.') _FLW_JSON_RETURN(start, json)
                if (n1[0] == '-' && n1[1] == '.') _FLW_JSON_RETURN(start, json)
                if (n1[0] == '-' && n1[1] == '0' && n1.length() > 2 && n1[2] != '.') _FLW_JSON_RETURN(start, json)
                if (n1 == "-") _FLW_JSON_RETURN(start, json)
                if (minus > 0 && n1[0] != '-') _FLW_JSON_RETURN(start, json)

                while (c == 'e' || c == 'E' || c == '-' || c == '+' || (c >= '0' && c <= '9')) {
                    n2 += c; text++; c = *text;
                }
                if (n2.empty() == false) {
                    if (n2.length() == 1) _FLW_JSON_RETURN(start, json)
                    _tokenize_count_dot_minus_plus_e(n2, dot, minus, plus, E);
                    if (plus + minus > 1 || E > 1) _FLW_JSON_RETURN(start, json)
                    if (plus > 0 && n2.back() == '+') _FLW_JSON_RETURN(start, json)
                    if (plus > 0 && n2[1] != '+') _FLW_JSON_RETURN(start, json)
                    if (minus > 0 && n2.back() == '-') _FLW_JSON_RETURN(start, json)
                    if (minus > 0 && n2[1] != '-') _FLW_JSON_RETURN(start, json)
                }
                else if (n1[0] == '0' && n1.length() > 1 && n1[1] != '.') _FLW_JSON_RETURN(start, json)

                tokens.push_back(_Token(json::NUMBER, n1 + n2, (size_t) start - (size_t) json, depth, line));
                _FLW_JSON_CHECK_SEPARATOR(c)
            }
            else {
                _FLW_JSON_RETURN(text, json);
            }
        }

        return Err();
    }

    // ----------------------------------------------------------------------
    // ----------------------------------------------------------------------
    // ----------------------------------------------------------------------

    // ----------------------------------------------------------------------
    void Node::print() const {
        printf("%5u| %5u| %2d| %*s %-10s <%s>", (unsigned) index, (unsigned) textpos, depth, depth * 4, "", _TYPE_NAMES[(int) type], name.c_str());
        if (type == json::STRING || type == json::NUMBER || type == json::BOOL) printf(" => <%s>\n", value.c_str());
        else printf("\n");
        fflush(stdout);
    }

    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------

    //--------------------------------------------------------------------------
    std::string escape_string(const std::string& string) {
        std::string res;

        for (unsigned char c : string) {
            if (c == 9) res += "\\t";
            else if (c == 10) res += "\\n";
            else if (c == 13) res += "\\r";
            else if (c == 8) res += "\\b";
            else if (c == 14) res += "\\f";
            else if (c == 34) res += "\\\"";
            else if (c == 92) res += "\\\\";
            else res += c;
        }

        return res;
    }

    //--------------------------------------------------------------------------
    NodeVector find_children(const NodeVector& nodes, const Node& start, bool grandchildren) {
        auto res = NodeVector();
        if (start.index + 1 >= nodes.size()) return res;

        for (size_t f = start.index + 1; f < nodes.size(); f++) {
            auto& node = nodes[f];

            if (node.depth <= start.depth) break;
            else if (grandchildren == true) res.push_back(node);
            else if (node.depth == start.depth + 1 && node.is_end() == false) res.push_back(node);
        }
        return res;
    }

    //--------------------------------------------------------------------------
    NodeVector find_nodes(const NodeVector& nodes, std::string name, TYPE type) {
        auto res = NodeVector();

        for (size_t f = 0; f < nodes.size(); f++) {
            auto& node = nodes[f];

            if (name == "") {
                if (type == json::NA) res.push_back(node);
                else if (node.type == type) res.push_back(node);
            }
            else if (node.name == name) {
                if (type == json::NA) res.push_back(node);
                else if (node.type == type) res.push_back(node);
            }
        }
        return res;
    }

    //--------------------------------------------------------------------------
    NodeVector find_siblings(const NodeVector& nodes, const Node& start) {
        auto res = NodeVector();
        if (start.index >= nodes.size()) return res;

        for (size_t f = start.index; f < nodes.size(); f++) {
            auto& node = nodes[f];

            if (node.depth < start.depth) break;
            else if (node.depth == start.depth && node.is_end() == false) res.push_back(node);
        }
        return res;
    }

    //--------------------------------------------------------------------------
    Err parse(const char* json, NodeVector& nodes, bool ignore_trailing_comma) {
        auto tokens      = _TokenVector();
        auto len         = strlen(json);
        auto err         = _tokenize(json, len, tokens);
        auto size        = tokens.size();
        auto obj         = 0;
        auto arr         = 0;
        auto num         = 0.0;
        auto arr_name    = std::vector<std::string>();
        auto obj_name    = std::vector<std::string>();
        auto containers  = std::vector<std::string>();
        auto const DUMMY = _Token();

        nodes.clear();

        if (err.pos >= 0) return err;
        else if (size < 2) return Err(size, 1);

        for (size_t f = 0; f < size; f++) {
            auto& t  = tokens[f];
            auto& T0 = (f > 0) ? tokens[f - 1] : DUMMY;
            auto& T1 = (f < size - 1) ? tokens[f + 1] : DUMMY;
            auto& T2 = (f < size - 2) ? tokens[f + 2] : DUMMY;

            if ((T0.type == json::STRING || T0.type == json::NUMBER || T0.type == json::NIL || T0.type == json::BOOL) &&
                (t.type != json::COMMA && t.type != json::END_ARRAY && t.type != json::END_OBJECT)) {
                _FLW_JSON_RETURN_POS(t)
            }
            else if (obj == 0 && arr == 0 && nodes.size() > 0) {
                _FLW_JSON_RETURN_POS(t)
            }
            else if (obj == 0 && arr == 0 && t.type != json::OBJECT && t.type != json::ARRAY) {
                _FLW_JSON_RETURN_POS(t)
            }
            else if (t.type == json::COMMA && T0.type == json::COMMA) {
                _FLW_JSON_RETURN_POS(t)
            }
            else if (t.type == json::COMMA && T0.type == json::ARRAY) {
                _FLW_JSON_RETURN_POS(t)
            }
            else if (t.type == json::COLON && T0.type != json::STRING) {
                _FLW_JSON_RETURN_POS(t)
            }
            else if (t.type == json::END_OBJECT) {
                if (ignore_trailing_comma == false && T0.type == json::COMMA) _FLW_JSON_RETURN_POS(t)
                containers.pop_back();
                obj--;
                if (obj < 0) _FLW_JSON_RETURN_POS(t)
               nodes.push_back(Node(json::END_OBJECT, _remove_last(obj_name), "", t.depth, t.pos));
            }
            else if (t.type == json::OBJECT) {
                if (containers.size() && containers.back() == "O") _FLW_JSON_RETURN_POS(t)
                if (T0.is_end() == true && T0.depth == t.depth) _FLW_JSON_RETURN_POS(t)
                containers.push_back("O");
                obj++;
                nodes.push_back(Node(json::OBJECT, "", "", t.depth, t.pos));
                obj_name.push_back("");
            }
            else if (t.type == json::STRING && T1.type == json::COLON && T2.type == json::OBJECT) {
                if (T0.is_end() == true && T0.depth == T2.depth) _FLW_JSON_RETURN_POS(t)
                if (obj == 0) _FLW_JSON_RETURN_POS(t)
                containers.push_back("O");
                obj++;
                nodes.push_back(Node(json::OBJECT, t.value, "", T2.depth, t.pos));
                obj_name.push_back(t.value);
                f += 2;
            }
            else if (t.type == json::END_ARRAY) {
                if (ignore_trailing_comma == false && T0.type == json::COMMA) _FLW_JSON_RETURN_POS(t)
                containers.pop_back();
                arr--;
                if (arr < 0) _FLW_JSON_RETURN_POS(t)
                nodes.push_back(Node(json::END_ARRAY, _remove_last(arr_name), "", t.depth, t.pos));
            }
            else if (t.type == json::ARRAY) {
                if (containers.size() && containers.back() == "O") _FLW_JSON_RETURN_POS(t)
                if (T0.is_end() == true && T0.depth == t.depth) _FLW_JSON_RETURN_POS(t)
                containers.push_back("A");
                arr++;
                nodes.push_back(Node(json::ARRAY, "", "", t.depth, t.pos));
                arr_name.push_back("");
            }
            else if (t.type == json::STRING && T1.type == json::COLON && T2.type == json::ARRAY) {
                if (T0.is_end() == true && T0.depth == T2.depth) _FLW_JSON_RETURN_POS(t)
                if (obj == 0) _FLW_JSON_RETURN_POS(t)
                containers.push_back("A");
                arr++;
                nodes.push_back(Node(json::ARRAY, t.value, "", T2.depth, t.pos));
                arr_name.push_back(t.value);
                f += 2;
            }
            else if (t.type == json::BOOL) {
                nodes.push_back(Node(json::BOOL, "", t.value, t.depth, t.pos));
                if (containers.size() && containers.back() == "O") _FLW_JSON_RETURN_POS(t)
            }
            else if (t.type == json::STRING && T1.type == json::COLON && T2.type == json::BOOL) {
                nodes.push_back(Node(json::BOOL, t.value, T2.value, T2.depth, t.pos));
                f += 2;
                if (containers.size() && containers.back() == "A") _FLW_JSON_RETURN_POS(t)
            }
            else if (t.type == json::NIL) {
                nodes.push_back(Node(json::NIL, "", "", t.depth, t.pos));
                if (containers.size() && containers.back() == "O") _FLW_JSON_RETURN_POS(t)
            }
            else if (t.type == json::STRING && T1.type == json::COLON && T2.type == json::NIL) {
                nodes.push_back(Node(json::NIL, t.value, "", T2.depth, t.pos));
                f += 2;
                if (containers.size() && containers.back() == "A") _FLW_JSON_RETURN_POS(t)
            }
            else if (t.type == json::NUMBER) {
                if (_convert_num(t.value.c_str(), num) == false) _FLW_JSON_RETURN_POS(t)
                nodes.push_back(Node(json::NUMBER, "", t.value, t.depth, t.pos));
                if (containers.size() && containers.back() == "O") _FLW_JSON_RETURN_POS(t)
            }
            else if (t.type == json::STRING && T1.type == json::COLON && T2.type == json::NUMBER) {
                if (_convert_num(T2.value.c_str(), num) == false) _FLW_JSON_RETURN_POS(t)
                nodes.push_back(Node(json::NUMBER, t.value, T2.value, t.depth, t.pos));
                f += 2;
                if (containers.size() && containers.back() == "A") _FLW_JSON_RETURN_POS(t)
            }
            else if (t.type == json::STRING && T1.type == json::COLON && T2.type == json::STRING) {
                nodes.push_back(Node(json::STRING, t.value, T2.value, t.depth, t.pos));
                f += 2;
                if (containers.size() && containers.back() == "A") _FLW_JSON_RETURN_POS(t)
            }
            else if (t.type == json::STRING) {
                nodes.push_back(Node(json::STRING, "", t.value, t.depth, t.pos));
                if (containers.size() && containers.back() == "O") _FLW_JSON_RETURN_POS(t)
            }

            if (nodes.size() > 0) nodes.back().index   = nodes.size() - 1;
            if (arr < 0 || obj < 0) _FLW_JSON_RETURN_POS(t)
        }

        if (arr != 0 || obj != 0) return Err(len, -1);
        return Err();
    }

    //--------------------------------------------------------------------------
    Err parse(std::string json, NodeVector& nodes, bool ignore_trailing_commas) {
        return parse(json.c_str(), nodes, ignore_trailing_commas);
    }

    // ----------------------------------------------------------------------
    void print(const NodeVector& nodes) {
        auto c = 0;

        printf("\n%d Node objects\n", (int) nodes.size());
        printf("%6s| %6s| %6s| %6s| %s\n", "COUNT", "INDEX", "POS", "DEPTH", "VALUES");
        printf("-------------------------------------------------------\n");
        for (auto& node : nodes) {
            printf("%6d| %6u| %6u| %6d|%*s  %-10s <%s>", c, (unsigned) node.index, (unsigned) node.textpos, node.depth, node.depth * 4, "", _TYPE_NAMES[(int) node.type], node.name.c_str());
            if (node.type == json::STRING || node.type == json::NUMBER || node.type == json::BOOL) printf(" => <%s>", node.value.c_str());
            printf("\n");
            c++;
        }
        fflush(stdout);
    }

    //--------------------------------------------------------------------------
    std::string tostring(const NodeVector& nodes) {
        if (nodes.size() < 2) return "";

        auto res    = std::string();
        auto DUMMY  = Node();
        auto last   = nodes.size() - 1;
        auto indent = std::string();
        auto arr    = json::NA;

        res.reserve(20 * nodes.size());

        for (size_t f = 0; f <= last; f++) {
            auto& node = nodes[f];
            auto& next = (f < last) ? nodes[f + 1] : DUMMY;
            auto  nl   = std::string("\n");
            auto  nl2  = std::string("");

            if (node.is_data() && next.is_end() == false) {
                nl  = ",\n";
                nl2 = ",";
            }
            else if (node.is_end() && (next.is_start() == true || next.is_data() == true)) {
                nl  = ",\n";
                nl2 = ",";
            }

            if (node.type == json::END_OBJECT) {
                indent.pop_back();
                res += indent;
                res += "}";
            }
            else if (node.type == json::END_ARRAY) {
                indent.pop_back();
                if (arr != json::ARRAY) res += indent;
                res += "]";
                arr = json::NA;
            }
            else {
                if (arr != json::ARRAY) res += indent;

                if (node.type == json::OBJECT) {
                    indent += "\t";
                    if (node.name == "") res += "{";
                    else res += "\"" + node.name + "\": {";
                }
                else if (node.type == json::ARRAY || node.type == json::ARRAY_NL) {
                    indent += "\t";
                    if (node.name == "") res += "[";
                    else res += "\"" + node.name + "\": [";
                    arr = (node.type == json::ARRAY_NL) ? json::ARRAY : json::NA;
                }
                else if (node.type == json::STRING) {
                    if (node.name == "") res += "\"" + node.value + "\"";
                    else res += "\"" + node.name + "\": \"" + node.value + "\"";
                }
                else if (node.type == json::NUMBER) {
                    if (node.name == "") res += node.value;
                    else res += "\"" + node.name + "\": " + node.value;
                }
                else if (node.type == json::BOOL) {
                    if (node.name == "") res += node.value;
                    else res += "\"" + node.name + "\": " + node.value;
                }
                else if (node.type == json::NIL) {
                    if (node.name == "") res += "null";
                    else res += "\"" + node.name + "\": null";
                }
            }

            if (arr != json::ARRAY) res += nl;
            else res += nl2;
        }

        return res;
    }

    //--------------------------------------------------------------------------
    std::string unescape_string(const std::string& string) {
        std::string res;

        for (size_t f = 0; f < string.length(); f++) {
            unsigned char c = string[f];
            unsigned char n = string[f + 1];

            if (c == '\\') {
                if (n == 't') res += '\t';
                else if (n == 'n') res += '\n';
                else if (n == 'r') res += '\r';
                else if (n == 'b') res += '\b';
                else if (n == 'f') res += '\f';
                else if (n == '\"') res += '"';
                else if (n == '\\') res += '\\';
                f++;
            }
            else res += c;
        }

        return res;
    }

    //--------------------------------------------------------------------------
     Err validate(const char* json) {
        auto nodes = NodeVector();
        return parse(json, nodes);
    }

    //--------------------------------------------------------------------------
     Err validate(std::string json) {
        auto nodes = NodeVector();
        return parse(json.c_str(), nodes);
    }
} // json
} // flw

// MKALGAM_OFF
