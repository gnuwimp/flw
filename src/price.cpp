// Copyright gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#include "price.h"
#include "util.h"

// MKALGAM_ON

#include <math.h>
#include <algorithm>

//------------------------------------------------------------------------------
flw::Price::Price() {
    high  = 0.0;
    low   = 0.0;
    close = 0.0;
    vol   = 0.0;
}

//------------------------------------------------------------------------------
flw::Price::Price(const std::string& date, double value) {
    this->date = date;
    high       = value;
    low        = value;
    close      = value;
    vol        = value;
}

//------------------------------------------------------------------------------
flw::Price::Price(const std::string& date, double high, double low, double close, double vol) {
    this->date  = date;
    this->high  = high;
    this->low   = low;
    this->close = close;
    this->vol   = vol;

#ifdef DEBUG
    if (close > high || close < low || high < low) {
        fprintf(stderr, "error: values out of order in Price(%s, %15.5f  >=  %15.5f  <=  %15.5f)\n", date.c_str(), high, low, close);
    }
#endif
}

//------------------------------------------------------------------------------
flw::Price::Price(const Price& price) {
    date  = price.date;
    high  = price.high;
    low   = price.low;
    close = price.close;
    vol   = price.vol;
}

//------------------------------------------------------------------------------
flw::Price::Price(Price&& price) {
    date  = price.date;
    high  = price.high;
    low   = price.low;
    close = price.close;
    vol   = price.vol;
}

//------------------------------------------------------------------------------
flw::Price& flw::Price::operator=(const Price& price) {
    date  = price.date;
    high  = price.high;
    low   = price.low;
    close = price.close;
    vol   = price.vol;

    return *this;
}

//------------------------------------------------------------------------------
flw::Price& flw::Price::operator=(Price&& price) {
    date  = price.date;
    high  = price.high;
    low   = price.low;
    close = price.close;
    vol   = price.vol;

    return *this;
}

//------------------------------------------------------------------------------
flw::PriceVector flw::Price::Atr(const PriceVector& in, std::size_t days) {
    PriceVector res;

    if (days > 1 && in.size() > days) {
        auto        tot        = 0.0;
        auto        prev_close = 0.0;
        auto        prev_range = 0.0;
        std::size_t f        = 0;

        days--;

        for (auto& price : in) {
            if (f == 0) {
                tot += price.high - price.low;
            }
            else {
                auto t1 = price.high - price.low;
                auto t2 = fabs(price.high - prev_close);
                auto t3 = fabs(price.low - prev_close);
                auto ra = 0.0;

                if (t1 > t2 && t1 > t3) {
                    ra = t1;
                }
                else if (t2 > t1 && t2 > t3) {
                    ra = t2;
                }
                else {
                    ra = t3;
                }

                tot += ra;

                if (f == days) {
                    prev_range = tot / (days + 1);
                    res.push_back(Price(price.date, prev_range));
                }
                else if (f > days) {
                    prev_range = ((prev_range * days) + ra) / (days + 1);
                    res.push_back(Price(price.date, prev_range));
                }
            }

            prev_close = price.close;
            f++;
        }
    }

    return res;
}

//------------------------------------------------------------------------------
flw::PriceVector flw::Price::DateSerie(const char* start_date, const char* stop_date, Date::RANGE range, const PriceVector& block, bool long_format) {
    int         month   = -1;
    Date        current = Date::FromString(start_date);
    Date        stop    = Date::FromString(stop_date);
    PriceVector res;

    if (range == Date::RANGE::HOUR && current.year() < 1970) {
        return res;
    }

    if (range == Date::RANGE::FRIDAY) {
        while (current.weekday() != Date::DAY::FRIDAY)
            current.add_days(1);
    }
    else if (range == Date::RANGE::SUNDAY) {
        while (current.weekday() != Date::DAY::SUNDAY) {
            current.add_days(1);
        }
    }

    while (current <= stop) {
        Date date(1, 1, 1);

        if (range == Date::RANGE::DAY) {
            date = Date(current);
            current.add_days(1);
        }
        else if (range == Date::RANGE::WEEKDAY) {
            Date::DAY weekday = current.weekday();

            if (weekday >= Date::DAY::MONDAY && weekday <= Date::DAY::FRIDAY) {
                date = Date(current);
            }

            current.add_days(1);
        }
        else if (range == Date::RANGE::FRIDAY || range == Date::RANGE::SUNDAY) {
            date = Date(current);
            current.add_days(7);
        }
        else if (range == Date::RANGE::MONTH) {
            if (current.month() != month) {
                current.day(current.month_days());
                date = Date(current);
                month = current.month();
            }

            current.add_months(1);
        }
        else if (range == Date::RANGE::HOUR) {
            date = Date(current);
            current.add_seconds(3600);
        }
        else if (range == Date::RANGE::MIN) {
            date = Date(current);
            current.add_seconds(60);
        }
        else if (range == Date::RANGE::SEC) {
            date = Date(current);
            current.add_seconds(1);
        }

        if (date.year() > 1) {
            Price price;

            if (range == Date::RANGE::HOUR || range == Date::RANGE::MIN || range == Date::RANGE::SEC) {
                price.date = date.format((long_format == false) ? Date::FORMAT::ISO_TIME : Date::FORMAT::ISO_TIME_LONG);
            }
            else {
                price.date = date.format((long_format == false) ? Date::FORMAT::ISO : Date::FORMAT::ISO_LONG);
            }

            if (block.size() == 0 || std::binary_search(block.begin(), block.end(), price) == false) {
                res.push_back(price);
            }
        }
    }

    return res;
}

