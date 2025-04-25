/**
* @file
* @brief File and directory functions.
*
* gnu::file namespace has assorted functions for files and directories.\n
* gnu::file::File class has common file info data such as name, size, type.\n
* gnu::file::Buf class is a simple buffer container.\n
*
* @author gnuwimp@gmail.com
* @copyright Released under the GNU General Public License v3.0
*/

#ifndef GNU_FILE_H
#define GNU_FILE_H

// MKALGAM_ON

#include <cassert>
#include <cstdint>
#include <cstring>
#include <string>
#include <vector>

namespace gnu {
namespace file {

class File;
struct Buf;

typedef bool (*CallbackCopy)(int64_t size, int64_t copied, void* data); ///< Callback for file copy.
typedef std::vector<File> Files;

/***
 *       __ _ _
 *      / _(_) |
 *     | |_ _| | ___
 *     |  _| | |/ _ \
 *     | | | | |  __/
 *     |_| |_|_|\___|
 *
 *
 */

/** @brief File type.
*
*/
enum class TYPE {
                                MISSING,
                                DIR,
                                FILE,
                                OTHER,
};

#ifdef _WIN32
    static const int            DEFAULT_DIR_MODE  = 0x00000080;
    static const int            DEFAULT_FILE_MODE = 0x00000080;
#else
    static const int            DEFAULT_DIR_MODE  = 0755;
    static const int            DEFAULT_FILE_MODE = 0664;
#endif

char*                           allocate(char* resize_or_null, size_t size, bool exception = true);
std::string                     canonical_name(const std::string& path);
bool                            chdir(const std::string& path);
std::string                     check_filename(const std::string& name);
bool                            chmod(const std::string& path, int mode);
bool                            chtime(const std::string& path, int64_t time);
Buf                             close_stderr();
Buf                             close_stdout();
bool                            copy(const std::string& from, const std::string& to, CallbackCopy callback = nullptr, void* data = nullptr, bool flush_write = true);
uint64_t                        fletcher64(const char* p, size_t s);
void                            flush(FILE* file);
File                            home_dir();
bool                            is_circular(const std::string& path);
bool                            mkdir(const std::string& path);
FILE*                           open(const std::string& path, const std::string& mode);
std::string                     os();
FILE*                           popen(const std::string& cmd, bool write = false);
Buf                             read(const std::string& path);
Buf*                            read2(const std::string& path);
Files                           read_dir(const std::string& path);
Files                           read_dir_rec(const std::string& path);
bool                            redirect_stderr();
bool                            redirect_stdout();
bool                            remove(const std::string& path);
bool                            remove_rec(const std::string& path);
bool                            rename(const std::string& from, const std::string& to);
int                             run(const std::string& cmd, bool background, bool hide_win32_window = false);
File                            tmp_dir();
File                            tmp_file(const std::string& prepend = "");
File                            work_dir();
bool                            write(const std::string& path, const char* buffer, size_t size, bool flush = true);
bool                            write(const std::string& path, const Buf& b, bool flush = true);

/***
 *      ____         __
 *     |  _ \       / _|
 *     | |_) |_   _| |_
 *     |  _ <| | | |  _|
 *     | |_) | |_| | |
 *     |____/ \__,_|_|
 *
 *
 */

/** @brief Store binary data.
*
* Data can be NULL.
*
*/
class Buf {
public:
                                Buf()
                                    { _str = nullptr; _size = 0; } ///< Create empty buffer with NULL data.
    explicit                    Buf(size_t size);
                                Buf(const char* buffer, size_t size);
                                Buf(const Buf& b);
                                Buf(Buf&& b)
                                    { _str = b._str; _size = b._size; b._str = nullptr; } ///< Move buffer.
                                Buf(const std::string& string)
                                    { _str = nullptr; add(string.c_str(), string.length()); } ///< Copy input string.
    virtual                     ~Buf()
                                    { free(_str); } ///< Free memory.
    Buf&                        operator=(const Buf& b)
                                    { return set(b._str, b._size); } ///< COpy other buffer.
    Buf&                        operator=(Buf&& b)
                                    { free(_str); _str = b._str; _size = b._size; b._str = nullptr; return *this; } ///< Move data to this object.
    Buf&                        operator=(const std::string& string)
                                    { free(_str); _str = nullptr; add(string.c_str(), string.length()); return *this; } ///< Copy string.
    Buf&                        operator+=(const Buf& b)
                                    { return add(b._str, b._size); } ///< Add buffer.
    bool                        operator==(const Buf& other) const;
    bool                        operator!=(const Buf& other) const 
                                    { return (*this == other) == false; } ///< Compare buffer objects.
    Buf&                        add(const char* buffer, size_t size);
    const char*                 c_str() const
                                    { return _str; } ///< Return buffer data, can be NULL.
    void                        clear()
                                    { free(_str); _str = nullptr; _size = 0; } ///< Delete memory and set internal buffer to NULL.
    void                        count(size_t count[257]) const
                                    { Buf::Count(_str, _size, count); } ///< Count bytes and longest text line.
    void                        debug() const
                                    { printf("gnu::Buf(0x%p, %u)\n", _str, (unsigned) _size); } ///< Print debug info.
    uint64_t                    fletcher64() const
                                    { return file::fletcher64(_str, _size); } ///< Return checksum for this object.
    Buf&                        grab(char* buffer, size_t size)
                                    { free(_str); _str = buffer; _size = size; return *this; } ///< Delete internal memory and take control of input buffer.
    Buf                         insert_cr(bool dos = true, bool trailing = false) const
                                    { return Buf::InsertCR(_str, _size, dos, trailing); } ///< Insert \r and remove trailing whitespace.
    char*                       release()
                                    { auto res = _str; _str = nullptr; _size = 0; return res; } ///< Release control of internal buffer and return memory (internal memory will be set to NULL).
    Buf                         remove_cr() const
                                    { return Buf::RemoveCR(_str, _size); } ///< Remove \r from buffer.
    Buf&                        set(const char* buffer, size_t size);
    size_t                      size() const
                                    { return _size; } ///< Return size in bytes.
    void                        size(size_t new_size)
                                    { assert(new_size <= _size); _size = new_size; } ///< Set new size, must be less than current size.
    char*                       str()
                                    { return _str; } ///< Return buffer data, can be NULL.
    bool                        write(const std::string& path, bool flush = true) const;

