// Copyright gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#include "logdisplay.h"
#include "dlg.h"
#include "json.h"

// MKALGAM_ON

#include <assert.h>
#include <FL/fl_ask.H>
#include <FL/Fl_File_Chooser.H>

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

Fl_Text_Display::Style_Table_Entry _LOGDISPLAY_STYLE_TABLE[] = {
    { FL_FOREGROUND_COLOR,  FL_COURIER,         14, 0, 0 },
    { FL_GRAY,              FL_COURIER,         14, 0, 0 },
    { FL_RED,               FL_COURIER,         14, 0, 0 },
    { FL_DARK_GREEN,        FL_COURIER,         14, 0, 0 },
    { FL_BLUE,              FL_COURIER,         14, 0, 0 },
    { FL_MAGENTA,           FL_COURIER,         14, 0, 0 },
    { FL_DARK_YELLOW,       FL_COURIER,         14, 0, 0 },
    { FL_CYAN,              FL_COURIER,         14, 0, 0 },

    { FL_FOREGROUND_COLOR,  FL_COURIER_BOLD,    14, 0, 0 },
    { FL_GRAY,              FL_COURIER_BOLD,    14, 0, 0 },
    { FL_RED,               FL_COURIER_BOLD,    14, 0, 0 },
    { FL_DARK_GREEN,        FL_COURIER_BOLD,    14, 0, 0 },
    { FL_BLUE,              FL_COURIER_BOLD,    14, 0, 0 },
    { FL_MAGENTA,           FL_COURIER_BOLD,    14, 0, 0 },
    { FL_DARK_YELLOW,       FL_COURIER_BOLD,    14, 0, 0 },
    { FL_CYAN,              FL_COURIER_BOLD,    14, 0, 0 },

    { FL_FOREGROUND_COLOR,  FL_COURIER,         14, Fl_Text_Display::ATTR_BGCOLOR, 0 },
    { FL_GRAY,              FL_COURIER,         14, Fl_Text_Display::ATTR_BGCOLOR, 0 },
    { FL_RED,               FL_COURIER,         14, Fl_Text_Display::ATTR_BGCOLOR, 0 },
    { FL_DARK_GREEN,        FL_COURIER,         14, Fl_Text_Display::ATTR_BGCOLOR, 0 },
    { FL_BLUE,              FL_COURIER,         14, Fl_Text_Display::ATTR_BGCOLOR, 0 },
    { FL_MAGENTA,           FL_COURIER,         14, Fl_Text_Display::ATTR_BGCOLOR, 0 },
    { FL_DARK_YELLOW,       FL_COURIER,         14, Fl_Text_Display::ATTR_BGCOLOR, 0 },
    { FL_CYAN,              FL_COURIER,         14, Fl_Text_Display::ATTR_BGCOLOR, 0 },

    { FL_FOREGROUND_COLOR,  FL_COURIER_BOLD,    14, Fl_Text_Display::ATTR_BGCOLOR, 0 },
    { FL_GRAY,              FL_COURIER_BOLD,    14, Fl_Text_Display::ATTR_BGCOLOR, 0 },
    { FL_RED,               FL_COURIER_BOLD,    14, Fl_Text_Display::ATTR_BGCOLOR, 0 },
    { FL_DARK_GREEN,        FL_COURIER_BOLD,    14, Fl_Text_Display::ATTR_BGCOLOR, 0 },
    { FL_BLUE,              FL_COURIER_BOLD,    14, Fl_Text_Display::ATTR_BGCOLOR, 0 },
    { FL_MAGENTA,           FL_COURIER_BOLD,    14, Fl_Text_Display::ATTR_BGCOLOR, 0 },
    { FL_DARK_YELLOW,       FL_COURIER_BOLD,    14, Fl_Text_Display::ATTR_BGCOLOR, 0 },
    { FL_CYAN,              FL_COURIER_BOLD,    14, Fl_Text_Display::ATTR_BGCOLOR, 0 },
};

