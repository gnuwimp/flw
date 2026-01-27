/**
* @file
* @brief Assorted dialog functions and classes.
*
* @author gnuwimp@gmail.com
* @copyright  Released under the GNU General Public License v3.0
*/

#ifndef FLW_DLG_H
#define FLW_DLG_H

#include "flw.h"

// MKALGAM_ON

#include <FL/Fl_Button.H>

namespace flw {

/*
 *          _ _
 *         | | |
 *       __| | | __ _
 *      / _` | |/ _` |
 *     | (_| | | (_| |
 *      \__,_|_|\__, |
 *               __/ |
 *              |___/
 */

/** @brief A collections of dialog functions.
*
* There are some other dialogs in dlg namespace that are declared in other include files.\n
*/
namespace dlg {

void                            center_fl_message_dialog();
void                            html(const std::string& title, const std::string& text, int W = 40, int H = 23);
std::string                     input(const std::string& title, const std::string& message, std::string& value, int W = 40, int H = 10);
std::string                     input_double(const std::string& title, const std::string& message, double& value, int W = 40, int H = 10);
std::string                     input_int(const std::string& title, const std::string& message, int64_t& value, int W = 40, int H = 10);
std::string                     input_multi(const std::string& title, const std::string& message, std::string& value, int W = 54, int H = 22);
std::string                     input_secret(const std::string& title, const std::string& message, std::string& value, int W = 40, int H = 10);
void                            list(const std::string& title, const StringVector& list, bool fixed_font = false, int W = 40, int H = 23);
void                            list(const std::string& title, const std::string& list, bool fixed_font = false, int W = 40, int H = 23);
void                            list_file(const std::string& title, const std::string& file, bool fixed_font = false, int W = 40, int H = 23);
void                            msg(const std::string& title, const std::string& message, int W = 40, int H = 7);
void                            msg_alert(const std::string& title, const std::string& message, int W = 40, int H = 7);
std::string                     msg_ask(const std::string& title, const std::string& message, const std::string& b1 = labels::YES, const std::string& b2 = labels::NO, const std::string& b3 = "", const std::string& b4 = "", const std::string& b5 = "", int W = 40, int H = 7);
void                            msg_error(const std::string& title, const std::string& message, int W = 40, int H = 7);
std::string                     msg_warning(const std::string& title, const std::string& message, const std::string& b1 = labels::YES, const std::string& b2 = labels::NO, int W = 40, int H = 7);
void                            panic(const std::string& message);
bool                            password(const std::string& title, std::string& password);
bool                            password_confirm(const std::string& title, std::string& password);
bool                            password_confirm_and_file(const std::string& title, std::string& password, std::string& file);
bool                            password_and_file(const std::string& title, std::string& password, std::string& file);
StringVector                    select_checkboxes(const std::string& title, const StringVector& list);
int                             select_choice(const std::string& title, const std::string& message, const StringVector& list, int selected = 0);
int                             select_string(const std::string& title, const StringVector& list, int select_row = -1, bool fixed_font = false, int W = 40, int H = 23);
int                             select_string(const std::string& title, const StringVector& list, const std::string& select_row, bool fixed_font = false, int W = 40, int H = 23);
bool                            slider(const std::string& title, double min, double max, double& value, double step = 1.0);
void                            text(const std::string& title, const std::string& text, int W = 40, int H = 23);
bool                            text_edit(const std::string& title, std::string& text, int W = 40, int H = 23);

} // flw::dlg
} // flw

// MKALGAM_OFF

#endif