    static void                 Count(const char* buffer, size_t size, size_t count[257]);
    static inline Buf           Grab(char* string)
                                    { auto res = Buf(); res._str = string; res._size = strlen(string); return res; } ///< Create new object that takes control of input string.
    static inline Buf           Grab(char* buffer, size_t size)
                                    { auto res = Buf(); res._str = buffer; res._size = size; return res; } ///< Create new object that takes control of input buffer.
    static Buf                  InsertCR(const char* buffer, size_t size, bool dos, bool trailing = false);
    static Buf                  RemoveCR(const char* buffer, size_t size);

private:
    char*                       _str;
    size_t                      _size;

};

/***
 *      ______ _ _
 *     |  ____(_) |
 *     | |__   _| | ___
 *     |  __| | | |/ _ \
 *     | |    | | |  __/
 *     |_|    |_|_|\___|
 *
 *
 */

/** @brief Portable file information class.
*
* Paths in windows are always slashes.
*
*/
class File {
public:
    explicit                    File(const std::string& path = "", bool realpath = false);
    bool                        operator==(const File& other) const
                                    { return _filename == other._filename; } ///< Compare filenames.
    bool                        operator<(const File& other) const
                                    { return _filename < other._filename; } ///< Compare filenames.
    bool                        operator<=(const File& other) const
                                    { return _filename <= other._filename; } ///< Compare filenames.
    const char*                 c_str() const
                                    { return _filename.c_str(); } ///< Return filename.
    std::string                 canonical_name() const
                                    { return file::canonical_name(_filename); } ///< Return canonical file name.
    int64_t                     ctime() const
                                    { return _ctime; } ///< Return created file time.
    void                        debug(bool short_version = true) const
                                    { printf("%s\n", to_string(short_version).c_str()); fflush(stdout); } ///< Print file info to stdout.
    bool                        exist() const
                                    { return _type != TYPE::MISSING; } ///< Does file exist?
    const std::string&          ext() const
                                    { return _ext; } ///< Return file extension, not for directories.
    const std::string&          filename() const
                                    { return _filename; } ///< Return full filename.
    bool                        is_dir() const
                                    { return _type == TYPE::DIR; } ///< Is file a directory?
    bool                        is_file() const
                                    { return _type == TYPE::FILE; } ///< Is file a plain file?
    bool                        is_link() const
                                    { return _link; } ///< Is file a link?
    bool                        is_missing() const
                                    { return _type == TYPE::MISSING; }  ///< Is file missing?
    bool                        is_other() const
                                    { return _type == TYPE::OTHER; }  ///< Is file something else?
    std::string                 linkname() const;
    int                         mode() const
                                    { return _mode; } ///< Return file mode.
    int64_t                     mtime() const
                                    { return _mtime; } ///< Return file modified time.
    const std::string&          name() const
                                    { return _name; } ///< Return name without path.
    std::string                 name_without_ext() const;
    const std::string&          path() const
                                    { return _path; } ///< Return parent path.
    int64_t                     size() const
                                    { return _size; } ///< Return size in bytes.
    std::string                 to_string(bool short_version = true) const;
    TYPE                        type() const
                                    { return _type; } ///< Return file type.
    std::string                 type_name() const;

private:
    TYPE                        _type;
    bool                        _link;
    int                         _mode;
    int64_t                     _ctime;
    int64_t                     _mtime;
    int64_t                     _size;
    std::string                 _ext;
    std::string                 _filename;
    std::string                 _name;
    std::string                 _path;
};

} // file
} // gnu

// MKALGAM_OFF

#endif // GNU_FILE_H
