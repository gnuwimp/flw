/**
* @file
* @brief A static text display that can be used for logging or similar purposes.
*
* @author gnuwimp@gmail.com
* @copyright Released under the GNU General Public License v3.0
*/

#include "dlg.h"
#include "json.h"
#include "logdisplay.h"
#include "theme.h"

// MKALGAM_ON

#include <FL/Fl_File_Chooser.H>

namespace flw {
namespace priv {

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

    { FL_FOREGROUND_COLOR,  FL_COURIER,         14, Fl_Text_Display::ATTR_UNDERLINE, 0 },
    { FL_GRAY,              FL_COURIER,         14, Fl_Text_Display::ATTR_UNDERLINE, 0 },
    { FL_RED,               FL_COURIER,         14, Fl_Text_Display::ATTR_UNDERLINE, 0 },
    { FL_DARK_GREEN,        FL_COURIER,         14, Fl_Text_Display::ATTR_UNDERLINE, 0 },
    { FL_BLUE,              FL_COURIER,         14, Fl_Text_Display::ATTR_UNDERLINE, 0 },
    { FL_MAGENTA,           FL_COURIER,         14, Fl_Text_Display::ATTR_UNDERLINE, 0 },
    { FL_DARK_YELLOW,       FL_COURIER,         14, Fl_Text_Display::ATTR_UNDERLINE, 0 },
    { FL_CYAN,              FL_COURIER,         14, Fl_Text_Display::ATTR_UNDERLINE, 0 },

    { FL_FOREGROUND_COLOR,  FL_COURIER_BOLD,    14, Fl_Text_Display::ATTR_UNDERLINE, 0 },
    { FL_GRAY,              FL_COURIER_BOLD,    14, Fl_Text_Display::ATTR_UNDERLINE, 0 },
    { FL_RED,               FL_COURIER_BOLD,    14, Fl_Text_Display::ATTR_UNDERLINE, 0 },
    { FL_DARK_GREEN,        FL_COURIER_BOLD,    14, Fl_Text_Display::ATTR_UNDERLINE, 0 },
    { FL_BLUE,              FL_COURIER_BOLD,    14, Fl_Text_Display::ATTR_UNDERLINE, 0 },
    { FL_MAGENTA,           FL_COURIER_BOLD,    14, Fl_Text_Display::ATTR_UNDERLINE, 0 },
    { FL_DARK_YELLOW,       FL_COURIER_BOLD,    14, Fl_Text_Display::ATTR_UNDERLINE, 0 },
    { FL_CYAN,              FL_COURIER_BOLD,    14, Fl_Text_Display::ATTR_UNDERLINE, 0 },
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
All options will be called (sequentially) for every line.
Text must be valid JSON wrapped within [].
Count property is how many strings to color, 0 means all.
If inclusive is set to false only characters between found strings will be colored.
Colors might be overwritten unless a lock has been set.

@bIf lock is true then never change colors that have been set in current line.
@f{
@f    "style": "lock",
@f    "on": true
@f}

@bColor characters by using index in line.
@f{
@f    "style": "line",
@f    "start": 0,
@f    "stop": 10,
@f    "color": "RED"
@f}

@bColor all numbers.
@f{
@f    "style": "num",
@f    "color": "MAGENTA",
@f    "count": 0
@f}

@bColor strings.
@f{
@f    "style": "string",
@f    "word1": "find_text_from_left",
@f    "color": "BLUE",
@f    "count": 0
@f}

@bColor string but start from the right.
@f{
@f    "style": "rstring",
@f    "word1": "find_text_from_right",
@f    "color": "BLUE",
@f    "count": 0
@f}

@bColor text between two strings.
@f{
@f    "style": "range",
@f    "word1": "from_string",
@f    "word2": "to_string",
@f    "inclusive": true,
@f    "color": "BLUE",
@f    "count": 0
@f}

@bColor text from first found string to the last found string.
@f{
@f    "style": "between",
@f    "word1": "from_first_string",
@f    "word2": "to_last_string",
@f    "inclusive": true,
@f    "color": "BLUE"
@f}

@bThis property will call LogDisplay::line_custom_cb() which does nothing so override it.
@f{
@f    "style": "custom",
@f    "word1": "string_1",
@f    "word2": "string_2",
@f    "start": 0,
@f    "stop": 10,
@f    "inclusive": true,
@f    "color": "BLUE"
@f}

@bNormal colors:
FOREGROUND
GRAY
RED
GREEN
BLUE
MAGENTA
YELLOW
CYAN

@bBold colors:
BOLD_FOREGROUND
BOLD_GRAY
BOLD_RED
BOLD_GREEN
BOLD_BLUE
BOLD_MAGENTA
BOLD_YELLOW
BOLD_CYAN

@bNormal colors with underline:
U_FOREGROUND
U_GRAY
U_RED
U_GREEN
U_BLUE
U_MAGENTA
U_YELLOW
U_CYAN

@bBold colors with underline:
U_BOLD_FOREGROUND
U_BOLD_GRAY
U_BOLD_RED
U_BOLD_GREEN
U_BOLD_BLUE
U_BOLD_MAGENTA
U_BOLD_YELLOW
U_BOLD_CYAN

)";

static const std::string _LOGDISPLAY_TOOLTIP = R"(Ctrl + 'f' for enter search text.
F3 to search for next word.
Shift + F3 to search for previous word.
Ctrl + 'h' to display style string help.
Ctrl + 'e' to edit style string.
)";

