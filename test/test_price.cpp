// Copyright 2019 - 2021 gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#include "price.h"
#include "util.h"

using namespace flw;

#define FLW_ASSERT(X,Y)         util::test(X,Y,__LINE__,__func__);
#define FLW_TRUE(X)             util::test(X,__LINE__,__func__);
#define FLW_ASSERTD(X,Y,Z)      util::test(X,Y,Z,__LINE__,__func__);

static void atr() {
    fprintf(stderr, "Price::Atr()\n"); fflush(stderr);

    std::vector<Price> in;
    std::vector<Price> out;

    in.push_back(Price("20001023", 61.0000, 59.0312, 59.3750));
    in.push_back(Price("20001024", 61.0000, 58.3750, 58.9062));
    in.push_back(Price("20001025", 58.8438, 53.6250, 54.3125));
    in.push_back(Price("20001026", 55.1250, 47.4375, 51.0000));
    in.push_back(Price("20001027", 54.0625, 50.5000, 51.5938));

    in.push_back(Price("20001030", 53.9688, 49.7812, 52.0000));
    in.push_back(Price("20001031", 56.0000, 52.5000, 55.4375));
    in.push_back(Price("20001101", 55.2188, 52.6250, 52.9375));
    in.push_back(Price("20001102", 55.0312, 53.2500, 54.5312));
    in.push_back(Price("20001103", 57.4922, 53.7500, 56.5312));

    in.push_back(Price("20001106", 57.0938, 55.2500, 55.3438));
    in.push_back(Price("20001107", 56.8125, 54.3438, 55.7188));
    in.push_back(Price("20001108", 55.5625, 50.0000, 50.1562));
    in.push_back(Price("20001109", 50.0625, 46.8438, 48.8125));
    in.push_back(Price("20001110", 47.6875, 44.4688, 44.5938));

    in.push_back(Price("20001113", 44.9062, 40.6250, 42.6562));
    in.push_back(Price("20001114", 47.3750, 44.1641, 47.0000));
    in.push_back(Price("20001115", 47.6250, 45.1250, 46.9688));
    in.push_back(Price("20001116", 48.0156, 43.2500, 43.6250));
    in.push_back(Price("20001117", 45.0391, 42.6875, 44.6562));

    out = Price::Atr(in, 14);

    FLW_TRUE(out[0].date == "20001109")
    FLW_TRUE(out[1].date == "20001110")
    FLW_TRUE(out[2].date == "20001113")
    FLW_TRUE(out[3].date == "20001114")
    FLW_TRUE(out[4].date == "20001115")
    FLW_TRUE(out[5].date == "20001116")
    FLW_TRUE(out[6].date == "20001117")

    FLW_ASSERTD(3.6646, out[0].high, 0.001)
    FLW_ASSERTD(3.7131, out[1].high, 0.001)
    FLW_ASSERTD(3.7537, out[2].high, 0.001)
    FLW_ASSERTD(3.8226, out[3].high, 0.001)
    FLW_ASSERTD(3.7282, out[4].high, 0.001)
    FLW_ASSERTD(3.8023, out[5].high, 0.001)
    FLW_ASSERTD(3.6986, out[6].high, 0.001)
}

static void day_to_month() {
    fprintf(stderr, "Price::DayToMonth()\n"); fflush(stderr);

    std::vector<Price> in;
    std::vector<Price> out;

    in.push_back(Price("20000105", 1.0, 1.0, 1.0, 11.0));

    out = Price::DayToMonth(in);
    FLW_TRUE(out.size() == 1)
    FLW_TRUE(out[0].date == "20000131")
    FLW_TRUE(out[0].low == 1)
    FLW_TRUE(out[0].high == 1)
    FLW_TRUE(out[0].vol == 11)

    in.push_back(Price("20000115", 2.0, 2.0, 2.0, 22.0));
    in.push_back(Price("20000124", 3.0, 3.0, 3.0, 33.0));
    in.push_back(Price("20000202", 4.0, 4.0, 4.0, 44.0));
    in.push_back(Price("20000203", 5.0, 5.0, 5.0, 55.0));
    in.push_back(Price("20000229", 6.0, 6.0, 6.0, 66.0));
    in.push_back(Price("20000330", 7.0, 7.0, 7.0, 77.0));
    in.push_back(Price("20000401", 28.0, 7.0, 7.0, 77.0));
    in.push_back(Price("20000421", 7.0, 2.0, 5.0, 23.0));

    out = Price::DayToMonth(in);
    FLW_TRUE(out.size() == 4)

    FLW_TRUE(out[0].date == "20000131")
    FLW_TRUE(out[0].low == 1)
    FLW_TRUE(out[0].high == 3)
    FLW_TRUE(out[0].close == 3)
    FLW_TRUE(out[0].vol == 66)

    FLW_TRUE(out[1].date == "20000229")
    FLW_TRUE(out[1].low == 4)
    FLW_TRUE(out[1].high == 6)
    FLW_TRUE(out[1].vol == 165)

    FLW_TRUE(out[2].date == "20000331")
    FLW_TRUE(out[2].low == 7)
    FLW_TRUE(out[2].high == 7)
    FLW_TRUE(out[2].vol == 77)

    FLW_TRUE(out[3].date == "20000430")
    FLW_TRUE(out[3].low == 2)
    FLW_TRUE(out[3].high == 28)
    FLW_TRUE(out[3].close == 5)
    FLW_TRUE(out[3].vol == 100)
}

