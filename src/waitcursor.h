// Copyright gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#ifndef FLW_WAITCURSOR_H
#define FLW_WAITCURSOR_H

// MKALGAM_ON

namespace flw {
    //--------------------------------------------------------------------------
    // Show an wait mouse pointer until object is deleted
    //
    class WaitCursor {
        static WaitCursor*              WAITCURSOR;

    public:
                                        WaitCursor(const WaitCursor&) = delete;
                                        WaitCursor(WaitCursor&&) = delete;
                                        WaitCursor& operator=(const WaitCursor&) = delete;
                                        WaitCursor& operator=(WaitCursor&&) = delete;

                                        WaitCursor();
                                        ~WaitCursor();
    };
}

// MKALGAM_OFF

#endif
