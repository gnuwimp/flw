// Copyright gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#include "flw.h"

int main() {
    // [gnu::file example]

    gnu::file::File file = gnu::file::File("file.cpp");

    printf("Filename: %s\n", file.filename().c_str());
    printf("Path:     %s\n", file.path().c_str());
    printf("Name:     %s\n", file.name().c_str());
    printf("Ext:      %s\n", file.ext().c_str());
    printf("Size:     %u\n", (unsigned) file.size());
    printf("Mod Time: %u\n", (unsigned) file.mtime());

/*
Filename: /home/.../flw/examples/file.cpp
Path:     /home/.../flw/examples
Name:     file.cpp
Ext:      cpp
Size:     1127
Mod Time: 1745841673
*/

    gnu::file::File  work  = gnu::file::work_dir();
    gnu::file::Files files = gnu::file::read_dir(work.path() + "/src");
    gnu::file::Buf   all   = gnu::file::Buf();

    for (auto& file : files) {
        if (file.is_file() == true) {
            gnu::file::Buf buf = gnu::file::read(file.filename());
            uint64_t       sum = 0;

            for (size_t f = 0; f < buf.size(); f++) {
                sum += buf[f];
            }

            all += buf;

            printf("size: %6u, sum: %8llu, avg: %2.2f, name: %s\n", (unsigned) file.size(), (long long unsigned) sum, (double) sum / (double) file.size(), file.name().c_str());
        }

    }

    printf("total: %llu\n", (long long unsigned) all.size());

    gnu::file::write("all.cpp", all);
    assert((size_t) gnu::file::File("all.cpp").size() == all.size());

    gnu::file::remove("all.cpp");
    assert(gnu::file::File("all.cpp").size() == -1);

/*
size: 117468, sum:  7932968, avg: 67.53, name: chart.cpp
size:  22682, sum:  1429441, avg: 63.02, name: chart.h
size:  24032, sum:  1497512, avg: 62.31, name: date.cpp
size:   7064, sum:   411843, avg: 58.30, name: date.h
size:  16202, sum:  1015692, avg: 62.69, name: datechooser.cpp
size:   2273, sum:   150550, avg: 66.23, name: datechooser.h
...
*/
    // [gnu::file example]

    return 0;
}
