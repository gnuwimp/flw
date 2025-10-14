/**
* @file
* @brief A static text display that can be used for logging or similar purposes.
*
* @author gnuwimp@gmail.com
* @copyright Released under the GNU General Public License v3.0
*/

#ifndef FLW_LOGDISPLAY_H
#define FLW_LOGDISPLAY_H

// MKALGAM_ON

#include <FL/Fl_Text_Display.H>
#include <string>

namespace flw {

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

/** @brief A static text display widget with line based text coloring.
*
* LogDisplay will manage text and style buffers.\n
*\n
* Either implement line_cb() which will be called for every line.\n
* Or use a json string to set colors.\n
* Using json is less flexible than manage highlightning yourself.\n
*\n
* Press ctrl + 'f' to show find text dialog.\n
* Press F3 to search for next string.\n
* Press shift + F3 to search for previous string.\n
* Press ctrl + 'e' to edit json.\n
* Press ctrl + 's' to save text.\n
* Press ctrl + 'h' to show some help.\n
*
* @snippet logdisplay.cpp flw::LogDisplay example
* @image html logdisplay.png
*/
class LogDisplay : public Fl_Text_Display {
public:
    /** @brief Color values for text.
    *
    * All value starting with U_ has an underline.
    *
    */
    enum class Color {
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
                                U_FOREGROUND        = 'Q',
                                U_GRAY              = 'R',
                                U_RED               = 'S',
                                U_GREEN             = 'T',
                                U_BLUE              = 'U',
                                U_MAGENTA           = 'V',
                                U_YELLOW            = 'W',
                                U_CYAN              = 'X',
                                U_BOLD_FOREGROUND   = 'Y',
                                U_BOLD_GRAY         = 'Z',
                                U_BOLD_RED          = '[',
                                U_BOLD_GREEN        = '\\',
                                U_BOLD_BLUE         = ']',
                                U_BOLD_MAGENTA      = '^',
                                U_BOLD_YELLOW       = '_',
                                U_BOLD_CYAN         = '`',
                                LAST                = U_BOLD_CYAN,
    };

    explicit                    LogDisplay(int X = 0, int Y = 0, int W = 0, int H = 0, const char* l = nullptr);
                                ~LogDisplay();
    void                        edit_styles();
    void                        find(bool next, bool force_ask);
    int                         handle(int event) override;
    void                        lock_colors()
                                    { _lock_colors = true; } ///< @brief All colors that have been set will not change again.
    void                        save_file();
    void                        style(const std::string& json = "");
    void                        unlock_colors()
                                    { _lock_colors = false; } ///< @brief Colors will always change.
    void                        update_pref();
    void                        value(const char* text);
    void                        value(const std::string& text)
                                    { value(text.c_str()); } ///< @brief Text to show.

protected:
    virtual void                line_cb(size_t row, const std::string& line);
    virtual void                line_custom_cb(size_t row, const std::string& line, const std::string& word1, const std::string& word2, LogDisplay::Color color, bool inclusive, size_t start = 0, size_t stop = 0, size_t count = 0);
    void                        style_between(const std::string& line, const std::string& word1, const std::string& word2, bool inclusive, LogDisplay::Color color);
    void                        style_line(size_t start, size_t stop, LogDisplay::Color color);
    void                        style_num(const std::string& line, LogDisplay::Color color, size_t count = 0);
    void                        style_range(const std::string& line, const std::string& word1, const std::string& word2, bool inclusive, LogDisplay::Color color, size_t count = 0);
    void                        style_rstring(const std::string& line, const std::string& word1, LogDisplay::Color color, size_t count = 0);
    void                        style_string(const std::string& line, const std::string& word1, LogDisplay::Color color, size_t count = 0);

private:
    const char*                 _check_text(const char* text);
    char                        _style_char(size_t pos) const
                                    { pos += _tmp->pos; return (pos < _tmp->size) ? _tmp->buf[pos] : 0; } ///< @brief Get character. @param[in] Valid index. @return Color code or 0 if index is out of range.

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
