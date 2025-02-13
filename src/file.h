// Copyright gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#ifndef GNU_FILE_H
#define GNU_FILE_H

// MKALGAM_ON

#include <cstdint>
#include <cstring>
#include <string>
#include <vector>

namespace gnu {
namespace file {

class File;
struct Buf;

typedef bool (*CallbackCopy)(int64_t size, int64_t copied, void* data);
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
std::string                     canonical_name(std::string filename);
bool                            chdir(std::string path);
std::string                     check_filename(std::string filename);
bool                            chmod(std::string path, int mode);
Buf                             close_stderr();
Buf                             close_stdout();
bool                            copy(std::string from, std::string to, CallbackCopy callback = nullptr, void* data = nullptr);
uint64_t                        fletcher64(const char* p, size_t s);
File                            home_dir();
bool                            mkdir(std::string path);
bool                            mod_time(std::string path, int64_t time);
FILE*                           open(std::string path, std::string mode);
std::string                     os();
FILE*                           popen(std::string cmd, bool write = false);
Buf                             read(std::string path);
Buf*                            read2(std::string path);
Files                           read_dir(std::string path);
Files                           read_dir_rec(std::string path);
bool                            redirect_stderr();
bool                            redirect_stdout();
bool                            remove(std::string path);
bool                            remove_rec(std::string path);
bool                            rename(std::string from, std::string to);
int                             run(std::string cmd, bool background, bool hide_win32_window = false);
File                            tmp_dir();
File                            tmp_file(std::string prepend = "");
File                            work_dir();
bool                            write(std::string filename, const char* in, size_t in_size);
bool                            write(std::string filename, const Buf& b);

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

//------------------------------------------------------------------------------
// Store data from file.
//
struct Buf {
    char*                       p;
    size_t                      s;

                                Buf()
                                    { p = nullptr; s = 0; }
    explicit                    Buf(size_t S);
                                Buf(const char* P, size_t S);
                                Buf(const Buf& b);
                                Buf(Buf&& b)
                                    { p = b.p; s = b.s; b.p = nullptr; }
                                Buf(const std::string& S)
                                    { p = nullptr; add(S.c_str(), S.length()); }
    virtual                     ~Buf()
                                    { free(p); }
    Buf&                        operator=(const Buf& b)
                                    { return set(b.p, b.s); }
    Buf&                        operator=(Buf&& b)
                                    { free(p); p = b.p; s = b.s; b.p = nullptr; return *this; }
    Buf&                        operator=(const std::string& S)
                                    { free(p); p = nullptr; add(S.c_str(), S.length()); return *this; }
    Buf&                        operator+=(const Buf& b)
                                    { return add(b.p, b.s); }
    bool                        operator==(const Buf& other) const;
    bool                        operator!=(const Buf& other) const
                                    { return (*this == other) == false; }
    Buf&                        add(const char* P, size_t S);
    void                        clear()
                                    { free(p); p = nullptr; s = 0; }
    void                        count(size_t count[257]) const
                                    { Buf::Count(p, s, count); }
    void                        debug() const
                                    { printf("gnu::Buf(0x%p, %u)\n", p, (unsigned) s); }
    uint64_t                    fletcher64() const
                                    { return file::fletcher64(p, s); }
    Buf&                        grab(char* P, size_t S)
                                    { free(p); p = P; s = S; return *this; }
    Buf                         insert_cr(bool dos = true, bool trailing = false) const
                                    { return Buf::InsertCR(p, s, dos, trailing); }
    char*                       release()
                                    { auto res = p; p = nullptr; s = 0; return res; }
    Buf                         remove_cr() const
                                    { return Buf::RemoveCR(p, s); }
    Buf&                        set(const char* P, size_t S);
    bool                        write(std::string filename) const;

    static void                 Count(const char* P, size_t S, size_t count[257]);
    static inline Buf           Grab(char* P)
                                    { auto res = Buf(); res.p = P; res.s = strlen(P); return res; }
    static inline Buf           Grab(char* P, size_t S)
                                    { auto res = Buf(); res.p = P; res.s = S; return res; }
    static Buf                  InsertCR(const char* P, size_t S, bool dos, bool trailing = false);
    static Buf                  RemoveCR(const char* P, size_t S);
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

//------------------------------------------------------------------------------
// File information class.
//
class File {
public:
    TYPE                        type;
    bool                        link;
    int                         mode;
    int64_t                     ctime;
    int64_t                     mtime;
    int64_t                     size;
    std::string                 ext;
    std::string                 filename;
    std::string                 name;
    std::string                 path;

                                File()
                                    { update(""); }
    explicit                    File(std::string in, bool realpath = false)
                                    { update(in, realpath); }
    File&                       operator=(std::string in)
                                    { return update(in); }
    File&                       operator+=(std::string in)
                                    { filename += in; return *this; }
    bool                        operator==(const File& other) const
                                    { return filename == other.filename; }
    bool                        operator<(const File& other) const
                                    { return filename < other.filename; }
    bool                        operator<=(const File& other) const
                                    { return filename <= other.filename; }
    const char*                 c_str() const
                                    { return filename.c_str(); }
    std::string                 canonical_name() const
                                    { return file::canonical_name(filename); }
    void                        debug(bool short_version = true) const
                                    { printf("%s\n", to_string(short_version).c_str()); fflush(stdout); }
    bool                        exist() const
                                    { return type != TYPE::MISSING; }
    bool                        is_circular() const;
    bool                        is_dir() const
                                    { return type == TYPE::DIR; }
    bool                        is_file() const
                                    { return type == TYPE::FILE; }
    bool                        is_link() const
                                    { return link; }
    bool                        is_missing() const
                                    { return type == TYPE::MISSING; }
    bool                        is_other() const
                                    { return type == TYPE::OTHER; }
    std::string                 linkname() const;
    std::string                 name_without_ext() const;
    File&                       update();
    File&                       update(std::string in, bool realpath = false);
    std::string                 to_string(bool short_version = true) const;
    std::string                 type_name() const;
};

} // file
} // gnu

// MKALGAM_OFF

#endif // GNU_FILE_H
