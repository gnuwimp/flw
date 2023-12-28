// Copyright gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#ifndef FLW_UTIL_H
#define FLW_UTIL_H

// MKALGAM_ON

#include <assert.h>
#include <string>
#include <vector>
#include <iostream>
#include <FL/Fl_Menu_.H>

#define _FLW_TOSTRING(X)            #X
#define FLW_TOSTRING(X)             _FLW_TOSTRING(X)
#define FLW_LINE                    { fprintf(stderr, "%5d: %s - %s\n", __LINE__, __func__, __FILE__); fflush(stderr); }
#define FLW_RED                     { fprintf(stderr, "\033[7m\033[31m" "%s: %s: " FLW_TOSTRING(__LINE__) " \033[0m\n", __FILE__, __func__); fflush(stderr); }
#define FLW_GREEN                   { fprintf(stderr, "\033[7m\033[32m" "%s: %s: " FLW_TOSTRING(__LINE__) " \033[0m\n", __FILE__, __func__); fflush(stderr); }
#define FLW_YELLOW                  { fprintf(stderr, "\033[7m\033[33m" "%s: %s: " FLW_TOSTRING(__LINE__) " \033[0m\n", __FILE__, __func__); fflush(stderr); }
#define FLW_BLUE                    { fprintf(stderr, "\033[7m\033[34m" "%s: %s: " FLW_TOSTRING(__LINE__) " \033[0m\n", __FILE__, __func__); fflush(stderr); }
#define FLW_MAGENTA                 { fprintf(stderr, "\033[7m\033[35m" "%s: %s: " FLW_TOSTRING(__LINE__) " \033[0m\n", __FILE__, __func__); fflush(stderr); }
#define FLW_CYAN                    { fprintf(stderr, "\033[7m\033[36m" "%s: %s: " FLW_TOSTRING(__LINE__) " \033[0m\n", __FILE__, __func__); fflush(stderr); }
#define FLW_WHITE                   { fprintf(stderr, "\033[7m\033[37m" "%s: %s: " FLW_TOSTRING(__LINE__) " \033[0m\n", __FILE__, __func__); fflush(stderr); }
#define FLW_PRINT(...)              FLW_PRINT_MACRO(__VA_ARGS__, FLW_PRINT7, FLW_PRINT6, FLW_PRINT5, FLW_PRINT4, FLW_PRINT3, FLW_PRINT2, FLW_PRINT1)(__VA_ARGS__);
#define FLW_PRINT1(A)               { std::cerr << __FILE__ << "::" << __func__ << "::" << __LINE__ << " (" FLW_TOSTRING(A) ")=" << (A) << std::endl; fflush(stderr); }
#define FLW_PRINT2(A,B)             { std::cerr << __FILE__ << "::" << __func__ << "::" << __LINE__ << " (" FLW_TOSTRING(A) ")=" << (A) << ", (" FLW_TOSTRING(B) ")=" << (B) << std::endl; fflush(stderr); }
#define FLW_PRINT3(A,B,C)           { std::cerr << __FILE__ << "::" << __func__ << "::" << __LINE__ << " (" FLW_TOSTRING(A) ")=" << (A) << ", (" FLW_TOSTRING(B) ")=" << (B) << ", (" FLW_TOSTRING(C) ")=" << (C) << std::endl; fflush(stderr); }
#define FLW_PRINT4(A,B,C,D)         { std::cerr << __FILE__ << "::" << __func__ << "::" << __LINE__ << " (" FLW_TOSTRING(A) ")=" << (A) << ", (" FLW_TOSTRING(B) ")=" << (B) << ", (" FLW_TOSTRING(C) ")=" << (C) << ", (" FLW_TOSTRING(D) ")=" << (D) << std::endl; fflush(stderr); }
#define FLW_PRINT5(A,B,C,D,E)       { std::cerr << __FILE__ << "::" << __func__ << "::" << __LINE__ << " (" FLW_TOSTRING(A) ")=" << (A) << ", (" FLW_TOSTRING(B) ")=" << (B) << ", (" FLW_TOSTRING(C) ")=" << (C) << ", (" FLW_TOSTRING(D) ")=" << (D) << ", (" FLW_TOSTRING(E) ")=" << (E) << std::endl; fflush(stderr); }
#define FLW_PRINT6(A,B,C,D,E,F)     { std::cerr << __FILE__ << "::" << __func__ << "::" << __LINE__ << " (" FLW_TOSTRING(A) ")=" << (A) << ", (" FLW_TOSTRING(B) ")=" << (B) << ", (" FLW_TOSTRING(C) ")=" << (C) << ", (" FLW_TOSTRING(D) ")=" << (D) << ", (" FLW_TOSTRING(E) ")=" << (E) << ", (" FLW_TOSTRING(F) ")=" << (F) << std::endl; fflush(stderr); }
#define FLW_PRINT7(A,B,C,D,E,F,G)   { std::cerr << __FILE__ << "::" << __func__ << "::" << __LINE__ << " (" FLW_TOSTRING(A) ")=" << (A) << ", (" FLW_TOSTRING(B) ")=" << (B) << ", (" FLW_TOSTRING(C) ")=" << (C) << ", (" FLW_TOSTRING(D) ")=" << (D) << ", (" FLW_TOSTRING(E) ")=" << (E) << ", (" FLW_TOSTRING(F) ")=" << (F) << ", (" FLW_TOSTRING(G) ")=" << (G) << std::endl; fflush(stderr); }
#define FLW_PRINT_MACRO(A,B,C,D,E,F,G,N,...) N