static const std::string _LOGDISPLAY_JSON_EXAMPLE = R"(Example json string
All available options are below.
Use one or all.
[
    {
        "style": "lock",
        "on": true
    },
    {
        "style": "line",
        "start": 0,
        "stop": 10,
        "color": "RED"
    },
    {
        "style": "num",
        "color": "MAGENTA",
        "count": 0
    },
    {
        "style": "string",
        "word1": "find_text_from_left",
        "color": "BLUE",
        "count": 0
    },
    {
        "style": "rstring",
        "word1": "find_text_from_right",
        "color": "BLUE",
        "count": 1
    },
    {
        "style": "range",
        "word1": "from_string",
        "word2": "to_string",
        "inclusive": true,
        "color": "BLUE",
        "count": 0
    },
    {
        "style": "between",
        "word1": "from_first_string",
        "word2": "to_last_string",
        "inclusive": true,
        "color": "BLUE",
        "count": 0
    },
    {
        "style": "custom",
        "word1": "string_1",
        "word2": "string_2",
        "start": 0,
        "stop": 10,
        "inclusive": true,
        "color": "BLUE",
        "count": 0
    }
]
)";

static const std::string _LOGDISPLAY_HELP = R"(@bSet style colors
All options will be called for every line.
Text must be valid JSON wrapped within [].
Count property is how many strings to color, 0 means all.
If inclusive is set to false only characters between found strings will be colored.

If lock is true then never change colors that have been set.
@f{
@f    "style": "lock",
@f    "on": true
@f}

Color characters by using index in line.
@f{
@f    "style": "line",
@f    "start": 0,
@f    "stop": 10,
@f    "color": "RED"
@f}

Color all numbers.
@f{
@f    "style": "num",
@f    "color": "MAGENTA",
@f    "count": 0
@f}

Color strings.
@f{
@f    "style": "string",
@f    "word1": "find_text_from_left",
@f    "color": "BLUE",
@f    "count": 0
@f}

Color string but start from the right.
@f{
@f    "style": "rstring",
@f    "word1": "find_text_from_right",
@f    "color": "BLUE",
@f    "count": 0
@f}

Color text between two strings.
@f{
@f    "style": "range",
@f    "word1": "from_string",
@f    "word2": "to_string",
@f    "inclusive": true,
@f    "color": "BLUE",
@f    "count": 0
@f}

Color text from first found string to the last found string.
@f{
@f    "style": "between",
@f    "word1": "from_first_string",
@f    "word2": "to_last_string",
@f    "inclusive": true,
@f    "color": "BLUE"
@f}

This property will call LogDisplay::line_custom_cb() which does nothing so override it.
@f{
@f    "style": "custom",
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

static const std::string _LOGDISPLAY_TOOLTIP = R"(Ctrl + 'f' for enter search text.
F3 to search for next word.
Shift + F3 to search for previous word.
Ctrl + 'h' to display style string help.
Ctrl + 'e' to edit style string.
)";

//----------------------------------------------------------------------
struct _LogDisplayStyle {
    enum STYLE {
                                EMPTY,
                                BETWEEN,
                                CUSTOM,
                                LINE,
                                LOCK,
                                NUM,
                                RANGE,
                                RSTRING,
                                STRING,
    };

    LogDisplay::COLOR           color;
    STYLE                       style;
    bool                        inclusive;
    bool                        on;
    size_t                      count;
    size_t                      start;
    size_t                      stop;
    std::string                 word1;
    std::string                 word2;

    //------------------------------------------------------------------
    _LogDisplayStyle() {
        color     = LogDisplay::FOREGROUND;
        count     = 0;
        inclusive = false;
        on        = false;
        start     = 0;
        stop      = 0;
        style     = _LogDisplayStyle::EMPTY;
    }
};

