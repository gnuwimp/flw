// Copyright 2016 - 2021 gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#include "splitgroup.h"
#include <FL/fl_draw.H>

// MALAGMA_ON

//------------------------------------------------------------------------------
flw::SplitGroup::SplitGroup(int X, int Y, int W, int H, const char* l) : Fl_Group(X, Y, W, H, l) {
    end();
    clip_children(1);
    clear();
}

//------------------------------------------------------------------------------
void flw::SplitGroup::add(Fl_Widget* widget, SplitGroup::CHILD child) {
    auto num = child == SplitGroup::CHILD::FIRST ? 0 : 1;

    if (_widgets[num]) {
        remove(_widgets[num]);
        delete _widgets[num];
    }

    _widgets[num] = widget;

    if (widget) {
        Fl_Group::add(widget);
    }
}

//------------------------------------------------------------------------------
void flw::SplitGroup::clear() {
    Fl_Group::clear();

    _drag       = false;
    _min        = 50;
    _split_pos  = -1;
    _direction  = DIRECTION::VERTICAL;
    _widgets[0] = nullptr;
    _widgets[1] = nullptr;
}

//------------------------------------------------------------------------------
// When direction is set the child size will by default be set to equal sizes
// Set new size after if you want to change it
//
void flw::SplitGroup::direction(SplitGroup::DIRECTION direction) {
    _direction = direction;
    _split_pos = -1;

    Fl::redraw();
}

//------------------------------------------------------------------------------
int flw::SplitGroup::handle(int event) {
    if (event == FL_DRAG) {
        if (_drag) {
            if (_direction == SplitGroup::DIRECTION::VERTICAL) {
                _split_pos = Fl::event_x() - x();
            }
            else {
                _split_pos = Fl::event_y() - y();
            }

            resize();
            Fl::flush();
            return 1;
        }
    }
    else if (event == FL_LEAVE) {
        if (_direction == SplitGroup::DIRECTION::VERTICAL) {
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
        if (_widgets[0] && _widgets[1] && _widgets[0]->visible() && _widgets[1]->visible()) {
            if (_direction == SplitGroup::DIRECTION::VERTICAL) {
                auto pos = x() + _split_pos;

                if (Fl::event_x() > (pos - 3) && Fl::event_x() <= (pos + 3)) {
                    if (_drag == false) {
                        _drag = true;
                        fl_cursor(FL_CURSOR_WE);
                    }

                    return 1;
                }
            }
            else {
                auto pos = y() + _split_pos;

                if (Fl::event_y() > (pos - 3) && Fl::event_y() <= (pos + 3)) {
                    if (_drag == false) {
                        _drag = true;
                        fl_cursor(FL_CURSOR_NS);
                    }

                    return 1;
                }
            }
        }

        if (_drag) {
            _drag = false;
            fl_cursor(FL_CURSOR_DEFAULT);
        }
    }
    else if (event == FL_PUSH) {
        if (_drag) {
            return 1;
        }
    }

    return Fl_Group::handle(event);
}

//------------------------------------------------------------------------------
void flw::SplitGroup::resize(int X, int Y, int W, int H) {
    Fl_Widget::resize(X, Y, W, H);
    Fl::redraw();

    auto currx = X;
    auto curry = Y;
    auto currh = H;
    auto currw = W;

    if (_direction == SplitGroup::DIRECTION::VERTICAL) {
        if (currw > 0 && currh > 0) {
            if (_widgets[0] && _widgets[1] && _widgets[0]->visible() && _widgets[1]->visible()) {
                if (_split_pos == -1) {
                    _split_pos = W / 2;
                }
                else if (_split_pos >= W - _min) {
                    _split_pos = W - _min;
                }
                else if (_split_pos <= _min) {
                    _split_pos = _min;
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

                _widgets[0]->resize(currx, curry, w1, currh);
                _widgets[1]->resize(currx + w1 + 4, curry, w2, currh);
            }
            else if (_widgets[0] && _widgets[0]->visible()) {
                _widgets[0]->resize(currx, curry, currw, currh);
            }
            else if (_widgets[1] && _widgets[1]->visible()) {
                _widgets[1]->resize(currx, curry, currw, currh);
            }
        }
    }
    else {
        if (currw > 0 && currh > 0) {
            if (_widgets[0] && _widgets[1] && _widgets[0]->visible() && _widgets[1]->visible()) {
                if (_split_pos == -1) {
                    _split_pos = H / 2;
                }
                else if (_split_pos >= H - _min) {
                    _split_pos = H - _min;
                }
                else if (_split_pos <= _min) {
                    _split_pos = _min;
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

                _widgets[0]->resize(currx, curry, currw, h1);
                _widgets[1]->resize(currx, curry + h1 + 4, currw, h2);
                // fprintf(stderr, "%s %4d - %4d <=> %4d - %4d\n", _widgets[0]->label(), _widgets[0]->x(), _widgets[0]->y(), _widgets[0]->w(), _widgets[0]->h());
                // fprintf(stderr, "%s %4d - %4d <=> %4d - %4d\n", _widgets[1]->label(), _widgets[1]->x(), _widgets[1]->y(), _widgets[1]->w(), _widgets[1]->h());
            }
            else if (_widgets[0] && _widgets[0]->visible()) {
                _widgets[0]->resize(currx, curry, currw, currh);
            }
            else if (_widgets[1] && _widgets[1]->visible()) {
                _widgets[1]->resize(currx, curry, currw, currh);
            }
        }
    }
}


//------------------------------------------------------------------------------
void flw::SplitGroup::toggle(SplitGroup::CHILD child, SplitGroup::DIRECTION direction, int second_size) {
    if (_widgets[0] == nullptr || _widgets[1] == nullptr) {
        return;
    }
    else {
        auto num = child == SplitGroup::CHILD::FIRST ? 0 : 1;

        if (_widgets[num]->visible() && _direction == direction) { // Direction is the same so hide widget
            _widgets[num]->hide();
        }
        else {
            _widgets[num]->show();

            if (_direction != direction || split_pos() == -1) { // Direction has changed or has no size so resize widgets
                _direction = direction;

                if (second_size == -1) {
                    split_pos(-1);
                }
                else if (_direction == SplitGroup::DIRECTION::VERTICAL) {
                    split_pos(w() - second_size);
                }
                else {
                    split_pos(h() - second_size);
                }
            }
        }
    }
}

// MALAGMA_OFF