/*
 *           _                 _____  _           _             ____         __  __
 *          | |               |  __ \(_)         | |           |  _ \       / _|/ _|
 *          | |     ___   __ _| |  | |_ ___ _ __ | | __ _ _   _| |_) |_   _| |_| |_ ___ _ __
 *          | |    / _ \ / _` | |  | | / __| '_ \| |/ _` | | | |  _ <| | | |  _|  _/ _ \ '__|
 *          | |___| (_) | (_| | |__| | \__ \ |_) | | (_| | |_| | |_) | |_| | | | ||  __/ |
 *          |______\___/ \__, |_____/|_|___/ .__/|_|\__,_|\__, |____/ \__,_|_| |_| \___|_|
 *      ______            __/ |            | |             __/ |
 *     |______|          |___/             |_|            |___/
 */


/** @brief Buffer data.
* @private
*/
struct _LogDisplayBuffer {
    char*                   buf;    ///< @brief Text buffer.
    size_t                  line;   ///< @brief Current line length.
    size_t                  pos;    ///< @brief Current pos in buffer.
    size_t                  size;   ///< @brief Text length.

    _LogDisplayBuffer() {
        buf  = nullptr;
        line = 0;
        pos  = 0;
        size = 0;
    }

    ~_LogDisplayBuffer() {
        free(buf);
    }
};

/*
 *           _                 _____  _           _              _____ _         _
 *          | |               |  __ \(_)         | |            / ____| |       | |
 *          | |     ___   __ _| |  | |_ ___ _ __ | | __ _ _   _| (___ | |_ _   _| | ___
 *          | |    / _ \ / _` | |  | | / __| '_ \| |/ _` | | | |\___ \| __| | | | |/ _ \
 *          | |___| (_) | (_| | |__| | \__ \ |_) | | (_| | |_| |____) | |_| |_| | |  __/
 *          |______\___/ \__, |_____/|_|___/ .__/|_|\__,_|\__, |_____/ \__|\__, |_|\___|
 *      ______            __/ |            | |             __/ |            __/ |
 *     |______|          |___/             |_|            |___/            |___/
 */

/** @brief Display styles.
* @private
*
*/
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

    LogDisplay::Color           color;
    STYLE                       style;
    bool                        inclusive;
    bool                        on;
    size_t                      count;
    size_t                      start;
    size_t                      stop;
    std::string                 word1;
    std::string                 word2;

    /** @brief Init.
    *
    */
    _LogDisplayStyle() {
        color     = LogDisplay::Color::FOREGROUND;
        count     = 0;
        inclusive = false;
        on        = false;
        start     = 0;
        stop      = 0;
        style     = _LogDisplayStyle::EMPTY;
    }
};

