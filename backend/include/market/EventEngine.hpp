#ifndef TRADEVERSE_EVENT_ENGINE_HPP
#define TRADEVERSE_EVENT_ENGINE_HPP

#include <string>
#include <vector>
#include <unordered_map>
#include <mutex>
#include <chrono>

#include "../dsa/Graph.hpp"
#include "../models/MarketEvent.hpp"

namespace tradeverse {
namespace market {

/**
 * @brief Market Event & Shockwave Propagation Engine.
 * 
 * Powered by Custom Graph and Dijkstra's Shortest Path Algorithm.
 */
class EventEngine {
private:
    dsa::Graph marketGraph;
    std::vector<models::MarketEvent> activeEvents;
    std::vector<models::MarketEvent> eventHistory;
    std::mutex eventMutex;
    int64_t eventIdCounter;

    int64_t getCurrentTimeMs() const {
        return std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()
        ).count();
    }

public:
    EventEngine() : eventIdCounter(100) {
        buildMarketGraph();
    }

    void buildMarketGraph() {
        // Sectors
        marketGraph.addVertex("Technology");
        marketGraph.addVertex("Semiconductors");
        marketGraph.addVertex("Cloud & Software");
        marketGraph.addVertex("Consumer Electronics");
        marketGraph.addVertex("Automotive & CleanTech");
        marketGraph.addVertex("E-Commerce");
        marketGraph.addVertex("Entertainment");

        // Sector <-> Stock edges with correlation weights
        marketGraph.addEdge("Semiconductors", "NVDA", 0.6, "primary_sector");
        marketGraph.addEdge("Semiconductors", "AMD", 0.7, "primary_sector");
        marketGraph.addEdge("Semiconductors", "INTC", 0.8, "primary_sector");

        marketGraph.addEdge("Consumer Electronics", "AAPL", 0.6, "primary_sector");
        marketGraph.addEdge("Cloud & Software", "MSFT", 0.6, "primary_sector");
        marketGraph.addEdge("Cloud & Software", "GOOGL", 0.7, "primary_sector");
        marketGraph.addEdge("Cloud & Software", "AMZN", 0.8, "aws_cloud");

        marketGraph.addEdge("Technology", "META", 0.7, "primary_sector");
        marketGraph.addEdge("Automotive & CleanTech", "TSLA", 0.6, "primary_sector");
        marketGraph.addEdge("Entertainment", "NFLX", 0.6, "primary_sector");

        // Inter-company supply-chain & sector linkages
        marketGraph.addBidirectionalEdge("Semiconductors", "Technology", 0.8, "hardware_foundation");
        marketGraph.addBidirectionalEdge("Semiconductors", "Consumer Electronics", 0.9, "chip_supply");
        marketGraph.addBidirectionalEdge("NVDA", "MSFT", 0.7, "ai_datacenter_partner");
        marketGraph.addBidirectionalEdge("NVDA", "GOOGL", 0.8, "tpu_gpu_competition");
        marketGraph.addBidirectionalEdge("AMD", "INTC", 0.5, "direct_rival");
        marketGraph.addBidirectionalEdge("AAPL", "TSLA", 1.4, "consumer_sentiment");
        marketGraph.addBidirectionalEdge("AMZN", "MSFT", 0.8, "cloud_rival");
    }

    const dsa::Graph& getGraph() const { return marketGraph; }

    /**
     * @brief Triggers a market event and computes shock impact on all stocks.
     */
    models::MarketEvent triggerEvent(const std::string& title, const std::string& description,
                                      const std::string& epicentre, double baseImpact) {
        std::lock_guard<std::mutex> lock(eventMutex);
        models::MarketEvent evt;
        evt.eventId = "EVT-" + std::to_string(++eventIdCounter);
        evt.title = title;
        evt.description = description;
        evt.epicentreNode = epicentre;
        evt.baseImpact = baseImpact;
        evt.timestamp = getCurrentTimeMs();

        auto impacts = marketGraph.propagateShock(epicentre, baseImpact);
        for (const auto& pair : impacts) {
            if (std::abs(pair.second) > 0.01) {
                evt.affectedSymbols.push_back(pair.first);
            }
        }

        activeEvents.push_back(evt);
        eventHistory.push_back(evt);
        return evt;
    }

    std::unordered_map<std::string, double> calculateTotalShockDrifts() {
        std::lock_guard<std::mutex> lock(eventMutex);
        std::unordered_map<std::string, double> cumulativeDrifts;

        for (const auto& evt : activeEvents) {
            auto impacts = marketGraph.propagateShock(evt.epicentreNode, evt.baseImpact);
            for (const auto& pair : impacts) {
                cumulativeDrifts[pair.first] += pair.second;
            }
        }
        return cumulativeDrifts;
    }

    void clearActiveEvents() {
        std::lock_guard<std::mutex> lock(eventMutex);
        activeEvents.clear();
    }

    std::vector<models::MarketEvent> getRecentEvents(size_t limit = 10) {
        std::lock_guard<std::mutex> lock(eventMutex);
        std::vector<models::MarketEvent> result;
        size_t start = (eventHistory.size() > limit) ? (eventHistory.size() - limit) : 0;
        for (size_t i = start; i < eventHistory.size(); ++i) {
            result.push_back(eventHistory[i]);
        }
        return result;
    }
};

} // namespace market
} // namespace tradeverse

#endif // TRADEVERSE_EVENT_ENGINE_HPP