static void day_to_week() {
    fprintf(stderr, "Price::DayToWeek()\n"); fflush(stderr);

    std::vector<Price> in;
    std::vector<Price> out;

    in.push_back(Price("20000105", 1.0, 1.0, 1.0, 11.0));
    out = Price::DayToWeek(in, Date::DAY::FRIDAY);
    FLW_TRUE(out.size() == 1)

    FLW_TRUE(out[0].date == "20000107")
    FLW_TRUE((int) (out[0].high) == 1)
    FLW_TRUE((int) (out[0].vol) == 11)

    in.push_back(Price("20000115", 2, 2, 2, 22));
    in.push_back(Price("20000124", 3, 3, 3, 33));
    in.push_back(Price("20000202", 4, 4, 4, 44));
    in.push_back(Price("20000203", 5, 5, 5, 55));
    in.push_back(Price("20000224", 6, 6, 6, 66));

    out = Price::DayToWeek(in, Date::DAY::FRIDAY);
    FLW_TRUE(out.size() == 5)

    FLW_TRUE(out[0].date == "20000107")
    FLW_TRUE((int) (out[0].high) == 1)
    FLW_TRUE((int) (out[0].vol) == 11)

    FLW_TRUE(out[1].date == "20000121")
    FLW_TRUE((int) (out[1].high) == 2)
    FLW_TRUE((int) (out[1].vol) == 22)

    FLW_TRUE(out[2].date == "20000128")
    FLW_TRUE((int) (out[2].high) == 3)
    FLW_TRUE((int) (out[2].vol) == 33)

    FLW_TRUE(out[3].date == "20000204")
    FLW_TRUE((int) (out[3].high) == 5)
    FLW_TRUE((int) (out[3].low) == 4)
    FLW_TRUE((int) (out[3].close) == 5)
    FLW_TRUE((int) (out[3].vol) == 55 + 44)

    FLW_TRUE(out[4].date == "20000225")
    FLW_TRUE((int) (out[4].high) == 6)
    FLW_TRUE((int) (out[4].vol) == 66)

    out = Price::DayToWeek(in, Date::DAY::SUNDAY);
    FLW_TRUE(out.size() == 5)

    FLW_TRUE(out[0].date == "20000109")
    FLW_TRUE((int) (out[0].high) == 1)
    FLW_TRUE((int) (out[0].vol) == 11)

    FLW_TRUE(out[1].date == "20000116")
    FLW_TRUE((int) (out[1].high) == 2)
    FLW_TRUE((int) (out[1].vol) == 22)

    FLW_TRUE(out[2].date == "20000130")
    FLW_TRUE((int) (out[2].high) == 3)
    FLW_TRUE((int) (out[2].vol) == 33)

    FLW_TRUE(out[3].date == "20000206")
    FLW_TRUE((int) (out[3].high) == 5)
    FLW_TRUE((int) (out[3].low) == 4)
    FLW_TRUE((int) (out[3].close) == 5)
    FLW_TRUE((int) (out[3].vol) == 44 + 55)

    FLW_TRUE(out[4].date == "20000227")
    FLW_TRUE((int) (out[4].high) == 6)
    FLW_TRUE((int) (out[4].vol) == 66)

    out = Price::DayToWeek(in, Date::DAY::MONDAY);
    FLW_TRUE(out.size() == 5)

    FLW_TRUE(out[0].date == "20000110")
    FLW_TRUE((int) (out[0].high) == 1)
    FLW_TRUE((int) (out[0].vol) == 11)

    FLW_TRUE(out[1].date == "20000117")
    FLW_TRUE((int) (out[1].high) == 2)
    FLW_TRUE((int) (out[1].vol) == 22)

    FLW_TRUE(out[2].date == "20000124")
    FLW_TRUE((int) (out[2].high) == 3)
    FLW_TRUE((int) (out[2].vol) == 33)

    FLW_TRUE(out[3].date == "20000207")
    FLW_TRUE((int) (out[3].high) == 5)
    FLW_TRUE((int) (out[3].low) == 4)
    FLW_TRUE((int) (out[3].close) == 5)
    FLW_TRUE((int) (out[3].vol) == 44 + 55)

    FLW_TRUE(out[4].date == "20000228")
    FLW_TRUE((int) (out[4].high) == 6)
    FLW_TRUE((int) (out[4].vol) == 66)

    out = Price::DayToWeek(in, Date::DAY::WENDSDAY);
    FLW_TRUE(out.size() == 6)

    FLW_TRUE(out[0].date == "20000105")
    FLW_TRUE((int) (out[0].high) == 1)
    FLW_TRUE((int) (out[0].vol) == 11)

    FLW_TRUE(out[1].date == "20000119")
    FLW_TRUE((int) (out[1].high) == 2)
    FLW_TRUE((int) (out[1].vol) == 22)

    FLW_TRUE(out[2].date == "20000126")
    FLW_TRUE((int) (out[2].high) == 3)
    FLW_TRUE((int) (out[2].vol) == 33)

    FLW_TRUE(out[3].date == "20000202")
    FLW_TRUE((int) (out[3].high) == 4)
    FLW_TRUE((int) (out[3].vol) == 44)

    FLW_TRUE(out[4].date == "20000209")
    FLW_TRUE((int) (out[4].high) == 5)
    FLW_TRUE((int) (out[4].vol) == 55)

    FLW_TRUE(out[5].date == "20000301")
    FLW_TRUE((int) (out[5].high) == 6)
    FLW_TRUE((int) (out[5].vol) == 66)

    in.clear();
    in.push_back(Price("20000103", 1, 1, 1, 11));
    in.push_back(Price("20000104", 2, 2, 2, 22));
    in.push_back(Price("20000105", 23, 3, 3, 33));
    in.push_back(Price("20000106", 4, 4, 4, 44));
    in.push_back(Price("20000107", 5, 5, 5, 55));
    in.push_back(Price("20000110", 6, 6, 6, 66));

    out = Price::DayToWeek(in, Date::DAY::FRIDAY);
    FLW_TRUE(out.size() == 2)

    FLW_TRUE(out[0].date == "20000107")
    FLW_TRUE((int) (out[0].high) == 23)
    FLW_TRUE((int) (out[0].low) == 1)
    FLW_TRUE((int) (out[0].close) == 5)
    FLW_TRUE((int) (out[0].vol) == 11 + 22 + 33 + 44 + 55)

    FLW_TRUE(out[1].date == "20000114")
    FLW_TRUE((int) (out[1].high) == 6)
    FLW_TRUE((int) (out[1].low) == 6)
    FLW_TRUE((int) (out[1].close) == 6)
    FLW_TRUE((int) (out[1].vol) == 66)
}