/** @brief Convert string color to real color.
*
*/
static LogDisplay::Color _logdisplay_convert_color(const std::string& name) {
    if (name == "GRAY") return LogDisplay::Color::GRAY;
    else if (name == "RED") return LogDisplay::Color::RED;
    else if (name == "GREEN") return LogDisplay::Color::GREEN;
    else if (name == "BLUE") return LogDisplay::Color::BLUE;
    else if (name == "MAGENTA") return LogDisplay::Color::MAGENTA;
    else if (name == "YELLOW") return LogDisplay::Color::YELLOW;
    else if (name == "CYAN") return LogDisplay::Color::CYAN;
    else if (name == "BOLD_FOREGROUND") return LogDisplay::Color::BOLD_FOREGROUND;
    else if (name == "BOLD_GRAY") return LogDisplay::Color::BOLD_GRAY;
    else if (name == "BOLD_RED") return LogDisplay::Color::BOLD_RED;
    else if (name == "BOLD_GREEN") return LogDisplay::Color::BOLD_GREEN;
    else if (name == "BOLD_BLUE") return LogDisplay::Color::BOLD_BLUE;
    else if (name == "BOLD_MAGENTA") return LogDisplay::Color::BOLD_MAGENTA;
    else if (name == "BOLD_YELLOW") return LogDisplay::Color::BOLD_YELLOW;
    else if (name == "BOLD_CYAN") return LogDisplay::Color::BOLD_CYAN;
    else if (name == "U_FOREGROUND") return LogDisplay::Color::U_FOREGROUND;
    else if (name == "U_GRAY") return LogDisplay::Color::U_GRAY;
    else if (name == "U_RED") return LogDisplay::Color::U_RED;
    else if (name == "U_GREEN") return LogDisplay::Color::U_GREEN;
    else if (name == "U_BLUE") return LogDisplay::Color::U_BLUE;
    else if (name == "U_MAGENTA") return LogDisplay::Color::U_MAGENTA;
    else if (name == "U_YELLOW") return LogDisplay::Color::U_YELLOW;
    else if (name == "U_CYAN") return LogDisplay::Color::U_CYAN;
    else if (name == "U_BOLD_FOREGROUND") return LogDisplay::Color::U_BOLD_FOREGROUND;
    else if (name == "U_BOLD_GRAY") return LogDisplay::Color::U_BOLD_GRAY;
    else if (name == "U_BOLD_RED") return LogDisplay::Color::U_BOLD_RED;
    else if (name == "U_BOLD_GREEN") return LogDisplay::Color::U_BOLD_GREEN;
    else if (name == "U_BOLD_BLUE") return LogDisplay::Color::U_BOLD_BLUE;
    else if (name == "U_BOLD_MAGENTA") return LogDisplay::Color::U_BOLD_MAGENTA;
    else if (name == "U_BOLD_YELLOW") return LogDisplay::Color::U_BOLD_YELLOW;
    else if (name == "U_BOLD_CYAN") return LogDisplay::Color::U_BOLD_CYAN;
    else return LogDisplay::Color::GRAY;
}

/** @brief Convert json to display styles.
*
*/
static std::vector<_LogDisplayStyle> _logdisplay_parse_json(const std::string& json) {
    #define FLW_LOGDISPLAY_ERROR(X) { dlg::msg_alert("LogDisplay", util::format("Illegal value at pos %u", (X)->pos())); res.clear(); return res; }

    auto res = std::vector<_LogDisplayStyle>();
    auto js  = gnu::json::decode(json.c_str(), json.length(), true);

    if (js.has_err() == true) {
        dlg::msg_alert("LogDisplay", util::format("Failed to parse json!\n%s", js.err_c()));
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

/** @brief Returns new converted buffer if it does contain \\r.
*
* Otherwise it returns nullptr.
*/
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

} // flw::priv
} // flw

/*
 *      _                 _____  _           _
 *     | |               |  __ \(_)         | |
 *     | |     ___   __ _| |  | |_ ___ _ __ | | __ _ _   _
 *     | |    / _ \ / _` | |  | | / __| '_ \| |/ _` | | | |
 *     | |___| (_) | (_| | |__| | \__ \ |_) | | (_| | |_| |
 *     |______\___/ \__, |_____/|_|___/ .__/|_|\__,_|\__, |
 *                   __/ |            | |             __/ |
 *                  |___/             |_|            |___/
 */

