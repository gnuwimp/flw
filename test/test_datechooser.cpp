// Copyright gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#include "flw.h"

#ifndef FLW_AMALGAM
    #include "datechooser.h"
    #include "theme.h"
#endif

//------------------------------------------------------------------------------
int main(int argc, const char** argv) {
    if (flw::theme::parse(argc, argv) == false) {
        flw::theme::load("oxy");
    }

    auto date = gnu::Date("1999-12-31");
    flw::dlg::date("flw::dlg::date() using flw::DateChooser", date);
    FLW_PRINTV(date.format())
    
    return 0;
}