static void exponential_moving_average() {
    fprintf(stderr, "Price::ExponentialMovingAverage()\n"); fflush(stderr);

    std::vector<Price> in;
    std::vector<Price> out;

    in.push_back(Price("20010101", 64.75));
    in.push_back(Price("20010102", 63.79));
    in.push_back(Price("20010103", 63.73));
    in.push_back(Price("20010104", 63.73));
    in.push_back(Price("20010105", 63.55));
    in.push_back(Price("20010106", 63.19));
    in.push_back(Price("20010107", 63.91));
    in.push_back(Price("20010108", 63.85));
    in.push_back(Price("20010109", 62.95));
    in.push_back(Price("20010110", 63.37));

    in.push_back(Price("20010111", 61.33));
    in.push_back(Price("20010112", 61.51));
    in.push_back(Price("20010113", 61.87));
    in.push_back(Price("20010114", 60.25));
    in.push_back(Price("20010115", 59.35));
    in.push_back(Price("20010116", 59.95));
    in.push_back(Price("20010117", 58.93));
    in.push_back(Price("20010118", 57.68));
    in.push_back(Price("20010119", 58.82));
    in.push_back(Price("20010120", 58.87));

    out = Price::ExponentialMovingAverage(in, 10);

    FLW_ASSERT(11, out.size())
    FLW_ASSERTD(63.682, out[0].close, 0.001)
    FLW_ASSERTD(63.254, out[1].close, 0.001)
    FLW_ASSERTD(59.870, out[10].close, 0.001)
    FLW_TRUE(out[0].date == "20010110")
    FLW_TRUE(out[10].date == "20010120")
}

