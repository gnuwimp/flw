/**
* @file
* @brief Assorted utility stuff and some global values.
*
* @author gnuwimp@gmail.com
* @copyright Released under the GNU General Public License v3.0
*/

#include "flw.h"
#include "waitcursor.h"

// MKALGAM_ON

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdarg>
#include <FL/Fl_File_Chooser.H>
#include <FL/fl_ask.H>
#include <FL/Fl_SVG_Image.H>

#ifdef _WIN32
    #include <windows.h>
#else
    #include <unistd.h>
#endif

#ifdef FLW_USE_PNG
    #include <FL/Fl_PNG_Image.H>
    #include <FL/fl_draw.H>
#endif

/*
 *       __ _
 *      / _| |
 *     | |_| |_      __
 *     |  _| \ \ /\ / /
 *     | | | |\ V  V /
 *     |_| |_| \_/\_/
 *
 *
 */

namespace flw {

std::vector<char*>  PREF_FONTNAMES;
int                 PREF_FIXED_FONT         = FL_COURIER;
std::string         PREF_FIXED_FONTNAME     = "FL_COURIER";
int                 PREF_FIXED_FONTSIZE     = 14;
int                 PREF_FONT               = FL_HELVETICA;
int                 PREF_FONTSIZE           = 14;
std::string         PREF_FONTNAME           = "FL_HELVETICA";
double              PREF_SCALE_VAL          = 1.0;
bool                PREF_SCALE_ON           = true;
std::string         PREF_THEME              = "default";
Fl_Window*          PREF_WINDOW             = nullptr;

const StringVector PREF_THEMES = {
    "default",
    "gleam",
    "blue gleam",
    "dark gleam",
    "tan gleam",
    "gtk",
    "blue gtk",
    "dark gtk",
    "tan gtk",
    "oxy",
    "tan oxy",
    "plastic",
    "tan plastic",
};

} // flw

/*
 *          _      _
 *         | |    | |
 *       __| | ___| |__  _   _  __ _
 *      / _` |/ _ \ '_ \| | | |/ _` |
 *     | (_| |  __/ |_) | |_| | (_| |
 *      \__,_|\___|_.__/ \__,_|\__, |
 *                              __/ |
 *                             |___/
 */

/** @brief Print widget sizes and labels.
*
* @param[in] widget     Valid widget.
* @param[in] recursive  True to print recursive if input widget is a group widget.
*
*/
void flw::debug::print(const Fl_Widget* widget, bool recursive) {
    std::string indent;
    flw::debug::print(widget, indent, recursive);
}

/** @brief Print widget sizes and labels.
*
* @param[in] widget     Widget pointer, can be NULL.
* @param[in] indent     Indentation string.
* @param[in] recursive  True to print recursive if input widget is a group widget.
*
*/
void flw::debug::print(const Fl_Widget* widget, std::string& indent, bool recursive) {
    if (widget == nullptr) {
        puts("flw::debug::print() => null widget");
    }
    else {
        printf("%sx=%4d, y=%4d, w=%4d, h=%4d, X=%4d, Y=%4d, %c, \"%s\"\n",
            indent.c_str(),
            widget->x(),
            widget->y(),
            widget->w(),
            widget->h(),
            widget->x() + widget->w(),
            widget->y() + widget->h(),
            widget->visible() ? 'V' : 'H',
            widget->label() ? widget->label() : "NULL"
        );
        auto group = widget->as_group();

        if (group != nullptr && recursive == true) {
            indent += "\t";

            for (int f = 0; f < group->children(); f++) {
                flw::debug::print(group->child(f), indent);
            }

            indent.pop_back();
        }
    }

    fflush(stdout);
}

/** @brief Test value and print to stderr for error.
*
* @param[in] val   Test that value is true.
* @param[in] line  Line number.
* @param[in] func  Function name.
*
* @return Return true if ok.
*/
bool flw::debug::test(bool val, int line, const char* func) {
    if (val == false) {
        fprintf(stderr, "Error: test failed at line %d in %s\n", line, func);
        fflush(stderr);
        return false;
    }

    return true;
}

/** @brief Test value and print to stderr for error.
*
* @param[in] ref   Reference string, can be NULL.
* @param[in] val   Test string, can be NULL.
* @param[in] line  Line number.
* @param[in] func  Function name.
*
* @return Return true if ok.
*/
bool flw::debug::test(const char* ref, const char* val, int line, const char* func) {
    if (ref == nullptr && val == nullptr) {
        return true;
    }
    else if (ref == nullptr || val == nullptr || strcmp(ref, val) != 0) {
        fprintf(stderr, "Error: test failed '%s' != '%s' at line %d in %s\n", ref ? ref : "NULL", val ? val : "NULL", line, func);
        fflush(stderr);
        return false;
    }

    return true;
}

/** @brief Test value and print to stderr for error.
*
* @param[in] ref   Reference number.
* @param[in] val   Test number.
* @param[in] line  Line number.
* @param[in] func  Function name.
*
* @return Return true if ok.
*/
bool flw::debug::test(int64_t ref, int64_t val, int line, const char* func) {
    if (ref != val) {
        fprintf(stderr, "Error: test failed '%lld' != '%lld' at line %d in %s\n", (long long int) ref, (long long int) val, line, func);
        fflush(stderr);
        return false;
    }

    return true;
}

/** @brief Test value and print to stderr for error.
*
* @param[in] ref   Reference number.
* @param[in] val   Test number.
* @param[in] diff  Number range to compare as equal.
* @param[in] line  Line number.
* @param[in] func  Function name.
*
* @return Return true if ok.
*/
bool flw::debug::test(double ref, double val, double diff, int line, const char* func) {
    if (fabs(ref - val) > diff) {
        fprintf(stderr, "Error: test failed '%f' != '%f' at line %d in %s\n", ref, val, line, func);
        fflush(stderr);
        return false;
    }

    return true;
}

/*
 *      _
 *     (_)
 *      _  ___ ___  _ __  ___
 *     | |/ __/ _ \| '_ \/ __|
 *     | | (_| (_) | | | \__ \
 *     |_|\___\___/|_| |_|___/
 *
 *
 */

std::string flw::icons::ALERT = R"(
<?xml version="1.0" encoding="iso-8859-1"?>
<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg height="800px" width="800px" version="1.1" id="Layer_1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink"
	 viewBox="0 0 506.4 506.4" xml:space="preserve">
<circle style="fill:#DF5C4E;" cx="253.2" cy="253.2" r="249.2"/>
<g>
	<path style="fill:#F4EFEF;" d="M253.2,332.4c-10.8,0-20-8.8-20-19.6v-174c0-10.8,9.2-19.6,20-19.6s20,8.8,20,19.6v174
		C273.2,323.6,264,332.4,253.2,332.4z"/>
	<path style="fill:#F4EFEF;" d="M253.2,395.6c-5.2,0-10.4-2-14-5.6s-5.6-8.8-5.6-14s2-10.4,5.6-14s8.8-6,14-6s10.4,2,14,6
		c3.6,3.6,6,8.8,6,14s-2,10.4-6,14C263.6,393.6,258.4,395.6,253.2,395.6z"/>
</g>
<path d="M253.2,506.4C113.6,506.4,0,392.8,0,253.2S113.6,0,253.2,0s253.2,113.6,253.2,253.2S392.8,506.4,253.2,506.4z M253.2,8
	C118,8,8,118,8,253.2s110,245.2,245.2,245.2s245.2-110,245.2-245.2S388.4,8,253.2,8z"/>
<path d="M249.2,336.4c-13.2,0-24-10.8-24-23.6v-174c0-13.2,10.8-23.6,24-23.6s24,10.8,24,23.6v174
	C273.2,325.6,262.4,336.4,249.2,336.4z M249.2,122.8c-8.8,0-16,7.2-16,15.6v174c0,8.8,7.2,15.6,16,15.6s16-7.2,16-15.6v-174
	C265.2,130,258,122.8,249.2,122.8z"/>
<path d="M249.2,399.6c-6.4,0-12.4-2.4-16.8-6.8c-4.4-4.4-6.8-10.4-6.8-16.8s2.4-12.4,6.8-16.8c4.4-4.4,10.8-6.8,16.8-6.8
	c6.4,0,12.4,2.4,16.8,6.8c4.4,4.4,6.8,10.4,6.8,16.8s-2.4,12.4-7.2,16.8C261.6,397.2,255.6,399.6,249.2,399.6z M249.2,360
	c-4,0-8.4,1.6-11.2,4.8c-2.8,2.8-4.4,6.8-4.4,11.2c0,4,1.6,8.4,4.8,11.2c2.8,2.8,7.2,4.8,11.2,4.8s8.4-1.6,11.2-4.8
	c2.8-2.8,4.8-7.2,4.8-11.2s-1.6-8.4-4.8-11.2C257.2,361.6,253.2,360,249.2,360z"/>
</svg>
)";

