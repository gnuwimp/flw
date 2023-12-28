// Copyright 2019 - 2021 gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#include "logdisplay.h"
#include "theme.h"
#include <FL/Fl_Double_Window.H>
#include <FL/Fl.H>

const char* TEXT =
"AAA123123123123123123123III123123123123123123123123RRR123123123123123123YYYZZZ123123123123123123123123ccc123123123ggg123123123kkk123123123ooo123123123XXXX\n"
"AAA123123123123123123123III123123123123123123123123RRR123123123123123123YYYZZZ123123123123123123123123ccc123123123ggg123123123kkk123123123ooo123123123XXXX\n"
"64.242.88.10 - - [07/Mar/2004:16:05:49 -0800] \"GET /twiki/bin/edit/Main/Double_bounce_sender?topicparent=Main.ConfigurationVariables HTTP/1.1\" 401 12846\n"
"64.242.88.10 - - [07/Mar/2004:16:06:51 -0800] \"GET /twiki/bin/rdiff/TWiki/NewUserTemplate?rev1=1.3&rev2=1.2 HTTP/1.1\" 200 4523\n"
"64.242.88.10 - - [07/Mar/2004:16:10:02 -0800] \"GET /mailman/listinfo/hsdivision HTTP/1.1\" 200 6291\n"
"64.242.88.10 - - [07/Mar/2004:16:11:58 -0800] \"GET /twiki/bin/view/TWiki/WikiSyntax HTTP/1.1\" 200 7352\n"
"64.242.88.10 - - [07/Mar/2004:16:20:55 -0800] \"GET /twiki/bin/view/Main/DCCAndPostFix HTTP/1.1\" 200 5253\n"
"64.242.88.10 - - [07/Mar/2004:16:23:12 -0800] \"GET /twiki/bin/oops/TWiki/AppendixFileSystem?template=oopsmore¶m1=1.12¶m2=1.12 HTTP/1.1\" 200 11382\n"
"64.242.88.10 - - [07/Mar/2004:16:24:16 -0800] \"GET /twiki/bin/view/Main/PeterThoeny HTTP/1.1\" 200 4924\n"
"64.242.88.10 - - [07/Mar/2004:16:29:16 -0800] \"GET /twiki/bin/edit/Main/Header_checks?topicparent=Main.ConfigurationVariables HTTP/1.1\" 401 12851\n"
"64.242.88.10 - - [07/Mar/2004:16:30:29 -0800] \"GET /twiki/bin/attach/Main/OfficeLocations HTTP/1.1\" 401 12851\n"
"64.242.88.10 - - [07/Mar/2004:16:31:48 -0800] \"GET /twiki/bin/view/TWiki/WebTopicEditTemplate HTTP/1.1\" 200 3732\n"
"64.242.88.10 - - [07/Mar/2004:16:32:50 -0800] \"GET /twiki/bin/view/Main/WebChanges HTTP/1.1\" 200 40520\n"
"64.242.88.10 - - [07/Mar/2004:16:33:53 -0800] \"GET /twiki/bin/edit/Main/Smtpd_etrn_restrictions?topicparent=Main.ConfigurationVariables HTTP/1.1\" 401 12851\n"
"64.242.88.10 - - [07/Mar/2004:16:35:19 -0800] \"GET /mailman/listinfo/business HTTP/1.1\" 200 6379\n"
"64.242.88.10 - - [07/Mar/2004:16:36:22 -0800] \"GET /twiki/bin/rdiff/Main/WebIndex?rev1=1.2&rev2=1.1 HTTP/1.1\" 200 46373\n"
"64.242.88.10 - - [07/Mar/2004:16:37:27 -0800] \"GET /twiki/bin/view/TWiki/DontNotify HTTP/1.1\" 200 4140\n"
"64.242.88.10 - - [07/Mar/2004:16:39:24 -0800] \"GET /twiki/bin/view/Main/TokyoOffice HTTP/1.1\" 200 3853\n"
"64.242.88.10 - - [07/Mar/2004:16:43:54 -0800] \"GET /twiki/bin/view/Main/MikeMannix HTTP/1.1\" 200 3686\n"
"64.242.88.10 - - [07/Mar/2004:16:45:56 -0800] \"GET /twiki/bin/attach/Main/PostfixCommands HTTP/1.1\" 401 12846\n"
"64.242.88.10 - - [07/Mar/2004:16:47:12 -0800] \"GET /robots.txt HTTP/1.1\" 200 68\n"
"64.242.88.10 - - [07/Mar/2004:16:47:46 -0800] \"GET /twiki/bin/rdiff/Know/ReadmeFirst?rev1=1.5&rev2=1.4 HTTP/1.1\" 200 5724\n"
"64.242.88.10 - - [07/Mar/2004:16:49:04 -0800] \"GET /twiki/bin/view/Main/TWikiGroups?rev=1.2 HTTP/1.1\" 200 5162\n"
"64.242.88.10 - - [07/Mar/2004:16:50:54 -0800] \"GET /twiki/bin/rdiff/Main/ConfigurationVariables HTTP/1.1\" 200 59679\n"
"64.242.88.10 - - [07/Mar/2004:16:52:35 -0800] \"GET /twiki/bin/edit/Main/Flush_service_name?topicparent=Main.ConfigurationVariables HTTP/1.1\" 401 12851\n"
"64.242.88.10 - - [07/Mar/2004:16:53:46 -0800] \"GET /twiki/bin/rdiff/TWiki/TWikiRegistration HTTP/1.1\" 200 34395\n"
"64.242.88.10 - - [07/Mar/2004:16:54:55 -0800] \"GET /twiki/bin/rdiff/Main/NicholasLee HTTP/1.1\" 200 7235\n"
"64.242.88.10 - - [07/Mar/2004:16:56:39 -0800] \"GET /twiki/bin/view/Sandbox/WebHome?rev=1.6 HTTP/1.1\" 200 8545\n"
"64.242.88.10 - - [07/Mar/2004:16:58:54 -0800] \"GET /mailman/listinfo/administration HTTP/1.1\" 200 6459\n"
"lordgun.org - - [07/Mar/2004:17:01:53 -0800] \"GET /razor.html HTTP/1.1\" 200 2869\n"
"64.242.88.10 - - [07/Mar/2004:17:09:01 -0800] \"GET /twiki/bin/search/Main/SearchResult?scope=text®ex=on&search=Joris%20*Benschop[^A-Za-z] HTTP/1.1\" 200 4284\n"
"64.242.88.10 - - [07/Mar/2004:17:10:20 -0800] \"GET /twiki/bin/oops/TWiki/TextFormattingRules?template=oopsmore¶m1=1.37¶m2=1.37 HTTP/1.1\" 200 11400\n"
"64.242.88.10 - - [07/Mar/2004:17:13:50 -0800] \"GET /twiki/bin/edit/TWiki/DefaultPlugin?t=1078688936 HTTP/1.1\" 401 12846\n"
"64.242.88.10 - - [07/Mar/2004:17:16:00 -0800] \"GET /twiki/bin/search/Main/?scope=topic®ex=on&search=^g HTTP/1.1\" 200 3675\n"
"64.242.88.10 - - [07/Mar/2004:17:17:27 -0800] \"GET /twiki/bin/search/TWiki/?scope=topic®ex=on&search=^d HTTP/1.1\" 200 5773\n"
"\n"
"lj1036.inktomisearch.com - - [07/Mar/2004:17:18:36 -0800] \"GET /robots.txt HTTP/1.0\" 200 68\n"
"lj1090.inktomisearch.com - - [07/Mar/2004:17:18:41 -0800] \"GET /twiki/bin/view/Main/LondonOffice HTTP/1.0\" 200 3860\n"
"\n"
"64.242.88.10 - - [07/Mar/2004:17:21:44 -0800] \"GET /twiki/bin/attach/TWiki/TablePlugin HTTP/1.1\" 401 12846\n"
"64.242.88.10 - - [07/Mar/2004:17:22:49 -0800] \"GET /twiki/bin/view/TWiki/ManagingWebs?rev=1.22 HTTP/1.1\" 200 9310\n"
"64.242.88.10 - - [07/Mar/2004:17:23:54 -0800] \"GET /twiki/bin/statistics/Main HTTP/1.1\" 200 808\n"
"64.242.88.10 - - [07/Mar/2004:17:26:30 -0800] \"GET /twiki/bin/view/TWiki/WikiCulture HTTP/1.1\" 200 5935\n"
"64.242.88.10 - - [07/Mar/2004:17:27:37 -0800] \"GET /twiki/bin/edit/Main/WebSearch?t=1078669682 HTTP/1.1\" 401 12846\n"
"64.242.88.10 - - [07/Mar/2004:17:28:45 -0800] \"GET /twiki/bin/oops/TWiki/ResetPassword?template=oopsmore¶m1=1.4¶m2=1.4 HTTP/1.1\" 200 11281\n"
"64.242.88.10 - - [07/Mar/2004:17:29:59 -0800] \"GET /twiki/bin/view/TWiki/ManagingWebs?skin=print HTTP/1.1\" 200 8806\n"
"64.242.88.10 - - [07/Mar/2004:17:31:39 -0800] \"GET /twiki/bin/edit/Main/UvscanAndPostFix?topicparent=Main.WebHome HTTP/1.1\" 401 12846\n"
"64.242.88.10 - - [07/Mar/2004:17:35:35 -0800] \"GET /twiki/bin/view/TWiki/KlausWriessnegger HTTP/1.1\" 200 3848\n"
"64.242.88.10 - - [07/Mar/2004:17:39:39 -0800] \"GET /twiki/bin/view/Main/SpamAssassin HTTP/1.1\" 200 4081\n"
"64.242.88.10 - - [07/Mar/2004:17:42:15 -0800] \"GET /twiki/bin/oops/TWiki/RichardDonkin?template=oopsmore¶m1=1.2¶m2=1.2 HTTP/1.1\" 200 11281\n"
"64.242.88.10 - - [07/Mar/2004:17:46:17 -0800] \"GET /twiki/bin/rdiff/TWiki/AlWilliams?rev1=1.3&rev2=1.2 HTTP/1.1\" 200 4485\n"
"64.242.88.10 - - [07/Mar/2004:17:47:43 -0800] \"GET /twiki/bin/rdiff/TWiki/AlWilliams?rev1=1.2&rev2=1.1 HTTP/1.1\" 200 5234\n"
"64.242.88.10 - - [07/Mar/2004:17:50:44 -0800] \"GET /twiki/bin/view/TWiki/SvenDowideit HTTP/1.1\" 200 3616\n"
"64.242.88.10 - - [07/Mar/2004:17:53:45 -0800] \"GET /twiki/bin/search/Main/SearchResult?scope=text®ex=on&search=Office%20*Locations[^A-Za-z] HTTP/1.1\" 200 7771"
;

