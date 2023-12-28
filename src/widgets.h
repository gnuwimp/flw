// Copyright 2021 gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#ifndef FLW_WIDGETS_H
#define FLW_WIDGETS_H

#include "util.h"
#include <FL/Fl_Hold_Browser.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Menu_Button.H>


// MKALGAM_ON

namespace flw {
    class _InputMenu;

    //--------------------------------------------------------------------------
    // First string in a vector is the latest pushed in
    //
    class InputMenu : public Fl_Group {
    public:
                                        InputMenu(const InputMenu&) = delete;
                                        InputMenu(InputMenu&&) = delete;
        InputMenu&                      operator=(const InputMenu&) = delete;
        InputMenu&                      operator=(InputMenu&&) = delete;

                                        InputMenu(int X = 0, int Y = 0, int W = 0, int H = 0, const char* l = nullptr);
        void                            clear();
        StringVector                    get_history() const;
        Fl_Input*                       input()
                                            { return (Fl_Input*) _input; }
        void                            insert(const std::string& string, int max_list_len);
        Fl_Menu_Button*                 menu()
                                            { return _menu; }
        void                            resize(int X, int Y, int W, int H) override;
        void                            set(const StringVector& list, bool copy_first_to_input = true);
        void                            update_pref(Fl_Font text_font = flw::PREF_FIXED_FONT, Fl_Fontsize text_size = flw::PREF_FIXED_FONTSIZE);
        const char*                     value() const;
        void                            value(const char* string);

        static void                     Callback(Fl_Widget*, void*);

    private:
        void                            _add(bool insert, const std::string& string, int max_list_len);
        void                            _add(bool insert, const StringVector& list);

        _InputMenu*                     _input;
        Fl_Menu_Button*                 _menu;
    };

    //--------------------------------------------------------------------------
    class ScrollBrowser : public Fl_Hold_Browser {
    public:
                                        ScrollBrowser(const ScrollBrowser&) = delete;
                                        ScrollBrowser(ScrollBrowser&&) = delete;
        ScrollBrowser&                  operator=(const ScrollBrowser&) = delete;
        ScrollBrowser&                  operator=(ScrollBrowser&&) = delete;

                                        ScrollBrowser(int scroll = 9, int X = 0, int Y = 0, int W = 0, int H = 0, const char* l = nullptr);
        void                            enable_menu(bool menu)
                                            { _flag_menu = menu; }
        void                            enable_pagemove(bool move)
                                            { _flag_move = move; }
        int                             handle(int event) override;
        Fl_Menu_Button*                 menu()
                                            { return _menu; }
        void                            update_pref(Fl_Font text_font = flw::PREF_FONT, Fl_Fontsize text_size = flw::PREF_FONTSIZE);

        static void                     Callback(Fl_Widget*, void*);
        static std::string              RemoveFormat(const char* text);

    private:
        Fl_Menu_Button*                 _menu;
        bool                            _flag_menu;
        bool                            _flag_move;
        int                             _scroll;
    };
}

// MKALGAM_OFF

#endif
