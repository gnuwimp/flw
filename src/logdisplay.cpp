// Copyright gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#include "logdisplay.h"
#include "dlg.h"
#include "json.h"
#include "util.h"
#include "theme.h"

// MKALGAM_ON

#include <assert.h>
#include <FL/fl_ask.H>
#include <FL/Fl_File_Chooser.H>

namespace flw {
    namespace logdisplay {
        #define STYLE_ATTRIBUTE 0, 0
        #define STYLE_ATTRIBUTE2 Fl_Text_Display::ATTR_BGCOLOR, 0

        Fl_Color BG_COLOR      = fl_lighter(FL_GRAY);
        Fl_Color BG_DARK_COLOR = FL_WHITE;
        size_t   MAX_LINE_LEN  = 1000;
        size_t   TURN_WRAP_ON  =  300;

        Fl_Text_Display::Style_Table_Entry STYLE_TABLE[] = {
            { FL_FOREGROUND_COLOR,  FL_COURIER,         14, STYLE_ATTRIBUTE },
            { FL_GRAY,              FL_COURIER,         14, STYLE_ATTRIBUTE },
            { FL_RED,               FL_COURIER,         14, STYLE_ATTRIBUTE },
            { FL_DARK_GREEN,        FL_COURIER,         14, STYLE_ATTRIBUTE },
            { FL_BLUE,              FL_COURIER,         14, STYLE_ATTRIBUTE },
            { FL_MAGENTA,           FL_COURIER,         14, STYLE_ATTRIBUTE },
            { FL_DARK_YELLOW,       FL_COURIER,         14, STYLE_ATTRIBUTE },
            { FL_CYAN,              FL_COURIER,         14, STYLE_ATTRIBUTE },

            { FL_FOREGROUND_COLOR,  FL_COURIER_BOLD,    14, STYLE_ATTRIBUTE },
            { FL_GRAY,              FL_COURIER_BOLD,    14, STYLE_ATTRIBUTE },
            { FL_RED,               FL_COURIER_BOLD,    14, STYLE_ATTRIBUTE },
            { FL_DARK_GREEN,        FL_COURIER_BOLD,    14, STYLE_ATTRIBUTE },
            { FL_BLUE,              FL_COURIER_BOLD,    14, STYLE_ATTRIBUTE },
            { FL_MAGENTA,           FL_COURIER_BOLD,    14, STYLE_ATTRIBUTE },
            { FL_DARK_YELLOW,       FL_COURIER_BOLD,    14, STYLE_ATTRIBUTE },
            { FL_CYAN,              FL_COURIER_BOLD,    14, STYLE_ATTRIBUTE },

            { FL_FOREGROUND_COLOR,  FL_COURIER,         14, STYLE_ATTRIBUTE2 },
            { FL_GRAY,              FL_COURIER,         14, STYLE_ATTRIBUTE2 },
            { FL_RED,               FL_COURIER,         14, STYLE_ATTRIBUTE2 },
            { FL_DARK_GREEN,        FL_COURIER,         14, STYLE_ATTRIBUTE2 },
            { FL_BLUE,              FL_COURIER,         14, STYLE_ATTRIBUTE2 },
            { FL_MAGENTA,           FL_COURIER,         14, STYLE_ATTRIBUTE2 },
            { FL_DARK_YELLOW,       FL_COURIER,         14, STYLE_ATTRIBUTE2 },
            { FL_CYAN,              FL_COURIER,         14, STYLE_ATTRIBUTE2 },

            { FL_FOREGROUND_COLOR,  FL_COURIER_BOLD,    14, STYLE_ATTRIBUTE2 },
            { FL_GRAY,              FL_COURIER_BOLD,    14, STYLE_ATTRIBUTE2 },
            { FL_RED,               FL_COURIER_BOLD,    14, STYLE_ATTRIBUTE2 },
            { FL_DARK_GREEN,        FL_COURIER_BOLD,    14, STYLE_ATTRIBUTE2 },
            { FL_BLUE,              FL_COURIER_BOLD,    14, STYLE_ATTRIBUTE2 },
            { FL_MAGENTA,           FL_COURIER_BOLD,    14, STYLE_ATTRIBUTE2 },
            { FL_DARK_YELLOW,       FL_COURIER_BOLD,    14, STYLE_ATTRIBUTE2 },
            { FL_CYAN,              FL_COURIER_BOLD,    14, STYLE_ATTRIBUTE2 },
        };