namespace flw {
    extern int                          PREF_FIXED_FONT;
    extern std::string                  PREF_FIXED_FONTNAME;
    extern int                          PREF_FIXED_FONTSIZE;
    extern int                          PREF_FONT;
    extern int                          PREF_FONTSIZE;
    extern std::string                  PREF_FONTNAME;
    typedef std::vector<std::string>    StringVector;

    //--------------------------------------------------------------------------
    struct Buf {
        char*                           p;
        size_t                          s;

                                        Buf();
                                        Buf(size_t s);
                                        Buf(char* p, size_t s);
                                        Buf(const char* p, size_t s);
                                        Buf(const Buf& b);
                                        Buf(Buf&& b);
        Buf&                            operator=(const Buf& b);
        Buf&                            operator=(Buf&& b);
        Buf&                            operator+=(const Buf& b);
        bool                            operator==(const Buf& other) const;
        virtual                         ~Buf();
    };

    namespace util {
        //----------------------------------------------------------------------
        // Collection of utility functions
        //
        size_t                          add_string(StringVector& in, size_t max_size, std::string string);
        char*                           allocate(size_t size, bool terminate = true);
        void                            center_window(Fl_Window* window, Fl_Window* parent = nullptr);
        int                             count_decimals(double number);
        std::string                     fix_menu_string(std::string in);
        std::string                     format(const char* format, ...);
        std::string                     format_double(double num, int decimals, char del = ' ');
        std::string                     format_int(int64_t num, char del = ' ');
        size_t                          insert_string(StringVector& in, size_t max_size, std::string string);
        void                            labelfont(Fl_Widget* widget);
        Buf                             load_file(std::string filename, bool alert = true);
        void                            menu_item_enable(Fl_Menu_* menu, const char* text, bool value);
        Fl_Menu_Item*                   menu_item_get(Fl_Menu_* menu, const char* text);
        void                            menu_item_set(Fl_Menu_* menu, const char* text, bool value);
        void                            menu_item_set_only(Fl_Menu_* menu, const char* text);
        bool                            menu_item_value(Fl_Menu_* menu, const char* text);
        void                            png_save(std::string opt_name, Fl_Window* window, int X = 0, int Y = 0, int W = 0, int H = 0);
        void                            print(Fl_Widget* widget, bool tab = false);
        void                            print(Fl_Group* group);
        int                             replace(std::string& string, std::string find, std::string replace);
        bool                            save_file(std::string filename, const void* data, size_t size, bool alert = true);
        StringVector                    split(const std::string& string, std::string split);
        bool                            test(bool val, int line, const char* func);
        bool                            test(const char* ref, const char* val, int line, const char* func);
        bool                            test(int64_t ref, int64_t val, int line, const char* func);
        bool                            test(double ref, double val, double diff, int line, const char* func);
        double                          time();
        int64_t                         time_micro();
        int32_t                         time_milli();
        void                            time_sleep(int milli);
        double                          to_double(std::string, double def = 0.0);
        int                             to_doubles(std::string string, double numbers[], size_t size);
        int64_t                         to_int(std::string string, int64_t def = 0);
        int                             to_ints(std::string string, int64_t numbers[], size_t size);
        char*                           win_to_unix(const char* string);
        void*                           zero_memory(char* string);
        void*                           zero_memory(void* Buf, size_t size);
        void*                           zero_memory(std::string& string);
    }

