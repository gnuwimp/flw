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
* FontDialog - A dialog for selecting font and font size.
* WorkDialog - A dialog with a message list and a progress bar for time consuming work.
* dlg::date - Show a date selection dialog.
* dlg::html - Show text in a html view.
* dlg::list - Show a list of strings.
* dlg::password - Ask user for password and optional password confirmation.
* dlg::print - Print drawing from a user callback.
* dlg::print_text - Print plain text to postscript.
* dlg::select_checkboxes - Show a dialog with a list of checkboxes.
* dlg::select_choice - Show a string selection dialog with Fl_Choice.
* dlg::select_string - Let user select a string from a list and also to filter shown strings.
* dlg::slider - Show a number input dialog that uses a Fl_Hor_Value_Slider widget.
* dlg::text - Show text in an editor widget.
* dlg::text_edit - Show and edit a text in an editor widget.
* dlg::theme - Let user select a theme.

## Screenshots of some of the widgets

Chart widget (flw::Chart)<br>
<img src="images/chart.png"/>

Date chooser widget (flw::DateChooser)<br>
<img src="images/datechooser.png"/>

Font dialog (flw::dlg::FontDialog)<br>
<img src="images/font_dialog.png"/>

Input menu (flw::InputMenu)<br>
<img src="images/inputmenu.png"/>

List selection dialog (flw::dlg::select())<br>
<img src="images/list_dialog.png"/>

Password dialog (flw::dlg::password4())<br>
<img src="images/password_dialog.png"/>

Print text dialog (flw::dlg::print_text())<br>
<img src="images/print_text_dialog.png"/>

Checkbox dialog (flw::dlg::check())<br>
<img src="images/check_dialog.png"/>

Plot widget (flw::Plot)<br>
<img src="images/plot.png"/>

Table editor widget (flw::TableDisplay and flw::TableEditor)<br>
<img src="images/tableeditor.png"/>

Grid layout widget (flw::GridGroup)<br>
<img src="images/gridgroup.png"/>

Tab layout widget (flw::TabsGroup)<br>
<img src="images/tabsgroup.png"/>

Toolbar layout widget (flw::ToolGroup)<br>
<img src="images/toolgroup.png"/>

Theme dialog (flw::dlg::theme())<br>
<img src="images/theme_dialog.png"/>

## Theme examples

Tan gleam theme<br>
<img src="images/tan_gleam.png"/>

Blue gtk theme<br>
<img src="images/blue_gtk.png"/>

Dark gleam theme<br>
<img src="images/dark_gleam.png"/>