//----------------------------------------------------------------------
static LogDisplay::COLOR _logdisplay_convert_color(std::string name) {
    if (name == "GRAY") return LogDisplay::GRAY;
    else if (name == "RED") return LogDisplay::RED;
    else if (name == "GREEN") return LogDisplay::GREEN;
    else if (name == "BLUE") return LogDisplay::BLUE;
    else if (name == "MAGENTA") return LogDisplay::MAGENTA;
    else if (name == "YELLOW") return LogDisplay::YELLOW;
    else if (name == "CYAN") return LogDisplay::CYAN;
    else if (name == "BOLD_FOREGROUND") return LogDisplay::BOLD_FOREGROUND;
    else if (name == "BOLD_GRAY") return LogDisplay::BOLD_GRAY;
    else if (name == "BOLD_RED") return LogDisplay::BOLD_RED;
    else if (name == "BOLD_GREEN") return LogDisplay::BOLD_GREEN;
    else if (name == "BOLD_BLUE") return LogDisplay::BOLD_BLUE;
    else if (name == "BOLD_MAGENTA") return LogDisplay::BOLD_MAGENTA;
    else if (name == "BOLD_YELLOW") return LogDisplay::BOLD_YELLOW;
    else if (name == "BOLD_CYAN") return LogDisplay::BOLD_CYAN;
    else if (name == "BG_FOREGROUND") return LogDisplay::BG_FOREGROUND;
    else if (name == "BG_GRAY") return LogDisplay::BG_GRAY;
    else if (name == "BG_RED") return LogDisplay::BG_RED;
    else if (name == "BG_GREEN") return LogDisplay::BG_GREEN;
    else if (name == "BG_BLUE") return LogDisplay::BG_BLUE;
    else if (name == "BG_MAGENTA") return LogDisplay::BG_MAGENTA;
    else if (name == "BG_YELLOW") return LogDisplay::BG_YELLOW;
    else if (name == "BG_CYAN") return LogDisplay::BG_CYAN;
    else if (name == "BG_BOLD_FOREGROUND") return LogDisplay::BG_BOLD_FOREGROUND;
    else if (name == "BG_BOLD_GRAY") return LogDisplay::BG_BOLD_GRAY;
    else if (name == "BG_BOLD_RED") return LogDisplay::BG_BOLD_RED;
    else if (name == "BG_BOLD_GREEN") return LogDisplay::BG_BOLD_GREEN;
    else if (name == "BG_BOLD_BLUE") return LogDisplay::BG_BOLD_BLUE;
    else if (name == "BG_BOLD_MAGENTA") return LogDisplay::BG_BOLD_MAGENTA;
    else if (name == "BG_BOLD_YELLOW") return LogDisplay::BG_BOLD_YELLOW;
    else if (name == "BG_BOLD_CYAN") return LogDisplay::BG_BOLD_CYAN;
    else return LogDisplay::GRAY;
}

//----------------------------------------------------------------------
static std::vector<_LogDisplayStyle> _logdisplay_parse_json(std::string json) {
    #define FLW_LOGDISPLAY_ERROR(X) { fl_alert("error: illegal value at pos %u", (X)->pos()); res.clear(); return res; }

    auto res = std::vector<_LogDisplayStyle>();
    auto js  = gnu::JS();
    auto err = js.decode(json.c_str(), json.length());

    if (err != "") {
        fl_alert("error: failed to parse json\n%s", err.c_str());
        return res;
    }

    if (js.is_array() == false) FLW_LOGDISPLAY_ERROR(&js)

    for (const auto j : *js.va()) {
        if (j->is_object() == false) FLW_LOGDISPLAY_ERROR(j);
        auto style = _LogDisplayStyle();

        for (const auto j2 : j->vo_to_va()) {
            if (j2->name() == "color" && j2->is_string() == true)           style.color     = _logdisplay_convert_color(j2->vs());
            else if (j2->name() == "count" && j2->is_number() == true)      style.count     = (size_t) j2->vn_i();
            else if (j2->name() == "inclusive" && j2->is_bool() == true)    style.inclusive = j2->vb();
            else if (j2->name() == "on" && j2->is_bool() == true)           style.on        = j2->vb();
            else if (j2->name() == "start" && j2->is_number() == true)      style.start     = (size_t) j2->vn_i();
            else if (j2->name() == "stop" && j2->is_number() == true)       style.stop      = (size_t) j2->vn_i();
            else if (j2->name() == "word1" && j2->is_string() == true)      style.word1     = j2->vs_u();
            else if (j2->name() == "word2" && j2->is_string() == true)      style.word2     = j2->vs_u();
            else if (j2->name() == "style" && j2->is_string() == true) {
                if (j2->vs() == "between")      style.style = _LogDisplayStyle::BETWEEN;
                else if (j2->vs() == "custom")  style.style = _LogDisplayStyle::CUSTOM;
                else if (j2->vs() == "line")    style.style = _LogDisplayStyle::LINE;
                else if (j2->vs() == "lock")    style.style = _LogDisplayStyle::LOCK;
                else if (j2->vs() == "num")     style.style = _LogDisplayStyle::NUM;
                else if (j2->vs() == "range")   style.style = _LogDisplayStyle::RANGE;
                else if (j2->vs() == "rstring") style.style = _LogDisplayStyle::RSTRING;
                else if (j2->vs() == "string")  style.style = _LogDisplayStyle::STRING;
                else FLW_LOGDISPLAY_ERROR(j2)
            }
            else FLW_LOGDISPLAY_ERROR(j2)
        }

        if (style.style != _LogDisplayStyle::EMPTY) {
            res.push_back(style);
        }
    }

    return res;
}

