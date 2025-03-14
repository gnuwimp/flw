// Copyright gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#ifndef FLW_TABLEEDITOR_H
#define FLW_TABLEEDITOR_H

#include "tabledisplay.h"

// MKALGAM_ON

namespace flw {

/***
 *      _______    _     _      ______    _ _ _
 *     |__   __|  | |   | |    |  ____|  | (_) |
 *        | | __ _| |__ | | ___| |__   __| |_| |_ ___  _ __
 *        | |/ _` | '_ \| |/ _ \  __| / _` | | __/ _ \| '__|
 *        | | (_| | |_) | |  __/ |___| (_| | | || (_) | |
 *        |_|\__,_|_.__/|_|\___|______\__,_|_|\__\___/|_|
 *
 *
 */
//------------------------------------------------------------------------------
// A table editing widget
//
class TableEditor : public TableDisplay {
    using TableDisplay::cell_value;

public:
    enum class FORMAT {
                                DEFAULT,
                                INT_DEF,
                                INT_SEP,
                                DEC_DEF,
                                DEC_0,
                                DEC_1,
                                DEC_2,
                                DEC_3,
                                DEC_4,
                                DEC_5,
                                DEC_6,
                                DEC_7,
                                DEC_8,
                                DATE_DEF,
                                DATE_WORLD,
                                DATE_US,
                                SECRET_DEF,
                                SECRET_DOT,
    };

    enum class REND {
                                TEXT,
                                INTEGER,
                                NUMBER,
                                BOOLEAN,
                                SECRET,
                                CHOICE,
                                INPUT_CHOICE,
                                SLIDER,
                                VALUE_SLIDER,
                                DLG_COLOR,
                                DLG_FILE,
                                DLG_DIR,
                                DLG_DATE,
                                DLG_LIST,
    };
    
    static const char*          SELECT_DATE;
    static const char*          SELECT_DIR;
    static const char*          SELECT_FILE;
    static const char*          SELECT_LIST;

    explicit                    TableEditor(int X = 0, int Y = 0, int W = 0, int H = 0, const char* l = nullptr);
    void                        send_changed_event_always(bool value)
                                    { _send_changed_event_always = value; }
    virtual StringVector        cell_choice(int row, int col)
                                    { (void) row; (void) col; return StringVector(); }
    virtual bool                cell_edit(int row, int col)
                                    { (void) row; (void) col; return false; }
    virtual FORMAT              cell_format(int row, int col)
                                    { (void) row; (void) col; return TableEditor::FORMAT::DEFAULT; }
    virtual REND                cell_rend(int row, int col)
                                    { (void) row; (void) col; return TableEditor::REND::TEXT; }
    virtual bool                cell_value(int row, int col, const char* value)
                                    { (void) row; (void) col; (void) value; return false; }
    void                        cmd_add(int count);
    void                        cmd_cut();
    void                        cmd_delete();
    void                        cmd_paste();
    int                         handle(int event) override;
    void                        reset() override;

    static const char*          FormatSlider(double val, double min, double max, double step);

private:
    bool                        _send_changed_event_always;
    void                        _draw_cell(int row, int col, int X, int Y, int W, int H, bool ver, bool hor, bool current = false) override;
    void                        _edit_create();
    void                        _edit_quick(const char* key);
    void                        _edit_show();
    void                        _edit_start(const char* key = "");
    void                        _edit_stop(bool save = true);
    int                         _ev_keyboard_down2();
    int                         _ev_mouse_click2();
    int                         _ev_paste();

    Fl_Widget*                  _edit2;
    Fl_Widget*                  _edit3;
};

} // flw

// MKALGAM_OFF

#endif
