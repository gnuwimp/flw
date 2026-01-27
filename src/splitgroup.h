/**
* @file
* @brief Split group widget for 2 child widgets that can be resized with mouse.
*
* @author gnuwimp@gmail.com
* @copyright Released under the GNU General Public License v3.0
*/

#ifndef FLW_SPLITGROUP_H
#define FLW_SPLITGROUP_H

#include "flw.h"

// MKALGAM_ON

#include <FL/Fl_Group.H>

namespace flw {

/** @brief A split view widget for two child widgets.
*
* The widget sizes can be changed by dragging the border between the widgets.\n
* By default it places widgets in Pos::VERTICAL direction (child widgets get full height).\n
* If a widget already exist at specific Pos it will be deleted and replaced.\n
*
* @snippet splitgroup.cpp flw::SplitGroup example
* @image html splitgroup.png
*/
class SplitGroup : public Fl_Group {
public:

    /** @brief Layout options.
    *
    */
    enum class Pos {
                                HORIZONTAL,  ///< @brief Resize child widgets horizontal (all width).
                                VERTICAL,    ///< @brief Resize child widgets vertical (all height).
    };

    explicit                    SplitGroup(int X = 0, int Y = 0, int W = 0, int H = 0, const char* l = nullptr);
    void                        add(Fl_Widget* widget, bool first);
    Fl_Widget*                  child(bool first)
                                    { return _widgets[first]; } ///< @brief Get child widget. @param[in] first  True to get top or left widget.
    void                        clear();
    void                        do_layout()
                                    { SplitGroup::resize(x(), y(), w(), h()); Fl::redraw(); } ///< @brief Resize all children.
    int                         handle(int event) override;
    void                        hide_child(bool first);
    int                         min_split_pos() const
                                    { return _min_split_pos; } ///< Get min split size (width or height depending on layout).
    void                        min_split_pos(unsigned min_size)
                                    { _min_split_pos = min_size; } ///< Set min child size. @param[in] min_size  Number of pixels.
    Pos                         pos() const
                                    { return _pos; } ///< @brief Get layout value.
    void                        pos(Pos pos);
    void                        resize(int X, int Y, int W, int H) override;
    int                         split_pos() const
                                    { return _split_pos; } ///< Get split pos.
    void                        split_pos(int split_pos)
                                    { if (split_pos > 0 || split_pos == -1) _split_pos = split_pos; } ///< Set split pos. @param[in] split_pos  Position in pixels (>= 0 or -1 to set split in the middle), call do_layout() or resize() to update child widgets.
    void                        show_child(bool first);
    void                        swap()
                                    { auto tmp = _widgets[0]; _widgets[0] = _widgets[1]; _widgets[1] = tmp; do_layout(); } ///< Swap child widgets.

private:

    Pos                         _pos;               ///< @brief Layout.
    Fl_Widget*                  _widgets[2];        ///< @brief Child widgets, [0] is the right/bottom and [1] is the left/top.
    bool                        _drag;              ///< @brief Drag event.
    int                         _min_split_pos;     ///< @brief Min split pos.
    int                         _split_pos;         ///< @brief Current split pos.
};

}

// MKALGAM_OFF

#endif
