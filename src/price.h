// Copyright 2016 - 2021 gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#ifndef FLW_PRICE_H
#define FLW_PRICE_H

#include "date.h"
#include <vector>
#include <string>

// MKALGAM_ON

namespace flw {
    struct Price;

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
        std::string                     format(Date::FORMAT format = Date::FORMAT::ISO) const;
        void                            print() const;

        static std::vector<Price>       Atr(const std::vector<Price>& in, std::size_t days);
        static std::vector<Price>       DateSerie(const char* start_date, const char* stop_date, Date::RANGE range = Date::RANGE::DAY, const std::vector<Price>& block = std::vector<Price>());
        static std::vector<Price>       DayToMonth(const std::vector<Price>& in);
        static std::vector<Price>       DayToWeek(const std::vector<Price>& in, Date::DAY weekday);
        static std::vector<Price>       ExponentialMovingAverage(const std::vector<Price>& in, std::size_t days);
        static std::vector<Price>       Momentum(const std::vector<Price>& in, std::size_t days);
        static std::vector<Price>       MovingAverage(const std::vector<Price>& in, std::size_t days);
        static void                     Print(const std::vector<Price>& in);
        static std::vector<Price>       RSI(const std::vector<Price>& in, std::size_t days);
        static std::vector<Price>       StdDev(const std::vector<Price>& in, std::size_t days);
        static std::vector<Price>       Stochastics(const std::vector<Price>& in, std::size_t days);
    };
}

// MKALGAM_OFF

#endif // FLW_PRICE_H
