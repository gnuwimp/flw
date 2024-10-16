// Copyright gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#include "file.h"
#include "bug.h"

// MKALGAM_ON

#include <algorithm>
#include <assert.h>
#include <ctime>
#include <dirent.h>
#include <unistd.h>

#ifdef _WIN32
    #include <shlobj.h>
    #include <time.h>
#else
    #include <sys/stat.h>
    #include <utime.h>
#endif

#ifndef __APPLE__
    #include <filesystem>
#endif

#ifndef PATH_MAX
    #define PATH_MAX 1050
#endif

namespace gnu {

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

static std::string _FILE_STDOUT_NAME = "";
static std::string _FILE_STDERR_NAME = "";

#ifdef _WIN32

//------------------------------------------------------------------------------
static char* _file_from_wide(const wchar_t* in) {
    auto out_len = WideCharToMultiByte(CP_UTF8, 0, in, -1, nullptr, 0, nullptr, nullptr);
    auto out     = File::Allocate(nullptr, out_len + 1);

    WideCharToMultiByte(CP_UTF8, 0, in, -1, (LPSTR) out, out_len, nullptr, nullptr);
    return (char*) out;
}

//------------------------------------------------------------------------------
static wchar_t* _file_to_wide(const char* in) {
    auto out_len = MultiByteToWideChar(CP_UTF8, 0, in , -1, nullptr , 0);
    auto out     = reinterpret_cast<wchar_t*>(File::Allocate(nullptr, out_len * sizeof(wchar_t) + sizeof(wchar_t)));

    MultiByteToWideChar(CP_UTF8, 0, in , -1, out, out_len);
    return out;
}

//------------------------------------------------------------------------------
static int64_t _file_time(FILETIME* ft) {
    int64_t res = (int64_t) ft->dwHighDateTime << 32 | (int64_t) ft->dwLowDateTime;
    res = res / 10000000;
    res = res - 11644473600;
    return res;
}

#endif

//------------------------------------------------------------------------------
static FileBuf _file_close_redirect(int type) {
    std::string fname;
    FILE* fhandle;

    if (type == 2) {
        if (_FILE_STDERR_NAME == "") return FileBuf();
        fname = _FILE_STDERR_NAME;
        fhandle = stderr;
        _FILE_STDERR_NAME = "";
    }
    else {
        if (_FILE_STDOUT_NAME == "") return FileBuf();
        fname = _FILE_STDOUT_NAME;
        fhandle = stdout;
        _FILE_STDOUT_NAME = "";
    }

#ifdef _WIN32
    fflush(fhandle);
    freopen("CON", "w", fhandle);
#else
    fflush(fhandle);
    auto r = freopen("/dev/tty", "w", fhandle);
    (void) r;
#endif

    auto res = File::Read(fname);
    File::Remove(fname);
    return res;
}

//------------------------------------------------------------------------------
static bool _file_open_redirect(int type) {
    bool res = false;
    std::string fname;
    FILE* fhandle = nullptr;

    if (type == 2) {
        if (_FILE_STDERR_NAME != "") return res;
        fname = _FILE_STDERR_NAME = File::TmpFile("stderr_").filename;
        fhandle = stderr;
    }
    else {
        if (_FILE_STDOUT_NAME != "") return res;
        fname = _FILE_STDOUT_NAME = File::TmpFile("stdout_").filename;
        fhandle = stdout;
    }

#ifdef _WIN32
        auto wpath = _file_to_wide(fname.c_str());
        auto wmode = _file_to_wide("wb");

        res = _wfreopen(wpath, wmode, fhandle) != nullptr;
        free(wpath);
        free(wmode);
#else
        res = freopen(fname.c_str(), "wb", fhandle) != nullptr;
#endif

    if (res == false && type == 1) _FILE_STDOUT_NAME = "";
    else if (res == false && type == 2) _FILE_STDERR_NAME = "";
    return res;
}

//------------------------------------------------------------------------------
static int _file_rand() {
    static bool INIT = false;
    if (INIT == false) srand(time(nullptr));
    INIT = true;
    return rand() % 10'000;
}

//------------------------------------------------------------------------------
static std::string& _file_replace_all(std::string& string, const std::string& find, const std::string& replace) {
    if (find == "") {
        return string;
    }
    else {
        size_t start = 0;

        while ((start = string.find(find, start)) != std::string::npos) {
            string.replace(start, find.length(), replace);
            start += replace.length();
        }

        return string;
    }
}

//------------------------------------------------------------------------------
static std::string _file_substr(const std::string& in, std::string::size_type pos, std::string::size_type size = std::string::npos) {
    try { return in.substr(pos, size); }
    catch(...) { return ""; }
}

//------------------------------------------------------------------------------
static void _file_sync(FILE* file) {
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

//------------------------------------------------------------------------------
static const std::string _file_to_absolute_path(const std::string& in, bool realpath) {
    std::string res;
    auto name = in;

    if (name == "") {
        return "";
    }

#ifdef _WIN32
    if (name.find("\\\\") == 0) {
        res = name;
        return name;
    }
    else if (name.size() < 2 || name[1] != ':') {
        auto work = File(File::WorkDir());
        res = work.filename;
        res += "\\";
        res += name;
    }
    else {
        res = name;
    }

    _file_replace_all(res, "\\", "/");

    auto len = res.length();
    _file_replace_all(res, "//", "/");

    while (len > res.length()) {
        len = res.length();
        _file_replace_all(res, "//", "/");
    }

    while (res.size() > 3 && res.back() == '/') {
        res.pop_back();
    }
#else
    if (name[0] != '/') {
        auto work = File(File::WorkDir());
        res = work.filename;
        res += "/";
        res += name;
    }
    else {
        res = name;
    }

    auto len = res.length();
    _file_replace_all(res, "//", "/");

    while (len > res.length()) {
        len = res.length();
        _file_replace_all(res, "//", "/");
    }

    while (res.size() > 1 && res.back() == '/') {
        res.pop_back();
    }
#endif

    return (realpath == true) ? File::CanonicalName(res) : res;
}

//------------------------------------------------------------------------------
static void _file_split_paths(std::string filename, std::string& path, std::string& name, std::string& ext) {
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
            path = _file_substr(filename, 0, pos1);
        }

        name = _file_substr(filename, pos1 + 1);
    }

