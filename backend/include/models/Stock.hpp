#ifndef TRADEVERSE_STOCK_HPP
#define TRADEVERSE_STOCK_HPP

#include <string>
#include <vector>
#include <cstdint>

namespace tradeverse {
namespace models {

struct Candle {
    int64_t timestamp;
    double open;
    double high;
    double low;
    double close;
    int64_t volume;
};

/**
 * @brief Represents a tradable stock asset in the simulated market.
 */
struct Stock {
    std::string symbol;
    std::string companyName;
    std::string sector;
    double currentPrice;
    double openPrice;
    double highPrice;
    double lowPrice;
    double previousClose;
    int64_t volume;
    double marketCap;
    double volatility; // annualized volatility parameter for GBM
    double drift;      // expected return drift parameter

    double change() const {
        return currentPrice - previousClose;
    }

    double changePercent() const {
        if (previousClose == 0.0) return 0.0;
        return ((currentPrice - previousClose) / previousClose) * 100.0;
    }
};

} // namespace models
} // namespace tradeverse

#endif // TRADEVERSE_STOCK_HPP