std::string flw::icons::BACK = R"(
<?xml version="1.0" encoding="iso-8859-1"?>
<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg height="800px" width="800px" version="1.1" id="Layer_1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink"
	 viewBox="0 0 511.999 511.999" xml:space="preserve">
<g>
	<path style="fill:#B3404A;" d="M382.795,358.347h-263.03c-7.958,0-14.411-6.452-14.411-14.411s6.453-14.411,14.411-14.411h263.029
		c55.352,0,100.384-45.032,100.384-100.384v-25.501c0-55.352-45.032-100.384-100.384-100.384h-84.939v42.04h84.939
		c7.958,0,14.411,6.452,14.411,14.411s-6.453,14.411-14.411,14.411h-99.349c-7.958,0-14.411-6.452-14.411-14.411V88.846
		c0-7.959,6.453-14.411,14.411-14.411h99.349c71.243,0,129.205,57.961,129.205,129.205v25.501
		C512,300.386,454.038,358.347,382.795,358.347z"/>
	<path style="fill:#B3404A;" d="M382.795,287.487H265.631c-7.958,0-14.411-6.452-14.411-14.411c0-7.959,6.453-14.411,14.411-14.411
		h117.164c15.728,0,29.523-13.797,29.523-29.523v-25.501c0-7.959,6.453-14.411,14.411-14.411s14.411,6.452,14.411,14.411v25.501
		C441.139,260.768,414.42,287.487,382.795,287.487z"/>
</g>
<polygon style="fill:#F4B2B0;" points="200.171,423.152 14.411,307.407 200.171,191.663 "/>
<path style="fill:#B3404A;" d="M200.171,437.563c-2.649,0-5.293-0.729-7.62-2.18L6.79,319.638C2.567,317.006,0,312.383,0,307.407
	s2.567-9.599,6.79-12.23l185.761-115.744c4.441-2.77,10.038-2.909,14.615-0.369c4.578,2.541,7.416,7.364,7.416,12.599v231.49
	c0,5.235-2.839,10.059-7.416,12.599C204.987,436.961,202.578,437.563,200.171,437.563z M41.661,307.407l144.1,89.786V217.62
	L41.661,307.407z"/>
</svg>
)";

std::string flw::icons::CANCEL = R"(
<?xml version="1.0" encoding="UTF-8"?>
<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg width="800px" height="800px" enable-background="new 0 0 32 32" version="1.1" viewBox="0 0 32 32" xml:space="preserve" xmlns="http://www.w3.org/2000/svg">
<g id="Close">
<path d="m26.6 5.4c-2.8-2.8-6.6-4.4-10.6-4.4s-7.8 1.6-10.6 4.4-4.4 6.6-4.4 10.6 1.6 7.8 4.4 10.6 6.6 4.4 10.6 4.4 7.8-1.6 10.6-4.4 4.4-6.6 4.4-10.6-1.6-7.8-4.4-10.6z" fill="#FE9803"/>
<path d="m17.4 16 5-5c0.2-0.2 0.3-0.5 0.3-0.7 0-0.3-0.1-0.5-0.3-0.7-0.4-0.4-1-0.4-1.4 0l-5 4.9-5-4.9c-0.4-0.4-1-0.4-1.4 0-0.2 0.2-0.3 0.4-0.3 0.7s0.1 0.5 0.3 0.7l5 5-5 5c-0.2 0.2-0.3
0.4-0.3 0.7s0.1 0.5 0.3 0.7 0.5 0.3 0.7 0.3c0.3 0 0.5-0.1 0.7-0.3l5-5 5 5c0.2 0.2 0.5 0.3 0.7 0.3 0.3 0 0.5-0.1 0.7-0.3s0.3-0.5 0.3-0.7c0-0.3-0.1-0.5-0.3-0.7l-5-5z" fill="#673AB7"/>
</g>
</svg>
)";

std::string flw::icons::CONFIRM = R"(
<?xml version="1.0" encoding="iso-8859-1"?>
<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg height="800px" width="800px" version="1.1" id="Layer_1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink"
	 viewBox="0 0 506.4 506.4" xml:space="preserve">
<circle style="fill:#54B265;" cx="253.2" cy="253.2" r="249.2"/>
<path style="fill:#F4EFEF;" d="M372.8,200.4l-11.2-11.2c-4.4-4.4-12-4.4-16.4,0L232,302.4l-69.6-69.6c-4.4-4.4-12-4.4-16.4,0
	L134.4,244c-4.4,4.4-4.4,12,0,16.4l89.2,89.2c4.4,4.4,12,4.4,16.4,0l0,0l0,0l10.4-10.4l0.8-0.8l121.6-121.6
	C377.2,212.4,377.2,205.2,372.8,200.4z"/>
<path d="M253.2,506.4C113.6,506.4,0,392.8,0,253.2S113.6,0,253.2,0s253.2,113.6,253.2,253.2S392.8,506.4,253.2,506.4z M253.2,8
	C118,8,8,118,8,253.2s110,245.2,245.2,245.2s245.2-110,245.2-245.2S388.4,8,253.2,8z"/>
<path d="M231.6,357.2c-4,0-8-1.6-11.2-4.4l-89.2-89.2c-6-6-6-16,0-22l11.6-11.6c6-6,16.4-6,22,0l66.8,66.8L342,186.4
	c2.8-2.8,6.8-4.4,11.2-4.4c4,0,8,1.6,11.2,4.4l11.2,11.2l0,0c6,6,6,16,0,22L242.8,352.4C239.6,355.6,235.6,357.2,231.6,357.2z
	 M154,233.6c-2,0-4,0.8-5.6,2.4l-11.6,11.6c-2.8,2.8-2.8,8,0,10.8l89.2,89.2c2.8,2.8,8,2.8,10.8,0l132.8-132.8c2.8-2.8,2.8-8,0-10.8
	l-11.2-11.2c-2.8-2.8-8-2.8-10.8,0L234.4,306c-1.6,1.6-4,1.6-5.6,0l-69.6-69.6C158,234.4,156,233.6,154,233.6z"/>
</svg>
)";

std::string flw::icons::DEL = R"(
<?xml version="1.0" encoding="iso-8859-1"?>
<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg height="800px" width="800px" version="1.1" id="Layer_1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink"
	 viewBox="0 0 506.4 506.4" xml:space="preserve">
<circle style="fill:#DF5C4E;" cx="253.2" cy="253.2" r="249.2"/>
<path style="fill:#F4EFEF;" d="M373.2,244.8c0-6.4-5.2-11.6-11.6-11.6H140.8c-6.4,0-11.6,5.2-11.6,11.6v16.8
	c0,6.4,5.2,11.6,11.6,11.6h220.8c6.4,0,11.6-5.2,11.6-11.6V244.8z"/>
<path d="M253.2,506.4C113.6,506.4,0,392.8,0,253.2S113.6,0,253.2,0s253.2,113.6,253.2,253.2S392.8,506.4,253.2,506.4z M253.2,8
	C118,8,8,118,8,253.2s110,245.2,245.2,245.2s245.2-110,245.2-245.2S388.4,8,253.2,8z"/>
<path d="M357.6,277.2H136.8c-8.8,0-15.6-7.2-15.6-15.6v-16.8c0-8.8,7.2-15.6,15.6-15.6h220.8c8.8,0,15.6,7.2,15.6,15.6v16.8
	C373.2,270,366,277.2,357.6,277.2z M136.8,237.2c-4.4,0-7.6,3.6-7.6,7.6v16.8c0,4.4,3.6,7.6,7.6,7.6h220.8c4.4,0,7.6-3.6,7.6-7.6
	v-16.8c0-4.4-3.6-7.6-7.6-7.6H136.8z"/>
</svg>
)";

std::string flw::icons::DOWN = R"(
<?xml version="1.0" encoding="utf-8"?><!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg width="800px" height="800px" viewBox="0 0 48 48" fill="none" xmlns="http://www.w3.org/2000/svg">
<rect width="48" height="48" fill="white" fill-opacity="0.01"/>
<path d="M7.05322 10.0003L29.0532 10.0003V4.00032L7.05322 4.00032V10.0003Z" fill="#2F88FF" stroke="#000000" stroke-width="4" stroke-linejoin="round"/>
<path fill-rule="evenodd" clip-rule="evenodd" d="M29.0531 10.0003C35.5721 17.1232 39.3127 21.2512 40.2749 22.3841C41.7183 24.0835 41.1122 26.0043 37.5001 26.0043C33.8879 26.0043 31.8047 20.7189 29.0531 20.7189C29.0367 20.7155 29.0356 27.4746 29.0498 40.9963C29.0515 42.6536 27.7094 43.9986 26.0521 44.0003L26.0489 44.0003C24.3898 44.0003 23.0447 42.6553 23.0447 40.9962V32.9867C15.072 31.7779 10.7374 31.111 10.0411 30.9859C8.99656 30.7984 7.05308 29.8014 7.05308 26.9323C7.05308 25.0196 7.05308 20.0423 7.05308 10.0003L29.0531 10.0003Z" stroke="#000000" stroke-width="4" stroke-linejoin="round"/>
</svg>
)";

