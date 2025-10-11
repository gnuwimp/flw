// Copyright gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#include "flw.h"

int main() {
    Fl::scheme("oxy");
    Fl::screen_scale(0, 1.0);
    srand(time(0));

    // [flw::plot::Plot example]

    auto win     = new Fl_Window(Fl::w() / 2 - 640, Fl::h() / 2 - 320, 1280, 640, "flw::plot::Plot");
    auto plot    = new flw::plot::Plot(0, 0, 1280, 640);
    auto points1 = flw::plot::PointVector();
    auto points2 = flw::plot::PointVector();
    auto points3 = flw::plot::PointVector();
    auto points4 = flw::plot::PointVector();
    auto x       = 1.0;

    while (x < 16.0) {
        points1.push_back(flw::plot::Point(x, sin(x)));
        points2.push_back(flw::plot::Point(x, cos(x)));
        x += 0.1;
    }

    x = 1.0;
    while (x < 16.0) {
        auto rnd = rand() % 19;
        points3.push_back(flw::plot::Point(x, -0.9 + rnd * 0.1));

        rnd = rand() % 19;
        points4.push_back(flw::plot::Point(x, -0.9 + rnd * 0.1));

        x += 1.0;
    }

    plot->add_line(flw::plot::Line(points1, "LINE", flw::plot::LineType::LINE, FL_BLUE, 2));
    plot->add_line(flw::plot::Line(points2, "LINE_WITH_SQUARE", flw::plot::LineType::LINE_WITH_SQUARE, FL_RED, 1));
    plot->add_line(flw::plot::Line(points3, "FILLED_CIRCLE", flw::plot::LineType::FILLED_CIRCLE, FL_DARK_GREEN, 10));
    plot->add_line(flw::plot::Line(points4, "VECTOR", flw::plot::LineType::VECTOR, FL_DARK_MAGENTA, 4));
    plot->set_label("Plot");
    plot->xscale().set_label("1 to 16");
    plot->yscale().set_label("Sin() and Cos()");
    plot->init_new_data();
    plot->do_layout();

    win->resizable(plot);
    win->show();
    Fl::run();

    // [flw::plot::Plot example]
}
