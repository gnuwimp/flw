// Copyright gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#ifndef FLW_JSON_H
#define FLW_JSON_H

// MKALGAM_ON

#include <string>
#include <vector>

namespace flw {
namespace json {
    //----------------------------------------------------------------------
    // Very simple json parser
    // Don't use it for arbitrary data download from internet

    // Macros for creating json
    #define FLW_JSON_START(VEC, X)                      { json::NodeVector& V = VEC; int D = 0; char B[50]; X; (void) B; size_t I = 0; for (auto& n : V) n.index = I++; }
    #define FLW_JSON_START_OBJECT(X)                    { V.push_back(json::Node(json::OBJECT, "", "", D++)); X; V.push_back(json::Node(json::END_OBJECT, "", "", --D)); }
    #define FLW_JSON_START_ARRAY(X)                     { V.push_back(json::Node(json::ARRAY, "", "", D++)); X; V.push_back(json::Node(json::END_ARRAY, "", "", --D)); }
    #define FLW_JSON_START_ARRAY_NL(X)                  { V.push_back(json::Node(json::ARRAY_NL, "", "", D++)); X; V.push_back(json::Node(json::END_ARRAY, "", "", --D)); }

    #define FLW_JSON_ADD_OBJECT(NAME,X)                 { std::string N = json::escape_string(NAME); V.push_back(json::Node(json::OBJECT, N, "", D++)); X; V.push_back(json::Node(json::END_OBJECT, N, "", --D)); }
    #define FLW_JSON_ADD_ARRAY(NAME,X)                  { std::string N = json::escape_string(NAME); V.push_back(json::Node(json::ARRAY, N, "", D++)); X; V.push_back(json::Node(json::END_ARRAY, N, "", --D)); }
    #define FLW_JSON_ADD_ARRAY_NL(NAME,X)               { std::string N = json::escape_string(NAME); V.push_back(json::Node(json::ARRAY_NL, N, "", D++)); X; V.push_back(json::Node(json::END_ARRAY, N, "", --D)); }

    #define FLW_JSON_ADD_STRING(NAME,VALUE)             { V.push_back(json::Node(json::STRING, json::escape_string(NAME), json::escape_string(VALUE), D)); }
    #define FLW_JSON_ADD_NUMBER(NAME,VALUE)             { snprintf(B, 50, "%f", (double) VALUE); V.push_back(json::Node(json::NUMBER, json::escape_string(NAME), B, D)); }
    #define FLW_JSON_ADD_NUMBER2(NAME,VALUE)            { snprintf(B, 50, "%.2f", (double) VALUE); V.push_back(json::Node(json::NUMBER, json::escape_string(NAME), B, D)); }
    #define FLW_JSON_ADD_INT(NAME,VALUE)                { snprintf(B, 50, "%lld", (long long int) VALUE); V.push_back(json::Node(json::NUMBER, json::escape_string(NAME), B, D)); }
    #define FLW_JSON_ADD_UINT(NAME,VALUE)               { snprintf(B, 50, "%llu", (long long unsigned) VALUE); V.push_back(json::Node(json::NUMBER, json::escape_string(NAME), B, D)); }
    #define FLW_JSON_ADD_BOOL(NAME,VALUE)               { V.push_back(json::Node(json::BOOL, json::escape_string(NAME), VALUE == true ? "true" : "false", D)); }
    #define FLW_JSON_ADD_NIL(NAME)                      { V.push_back(json::Node(json::NIL, json::escape_string(NAME), "", D)); }

    #define FLW_JSON_ADD_STRING_TO_ARRAY(VALUE)         { V.push_back(json::Node(json::STRING, "", json::escape_string(VALUE), D)); }
    #define FLW_JSON_ADD_NUMBER_TO_ARRAY(VALUE)         { snprintf(B, 50, "%f", (double) VALUE); V.push_back(json::Node(json::NUMBER, "", B, D)); }
    #define FLW_JSON_ADD_NUMBER2_TO_ARRAY(VALUE)        { snprintf(B, 50, "%.2f", (double) VALUE); V.push_back(json::Node(json::NUMBER, "", B, D)); }
    #define FLW_JSON_ADD_INT_TO_ARRAY(VALUE)            { snprintf(B, 50, "%lld", (long long int) VALUE); V.push_back(json::Node(json::NUMBER, "", B, D)); }
    #define FLW_JSON_ADD_UINT_TO_ARRAY(VALUE)           { snprintf(B, 50, "%llu", (long long unsigned) VALUE); V.push_back(json::Node(json::NUMBER, "", B, D)); }
    #define FLW_JSON_ADD_BOOL_TO_ARRAY(VALUE)           { V.push_back(json::Node(json::BOOL, "", VALUE == true ? "true" : "false", D)); }
    #define FLW_JSON_ADD_NIL_TO_ARRAY()                 { V.push_back(json::Node(json::NIL, "", "", D)); }