//----------------------------------------------------------------------
// Returns new converted buffer if it does contain \r.
// Otherwise it returns nullptr.
//
static char* _logdisplay_win_to_unix(const char* string) {
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
    auto res = static_cast<char*>(calloc(len + 1, 1));

    if (res != nullptr) {
        auto pos = 0;
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

//------------------------------------------------------------------------------
LogDisplay::LogDisplay::Tmp::Tmp() {
    buf  = nullptr;
    len  = 0;
    pos  = 0;
    size = 0;
}

//------------------------------------------------------------------------------
LogDisplay::LogDisplay::Tmp::~Tmp() {
    free(buf);
}

/***
 *      _                 _____  _           _
 *     | |               |  __ \(_)         | |
 *     | |     ___   __ _| |  | |_ ___ _ __ | | __ _ _   _
 *     | |    / _ \ / _` | |  | | / __| '_ \| |/ _` | | | |
 *     | |___| (_) | (_| | |__| | \__ \ |_) | | (_| | |_| |
 *     |______\___/ \__, |_____/|_|___/ .__/|_|\__,_|\__, |
 *                   __/ |            | |             __/ |
 *                  |___/             |_|            |___/
 */

//------------------------------------------------------------------------------
LogDisplay::LogDisplay(int x, int y, int w, int h, const char *l) : Fl_Text_Display(x, y, w, h, l) {
    _buffer      = new Fl_Text_Buffer();
    _style       = new Fl_Text_Buffer();
    _lock_colors = false;
    _tmp         = nullptr;

    buffer(_buffer);
    linenumber_align(FL_ALIGN_RIGHT);
    linenumber_format("%5d");
    update_pref();
    tooltip(_LOGDISPLAY_TOOLTIP.c_str());
}

//------------------------------------------------------------------------------
LogDisplay::~LogDisplay() {
    buffer(nullptr);
    delete _buffer;
    delete _style;
    delete _tmp;
}

//------------------------------------------------------------------------------
void LogDisplay::edit_styles() {
    auto json    = (_json == "") ? _LOGDISPLAY_JSON_EXAMPLE : _json;
    auto changed = dlg::text_edit("Edit JSON Style String", json, top_window(), 40, 50);

    if (changed == false) {
        return;
    }

    style(json);
}

//------------------------------------------------------------------------------
void LogDisplay::find(bool next, bool force_ask) {
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
int LogDisplay::handle(int event) {
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
                dlg::list("Style Help", _LOGDISPLAY_HELP, top_window(), false, 40, 30);
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
void LogDisplay::save_file() {
    auto filename = fl_file_chooser("Select Destination File", nullptr, nullptr, 0);

    if (filename != nullptr && _buffer->savefile(filename) != 0) {
        fl_alert("error: failed to save text to %s", filename);
    }
}

//------------------------------------------------------------------------------
// Parse string and style text
// If input string is empty then LogDisplay::line_cb() is called (must be overriden)
//
void LogDisplay::style(std::string json) {
    auto ds  = (json != "") ? _logdisplay_parse_json(json) : std::vector<_LogDisplayStyle>();

    _json      = json;
    _tmp       = new Tmp();
    _tmp->size = _buffer->length();
    _tmp->buf  = static_cast<char*>(calloc(_tmp->size + 1, 1));

    if (_tmp->buf != nullptr) {
        auto row = 1;
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
                    if (d.style == _LogDisplayStyle::BETWEEN) {
                        style_range(line, d.word1, d.word2, d.inclusive, d.color);
                    }
                    else if (d.style == _LogDisplayStyle::CUSTOM) {
                        line_custom_cb(row, line, d.word1, d.word2, d.color, d.inclusive, d.start, d.stop, d.count);
                    }
                    else if (d.style == _LogDisplayStyle::LINE) {
                        style_line(d.start, d.stop, d.color);
                    }
                    else if (d.style == _LogDisplayStyle::LOCK) {
                        lock_colors(d.on);
                    }
                    else if (d.style == _LogDisplayStyle::NUM) {
                        style_num(line, d.color, d.count);
                    }
                    else if (d.style == _LogDisplayStyle::RANGE) {
                        style_range(line, d.word1, d.word2, d.inclusive, d.color, d.count);
                    }
                    else if (d.style == _LogDisplayStyle::RSTRING) {
                        style_rstring(line, d.word1, d.color, d.count);
                    }
                    else if (d.style == _LogDisplayStyle::STRING) {
                        style_string(line, d.word1, d.color, d.count);
                    }
                }
            }

            _tmp->pos += _tmp->len + 1;
            row += 1;
            free(line);
        }

        _style->text(_tmp->buf);
        highlight_data(_style, _LOGDISPLAY_STYLE_TABLE, sizeof(_LOGDISPLAY_STYLE_TABLE) / sizeof(_LOGDISPLAY_STYLE_TABLE[0]), (char) LogDisplay::FOREGROUND, nullptr, 0);
    }

    delete _tmp;
    _tmp = nullptr;
}

//------------------------------------------------------------------------------
void LogDisplay::style_between(const std::string& line, const std::string& word1, const std::string& word2, bool inclusive, LogDisplay::COLOR color) {
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
void LogDisplay::style_line(size_t start, size_t stop, LogDisplay::COLOR c) {
    assert(_tmp);

    start += _tmp->pos;
    stop  += _tmp->pos;

    while (start <= stop && start < _tmp->size && start < _tmp->pos + _tmp->len) {
        if (_lock_colors == false || _tmp->buf[start] == (char) LogDisplay::LogDisplay::FOREGROUND) {
            _tmp->buf[start] = (char) c;
        }

        start++;
    }
}

//------------------------------------------------------------------------------
// Color all numbers
//
void LogDisplay::style_num(const std::string& line, LogDisplay::COLOR color, size_t count) {
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
void LogDisplay::style_range(const std::string& line, const std::string& word1, const std::string& word2, bool inclusive, LogDisplay::COLOR color, size_t count) {
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
            else {
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
void LogDisplay::style_rstring(const std::string& line, const std::string& word1, LogDisplay::COLOR color, size_t count) {
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
void LogDisplay::style_string(const std::string& line, const std::string& word1, LogDisplay::COLOR color, size_t count) {
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
void LogDisplay::update_pref() {
    labelsize(flw::PREF_FIXED_FONTSIZE);
    linenumber_bgcolor(FL_BACKGROUND_COLOR);
    linenumber_fgcolor(FL_FOREGROUND_COLOR);
    linenumber_font(flw::PREF_FIXED_FONT);
    linenumber_size(flw::PREF_FIXED_FONTSIZE);
    linenumber_width(flw::PREF_FIXED_FONTSIZE * 3);
    textfont(flw::PREF_FIXED_FONT);
    textsize(flw::PREF_FIXED_FONTSIZE);

    for (size_t f = 0; f < sizeof(_LOGDISPLAY_STYLE_TABLE) / sizeof(_LOGDISPLAY_STYLE_TABLE[0]); f++) {
        _LOGDISPLAY_STYLE_TABLE[f].size = flw::PREF_FIXED_FONTSIZE;

        if (theme::is_dark() == true) {
            _LOGDISPLAY_STYLE_TABLE[f].bgcolor = FL_WHITE;
        }
        else {
            _LOGDISPLAY_STYLE_TABLE[f].bgcolor = fl_lighter(FL_GRAY);
        }
    }
}

//------------------------------------------------------------------------------
void LogDisplay::value(const char* text) {
    assert(text);

    _buffer->text("");
    _style->text("");

    auto win = _logdisplay_win_to_unix(text);

    if (win != nullptr) {
        _buffer->text(win);
        free(win);
    }
    else {
        _buffer->text(text);
    }
}

} // flw

// MKALGAM_OFF