    //--------------------------------------------------------------------------
    // Mode is 0 for missing, 1 for directory, 2 for file and 3 for other
    //
    struct Stat {
        int64_t                         size;
        int64_t                         mtime;
        int                             mode;

                                        Stat()
                                            { size = mtime = 0; mode = 0; }
                                        Stat(std::string filename);
    };

    namespace json {
        //----------------------------------------------------------------------
        // Very simple json parser
        // Don't use it for arbitrary data download from internet

        // Macros for creating json
        #define FLW_JSON_START(VEC, X)                      { flw::json::NodeVector& V = VEC; int D = 0; char B[50]; X; (void) B; size_t I = 0; for (auto& n : V) n.index = I++; }
        #define FLW_JSON_START_OBJECT(X)                    { V.push_back(flw::json::Node(flw::json::TYPE::OBJECT, "", "", D++)); X; V.push_back(flw::json::Node(flw::json::TYPE::END_OBJECT, "", "", --D)); }
        #define FLW_JSON_START_ARRAY(X)                     { V.push_back(flw::json::Node(flw::json::TYPE::ARRAY, "", "", D++)); X; V.push_back(flw::json::Node(flw::json::TYPE::END_ARRAY, "", "", --D)); }
        #define FLW_JSON_START_ARRAY_NL(X)                  { V.push_back(flw::json::Node(flw::json::TYPE::ARRAY_NL, "", "", D++)); X; V.push_back(flw::json::Node(flw::json::TYPE::END_ARRAY, "", "", --D)); }

        #define FLW_JSON_ADD_OBJECT(NAME,X)                 { std::string N = flw::json::escape_string(NAME); V.push_back(flw::json::Node(flw::json::TYPE::OBJECT, N, "", D++)); X; V.push_back(flw::json::Node(flw::json::TYPE::END_OBJECT, N, "", --D)); }
        #define FLW_JSON_ADD_ARRAY(NAME,X)                  { std::string N = flw::json::escape_string(NAME); V.push_back(flw::json::Node(flw::json::TYPE::ARRAY, N, "", D++)); X; V.push_back(flw::json::Node(flw::json::TYPE::END_ARRAY, N, "", --D)); }
        #define FLW_JSON_ADD_ARRAY_NL(NAME,X)               { std::string N = flw::json::escape_string(NAME); V.push_back(flw::json::Node(flw::json::TYPE::ARRAY_NL, N, "", D++)); X; V.push_back(flw::json::Node(flw::json::TYPE::END_ARRAY, N, "", --D)); }

        #define FLW_JSON_ADD_STRING(NAME,VALUE)             { V.push_back(flw::json::Node(flw::json::TYPE::STRING, flw::json::escape_string(NAME), flw::json::escape_string(VALUE), D)); }
        #define FLW_JSON_ADD_NUMBER(NAME,VALUE)             { snprintf(B, 50, "%f", (double) VALUE); V.push_back(flw::json::Node(flw::json::TYPE::NUMBER, flw::json::escape_string(NAME), B, D)); }
        #define FLW_JSON_ADD_NUMBER2(NAME,VALUE)            { snprintf(B, 50, "%.2f", (double) VALUE); V.push_back(flw::json::Node(flw::json::TYPE::NUMBER, flw::json::escape_string(NAME), B, D)); }
        #define FLW_JSON_ADD_INT(NAME,VALUE)                { snprintf(B, 50, "%lld", (long long int) VALUE); V.push_back(flw::json::Node(flw::json::TYPE::NUMBER, flw::json::escape_string(NAME), B, D)); }
        #define FLW_JSON_ADD_UINT(NAME,VALUE)               { snprintf(B, 50, "%llu", (long long unsigned) VALUE); V.push_back(flw::json::Node(flw::json::TYPE::NUMBER, flw::json::escape_string(NAME), B, D)); }
        #define FLW_JSON_ADD_BOOL(NAME,VALUE)               { V.push_back(flw::json::Node(flw::json::TYPE::BOOL, flw::json::escape_string(NAME), VALUE == true ? "true" : "false", D)); }
        #define FLW_JSON_ADD_NIL(NAME)                      { V.push_back(flw::json::Node(flw::json::TYPE::NIL, flw::json::escape_string(NAME), "", D)); }

