/**
* @file
* @brief A 7 segment widget.
*
* @author z_hossain
* @author gnuwimp@gmail.com
*/

#ifndef FLW_LCDDISPLAY_H
#define FLW_LCDDISPLAY_H

#include "flw.h"

// MKALGAM_ON

#include <FL/Fl_Box.H>

namespace flw {

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

/** @brief A 7 segment label widget.
*
* Valid characters are:\n
* 0123456789\n
* abcdefghijklmnopqrstuvwxyz\n
* -.:\n
*
* Letters are somewhat problematic to read du to 7 segments limitation.\n
*
* @snippet lcddisplay.cpp flw::LCDDisplay::timeout example
* @snippet lcddisplay.cpp flw::LCDDisplay example
* @image html lcddisplay.png
*/
class LCDDisplay : public Fl_Box {
public:
                                LCDDisplay(int X = 0, int Y = 0, int W = 0, int H = 0, const char* l = nullptr);
    Fl_Align                    align() const
                                    { return _align; } ///< @brief LCD alignment.
    void                        align(Fl_Align align)
                                    { _align = align; Fl::redraw(); } ///< @brief Set LCD alignment.
    int                         dot_size() const
                                    { return _dot_size; } ///< @brief Get dot size.
    void                        dot_size(int size)
                                    { _dot_size = size; Fl::redraw(); } ///< @brief Set dot size in pixels.
    void                        draw() override;
    Fl_Color                    segment_color() const
                                    { return _seg_color; } ///< @brief Get segment color.
    void                        segment_color(Fl_Color color)
                                    { _seg_color = color; Fl::redraw(); } ///< @brief Set segment color.
    int                         segment_gap() const
                                    { return _seg_gap; } ///< @brief Get gap between segments.
    void                        segment_gap(int gap)
                                    { _seg_gap = gap; Fl::redraw(); } ///< @brief Set gap in segments in pixels.
    int                         thickness() const
                                    { return _thick; } ///< @brief Get segment thickness.
    void                        thickness(int thickness)
                                    { _thick = thickness; Fl::redraw(); } ///< @brief Set thickness in segments in pxiels.
    int                         unit_gap()
                                    { return _unit_gap; } ///< @brief Space between characters.
    void                        unit_gap(int gap)
                                    { _unit_gap = gap; Fl::redraw(); } ///< @brief Set characters spacement.
    int                         unit_h() const
                                    { return _unit_h; } ///< @brief Get character height.
    void                        unit_h(int height)
                                    { _unit_h = height; Fl::redraw(); } ///< @brief Set character height in pixels.
    int                         unit_w() const
                                    { return _unit_w; } ///< @brief Get character width.
    void                        unit_w(int width)
                                    { _unit_w = width; Fl::redraw(); } ///< @brief Set character width in pixels.
    const std::string           value() const
                                    { return _value; } ///< @brief Get hex codes that contains draw information.
    void                        value(const std::string& value);

private:
    void                        _draw(unsigned a, int x, int y, int w, int h);

    Fl_Align                    _align;
    Fl_Color                    _seg_color;
    std::string                 _value;
    int                         _dot_size;
    int                         _seg_gap;
    int                         _thick;
    int                         _unit_gap;
    int                         _unit_h;
    int                         _unit_w;
};

}

// MKALGAM_OFF

#endif