        static const std::string JSON_EXAMPLE = R"(Example json string
All available options are below.
Use one or all.
{
    "lock": {
        "on": true
    },
    "line": {
        "start": 0,
        "stop": 10,
        "color": "RED"
    },
    "num": {
        "color": "MAGENTA",
        "count": 0
    },
    "string": {
        "word1": "find_text_from_left",
        "color": "BLUE",
        "count": 0
    },
    "rstring": {
        "word1": "find_text_from_right",
        "color": "BLUE",
        "count": 1
    },
    "range": {
        "word1": "from_string",
        "word2": "to_string",
        "inclusive": true,
        "color": "BLUE",
        "count": 0
    },
    "between": {
        "word1": "from_first_string",
        "word2": "to_last_string",
        "inclusive": true,
        "color": "BLUE",
        "count": 0
    },
    "custom": {
        "word1": "string_1",
        "word2": "string_2",
        "start": 0,
        "stop": 10,
        "inclusive": true,
        "color": "BLUE",
        "count": 0
    }
}
    )";

        static const std::string HELP = R"(@bSet style colors
All options will be called for every line.
Text must be valid JSON wrapped within [].
Count property is how many strings to color, 0 means all.
If inclusive is set to false only characters between found strings will be colored.

If lock is true then never change colors that have been set.
@f"lock": {
@f    "on": true
@f}

Color characters by using index in line.
@f"line": {
@f    "start": 0,
@f    "stop": 10,
@f    "color": "RED"
@f}

Color all numbers.
@f"num": {
@f    "color": "MAGENTA",
@f    "count": 0
@f}

Color strings.
@f"string": {
@f    "word1": "find_text_from_left",
@f    "color": "BLUE",
@f    "count": 0
@f}

Color string but start from the right.
@f"rstring": {
@f    "word1": "find_text_from_right",
@f    "color": "BLUE",
@f    "count": 0
@f}

Color text between two strings.
@f"range": {
@f    "word1": "from_string",
@f    "word2": "to_string",
@f    "inclusive": true,
@f    "color": "BLUE",
@f    "count": 0
@f}

Color text from first found string to the last found string.
@f"between": {
@f    "word1": "from_first_string",
@f    "word2": "to_last_string",
@f    "inclusive": true,
@f    "color": "BLUE"
@f}

This property will call LogDisplay::line_custom_cb() which does nothing so override it.
@f"custom": {
@f    "word1": "string_1",
@f    "word2": "string_2",
@f    "start": 0,
@f    "stop": 10,
@f    "inclusive": true,
@f    "color": "BLUE"
@f}

@bValid colors
FOREGROUND
GRAY
RED
GREEN
BLUE
MAGENTA
YELLOW
CYAN
BOLD_FOREGROUND
BOLD_GRAY
BOLD_RED
BOLD_GREEN
BOLD_BLUE
BOLD_MAGENTA
BOLD_YELLOW
BOLD_CYAN
BG_FOREGROUND
BG_GRAY
BG_RED
BG_GREEN
BG_BLUE
BG_MAGENTA
BG_YELLOW
BG_CYAN
BG_BOLD_FOREGROUND
BG_BOLD_GRAY
BG_BOLD_RED
BG_BOLD_GREEN
BG_BOLD_BLUE
BG_BOLD_MAGENTA
BG_BOLD_YELLOW
BG_BOLD_CYAN

    )";

        static const std::string TOOLTIP = R"(Ctrl + 'f' for enter search text.