std::string flw::icons::EDIT = R"(
<?xml version="1.0" encoding="utf-8"?>
<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg width="800px" height="800px" viewBox="0 0 1024 1024" class="icon"  version="1.1" xmlns="http://www.w3.org/2000/svg">
<path d="M823.3 938.8H229.4c-71.6 0-129.8-58.2-129.8-129.8V215.1c0-71.6 58.2-129.8 129.8-129.8h297c23.6 0 42.7 19.1 42.7 42.7s-19.1 42.7-42.7 42.7h-297c-24.5 0-44.4 19.9-44.4
44.4V809c0 24.5 19.9 44.4 44.4 44.4h593.9c24.5 0 44.4-19.9 44.4-44.4V512c0-23.6 19.1-42.7 42.7-42.7s42.7 19.1 42.7 42.7v297c0 71.6-58.2 129.8-129.8 129.8z" fill="#3688FF" />
<path d="M483 756.5c-1.8 0-3.5-0.1-5.3-0.3l-134.5-16.8c-19.4-2.4-34.6-17.7-37-37l-16.8-134.5c-1.6-13.1 2.9-26.2 12.2-35.5l374.6-374.6c51.1-51.1 134.2-51.1 185.3 0l26.3 26.3c24.8
24.7 38.4 57.6 38.4 92.7 0 35-13.6 67.9-38.4 92.7L513.2 744c-8.1 8.1-19 12.5-30.2 12.5z m-96.3-97.7l80.8 10.1 359.8-359.8c8.6-8.6 13.4-20.1 13.4-32.3 0-12.2-4.8-23.7-13.4-32.3L801 218.2c-17.9-17.8-46.8-17.8-64.6 0L376.6 578l10.1 80.8z" fill="#5F6379" /></svg>
)";

std::string flw::icons::ERR = R"(
<?xml version="1.0" encoding="UTF-8"?>
<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg width="800px" height="800px" version="1.1" viewBox="0 0 512 512" xml:space="preserve" xmlns="http://www.w3.org/2000/svg">
<circle cx="256" cy="256" r="245.8" fill="#FF757C"/>
<polygon points="395.56 164.04 347.96 116.44 256 208.4 164.04 116.44 116.44 164.04 208.4 256 116.44 347.96 164.04 395.56 256 303.6 347.96 395.56 395.56 347.96 303.6 256" fill="#F2F2F2"/>
<g fill="#4D4D4D">
	<path d="M256,512c-68.38,0-132.667-26.628-181.02-74.98S0,324.38,0,256S26.628,123.333,74.98,74.98   S187.62,0,256,0s132.667,26.628,181.02,74.98S512,187.62,512,256s-26.628,132.667-74.98,181.02S324.38,512,256,512z M256,20.398   C126.089,20.398,20.398,126.089,20.398,256S126.089,491.602,256,491.602S491.602,385.911,491.602,256S385.911,20.398,256,20.398z"/>
	<path d="m347.96 405.76c-2.61 0-5.221-0.996-7.212-2.987l-84.75-84.75-84.749 84.75c-3.983 3.982-10.441 3.982-14.425 0l-47.599-47.599c-3.983-3.983-3.983-10.441 0-14.425l84.751-84.748-84.75-84.749c-3.983-3.983-3.983-10.441 0-14.425l47.599-47.599c3.983-3.982 10.441-3.982 14.425 0l84.748 84.751 84.749-84.75c3.983-3.982 10.441-3.982 14.425 0l47.599
	47.599c3.983 3.983 3.983 10.441 0 14.425l-84.751 84.748 84.75 84.749c3.983 3.983 3.983 10.441 0 14.425l-47.599 47.599c-1.991 1.991-4.601 2.986-7.211 2.986zm-91.962-112.36c2.61 0 5.221 0.996 7.212 2.987l84.749 84.75 33.175-33.175-84.75-84.749c-3.983-3.983-3.983-10.441 0-14.425l84.75-84.749-33.175-33.175-84.749 84.75c-3.983 3.982-10.441 3.982-14.425
	0l-84.749-84.75-33.175 33.175 84.75 84.749c3.983 3.983 3.983 10.441 0 14.425l-84.75 84.749 33.175 33.175 84.749-84.75c1.992-1.99 4.603-2.987 7.213-2.987z"/>
</g>
</svg>
)";

std::string flw::icons::FORWARD = R"(
<?xml version="1.0" encoding="iso-8859-1"?>
<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg height="800px" width="800px" version="1.1" id="Layer_1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink"
	 viewBox="0 0 512 512" xml:space="preserve">
<g>
	<path style="fill:#B3404A;" d="M392.235,358.347h-263.03C57.962,358.347,0,300.386,0,229.143v-25.501
		C0,132.399,57.962,74.437,129.205,74.437h99.349c7.958,0,14.411,6.453,14.411,14.411v70.861c0,7.958-6.453,14.411-14.411,14.411
		h-99.349c-7.958,0-14.411-6.453-14.411-14.411c0-7.958,6.453-14.411,14.411-14.411h84.939v-42.041h-84.939
		c-55.352,0-100.384,45.032-100.384,100.384v25.501c0,55.351,45.032,100.383,100.384,100.383h263.029
		c7.958,0,14.411,6.453,14.411,14.411S400.193,358.347,392.235,358.347z"/>
	<path style="fill:#B3404A;" d="M246.369,287.486H129.205c-31.625,0-58.344-26.717-58.344-58.343v-25.501
		c0-7.958,6.453-14.411,14.411-14.411s14.411,6.453,14.411,14.411v25.501c0,15.726,13.795,29.521,29.523,29.521h117.164
		c7.958,0,14.411,6.453,14.411,14.411C260.779,281.033,254.326,287.486,246.369,287.486z"/>
</g>
<polygon style="fill:#F4B2B0;" points="311.829,423.152 497.589,307.407 311.829,191.663 "/>
<path style="fill:#B3404A;" d="M311.829,437.563c-2.407,0-4.816-0.602-6.995-1.811c-4.578-2.542-7.416-7.364-7.416-12.599v-231.49
	c0-5.235,2.839-10.057,7.416-12.599c4.575-2.541,10.172-2.401,14.615,0.369L505.21,295.177c4.224,2.631,6.79,7.254,6.79,12.23
	c0,4.976-2.567,9.599-6.79,12.23L319.449,435.381C317.122,436.834,314.477,437.563,311.829,437.563z M326.239,217.62v179.574
	l144.1-89.788L326.239,217.62z"/>
</svg>
)";

std::string flw::icons::INFO = R"(
<?xml version="1.0" encoding="UTF-8"?>
<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg width="800px" height="800px" version="1.1" viewBox="0 0 512 512" xml:space="preserve" xmlns="http://www.w3.org/2000/svg">
<circle cx="256" cy="256" r="245.8" fill="#BCC987"/>
<g fill="#FFE6B8">
	<path d="m256 411.54c-14.082 0-25.498-11.416-25.498-25.498v-177.47c0-14.082 11.416-25.498 25.498-25.498s25.498 11.416 25.498 25.498v177.47c0 14.082-11.416 25.498-25.498 25.498z"/>
	<path d="m256 151.46c-6.711 0-13.28-2.723-18.033-7.466-4.742-4.753-7.465-11.321-7.465-18.032s2.723-13.29 7.465-18.033c4.753-4.742 11.322-7.465 18.033-7.465s13.279 2.723 18.032 7.465c4.743 4.743 7.466 11.322 7.466 18.033s-2.723 13.279-7.466 18.032c-4.753 4.743-11.321 7.466-18.032 7.466z"/>
</g>
<g fill="#4D3D36">
	<path d="M256,512c-68.381,0-132.667-26.628-181.019-74.981C26.628,388.667,0,324.381,0,256   S26.628,123.333,74.981,74.981C123.333,26.628,187.619,0,256,0s132.667,26.628,181.019,74.981C485.372,123.333,512,187.619,512,256   s-26.628,132.667-74.981,181.019C388.667,485.372,324.381,512,256,512z M256,20.398C126.089,20.398,20.398,126.089,20.398,256   S126.089,491.602,256,491.602S491.602,385.911,491.602,256S385.911,20.398,256,20.398z"/>


	<path d="m256 421.74c-19.683 0-35.697-16.014-35.697-35.697v-177.47c0-19.683 16.014-35.697 35.697-35.697s35.697 16.014 35.697 35.697v177.47c0 19.683-16.014 35.697-35.697 35.697zm0-228.46c-8.436 0-15.299 6.863-15.299 15.299v177.47c0 8.436 6.863 15.299 15.299 15.299s15.299-6.863 15.299-15.299v-177.47c0-8.436-6.863-15.299-15.299-15.299z"/>
	<path d="m256 161.66c-9.386 0-18.585-3.807-25.237-10.446-6.654-6.668-10.46-15.867-10.46-25.251 0-9.4 3.809-18.6 10.451-25.244 6.662-6.647 15.861-10.453 25.246-10.453 9.384 0 18.583 3.806 25.235 10.444 6.653 6.652 10.462 15.853 10.462 25.253 0 9.385-3.807 18.584-10.446 25.236-6.667 6.654-15.866 10.461-25.251 10.461zm0-50.996c-4.025 0-7.972 1.636-10.829 4.486-2.836 2.837-4.469 6.781-4.469 10.813 0 4.024 1.636 7.971 4.486 10.829 2.841 2.834 6.788 4.47 10.813 4.47 4.024 0 7.97-1.636 10.827-4.486 2.835-2.843 4.471-6.789 4.471-10.813 0-4.031-1.633-7.974-4.479-10.82-2.849-2.843-6.796-4.479-10.82-4.479z"/>