    auto pos2 = name.find_last_of('.');

    if (pos2 != std::string::npos && pos2 != 0) {
        ext = _file_substr(name, pos2 + 1);
    }

    if (path.back() == ':') {
        path += sep;
        return;
    }
#else
    auto pos1 = filename.find_last_of('/');

    if (pos1 != std::string::npos) {
        if (pos1 > 0) {
            path = _file_substr(filename, 0, pos1);
        }
        else if (filename != "/") {
            path = "/";
        }

        if (filename != "/") {
            name = _file_substr(filename, pos1 + 1);
        }
    }

    auto pos2 = filename.find_last_of('.');

    if (pos2 != std::string::npos && pos2 > pos1 + 1) {
        ext = _file_substr(filename, pos2 + 1);
    }
#endif
}

//------------------------------------------------------------------------------
static void _file_read(std::string path, FileBuf& buf) {
    File file(path);

    if (file.is_file() == false || (long long unsigned int) file.size > SIZE_MAX) {
        return;
    }

    auto out = File::Allocate(nullptr, file.size + 1);

    if (file.size == 0) {
        buf.p = out;
        return;
    }

    auto handle = File::Open(file.filename, "rb");

    if (handle == nullptr) {
        free(out);
        return;
    }
    else if (fread(out, 1, file.size, handle) != (size_t) file.size) {
        fclose(handle);
        free(out);
    }
    else {
        fclose(handle);
        buf.p = out;
        buf.s = file.size;
    }
}

//------------------------------------------------------------------------------
static void _file_read_dir_rec(FileVector& res, FileVector& files) {
    for (auto& file : files) {
        res.push_back(file);

        if (file.type == File::TYPE::DIR && file.link == false && file.is_circular() == false) {
            auto v = File::ReadDir(file.filename);
            _file_read_dir_rec(res, v);
        }
    }
}

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
FileBuf::FileBuf(size_t S) {
    p = File::Allocate(nullptr, S + 1);
    s = S;
}

//------------------------------------------------------------------------------
FileBuf::FileBuf(const char* P, size_t S) {
    if (P == nullptr) {
        p = nullptr;
        s = 0;
        return;
    }

    p = File::Allocate(nullptr, S + 1);
    s = S;
    std::memcpy(p, P, S);
}

//------------------------------------------------------------------------------
FileBuf::FileBuf(const FileBuf& b) {
    if (b.p == nullptr) {
        p = nullptr;
        s = 0;
        return;
    }

    p = File::Allocate(nullptr, b.s + 1);
    s = b.s;
    std::memcpy(p, b.p, b.s);
}

