// Copyright 2019 - 2021 gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#include "logdisplay.h"
#include "util.h"
#include "theme.h"
#include <string.h>

// MKALGAM_ON

namespace flw {
#if FL_MINOR_VERSION == 4
    Fl_Text_Display::Style_Table_Entry _LOGDISPLAY_STYLE[] = {
        { FL_FOREGROUND_COLOR,          FL_COURIER,             14, 0, 0 }, // FOREGROUND
        { fl_rgb_color(115, 115, 115),  FL_COURIER,             14, 0, 0 }, // GRAY
        { fl_rgb_color(255, 64, 64),    FL_COURIER,             14, 0, 0 }, // RED
        { fl_rgb_color(0, 230, 0),      FL_COURIER,             14, 0, 0 }, // GREEN
        { fl_rgb_color(0, 168, 255),    FL_COURIER,             14, 0, 0 }, // BLUE
        { fl_rgb_color(192, 0, 0),      FL_COURIER,             14, 0, 0 }, // DARK_RED
        { fl_rgb_color(0, 128, 0),      FL_COURIER,             14, 0, 0 }, // DARK_GREEN
        { fl_rgb_color(0, 0, 255),      FL_COURIER,             14, 0, 0 }, // DARK_BLUE
        { fl_rgb_color(255, 128, 0),    FL_COURIER,             14, 0, 0 }, // ORANGE
        { FL_MAGENTA,                   FL_COURIER,             14, 0, 0 }, // MAGENTA
        { FL_YELLOW,                    FL_COURIER,             14, 0, 0 }, // YELLOW
        { FL_CYAN,                      FL_COURIER,             14, 0, 0 }, // CYAN
        { FL_DARK_MAGENTA,              FL_COURIER,             14, 0, 0 }, // DARK_MAGENTA
        { FL_DARK_YELLOW,               FL_COURIER,             14, 0, 0 }, // DARK_YELLOW
        { FL_DARK_CYAN,                 FL_COURIER,             14, 0, 0 }, // DARK_CYAN
        { FL_BLACK,                     FL_COURIER,             14, 0, 0 }, // BLACK
        { FL_WHITE,                     FL_COURIER,             14, 0, 0 }, // WHITE

        { FL_FOREGROUND_COLOR,          FL_COURIER_BOLD,        14, 0, 0 }, // BOLD_FOREGROUND
        { fl_rgb_color(115, 115, 115),  FL_COURIER_BOLD,        14, 0, 0 }, // BOLD_GRAY
        { fl_rgb_color(255, 64, 64),    FL_COURIER_BOLD,        14, 0, 0 }, // BOLD_RED
        { fl_rgb_color(0, 230, 0),      FL_COURIER_BOLD,        14, 0, 0 }, // BOLD_GREEN
        { fl_rgb_color(0, 168, 255),    FL_COURIER_BOLD,        14, 0, 0 }, // BOLD_BLUE
        { fl_rgb_color(192, 0, 0),      FL_COURIER_BOLD,        14, 0, 0 }, // BOLD_DARK_RED
        { fl_rgb_color(0, 128, 0),      FL_COURIER_BOLD,        14, 0, 0 }, // BOLD_DARK_GREEN
        { fl_rgb_color(0, 0, 255),      FL_COURIER_BOLD,        14, 0, 0 }, // BOLD_DARK_BLUE
        { fl_rgb_color(255, 128, 0),    FL_COURIER_BOLD,        14, 0, 0 }, // BOLD_ORANGE
        { FL_MAGENTA,                   FL_COURIER_BOLD,        14, 0, 0 }, // BOLD_MAGENTA
        { FL_YELLOW,                    FL_COURIER_BOLD,        14, 0, 0 }, // BOLD_YELLOW
        { FL_CYAN,                      FL_COURIER_BOLD,        14, 0, 0 }, // BOLD_CYAN
        { FL_DARK_MAGENTA,              FL_COURIER_BOLD,        14, 0, 0 }, // BOLD_DARK_MAGENTA
        { FL_DARK_YELLOW,               FL_COURIER_BOLD,        14, 0, 0 }, // BOLD_DARK_YELLOW
        { FL_DARK_CYAN,                 FL_COURIER_BOLD,        14, 0, 0 }, // BOLD_DARK_CYAN
        { FL_BLACK,                     FL_COURIER_BOLD,        14, 0, 0 }, // BOLD_BLACK
        { FL_WHITE,                     FL_COURIER_BOLD,        14, 0, 0 }, // BOLD_WHITE

        { FL_FOREGROUND_COLOR,          FL_COURIER_ITALIC,      14, 0, 0 }, // ITALIC_FOREGROUND
        { fl_rgb_color(255, 64, 64),    FL_COURIER_ITALIC,      14, 0, 0 }, // ITALIC_RED
        { fl_rgb_color(0, 230, 0),      FL_COURIER_ITALIC,      14, 0, 0 }, // ITALIC_GREEN
        { fl_rgb_color(0, 168, 255),    FL_COURIER_ITALIC,      14, 0, 0 }, // ITALIC_BLUE
        { fl_rgb_color(255, 128, 0),    FL_COURIER_ITALIC,      14, 0, 0 }, // ITALIC_ORANGE
        { FL_MAGENTA,                   FL_COURIER_ITALIC,      14, 0, 0 }, // ITALIC_MAGENTA
        { FL_YELLOW,                    FL_COURIER_ITALIC,      14, 0, 0 }, // ITALIC_YELLOW
        { FL_CYAN,                      FL_COURIER_ITALIC,      14, 0, 0 }, // ITALIC_CYAN

        { FL_FOREGROUND_COLOR,          FL_COURIER_BOLD_ITALIC, 14, 0, 0 }, // BOLD_ITALIC_FOREGROUND
        { fl_rgb_color(255, 64, 64),    FL_COURIER_BOLD_ITALIC, 14, 0, 0 }, // BOLD_ITALIC_RED
        { fl_rgb_color(0, 230, 0),      FL_COURIER_BOLD_ITALIC, 14, 0, 0 }, // BOLD_ITALIC_GREEN
        { fl_rgb_color(0, 168, 255),    FL_COURIER_BOLD_ITALIC, 14, 0, 0 }, // BOLD_ITALIC_BLUE
        { fl_rgb_color(255, 128, 0),    FL_COURIER_BOLD_ITALIC, 14, 0, 0 }, // BOLD_ITALIC_ORANGE
        { FL_MAGENTA,                   FL_COURIER_BOLD_ITALIC, 14, 0, 0 }, // BOLD_ITALIC_MAGENTA
        { FL_YELLOW,                    FL_COURIER_BOLD_ITALIC, 14, 0, 0 }, // BOLD_ITALIC_YELLOW
        { FL_CYAN,                      FL_COURIER_BOLD_ITALIC, 14, 0, 0 }, // BOLD_ITALIC_CYAN
    };
#else
    Fl_Text_Display::Style_Table_Entry _LOGDISPLAY_STYLE[] = {
        { FL_FOREGROUND_COLOR,          FL_COURIER,             14, 0 }, // FOREGROUND
        { fl_rgb_color(115, 115, 115),  FL_COURIER,             14, 0 }, // GRAY
        { fl_rgb_color(255, 64, 64),    FL_COURIER,             14, 0 }, // RED
        { fl_rgb_color(0, 230, 0),      FL_COURIER,             14, 0 }, // GREEN
        { fl_rgb_color(0, 168, 255),    FL_COURIER,             14, 0 }, // BLUE
        { fl_rgb_color(192, 0, 0),      FL_COURIER,             14, 0 }, // DARK_RED
        { fl_rgb_color(0, 128, 0),      FL_COURIER,             14, 0 }, // DARK_GREEN
        { fl_rgb_color(0, 0, 255),      FL_COURIER,             14, 0 }, // DARK_BLUE
        { fl_rgb_color(255, 128, 0),    FL_COURIER,             14, 0 }, // ORANGE
        { FL_MAGENTA,                   FL_COURIER,             14, 0 }, // MAGENTA
        { FL_YELLOW,                    FL_COURIER,             14, 0 }, // YELLOW
        { FL_CYAN,                      FL_COURIER,             14, 0 }, // CYAN
        { FL_DARK_MAGENTA,              FL_COURIER,             14, 0 }, // DARK_MAGENTA
        { FL_DARK_YELLOW,               FL_COURIER,             14, 0 }, // DARK_YELLOW
        { FL_DARK_CYAN,                 FL_COURIER,             14, 0 }, // DARK_CYAN
        { FL_BLACK,                     FL_COURIER,             14, 0 }, // BLACK
        { FL_WHITE,                     FL_COURIER,             14, 0 }, // WHITE

        { FL_FOREGROUND_COLOR,          FL_COURIER_BOLD,        14, 0 }, // BOLD_FOREGROUND
        { fl_rgb_color(115, 115, 115),  FL_COURIER_BOLD,        14, 0 }, // BOLD_GRAY
        { fl_rgb_color(255, 64, 64),    FL_COURIER_BOLD,        14, 0 }, // BOLD_RED
        { fl_rgb_color(0, 230, 0),      FL_COURIER_BOLD,        14, 0 }, // BOLD_GREEN
        { fl_rgb_color(0, 168, 255),    FL_COURIER_BOLD,        14, 0 }, // BOLD_BLUE
        { fl_rgb_color(192, 0, 0),      FL_COURIER_BOLD,        14, 0 }, // BOLD_DARK_RED
        { fl_rgb_color(0, 128, 0),      FL_COURIER_BOLD,        14, 0 }, // BOLD_DARK_GREEN
        { fl_rgb_color(0, 0, 255),      FL_COURIER_BOLD,        14, 0 }, // BOLD_DARK_BLUE
        { fl_rgb_color(255, 128, 0),    FL_COURIER_BOLD,        14, 0 }, // BOLD_ORANGE
        { FL_MAGENTA,                   FL_COURIER_BOLD,        14, 0 }, // BOLD_MAGENTA
        { FL_YELLOW,                    FL_COURIER_BOLD,        14, 0 }, // BOLD_YELLOW
        { FL_CYAN,                      FL_COURIER_BOLD,        14, 0 }, // BOLD_CYAN
        { FL_DARK_MAGENTA,              FL_COURIER_BOLD,        14, 0 }, // BOLD_DARK_MAGENTA
        { FL_DARK_YELLOW,               FL_COURIER_BOLD,        14, 0 }, // BOLD_DARK_YELLOW
        { FL_DARK_CYAN,                 FL_COURIER_BOLD,        14, 0 }, // BOLD_DARK_CYAN
        { FL_BLACK,                     FL_COURIER_BOLD,        14, 0 }, // BOLD_BLACK
        { FL_WHITE,                     FL_COURIER_BOLD,        14, 0 }, // BOLD_WHITE

        { FL_FOREGROUND_COLOR,          FL_COURIER_ITALIC,      14, 0 }, // ITALIC_FOREGROUND
        { fl_rgb_color(255, 64, 64),    FL_COURIER_ITALIC,      14, 0 }, // ITALIC_RED
        { fl_rgb_color(0, 230, 0),      FL_COURIER_ITALIC,      14, 0 }, // ITALIC_GREEN
        { fl_rgb_color(0, 168, 255),    FL_COURIER_ITALIC,      14, 0 }, // ITALIC_BLUE
        { fl_rgb_color(255, 128, 0),    FL_COURIER_ITALIC,      14, 0 }, // ITALIC_ORANGE
        { FL_MAGENTA,                   FL_COURIER_ITALIC,      14, 0 }, // ITALIC_MAGENTA
        { FL_YELLOW,                    FL_COURIER_ITALIC,      14, 0 }, // ITALIC_YELLOW
        { FL_CYAN,                      FL_COURIER_ITALIC,      14, 0 }, // ITALIC_CYAN

        { FL_FOREGROUND_COLOR,          FL_COURIER_BOLD_ITALIC, 14, 0 }, // BOLD_ITALIC_FOREGROUND
        { fl_rgb_color(255, 64, 64),    FL_COURIER_BOLD_ITALIC, 14, 0 }, // BOLD_ITALIC_RED
        { fl_rgb_color(0, 230, 0),      FL_COURIER_BOLD_ITALIC, 14, 0 }, // BOLD_ITALIC_GREEN
        { fl_rgb_color(0, 168, 255),    FL_COURIER_BOLD_ITALIC, 14, 0 }, // BOLD_ITALIC_BLUE
        { fl_rgb_color(255, 128, 0),    FL_COURIER_BOLD_ITALIC, 14, 0 }, // BOLD_ITALIC_ORANGE
        { FL_MAGENTA,                   FL_COURIER_BOLD_ITALIC, 14, 0 }, // BOLD_ITALIC_MAGENTA
        { FL_YELLOW,                    FL_COURIER_BOLD_ITALIC, 14, 0 }, // BOLD_ITALIC_YELLOW
        { FL_CYAN,                      FL_COURIER_BOLD_ITALIC, 14, 0 }, // BOLD_ITALIC_CYAN
    };
#endif
}

