// Copyright gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#ifndef FLW_LOGDISPLAY_H
#define FLW_LOGDISPLAY_H

// MKALGAM_ON

#include <FL/Fl_Text_Display.H>
#include <string>

namespace flw {

//--------------------------------------------------------------------------
// A static text display widget with line based text coloring
// LogDisplay will manage text and style buffers
// Search text with ctrl + 'f', F3, shift + F3
//
// Implement line_cb() method which will be called for every line
// If you are doing more then one color call per line,
//   you can disable changing colors that has already been set
//   by calling lock_color(true)
//
// If json string is used then line_cb() doesn't have to be overidden
// But it is less flexible
// Edit json string in the widget with ctrl + 'e'
//
// BG colors only work in fltk 1.4
//
class LogDisplay : public Fl_Text_Display {
public:
    enum COLOR {
                                FOREGROUND          = 'A',
                                GRAY                = 'B',
                                RED                 = 'C',
                                GREEN               = 'D',
                                BLUE                = 'E',
                                MAGENTA             = 'F',
                                YELLOW              = 'G',
                                CYAN                = 'H',
                                BOLD_FOREGROUND     = 'I',
                                BOLD_GRAY           = 'J',
                                BOLD_RED            = 'K',
                                BOLD_GREEN          = 'L',
                                BOLD_BLUE           = 'M',
                                BOLD_MAGENTA        = 'N',
                                BOLD_YELLOW         = 'O',
                                BOLD_CYAN           = 'P',
                                BG_FOREGROUND       = 'Q',
                                BG_GRAY             = 'R',
                                BG_RED              = 'S',
                                BG_GREEN            = 'T',
                                BG_BLUE             = 'U',
                                BG_MAGENTA          = 'V',
                                BG_YELLOW           = 'W',
                                BG_CYAN             = 'X',
                                BG_BOLD_FOREGROUND  = 'Y',
                                BG_BOLD_GRAY        = 'Z',
                                BG_BOLD_RED         = '[',
                                BG_BOLD_GREEN       = '\\',
                                BG_BOLD_BLUE        = ']',
                                BG_BOLD_MAGENTA     = '^',
                                BG_BOLD_YELLOW      = '_',
                                BG_BOLD_CYAN        = '`',
                                LAST                = '`',
    };
                                LogDisplay(int X = 0, int Y = 0, int W = 0, int H = 0, const char* l = nullptr);
                                ~LogDisplay();
    void                        edit_styles();
    void                        find(bool next, bool force_ask);
    int                         handle(int event) override;
    void                        lock_colors(bool lock)
                                    { _lock_colors = lock; }
    void                        save_file();
    void                        style(std::string json = "");
    void                        update_pref();
    void                        value(const char* text);

protected:
    virtual void                line_cb(size_t row, const std::string& line)
                                    { (void) row; (void) line; }
    virtual void                line_custom_cb(size_t row, const std::string& line, const std::string& word1, const std::string& word2, LogDisplay::COLOR color, bool inclusive, size_t start = 0, size_t stop = 0, size_t count = 0)
                                    { (void) row; (void) line; (void) word1; (void) word2; (void) color;  (void) inclusive;  (void) start;  (void) stop;  (void) count; }
    void                        style_between(const std::string& line, const std::string& word1, const std::string& word2, bool inclusive, LogDisplay::COLOR color);
    void                        style_line(size_t start, size_t stop, LogDisplay::COLOR c);
    void                        style_num(const std::string& line, LogDisplay::COLOR color, size_t count = 0);
    void                        style_range(const std::string& line, const std::string& word1, const std::string& word2, bool inclusive, LogDisplay::COLOR color, size_t count = 0);
    void                        style_rstring(const std::string& line, const std::string& word1, LogDisplay::COLOR color, size_t count = 0);
    void                        style_string(const std::string& line, const std::string& word1, LogDisplay::COLOR color, size_t count = 0);

private:
    const char*                 _check_text(const char* text);
    char                        _style_char(size_t pos) const
                                    { pos += _tmp->pos; return (pos < _tmp->size) ? _tmp->buf[pos] : 0; }

    struct Tmp {
        char*                   buf;
        size_t                  len;
        size_t                  pos;
        size_t                  size;

                                Tmp();
                                ~Tmp();
    };

    Fl_Text_Buffer*             _buffer;
    std::string                 _find;
    bool                        _lock_colors;
    std::string                 _json;
    Fl_Text_Buffer*             _style;
    Tmp*                        _tmp;
};

}

// MKALGAM_OFF

#endif