//--------------------------------------------------------------------------
void momentum() {
    fprintf(stderr, "Price::Momentum()\n"); fflush(stderr);

    std::vector<Price> in;
    std::vector<Price> out;

    in.push_back(Price("20010101", 64.75));
    in.push_back(Price("20010102", 63.79));
    in.push_back(Price("20010103", 63.73));
    in.push_back(Price("20010104", 63.73));

    in.push_back(Price("20010105", 63.55));
    in.push_back(Price("20010106", 63.19));
    in.push_back(Price("20010107", 63.91));
    in.push_back(Price("20010108", 63.85));
    in.push_back(Price("20010109", 62.95));
    in.push_back(Price("20010110", 63.37));

    out = Price::Momentum(in, 5);

    FLW_ASSERT(6, out.size())
    FLW_ASSERTD(63.55 - 64.75, out[0].close, 0.001)
    FLW_ASSERTD(63.37 - 63.19, out[5].close, 0.001)
    FLW_TRUE(out[0].date == "20010105")
    FLW_TRUE(out[5].date == "20010110")
}

void moving_average() {
    fprintf(stderr, "Price::MovingAverage()\n"); fflush(stderr);

    std::vector<Price> in;
    std::vector<Price> out;

    in.push_back(Price("20010101", 67.50));
    in.push_back(Price("20010102", 66.50));
    in.push_back(Price("20010103", 66.44));
    in.push_back(Price("20010104", 66.44));
    in.push_back(Price("20010105", 66.35));
    in.push_back(Price("20010106", 65.88));
    in.push_back(Price("20010107", 66.63));
    in.push_back(Price("20010108", 66.56));
    in.push_back(Price("20010109", 65.63));

    in.push_back(Price("20010110", 66.06));
    in.push_back(Price("20010111", 63.94));
    in.push_back(Price("20010112", 64.13));
    in.push_back(Price("20010113", 64.50));
    in.push_back(Price("20010114", 62.81));
    in.push_back(Price("20010115", 61.88));

    out = Price::MovingAverage(in, 10);

    FLW_ASSERT(6, out.size())
    FLW_ASSERTD(66.399, out[0].close, 0.0001)
    FLW_ASSERTD(64.802, out[5].close, 0.0001)
    FLW_TRUE(out[0].date == "20010110")
    FLW_TRUE(out[5].date == "20010115")
}

void rsi() {
    fprintf(stderr, "Price::RSI()\n"); fflush(stderr);

    std::vector<Price> in;
    std::vector<Price> out;

    in.push_back(Price("20000100", 46.1250));
    in.push_back(Price("20000101", 47.1250));
    in.push_back(Price("20000102", 46.4375));
    in.push_back(Price("20000103", 46.9375));
    in.push_back(Price("20000104", 44.9375));
    in.push_back(Price("20000105", 44.2500));
    in.push_back(Price("20000106", 44.6250));
    in.push_back(Price("20000107", 45.7500));
    in.push_back(Price("20000108", 47.8125));
    in.push_back(Price("20000109", 47.5625));
    in.push_back(Price("20000110", 47.0000));
    in.push_back(Price("20000111", 44.5625));
    in.push_back(Price("20000112", 46.3125));
    in.push_back(Price("20000113", 47.6875));
    in.push_back(Price("20000114", 46.6875));
    in.push_back(Price("20000115", 45.6875));
    in.push_back(Price("20000116", 43.0625));
    in.push_back(Price("20000117", 43.5625));
    in.push_back(Price("20000118", 44.8750));
    in.push_back(Price("20000119", 43.6875));

    out  = Price::RSI(in, 14);

    FLW_ASSERT(6, out.size())
    FLW_ASSERTD(51.7786, out[0].close, 0.001)
    FLW_ASSERTD(43.9921, out[5].close, 0.001)
    FLW_TRUE(out[0].date == "20000114")
    FLW_TRUE(out[5].date == "20000119")
}