//------------------------------------------------------------------------------
flw::LogDisplay::LogDisplay(int x, int y, int w, int h, const char *l) : Fl_Text_Display(x, y, w, h, l) {
    _buffer = new Fl_Text_Buffer();
    _style  = new Fl_Text_Buffer();
    _tmp    = nullptr;

    buffer(_buffer);
    labelsize(flw::PREF_FIXED_FONTSIZE);
    linenumber_align(FL_ALIGN_RIGHT);
    linenumber_bgcolor(FL_BACKGROUND_COLOR);
    linenumber_fgcolor(FL_FOREGROUND_COLOR);
    linenumber_font(PREF_FIXED_FONT);
    linenumber_format("%5d");
    linenumber_size(flw::PREF_FIXED_FONTSIZE);
    linenumber_width(flw::PREF_FIXED_FONTSIZE * 3);
    textfont(flw::PREF_FIXED_FONT);
    textsize(flw::PREF_FIXED_FONTSIZE);
    style_size(flw::PREF_FIXED_FONTSIZE);
}

//------------------------------------------------------------------------------
flw::LogDisplay::~LogDisplay() {
    buffer(nullptr);
    delete _buffer;
    delete _style;
}

//------------------------------------------------------------------------------
void flw::LogDisplay::color_word(const char* string, const char* word, flw::LogDisplay::COLOR color) {
    auto pos   = 0;
    auto index = 0;
    auto len   = strlen(word);

    while (str_index(string + pos, word, &index) == 1) {
        style(index + pos, index + pos + len - 1, color);
        pos += index + len;
    }
}