</g>
</svg>
)";

std::string flw::icons::LEFT = R"(
<?xml version="1.0" encoding="utf-8"?><!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg width="800px" height="800px" viewBox="0 0 48 48" fill="none" xmlns="http://www.w3.org/2000/svg">
<rect width="48" height="48" fill="white" fill-opacity="0.01"/>
<path d="M44 40.9998V18.9998H38V40.9998H44Z" fill="#2F88FF" stroke="#000000" stroke-width="4" stroke-linejoin="round"/>
<path fill-rule="evenodd" clip-rule="evenodd" d="M38 19C30.8948 12.4302 26.7757 8.66342 25.6428 7.69966C23.9433 6.25403 22.0226 6.86106 22.0226 10.4789C22.0226 14.0967 27.2864 16.2441 27.2864 19C27.2898 19.0164 20.529 19.0175 7.00404 19.0033C5.3467 19.0015 4.00175 20.3437 4 22.001C4 22.0021 4 22.0031 4 22.0042C4 23.6633 5.34501 25.0083 7.00417 25.0083H14.0165C15.2234 32.9769 15.8893 37.3099 16.0144 38.0073C16.2019 39.0535 17.199 41 20.068 41C21.9807 41 27.9581 41 38 41V19Z" stroke="#000000" stroke-width="4" stroke-linejoin="round"/>
</svg>
)";

std::string flw::icons::PASSWORD = R"(
<?xml version="1.0" encoding="utf-8"?>
<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg width="800px" height="800px" viewBox="0 0 1024 1024" class="icon"  version="1.1" xmlns="http://www.w3.org/2000/svg"><path d="M512.340992 730.22464c28.653568 0
51.883008-23.228416 51.883008-51.883008 0-28.653568-23.22944-51.881984-51.883008-51.881984-28.653568 0-51.883008 23.228416-51.883008 51.881984 0 28.654592 23.22944
51.883008 51.883008 51.883008zM97.28 591.720448c0-76.327936 62.0544-138.203136 138.476544-138.203136h553.168896c76.478464 0 138.476544 62.006272 138.476544
138.203136v190.537728c0 76.326912-62.0544 138.203136-138.476544 138.203136H235.756544C159.27808 920.461312 97.28 858.454016 97.28 782.258176V591.720448z"
fill="#FFF200" /><path d="M661.95456 171.87328C622.099456 128.39424 568.96512 104.448 512.342016 104.448c-56.623104 0-109.75744 23.94624-149.613568 67.426304-39.40352
42.984448-61.10208 100.01408-61.10208 160.584704h40.96C342.585344 229.318656 418.737152 145.408 512.340992 145.408c93.60384 0 169.756672 83.91168 169.756672
187.051008h40.96c0-60.5696-21.699584-117.600256-61.10208-160.58368M512.340992 730.22464c-28.653568 0-51.881984-23.228416-51.881984-51.883008 0-28.653568 23.228416-51.881984
51.881984-51.881984 28.654592 0 51.883008 23.228416 51.883008 51.881984 0 28.654592-23.228416 51.883008-51.883008 51.883008m276.584448-276.707328H235.75552C159.3344 453.517312
97.28 515.393536 97.28 591.720448v190.537728c0 76.196864 61.99808 138.203136 138.47552 138.203136h553.16992c76.42112 0 138.47552-61.876224 138.47552-138.203136V591.720448c0-76.19584-61.997056-138.203136-138.47552-138.203136M512.340992
771.18464c51.193856 0 92.843008-41.649152 92.843008-92.843008 0-51.192832-41.649152-92.841984-92.843008-92.841984s-92.841984 41.649152-92.841984 92.841984c0 51.193856 41.648128 92.843008 92.841984 92.843008m276.584448-276.707328c53.77024 0 97.51552 43.623424 97.51552 97.243136v190.537728c0 53.619712-43.74528 97.243136-97.51552 97.243136H235.75552c-53.77024 0-97.516544-43.6224-97.516544-97.243136V591.720448c0-53.619712 43.74528-97.243136 97.51552-97.243136h553.16992" fill="#000000" /></svg>
)";

std::string flw::icons::PAUSE = R"(
<?xml version="1.0" encoding="iso-8859-1"?>
<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg height="800px" width="800px" version="1.1" id="Layer_1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink"
	 viewBox="0 0 506.4 506.4" xml:space="preserve">
<path style="fill:#E07D46;" d="M502.4,253.2c0,137.6-111.6,249.2-249.2,249.2S4,390.8,4,253.2S115.6,4,253.2,4
	S502.4,115.6,502.4,253.2z"/>
<g>
	<path style="fill:#F4EFEF;" d="M225.2,145.2V364c0,6.4-4.8,13.2-11.2,13.2h-8.4c-6.4,0-8.4-6.8-8.4-13.2V142.4c0-6.4,2-9.2,8.4-9.2
		h8.4c6.4,0,10.8,5.6,10.8,12"/>
	<path style="fill:#F4EFEF;" d="M309.2,145.2V364c0,6.4-5.6,13.2-12,13.2h-8.4c-6.4,0-7.6-6.8-7.6-13.2V142.4c0-6.4,1.2-9.2,7.6-9.2
		h8.4c6.4,0,10.8,5.6,10.8,12"/>
</g>
<path d="M253.2,506.4C113.6,506.4,0,392.8,0,253.2S113.6,0,253.2,0s253.2,113.6,253.2,253.2S392.8,506.4,253.2,506.4z M253.2,8
	C118,8,8,118,8,253.2s110,245.2,245.2,245.2s245.2-110,245.2-245.2S388.4,8,253.2,8z"/>
<path d="M298.4,385.2h-9.2c-8.8,0-16-7.2-16-15.6V144.8c0-8.4,8-16,16.8-16h7.6c8.4,0,15.6,7.2,15.6,16v224.4
	C313.2,378.4,306.8,385.2,298.4,385.2z M290,137.2c-4.4,0-8.8,3.6-8.8,8V370c0,4,3.6,7.6,8,7.6h9.2c4.4,0,6.8-4,6.8-8V145.2
	c0-4.4-3.2-8-7.6-8H290z"/>
<path d="M216.4,383.2h-9.2c-8.8,0-16-7.2-16-15.6V142.8c0-8.4,8-16,16.8-16h7.6c8.4,0,15.6,7.2,15.6,16v224.4
	C231.2,376.4,224.8,383.2,216.4,383.2z M208,135.2c-4.4,0-8.8,3.6-8.8,8V368c0,4,3.6,7.6,8,7.6h9.2c4.4,0,6.8-4,6.8-8V143.2
	c0-4.4-3.2-8-7.6-8H208z"/>
</svg>
)";

std::string flw::icons::PLAY = R"(
<?xml version="1.0" encoding="utf-8"?>
<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg width="800px" height="800px" viewBox="0 0 32 32" xmlns="http://www.w3.org/2000/svg">
  <g id="Group_17" data-name="Group 17" transform="translate(-776 -253) ">
    <circle id="Ellipse_5" data-name="Ellipse 5" cx="15" cy="15" r="15" transform="translate(777 254) " fill="#e8f7f9" stroke="#333" stroke-linecap="round" stroke-linejoin="round" stroke-width="2"/>
    <path id="Path_20" data-name="Path 20" d="M788,274l10-5-10-5Z" fill="#ffc2c2" stroke="#333" stroke-linecap="round" stroke-linejoin="round" stroke-width="2"/>
  </g>
</svg>
)";

std::string flw::icons::QUESTION = R"(
<?xml version="1.0" encoding="iso-8859-1"?>
<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg height="800px" width="800px" version="1.1" id="Layer_1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink"
	 viewBox="0 0 506.4 506.4" xml:space="preserve">
