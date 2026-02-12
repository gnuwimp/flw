/**
* @file
* @brief A button with icon made from an svg image.
*
* @author gnuwimp@gmail.com
* @copyright Released under the GNU General Public License v3.0
*/

#include "svgbutton.h"

// MKALGAM_ON

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

unsigned flw::SVGButton::SPACING = 8;

/** @brief Create svg button.
*
* This constructor is mainly used by dlg::msg_ask() and dlg::msg_warning().\n
*\n
* Prepend label with one of the predefined svg images.\n
* "CONFIRM:Ok" Will create an image from icons::CONFIRM with label "Ok".\n
* "CONFIRM:^Ok" Creates the above button but with a darker background when button has the focus.\n
* "CONFIRM:!Ok" Creates a button that is the the default button which can be activated with the enter key.\n
* "CONFIRM:^!Ok" Sets both darker background and makes it the default button.\n
*
* @param[in] X  X pos.
* @param[in] Y  Y pos.
* @param[in] W  Width.
* @param[in] H  Height.
* @param[in] l  Label string.
*/
flw::SVGButton::SVGButton(int X, int Y, int W, int H, const std::string& l) : Fl_Button(X, Y, W, H) {
    _return = false;
    _dark   = false;

    auto t = util::trim(l);
    auto v = util::split_string(t, ":");
    auto i = std::string();
    auto r = (size_t) 0;

    if (v.size() == 2) {
        t = v[1];

        if (v[0] == "ALERT")            i = icons::ALERT;
        else if (v[0] == "BACK")        i = icons::BACK;
        else if (v[0] == "CANCEL")      i = icons::CANCEL;
        else if (v[0] == "CONFIRM")     i = icons::CONFIRM;
        else if (v[0] == "DEL")         i = icons::DEL;
        else if (v[0] == "DOWN")        i = icons::DOWN;
        else if (v[0] == "EDIT")        i = icons::EDIT;
        else if (v[0] == "ERR")         i = icons::ERR;
        else if (v[0] == "FORWARD")     i = icons::FORWARD;
        else if (v[0] == "INFO")        i = icons::INFO;
        else if (v[0] == "LEFT")        i = icons::LEFT;
        else if (v[0] == "PASSWORD")    i = icons::PASSWORD;
        else if (v[0] == "PAUSE")       i = icons::PAUSE;
        else if (v[0] == "PLAY")        i = icons::PLAY;
        else if (v[0] == "QUESTION")    i = icons::QUESTION;
        else if (v[0] == "RIGHT")       i = icons::RIGHT;
        else if (v[0] == "STOP")        i = icons::STOP;
        else if (v[0] == "UP")          i = icons::UP;
        else if (v[0] == "WARNING")     i = icons::WARNING;
    }

    if (t.find("^!") != std::string::npos || t.find("!^") != std::string::npos) {
        _dark   = true;
        _return = true;
        r       = 2;
    }
    else if (t.find("^") != std::string::npos) {
        _dark = true;
        r     = 1;

    }
    else if (t.find("!") != std::string::npos) {
        _return = true;
        r       = 1;
    }

    if (t.length() > r) {
        t = t.substr(r);
    }
    else {
        t = "";
    }

    set_icon(t, i);
}

/** @brief Create button and set all options.
*
* @param[in] X     X pos.
* @param[in] Y     Y pos.
* @param[in] W     Width.
* @param[in] H     Height.
* @param[in] l     Label string.
* @param[in] svg   SVG image string.
* @param[in] ret   True to make button an default button.
* @param[in] dark  True to mark draw button with darker background color.
* @param[in] pos   Where to put the image.
* @param[in] size  Size (1.0 - 5.0) which will be multiplied with flw::PREF_FONTSIZE (icon can not be less than 12 pixels).
*/
flw::SVGButton::SVGButton(
    int                 X,
    int                 Y,
    int                 W,
    int                 H,
    const std::string&  l,
    const std::string&  svg,
    bool                ret,
    bool                dark,
    Pos                 pos,
    double              size
    ) : Fl_Button(X, Y, W, H) {

    _return = ret;
    _dark   = dark;

    set_icon(l, svg, pos, size);
}

/** @brief Draw button with optional darker background color if button has the focus.
*
*/
void flw::SVGButton::draw() {
    if (Fl::focus() == this && _dark == true) {
        color(fl_darker(FL_BACKGROUND_COLOR));
    }
    else {
        color(FL_BACKGROUND_COLOR);
    }

    Fl_Button::draw();
}

/** @brief Take care of enter presses.
*
*/
int flw::SVGButton::handle(int event) {
    if (_return == true &&
        event == FL_SHORTCUT &&
        (Fl::event_key() == FL_Enter || Fl::event_key() == FL_KP_Enter)) {

        simulate_key_action();
        do_callback(FL_REASON_SELECTED);
        return 1;
    }

    return Fl_Button::handle(event);
}

/** @brief Set label and create optional icon.
*
* @param[in] l     Label string.
* @param[in] svg   SVG string.
* @param[in] pos   Where to put the image.
* @param[in] size  Size (1.0 - 5.0) which will be multiplied with flw::PREF_FONTSIZE (icon can not be less than 12 pixels).
*
* @return True if icon was set.
*/
bool flw::SVGButton::set_icon(const std::string& l, const std::string& svg, Pos pos, double size) {
    copy_label(l.c_str());

    auto al = FL_ALIGN_IMAGE_NEXT_TO_TEXT;

    if (pos == Pos::RIGHT) {
        al = FL_ALIGN_TEXT_NEXT_TO_IMAGE;
    }
    else if (pos == Pos::ABOVE) {
        al = FL_ALIGN_IMAGE_OVER_TEXT;
    }
    else if (pos == Pos::BELOW) {
        al = FL_ALIGN_TEXT_OVER_IMAGE;
    }
    else if (pos == Pos::UNDER) {
        al = FL_ALIGN_IMAGE_BACKDROP;
    }

    if (size < 1.0) {
        size = 1.0;
    }
    else if (size > 5.0) {
        size = 5.0;
    }

    if (svg != "") {
        if (util::icon(this, svg, flw::PREF_FONTSIZE * size) == true) {
            align(FL_ALIGN_CENTER | FL_ALIGN_INSIDE | FL_ALIGN_CLIP | al);
            label_image_spacing(SVGButton::SPACING);
            return true;
        }
        else {
            align(FL_ALIGN_CENTER | FL_ALIGN_INSIDE | FL_ALIGN_CLIP);
        }
    }
    else {
        align(FL_ALIGN_CENTER | FL_ALIGN_INSIDE | FL_ALIGN_CLIP);
    }

    return false;
}

// MKALGAM_OFF
