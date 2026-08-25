#ifndef TRADEVERSE_MARKET_EVENT_HPP
#define TRADEVERSE_MARKET_EVENT_HPP

#include <string>
#include <vector>
#include <cstdint>

namespace tradeverse {
namespace models {

/**
 * @brief Represents a macro or sector-specific market event that shocks stock prices.
 */
struct MarketEvent {
    std::string eventId;
    std::string title;
    std::string description;
    std::string epicentreNode; // Sector or Stock symbol (e.g. "Semiconductors" or "NVDA")
    double baseImpact;        // e.g. -0.08 (-8%) or +0.10 (+10%)
    int64_t timestamp;
    std::vector<std::string> affectedSymbols;
};

} // namespace models
} // namespace tradeverse

#endif // TRADEVERSE_MARKET_EVENT_HPP
