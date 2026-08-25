#ifndef TRADEVERSE_TRADE_HPP
#define TRADEVERSE_TRADE_HPP

#include <string>
#include <cstdint>

namespace tradeverse {
namespace models {

/**
 * @brief Represents an executed trade between a buyer and seller.
 */
struct Trade {
    std::string tradeId;
    std::string buyOrderId;
    std::string sellOrderId;
    std::string symbol;
    int quantity;
    double price;
    std::string buyerId;
    std::string sellerId;
    int64_t timestamp; // Epoch milliseconds
    double totalValue() const { return quantity * price; }
};

} // namespace models
} // namespace tradeverse

#endif // TRADEVERSE_TRADE_HPP