<circle style="fill:#ACD9EA;" cx="253.2" cy="253.2" r="249.2"/>
<g>
	<path style="fill:#F4EFEF;" d="M253.2,332.4c-10.8,0-20-8.8-20-19.6v-36c0-10.8,8.8-19.6,20-19.6c28,0,51.2-22.8,51.2-50.8
		s-22.8-50.8-50.8-50.8s-50.8,22.8-50.8,50.8c0,10.8-8.8,19.6-19.6,19.6s-19.6-8.8-19.6-19.6c0-50,40.8-90.4,90.4-90.4
		s90.8,40.8,90.8,90.4c0,43.2-26.8,79.2-70.8,88.4v18.4C273.2,323.6,264,332.4,253.2,332.4z"/>
	<path style="fill:#F4EFEF;" d="M252.4,395.6c-5.2,0-10.4-2-14-6c-3.6-3.6-5.6-8.8-5.6-14s2-10.4,5.6-14s8.8-5.6,14-5.6
		s10.4,2,14,5.6s6,8.8,6,14s-2,10.4-6,14C262.4,393.6,257.6,395.6,252.4,395.6z"/>
</g>
<path d="M253.2,506.4C113.6,506.4,0,392.8,0,253.2S113.6,0,253.2,0s253.2,113.6,253.2,253.2S392.8,506.4,253.2,506.4z M253.2,8
	C118,8,8,118,8,253.2s110,245.2,245.2,245.2s245.2-110,245.2-245.2S388.4,8,253.2,8z"/>
<path d="M249.2,336.4c-13.2,0-24-10.8-24-23.6v-36c0-12.8,12-23.6,26-23.6c26,0,48-21.6,48-46.8c0-26-20.8-46.8-46.4-46.8
	c-25.6,0-46.8,21.2-46.8,46.8c0,13.2-10.8,23.6-23.6,23.6c-13.2,0-23.6-10.8-23.6-23.6c0-52,42.4-94.4,94.4-94.4
	s92.8,41.6,92.8,94.4c0,44.8-29.2,81.2-72.8,91.6v15.2C273.2,325.6,262.4,336.4,249.2,336.4z M252.8,150.8c30,0,54.4,24.8,54.4,54.8
	c0,29.6-25.6,54.8-56,54.8c-8.4,0-18,6.8-18,15.6v36c0,8.8,7.2,15.6,16,15.6s16-7.2,16-15.6v-18.4c0-2,1.2-3.6,3.2-4
	c41.6-8.4,69.6-42.4,69.6-84.4c0-48.4-37.2-86.4-84.8-86.4s-86.4,38.8-86.4,86.4c0,8.8,7.2,15.6,15.6,15.6c8.8,0,15.6-7.2,15.6-15.6
	C198,175.2,222.4,150.8,252.8,150.8z"/>
<path d="M252.4,399.6c-6.4,0-12.4-2.4-16.8-7.2c-4.4-4.4-6.8-10.4-6.8-16.8s2.4-12.4,6.8-16.8c4.4-4.4,10.4-6.8,16.8-6.8
	c6.4,0,12.4,2.4,16.8,6.8c4.4,4.4,6.8,10.8,6.8,16.8c0,6.4-2.4,12.4-6.8,16.8C264.8,397.2,258.4,399.6,252.4,399.6z M252.4,360
	c-4,0-8.4,1.6-11.2,4.8c-2.8,2.8-4.4,6.8-4.4,11.2s1.6,8.4,4.4,11.2c2.8,2.8,7.2,4.8,11.2,4.8s8.4-1.6,11.2-4.8
	c2.8-2.8,4.8-7.2,4.8-11.2s-1.6-8-4.8-11.2C260.4,361.6,256.4,360,252.4,360z"/>
</svg>
)";

std::string flw::icons::RIGHT = R"(
<?xml version="1.0" encoding="utf-8"?><!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg width="800px" height="800px" viewBox="0 0 48 48" fill="none" xmlns="http://www.w3.org/2000/svg">
<rect width="48" height="48" fill="white" fill-opacity="0.01"/>
<path d="M10.0266 40.9736L10.0266 18.9736H4.02661L4.02661 40.9736H10.0266Z" fill="#2F88FF" stroke="#000000" stroke-width="4" stroke-linejoin="round"/>
<path fill-rule="evenodd" clip-rule="evenodd" d="M10.0266 18.9736C17.1495 12.4546 21.2774 8.71403 22.4104 7.75178C24.1098 6.30839 26.0306 6.91448 26.0306 10.5266C26.0306 14.1388 20.7452 16.222 20.7452 18.9736C20.7418 18.99 27.5009 18.9911 41.0226 18.9769C42.6799 18.9752 44.0249 20.3173 44.0266 21.9746L44.0266 21.9778C44.0266 23.637 42.6816 24.982 41.0224 24.982H33.013C31.8042 32.9547 31.1373 37.2893 31.0122 37.9856C30.8247 39.0302 29.8276 40.9736 26.9586 40.9736C25.0459 40.9736 20.0686 40.9736 10.0266 40.9736V18.9736Z" stroke="#000000" stroke-width="4" stroke-linejoin="round"/>
</svg>
)";

std::string flw::icons::STOP = R"(
<?xml version="1.0" encoding="UTF-8"?>
<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg width="800px" height="800px" version="1.1" viewBox="0 0 506.4 506.4" xml:space="preserve" xmlns="http://www.w3.org/2000/svg">
<circle cx="253.2" cy="253.2" r="249.2" fill="#80b3ff"/>
<path d="m349.2 331.6c0 12-9.6 21.6-21.6 21.6h-152.8c-12 0-21.6-9.6-21.6-21.6v-152.8c0-12 9.6-21.6 21.6-21.6h152.8c12 0 21.6 9.6 21.6 21.6v152.8z" fill="#F4EFEF"/>
<path d="M253.2,506.4C113.6,506.4,0,392.8,0,253.2S113.6,0,253.2,0s253.2,113.6,253.2,253.2S392.8,506.4,253.2,506.4z M253.2,8  C118,8,8,118,8,253.2s110,245.2,245.2,245.2s245.2-110,245.2-245.2S388.4,8,253.2,8z"/>
<path d="m323.6 357.2h-152.8c-14 0-25.6-11.6-25.6-25.6v-152.8c0-14 11.6-25.6 25.6-25.6h152.8c14 0 25.6 11.6 25.6 25.6v152.8c0 14-11.6 25.6-25.6 25.6zm-152.8-196c-9.6 0-17.6 8-17.6 17.6v152.8c0 9.6 8 17.6 17.6 17.6h152.8c9.6 0 17.6-8 17.6-17.6v-152.8c0-9.6-8-17.6-17.6-17.6h-152.8z"/>
</svg>
)";

std::string flw::icons::UP = R"(
<?xml version="1.0" encoding="utf-8"?><!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg width="800px" height="800px" viewBox="0 0 48 48" fill="none" xmlns="http://www.w3.org/2000/svg">
<rect width="48" height="48" fill="white" fill-opacity="0.01"/>
<path d="M41 38.0001H19V44.0001H41V38.0001Z" fill="#2F88FF" stroke="#000000" stroke-width="4" stroke-linejoin="round"/>
<path fill-rule="evenodd" clip-rule="evenodd" d="M19.0001 38.0001C12.4812 30.8772 8.74054 26.7493 7.77829 25.6164C6.33491 23.9169 6.941 21.9962 10.5532 21.9962C14.1653 21.9962 16.2485 27.2816 19.0001 27.2816C19.0165 27.285 19.0176 20.5258 19.0034 7.00418C19.0017 5.34683 20.3438 4.00188 22.0012 4.00014L22.0043 4.00014C23.6635 4.00014 25.0085 5.34515 25.0085 7.0043V15.0137C32.9813 16.2226 37.3158 16.8895 38.0122 17.0145C39.0567 17.2021 41.0001 18.1991 41.0001 21.0682C41.0001 22.9809 41.0001 27.9582 41.0001 38.0001H19.0001Z" stroke="#000000" stroke-width="4" stroke-linejoin="round"/>
</svg>
)";

std::string flw::icons::WARNING = R"(
<?xml version="1.0" encoding="iso-8859-1"?>
<!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
<svg height="800px" width="800px" version="1.1" id="Layer_1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink"
	 viewBox="0 0 507.425 507.425" xml:space="preserve">
<path style="fill:#FFC52F;" d="M329.312,129.112l13.6,22l150.8,242.4c22.4,36,6,65.2-36.8,65.2h-406.4c-42.4,0-59.2-29.6-36.8-65.6
	l198.8-320.8c22.4-36,58.8-36,81.2,0L329.312,129.112z"/>
<g>
	<path style="fill:#F4EFEF;" d="M253.712,343.512c-10.8,0-20-8.8-20-20v-143.2c0-10.8,9.2-20,20-20s20,8.8,20,20v143.2
		C273.712,334.312,264.512,343.512,253.712,343.512z"/>
	<path style="fill:#F4EFEF;" d="M253.312,407.112c-5.2,0-10.4-2-14-6c-3.6-3.6-6-8.8-6-14s2-10.4,6-14c3.6-3.6,8.8-6,14-6
		s10.4,2,14,6c3.6,3.6,6,8.8,6,14s-2,10.4-6,14C263.712,404.712,258.512,407.112,253.312,407.112z"/>
