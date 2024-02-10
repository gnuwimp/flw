// Copyright gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#ifndef FLW_INPUTMENU_H
#define FLW_INPUTMENU_H

#include "flw.h"

// MKALGAM_ON

#include <FL/Fl_Group.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Menu_Button.H>

namespace flw {

/***
 *      _____                   _   __  __                  
 *     |_   _|                 | | |  \/  |                 
 *       | |  _ __  _ __  _   _| |_| \  / | ___ _ __  _   _ 
 *       | | | '_ \| '_ \| | | | __| |\/| |/ _ \ '_ \| | | |
 *      _| |_| | | | |_) | |_| | |_| |  | |  __/ | | | |_| |
 *     |_____|_| |_| .__/ \__,_|\__|_|  |_|\___|_| |_|\__,_|
 *                 | |                                      
 *                 |_|                                      
 */

class _InputMenu;

//------------------------------------------------------------------------------
// First string in the vector for InputMenu::values() is the latest pushed in.
//
class InputMenu : public Fl_Group {
public:
                                InputMenu(const InputMenu&) = delete;
                                InputMenu(InputMenu&&) = delete;
    InputMenu&                  operator=(const InputMenu&) = delete;
    InputMenu&                  operator=(InputMenu&&) = delete;

    explicit                    InputMenu(int X = 0, int Y = 0, int W = 0, int H = 0, const char* l = nullptr);
    void                        clear();
    bool                        enable_menu() const
                                    { return _menu->visible() != 0; }
    void                        enable_menu(bool value)
                                    { if (value == true) _menu->show(); else _menu->hide(); resize(x(), y(), w(), h()); }
    StringVector                get_history() const;
    Fl_Input*                   input()
                                    { return reinterpret_cast<Fl_Input*>(_input); }
    void                        insert(std::string string, int max_list_len);
    Fl_Menu_Button*             menu()
                                    { return _menu; }
    void                        resize(int X, int Y, int W, int H) override;
    void                        update_pref(Fl_Font text_font = flw::PREF_FONT, Fl_Fontsize text_size = flw::PREF_FONTSIZE);
    const char*                 value() const;
    void                        value(const char* string);
    void                        values(const StringVector& list, bool copy_first_to_input = true);

    static void                 Callback(Fl_Widget*, void*);

private:
    void                        _add(bool insert, const std::string& string, int max_list_len);
    void                        _add(bool insert, const StringVector& list);

    _InputMenu*                 _input;
    Fl_Menu_Button*             _menu;
};

} // flw

// MKALGAM_OFF

#endif