/** @brief Create log display widget.
*
* @param[in] X  X pos.
* @param[in] Y  Y pos.
* @param[in] W  Width.
* @param[in] H  Height.
* @param[in] l  Label.
*/
flw::LogDisplay::LogDisplay(int X, int Y, int W, int H, const char *l) : Fl_Text_Display(X, Y, W, H, l) {
    _buffer      = new Fl_Text_Buffer();
    _style       = new Fl_Text_Buffer();
    _lock_colors = false;
    _tmp         = nullptr;

    buffer(_buffer);
    linenumber_align(FL_ALIGN_RIGHT);
    linenumber_format("%5d");
    update_pref();
    tooltip(priv::_LOGDISPLAY_TOOLTIP.c_str());
}

/** @brief Free buffer.
*
*/
flw::LogDisplay::~LogDisplay() {
    buffer(nullptr);
    delete _buffer;
    delete _style;
    delete _tmp;
}

/** @brief Start dialog to edit style string.
*
*/
void flw::LogDisplay::edit_styles() {
    auto json    = (_json == "") ? priv::_LOGDISPLAY_JSON_EXAMPLE : _json;
    auto changed = dlg::text_edit("LogDisplay - Edit JSON Style", json, 40, 50);

    if (changed == false) {
        return;
    }

    style(json);
}

