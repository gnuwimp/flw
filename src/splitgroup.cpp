/**
* @file
* @brief Split group widget for 2 child widgets that can be resized with mouse.
*
* @author gnuwimp@gmail.com
* @copyright Released under the GNU General Public License v3.0
*/

#include "splitgroup.h"

// MKALGAM_ON

#include <FL/fl_draw.H>

/** @brief Create widget.
*
* @param[in] X  X pos.
* @param[in] Y  Y pos.
* @param[in] W  Width.
* @param[in] H  Height.
* @param[in] l  Optional label.
*/
flw::SplitGroup::SplitGroup(int X, int Y, int W, int H, const char* l) : Fl_Group(X, Y, W, H, l) {
    end();
    clip_children(1);
    resizable(nullptr);
    clear();
}

/** @brief Add child widget.
*
* Existing widget will be deleted.
*
* @param[in] widget  Widget to add.
* @param[in] first   True to add widget to the left/top, false for right/bottom.
*/
void flw::SplitGroup::add(Fl_Widget* widget, bool first) {
    if (_widgets[first] != nullptr) {
        remove(_widgets[first]);
        delete _widgets[first];
    }

    _widgets[first] = widget;

    if (widget != nullptr) {
        Fl_Group::add(widget);
    }
}

/** @brief Delete all children.
*
* And reset internal state to initial values.
*/
void flw::SplitGroup::clear() {
    Fl_Group::clear();

    _drag          = false;
    _min_split_pos = 50;
    _split_pos     = -1;
    _pos           = Pos::VERTICAL;
    _widgets[0]    = nullptr;
    _widgets[1]    = nullptr;
}

/** @brief Take care of mouse drag.
*
*/
int flw::SplitGroup::handle(int event) {
    if (event == FL_DRAG) {
        if (_drag == true) {
            auto pos = 0;

            if (_pos == Pos::VERTICAL) {
                pos = Fl::event_x() - x();
            }
            else {
                pos = Fl::event_y() - y();
            }

            if (pos != _split_pos && pos >= _min_split_pos) {
                _split_pos = pos;
                do_layout();
            }

            return 1;
        }
    }
    else if (event == FL_LEAVE) {
        if (_pos == Pos::VERTICAL) {
            if (Fl::event_y() < y() || Fl::event_y() > y() + h() || Fl::event_x() < x() || Fl::event_x() > x() + w()) {
                _drag = false;
                fl_cursor(FL_CURSOR_DEFAULT);
            }
        }
        else {
            if (Fl::event_y() < y() || Fl::event_y() > y() + h() || Fl::event_x() < x() || Fl::event_x() > x() + w()) {
                _drag = false;
                fl_cursor(FL_CURSOR_DEFAULT);
            }
        }
    }
    else if (event == FL_MOVE) {
        if (_widgets[0] != nullptr && _widgets[1] != nullptr && _widgets[0]->visible() != 0 && _widgets[1]->visible() != 0) {
            if (_pos == Pos::VERTICAL) {
                auto mx  = Fl::event_x();
                auto pos = x() + _split_pos;

                if (mx > (pos - 3) && mx <= (pos + 3)) {
                    if (_drag == false) {
                        _drag = true;
                        fl_cursor(FL_CURSOR_WE);
                    }

                    return 1;
                }
            }
            else {
                auto my  = Fl::event_y();
                auto pos = y() + _split_pos;

                if (my > (pos - 3) && my <= (pos + 3)) {
                    if (_drag == false) {
                        _drag = true;
                        fl_cursor(FL_CURSOR_NS);
                    }

                    return 1;
                }
            }
        }

        if (_drag == true) {
            _drag = false;
            fl_cursor(FL_CURSOR_DEFAULT);
        }
    }
    else if (event == FL_PUSH) {
        if (_drag == true) {
            return 1;
        }
    }

    return Fl_Group::handle(event);
}

/** @brief Hide child widget.
*
* @param[in] first  True to hide left/top widget, false for right/bottom widget.
*/
void flw::SplitGroup::hide_child(bool first) {
    if (_widgets[first] == nullptr) {
        return;
    }

    _widgets[first]->hide();
    do_layout();
}

/** @brief Set new layout.
*
* When pos is changed the child sizes will by default be set to equal sizes.\n
* Set new size after if you want to change it.\n
*
* @param[in] pos  Layout option.
*/
void flw::SplitGroup::pos(Pos pos) {
    _pos       = pos;
    _split_pos = -1;

    do_layout();
    Fl::redraw();
}

/** @brief Resize widgets.
*
*/
void flw::SplitGroup::resize(int X, int Y, int W, int H) {
    Fl_Widget::resize(X, Y, W, H);

    if (W == 0 || H == 0) {
        return;
    }

    auto currx    = X;
    auto curry    = Y;
    auto currh    = H;
    auto currw    = W;

    if (_pos == Pos::VERTICAL) {
        if (_widgets[0] != nullptr && _widgets[1] != nullptr && _widgets[0]->visible() != 0 && _widgets[1]->visible() != 0) {
            if (_split_pos < 0) {
                _split_pos = W / 2;
            }
            else if (_split_pos >= W - _min_split_pos) {
                _split_pos = W - _min_split_pos;
            }
            else if (_split_pos < _min_split_pos) {
                _split_pos = _min_split_pos;
            }

            auto max = (X + W) - 4;
            auto pos = _split_pos + X;

            if (pos < X) {
                pos = X;
            }
            else if (pos > max) {
                pos = max;
            }

            auto w1 = pos - (X + 2);
            auto w2 = (X + W) - (pos + 2);

            _widgets[1]->resize(currx, curry, w1, currh);
            _widgets[0]->resize(currx + w1 + 4, curry, w2, currh);
        }
        else if (_widgets[0] && _widgets[0]->visible()) {
            _widgets[0]->resize(currx, curry, currw, currh);
        }
        else if (_widgets[1] && _widgets[1]->visible()) {
            _widgets[1]->resize(currx, curry, currw, currh);
        }
    }
    else if (_widgets[0] != nullptr && _widgets[1] != nullptr && _widgets[0]->visible() != 0 && _widgets[1]->visible() != 0) { // Pos::HORIZONTAL
        if (_split_pos < 0) {
            _split_pos = H / 2;
        }
        else if (_split_pos >= H - _min_split_pos) {
            _split_pos = H - _min_split_pos;
        }
        else if (_split_pos < _min_split_pos) {
            _split_pos = _min_split_pos;
        }

        auto max = (Y + H) - 4;
        auto pos = _split_pos + Y;

        if (pos < Y) {
            pos = Y;
        }
        else if (pos > max) {
            pos = max;
        }

        auto h1 = pos - (Y + 2);
        auto h2 = (Y + H) - (pos + 2);

        _widgets[1]->resize(currx, curry, currw, h1);
        _widgets[0]->resize(currx, curry + h1 + 4, currw, h2);
    }
    else if (_widgets[0] != nullptr && _widgets[0]->visible() != 0) {
        _widgets[0]->resize(currx, curry, currw, currh);
    }
    else if (_widgets[1] != nullptr && _widgets[1]->visible() != 0) {
        _widgets[1]->resize(currx, curry, currw, currh);
    }
}

/** @brief Show child widget.
*
* @param[in] first  True to show left/top widget, false for right/bottom widget.
*/
void flw::SplitGroup::show_child(bool first) {
    if (_widgets[first] == nullptr) {
        return;
    }

    _widgets[first]->show();
    do_layout();
}

// MKALGAM_OFF