        #define FLW_JSON_ADD_STRING_TO_ARRAY(VALUE)         { V.push_back(flw::json::Node(flw::json::TYPE::STRING, "", flw::json::escape_string(VALUE), D)); }
        #define FLW_JSON_ADD_NUMBER_TO_ARRAY(VALUE)         { snprintf(B, 50, "%f", (double) VALUE); V.push_back(flw::json::Node(flw::json::TYPE::NUMBER, "", B, D)); }
        #define FLW_JSON_ADD_NUMBER2_TO_ARRAY(VALUE)        { snprintf(B, 50, "%.2f", (double) VALUE); V.push_back(flw::json::Node(flw::json::TYPE::NUMBER, "", B, D)); }
        #define FLW_JSON_ADD_INT_TO_ARRAY(VALUE)            { snprintf(B, 50, "%lld", (long long int) VALUE); V.push_back(flw::json::Node(flw::json::TYPE::NUMBER, "", B, D)); }
        #define FLW_JSON_ADD_UINT_TO_ARRAY(VALUE)           { snprintf(B, 50, "%llu", (long long unsigned) VALUE); V.push_back(flw::json::Node(flw::json::TYPE::NUMBER, "", B, D)); }
        #define FLW_JSON_ADD_BOOL_TO_ARRAY(VALUE)           { V.push_back(flw::json::Node(flw::json::TYPE::BOOL, "", VALUE == true ? "true" : "false", D)); }
        #define FLW_JSON_ADD_NIL_TO_ARRAY()                 { V.push_back(flw::json::Node(flw::json::TYPE::NIL, "", "", D)); }

        enum class TYPE {
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

        std::string                     escape_string(const std::string& string);
        std::string                     unescape_string(const std::string& string);

        struct Node {
            TYPE                        type;
            int                         depth;
            size_t                      index;
            std::string                 value;
            std::string                 name;
            size_t                      textpos;

                                        Node(TYPE type = TYPE::NA, const std::string& name = "", const std::string& value = "", int depth = 0, size_t textpos = 0)
                                            { this->type = type; this->name = name; this->value = value; this->depth = depth; this->textpos = textpos; }
            bool                        operator==(const Node& other) const
                                            { return (type == other.type || (type == TYPE::ARRAY && other.type == TYPE::ARRAY_NL) || (type == TYPE::ARRAY_NL && other.type == TYPE::ARRAY)) && depth == other.depth && value == other.value && name == other.name; }
            bool                        is_array() const
                                            { return type == json::TYPE::ARRAY; }
            bool                        is_bool() const
                                            { return type == json::TYPE::BOOL; }
            bool                        is_data() const
                                            { return type == json::TYPE::STRING || type == json::TYPE::NUMBER || type == json::TYPE::BOOL || type == json::TYPE::NIL; }
            bool                        is_end() const
                                            { return type == json::TYPE::END_ARRAY || type == json::TYPE::END_OBJECT; }
            bool                        is_nil() const
                                            { return type == json::TYPE::NIL; }
            bool                        is_number() const
                                            { return type == json::TYPE::NUMBER; }
            bool                        is_object() const
                                            { return type == json::TYPE::OBJECT; }
            bool                        is_start() const
                                            { return type == json::TYPE::ARRAY || type == json::TYPE::ARRAY_NL || type == json::TYPE::OBJECT; }
            bool                        is_string() const
                                            { return type == json::TYPE::STRING; }
            void                        print() const;
            bool                        tobool() const
                                            { return value == "true"; }
            long long                   toint() const
                                            { return (type == json::TYPE::NUMBER) ? strtoll(value.c_str(), nullptr, 0) : 0; }
            double                      tonumber() const
                                            { return (type == json::TYPE::NUMBER) ? strtod(value.c_str(), nullptr) : 0.0; }
            std::string                 tostring() const
                                            { return unescape_string(value); }
            std::string                 unescape_name() const
                                            { return unescape_string(name); }
        };

        typedef std::vector<Node>       NodeVector;
        typedef std::vector<size_t>     SizeTVector;

        NodeVector                      find_children(const NodeVector& nodes, const Node& start, bool grandchildren = false);
        NodeVector                      find_nodes(const NodeVector& nodes, std::string name, TYPE type = TYPE::NA);
        NodeVector                      find_siblings(const NodeVector& nodes, const Node& start);
        size_t                          parse(const char* json, NodeVector& nodes, bool ignore_trailing_comma = false);
        size_t                          parse(std::string json, NodeVector& nodes, bool ignore_trailing_comma = false);
        void                            print(const NodeVector& nodes);
        std::string                     tostring(const NodeVector& nodes);
        size_t                          validate(const char* json);
        size_t                          validate(std::string json);
    }
}

// MKALGAM_OFF

#endif