namespace flw {

class MyDisplay : public flw::LogDisplay {
public:
    MyDisplay(int X, int Y, int W, int H) : flw::LogDisplay(X, Y, W, H, nullptr) {
    }

    void line(int count, const char* line, int len) {
        if (count < 3) {
            COLOR c = COLOR::FOREGROUND;

            for (int f = 0; f < 150; f += 3) {
                style(f, f + 2, c);

                if (c < COLOR::LAST) {
                    c = (COLOR) ((int) c + 1);
                }
            }
        }
        else {
            int i1, i2;

            str_index(line, " - - ", &i1);
            if (i1 != -1) {
                style(0, i1, COLOR::RED);
            }

            str_index(line, "[", &i1, "]", &i2);
            if (i1 != -1 && i2 != -1) {
                style(i1 + 1, i2 - 1, COLOR::GRAY);
            }

            str_index(line, "\"", &i1, "\"", &i2);
            if (i1 != -1 && i2 != -1) {
                style(i1 + 1, i2 - 1, COLOR::DARK_GREEN);
                style(i2 + 1, len, COLOR::BLUE);
            }
        }

    }
};
}

class Test : public Fl_Double_Window {
public:
    Test(int W, int H, const char* l) : Fl_Double_Window(W, H, l) {
        log = new flw::MyDisplay(0, 0, W, H);

        log->value(TEXT);
        log->find("07/Mar/2004:17:31:39 -0800", true);
        resizable(this);
        size_range(64, 48);
    }

    flw::LogDisplay* log;
};

int main(int argc, const char** argv) {
    if (flw::theme::parse(argc, argv) == false) {
        flw::theme::load("gtk");
    }

    Test win(800, 480, "test_logdisplay.cpp");
    win.show();
    return Fl::run();
}
