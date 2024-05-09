// Copyright gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#ifndef FLW_TOOLGROUP_H
#define FLW_TOOLGROUP_H

#include "flw.h"

// MKALGAM_ON

#include <FL/Fl_Group.H>

namespace flw {

/***
 *      _______          _  _____                       
 *     |__   __|        | |/ ____|                      
 *        | | ___   ___ | | |  __ _ __ ___  _   _ _ __  
 *        | |/ _ \ / _ \| | | |_ | '__/ _ \| | | | '_ \ 
 *        | | (_) | (_) | | |__| | | | (_) | |_| | |_) |
 *        |_|\___/ \___/|_|\_____|_|  \___/ \__,_| .__/ 
 *                                               | |    
 *                                               |_|    
 */
 
//------------------------------------------------------------------------------
class ToolGroup : public Fl_Group {
public:
    enum class DIRECTION {
                                HORIZONTAL,
                                VERTICAL,
    };

    explicit                    ToolGroup(DIRECTION direction = DIRECTION::HORIZONTAL, int X = 0, int Y = 0, int W = 0, int H = 0, const char* l = nullptr);
                                ~ToolGroup();
    void                        add(Fl_Widget* widget, int SIZE = 0);
    void                        clear();
    DIRECTION                   direction() const
                                    { return _direction; }
    void                        direction(DIRECTION direction)
                                    { _direction = direction; }
    void                        do_layout()
                                    { resize(x(), y(), w(), h()); Fl::redraw(); }
    bool                        expand_last() const
                                    { return _expand; }
    void                        expand_last(bool value)
                                    { _expand = value; }
    Fl_Widget*                  remove(Fl_Widget* widget);
    void                        resize(int X, int Y, int W, int H) override;
    void                        resize(Fl_Widget* widget, int SIZE = 0);

private:
    DIRECTION                   _direction;
    VoidVector                  _widgets;
    bool                        _expand;
};

}

// MKALGAM_OFF

#endif