F3 to search for next word.
Shift + F3 to search for previous word.
Ctrl + 'h' to display style string help.
Ctrl + 'e' to edit style string.
    )";

        //----------------------------------------------------------------------
        enum class STYLE {
            EMPTY,
            LOCK,
            LINE,
            STRING,
            RSTRING,
            RANGE,
            NUM,
            BETWEEN,
            CUSTOM,
        };

        //----------------------------------------------------------------------
        struct Style {
            COLOR               color;
            size_t              count;
            bool                inclusive;
            bool                on;
            size_t              start;
            size_t              stop;
            STYLE               style;
            std::string         word1;
            std::string         word2;

            //------------------------------------------------------------------
            Style() {
                color     = COLOR::FOREGROUND;
                count     = ALL_STRINGS;
                inclusive = false;
                on        = false;
                start     = 0;
                stop      = 0;
                style     = STYLE::EMPTY;
            }

        };

        //----------------------------------------------------------------------
        static COLOR convert_color_string(std::string name) {
            if (name == "GRAY") return COLOR::GRAY;
            else if (name == "RED") return COLOR::RED;
            else if (name == "GREEN") return COLOR::GREEN;
            else if (name == "BLUE") return COLOR::BLUE;
            else if (name == "MAGENTA") return COLOR::MAGENTA;
            else if (name == "YELLOW") return COLOR::YELLOW;
            else if (name == "CYAN") return COLOR::CYAN;
            else if (name == "BOLD_FOREGROUND") return COLOR::BOLD_FOREGROUND;
            else if (name == "BOLD_GRAY") return COLOR::BOLD_GRAY;
            else if (name == "BOLD_RED") return COLOR::BOLD_RED;
            else if (name == "BOLD_GREEN") return COLOR::BOLD_GREEN;
            else if (name == "BOLD_BLUE") return COLOR::BOLD_BLUE;
            else if (name == "BOLD_MAGENTA") return COLOR::BOLD_MAGENTA;
            else if (name == "BOLD_YELLOW") return COLOR::BOLD_YELLOW;
            else if (name == "BOLD_CYAN") return COLOR::BOLD_CYAN;
            else if (name == "BG_FOREGROUND") return COLOR::BG_FOREGROUND;
            else if (name == "BG_GRAY") return COLOR::BG_GRAY;
            else if (name == "BG_RED") return COLOR::BG_RED;
            else if (name == "BG_GREEN") return COLOR::BG_GREEN;
            else if (name == "BG_BLUE") return COLOR::BG_BLUE;
            else if (name == "BG_MAGENTA") return COLOR::BG_MAGENTA;
            else if (name == "BG_YELLOW") return COLOR::BG_YELLOW;
            else if (name == "BG_CYAN") return COLOR::BG_CYAN;
            else if (name == "BG_BOLD_FOREGROUND") return COLOR::BG_BOLD_FOREGROUND;
            else if (name == "BG_BOLD_GRAY") return COLOR::BG_BOLD_GRAY;
            else if (name == "BG_BOLD_RED") return COLOR::BG_BOLD_RED;
            else if (name == "BG_BOLD_GREEN") return COLOR::BG_BOLD_GREEN;
            else if (name == "BG_BOLD_BLUE") return COLOR::BG_BOLD_BLUE;
            else if (name == "BG_BOLD_MAGENTA") return COLOR::BG_BOLD_MAGENTA;
            else if (name == "BG_BOLD_YELLOW") return COLOR::BG_BOLD_YELLOW;
            else if (name == "BG_BOLD_CYAN") return COLOR::BG_BOLD_CYAN;
            else if (name == "BG_BOLD_CYAN") return COLOR::BG_BOLD_CYAN;
            else return COLOR::GRAY;
        }

        //----------------------------------------------------------------------
        static std::vector<Style> parse_json(std::string json) {
            auto nv  = json::NodeVector();
            auto err = json::parse(json, nv);
            auto res = std::vector<Style>();

            if (err.pos >= 0) {
                fl_alert("error: failed to parse json string at line %d and byte %d", (int) err.line, (int) err.pos);
                return res;
            }
            else if (nv.size() < 3) {
                fl_alert("error: nothing to parse");
                return res;
            }

            for (const auto& n : json::find_children(nv, nv[0])) {
                if (n.is_object() == true) {
                    auto style = Style();

                    if (n.name == "lock") {
                        style.style = STYLE::LOCK;
                    }
                    else if (n.name == "line") {
                        style.style = STYLE::LINE;
                    }
                    else if (n.name == "num") {
                        style.style = STYLE::NUM;
                    }
                    else if (n.name == "string") {
                        style.style = STYLE::STRING;
                    }
                    else if (n.name == "rstring") {
                        style.style = STYLE::RSTRING;
                    }
                    else if (n.name == "between") {
                        style.style = STYLE::BETWEEN;
                    }
                    else if (n.name == "range") {
                        style.style = STYLE::RANGE;
                    }
                    else if (n.name == "custom") {
                        style.style = STYLE::CUSTOM;
                    }

                    auto children = json::find_children(nv, n);

                    for (const auto& n2 : children) {
                        if (n2.name == "on" && n2.is_bool() == true) {
                            style.on = n2.tobool();
                        }
                        else if (n2.name == "inclusive" && n2.is_bool() == true) {
                            style.inclusive = n2.tobool();
                        }
                        else if (n2.name == "start" && n2.is_number() == true) {
                            style.start = (size_t) n2.toint();
                        }
                        else if (n2.name == "stop" && n2.is_number() == true) {
                            style.stop = (size_t) n2.toint();
                        }
                        else if (n2.name == "count" && n2.is_number() == true) {
                            style.count = (size_t) n2.toint();
                        }
                        else if (n2.name == "color" && n2.is_string() == true) {
                            style.color = convert_color_string(n2.value);
                        }
                        else if (n2.name == "word1" && n2.is_string() == true) {
                            style.word1 = n2.tostring();
                        }
                        else if (n2.name == "word2" && n2.is_string() == true) {
                            style.word2 = n2.tostring();
                        }
                    }

                    if (style.style != STYLE::EMPTY) {
                        res.push_back(style);
                    }
                }
            }

            return res;
        }

        //----------------------------------------------------------------------
        // Returns new converted buffer if it does contain \r
        // Otherwise it returns nullptr
        //
        char* win_to_unix(const char* string) {
            auto r = false;
            auto b = string;

            while (*b != 0) {
                if (*b++ == '\r') {
                    r = true;
                    break;
                }
            }

            if (r == false) {
                return nullptr;
            }

            auto len = strlen(string);
            auto res = (char*) calloc(len + 1, 1);
            auto pos = 0;

            if (res != nullptr) {
                b = string;

                while (*b != 0) {
                    if (*b != '\r') {
                        res[pos++] = *b;
                    }

                    b++;
                }
            }

            return res;
        }
    } // logdisplay
} // flw

