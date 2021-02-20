// Copyright 2016 - 2021 gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#include "util.h"

using namespace flw;

#define FLW_ASSERT(X,Y)         util::test(X,Y,__LINE__,__func__);
#define FLW_TRUE(X)             util::test(X,__LINE__,__func__);
#define FLW_ASSERTD(X,Y,Z)      util::test(X,Y,Z,__LINE__,__func__);

int main() {
    { // util::str_copy()
        char buffer[10];

        FLW_ASSERT("Hello", util::str_copy(buffer, "Hello", 10))
        FLW_ASSERT("012345678", util::str_copy(buffer, "012345678", 10))
        FLW_ASSERT("", util::str_copy(buffer, "0123456789", 10))
        FLW_ASSERT("", util::str_copy(buffer, "0123456789", 1))
        FLW_ASSERT("012345678", util::str_copy(buffer, "012345678", 10))
        FLW_ASSERT("", util::str_copy(nullptr, "012345678", 10))
        FLW_ASSERT("", util::str_copy(buffer, nullptr, 10))
    }

    { // util::to_doubles()
        double a, b, c, d, e;
        FLW_ASSERT(3, util::to_doubles("1.23 2.34  3.45 : 4.56 -5.67", &a, &b, &c, &d, &e))
        FLW_ASSERT(5, util::to_doubles("1.23 2.34  3.45   4.56 -5.67", &a, &b, &c, &d, &e))

        FLW_ASSERTD(1.23, a, 0.001)
        FLW_ASSERTD(2.34, b, 0.001)
        FLW_ASSERTD(3.45, c, 0.001)
        FLW_ASSERTD(4.56, d, 0.001)
        FLW_ASSERTD(-5.67, e, 0.001)
    }

    { // util::format_int()
        FLW_ASSERT("123", util::format_int(123).c_str())
        FLW_ASSERT("-12 345", util::format_int(-12345).c_str())
        FLW_ASSERT("1 234", util::format_int(1234).c_str())
        FLW_ASSERT("123 456 789", util::format_int(123456789).c_str())
        FLW_ASSERT("1_234_567_890", util::format_int(1234567890, '_').c_str())
    }

    { // util::time_sleep()
        auto t1 = util::time_micro();
        util::time_sleep(1000);
        auto t2 = (int) (util::time_micro() - t1);
        // printf("sleept in %d\n", t2);
        FLW_TRUE(t2 >= 1000000 && t2 <= 1001000)
    }

    printf("\nTests Finished\n");
    return 0;
}
