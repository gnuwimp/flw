# flw (fast light widgets)

flw is an collection of widgets for the [FLTK](http://www.fltk.org) version 1.4 GUI library.
The flw library has been tested on Windows 10 using msys2 compiler tools and Kubuntu 25.04.
Earlier versions have been compiled successfully on a Raspberry Pi 4.
It might work on macOS with some tweaks.

FLTK must be compiled with --enable-use_std flag.
The source will NOT compile with Microsoft Visual Studio.
The API is not stable and may change from time to time.

The source are using C++17 features and all widgets are in the namespace flw. 
None gui classes are in namespace gnu (Date, File, JS).

Use the amalgamated version of all source files in the root directory of flw (flw.h, flw.cpp). 
The source files in the src/ directory are intended for development of flw. 
Although date.h, date.cpp, file.h, file.cpp json.h, json.cpp are completely 
independent of flw and FLTK and can be used stand-alone.

All files (except lcdnumber.h, lcdnumber.cpp) are released under the [GNU General Public License v3.0](LICENSE).

Widgets:
* Chart - Chart widget with dates as x data.
* DateChooser - Show month date view.
* Grid - Edit data in TableEditor with built in storage.
* GridGroup - A layout widget using a grid.
* InputMenu - Similar to Fl_Input_Choice, but can keep track of an history list and use arrow keys in the input widget to switch between strings.
* LcdNumber - Number widget in lcd style.
* Plot - Numerical chart.
* ScrollBrowser - Scroll faster and let user copy a line to clipboard.
* SplitGroup - A layout widget for two widgets that can be resized with mouse.
* TableDisplay - Show data in a grid.
* TableEditor - Edit data in a grid.
* ToolGroup - A layout widget for a horizontal or vertical group of widgets.
* TabsGroup - A tabbed layout widget that can have tabs at every direction. Tabs can be moved or selected with keyboard shortcuts.

Dialogs:
* dlg::Font - A dialog for selecting font and font size.
* dlg::Progress - A dialog with a message list and a progress bar for time consuming work.
* dlg::date - Show a date selection dialog.
* dlg::html - Show text in a html view.
* dlg::list* - Show a list of strings.
* dlg::password* - Ask user for password and optional password confirmation.
* dlg::print - Print drawing from a user callback.
* dlg::print_text* - Print plain text to postscript.
* dlg::select_checkboxes - Show a dialog with a list of checkboxes.
* dlg::select_choice - Show a string selection dialog with Fl_Choice.
* dlg::select_string* - Let user select a string from a list and also to filter shown strings.
* dlg::slider - Show a number input dialog that uses a Fl_Hor_Value_Slider widget.
* dlg::text - Show text in an editor widget.
* dlg::text_edit - Show and edit a text in an editor widget.
* dlg::theme - Let user select a theme.

## Screenshots of some of the widgets

Date dialog<br>
<img src="documentation/images/date_dialog.png"/>

Font dialog<br>
<img src="documentation/images/font_dialog.png"/>

Password dialog<br>
<img src="documentation/images/password_dialog.png"/>

List dialog<br>
<img src="documentation/images/list_dialog.png"/>

Select string dialog<br>
<img src="documentation/images/select_string_dialog.png"/>

Checkbox dialog<br>
<img src="documentation/images/select_checkboxes_dialog.png"/>

Theme dialog<br>
<img src="documentation/images/theme_dialog.png"/>

Grid layout widget<br>
<img src="documentation/images/gridgroup.png"/>

Tab layout widget<br>
<img src="documentation/images/tabsgroup.png"/>

Table widget<br>
<img src="documentation/images/tableeditor.png"/>

Chart widget<br>
<img src="documentation/images/chart.png"/>

Plot widget<br>
<img src="documentation/images/plot.png"/>