    struct Err {
        ssize_t                         pos;
        ssize_t                         line;
                                        Err()
                                            { pos = -1; line = -1; }
                                        Err(ssize_t pos, ssize_t line)
                                            { this->pos = pos; this->line = line; }
    };

    enum TYPE {
                                        NA,
                                        OBJECT,
                                        END_OBJECT,
                                        ARRAY,
                                        END_ARRAY,
                                        STRING,
                                        NUMBER,
                                        BOOL,
                                        NIL,

                                        COLON, // Only for tokenizer
                                        COMMA, // Only for tokenizer
                                        ARRAY_NL, // Only for creating json
    };

    std::string                         escape_string(const std::string& string);
    std::string                         unescape_string(const std::string& string);

    struct Node {
        TYPE                            type;
        int                             depth;
        size_t                          index;
        std::string                     value;
        std::string                     name;
        size_t                          textpos;

                                        Node(TYPE type = TYPE::NA, const std::string& name = "", const std::string& value = "", int depth = 0, size_t textpos = 0)
                                            { this->type = type; this->name = name; this->value = value; this->depth = depth; this->textpos = textpos; }
        bool                            operator==(const Node& other) const
                                            { return (type == other.type || (type == TYPE::ARRAY && other.type == TYPE::ARRAY_NL) || (type == TYPE::ARRAY_NL && other.type == TYPE::ARRAY)) && depth == other.depth && value == other.value && name == other.name; }
        bool                            is_array() const
                                            { return type == json::ARRAY; }
        bool                            is_bool() const
                                            { return type == json::BOOL; }
        bool                            is_data() const
                                            { return type == json::STRING || type == json::NUMBER || type == json::BOOL || type == json::NIL; }
        bool                            is_end() const
                                            { return type == json::END_ARRAY || type == json::END_OBJECT; }
        bool                            is_nil() const
                                            { return type == json::NIL; }
        bool                            is_number() const
                                            { return type == json::NUMBER; }
        bool                            is_object() const
                                            { return type == json::OBJECT; }
        bool                            is_start() const
                                            { return type == json::ARRAY || type == json::ARRAY_NL || type == json::OBJECT; }
        bool                            is_string() const
                                            { return type == json::STRING; }
        void                            print() const;
        bool                            tobool() const
                                            { return value == "true"; }
        long long                       toint() const
                                            { return (type == json::NUMBER) ? strtoll(value.c_str(), nullptr, 0) : 0; }
        double                          tonumber() const
                                            { return (type == json::NUMBER) ? strtod(value.c_str(), nullptr) : 0.0; }
        std::string                     tostring() const
                                            { return unescape_string(value); }
        std::string                     unescape_name() const
                                            { return unescape_string(name); }
    };

    typedef std::vector<Node>           NodeVector;
    typedef std::vector<size_t>         SizeTVector;

    NodeVector                          find_children(const NodeVector& nodes, const Node& start, bool grandchildren = false);
    NodeVector                          find_nodes(const NodeVector& nodes, std::string name, TYPE type = TYPE::NA);
    NodeVector                          find_siblings(const NodeVector& nodes, const Node& start);
    Err                                 parse(const char* json, NodeVector& nodes, bool ignore_trailing_comma = false);
    Err                                 parse(std::string json, NodeVector& nodes, bool ignore_trailing_comma = false);
    void                                print(const NodeVector& nodes);
    std::string                         tostring(const NodeVector& nodes);
    Err                                 validate(const char* json);
    Err                                 validate(std::string json);
} // json
} // gw2

// MKALGAM_OFF

#endif
