// Copyright gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#include "datechooser.h"
#include "theme.h"
#include "util.h"
#include <FL/Fl_Double_Window.H>
#include <FL/Fl.H>

using namespace flw;

class Test : public Fl_Double_Window {
public:
    DateChooser* dc;

    Test(int W, int H) : Fl_Double_Window(W, H, "test_datechooser.cpp") {
        dc = new DateChooser(flw::PREF_FONTSIZE, flw::PREF_FONTSIZE, W - flw::PREF_FONTSIZE * 2, H - flw::PREF_FONTSIZE * 2);
        dc->callback(Callback);
        dc->set(Date());
        size_range(64, 48);
        resizable(this);
    }

    void resize(int X, int Y, int W, int H) override {
        Fl_Double_Window::resize(X, Y, W, H);
        dc->resize(0, 0, W, H);
        // dc->resize(20, 20, W - 40, H - 40);
    }

    static void Callback(Fl_Widget* w, void* o) {
        (void) o;

        auto dc   = (const DateChooser*) w;
        auto date = dc->get();

        printf("Callback=%s\n", date.format(Date::FORMAT::ISO_LONG).c_str());
        fflush(stdout);
    }
};

int main(int argc, const char** argv) {
    if (flw::theme::parse(argc, argv) == false) {
        flw::theme::load("gtk");
    }

    Test win(flw::PREF_FONTSIZE * 35, flw::PREF_FONTSIZE * 25);
    win.show();
    win.dc->focus();
    return Fl::run();
}
