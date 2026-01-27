// Copyright gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#include "flw.h"

int main() {
    Fl::scheme("oxy");
    Fl::screen_scale(0, 1.0);

    // [flw::ToolGroup example]

    auto win   = new Fl_Window(Fl::w() / 2 - 320, Fl::h() / 2 - 240, 600, 480, "flw::ToolGroup");
    auto toolh = new flw::ToolGroup(0, 0, 600, 70);
    auto toolv = new flw::ToolGroup(0, 70, 120, 410);

    // Add child widgets to horizontal group, the second button is the default button in this group, which can be activated with enter key.
    toolh->add(new flw::SVGButton(0, 0, 0, 0, "icons::BACK", flw::icons::BACK, false, false, flw::SVGButton::Pos::ABOVE));
    toolh->add(new flw::SVGButton(0, 0, 0, 0, "icons::FORWARD", flw::icons::FORWARD, true, false, flw::SVGButton::Pos::ABOVE));
    toolh->add(new flw::SVGButton(0, 0, 0, 0, "icons::CANCEL", flw::icons::CANCEL, false, false, flw::SVGButton::Pos::ABOVE));
    toolh->add(new flw::SVGButton(0, 0, 0, 0, "icons::DEL", flw::icons::DEL, false, false, flw::SVGButton::Pos::ABOVE));
    toolh->add(new flw::SVGButton(0, 0, 0, 0, "icons::CONFIRM", flw::icons::CONFIRM, false, false, flw::SVGButton::Pos::ABOVE));

    // Make sure last widget does not have a gap.
    toolh->expand_last(true);

    // Make toolgroup vertical.
    toolv->pos(flw::ToolGroup::Pos::VERTICAL);

    // Add child widgets to vertical group.
    // First is the default button ("!").
    // Focused buttons does have a darker background ("^").
    // The string up to ":" is the name of icon from flw::icon namespace.
    toolv->add(new flw::SVGButton(0, 0, 0, 0, "CONFIRM:!^Vertical 1"));
    toolv->add(new flw::SVGButton(0, 0, 0, 0, "UP:^Vertical 2"));
    toolv->add(new flw::SVGButton(0, 0, 0, 0, "DOWN:^Vertical 3"));

    // Set widgets to equal height with 6 font units.
    toolv->size(6);

    toolh->do_layout();
    toolv->do_layout();
    win->show();
    Fl::run();

    // [flw::ToolGroup example]
}
