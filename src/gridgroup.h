// Copyright gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#ifndef FLW_GRIDGROUP_H
#define FLW_GRIDGROUP_H

#include "flw.h"

// MKALGAM_ON

#include <FL/Fl_Group.H>

namespace flw {

/***
 *       _____      _     _  _____
 *      / ____|    (_)   | |/ ____|
 *     | |  __ _ __ _  __| | |  __ _ __ ___  _   _ _ __
 *     | | |_ | '__| |/ _` | | |_ | '__/ _ \| | | | '_ \
 *     | |__| | |  | | (_| | |__| | | | (_) | |_| | |_) |
 *      \_____|_|  |_|\__,_|\_____|_|  \___/ \__,_| .__/
 *                                                | |
 *                                                |_|
 */

//------------------------------------------------------------------------------
// A simple layout widget that uses a grid for placing widgets.
// One cell is set to flw::PREF_FONTSIZE / 2.
// Override by calling size() method (min 4, max 72 pixels).
//
class GridGroup : public Fl_Group {
public:
    explicit                    GridGroup(int X = 0, int Y = 0, int W = 0, int H = 0, const char* l = nullptr);
                                ~GridGroup();
    void                        add(Fl_Widget* widget, int X, int Y, int W, int H);
    void                        adjust(Fl_Widget* widget, int L = 0, int R = 0, int T = 0, int B = 0);
    void                        clear();
    void                        do_layout()
                                    { resize(x(), y(), w(), h()); Fl::redraw(); }
    int                         handle(int event) override;
    Fl_Widget*                  remove(Fl_Widget* widget);
    void                        resize(int X, int Y, int W, int H) override;
    void                        resize(Fl_Widget* widget, int X, int Y, int W, int H);
    int                         size() const
                                    { return _size; }
    void                        size(int size)
                                    { _size = (size >= 4 && size <= 72) ? size : 0; }

private:
    void                        _last_active_widget(Fl_Widget** first, Fl_Widget** last);

    VoidVector                  _widgets;
    int                         _size;
};

} // flw

// MKALGAM_OFF

#endif