</g>
<path d="M456.912,465.512h-406.4c-22,0-38.4-7.6-46-21.6s-5.6-32.8,6-51.2l198.8-321.6c11.6-18.8,27.2-29.2,44.4-29.2l0,0
	c16.8,0,32.4,10,43.6,28.4l35.2,56.4l0,0l13.6,22l150.8,243.6c11.6,18.4,13.6,37.2,6,51.2
	C495.312,457.912,478.912,465.512,456.912,465.512z M253.312,49.912L253.312,49.912c-14,0-27.2,8.8-37.6,25.2l-198.8,321.6
	c-10,16-12,31.6-5.6,43.2s20.4,17.6,39.2,17.6h406.4c18.8,0,32.8-6.4,39.2-17.6c6.4-11.2,4.4-27.2-5.6-43.2l-150.8-243.6l-13.6-22
	l-35.2-56.4C280.512,58.712,267.312,49.912,253.312,49.912z"/>
<path d="M249.712,347.512c-13.2,0-24-10.8-24-24v-143.2c0-13.2,10.8-24,24-24s24,10.8,24,24v143.2
	C273.712,336.712,262.912,347.512,249.712,347.512z M249.712,164.312c-8.8,0-16,7.2-16,16v143.2c0,8.8,7.2,16,16,16s16-7.2,16-16
	v-143.2C265.712,171.512,258.512,164.312,249.712,164.312z"/>
<path d="M249.712,411.112L249.712,411.112c-6.4,0-12.4-2.4-16.8-6.8c-4.4-4.4-6.8-10.8-6.8-16.8c0-6.4,2.4-12.4,6.8-16.8
	c4.4-4.4,10.8-7.2,16.8-7.2c6.4,0,12.4,2.4,16.8,7.2c4.4,4.4,7.2,10.4,7.2,16.8s-2.4,12.4-7.2,16.8
	C262.112,408.312,256.112,411.112,249.712,411.112z M249.712,371.112c-4,0-8.4,1.6-11.2,4.8c-2.8,2.8-4.8,7.2-4.8,11.2
	c0,4.4,1.6,8.4,4.8,11.2c2.8,2.8,7.2,4.8,11.2,4.8s8.4-1.6,11.2-4.8c2.8-2.8,4.8-7.2,4.8-11.2s-1.6-8.4-4.8-11.2
	C258.112,372.712,253.712,371.112,249.712,371.112z"/>
</svg>
)";

/*
 *      _       _          _
 *     | |     | |        | |
 *     | | __ _| |__   ___| |___
 *     | |/ _` | '_ \ / _ \ / __|
 *     | | (_| | |_) |  __/ \__ \
 *     |_|\__,_|_.__/ \___|_|___/
 *
 *
 */

std::string flw::labels::BROWSE   = "&Browse";
std::string flw::labels::CANCEL   = "&Cancel";
std::string flw::labels::CLOSE    = "Cl&ose";
std::string flw::labels::DEL      = "&Delete";
std::string flw::labels::DISCARD  = "&Discard";
std::string flw::labels::MONO     = "&Mono Font";
std::string flw::labels::NEXT     = "&Next";
std::string flw::labels::NO       = "&No";
std::string flw::labels::OK       = "&Ok";
std::string flw::labels::PREV     = "&Previous";
std::string flw::labels::PRINT    = "&Print";
std::string flw::labels::REGULAR  = "&Regular Font";
std::string flw::labels::SAVE     = "&Save";
std::string flw::labels::SAVE_DOT = "&Save...";
std::string flw::labels::SELECT   = "&Select";
std::string flw::labels::UPDATE   = "&Update";
std::string flw::labels::YES      = "&Yes";

/*
 *
 *
 *      _ __ ___   ___ _ __  _   _
 *     | '_ ` _ \ / _ \ '_ \| | | |
 *     | | | | | |  __/ | | | |_| |
 *     |_| |_| |_|\___|_| |_|\__,_|
 *
 *
 */

namespace flw {

/** @brief Private classes and functions.
*
*/
namespace priv {

/** @brief Find menu item.
*
* Either by searching for label or data.
*
* @param[in] menu  Menu item owner.
* @param[in] text  Menu item label.
* @param[in] v     Menu item data.
*
* @return Menu item or NULL.
*/
static Fl_Menu_Item* _item(Fl_Menu_* menu, const char* text, void* v = nullptr) {
    const Fl_Menu_Item* item;

    if (v == nullptr) {
        assert(menu && text);
        item = menu->find_item(text);
    }
    else {
        item = menu->find_item_with_user_data(v);
    }
#ifdef DEBUG
    if (item == nullptr) fprintf(stderr, "error: cant find menu item <%s>\n", text);
#endif
    return const_cast<Fl_Menu_Item*>(item);
}

} // flw::priv
} // flw

/** @brief Enable or disable menu item.
*
* Either by searching for label or data.
*
* @param[in] menu   Menu item owner.
* @param[in] text   Menu item label.
* @param[in] value  On or off.
*/
void flw::menu::enable_item(Fl_Menu_* menu, const char* text, bool value) {
    auto item = flw::priv::_item(menu, text);

    if (item == nullptr) {
        return;
    }

    if (value == true) {
        item->activate();
    }
    else {
        item->deactivate();
    }
}

/** @brief Get menu item.
*
* @param[in] menu  Menu item owner.
* @param[in] text  Menu item label.
*
* @return Item or NULL.
*/
Fl_Menu_Item* flw::menu::get_item(Fl_Menu_* menu, const char* text) {
    return flw::priv::_item(menu, text);
}

/** @brief Get menu item.
*
* @param[in] menu  Menu item owner.
* @param[in] v     Menu item data.
*
* @return Item or NULL.
*/
Fl_Menu_Item* flw::menu::get_item(Fl_Menu_* menu, void* v) {
    return flw::priv::_item(menu, nullptr, v);
}

/** @brief Get checked menu value.
*
* @param[in] menu  Menu item owner.
* @param[in] text  Menu item label.
*
* @return True or false.
*/
bool flw::menu::item_value(Fl_Menu_* menu, const char* text) {
    auto item = flw::priv::_item(menu, text);

    if (item == nullptr) {
        return false;
    }

    return item->value();
}

/** @brief Set checked menu value.
*
* @param[in] menu   Menu item owner.
* @param[in] text   Menu item label.
* @param[in] value  On or off.
*/
void flw::menu::set_item(Fl_Menu_* menu, const char* text, bool value) {
    auto item = flw::priv::_item(menu, text);

    if (item == nullptr) {
        return;
    }

    if (value == true) {
        item->set();
    }
    else {
        item->clear();
    }
}

/** @brief Turn on item for one and of for all other in a group.
*
* @param[in] menu   Menu item owner.
* @param[in] text   Menu item label.
*/
void flw::menu::setonly_item(Fl_Menu_* menu, const char* text) {
    auto item = flw::priv::_item(menu, text);

    if (item == nullptr) {
        return;
    }

    menu->setonly(item);
}

/*
 *            _   _ _
 *           | | (_) |
 *      _   _| |_ _| |
 *     | | | | __| | |
 *     | |_| | |_| | |
 *      \__,_|\__|_|_|
 *
 *
 */

/** @brief Center window on screen or parent.
*
* @param[in] window  Valid window.
* @param[in] parent  Valid parent or NULL.
*/
void flw::util::center_window(Fl_Window* window, Fl_Window* parent) {
    if (parent != nullptr) {
        int x = parent->x() + parent->w() / 2;
        int y = parent->y() + parent->h() / 2;

        window->resize(x - window->w() / 2, y - window->h() / 2, window->w(), window->h());
    }
    else {
        window->resize((Fl::w() / 2) - (window->w() / 2), (Fl::h() / 2) - (window->h() / 2), window->w(), window->h());
    }
}

/** @brief Return time as seconds since 1970.
*
* @return Seconds as a double number.
*/
//
double flw::util::clock() {
#ifdef _WIN32
    struct timeb timeVal;
    ftime(&timeVal);
    return (double) timeVal.time + (double) (timeVal.millitm / 1000.0);
#else
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return (double) (ts.tv_sec) + (ts.tv_nsec / 1000000000.0);
#endif
}

