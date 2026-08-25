#ifndef TRADEVERSE_PORTFOLIO_HPP
#define TRADEVERSE_PORTFOLIO_HPP

#include <string>
#include <vector>
#include <unordered_map>

namespace tradeverse {
namespace models {

struct Holding {
    std::string symbol;
    int quantity;
    double averageBuyPrice;
    double currentPrice;

    double investedValue() const {
        return quantity * averageBuyPrice;
    }

    double marketValue() const {
        return quantity * currentPrice;
    }

    double unrealizedPnL() const {
        return marketValue() - investedValue();
    }

    double pnlPercent() const {
        if (investedValue() == 0.0) return 0.0;
        return (unrealizedPnL() / investedValue()) * 100.0;
    }
};

/**
 * @brief User portfolio snapshot and holdings summary.
 */
struct Portfolio {
    std::string userId;
    double cashBalance;
    std::unordered_map<std::string, Holding> holdings;
    double realizedPnL;

    double totalInvestedValue() const {
        double sum = 0.0;
        for (const auto& pair : holdings) {
            if (pair.second.quantity > 0) {
                sum += pair.second.investedValue();
            }
        }
        return sum;
    }

    double totalMarketValue() const {
        double sum = 0.0;
        for (const auto& pair : holdings) {
            if (pair.second.quantity > 0) {
                sum += pair.second.marketValue();
            }
        }
        return sum;
    }

    double totalPortfolioValue() const {
        return cashBalance + totalMarketValue();
    }

    double totalUnrealizedPnL() const {
        return totalMarketValue() - totalInvestedValue();
    }

    double totalPnL() const {
        return totalPortfolioValue() - 1000000.0; // Benchmark against starting ₹10,00,000 capital
    }

    double returnPercent() const {
        return (totalPnL() / 1000000.0) * 100.0;
    }
};

} // namespace models
} // namespace tradeverse

#endif // TRADEVERSE_PORTFOLIO_HPP
