# flw (fast light widgets)

flw is an collection of widgets for the [FLTK](http://www.fltk.org) gui library.
They work on all platforms that FLTK are supported.
This library has been tested on Windows 10 + msys2, solus + gcc and macOS 10.13 + clang and with FLTK version 1.4.
They are compiled with c++17 flag and all are in the namespace flw. The source will not compile with Microsoft Visual Studio as it is.

There is an amalgamated version of all source files in the top directory.

Most widgets are released under the [GNU General Public License v3.0](LICENSE).

## Screenshots of some of the widgets

Chart widget (flw::Chart)<br>
<img src="images/chart.png"/>

Date chooser widget (flw::DateChooser)<br>
<img src="images/datechooser.png"/>

Table editor widget (flw::TableDisplay and flw::TableEditor)<br>
<img src="images/tableeditor.png"/>

Grid layout widget (flw::GridGroup)<br>
<img src="images/gridgroup.png"/>

Tab layout widget (flw::TabsGroup)<br>
<img src="images/tabsgroup.png"/>

Font dialog (flw::dlg::FontDialog)<br>
<img src="images/font_dialog.png"/>

Password dialog (flw::dlg::password4())<br>
<img src="images/password_dialog.png"/>

List selection dialog (flw::dlg::select())<br>
<img src="images/list_dialog.png"/>

Theme dialog (flw::dlg::theme())<br>
<img src="images/theme_dialog.png"/>

## Theme examples

Dark blue gleam theme<br>
<img src="images/darkblue_gleam_theme.png"/>

Dark gtk theme<br>
<img src="images/dark_gtk_theme.png"/>

Tan gleam theme<br>
<img src="images/tan_gleam_theme.png"/>

Blue gtk theme<br>
<img src="images/bluegtk_theme.png"/>
