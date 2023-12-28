// Copyright 2022 gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#include <FL/Fl_Menu_.H>
#include "util.h"

class Fl_Preferences;

// MKALGAM_ON

namespace flw {
    class RecentMenu {
    public:
                                        RecentMenu(Fl_Menu_* menu, Fl_Callback* file_callback, void* userdata, std::string base_label = "&File/Open recent", std::string clear_label = "/Clear");
        void                            append(std::string file)
                                            { return _add(file, true); }
        static void                     CallbackClear(Fl_Widget*, void* o);
        void                            insert(std::string file)
                                            { return _add(file, false); }
        StringVector                    items() const
                                            { return _files; }
        void                            max_items(size_t max)
                                            { assert(max > 0 && max <= 100); _max = max; }
        void                            load_pref(Fl_Preferences& pref, std::string base_name = "files");
        void                            save_pref(Fl_Preferences& pref, std::string base_name = "files");

        private:
        void                            _add(std::string file, bool append);

        std::string                     _base;
        Fl_Callback*                    _callback;
        std::string                     _clear;
        StringVector                    _files;
        size_t                          _max;
        Fl_Menu_*                       _menu;
        void*                           _user;
    };
}

// MKALGAM_OFF