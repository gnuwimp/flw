/**
* @file
* @brief A 7 segment widget.
*
* @author z_hossain
* @author gnuwimp@gmail.com
*/

#include "lcddisplay.h"

// MKALGAM_ON

#include <FL/fl_draw.H>

namespace flw {
namespace priv {

static const unsigned char _LCDNUMBER_SEGMENTS[0x29] = {
//  MIDDLE 6      5      4      3      2      1
//  0x01 | 0x02 | 0x04 | 0x08 | 0x10 | 0x20 | 0x40
    0x00,                                           //  0x00
    0x00 | 0x02 | 0x04 | 0x08 | 0x10 | 0x20 | 0x40, //0 0x01
    0x00 | 0x00 | 0x00 | 0x00 | 0x10 | 0x20 | 0x00, //1 0x02
    0x01 | 0x00 | 0x04 | 0x08 | 0x00 | 0x20 | 0x40, //2 0x03
    0x01 | 0x00 | 0x00 | 0x08 | 0x10 | 0x20 | 0x40, //3 0x04
    0x01 | 0x02 | 0x00 | 0x00 | 0x10 | 0x20 | 0x00, //4 0x05
    0x01 | 0x02 | 0x00 | 0x08 | 0x10 | 0x00 | 0x40, //5 0x06
    0x01 | 0x02 | 0x04 | 0x08 | 0x10 | 0x00 | 0x40, //6 0x07
    0x00 | 0x00 | 0x00 | 0x00 | 0x10 | 0x20 | 0x40, //7 0x08
    0x01 | 0x02 | 0x04 | 0x08 | 0x10 | 0x20 | 0x40, //8 0x09
    0x01 | 0x02 | 0x00 | 0x08 | 0x10 | 0x20 | 0x40, //9 0x0a
    0x01 | 0x02 | 0x04 | 0x00 | 0x10 | 0x20 | 0x40, //A 0x0b
    0x01 | 0x02 | 0x04 | 0x08 | 0x10 | 0x00 | 0x00, //B 0x0c
    0x00 | 0x02 | 0x04 | 0x08 | 0x00 | 0x00 | 0x40, //C 0x0d
    0x01 | 0x00 | 0x04 | 0x08 | 0x10 | 0x20 | 0x00, //D 0x0e
    0x01 | 0x02 | 0x04 | 0x08 | 0x00 | 0x00 | 0x40, //E 0x0f
    0x01 | 0x02 | 0x04 | 0x00 | 0x00 | 0x00 | 0x40, //F 0x10
    0x00 | 0x02 | 0x04 | 0x08 | 0x10 | 0x00 | 0x40, //G 0x11
    0x01 | 0x02 | 0x04 | 0x00 | 0x10 | 0x20 | 0x00, //H 0x12
    0x00 | 0x02 | 0x04 | 0x00 | 0x00 | 0x00 | 0x00, //I 0x13
    0x00 | 0x00 | 0x04 | 0x08 | 0x10 | 0x20 | 0x40, //J 0x14
    0x01 | 0x02 | 0x04 | 0x00 | 0x10 | 0x00 | 0x40, //K 0x15
    0x00 | 0x02 | 0x04 | 0x08 | 0x00 | 0x00 | 0x00, //L 0x16
    0x01 | 0x00 | 0x04 | 0x00 | 0x10 | 0x00 | 0x40, //M 0x17
    0x01 | 0x00 | 0x04 | 0x00 | 0x10 | 0x00 | 0x00, //N 0x18
    0x01 | 0x00 | 0x04 | 0x08 | 0x10 | 0x00 | 0x00, //O 0x19
    0x01 | 0x02 | 0x04 | 0x00 | 0x00 | 0x20 | 0x40, //P 0x1a
    0x01 | 0x02 | 0x00 | 0x00 | 0x10 | 0x20 | 0x40, //Q 0x1b
    0x01 | 0x00 | 0x04 | 0x00 | 0x00 | 0x00 | 0x00, //R 0x1c
    0x00 | 0x02 | 0x00 | 0x08 | 0x10 | 0x00 | 0x40, //S 0x1d
    0x01 | 0x02 | 0x04 | 0x08 | 0x00 | 0x00 | 0x00, //T 0x1e
    0x00 | 0x00 | 0x04 | 0x08 | 0x10 | 0x00 | 0x00, //U 0x1f
    0x00 | 0x02 | 0x00 | 0x08 | 0x00 | 0x20 | 0x00, //V 0x20
    0x01 | 0x02 | 0x00 | 0x08 | 0x00 | 0x20 | 0x00, //W 0x21
    0x00 | 0x00 | 0x04 | 0x00 | 0x10 | 0x00 | 0x00, //X 0x22
    0x01 | 0x02 | 0x00 | 0x08 | 0x10 | 0x20 | 0x00, //Y 0x23
    0x00 | 0x00 | 0x04 | 0x08 | 0x00 | 0x20 | 0x40, //Z 0x24
    0x01 | 0x00 | 0x00 | 0x00 | 0x00 | 0x00 | 0x00, //- 0x25
    0x26,                                           //. 0x26
    0x27,                                           //: 0x27
    0x00,                                           //  0x28
};

} // flw::priv
} // flw