void std_dev() {
    fprintf(stderr, "Price::StdDev()\n"); fflush(stderr);

    std::vector<Price> in;
    std::vector<Price> out;

    in.push_back(Price("20010101", 109.00));
    in.push_back(Price("20010102", 103.06));
    in.push_back(Price("20010103", 102.75));
    in.push_back(Price("20010104", 108.00));
    in.push_back(Price("20010105", 107.56));
    in.push_back(Price("20010106", 105.25));
    in.push_back(Price("20010107", 107.69));
    in.push_back(Price("20010108", 108.63));
    in.push_back(Price("20010109", 107.00));
    in.push_back(Price("20010110", 109.00));
    in.push_back(Price("20010111", 110.00));
    in.push_back(Price("20010112", 112.75));
    in.push_back(Price("20010113", 113.50));
    in.push_back(Price("20010114", 114.25));
    in.push_back(Price("20010115", 115.25));
    in.push_back(Price("20010116", 121.50));
    in.push_back(Price("20010117", 126.88));
    in.push_back(Price("20010118", 122.50));
    in.push_back(Price("20010119", 119.00));
    in.push_back(Price("20010120", 122.50));
    in.push_back(Price("20010121", 123.50));

    out = Price::StdDev(in, 20);

    FLW_ASSERT(2, out.size())
    FLW_ASSERTD(6.787, out[0].close, 0.001)
    FLW_ASSERTD(7.160, out[1].close, 0.001)
    FLW_TRUE(out[0].date == "20010120")
    FLW_TRUE(out[1].date == "20010121")
}

