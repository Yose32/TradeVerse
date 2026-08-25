#ifndef TRADEVERSE_PORTFOLIO_MANAGER_HPP
#define TRADEVERSE_PORTFOLIO_MANAGER_HPP

#include <string>
#include <unordered_map>
#include <mutex>
#include <vector>
#include <stdexcept>
#include "../models/Portfolio.hpp"
#include "../models/Trade.hpp"

namespace tradeverse {
namespace trading {

/**
 * @brief Manages virtual cash balances, asset holdings, and real-time PnL accounting.
 */
class PortfolioManager {
private:
    std::unordered_map<std::string, models::Portfolio> userPortfolios;
    std::mutex portfolioMutex;

public:
    PortfolioManager() = default;

    void initializeUser(const std::string& userId, double startingBalance = 1000000.0) {
        std::lock_guard<std::mutex> lock(portfolioMutex);
        if (userPortfolios.find(userId) == userPortfolios.end()) {
            models::Portfolio p;
            p.userId = userId;
            p.cashBalance = startingBalance;
            p.realizedPnL = 0.0;
            userPortfolios[userId] = p;
        }
    }

    bool canAffordBuy(const std::string& userId, double estimatedTotal) {
        std::lock_guard<std::mutex> lock(portfolioMutex);
        auto it = userPortfolios.find(userId);
        if (it == userPortfolios.end()) return false;
        return it->second.cashBalance >= estimatedTotal;
    }

    bool hasEnoughShares(const std::string& userId, const std::string& symbol, int quantity) {
        std::lock_guard<std::mutex> lock(portfolioMutex);
        auto it = userPortfolios.find(userId);
        if (it == userPortfolios.end()) return false;
        auto hIt = it->second.holdings.find(symbol);
        if (hIt == it->second.holdings.end()) return false;
        return hIt->second.quantity >= quantity;
    }

    void applyTrade(const models::Trade& trade) {
        std::lock_guard<std::mutex> lock(portfolioMutex);
        double tradeValue = trade.quantity * trade.price;

        // Update Buyer Portfolio
        if (!trade.buyerId.empty()) {
            auto& buyerPort = userPortfolios[trade.buyerId];
            if (buyerPort.userId.empty()) {
                buyerPort.userId = trade.buyerId;
                buyerPort.cashBalance = 1000000.0;
            }
            buyerPort.cashBalance -= tradeValue;

            auto& h = buyerPort.holdings[trade.symbol];
            h.symbol = trade.symbol;
            double currentTotalCost = h.quantity * h.averageBuyPrice;
            h.quantity += trade.quantity;
            h.averageBuyPrice = (currentTotalCost + tradeValue) / h.quantity;
            h.currentPrice = trade.price;
        }

        // Update Seller Portfolio
        if (!trade.sellerId.empty()) {
            auto& sellerPort = userPortfolios[trade.sellerId];
            if (sellerPort.userId.empty()) {
                sellerPort.userId = trade.sellerId;
                sellerPort.cashBalance = 1000000.0;
            }
            sellerPort.cashBalance += tradeValue;

            auto& h = sellerPort.holdings[trade.symbol];
            h.symbol = trade.symbol;
            double costOfSoldShares = trade.quantity * h.averageBuyPrice;
            sellerPort.realizedPnL += (tradeValue - costOfSoldShares);
            h.quantity -= trade.quantity;
            h.currentPrice = trade.price;
        }
    }

    void updateMarketPrices(const std::unordered_map<std::string, double>& currentPrices) {
        std::lock_guard<std::mutex> lock(portfolioMutex);
        for (auto& userPair : userPortfolios) {
            for (auto& hPair : userPair.second.holdings) {
                auto priceIt = currentPrices.find(hPair.first);
                if (priceIt != currentPrices.end()) {
                    hPair.second.currentPrice = priceIt->second;
                }
            }
        }
    }

    models::Portfolio getPortfolio(const std::string& userId) {
        std::lock_guard<std::mutex> lock(portfolioMutex);
        auto it = userPortfolios.find(userId);
        if (it != userPortfolios.end()) {
            return it->second;
        }
        models::Portfolio emptyPort;
        emptyPort.userId = userId;
        emptyPort.cashBalance = 1000000.0;
        emptyPort.realizedPnL = 0.0;
        return emptyPort;
    }

    std::vector<models::Portfolio> getAllPortfolios() {
        std::lock_guard<std::mutex> lock(portfolioMutex);
        std::vector<models::Portfolio> all;
        for (const auto& pair : userPortfolios) {
            all.push_back(pair.second);
        }
        return all;
    }
};

} // namespace trading
} // namespace tradeverse

#endif // TRADEVERSE_PORTFOLIO_MANAGER_HPP
