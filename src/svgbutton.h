/**
* @file
* @brief A button with icon made from an svg image.
*
* @author gnuwimp@gmail.com
* @copyright Released under the GNU General Public License v3.0
*/

#ifndef FLW_SVGBUTTON_H
#define FLW_SVGBUTTON_H

#include "flw.h"

// MKALGAM_ON

#include <FL/Fl_Button.H>

namespace flw {

/*
 *       _______      _______ ____        _   _
 *      / ____\ \    / / ____|  _ \      | | | |
 *     | (___  \ \  / / |  __| |_) |_   _| |_| |_ ___  _ __
 *      \___ \  \ \/ /| | |_ |  _ <| | | | __| __/ _ \| '_ \
 *      ____) |  \  / | |__| | |_) | |_| | |_| || (_) | | | |
 *     |_____/    \/   \_____|____/ \__,_|\__|\__\___/|_| |_|
 *
 *
 */

/** @brief A button with icon made from an svg image.
*
* Turn on SVGButton::set_dark() property to draw the focused button with a darker background.\n
* Turn on SVGButton::set_return() property to make the button the default which can be activated with enter key.\n
* Set only one button to be the return button.\n
*\n
* By default the image will be sized 1.4 * flw::PREF_FONTSIZE.\n
* Image should be square.\n
*
* @see flw::icons
*
* @snippet toolgroup.cpp flw::ToolGroup example
* @image html toolgroup.png
* <br>
* @image html icons.png
*/
class SVGButton : public Fl_Button {
public:
    static unsigned             SPACING;    ///< @brief Space between label and image, default 8 pixels.

    enum class Pos {
                                RIGHT,      ///< @brief Draw image to the right of label.
                                LEFT,       ///< @brief Draw image to the left of label.
                                ABOVE,      ///< @brief Draw image above label.
                                BELOW,      ///< @brief Draw image below label.
                                UNDER,      ///< @brief Draw image under label.
    };

                                SVGButton(int X, int Y, int W, int H, const std::string& l);
                                SVGButton(int X, int Y, int W, int H, const std::string& l, const std::string& svg, bool ret = false, bool dark = false, Pos pos = Pos::RIGHT, double size = 1.3);
    void                        draw() override;
    int                         handle(int event) override;
    bool                        has_dark() const
                                    { return _dark; } ///< @brief Return true if button will draw with a darker background if it has focus.
    bool                        has_image() const
                                    { return image() != nullptr; } ///< @brief Return true if button has a image.
    bool                        has_return() const
                                    { return _return; } ///< @brief Return true if button can be activated with enter key.
    void                        set_dark(bool value)
                                    { _dark = value; } ///< @brief Set active property.
    bool                        set_icon(const std::string& l, const std::string& svg_image, Pos pos = Pos::RIGHT, double size = 1.3);
    void                        set_return(bool value)
                                    { _return = value; } ///< @brief Set return property.

private:
    bool                        _dark;      ///< @brief Dark color for the active button color.
    bool                        _return;    ///< @brief If true then it is an return button.
};

}

// MKALGAM_OFF

#endif
