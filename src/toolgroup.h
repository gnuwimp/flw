/**
* @file
* @brief Toolbar group widget.
*
* @author gnuwimp@gmail.com
* @copyright Released under the GNU General Public License v3.0
*/

#ifndef FLW_TOOLGROUP_H
#define FLW_TOOLGROUP_H

#include "flw.h"

// MKALGAM_ON

#include <FL/Fl_Group.H>

namespace flw {

/*
 *      _______          _  _____
 *     |__   __|        | |/ ____|
 *        | | ___   ___ | | |  __ _ __ ___  _   _ _ __
 *        | |/ _ \ / _ \| | | |_ | '__/ _ \| | | | '_ \
 *        | | (_) | (_) | | |__| | | | (_) | |_| | |_) |
 *        |_|\___/ \___/|_|\_____|_|  \___/ \__,_| .__/
 *                                               | |
 *                                               |_|
 */

/** @brief Toolbar group widget.
*
* Default layout is Pos::HORIZONTAL.\n
* Expand last widget is set to off.\n
*\n
* All child widgets have same height or width depending on layout.\n
* Use 0 for size to have equal width/height.\n
*\n
*\n
* Do a resize after adding widgets by calling do_layout().\n
*
* @snippet toolgroup.cpp flw::ToolGroup example
* @image html toolgroup.png
*/
class ToolGroup : public Fl_Group {
public:
    /** @brief Layout options.
    *
    */
    enum class Pos {
                                HORIZONTAL,     ///< @brief Layout widgets in horizontal direction.
                                VERTICAL,       ///< @brief Layout widgets in vertical direction.
    };

                                ToolGroup(int X = 0, int Y = 0, int W = 0, int H = 0, const char* l = nullptr);
                                ~ToolGroup();
    Fl_Widget*                  add(Fl_Widget* widget, unsigned size = 0);
    void                        clear();
    void                        do_layout()
                                    { resize(x(), y(), w(), h()); Fl::redraw(); }
    bool                        expand_last() const
                                    { return _expand; } ///< @brief True for expanding last widget.
    void                        expand_last(bool value)
                                    { _expand = value; do_layout(); } ///< @brief Set expand last widget. @param[in] value  True to expand, false to have last widget the same size as all other.
    Pos                         pos() const
                                    { return _pos; } ///< @brief Get layout value.
    void                        pos(Pos pos)
                                    { _pos = pos; do_layout(); } ///< @brief Set layout value.
    Fl_Widget*                  remove(Fl_Widget* widget);
    Fl_Widget*                  replace(Fl_Widget* old_widget, Fl_Widget* new_widget);
    void                        resize(int X, int Y, int W, int H) override;
    void                        size(Fl_Widget* widget, unsigned size);
    void                        size(unsigned size);

private:
    Pos                         _pos;       ///< @brief Layout option.
    VoidVector                  _widgets;   ///< @brief Vector of _ToolGroupChild objects.
    bool                        _expand;    ///< @brief Expand last widget option.
};

}

// MKALGAM_OFF

#endif
