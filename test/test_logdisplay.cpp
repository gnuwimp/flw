// Copyright gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#include "flw.h"

#ifndef FLW_AMALGAM
    #include "logdisplay.h"
#endif

#include <FL/Fl.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Double_Window.H>

using namespace flw;

const char* TEXT =
"AAA123123123123123123123III123123123123123123123123RRR123123123123123123YYYZZZ123123123123123123XXXX\r\n"
"AAA123123123123123123123III123123123123123123123123RRR123123123123123123YYYZZZ123123123123123123XXXX\n"
"HelloWorld Hello World Hello World Hello World Hello World Hello World Hello World HelloXWorldHelloWorld\r\n"
"HelloWorld Hello World Hello World Hello World Hello World Hello World Hello World HelloXWorldHelloWorld\r\n"
"Hello World Hello World Hello World Hello World Hello World Hello World HelloWorldHelloXWorldHelloWorld\n"
"94.242.88.10 -102 +123 .666 [07/Mar/2004:16:05:49 -0800] [] [10::00] \"GET [/twiki/bin/edit/Main/Double_bounce_sender?topicparent=Main.ConfigurationVariables] HTTP/ 1.1\" 401 12846\n"
"94.242.88.10 -102 +123 .666 [07/Mar/2004:16:05:49 -0800] [] [10::00] \"GET [/twiki/bin/edit/Main/Double_bounce_sender?topicparent=Main.ConfigurationVariables] HTTP/ 1.1\" 401 12846\n"
"94.242.88.10 - - [07/Mar/2004:16:05:49 -0800] [] [10::00] [ XXX ] \"GET /twiki/bin/edit/Main/Double_bounce_sender?topicparent=Main.ConfigurationVariables HTTP/1.1\" 401 12846\n"
"94.242.88.10 - - [07/Mar/2004:16:05:49 -0800] [] [10::00] [[ XXX ]] \"GET /twiki/bin/edit/Main/Double_bounce_sender?topicparent=Main.ConfigurationVariables HTTP/1.1\" 401 12846\n"
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
"1\n"
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
"64.242.88.10 - - [07/Mar/2004:17:42:15 -0800] \"GET /twiki/bin/oops/TWiki/RichardDonkin?template=oopsmore¶m1=1.2¶m2=1.2 HTTP/1.1\" 200 11281\r\n"
"64.242.88.10 - - [07/Mar/2004:17:46:17 -0800] \"GET /twiki/bin/rdiff/TWiki/AlWilliams?rev1=1.3&rev2=1.2 HTTP/1.1\" 200 4485\r\n"
"64.242.88.10 - - [07/Mar/2004:17:47:43 -0800] \"GET /twiki/bin/rdiff/TWiki/AlWilliams?rev1=1.2&rev2=1.1 HTTP/1.1\" 200 5234\r\n"
"64.242.88.10 - - [07/Mar/2004:17:50:44 -0800] \"GET\"X\"/twiki/bin/view/TWiki/SvenDowideit\"X\"HTTP/1.1\" 200 3616\r\n"
"64.242.88.10 - - [07/Mar/2004:17:50:44 -0800] \"\"GET\"\"X\"\"/twiki/bin/view/TWiki/SvenDowideit\"\"X\"\"HTTP/1.1\"\" 200 3616\r\n"
"||A||||A||||A||X||A||||A||\r\n"
"||A||||A||||A||X||A||||A||\r\n"
"||A||||A||||A||X||A||||A||\r\n"
"64.242.88.10 - - [07/Mar/2004:17:53:45 -0800] \"GET /twiki/bin/search/Main/SearchResult?scope=text®ex=on&search=Office%20*Locations[^A-Za-z] HTTP/1.1\" 200 7771\r"
;

