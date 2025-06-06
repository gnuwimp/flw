// Copyright gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#include "flw.h"

int main() {
    Fl::scheme("oxy");
    Fl::screen_scale(0, 1.0);

    // [flw::GridGroup GridGroup example]

    auto win = new Fl_Window(Fl::w() / 2 - 320, Fl::h() / 2 - 240, 640, 480, "flw::GridGroup");
    auto grid = new flw::GridGroup(0, 0, 640, 480);

    // Button at the top that use all available width.
    grid->add(new Fl_Button(0, 0, 0, 0, "X=1, Y=1, W=-1, H=4"), 1, 1, -1, 4);

    // Two buttons at the same row, this has static width.
    grid->add(new Fl_Button(0, 0, 0, 0, "X=1, Y=6, W=30, H=4"), 1, 6, 30, 4);

    // And swecond button has dynamic width.
    grid->add(new Fl_Button(0, 0, 0, 0, "X=32, Y=6, W=-1, H=4"), 32, 6, -1, 4);

    // Fixed size button.
    grid->add(new Fl_Button(0, 0, 0, 0, "X=1, Y=11, W=30, H=10"), 1, 11, 30, 10);

    // Fixed size button except for the height.
    grid->add(new Fl_Button(0, 0, 0, 0, "X=11, Y=21, W=30, H=-6\nDynamic height"), 1, 21, 30, -6);

    auto w = new Fl_Button(0, 0, 0, 0, "X=32, Y=11, W=-1, H=-6\nAdjusted -5 pixels on each side\nExpand all available height and width");
    // Fill all available space in the middle/right.
    grid->add(w, 32, 11, -1, -6);
    // Shrink widget with 5 pixels on all sides.
    grid->adjust(w, 5, -10, 5, -10);

    // Two buttons at the bottom.
    grid->add(new Fl_Button(0, 0, 0, 0, "X=-31, Y=-5, W=30, H=4"), -31, -5, 30, 4);
    grid->add(new Fl_Button(0, 0, 0, 0, "X=-62, Y=-5, W=30, H=4"), -62, -5, 30, 4);

    grid->do_layout();
    win->resizable(grid);
    win->show();
    Fl::run();

    // [flw::GridGroup GridGroup example]
}