/** @brief Show dialog and search for text.
*
* @param[in] next       Find next.
* @param[in] force_ask  True to force showing dialog.
*/
void flw::LogDisplay::find(bool next, bool force_ask) {
    if (_find == "" || force_ask) {
        auto answer = dlg::input("LogDisplay", "Enter search string.", _find);

        if (answer == labels::CANCEL || _find == "") {
            return;
        }
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

/** @brief Handle keyboard shortcuts.
*
*/
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
                dlg::list("LogDisplay - Style Help", priv::_LOGDISPLAY_HELP, false, 40, 30);
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

/** @brief Callback for custom styling.
*
* @param[in] row   Line number.
* @param[in] line  Text line.
*/
void flw::LogDisplay::line_cb(size_t row, const std::string& line) {
    (void) row;
    (void) line;
}

/** @brief Callback for custom styling.
*
* Not all values has to be used at the same time.
*
* @param[in] row        Line number.
* @param[in] line       Text line.
* @param[in] word1      Start word.
* @param[in] word2      Stop word.
* @param[in] color      Text color.
* @param[in] inclusive  True to color inclusive.
* @param[in] start      Start pos.
* @param[in] stop       Stop pos.
* @param[in] count      Number of stylings.
*/
void flw::LogDisplay::line_custom_cb(size_t row, const std::string& line, const std::string& word1, const std::string& word2, LogDisplay::Color color, bool inclusive, size_t start, size_t stop, size_t count)  {
    (void) row;
    (void) line;
    (void) word1;
    (void) word2;
    (void) color;
    (void) inclusive;
    (void) start;
    (void) stop;
    (void) count;
}

/** @brief Save text to file.
*
*/
void flw::LogDisplay::save_file() {
    auto filename = fl_file_chooser("Select Destination File", nullptr, nullptr, 0);

    if (filename != nullptr && _buffer->savefile(filename) != 0) {
        dlg::msg_alert("LogDisplay", util::format("Failed to save text to %s!", filename));
    }
}

/** @brief Parse json string and style text.
*
* If JSON string is empty then LogDisplay::line_cb() is called (must be overriden).\n
*\n
* All JSON strings are parsed sequentially for every line.\n
* JSON examples:\n
*\n
* If lock is true then never change colors that have been set in current line.
* @code
* {
*     "style": "lock",
*     "on": true
* }
* @endcode
*
* Color characters by using index in line.
* @code
* {
*     "style": "line",
*     "start": 0,
*     "stop": 10,
*     "color": "RED"
* }
* @endcode
*
* Color all numbers.
* @code
* {
*     "style": "num",
*     "color": "MAGENTA",
*     "count": 0
* }
* @endcode
*
* Color strings.
* @code
* {
*     "style": "string",
*     "word1": "find_text_from_left",
*     "color": "BLUE",
*     "count": 0
* }
* @endcode
*
* Color string but start from the right.
* @code
* {
*     "style": "rstring",
*     "word1": "find_text_from_right",
*     "color": "BLUE",
*     "count": 0
* }
* @endcode
*
* Color text between two strings.
* @code
* {
*     "style": "range",
*     "word1": "from_string",
*     "word2": "to_string",
*     "inclusive": true,
*     "color": "BLUE",
*     "count": 0
* }
* @endcode
*
* Color text from first found string to the last found string.
* @code
* {
*     "style": "between",
*     "word1": "from_first_string",
*     "word2": "to_last_string",
*     "inclusive": true,
*     "color": "BLUE"
* }
* @endcode
*
* This property will call LogDisplay::line_custom_cb() which does nothing so override it.
* @code
* {
*     "style": "custom",
*     "word1": "string_1",
*     "word2": "string_2",
*     "start": 0,
*     "stop": 10,
*     "inclusive": true,
*     "color": "BLUE"
* }
* @endcode
*
* @param[in] json  JSON string.
*/
void flw::LogDisplay::style(const std::string& json) {
    auto ds = (json != "") ? priv::_logdisplay_parse_json(json) : std::vector<priv::_LogDisplayStyle>();

    _json      = json;
    _tmp       = new priv::_LogDisplayBuffer();
    _tmp->size = _buffer->length();
    _tmp->buf  = static_cast<char*>(calloc(_tmp->size + 1, 1));

    if (_tmp->buf != nullptr) {
        auto row = 1;
        memset(_tmp->buf, 'A', _tmp->size);

        while (_tmp->pos < (size_t) _buffer->length()) {
            auto line = _buffer->line_text(_tmp->pos);

            _tmp->line = strlen(line);

            if (_json == "") {
                line_cb(row, line);
            }
            else {
                unlock_colors(); // Every text line starts with color unlocked.

                for (const auto& d : ds) {
                    if (d.style == priv::_LogDisplayStyle::BETWEEN) {
                        style_range(line, d.word1, d.word2, d.inclusive, d.color);
                    }
                    else if (d.style == priv::_LogDisplayStyle::CUSTOM) {
                        line_custom_cb(row, line, d.word1, d.word2, d.color, d.inclusive, d.start, d.stop, d.count);
                    }
                    else if (d.style == priv::_LogDisplayStyle::LINE) {
                        style_line(d.start, d.stop, d.color);
                    }
                    else if (d.style == priv::_LogDisplayStyle::LOCK) {
                        if (d.on == true) {
                            lock_colors();
                        }
                        else {
                            unlock_colors();
                        }
                    }
                    else if (d.style == priv::_LogDisplayStyle::NUM) {
                        style_num(line, d.color, d.count);
                    }
                    else if (d.style == priv::_LogDisplayStyle::RANGE) {
                        style_range(line, d.word1, d.word2, d.inclusive, d.color, d.count);
                    }
                    else if (d.style == priv::_LogDisplayStyle::RSTRING) {
                        style_rstring(line, d.word1, d.color, d.count);
                    }
                    else if (d.style == priv::_LogDisplayStyle::STRING) {
                        style_string(line, d.word1, d.color, d.count);
                    }
                }
            }

            _tmp->pos += _tmp->line + 1;
            row += 1;
            free(line);
        }

        _style->text(_tmp->buf);
        highlight_data(_style, priv::_LOGDISPLAY_STYLE_TABLE, sizeof(priv::_LOGDISPLAY_STYLE_TABLE) / sizeof(priv::_LOGDISPLAY_STYLE_TABLE[0]), static_cast<char>(Color::FOREGROUND), nullptr, 0);
    }

    delete _tmp;
    _tmp = nullptr;
}

/** @brief Style text between two words.
*
* @param[in] line       Line of text.
* @param[in] word1      Start word.
* @param[in] word2      Stop word.
* @param[in] inclusive  True to use from start to stop, false from from + 1 to stop - 1.
* @param[in] color      Color to use.
*/
void flw::LogDisplay::style_between(const std::string& line, const std::string& word1, const std::string& word2, bool inclusive, LogDisplay::Color color) {
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

/** @brief Get character.
*
* @param[in] pos  Valid index. @return Color code or 0 if index is out of range.
*/
char flw::LogDisplay::_style_char(size_t pos) const {
    pos += _tmp->pos;
    return (pos < _tmp->size) ? _tmp->buf[pos] : 0;
}

/** @brief Set color between two indexes.
*
* Start and stop are positions in current line.
*
* @param[in] start  Start index.
* @param[in] stop   Stop index.
* @param[in] color  Color value.
*/
void flw::LogDisplay::style_line(size_t start, size_t stop, LogDisplay::Color color) {
    assert(_tmp);

    start += _tmp->pos;
    stop  += _tmp->pos;

    while (start <= stop && start < _tmp->size && start < _tmp->pos + _tmp->line) {
        if (_lock_colors == false || _tmp->buf[start] == static_cast<char>(Color::FOREGROUND)) {
            _tmp->buf[start] = static_cast<char>(color);
        }

        start++;
    }
}

/** @brief Color all numbers.
*
* @param[in] line   Text line.
* @param[in] color  Color value.
* @param[in] count  Max numbers of stylings.
*/
void flw::LogDisplay::style_num(const std::string& line, LogDisplay::Color color, size_t count) {
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

/** @brief Color text between from and to string.
*
* @param[in] line       Text line.
* @param[in] word1      From string.
* @param[in] word2      To string.
* @param[in] inclusive  True to color inclusive found.
* @param[in] color      Color value.
* @param[in] count      Max number of stylings.
*/
void flw::LogDisplay::style_range(const std::string& line, const std::string& word1, const std::string& word2, bool inclusive, LogDisplay::Color color, size_t count) {
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

/** @brief Color word, starting from right.
*
* @param[in] line   Text line.
* @param[in] word1  From string.
* @param[in] color  Color value.
* @param[in] count  Max number of stylings.
*/
void flw::LogDisplay::style_rstring(const std::string& line, const std::string& word1, LogDisplay::Color color, size_t count) {
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

/** @brief Color word, starting from left.
*
* @param[in] line   Text line.
* @param[in] word1  From string.
* @param[in] color  Color value.
* @param[in] count  Max number of stylings.
*/
void flw::LogDisplay::style_string(const std::string& line, const std::string& word1, LogDisplay::Color color, size_t count) {
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

/** @brief Updpate font preferences using flw::PREF_FIXED_FONT.
*
*/
void flw::LogDisplay::update_pref() {
    labelsize(flw::PREF_FIXED_FONTSIZE);
    linenumber_bgcolor(FL_BACKGROUND_COLOR);
    linenumber_fgcolor(FL_FOREGROUND_COLOR);
    linenumber_font(flw::PREF_FIXED_FONT);
    linenumber_size(flw::PREF_FIXED_FONTSIZE);
    linenumber_width(flw::PREF_FIXED_FONTSIZE * 3);
    textfont(flw::PREF_FIXED_FONT);
    textsize(flw::PREF_FIXED_FONTSIZE);

    for (size_t f = 0; f < sizeof(priv::_LOGDISPLAY_STYLE_TABLE) / sizeof(priv::_LOGDISPLAY_STYLE_TABLE[0]); f++) {
        priv::_LOGDISPLAY_STYLE_TABLE[f].size = flw::PREF_FIXED_FONTSIZE;

        if (theme::is_dark() == true) {
            priv::_LOGDISPLAY_STYLE_TABLE[f].bgcolor = FL_WHITE;
        }
        else {
            priv::_LOGDISPLAY_STYLE_TABLE[f].bgcolor = fl_lighter(FL_GRAY);
        }
    }
}

/** @brief Set text.
*
* @param[in] text  Text to show.
*/
void flw::LogDisplay::value(const char* text) {
    _buffer->text("");
    _style->text("");

    auto win = priv::_logdisplay_win_to_unix(text);

    if (win != nullptr) {
        _buffer->text(win);
        free(win);
    }
    else {
        _buffer->text(text);
    }
}

// MKALGAM_OFF
