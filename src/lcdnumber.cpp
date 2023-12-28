// Copyright (C) 2006 z_hossain
// Released under the GNU General Public License v3.0

#include "lcdnumber.h"
#include <FL/fl_draw.H>
#include <string.h>

// MKALGAM_ON

namespace flw {
    static const unsigned char _LCDNUMBER_SEGMENTS[20] = {
        0x00,
        0x7E, //0
        0x30, //1
        0x6D, //2
        0x79, //3
        0x33, //4
        0x5B, //5
        0x5F, //6
        0x70, //7
        0x7F, //8
        0x7B, //9
        0x77, //A
        0x1F, //B
        0x4E, //C
        0x3D, //D
        0x4F, //E
        0x47, //F
        0x01, //-
        0xA0, //.
        0x00, // Repeat the first one.
    };
}

//------------------------------------------------------------------------------
flw::LcdNumber::LcdNumber(int x, int y, int w, int h, const char *l) : Fl_Box(x, y, w, h, l) {
    _align     = FL_ALIGN_LEFT;
    _dot_size  = 6;
    _seg_color = FL_FOREGROUND_COLOR;
    _seg_gap   = 1;
    _thick     = 4;
    _unit_gap  = 4;
    _unit_h    = 42;
    _unit_w    = 21;

    *_value = 0;
    value("0");
    box(FL_NO_BOX);
}

//------------------------------------------------------------------------------
void flw::LcdNumber::draw() {
    Fl_Box::draw();

    int sw   = 0;
    int tx   = x();
    int ty   = y();
    int tw   = w();
    int th   = h();

    for (size_t f = 0; f < strlen(_value); f++) {
        if (_value[f] == 0x12) {
            sw += _dot_size + _unit_gap;
        }
        else {
            sw += _unit_w + _unit_gap;
        }
    }

    switch(_align) {
        case FL_ALIGN_RIGHT:
            tx += (tw - sw) - _unit_w;
            break;

        case FL_ALIGN_CENTER:
            tx += (tw / 2 - sw / 2);
            break;

        default:
            tx += _unit_w;
            break;
    }

    ty += ((th - _unit_h) >> 1);
    fl_color(active_r() ? _seg_color : fl_inactive(_seg_color));
    int xx = tx;

    for(int i = 0; _value[i]; i++) {
        if (_value[i] == 0x12) {
            fl_rectf(xx, ty + _unit_h + 1 - _dot_size, _dot_size, _dot_size);
            xx += (_dot_size + _unit_gap);
        }
        else {
            _draw_seg(_LCDNUMBER_SEGMENTS[(int) _value[i]], xx, ty, _unit_w, _unit_h);
            xx += (_unit_w + _unit_gap);
        }
    }
}

//------------------------------------------------------------------------------
void flw::LcdNumber::_draw_seg(uchar a, int x, int y, int w, int h) {
    int x0, y0, x1, y1, x2, y2, x3, y3;
    int h2     = h >> 1;
    int thick2 = _thick >> 1;

    if (a & 0x40) {
        x0 = x + _seg_gap;
        y0 = y;
        x1 = x0 + _thick;
        y1 = y0 + _thick;
        x2 = x + w - _thick - _seg_gap;
        y2 = y1;
        x3 = x2 + _thick;
        y3 = y0;

        fl_polygon(x0, y0, x1, y1, x2, y2, x3, y3);
    }

    if (a & 0x20) {
        x0 = x + w;
        y0 = y + _seg_gap;
        x1 = x0 - _thick;
        y1 = y0 + _thick;
        x2 = x1;
        y2 = y + h2 - thick2 - _seg_gap;
        x3 = x0;
        y3 = y2 + thick2;

        fl_polygon(x0, y0, x1, y1, x2, y2, x3, y3);
    }

    if (a & 0x10) {
        x0 = x + w;
        y0 = y + h2 + _seg_gap;
        x1 = x0 - _thick;
        y1 = y0 + thick2;
        x2 = x1;
        y2 = y + h - _thick - _seg_gap;
        x3 = x0;
        y3 = y2 + _thick;

        fl_polygon(x0, y0, x1, y1, x2, y2, x3, y3);
    }

    if (a & 0x08) {
        x0 = x + _seg_gap;
        y0 = y + h;
        x1 = x0 + _thick;
        y1 = y0 - _thick;
        x2 = x + w - _thick - _seg_gap;
        y2 = y1;
        x3 = x2 + _thick;
        y3 = y0;

        fl_polygon(x0, y0, x1, y1, x2, y2, x3, y3);
    }

    if (a & 0x04) {
        x0 = x;
        y0 = y + h2 + _seg_gap;
        x1 = x0 + _thick;
        y1 = y0 + thick2;
        x2 = x1;
        y2 = y + h - _thick - _seg_gap;
        x3 = x0;
        y3 = y2 + _thick;

        fl_polygon(x0, y0, x1, y1, x2, y2, x3, y3);
    }

    if (a & 0x02) {
        x0 = x;
        y0 = y + _seg_gap;
        x1 = x0 + _thick;
        y1 = y0 + _thick;
        x2 = x1;
        y2 = y + h2 - thick2 - _seg_gap;
        x3 = x0;
        y3 = y2 + thick2;

        fl_polygon(x0, y0, x1, y1, x2, y2, x3, y3);
    }

    if (a & 0x01) {
        x0 = x + _seg_gap;
        y0 = y + h2;
        x1 = x0 + _thick;
        y1 = y0 - thick2;
        x2 = x1;
        y2 = y1 + _thick;

        fl_polygon(x0, y0, x1, y1, x2, y2);

        x0 = x1;
        y0 = y1;
        x1 = x0 + w - _thick * 2 - _seg_gap * 2;
        y1 = y0;
        x2 = x1;
        y2 = y1 + _thick;
        x3 = x0;
        y3 = y2;

        fl_polygon(x0, y0, x1, y1, x2, y2, x3, y3);

        x0 = x + w - _seg_gap;
        y0 = y + h2;
        x1 = x0 - _thick;
        y1 = y0 - thick2;
        x2 = x1;
        y2 = y1 + _thick;

        fl_polygon(x0, y0, x1, y1, x2, y2);
    }
}

//------------------------------------------------------------------------------
void flw::LcdNumber::value(const char *value) {
    size_t l = value ? strlen(value) : 0;

    if (l && l < 100) {
        for (size_t i = 0; i < l; i++) {
            if (value[i] >= 0x30 && value[i] <= 0x39) {
                _value[i] = value[i] - 0x2F;
            }
            else if (value[i] >= 0x41 && value[i] <= 0x46) {
                _value[i] = value[i] - 0x36;
            }
            else if (value[i] >= 0x61 && value[i] <= 0x66) {
                _value[i] = value[i] - 0x56;
            }
            else if (value[i] == '-') {
                _value[i] = 0x11;
            }
            else if (value[i] == ' ') {
                _value[i] = 0x13;
            }
            else if (value[i] == '.') {
                _value[i] = 0x12;
            }
            else if (value[i] == ':') {
                _value[i] = 0x12;
            }
            else {
                _value[i] = 0x13;
            }

            _value[i + 1] = 0;
        }
    }
    else {
        *_value = 0;
    }

    Fl::redraw();
}

// MKALGAM_OFF
