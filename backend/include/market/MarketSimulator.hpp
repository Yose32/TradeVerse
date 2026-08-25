#ifndef TRADEVERSE_MARKET_SIMULATOR_HPP
#define TRADEVERSE_MARKET_SIMULATOR_HPP

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <mutex>
#include <random>
#include <cmath>
#include <chrono>

#include "EventEngine.hpp"
#include "../trading/MatchingEngine.hpp"
#include "../models/Stock.hpp"

namespace tradeverse {
namespace market {

enum class MarketRegime {
    BULL,
    BEAR,
    SIDEWAYS,
    HIGH_VOLATILITY,
    LOW_VOLATILITY
};

inline std::string regimeToString(MarketRegime r) {
    switch (r) {
        case MarketRegime::BULL: return "BULL";
        case MarketRegime::BEAR: return "BEAR";
        case MarketRegime::SIDEWAYS: return "SIDEWAYS";
        case MarketRegime::HIGH_VOLATILITY: return "HIGH_VOLATILITY";
        case MarketRegime::LOW_VOLATILITY: return "LOW_VOLATILITY";
    }
    return "SIDEWAYS";
}

inline MarketRegime stringToRegime(const std::string& str) {
    if (str == "BULL") return MarketRegime::BULL;
    if (str == "BEAR") return MarketRegime::BEAR;
    if (str == "HIGH_VOLATILITY") return MarketRegime::HIGH_VOLATILITY;
    if (str == "LOW_VOLATILITY") return MarketRegime::LOW_VOLATILITY;
    return MarketRegime::SIDEWAYS;
}

/**
 * @brief Realistic Market Simulator generating Stochastic Price Movements (GBM)
 * and Market Maker Liquidity into the C++ Matching Engine.
 */
class MarketSimulator {
private:
    std::unordered_map<std::string, models::Stock> stocks;
    std::unordered_map<std::string, std::vector<models::Candle>> stockCandles;
    std::shared_ptr<trading::MatchingEngine> matchingEngine;
    std::shared_ptr<EventEngine> eventEngine;
    MarketRegime currentRegime;
    bool running;
    std::mutex simMutex;

    std::mt19937 rng;
    std::normal_distribution<double> normalDist;
    int64_t botOrderIdCounter;

    int64_t getCurrentTimeMs() const {
        return std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()
        ).count();
    }

public:
    MarketSimulator(std::shared_ptr<trading::MatchingEngine> engine,
                    std::shared_ptr<EventEngine> events)
        : matchingEngine(engine), eventEngine(events), currentRegime(MarketRegime::BULL),
          running(false), rng(1337), normalDist(0.0, 1.0), botOrderIdCounter(50000) {
        initializeStocks();
    }

    void initializeStocks() {
        std::vector<models::Stock> initialStocks = {
            {"AAPL", "Apple Inc.", "Consumer Electronics", 232.50, 230.00, 234.00, 229.50, 230.00, 48200000, 3550000000000.0, 0.015, 0.0004},
            {"MSFT", "Microsoft Corp.", "Cloud & Software", 448.20, 445.00, 450.50, 444.00, 445.00, 21500000, 3320000000000.0, 0.014, 0.0005},
            {"NVDA", "NVIDIA Corp.", "Semiconductors", 128.40, 124.00, 131.20, 123.50, 124.00, 89400000, 3150000000000.0, 0.025, 0.0008},
            {"TSLA", "Tesla Inc.", "Automotive & CleanTech", 225.80, 220.00, 228.00, 218.50, 220.00, 52100000, 720000000000.0, 0.030, 0.0002},
            {"AMZN", "Amazon.com Inc.", "E-Commerce", 186.40, 185.00, 188.20, 184.10, 185.00, 34200000, 1940000000000.0, 0.018, 0.0004},
            {"GOOGL", "Alphabet Inc.", "Cloud & Software", 178.60, 177.00, 180.50, 176.20, 177.00, 24800000, 2210000000000.0, 0.016, 0.0003},
            {"META", "Meta Platforms Inc.", "Technology", 512.30, 508.00, 516.00, 506.00, 508.00, 16800000, 1300000000000.0, 0.022, 0.0006},
            {"AMD", "Advanced Micro Devices", "Semiconductors", 154.20, 151.00, 156.80, 150.20, 151.00, 38600000, 250000000000.0, 0.028, 0.0005},
            {"NFLX", "Netflix Inc.", "Entertainment", 685.50, 680.00, 692.00, 678.00, 680.00, 8400000, 295000000000.0, 0.020, 0.0004},
            {"INTC", "Intel Corp.", "Semiconductors", 21.80, 22.20, 22.50, 21.40, 22.20, 46200000, 93000000000.0, 0.024, -0.0002}
        };

        int64_t now = getCurrentTimeMs();
        for (const auto& s : initialStocks) {
            stocks[s.symbol] = s;
            matchingEngine->getOrCreateBook(s.symbol, s.currentPrice);

            // Generate initial historical candles (past 30 bars)
            std::vector<models::Candle> candles;
            double p = s.previousClose * 0.95;
            for (int i = 30; i >= 1; --i) {
                int64_t barTime = now - (i * 60 * 1000); // 1 minute bars
                double open = p;
                double change = (normalDist(rng) * 0.008) * p;
                double close = std::max(1.0, open + change);
                double high = std::max(open, close) + std::abs(normalDist(rng) * 0.004 * p);
                double low = std::min(open, close) - std::abs(normalDist(rng) * 0.004 * p);
                int64_t vol = 10000 + static_cast<int64_t>(std::abs(normalDist(rng)) * 25000);
                candles.push_back({barTime, open, high, low, close, vol});
                p = close;
            }
            stockCandles[s.symbol] = candles;

            // Seed initial market maker liquidity orders
            seedLiquidity(s.symbol, s.currentPrice);
        }
    }

