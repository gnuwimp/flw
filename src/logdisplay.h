// Copyright 2019 - 2021 gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#ifndef FLW_LOGDISPLAY_H
#define FLW_LOGDISPLAY_H

#include <FL/Fl_Text_Display.H>

// MKALGAM_ON

namespace flw {
    //--------------------------------------------------------------------------
    // A static text display widget with line based text coloring
    // Implement line() method which will be called for every line when you set text with value()
    // LogDisplay will manage text and style buffers
    //
    class LogDisplay : public Fl_Text_Display {
    public:
        enum class COLOR {
                                        FOREGROUND             = 'A',
                                        GRAY                   = 'B',
                                        RED                    = 'C',
                                        GREEN                  = 'D',
                                        BLUE                   = 'E',
                                        DARK_RED               = 'F',
                                        DARK_GREEN             = 'G',
                                        DARK_BLUE              = 'H',
                                        ORANGE                 = 'I',
                                        MAGENTA                = 'J',
                                        YELLOW                 = 'K',
                                        CYAN                   = 'L',
                                        DARK_MAGENTA           = 'M',
                                        DARK_YELLOW            = 'N',
                                        DARK_CYAN              = 'O',
                                        BLACK                  = 'P',
                                        WHITE                  = 'Q',
                                        BOLD_FOREGROUND        = 'R',
                                        BOLD_GRAY              = 'S',
                                        BOLD_RED               = 'T',
                                        BOLD_GREEN             = 'U',
                                        BOLD_BLUE              = 'V',
                                        BOLD_DARK_RED          = 'W',
                                        BOLD_DARK_GREEN        = 'X',
                                        BOLD_DARK_BLUE         = 'Y',
                                        BOLD_ORANGE            = 'Z',
                                        BOLD_MAGENTA           = '[',
                                        BOLD_YELLOW            = '\\',
                                        BOLD_CYAN              = ']',
                                        BOLD_DARK_MAGENTA      = '`',
                                        BOLD_DARK_YELLOW       = '^',
                                        BOLD_DARK_CYAN         = '_',
                                        BOLD_BLACK             = 'a',
                                        BOLD_WHITE             = 'b',
                                        ITALIC_FOREGROUND      = 'c',
                                        ITALIC_RED             = 'd',
                                        ITALIC_GREEN           = 'e',
                                        ITALIC_BLUE            = 'f',
                                        ITALIC_ORANGE          = 'g',
                                        ITALIC_MAGENTA         = 'h',
                                        ITALIC_YELLOW          = 'i',
                                        ITALIC_CYAN            = 'j',
                                        BOLD_ITALIC_FOREGROUND = 'k',
                                        BOLD_ITALIC_RED        = 'l',
                                        BOLD_ITALIC_GREEN      = 'm',
                                        BOLD_ITALIC_BLUE       = 'n',
                                        BOLD_ITALIC_ORANGE     = 'o',
                                        BOLD_ITALIC_MAGENTA    = 'p',
                                        BOLD_ITALIC_YELLOW     = 'q',
                                        BOLD_ITALIC_CYAN       = 'r',
                                        LAST                   = 'r',
        };

                                        LogDisplay(int X = 0, int Y = 0, int W = 0, int H = 0, const char* l = nullptr);
                                        ~LogDisplay();
        void                            find(const char* text, bool select);
        virtual void                    line(int count, const char* line, int len);
        void                            style_size(Fl_Fontsize textsize);
        void                            value(const char* text);

    protected:
        void                            color_word(const char* string, const char* word, LogDisplay::COLOR color);
        int                             str_index(const char* string, const char* find1, int* index1, const char* find2 = nullptr, int* index2 = nullptr, const char* find3 = nullptr, int* index3 = nullptr);
        void                            style(int start, int stop, LogDisplay::COLOR c);

    private:
        char*                           _tmp;
        int                             _tmp_pos;
        int                             _tmp_size;
        Fl_Text_Buffer*                 _buffer;
        Fl_Text_Buffer*                 _style;
    };
}

// MKALGAM_OFF

#endif