//------------------------------------------------------------------------------
void flw::LogDisplay::find(const char* find, bool select) {
    auto offset1 = 0;
    auto offset2 = strlen(find);
    auto found   = _buffer->search_forward(0, find, &offset1, 1);

    if (found) {
        auto lines = 0;

        for (int f = 0; f < offset1; f++) {
            if (_buffer->byte_at(f) == 10) {
                lines++;
            }
        }

        offset2 += offset1;
        scroll(lines, 0);

        if (select) {
            _buffer->select(offset1, offset2);
        }
    }
}

//------------------------------------------------------------------------------
void flw::LogDisplay::line(int, const char*, int) {
}

//------------------------------------------------------------------------------
int flw::LogDisplay::str_index(const char* string, const char* find1, int* index1, const char* find2, int* index2, const char* find3, int* index3) {
    if (string == nullptr || find1 == nullptr || *find1 == 0 || index1 == nullptr) {
        return 0;
    }
    else {
        auto found = strstr(string, find1);

        *index1 = found ? (int) (found - string) : -1;

        if (*index1 >= 0 && find2 && *find2 && index2) {
            found   = strstr(string + *index1 + 1, find2);
            *index2 = found ? (int) (found - string) : -1;

            if (*index2 >= 0 && find3 && *find3 && index3) {
                found   = strstr(string + *index2 + 1, find3);
                *index3 = found ? (int) (found - string) : -1;

                return *index3 == -1 ? 2 : 3;
            }
            else {
                return *index2 == -1 ? 1 : 2;
            }
        }
        else {
            return *index1 == -1 ? 0 : 1;
        }
    }
}