    void setRegime(MarketRegime regime) {
        std::lock_guard<std::mutex> lock(simMutex);
        currentRegime = regime;
    }

    MarketRegime getRegime() const {
        return currentRegime;
    }

    void seedLiquidity(const std::string& symbol, double centerPrice) {
        // Place 5 bid levels below centerPrice and 5 ask levels above centerPrice
        int64_t now = getCurrentTimeMs();
        for (int i = 1; i <= 5; ++i) {
            double bidPrice = std::round((centerPrice * (1.0 - 0.002 * i)) * 100.0) / 100.0;
            double askPrice = std::round((centerPrice * (1.0 + 0.002 * i)) * 100.0) / 100.0;
            int qty = 50 * i + (std::rand() % 50);

            models::Order bid{
                "BOT-B-" + std::to_string(++botOrderIdCounter),
                "market_maker_bot",
                symbol,
                models::OrderType::LIMIT,
                models::OrderSide::BUY,
                qty, 0, bidPrice, models::OrderStatus::PENDING, now - (100 * i)
            };
            models::Order ask{
                "BOT-S-" + std::to_string(++botOrderIdCounter),
                "market_maker_bot",
                symbol,
                models::OrderType::LIMIT,
                models::OrderSide::SELL,
                qty, 0, askPrice, models::OrderStatus::PENDING, now - (100 * i)
            };
            matchingEngine->processOrder(bid);
            matchingEngine->processOrder(ask);
        }
    }

    /**
     * @brief Performs one step of market simulation for all stocks.
     */
    void tick() {
        std::lock_guard<std::mutex> lock(simMutex);
        int64_t now = getCurrentTimeMs();
        auto shockDrifts = eventEngine->calculateTotalShockDrifts();

        double regimeDrift = 0.0;
        double regimeVolMultiplier = 1.0;

        switch (currentRegime) {
            case MarketRegime::BULL:
                regimeDrift = 0.0008;
                regimeVolMultiplier = 1.0;
                break;
            case MarketRegime::BEAR:
                regimeDrift = -0.0008;
                regimeVolMultiplier = 1.4;
                break;
            case MarketRegime::SIDEWAYS:
                regimeDrift = 0.0;
                regimeVolMultiplier = 0.6;
                break;
            case MarketRegime::HIGH_VOLATILITY:
                regimeDrift = 0.0;
                regimeVolMultiplier = 2.5;
                break;
            case MarketRegime::LOW_VOLATILITY:
                regimeDrift = 0.0;
                regimeVolMultiplier = 0.3;
                break;
        }

        for (auto& pair : stocks) {
            models::Stock& stock = pair.second;
            double shock = 0.0;
            auto shockIt = shockDrifts.find(stock.symbol);
            if (shockIt != shockDrifts.end()) {
                shock = shockIt->second * 0.05; // apply scaled shock impact per tick
            }

            double dt = 1.0 / 252.0; // daily fraction
            double vol = stock.volatility * regimeVolMultiplier;
            double drift = stock.drift + regimeDrift + shock;
            double z = normalDist(rng);

            // Geometric Brownian Motion step
            double returnStep = (drift - 0.5 * vol * vol) * dt + vol * std::sqrt(dt) * z;
            double newPrice = stock.currentPrice * std::exp(returnStep);
            newPrice = std::max(1.0, std::round(newPrice * 100.0) / 100.0);

            stock.currentPrice = newPrice;
            stock.highPrice = std::max(stock.highPrice, newPrice);
            stock.lowPrice = std::min(stock.lowPrice, newPrice);
            stock.volume += static_cast<int64_t>(std::abs(z) * 1200 + 100);

            // Append or update current 1-min candle
            auto& candles = stockCandles[stock.symbol];
            if (candles.empty() || (now - candles.back().timestamp > 60000)) {
                candles.push_back({now, newPrice, newPrice, newPrice, newPrice, 500});
                if (candles.size() > 200) candles.erase(candles.begin());
            } else {
                auto& lastCandle = candles.back();
                lastCandle.high = std::max(lastCandle.high, newPrice);
                lastCandle.low = std::min(lastCandle.low, newPrice);
                lastCandle.close = newPrice;
                lastCandle.volume += 50;
            }

            // Keep order book liquid around newPrice
            seedLiquidity(stock.symbol, newPrice);
        }
    }

    std::vector<models::Stock> getAllStocks() {
        std::lock_guard<std::mutex> lock(simMutex);
        std::vector<models::Stock> result;
        for (const auto& pair : stocks) {
            result.push_back(pair.second);
        }
        return result;
    }

    models::Stock* getStock(const std::string& symbol) {
        std::lock_guard<std::mutex> lock(simMutex);
        auto it = stocks.find(symbol);
        if (it != stocks.end()) {
            return &(it->second);
        }
        return nullptr;
    }

    std::vector<models::Candle> getCandles(const std::string& symbol) {
        std::lock_guard<std::mutex> lock(simMutex);
        auto it = stockCandles.find(symbol);
        if (it != stockCandles.end()) {
            return it->second;
        }
        return {};
    }

    std::unordered_map<std::string, double> getCurrentPriceMap() {
        std::lock_guard<std::mutex> lock(simMutex);
        std::unordered_map<std::string, double> map;
        for (const auto& pair : stocks) {
            map[pair.first] = pair.second.currentPrice;
        }
        return map;
    }
};

} // namespace market
} // namespace tradeverse

#endif // TRADEVERSE_MARKET_SIMULATOR_HPP
