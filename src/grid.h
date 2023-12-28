// Copyright 2016 - 2021 gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#ifndef FLW_GRID_H
#define FLW_GRID_H

#include "tableeditor.h"
#include <string>
#include <map>

// MKALGAM_ON

namespace flw {
    //--------------------------------------------------------------------------
    // A Table editing widget with built in data storage
    //
    class Grid : public TableEditor {
        friend class                    _TableChoice;

    public:
                                        Grid(int rows, int cols, int X = 0, int Y = 0, int W = 0, int H = 0, const char* l = nullptr);
        virtual                         ~Grid();
        Fl_Align                        cell_align(int row, int col) override;
        void                            cell_align(int row, int col, Fl_Align align);
        std::vector<std::string>        cell_choice(int row, int col) override;
        void                            cell_choice(int row, int col, const char* value);
        Fl_Color                        cell_color(int row, int col) override;
        void                            cell_color(int row, int col, Fl_Color color);
        bool                            cell_edit(int row, int col) override;
        void                            cell_edit(int row, int col, bool value);
        TableEditor::FORMAT             cell_format(int row, int col) override;
        void                            cell_format(int row, int col, TableEditor::FORMAT value);
        TableEditor::REND               cell_rend(int row, int col) override;
        void                            cell_rend(int row, int col, TableEditor::REND rend);
        Fl_Color                        cell_textcolor(int row, int col) override;
        void                            cell_textcolor(int row, int col, Fl_Color color);
        const char*                     cell_value(int row, int col) override;
        bool                            cell_value(int row, int col, const char* value) override;
        void                            cell_value2(int row, int col, const char* format, ...);
        int                             cell_width(int col) override;
        void                            cell_width(int col, int width) override;
        void                            size(int rows, int cols) override;

    private:
        int                             _get_int(std::map<std::string, std::string>& map, int row, int col, int def = 0);
        const char*                     _get_key(int row, int col);
        const char*                     _get_string(std::map<std::string, std::string>& map, int row, int col, const char* def = "");
        void                            _set_int(std::map<std::string, std::string>& map, int row, int col, int value);
        void                            _set_string(std::map<std::string, std::string>& map, int row, int col, const char* value);

        char                                _key[100];
        char*                               _buffer;
        std::map<std::string, std::string>  _align;
        std::map<std::string, std::string>  _cell;
        std::map<std::string, std::string>  _choice;
        std::map<std::string, std::string>  _color;
        std::map<std::string, std::string>  _edit;
        std::map<std::string, std::string>  _format;
        std::map<std::string, std::string>  _rend;
        std::map<std::string, std::string>  _textcolor;
        std::map<std::string, std::string>  _width;
        std::vector<std::string>            _choices;
    };
}

// MKALGAM_OFF

#endif
