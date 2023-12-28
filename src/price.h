// Copyright gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#ifndef FLW_PRICE_H
#define FLW_PRICE_H

#include "date.h"
#include <vector>
#include <string>

// MKALGAM_ON

namespace flw {
    struct Price;

    typedef std::vector<Price> PriceVector;

    //------------------------------------------------------------------------------
    // Price object that is used by the Chart widget
    //
    struct Price {
        std::string                     date;
        double                          close;
        double                          high;
        double                          low;
        double                          vol;

                                        Price();
                                        Price(const std::string& date_value, double value = 0.0);
                                        Price(const std::string& date, double high, double low, double close, double vol = 0.0);
                                        Price(const Price& price);
                                        Price(Price&& price);
        Price&                          operator=(const Price& price);
        Price&                          operator=(Price&& price);
        bool                            operator<(const Price& price) const { return date < price.date; }
        bool                            operator<=(const Price& price) const { return date <= price.date; }
        bool                            operator==(const Price& price) const { return date == price.date; }
        bool                            operator!=(const Price& price) const { return date != price.date; }
        std::string                     format_date(Date::FORMAT format = Date::FORMAT::ISO) const;
        std::string                     format_price(Date::FORMAT format = Date::FORMAT::ISO, bool hlc = true, bool v = true) const;
        void                            print() const;

        static PriceVector              Atr(const PriceVector& in, std::size_t days);
        static PriceVector              DateSerie(const char* start_date, const char* stop_date, Date::RANGE range = Date::RANGE::DAY, const PriceVector& block = PriceVector(), bool long_format = false);
        static PriceVector              DayToMonth(const PriceVector& in);
        static PriceVector              DayToWeek(const PriceVector& in, Date::DAY weekday);
        static PriceVector              ExponentialMovingAverage(const PriceVector& in, std::size_t days);
        static PriceVector              Momentum(const PriceVector& in, std::size_t days);
        static PriceVector              MovingAverage(const PriceVector& in, std::size_t days);
        static void                     Print(const PriceVector& in);
        static PriceVector              RSI(const PriceVector& in, std::size_t days);
        static PriceVector              StdDev(const PriceVector& in, std::size_t days);
        static PriceVector              Stochastics(const PriceVector& in, std::size_t days);
    };
}

// MKALGAM_OFF

#endif // FLW_PRICE_H
