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

#include "file.h"

// MKALGAM_ON

#include <algorithm>
#include <filesystem>
#include <climits>
#include <ctime>
#include <assert.h>
#include <dirent.h>
#include <unistd.h>

#ifdef _WIN32
    #include <shlobj.h>
    #include <time.h>
#else
    #include <sys/stat.h>
    #include <utime.h>
#endif

#ifndef PATH_MAX
    #define PATH_MAX 1050
#endif

namespace gnu {
namespace file {

/***
 *                 _            _
 *                (_)          | |
 *      _ __  _ __ ___   ____ _| |_ ___
 *     | '_ \| '__| \ \ / / _` | __/ _ \
 *     | |_) | |  | |\ V / (_| | ||  __/
 *     | .__/|_|  |_| \_/ \__,_|\__\___|
 *     | |
 *     |_|
 */

static std::string          _STDOUT_NAME = "";
static std::string          _STDERR_NAME = "";

#ifdef _WIN32
static char*                _from_wide(const wchar_t* wstring);
static int64_t              _time(FILETIME* ft);
static wchar_t*             _to_wide(const char* string);
#endif

static Buf                  _close_redirect(int type);
static bool                 _open_redirect(int type);
static unsigned             _rand();
static void                 _read(const std::string& path, Buf& buf);
static std::string&         _replace_all(std::string& string, const std::string& find, const std::string& replace);
static void                 _read_dir_rec(std::vector<File>& res, std::vector<File>& files);
static std::string&         _replace_all(std::string& string, const std::string& find, const std::string& replace);
static void                 _split_paths(const std::string& filename, std::string& path, std::string& name, std::string& ext);
static std::string          _substr(const std::string& in, std::string::size_type pos, std::string::size_type size = std::string::npos);
static std::string          _to_absolute_path(const std::string& filename, bool realpath);

/** @brief Close redirect.
*
* @param[in] type  1 (stdout) or 2 (stderr).
*
* @return Read bytes from file.
*/
static Buf _close_redirect(int type) {
    std::string fname;
    FILE* fhandle;

    if (type == 2) {
        if (_STDERR_NAME == "") return Buf();
        fname = _STDERR_NAME;
        fhandle = stderr;
        _STDERR_NAME = "";
    }
    else {
        if (_STDOUT_NAME == "") return Buf();
        fname = _STDOUT_NAME;
        fhandle = stdout;
        _STDOUT_NAME = "";
    }

#ifdef _WIN32
    fflush(fhandle);
    freopen("CON", "w", fhandle);
#else
    fflush(fhandle);
    auto r = freopen("/dev/tty", "w", fhandle);
    (void) r;
#endif

    auto res = file::read(fname);
    file::remove(fname);

    return res;
}

#ifdef _WIN32
/** @brief Convert wide to utf.
*
* @param[in] wstring  Wide character string.
*
* @return UTF string, delete with free().
*/
static char* _from_wide(const wchar_t* wstring) {
    auto out_len = WideCharToMultiByte(CP_UTF8, 0, wstring, -1, nullptr, 0, nullptr, nullptr);
    auto out     = file::allocate(nullptr, out_len + 1);

    WideCharToMultiByte(CP_UTF8, 0, wstring, -1, (LPSTR) out, out_len, nullptr, nullptr);

    return (char*) out;
}
#endif

/** @brief Open redirect.
*
* Open result file.
*
* @param[in] type  1 (stdout) or 2 (stderr).
*
* @return True if ok.
*/
static bool _open_redirect(int type) {
    bool res = false;
    std::string fname;
    FILE* fhandle = nullptr;

    if (type == 2) {
        if (_STDERR_NAME != "") return res;
        fname = _STDERR_NAME = file::tmp_file("stderr_").filename();
        fhandle = stderr;
    }
    else {
        if (_STDOUT_NAME != "") return res;
        fname = _STDOUT_NAME = file::tmp_file("stdout_").filename();
        fhandle = stdout;
    }

#ifdef _WIN32
        auto wpath = file::_to_wide(fname.c_str());
        auto wmode = file::_to_wide("wb");

        res = _wfreopen(wpath, wmode, fhandle) != nullptr;
        free(wpath);
        free(wmode);
#else
        res = freopen(fname.c_str(), "wb", fhandle) != nullptr;
#endif

    if (res == false && type == 1) {
        _STDOUT_NAME = "";
    }
    else if (res == false && type == 2) {
        _STDERR_NAME = "";
    }

    return res;
}

/** @brief Generate random number.
*
* @return Random number.
*/
static unsigned _rand() {
    static bool INIT = false;

    if (INIT == false) {
        srand(time(nullptr));
    }

    INIT = true;

    static unsigned long next = 1;

    if (RAND_MAX < 50000) {
        next = next * 1103515245 + 12345;
        return ((unsigned)(next / 65536) % 32768);
    }
    else {
        return rand();
    }
}

/** @brief Read bytes from file.
*
* @param[in] path  File path.
* @param[in] buf   Result buffer.
*/
static void _read(const std::string& path, Buf& buf) {
    assert(buf.c_str() == nullptr && buf.size() == 0);

    File file(path);

    if (file.is_file() == false || static_cast<long long unsigned int>(file.size()) > SSIZE_MAX) {
        return;
    }

    auto out = file::allocate(nullptr, file.size() + 1);

    if (file.size() == 0) {
        buf.grab(out, 0);
        return;
    }

    auto handle = file::open(file.filename(), "rb");

    if (handle == nullptr) {
        free(out);
        return;
    }
    else if (fread(out, 1, static_cast<size_t>(file.size()), handle) != static_cast<size_t>(file.size())) {
        fclose(handle);
        free(out);
        return;
    }

    fclose(handle);
    buf.grab(out, file.size());
}

/** @brief Read file entries recursive.
*
* @param[in] res    Result vector.
* @param[in] files  Files from current directory.
*/
static void _read_dir_rec(std::vector<File>& res, std::vector<File>& files) {
    for (auto& file : files) {
        res.push_back(file);

        if (file.type() == file::TYPE::DIR && file.is_link() == false) {
            auto v = file::read_dir(file.filename());
            file::_read_dir_rec(res, v);
        }
    }
}

/** @brief Replace strings.
*
* @param[in] string   String to be replaced in.
* @param[in] find     Find this string.
* @param[in] replace  Replace with this string.
*
* @return Input string.
*/

static std::string& _replace_all(std::string& string, const std::string& find, const std::string& replace) {
    if (find.empty() == true) {
        return string;
    }

    size_t start = 0;

    while ((start = string.find(find, start)) != std::string::npos) {
        string.replace(start, find.length(), replace);
        start += replace.length();
    }

    return string;
}

/** @brief Split full filename.
*
* @param[in] filename  Filename to split.
* @param[in] path      Result path.
* @param[in] name      Result name + ext.
* @param[in] ext       Extension only.
*/
static void _split_paths(const std::string& filename, std::string& path, std::string& name, std::string& ext) {
    path = "";
    name = "";
    ext  = "";

    if (filename == "") {
        return;
    }

#ifdef _WIN32
    auto sep = '/';

    if (filename.find("\\\\") == 0) {
        sep = '\\';

        if (filename.back() == '\\') {
            return;
        }
    }

    auto pos1 = filename.find_last_of(sep);

    if (pos1 != std::string::npos) {
        if (filename.length() != 3) {
            path = file::_substr(filename, 0, pos1);
        }

        name = file::_substr(filename, pos1 + 1);
    }

    auto pos2 = name.find_last_of('.');

    if (pos2 != std::string::npos && pos2 != 0) {
        ext = file::_substr(name, pos2 + 1);
    }

    if (path.back() == ':') {
        path += sep;
        return;
    }
#else
    auto pos1 = filename.find_last_of('/');

    if (pos1 != std::string::npos) {
        if (pos1 > 0) {
            path = file::_substr(filename, 0, pos1);
        }
        else if (filename != "/") {
            path = "/";
        }

        if (filename != "/") {
            name = file::_substr(filename, pos1 + 1);
        }
    }

    auto pos2 = filename.find_last_of('.');

    if (pos2 != std::string::npos && pos2 > pos1 + 1) {
        ext = file::_substr(filename, pos2 + 1);
    }
#endif
}

/** @brief Get substring.
*
* @param[in] string  String to be replaced in.
* @param[in] pos     Position in string.
* @param[in] count   Number of bytes.
*
* @return Replaced string.
*/
static std::string _substr(const std::string& in, std::string::size_type pos, std::string::size_type count) {
    try { return in.substr(pos, count); }
    catch(...) { return ""; }
}

#ifdef _WIN32
/** @brief Convert FILETIME to seconds.
*
* @param[in] ft  Filetime.
*
* @return Seconds.
*/
static int64_t _time(FILETIME* ft) {
    int64_t res = static_cast<int64_t>(ft->dwHighDateTime) << 32 | static_cast<int64_t>(ft->dwLowDateTime);
    res = res / 10000000;
    res = res - 11644473600;
    return res;
}
#endif

/** @brief Convert filename to absolute path.
*
* @param[in] filename  Filename to convert.
* @param[in] realpath  True to use the real file path.
*
* @return Absolute path.
*/
static std::string _to_absolute_path(const std::string& filename, bool realpath) {
    std::string res;
    auto name = filename;

#ifdef _WIN32
    if (name.find("\\\\") == 0) {
        res = name;

        return name;
    }
    else if (name.size() < 2 || name[1] != ':') {
        auto work = File(file::work_dir());
        res = work.filename();
        res += "\\";
        res += name;
    }
    else {
        res = name;
    }

    file::_replace_all(res, "\\", "/");

    auto len = res.length();
    file::_replace_all(res, "//", "/");

    while (len > res.length()) {
        len = res.length();
        file::_replace_all(res, "//", "/");
    }

    while (res.size() > 3 && res.back() == '/') {
        res.pop_back();
    }
#else
    if (name[0] != '/') {
        auto work = File(file::work_dir());
        res = work.filename();
        res += "/";
        res += name;
    }
    else {
        res = name;
    }

    auto len = res.length();
    file::_replace_all(res, "//", "/");

    while (len > res.length()) {
        len = res.length();
        file::_replace_all(res, "//", "/");
    }

    while (res.size() > 1 && res.back() == '/') {
        res.pop_back();
    }
#endif

    return (realpath == true) ? file::canonical_name(res) : res;
}

#ifdef _WIN32
/** @brief Convert utf to wide.
*
* @param[in] string  UTF character string.
*
* @return Wide character string, delete with free().
*/
static wchar_t* _to_wide(const char* string) {
    auto out_len = MultiByteToWideChar(CP_UTF8, 0, string , -1, nullptr , 0);
    auto out     = reinterpret_cast<wchar_t*>(file::allocate(nullptr, out_len * sizeof(wchar_t) + sizeof(wchar_t)));

    MultiByteToWideChar(CP_UTF8, 0, string , -1, out, out_len);

    return out;
}
#endif

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

/** @brief Allocate memory.
*
* New memory is set to 0.\n
* But not reallocated memory.\n
*
* @param[in] resize_or_null  Pointer for reallocation or NULL to allocate new memory.
* @param[in] size            Size in bytes.
*
* @return Allocated memory, delete with free().
*
* @throws std::string exception on error.
*/
char* allocate(char* resize_or_null, size_t size) {
    void* res = nullptr;

    if (resize_or_null == nullptr) {
        res = calloc(size, 1);
    }
    else {
        res = realloc(resize_or_null, size);
    }

    if (res == nullptr) {
        throw std::string("error: gnu::file::allocate(): memory allocation failed");
    }

    return (char*) res;
}

/** @brief Convert filename to a canonicalized absolute pathname.
*
* @param[in] path  Path to convert.
*
* @return Result filename or input path for enu error.
*/
std::string canonical_name(const std::string& path) {
#if defined(_WIN32)
    wchar_t wres[PATH_MAX];
    auto    wpath = file::_to_wide(path.c_str());
    auto    len   = GetFullPathNameW(wpath, PATH_MAX, wres, nullptr);

    if (len > 0 && len < PATH_MAX) {
        auto cpath = file::_from_wide(wres);
        auto res   = std::string(cpath);

        free(cpath);
        free(wpath);

        file::_replace_all(res, "\\", "/");
        return res;
    }
    else {
        free(wpath);
        return path;
    }
#else
    auto tmp = realpath(path.c_str(), nullptr);
    auto res = (tmp != nullptr) ? std::string(tmp) : path;

    free(tmp);
    return res;
#endif
}

/** @brief Change directory.
*
* @param[in] path  Directory path.
*
* @return True if ok.
*/
bool chdir(const std::string& path) {
#ifdef _WIN32
    auto wpath = file::_to_wide(path.c_str());
    auto res   = _wchdir(wpath);

    free(wpath);
    return res == 0;
#else
    return ::chdir(path.c_str()) == 0;
#endif
}

/** @brief Check filename for invalid characters.
*
* @param[in] name  Filename without any path.
*
* @return A copy of input filename or a filename without illegal characters.
*/
std::string check_filename(const std::string& name) {
    static const std::string ILLEGAL = "<>:\"/\\|?*\n\t\r";
    std::string res;

    for (auto& c : name) {
        if (ILLEGAL.find(c) == std::string::npos) {
            res += c;
        }
    }

    return res;
}

/** @brief Change file mode.
*
* @param[in] path  Path.
* @param[in] mode  OS dependent value.
*
* @return True if ok.
*/
bool chmod(const std::string& path, int mode) {
    auto res = false;

    if (mode < 0) {
        return false;
    }

#ifdef _WIN32
    auto wpath = file::_to_wide(path.c_str());

    res = SetFileAttributesW(wpath, mode);
    free(wpath);
#else
    res = ::chmod(path.c_str(), mode) == 0;
#endif

    return res;
}

/** @brief Set modified time.
*
* @param[in] path  File path.
* @param[in] time  Time in seconds since epoch (>= 0).
*
* @return True if ok.
*/
bool chtime(const std::string& path, int64_t time) {
    auto res = false;

#ifdef _WIN32
    auto wpath  = file::_to_wide(path.c_str());
    auto handle = CreateFileW(wpath, GENERIC_WRITE, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

    if (handle != INVALID_HANDLE_VALUE) {
        FILETIME ftLastAccessTime;
        FILETIME ftLastWriteTime;
        auto     lm = (LONGLONG) 0;

        lm = Int32x32To64((time_t) time, 10000000) + 116444736000000000;
        ftLastAccessTime.dwLowDateTime  = (DWORD)lm;
        ftLastAccessTime.dwHighDateTime = lm >> 32;
        ftLastWriteTime.dwLowDateTime   = (DWORD)lm;
        ftLastWriteTime.dwHighDateTime  = lm >> 32;

        res = SetFileTime(handle, nullptr, &ftLastAccessTime, &ftLastWriteTime);
        CloseHandle(handle);
    }

    free(wpath);
#else
    utimbuf ut;

    ut.actime  = (time_t) time;
    ut.modtime = (time_t) time;
    res        = utime(path.c_str(), &ut) == 0;
#endif

    return res;
}

/** @brief Close redirect.
*
* @return Data from stderr.
*/
Buf close_stderr() {
    return file::_close_redirect(2);
}

/** @brief Close redirect.
*
* @return Data from stdout.
*/
Buf close_stdout() {
    return file::_close_redirect(1);
}

/** @brief Copy file.
*
* @param[in] from   From path.
* @param[in] to     To path.
* @param[in] cb     Callback between every time (optional).
* @param[in] data   Callback data (optional).
* @param[in] flush  True to call flush after file have been copied (optional, default true).
*
* @return True if ok.
*/
bool copy(const std::string& from, const std::string& to, CallbackCopy cb, void* data, bool flush) {
#ifdef DEBUG
    static const size_t BUF_SIZE = 16384;
#else
    static const size_t BUF_SIZE = 131072;
#endif

    auto file1 = File(from);
    auto file2 = File(to);

    if (file1 == file2) {
        return false;
    }

    auto buf   = file::allocate(nullptr, BUF_SIZE);
    auto read  = file::open(from, "rb");
    auto write = file::open(to, "wb");
    auto count = static_cast<int64_t>(0);
    auto size  = static_cast<size_t>(0);

    if (read == nullptr || write == nullptr) {
        if (read != nullptr) {
            fclose(read);
        }

        if (write != nullptr) {
            fclose(write);
            file::remove(to);
        }

        free(buf);
        return false;
    }

    while ((size = fread(buf, 1, BUF_SIZE, read)) > 0) {
        if (fwrite(buf, 1, size, write) != size) {
            break;
        }

        count += size;

        if (cb != nullptr && cb(file1.size(), count, data) == false && count != file1.size()) {
            break;
        }
    }

    fclose(read);

    if (flush == true) {
        file::flush(write);
    }

    fclose(write);
    free(buf);

    if (count != file1.size()) {
        file::remove(to);
        return false;
    }

    file::chtime(to, file1.mtime());
    file::chmod(to, file1.mode());

    return true;
}

/** @brief Create fletcher checksum.
*
* @param[in] buffer       Input buffer.
* @param[in] buffer_size  Buffer size.
*
* @return Checksum.
*/
uint64_t fletcher64(const char* buffer, size_t buffer_size) {
    if (buffer == nullptr || buffer_size == 0) {
        return 0;
    }

    auto u8data = reinterpret_cast<const uint8_t*>(buffer);
    auto dwords = static_cast<uint64_t>(buffer_size / 4);
    auto sum1   = static_cast<uint64_t>(0);
    auto sum2   = static_cast<uint64_t>(0);
    auto data32 = reinterpret_cast<const uint32_t*>(u8data);
    auto left   = static_cast<uint64_t>(0);

    for (uint64_t f = 0; f < dwords; ++f) {
        sum1 = (sum1 + data32[f]) % UINT32_MAX;
        sum2 = (sum2 + sum1) % UINT32_MAX;
    }

    left = buffer_size - dwords * 4;

    if (left > 0) {
        auto tmp  = static_cast<uint32_t>(0);
        auto byte = reinterpret_cast<uint8_t*>(&tmp);

        for (auto f = static_cast<uint64_t>(0); f < left; ++f) {
            byte[f] = u8data[dwords * 4 + f];
        }

        sum1 = (sum1 + tmp) % UINT32_MAX;
        sum2 = (sum2 + sum1) % UINT32_MAX;
    }

    return (sum2 << 32) | sum1;
}

/**
* @brief Flush file handle.
*
* @param[in,out] file  File handle.
*/
void flush(FILE* file) {
    if (file != nullptr) {
#ifdef _WIN32
        auto handle = (HANDLE) _get_osfhandle(_fileno(file));

        if (handle != INVALID_HANDLE_VALUE) {
            FlushFileBuffers(handle);
        }
#else
        fsync(fileno(file));
#endif
    }
}

/**
* @brief Get home directory.
*
* @return Home directory or work directory for any failure.
*/
File home_dir() {

#ifdef _WIN32
    wchar_t wpath[PATH_MAX];

    if (SHGetFolderPathW(nullptr, CSIDL_PROFILE, nullptr, 0, wpath) == S_OK) {
        auto path = file::_from_wide(wpath);
        auto res = File(path);
        free(path);
        return res;
    }
#else
    const char* path = getenv("HOME");

    if (path != nullptr) {
        return File(path);
    }
#endif

    return work_dir();
}

/**
* @brief Is link to a directory a circular one?
*
* @param[in] path  Path to file.
*
* @return True if directory is circular.
*/
bool is_circular(const std::string& path) {
    auto file = File(path, false);

    if (file.type() != TYPE::DIR || file.is_link() == false) {
        return false;
    }

    auto l = file.canonical_name() + "/";

    return file.filename().find(l) == 0;
}

/** @brief Create a directory.
*
* Default file mode is file::DEFAULT_DIR_MODE.
*
* @param[in] path  Path to directory.
*
* @return True if ok.
*/
bool mkdir(const std::string& path) {
    bool res = false;

#ifdef _WIN32
    auto wpath = file::_to_wide(path.c_str());

    res = _wmkdir(wpath) == 0;
    free(wpath);
#else
    res = ::mkdir(path.c_str(), file::DEFAULT_DIR_MODE) == 0;
#endif

    return res;
}

/**
* @brief Open file handle.
*
* @param[in] path  Path to file.
* @param[in] mode  File mode.
*
* @return File handle or NULL.
*/
FILE* open(const std::string& path, const std::string& mode) {
    FILE* res = nullptr;

#ifdef _WIN32
    auto wpath = file::_to_wide(path.c_str());
    auto wmode = file::_to_wide(mode.c_str());

    res = _wfopen(wpath, wmode);
    free(wpath);
    free(wmode);
#else
    res = fopen(path.c_str(), mode.c_str());
#endif

    return res;
}

/**
* @brief Return operating system name.
*
* @return "windows", "macos", "linux" or "unknown".
*/
std::string os() {
#if defined(_WIN32)
    return "windows";
#elif defined(__APPLE__)
    return "macos";
#elif defined(__linux__)
    return "linux";
#elif defined(__unix__)
    return "unix";
#else
    return "unknown";
#endif
}

/**
* @brief Open process handle.
*
* @param[in] cmd    Command.
* @param[in] write  True to write to process.
*
* @return File handle or NULL.
*/
FILE* popen(const std::string& cmd, bool write) {
    FILE* file = nullptr;

#ifdef _WIN32
    auto wpath = file::_to_wide(cmd.c_str());
    auto wmode = file::_to_wide(write ? "wb" : "rb");

    file = _wpopen(wpath, wmode);
    free(wpath);
    free(wmode);
#else
    ::fflush(nullptr);
    file = ::popen(cmd.c_str(), (write == true) ? "w" : "r");
#endif

    return file;
}

/**
* @brief Read file.
*
* @param[in] path  Path to file.
*
* @return Read data or empty for any error.
*/
Buf read(const std::string& path) {
    Buf buf;
    file::_read(path, buf);
    return buf;
}

/**
* @brief Read file.
*
* @param[in] path  Path to file.
*
* @return Read data or empty for any error.
*/
Buf* read2(const std::string& path) {
    auto buf = new Buf();
    file::_read(path, *buf);
    return buf;
}

/** @brief Read directory.
*
* @param[in] path  Path to directory.
*
* @return Vector with files.
*/
std::vector<File> read_dir(const std::string& path) {
    auto file = File(path, false);
    auto res  = std::vector<File>();

    if (file.type() != TYPE::DIR || file::is_circular(path) == true) {
        return res;
    }

#ifdef _WIN32
    auto wpath = file::_to_wide(file.filename().c_str());
    auto dirp  = _wopendir(wpath);
    auto sep   = '/';

    if (file.filename().find("\\\\") == 0) {
        sep = '\\';
    }

    if (dirp != nullptr) {
        auto entry = _wreaddir(dirp);

        while (entry != nullptr) {
            auto cpath = file::_from_wide(entry->d_name);

            if (strcmp(cpath, ".") != 0 && strcmp(cpath, "..") != 0) {
                auto name = (file.name() == ".") ? file.path() + sep + cpath : file.filename() + sep + cpath;
                res.push_back(File(name));
            }

            free(cpath);
            entry = _wreaddir(dirp);
        }

        _wclosedir(dirp);
    }

    free(wpath);
#else
    auto dirp = ::opendir(file.filename().c_str());

    if (dirp != nullptr) {
        auto entry = ::readdir(dirp);

        while (entry != nullptr) {
            if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
                auto name = (file.name() == ".") ? file.path() + "/" + entry->d_name : file.filename() + "/" + entry->d_name;
                res.push_back(File(name));
            }

            entry = ::readdir(dirp);
        }

        ::closedir(dirp);
    }
#endif

    std::sort(res.begin(), res.end());
    return res;
}

/** @brief Read directory and all child directories.
*
* @param[in] path  Path to directory.
*
* @return Vector with files.
*/
std::vector<File> read_dir_rec(const std::string& path) {
    auto res   = std::vector<File>();
    auto files = file::read_dir(path);

    file::_read_dir_rec(res, files);
    return res;
}

/** @brief Redirecting stderr to file.
*
* @return True if ok.
*/
bool redirect_stderr() {
    return _open_redirect(2);
}

/** @brief Redirecting stdout to file.
*
* @return True if ok.
*/
bool redirect_stdout() {
    return _open_redirect(1);
}

/** @brief Remove file or directory.
*
* @param[in] path  Path to file/directory.
*
* @return True if deleted, missing file or failure will return false.
*/
bool remove(const std::string& path) {
    auto f = File(path);

    if (f.type() == TYPE::MISSING && f.is_link() == false) {
        return false;
    }

    auto res = false;

#ifdef _WIN32
    auto wpath = file::_to_wide(path.c_str());

    if (f.type() == TYPE::DIR) {
        res = RemoveDirectoryW(wpath);
    }
    else {
        res = DeleteFileW(wpath);
    }

    if (res == false) {
        if (f.type() == TYPE::DIR) {
            file::chmod(path, file::DEFAULT_DIR_MODE);
            res = RemoveDirectoryW(wpath);
        }
        else {
            file::chmod(path, file::DEFAULT_FILE_MODE);
            res = DeleteFileW(wpath);
        }
    }

    free(wpath);
#else
    if (f.type() == TYPE::DIR && f.is_link() == false) {
        res = ::rmdir(path.c_str()) == 0;
    }
    else {
        res = ::unlink(path.c_str()) == 0;
    }
#endif

    return res;
}

/** @brief Remove file or directory.
*
* If it is a directory all its child directories will be deleted.
*
* @param[in] path  Path to file/directory.
*
* @return True if deleted, missing file or failure will return false.
*/
bool remove_rec(const std::string& path) {
    auto file = File(path, true);

    if (file == file::home_dir() || file.path() == "") {
        return false;
    }

    auto files = file::read_dir_rec(path);

    std::reverse(files.begin(), files.end());

    for (const auto& file : files) {
        file::remove(file.filename());
    }

    return file::remove(path);
}

/** @brief Rename file or directory.
*
* @param[in] from  Source path.
* @param[in] to    Destination path.
*
* @return True if ok.
*/
bool rename(const std::string& from, const std::string& to) {
    auto res    = false;
    auto from_f = File(from);
    auto to_f   = File(to);

    if (from_f == to_f) {
        return false;
    }

#ifdef _WIN32
    auto wfrom = file::_to_wide(from_f.filename().c_str());
    auto wto   = file::_to_wide(to_f.filename().c_str());

    if (to_f.type() == TYPE::DIR) {
        file::remove_rec(to_f.filename());
        res = MoveFileExW(wfrom, wto, MOVEFILE_COPY_ALLOWED | MOVEFILE_WRITE_THROUGH);
    }
    else if (to_f.type() == TYPE::MISSING) {
        res = MoveFileExW(wfrom, wto, MOVEFILE_COPY_ALLOWED | MOVEFILE_WRITE_THROUGH);
    }
    else {
        res = ReplaceFileW(wto, wfrom, nullptr, REPLACEFILE_WRITE_THROUGH, 0, 0);
    }

    free(wfrom);
    free(wto);
#else
    if (to_f.type() == TYPE::DIR) {
        file::remove_rec(to_f.filename());
    }

    res = ::rename(from_f.filename().c_str(), to_f.filename().c_str()) == 0;
#endif

    return res;
}

/** @brief Run a command.
*
* @param[in] cmd                Command line arguments.
* @param[in] background         Run in backgrund as a separate process.
* @param[in] hide_win32_window  True to hide window (does int work?).
*
* @return Return code from process or 1 for error.
*/
int run(const std::string& cmd, bool background, bool hide_win32_window) {
#ifdef _WIN32
    wchar_t*            cmd_w = file::_to_wide(cmd.c_str());
    STARTUPINFOW        startup_info;
    PROCESS_INFORMATION process_info;

    ZeroMemory(&startup_info, sizeof(STARTUPINFOW));
    ZeroMemory(&process_info, sizeof(PROCESS_INFORMATION));

    startup_info.cb          = sizeof(STARTUPINFOW);
    startup_info.dwFlags     = STARTF_USESHOWWINDOW;
    startup_info.wShowWindow = (hide_win32_window == true) ? SW_HIDE : SW_SHOW;

    if (CreateProcessW(nullptr, cmd_w, nullptr, nullptr, false, 0, nullptr, nullptr, &startup_info, &process_info) == 0) {
        free(cmd_w);
        return -1;
    }
    else {
        ULONG rc = 0;

        if (background == false) {
            WaitForSingleObject(process_info.hProcess, INFINITE);
            GetExitCodeProcess(process_info.hProcess, &rc);
        }

        CloseHandle(process_info.hThread);
        CloseHandle(process_info.hProcess);
        free(cmd_w);
        return (int) rc;
    }
#else
    (void) hide_win32_window;

    auto cmd2 = cmd;

    if (background == true) {
        cmd2 += " 2>&1 > /dev/null &";
    }

    return system(cmd2.c_str());
#endif
}

/** @brief Get temp directory.
*
* @return Tmp path.
*/
File tmp_dir() {
    try {
#if defined(_WIN32)
        auto path = std::filesystem::temp_directory_path();
        auto utf  = file::_from_wide(path.c_str());
        auto res = File(utf);
        free(utf);
        return res;
#elif defined(__APPLE__)
        return File("/tmp");
#else
        auto path = std::filesystem::temp_directory_path();
        return File(path);
#endif
    }
    catch(...) {
    }

    return file::work_dir();
}

/** @brief Get temp file.
*
* @return Tmp path.
*/
File tmp_file(const std::string& prepend) {
    char buf[100];
    snprintf(buf, 100, "%u", file::_rand());
    std::string res = prepend + buf;
    return File(tmp_dir().filename() + "/" + res);
}

/** @brief Get working directory.
*
* @return Working directory or "." for any error.
*/
File work_dir() {
#ifdef _WIN32
    auto wpath = _wgetcwd(nullptr, 0);

    if (wpath != nullptr) {
        auto path = file::_from_wide(wpath);
        auto res  = File(path);

        free(wpath);
        free(path);

        return res;
    }
#else
    auto path = getcwd(nullptr, 0);

    if (path != nullptr) {
        auto res = File(path);
        free(path);
        return res;
    }
#endif

    return File(".");
}

/**
* @brief Write data to file.
*
* @param[in] path    Destination filename.
* @param[in] buffer  Buffer to write.
* @param[in] size    Buffer size.
* @param[in] flush   True to flush after write, default true.
*
* @return True if ok.
*/
bool write(const std::string& path, const char* buffer, size_t size, bool flush) {
    if (File(path).type() == TYPE::DIR) {
        return false;
    }

    auto tmpfile = path + ".~tmp";
    auto file    = file::open(tmpfile, "wb");

    if (file == nullptr) {
        return false;
    }

    auto wrote = fwrite(buffer, 1, size, file);

    if (flush == true) {
        file::flush(file);
    }

    fclose(file);

    if (wrote != size) {
        file::remove(tmpfile);
        return false;
    }
    else if (file::rename(tmpfile, path) == false) {
        file::remove(tmpfile);
        return false;
    }

    return true;
}

/**
* @brief Write data to file.
*
* @param[in] path   Destination filename.
* @param[in] buf    Buffer to write.
* @param[in] flush  True to flush after write, default true.
*
* @return True if ok.
*/
bool write(const std::string& path, const Buf& buf, bool flush) {
    return write(path, buf.c_str(), buf.size(), flush);
}

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

/**
* @brief Create new buffer.
*
* One extra byte is automatically allocated.
*
* @param[in] size  Number of bytes.
*
* @throws std::string exception on error.
*/
Buf::Buf(size_t size) {
    if (size == (size_t) -1) {
        throw std::string("error: gnu::file::Buf(): size out of range");
    }

    _str = file::allocate(nullptr, size + 1);
    _size = size;
}

/**
* @brief Create new buffer and copy indata.
*
* One extra bytes is automatically allocated if input buffer is not NULL.
*
* @param[in] buffer  Buffer, if NULL then nothing will be allocated.
* @param[in] size    Number of bytes.
*
* @throws std::string exception on error.
*/
Buf::Buf(const char* buffer, size_t size) {
    if (size == (size_t) -1) {
        throw std::string("error: gnu::file::Buf(): size out of range");
    }

    if (buffer == nullptr) {
        _str  = nullptr;
        _size = 0;
    }
    else {
        _str  = file::allocate(nullptr, size + 1);
        _size = size;

        std::memcpy(_str, buffer, size);
    }
}

/**
* @brief Create new buffer and copy indata.
*
* One extra bytes is automatically allocated if input buffer is not NULL.
*
* @param[in] b  Buffer object to copy.
*
* @throws std::string exception on error.
*/
Buf::Buf(const Buf& b) {
    if (b._str == nullptr) {
        _str  = nullptr;
        _size = 0;
    }
    else {
        _str  = file::allocate(nullptr, b._size + 1);
        _size = b._size;

        std::memcpy(_str, b._str, b._size);
    }
}

/**
* @brief Compare two objects.
*
* @param[in] other  Object to compare with.
*
* @return True if equal, same size and equal bytes.
*/
bool Buf::operator==(const Buf& other) const {
    return _str != nullptr && _size == other._size && std::memcmp(_str, other._str, _size) == 0;
}

/**
* @brief Add buffer to object.
*
* If internal buffer is NULL then new memory will be allocated.
*
* @param[in] buffer  Buffer, if NULL then nothing will be added.
* @param[in] size    Number of bytes.
*
* @return True if equal, same size and equal bytes.
*
* @throws std::string exception on error.
*/
Buf& Buf::add(const char* buffer, size_t size) {
    if (size == (size_t) -1) {
        throw std::string("error: gnu::file::Buf:add(): size out of range");
    }

    if (_str == buffer || buffer == nullptr) {
    }
    else if (_str == nullptr) {
        _str = file::allocate(nullptr, size + 1);
        std::memcpy(_str, buffer, size);
        _size = size;
    }
    else if (size > 0) {
        auto t = file::allocate(nullptr, _size + size + 1);
        std::memcpy(t, _str, _size);
        std::memcpy(t + _size, buffer, size);
        free(_str);
        _str = t;
        _size += size;
    }

    return *this;
}

/**
* @brief Count all bytes in text file.
*
* @param[in] buffer  Buffer to count.
* @param[in] size    Number of bytes.
*
* @return An array with 257 values, value [256] contains byte count for the longest line.
*/
std::array<size_t, 257> Buf::Count(const char* buffer, size_t size) {
    size_t max_line     = 0;
    size_t current_line = 0;
    auto   count        = std::array<size_t, 257>{0};

    if (buffer == nullptr) {
        return count;
    }

    for (size_t f = 0; f < size; f++) {
        auto c = (unsigned char) buffer[f];

        count[c] += 1;

        if (current_line > max_line) {
            max_line = current_line;
        }

        if (c == 0 ||c == 10 || c == 13) {
            current_line = 0;
        }
        else {
            current_line++;
        }
    }

    count[256] = max_line;

    return count;
}

/**
* @brief Remove (optional) trailing space and insert (optional) "\r" for every "\n".
*
* For text only.
*
* @param[in] buffer    Input buffer.
* @param[in] size      Buffer size.
* @param[in] dos       Insert "\r" for every "\n".
* @param[in] trailing  True to remove trailing whitespace.
*
* @return Converted text or an object with NULL data if "trailing" and "dos" is false.
*
* @throws std::string exception on error.
*/
Buf Buf::InsertCR(const char* buffer, size_t size, bool dos, bool trailing) {
    if (size == (size_t) -1) {
        throw std::string("error: gnu::file::Buf::InsertCR(): size out of range");
    }
    else if (buffer == nullptr || size == 0 || (trailing == false && dos == false)) {
        return Buf();
    }

    auto res_size = size;

    if (dos == true) { // Count lines.
        for (size_t f = 0; f < size; f++) {
            res_size += (buffer[f] == '\n');
        }
    }

    auto res     = file::allocate(nullptr, res_size + 1);
    auto restart = std::string::npos;
    auto res_pos = (size_t) 0;
    auto p       = (unsigned char) 0;

    for (size_t f = 0; f < size; f++) {
        auto c = (unsigned char) buffer[f];

        if (trailing == true) {
            if (c == '\n') {
                if (restart != std::string::npos) {
                    res_pos = restart;
                }

                restart = std::string::npos;
            }
            else if (restart == std::string::npos && (c == ' ' || c == '\t')) {
                restart = res_pos;
            }
            else if (c != ' ' && c != '\t') {
                restart = std::string::npos;
            }
        }

        if (dos == true && c == '\n' && p != '\r') {
            res[res_pos++] = '\r';
        }

        res[res_pos++] = c;
        p = c;
    }

    res[res_pos] = 0;

    if (restart != std::string::npos) {
        res[restart] = 0;
        res_pos = restart;
    }

    return Buf::Grab(res, res_pos);
}

/**
* @brief Remove "\r" from text.
*
* @param[in] buffer  Input buffer.
* @param[in] size    Buffer size.
*
* @return Converted text.
*
* @throws std::string exception on error.
*/
Buf Buf::RemoveCR(const char* buffer, size_t size) {
    auto res = Buf(size);

    for (size_t f = 0, e = 0; f < size; f++) {
        auto c = buffer[f];

        if (c != 13) {
            res._str[e++] = c;
        }
        else {
            res._size--;
        }
    }

    return res;
}

/**
* @brief Set new data.
*
* Current memory will be deleted.
*
* @param[in] buffer  Copy this buffer.
* @param[in] size    Buffer size.
*
* @return Reference to this object.
*
* @throws std::string exception on error.
*/
Buf& Buf::set(const char* buffer, size_t size) {
    if (size == (size_t) -1) {
        throw std::string("error: gnu::file::Buf:set(): size out of range");
    }

    if (_str == buffer) {
    }
    else if (buffer == nullptr) {
        free(_str);
        _str = nullptr;
        _size = 0;
    }
    else {
        free(_str);
        _str = file::allocate(nullptr, size + 1);
        _size = size;
        std::memcpy(_str, buffer, size);
    }

    return *this;
}

/**
* @brief Write buffer to file.
*
* @param[in] path   Path to write to.
* @param[in] flush  True to flush file (default true).
*
* @return True if ok.
*/
bool Buf::write(const std::string& path, bool flush) const {
    return file::write(path, _str, _size, flush);
}

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

/**
* @brief Get file info for input path.
*
* @param[in] path      File path.
* @param[in] realpath  True to use the real path if it is an link.
*/
File::File(const std::string& path, bool realpath) {
    _ctime = -1;
    _link  = false;
    _mode  = -1;
    _mtime = -1;
    _size  = -1;
    _type  = TYPE::MISSING;

    if (path != "") {
        _filename = file::_to_absolute_path(path, realpath);
        file::_split_paths(_filename, _path, _name, _ext);
    }
    else {
        return;
    }

#ifdef _WIN32
    auto wpath = file::_to_wide(_filename.c_str());

    WIN32_FILE_ATTRIBUTE_DATA attr;

    if (GetFileAttributesExW(wpath, GetFileExInfoStandard, &attr) != 0) {
        if (attr.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            _type = TYPE::DIR;
            _size = 0;
        }
        else {
            _type = TYPE::FILE;
            _size = (attr.nFileSizeHigh * 4294967296) + attr.nFileSizeLow;
        }

        _mtime = file::_time(&attr.ftLastWriteTime);
        _ctime = file::_time(&attr.ftCreationTime);

        if (attr.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) {
            _link = true;

            HANDLE handle = CreateFileW(wpath, 0, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_BACKUP_SEMANTICS, NULL);

            if (handle != INVALID_HANDLE_VALUE) {
                _size = GetFileSize(handle, NULL);

                FILETIME ftCreationTime;
                FILETIME ftLastAccessTime;
                FILETIME ftLastWriteTime;

                if (GetFileTime(handle, &ftCreationTime, &ftLastAccessTime, &ftLastWriteTime) != 0) {
                    _mtime = file::_time(&ftLastWriteTime);
                    _ctime = file::_time(&ftCreationTime);
                }

                CloseHandle(handle);
            }
        }
    }

    free(wpath);
#else
    struct stat st;
    char        tmp[PATH_MAX + 1];

    if (::stat(_filename.c_str(), &st) == 0) {
        _size  = st.st_size;
        _ctime = st.st_ctime;
        _mtime = st.st_mtime;

        if (S_ISDIR(st.st_mode)) {
            _type = TYPE::DIR;
        }
        else if (S_ISREG(st.st_mode)) {
            _type = TYPE::FILE;
        }
        else {
            _type = TYPE::OTHER;
        }

        snprintf(tmp, PATH_MAX, "%o", st.st_mode);
        auto l = strlen(tmp);

        if (l > 2) {
            _mode = strtol(tmp + (l - 3), nullptr, 8);
        }

        if (lstat(_filename.c_str(), &st) == 0 && S_ISLNK(st.st_mode)) {
            _link = true;
        }
    }
    else {
        auto tmp_size = readlink(_filename.c_str(), tmp, PATH_MAX);

        if (tmp_size > 0 && tmp_size < PATH_MAX) {
            _link = true;
        }
    }
#endif

    if (_type == TYPE::DIR) {
        _ext = "";
    }
}

/**
* @brief Read link name (not for windows).
*
* @return Link name, if not an link it will return a empty string.
*/
std::string File::linkname() const {
#ifdef _WIN32
    return "";
#else
    char tmp[PATH_MAX + 1];
    auto tmp_size = readlink(_filename.c_str(), tmp, PATH_MAX);

    if (tmp_size > 0 && tmp_size < PATH_MAX) {
        tmp[tmp_size] = 0;
        return _path + "/" + tmp;
    }

    return "";
#endif
}

/**
* @brief If file is not and directory return name without the extension.
*
* @return Name without extension.
*/
std::string File::name_without_ext() const {
    if (_type != TYPE::DIR) {
        auto dot = _name.find_last_of(".");
        return (dot == std::string::npos) ? _name : _name.substr(0, dot);
    }
    else {
        return _name;
    }
}

/**
* @brief Return a file info string.
*
* @param[in] short_version  True to make result string shorter.
*
* @return String with name and size.
*/
std::string File::to_string(bool short_version) const {
    char tmp[PATH_MAX + 100];
    int n = 0;

    if (short_version == true) {
        n = snprintf(tmp, PATH_MAX + 100, "File(filename=%s, type=%s, %ssize=%lld, mtime=%lld)",
            _filename.c_str(),
            type_name().c_str(),
            _link ? "LINK, " : "",
            (long long int) _size,
            (long long int) _mtime);
    }
    else {
        n = snprintf(tmp, PATH_MAX + 100, "File(filename=%s, name=%s, ext=%s, path=%s, type=%s, link=%s, size=%lld, mtime=%lld, mode=%o)",
            _filename.c_str(),
            _name.c_str(),
            _ext.c_str(),
            _path.c_str(),
            type_name().c_str(),
            _link ? "YES" : "NO",
            (long long int) _size,
            (long long int) _mtime,
            _mode > 0 ? _mode : 0);
    }

    return (n > 0 && n < PATH_MAX + 100) ? tmp : "";
}

/**
* @brief Return type name in english.
*
* @return "Missing", Directory", "File" or "Other".
*/
std::string File::type_name() const {
    static const char* NAMES[] = { "Missing", "Directory", "File", "Other", "", };
    return NAMES[static_cast<size_t>(_type)];
}

} // file
} // gnu

// MKALGAM_OFF