//------------------------------------------------------------------------------
flw::PriceVector flw::Price::DayToMonth(const PriceVector& in) {
    PriceVector res;
    Price       current;
    Date        stop;
    Date        pdate;
    std::size_t f = 0;

    for (auto& price : in) {
        if (f == 0) {
            current = price;
            stop = Date::FromString(current.date.c_str());
            stop.day_last();
        }
        else {
            pdate = Date::FromString(price.date.c_str());

            if (stop < pdate) {
                current.date = stop.format();
                res.push_back(current);
                current = price;
                stop = Date::FromString(current.date.c_str());
                stop.day_last();
            }
            else {
                if (price.high > current.high) {
                    current.high = price.high;
                }

                if (price.low < current.low) {
                    current.low = price.low;
                }

                current.vol   += price.vol;
                current.close  = price.close;

            }
        }

        if (f + 1 == in.size()) {
            auto s = stop.format();
            stop.day_last();
            current.date = s;
            res.push_back(current);
        }

        f++;
    }

    return res;
}


//------------------------------------------------------------------------------
flw::PriceVector flw::Price::DayToWeek(const PriceVector& in, Date::DAY weekday) {
    Price       current;
    Date        stop;
    Date        pdate;
    PriceVector res;
    std::size_t f = 0;

    for (auto& price : in) {
        if (f == 0) {
            stop = Date::FromString(price.date.c_str());

            if (weekday > stop.weekday()) {
                stop.weekday(weekday);
            }
            else if (weekday < stop.weekday()) {
                stop.weekday(weekday);
                stop.add_days(7);
            }

            current = price;
        }
        else {
            pdate = Date::FromString(price.date.c_str());

            if (stop < pdate) {
                current.date = stop.format();
                res.push_back(current);
                current = price;
            }
            else {
                if (price.high > current.high) {
                    current.high = price.high;
                }

                if (price.low < current.low) {
                    current.low = price.low;
                }

                current.vol   += price.vol;
                current.close  = price.close;
            }

            while (stop < pdate) {
                stop.add_days(7);
            }

            current.date = stop.format();
        }

        if (f + 1 == in.size()) {
            current.date = stop.format();
            res.push_back(current);
        }

        f++;
    }

    return res;
}

//------------------------------------------------------------------------------
flw::PriceVector flw::Price::ExponentialMovingAverage(const PriceVector& in, std::size_t days) {
    PriceVector res;

    if (days > 1 && days < in.size()) {
        auto         sma   = 0.0;
        auto         prev  = 0.0;
        auto         multi = 2.0 / double(days + 1.0);
        std::size_t f      = 0;

        for (auto& price : in) {
            if (f < (days - 1)) {
                sma += price.close;
            }
            else if (f == (days - 1)) {
                sma += price.close;
                prev = sma / days;
                res.push_back(Price(price.date, prev));
            }
            else {
                prev = ((price.close - prev) * multi) + prev;
                res.push_back(Price(price.date, prev));
            }

            f++;
        }
    }

    return res;
}

//------------------------------------------------------------------------------
std::string flw::Price::format_date(Date::FORMAT format) const {
    auto date = flw::Date::FromString(this->date.c_str());
    return date.format(format);
}

//------------------------------------------------------------------------------
std::string flw::Price::format_price(Date::FORMAT format, bool hlc, bool v) const {
    auto d = flw::Date::FromString(this->date.c_str());

    if (v == true) {
        return flw::util::format("%s\t%f\t%f\t%f\t%f", d.format(format).c_str(), high, low, close, vol);
    }
    else if (hlc == false || (fabs(close - low) < 0.000001 && fabs(close - high) < 0.000001)) {
        return flw::util::format("%s\t%f", d.format(format).c_str(), close);
    }
    else {
        return flw::util::format("%s\t%f\t%f\t%f", d.format(format).c_str(), high, low, close);
    }
}

