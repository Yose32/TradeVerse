#ifndef TRADEVERSE_MATCHING_ENGINE_HPP
#define TRADEVERSE_MATCHING_ENGINE_HPP

#include <string>
#include <vector>
#include <memory>
#include <mutex>
#include <unordered_map>
#include <chrono>
#include <iomanip>
#include <sstream>

#include "OrderBook.hpp"
#include "../models/Order.hpp"
#include "../models/Trade.hpp"

namespace tradeverse {
namespace trading {

struct MatchResult {
    models::Order incomingOrder;
    std::vector<models::Trade> executedTrades;
    std::vector<models::Order> modifiedBookOrders;
    std::vector<std::string> dsaTraceSteps;
    double lastTradedPrice;
    bool success;
    std::string errorMessage;
};

/**
 * @brief Price-Time Priority Matching Engine.
 * 
 * Core algorithm for executing orders against MaxHeap (Bids) and MinHeap (Asks).
 */
class MatchingEngine {
private:
    std::unordered_map<std::string, std::shared_ptr<OrderBook>> orderBooks;
    std::mutex engineMutex;
    int64_t tradeIdCounter;

    std::string generateTradeId() {
        return "TRD-" + std::to_string(++tradeIdCounter);
    }

    int64_t getCurrentTimeMs() const {
        return std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()
        ).count();
    }

public:
    MatchingEngine() : tradeIdCounter(1000) {}

    std::shared_ptr<OrderBook> getOrCreateBook(const std::string& symbol, double defaultPrice = 100.0) {
        std::lock_guard<std::mutex> lock(engineMutex);
        auto it = orderBooks.find(symbol);
        if (it != orderBooks.end()) {
            return it->second;
        }
        auto book = std::make_shared<OrderBook>(symbol, defaultPrice);
        orderBooks[symbol] = book;
        return book;
    }

    std::shared_ptr<OrderBook> getBook(const std::string& symbol) {
        std::lock_guard<std::mutex> lock(engineMutex);
        auto it = orderBooks.find(symbol);
        if (it != orderBooks.end()) {
            return it->second;
        }
        return nullptr;
    }

