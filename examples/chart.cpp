// Copyright gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#include "flw.h"

int main() {
    Fl::scheme("oxy");
    Fl::screen_scale(0, 1.0);

    // [flw::chart::Chart example]

    auto win     = new Fl_Window(Fl::w() / 2 - 640, Fl::h() / 2 - 320, 1280, 640, "flw::chart::Chart");
    auto chart   = new flw::chart::Chart(0, 0, 1280, 640);
    auto start   = gnu::Date("2000-01-01");
    auto stop    = gnu::Date("2000-12-31");
    auto points1 = flw::chart::PointVector();
    auto points2 = flw::chart::PointVector();
    auto points3 = flw::chart::PointVector();
    auto points4 = flw::chart::PointVector();
    auto points5 = flw::chart::PointVector();

    while (start <= stop) {
        auto r1 = rand() % 200 + 500;
        auto r2 = rand() % 120'000 + 20'000;
        points1.push_back(flw::chart::Point(start.format(gnu::Date::Format::ISO_TIME), r1, r1 * 0.9, r1 * 0.95));
        points2.push_back(flw::chart::Point(start.format(gnu::Date::Format::ISO_TIME), r2));
        start.add_days(1);
    }

    points3 = flw::chart::Point::MovingAverage(points1, 8);
    points4 = flw::chart::Point::Modify(flw::chart::Point::MovingAverage(points1, 5), flw::chart::Modifier::MULTIPLICATION, 4.0);
    points5 = flw::chart::Point::MovingAverage(points2, 8);

    auto line1 = flw::chart::Line(points1, "BAR_HLC", flw::chart::LineType::BAR_HLC);
    line1.set_color(FL_BLUE);
    line1.set_width(4);

    auto line2 = flw::chart::Line(points3, "MVG 8", flw::chart::LineType::LINE);
    line2.set_color(FL_DARK_GREEN);
    line2.set_width(2);

    auto line3 = flw::chart::Line(points4, "MVG 5", flw::chart::LineType::LINE);
    line3.set_color(FL_RED);
    line3.set_width(2);
    line3.set_align(FL_ALIGN_RIGHT);

    auto line4 = flw::chart::Line(points2, "BAR_CLAMP", flw::chart::LineType::BAR_CLAMP);
    line4.set_color(FL_DARK_BLUE);
    line4.set_width(8);

    auto line5 = flw::chart::Line(points5, "MVG 8", flw::chart::LineType::LINE);
    line5.set_color(FL_DARK_GREEN);
    line5.set_width(2);

    chart->area(flw::chart::AreaNum::ONE).add_line(line1);
    chart->area(flw::chart::AreaNum::ONE).add_line(line2);
    chart->area(flw::chart::AreaNum::ONE).add_line(line3);
    chart->area(flw::chart::AreaNum::TWO).add_line(line4);
    chart->area(flw::chart::AreaNum::TWO).add_line(line5);
    chart->area(flw::chart::AreaNum::TWO).set_min_clamp(0);

    chart->set_area_size(60, 40);
    chart->set_date_range(flw::chart::DateRange::DAY);
    chart->set_tick_width(24);
    chart->set_main_label("Example");
    chart->set_hor_lines(false);
    chart->init_new_data();
    chart->do_layout();

    win->resizable(chart);
    win->show();
    Fl::run();

    // [flw::chart::Chart example]
}
