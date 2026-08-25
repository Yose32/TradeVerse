#include <iostream>
#include <cassert>
#include <string>
#include <vector>
#include <cmath>

#include "../include/trading/MatchingEngine.hpp"
#include "../include/trading/PortfolioManager.hpp"

using namespace tradeverse;
using namespace tradeverse::trading;
using namespace tradeverse::models;

void testMatchingEngine() {
    std::cout << "[TEST] Running Matching Engine & Price-Time Priority tests..." << std::endl;
    MatchingEngine engine;
    PortfolioManager portfolio;

    portfolio.initializeUser("user_buyer", 1000000.0);
    portfolio.initializeUser("user_seller_1", 1000000.0);
    portfolio.initializeUser("user_seller_2", 1000000.0);

    // Initial state: Seed sell orders in the book
    // Seller 1: Sells 100 AAPL @ 250
    Order s1{"ORD-S1", "user_seller_1", "AAPL", OrderType::LIMIT, OrderSide::SELL, 100, 0, 250.0, OrderStatus::PENDING, 1000};
    auto r1 = engine.processOrder(s1);
    assert(r1.executedTrades.empty());
    assert(r1.incomingOrder.remainingQuantity() == 100);

    // Seller 2: Sells 50 AAPL @ 248 (better price!)
    Order s2{"ORD-S2", "user_seller_2", "AAPL", OrderType::LIMIT, OrderSide::SELL, 50, 0, 248.0, OrderStatus::PENDING, 1005};
    auto r2 = engine.processOrder(s2);
    assert(r2.executedTrades.empty());

    // Buyer arrives: Wants to BUY 120 AAPL @ 250
    // Expected behavior:
    // 1. Matches 50 shares @ 248 from Seller 2 (better price executed first!)
    // 2. Matches 70 shares @ 250 from Seller 1 (partial fill on S1)
    // 3. Buyer is fully filled (120/120). S1 has 30 shares remaining @ 250 in the book.
    Order b1{"ORD-B1", "user_buyer", "AAPL", OrderType::LIMIT, OrderSide::BUY, 120, 0, 250.0, OrderStatus::PENDING, 1010};
    auto r3 = engine.processOrder(b1);

    assert(r3.executedTrades.size() == 2);
    assert(r3.incomingOrder.status == OrderStatus::FILLED);
    assert(r3.incomingOrder.filledQuantity == 120);

    // Trade 1: 50 @ 248
    assert(r3.executedTrades[0].quantity == 50);
    assert(std::abs(r3.executedTrades[0].price - 248.0) < 1e-6);
    assert(r3.executedTrades[0].sellerId == "user_seller_2");

    // Trade 2: 70 @ 250
    assert(r3.executedTrades[1].quantity == 70);
    assert(std::abs(r3.executedTrades[1].price - 250.0) < 1e-6);
    assert(r3.executedTrades[1].sellerId == "user_seller_1");

    // Apply trades to portfolios
    for (const auto& trd : r3.executedTrades) {
        portfolio.applyTrade(trd);
    }

    auto buyerPort = portfolio.getPortfolio("user_buyer");
    assert(buyerPort.holdings["AAPL"].quantity == 120);
    // Average price: (50 * 248 + 70 * 250) / 120 = (12400 + 17500) / 120 = 29900 / 120 = 249.16666
    double expectedAvg = 29900.0 / 120.0;
    assert(std::abs(buyerPort.holdings["AAPL"].averageBuyPrice - expectedAvg) < 1e-4);

    // Verify remaining depth in the book:
    // Should have 30 shares remaining on S1 @ 250
    auto depth = engine.getDepth("AAPL");
    assert(depth.asks.size() == 1);
    assert(depth.asks[0].price == 250.0);
    assert(depth.asks[0].totalQuantity == 30);

    // Test cancellation of remaining S1 order
    bool cancelled = engine.cancelOrder("AAPL", "ORD-S1");
    assert(cancelled);
    auto depthAfterCancel = engine.getDepth("AAPL");
    assert(depthAfterCancel.asks.empty());

    std::cout << "[PASS] Matching Engine & Price-Time Priority tests passed successfully!" << std::endl;
}

int main() {
    testMatchingEngine();
    return 0;
}