static const std::string JSON1 = R"({
    "line": {
        "start": 0,
        "stop": 1,
        "color": "RED"
    },
    "lock": {
        "on": true
    },
    "line": {
        "start": 0,
        "stop": 3,
        "color": "GREEN"
    },
    "num": {
        "color": "MAGENTA",
        "count": 5
    },
    "string": {
        "word1": "GET",
        "color": "BLUE",
        "count": 0
    },
    "rstring": {
        "word1": "00",
        "color": "BG_BLUE",
        "count": 1
    },
    "range": {
        "word1": "[",
        "word2": "]",
        "inclusive": false,
        "color": "BLUE",
        "count": 1
    },
    "between": {
        "word1": "\"",
        "word2": "\"",
        "inclusive": true,
        "color": "BG_GREEN"
    },
    "range": {
        "word1": "||",
        "word2": "||",
        "inclusive": false,
        "color": "BG_BOLD_YELLOW",
        "count": 0
    }
}
)";

static const std::string JSON2 = R"({
    "line": {
        "start": 0,
        "stop": 1,
        "color": "RED"
    },
    "range": {
        "word1": "\"",
        "word2": "\"",
        "inclusive": false,
        "color": "BLUE",
        "count": 0
    },
    "between": {
        "word1": "[",
        "word2": "]",
        "inclusive": false,
        "color": "BG_GREEN"
    }
}
)";

static const std::string JSON3 = R"({
    "range": {
        "word1": "[",
        "word2": "]",
        "inclusive": true,
        "color": "BOLD_BLUE",
        "count": 1
    },
    "between": {
        "word1": "\"",
        "word2": "\"",
        "inclusive": true,
        "color": "BOLD_GREEN"
    },
    "lock": {
        "on": true
    },
    "num": {
        "color": "BG_RED"
    },
    "custom": {
        "word1": "|A|",
        "color": "BG_BOLD_CYAN"
    }
}
)";

class MyDisplay : public LogDisplay {
public:
    MyDisplay(int X, int Y, int W, int H) : LogDisplay(X, Y, W, H, nullptr) {
    }

    void line_cb(size_t row, const std::string& line) override {
        // fprintf(stderr, "%5d: %5d: |%s|\n", (int) row, (int) line.length(), line.c_str());

        if (row < 3) {
            auto c = logdisplay::COLOR::FOREGROUND;

            for (int f = 0; f < 96; f += 3) {
                style_line(f, f + 2, c);

                if (c < logdisplay::COLOR::LAST) {
                    c = (logdisplay::COLOR) ((int) c + 1);
                }
                else {
                    c = logdisplay::COLOR::FOREGROUND;
                }
            }

            style_line(96, line.length() - 2, logdisplay::COLOR::RED);
        }
        else if (row == 3) {
            style_string(line, "Hello", logdisplay::COLOR::BOLD_RED);
            style_string(line, "World", logdisplay::COLOR::BOLD_BLUE);
            style_string(line, "", logdisplay::COLOR::BG_BOLD_MAGENTA);
        }
        else if (row == 4) {
            style_string(line, "Hello", logdisplay::COLOR::BOLD_RED, 5);
            style_string(line, "World", logdisplay::COLOR::BOLD_BLUE, 1);
        }
        else if (row == 5) {
            style_rstring(line, "Hello", logdisplay::COLOR::BOLD_RED, 5);
            style_rstring(line, "World", logdisplay::COLOR::BOLD_BLUE, 3);
        }
        else if (row == 6) {
            style_range(line, "\"", "\"", false, logdisplay::COLOR::GREEN);
            style_range(line, "[", "]", false, logdisplay::COLOR::BOLD_MAGENTA);
            style_num(line, logdisplay::COLOR::RED);
        }
        else if (row == 7) {
            lock_colors(true);
            style_range(line, "\"", "\"", false, logdisplay::COLOR::GREEN);
            style_range(line, "[", "]", false, logdisplay::COLOR::BOLD_MAGENTA);
            style_num(line, logdisplay::COLOR::RED);
        }
        else if (row == 8) {
            style_range(line, "[", "]", true, logdisplay::COLOR::YELLOW);
            style_num(line, logdisplay::COLOR::RED, 2);
        }
        else if (row == 9) {
            style_between(line, "[", "]", false, logdisplay::COLOR::BLUE);
            style_num(line, logdisplay::COLOR::RED, 5);
        }
        else if (row == 61) {
            style_range(line, "\"", "\"", false, logdisplay::COLOR::YELLOW);
        }
        else if (row == 62) {
            style_range(line, "\"\"", "\"\"", true, logdisplay::COLOR::RED, 2);
        }
        else if (row == 63) {
            style_range(line, "||", "||", false, logdisplay::COLOR::RED);
        }
        else if (row == 64) {
            style_range(line, "||", "||", true, logdisplay::COLOR::RED);
        }
        else if (row == 65) {
            style_range(line, "||", "||", true, logdisplay::COLOR::RED, 1);
        }
        else {
            style_string(line, "", logdisplay::COLOR::BG_BOLD_MAGENTA);
            style_rstring(line, "", logdisplay::COLOR::BG_BOLD_MAGENTA);
            style_range(line, "", "", false, logdisplay::COLOR::BG_BOLD_MAGENTA);
            style_between(line, "", "", false, logdisplay::COLOR::BG_BOLD_MAGENTA);

            style_range(line, "[", "]", false, logdisplay::COLOR::BG_BOLD_CYAN);
            style_range(line, "\"", "\"", false, logdisplay::COLOR::GREEN);
            style_num(line, logdisplay::COLOR::MAGENTA);
        }
    }