//------------------------------------------------------------------------------
bool FileBuf::operator==(const FileBuf& other) const {
    return p != nullptr && s == other.s && std::memcmp(p, other.p, s) == 0;
}

//------------------------------------------------------------------------------
FileBuf& FileBuf::add(const char* P, size_t S) {
    if (p == P || P == nullptr) {
    }
    else if (p == nullptr) {
        p = File::Allocate(nullptr, S + 1);
        std::memcpy(p, P, S);
        s = S;
    }
    else if (S > 0) {
        auto t = File::Allocate(nullptr, s + S + 1);
        std::memcpy(t, p, s);
        std::memcpy(t + s, P, S);
        free(p);
        p = t;
        s += S;
    }

    return *this;
}

//------------------------------------------------------------------------------
void FileBuf::Count(const char* P, size_t S, size_t count[257]) {
    assert(P);

    auto max_line     = 0;
    auto current_line = 0;

    std::memset(count, 0, sizeof(size_t) * 257);

    for (size_t f = 0; f < S; f++) {
        auto c = (unsigned char) P[f];

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
}

//------------------------------------------------------------------------------
uint64_t FileBuf::Fletcher64(const char* P, size_t S) {
    if (P == nullptr || S == 0) {
        return 0;
    }

    auto u8data = reinterpret_cast<const uint8_t*>(P);
    auto dwords = (uint64_t) S / 4;
    auto sum1   = (uint64_t) 0;
    auto sum2   = (uint64_t) 0;
    auto data32 = reinterpret_cast<const uint32_t*>(u8data);
    auto left   = (uint64_t) 0;

    for (size_t f = 0; f < dwords; ++f) {
        sum1 = (sum1 + data32[f]) % UINT32_MAX;
        sum2 = (sum2 + sum1) % UINT32_MAX;
    }

    left = S - dwords * 4;

    if (left > 0) {
        auto tmp  = (uint32_t) 0;
        auto byte = reinterpret_cast<uint8_t*>(&tmp);

        for (auto f = (uint64_t) 0; f < left; ++f) {
            byte[f] = u8data[dwords * 4 + f];
        }

        sum1 = (sum1 + tmp) % UINT32_MAX;
        sum2 = (sum2 + sum1) % UINT32_MAX;
    }

    return (sum2 << 32) | sum1;
}

//------------------------------------------------------------------------------
// Remove (optional) trailing space and insert (optional) \r for every \n.
// Returns FileBuf with NULL p if nothing has been changed or invalid arguments.
//
FileBuf FileBuf::InsertCR(const char* P, size_t S, bool dos, bool trailing) {
    if (P == nullptr || S == 0 || (trailing == false && dos == false)) {
        return FileBuf();
    }

    auto res_size = S;

    if (dos == true) { // Count lines.
        for (size_t f = 0; f < S; f++) {
            res_size += (P[f] == '\n');
        }
    }

    auto res     = File::Allocate(nullptr, res_size + 1);
    auto restart = std::string::npos;
    auto res_pos = (size_t) 0;
    auto p       = (unsigned char) 0;

    for (size_t f = 0; f < S; f++) {
        auto c = (unsigned char) P[f];

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

    return FileBuf::Grab(res, res_pos);
}

//------------------------------------------------------------------------------
FileBuf FileBuf::RemoveCR(const char* P, size_t S) {
    auto res = FileBuf(S);

    for (size_t f = 0, e = 0; f < S; f++) {
        auto c = P[f];

        if (c != 13) {
            res.p[e++] = c;
        }
        else {
            res.s--;
        }
    }

    return res;
}

//------------------------------------------------------------------------------
FileBuf& FileBuf::set(const char* P, size_t S) {
    if (p == P) {
    }
    else if (P == nullptr) {
        free(p);
        p = nullptr;
        s = 0;
    }
    else {
        free(p);
        p = File::Allocate(nullptr, S + 1);
        s = S;
        std::memcpy(p, P, S);
    }

    return *this;
}

//------------------------------------------------------------------------------
bool FileBuf::write(std::string filename) const {
    return File::Write(filename, p, s);
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

//------------------------------------------------------------------------------
char* File::Allocate(char* resize_or_null, size_t size, bool exception) {
    void* res = nullptr;

    if (resize_or_null == nullptr) {
        res = calloc(size, 1);
    }
    else {
        res = realloc(resize_or_null, size);
    }

    if (res == nullptr && exception == true) {
        throw "error: memory allocation failed in File::Allocate()";
    }

    return (char*) res;
}

//------------------------------------------------------------------------------
std::string File::CanonicalName(std::string filename) {
#if defined(_WIN32)
    wchar_t wres[PATH_MAX];
    auto    wpath = _file_to_wide(filename.c_str());
    auto    len   = GetFullPathNameW(wpath, PATH_MAX, wres, nullptr);

    if (len > 0 && len < PATH_MAX) {
        auto cpath = _file_from_wide(wres);
        auto res   = std::string(cpath);

        free(cpath);
        free(wpath);

        _file_replace_all(res, "\\", "/");
        return res;
    }
    else {
        free(wpath);
        return filename;
    }
#elif defined(__linux__)
    auto path = canonicalize_file_name(filename.c_str());
    auto res  = (path != nullptr) ? std::string(path) : filename;

    free(path);
    return res;
#else
    auto path = realpath(filename.c_str(), nullptr);
    auto res  = (path != nullptr) ? std::string(path) : filename;

    free(path);
    return res;
#endif
    return "";
}

//------------------------------------------------------------------------------
// Change directory for an file or directory
//
bool File::ChDir(std::string path) {
#ifdef _WIN32
    auto wpath = _file_to_wide(path.c_str());
    auto res   = _wchdir(wpath);

    free(wpath);
    return res == 0;
#else
    return ::chdir(path.c_str()) == 0;
#endif
}

//------------------------------------------------------------------------------
std::string File::CheckFilename(std::string filename) {
    static const std::string ILLEGAL = "<>:\"/\\|?*\n\t\r";
    std::string res;

    for (auto& c : filename) {
        if (ILLEGAL.find(c) == std::string::npos) {
            res += c;
        }
    }

    return res;
}

//------------------------------------------------------------------------------
bool File::ChMod(std::string path, int mode) {
    auto res = false;

    if (mode < 0) {
        return false;
    }

#ifdef _WIN32
    auto wpath = _file_to_wide(path.c_str());

    res = SetFileAttributesW(wpath, mode);
    free(wpath);
#else
    res = ::chmod(path.c_str(), mode) == 0;
#endif

    return res;
}

//------------------------------------------------------------------------------
FileBuf File::CloseStderr() {
    return _file_close_redirect(2);
}

//------------------------------------------------------------------------------
FileBuf File::CloseStdout() {
    return _file_close_redirect(1);
}

//------------------------------------------------------------------------------
bool File::Copy(std::string from, std::string to, CallbackCopy callback, void* data) {
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

    auto buf   = File::Allocate(nullptr, BUF_SIZE);
    auto read  = File::Open(from, "rb");
    auto write = File::Open(to, "wb");
    auto count = (int64_t) 0;
    auto size  = (size_t) 0;

    if (read == nullptr || write == nullptr) {
        if (read != nullptr) {
            fclose(read);
        }

        if (write != nullptr) {
            fclose(write);
            File::Remove(to);
        }

        free(buf);
        return false;
    }

    while ((size = fread(buf, 1, BUF_SIZE, read)) > 0) {
        if (fwrite(buf, 1, size, write) != size) {
            break;
        }

        count += size;

        if (callback != nullptr && callback(file1.size, count, data) == false && count != file1.size) {
            break;
        }
    }

    fclose(read);
    _file_sync(write);
    fclose(write);
    free(buf);

    if (count != file1.size) {
        File::Remove(to);
        return false;
    }

    File::ModTime(to, file1.mtime);
    File::ChMod(to, file1.mode);
    return true;
}

//------------------------------------------------------------------------------
File File::HomeDir() {
    std::string res;

#ifdef _WIN32
    wchar_t wpath[PATH_MAX];

    if (SHGetFolderPathW(nullptr, CSIDL_PROFILE, nullptr, 0, wpath) == S_OK) {
        auto path = _file_from_wide(wpath);
        res = path;
        free(path);
    }
#else
    const char* tmp = getenv("HOME");
    res = tmp ? tmp : "";
#endif

    return File(res);
}

//------------------------------------------------------------------------------
bool File::is_circular() const {
    if (type == TYPE::DIR && link == true) {
        auto l = canonicalname() + "/";
        return filename.find(l) == 0;
    }

    return false;
}

//------------------------------------------------------------------------------
std::string File::linkname() const {
#ifdef _WIN32
    return "";
#else
    char tmp[PATH_MAX + 1];
    auto tmp_size = readlink(filename.c_str(), tmp, PATH_MAX);

    if (tmp_size > 0 && tmp_size < PATH_MAX) {
        tmp[tmp_size] = 0;
        return path + "/" + tmp;
    }

    return "";
#endif
}

//------------------------------------------------------------------------------
bool File::MkDir(std::string path) {
    bool res = false;

#ifdef _WIN32
    auto wpath = _file_to_wide(path.c_str());

    res = _wmkdir(wpath) == 0;
    free(wpath);
#else
    res = ::mkdir(path.c_str(), File::DEFAULT_DIR_MODE) == 0;
#endif

    return res;
}

//------------------------------------------------------------------------------
bool File::ModTime(std::string path, int64_t time) {
    auto res = false;

#ifdef _WIN32
    auto wpath  = _file_to_wide(path.c_str());
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

//------------------------------------------------------------------------------
std::string File::name_without_ext() const {
    auto dot = name.find_last_of(".");
    return (dot == std::string::npos) ? name : name.substr(0, dot);
}

//------------------------------------------------------------------------------
FILE* File::Open(std::string path, std::string mode) {
    FILE* res = nullptr;

#ifdef _WIN32
    auto wpath = _file_to_wide(path.c_str());
    auto wmode = _file_to_wide(mode.c_str());

    res = _wfopen(wpath, wmode);
    free(wpath);
    free(wmode);
#else
    res = fopen(path.c_str(), mode.c_str());
#endif

    return res;
}

//------------------------------------------------------------------------------
std::string File::OS() {
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

//------------------------------------------------------------------------------
FILE* File::Popen(std::string cmd, bool write) {
    FILE* file = nullptr;

#ifdef _WIN32
    auto wpath = _file_to_wide(cmd.c_str());
    auto wmode = _file_to_wide(write ? "wb" : "rb");

    file = _wpopen(wpath, wmode);
    free(wpath);
    free(wmode);
#else
    ::fflush(nullptr);
    file = ::popen(cmd.c_str(), (write == true) ? "w" : "r");
#endif

    return file;
}

//------------------------------------------------------------------------------
FileBuf File::Read(std::string path) {
    FileBuf buf;
    _file_read(path, buf);
    return buf;
}

//------------------------------------------------------------------------------
FileBuf* File::Read2(std::string path) {
    auto buf = new FileBuf();
    _file_read(path, *buf);
    return buf;
}

//------------------------------------------------------------------------------
FileVector File::ReadDir(std::string path) {
    auto file = File(path, true);
    auto res  = FileVector();

    if (file.type != TYPE::DIR) {
        return res;
    }

#ifdef _WIN32
    auto wpath = _file_to_wide(file.filename.c_str());
    auto dirp  = _wopendir(wpath);
    auto sep   = '/';

    if (file.filename.find("\\\\") == 0) {
        sep = '\\';
    }

    if (dirp != nullptr) {
        auto entry = _wreaddir(dirp);

        while (entry != nullptr) {
            auto cpath = _file_from_wide(entry->d_name);

            if (strcmp(cpath, ".") != 0 && strcmp(cpath, "..") != 0) {
                auto name = (file.name == ".") ? file.path + sep + cpath : file.filename + sep + cpath;
                res.push_back(File(name));
            }

            free(cpath);
            entry = _wreaddir(dirp);
        }

        _wclosedir(dirp);
    }

    free(wpath);
#else
    auto dirp = ::opendir(file.filename.c_str());

    if (dirp != nullptr) {
        auto entry = ::readdir(dirp);

        while (entry != nullptr) {
            if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
                auto name = (file.name == ".") ? file.path + "/" + entry->d_name : file.filename + "/" + entry->d_name;
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

//------------------------------------------------------------------------------
FileVector File::ReadDirRec(std::string path) {
    auto res   = FileVector();
    auto files = File::ReadDir(path);

    _file_read_dir_rec(res, files);
    return res;
}

//------------------------------------------------------------------------------
bool File::RedirectStderr() {
    return _file_open_redirect(2);
}

//------------------------------------------------------------------------------
bool File::RedirectStdout() {
    return _file_open_redirect(1);
}

//------------------------------------------------------------------------------
bool File::Remove(std::string path) {
    auto f = File(path);

    if (f.type == TYPE::MISSING && f.link == false) {
        return false;
    }

    auto res = false;

#ifdef _WIN32
    auto wpath = _file_to_wide(path.c_str());

    if (f.type == TYPE::DIR) {
        res = RemoveDirectoryW(wpath);
    }
    else {
        res = DeleteFileW(wpath);
    }

    if (res == false) {
        if (f.type == TYPE::DIR) {
            File::ChMod(path, File::DEFAULT_DIR_MODE);
            res = RemoveDirectoryW(wpath);
        }
        else {
            File::ChMod(path, File::DEFAULT_FILE_MODE);
            res = DeleteFileW(wpath);
        }
    }

    free(wpath);
#else
    if (f.type == TYPE::DIR && f.link == false) {
        res = ::rmdir(path.c_str()) == 0;
    }
    else {
        res = ::unlink(path.c_str()) == 0;
    }
#endif

    return res;
}

//------------------------------------------------------------------------------
bool File::RemoveRec(std::string path) {
    auto file = File(path, true);

    if (file == File::HomeDir() || file.path == "") {
        return false;
    }

    auto files = File::ReadDirRec(path);

    std::reverse(files.begin(), files.end());

    for (const auto& file : files) {
        File::Remove(file.filename);
    }

    return File::Remove(path);
}

//------------------------------------------------------------------------------
bool File::Rename(std::string from, std::string to) {
    auto res    = false;
    auto from_f = File(from);
    auto to_f   = File(to);

    if (from_f == to_f) {
        return false;
    }

#ifdef _WIN32
    auto wfrom = _file_to_wide(from_f.filename.c_str());
    auto wto   = _file_to_wide(to_f.filename.c_str());

    if (to_f.type == TYPE::DIR) {
        File::RemoveRec(to_f.filename);
        res = MoveFileExW(wfrom, wto, MOVEFILE_COPY_ALLOWED | MOVEFILE_WRITE_THROUGH);
    }
    else if (to_f.type == TYPE::MISSING) {
        res = MoveFileExW(wfrom, wto, MOVEFILE_COPY_ALLOWED | MOVEFILE_WRITE_THROUGH);
    }
    else {
        res = ReplaceFileW(wto, wfrom, nullptr, REPLACEFILE_WRITE_THROUGH, 0, 0);
    }

    free(wfrom);
    free(wto);
#else
    if (to_f.type == TYPE::DIR) {
        File::RemoveRec(to_f.filename);
    }

    res = ::rename(from_f.filename.c_str(), to_f.filename.c_str()) == 0;
#endif

    return res;
}

//------------------------------------------------------------------------------
int File::Run(std::string cmd, bool background, bool hide_win32_window) {
#ifdef _WIN32
    wchar_t*            cmd_w = _file_to_wide(cmd.c_str());
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

//------------------------------------------------------------------------------
File File::TmpDir() {
    std::string res;

    try {
#if defined(_WIN32)
        auto path = std::filesystem::temp_directory_path();
        auto utf  = _file_from_wide(path.c_str());
        res = utf;
        free(utf);
#elif defined(__APPLE__)
        res = "/tmp";
#else
        auto path = std::filesystem::temp_directory_path();
        res = path.c_str();
#endif
    }
    catch(...) {
        return File::WorkDir();
    }

    return File(res);
}

//------------------------------------------------------------------------------
File File::TmpFile(std::string prepend) {
    assert(prepend.length() < 50);

    char buf[100];
    snprintf(buf, 100, "%s%04d%04d%04d", prepend.c_str(), _file_rand(), _file_rand(), _file_rand());
    return File(TmpDir().filename + "/" + buf);
}

//------------------------------------------------------------------------------
std::string File::to_string(bool short_version) const {
    char tmp[PATH_MAX + 100];
    int n = 0;

    if (short_version == true) {
        n = snprintf(tmp, PATH_MAX + 100, "File(filename=%s, type=%s, %ssize=%lld, mtime=%lld)",
            filename.c_str(),
            type_name().c_str(),
            link ? "LINK, " : "",
            (long long int) size,
            (long long int) mtime);
    }
    else {
        n = snprintf(tmp, PATH_MAX + 100, "File(filename=%s, name=%s, ext=%s, path=%s, type=%s, link=%s, size=%lld, mtime=%lld, mode=%o)",
            filename.c_str(),
            name.c_str(),
            ext.c_str(),
            path.c_str(),
            type_name().c_str(),
            link ? "YES" : "NO",
            (long long int) size,
            (long long int) mtime,
            mode > 0 ? mode : 0);
    }

    return (n > 0 && n < PATH_MAX + 100) ? tmp : "";
}

//------------------------------------------------------------------------------
std::string File::type_name() const {
    static const char* NAMES[] = { "Missing", "Directory", "File", "Other", "", };
    return NAMES[static_cast<size_t>(type)];
}

//------------------------------------------------------------------------------
File& File::update() {
    ctime = -1;
    link  = false;
    mode  = -1;
    mtime = -1;
    size  = -1;
    type  = TYPE::MISSING;

    if (filename == "") {
        return *this;
    }

#ifdef _WIN32
    auto wpath = _file_to_wide(filename.c_str());

    WIN32_FILE_ATTRIBUTE_DATA attr;

    if (GetFileAttributesExW(wpath, GetFileExInfoStandard, &attr) != 0) {
        if (attr.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            type = TYPE::DIR;
            size = 0;
        }
        else {
            type = TYPE::FILE;
            size = (attr.nFileSizeHigh * 4294967296) + attr.nFileSizeLow;
        }

        mtime = _file_time(&attr.ftLastWriteTime);
        ctime = _file_time(&attr.ftCreationTime);

        if (attr.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) {
            link = true;

            HANDLE handle = CreateFileW(wpath, 0, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_BACKUP_SEMANTICS, NULL);

            if (handle != INVALID_HANDLE_VALUE) {
                size = GetFileSize(handle, NULL);

                FILETIME ftCreationTime;
                FILETIME ftLastAccessTime;
                FILETIME ftLastWriteTime;

                if (GetFileTime(handle, &ftCreationTime, &ftLastAccessTime, &ftLastWriteTime) != 0) {
                    mtime = _file_time(&ftLastWriteTime);
                    ctime = _file_time(&ftCreationTime);
                }

                CloseHandle(handle);
            }
        }
    }

    free(wpath);
#else
    struct stat st;
    char        tmp[PATH_MAX + 1];

    if (::stat(filename.c_str(), &st) == 0) {
        size  = st.st_size;
        ctime = st.st_ctime;
        mtime = st.st_mtime;

        if (S_ISDIR(st.st_mode)) {
            type = TYPE::DIR;
        }
        else if (S_ISREG(st.st_mode)) {
            type = TYPE::FILE;
        }
        else {
            type = TYPE::OTHER;
        }


        snprintf(tmp, PATH_MAX, "%o", st.st_mode);
        auto l = strlen(tmp);

        if (l > 2) {
            mode = strtol(tmp + (l - 3), nullptr, 8);
        }

        if (lstat(filename.c_str(), &st) == 0 && S_ISLNK(st.st_mode)) {
            link = true;
        }
    }
    else {
        auto tmp_size = readlink(filename.c_str(), tmp, PATH_MAX);

        if (tmp_size > 0 && tmp_size < PATH_MAX) {
            link = true;
        }
    }
#endif

    return *this;
}

//------------------------------------------------------------------------------
File& File::update(std::string in, bool realpath) {
    filename = (in != "") ? _file_to_absolute_path(in, realpath) : "";
    _file_split_paths(filename, path, name, ext);
    update();
    return *this;
}

//------------------------------------------------------------------------------
File File::WorkDir() {
    std::string res;

#ifdef _WIN32
    auto wpath = _wgetcwd(nullptr, 0);

    if (wpath != nullptr) {
        auto path = _file_from_wide(wpath);
        free(wpath);
        res = path;
        free(path);
    }
#else
    auto path = getcwd(nullptr, 0);

    if (path != nullptr) {
        res = path;
        free(path);
    }
#endif

    return File(res);
}

//------------------------------------------------------------------------------
bool File::Write(std::string filename, const char* in, size_t in_size) {
    if (File(filename).type == TYPE::DIR) {
        return false;
    }

    auto tmpfile = filename + ".~tmp";
    auto file    = File::Open(tmpfile, "wb");

    if (file == nullptr) {
        return false;
    }

    auto wrote = fwrite(in, 1, in_size, file);
    _file_sync(file);
    fclose(file);

    if (wrote != in_size) {
        File::Remove(tmpfile);
        return false;
    }
    else if (File::Rename(tmpfile, filename) == false) {
        File::Remove(tmpfile);
        return false;
    }

    return true;
}

} // flw

// MKALGAM_OFF
