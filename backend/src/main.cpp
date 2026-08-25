#include <iostream>
#include <memory>
#include <thread>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <algorithm>

#include "../include/dsa/MaxHeap.hpp"
#include "../include/dsa/MinHeap.hpp"
#include "../include/dsa/Queue.hpp"
#include "../include/dsa/Stack.hpp"
#include "../include/dsa/HashMap.hpp"
#include "../include/dsa/DoublyLinkedList.hpp"
#include "../include/dsa/Graph.hpp"
#include "../include/dsa/Sorting.hpp"
#include "../include/dsa/Searching.hpp"

#include "../include/models/User.hpp"
#include "../include/models/Order.hpp"
#include "../include/models/Trade.hpp"
#include "../include/models/Stock.hpp"
#include "../include/models/Portfolio.hpp"
#include "../include/models/MarketEvent.hpp"

#include "../include/auth/AuthManager.hpp"
#include "../include/trading/MatchingEngine.hpp"
#include "../include/trading/PortfolioManager.hpp"
#include "../include/market/EventEngine.hpp"
#include "../include/market/MarketSimulator.hpp"
#include "../include/database/Database.hpp"
#include "../include/api/HttpServer.hpp"
#include "../include/api/Json.hpp"

using namespace tradeverse;

int main() {
    std::cout << "==========================================================" << std::endl;
    std::cout << " TradeVerse -- Full-Stack C++ DSA Trading Simulator Engine " << std::endl;
    std::cout << "==========================================================" << std::endl;

    auto database = std::make_shared<database::Database>("tradeverse.db");
    database->open();
    database->initSchema();

    auto authManager = std::make_shared<auth::AuthManager>();
    auto matchingEngine = std::make_shared<trading::MatchingEngine>();
    auto portfolioManager = std::make_shared<trading::PortfolioManager>();
    auto eventEngine = std::make_shared<market::EventEngine>();
    auto marketSimulator = std::make_shared<market::MarketSimulator>(matchingEngine, eventEngine);

    for (const auto& u : authManager->getAllUsers()) {
        portfolioManager->initializeUser(u.userId, u.cashBalance);
    }

    {
        models::Trade t1{"TRD-INIT-1", "ORD-1", "ORD-2", "AAPL", 100, 220.0, "USR-1001", "market_maker", 1000};
        models::Trade t2{"TRD-INIT-2", "ORD-3", "ORD-4", "NVDA", 250, 115.0, "USR-1001", "market_maker", 1010};
        models::Trade t3{"TRD-INIT-3", "ORD-5", "ORD-6", "MSFT", 50, 430.0, "USR-1001", "market_maker", 1020};
        portfolioManager->applyTrade(t1);
        portfolioManager->applyTrade(t2);
        portfolioManager->applyTrade(t3);
        database->saveTrade(t1);
        database->saveTrade(t2);
        database->saveTrade(t3);

        models::Trade t4{"TRD-INIT-4", "ORD-7", "ORD-8", "NVDA", 800, 100.0, "USR-1003", "market_maker", 1000};
        portfolioManager->applyTrade(t4);

        models::Trade t5{"TRD-INIT-5", "ORD-9", "ORD-10", "META", 300, 480.0, "USR-1004", "market_maker", 1000};
        portfolioManager->applyTrade(t5);
    }

    api::HttpServer server(8080);

    auto authenticateRequest = [&](const api::HttpRequest& req, models::User& outUser) -> bool {
        if (req.authUserToken.empty()) return false;
        return authManager->validateToken(req.authUserToken, outUser);
    };

    server.addRoute("POST", "/api/auth/register", [&](const api::HttpRequest& req) {
        std::string username = json::extractString(req.body, "username");
        std::string email = json::extractString(req.body, "email");
        std::string fullName = json::extractString(req.body, "fullName");
        std::string password = json::extractString(req.body, "password");

        auto result = authManager->registerUser(username, email, fullName, password);
        if (!result.success) {
            return api::HttpResponse(400, "{\"success\": false, \"error\": \"" + result.errorMessage + "\"}");
        }

        portfolioManager->initializeUser(result.user.userId, 1000000.0);

        std::ostringstream ss;
        ss << "{\"success\": true, \"token\": \"" << result.token
           << "\", \"user\": {\"userId\": \"" << result.user.userId
           << "\", \"username\": \"" << result.user.username
           << "\", \"email\": \"" << result.user.email
           << "\", \"fullName\": \"" << result.user.fullName
           << "\", \"cashBalance\": " << result.user.cashBalance
           << ", \"isAdmin\": " << (result.user.isAdmin ? "true" : "false") << "}}";
        return api::HttpResponse(201, ss.str());
    });

    server.addRoute("POST", "/api/auth/login", [&](const api::HttpRequest& req) {
        std::string identifier = json::extractString(req.body, "username");
        if (identifier.empty()) identifier = json::extractString(req.body, "email");
        std::string password = json::extractString(req.body, "password");

        auto result = authManager->loginUser(identifier, password);
        if (!result.success) {
            return api::HttpResponse(401, "{\"success\": false, \"error\": \"" + result.errorMessage + "\"}");
        }

        portfolioManager->initializeUser(result.user.userId, result.user.cashBalance);

        std::ostringstream ss;
        ss << "{\"success\": true, \"token\": \"" << result.token
           << "\", \"user\": {\"userId\": \"" << result.user.userId
           << "\", \"username\": \"" << result.user.username
           << "\", \"email\": \"" << result.user.email
           << "\", \"fullName\": \"" << result.user.fullName
           << "\", \"cashBalance\": " << result.user.cashBalance
           << ", \"isAdmin\": " << (result.user.isAdmin ? "true" : "false") << "}}";
        return api::HttpResponse(200, ss.str());
    });

    server.addRoute("POST", "/api/auth/logout", [&](const api::HttpRequest& req) {
        if (!req.authUserToken.empty()) {
            authManager->logout(req.authUserToken);
        }
        return api::HttpResponse(200, "{\"success\": true, \"message\": \"Logged out successfully\"}");
    });

    server.addRoute("GET", "/api/user/profile", [&](const api::HttpRequest& req) {
        models::User user;
        if (!authenticateRequest(req, user)) {
            return api::HttpResponse(401, "{\"error\": \"Unauthorized\"}");
        }
        auto port = portfolioManager->getPortfolio(user.userId);
        std::ostringstream ss;
        ss << "{\"userId\": \"" << user.userId
           << "\", \"username\": \"" << user.username
           << "\", \"email\": \"" << user.email
           << "\", \"fullName\": \"" << user.fullName
           << "\", \"cashBalance\": " << port.cashBalance
           << ", \"totalPortfolioValue\": " << port.totalPortfolioValue()
           << ", \"totalPnL\": " << port.totalPnL()
           << ", \"returnPercent\": " << port.returnPercent()
           << ", \"isAdmin\": " << (user.isAdmin ? "true" : "false") << "}";
        return api::HttpResponse(200, ss.str());
    });

    server.addRoute("GET", "/api/stocks", [&](const api::HttpRequest&) {
        auto stocks = marketSimulator->getAllStocks();
        std::ostringstream ss;
        ss << "[";
        for (size_t i = 0; i < stocks.size(); ++i) {
            const auto& s = stocks[i];
            ss << "{\"symbol\": \"" << s.symbol
               << "\", \"companyName\": \"" << json::escapeString(s.companyName)
               << "\", \"sector\": \"" << json::escapeString(s.sector)
               << "\", \"currentPrice\": " << s.currentPrice
               << ", \"openPrice\": " << s.openPrice
               << ", \"highPrice\": " << s.highPrice
               << ", \"lowPrice\": " << s.lowPrice
               << ", \"previousClose\": " << s.previousClose
               << ", \"change\": " << s.change()
               << ", \"changePercent\": " << s.changePercent()
               << ", \"volume\": " << s.volume
               << ", \"marketCap\": " << s.marketCap << "}";
            if (i + 1 < stocks.size()) ss << ",";
        }
        ss << "]";
        return api::HttpResponse(200, ss.str());
    });

    server.addPrefixRoute("/api/stocks/", [&](const api::HttpRequest& req) {
        std::string subPath = req.path.substr(12);
        size_t slash = subPath.find('/');
        std::string symbol = (slash == std::string::npos) ? subPath : subPath.substr(0, slash);

        if (slash != std::string::npos) {
            std::string action = subPath.substr(slash + 1);
            if (action == "orderbook") {
                auto depth = matchingEngine->getDepth(symbol, 10);
                std::ostringstream ss;
                ss << "{\"symbol\": \"" << depth.symbol
                   << "\", \"lastPrice\": " << depth.lastPrice
                   << ", \"bestBid\": " << depth.bestBid
                   << ", \"bestAsk\": " << depth.bestAsk
                   << ", \"spread\": " << depth.spread
                   << ", \"bids\": [";
                for (size_t i = 0; i < depth.bids.size(); ++i) {
                    ss << "{\"price\": " << depth.bids[i].price
                       << ", \"quantity\": " << depth.bids[i].totalQuantity
                       << ", \"orderCount\": " << depth.bids[i].orderCount << "}";
                    if (i + 1 < depth.bids.size()) ss << ",";
                }
                ss << "], \"asks\": [";
                for (size_t i = 0; i < depth.asks.size(); ++i) {
                    ss << "{\"price\": " << depth.asks[i].price
                       << ", \"quantity\": " << depth.asks[i].totalQuantity
                       << ", \"orderCount\": " << depth.asks[i].orderCount << "}";
                    if (i + 1 < depth.asks.size()) ss << ",";
                }
                ss << "]}";
                return api::HttpResponse(200, ss.str());
            } else if (action == "candles") {
                auto candles = marketSimulator->getCandles(symbol);
                std::ostringstream ss;
                ss << "[";
                for (size_t i = 0; i < candles.size(); ++i) {
                    const auto& c = candles[i];
                    ss << "{\"timestamp\": " << c.timestamp
                       << ", \"open\": " << c.open
                       << ", \"high\": " << c.high
                       << ", \"low\": " << c.low
                       << ", \"close\": " << c.close
                       << ", \"volume\": " << c.volume << "}";
                    if (i + 1 < candles.size()) ss << ",";
                }
                ss << "]";
                return api::HttpResponse(200, ss.str());
            }
        }

        auto* stock = marketSimulator->getStock(symbol);
        if (!stock) {
            return api::HttpResponse(404, "{\"error\": \"Stock not found\"}");
        }

        std::ostringstream ss;
        ss << "{\"symbol\": \"" << stock->symbol
           << "\", \"companyName\": \"" << json::escapeString(stock->companyName)
           << "\", \"sector\": \"" << json::escapeString(stock->sector)
           << "\", \"currentPrice\": " << stock->currentPrice
           << ", \"openPrice\": " << stock->openPrice
           << ", \"highPrice\": " << stock->highPrice
           << ", \"lowPrice\": " << stock->lowPrice
           << ", \"previousClose\": " << stock->previousClose
           << ", \"change\": " << stock->change()
           << ", \"changePercent\": " << stock->changePercent()
           << ", \"volume\": " << stock->volume
           << ", \"marketCap\": " << stock->marketCap << "}";
        return api::HttpResponse(200, ss.str());
    });

    server.addRoute("POST", "/api/orders", [&](const api::HttpRequest& req) {
        models::User user;
        if (!authenticateRequest(req, user)) {
            return api::HttpResponse(401, "{\"error\": \"Unauthorized\"}");
        }

        std::string symbol = json::extractString(req.body, "symbol");
        std::string typeStr = json::extractString(req.body, "type");
        std::string sideStr = json::extractString(req.body, "side");
        int quantity = json::extractInt(req.body, "quantity");
        double limitPrice = json::extractDouble(req.body, "price");

        if (symbol.empty() || quantity <= 0) {
            return api::HttpResponse(400, "{\"error\": \"Invalid symbol or quantity\"}");
        }

        auto* stock = marketSimulator->getStock(symbol);
        if (!stock) {
            return api::HttpResponse(404, "{\"error\": \"Stock not found\"}");
        }

        models::OrderSide side = (sideStr == "BUY") ? models::OrderSide::BUY : models::OrderSide::SELL;
        models::OrderType type = (typeStr == "MARKET") ? models::OrderType::MARKET : models::OrderType::LIMIT;
        double price = (type == models::OrderType::MARKET) ? stock->currentPrice : limitPrice;

        if (price <= 0) {
            return api::HttpResponse(400, "{\"error\": \"Invalid price\"}");
        }

        if (side == models::OrderSide::BUY) {
            double estimatedCost = quantity * price;
            if (!portfolioManager->canAffordBuy(user.userId, estimatedCost)) {
                return api::HttpResponse(400, "{\"error\": \"Insufficient virtual cash balance for this order.\"}");
            }
        } else {
            if (!portfolioManager->hasEnoughShares(user.userId, symbol, quantity)) {
                return api::HttpResponse(400, "{\"error\": \"Insufficient shares in portfolio to execute sell.\"}");
            }
        }

        static int64_t userOrderSeq = 1000;
        models::Order order;
        order.orderId = "ORD-U-" + std::to_string(++userOrderSeq);
        order.userId = user.userId;
        order.symbol = symbol;
        order.type = type;
        order.side = side;
        order.quantity = quantity;
        order.filledQuantity = 0;
        order.price = price;
        order.status = models::OrderStatus::PENDING;
        order.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()
        ).count();

        auto matchResult = matchingEngine->processOrder(order);

        for (const auto& trade : matchResult.executedTrades) {
            portfolioManager->applyTrade(trade);
            database->saveTrade(trade);
        }

        database->saveOrder(matchResult.incomingOrder);

        std::ostringstream ss;
        ss << "{\"success\": true, \"orderId\": \"" << matchResult.incomingOrder.orderId
           << "\", \"status\": \"" << models::orderStatusToString(matchResult.incomingOrder.status)
           << "\", \"filledQuantity\": " << matchResult.incomingOrder.filledQuantity
           << ", \"remainingQuantity\": " << matchResult.incomingOrder.remainingQuantity()
           << ", \"tradesExecuted\": " << matchResult.executedTrades.size()
           << ", \"dsaTrace\": [";
        for (size_t i = 0; i < matchResult.dsaTraceSteps.size(); ++i) {
            ss << "\"" << json::escapeString(matchResult.dsaTraceSteps[i]) << "\"";
            if (i + 1 < matchResult.dsaTraceSteps.size()) ss << ",";
        }
        ss << "]}";
        return api::HttpResponse(200, ss.str());
    });

    server.addRoute("GET", "/api/orders", [&](const api::HttpRequest& req) {
        models::User user;
        if (!authenticateRequest(req, user)) {
            return api::HttpResponse(401, "{\"error\": \"Unauthorized\"}");
        }
        auto orders = database->getUserOrders(user.userId);
        std::ostringstream ss;
        ss << "[";
        for (size_t i = 0; i < orders.size(); ++i) {
            const auto& o = orders[i];
            ss << "{\"orderId\": \"" << o.orderId
               << "\", \"symbol\": \"" << o.symbol
               << "\", \"type\": \"" << models::orderTypeToString(o.type)
               << "\", \"side\": \"" << models::orderSideToString(o.side)
               << "\", \"quantity\": " << o.quantity
               << ", \"filledQuantity\": " << o.filledQuantity
               << ", \"remainingQuantity\": " << o.remainingQuantity()
               << ", \"price\": " << o.price
               << ", \"status\": \"" << models::orderStatusToString(o.status)
               << "\", \"timestamp\": " << o.timestamp << "}";
            if (i + 1 < orders.size()) ss << ",";
        }
        ss << "]";
        return api::HttpResponse(200, ss.str());
    });

    server.addRoute("GET", "/api/trades", [&](const api::HttpRequest& req) {
        models::User user;
        if (!authenticateRequest(req, user)) {
            return api::HttpResponse(401, "{\"error\": \"Unauthorized\"}");
        }
        auto trades = database->getUserTrades(user.userId);
        std::ostringstream ss;
        ss << "[";
        for (size_t i = 0; i < trades.size(); ++i) {
            const auto& t = trades[i];
            bool isBuyer = (t.buyerId == user.userId);
            ss << "{\"tradeId\": \"" << t.tradeId
               << "\", \"symbol\": \"" << t.symbol
               << "\", \"side\": \"" << (isBuyer ? "BUY" : "SELL")
               << "\", \"quantity\": " << t.quantity
               << ", \"price\": " << t.price
               << ", \"totalValue\": " << t.totalValue()
               << ", \"timestamp\": " << t.timestamp << "}";
            if (i + 1 < trades.size()) ss << ",";
        }
        ss << "]";
        return api::HttpResponse(200, ss.str());
    });

    server.addRoute("GET", "/api/portfolio", [&](const api::HttpRequest& req) {
        models::User user;
        if (!authenticateRequest(req, user)) {
            return api::HttpResponse(401, "{\"error\": \"Unauthorized\"}");
        }

        portfolioManager->updateMarketPrices(marketSimulator->getCurrentPriceMap());
        auto port = portfolioManager->getPortfolio(user.userId);

        std::ostringstream ss;
        ss << "{\"userId\": \"" << port.userId
           << "\", \"cashBalance\": " << port.cashBalance
           << ", \"investedValue\": " << port.totalInvestedValue()
           << ", \"marketValue\": " << port.totalMarketValue()
           << ", \"totalPortfolioValue\": " << port.totalPortfolioValue()
           << ", \"unrealizedPnL\": " << port.totalUnrealizedPnL()
           << ", \"realizedPnL\": " << port.realizedPnL
           << ", \"totalPnL\": " << port.totalPnL()
           << ", \"returnPercent\": " << port.returnPercent()
           << ", \"holdings\": [";

        bool first = true;
        for (const auto& pair : port.holdings) {
            if (pair.second.quantity <= 0) continue;
            if (!first) ss << ",";
            first = false;
            const auto& h = pair.second;
            ss << "{\"symbol\": \"" << h.symbol
               << "\", \"quantity\": " << h.quantity
               << ", \"averageBuyPrice\": " << h.averageBuyPrice
               << ", \"currentPrice\": " << h.currentPrice
               << ", \"investedValue\": " << h.investedValue()
               << ", \"marketValue\": " << h.marketValue()
               << ", \"unrealizedPnL\": " << h.unrealizedPnL()
               << ", \"pnlPercent\": " << h.pnlPercent() << "}";
        }
        ss << "]}";
        return api::HttpResponse(200, ss.str());
    });

    server.addRoute("GET", "/api/leaderboard", [&](const api::HttpRequest&) {
        portfolioManager->updateMarketPrices(marketSimulator->getCurrentPriceMap());
        auto allPortfolios = portfolioManager->getAllPortfolios();

        dsa::Sorting::mergeSort(allPortfolios, [](const models::Portfolio& a, const models::Portfolio& b) {
            return a.totalPortfolioValue() > b.totalPortfolioValue();
        });

        std::ostringstream ss;
        ss << "[";
        for (size_t i = 0; i < allPortfolios.size(); ++i) {
            const auto& p = allPortfolios[i];
            auto* u = authManager->getUserById(p.userId);
            std::string username = u ? u->username : p.userId;
            std::string fullName = u ? u->fullName : username;

            ss << "{\"rank\": " << (i + 1)
               << ", \"userId\": \"" << p.userId
               << "\", \"username\": \"" << username
               << "\", \"fullName\": \"" << fullName
               << "\", \"portfolioValue\": " << p.totalPortfolioValue()
               << ", \"totalPnL\": " << p.totalPnL()
               << ", \"returnPercent\": " << p.returnPercent() << "}";
            if (i + 1 < allPortfolios.size()) ss << ",";
        }
        ss << "]";
        return api::HttpResponse(200, ss.str());
    });

    server.addRoute("GET", "/api/dsa/graph", [&](const api::HttpRequest&) {
        const auto& graph = eventEngine->getGraph();
        const auto& vertices = graph.getVertices();
        const auto& adj = graph.getAdjList();

        std::ostringstream ss;
        ss << "{\"vertices\": [";
        for (size_t i = 0; i < vertices.size(); ++i) {
            ss << "\"" << vertices[i] << "\"";
            if (i + 1 < vertices.size()) ss << ",";
        }
        ss << "], \"edges\": [";
        bool firstEdge = true;
        for (const auto& vPair : adj) {
            const std::string& src = vPair.first;
            for (const auto& edge : vPair.second) {
                if (!firstEdge) ss << ",";
                firstEdge = false;
                ss << "{\"source\": \"" << src
                   << "\", \"target\": \"" << edge.target
                   << "\", \"weight\": " << edge.weight
                   << ", \"relationship\": \"" << edge.relationship << "\"}";
            }
        }
        ss << "]}";
        return api::HttpResponse(200, ss.str());
    });

    server.addRoute("POST", "/api/dsa/bfs", [&](const api::HttpRequest& req) {
        std::string startNode = json::extractString(req.body, "startNode");
        if (startNode.empty()) startNode = "Semiconductors";

        std::vector<dsa::GraphStepLog> logs;
        auto result = eventEngine->getGraph().bfs(startNode, &logs);

        std::ostringstream ss;
        ss << "{\"algorithm\": \"BFS\", \"startNode\": \"" << startNode << "\", \"traversal\": [";
        for (size_t i = 0; i < result.size(); ++i) {
            ss << "\"" << result[i] << "\"";
            if (i + 1 < result.size()) ss << ",";
        }
        ss << "], \"steps\": [";
        for (size_t i = 0; i < logs.size(); ++i) {
            ss << "{\"step\": " << (i + 1)
               << ", \"currentNode\": \"" << logs[i].currentNode
               << "\", \"description\": \"" << json::escapeString(logs[i].description) << "\"}";
            if (i + 1 < logs.size()) ss << ",";
        }
        ss << "]}";
        return api::HttpResponse(200, ss.str());
    });

    server.addRoute("POST", "/api/dsa/dfs", [&](const api::HttpRequest& req) {
        std::string startNode = json::extractString(req.body, "startNode");
        if (startNode.empty()) startNode = "Technology";

        std::vector<dsa::GraphStepLog> logs;
        auto result = eventEngine->getGraph().dfs(startNode, &logs);

        std::ostringstream ss;
        ss << "{\"algorithm\": \"DFS\", \"startNode\": \"" << startNode << "\", \"traversal\": [";
        for (size_t i = 0; i < result.size(); ++i) {
            ss << "\"" << result[i] << "\"";
            if (i + 1 < result.size()) ss << ",";
        }
        ss << "], \"steps\": [";
        for (size_t i = 0; i < logs.size(); ++i) {
            ss << "{\"step\": " << (i + 1)
               << ", \"currentNode\": \"" << logs[i].currentNode
               << "\", \"description\": \"" << json::escapeString(logs[i].description) << "\"}";
            if (i + 1 < logs.size()) ss << ",";
        }
        ss << "]}";
        return api::HttpResponse(200, ss.str());
    });

    server.addRoute("POST", "/api/dsa/dijkstra", [&](const api::HttpRequest& req) {
        std::string source = json::extractString(req.body, "source");
        std::string target = json::extractString(req.body, "target");
        if (source.empty()) source = "Semiconductors";
        if (target.empty()) target = "MSFT";

        auto dRes = eventEngine->getGraph().dijkstra(source);
        auto path = dRes.getPathTo(target);
        double dist = dRes.distances[target];

        std::ostringstream ss;
        ss << "{\"algorithm\": \"Dijkstra\", \"source\": \"" << source
           << "\", \"target\": \"" << target
           << "\", \"distance\": " << dist
           << ", \"path\": [";
        for (size_t i = 0; i < path.size(); ++i) {
            ss << "\"" << path[i] << "\"";
            if (i + 1 < path.size()) ss << ",";
        }
        ss << "], \"steps\": [";
        for (size_t i = 0; i < dRes.logs.size(); ++i) {
            ss << "{\"step\": " << (i + 1)
               << ", \"currentNode\": \"" << dRes.logs[i].currentNode
               << "\", \"description\": \"" << json::escapeString(dRes.logs[i].description) << "\"}";
            if (i + 1 < dRes.logs.size()) ss << ",";
        }
        ss << "]}";
        return api::HttpResponse(200, ss.str());
    });

    server.addRoute("POST", "/api/dsa/sort/trace", [&](const api::HttpRequest& req) {
        std::string algo = json::extractString(req.body, "algorithm");
        std::vector<int> sample = {65, 28, 14, 88, 32, 95, 45, 12, 77, 50};

        std::vector<dsa::SortStepLog> logs;
        if (algo == "QUICK_SORT") {
            logs = dsa::Sorting::traceQuickSort(sample);
        } else {
            logs = dsa::Sorting::traceMergeSort(sample);
        }

        std::ostringstream ss;
        ss << "{\"algorithm\": \"" << algo << "\", \"steps\": [";
        for (size_t i = 0; i < logs.size(); ++i) {
            ss << "{\"step\": " << (i + 1)
               << ", \"description\": \"" << json::escapeString(logs[i].description)
               << "\", \"array\": [";
            for (size_t j = 0; j < logs[i].arrayState.size(); ++j) {
                ss << logs[i].arrayState[j];
                if (j + 1 < logs[i].arrayState.size()) ss << ",";
            }
            ss << "], \"highlighted\": [";
            for (size_t k = 0; k < logs[i].highlightedIndices.size(); ++k) {
                ss << logs[i].highlightedIndices[k];
                if (k + 1 < logs[i].highlightedIndices.size()) ss << ",";
            }
            ss << "]}";
            if (i + 1 < logs.size()) ss << ",";
        }
        ss << "]}";
        return api::HttpResponse(200, ss.str());
    });

    server.addRoute("POST", "/api/admin/regime", [&](const api::HttpRequest& req) {
        std::string regimeStr = json::extractString(req.body, "regime");
        auto regime = market::stringToRegime(regimeStr);
        marketSimulator->setRegime(regime);
        return api::HttpResponse(200, "{\"success\": true, \"currentRegime\": \"" + market::regimeToString(regime) + "\"}");
    });

    server.addRoute("POST", "/api/admin/events/trigger", [&](const api::HttpRequest& req) {
        std::string title = json::extractString(req.body, "title");
        std::string desc = json::extractString(req.body, "description");
        std::string epicentre = json::extractString(req.body, "epicentre");
        double impact = json::extractDouble(req.body, "impact", -0.08);

        if (title.empty()) title = "Simulated Macro Event";
        if (epicentre.empty()) epicentre = "Semiconductors";

        auto evt = eventEngine->triggerEvent(title, desc, epicentre, impact);
        return api::HttpResponse(200, "{\"success\": true, \"eventId\": \"" + evt.eventId + "\", \"epicentre\": \"" + evt.epicentreNode + "\"}");
    });

    server.addRoute("POST", "/api/admin/tick", [&](const api::HttpRequest&) {
        marketSimulator->tick();
        return api::HttpResponse(200, "{\"success\": true, \"message\": \"Simulation ticked\"}");
    });

    server.addRoute("POST", "/api/ai/ask", [&](const api::HttpRequest& req) {
        std::string question = json::extractString(req.body, "question");
        std::string promptLower = question;
        std::transform(promptLower.begin(), promptLower.end(), promptLower.begin(), ::tolower);

        std::string answer;
        if (promptLower.find("dijkstra") != std::string::npos) {
            answer = "Dijkstra's Algorithm in TradeVerse finds the lowest-resistance shock propagation path across interconnected companies and sectors. It operates in O((V + E) log V) time using a custom Min-Priority Heap.";
        } else if (promptLower.find("heap") != std::string::npos || promptLower.find("order book") != std::string::npos) {
            answer = "The TradeVerse Order Book uses a Binary Max-Heap for Buy bids (highest price gets priority) and a Binary Min-Heap for Sell asks (lowest ask gets priority). When prices cross, the Price-Time Priority matching engine executes orders in O(log N) per fill.";
        } else if (promptLower.find("portfolio") != std::string::npos || promptLower.find("pnl") != std::string::npos || promptLower.find("money") != std::string::npos) {
            answer = "Your portfolio value is continuously calculated in C++ by aggregating your liquid cash balance with the mark-to-market value of all active holdings. Realized P&L is locked upon selling shares, while unrealized P&L tracks live price fluctuations.";
        } else if (promptLower.find("merge sort") != std::string::npos || promptLower.find("leaderboard") != std::string::npos) {
            answer = "The Leaderboard ranking uses custom C++ Merge Sort, guaranteeing a stable O(N log N) ordering of all competitor portfolios by total net worth.";
        } else {
            answer = "I am TradeVerse AI, your algorithmic trading companion. In this simulation, every order matching, priority queue extraction, graph contagion, and leaderboard sort is computed by the custom C++ Data Structures & Algorithms engine.";
        }

        std::ostringstream ss;
        ss << "{\"answer\": \"" << json::escapeString(answer) << "\"}";
        return api::HttpResponse(200, ss.str());
    });

    std::thread tickerThread([&]() {
        while (true) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1500));
            marketSimulator->tick();
        }
    });
    tickerThread.detach();

    server.start();
    std::cout << "[TradeVerse] Engine initialized and ready to serve requests on port 8080." << std::endl;

    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(10));
    }

    return 0;
}