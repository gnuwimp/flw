// Copyright 2016 - 2022 gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#include "util.h"

using namespace flw;

#define FLW_ASSERT(X,Y)         util::test(X,Y,__LINE__,__func__);
#define FLW_TRUE(X)             util::test(X,__LINE__,__func__);
#define FLW_ASSERTD(X,Y,Z)      util::test(X,Y,Z,__LINE__,__func__);

Buf create() {
    return Buf("Hello World", 12);
}

int main() {
    {
        FLW_ASSERTD(1.0, util::to_double("", 1.0), 0.001)
        FLW_ASSERTD(1.0, util::to_double(":", 1.0), 0.001)
        FLW_ASSERTD(345.678, util::to_double("345.678", 1.0), 0.001)
        FLW_ASSERTD(0.678, util::to_double(".678", 1.0), 0.001)
        FLW_ASSERTD(-123.0, util::to_double("-", -123.0), 0.001)
        FLW_ASSERTD(-123.0, util::to_double(".", -123.0), 0.001)
        FLW_ASSERTD(-123.0, util::to_double("-.", -123.0), 0.001)
        FLW_ASSERTD(-123.0, util::to_double(".-", -123.0), 0.001)
        FLW_ASSERTD(0.0, util::to_double("-.0", -123.0), 0.001)
        FLW_ASSERTD(999999999999999.000000, util::to_double("999999999999999.000000", 1.0), 0.001)
        FLW_ASSERTD(-999999999999999.000000, util::to_double("-999999999999999.000000", 1.0), 0.001)

        FLW_ASSERT(666, util::to_int("", 666))
        FLW_ASSERT(123456789012345, util::to_int("123456789012345", 666))
        FLW_ASSERT(-123456789012345, util::to_int("-123456789012345", 666))
    }

    { // util::to_doubles()
        double num[20];
        FLW_ASSERT(0, util::to_doubles("", num, 20))
        FLW_ASSERT(0, util::to_doubles(":1.23 2.34  3.45   4.56 -5.67", num, 20))
        FLW_ASSERT(1, util::to_doubles("1.23 : 2.34  3.45   4.56 -5.67", num, 20))
        FLW_ASSERT(3, util::to_doubles("1.23 2.34  3.45 : 4.56 -5.67", num, 20))
        FLW_ASSERT(5, util::to_doubles("1.23 2.34  3.45   4.56 -5.67", num, 20))

        FLW_ASSERTD(1.23, num[0], 0.001)
        FLW_ASSERTD(2.34, num[1], 0.001)
        FLW_ASSERTD(3.45, num[2], 0.001)
        FLW_ASSERTD(4.56, num[3], 0.001)
        FLW_ASSERTD(-5.67, num[4], 0.001)
    }

    { // util::to_ints()
        int64_t num[20];
        FLW_ASSERT(0, util::to_ints("", num, 20))
        FLW_ASSERT(0, util::to_ints(":123 234  345   456 -567", num, 20))
        FLW_ASSERT(1, util::to_ints("123 : 234  345   456 -567", num, 20))
        FLW_ASSERT(3, util::to_ints("123 234  345 : 456 -567", num, 20))
        FLW_ASSERT(6, util::to_ints("123 234  345   456 -567 123456789123456789", num, 20))

        FLW_ASSERT(123, num[0])
        FLW_ASSERT(234, num[1])
        FLW_ASSERT(345, num[2])
        FLW_ASSERT(456, num[3])
        FLW_ASSERT(-567, num[4])
        FLW_ASSERT(123456789123456789, num[5])
    }

    { // util::format_int()
        FLW_ASSERT("123", util::format_int(123).c_str())
        FLW_ASSERT("-12 345", util::format_int(-12345).c_str())
        FLW_ASSERT("1 234", util::format_int(1234).c_str())
        FLW_ASSERT("123 456 789", util::format_int(123456789).c_str())
        FLW_ASSERT("1_234_567_890", util::format_int(1234567890, '_').c_str())
    }

    { // util::time_sleep()
        auto t1 = util::time_milli();
        util::time_sleep(1000);
        auto t2 = (int) (util::time_milli() - t1);
        // printf("sleept %d mS\n", t2);
        FLW_TRUE(t2 >= 1'000 && t2 <= 1'020)
    }

    {
        auto a = create();
        auto b = a;
        auto c = Buf(b);
        auto d = Buf(1000);
        auto e = std::move(create());
        FLW_ASSERT("Hello World", a.p)
        FLW_ASSERT(12, a.s)
        FLW_ASSERT("Hello World", b.p)
        FLW_ASSERT(12, b.s)
        FLW_ASSERT("Hello World", c.p)
        FLW_ASSERT(12, c.s)
        FLW_ASSERT(1000, d.s)
        FLW_ASSERT("Hello World", e.p)
        FLW_ASSERT(12, e.s)

        e = d;
        FLW_ASSERT("", e.p)
        FLW_ASSERT(1000, e.s)

        // e = e;
    }

    {
        auto s = util::split("abc\tdef", "\t");
        FLW_ASSERT(2, s.size())
        FLW_ASSERT("abc", s[0].c_str())
        FLW_ASSERT("def", s[1].c_str())

        s = util::split("abc\tdef\t\t", "\t");
        FLW_ASSERT(4, s.size())
        FLW_ASSERT("abc", s[0].c_str())
        FLW_ASSERT("def", s[1].c_str())
        FLW_ASSERT("", s[2].c_str())
        FLW_ASSERT("", s[3].c_str())

        s = util::split("\t\t\t", "\t");
        FLW_ASSERT(4, s.size())
        FLW_ASSERT("", s[0].c_str())
        FLW_ASSERT("", s[1].c_str())
        FLW_ASSERT("", s[2].c_str())

        s = util::split("", "\t");
        FLW_ASSERT(1, s.size())
        FLW_ASSERT("", s[0].c_str())

        s = util::split("HELLO", "\t");
        FLW_ASSERT(1, s.size())
        FLW_ASSERT("HELLO", s[0].c_str())
    }

    printf("\nTests Finished\n");
    return 0;
}