void stochastics() {
    fprintf(stderr, "Price::Stochastics()\n"); fflush(stderr);

    std::vector<Price> in;
    std::vector<Price> out;

    in.push_back(Price("20130422", 1565.55, 1548.19, 1562.50, /*1555.25,*/ 2979880000));
    in.push_back(Price("20130423", 1579.58, 1562.50, 1578.78, /*1562.50,*/ 3565150000));
    in.push_back(Price("20130424", 1583.00, 1575.80, 1578.79, /*1578.78,*/ 3598240000));
    in.push_back(Price("20130425", 1592.64, 1578.93, 1585.16, /*1578.93,*/ 3908580000));
    in.push_back(Price("20130426", 1585.78, 1577.56, 1582.24, /*1585.16,*/ 3198620000));
    in.push_back(Price("20130429", 1596.65, 1582.34, 1593.61, /*1582.34,*/ 2891200000));
    in.push_back(Price("20130430", 1597.57, 1586.50, 1597.57, /*1593.58,*/ 3745070000));
    in.push_back(Price("20130501", 1597.55, 1581.28, 1582.70, /*1597.55,*/ 3530320000));
    in.push_back(Price("20130502", 1598.60, 1582.77, 1597.59, /*1582.77,*/ 3366950000));
    in.push_back(Price("20130503", 1618.46, 1597.60, 1614.42, /*1597.60,*/ 3603910000));
    in.push_back(Price("20130506", 1619.77, 1614.21, 1617.50, /*1614.40,*/ 3062240000));
    in.push_back(Price("20130507", 1626.03, 1616.64, 1625.96, /*1617.55,*/ 3309580000));
    in.push_back(Price("20130508", 1632.78, 1622.70, 1632.69, /*1625.95,*/ 3554700000));
    in.push_back(Price("20130509", 1635.01, 1623.09, 1626.67, /*1632.69,*/ 3457400000));
    in.push_back(Price("20130510", 1633.70, 1623.71, 1633.70, /*1626.69,*/ 3086470000));
    in.push_back(Price("20130513", 1636.00, 1626.74, 1633.77, /*1632.10,*/ 2910600000));
    in.push_back(Price("20130514", 1651.10, 1633.75, 1650.34, /*1633.75,*/ 3457790000));
    in.push_back(Price("20130515", 1661.49, 1646.68, 1658.78, /*1649.13,*/ 3657440000));
    in.push_back(Price("20130516", 1660.51, 1648.60, 1650.47, /*1658.07,*/ 3513130000));
    in.push_back(Price("20130517", 1667.47, 1652.45, 1667.47, /*1652.45,*/ 3440710000));
    in.push_back(Price("20130520", 1672.84, 1663.52, 1666.29, /*1665.71,*/ 3275080000));
    in.push_back(Price("20130521", 1674.93, 1662.67, 1669.16, /*1666.20,*/ 3513560000));
    in.push_back(Price("20130522", 1687.18, 1648.86, 1655.35, /*1669.39,*/ 4361020000));
    in.push_back(Price("20130523", 1655.50, 1635.53, 1650.51, /*1651.62,*/ 3945510000));
    in.push_back(Price("20130524", 1649.78, 1636.88, 1649.60, /*1646.67,*/ 2758080000));
    in.push_back(Price("20130528", 1674.21, 1652.63, 1660.06, /*1652.63,*/ 3457400000));
    in.push_back(Price("20130529", 1656.57, 1640.05, 1648.36, /*1656.57,*/ 3587140000));
    in.push_back(Price("20130530", 1661.91, 1648.61, 1654.41, /*1649.14,*/ 3498620000));
    in.push_back(Price("20130531", 1658.99, 1630.74, 1630.74, /*1652.13,*/ 4099600000));
    in.push_back(Price("20130603", 1640.42, 1622.72, 1640.42, /*1631.71,*/ 3952070000));
    in.push_back(Price("20130604", 1646.53, 1623.62, 1631.38, /*1640.73,*/ 3653840000));
    in.push_back(Price("20130605", 1629.31, 1607.09, 1608.90, /*1629.05,*/ 3632350000));
    in.push_back(Price("20130606", 1622.56, 1598.23, 1622.56, /*1609.29,*/ 3547380000));
    in.push_back(Price("20130607", 1644.40, 1625.27, 1643.38, /*1625.27,*/ 3371990000));
    in.push_back(Price("20130610", 1648.69, 1639.26, 1642.81, /*1644.67,*/ 2978730000));
    in.push_back(Price("20130611", 1640.13, 1622.92, 1626.13, /*1638.64,*/ 3435710000));
    in.push_back(Price("20130612", 1637.71, 1610.92, 1612.52, /*1629.94,*/ 3202550000));
    in.push_back(Price("20130613", 1639.25, 1608.07, 1636.36, /*1612.15,*/ 3378620000));
    in.push_back(Price("20130614", 1640.80, 1623.96, 1626.73, /*1635.52,*/ 2939400000));
    in.push_back(Price("20130617", 1646.50, 1630.34, 1639.04, /*1630.64,*/ 3137080000));
    in.push_back(Price("20130618", 1654.19, 1639.77, 1651.81, /*1639.77,*/ 3120980000));
    in.push_back(Price("20130619", 1652.45, 1628.91, 1628.93, /*1651.83,*/ 3545060000));
    in.push_back(Price("20130620", 1624.62, 1584.32, 1588.19, /*1624.62,*/ 4858850000));

    out = Price::Stochastics(in, 14);

    FLW_ASSERT(30, out.size())
    FLW_ASSERTD(90.393, out[0].close, 0.001)
    FLW_ASSERTD(5.538, out[29].close, 0.001)
    FLW_TRUE(out[0].date == "20130509")
    FLW_TRUE(out[29].date == "20130620")
}