//------------------------------------------------------------------------------
flw::PriceVector flw::Price::Momentum(const PriceVector& in, std::size_t days) {
    std::size_t start = days - 1;
    PriceVector res;

    if (days > 1 && days < in.size()) {
        for (auto f = start; f < in.size(); f++) {
            auto& price1 = in[f];
            auto& price2 = in[f - start];

            res.push_back(Price(price1.date, price1.close - price2.close));
        }

    }

    return res;
}

//------------------------------------------------------------------------------
flw::PriceVector flw::Price::MovingAverage(const PriceVector& in, std::size_t days) {
    PriceVector res;

    if (days > 1 && days <= 500 && days < in.size()) {
        std::size_t count = 0;
        auto        sum   = 0.0;
        auto        tmp   = new double[in.size() + 1];

        for (auto& price : in) {
            count++;

            if (count < days) { //  Add data until the first moving average price can be calculated
                tmp[count - 1] = price.close;
                sum += price.close;
            }
            else if (count == days) { //  This is the first point
                tmp[count - 1] = price.close;
                sum += price.close;
                res.push_back(Price(price.date, sum / days));
            }
            else { //  Remove oldest data in range and add current to sum
                tmp[count - 1] = price.close;
                sum -= tmp[count - (days + 1)];
                sum += price.close;
                res.push_back(Price(price.date, sum / days));
            }
        }

        delete [] tmp;
    }

    return res;
}

//------------------------------------------------------------------------------
void flw::Price::print() const {
    printf("Price| date: %s   high=%12.4f   low=%12.4f   close=%12.4f   vol=%12.0f\n", date.c_str(), high, low, close, vol);
    fflush(stdout);
}

//------------------------------------------------------------------------------
void flw::Price::Print(const PriceVector& in) {
    printf("PriceVector(%d)\n", (int) in.size());

    for (auto& price : in) {
        price.print();
    }
}

//------------------------------------------------------------------------------
flw::PriceVector flw::Price::RSI(const PriceVector& in, std::size_t days) {
    PriceVector res;

    if (days > 1 && days <= in.size()) {
        auto avg_gain = 0.0;
        auto avg_loss = 0.0;

        for (std::size_t f = 1; f < in.size(); f++) {
            auto  diff  = 0.0;
            auto& price = in[f];
            auto& prev  = in[f - 1];

            diff = price.close - prev.close;

            if (f <= days) {
                if (diff > 0) {
                    avg_gain += diff;
                }
                else {
                    avg_loss += fabs(diff);
                }
            }

            if (f == days) {
                avg_gain = avg_gain / days;
                avg_loss = avg_loss / days;
                res.push_back(Price(price.date, 100 - (100 / (1 + (avg_gain / avg_loss)))));
            }
            else if (f > days) {
                avg_gain = ((avg_gain * (days - 1)) + ((diff > 0) ? fabs(diff) : 0)) / days;
                avg_loss = ((avg_loss * (days - 1)) + ((diff < 0) ? fabs(diff) : 0)) / days;
                res.push_back(Price(price.date, 100 - (100 / (1 + (avg_gain / avg_loss)))));
            }
        }
    }

    return res;
}

//------------------------------------------------------------------------------
flw::PriceVector flw::Price::StdDev(const PriceVector& in, std::size_t days) {
    PriceVector res;

    if (days > 2 && days <= in.size()) {
        std::size_t count  = 0;
        auto        sum    = 0.0;
        Price       price2;

        for (auto& price : in) {
            count++;
            sum += price.close;

            if (count >= days) {
                auto mean = sum / days;
                auto dev  = 0.0;

                for (std::size_t j = count - days; j < count; j++) {
                    price2 = in[j];
                    auto tmp = price2.close - mean;

                    tmp *= tmp;
                    dev  += tmp;
                }

                dev /= days;
                dev = sqrt(dev);

                price2 = in[count - days];
                sum -= price2.close;

                price2 = in[count - 1];
                res.push_back(Price(price2.date, dev));
            }
        }
    }

    return res;
}

//------------------------------------------------------------------------------
flw::PriceVector flw::Price::Stochastics(const PriceVector& in, std::size_t days) {
    auto        high = 0.0;
    auto        low  = 0.0;
    PriceVector res;

    for (std::size_t f = 0; f < in.size(); f++) {
        auto& price = in[f];

        if ((f + 1) >= days) {
            high = price.high;
            low  = price.low;

            for (std::size_t j = (f + 1) - days; j < (f + 1) - 1; j++) { //  Get max/min in for current range.
                auto& price2 = in[j];

                if (price2.high > high) {
                    high = price2.high;
                }

                if (price2.low < low) {
                    low = price2.low;
                }
            }

            auto diff1 = price.close - low;
            auto diff2 = high - low;

            if (diff2 > 0.0001) {
                auto kval  = 100 * (diff1 / diff2);
                res.push_back(Price(price.date, kval));
            }
        }
    }

    return res;
}

// MKALGAM_OFF