/*
 *      _      _____ _____  _____  _           _
 *     | |    / ____|  __ \|  __ \(_)         | |
 *     | |   | |    | |  | | |  | |_ ___ _ __ | | __ _ _   _
 *     | |   | |    | |  | | |  | | / __| '_ \| |/ _` | | | |
 *     | |___| |____| |__| | |__| | \__ \ |_) | | (_| | |_| |
 *     |______\_____|_____/|_____/|_|___/ .__/|_|\__,_|\__, |
 *                                      | |             __/ |
 *                                      |_|            |___/
 */

/** @brief Create lcd display widget.
*
* @param[in] X  X pos.
* @param[in] Y  Y pos.
* @param[in] W  Width.
* @param[in] H  Height.
* @param[in] l  Label.
*/
flw::LCDDisplay::LCDDisplay(int X, int Y, int W, int H, const char *l) : Fl_Box(X, Y, W, H, l) {
    _align     = FL_ALIGN_LEFT;
    _dot_size  = 6;
    _seg_color = FL_FOREGROUND_COLOR;
    _seg_gap   = 1;
    _thick     = 4;
    _unit_gap  = 4;
    _unit_h    = 42;
    _unit_w    = 21;

    box(FL_NO_BOX);
}

/** @brief Draw widget.
*
*/
void flw::LCDDisplay::draw() {
    Fl_Box::draw();

    int sw = 0;
    int tx = x();
    int ty = y();
    int tw = w();
    int th = h();

    for (auto c : _value) {
        if (c == 0x26 || c == 0x27) {
            sw += _dot_size + _unit_gap;
        }
        else {
            sw += _unit_w + _unit_gap;
        }
    }

    switch (_align) {
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

    for (auto c : _value) {
        if (c == 0x26) { // Draw dot.
            fl_rectf(xx, ty + _unit_h + 1 - _dot_size, _dot_size, _dot_size);
            xx += (_dot_size + _unit_gap);
        }
        else if (c == 0x27) { // Draw colon.
            fl_rectf(xx, ty + _unit_h + 1 - _dot_size, _dot_size, _dot_size);
            fl_rectf(xx, ty + _unit_h + 1 - _dot_size * 3, _dot_size, _dot_size);
            xx += (_dot_size + _unit_gap);
        }
        else if (c > 0x00 && c < 0x26) { // Draw numbers, letters and hyphen.
            _draw(priv::_LCDNUMBER_SEGMENTS[static_cast<size_t>(c)], xx, ty, _unit_w, _unit_h);
            xx += (_unit_w + _unit_gap);
        }
        else {
            xx += (_unit_w + _unit_gap);
        }
    }
}

/** @brief Draw character.
*
* @param[in] a  Number with 7 bits, one for each segment.
* @param[in] x  X pos.
* @param[in] y  Y pos.
* @param[in] w  Width.
* @param[in] h  Height.
*/
void flw::LCDDisplay::_draw(unsigned a, int x, int y, int w, int h) {
    int x0, y0, x1, y1, x2, y2, x3, y3;
    int h2     = h >> 1;
    int thick2 = _thick >> 1;

    if (a & 0x40) { // One.
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

    if (a & 0x20) { // Two.
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

    if (a & 0x10) { // Three.
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

    if (a & 0x08) { // Four.
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

    if (a & 0x04) { // Five.
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

    if (a & 0x02) { // Six.
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

    if (a & 0x01) { // Middle.
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

/** @brief Set value.
*
* @param[in] value  String with "0-9", "a-z",  "A-Z" and "-.:".
*/
void flw::LCDDisplay::value(const std::string& value) {
    _value = "";

    for (auto c : value) {
        if (c >= 0x30 && c <= 0x39) {
            _value += (c - 0x2F);
        }
        else if (c >= 0x41 && c <= 0x5a) {
            _value += (c - 0x36);
        }
        else if (c >= 0x61 && c <= 0x7a) {
            _value += (c - 0x56);
        }
        else if (c == '-') {
            _value += (0x25);
        }
        else if (c == '.') {
            _value += (0x26);
        }
        else if (c == ':') {
            _value += (0x27);
        }
        else {
            _value += (0x28);
        }
    }

    Fl::redraw();
}

// MKALGAM_OFF
