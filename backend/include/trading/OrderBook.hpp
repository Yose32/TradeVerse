#ifndef TRADEVERSE_ORDER_BOOK_HPP
#define TRADEVERSE_ORDER_BOOK_HPP

#include <string>
#include <vector>
#include <mutex>
#include <map>
#include <algorithm>
#include "../dsa/MaxHeap.hpp"
#include "../dsa/MinHeap.hpp"
#include "../models/Order.hpp"

namespace tradeverse {
namespace trading {

struct PriceLevel {
    double price;
    int totalQuantity;
    int orderCount;
};

struct OrderBookDepth {
    std::string symbol;
    double lastPrice;
    double bestBid;
    double bestAsk;
    double spread;
    std::vector<PriceLevel> bids; // Sorted highest price to lowest
    std::vector<PriceLevel> asks; // Sorted lowest price to highest
};

/**
 * @brief Order Book for a specific stock symbol.
 * 
 * Uses:
 * - MaxHeap for Buy Orders (highest price, earliest timestamp first)
 * - MinHeap for Sell Orders (lowest price, earliest timestamp first)
 */
class OrderBook {
private:
    std::string symbol;
    mutable std::mutex bookMutex;
    dsa::MaxHeap<models::Order, models::BuyOrderComparator> buyHeap;
    dsa::MinHeap<models::Order, models::SellOrderComparator> sellHeap;
    double lastTradedPrice;

public:
    explicit OrderBook(const std::string& sym, double initialPrice = 100.0)
        : symbol(sym), lastTradedPrice(initialPrice) {}

    const std::string& getSymbol() const { return symbol; }
    double getLastPrice() const { return lastTradedPrice; }
    void setLastPrice(double price) { lastTradedPrice = price; }

    void addBuyOrder(const models::Order& order) {
        std::lock_guard<std::mutex> lock(bookMutex);
        buyHeap.push(order);
    }

    void addSellOrder(const models::Order& order) {
        std::lock_guard<std::mutex> lock(bookMutex);
        sellHeap.push(order);
    }

    bool hasBuyOrders() const {
        std::lock_guard<std::mutex> lock(bookMutex);
        return !buyHeap.empty();
    }

    bool hasSellOrders() const {
        std::lock_guard<std::mutex> lock(bookMutex);
        return !sellHeap.empty();
    }

    models::Order peekBestBuy() const {
        std::lock_guard<std::mutex> lock(bookMutex);
        return buyHeap.top();
    }

    models::Order peekBestSell() const {
        std::lock_guard<std::mutex> lock(bookMutex);
        return sellHeap.top();
    }

    models::Order popBestBuy() {
        std::lock_guard<std::mutex> lock(bookMutex);
        return buyHeap.pop();
    }

    models::Order popBestSell() {
        std::lock_guard<std::mutex> lock(bookMutex);
        return sellHeap.pop();
    }

    bool cancelOrder(const std::string& orderId) {
        std::lock_guard<std::mutex> lock(bookMutex);
        bool removed = buyHeap.removeIf([&](const models::Order& o) {
            return o.orderId == orderId;
        });
        if (removed) return true;

        return sellHeap.removeIf([&](const models::Order& o) {
            return o.orderId == orderId;
        });
    }

    OrderBookDepth getDepth(size_t maxLevels = 10) const {
        std::lock_guard<std::mutex> lock(bookMutex);
        OrderBookDepth depth;
        depth.symbol = symbol;
        depth.lastPrice = lastTradedPrice;
        depth.bestBid = buyHeap.empty() ? 0.0 : buyHeap.top().price;
        depth.bestAsk = sellHeap.empty() ? 0.0 : sellHeap.top().price;
        depth.spread = (depth.bestBid > 0 && depth.bestAsk > 0) ? (depth.bestAsk - depth.bestBid) : 0.0;

        // Aggregate bids by price level
        auto sortedBids = buyHeap.toSortedVector();
        std::map<double, PriceLevel, std::greater<double>> bidMap;
        for (const auto& o : sortedBids) {
            if (o.remainingQuantity() <= 0) continue;
            auto& level = bidMap[o.price];
            level.price = o.price;
            level.totalQuantity += o.remainingQuantity();
            level.orderCount += 1;
        }

        for (const auto& pair : bidMap) {
            if (depth.bids.size() >= maxLevels) break;
            depth.bids.push_back(pair.second);
        }

        // Aggregate asks by price level
        auto sortedAsks = sellHeap.toSortedVector();
        std::map<double, PriceLevel, std::less<double>> askMap;
        for (const auto& o : sortedAsks) {
            if (o.remainingQuantity() <= 0) continue;
            auto& level = askMap[o.price];
            level.price = o.price;
            level.totalQuantity += o.remainingQuantity();
            level.orderCount += 1;
        }

        for (const auto& pair : askMap) {
            if (depth.asks.size() >= maxLevels) break;
            depth.asks.push_back(pair.second);
        }

        return depth;
    }

    std::vector<models::Order> getAllOrders() const {
        std::lock_guard<std::mutex> lock(bookMutex);
        std::vector<models::Order> all;
        auto bids = buyHeap.rawData();
        auto asks = sellHeap.rawData();
        all.insert(all.end(), bids.begin(), bids.end());
        all.insert(all.end(), asks.begin(), asks.end());
        return all;
    }
};

} // namespace trading
} // namespace tradeverse

#endif // TRADEVERSE_ORDER_BOOK_HPP
