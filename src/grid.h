// Copyright gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#ifndef FLW_GRID_H
#define FLW_GRID_H

#include "tableeditor.h"
#include <string>
#include <map>

// MKALGAM_ON

namespace flw {
    typedef std::map<std::string, std::string> StringMap;

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
        StringVector                    cell_choice(int row, int col) override;
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
        int                             _get_int(StringMap& map, int row, int col, int def = 0);
        const char*                     _get_key(int row, int col);
        const char*                     _get_string(StringMap& map, int row, int col, const char* def = "");
        void                            _set_int(StringMap& map, int row, int col, int value);
        void                            _set_string(StringMap& map, int row, int col, const char* value);

        char*                           _buffer;
        StringMap                       _cell_align;
        StringMap                       _cell_choice;
        StringVector                    _cell_choices;
        StringMap                       _cell_color;
        StringMap                       _cell_edit;
        StringMap                       _cell_format;
        StringMap                       _cell_rend;
        StringMap                       _cell_textcolor;
        StringMap                       _cell_value;
        StringMap                       _cell_width;
        char                            _key[100];
    };
}

// MKALGAM_OFF

#endif