/** @brief Try to count decimals in a number
*
* @param[in] num  Number value.
*
* @return 0 - 8?.
*/
int flw::util::count_decimals(double num) {
    num = fabs(num);

    if (num > 999'999'999'999'999) {
        return 0;
    }

    int    res     = 0;
    char*  end     = 0;
    double inum = static_cast<int64_t>(num);
    double fnum = num - inum;
    char   buffer[400];

    if (num > 9'999'999'999'999) {
        snprintf(buffer, 400, "%.1f", fnum);
    }
    else if (num > 999'999'999'999) {
        snprintf(buffer, 400, "%.2f", fnum);
    }
    else if (num > 99'999'999'999) {
        snprintf(buffer, 400, "%.3f", fnum);
    }
    else if (num > 9'999'999'999) {
        snprintf(buffer, 400, "%.4f", fnum);
    }
    else if (num > 999'999'999) {
        snprintf(buffer, 400, "%.5f", fnum);
    }
    else if (num > 99'999'999) {
        snprintf(buffer, 400, "%.6f", fnum);
    }
    else if (num > 9'999'999) {
        snprintf(buffer, 400, "%.7f", fnum);
    }
    else {
        snprintf(buffer, 400, "%.8f", fnum);
    }

    size_t len = strlen(buffer);
    end = buffer + len - 1;

    while (*end == '0') {
        *end = 0;
        end--;
    }

    res = strlen(buffer) - 2;

    return res;
}

/** @brief Find widget by using the label.
*
* Widgets are searched recursive.\n
* First found widget label is returned.\n
*
* @param[in] group  Group widget.
* @param[in] label  Child label.
*
* @return Widget or NULL.
*/
Fl_Widget* flw::util::find_widget(Fl_Group* group, const std::string& label) {
    for (int f = 0; f < group->children(); f++) {
        auto w = group->child(f);

        if (w->label() != nullptr && label == w->label()) {
            return w;
        }
    }

    for (int f = 0; f < group->children(); f++) {
        auto w = group->child(f);
        auto g = w->as_group();

        if (g != nullptr) {
            w = flw::util::find_widget(g, label);

            if (w != nullptr) {
                return w;
            }
        }
    }

    return nullptr;
}

/** @brief Escape '\', '_', '/', '&' characters.
*
* @param[in] label  Input label.
*
* @return Escaped input string.
*/
std::string flw::util::fix_menu_string(const std::string& label) {
    std::string res = label;

    flw::util::replace_string(res, "\\", "\\\\");
    flw::util::replace_string(res, "_", "\\_");
    flw::util::replace_string(res, "/", "\\/");
    flw::util::replace_string(res, "&", "&&");

    return res;
}

/** @brief Format a string.
*
* @param[in] format  Valid format string, see sprintf() for formatting values.
* @param[in] ...     Additional arguments.
*
* @return Formatted string or empty for any error.
*/
std::string flw::util::format(const char* format, ...) {
    if (*format == 0) {
        return "";
    }

    int         l   = 128;
    int         n   = 0;
    char*       buf = static_cast<char*>(calloc(l, 1));
    std::string res;
    va_list     args;

    va_start(args, format);
    n = vsnprintf(buf, l, format, args);
    va_end(args);

    if (n < 0) {
        free(buf);
        return res;
    }

    if (n < l) {
        res = buf;
        free(buf);

        return res;
    }

    free(buf);
    l = n + 1;
    buf = static_cast<char*>(calloc(l, 1));

    if (buf == nullptr) {
        return res;
    }

    va_start(args, format);
    vsnprintf(buf, l, format, args);
    va_end(args);
    res = buf;
    free(buf);

    return res;
}

/** @brief Format a decimal number by inserting a thousand delimiter.
*
* Only none decimal numbers are separated.
*
* @param[in] num       Number to format.
* @param[in] decimals  Number of decimals, use -1 to try to count decimals.
* @param[in] del       Thousand separator, default space.
*
* @return Formatted number string.
*/
std::string flw::util::format_double(double num, int decimals, char del) {
    char fr_str[100];

    if (num > 9'007'199'254'740'992.0) {
        return "";
    }

    if (decimals < 0) {
        decimals = flw::util::count_decimals(num);
    }

    if (del < 32) {
        del = 32;
    }

    if (decimals == 0 || decimals > 9) {
        return flw::util::format_int(static_cast<int64_t>(num), del);
    }

    auto fabs_num   = fabs(num + 0.00000001);
    auto int_num    = static_cast<int64_t>(fabs_num);
    auto double_num = static_cast<double>(fabs_num - int_num);

    if (double_num < 0.0000001) {
        double_num = 0.0;
    }

    auto res = flw::util::format_int(int_num, del);
    auto n   = snprintf(fr_str, 100, "%.*f", decimals, double_num);

    if (num < 0.0) {
        res = "-" + res;
    }

    if (n > 0 && n < 100) {
        res += fr_str + 1;
    }

    return res;
}

/** @brief Format an integer by inserting a thousand delimiter.
*
* @param[in] num  Integer to format.
* @param[in] del  Thousand separator, default space.
*
* @return Formatted number string.
*/
std::string flw::util::format_int(int64_t num, char del) {
    auto pos = 0;
    char tmp1[32];
    char tmp2[32];

    if (del < 32) {
        del = 32;
    }
    memset(tmp2, 0, 32);
    snprintf(tmp1, 32, "%lld", (long long int) num);
    auto len = strlen(tmp1);

    for (int f = len - 1, i = 0; f >= 0 && pos < 32; f--, i++) {
        char c = tmp1[f];

        if ((i % 3) == 0 && i > 0 && c != '-') {
            tmp2[pos++] = del;
        }

        tmp2[pos++] = c;
    }

    std::string r = tmp2;
    std::reverse(r.begin(), r.end());
    return r;
}

/** @brief Set icon for widget.
*
* Widget manages image memory and will delete it.\n
* Image should be square.\n
*
* @param[in] widget     Widget to set icon for.
* @param[in] svg_image  Valid svg image.
* @param[in] max_size   Max image size (from 16 - 4096).
*
* @return True if icon was created and set.
*
* @image html icons.png
*/
bool flw::util::icon(Fl_Widget* widget, const std::string& svg_image, unsigned max_size) {
    if (svg_image.length() < 40) {
        return false;
    }

    auto svg = new Fl_SVG_Image(nullptr, svg_image.c_str());

    if (svg == nullptr) {
        return false;
    }
    else if (max_size < 16 || max_size > 4096) {
        delete svg;
        return false;
    }

    auto image   = svg->copy();
    auto deimage = image->copy();

    image->scale(max_size, max_size);
    deimage->inactive();
    deimage->scale(max_size, max_size);
    widget->bind_image(image);
    widget->bind_deimage(deimage);
    widget->bind_image(1);
    widget->bind_deimage(1);
    delete svg;

    return true;
}

/** @brief Check if string contains any letter but not control character.
*
* If the string contains at least one control character (1 - 31) it will always return true.\n
* If the string is empty or only whitespaces it will return true.\n
* If the string has whitespaces but at least one letter it will return false.\n
*
* @param[in] string  Input string.
*
* @return True for valid string.
*/
bool flw::util::is_empty(const std::string& string) {
    bool ctrl   = false;
    bool space  = false;
    bool letter = false;

    for (auto c : string) {
        if (c > 0 && c < 32) {
            ctrl = true;
        }
        else if (c == 32) {
            space = true;
        }
        else {
            letter = true;
        }
    }

    if (ctrl == true) {
        return true;
    }
    else if (space == true && letter == false) {
        return true;
    }
    else if (letter == true) {
        return false;
    }
    else {
        return true;
    }
}

/** @brief Set label font properties for a widget.
*
* If it is an group widget it will set properties for all child widgets also (recursive).
*
* @param[in] widget  Valid widget.
* @param[in] font    Font to use.
* @param[in] size    Font size.
*/
void flw::util::labelfont(Fl_Widget* widget, Fl_Font font, int size) {
    widget->labelfont(font);
    widget->labelsize(size);

    auto group = widget->as_group();

    if (group != nullptr) {
        for (auto f = 0; f < group->children(); f++) {
            flw::util::labelfont(group->child(f), font, size);
        }
    }
}

/** @brief Return time stamp.
*
* @return Time in microseconds.
*/
int64_t flw::util::microseconds() {
#if defined(_WIN32)
    LARGE_INTEGER StartingTime;
    LARGE_INTEGER Frequency;

    QueryPerformanceFrequency(&Frequency);
    QueryPerformanceCounter(&StartingTime);

    StartingTime.QuadPart *= 1000000;
    StartingTime.QuadPart /= Frequency.QuadPart;
    return StartingTime.QuadPart;
#else
    timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return (t.tv_sec * 1000000 + t.tv_nsec / 1000);
#endif
}

/** @brief Return time stamp.
*
* @return Time in milliseconds.
*/
int32_t flw::util::milliseconds() {
#if defined(_WIN32)
    LARGE_INTEGER StartingTime;
    LARGE_INTEGER Frequency;

    QueryPerformanceFrequency(&Frequency);
    QueryPerformanceCounter(&StartingTime);

    StartingTime.QuadPart *= 1000000;
    StartingTime.QuadPart /= Frequency.QuadPart;
    return StartingTime.QuadPart / 1000;
#else
    timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return (t.tv_sec * 1000000 + t.tv_nsec / 1000) / 1000;
#endif
}

