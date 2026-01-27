// Copyright gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#include "flw.h"

int main() {
    Fl::scheme("oxy");
    Fl::screen_scale(0, 1.0);

    // [flw::LogDisplay example]

const std::string TEXT = R"(64.242.88.10 - [07/Mar/2004:16:11:58 -0800] "GET /twiki/bin/view/TWiki/WikiSyntax HTTP/1.1" 200 7352
64.242.88.10 - [07/Mar/2004:16:20:55 -0800] "GET /twiki/bin/view/Main/DCCAndPostFix HTTP/1.1" 200 5253
64.242.88.10 - [07/Mar/2004:16:23:12 -0800] "GET /twiki/bin/oops/TWiki/AppendixFileSystem?template=oopsmore,m1=1.12,m2=1.12 HTTP/1.1" 200 11382
64.242.88.10 - [07/Mar/2004:16:24:16 -0800] "GET /twiki/bin/view/Main/PeterThoeny HTTP/1.1" 200 4924
64.242.88.10 - [07/Mar/2004:16:29:16 -0800] "GET /twiki/bin/edit/Main/Header_checks?topicparent=Main.ConfigurationVariables HTTP/1.1" 401 12851
64.242.88.10 - [07/Mar/2004:16:30:29 -0800] "GET /twiki/bin/attach/Main/OfficeLocations HTTP/1.1" 401 12851
64.242.88.10 - [07/Mar/2004:16:31:48 -0800] "GET /twiki/bin/view/TWiki/WebTopicEditTemplate HTTP/1.1" 200 3732
)";

static const std::string JSON = R"([
    {
        "style": "range",
        "word1": "[",
        "word2": "]",
        "inclusive": true,
        "color": "BLUE",
        "count": 1
    },
    {
        "style": "lock",
        "on": true
    },
    {
        "style": "num",
        "color": "RED"
    },
    {
        "style": "lock",
        "on": false
    },
    {
        "style": "between",
        "word1": "\"",
        "word2": "\"",
        "inclusive": true,
        "color": "GREEN"
    },
    {
        "style": "string",
        "word1": "HTTP/1.1",
        "color": "U_BOLD_MAGENTA",
        "count": 0
    }
]
)";

    auto win = new Fl_Window(Fl::w() / 2 - 650, Fl::h() / 2 - 150, 1300, 300, "flw::LogDisplay");
    auto log = new flw::LogDisplay(0, 0, 1300, 300);

    log->box(FL_BORDER_BOX);
    log->value(TEXT);
    log->style(JSON);

    win->resizable(log);
    win->show();
    Fl::run();

    // [flw::LogDisplay example]
}