    /**
     * @brief Process an incoming order using Price-Time Priority matching.
     */
    MatchResult processOrder(models::Order incoming) {
        MatchResult result;
        result.incomingOrder = incoming;
        result.success = true;
        result.lastTradedPrice = 0.0;

        auto book = getOrCreateBook(incoming.symbol);
        int64_t now = getCurrentTimeMs();
        if (incoming.timestamp == 0) {
            incoming.timestamp = now;
            result.incomingOrder.timestamp = now;
        }

        std::ostringstream ss;
        ss << "[DSA] Ingested incoming " << models::orderSideToString(incoming.side)
           << " order for " << incoming.quantity << " " << incoming.symbol
           << " @ ₹" << std::fixed << std::setprecision(2) << incoming.price;
        result.dsaTraceSteps.push_back(ss.str());

        if (incoming.side == models::OrderSide::BUY) {
            // BUY ORDER: Match against MinHeap (sell book)
            while (incoming.remainingQuantity() > 0 && book->hasSellOrders()) {
                models::Order bestSell = book->peekBestSell();

                // Check price match condition
                if (incoming.type == models::OrderType::LIMIT && incoming.price < bestSell.price) {
                    std::ostringstream step;
                    step << "[DSA] Bid price ₹" << incoming.price << " < Best Ask ₹" << bestSell.price
                         << ". No immediate match possible.";
                    result.dsaTraceSteps.push_back(step.str());
                    break; // Spread cannot cross
                }

                // Match found! Pop from MinHeap
                book->popBestSell();
                int matchQty = std::min(incoming.remainingQuantity(), bestSell.remainingQuantity());
                double executionPrice = bestSell.price; // Maker's price

                incoming.filledQuantity += matchQty;
                bestSell.filledQuantity += matchQty;

                if (incoming.isFilled()) {
                    incoming.status = models::OrderStatus::FILLED;
                } else {
                    incoming.status = models::OrderStatus::PARTIALLY_FILLED;
                }

                if (bestSell.isFilled()) {
                    bestSell.status = models::OrderStatus::FILLED;
                } else {
                    bestSell.status = models::OrderStatus::PARTIALLY_FILLED;
                    // Push partially filled sell order back to MinHeap
                    book->addSellOrder(bestSell);
                }

                // Create executed trade
                models::Trade trade;
                trade.tradeId = generateTradeId();
                trade.buyOrderId = incoming.orderId;
                trade.sellOrderId = bestSell.orderId;
                trade.symbol = incoming.symbol;
                trade.quantity = matchQty;
                trade.price = executionPrice;
                trade.buyerId = incoming.userId;
                trade.sellerId = bestSell.userId;
                trade.timestamp = now;

                result.executedTrades.push_back(trade);
                result.modifiedBookOrders.push_back(bestSell);
                result.lastTradedPrice = executionPrice;
                book->setLastPrice(executionPrice);

                std::ostringstream matchLog;
                matchLog << "[DSA Match] Executed " << matchQty << " shares @ ₹" << executionPrice
                         << " (Buyer: " << incoming.userId << ", Seller: " << bestSell.userId << ")";
                result.dsaTraceSteps.push_back(matchLog.str());
            }

            // If remaining quantity exists for a LIMIT order, insert into MaxHeap
            if (incoming.remainingQuantity() > 0) {
                if (incoming.type == models::OrderType::LIMIT) {
                    book->addBuyOrder(incoming);
                    std::ostringstream restLog;
                    restLog << "[DSA MaxHeap] Inserted remaining " << incoming.remainingQuantity()
                            << " shares into Buy Max-Heap @ ₹" << incoming.price;
                    result.dsaTraceSteps.push_back(restLog.str());
                } else {
                    // Market order with unfilled remaining quantity gets cancelled / filled partially
                    std::ostringstream mktLog;
                    mktLog << "[DSA Market Order] Market depth exhausted. Filled "
                           << incoming.filledQuantity << "/" << incoming.quantity;
                    result.dsaTraceSteps.push_back(mktLog.str());
                }
            }
        } else {
            // SELL ORDER: Match against MaxHeap (buy book)
            while (incoming.remainingQuantity() > 0 && book->hasBuyOrders()) {
                models::Order bestBuy = book->peekBestBuy();

                // Check price match condition
                if (incoming.type == models::OrderType::LIMIT && incoming.price > bestBuy.price) {
                    std::ostringstream step;
                    step << "[DSA] Ask price ₹" << incoming.price << " > Best Bid ₹" << bestBuy.price
                         << ". No immediate match possible.";
                    result.dsaTraceSteps.push_back(step.str());
                    break;
                }

                // Match found! Pop from MaxHeap
                book->popBestBuy();
                int matchQty = std::min(incoming.remainingQuantity(), bestBuy.remainingQuantity());
                double executionPrice = bestBuy.price; // Maker's price

                incoming.filledQuantity += matchQty;
                bestBuy.filledQuantity += matchQty;

                if (incoming.isFilled()) {
                    incoming.status = models::OrderStatus::FILLED;
                } else {
                    incoming.status = models::OrderStatus::PARTIALLY_FILLED;
                }

                if (bestBuy.isFilled()) {
                    bestBuy.status = models::OrderStatus::FILLED;
                } else {
                    bestBuy.status = models::OrderStatus::PARTIALLY_FILLED;
                    // Push partially filled buy order back to MaxHeap
                    book->addBuyOrder(bestBuy);
                }

                // Create executed trade
                models::Trade trade;
                trade.tradeId = generateTradeId();
                trade.buyOrderId = bestBuy.orderId;
                trade.sellOrderId = incoming.orderId;
                trade.symbol = incoming.symbol;
                trade.quantity = matchQty;
                trade.price = executionPrice;
                trade.buyerId = bestBuy.userId;
                trade.sellerId = incoming.userId;
                trade.timestamp = now;

                result.executedTrades.push_back(trade);
                result.modifiedBookOrders.push_back(bestBuy);
                result.lastTradedPrice = executionPrice;
                book->setLastPrice(executionPrice);

                std::ostringstream matchLog;
                matchLog << "[DSA Match] Executed " << matchQty << " shares @ ₹" << executionPrice
                         << " (Buyer: " << bestBuy.userId << ", Seller: " << incoming.userId << ")";
                result.dsaTraceSteps.push_back(matchLog.str());
            }

            // If remaining quantity exists for a LIMIT order, insert into MinHeap
            if (incoming.remainingQuantity() > 0) {
                if (incoming.type == models::OrderType::LIMIT) {
                    book->addSellOrder(incoming);
                    std::ostringstream restLog;
                    restLog << "[DSA MinHeap] Inserted remaining " << incoming.remainingQuantity()
                            << " shares into Sell Min-Heap @ ₹" << incoming.price;
                    result.dsaTraceSteps.push_back(restLog.str());
                } else {
                    std::ostringstream mktLog;
                    mktLog << "[DSA Market Order] Market depth exhausted. Filled "
                           << incoming.filledQuantity << "/" << incoming.quantity;
                    result.dsaTraceSteps.push_back(mktLog.str());
                }
            }
        }

        result.incomingOrder = incoming;
        return result;
    }

    bool cancelOrder(const std::string& symbol, const std::string& orderId) {
        auto book = getBook(symbol);
        if (book) {
            return book->cancelOrder(orderId);
        }
        return false;
    }

    OrderBookDepth getDepth(const std::string& symbol, size_t maxLevels = 10) {
        auto book = getOrCreateBook(symbol);
        return book->getDepth(maxLevels);
    }
};

} // namespace trading
} // namespace tradeverse

#endif // TRADEVERSE_MATCHING_ENGINE_HPP
