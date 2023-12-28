// Copyright gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#ifndef FLW_FONTDIALOG_H
#define FLW_FONTDIALOG_H

#include <FL/Fl_Double_Window.H>
#include <string>

class Fl_Box;
class Fl_Button;

// MKALGAM_ON

namespace flw {
    class ScrollBrowser;

    namespace dlg {
        //----------------------------------------------------------------------
        // Dialog for selecting font and font size
        // FontDialog::LoadFonts() will be called automatically (or do it manually)
        // It is only needed once
        // Call FontDialog::DeleteFonts() before app exit (this is unnecessarily, only for keeping memory sanitizers satisfied)
        //
        class FontDialog : public Fl_Double_Window {
        public:
                                        FontDialog(const FontDialog&) = delete;
                                        FontDialog(FontDialog&&) = delete;
            FontDialog&                 operator=(const FontDialog&) = delete;
            FontDialog&                 operator=(FontDialog&&) = delete;

                                        FontDialog(Fl_Font font, Fl_Fontsize fontsize, const std::string& label);
                                        FontDialog(std::string font, Fl_Fontsize fontsize, std::string label);
            void                        activate_font()
                                            { ((Fl_Widget*) _fonts)->activate(); }
            void                        deactivate_font()
                                            { ((Fl_Widget*) _fonts)->deactivate(); }
            void                        deactivate_fontsize()
                                            { ((Fl_Widget*) _sizes)->deactivate(); }
            int                         font()
                                            { return _font; }
            std::string                 fontname()
                                            { return _fontname; }
            int                         fontsize()
                                            { return _fontsize; }
            void                        resize(int X, int Y, int W, int H) override;
            bool                        run(Fl_Window* parent = nullptr);

            static void                 Callback(Fl_Widget* w, void* o);
            static void                 DeleteFonts();
            static int                  LoadFont(std::string requested_font);
            static void                 LoadFonts(bool iso8859_only = true);

        private:
            void                        _activate();
            void                        _create(Fl_Font font, std::string fontname, Fl_Fontsize fontsize, std::string label);
            void                        _select_name(std::string font_name);

            Fl_Box*                     _label;
            Fl_Button*                  _cancel;
            Fl_Button*                  _select;
            ScrollBrowser*              _fonts;
            ScrollBrowser*              _sizes;
            bool                        _ret;
            int                         _font;
            int                         _fontsize;
            std::string                 _fontname;
        };
    }
}

// MKALGAM_OFF

#endif
