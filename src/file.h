// Copyright gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#ifndef FLW_FILE_H
#define FLW_FILE_H

// MKALGAM_ON

#include <cstdint>
#include <cstring>
#include <string>
#include <vector>

namespace flw {

class File;

typedef bool (*CallbackCopy)(int64_t size, int64_t copied, void* data);
typedef std::vector<File> FileVector;

/***
 *      ______ _ _      ____         __
 *     |  ____(_) |    |  _ \       / _|
 *     | |__   _| | ___| |_) |_   _| |_
 *     |  __| | | |/ _ \  _ <| | | |  _|
 *     | |    | | |  __/ |_) | |_| | |
 *     |_|    |_|_|\___|____/ \__,_|_|
 *
 *
 */

//------------------------------------------------------------------------------
// Store data from file.
//
struct FileBuf {
    char*                       p;
    size_t                      s;

                                FileBuf()
                                    { p = nullptr; s = 0; }
    explicit                    FileBuf(size_t S);
                                FileBuf(const char* P, size_t S);
                                FileBuf(const FileBuf& b);
                                FileBuf(FileBuf&& b)
                                    { p = b.p; s = b.s; b.p = nullptr; }
                                FileBuf(const std::string& S)
                                    { p = nullptr; add(S.c_str(), S.length()); }
    virtual                     ~FileBuf()
                                    { free(p); }
    FileBuf&                    operator=(const FileBuf& b)
                                    { return set(b.p, b.s); }
    FileBuf&                    operator=(FileBuf&& b)
                                    { free(p); p = b.p; s = b.s; b.p = nullptr; return *this; }
    FileBuf&                    operator=(const std::string& S)
                                    { free(p); p = nullptr; add(S.c_str(), S.length()); return *this; }
    FileBuf&                    operator+=(const FileBuf& b)
                                    { return add(b.p, b.s); }
    bool                        operator==(const FileBuf& other) const;
    bool                        operator!=(const FileBuf& other) const
                                    { return (*this == other) == false; }
    FileBuf&                    add(const char* P, size_t S);
    void                        clear()
                                    { free(p); p = nullptr; s = 0; }
    void                        count(size_t count[257]) const
                                    { FileBuf::Count(p, s, count); }
    void                        debug() const
                                    { printf("gnu::FileBuf(0x%p, %u)\n", p, (unsigned) s); }
    uint64_t                    fletcher64() const
                                    { return FileBuf::Fletcher64(p, s); }
    FileBuf&                    grab(char* P, size_t S)
                                    { free(p); p = P; s = S; return *this; }
    FileBuf                     insert_cr(bool dos = true, bool trailing = false) const
                                    { return FileBuf::InsertCR(p, s, dos, trailing); }
    char*                       release()
                                    { auto res = p; p = nullptr; s = 0; return res; }
    FileBuf                     remove_cr() const
                                    { return FileBuf::RemoveCR(p, s); }
    FileBuf&                    set(const char* P, size_t S);
    bool                        write(std::string filename) const;

    static void                 Count(const char* P, size_t S, size_t count[257]);
    static uint64_t             Fletcher64(const char* p, size_t s);
    static inline FileBuf       Grab(char* P)
                                    { auto res = FileBuf(); res.p = P; res.s = strlen(P); return res; }
    static inline FileBuf       Grab(char* P, size_t S)
                                    { auto res = FileBuf(); res.p = P; res.s = S; return res; }
    static FileBuf              InsertCR(const char* P, size_t S, bool dos, bool trailing = false);
    static FileBuf              RemoveCR(const char* P, size_t S);
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
// It also have some static file(directory functions.
//
class File {
public:

#ifdef _WIN32
    static const int            DEFAULT_DIR_MODE  = 0x00000080;
    static const int            DEFAULT_FILE_MODE = 0x00000080;
#else
    static const int            DEFAULT_DIR_MODE  = 0755;
    static const int            DEFAULT_FILE_MODE = 0664;
#endif
    enum class TYPE {
                                MISSING,
                                DIR,
                                FILE,
                                OTHER,
    };

    File::TYPE                  type;
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
    std::string                 canonicalname() const
                                    { return File::CanonicalName(filename); }
    void                        debug(bool short_version = true) const
                                    { printf("%s\n", to_string(short_version).c_str()); fflush(stdout); }
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
    std::string                 type_name() const;
    std::string                 to_string(bool short_version = true) const;

    static char*                Allocate(char* resize_or_null, size_t size, bool exception = true);
    static std::string          CanonicalName(std::string filename);
    static bool                 ChDir(std::string path);
    static std::string          CheckFilename(std::string filename);
    static bool                 ChMod(std::string path, int mode);
    static FileBuf              CloseStderr();
    static FileBuf              CloseStdout();
    static bool                 Copy(std::string from, std::string to, CallbackCopy callback = nullptr, void* data = nullptr);
    static std::string          HomeDir();
    static bool                 MkDir(std::string path);
    static bool                 ModTime(std::string path, int64_t time);
    static FILE*                Open(std::string path, std::string mode);
    static std::string          OS();
    static FILE*                Popen(std::string cmd, bool write = false);
    static FileBuf              Read(std::string path);
    static FileBuf*             Read2(std::string path);
    static FileVector           ReadDir(std::string path);
    static FileVector           ReadDirRec(std::string path);
    static bool                 RedirectStderr();
    static bool                 RedirectStdout();
    static bool                 Remove(std::string path);
    static bool                 RemoveRec(std::string path);
    static bool                 Rename(std::string from, std::string to);
    static int                  Run(std::string cmd, bool background, bool hide_win32_window = false);
    static File                 TmpDir();
    static File                 TmpFile(std::string prepend = "");
    static File                 WorkDir();
    static bool                 Write(std::string filename, const char* in, size_t in_size);
    static inline bool          Write(std::string filename, const FileBuf& b)
                                    { return Write(filename, b.p, b.s); }
    };
} // flw

// MKALGAM_OFF

#endif