static void date_serie() {
    fprintf(stderr, "Price::DateSerie()\n"); fflush(stderr);

    std::vector<Price> block;

    block.push_back(Price("20120210")); // Fr
    block.push_back(Price("20120225")); // Sa
    block.push_back(Price("20120327")); // Tu
    block.push_back(Price("20120328")); // We
    block.push_back(Price("20121031")); // We

    {
        std::vector<Price> arr = Price::DateSerie("20120201", "20120331");
        // Price::Print(arr);
        FLW_TRUE(arr.size() == 60)
        FLW_TRUE(arr[0].date == "20120201")
        FLW_TRUE(arr.back().date == "20120331")
    }

    {
        std::vector<Price> arr = Price::DateSerie("20120201", "20120331", Date::RANGE::DAY, block);
        // Price::Print(arr);
        FLW_TRUE(arr.size() == 56)
        FLW_TRUE(arr[0].date == "20120201")
        FLW_TRUE(arr.back().date == "20120331")
    }

    {
        std::vector<Price> arr = Price::DateSerie("20120201", "20120401", Date::RANGE::WEEKDAY);
        // Price::Print(arr);
        FLW_TRUE(arr.size() == 43)
        FLW_TRUE(arr[0].date == "20120201")
        FLW_TRUE(arr.back().date == "20120330")
    }

    {
        std::vector<Price> arr = Price::DateSerie("20120201", "20120401", Date::RANGE::WEEKDAY, block);
        // Price::Print(arr);
        FLW_TRUE(arr.size() == 40)
        FLW_TRUE(arr[0].date == "20120201")
        FLW_TRUE(arr.back().date == "20120330")
    }

    {
        std::vector<Price> arr = Price::DateSerie("20120201", "20120331", Date::RANGE::FRIDAY);
        // Price::Print(arr);
        FLW_TRUE(arr.size() == 9)
        FLW_TRUE(arr[0].date == "20120203")
        FLW_TRUE(arr.back().date == "20120330")
    }

    {
        std::vector<Price> arr = Price::DateSerie("20120201", "20120330", Date::RANGE::FRIDAY);
        // Price::Print(arr);
        FLW_TRUE(arr.size() == 9)
        FLW_TRUE(arr[0].date == "20120203")
        FLW_TRUE(arr.back().date == "20120330")
    }

    {
        std::vector<Price> arr = Price::DateSerie("20120201", "20120331", Date::RANGE::SUNDAY);
        // Price::Print(arr);
        FLW_TRUE(arr.size() == 8)
        FLW_TRUE(arr[0].date == "20120205")
        FLW_TRUE(arr.back().date == "20120325")
    }

    {
        std::vector<Price> arr = Price::DateSerie("20120111", "20121130", Date::RANGE::MONTH);
        // Price::Print(arr);
        FLW_TRUE(arr.size() == 11)
        FLW_TRUE(arr[0].date == "20120131")
        FLW_TRUE(arr.back().date == "20121130")
    }

    {
        std::vector<Price> arr = Price::DateSerie("20120111", "20121130", Date::RANGE::MONTH, block);
        // Price::Print(arr);
        FLW_TRUE(arr.size() == 10)
        FLW_TRUE(arr[0].date == "20120131")
        FLW_TRUE(arr.back().date == "20121130")
    }

    {
        std::vector<Price> arr = Price::DateSerie("20120111", "20121031", Date::RANGE::MONTH, block);
        // Price::Print(arr);
        FLW_TRUE(arr.size() == 9)
        FLW_TRUE(arr[0].date == "20120131")
        FLW_TRUE(arr.back().date == "20120930")
    }

    {
        std::vector<Price> arr = Price::DateSerie("20120201", "20130313", Date::RANGE::MONTH);
        // Price::Print(arr);
        FLW_TRUE(arr.size() == 13)
        FLW_TRUE(arr[0].date == "20120229")
        FLW_TRUE(arr.back().date == "20130228")
    }

    {
        std::vector<Price> arr = Price::DateSerie("2021-11-02 10:00:00", "2021-11-05 23:00:00", Date::RANGE::HOUR);
        // Price::Print(arr);
        FLW_TRUE(arr.size() == 86)
        FLW_TRUE(arr[0].date == "20211102 100000")
        FLW_TRUE(arr.back().date == "20211105 230000")
    }

    {
        std::vector<Price> arr = Price::DateSerie("2021-11-02 10:00:00", "2021-11-05 23:00:00", Date::RANGE::MIN);
        // Price::Print(arr);
        FLW_TRUE(arr.size() == 5101)
        FLW_TRUE(arr[0].date == "20211102 100000")
        FLW_TRUE(arr.back().date == "20211105 230000")
    }

    {
        std::vector<Price> arr = Price::DateSerie("2021-11-02 10:00:00", "2021-11-05 23:00:00", Date::RANGE::SEC);
        // Price::Print(arr);
        FLW_TRUE(arr.size() == 306001)
        FLW_TRUE(arr[0].date == "20211102 100000")
        FLW_TRUE(arr.back().date == "20211105 230000")
    }
}

int main() {
    atr();
    day_to_month();
    day_to_week();
    exponential_moving_average();
    momentum();
    moving_average();
    rsi();
    std_dev();
    stochastics();
    date_serie();

    fprintf(stderr, "Test Finished\n");
    return 0;
}