    void line_custom_cb(size_t row, const std::string& line, const std::string& word1, const std::string& word2, logdisplay::COLOR color, bool inclusive, size_t start = 0, size_t stop = 0, size_t count = 0) override {
        (void) row;
        (void) word2;
        (void) inclusive;
        (void) start;
        (void) stop;
        (void) count;

        auto pos = line.find(word1);

        while (pos != std::string::npos) {
            style_line(pos, pos + word1.length() - 1, color);
            pos = line.find(word1, pos + word1.length());
        }
    }
};

class Test : public Fl_Double_Window {
public:
    Test(int W, int H, const char* l) : Fl_Double_Window(W, H, l) {
        b1  = new Fl_Button(0, 0, 0, 0, "By Callback");
        b2  = new Fl_Button(0, 0, 0, 0, "JSON String 1");
        b3  = new Fl_Button(0, 0, 0, 0, "JSON String 2");
        b4  = new Fl_Button(0, 0, 0, 0, "JSON String 3");
        log = new MyDisplay(0, 0, 0, 0);

        b1->callback(Test::Callback, this);
        b2->callback(Test::Callback, this);
        b3->callback(Test::Callback, this);
        b4->callback(Test::Callback, this);
        log->value(TEXT);
        log->style();
        util::labelfont(this);
        resizable(this);
        size_range(64, 48);
    }

    static void Callback(Fl_Widget* w, void* o) {
        auto test = (Test*) o;

        test->log->lock_colors(false);

        if (w == test->b1) {
            test->log->style();
        }
        else if (w == test->b2) {
            test->log->style(JSON1);
        }
        else if (w == test->b3) {
            test->log->style(JSON2);
        }
        else if (w == test->b4) {
            test->log->style(JSON3);
        }
    }

    void resize(int X, int Y, int W, int H) override {
        Fl_Double_Window::resize(X, Y, W, H);
        b1->resize(0, 0, PREF_FONTSIZE * 10, PREF_FONTSIZE * 2);
        b2->resize(PREF_FONTSIZE * 10, 0, PREF_FONTSIZE * 10, PREF_FONTSIZE * 2);
        b3->resize(PREF_FONTSIZE * 20, 0, PREF_FONTSIZE * 10, PREF_FONTSIZE * 2);
        b4->resize(PREF_FONTSIZE * 30, 0, PREF_FONTSIZE * 10, PREF_FONTSIZE * 2);
        log->resize(0, PREF_FONTSIZE * 2, W, H - PREF_FONTSIZE * 2);
    }

    LogDisplay* log;
    Fl_Button*       b1;
    Fl_Button*       b2;
    Fl_Button*       b3;
    Fl_Button*       b4;
};

int main(int argc, const char** argv) {
    if (theme::parse(argc, argv) == false) {
        theme::load("gtk");
    }

    Test win(800, 480, "test_logdisplay.cpp");
    win.show();
    return Fl::run();
}