//------------------------------------------------------------------------------
flw::LogDisplay::LogDisplay::Tmp::Tmp() {
    buf  = nullptr;
    len  = 0;
    pos  = 0;
    size = 0;
}

//------------------------------------------------------------------------------
flw::LogDisplay::LogDisplay::Tmp::~Tmp() {
    free(buf);
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
flw::LogDisplay::LogDisplay(int x, int y, int w, int h, const char *l) : Fl_Text_Display(x, y, w, h, l) {
    _buffer      = new Fl_Text_Buffer();
    _style       = new Fl_Text_Buffer();
    _lock_colors = false;
    _tmp         = nullptr;

    buffer(_buffer);
    linenumber_align(FL_ALIGN_RIGHT);
    linenumber_format("%5d");
    update_pref();
    tooltip(logdisplay::TOOLTIP.c_str());
}

//------------------------------------------------------------------------------
flw::LogDisplay::~LogDisplay() {
    buffer(nullptr);
    delete _buffer;
    delete _style;
    delete _tmp;
}

//------------------------------------------------------------------------------
void flw::LogDisplay::edit_styles() {
    auto json    = (_json == "") ? logdisplay::JSON_EXAMPLE : _json;
    auto changed = dlg::text_edit("Edit JSON Style String", json, top_window(), 40, 50);

    if (changed == false) {
        return;
    }

    style(json);
}

//------------------------------------------------------------------------------
void flw::LogDisplay::find(bool next, bool force_ask) {
    if (_find == "" || force_ask) {
        auto s = fl_input("Enter search string", _find.c_str());

        if (s == nullptr || *s == 0) {
            return;
        }

        _find = s;
    }

    auto found = false;
    auto pos   = 0;
    auto start = 0;
    auto end   = 0;
    auto sel   = _buffer->selection_position(&start, &end) != 0;

    if (sel == true) {
        pos = (next == true) ? end : start - 1;
    }
    else {
        pos = 0;
    }

    if (next == true) {
        found = _buffer->search_forward(pos, _find.c_str(), &pos, 1);
    }
    else {
        found = _buffer->search_backward(pos, _find.c_str(), &pos, 1);
    }

    if (found == false && sel == true) {
        if (next == true) {
            found = _buffer->search_forward(0, _find.c_str(), &pos, 1);
        }
        else {
            found = _buffer->search_backward(_buffer->length(), _find.c_str(), &pos, 1);
        }
    }

    if (found == false) {
        fl_beep(FL_BEEP_ERROR);
        return;
    }

    _buffer->select(pos, pos + _find.length());
    insert_position(pos);
    show_insert_position();
}

//------------------------------------------------------------------------------
int flw::LogDisplay::handle(int event) {
    if (event == FL_KEYBOARD) {
        auto key = Fl::event_key();

        if (Fl::event_ctrl() != 0) {
            if (key == 'f') {
                _buffer->unselect();
                find(true, true);
                return 1;
            }
            else if (key == 'e') {
                edit_styles();
                return 1;
            }
            else if (key == 's') {
                save_file();
                return 1;
            }
            else if (key == 'h') {
                dlg::list("Style Help", logdisplay::HELP, top_window(), false, 40, 30);
                return 1;
            }
        }
        else if (Fl::event_shift() != 0 && key == FL_F + 3) {
            find(false, false);
        }
        else if (key == FL_F + 3) {
            find(true, false);
        }
    }

    return Fl_Text_Display::handle(event);
}

//------------------------------------------------------------------------------
void flw::LogDisplay::save_file() {
    auto filename = fl_file_chooser("Select Destination File", nullptr, nullptr, 0);

    if (filename != nullptr && _buffer->savefile(filename) != 0) {
        fl_alert("error: failed to save text to %s", filename);
    }
}

//------------------------------------------------------------------------------
// Parse string and style text
// If input string is empty then LogDisplay::line_cb() is called (must be overriden)
//
void flw::LogDisplay::style(std::string json) {
    auto row = 1;
    auto ds  = (json != "") ? logdisplay::parse_json(json) : std::vector<logdisplay::Style>();

    _json      = json;
    _tmp       = new Tmp();
    _tmp->size = _buffer->length();
    _tmp->buf  = (char*) calloc(_tmp->size + 1, 1);

    if (_tmp->buf != nullptr) {
        memset(_tmp->buf, 'A', _tmp->size);

        while (_tmp->pos < (size_t) _buffer->length()) {
            auto line = _buffer->line_text(_tmp->pos);

            _tmp->len = strlen(line);

            if (_json == "") {
                line_cb(row, line);
            }
            else {
                lock_colors(false);

                for (const auto& d : ds) {
                    if (d.style == logdisplay::STYLE::LOCK) {
                        lock_colors(d.on);
                    }
                    else if (d.style == logdisplay::STYLE::LINE) {
                        style_line(d.start, d.stop, d.color);
                    }
                    else if (d.style == logdisplay::STYLE::STRING) {
                        style_string(line, d.word1, d.color, d.count);
                    }
                    else if (d.style == logdisplay::STYLE::RSTRING) {
                        style_rstring(line, d.word1, d.color, d.count);
                    }
                    else if (d.style == logdisplay::STYLE::RANGE) {
                        style_range(line, d.word1, d.word2, d.inclusive, d.color, d.count);
                    }
                    else if (d.style == logdisplay::STYLE::NUM) {
                        style_num(line, d.color, d.count);
                    }
                    else if (d.style == logdisplay::STYLE::BETWEEN) {
                        style_range(line, d.word1, d.word2, d.inclusive, d.color);
                    }
                    else if (d.style == logdisplay::STYLE::CUSTOM) {
                        line_custom_cb(row, line, d.word1, d.word2, d.color, d.inclusive, d.start, d.stop, d.count);
                    }
                }
            }

            _tmp->pos += _tmp->len + 1;
            row += 1;
            free(line);
        }

        _style->text(_tmp->buf);
        highlight_data(_style, logdisplay::STYLE_TABLE, sizeof(logdisplay::STYLE_TABLE) / sizeof(logdisplay::STYLE_TABLE[0]), (char) logdisplay::COLOR::FOREGROUND, nullptr, 0);
    }

    delete _tmp;
    _tmp = nullptr;
}

//------------------------------------------------------------------------------
void flw::LogDisplay::style_between(const std::string& line, const std::string& word1, const std::string& word2, bool inclusive, logdisplay::COLOR color) {
    if (word1 == "" || word2 == "") {
        return;
    }

    auto pos_left  = line.find(word1);
    auto pos_right = line.rfind(word2);

    if (pos_left != std::string::npos && pos_right != std::string::npos && pos_left < pos_right) {
        if (inclusive == false && pos_right - pos_left > 1) {
            style_line(pos_left + 1, pos_right - 1, color);
        }
        else if (inclusive == true) {
            style_line(pos_left, pos_right, color);
        }
    }
}

//------------------------------------------------------------------------------
// Start and stop are positions in current line
//
void flw::LogDisplay::style_line(size_t start, size_t stop, logdisplay::COLOR c) {
    assert(_tmp);

    start += _tmp->pos;
    stop  += _tmp->pos;

    while (start <= stop && start < _tmp->size && start < _tmp->pos + _tmp->len) {
        if (_lock_colors == false || _tmp->buf[start] == (char) logdisplay::COLOR::FOREGROUND) {
            _tmp->buf[start] = (char) c;
        }

        start++;
    }
}

//------------------------------------------------------------------------------
// Color all numbers
//
void flw::LogDisplay::style_num(const std::string& line, logdisplay::COLOR color, size_t count) {
    if (count == 0) {
        count = 999;
    }

    for (size_t f = 0; f < line.length() && count > 0; f++) {
        auto c = line[f];

        if (c >= '0' && c <= '9') {
            style_line(f, f, color);
            count--;
        }
    }
}

//------------------------------------------------------------------------------
// Color text between from and to string
//
void flw::LogDisplay::style_range(const std::string& line, const std::string& word1, const std::string& word2, bool inclusive, logdisplay::COLOR color, size_t count) {
    if (word1 == "" || word2 == "") {
        return;
    }

    auto pos_from = line.find(word1);

    if (pos_from != std::string::npos) {
        auto pos_to = line.find(word2, pos_from + word1.length());

        if (count == 0) {
            count = 999;
        }

        while (pos_from != std::string::npos && pos_to != std::string::npos && count > 0) {
            if (inclusive == false) {
                style_line(pos_from + word1.length(), pos_to - 1, color);
            }
            else if (inclusive == true) {
                style_line(pos_from, pos_to + word2.length() - 1, color);
            }

            pos_from = line.find(word1, pos_to + word2.length());

            if (pos_from != std::string::npos) {
                pos_to = line.find(word2, pos_from + word1.length());
            }
            else {
                break;
            }

            count--;
        }
    }
}

//------------------------------------------------------------------------------
// Color string - start from the end
//
void flw::LogDisplay::style_rstring(const std::string& line, const std::string& word1, logdisplay::COLOR color, size_t count) {
    if (word1 == "") {
        return;
    }

    auto pos = line.rfind(word1);

    if (count == 0) {
        count = 999;
    }

    while (pos != std::string::npos && count > 0) {
        style_line(pos, pos + word1.length() - 1, color);

        if (pos == 0) {
            break;
        }

        pos = line.rfind(word1, pos - 1);
        count--;
    }
}

//------------------------------------------------------------------------------
// Color string - start from the beginning
//
void flw::LogDisplay::style_string(const std::string& line, const std::string& word1, logdisplay::COLOR color, size_t count) {
    if (word1 == "") {
        return;
    }

    auto pos = line.find(word1);

    if (count == 0) {
        count = 999;
    }

    while (pos != std::string::npos && count > 0) {
        style_line(pos, pos + word1.length() - 1, color);
        pos = line.find(word1, pos + word1.length());
        count--;
    }
}

//------------------------------------------------------------------------------
void flw::LogDisplay::update_pref() {
    labelsize(flw::PREF_FIXED_FONTSIZE);
    linenumber_bgcolor(FL_BACKGROUND_COLOR);
    linenumber_fgcolor(FL_FOREGROUND_COLOR);
    linenumber_font(flw::PREF_FIXED_FONT);
    linenumber_size(flw::PREF_FIXED_FONTSIZE);
    linenumber_width(flw::PREF_FIXED_FONTSIZE * 3);
    textfont(flw::PREF_FIXED_FONT);
    textsize(flw::PREF_FIXED_FONTSIZE);

    for (size_t f = 0; f < sizeof(logdisplay::STYLE_TABLE) / sizeof(logdisplay::STYLE_TABLE[0]); f++) {
        logdisplay::STYLE_TABLE[f].size = flw::PREF_FIXED_FONTSIZE;

#if FL_MINOR_VERSION == 4
        if (theme::is_dark() == true) {
            logdisplay::STYLE_TABLE[f].bgcolor = logdisplay::BG_DARK_COLOR;
        }
        else {
            logdisplay::STYLE_TABLE[f].bgcolor = logdisplay::BG_COLOR;
        }
#endif
    }
}

//------------------------------------------------------------------------------
void flw::LogDisplay::value(const char* text) {
    assert(text);

    _buffer->text("");
    _style->text("");

    auto win = logdisplay::win_to_unix(text);

    if (win != nullptr) {
        _buffer->text(win);
        free(win);
    }
    else {
        _buffer->text(text);
    }
}

// MKALGAM_OFF