//------------------------------------------------------------------------------
void flw::LogDisplay::value(const char* text) {
    auto line  = (char*) nullptr;
    auto end   = (const char*) nullptr;
    auto start = (const char*) nullptr;
    auto count = 0;
    auto len   = 0;
    auto alloc = 100;

    if (text == nullptr || *text == 0) {
        goto EXIT;
    }
    else {
        len       = strlen(text);
        start     = text;
        end       = text;
        line      = (char*) calloc(alloc + 1, 1);
        _tmp      = (char*) calloc(len + 1, 1);
        _tmp_size = len;

        if (_tmp == nullptr || line == nullptr) {
            goto EXIT;
        }
        else {
            memset(_tmp, 'A', len);

            while (*end) {
                if (*end == '\n' || *(end + 1) == 0) {
                    auto line_len = (int) (end - start) + (*end == '\n' ? 0 : 1);
                    _tmp_pos = (int) (start - text);

                    if (line_len > alloc) {
                        alloc = line_len;
                        free(line);
                        line = (char*) calloc(alloc + 1, 1);

                        if (line == nullptr) {
                            goto EXIT;
                        }
                    }

                    count++;

                    if (line_len > 0) {
                        memcpy(line, start, line_len);
                        line[line_len] = 0;
                        this->line(count, line, line_len);
                    }

                    start = end + 1;
                }

                end++;
            }

            _buffer->text(text);
            _style->text(_tmp);
            highlight_data(_style, _LOGDISPLAY_STYLE, sizeof(_LOGDISPLAY_STYLE) / sizeof(_LOGDISPLAY_STYLE[0]), (char) COLOR::FOREGROUND, nullptr, 0);
            scroll(0, 0);
            free(line);
            free(_tmp);
            _tmp = nullptr;
            return;
        }
    }

EXIT:
    _buffer->text("");
    _style->text("");
    highlight_data(_style, _LOGDISPLAY_STYLE, sizeof(_LOGDISPLAY_STYLE) / sizeof(_LOGDISPLAY_STYLE[0]), (char) COLOR::FOREGROUND, nullptr, 0);
    scroll(0, 0);
    free(line);
    free(_tmp);
    _tmp = nullptr;
}

//------------------------------------------------------------------------------
void flw::LogDisplay::style(int start, int stop, flw::LogDisplay::COLOR c) {
    start += _tmp_pos;
    stop  += _tmp_pos;

    while (start <= stop && start < _tmp_size) {
        *(_tmp + start++) = (char) c;
    }
}

//------------------------------------------------------------------------------
void flw::LogDisplay::style_size(Fl_Fontsize textsize) {
    for (size_t f = 0; f < sizeof(_LOGDISPLAY_STYLE) / sizeof(_LOGDISPLAY_STYLE[0]); f++) {
        _LOGDISPLAY_STYLE[f].size = textsize;
    }
}

// MKALGAM_OFF
