#ifndef TRADEVERSE_ORDER_HPP
#define TRADEVERSE_ORDER_HPP

#include <string>
#include <cstdint>
#include <iomanip>
#include <sstream>

namespace tradeverse {
namespace models {

enum class OrderType {
    MARKET,
    LIMIT
};

enum class OrderSide {
    BUY,
    SELL
};

enum class OrderStatus {
    PENDING,
    PARTIALLY_FILLED,
    FILLED,
    CANCELLED,
    REJECTED
};

inline std::string orderTypeToString(OrderType type) {
    return (type == OrderType::MARKET) ? "MARKET" : "LIMIT";
}

inline OrderType stringToOrderType(const std::string& str) {
    return (str == "MARKET") ? OrderType::MARKET : OrderType::LIMIT;
}

inline std::string orderSideToString(OrderSide side) {
    return (side == OrderSide::BUY) ? "BUY" : "SELL";
}

inline OrderSide stringToOrderSide(const std::string& str) {
    return (str == "BUY") ? OrderSide::BUY : OrderSide::SELL;
}

inline std::string orderStatusToString(OrderStatus status) {
    switch (status) {
        case OrderStatus::PENDING: return "PENDING";
        case OrderStatus::PARTIALLY_FILLED: return "PARTIALLY_FILLED";
        case OrderStatus::FILLED: return "FILLED";
        case OrderStatus::CANCELLED: return "CANCELLED";
        case OrderStatus::REJECTED: return "REJECTED";
    }
    return "UNKNOWN";
}

inline OrderStatus stringToOrderStatus(const std::string& str) {
    if (str == "PENDING") return OrderStatus::PENDING;
    if (str == "PARTIALLY_FILLED") return OrderStatus::PARTIALLY_FILLED;
    if (str == "FILLED") return OrderStatus::FILLED;
    if (str == "CANCELLED") return OrderStatus::CANCELLED;
    return OrderStatus::REJECTED;
}

/**
 * @brief Represents a trading order placed by a user or simulation bot.
 */
struct Order {
    std::string orderId;
    std::string userId;
    std::string symbol;
    OrderType type;
    OrderSide side;
    int quantity;
    int filledQuantity;
    double price; // Limit price (for market orders, 0 or best execution)
    OrderStatus status;
    int64_t timestamp; // Epoch milliseconds for price-time priority

    int remainingQuantity() const {
        return quantity - filledQuantity;
    }

    bool isFilled() const {
        return filledQuantity >= quantity;
    }
};

/**
 * @brief Comparator for Buy orders in MaxHeap.
 * Best (highest) bid price gets highest priority.
 * Ties are broken by earlier timestamp (FIFO).
 */
struct BuyOrderComparator {
    bool operator()(const Order& a, const Order& b) const {
        if (a.price != b.price) {
            return a.price < b.price; // a has lower priority than b if a.price < b.price
        }
        return a.timestamp > b.timestamp; // earlier timestamp has higher priority
    }
};

/**
 * @brief Comparator for Sell orders in MinHeap.
 * Best (lowest) ask price gets highest priority.
 * Ties are broken by earlier timestamp (FIFO).
 */
struct SellOrderComparator {
    bool operator()(const Order& a, const Order& b) const {
        if (a.price != b.price) {
            return a.price > b.price; // a has lower priority than b if a.price > b.price
        }
        return a.timestamp > b.timestamp; // earlier timestamp has higher priority
    }
};

} // namespace models
} // namespace tradeverse

#endif // TRADEVERSE_ORDER_HPP
