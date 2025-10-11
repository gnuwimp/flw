// Copyright gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#include "flw.h"

int main() {
    Fl::scheme("oxy");
    Fl::screen_scale(0, 1.0);

    // [flw::SplitGroup example]

    auto* win   = new Fl_Window(Fl::w() / 2 - 320, Fl::h() / 2 - 240, 640, 480, "flw::SplitGroup");
    auto* split = new flw::SplitGroup(0, 0, 640, 480);

    // Add left widget.
    split->add(new Fl_Button(0, 0, 0, 0, "Swap"), true);

    // Add right widget.
    split->add(new Fl_Button(0, 0, 0, 0, "HORIZONTAL/VERTICAL"), false);

    // Swap the two buttons.
    split->child(true)->callback([](Fl_Widget*, void* o) {
        auto s = (flw::SplitGroup*) o;
        s->swap();
    }, split);

    // Toggle layout between horizontal and vertical.
    split->child(false)->callback([](Fl_Widget*, void* o) {
        auto s = (flw::SplitGroup*) o;
        s->pos(s->pos() == flw::SplitGroup::Pos::HORIZONTAL ? flw::SplitGroup::Pos::VERTICAL : flw::SplitGroup::Pos::HORIZONTAL);
    }, split);

    // Set min split size.
    split->min_split_pos(100);
    split->do_layout();

    win->resizable(split);
    win->show();
    Fl::run();

    // [flw::SplitGroup example]
}