/** @brief
*
* Must be compiled with FLW_USE_PNG flag and linked with fltk images (fltk-config --ldflags --use-images).\n
* If filename is empty, user will be asked for the name.\n
* If coordinates are 0 it will use all widget.\n
* Might look fuzzy if FLTK scaling is turned on.\
*
* @param[in] window    Window to save.
* @param[in] opt_name  Optional filename.
* @param[in] X         X pos in window.
* @param[in] Y         X pos in window.
* @param[in] W         Window width.
* @param[in] H         Window height.
*
* @return True if an image has been saved.
*/
bool flw::util::png_save(Fl_Window* window, const std::string& opt_name, int X, int Y, int W, int H) {
    auto res = false;

#ifdef FLW_USE_PNG
    auto filename = (opt_name == "") ? fl_file_chooser("Save To PNG File", "All Files (*)\tPNG Files (*.png)", "") : opt_name.c_str();

    if (filename != nullptr) {
        window->make_current();

        if (X == 0 && Y == 0 && W == 0 && H == 0) {
            X = window->x();
            Y = window->y();
            W = window->w();
            H = window->h();
        }

        auto image = fl_read_image(nullptr, X, Y, W, H);

        if (image != nullptr) {
            auto ret = fl_write_png(filename, image, W, H);

            if (ret == 0) {
                res = true;
            }
            else if (ret == -1) {
                fl_alert("%s", "Error: missing libraries!");
            }
            else if (ret == -2) {
                fl_alert("Error: failed to save image to %s!", filename);
            }

            delete []image;
        }
        else {
            fl_alert("%s", "Error: failed to grab image!");
        }
    }
#else
    (void) opt_name;
    (void) window;
    (void) X;
    (void) Y;
    (void) W;
    (void) H;
    fl_alert("Error: flw has not been compiled with FLW_USE_PNG flag!");
#endif

    return res;
}

/** @brief Remove formatting from a browser text line.
*
* Read Fl_Browser::format_char() for browser formatting.\n
* Returns an empty string if exception has been thrown.\n
*
* @param[in] text  Input text.
*
* @return String without control characters.
*/
std::string flw::util::remove_browser_format(const std::string& text) {
    auto res = text;
    auto f   = res.find_last_of("@");

    if (f != std::string::npos) {
        try {
            auto tmp = res.substr(f + 1);

            if (tmp[0] == '.' || tmp[0] == 'l' || tmp[0] == 'm' || tmp[0] == 's' || tmp[0] == 'b' || tmp[0] == 'i' || tmp[0] == 'f' || tmp[0] == 'c' || tmp[0] == 'r' || tmp[0] == 'u' || tmp[0] == '-') {
                res = tmp.substr(1);
            }
            else if (tmp[0] == 'B' || tmp[0] == 'C' || tmp[0] == 'F' || tmp[0] == 'S') {
                auto s = std::string();
                auto e = false;

                tmp = tmp.substr(f + 1);

                for (auto c : tmp) {
                    if (e == false && c >= '0' && c <= '9') {
                    }
                    else {
                        e = true;
                        s += c;
                    }
                }

                res = s;
            }
            else {
                res = res.substr(f);
            }
        }
        catch (...) {
            res = "";
        }
    }

    return res;
}

/** @brief Replace all found strings in input string.
*
* Returns an empty string if exception has been thrown.
*
* @param[in,out] string   Input string.
* @param[in]     find     Find string.
* @param[in]     replace  Replace string.
*
* @return Input string or empty for error.
*/
std::string& flw::util::replace_string(std::string& string, const std::string& find, const std::string& replace) {
    if (find == "") {
        return string;
    }

    try {
        auto res   = std::string();
        auto start = static_cast<size_t>(0);
        auto pos   = static_cast<size_t>(0);

        res.reserve(string.length() + (replace.length() > find.length() ? string.length() * 0.1 : 0));

        while ((pos = string.find(find, start)) != std::string::npos) {
            res   += string.substr(start, pos - start);
            res   += replace;
            pos   += find.length();
            start  = pos;
        }

        res += string.substr(start);
        string.swap(res);
    }
    catch(...) {
        string = "";
    }

    return string;
}

/** @brief Sleep for a while.
*
* @param[in] milli  Number of milliseconds.
*/
void flw::util::sleep(unsigned milli) {
#ifdef _WIN32
    Sleep(milli);
#else
    usleep(milli * 1000);
#endif
}

/** @brief Split a string and return an vector with strings.
*
* @param[in] string  Input string.
* @param[in] split   Split input string on this string.
*
* @return Result vector, empty for error.
*/
flw::StringVector flw::util::split_string(const std::string& string, const std::string& split) {
    auto res = StringVector();

    try {
        if (split != "") {
            auto pos1 = (std::string::size_type) 0;
            auto pos2 = string.find(split);

            while (pos2 != std::string::npos) {
                res.push_back(string.substr(pos1, pos2 - pos1));
                pos1 = pos2 + split.size();
                pos2 = string.find(split, pos1);
            }

            if (pos1 <= string.size()) {
                res.push_back(string.substr(pos1));
            }
        }
    }
    catch(...) {
        res.clear();
    }

    return res;
}

/** @brief Return sub string.
*
* Returns an empty string if exception has been thrown.
*
* @param[in] string  Input string.
* @param[in] pos     Start position.
* @param[in] size    Number of bytes, std::string::npos for all bytes form pos.
*
* @return Sub string.
*/
std::string flw::util::substr(const std::string& string, std::string::size_type pos, std::string::size_type size) {
    try {
        return string.substr(pos, size);
    }
    catch(...) {
        return "";
    }
}

/** @brief Trim whitespace from both ends of string
*
* @param[in,out] string  Input string.
*
* @return Trimmed input string.
*/
std::string flw::util::trim(const std::string& string) {
    auto res = string;

    try {
        res.erase(res.begin(), std::find_if(res.begin(), res.end(), [](auto c) { return !std::isspace(c);} ));
        res.erase(std::find_if(res.rbegin(), res.rend(), [](int ch) { return !std::isspace(ch); }).base(), res.end());
        return res;
    }
    catch(...) {
        return string;
    }
}

/** @brief Swap rectangles.
*
* @param[in,out] w1  Widget 1.
* @param[in,out] w2  Widget 2.
*/
void flw::util::swap_rect(Fl_Widget* w1, Fl_Widget* w2) {
    auto r1 = Fl_Rect(w1);
    auto r2 = Fl_Rect(w2);

    w1->resize(r2.x(), r2.y(), r2.w(), r2.h());
    w2->resize(r1.x(), r1.y(), r1.w(), r1.h());
}

/** @brief Convert string to number.
*
* @param[in] string  String to convert.
*
* @return Converted number or INFINITY.
*/
double flw::util::to_double(const std::string& string) {
    try {
        return std::stod(string);
    }
    catch(...) {
        return INFINITY;
    }
}

/** @brief Convert string to an vector of double numbers.
*
* @param[in]  string   String to convert.
* @param[out] numbers  Result vector.
* @param[in]  size     Size of result vector.
*
* @return Number of converted numbers.
*/
size_t flw::util::to_doubles(const std::string& string, double numbers[], size_t size) {
    auto end = (char*) nullptr;
    auto in  = string.c_str();
    auto f   = (size_t) 0;

    errno = 0;

    for (; f < size; f++) {
        numbers[f] = strtod(in, &end);

        if (errno != 0 || in == end) {
            return f;
        }

        in = end;
    }

    return f;
}

/** @brief Convert string to integer.
*
* @param[in] string  String to convert.
* @param[in] def     Default number if exception is thrown.
*
* @return Converted integer or default integer.
*/
int64_t flw::util::to_int(const std::string& string, int64_t def) {
    try {
        return static_cast<int64_t>(std::stoll(string));
    }
    catch(...) {
        return def;
    }
}

/** @brief Convert string to integer.
*
* @param[in] string  String to convert.
* @param[in] def     Default number if exception is thrown.
*
* @return Converted integer or default integer.
*/
long long flw::util::to_long(const std::string& string, long long def) {
    try {
        return std::stoll(string);
    }
    catch(...) {
        return def;
    }
}

/** @brief Zero buffer bytes.
*
* @param[in,out]   mem  Memory to zero.
* @param[in] size  Number of bytes.
*
* @return Input pointer.
*/
void* flw::util::zero_memory(char* mem, size_t size) {
    if (mem == nullptr || size == 0) return mem;
#ifdef _WIN32
    RtlSecureZeroMemory(mem, size);
#else
    auto p = reinterpret_cast<volatile unsigned char*>(mem);

    while (size--) {
        *p = 0;
        p++;
    }
#endif

    return mem;
}

// MKALGAM_OFF
